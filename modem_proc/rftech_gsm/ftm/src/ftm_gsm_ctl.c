/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                           
                       F T M  G S M  C T L

GENERAL DESCRIPTION
  This is the FTM RF file which contains RF specific commands

Copyright (c) 2000, 2002  by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2007  by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2013  by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2014  by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2015  by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_gsm/ftm/src/ftm_gsm_ctl.c#1 $ 
  $DateTime: 2016/12/13 07:59:51 $ 
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/20/15   par     Remove redundant init condition
10/20/15   par     Update GL1 state in FTM for LIF for Rx and Rx/Tx cases
10/08/15   par     Override LIF mode flag to always enable during FTM calibration mode
09/14/15   par     Fix crash in FTM GSM deact sequence
04/15/15   bp      Support bands differentiation during div enter_mode controlled by NV item
11/13/14   ck      add capability to disable/enable gsm diversity 
10/30/14   ck      store ftm mode id for hooking ftm concurrency manager for gsm diversity 
09/30/14   tsr     Fixed OFT compiler error
09/25/14   tsr     DR-DSDS: Add support to instruct GL1 to execute RX only HW init 
                   if device is not TX capable
11/19/13   cpm     Add EGSM support for FTM_PROGRAM_QFE_SCENARIO
09/09/14   tsr     DR-DSDS C2/C3 RX calibration support
08/14/14   tsr     Reenable FTM GSM RX Diversity for Multisim(Disabled from earlier checkin)
08/12/14   ec      Fix for DRx crash on DSDA targets DPM2
07/25/14   par     Waiting for a GSM NS exit FTM signal outside 
                   STOP_MODE_REQ
06/30/14   ec      Mainline Rx Diversity
06/13/14   tsr     Reenable  GSM RX Diversity
05/22/14   tsr     Disable GSM RX Diversity Temporarily  
04/18/14   zbz     Fix FTM RF failure during handover
04/16/14   tsr     Add support for GSM RX Diversity (GL1 Dependant)
03/26/14   sb      Add support for IP2 cal for Dime PM
02/26/14   sc      Pass core data through cmd processing and mc layer
02/17/14   sc      Clean up mDSP APIs
07/01/14   sw      Move the calls to the geran test mode api into rftech_gsm
09/09/13   sb      Added hooks for exit GSM mode in FTM
09/04/13   ck      Update ftm_current_rf_mode when entering GSM mode to fix enh therm reading
08/21/13   tws     Replace FEATURE_TRITON_MODEM with FEATURE_DUAL_SIM
07/24/13   ec	   Pass FTM task ID into MC layer
05/23/13   ck      Fix corrupted first IQ samples
05/07/13   ck      Add waiting for the signal to ensure GSM finish stopping
04/15/13   ec      Featurise geran_test_mode apis to use as_id only for Triton 
04/12/12   ec	   Use as_id in calls to geran_test_mode_api
04/03/13   ec	   Pass rfm_dev to NS functions
04/02/13   aka     Support for band change in frame sweep functionality. 
03/28/13   aka     GSM frame sweep feature changes.
02/26/13   tws     Make the TX frequency offset device specific.
02/14/13   tws     The ftm gsm vars are now created in the dispatch handler.
02/06/13   tws     Call rfgsm_mc_set_band with the device set correctly.
02/04/13   tws     Update for geran_test_mode_api change.
01/2/13    aka     Changes to support multi chain GSM operation in FTM mode
01/18/13   tws     Set TX frequency offset is not device dependent.
01/08/13   tws     Pass as_id to mdsp intf api.
01/3/13    av      Changes to support chain2 for GSM
12/10/12   tws     Allocate FTM variables in FTM mode only.
12/05/12   tws     Use device2_as_id to get the correct ID for GL1. Mainline
                   the RF task context flag.
12/04/12   aka     Featurization changes for Dual active
12/03/12   tws     Move the FTM variable initialisation back to core.
11/30/12   tws     Remove AS_ID mapping until dependency is fixed.
11/27/12   tws     Make global variables device dependent.
11/23/12   tws     Remove dependecy on legacy GERAN API.
11/21/12   av      Added rfm_device parameters to mc/core/mdsp/msm layers.
11/13/12   ec	   Update to use new geran api for DSDA 
10/10/12   ec      Replace call to rfgsm_core_set_tx_pow with new ftm wrapper
09/25/12   tc      Mainline RF_HAS_RFA_GSM_TASK. 
08/20/12   sc      Add back gl1_hw_turn_tx_on as requirement in RF due to GL1 not calling
08/14/12   ec      Removed gl1_hw_turn_tx_on call in ftm_gsm_set_mode to avoid gfw error 
08/13/12   ec      Commented out RXFE status check in ftm_gsm_set_mode to unblock bring-up 
08/07/12   ec      Unstub GL1 calls for ftm_gsm_set_mode, make ftm_gsm_rf_call_in_rf_task_context
                   feature switched 
07/12/12   ec      Fix compilation error with CCS defined, using extern prototype for gl1_hw_rf_set_tx_band 
                   instead if #include gl1_hw_g.h 
07/06/12   ec      Use gl1_hw_rf_set_tx_band instead of rfm call
06/27/12   sb      Made changes to remove unused MDSP APIs to the grfc status  
05/11/12   jr      Changed the sequence of code within ftm_gsm_set_mode().
04/26/12   sb      Add FTM recovery from RXFE stuck  
03/23/12   sr      Removed the FEATURE_GSM_MDSP_QDSP6 featurization.
03/22/12   dej     Initializing variable req to zero to avoid storing junk values
03/17/12   jfc     Remove clkrgm* header files
03/05/12   zc      Added call to ftm_gsm_rx_disable(); in ftm_gsm_set_mode
02/28/12   sb      Added changes to support disabling Idle frame in FTM mode 
02/16/12   sb      Fixing ftm_gsm_set_mode leaking rex critical sections
01/16/12   sr      Made changes not todo idle_frame processing 
                   (temp comp and vbatt comp) during calibration.
01/11/12   sr      Added "ftm_task" include file to fix compiler error.
01/05/12   sr      geran api changes for non-signaling.
12/07/11   sb      Support for GSM IQ capture in Online and FTM mode  
10/19/11   sb      Apply therm read in idle frame and enable FTM temp comp and vbatt comp during idle frame
09/28/11   sb      Mainline FEATURE_RF_COMMON_LM_RFM_INTERFACE and FEATURE_RF_WCDMA_LM_RFM_INTERFACE. 
09/20/11   av      Call rfm_set_tx_band() during FTM. 
06/21/11   aro     Moved ftm_sys_control functionality to ftm_common_control
04/28/11   rsr     Move GSM set mode functionality to ftm_rfmode_enter.
04/25/11   aro     Renamed FTM RFMODE to FTM State
04/21/11   rsr     Fix to send LM buffer index to rfm_enable_tx. Temporary fix to allow files to compile. Need to talk to 
                   L1 and fix properly later.
03/24/11   aro     Renamed FTM system control filename
03/24/11   aro     Renamed FTM Enter Mode function 
03/16/11   sr      Removed un-used code.
03/11/11   sr      Removed un-used featurization
03/02/11   aro     Renamed FTM and FTM-NS mode enums
03/01/11   aro     Moved ftm_gsm_enter() to ftm_sys_ctl.c
02/15/11   aro     Added direct call to ftm_rfmode_set()
01/25/11   sr      removed the clk.h dependency
01/18/11   mkv     Support WCDMA/GSM to LTE FTM mode transition. 
12/20/10   pv      Updated srch_mdsp_disable() with ftm_mdsp_disable(), to avoid 1x 
                   FW running in other techs RF Calibration.
10/08/10   ggs     CR257912 fix pulled in from QSC6295:
                   Made changes to turn-off the cont_tx in tdma_isr instead of from FTM task
08/20/10   tsr     Remove INTLOCK/INTFREE code 
08/12/10   rsr     Removing reference to unused Headers for QDSP6 targets.
07/20/10   ggs     Merged changes from from /dev/mdm9x00_bringup branch
03/19/10   lcl     Pull in fixes from QSC6295
03/09/10   lcl     Removed GPS app from ftm_gsm_enter.
01/06/10   sar     Renamed ftm_task.h to rf_test_task_v.h.
12/28/09   sr      High/Medium lint errors fix
12/22/09   sr      changed the debug msg macro from legacy to new one.
12/02/09   sr      Removed the tx_band setting during FTM Mode command.
10/12/09   sr      Fixed warnings
09/17/09   sar     Removed deprecated file, adie.h for CMI conformance.
09/17/09   sar     Updated clk interface for CMI conformance.
07/06/09   sr      change the rfgsmcard.tx_band to rfcom_gsm_band type
07/04/09   sr      set the rfgsmcard.tx_band in the set_mode.
04/01/09   sr      include file cleanup.
06/30/08   dw      Unload 1x mdsp image through SRCH
02/28/08   rmd     Added ANT_SEL temporary fix for TRF6285.
02/22/08   avi     Added check for FTM_RF_DB_IDLE when entering mode GSM (caused
                   FW download issues on MM targets)
12/14/07    ad     Defined ftm_gsm_toggle_modes() for EDGE only support
12/14/07    ad     added call to ftm_rfmode_gsm_cal() at the top of 
                   ftm_gsm_set_mode()- fixes mode switching crash
12/14/07    ad     Fix for uninitialized pointer issue in log packet in ftm_tdma_isr
12/13/07   adk     Fixed compiler warning.
12/07/07   adk     Added function-call ftm_rfmode_gsm() in ftm_gsm_set_mode()
                   to let 1x know that the 1x mode has been exited.
11/06/07    ka     Ensure 1x fw is unloaded to prevent crash.
08/08/07   jfc     Mainlined T_MSM6275 feature
08/08/07   jfc     Mainlined T_MSM6280 feature
08/06/07   jfc     Featurized code for EDGE-only builds
07/25/07   ycl     Lint warning fix.
12/13/06   jfc     Add ftm_send_FTM_STOP_GSM_MODE_REQ()
12/08/06   jfc     Specify MDSP app when starting WL1 for FEATURE_APP_W2G
10/18/06   ycl     Fixed rfm_enable_tx call in ftm_set_mode.
10/17/06   ycl     Modifications for initial MSM7600 build.
03/28/06   rsr     Lint cleanup. 
03/06/06   rsr     Fixes to remove compiler warnings.
02/21/06   tws     Change WCDMA_1800 to BC3. Add support for BC4 and BC9.
01/31/06   et      fixed pre-existing compile issue w/ rfm_enable_tx and added
                   HDR featurization
12/13/05   jfc     Reset tags_rd_index and tags_wr_index during ftm_set_mode()
08/23/05   rsr/gfr Changes to support gl1 related (DTM) changes. 
06/07/05   rsr     Removed calls to gl1_hw_schedule_reset_events and replaced by 
           gl1_hw_schedule_frame_tick in ftm_tdma_isr, per graham's suggestion. 
05/04/05   rsr     Removed warnings.Cleanup
04/21/05   rsr     Added support for new NV items
02/09/05   rsr/lcl  Renamed FEATURE_POLAR_RF to FEATURE_POLAR
02/07/05   xw      Added another signal to firmware download per gl1 interface change. 
10/07/04   bhas    update ftm_gsm_wait is not used anymore
01/14/03   lcl     If tx_cal_sweep is on, give it higher priority in
                   ftm_tdma_isr.
08/18/03   lcl     Added support for 850 and 1900 bands.
12/15/03   wd/jwh  Changed calls of gl1_hw_init() to specify FTM_SIG_WAIT_SIG.
12/10/02   xw      Code clean up and renamed it to ftm_gsm_ctl.c
08-27-02   rjr     Created.
===========================================================================*/
/*===========================================================================
                              INCLUDES
===========================================================================*/

#include "rfa_variation.h"
#include "comdef.h"
#include "target.h"

#ifdef FEATURE_FACTORY_TESTMODE
#include "ftmicap.h"
#include "rex.h"

#ifdef FTM_HAS_UMTS

#ifdef FEATURE_GSM

#include "ftm.h"
#include "ftm_gsm_ctl.h"
#include "ftm_gsm_rfctl.h"
#include "ftm_gsm.h"
#include "rf.h"
#include "diagi.h"
#include "ftm.h"
#include "rf_test_task_v.h"
#include "ftm_task.h"
#include "ftm_gsm_rfctl.h"
#include "ftm_gsm_rfprot.h"
#include "ftm_gsm_prot.h"
#include "rfgsm_core_util.h"
#include "ftm_common_control.h"
#include "ftm_gsm_rfctl.h"
#ifdef FEATURE_WCDMA
 #include "assert.h" 
 #include "l1task.h"
#endif

#include "rfgsm_mc.h"
#include "rfgsm_nv_cmn.h"
#include "rfgsm_mdsp_cal.h"
/* For reference to SYS_BAND macros. */
#include "sys_type.h"
#include "ftm_log.h"

#include "gl1_hw.h"
#include "geran_test_mode_api.h"

#ifdef FEATURE_GSM_RFLM
#include "rxlm_intf.h"
#include "txlm_intf.h"
#endif

#include "rfcommon_nv_mm.h"

/*===========================================================================
             GLOBAL VARIABLES 
=============================================================================*/

//static uint32 ftm_gsm_txlm_buff_idx = MDSP_RXLM_BUF_IDX_INVALID;
/*===========================================================================
             EXTERNAL VARIABLES 
=============================================================================*/
/* To track the current RF mode */
extern ftm_rf_mode_type ftm_current_rf_mode; 


/* Variables to keep track of default AMSS values */
LOCAL int8  therm_recorded;
LOCAL int8  temp_comp_bin_index;
LOCAL int8  temp_comp_rem;
LOCAL int8  temp_comp_val;
LOCAL int16  vbatt_recorded;
LOCAL int16  vbatt_backoff;



/*Function in ftm_gsm.c to disable Rx*/
extern void ftm_gsm_rx_disable( rfm_device_enum_type rfm_dev );


#define FTM_GSM_IDLE_FRAME_NUM 26

/* FTM GSM mutex */

rex_crit_sect_type ftm_gsm_lock;

static rfcom_device_enum_type ftm_gsm_current_tx_cal_dev = RFM_INVALID_DEVICE;

void ftm_mutex_init(void);

void ftm_set_gsm_rf_db_state(rfgsm_band_type band);

/*===========================================================================
          EXTERNAL FUNCTION PROTOTYPES
===========================================================================*/


/*===========================================================================
          PUBLIC FUNCTION IMPLEMENTATIONS
===========================================================================*/




/*===========================================================================

FUNCTION FTM_MODE_WAS_GSM

DESCRIPTION
    This function will determine if the current mode is GSM or not.

DEPENDENCIES
   
RETURN VALUE
   TRUE if the current mode is GSM.
   FALSE if the current mode is not GSM. 

SIDE EFFECTS
   None

===========================================================================*/
boolean ftm_mode_was_gsm(void)
{
  if((ftm_get_current_state(RFM_DEVICE_0) == FTM_STATE_GSM) ||
     (ftm_get_current_state(RFM_DEVICE_0) == FTM_STATE_GSM_NS))
 {
    return(TRUE);
 }
 else
    return(FALSE);

}

/*===========================================================================

FUNCTION FTM_GSM_SET_MODE

DESCRIPTION
    This function will switch modes to EGSM or DCS based
    on the entry parameter.   

DEPENDENCIES
   
RETURN VALUE
   None.

SIDE EFFECTS
   None

===========================================================================*/
void ftm_gsm_set_mode( rfm_device_enum_type rfm_dev,
                       ftm_mode_id_type mode )
{
  boolean change_mode = FALSE;
  uint16  default_channel =1;
  rfgsm_band_type gsm_band = RFGSM_BAND_GSM900;
  sys_band_T gsm_sys_band = INVALID_BAND;
  rfgsm_core_data_type rfgsm_core_data;
  rfgsm_core_handle_type* rfgsm_core_handle_ptr = NULL;
  sys_modem_as_id_e_type as_id = gl1_hw_rf_map_device2_as_id( rfm_dev, ftm_mode );
  boolean device_is_tx = rfgsm_core_get_device_tx_capability(rfm_dev);

  ftm_gsm_rfctl_vars_s * vars_ptr = ftm_gsm_get_vars_ptr(rfm_dev);
  if ( vars_ptr == NULL )
  {
    MSG_1(MSG_SSID_FTM, MSG_LEGACY_ERROR, "FTM variables are NULL", 0);
    return;
  }

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);
  if ( rfgsm_core_handle_ptr == NULL )
  {
    MSG_1(MSG_SSID_FTM, MSG_LEGACY_ERROR, "rfgsm_core_handle_ptr is NULL", rfm_dev);
    return;
  }

  vars_ptr->ftm_mode_id = mode;
  
  ftm_rfmode_enter( rfm_dev, FTM_STATE_GSM );

  switch ( mode )
  {
  case FTM_PHONE_MODE_GSM_850:
    if ( vars_ptr->ftm_gl1_hw_gsm_band != SYS_BAND_CELL_850 )
    {
      change_mode = TRUE;
      gsm_band = RFGSM_BAND_GSM850;
      gsm_sys_band = SYS_BAND_CELL_850;
      default_channel = 128;
    }
    break;

  case FTM_PHONE_MODE_GSM_900:
    if ( vars_ptr->ftm_gl1_hw_gsm_band != SYS_BAND_EGSM_900 )
    {
      change_mode = TRUE;
      gsm_band = RFGSM_BAND_GSM900;
      gsm_sys_band = SYS_BAND_EGSM_900;
      default_channel = 1;
    }
    break;

  case FTM_PHONE_MODE_GSM_1800:
    if ( vars_ptr->ftm_gl1_hw_gsm_band != SYS_BAND_DCS_1800 )
    {
      change_mode = TRUE;
      gsm_band = RFGSM_BAND_GSM1800;
      gsm_sys_band = SYS_BAND_DCS_1800;
      default_channel = 512;
    }
    break;

  case FTM_PHONE_MODE_GSM_1900:
    if ( vars_ptr->ftm_gl1_hw_gsm_band != SYS_BAND_PCS_1900 )
    {
      change_mode = TRUE;      
      gsm_band = RFGSM_BAND_GSM1900;
      gsm_sys_band = SYS_BAND_PCS_1900;
      default_channel = 512;
    }
    break;

  case FTM_PHONE_MODE_SLEEP:

    MSG_1(MSG_SSID_FTM, MSG_LEGACY_HIGH, "ftm_gsm_set_mode, mode:%d", mode);
    ftm_rfmode_exit( rfm_dev, FTM_STATE_PARK );

    break;

  default:
    MSG_1(MSG_SSID_FTM, MSG_LEGACY_ERROR,"unsupported band %d",mode);
    change_mode = FALSE;
    break;
  }

  if ( change_mode == TRUE )
  {
    /* reset the frame counter for TDMA ISR initialize RX, TX and Synth */
    vars_ptr->ftm_frame_count = 0;

    /* Change the power table index step size so that FTM has a finer
     * power resolution than normal operation.
    */
    (void)ftm_gsm_set_tx_pow_dbm( rfm_dev, 2000,0);

    /*If transmiter is on, turn it off before switching mode.  Note, we cannot have this
     * API call inside an INTLOCK because ftm_FNmod4 will not increase due to the INTLOCK.
     * Thus, the phone will hang if we are changing mode while TX is on.  Not sure why we
     * need INTLOCK.  Do keep this in mind in case there is problem.
     */
    if ( vars_ptr->ftm_gsm_state.ftm_tx_state )
    {
      ftm_gsm_set_tx( rfm_dev, OFF);
    }

    /*Disable GSM Rx and wait 20ms to prevent race condition observed during test*/
    ftm_gsm_rx_disable( rfm_dev );
	
    /* update band */
    vars_ptr->ftm_gl1_hw_gsm_band = gsm_sys_band;
	
    /* stop TX and RX if it is going */
    vars_ptr->ftm_cont_tx = FALSE;
    vars_ptr->ftm_cont_rx = FALSE;
    vars_ptr->ftm_num_bursts_tx = 0;
    vars_ptr->ftm_num_bursts_rx = 0;
    vars_ptr->ftm_gsm_state.ftm_rf_mode = (word)mode;
	
    /* Reset GSM burst tags index */
    vars_ptr->tags_rd_index = 0;
    vars_ptr->tags_wr_index = 0;

    /* Rx Burst is not on */
    vars_ptr->rxBurstOn = FALSE;

    /* Set the core data */
    rfgsm_core_data.rf_task = RF_TASK_FTM;
    rfgsm_core_data.rfm_dev = rfm_dev;
    rfgsm_core_data.sub_id = 0; //Default for now
    rfgsm_core_data.as_id = as_id;
    rfgsm_core_data.triple_buffer_id = 0; //Default for now
    rfgsm_core_data.rfgsm_core_handle_ptr = rfgsm_core_handle_ptr;

    /* Set the band */
    rfgsm_mc_set_band(&rfgsm_core_data, (rfgsm_band_type)gsm_band );

    if (device_is_tx) 
    {
      geran_test_mode_api_rf_set_tx_band((int8)gsm_band,as_id);
    }

    ftm_set_gsm_rf_db_state(gsm_band);

    /* Set to default channel */
    ftm_gsm_set_arfcn( rfm_dev, default_channel );

    ftm_clk_rex_wait(10);
  }
} /* end ftm_gsm_set_mode */


/*===========================================================================

FUNCTION FTM_SET_GSM_RF_DB_STATE

DESCRIPTION
    This function will update ftm_current_rf_mode according to corresponding GSM band

DEPENDENCIES
   
RETURN VALUE
   None.

SIDE EFFECTS
   None

===========================================================================*/


void ftm_set_gsm_rf_db_state(rfgsm_band_type band)
{
  switch ( band )
  {
    case RFGSM_BAND_GSM850:
      ftm_set_rf_db_state(FTM_DB_RF_GSM_850);
      break;
	
    case RFGSM_BAND_GSM900:
      ftm_set_rf_db_state(FTM_DB_RF_GSM_900);
      break;
	
    case RFGSM_BAND_GSM1800:		
      ftm_set_rf_db_state(FTM_DB_RF_GSM_1800);
      break;
	
    case RFGSM_BAND_GSM1900:		
      ftm_set_rf_db_state(FTM_DB_RF_GSM_1900);
      break;
	
    default:
      MSG_1(MSG_SSID_FTM, MSG_LEGACY_ERROR,"unsupported band %d",band);
      ftm_set_rf_db_state(FTM_DB_RF_IDLE);
      break;
  }
}



/*===========================================================================

FUNCTION FTM_GSM_FRAME_SWEEP_SET_BAND

DESCRIPTION
    This function sets a new GSM band. Rx and Tx should be off before calling this API 
    as it does not take care of any race conditions.

DEPENDENCIES
   
RETURN VALUE
   None.

SIDE EFFECTS
   None

===========================================================================*/
void ftm_gsm_frame_sweep_set_band( rfm_device_enum_type rfm_dev,
                                   ftm_mode_id_type mode )
{
  boolean change_mode = FALSE;
  uint16  default_channel =1;
  rfgsm_band_type gsm_band = RFGSM_BAND_GSM900;
  rfgsm_core_data_type rfgsm_core_data;
  rfgsm_core_handle_type* rfgsm_core_handle_ptr = NULL;

  sys_modem_as_id_e_type as_id = gl1_hw_rf_map_device2_as_id( rfm_dev, ftm_mode );

  ftm_gsm_rfctl_vars_s * vars_ptr = ftm_gsm_get_vars_ptr(rfm_dev);

  if ( vars_ptr == NULL )
  {
    MSG_1(MSG_SSID_FTM, MSG_LEGACY_ERROR, "FTM variables are NULL", 0);
    return;
  }

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);
  if ( rfgsm_core_handle_ptr == NULL )
  {
    MSG_1(MSG_SSID_FTM, MSG_LEGACY_ERROR, "rfgsm_core_handle_ptr is NULL", rfm_dev);
    return;
  }

  switch ( mode )
  {
  case FTM_PHONE_MODE_GSM_850:
    if ( vars_ptr->ftm_gl1_hw_gsm_band != SYS_BAND_CELL_850 )
    {
      change_mode = TRUE;
      gsm_band = RFGSM_BAND_GSM850;
      vars_ptr->ftm_gl1_hw_gsm_band = SYS_BAND_CELL_850;
      default_channel = 128;
    }
    break;

  case FTM_PHONE_MODE_GSM_900:
    if ( vars_ptr->ftm_gl1_hw_gsm_band != SYS_BAND_EGSM_900 )
    {
      change_mode = TRUE;
      gsm_band = RFGSM_BAND_GSM900;
      vars_ptr->ftm_gl1_hw_gsm_band = SYS_BAND_EGSM_900;
      default_channel = 1;
    }
    break;

  case FTM_PHONE_MODE_GSM_1800:
    if ( vars_ptr->ftm_gl1_hw_gsm_band != SYS_BAND_DCS_1800 )
    {
      change_mode = TRUE;
      gsm_band = RFGSM_BAND_GSM1800;
      vars_ptr->ftm_gl1_hw_gsm_band = SYS_BAND_DCS_1800;
      default_channel = 512;
    }
    break;

  case FTM_PHONE_MODE_GSM_1900:
    if ( vars_ptr->ftm_gl1_hw_gsm_band != SYS_BAND_PCS_1900 )
    {
      change_mode = TRUE;      
      gsm_band = RFGSM_BAND_GSM1900;
      vars_ptr->ftm_gl1_hw_gsm_band = SYS_BAND_PCS_1900;
      default_channel = 512;
    }
    break;

  default:
    MSG_1(MSG_SSID_FTM, MSG_LEGACY_ERROR,"unsupported band %d",mode);
    change_mode = FALSE;
    break;
  }

  if ( change_mode == TRUE )
  {
    /* reset the frame counter for TDMA ISR initialize RX, TX and Synth */
    vars_ptr->ftm_frame_count = 0;

    vars_ptr->ftm_gsm_state.ftm_rf_mode = (word)mode;

    /* Reset GSM burst tags index */
    vars_ptr->tags_rd_index = 0;
    vars_ptr->tags_wr_index = 0;

    /* Set the core data */
    rfgsm_core_data.rf_task = RF_TASK_FTM;
    rfgsm_core_data.rfm_dev = rfm_dev;
    rfgsm_core_data.sub_id = 0; //Default for now
    rfgsm_core_data.as_id = as_id;
    rfgsm_core_data.triple_buffer_id = 0; //Default for now
    rfgsm_core_data.rfgsm_core_handle_ptr = rfgsm_core_handle_ptr;

    /* set the band */
    rfgsm_mc_set_band(&rfgsm_core_data, (rfgsm_band_type)gsm_band );

    geran_test_mode_api_rf_set_tx_band((int8)gsm_band,as_id);

    /* Set to default channel */
    vars_ptr->ftm_gsm_state.ftm_channel = default_channel;
  }
} /* end ftm_gsm_frame_sweep_set_band */


/*===========================================================================

FUNCTION  ftm_tdma_isr

DESCRIPTION
  this function is a TDMA frame processing handler for FTM
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ftm_tdma_isr( rfm_device_enum_type rfm_dev )
{
  rfgsm_core_data_type rfgsm_core_data;
#ifdef FTM_HAS_LOGGING
  ftm_log_gsm_therm_vbatt_type *ftm_log_pkt = NULL;
#endif
  rfgsm_core_handle_type *rfgsm_core_handle_ptr = NULL;

  sys_modem_as_id_e_type as_id = gl1_hw_rf_map_device2_as_id( rfm_dev, ftm_mode );
  ftm_gsm_rfctl_vars_s * vars_ptr = ftm_gsm_get_vars_ptr(rfm_dev);

  rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfm_dev);
  if ( rfgsm_core_handle_ptr == NULL )
  {
    MSG(MSG_SSID_FTM, MSG_LEGACY_ERROR,"rfgsm_core_handle_ptr NULL!");
    return;
  }

  /* Set the core data */
  rfgsm_core_data.rf_task = RF_TASK_FTM;
  rfgsm_core_data.rfm_dev = rfm_dev;
  rfgsm_core_data.sub_id = 0;
  rfgsm_core_data.as_id = 0; //temp until GL1 export as_id retrieval API
  rfgsm_core_data.triple_buffer_id = 0;
  rfgsm_core_data.rfgsm_core_handle_ptr = rfgsm_core_handle_ptr;

  /* Increment frame counter */
  vars_ptr->ftm_frame_count++;

  geran_test_mode_api_schedule_frame_tick (vars_ptr->ftm_frame_count,as_id);

  //Log the data 
#ifdef FTM_HAS_LOGGING
  ftm_log_pkt = (ftm_log_gsm_therm_vbatt_type *)ftm_log_malloc ((uint16)FTM_LOG_GSM_THERM_VBATT, sizeof( ftm_log_gsm_therm_vbatt_type) );
  if ( ftm_log_pkt != NULL )
  {

    ftm_log_pkt->therm_recorded      = therm_recorded;
    ftm_log_pkt->temp_comp_bin_index = temp_comp_bin_index;
    ftm_log_pkt->temp_comp_rem       = temp_comp_rem;
    ftm_log_pkt->temp_comp_val       = temp_comp_val;
    ftm_log_pkt->vbatt_recorded      = vbatt_recorded;
    ftm_log_pkt->vbatt_backoff       = vbatt_backoff;
  }
  /* send log */
  log_commit( (ftm_log_type*) ftm_log_pkt );
#endif  

  /* initialize RX/TX chain (channel filter and PA power profile etc.) */
  if ( vars_ptr->ftm_frame_count == RX_TX_START_INIT_CNT )
  {
    ftm_gsm_init_rx( rfm_dev );
    ftm_gsm_init_tx( rfm_dev );
    MSG_1(MSG_SSID_FTM, MSG_LEGACY_HIGH,"RX/TX Initialized on frame %d", vars_ptr->ftm_frame_count);

  }
  
  /* handler for FTM sweep funtionality*/
  ftm_gsm_frame_sweep_handler(rfm_dev);

  /* Perform the TX command if any */
  if ( vars_ptr->ftm_gsm_state.ftm_tx_state == ON )
  {

    if ( vars_ptr->ftm_cont_tx || ((vars_ptr->ftm_num_bursts_tx) ? vars_ptr->ftm_num_bursts_tx-- : vars_ptr->ftm_num_bursts_tx) )
    {
      /* if ftm_gsm_idle_proc_flag is true , idle processing is enabled*/
      if(( vars_ptr->ftm_frame_count % FTM_GSM_IDLE_FRAME_NUM == 1 ) && vars_ptr->ftm_gsm_idle_proc_flag) /* Idle frame*/   
      {
        if ( vars_ptr->ftm_gsm_cal_flag == FALSE ) /* if the cal is NOT in progress we can do IDLE frame processing */
        {
          if( !rfgsm_mc_do_idle_frame_processing(&rfgsm_core_data) )
          {
            MSG(MSG_SSID_FTM, MSG_LEGACY_ERROR,"rfgsm_core_handle_ptr NULL!");
            return;
          }
        }
      }
      if( ( vars_ptr->ftm_frame_count % FTM_GSM_IDLE_FRAME_NUM != 0 ) || 
          (! vars_ptr->ftm_gsm_idle_proc_flag) )/* do not call TX function on the frame immediately before the idle frame*/
      {
        (*vars_ptr->ftm_tx_funct)(rfm_dev);
      }
    }
  }
  else
  {
    ftm_gsm_do_tx_stop_cont( rfm_dev); 
  }

  /* Perform the RX command if any */
  if ( vars_ptr->ftm_cont_rx || 
       ( (vars_ptr->ftm_num_bursts_rx) ? vars_ptr->ftm_num_bursts_rx-- : vars_ptr->ftm_num_bursts_rx) )
  {
    (*vars_ptr->ftm_rx_funct)(rfm_dev);
   
  }
  if( vars_ptr->do_tx_da_cal ) 
  {
      ftm_da_cal_send_tx_cal_cmd( rfm_dev );

  }


  if ( vars_ptr->ip2_cal_data.do_ip2_cal )
  {

    ftm_gsm_do_ip2_cal(rfm_dev);
  }


  if (TRUE == vars_ptr->ftm_gsm_mode_enter)
  {
    /* Tick the DTM manager - must come after the DL and UL functions */
    geran_test_mode_api_sched_rx_tx(as_id);
  }

}

/*! @brief Calls the ISR fucntion as device 0 */
void ftm_tdma_isr_dev0( sys_modem_as_id_e_type as_id )
{
  as_id = as_id;

  ftm_tdma_isr( RFM_DEVICE_0 );
}

/*! @brief Calls the ISR fucntion as device 2 */
void ftm_tdma_isr_dev2( sys_modem_as_id_e_type as_id )
{
  as_id = as_id; 

  ftm_tdma_isr( RFM_DEVICE_2 );
}


/*===========================================================================

FUNCTION  ftm_send_FTM_STOP_GSM_MODE_REQ

DESCRIPTION
  This function sends a MPH_STOP_GSM_MODE_REQ to GSM L1
  
DEPENDENCIES
  None

RETURN VALUE
  Error code for 

SIDE EFFECTS
  None

===========================================================================*/
gs_status_T ftm_send_FTM_STOP_GSM_MODE_REQ( rfm_device_enum_type rfm_dev )
{
  sys_modem_as_id_e_type as_id = gl1_hw_rf_map_device2_as_id( rfm_dev, ftm_mode );
  gs_status_T status;


#ifdef FEATURE_DUAL_SIM
  status = geran_test_mode_api_send_FTM_STOP_GSM_MODE_REQ( as_id );
#else
  as_id = as_id;
  status =  geran_test_mode_api_send_FTM_STOP_GSM_MODE_REQ();
#endif /* FEATURE_DUAL_SIM */

  return status;

} /* ftm_send_FTM_STOP_GSM_MODE_REQ */

/*----------------------------------------------------------------------------*/
/*
@brief
    ftm_mutex_init() creates a mutex for the FTM Mode.
 
  @param None
    
 
  @retval None
    
 
  @details
  Mutexes are used to allow only one thread to enter the critical section
  of code that is accessing shared data. Thus, a given data structure which
  needs mutual exclusion will need to create a mutex to go with it. 
 
*/
void ftm_mutex_init(void)
{
  /* In Q6 both tasks and ISRs are handled the same way */
  rex_init_crit_sect(&ftm_gsm_lock);
}



/*!
@brief
   ftm_gsm_init_once does one time GSM FTM initialisation
 
  @param None
    
 
  @retval None
    
 
  @details
  Creates and populates the global variables required for FTM operation.
 
*/
void ftm_gsm_init_once( void )
{
  static boolean init_done_once = FALSE;

  if ( init_done_once == FALSE )
  {
    ftm_gsm_init_ftm_slot_mutex();

    ftm_mutex_init();

    init_done_once = TRUE;
  }
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Wait function 

  @details
  Waits for a given signal mask

  @param mask - signals to wait for.
  @param as_id - subscription id, not used

*/  
void ftm_gsm_sig_rex_wait_no_clear_cb
(
  rex_sigs_type   mask,        
  sys_modem_as_id_e_type as_id
)
{
  as_id = as_id;

  ftm_sig_rex_wait_no_clear_cb( mask );

} /* End ftm_gsm_sig_rex_wait_no_clear_cb */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Calls the geran test mode api start command 

  @details
  

  @param rfm_dev - Logical RF device

*/ 
void ftm_gsm_start_geran( rfm_device_enum_type rfm_dev )
{
  boolean enable_rxd = FALSE;
  uint64 rxd_dev_band_mask =0;
  /*Initialise the gl1_hw_init state to RX only type*/
  gl1_init_rflm_type gsm_init_rflm_state = GL1_HW_INIT_RX_ONLY;
  /*Get the as id from GL1*/
  sys_modem_as_id_e_type as_id = gl1_hw_rf_map_device2_as_id( rfm_dev, ftm_mode );
  /*Retrieve RXD device associated with a PRX Device*/
  rfm_device_enum_type rxd_dev = rfgsm_core_get_drx_dev(rfm_dev);
  /*Get pointer to ftm variables*/
  ftm_gsm_rfctl_vars_s * vars_ptr = ftm_gsm_get_vars_ptr( rfm_dev );
  sys_band_mask_type bit_mask;

  /*Check if rfm_device is tx capable*/
  if(rfgsm_core_get_device_tx_capability(rfm_dev))
  {
    gsm_init_rflm_state = GL1_HW_INIT_RX_TX;
  }

  /* Set the ISR function */
  switch ( rfm_dev )
  {
  case RFM_DEVICE_0:
    vars_ptr->ftm_tdma_isr_fnc = ftm_tdma_isr_dev0;
    break;
  case RFM_DEVICE_2:
    vars_ptr->ftm_tdma_isr_fnc = ftm_tdma_isr_dev2;
    break;
  default:
    MSG_1( MSG_SSID_FTM, MSG_LEGACY_ERROR, "Unsupported device %d", rfm_dev );
    return;
  }


  /*Query BC config NV for DRX to retrieve the band mask information*/
  rxd_dev_band_mask = rfcommon_nv_get_rf_bc_config( (rf_path_enum_type)(rxd_dev) );
   
  /* get the bit_mask for different bands */
  switch((ftm_mode_id_type)vars_ptr->ftm_mode_id)
  {
  case FTM_PHONE_MODE_GSM_850:
    bit_mask = SYS_BAND_MASK_GSM_850;
    break;
  case FTM_PHONE_MODE_GSM_900:
    bit_mask = SYS_BAND_MASK_GSM_EGSM_900;
    break;
  case FTM_PHONE_MODE_GSM_1800:
    bit_mask = SYS_BAND_MASK_GSM_DCS_1800;
    break;
  case FTM_PHONE_MODE_GSM_1900:
    bit_mask = SYS_BAND_MASK_GSM_PCS_1900;
    break;

  default:
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "invalid GSM FTM mode id to convert to rfcom gsm band, default to 850",
                                          vars_ptr->ftm_mode_id);
    bit_mask = SYS_BAND_MASK_GSM_850;
    break;				
  }

  /*Confirm if GSM Bands are supported*/
  if (rxd_dev_band_mask & bit_mask )
  {
    enable_rxd = vars_ptr->diversity_on;
    MSG_2( MSG_SSID_FTM, MSG_LEGACY_HIGH, "diversity is turned to %d on dev %d", enable_rxd, rfm_dev );  
  }

  /* this sets up rxlm and txlm buffers, requests mcpm resources for FTM, loads MDSP fw.  */
  geran_test_mode_api_start_geran( vars_ptr->ftm_tdma_isr_fnc,
                                   TASK_HANDLE(ftm),
                                   ftm_gsm_sig_rex_wait_no_clear_cb,
                                   FTM_SIG_WAIT_SIG, 
                                   FTM_SIG_WAIT_SIG2
                            #ifdef FEATURE_GERAN_NIKEL_HW_INIT
                                   , gsm_init_rflm_state
                            #endif
                                   ,as_id,
                                   enable_rxd,
                                   TRUE,
                                   TRUE
                                  );

  vars_ptr->ftm_gsm_mode_enter = TRUE;

}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Stops any RX and TX activity and calls the geran test mode api stop command 

  @details
  

  @param rfm_dev - Logical RF device

*/ 
void ftm_gsm_stop_geran( rfm_device_enum_type rfm_dev )
{
  ftm_gsm_rfctl_vars_s * vars_ptr = ftm_gsm_get_vars_ptr( rfm_dev );
  sys_modem_as_id_e_type as_id = gl1_hw_rf_map_device2_as_id( rfm_dev, ftm_mode );

  /* Disable TX */
  if ( vars_ptr->ftm_gsm_state.ftm_tx_state )
  {
    ftm_gsm_set_tx( rfm_dev, OFF );
  }

  /* Disable RX */
  ftm_gsm_rx_disable( rfm_dev );

  mdsp_ftm_wait_for_frame_tick(as_id);

  /* Tell the geran test mode API to stop*/
  geran_test_mode_api_stop_geran( TRUE, as_id );

  vars_ptr->ftm_gsm_mode_enter = FALSE;
}


/*===========================================================================

FUNCTION  FTM_GET_TX_FREQUENCY_OFFSET

DESCRIPTION
   This function gets the GSM TX frequency offset set in FTM mode. In online mode it returns 0.
   
DEPENDENCIES
   None.

RETURN VALUE
  GSM TX frequency offset set in FTM mode.

SIDE EFFECTS
   None.

===========================================================================*/
int32 ftm_gsm_get_tx_frequency_offset( void  )
{
  if ( ftm_gsm_vars_created(ftm_gsm_current_tx_cal_dev) )
  {
    return ftm_gsm_get_vars_ptr(ftm_gsm_current_tx_cal_dev)->ftm_gsm_tx_freq_offset;
  }
  else
  {
    /* Not in FTM mode or frequency offset not set */
    return 0;
  }
}

/*===========================================================================

FUNCTION  FTM_SET_TX_FREQUENCY_OFFSET

DESCRIPTION
   This function sets the GSM TX frequency offset set in FTM mode.
   
DEPENDENCIES
   None.

RETURN VALUE
  None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_gsm_set_tx_frequency_offset( rfm_device_enum_type rfm_dev, 
                                      int32 offset )
{  
  if( ftm_gsm_vars_created(rfm_dev) )
  {
    ftm_gsm_get_vars_ptr(rfm_dev)->ftm_gsm_tx_freq_offset = offset;

    ftm_gsm_current_tx_cal_dev = rfm_dev;

    MSG_2( MSG_SSID_FTM, MSG_LEGACY_HIGH,
           "GSM TX Freq offset for device %d set to %d", rfm_dev, offset );
  }
}


/*============================================================================

FUNCTION 
  ftm_gsm_program_qfe_scenario

DESCRIPTION
  This API will be called to program the tuner immediately and no script or FW 
  action is required. Inside this API check all the active paths then give a call to
  tuner manager associated with appropriate tuners

DEPENDENCIES
  None

RETURN VALUE
  Flag indicating if the api operation was successful (TRUE) or failed (FALSE)

SIDE EFFECTS
  None
=============================================================================*/

void ftm_gsm_program_qfe_scenario(void)
{
  boolean status = FALSE;

  status = rfgsm_mc_program_qfe_scenario();

  if(!status)
  {
    MSG_1(MSG_SSID_FTM, MSG_LEGACY_ERROR,"FTM_GSM_PROGRAM_QFE_SCENARIO: Failed ", 0);
  }
}

/*----------------------------------------------------------------------------*/
#endif //FEATURE_GSM
#endif // FTM_HAS_UMTS
#endif //FEATURE_FACTORY_TESTMODE

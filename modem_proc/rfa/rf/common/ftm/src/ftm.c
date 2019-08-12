/*!
  @file
  ftm.c

  @brief
  This module contains Control code for FTM.
*/

/*==============================================================================

  Copyright (c) 2002 - 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/src/ftm.c#1 $
   
when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
12/09/14   pl      Back out FTM MCPM request co-ordination between ftm APIs and 
                   FTM protocol activation\deactivation 
10/08/14   pl      Added FTM MCPM request co-ordination between ftm APIs and 
                   FTM protocol activation\deactivation 
01/03/14   svi     LTE B27 Support for BOLT
08/14/13   kab     LTE B30 Support
07/16/13   dj      Removed legacy Tx internal Cal support
04/22/13   zg      Added more information in the error messages.
04/09/13   cri     Add init and deinit for cal data module
03/11/13   sar     Updated file for APQ, GNSS only target.
03/07/13   bsh     B28/B29 support 
03/01/13   zhw     Request RF_start/stop for FTM mode 
02/26/13   tks     Add support to extract tech info from ftm_rf_mode_type 
02/19/13   tws     Handle activation/deactivation in the FTM task context
02/15/13   tws     Back out previous change.
02/17/13   tws     Handling of FTM activate and deactivate CMD from MMOC task has been added
11/30/12   tws     Call ftm_gsm_init to setup gsm ftm variables. Tidy up code.
11/09/12   gvn     Add new LTE bands 14,23,26,42 (merge from Nikel)
04/24/12   APU     Delayed going into AMSS Mode in ftm_deactivate_protocol() 
                   until phone calibration is complete.
06/07/12   pv      Added ftm_lte_init back to make sure FTM->Online->FTM works without 
                   power cycle. 
06/07/12   pv      Removal of ftm_lte_init from ftm_activate_protocol
05/03/12   id      Put Internal Device Cal command on FTM task at bootup
04/16/12   jf      Add LTE B6 and B10 
04/24/12   jfc     Use TASK_HANDLE instead of rex_self()
03/22/12   id      Fixed compiler warning
03/22/12   id      Added support to do Tx Internal Dev Cal in FTM mode at boot
03/19/12   jfc     Removed unused okts sleep code
02/14/12   sar     Reinstated TDSCDMA features.
01/20/12   php     Added support for B34 and B39
01/28/12   sar     Feature reduction changes.
01/17/12   aro     Added "New Mode" as a argument in FTM EXIT function
12/22/11   sbm     Added LTE band 9 support.
12/05/11   sbm     Added XGP band (B41) 
09/28/11   sr      Fixed FTM to online mode transition crash for GSM
09/22/11   jyu     Added LTE B12 support 
08/05/11   whc     Added B25 Support
07/11/11   hy      Including TD-SCDMA support.
06/21/11   aro     Moved ftm_sys_control functionality to ftm_common_control
06/16/11   sar     Including internal 1x interface.
06/07/11   aro     Mainlined FEATURE_RF_SVDO_API
05/14/11   ad      Fix compiler warning
05/13/11   sar     Fixed a merge error.
05/13/11   sar     Fixed compiler errors.
04/20/11   sar     Merged 1X API cleanup changes.  
04/27/11   dw      Fix warnings 
04/26/11   dw      UMTS clean up in activate/deactivate protocols
04/20/11   pl      use ftm_lte_msgr_init/deinit functions for activate and de-activate
03/24/11   aro     Renamed FTM system control filename
03/24/11   aro     [1] Removed unused functions
                   [2] Cleaned up Activate and de-activate FTM functions
03/18/11   aro     Removed old FTM clock config to be repalced with MCPM
03/08/11   aro     Compiler Warning Fix
03/03/11   tks     Added support for WCDMA Band XIX.  
02/02/11   aro     Added FTM CDMA Init
01/31/11   bmg     Re-arranged and documented ftm_mode variable
01/14/11   can     Initial B18 bring-up on J2 HW.	
01/13/11   mkv     Clean up LTE FTM FW and RF upon exiting FTM mode 
01/11/11   can     Initial B5 bring-up on Viking A1 HW.	 
12/17/10   aro     Deleted obsolete Code
12/14/10   aro     Removed rf_chain_status and expanded Status field
12/13/10   aro     Added Fixes to remove Linker Error on SVDO Build
12/09/10   aro     Fixes for Zero-Compile Error SVDO Enabled Build
12/08/10   aro     Fixed include filename
12/07/10   aro     Added Support for SV Enabled API
15/10/10   mkv     Support LTE Composite Calibration 
08/05/10   anr     Added support for Enhanced Therm API in 1x mode
07/13/10   lcl     Added vote to prevent TCXO shutdown in FTM mode.
05/12/10   ac       B11 bringup on j8220
03/25/10   pl/mkv  Adding support for setting L! to ONLINE mode when FTM exits 
03/19/10   lcl     Pull in fixes from QSC6295
03/03/10   Vish    Merged from prerel/poseidon2.1.x: 01/21/10 - 02/02/10 
02/02/10   sar     Removed definition for unused variable ftm_beta.
01/28/10   sar     Removed lint warnings.
01/21/10   sar     Lint fixes.
02/11/10   mkv     Moving LTE FW enable/disable app into LTE specific area. 
01/28/10   mkv     Adding LTE FW enable/disable app.
01/20/10   pl      Register FTM LTE NS messages when entering FTM mode
01/12/10   sr      Mainlined RF_HAS_MDSP_TX_AGC
01/06/10   sar     Renamed ftm_task.h to rf_test_task_v.h.
11/11/09   ka      Featurization for WPLT.
09/15/09   sr      Miseed one instance of rf_card in previous checkin
09/15/09   sr      Replaced use of rf_card with rfc_card
09/14/09   sr      Fixed merge errors
09/13/09   sr      Fixed compile errors
09/09/09   aro     Featurized Code with FEATURE_RF_SVDO_API
                   + Removed Debug Variable
                   + Formatting: Replaced Tab with space 
09/08/09   Saul    Featurized call to msm_support_is_available with FEATURE_MSM_SUPPORT
                   to resolve linker error.
04/24/09   aro     Enabling RF_SUPPORTS_SVDO + Code Fixes
03/24/09   aro     Initial SVDO Support
12/12/08   ahg     Featurized  the below change with FEATURE_SRCH_CLKREGIM_RM
12/10/08   ahg     Modem clocks are enabled when set to FTM
11/27/08   ahg     Added changes to support 3U2C4G RF card
05/16/08   kma     Added support for platform F raptor2 and UCG224
04/25/08   jfc     Compute FTM backoff from MPR beta scaling NV item [0]
04/25/08   jfc     Added support for Gobi2000 data card.
04/08/08   dw      Removed obsolete targets support
03/21/08   ad      Create runtime configurability for rfrx_enable_chip_reset().
03/05/08   ad      Resolve msm_support() compilation issues on TRF6285 targets 
02/19/08   avi     Added support for msm_support() layer to enable MM
                   execution on UMTS only targets
02/06/08   ad      Moved ftm_db_to_rf_mode() here from ftm_wcdma_ctl.c to fix compiler warnings
01/21/08   ad      Support for ftm_enable_chip_reset()
01/12/08   rmd     For MSM6800 and MSM7500-90 TRF6285, in 
                   ftm_activate_protocol(), call rfm_enable_rx instead of 
                   ftm_rfmode_clk_init. This fixes GL1 error fatal when we try 
                   to make a call after we do a switch from ftm to online mode.
10/26/07   rsr     Added support for PLATFORM_F_SMPS
10/18/07   jfc     Separate EU and JP cases to properly set ftm_wcdma_ul_betac &
                    ftm_wcdma_ul_betac. Fixes 2dB of Tx cal error in BC8 amd BC9
10/05/07    ka     Added support for QRF7600.
08/27/07   adk     Updated function ftm_activate_protocol().
08/06/07   jfc     Featurized code for EDGE-only builds
04/13/07   ycl     Replace Platfrom G with HW U2.
04/13/07   ra      wrapped ftm_init_target_dependent_settings() with FTM_HAS_WCDMA
03/20/07   jfc     Use either full scale or 2dB backoff depending on target
02/27/07   jfc     Removed ftm_tx_beta_gain from ftm_init_target_dependent_settings()
02/15/07   ycl     Added support for U2 (Platform G).
11/08/06   ra      changed feature FEATURE_LCP with FEATURE_MMGPS to check inlcude lsm.h
10/31/06   ycl     Change ftm_hwtc_dc_read_disable() calls.
09/29/06   jfc     Added support for target dependent TX BETA GAIN
05/18/06   tm      Initial release for dsp diagnostics driver - new FTM subsystem.
                   Event mechanism to conserve signal defs, and support for delayed
                   response.
05/10/06   ra      For 1x added logic for sleep voting
02/21/06   tws     Change WCDMA_1800 to BC3. Add support for BC4 and BC9.
01/18/06   sar     Replaced reference to RF_STATE_GPS with DB_RF_GPS in ftm_deactivate_protocol().. 
08/01/05   ra      For 1x: removed call to change rfm_initialized due to Park Mode changes
07/20/05   dp      Ensure that FTM WCDMA BER sends stop request to L1 while ftm_mode = FTM_MODE
                   so that L1 sends the response back to FTM, not RRC.
07/12/05   jac     Added line to set ftm_current_rf_mode = FTM_DB_RF_IDLE in
                   ftm_deactivate_protocol().
06/23/05   jac     Added variable for GSM 1X change. 
06/20/05   jac     Added code to send a deactivate command to the searcher when 
                   exiting FTM mode to fix phone crash when transitioning from FTM to
                   online after entering FTM GSM RF mode. 
03/18/05   ra      Removed FTM_HAS_UMTS wrapper from ftm_get_mode() to allow all system mode access.
                   Also, deleted ftm_set_mode api located in this file(there is a 1x specific ftm_set_mode which has different 
                   functionality and is not part of this change). It set the global variable ftm_mode however this is to 
                   be done in ftm_activate_protocol and ftm_deactivate_protocol. 
02/09/05   ra      For 1x targets, fixed mode switch ftm->online for all modes.
01/07/05   jb      Removed rfm_init_digital from ftm_activate_protocol to fix 2nd chain call
11/29/04   ra      Changed 1x section of ftm_activate_protocol
11/23/04   ra      For 1x targets, set rfm_initialized global variable to false in ftm_deactivate_protocol to that the rf_apinit for the 3 device desriptors gets 
                   executed in rfm_init 
11/18/04   dp      Added FTM BER cleanup function calls in ftm_deactivate_protocol
10/12/04   jac     Mainlined ASYNC packet code.
10/01/04   ra      Renamed FTM_GET_RF_MODE to FTM_GET_RF_DB_STATE.
10/01/04   ra      Renamed FTM_SET_RF_MODE to FTM_SET_RF_DB_STATE.
09/27/04   et      initialized the ftm_current_rf_mode to FTM_DB_RF_IDLE
                   unconditionally for all targets.
09/27/04   xw      Included task.h to fix saber P1 compilation error
09/03/04   dp      Added access functions for ftm mode variables
                   Added support for FTM GSM BER mode
08/24/04   ra      Merge 1X and UMTS
08/23/04   xw      Added FTM PMIC dispatch. 
08/20/04   xw      Removed ftm_wait LED for Saber.
10/02/03   ka      Added support for I2C.
10/02/03   ka      Added support for variable lenght response packet.
08/21/03   xw      Added check to abort FTM commands if we are not in FTM mode.
                   For FTM_HWTC builds, this check is ignored and FTM commands
                   are executed in non FTM modes.
05/23/03   xw      Added CPHY_STOP_WCDMA_MODE_REQ cmd in ftm_deactivate_protocol()
05/06/03   xw      Changed mmoc_rpt_ph_stat_chgd_cnf() to mmoc_rpt_prot_deactd_cnf()
                   in ftm_deactivate_protocol()
04/25/03   xw      Reduced FTM heap size from 2048*2 to 2048 
                   Modified ftm_wait() and added ftm_gsm_wait() for gl1_hw_init()
                   Modified ftm_clk_rex_wait()
                   Removed case FTM_1X_C in ftm_req()
                   Moved tmc_task_start() out of ftm_task_init() and placed it after 
                   ftm_task_init()  
02/20/03   xw      Added ftm_task support
08/25/02   sb      Initial revision

==============================================================================*/

#include "rfa_variation.h"
#include "target.h"
#include "comdef.h"

#ifdef FEATURE_FACTORY_TESTMODE
#include "ftmicap.h"
#include "ftm.h"
#include "ftmdiag.h"
#include "rf_test_task_v.h"
#include "assert.h"
#include "task.h"
#include "err.h"
#include "rfcom.h"
#include "rfc_card.h"
#include "ftm_msg.h"
#include "rfm_device_types.h"
#include "ftm_common_control.h"
#include "diagcmd.h"
#include "diag.h"
#include "ftm_rf_cmd.h"
#include "mcpm_api.h"

#ifdef FEATURE_WCDMA
#ifdef FEATURE_MMGPS
#include "lsm.h"
#endif
#include "l1task.h"
#endif

#ifdef FEATURE_GSM
#include "gl1_hw.h"
#include "mmoc.h"
#include "ftm_gsm_ctl.h"
#endif 

#ifdef FEATURE_FTM_HWTC
#include "ftm_hwtc_dc.h"
#endif

#include "rfm.h"

#if defined(FEATURE_GSM) && defined(FEATURE_CDMA1X)
#include "onex_non_sig.h"
#endif //defined(FTM_HAS_GSM) && defined(FEATURE_CDMA1X)

#if defined(FEATURE_CDMA1X) && defined(FEATURE_WCDMA)
//jcen #include "msm_support.h"
#endif

#if defined(FTM_HAS_UMTS)
#include "rfrx.h"
#include "rfnv_wcdma.h"
#endif

#ifdef FEATURE_CDMA1X
#include "rfm_1x.h"
#include "rf_cdma_data.h"
#include "ftm_cdma_control.h"
#endif /* FEATURE_CDMA1X */

#ifdef FEATURE_LTE
#include "ftm_lte_common_msgr_helper_functions.h"
#include "ftm_lte_ns_proc.h"
#include "rf_lte_ftm_cmd_proc.h"
#endif

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Factory Test Mode state variable indicating the current operating mode
  of the RF Test Task.

  @details
  When this variable is set to AMSS_MODE, the RF Test Task treats the
  phone operating mode as Online and test commands are disabled.  If
  it is FTM_MODE, MMOC has activated the "FTM protocol" and test
  commands can be issued.
*/
ftm_mode_type ftm_mode = AMSS_MODE;

/* To track the current RF mode */
extern ftm_rf_mode_type ftm_current_rf_mode;


/*===========================================================================
 
                   LOCAL GLOBAL VARIABLES and CONSTANTS
===========================================================================*/
#ifdef FTM_HAS_UMTS
  #define FTM_WCDMA_UL_BETA_C_FS 12616
  #define FTM_WCDMA_UL_BETA_D_FS 12616
  
  #define FTM_WCDMA_UL_BETA_C_2DB_BO 10093
  #define FTM_WCDMA_UL_BETA_D_2DB_BO 10093

  /* Do not change defaults from 2dB backoff.
     There are dependencies on these settings */
  uint32 ftm_wcdma_ul_betac[RFCOM_NUM_WCDMA_BANDS] =
  {
    FTM_WCDMA_UL_BETA_C_2DB_BO,         /* IMT */
    FTM_WCDMA_UL_BETA_C_2DB_BO,         /* 1900 */
    FTM_WCDMA_UL_BETA_C_2DB_BO,         /* BC3 */
    FTM_WCDMA_UL_BETA_C_2DB_BO,         /* BC4 */
    FTM_WCDMA_UL_BETA_C_2DB_BO,         /* 800 */
    FTM_WCDMA_UL_BETA_C_2DB_BO,         /* BC8 */
    FTM_WCDMA_UL_BETA_C_2DB_BO          /* BC9 */
  };
  uint32 ftm_wcdma_ul_betad[RFCOM_NUM_WCDMA_BANDS] = 
  {
    FTM_WCDMA_UL_BETA_C_2DB_BO,         /* IMT */
    FTM_WCDMA_UL_BETA_C_2DB_BO,         /* 1900 */
    FTM_WCDMA_UL_BETA_C_2DB_BO,         /* BC3 */
    FTM_WCDMA_UL_BETA_C_2DB_BO,         /* BC4 */
    FTM_WCDMA_UL_BETA_C_2DB_BO,         /* 800 */
    FTM_WCDMA_UL_BETA_C_2DB_BO,         /* BC8 */
    FTM_WCDMA_UL_BETA_C_2DB_BO          /* BC9 */
  };

  extern uint16 ftm_compute_beta_backoff (uint16 mpr_beta_scale);
#endif /* FTM_HAS_UMTS */
  
  /* Set the default ftm_current_rf_mode to FTM idle */
  ftm_rf_mode_type ftm_current_rf_mode = FTM_DB_RF_IDLE;
  
/* Command queue for the test task.  All command to this task will be
** placed on this queue to await processing.
*/
q_type ftm_task_cmd_q;

/* Event queue for the task.  General commands (not 75 commands from Diag)
** will be placed on this queue to await processing.
*/
q_type ftm_task_event_q;

/*===========================================================================

FUNCTION FTM_CMD

DESCRIPTION  
   This function is used to send command to the ftm task.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_cmd
(
   ftm_msg_type *msg_ptr   
   /* pointer to a test command packet.  When the command has finished,
   ** the command structure is put on the "done queue". If there is no
   ** done queue", or this operation is not desirable, set the "done queue"
   ** pointer to NULL.  Also when the command has completed, the specified
   ** task is signaled with the sig parameter.  If no signaling is required,
   ** the task pointer parameter can be set to NULL.
   */
)
{
  (void) q_link(msg_ptr, &msg_ptr->hdr.link );      /* Initialize link field */

  q_put( &ftm_task_cmd_q, &msg_ptr->hdr.link );         /* Put on command queue */

  (void) rex_set_sigs( TASK_HANDLE(ftm), FTM_CMD_Q_SIG ); /* Signal a queue event */

} /* End ftm_cmd */



/*===========================================================================

FUNCTION FTM_EVENT

DESCRIPTION  
   This function is used to send events to the ftm task.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_event
(
   ftm_event_type *evt_ptr   
   /* see ftm_cmd */
)
{
  (void) q_link(evt_ptr, &evt_ptr->hdr.link );    /* Initialize link field */

  q_put( &ftm_task_event_q, &evt_ptr->hdr.link ); /* Put on command queue  */

  (void) rex_set_sigs( TASK_HANDLE(ftm), FTM_EVT_Q_SIG ); /* Signal a queue event  */

} /* End ftm_event */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Activate FTM Protocol Test

  @details
  The function is responsible for getting MB ready to FTM mode
  and is used only during off target testing.

  @param act_reason
  Enumeration indicating the reason for FTM protocol activate
*/
void
ftm_activate_protocol_offtarget_test
(
  prot_act_e_type act_reason
)
{
  act_reason = act_reason; /* Lint */
  ftm_mode = FTM_MODE;
#ifdef FEATURE_RF_SUPPORTS_TX_INT_CAL
  /* Request Packet */
  ftm_common_tx_int_dev_cal_rsp_pkt_type ftm_req_data;
#endif /* FEATURE_RF_SUPPORTS_TX_INT_CAL */

#ifdef FEATURE_LTE
  ftm_lte_init();
#endif /* FEATURE_FTM_HAS_LTE */

#ifdef FEATURE_CDMA1X
  (void)ftm_cdma_init();
  dog_autokick(FALSE);
  ftm_sig_rex_set( FTM_ENABLE_PROT_SIG ); 
#endif /* FEATURE_CDMA1X */


  FTM_MSG( FTM_HIGH, "Activate FTM Protocol" );

#ifdef FEATURE_RF_SUPPORTS_TX_INT_CAL
  FTM_MSG( FTM_HIGH, "Doing Tx Internal Cal"  );

  ftm_req_data.cmd = DIAG_SUBSYS_CMD_F;
  ftm_req_data.sub_sys = FTM;
  ftm_req_data.mode = FTM_COMMON_C;
  ftm_req_data.cdma_cmd = FTM_INT_PERFORM_INT_DEV_CAL;
  ftm_req_data.req_len = 0;
  ftm_req_data.rsp_len = 0;
  
  /* Call the Int Cal function to do the get the results */
  ftm_common_send_radio_command(&ftm_req_data, sizeof(ftm_common_tx_int_dev_cal_rsp_pkt_type));
#endif /* FEATURE_RF_SUPPORTS_TX_INT_CAL */

} /* ftm_activate_protocol_offtarget_test */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Activate FTM Protocol

  @details
  The function is responsible for getting MB ready to FTM mode.
  
  @param caller_tcb_ptr
  The tcb_ptr from which the function is getting called

  @param task_nv_wait_sig
  Signal on which the caller_tcb waits till deactivation is done

  @param rex_sigs_type
  Signal type like wait etc

  @param act_reason
  Enumeration indicating the reason for FTM protocol activation

  @param trans_id
  Command sequence number type in MMOCs command to different protocols
*/  
void ftm_activate_protocol( rex_tcb_type *caller_tcb_ptr ,
                            rex_sigs_type task_nv_wait_sig,
                            void (*task_wait_func_ptr)( rex_sigs_type ),
                            prot_act_e_type act_reason, 
                            prot_trans_type trans_id )
{
  /* A Request packet is formed and queued in the FTM command Q (goes through FTM common Dispatcher) */
  ftm_activate_pkt_type *req_pkt;
  ftm_msg_type *ftm_msg;

  req_pkt = ( ftm_activate_pkt_type *) ftm_malloc(sizeof(ftm_activate_pkt_type));

  if (!req_pkt)
  {
    FTM_MSG_ERROR("Failed to allocate memory for req. pkt in  FTM activate protocol ",0,0,0);
    return;
  }

   /* Filling the Required Parameters for the req_pkt */	       
   req_pkt->subsys_hdr.cmd_code         =  75;
   req_pkt->subsys_hdr.subsys_id        =  FTM;
   req_pkt->subsys_hdr.subsys_cmd_code  =  FTM_COMMON_C;
   req_pkt->ftm_hdr.cmd_id 	          =  FTM_ACTIVATE_FTM_MODE;
   req_pkt->act_reason                  =  act_reason;
   req_pkt->trans_id                    =  trans_id;
   req_pkt->caller_tcb_ptr              =  caller_tcb_ptr;
   req_pkt->task_nv_wait_sig            =  task_nv_wait_sig;

   ftm_msg = ftm_malloc(sizeof(ftm_msg_type));

   if (!ftm_msg)
   {
     /* Free the request packet */
     ftm_free(req_pkt);

     FTM_MSG_ERROR("Failed to allocate memory for ftm_msg in FTM activate protocol",0,0,0);
     return;
   }

   ftm_msg->req_pkt_len = sizeof(ftm_activate_pkt_type);
   ftm_msg->ftm_data_ptr = (void *) req_pkt;
   ftm_msg->hdr.done_q_ptr = NULL;
   ftm_msg->data_rsp_ptr = NULL;
   ftm_msg->rsp_pkt_len = 0;

   rex_clr_sigs(caller_tcb_ptr,req_pkt->task_nv_wait_sig);

   MSG_1(MSG_SSID_FTM, MSG_LVL_HIGH,"Send CMD to FTM task for activation",0);

   /* add the cmd to FTM task queue and wait for act_done_sig from FTM task */
   ftm_cmd(ftm_msg);	

   MSG_1(MSG_SSID_FTM, MSG_LVL_HIGH,"Wait for Response from FTM task", 0);

   task_wait_func_ptr(task_nv_wait_sig);
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  De-Activate FTM Protocol

  @details
   The function is responsible for release any resource used by FTM mode.

  @param caller_tcb_ptr
  The tcb_ptr from which the function is getting called

  @param task_nv_wait_sig
  Signal on which the caller_tcb waits till deactivation is done

  @param rex_sigs_type
  Signal type like wait etc
 
  @param deact_reason
  The reason for deactivation
 
  @param trans_id
  The transaction ID.
 
  @note
  RF_IDLE mode is used for FTM BER measurements.  Need to guarantee a STOP_GSM_MODE_REQ
  is sent to GSM L1 with ftm_mode = AMSS_MODE, so that GSM L1 is properly returned to a
  default state before beginning switching out of FTM mode
*/
void ftm_deactivate_protocol( rex_tcb_type *caller_tcb_ptr,
                              rex_sigs_type task_nv_wait_sig,
                              void (*task_wait_func_ptr)( rex_sigs_type ),
                              prot_deact_e_type deact_reason, 
                              prot_trans_type trans_id )
{

  /* A Request packet is formed and queued in the FTM command Q (goes through FTM common Dispatcher) */
  ftm_deactivate_pkt_type *req_pkt;
  ftm_msg_type *ftm_msg;

  req_pkt = ( ftm_deactivate_pkt_type *) ftm_malloc(sizeof(ftm_deactivate_pkt_type));

  if (!req_pkt)
  {
    FTM_MSG_ERROR("Failed to allocate memory for req. pkt in  FTM deactivate protocol ",0,0,0);
    return;
  }

  /* Filling the Required Parameters for the req_pkt */	
  req_pkt->subsys_hdr.cmd_code         =  75;
  req_pkt->subsys_hdr.subsys_id        =  FTM;
  req_pkt->subsys_hdr.subsys_cmd_code  =  FTM_COMMON_C;
  req_pkt->ftm_hdr.cmd_id              =  FTM_DEACTIVATE_FTM_MODE;
  req_pkt->deact_reason                =  deact_reason;
  req_pkt->trans_id                    =  trans_id;
  req_pkt->caller_tcb_ptr              =  caller_tcb_ptr;
  req_pkt->task_nv_wait_sig            =  task_nv_wait_sig;

  ftm_msg = ftm_malloc(sizeof(ftm_msg_type));

  if( ftm_msg == NULL )
  {     
    /* Free the request packet */
    ftm_free(req_pkt);

    FTM_MSG_ERROR("Failed to allocate memory for ftm_msg in FTM deactivate protocol",0,0,0);
    return;
  }

  ftm_msg->req_pkt_len = sizeof(ftm_deactivate_pkt_type);
  ftm_msg->ftm_data_ptr = (void *) req_pkt;
  ftm_msg->hdr.done_q_ptr = NULL;
  ftm_msg->data_rsp_ptr = NULL;
  ftm_msg->rsp_pkt_len = 0;

  rex_clr_sigs(caller_tcb_ptr,req_pkt->task_nv_wait_sig);

  MSG_1(MSG_SSID_FTM, MSG_LVL_HIGH,"Send CMD to FTM task for de-activation",0);

  /* add the cmd to FTM task queue and wait for deact_done_sig from FTM task */
  ftm_cmd(ftm_msg);	

  MSG_1(MSG_SSID_FTM, MSG_LVL_HIGH,"Wait for Response from FTM task", 0);

  task_wait_func_ptr(task_nv_wait_sig);
} /* end of ftm_deactivate_protocol */


/*===========================================================================

FUNCTION FTM_GET_MODE

DESCRIPTION
     

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
ftm_mode_type  ftm_get_mode( void)
{
  return ftm_mode;
}

/*===========================================================================

FUNCTION FTM_GET_RF_DB_STATE

DESCRIPTION
     

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
ftm_rf_mode_type  ftm_get_rf_db_state( void )
{
    return ftm_current_rf_mode;
}


/*===========================================================================

FUNCTION FTM_SET_RF_DB_STATE

DESCRIPTION
     

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_set_rf_db_state( ftm_rf_mode_type  mode)
{
    ftm_current_rf_mode = mode;
}

#if defined(FEATURE_UMTS) || defined(FEATURE_GSM) || defined(FTM_HAS_TDSCDMA)
/*===========================================================================

FUNCTION FTM_DB_TO_RF_MODE

DESCRIPTION
  This function converts the ftm_rf_mode_type to ftm_mode_id_type.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
ftm_mode_id_type ftm_db_to_rf_mode( ftm_rf_mode_type rfmode )
{
  ftm_mode_id_type mode_id;

  switch( rfmode )
  {
  case FTM_DB_RF_CDMA:                     /* RF is in CDMA band (800 or 1900) */
    mode_id = FTM_PHONE_MODE_CDMA;
    break;
  
  case FTM_DB_RF_CDMA_800:                 /* RF is in CDMA 800 mode of operation */
    mode_id = FTM_PHONE_MODE_CDMA_800;
    break;
  
  case FTM_DB_RF_CDMA_1900:                /* RF is in CDMA 1900 mode of operation */
    mode_id = FTM_PHONE_MODE_CDMA_1900;
    break;
  
  case FTM_DB_RF_WCDMA_IMT:                /* RF is in WCDMA IMT mode of operation          */
    mode_id = FTM_PHONE_MODE_WCDMA_IMT;
    break;
  
  case FTM_DB_RF_WCDMA_1900MHZ_A:          /* RF is in 1900MHz WCDMA, band A of operation   */
    mode_id = FTM_PHONE_MODE_WCDMA_1900A;
    break;
  
  case FTM_DB_RF_WCDMA_1900MHZ_B:          /* RF is in 1900MHz WCDMA, band B of operation   */
    mode_id = FTM_PHONE_MODE_WCDMA_1900B;
    break;
  
  case FTM_DB_RF_GSM_900:                  /* RF is in EGSM mode of operation               */
    mode_id = FTM_PHONE_MODE_GSM_900;
    break;
  
  case FTM_DB_RF_GSM_1800:                 /* RF is in GSM DCS mode of operation            */
    mode_id = FTM_PHONE_MODE_GSM_1800;
    break;
  
  case FTM_DB_RF_GPS:                      /* RF is in GPS mode of operation                */
    mode_id = FTM_PHONE_MODE_GPS;
    break;
  
  case FTM_DB_RF_SLEEP:                    /* RF is in sleep mode of operation              */
    mode_id = FTM_PHONE_MODE_SLEEP;
    break;
  
  case FTM_DB_RF_IDLE:                     /* RF is in idle FTM mode of operation           */
    mode_id = FTM_PHONE_MODE_SLEEP;   /* ??? no eqivalent */
    break;
  
  case FTM_DB_RF_GSM_850:                  /* RF is in GSM 850 band                         */
    mode_id = FTM_PHONE_MODE_GSM_850;
    break;
  
  case FTM_DB_RF_GSM_1900:                 /* RF is in GSM 1900 band                        */
    mode_id = FTM_PHONE_MODE_GSM_1900;
    break;
  
  case FTM_DB_RF_WCDMA_BC3:                /* RF is in WCDMA 1800MHz BC3 band               */
    mode_id = FTM_PHONE_MODE_WCDMA_BC3;
    break;
  
  case FTM_DB_RF_WCDMA_BC4:                /* RF is in WCDMA 17/21 BC4 band                 */
    mode_id = FTM_PHONE_MODE_WCDMA_BC4;
    break;
  
  case FTM_DB_RF_WCDMA_800MHZ:             /* RF is in WCDMA 800MHz band                    */
    mode_id = FTM_PHONE_MODE_WCDMA_800;
    break;
  
  case FTM_DB_RF_WCDMA_BC8:                /* RF is in WCDMA 900MHz BC8 band                */
    mode_id = FTM_PHONE_MODE_WCDMA_BC8;
    break;
  
  case FTM_DB_RF_WCDMA_BC9:                /* RF is in WCDMA 1800MHz BC9 band               */
    mode_id = FTM_PHONE_MODE_WCDMA_BC9;
    break;
  
  case FTM_DB_RF_WCDMA_BC11:                /* RF is in WCDMA 1500MHz BC11 band               */
    mode_id = FTM_PHONE_MODE_WCDMA_BC11;
    break;

  case FTM_DB_RF_WCDMA_BC19:               /* RF is in WCDMA BC19 band */
    mode_id = FTM_PHONE_MODE_WCDMA_BC19;
    break;    
  
  case FTM_DB_RF_TDSCDMA_B34:                /* RF is in TDSCDMA Band 34*/
    mode_id = FTM_PHONE_MODE_TDSCDMA_B34;
    break;

  case FTM_DB_RF_TDSCDMA_B39:                /* RF is in TDSCDMA Band 39*/
    mode_id = FTM_PHONE_MODE_TDSCDMA_B39;
    break;

  case FTM_DB_RF_TDSCDMA_B40:                /* RF is in TDSCDMA Band 40*/
    mode_id = FTM_PHONE_MODE_TDSCDMA_B40;
    break;

  default:
    FTM_MSG_ERROR("Unknown ftm_rf_mode_type: %d",rfmode,0,0);
    mode_id = FTM_PHONE_MODE_SLEEP;
    break;
  }
  return mode_id;

}
#endif /* FTM_HAS_UMTS||FTM_HAS_GSM */

/*===========================================================================

FUNCTION FTM_GET_RF_TECHNOLOGY

DESCRIPTION Returms the current technology (1x, GSM, WCDMA) in which the phone is operating
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
ftm_rf_technology_type ftm_get_rf_technology ( void )
{
  ftm_rf_technology_type ftm_rf_tech;

  switch( ftm_current_rf_mode )
  {
     case FTM_DB_RF_CDMA:
     case FTM_DB_RF_CDMA_800:
     case FTM_DB_RF_CDMA_1900:
        ftm_rf_tech = FTM_RF_TECH_CDMA;
        break;
  
     case FTM_DB_RF_WCDMA_IMT:
     case FTM_DB_RF_WCDMA_1900MHZ_A:
     case FTM_DB_RF_WCDMA_1900MHZ_B:
     case FTM_DB_RF_WCDMA_BC3:
     case FTM_DB_RF_WCDMA_BC4:
     case FTM_DB_RF_WCDMA_BC8:
     case FTM_DB_RF_WCDMA_BC9:
     case FTM_DB_RF_WCDMA_BC11:
     case FTM_DB_RF_WCDMA_BC19:
     case FTM_DB_RF_WCDMA_800MHZ:
        ftm_rf_tech = FTM_RF_TECH_WCDMA;
        break;

     case FTM_DB_RF_GSM_900:
     case FTM_DB_RF_GSM_1800:
     case FTM_DB_RF_GSM_850:
     case FTM_DB_RF_GSM_1900:
        ftm_rf_tech = FTM_RF_TECH_GSM;
        break;

     case FTM_DB_RF_LTE_BC1:
     case FTM_DB_RF_LTE_BC2:
     case FTM_DB_RF_LTE_BC3:
     case FTM_DB_RF_LTE_BC4:
     case FTM_DB_RF_LTE_BC5:
     case FTM_DB_RF_LTE_BC6:
     case FTM_DB_RF_LTE_BC7:
     case FTM_DB_RF_LTE_BC8:
     case FTM_DB_RF_LTE_BC10:
     case FTM_DB_RF_LTE_BC11:
     case FTM_DB_RF_LTE_BC12:
     case FTM_DB_RF_LTE_BC13:
     case FTM_DB_RF_LTE_BC17:
     case FTM_DB_RF_LTE_BC18:
     case FTM_DB_RF_LTE_BC20:
     case FTM_DB_RF_LTE_BC25:
     case FTM_DB_RF_LTE_BC34:
     case FTM_DB_RF_LTE_BC38:
     case FTM_DB_RF_LTE_BC39:
     case FTM_DB_RF_LTE_BC40:
     case FTM_DB_RF_LTE_BC41:
     case FTM_DB_RF_LTE_BC9:
     case FTM_DB_RF_LTE_BC23:
     case FTM_DB_RF_LTE_BC14:
     case FTM_DB_RF_LTE_BC42:
     case FTM_DB_RF_LTE_BC26:
     case FTM_DB_RF_LTE_BC27:
     case FTM_DB_RF_LTE_BC28:
     case FTM_DB_RF_LTE_BC29:
     case FTM_DB_RF_LTE_BC30:
        ftm_rf_tech = FTM_RF_TECH_LTE;
        break;
  
     case FTM_DB_RF_TDSCDMA_B34:
     case FTM_DB_RF_TDSCDMA_B39:
     case FTM_DB_RF_TDSCDMA_B40:
        ftm_rf_tech = FTM_RF_TECH_TDSCDMA;
        break;
  
     default:
        FTM_MSG_ERROR("Unknown RF technology: %d from ftm_current_rf_mode %d",
                      FTM_RF_TECH_UNKNOWN,ftm_current_rf_mode,0);
        ftm_rf_tech = FTM_RF_TECH_UNKNOWN;
        break;
  }
  return ftm_rf_tech;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Activate FTM Protocol Command

  @details
  The function is responsible for getting MB ready to FTM mode.
  
  @param caller_tcb_ptr
  The tcb_ptr from which the function is getting called

  @param task_nv_wait_sig
  Signal on which the caller_tcb waits till deactivation is done

  @param act_reason
  Enumeration indicating the reason for FTM protocol activation

  @param trans_id
  Command sequence number type in MMOCs command to different protocols
*/
void ftm_activate_protocol_cmd( rex_tcb_type *caller_tcb_ptr,
                                rex_sigs_type task_nv_wait_sig,
                                prot_act_e_type act_reason, 
                                prot_trans_type trans_id )
{
  act_reason = act_reason; /* Lint */
  ftm_mode = FTM_MODE;

  #ifdef FEATURE_RF_SUPPORTS_TX_INT_CAL
  /* Request Packet */
  ftm_common_tx_int_dev_cal_rsp_pkt_type ftm_req_data;
  #endif /* FEATURE_RF_SUPPORTS_TX_INT_CAL */

  ftm_common_init();

  #ifdef FEATURE_LTE
  ftm_lte_init();
  #endif /* FEATURE_FTM_HAS_LTE */

  #ifdef FEATURE_CDMA1X
  (void)ftm_cdma_init();
  dog_autokick(FALSE);
  ftm_sig_rex_set( FTM_ENABLE_PROT_SIG ); 
  #endif /* FEATURE_CDMA1X */

  MCPM_Config_Modem ( MCPM_RF_START_REQ, NULL); /* Request RF Start */

  FTM_MSG( FTM_MED, "Requested MCPM_RF_START_REQ");

  FTM_MSG( FTM_HIGH, "Activate FTM Protocol" );

  #ifdef FEATURE_RF_SUPPORTS_TX_INT_CAL
  FTM_MSG( FTM_HIGH, "Doing Tx Internal Cal"  );

  ftm_req_data.cmd = DIAG_SUBSYS_CMD_F;
  ftm_req_data.sub_sys = FTM;
  ftm_req_data.mode = FTM_COMMON_C;
  ftm_req_data.cdma_cmd = FTM_INT_PERFORM_INT_DEV_CAL;
  ftm_req_data.req_len = 0;
  ftm_req_data.rsp_len = 0;
  
  /* Call the Int Cal function to do the get the results */
  ftm_common_send_radio_command(&ftm_req_data, 
                     sizeof(ftm_common_tx_int_dev_cal_rsp_pkt_type));
  #endif /* FEATURE_RF_SUPPORTS_TX_INT_CAL */

  rex_set_sigs(caller_tcb_ptr,task_nv_wait_sig);
  
  FTM_MSG_HIGH("MMOC Act done signal set at FTM", 0, 0, 0);   
} 

/*----------------------------------------------------------------------------*/
/*!
  @brief
  De-activate FTM Protocol Command

  @details
  
  @param caller_tcb_ptr
  The tcb_ptr from which the function is getting called

  @param task_nv_wait_sig
  Signal on which the caller_tcb waits till deactivation is done

  @param rex_sigs_type
  Signal type like wait etc
 
  @param deact_reason
  The reason for deactivation
 
  @param trans_id
  The transaction ID.

*/
void ftm_deactivate_protocol_cmd( rex_tcb_type *caller_tcb_ptr,
                                  rex_sigs_type task_nv_wait_sig,
                                  prot_deact_e_type deact_reason, 
                                  prot_trans_type trans_id )
{
  rfm_device_enum_type device; /* RF Device */
#if defined( FEATURE_WCDMA )
  deact_reason = deact_reason; /* Lint */
#endif

#ifdef FEATURE_LTE
  ftm_lte_deinit();
#endif /* FEATURE_FTM_HAS_LTE */

  /* Reset ftm_current_rf_mode */
  ftm_current_rf_mode = FTM_DB_RF_IDLE;

  MCPM_Config_Modem( MCPM_RF_STOP_REQ, NULL); /* Request RF Stop */
  FTM_MSG( FTM_MED, "Requested MCPM_RF_STOP_REQ");

  FTM_MSG_HIGH("De-activate FTM Protocol:ftm_mode: %d", ftm_mode, 0, 0);   

#ifdef FEATURE_MMGPS
  /* Set test mode in LSM task */
  (void)lsm_cmd_pt_test_mode( FALSE );
#endif /* FEATURE_MMGPS */

  /* Exit RF Mode for all devices */
  for ( device = RFM_DEVICE_0; device < RFM_MAX_DEVICES; device++ )
  {
    ftm_rfmode_exit( device, FTM_STATE_PARK );
  }
  ftm_mode = DMSS_MODE;

  rex_set_sigs( caller_tcb_ptr, task_nv_wait_sig );

  FTM_MSG_HIGH("MMOC Deact done signal set at FTM", 0, 0, 0);  
} 


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Resolve RF Tech for given RF mode 

  @details
  

  DISCLAIMER : This API is used to facilitate the legacy RF Cal API. This API
  should not be called for other purpose. 

  @param rfmode
  
  @return
  
*/
ftm_rf_technology_type ftm_phone_mode_to_rf_tech_type 
( 
  ftm_mode_id_type phone_mode 
)
{
  ftm_rf_technology_type ftm_rf_tech;

  switch( phone_mode )
  {
    case FTM_PHONE_MODE_CDMA:
    case FTM_PHONE_MODE_IMT:
    case FTM_PHONE_MODE_CDMA_800:
    case FTM_PHONE_MODE_CDMA_1900:   
    case FTM_PHONE_MODE_CDMA_1800:
    case FTM_PHONE_MODE_JCDMA:
    case FTM_PHONE_MODE_CDMA_450:
    case FTM_PHONE_MODE_HDR:
    case FTM_PHONE_MODE_HDR_800:
    case FTM_PHONE_MODE_HDR_1900:  
    case FTM_PHONE_MODE_CDMA_BC14:
    case FTM_PHONE_MODE_CDMA_BC11:
    case FTM_PHONE_MODE_CDMA_BC15:
    case FTM_PHONE_MODE_CDMA_BC10:
      ftm_rf_tech = FTM_RF_TECH_CDMA; 
      break; 

    case FTM_PHONE_MODE_GSM_900:
    case FTM_PHONE_MODE_GSM_1800:
    case FTM_PHONE_MODE_GSM_1900:
    case FTM_PHONE_MODE_GSM_850: 
      ftm_rf_tech = FTM_RF_TECH_GSM; 
      break; 

    case FTM_PHONE_MODE_WCDMA_IMT:
    case FTM_PHONE_MODE_WCDMA_1900A:
    case FTM_PHONE_MODE_WCDMA_1900B:
    case FTM_PHONE_MODE_WCDMA_BC3:
    case FTM_PHONE_MODE_WCDMA_800:
    case FTM_PHONE_MODE_WCDMA_BC4:
    case FTM_PHONE_MODE_WCDMA_BC8:
    case FTM_PHONE_MODE_WCDMA_BC11:
    case FTM_PHONE_MODE_WCDMA_BC9:
    case FTM_PHONE_MODE_WCDMA_BC19:
      ftm_rf_tech = FTM_RF_TECH_WCDMA; 
      break; 

    case FTM_PHONE_MODE_LTE_B1:
    case FTM_PHONE_MODE_LTE_B7:
    case FTM_PHONE_MODE_LTE_B13:
    case FTM_PHONE_MODE_LTE_B17:
    case FTM_PHONE_MODE_LTE_B38:
    case FTM_PHONE_MODE_LTE_B40:
    case FTM_PHONE_MODE_LTE_B11:
    case FTM_PHONE_MODE_LTE_B4:
    case FTM_PHONE_MODE_LTE_B2:
    case FTM_PHONE_MODE_LTE_B3:
    case FTM_PHONE_MODE_LTE_B5:
    case FTM_PHONE_MODE_LTE_B6:
    case FTM_PHONE_MODE_LTE_B8:
    case FTM_PHONE_MODE_LTE_B9:
    case FTM_PHONE_MODE_LTE_B10:
    case FTM_PHONE_MODE_LTE_B12:
    case FTM_PHONE_MODE_LTE_B14:
    case FTM_PHONE_MODE_LTE_B15:
    case FTM_PHONE_MODE_LTE_B16:
    case FTM_PHONE_MODE_LTE_B18:
    case FTM_PHONE_MODE_LTE_B19:
    case FTM_PHONE_MODE_LTE_B20:
    case FTM_PHONE_MODE_LTE_B21:
    case FTM_PHONE_MODE_LTE_B22:
    case FTM_PHONE_MODE_LTE_B23:
    case FTM_PHONE_MODE_LTE_B24:
    case FTM_PHONE_MODE_LTE_B25:
    case FTM_PHONE_MODE_LTE_B26:
    case FTM_PHONE_MODE_LTE_B27:
    case FTM_PHONE_MODE_LTE_B28:
    case FTM_PHONE_MODE_LTE_B29:
    case FTM_PHONE_MODE_LTE_B30:
    case FTM_PHONE_MODE_LTE_B31:
    case FTM_PHONE_MODE_LTE_B32:
    case FTM_PHONE_MODE_LTE_B33:
    case FTM_PHONE_MODE_LTE_B34:
    case FTM_PHONE_MODE_LTE_B35:
    case FTM_PHONE_MODE_LTE_B36:
    case FTM_PHONE_MODE_LTE_B37:
    case FTM_PHONE_MODE_LTE_B39:
    case FTM_PHONE_MODE_LTE_B41:
    case FTM_PHONE_MODE_LTE_B42:
      ftm_rf_tech = FTM_RF_TECH_LTE; 
      break; 
    
    case FTM_PHONE_MODE_TDSCDMA_B34:
    case FTM_PHONE_MODE_TDSCDMA_B39:
    case FTM_PHONE_MODE_TDSCDMA_B40:
      ftm_rf_tech = FTM_RF_TECH_TDSCDMA; 
      break; 

    default:  
     FTM_MSG_ERROR("Unknown RF technology %d from phone_mode %d",FTM_RF_TECH_UNKNOWN,phone_mode,0);
     ftm_rf_tech = FTM_RF_TECH_UNKNOWN;
     break;
  }

  return ftm_rf_tech;

}/* ftm_common_rf_mode_to_rf_tech_type */


#endif /* FEATURE_FACTORY_TESTMODE */

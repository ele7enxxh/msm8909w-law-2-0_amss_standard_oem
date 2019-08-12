/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   F T M  T A S K  M O D U L E

GENERAL DESCRIPTION
  This file contains functions specific/internal to the FTM Task.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright(c) 2004 - 2013 by Qualcomm Technologies, Incorporated. All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/src/rf_test_task.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
04/05/16   rs      KW error fix
12/17/15   bir     Fixed KW. 
01/29/15   rmb     Execute WCDMA temp comp every 10 seconds in FTM RF mode. 
07/08/14   pk      Implementation of HDR FTM module
07/08/14   ec      Add RSP definitions for SAWless control messages
06/13/14   kab     Add 3xCA support
03/26/14   sb      Add support for IP2 cal for Dime PM
09/03/13   avss    Removed mis printed Msg added due to FTM Clean up
08/07/13   sml     FTM changes to support GSM RxD 
03/11/13   cj      Register for GSM IRAT
03/11/13   sar     Updated file for APQ, GNSS only target.
02/28/13   kb      Remove redundant tmc_task_start() from ftm_task_init()
02/12/13   pl      Remove redundant RFA_RF messages
02/07/13   spa     Register for HDR FTM IRAT
02/07/13   spa     Register for 1x FTM IRAT
02/15/13   sb      Added FTM hook to override tuner tune code in GSM
01/29/13   ec      Add framework for Non Signalling on multiple RF Devices
01/31/13   ms      Added ftm_tdscdma_meas_register()
01/30/13   ka      Added WCDMA secondary carrier device dispatch
12/04/12   aka     Removing Triton specific featurization for FTM GSM C2 cmd
11/29/12   tws     Add FTM GSM C2 ID.
11/19/12   aki     Cleanup FTM sequencer when RF task exists, tmc.h included 
                   and tmc_task_stop() called only if RCINIT is not defined
10/30/12   spa     Use RFM_DEVICE_3 as diversity device on SV path
10/09/12   pl      FTM IRAT framework revamp
10/05/12   av      Expanded rf task architecture to include more tasks. 
09/28/12   jyu     Enabled FTM support for TDSCDMA diversity
10/09/12   cri     LTE DPD IQ capture MSGR support and bug fixes
08/22/12   pl/sbm  Added support for FTM CA commands
08/10/12   ggs     Ensure that ftm_task_init prototype is declared and 
                   ftm_task_info.is_task_running is set if FEATURE_GSM 
                   is defined 
07/12/12   pl      Moved FTM command processing to FTM task
                   removed obsolete RFA_RF_LTE_FTM messages
07/12/12   pl      Add FTM LTE CA support
07/03/12   aca     Updated rflte_ftm_stop_lte_tx_waveform_transmit_cmd response type
06/25/12   ggs     Call ftm_gsm_msgr_dispatch_response_handler
06/27/12   aca     Moved tmc_task_start to end of ftm_task_init to avoid race condition (see CR 365684)
06/22/12   sty     Temp fix for linker err: 
                   commented out ftm_gsm_msgr_dispatch_response_handler 
06/14/12   gh      Added timer for ftm_wcdma_enter/exit when in ftm_tcb
06/07/12   pv      Reverting the previous changes due to a crash in Online mode for certain PL.
06/07/12   pv      Addition of ftm_lte_init during ftm_task_init.
05/24/12   ec      Added support for Set_band ftm cmd
05/22/12   aca     Added FTM command to extract RX AGC data
05/16/12   kb      Fixing issue due to TMC-deprecation on DIME
05/10/12   ec      Added support for Tune_Tx and Set_Tx_Pow_dBm commands, 
                   Added seperate REX umid list to stop errors due to queue overflow.
05/09/12   aki     Mainlined ftm_kick_watchdog function 
05/08/12   ars     Fix to gracefully exit FTM Sequencer when dealing with task based technologies (LTE)
04/19/12   ggs     Added support for FTM GSM MSGR commands. 
                   Initial support added to handle response messages from RF task  
04/24/12   jfc     Use TASK_HANDLE instead of rex_self() 
03/26/12   jfc     Change FEATURE_PMIC to FEATURE_PMIC_FTM
03/26/12   jfc     Fix the tcb/task compilation errors
03/24/12   yzw     Added FTM command FTM_LTE_SET_FREQ_ADJUST, FTM_LTE_ENABLE_FREQ_CORRECTION and FTM_LTE_DISABLE_FREQ_CORRECTION
03/19/12   jfc     Removed okts sleep code. Has been deprecated since 
03/18/12   aro     Created main common function to dispatch radio commands
                   to specific techs
03/02/12   sar     Added support for RCINIT.
02/29/12   yzw     Added LTE FTM command FTM_LTE_SET_LNA_STATE_DBM
02/15/12   sar     Replaced FEATURE_FTM_MC with FEATURE_CDMA1X.
02/15/11   aki     Memory handling improvements 
02/14/12   sb      Added support for FTM logging for GSM
02/14/12   sar     Reinstating TDSCDMA features.
01/28/12   sar     Feature reduction changes.
01/04/12   ems     Added support for LTE Cal V3 mode switching
01/04/12   hs      Added support for PMIC.
12/28/11   ars     Added ftm_sig_clk_rex_wait_no_clear()
12/07/11   sb      Support for GSM IQ capture in Online and FTM mode 
12/08/11   aki     Changed FTM_KICK_WATCHDOG macro to a function 
12/08/11   aki     Added handling FTM requests from RFCA task
11/14/11   aro     Mainlined the debug flags
11/10/11   sr      replaced the direct REX TCB structure access with rex api.
11/02/11   lp      Fixed case sensitive compilation error header file issue for Linux
10/27/11   gh      Add notification to rf_task when FTM_WCDMA_BER_CMD_Q_SIG received
10/27/11   ars     Adding a check to stop sequencer command execution if an FTM               
                   command is currently in progress. 
10/24/11   adk     Fixed KW error.
10/20/11   adk     Added support for SV diversity.
10/17/11   ars     Added FTM_RF_GET_RX_LEVEL_DBM command
10/17/11   ars     Added FTM_SET_PA_STATE and FTM_SET_TX_POWER_DBM FTM Layer
09/16/11   sar     Removed compiler warnings.
09/14/11   can     Merge MDM9K tips to NikeL.
09/07/11   sar     Clockworks error fix for (System: 7890).
07/29/11   hy      add ftm_tdscdma_ber_init() in ftm_task_init()
07/22/11   dw      deprecate FTM heap and use modem heap for malloc
07/11/11   hy      Including TD-SCDMA support.
06/23/11   pl      Remove obsolete interface : set_rx_on
06/07/11   aro     Mainlined FEATURE_RF_SVDO_API
06/01/11   aro     Added debug flag to use QRCT GUI for SV Chain
05/31/11   aro     Removed debug flag used for SV Chain Calibration
05/27/11   aro     Moved SV debug flag to main dispatch
05/24/11   bmg     CDMA 1x SV chain updates
05/09/11   sar     Added ftm_rf_cmd.h for rfa/api cleanup
04/29/11    av     Change in ftm_gsm_rx_disable(). The wait is now on the task that 
                    is running: FTM or RF task. Takes care of crash between G to L FTM
                    transition
04/28/11   sar     CMI-4 Cleanup.
04/20/11   pl      remove Obsolete FTM commands
04/12/11   sty     Replaced ERR() with FTM_MSG_ERROR()
03/28/11   bmg     Cancel watchdog report timer and exit the FTM task
                   function when receiving the TASK_STOP_SIG
03/14/11   pl      Adding Sleep and Wakeup Command for LTE FTM
02/18/11   aro     Added FTM_RFM_C SubSystem
02/09/11   aca     FTM dac setpoint value override support
02/08/11   aca     FTM modulation change support
12/21/10   whc     FTM Command for SMPS Override
12/09/10   aro     Fixes for Zero-Compile Error SVDO Enabled Build
12/07/10   aro     Added Initial SVDO Support
11/23/10   mkv     Fixing LTE NS response packet issues. 
10/05/10   ems     Added Support For FTM Sequencer
09/27/10   whc     Added txagc oride msgr rsp IDs
07/20/10   av      Fix compiler warnings
07/13/10   lcl     Added vote to prevent TCXO shutdown in FTM mode.
07/07/10   aki     Merging RFNV FTM changes from dev/rfnv_dev
07/02/10    dw     Bypass ftm_mode_check for FTM_GET_AGC_RESOLUTION_RX/TX in online mode
06/28/10   kma     Added SV FTM module
06/09/10   jyu     Fix warning 
04/01/10   adk     Merged branch scmm_mdm9k_merge_final
03/25/10   pl      Add LTE_CPHY_TEST_L1_MODE_CHANGE_CNF to RF_LTE umid list
03/16/10   pl      Remove Bringup Hack
03/16/10   pl      Improve LTE F3 Message information
03/01/10   lp      Removed FTM DSPDIAG support
03/01/10   pl      Fix merge error
02/20/10   mkv     Adding support for LTE Diversity commands.
01/29/10   pl      Fix address alignment issues in ftm_rex_msgr_msg_type
01/26/10   sar     Lint fixes.
01/26/10   sar     Fix compilation errors for stripped build.
01/26/10   sar     Lint fixes.
01/22/10   pl      Restore ftm_task under FEATURE_RFA_CMI_API
01/19/10   pl      Remove FTM LTE NS message registration from task init
01/04/10   hp      Enable bypass_ftm_mode_check flag for FTM_GNSS_C
12/09/09   pl      Initialize FTM LTE non-signaling processor in ftm_task_init  
11/22/09   mkv     Added MSGR registration for LTE Calibration response messages. 
11/16/09   ka      Fix compilation errors for WPLT.
10/16/09   zg      More SVDO fixes
10/15/09   hp      Added case FTM_GNSS_C in ftm_req() 
10/06/09   mkv/pl  Modified FTM Task init sequence.
10/01/09   pl      Adding support for diag response packet in LTE 
                   Non-signaling commands processing
10/06/09   jps     Added FTM IRAT dispatch
10/02/09   zg      Backed out FTM SVDO changes.
09/25/09   aro     Fixed "PACKED" usage to be compatible with QDSP6
09/13/09   pl      Enclosed ftm_get_task_priority() and ftm_set_task_priority() with FEATURE_L4
09/10/09   pl      move FTM-LTE specific inclusions to after FEATURE_FACTORY_TESTMODE
09/09/09   aro     Added a check to see if priority val is valid
09/09/09   aro     Featurized Code with FEATURE_RF_SVDO_API
                   + Removed Debug Variable
09/02/09   pl      Move ftm_tcb and ftm_stack definition to task.c
09/01/09   tnt     Adding code for common ino dispatch   
08/11/09   mkv     Merge MODEM_LTE_DEV into rfa2
07/22/09   aro     Added Feature to manage changing of FTM Task Priority
07/12/09   aro     Moved new Cal Dispatch to ftm_req()
07/10/09   aro     Re-organized 1xHDR FTM Files
07/07/09   dyc     FTM_HAS_FM support.
06/22/09   pl      Fix Lint warning in MDM9K
06/15/09   pl      Modification to loop through LTE command Q
06/15/09   pl      Merge with SCMM 
05/14/09   mkv     Adding support for LTE FTM calibration REX signals from diag 
                   and RF LTE FTM MSGR responses.Comments cleanup.
04/27/09   pl      Moving FTM task to REX
04/13/09   sr      commented out outpw(LEDS, 0x0000) to fix compile errs
03/30/09   aro     Added FTM SVDO Dispatch
06/18/08   adk     Include ftm_cmd.h and ftm_mc_dispatch.h in 1x mode only.
05/21/08   adm     Changes to fix compiler errors in MFLO build
05/20/08   adk     Added, rearranged include files to compile 7625.
12/18/07   ad      Include ftm_mbp.h only when FEATURE_DTV_DVBH is defined
12/18/07   ad      Use ftm_mflo_pkt_type when FTM_HAS_MBP is defined
12/17/07   ad      Reuseable events have conditional cleanup
12/17/07   ad      Added FTM_MF_NS_C for MFLO Non-Signaling support
12/17/07   ad      Support for MBP 
12/17/07   ad      Support to blow fuses via FTM commands
08/28/07   rk      Added call to ftm_bt_init() from ftm_task_init()
08/06/07   jfc     Featurized code for EDGE-only builds
07/25/07   ycl     Fix Lint Warning.
06/25/07   ad      Added support for single ended BER
05/10/07   jfc/tbh Call ftm_wlan_adp_init() from ftm_task_init()
02/28/07   ycl     Fix compiler warnings.
10/18/06   ycl     Fixed FTM_HAS_1X in ftm_req.
10/17/06   ycl     Fixed the FTM_LED type.
10/11/06   jf      Added FEATURE_FTM_PMIC_BYPASS
06/13/06   jac     Fixed compile errors for UMTS targets. Also fixed LINT errors.
06/08/06   hkk     Fixed compiler error/warning 
05/18/06   tm      Initial release for dsp diagnostics driver - new FTM subsystem.
                   Event mechanism to conserve signal defs, and support for delayed
                   response.
05/16/06   ra      Add support for vote to sleep
01/18/06   dp      Update to use WCDMA diversity commands.
09/12/05   hkk     Changes for TX/RX sweep cal - added case FTM_COMMON_C
06/16/05   dp      Add support for FTM EGPRS BER feature.
05/02/05   dbc     Added support for MFLO.
04/14/05   xw      Corrected ftm_tcb to tmc_tcb when setting TMC_ACK_SIG.
03/20/04   bmg     Added 1x calibration v2 API
03/11/05   ka      Added dispatch for gsm extended commands.
01/28/05   jfc     Added no_clear versions to ftm_sig_rex_wait(_cb)
01/25/05   ra      removed 0xFFCFFFFF mask in ftm_sign_rex_wait()
                   because this breaks 1x
12/07/04   jfc     Changed ftm_sign_rex_wait() to and mask with 0xFFCFFFFF
12/06/04   jfc     Changed ftm_sig_rex_wait() to NOT clear upper 3 bits of sigs when done
11/23/04   ra      removed obsolete header files
10/19/04   dp      Added dispatcher and cmd q handler for FTM WCDMA BER feature
10/15/04   ka      Removed unused code.
10/12/04   jac     Mainlined ASYNC packet code.
10/07/04   wd      Removed redundant functions ftm_gsm_wait and ftm_wait.
                   Added ftm_sig_rex_wait_cb function which can be passed
                   to drivers that will need to wait while executing on ftm
                   task context.
10/07/04   wd      Added missing break for log_dispatch.
10/06/04   lp      Added ftm_camera feature
10/05/04   ka      Added code to dispatch customer extensions.
10/01/04   wd      Added code to support 16 bit subsys_cmd_code and also
                   rsp_pkt_size in the ftm header.
10/01/04   hk      Added dispatch for Audio.
09/14/04   ra      reinserted 1x ftm_pmic support
09/13/04   ra      removed 1x ftm_pmic support for now
09/09/04   lcl     Bracked ftm_hwtc_bt_init_q with FEATURE_FTM_BT.
09/03/04   xw      Code clean-up.
                   Added ftm_wait() and fixed ftm_clk_rex_wait() bug.
                   Added ftm_gsm_ber_init() in ftm_task_init().
07/29/04   ra      Initial revision
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                           INCLUDE FILES FOR MODULE

===========================================================================*/
#include "rfa_variation.h"
#include "target.h"    /* Target specific definitions            */
#include "comdef.h"    /* Common stuff                           */
#include "rex.h"
#include "msgr_rex.h"
#include "rfcommon_msg.h"

#ifdef FEATURE_MODEM_RCINIT
#include "rcinit.h"
#endif /* FEATURE_MODEM_SUPPORTS_RCINIT */

#ifdef FEATURE_FACTORY_TESTMODE
#include "ftm.h"
#include "ftm_v.h"
#include "ftm_task.h"
#include "rf_test_task_v.h"
#include "ftmicap.h"
#include "ftmdiag.h"
#include "ftm_log.h"
#include "modem_mem.h"
#include "ftm_common_locks.h"
#include "ftm_sequencer_adaptation.h"
#ifdef FEATURE_CGPS
#include "ftm_gnss_dispatch.h"
#endif

#ifdef FEATURE_LTE
#include "dog.h"     /* For DOG_FTM_RPT (need FEATURE_FTM_TASK in rf_cap.h), DOG_FTM_RPT_TIME and dog_report()   */
#ifndef FEATURE_MODEM_RCINIT
  #include "tmc.h"     /* FOR TASK_START_SIG, TASK_STOP_SIG, tmc_task_start, tmc_task_stop */
#endif
#include "modem_mem.h" 
#include "diagpkt.h" /* for diagpkt*() Seems to be target dependent */
#endif

#include "ftm_rf_cmd.h"

#ifdef FEATURE_CDMA1X
#include "ftm_cmd.h"
#include "ftm_cdma_dispatch.h"
#include "ftm_1x_meas.h"
#include "ftm_hdr_meas.h"
#endif

#ifdef FEATURE_GSM
#include "ftm_gsm_dispatch.h"
#include "gl1_hw.h"
#include "memheap.h"
#include "assert.h"
#include "dog.h"
#include "ftm_gsm_meas.h"
#ifndef FEATURE_MODEM_RCINIT
  #include "tmc.h"
#endif
#include "rfgsm_core_log.h"

  #if defined(FEATURE_GSM_EGPRS) && defined(FEATURE_FTM_EGPRS_BER)
  #include "ftm_egprs_ber.h"
  #endif

  #ifdef FEATURE_FTM_GSM_BER
  #include "gs.h"
  #endif
#endif

#ifdef FEATURE_WCDMA
  #include "l1task.h"
  #include "mmoc.h"
  #include "ftm_wcdma_dispatch.h"
  #include "dog.h"
  #include "ftm_wcdma_ctl.h"
  #include "ftm_wcdma_ber.h"
  #include "ftm_wcdma_meas.h"
#ifdef FEATURE_MMGPS
  #include "ftm_agps_dispatch.h"
#endif
#endif /* FTM_HAS_WCDMA */

#ifdef FTM_HAS_TDSCDMA
  #include "ftm_tdscdma_dispatch.h"
  #ifdef FTM_HAS_TDSCDMA_BER  
  #include "ftm_tdscdma_ber.h"
  #endif /*FTM_HAS_TDSCDMA_BER*/
  #include "ftm_tdscdma_meas.h"
#endif /* FTM_HAS_TDSCDMA */

#ifdef FEATURE_FTM_HWTC
#include "ftm_hwtc_dispatch.h"
#include "ftm_hwtc_dvt.h"
#include "ftm_hwtc_dc.h"
#endif /* FEATURE_FTM_HWTC */

#ifdef FEATURE_FTM_CUSTOMER_EXTENSIONS
#include "ftm_customer_extensions.h"
#endif

#ifdef FTM_HAS_SE_BER
#include "ftm_se_ber.h"
#endif

#include "diagcmd.h"

#include "string.h"
#include "stdarg.h"
#include "stdio.h"
#include "ctype.h"

#ifdef FTM_HAS_IRAT
#include "ftm_meas_dispatch.h"
#include "ftm_meas.h"
#endif

#include "ftm_common_dispatch.h"

#ifdef FEATURE_LTE
#include "ftm_mgr_msg.h"
#include <ftm_lte_common_dispatch.h>
#include <ftm_lte_common_msgr_helper_functions.h>
#include <ftm_lte_ns_msg.h>
#include <ftm_lte_ns_proc.h>
#include "ftm_common_ino.h"
#include "ftm_lte_meas_mc.h"
#include "intf_ul_msg.h"
#endif

#ifdef FEATURE_GSM
#include "rfgsm_ftm_msg.h"
#endif /* FEATURE_GSM */
#ifdef FTM_HAS_SV_API
#include "ftm_sv_dispatch.h"
#endif /* FTM_HAS_SV_API */

#include "ftm_sequencer.h"
#include "ftm_rfnv_dispatch.h"
#include "rfm_internal.h"
#include "rf_task_common_services.h"

#ifdef FEATURE_RF_RFCA
  #include "rfca.h"
#endif

#ifdef FEATURE_PMIC_FTM
    #include "ftm_pmic.h"
#endif
/*===========================================================================

                         EXTERNAL GLOBAL VARIABLES

===========================================================================*/

extern ftm_pkt_data_type ftm_pkt_data; /* holds information for current ftm cmd */

rfm_device_enum_type ftm_gsm_ns_active_device = RFM_INVALID_DEVICE;


/*===========================================================================

                   LOCAL GLOBAL VARIABLES and CONSTANTS

===========================================================================*/
/* Timers used for watchdog reporting and generic purposes. */
LOCAL rex_timer_type ftm_rpt_timer;      /* Timer for the test task       */

/* timer used for ftm_rex_wait */
LOCAL rex_timer_type ftm_wait_timer;

/* Buffers which can be used to Send commands to this task. */
LOCAL ftm_msg_type ftm_task_msg_bufs[ FTM_NUM_CMDS ];

/* Command queue for the test task.  All command to this task will be
 placed on this queue to await processing. */
extern q_type ftm_task_cmd_q;
extern q_type ftm_task_event_q;

#ifdef FEATURE_LTE


/*! @brief This following structure is used  
 *         deliver a LTE message router message to
 *         FTM REX task. This structure is created
 *         for REX-MSGR interwork
 *         Diag.
 *
 *  @param link, The pointer to the REX queue the message is link to
 *  @param ftm_msgr_msg, The message router message including the header
 */
typedef struct ALIGN(8)
{
  q_link_type                 link;
  ftm_msgr_u                  ftm_msgr_msg;
} ftm_rex_msgr_msg_type;


static msgr_umid_type rf_lte_ftm_umid_list[] = 
{

    RFA_RF_LTE_FTM_CONFIGURE_TX_SWEEP_RSP,
    RFA_RF_LTE_FTM_SET_RX_IQ_MISMATCH_COMP_COEFFTS_RSP,
    RFA_RF_LTE_FTM_GENERIC_MESSAGE_RSP,
    LTE_CPHY_TEST_L1_MODE_CHANGE_CNF,
    RFA_RF_LTE_FTM_OVERRIDE_TXAGC_OUTPUT_POWER_RSP,
    RFA_RF_LTE_FTM_OVERRIDE_TXAGC_BACKOFF_RSP,
    LTE_LL1_UL_IQ_SAMP_CAPTURE_CNF,
};
#endif/* FEATURE_LTE */

#if defined (FEATURE_LTE) || defined (FEATURE_GSM)
/*! @brief Holds FTM task and related information. 
*/
typedef struct
{
  boolean        is_task_running; /*!< Flag set when the task gets initialized */
  msgr_client_t  ftm_msgr_client; /*!< All FTM interaction with the message router 
                                       includes this client as a parameter. */
  msgr_id_t      lte_mb;         /*!< Mailbox: LTE RF/L1/L2 messages. This mail box is required 
                                      irrespective of whether the FTM task is REX or POSIX. This
                                      mailbox allows FTM task to receive messages from a message 
                                      router based task.*/
}ftm_task_s;

static ftm_task_s ftm_task_info;
#endif /* ( FEATURE_LTE || FEATURE_GSM ) */

#ifdef FEATURE_GSM

/*! @brief This following structure is used  
 *         deliver a  message router message to
 *         FTM REX task. This structure is created
 *         for REX-MSGR interwork
 *         Diag.
 *
 *  @param link, The pointer to the REX queue the message is link to
 *  @param rfgsm_ftm_msgr_msg, The message router message including the header
 */
typedef struct ALIGN(8)
{
  q_link_type                 link;
/*! @todo Define a new union if FTM task needs to receive commands from SW entities other than RFGSM */
  rfgsm_ftm_msg_u            rfgsm_ftm_msgr_msg; 
} ftm_rfgsm_rex_msgr_msg_type;

/* The following UMID list contains all of the messages that MSGR_FTM_GSM clients 
 * are registered to receive.
 * Currently, two MSGR_FTM_GSM clients are registered
 * with MSGR.
 * One MSGR_FTM_GSM client is used to register a rex queue with MSGR to allow other
 * MSGR clients/tasks to send messages to FTM task.
 * The second MSGR_FTM_GSM MSGR client is used to register a mailbox for receiving
 * blocking RFA Task RSP messages. The mailbox will be used for receiving any RSP
 * messages from msgr_send() commands
 * which require FTM task to block until the command has been completed.
 */

 /*! @todo As a run-time optimisation, consider creating two separate UMID lists for each
  * MSGR_FTM_GSM clients, since
  * not all CMDs sent to RF task need to be blocking or require further handling from
  * from FTM task.
  */

static msgr_umid_type rfgsm_ftm_umid_cmd_list[] = 
{
    RFA_RF_GSM_FTM_SET_LINEAR_PA_RANGE_RSP,

/*! @todo The remainder of these commands are placeholders for possible future use */
    RFA_RF_GSM_FTM_SET_MODE_RSP,
    RFA_RF_GSM_FTM_SET_TX_ON_RSP,
    RFA_RF_GSM_FTM_SET_TX_OFF_RSP,
    RFA_RF_GSM_FTM_SET_LNA_RANGE_RSP,
    RFA_RF_GSM_FTM_SET_PDM_RSP,
    RFA_RF_GSM_FTM_SET_BAND_RSP,
    RFA_RF_GSM_FTM_SET_TRANSMIT_CONT_RSP,
    RFA_RF_GSM_FTM_SET_TRANSMIT_BURST_RSP,
    RFA_RF_GSM_FTM_SET_RX_BURST_RSP,
    RFA_RF_GSM_FTM_GET_RSSI_RSP,
    RFA_RF_GSM_FTM_SET_PA_START_DELTA_RSP,
    RFA_RF_GSM_FTM_SET_PA_STOP_DELTA_RSP,
    RFA_RF_GSM_FTM_SET_PA_DAC_INPUT_RSP,
    RFA_RF_GSM_FTM_SET_RX_CONTINUOUS_RSP,
    RFA_RF_GSM_FTM_SET_PATH_DELAY_RSP,
    RFA_RF_GSM_FTM_SET_SLOT_OVERRIDE_FLAG_RSP,
    RFA_RF_GSM_FTM_RX_GAIN_RANGE_CAL_RSP,
    RFA_RF_GSM_FTM_TX_KV_CAL_RSP,
    RFA_RF_GSM_FTM_TX_KV_CAL_V2_RSP,
    RFA_RF_GSM_FTM_SET_LINEAR_RGI_RSP,
    RFA_RF_GSM_FTM_CAPTURE_IQ_RSP,
    RFA_RF_GSM_FTM_SET_RX_TIMING_RSP, /* Legacy ftm_gsm_extended dispatch commands */
    RFA_RF_GSM_FTM_SET_TX_TIMING_RSP,
    RFA_RF_GSM_FTM_SET_TX_GAIN_SWEEP_RSP,
    RFA_RF_GSM_FTM_TX_ENVDC_CS_SWEEP_RSP,
    RFA_RF_GSM_FTM_CFG2_AMAM_SWEEP_V2_RSP,
    RFA_RF_GSM_FTM_SET_RX_MULTISLOT_RSP,
    RFA_RF_GSM_FTM_SET_RX_BURST_FOR_EXPECTED_PWR_RSP,
    RFA_RF_GSM_FTM_GET_MEAS_PWR_RSP,
    RFA_RF_GSM_FTM_SET_TX_POW_DBM_RSP,
    RFA_RF_GSM_FTM_TUNE_TX_RSP,
    RFA_RF_GSM_FTM_TUNER_TUNE_CODE_OVERRIDE_RSP,
    RFA_RF_GSM_FTM_SET_IP2_CAL_PARAMS_RSP,
    RFA_RF_GSM_FTM_GET_IP2_CAL_PARAMS_RSP,
    RFA_RF_GSM_FTM_GET_IP2_CAL_RESULTS_RSP,
    RFA_RF_GSM_FTM_SET_SAWLESS_LIN_MODE_RSP,
    RFA_RF_GSM_FTM_GET_SAWLESS_LIN_MODE_RSP,

};
static msgr_umid_type rfgsm_rex_ftm_umid_cmd_list[] = 
{ /*! @todo add additional commands to register with rex queue.
      Currently only RSP messages which we need to process asynchronously
      need to go here, set pa range is a placeholder. Blocking receive is being
      used for RSP at present.*/
    RFA_RF_GSM_FTM_SET_LINEAR_PA_RANGE_RSP,
};
#define FTM_GSM_MAX_FREE_QUEUE_ITEM   10
/*! @brief CMD queue for the FTM task to receive GSM SW commands from entities such as RFA,L1,L2 etc. Currently only used by RFA */
q_type ftm_task_rfgsm_q;
/*! @brief Free queue for the FTM task. When a POSIX task sends message router commands to a REX task,
 * a free queue has to be provided by the REX task to messages router. MSG router uses the free queue as a 
 * temporary buffer to put messages into the REX queue. 
 */ 
q_type ftm_task_rfgsm_free_q;
ftm_rfgsm_rex_msgr_msg_type rex_rfgsm_msg[FTM_GSM_MAX_FREE_QUEUE_ITEM];
#endif /* FEATURE_GSM */

#ifdef FEATURE_LTE
static ftm_msgr_u msgr_msg;

/*! @brief CMD queue for the FTM task to receive LTE SW commands from entities such as RFA,L1,L2 etc.  */
q_type ftm_task_lte_q;
/*! @brief Free queue for the FTM task. When a POSIX task sends message router commands to a REX task,
 * a free queue has to be provided by the REX task to messages router. MSG router uses the free queue as a 
 * temporary buffer to put messages into the REX queue. 
 */ 
#define FTM_MAX_FREE_QUEUE_ITEM   10
q_type                ftm_task_free_q;
ftm_rex_msgr_msg_type rex_lte_ns_msg[FTM_MAX_FREE_QUEUE_ITEM];
#endif/* FEATURE_LTE */

#if defined(FTM_HAS_UMTS) && defined(FEATURE_FTM_WCDMA_BER)
rex_timer_type ftm_wcdma_enter_exit_wait_timer_ftm_tcb;
#endif

#if defined(FTM_HAS_UMTS) && defined(FEATURE_WCDMA)
rex_timer_type ftm_wcdma_execute_temp_comp_timer;
#define FTM_WCDMA_TEMP_COMP_FREQUENCY_MS 10000 // Temp comp is exeucted every 10 sec= 10000 milli sec
#endif

/*!
Generic Flag to block to FTM Sequencer Process Loop Iteration for commands that require inter-task communication               
and return back to the ftm_task loop without completing the desired functionality
*/
boolean CmdInProgressBlockSequencer;


#ifdef FEATURE_MODEM_RCINIT
/*---------------------------------------------------------------------------
                  Variables pertaining to RCINIT
---------------------------------------------------------------------------*/

#define RF_TEST_RCEVT_PREFIX "RF_TEST:"
#define RF_TEST_RCEVT_READY RF_TEST_RCEVT_PREFIX "ready"

#endif /* FEATURE_MODEM_SUPPORTS_RCINIT */
/*===========================================================================

                               Macros

===========================================================================*/

#ifdef FEATURE_LTE
/* ! @brief Define the rf lte ftm message count for the mailboxes.
*/
#define RF_LTE_FTM_MSG_CNT \
    (sizeof(rf_lte_ftm_umid_list)/sizeof(msgr_umid_type))
#endif /* FEATURE_RF_HAS_LTE */
#ifdef FEATURE_GSM

#define RF_GSM_FTM_MSG_CNT \
    (sizeof(rfgsm_ftm_umid_cmd_list)/sizeof(msgr_umid_type))
#define RF_GSM_REX_FTM_MSG_CNT \
    (sizeof(rfgsm_rex_ftm_umid_cmd_list)/sizeof(msgr_umid_type))
#endif /* FEATURE_GSM */
/* Timers used for watchdog reporting and generic purposes. */
extern rex_timer_type rf_rpt_timer;      /* Timer for the RF watchdog report */

extern rex_timer_type rf_wait_timer;
/*===========================================================================

                     FUNCTION PROTOTYPES & DEFINITIONS

===========================================================================*/

#ifdef FEATURE_GSM
#ifdef FEATURE_FTM_GSM_BER
  extern ftm_rsp_pkt_type ftm_gsm_ber_dispatch( rfm_device_enum_type rfm_dev,
                                                ftm_gsm_ber_pkt_type *cmd_ptr);
  extern void ftm_gsm_ber_handle_rx_data_indications( void);
#endif
#endif

#ifdef FTM_HAS_UMTS
#ifdef FEATURE_FTM_WCDMA_BER
  extern ftm_rsp_pkt_type ftm_wcdma_ber_dispatch( ftm_wcdma_ber_pkt_type *cmd_ptr);
#endif
#endif

#ifdef FTM_HAS_TDSCDMA
#ifdef FTM_HAS_TDSCDMA_BER
  extern ftm_rsp_pkt_type ftm_tdscdma_ber_dispatch( ftm_tdscdma_ber_pkt_type *cmd_ptr);
#endif /*FTM_HAS_TDSCDMA_BER*/
#endif /*FTM_HAS_TDSCDMA*/

#if defined (FEATURE_LTE) || defined (FEATURE_GSM)
void  ftm_task_init( void );
#endif /* ( FEATURE_LTE || FEATURE_GSM ) */

#ifdef FEATURE_LTE
static void ftm_deinit(void);
boolean rf_lte_ftm_msgr_register( msgr_client_t *client_id, msgr_id_t id );
boolean rf_lte_ftm_msgr_deregister( msgr_client_t *client_id );
#endif /* FEATURE_RF_HAS_LTE */

#ifdef FEATURE_GSM
static void ftm_gsm_deinit_rex_msgr_client( void );
static void ftm_gsm_deinit_msgr_client( void );
#endif /* FEATURE_GSM */

/* ==================================================================
FUNCTION FTM_MALLOC
DESCRIPTION
   Dynamically allocates a specified number of bytes and returns a
   pointer to this chunk of memory.
===================================================================== */
void *ftm_malloc
(
  unsigned long num_bytes
)
{
  return modem_mem_alloc( num_bytes, MODEM_MEM_CLIENT_RFA );
} /* END ftm_malloc */

/* ==================================================================
FUNCTION FTM_FREE
DESCRIPTION
   Frees a chunk of memory allocated using ftm_malloc.
===================================================================== */
void ftm_free
(
  void *mem_ptr
)
{
  modem_mem_free(mem_ptr,MODEM_MEM_CLIENT_RFA);
  mem_ptr = NULL; //set freed memory pointer to NULL
  return;
} /* END ftm_free */

/* ==================================================================
FUNCTION FTM_KICK_WATCHDOG

DESCRIPTION
   Kicks the watchdog timer and sets a timer to wake it again.
===================================================================== */
void ftm_kick_watchdog(void)
{
   dog_report(DOG_FTM_RPT);
  (void) rex_set_timer(&ftm_rpt_timer, DOG_FTM_RPT_TIME);
} /* END ftm_kick_watchdog */

/*===========================================================================

FUNCTION FTM_SIG_REX_WAIT

DESCRIPTION
  This procedure waits for specified signals to be set.  While it waits,
  this procedure continues to set the Watchdog report signal and reset
  the mc_rpt_timer.

DEPENDENCIES
  None.

RETURN VALUE
  Signal mask from signals that were set after rex_wait call.

SIDE EFFECTS
  None.

===========================================================================*/
rex_sigs_type ftm_sig_rex_wait
(
  rex_sigs_type   mask        /* Mask of signals to wait for */
)
{
  rex_sigs_type   sigs = 0;   /* Signals returned by rex_get_sigs call */

  /* if mask is 0, then there is nothing to wait on */
  if (mask == 0)
  {
    return mask;
  }

  #ifdef FEATURE_FACTORY_TESTMODE
  sigs = ftm_sig_rex_wait_no_clear(mask);
  #endif

  (void) rex_clr_sigs(TASK_HANDLE(ftm), mask);

  return(sigs);
} /* End ftm_sig_rex_wait */

#ifdef FEATURE_FACTORY_TESTMODE
/*===========================================================================

FUNCTION FTM_SIG_REX_WAIT_NO_CLEAR

DESCRIPTION
  This procedure waits for specified signals to be set.  While it waits,
  this procedure continues to set the Watchdog report signal and reset
  the mc_rpt_timer. The signals this function waits for are not cleared
  at the end.

DEPENDENCIES
  None.

RETURN VALUE
  Signal mask from signals that were set after rex_wait call.

SIDE EFFECTS
  None.

===========================================================================*/
rex_sigs_type ftm_sig_rex_wait_no_clear
(
  rex_sigs_type   mask        /* Mask of signals to wait for */
)
{
  rex_sigs_type   sigs = 0;   /* Signals returned by rex_get_sigs call */

  /* if mask is 0, then there is nothing to wait on */
  if (mask == 0)
  {
    return mask;
  }

  ASSERT (rex_self () == TASK_HANDLE(ftm));

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Loop while waiting for signals specified in mask and   */
  /* for watchdog signal.  If watchdog signal then kick it  */
  /* and loop again until some other signal in mask is set. */

  while ((sigs & mask) == 0)
  {
    sigs = rex_wait( mask              |     /* Custom SIG            */
                     FTM_RPT_TIMER_SIG       /* Watchdog report timer */
                     );

    if ( sigs & FTM_RPT_TIMER_SIG )
    {
      ftm_kick_watchdog();
    }

  }

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  (void) rex_clr_sigs(TASK_HANDLE(ftm), FTM_RPT_TIMER_SIG );      /* Watchdog report timer */

  /* kick watchdog one last time and return the signals. */
  ftm_kick_watchdog();
  return(sigs);
} /* End ftm_sig_rex_wait_no_clear */
#endif /* FEATURE_FACTORY_TESTMODE */
/*===========================================================================

FUNCTION rf_ftm_sig_rex_wait

DESCRIPTION
  This procedure waits for specified signals to be set.  While it waits,
  this procedure continues to set the Watchdog report signal and reset
  the mc_rpt_timer. The signals this function waits for are not cleared
  at the end. This function waits on the present TASK_HANDLE(ftm) signals.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void rf_ftm_sig_rex_wait(uint32 msec )
{
  rex_sigs_type   sigs = 0;   /* Signals returned by rex_get_sigs call */
  rex_tcb_type*    tcb = TASK_HANDLE(ftm);//wait_timer->tcb_ptr; 
  dog_report_type   task_dog_rpt_var = 0; // = tcb->dog_report_val;
  rex_timer_type* watchdog_rpt_timer = NULL;
  rex_timer_type* wait_timer = NULL;
  uint32 task_dog_rpt_time_var = 0;
  timer_attrib_struct_type timer_params_struct;
  rex_sigs_type wait_timer_sig;
  rex_sigs_type watchdog_rpt_timer_sig;

  if(tcb == TASK_HANDLE(ftm))
  {
      watchdog_rpt_timer = &ftm_rpt_timer;
      wait_timer = &ftm_wait_timer;
      task_dog_rpt_var = DOG_FTM_RPT;
      task_dog_rpt_time_var = DOG_FTM_RPT_TIME; //dog_get_report_period doesn't work for FTM task!
  }
  else {
    if(tcb == TASK_HANDLE(rf))
    {
      watchdog_rpt_timer = rfa_get_watchdog_timer(RF_TASK_MAIN);
      wait_timer = rfa_get_wait_timer(RF_TASK_MAIN);
      task_dog_rpt_var = rex_get_dog_report_val_self();
      task_dog_rpt_time_var = dog_get_report_period( task_dog_rpt_var );
    }
    else
    {
      FTM_MSG_FATAL("Invalid tcb_ptr",0,0,0);
    }

  }
  
  if(wait_timer == NULL || watchdog_rpt_timer == NULL)
  {
    FTM_MSG_FATAL("wait or watchdog timer is NULL",0,0,0);
    return;
  }

  (void) rex_set_timer(wait_timer, msec);
  timer_get_params(wait_timer,&timer_params_struct);
  wait_timer_sig = (rex_sigs_type)timer_params_struct.sigs;

  timer_get_params(watchdog_rpt_timer,&timer_params_struct);
  watchdog_rpt_timer_sig = (rex_sigs_type)timer_params_struct.sigs;

  while ((sigs & (wait_timer_sig)) == 0)
  {
    sigs = rex_wait( (wait_timer_sig)              |     /* Custom SIG            */
                     (watchdog_rpt_timer_sig)       /* Watchdog report timer */
                     );

    if ( sigs & watchdog_rpt_timer_sig  )
    {
        if(task_dog_rpt_time_var > 0)    
        {
          /* Report DOG */
          dog_report(task_dog_rpt_var);
  
          /* Set timer for next report */
          (void) rex_set_timer(watchdog_rpt_timer, task_dog_rpt_time_var);
        }
    }

  }

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  (void) rex_clr_sigs(tcb, watchdog_rpt_timer_sig  );      /* Watchdog report timer */

    if(task_dog_rpt_time_var > 0)    
        {
          /* Report DOG */
          dog_report(task_dog_rpt_var);
  
          /* Set timer for next report */
          (void) rex_set_timer(watchdog_rpt_timer, task_dog_rpt_time_var);
        }

    (void) rex_clr_sigs(tcb, wait_timer_sig);

}
/*===========================================================================

FUNCTION FTM_SIG_REX_WAIT_CB

DESCRIPTION
  This function is a stub to be able to call ftm_sig_rex_wait where a
  [ void func (uint32) ] function signature is needed.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ftm_sig_rex_wait_cb
(
  rex_sigs_type   mask        /* Mask of signals to wait for */
)
{
  (void) ftm_sig_rex_wait(mask);
} /* End ftm_sig_rex_wait_cb */


/*===========================================================================

FUNCTION FTM_SIG_REX_WAIT_NO_CLEAR_CB

DESCRIPTION
  This function is a stub to be able to call ftm_sig_rex_wait_no_clear where
  a [ void func (uint32) ] function signature is needed.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ftm_sig_rex_wait_no_clear_cb
(
  rex_sigs_type   mask        /* Mask of signals to wait for */
)
{
  #ifdef FEATURE_FACTORY_TESTMODE
  (void) ftm_sig_rex_wait_no_clear(mask);
  #endif
} /* End ftm_sig_rex_wait_no_Clear_cb */


/*===========================================================================

FUNCTION FTM_SIG_REX_SET

DESCRIPTION
  This procedure sets a signal for ftm_task to handle.

DEPENDENCIES
  None.

RETURN VALUE
  Signal mask from signals that were set after rex_wait call.

SIDE EFFECTS
  None.

===========================================================================*/
void ftm_sig_rex_set
(
  rex_sigs_type   mask        /* Mask of signals to wait for */
)
{
  (void) rex_set_sigs(TASK_HANDLE(ftm), mask);
} /* End ftm_sig_rex_set */

/*===========================================================================

FUNCTION FTM_CLK_REX_WAIT

DESCRIPTION
   This function does a wait in ms, but does not hug the cpu. instead it lets
   the OS (REX) run and other tasks run.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_clk_rex_wait (uint32 msec)
{
  /* Set wait timer to msec */
  (void) rex_set_timer(&ftm_wait_timer, msec);

  /* Waiting for timer to expire and set the sig */
  (void) ftm_sig_rex_wait(FTM_CLK_WAIT_SIG);
} /* End ftm_clk_rex_wait */

/*===========================================================================
FUNCTION FTM_SIG_CLK_REX_WAIT

DESCRIPTION
  This procedure waits for specified signals to be set.  While it waits,
  this procedure continues to set the Watchdog report signal and reset
  the mc_rpt_timer. The clk addition means that this function is capable
  of waiting for a signal with a time out, instead of waiting forever.
  Use FTM_WAIT_FOREVER for msec for this function to behaive just like
  ftm_sig_rex_wait.

DEPENDENCIES
  None.

RETURN VALUE
  Signal mask from signals that were set after rex_wait call. The return
  value is the current siganls from REX, if this value is AND with
  FTM_WAIT_TIMEOUT and the result is FTM_WAIT_TIMEOUT then the operation
  timedout.


SIDE EFFECTS
  None.

===========================================================================*/
rex_sigs_type ftm_sig_clk_rex_wait
(
  rex_sigs_type   mask,        /* Mask of signals to wait for */
  uint32          msec         /* Timeout value in msecs */
)
{
  rex_sigs_type sigs;

  if ( msec != FTM_WAIT_FOREVER )
  {
    (void) rex_set_timer(&ftm_wait_timer, msec);

    sigs = ftm_sig_rex_wait(FTM_CLK_WAIT_SIG | mask);

    if ( !(sigs & FTM_CLK_WAIT_SIG) )
    {
      (void)rex_clr_timer(&ftm_wait_timer);
    }
  }
  else
  {
    sigs = ftm_sig_rex_wait (mask);
  }

  return sigs;

} /* End ftm_sig_clk_rex_wait */

/*===========================================================================
FUNCTION FTM_SIG_CLK_REX_WAIT_NO_CLEAR 
 
DESCRIPTION
  This procedure waits for specified signals to be set.  While it waits,
  this procedure continues to set the Watchdog report signal and reset
  the mc_rpt_timer. The clk addition means that this function is capable
  of waiting for a signal with a time out, instead of waiting forever.
  Use FTM_WAIT_FOREVER for msec for this function to behaive just like
  ftm_sig_rex_wait.

DEPENDENCIES
  None.

RETURN VALUE
  Signal mask from signals that were set after rex_wait call. The return
  value is the current siganls from REX, if this value is AND with
  FTM_WAIT_TIMEOUT and the result is FTM_WAIT_TIMEOUT then the operation
  timedout.


SIDE EFFECTS
  None.

===========================================================================*/
rex_sigs_type ftm_sig_clk_rex_wait_no_clear
(
  rex_sigs_type   mask,        /* Mask of signals to wait for */
  uint32          msec         /* Timeout value in msecs */
)
{
  rex_sigs_type sigs;

  if ( msec != FTM_WAIT_FOREVER )
  {
    (void) rex_set_timer(&ftm_wait_timer, msec);

    sigs = ftm_sig_rex_wait_no_clear(FTM_CLK_WAIT_SIG | mask);

    if ( !(sigs & FTM_CLK_WAIT_SIG) )
    {
      (void)rex_clr_timer(&ftm_wait_timer);
    }
  }
  else
  {
    sigs = ftm_sig_rex_wait_no_clear(mask);
  }

  return sigs;

} /* End ftm_sig_clk_rex_wait_no_clear */

#ifdef FEATURE_LTE
/*===========================================================================

  FUNCTION:  rf_lte_ftm_msgr_register

===========================================================================*/
/*!
  @brief
  This function is used by FTM Task to register for response messages sent by the
  RF LTE LTE Calibration processor via the message router.


  @return
  TRUE if all registration were successful, FALSE otherwise
*/
/*=========================================================================*/
boolean rf_lte_ftm_msgr_register( msgr_client_t *client_id, msgr_id_t id )
{
  boolean         rc         = TRUE;
  errno_enum_type ret_status = E_SUCCESS;

  /* Register for the RF LTE FTM message FTM task is  interested in receiving */
  ret_status = msgr_register_block(MSGR_FTM_CORE,
                                   client_id,
                                   id,
                                   rf_lte_ftm_umid_list,
                                   RF_LTE_FTM_MSG_CNT);
  if(ret_status != E_SUCCESS)
  {
      FTM_MSG_ERROR("rf_lte_ftm_msgr_register: Unable to register FTM message router register block.",0,0,0);
      rc = FALSE;
  }
  return (rc);
}

/*===========================================================================

  FUNCTION:  rf_lte_ftm_msgr_deregister

===========================================================================*/
/*!
  @brief
  This function is used by FTM Task to de-register the messages registered earlier 
  for handling the messages sent by the RF LTE FTM Calibration processor to the message router


  @return
  TRUE if all de-registration were successful, FALSE otherwise
*/
/*=========================================================================*/
boolean rf_lte_ftm_msgr_deregister( msgr_client_t *client_id )
{
  boolean         rc         = TRUE;
  errno_enum_type ret_status = E_SUCCESS;

  /* Deregister all the RF LTE FTM message FTM task has registerd for. */
  ret_status = msgr_deregister_block(MSGR_FTM_CORE,
                                     client_id,
                                     rf_lte_ftm_umid_list,
                                     RF_LTE_FTM_MSG_CNT);
  if(ret_status != E_SUCCESS)
  {
      FTM_MSG_ERROR("rf_lte_ftm_msgr_deregister: Unable to deregister FTM message router register block.",0,0,0);
      rc = FALSE;
  }
  return (rc);
}

/*===========================================================================

  FUNCTION:  ftm_create_msgr_client

===========================================================================*/
/*!
  @brief
  Creates the ftm Message router clients

*/
/*=========================================================================*/
static void ftm_create_msgr_client 
( 
  void
)
{
  errno_enum_type ret_status;
  ftm_rex_msgr_msg_type rex_msgr_msg;
  uint16 ftm_rex_msgr_hdr_offset = 0;

  /* Create FTM message router client for receiving messages from SW entities such RFA,L1,L2 etc.  */
  ret_status = msgr_client_create(&(ftm_task_info.ftm_msgr_client));
  if( ret_status != E_SUCCESS)
  {
      FTM_MSG_ERROR("Unable to create FTM message router client.",0,0,0);  
  };

  /*! @brief Create message queue to receive message router messages from LTE SW
   * Since FTM is a REX Task, it is required to add a REX message queue to the
   * message router client created above.
   *
   * msgr_client_add_rex_q() is used to add REX queue into message router client
   * The arguments are as followed:
   * msgr_client_add_rex_q ( 
   *                  client_id: the message router client identifier
   *                  pointer to REX task control block, required to MSGR to set signals
   *                  REX Signal: The signal to be sent when a message is to be delivered to this queue
   *                  Incoming message queue: the queue that the incoming message will be queued into
   *                  Free Queueu: Message router gets an item from this queue, copy the received message 
   *                               and queue it to the incoming message queue
   *                  MSGR header offset: Offset from the start of the message structure where 
   *                                      msgr_hdr_struct_type is found
   *                  CMD ID offset: Optional, if positive, the UMID of the received message will be
   *                                 copied into the location specified by the offset
   *                  MSG Q ID: the message queue ID returned by MSGR
   * 
   */
  ftm_rex_msgr_hdr_offset = (uint16)((uint32)&rex_msgr_msg.ftm_msgr_msg - (uint32)&rex_msgr_msg);
  ret_status = msgr_client_add_rex_q(
      &(ftm_task_info.ftm_msgr_client),
      TASK_HANDLE(ftm),
      FTM_LTE_CMD_Q_SIG, 
      &ftm_task_lte_q,
      &ftm_task_free_q,
      ftm_rex_msgr_hdr_offset,
      0,
      sizeof(ftm_rex_msgr_msg_type),
      &ftm_task_info.lte_mb
      );
  if(ret_status != E_SUCCESS)
  {
      FTM_MSG_ERROR("Unable to add mailbox for FTM messages to the FTM REX message router client.",0,0,0);
      ftm_deinit();
  }
}
#endif /* FEATURE_LTE  */

#ifdef FEATURE_GSM
static msgr_client_t     ftm_rfgsm_rex_msgr_client;
static msgr_id_t         ftm_rfgsm_rex_msgr_id;

/*===========================================================================

  FUNCTION:  ftm_rfgsm_init_rex_msgr_client

===========================================================================*/
/*!
  @brief
  Creates a MSGR Client and CMD queue for the FTM task to receive GSM SW commands
  from entities such as RFA,L1,L2 etc. Currently only used by RFA 
*/
/*=========================================================================*/


static void ftm_rfgsm_init_rex_msgr_client( void )
{
  errno_enum_type msgr_error;
  ftm_rfgsm_rex_msgr_msg_type rex_rfgsm_msgr_msg;
  uint16 ftm_rex_msgr_hdr_offset = 0;

  msgr_error = msgr_client_create( &ftm_rfgsm_rex_msgr_client );

  if( msgr_error != E_SUCCESS)
  {
     FTM_MSG_ERROR("Unable to create FTM REX MSGR client for RFGSM messages from RF task.",0,0,0);  
  };


  ftm_rex_msgr_hdr_offset = (uint16)( (uint32)&rex_rfgsm_msgr_msg.rfgsm_ftm_msgr_msg - (uint32)&rex_rfgsm_msgr_msg );

  /* Add a Rex Queue for RF task GSM messages to msgr client */

  msgr_error = msgr_client_add_rex_q( &ftm_rfgsm_rex_msgr_client,
                                      TASK_HANDLE(ftm),
                                      FTM_RF_GSM_CMD_Q_SIG, 
                                      &ftm_task_rfgsm_q,
                                      &ftm_task_rfgsm_free_q,
                                      ftm_rex_msgr_hdr_offset,
                                      0,
                                      sizeof(ftm_rfgsm_rex_msgr_msg_type),
                                      &ftm_rfgsm_rex_msgr_id );

  if(msgr_error != E_SUCCESS)
  {
     FTM_MSG_ERROR("Unable to add rex queue for RFGSM messages from RF task to the FTM REX MSGR client.",0,0,0);
  }

  msgr_error = msgr_register_block( MSGR_FTM_GSM,
                                    &ftm_rfgsm_rex_msgr_client,
                                    ftm_rfgsm_rex_msgr_id,
                                    rfgsm_rex_ftm_umid_cmd_list,
                                    RF_GSM_REX_FTM_MSG_CNT );
  if (msgr_error != E_SUCCESS)
  {
    FTM_MSG_ERROR("Unable to register RF GSM FTM messages with FTM Rex MSGR client.",0,0,0);
    ftm_gsm_deinit_rex_msgr_client();
  }

}

#define FTM_TASK_GSM_MB_DEPTH 3
#define FTM_TASK_GSM_MB_PRIORITY   0

/* This FTM_GSM MSGR Client is externally referenced during msgr_receive blocking wait */
msgr_client_t      ftm_rfgsm_msgr_client; 
static msgr_id_t   ftm_rfgsm_msgr_mb_id;

/*===========================================================================

  FUNCTION:  ftm_rfgsm_init_ftm_msgr_client

===========================================================================*/
/*!
  @brief
  Creates a MSGR Client, Initialises RF Task->FTM Task message router mailbox
  information and registers for UMID responses from RF Task.
  The mailbox will be used for receiving any RSP messages from msgr_send()
  commands which require FTM task to block until the command has been completed.
*/
/*=========================================================================*/

static void ftm_rfgsm_init_ftm_msgr_client( void )
{
  errno_enum_type msgr_error;
  msgr_error = msgr_client_create( &ftm_rfgsm_msgr_client );

  if( msgr_error != E_SUCCESS)
  {
     FTM_MSG_ERROR("Unable to create FTM MSGR client for receiving blocking RFGSM messages from RF task.",0,0,0);  
  };

  msgr_error = msgr_client_add_mq( "FTM TASK RFGSM Mailbox",
                                   &ftm_rfgsm_msgr_client,
                                   (uint8)FTM_TASK_GSM_MB_PRIORITY,
                                   (uint32)FTM_TASK_GSM_MB_DEPTH,
                                   (uint32) sizeof(rfgsm_ftm_msg_u),
                                   &ftm_rfgsm_msgr_mb_id );
  if(msgr_error != E_SUCCESS)
  {
     FTM_MSG_ERROR("Unable to add mailbox for blocking RFGSM messages from RF task to the FTM MSGR client",0,0,0); 
  }

  msgr_error = msgr_register_block( MSGR_FTM_GSM,
                                    &ftm_rfgsm_msgr_client,
                                    ftm_rfgsm_msgr_mb_id,
                                    rfgsm_ftm_umid_cmd_list,
                                    RF_GSM_FTM_MSG_CNT );

  if(msgr_error != E_SUCCESS)
  {
    FTM_MSG_ERROR("Unable to register for blocking RF GSM FTM messages with FTM MSGR client",0,0,0);
    ftm_gsm_deinit_msgr_client();
  }
}


#endif /* FEATURE_GSM  */

/*===========================================================================

FUNCTION FTM_TASK_INIT

DESCRIPTION
  This is the Factory Test Mode(FTM) task initialize file. It sets up all
  necessary items for FTM to run.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void  ftm_task_init( void )
{
#if defined (FEATURE_LTE) || defined (FEATURE_GSM)
  uint8       i         = 0;
  q_type      *q_ptr    = NULL;
  q_link_type *link_ptr = NULL;

  ftm_task_info.is_task_running = FALSE;
#endif /* ( FEATURE_LTE || FEATURE_GSM ) */

  /*Initialize Sequence command processing condition to prevent seq from running
     if flag is set to TRUE. Default setting - FALSE*/
  CmdInProgressBlockSequencer = FALSE;

   /* Initialize command and "free" queues - - - - - - - - - - - - - - - - - -*/
  (void) q_init( &ftm_task_cmd_q );       /* Initialize command queue */

  (void) q_init( &ftm_task_event_q );     /* Initialize command queue */


  /* init critical section data structure for FTM */
  ftm_common_init_critical_section();

  /* Initialize timers */
  rex_def_timer( &ftm_wait_timer, TASK_HANDLE(ftm), FTM_CLK_WAIT_SIG );

#if defined (FEATURE_LTE) || defined (FEATURE_GSM)
  // Set a flag to indicate that FTM is alive..
  ftm_task_info.is_task_running = TRUE;
#endif /* ( FEATURE_LTE || FEATURE_GSM ) */

  (void) rex_clr_sigs( TASK_HANDLE(ftm), TASK_START_SIG );
  /* Following Section is required to be called AFTER tmc_task_start() */

#ifdef FTM_HAS_IRAT
  ftm_meas_init();
#endif

#ifdef FEATURE_LTE
  /* Set up the Task Control Block for Message queues */
  q_ptr = q_init( &ftm_task_lte_q );       /* Initialize LTE message queue */
  q_ptr = q_init( &ftm_task_free_q );      /* Initialize free queue */
  /* Link all free elements into free queue*/
  for(i=0; i < FTM_MAX_FREE_QUEUE_ITEM; i++)
  {
    link_ptr = q_link( &rex_lte_ns_msg[i], &rex_lte_ns_msg[i].link );
    if( link_ptr != NULL )
    {
      q_put( q_ptr, link_ptr );
    }
    else
    {
      FTM_MSG_ERROR("FTM REX:ERROR: ftm_task_main: Linking Free item to Free Q",0,0,0);
    }
  }
  FTM_MSG_LOW("FTM REX: ftm_task_main: All queue created",0,0,0);
  /*Create the message router client for all commands FTM intends to recieve.
    Mailbox is a component services specific name which is not appropriate in 
  REX And POSIX. Thus replacing with a more OS-neutral term.*/
  ftm_create_msgr_client();

  //Register for the FTM Calibration UMIDs that FTM task wants to recieve.
  (void)rf_lte_ftm_msgr_register(&(ftm_task_info.ftm_msgr_client), ftm_task_info.lte_mb);

  /* Initialize FTM LTE Non-signaling processor */
  ftm_lte_ns_proc_init();
  ftm_lte_ns_proc_msgr_store(&(ftm_task_info.ftm_msgr_client), ftm_task_info.lte_mb);
  ftm_lte_meas_register();
#endif /* FEATURE_LTE */

#ifdef FEATURE_GSM
  /* Set up the Task Control Block for Message queues */
  q_ptr = q_init( &ftm_task_rfgsm_q );       /* Initialise RFGSM message queue */
  q_ptr = q_init( &ftm_task_rfgsm_free_q );      /* Initialise free queue */

  /* Link all free elements into free queue */
  for( i=0; i < FTM_GSM_MAX_FREE_QUEUE_ITEM; i++ )
  {
    link_ptr = q_link( &rex_rfgsm_msg[i], &rex_rfgsm_msg[i].link );
    if( link_ptr != NULL )
    {
      q_put( q_ptr, link_ptr );
    }
    else
    {
      FTM_MSG_ERROR("FTM REX:ERROR: ftm_task_main: Linking Free item to FTM GSM Free Q",0,0,0);
    }
  }

  /* Create an FTM REX MSGR client for adding REX queue and FTM Task signal */
  ftm_rfgsm_init_rex_msgr_client();

  /* Create a message router mailbox for receiving blocking RFA Task CNF messages.
   * The mailbox will be used for receiving any CNF messages from msgr_send() commands
   * which require FTM task to block until the command has been completed
   */
  ftm_rfgsm_init_ftm_msgr_client();
  ftm_gsm_meas_register();

#endif /* FEATURE_GSM */
  /* ----------------------------------------------------------- */

  /* Initialize timers - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  rex_def_timer(&ftm_rpt_timer, TASK_HANDLE(ftm), FTM_RPT_TIMER_SIG); /* Dog Report */

  /* call ftmdiag_init last, we need to make sure we have init all our
  resouces, before we can accept any cmds from diag.
  */
  ftmdiag_init();

#ifdef FEATURE_GSM
  #ifdef FTM_HAS_LOGGING
  rfgsm_core_log_q_init();
  #endif
  #ifdef FEATURE_FTM_GSM_BER
     ftm_gsm_ber_init();

     #if defined(FEATURE_GSM_EGPRS) && defined(FEATURE_FTM_EGPRS_BER)
     ftm_egprs_ber_init();
     #endif
  #endif
#endif

#ifdef FTM_HAS_UMTS
  #ifdef FEATURE_FTM_WCDMA_BER
     /* Define rex timer to be used for ftm_wcdma_enter/exit in ftm task */
     rex_def_timer(&ftm_wcdma_enter_exit_wait_timer_ftm_tcb, &ftm_tcb, FTM_WCDMA_BER_CMD_Q_SIG);
     ftm_wcdma_ber_init();
  #endif
  #ifdef FEATURE_WCDMA
  ftm_wcdma_meas_register();

  /* Define rex timer to be used for temp comp in ftm task */
  rex_def_timer(&ftm_wcdma_execute_temp_comp_timer, TASK_HANDLE(ftm), FTM_WCDMA_EXECUTE_TEMP_COMP_SIG);

  /* Clear the signal */
  (void)rex_clr_sigs( TASK_HANDLE(ftm), FTM_WCDMA_EXECUTE_TEMP_COMP_SIG );

  /* Set the timer = 10 sec for temp comp */
  (void)rex_set_timer(&ftm_wcdma_execute_temp_comp_timer, FTM_WCDMA_TEMP_COMP_FREQUENCY_MS);
  #endif
  #endif

#ifdef FTM_HAS_TDSCDMA
  #ifdef FTM_HAS_TDSCDMA_BER
     ftm_tdscdma_ber_init();
  #endif
  ftm_tdscdma_meas_register();
#endif


#ifdef FEATURE_CDMA1X
  /* Register 1x and HDR for FTM IRAT*/
  ftm_1x_meas_register();
  ftm_hdr_meas_register();
#endif

#ifdef FEATURE_FTM_HWTC
  /* init DVT string command handler */
  ftm_hwtc_dvt_init();
#endif /* FEATURE_FTM_HWTC */

#ifndef FEATURE_MODEM_RCINIT
  /* ----------------------------------------------------------- */
  /*
  ** Wait for the start signal from the Task Main Controller
  */
  tmc_task_start();
#endif /* FEATURE_MODEM_RCINIT */

#if defined (FEATURE_LTE) || defined (FEATURE_GSM)
  // Set a flag to indicate that FTM is alive..
  ftm_task_info.is_task_running = TRUE;
#endif /* ( FEATURE_LTE || FEATURE_GSM ) */

  (void) rex_clr_sigs( rex_self(), TASK_START_SIG );
  

  FTM_Sequencer_InitializeState(0);

  FTM_MSG_LOW("FTM task Initialization finished",0,0,0);
} /* End ftm_task_init */



/*===========================================================================

FUNCTION FTM_PROCESS_EVENTS

DESCRIPTION
   This function handles delayed rsp requests from Diag + events from other 
   susbsystems. Dispatches the appropriate functions and or macros.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/

LOCAL void ftm_process_events(void)
{

  ftm_event_type   *evt_ptr;                     /* Pointer to a ftm event */

  /* Get an event from the Queue.
  */
  while ( ( evt_ptr = (ftm_event_type *) q_get( &ftm_task_event_q) ) != NULL)
  {
    evt_ptr->queued = FALSE;

    switch (evt_ptr->evt_cat)
    {
    case FTM_EVENT_DIAG:
      ftm_diag_evt_processor(evt_ptr);
      break;

    default:
      /* message */
      break;
    }

    /* If there is a 'done queue' the queue the command on that queue
    ** as we are done processing it.
    */
    if (evt_ptr->hdr.done_q_ptr != NULL)
    {
      q_put(evt_ptr->hdr.done_q_ptr, &evt_ptr->hdr.link);
    }

    /* free up the event structure also */
    if (!evt_ptr->reuse)
    (void) ftm_free(evt_ptr);
  }
}



/*===========================================================================

FUNCTION FTM_ALLOCATE_DELAYED_RSP_BYTES

DESCRIPTION
   This function requests delayed response bytes from Diag 

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/

byte* ftm_allocate_delayed_rsp_bytes(
                           diagpkt_subsys_delayed_rsp_id_type delay_rsp_id,
                           diagpkt_subsys_cmd_code_type subsys_cmd_code,
                           uint16 rsp_cnt,
                           uint32 status, 
                           uint32 length)
{
  void* rsp_bytes = NULL;
  ftm_diagpkt_subsys_hdr_type_v2* rsp_hdr;

  rsp_bytes = (byte*) diagpkt_subsys_alloc_v2_delay(
                           (diagpkt_subsys_id_type)DIAG_SUBSYS_FTM, 
                           subsys_cmd_code, 
                           delay_rsp_id,
                           length);

  if (rsp_bytes)
  {
    rsp_hdr = (ftm_diagpkt_subsys_hdr_type_v2 *)rsp_bytes;
    // the response count is set to 1 by the API function (and status to 0)
    diagpkt_subsys_set_rsp_cnt(rsp_bytes, rsp_cnt);
    rsp_hdr->status = status;
  }

  return rsp_bytes;
}


/*===========================================================================

FUNCTION FTM_DIAG_EVT_PROCESSOR

DESCRIPTION
   This function handles delayed rsp requests from Diag 

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/

void ftm_diag_evt_processor(ftm_event_type *evt_ptr)
{

  ftm_diag_event_type            *diag_req;
  ftm_diagpkt_subsys_header_type *header;

  diag_req = (ftm_diag_event_type*) (evt_ptr->evt_data_ptr);
  header = (ftm_diagpkt_subsys_header_type*) diag_req->ftm_data_ptr;

  if (header == NULL)
  {
    FTM_MSG_HIGH("The pointer is NULL.",0,0,0);
    return;
  }

  /* route command depending on header information */

  // clean DIAG Payload bytes inside the 3 following fns
  switch (header->cmd_code)
  {
  case DIAG_SUBSYS_CMD_VER_2_F:
    ftm_diag_delayed_rsp_cmd_processor(evt_ptr);
    break;

  case DIAG_EXT_MSG_CONFIG_F:
    ftm_diag_msg_config_cmd_processor(evt_ptr);
    break;

  case DIAG_LOG_CONFIG_F:
    break;

  default:
    /* message */
    break;
  }

  /* clean request struct memory here */
  if (!evt_ptr->reuse)
  {
    if (diag_req->ref_ct == 0)
    {
      ftm_free(diag_req);
    }
  }
}



/*===========================================================================

FUNCTION FTM_DIAG_MSG_CONFIG_CMD_PROCESSOR

DESCRIPTION
   This function handles 125 requests from Diag 

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/

void ftm_diag_msg_config_cmd_processor(ftm_event_type *evt_ptr)
{
  uint16                          ftm_subsys_code;
  ftm_diag_125_event_type        *diag_req;

  diag_req = (ftm_diag_125_event_type*) (evt_ptr->evt_data_ptr);

  ftm_subsys_code = diag_req->ftm_subsys_code;

  switch (ftm_subsys_code)
  {

  case FTM_MODE_MAX_C:

    break;

  default:

    break;
  }

  if (!evt_ptr->reuse)
  {
    if (diag_req->ftm_diag_event.ftm_data_ptr)
    {
      if (diag_req->ftm_diag_event.ref_ct == 0)
      {
        ftm_free(diag_req->ftm_diag_event.ftm_data_ptr);
      }
    }
  }
}



/*===========================================================================

FUNCTION FTM_DIAG_DELAYED_RESPONSE_CMD_PROCESSOR

DESCRIPTION
   This function handles delayed rsp requests from Diag 

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/

void ftm_diag_delayed_rsp_cmd_processor(ftm_event_type *evt_ptr)
{
  // ret_code Will be set to subsystem specific status like a 
  // dspdiag_ret_code_type. So 1st element of enum should be a
  // generic failure like DSPDIAG_FAILED = 0
  word ret_code = 0;  

  ftm_diag_128_event_type        *diag_req;
  ftm_subsys_cmd_packet_type     *subsys_payload;
  ftm_subsys_rsp_packet_type     *rsp_packet;
  uint32                          rsp_length;
#ifndef FEATURE_FTM_HWTC
  boolean                         bypass_ftm_mode_check = FALSE;
#endif /* FEATURE_FTM_HWTC */
//  uint32                          subsys_payload_len;

  diag_req = (ftm_diag_128_event_type*) (evt_ptr->evt_data_ptr);

  subsys_payload = 
    (ftm_subsys_cmd_packet_type*)diag_req->ftm_diag_event.ftm_data_ptr;

  if (subsys_payload == NULL)
  {
    FTM_MSG_HIGH("The pointer is NULL.",0,0,0);
    return; 
  }
  /* route command depending on header information */

  FTM_MSG_HIGH("Start processing FTM packet: %d,%d,%d",
              subsys_payload->diag_header.cmd_code,
              subsys_payload->diag_header.subsys_id,
              subsys_payload->diag_header.subsys_cmd_code);

#ifndef FEATURE_FTM_HWTC
  if (ftm_mode == FTM_MODE || bypass_ftm_mode_check)
  {
#endif  /* FEATURE_FTM_HWTC */
    switch (subsys_payload->diag_header.subsys_id)
    {
    case FTM:
      switch (subsys_payload->diag_header.subsys_cmd_code)
      {

      default:
        FTM_MSG_ERROR("Unknown FTM Command Code: %d",
                      subsys_payload->diag_header.subsys_cmd_code,0,0);
        // rsp_pkt.cmd = FTM_RSP_BAD_CMD;
        break;
      }
      break;

    default:
      FTM_MSG_ERROR("Unknown subsys id",subsys_payload->diag_header.subsys_id,0,0);
      // rsp_pkt.cmd = FTM_RSP_BAD_CMD;
      break;
    } /* Switch */

#ifndef FEATURE_FTM_HWTC
  }

  else
  {
    FTM_MSG_ERROR("Mode = %d, Not in FTM mode, Aborting Command.",
                  ftm_mode,0,0);
    // rsp_pkt.cmd = FTM_RSP_BAD_MODE;
  }
#endif /* FEATURE_FTM_HWTC */

  FTM_MSG_MED("Done processing FTM packet: %d,%d,%d",
              subsys_payload->diag_header.cmd_code,
              subsys_payload->diag_header.subsys_id,
              subsys_payload->diag_header.subsys_cmd_code);

  // send back a unified delayed response from here


  // subsys_payload_len = diag_req->ftm_diag_event.req_pkt_len - 
  //                       sizeof(ftm_diagpkt_subsys_header_type);


  rsp_length = sizeof(ftm_diagpkt_subsys_hdr_type_v2); // + 
               // subsys_payload_len;
               

  rsp_packet = 
    (ftm_subsys_rsp_packet_type*)((void *) ftm_allocate_delayed_rsp_bytes(
                                    diag_req->delay_rsp_id,
                                    subsys_payload->diag_header.subsys_cmd_code,
                                    (uint16) 1,
                                    (uint32) ret_code,  
                                    (uint32) rsp_length));


  // subsys_payload = (ftm_subsys_cmd_packet_type*)diag_req;
  /*
  if (rsp_packet)
  {
    memcpy((void*)rsp_packet->data, 
           subsys_payload_len,
           (void*)subsys_payload->data, 
           subsys_payload_len);
  
    diagpkt_delay_commit(rsp_packet);
  }*/

  if (rsp_packet != NULL)
  {
    diagpkt_delay_commit(rsp_packet);
  }
  
  /* free up the ftm pkt memory. Subsystem may incr. ref_ct
     if it wants to clean it up later on.
  */
  if (!evt_ptr->reuse)
  {
    if (diag_req->ftm_diag_event.ftm_data_ptr)
    {
      if (diag_req->ftm_diag_event.ref_ct == 0)
        ftm_free(diag_req->ftm_diag_event.ftm_data_ptr);
    }
  }
}


/*===========================================================================

FUNCTION FTM_DISPATCH

DESCRIPTION
   This function handles requests to run tests and  other primitives
   by dispatching the appropriate functions and or macros.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
FTM_Sequencer_ErrorCode ftm_dispatch( FTM_Sequencer_ContextID       iContextID,
                                            unsigned short                iRequestPacketSize,
                                            unsigned char*                pRequestBytes,
                                            unsigned short*               piResponsePacketSize,
                                      FTM_Sequencer_ResponsePacket* pResponseBytes
                                    )  
{

    #ifdef FEATURE_LTE
    ftm_lte_pkt_type           *lte_msg_ptr       = NULL;
    ftm_lte_payload_type       *payload_ptr       = NULL;
    ftm_msgr_u                 *msgr_msg_ptr      = NULL;
    boolean                     is_cmd_valid      = FALSE;
    #endif /* FEATURE_RF_HAS_LTE */

    #ifndef FEATURE_FTM_HWTC
    boolean                         bypass_ftm_mode_check = FALSE;
    #endif /* FTM_HAS_LOGGING */

    FTM_Sequencer_ErrorCode         _eErrorCode = FTM_Sequencer_Error_None;
    ftm_rsp_pkt_type                rsp_pkt = {FTM_RSP_DO_LEGACY, 0, NULL, FALSE};
    ftm_diagpkt_subsys_header_type *header;           /* For shorthand access to parameters */

    /* route command depending on header information */
    header = (ftm_diagpkt_subsys_header_type*) pRequestBytes;

    if(!header)
    {
      FTM_MSG_HIGH("ftm_dispatch: NULL assignment to header",0,0,0);
      return FTM_Sequencer_Error_MemoryAllocation;
    }

    FTM_MSG_HIGH("Start processing FTM packet: %d,%d,%d",
                header->cmd_code,
                header->subsys_id,
                header->subsys_cmd_code);

    #ifndef FEATURE_FTM_HWTC
    #ifdef FTM_HAS_LOGGING
    if(header->subsys_id == FTM && header->subsys_cmd_code == FTM_LOG_C)
    {
      bypass_ftm_mode_check = TRUE;
    }
    /* Set bypass_ftm_mode_check flag for FTM_GNSS_C to enable ELNA QRCT FTM commands for Online */
    if(header->subsys_id == FTM && header->subsys_cmd_code == FTM_GNSS_C)
    {
      bypass_ftm_mode_check = TRUE;
    }
    #ifdef FTM_HAS_SV_API
    /* Set bypass_ftm_mode_check flag for FTM_SV_C to enable FTM commands for Online */
    if(header->subsys_id == FTM && header->subsys_cmd_code == FTM_SV_C)
    {
      bypass_ftm_mode_check = TRUE;
    }
    #endif

    if((header->subsys_id == FTM && header->subsys_cmd_code == FTM_COMMON_C)
       &&
    (((ftm_common_pkt_type*) pRequestBytes)->ftm_hdr.cmd_id==FTM_GET_AGC_RESOLUTION_RX
    ||((ftm_common_pkt_type*)pRequestBytes)->ftm_hdr.cmd_id==FTM_GET_AGC_RESOLUTION_TX)
    ||((ftm_common_pkt_type*)pRequestBytes)->ftm_hdr.cmd_id==FTM_RFLM_DBG_CMD))
 
    {
      bypass_ftm_mode_check = TRUE;
    }

    #endif


    #ifndef FEATURE_GNSS_ONLY_NO_WWAN
    if( (header->subsys_id == FTM) && 
        ( (header->subsys_cmd_code == FTM_GSM_C0_C) || 
          (header->subsys_cmd_code == FTM_GSM_C1_C) || 
          (header->subsys_cmd_code == FTM_GSM_C2_C) || 
          (header->subsys_cmd_code == FTM_GSM_C3_C)  )&& 
        (((ftm_pkt_type*)pRequestBytes)->rf_params.ftm_rf_cmd == FTM_GSM_CAPTURE_IQ))
    {
      MSG(MSG_SSID_FTM, MSG_LVL_HIGH,"Bypassing FTM mode check ");
      bypass_ftm_mode_check = TRUE;
    }
    #endif


    if ( ftm_mode == FTM_MODE || bypass_ftm_mode_check)
    {
    #endif /* FEATURE_FTM_HWTC */
      switch (header->subsys_id)
      {
      case FTM:
        switch (header->subsys_cmd_code)
        {
        #ifdef FEATURE_CDMA1X
        case FTM_MC_C:
          rsp_pkt = ftm_cdma_mc_dispatch((ftm_mc_pkt_type*)pRequestBytes);
          break;
        case FTM_1X_C0_C:
            rsp_pkt = ftm_cdma_dispatch(RFM_DEVICE_0, (ftm_pkt_type*)pRequestBytes);
          break;

        #ifdef FEATURE_MULTI_RX_CHAIN
        case FTM_1X_C1_C:
          rsp_pkt = ftm_cdma_dispatch(RFM_DEVICE_1, (ftm_pkt_type*)pRequestBytes);
          break;
        #endif

        #endif// FEATURE_CDMA1X

        #ifdef FEATURE_HDR
        case FTM_HDR_C:
          rsp_pkt = ftm_hdr_mc_dispatch((ftm_mc_pkt_type*)pRequestBytes);
          break;
    case FTM_HDR_C0_C:
      rsp_pkt = ftm_hdr_dispatch(RFM_DEVICE_0, (ftm_pkt_type*)pRequestBytes);
      break;
    case FTM_HDR_C1_C:
      rsp_pkt = ftm_hdr_dispatch(RFM_DEVICE_1, (ftm_pkt_type*)pRequestBytes);
      break;
    case FTM_HDR_C2_C:
      rsp_pkt = ftm_hdr_dispatch(RFM_DEVICE_2, (ftm_pkt_type*)pRequestBytes);
      break;
    case FTM_HDR_C3_C:
      rsp_pkt = ftm_hdr_dispatch(RFM_DEVICE_3, (ftm_pkt_type*)pRequestBytes);
      break;        
        #endif /* FEATURE_HDR */

        #ifdef FEATURE_GSM
        case FTM_GSM_C0_C:
          rsp_pkt = ftm_gsm_dispatch( RFM_DEVICE_0, (ftm_pkt_type*) pRequestBytes);
          break;
        
        case FTM_GSM_C1_C:
          rsp_pkt = ftm_gsm_dispatch( RFM_DEVICE_0, (ftm_pkt_type*) pRequestBytes);
          break;

        case FTM_GSM_C2_C:
          rsp_pkt = ftm_gsm_dispatch( RFM_DEVICE_2, (ftm_pkt_type*) pRequestBytes);
          break;
        
        case FTM_GSM_C3_C:
          rsp_pkt = ftm_gsm_dispatch( RFM_DEVICE_2, (ftm_pkt_type*) pRequestBytes);
          break;

        #ifdef FTM_HAS_GSM_EXTENDED
        case FTM_GSM_EXTENDED_C0_C:
          rsp_pkt = ftm_gsm_extended_dispatch( RFM_DEVICE_0, (ftm_gsm_extended_pkt_type*) pRequestBytes);
          break;
        
        case FTM_GSM_EXTENDED_C1_C:
          rsp_pkt = ftm_gsm_extended_dispatch( RFM_DEVICE_0, (ftm_gsm_extended_pkt_type*) pRequestBytes);
          break;

        case FTM_GSM_EXTENDED_C2_C:
          rsp_pkt = ftm_gsm_extended_dispatch( RFM_DEVICE_2, (ftm_gsm_extended_pkt_type*) pRequestBytes);
          break;

        case FTM_GSM_EXTENDED_C3_C:
          rsp_pkt = ftm_gsm_extended_dispatch( RFM_DEVICE_2, (ftm_gsm_extended_pkt_type*) pRequestBytes);
          break;

        #endif /* FTM_HAS_GSM_EXTENDED */
        #endif /* FEATURE_GSM */

        #ifdef FEATURE_WCDMA
        case FTM_WCDMA_C:
          rsp_pkt = ftm_wcdma_dispatch(RFM_DEVICE_0, (ftm_pkt_type*) pRequestBytes);
          break;

        #ifdef FEATURE_WCDMA_RX_DIVERSITY
        case FTM_WCDMA_RX_2_C:
          rsp_pkt = ftm_wcdma_dispatch(RFM_DEVICE_1, (ftm_pkt_type*) pRequestBytes);
          break;
        #endif    /* FTM_HAS_WCDMA_RX_DIVERSITY */
        
        case FTM_WCDMA_RX_3_C:
          rsp_pkt = ftm_wcdma_dispatch(RFM_DEVICE_2, (ftm_pkt_type*) pRequestBytes);
          break;

        case FTM_WCDMA_RX_4_C:
          rsp_pkt = ftm_wcdma_dispatch(RFM_DEVICE_3, (ftm_pkt_type*) pRequestBytes);
          break;
        
        #endif /*FEATURE_WCDMA*/

        #ifdef FTM_HAS_TDSCDMA
        case FTM_TDSCDMA_C:
          rsp_pkt = ftm_tdscdma_dispatch(RFM_DEVICE_0, (ftm_pkt_type*) pRequestBytes);
          break;

        case FTM_TDSCDMA_RX_2_C:        
          rsp_pkt = ftm_tdscdma_dispatch(RFM_DEVICE_1, (ftm_pkt_type*) pRequestBytes);
          break;

        case FTM_TDSCDMA_C2_C:
          rsp_pkt = ftm_tdscdma_dispatch(RFM_DEVICE_2, (ftm_pkt_type*) pRequestBytes);
          break;

        case FTM_TDSCDMA_C3_C:
          rsp_pkt = ftm_tdscdma_dispatch(RFM_DEVICE_3, (ftm_pkt_type*) pRequestBytes);
          break;

        #ifdef FTM_HAS_TDSCDMA_BER
        case FTM_TDSCDMA_BER_C:
          rsp_pkt = ftm_tdscdma_ber_dispatch( (ftm_tdscdma_ber_pkt_type*) pRequestBytes);
          break;
        #endif /* FTM_HAS_TDSCDMA_BER */

        #endif /*FTM_HAS_TDSCDMA*/

        #ifdef FTM_HAS_LOGGING
        case FTM_LOG_C:
          rsp_pkt = ftm_log_dispatch((ftm_log_req_pkt_type*)pRequestBytes);
          break;
        #endif

        #ifdef FEATURE_MMGPS
        case FTM_AGPS_C:
          rsp_pkt = ftm_agps_dispatch((ftm_agps_pkt_type*) pRequestBytes);
          break;
        #endif

        #ifdef FEATURE_PMIC_FTM
        case FTM_PMIC_C:
          {
              rsp_pkt = ftm_pmic_dispatch( (ftm_pmic_pkt_type *) pRequestBytes);
              break;
          }          
        #endif

        #ifdef FEATURE_GSM
        #ifdef FEATURE_FTM_GSM_BER
        case FTM_GSM_BER_C0_C:
          rsp_pkt = ftm_gsm_ber_dispatch( RFM_DEVICE_0, (ftm_gsm_ber_pkt_type*) pRequestBytes);
          break;

        case FTM_GSM_BER_C1_C:
          rsp_pkt = ftm_gsm_ber_dispatch( RFM_DEVICE_1, (ftm_gsm_ber_pkt_type*) pRequestBytes);
          break;

        case FTM_GSM_BER_C2_C:
          rsp_pkt = ftm_gsm_ber_dispatch( RFM_DEVICE_2, (ftm_gsm_ber_pkt_type*) pRequestBytes);
          break;

        case FTM_GSM_BER_C3_C:
          rsp_pkt = ftm_gsm_ber_dispatch( RFM_DEVICE_3, (ftm_gsm_ber_pkt_type*) pRequestBytes);
          break;

        #endif  /* FEATURE_FTM_GSM_BER */
        #endif  /* FEATURE_GSM */

        #ifdef FTM_HAS_UMTS
        #ifdef FEATURE_FTM_WCDMA_BER
        case FTM_WCDMA_BER_C:
          rsp_pkt = ftm_wcdma_ber_dispatch( (ftm_wcdma_ber_pkt_type*) pRequestBytes);
          break;
        #endif
        #endif

        #ifdef FTM_HAS_SE_BER
        case FTM_SE_BER_C:
          rsp_pkt = ftm_se_ber_dispatch( (ftm_common_pkt_type *) pRequestBytes);
          break;
        #endif

        #ifdef FEATURE_FTM_CUSTOMER_EXTENSIONS
        case FTM_MODE_FACTORY_C:
          rsp_pkt = ftm_customer_extensions_dispatch((ftm_custext_pkt_type *)pRequestBytes);
          break;
        #endif

        #ifdef FEATURE_CDMA1X
        case FTM_1X_CAL_V2_C:
          rsp_pkt = ftm_cdma_cal_v2_dispatch( pRequestBytes );
          break;
        #endif /* FEATURE_CDMA1X */

        case FTM_COMMON_C:
          rsp_pkt = ftm_common_dispatch((ftm_common_pkt_type*) pRequestBytes);
          break;

        /* Switch case for IRAT FTM */
        #ifdef FTM_HAS_IRAT
        case FTM_MEAS:
          rsp_pkt = ftm_meas_extended_dispatch((ftm_meas_extended_pkt_type*) pRequestBytes);
          break;
        #endif 

#ifdef FEATURE_LTE
        case FTM_LTE_C:
        case FTM_LTE_NS_C:
        case FTM_LTE_RX_2_C:
        case FTM_LTE_CAR1_C:
        case FTM_LTE_CAR1_RX_2_C:
        case FTM_LTE_CAR2_C:
        case FTM_LTE_CAR2_RX_2_C:

          FTM_MSG_HIGH( "Got LTE CMD 0x%x",header->subsys_cmd_code,0,0);
          /*Initializing the response packet params. In the FTM LTE MSG DISPATCH the params are updated as required.*/
          rsp_pkt.cmd = FTM_RSP_NO_RSP;
          rsp_pkt.pkt_len = 0;
          rsp_pkt.delete_payload = TRUE;
          rsp_pkt.pkt_payload = NULL;
          if( pRequestBytes != NULL)
          {
            msgr_msg_ptr = &(msgr_msg);
            lte_msg_ptr = (ftm_lte_pkt_type *)(pRequestBytes);
            payload_ptr = &(lte_msg_ptr->payload);

            if( ftm_lte_build_msgr_msg( payload_ptr, msgr_msg_ptr ) == TRUE )
            {
              is_cmd_valid = TRUE;
              CmdInProgressBlockSequencer = TRUE;
              ftm_lte_msg_dispatch( msgr_msg_ptr, sizeof(ftm_msgr_u), &rsp_pkt);
              if(rsp_pkt.cmd != FTM_RSP_NO_RSP)
              {
                  CmdInProgressBlockSequencer = FALSE;
              }
            }
            else
            {
              CmdInProgressBlockSequencer = TRUE;
              rsp_pkt = ftm_lte_rex_dispatch((void*)lte_msg_ptr);
              if(rsp_pkt.cmd != FTM_RSP_BAD_CMD)
              {
                is_cmd_valid = TRUE;
              }
              if(rsp_pkt.cmd != FTM_RSP_NO_RSP)
              {
                CmdInProgressBlockSequencer = FALSE;
              }
            }
            if (is_cmd_valid == FALSE)
            {
              FTM_MSG_ERROR("ftm_lte_build_msgr_msg: Unknown Command %d",lte_msg_ptr->payload.cmd_id,0,0);
            }
          }
          else
          {
            FTM_MSG_HIGH( "NULL Data Pointer",0,0,0);
            rsp_pkt.cmd = FTM_RSP_BAD_CMD;
          }

          break;

        case FTM_LTE_RFM_C:
          FTM_MSG_HIGH( "Got LTE CMD 0x%x",header->subsys_cmd_code,0,0);

          rsp_pkt = ftm_lte_rex_dispatch(pRequestBytes);
          ftm_pkt_data.data_rsp_ptr = NULL; /* This is required as this prevents
                                               ftmdiag_diag_free() from being called */

          break;

        case FTM_INO_C:
          //rsp_pkt = ftm_common_ino_dispatch((ftm_common_pkt_type *) msg_ptr->ftm_data_ptr);
          rsp_pkt = ftm_common_ino_dispatch(pRequestBytes);
          break;

#endif /*FEATURE_RF_HAS_LTE*/
#if defined ( FEATURE_CDMA1X )
        case FTM_1X_C2_C:
          rsp_pkt = ftm_cdma_dispatch(RFM_DEVICE_2, (ftm_pkt_type*)pRequestBytes);
          break;

        case FTM_1X_C3_C:
          rsp_pkt = ftm_cdma_dispatch(RFM_DEVICE_3, (ftm_pkt_type*)pRequestBytes); 
          break;

        case FTM_RFM_C:
          rsp_pkt = ftm_radio_dispatch(pRequestBytes);
          break;

#endif /* FEATURE_CDMA1X */
#ifdef FEATURE_CGPS
        case FTM_GNSS_C:
          rsp_pkt = ftm_gnss_dispatch((ftm_pkt_type*) pRequestBytes);
          break;
#endif

#ifdef FTM_HAS_SV_API
        case FTM_SV_C:
          rsp_pkt = ftm_sv_dispatch((ftm_sv_pkt_type*) pRequestBytes);
          break;
#endif

        case FTM_RFNV_C:
          rsp_pkt = ftm_rfnv_dispatch((ftm_rfnv_common_header_type*) pRequestBytes);
          break;

        default:
          FTM_MSG_ERROR("Unknown FTM Command Code: %d",header->subsys_cmd_code,0,0);
          rsp_pkt.cmd = FTM_RSP_BAD_CMD;
          break;
        }
        break;


#ifdef FEATURE_FTM_HWTC
      case FTM_HWTC:
        rsp_pkt = ftm_hwtc_dispatch(pRequestBytes);
        break;
#endif /* FEATURE_FTM_HWTC */

      default:
        FTM_MSG_ERROR("Unknown subsys id",header->subsys_id,0,0);
        rsp_pkt.cmd = FTM_RSP_BAD_CMD;
        break;
      } /* Switch */

#ifndef FEATURE_FTM_HWTC
    }

    else
    {
      FTM_MSG_ERROR("Mode = %d, Not in FTM mode, Aborting Command.",ftm_mode,0,0);
      rsp_pkt.cmd = FTM_RSP_BAD_MODE;
    }
 #endif

    FTM_MSG_MED("Done processing FTM packet: %d,%d,%d",
                header->cmd_code,
                header->subsys_id,
                header->subsys_cmd_code);

    /* Set DIAG_ASYNC_BLOCK_SIG */
    //ftmdiag_send_async_pkt (msg_ptr);

    /* process rsp pkt */
    if (rsp_pkt.cmd == FTM_RSP_DO_LEGACY)
    {
      rsp_pkt.pkt_len        = iRequestPacketSize;
      rsp_pkt.pkt_payload    = pRequestBytes;
      rsp_pkt.delete_payload = FALSE;
    }

    pResponseBytes->RspCmd        = rsp_pkt.cmd;
    pResponseBytes->DeletePayload = rsp_pkt.delete_payload;
    pResponseBytes->Pkt_Len       = rsp_pkt.pkt_len;
    pResponseBytes->PayLoadPtr    = rsp_pkt.pkt_payload;

    return _eErrorCode;

} /* End ftm_dispatch */


/*===========================================================================

FUNCTION FTM_REQ

DESCRIPTION
   This function handles requests to run tests and  other primitives
   by dispatching the appropriate functions and or macros.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
LOCAL void ftm_req(void)
{
  ftm_msg_type                   *msg_ptr;          /* Pointer to a ftm msg               */

  FTM_Sequencer_ErrorCode        _eErrorCode = FTM_Sequencer_Error_None;

  /* Get a command from the Queue.
  */
  while ( ( msg_ptr = (ftm_msg_type *) q_get( &ftm_task_cmd_q) ) != NULL)
  {
    ftm_rsp_pkt_type               rsp_pkt = {FTM_RSP_DO_LEGACY, 0, NULL, FALSE};    
#ifdef FEATURE_RF_RFCA
    boolean route_resp_to_rfca = FALSE;
    // Special handling for FTM requests sent from RFCA
    if(FTM_SEQUENCER_SUBSYSTEM_COMMAND_FROM_RFCA == ((byte*)msg_ptr->ftm_data_ptr)[0])
    {  
      route_resp_to_rfca = TRUE;
      ((byte*)msg_ptr->ftm_data_ptr)[0] = FTM_SEQUENCER_SUBSYSTEM_COMMAND;
    }
#endif

    /* update current pkt data information */
    ftm_pkt_data.req_pkt_len  = msg_ptr->req_pkt_len;
    ftm_pkt_data.rsp_pkt_len  = msg_ptr->rsp_pkt_len;
    ftm_pkt_data.ftm_data_ptr = msg_ptr->ftm_data_ptr;
    ftm_pkt_data.data_rsp_ptr = msg_ptr->data_rsp_ptr;
    
    _eErrorCode  = FTM_Sequencer_ProcessClientRequestPacket(0,
                                                            msg_ptr->req_pkt_len,
                                                            msg_ptr->ftm_data_ptr,
                                                            (FTM_Sequencer_ResponsePacket *)&rsp_pkt);
    if(_eErrorCode != FTM_Sequencer_Error_None)
    {
       FTM_MSG_ERROR("FTM_Sequencer_ProcessClientRequestPacket ERROR: %d",_eErrorCode,0,0);
    }

    /* If there is a 'done queue' the queue the command on that queue
    ** as we are done processing it.
    */
    if (msg_ptr->hdr.done_q_ptr != NULL)
    {
      q_put(msg_ptr->hdr.done_q_ptr, &msg_ptr->hdr.link);
    }

#ifdef FEATURE_RF_RFCA
    // Special handling for FTM requests sent from RFCA
    if(TRUE == route_resp_to_rfca)
    {  
      RFCA_ftm_async_cmd_resp(&rsp_pkt);
      if (rsp_pkt.delete_payload && rsp_pkt.pkt_payload != ftm_pkt_data.ftm_data_ptr)
      {
        (void) ftm_free(rsp_pkt.pkt_payload);
        rsp_pkt.pkt_payload = NULL;
      }      
    }
    else
#endif
    {
      (void)ftmdiag_process_async_rsp_pkt (rsp_pkt);
    }

    /* free up the ftm pkt memory */
    ftm_free(msg_ptr->ftm_data_ptr);
    msg_ptr->ftm_data_ptr = NULL;

    /* update current pkt data information */
    ftm_pkt_data.req_pkt_len = 0;
    ftm_pkt_data.rsp_pkt_len = 0;
    ftm_pkt_data.ftm_data_ptr = NULL;

    /* free up the message structure also */
    (void) ftm_free(msg_ptr);
    msg_ptr = NULL;

  } /* While */
} /* End ftm_req */

/*===========================================================================

FUNCTION       FTM_PRINTF

DESCRIPTION
  This function is a printf clone, it will work just as printf does, but
  will output to string to TBD.

DEPENDENCIES
  none.

===========================================================================*/
void ftm_printf(ftm_msg_lvl_type msg_lvl_enum, const char *fmt, ... )
{
  char   *local_printf_buffer = NULL;
  va_list arg_ptr;

  /* Allocate memory for local buffer with extra space because we don't
     yet know how big the string will be once the string formatting is
     done
  */
  local_printf_buffer = (char*) ftm_malloc(strlen(fmt) + strlen(fmt));

  /* check that we haev memory to work with */
  if (local_printf_buffer == NULL) return;

  /* first lets format the and copy the string. */
  va_start( arg_ptr, fmt ); /*lint !e530*/
  (void)vsnprintf(local_printf_buffer, sizeof(local_printf_buffer), fmt, arg_ptr); /*lint !e530 */
  va_end(arg_ptr);

  /* do string output here */
  switch (msg_lvl_enum)
  {
  case FTM_MSG_LVL_FATAL:
    FTM_MSG_FATAL("%s",local_printf_buffer,0,0);
    break;
  case FTM_MSG_LVL_ERROR:
    FTM_MSG_ERROR("%s",local_printf_buffer,0,0);
    break;
  case FTM_MSG_LVL_HIGH:
    FTM_MSG_HIGH("%s",local_printf_buffer,0,0);
    break;
  case FTM_MSG_LVL_MED:
    FTM_MSG_MED("%s",local_printf_buffer,0,0);
    break;
  default:
  case FTM_MSG_LVL_LOW:
    FTM_MSG_LOW("%s",local_printf_buffer,0,0);
    break;
  }

  /* clean up memeory */
  ftm_free ((void*)local_printf_buffer);

} /* End ftm_printf */

/*===========================================================================

FUNCTION FTM_TASK

DESCRIPTION
  This is the Factory Test Mode(FTM) task, created via rex_def_task
  from the Main Control task.  After processing task start procedure,
  the FTM task enters an infiniten loop, awaiting commands received on its
  command queue.  Each command received is processed to completion before
  the next command is processed.  While in the main loop the task kicks the
  watchdog periodically.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
#ifdef FEATURE_RFA_CMI_API
void  rf_test_task
#else
void  ftm_task
#endif
(
  dword              parm       /* from REX - ignored                     */
)
{
  rex_sigs_type sigs;             /* REX signals word                     */

#ifdef FEATURE_LTE
  ftm_rex_msgr_msg_type *rex_msgr_ptr = NULL;
  ftm_rsp_pkt_type      rsp_pkt       = {FTM_RSP_NO_RSP, 0, NULL, FALSE};
#endif /* FEATURE_RF_HAS_LTE */
#ifdef FEATURE_GSM
  ftm_rfgsm_rex_msgr_msg_type *rex_rfgsm_msgr_ptr = NULL;
#endif /* FEATURE_GSM */
/*-------------------------------------------------------------------------*/
  FTM_MSG_MED("FTM task started", 0, 0, 0);

  #ifdef FEATURE_MODEM_RCINIT
  /* Start handshake with RCINIT to start the init process */
  rcinit_handshake_startup();

  /* Initializes command and "free" queues,
     initializes critical section data structure for FTM,
     initializes FTM timers
     */
  ftm_task_init();

  /* Run time synch with other tasks is done here if needed.
     The following two calls are to let any other tasks that are waiting for
     this task to start.
     Currently not needed, but having the code and doing the signaling doesn't
     hurt.
  */
  (void)rcevt_wait_name(RCINIT_RCEVT_INITGROUPS);
  (void)rcevt_signal_name(RF_TEST_RCEVT_READY);

  #else
  /*
  ** Initialize legacy ftm task
  */
  ftm_task_init();

  (void)rex_set_sigs(TASK_HANDLE(ftm), FTM_RPT_TIMER_SIG);

  #endif /* FEATURE_MODEM_RCINIT */
  /* Loop forever handling commands */
  for (;;)
  {
    sigs = rex_wait( FTM_RPT_TIMER_SIG    |     /* Watchdog report timer */
                     FTM_CMD_Q_SIG        |     /* Command Queue signal  */
                     FTM_EVT_Q_SIG        |     /* Event Queue signal    */
                     FTM_SEQ_SIG          |     /* Process FTM SEQ       */

#ifdef FEATURE_GSM
                     FTM_RF_GSM_CMD_Q_SIG |     /* RF GSM MSGR FTM Sig */
                     FTM_GSM_Q_SIG        |
#ifdef FEATURE_FTM_GSM_BER
                     FTM_GSM_BER_CMD_Q_SIG |
#if defined(FEATURE_GSM_EGPRS) && defined(FEATURE_FTM_EGPRS_BER)
                     FTM_EGPRS_BER_CMD_Q_SIG |
#endif

#endif

#endif

#ifdef FTM_HAS_UMTS
#ifdef FEATURE_FTM_WCDMA_BER
                     FTM_WCDMA_BER_CMD_Q_SIG |
                     FTM_WCDMA_BER_INT_CMD_Q_SIG |
#ifdef FEATURE_WCDMA
                     FTM_WCDMA_EXECUTE_TEMP_COMP_SIG |
#endif
#endif
#endif

#ifdef FTM_HAS_TDSCDMA
#ifdef FTM_HAS_TDSCDMA_BER
                     FTM_TDSCDMA_BER_CMD_Q_SIG |
                     FTM_TDSCDMA_BER_INT_CMD_Q_SIG |
#endif /*FTM_HAS_TDSCDMA_BER*/
#endif /*FTM_HAS_TDSCDMA*/
                     #ifdef FEATURE_CDMA1X
                     FTM_ENABLE_PROT_SIG  |     /* FTM Enable protocol   */
                     #endif
#ifdef FEATURE_LTE
                     FTM_LTE_CMD_Q_SIG    |       /* FTM LTE support */
#endif /*FEATURE_FTM_HAS_LTE*/
                     TASK_STOP_SIG               /* Command to shutdown   */
                   );

    if ( sigs & FTM_RPT_TIMER_SIG )
    {
      ftm_kick_watchdog();
    }

    if ( sigs & TASK_STOP_SIG)
    {
      /* Cancel the watchdog report timer. */
      rex_clr_timer(&ftm_rpt_timer);

#ifdef FEATURE_GSM
     ftm_gsm_deinit_rex_msgr_client();
     ftm_gsm_deinit_msgr_client();
#endif /* FEATURE_GSM */
#ifdef FEATURE_LTE
       ftm_deinit(); /* Acknowledge were done */
#endif /* FEATURE_LTE */

       FTM_Sequencer_Destructor(0);

#if defined (FEATURE_LTE) || defined (FEATURE_GSM)
       /* Task is de-initialized */
       ftm_task_info.is_task_running = FALSE;
#endif /* ( FEATURE_LTE || FEATURE_GSM ) */
#ifndef FEATURE_MODEM_RCINIT
       tmc_task_stop();
#else
       rcinit_handshake_term();
#endif /* FEATURE_MODEM_RCINIT */

      (void) rex_clr_sigs(TASK_HANDLE(ftm), TASK_STOP_SIG);

      /* Task is stopped.  No need to run this function anymore. */
      break;
    }

    #ifdef FEATURE_CDMA1X
    if ( sigs & FTM_ENABLE_PROT_SIG)
    {
      (void) rex_clr_sigs( TASK_HANDLE(ftm), FTM_ENABLE_PROT_SIG);

      FTM_MSG_MED("Wait for MC FTM ready signal.",0,0,0);

      #ifdef FEATURE_FACTORY_TESTMODE
      if ( ftm_sig_clk_rex_wait( FTM_MC_READY_SIG, 10000 ) & FTM_WAIT_TIMEOUT )
      {
        FTM_MSG_ERROR("MC is not FTM ready, ftm commands may reboot the phone.",0,0,0);
      }
      else
      {
        (void) rex_clr_sigs( TASK_HANDLE(ftm), FTM_MC_READY_SIG);
        FTM_MSG_MED("Received MC FTM ready signal.",0,0,0);
      }
      #endif /* FEATURE_FACTORY_TESTMODE */
    }
    #endif

#ifdef FEATURE_LTE
   /* This signal is coming from the RFA, L1, L2 tasks, 
        most commonly as a response to a command that FTM sent them earlier.*/
    if ( sigs & FTM_LTE_CMD_Q_SIG)
    {
      FTM_MSG_LOW("FTM REX task Got FTM_LTE_CMD_Q_SIG", 0, 0, 0);
      /* Clear the signal prior to emptying the queue, to avoid the
         race condition where an item is added just as we are leaving.*/
      (void) rex_clr_sigs( TASK_HANDLE(ftm), FTM_LTE_CMD_Q_SIG);
      rex_msgr_ptr = (ftm_rex_msgr_msg_type *)q_get(&ftm_task_lte_q);
      while (rex_msgr_ptr != NULL)
      {
        /*No diag response should be sent back to the PC when LTE L1 confirmations are recieved.*/
        rsp_pkt.cmd = FTM_RSP_NO_RSP;
        rsp_pkt.pkt_len = 0;
        rsp_pkt.delete_payload = TRUE;
        rsp_pkt.pkt_payload = NULL;
        /* Send to LTE common dispatcher dispatcher handling both Calibration and Non-signaling commands. */
        ftm_lte_msg_dispatch( &(rex_msgr_ptr->ftm_msgr_msg), sizeof(ftm_msgr_u), &rsp_pkt );

        /* The if statement is true, if in FTM Sequencer mode with valid response packet. In this case
             access to FTM_Sequencer_ProcessLoopIteration() is unblocked and FTM Sequencer continues
             to process next command from its queue 
             Note:- This applies for both FTM NS and FTM RF list mode testing*/
        if ((rsp_pkt.cmd != FTM_RSP_NO_RSP) && (sigs & FTM_SEQ_SIG))
        {     
          CmdInProgressBlockSequencer = FALSE; 
        
          /* Sends Async response packet if in Sequencer mode */
          FTM_Sequencer_Adaptation_Process_RspPkt(0,&rsp_pkt);

          /* Exits the sequencer mode after processing the response packet, when
                    last command in the sequencer queue happens to be LTE command */
           FTM_Sequencer_Adaptation_Stop_Sequence_Execution(0);
        }
        else
        {
           /* The following check will allow seamless transition from ping-pong to list mode
                  by unlocking FTM_Sequencer_ProcessLoopIteration() before entering list mode */
           if(! (sigs & FTM_SEQ_SIG))
           {
              CmdInProgressBlockSequencer = FALSE; 
           }

          /*send response packet back to Diag in ping-pong mode*/
        (void)ftmdiag_process_async_rsp_pkt (rsp_pkt);
        }

        /* Put back into free queue after processing */
        q_put( &ftm_task_free_q, &(rex_msgr_ptr->link) );
        /* Get next message */
        rex_msgr_ptr = (ftm_rex_msgr_msg_type *)q_get(&ftm_task_lte_q);
      }
    }
#endif /*FEATURE_FTM_HAS_LTE*/

    if ( sigs & FTM_CMD_Q_SIG)
    {
      /* Clear the signal prior to emptying the queue, to avoid the
      ** race condition where an item is added just as we are leaving.
      */
      (void) rex_clr_sigs( TASK_HANDLE(ftm), FTM_CMD_Q_SIG);
      ftm_req();
    }

    if ( sigs & FTM_EVT_Q_SIG)
    {
      (void) rex_clr_sigs( TASK_HANDLE(ftm), FTM_EVT_Q_SIG);
      ftm_process_events();
    }

#ifdef FEATURE_GSM

    if( sigs & FTM_GSM_Q_SIG)
    {
        (void)rex_clr_sigs( TASK_HANDLE(ftm), FTM_GSM_Q_SIG);
        #ifdef FTM_HAS_LOGGING
        rfgsm_core_log_handle_q_from_rf();
        #endif /* FTM_HAS_LOGGING */
    }
#ifdef FEATURE_FTM_GSM_BER
    if( sigs & FTM_GSM_BER_CMD_Q_SIG)
    {
        gs_read_messages( GS_QUEUE_FTM_GSM);   //rex sig cleared in gs services!
        ftm_gsm_ber_handle_rx_data_indications();
    }
#if defined(FEATURE_GSM_EGPRS) && defined(FEATURE_FTM_EGPRS_BER)
    if( sigs & FTM_EGPRS_BER_CMD_Q_SIG)
    {
        (void)rex_clr_sigs( TASK_HANDLE(ftm), FTM_EGPRS_BER_CMD_Q_SIG);
        ftm_egprs_ber_handle_msgs_from_gpl1();
    }
#endif /* defined(FEATURE_GSM_EGPRS) && defined(FEATURE_FTM_EGPRS_BER) */

#endif /* FEATURE_FTM_GSM_BER */
    if (sigs & FTM_RF_GSM_CMD_Q_SIG)
    {
      FTM_MSG_LOW("FTM REX task got FTM_RF_GSM_CMD_Q_SIG", 0, 0, 0);

      /* Clear the signal prior to emptying the queue, to avoid the
         race condition where an item is added just as we are leaving.*/
      rex_clr_sigs( TASK_HANDLE(ftm), FTM_RF_GSM_CMD_Q_SIG );
      rex_rfgsm_msgr_ptr = (ftm_rfgsm_rex_msgr_msg_type *)q_get(&ftm_task_rfgsm_q);

       while (rex_rfgsm_msgr_ptr != NULL)
      {
        /* No diag response should be sent back to the PC when RF GSM responses are received. */

        /* Send to FTM GSM common dispatcher for handling RSP messages from RF task */
        ftm_gsm_msgr_dispatch_response_handler( &(rex_rfgsm_msgr_ptr->rfgsm_ftm_msgr_msg), sizeof(rfgsm_ftm_msg_u));

        /* Put back into free queue after processing */
        q_put( &ftm_task_rfgsm_free_q, &(rex_rfgsm_msgr_ptr->link) );

        /* Get next message */
        rex_rfgsm_msgr_ptr = (ftm_rfgsm_rex_msgr_msg_type *)q_get(&ftm_task_rfgsm_q);

      }


    }
#endif /* FEATURE_GSM */

#ifdef FTM_HAS_UMTS
#ifdef FEATURE_FTM_WCDMA_BER
    if( sigs & FTM_WCDMA_BER_CMD_Q_SIG)
    {
        (void)rex_clr_sigs( TASK_HANDLE(ftm), FTM_WCDMA_BER_CMD_Q_SIG);

        /* Notify rf_task if cmd is stop_wcdma_mode
           This is needed for wcdma_exit to enter LTE as LTE runs in rf_task
        */
        if(ftm_wcdma_ber_is_rrc_cmd_stop_wcdma_mode())
        {
           ftm_wcdma_notify_rf_task();
        }

        ftm_wcdma_ber_process_msgs();
    }
    if( sigs & FTM_WCDMA_BER_INT_CMD_Q_SIG)
    {
        (void)rex_clr_sigs( TASK_HANDLE(ftm), FTM_WCDMA_BER_INT_CMD_Q_SIG);
        ftm_wcdma_ber_process_int_msgs();
    }
#endif

#ifdef FEATURE_WCDMA
    if( sigs & FTM_WCDMA_EXECUTE_TEMP_COMP_SIG )
    {
      /* Clear the signal */
      (void)rex_clr_sigs( TASK_HANDLE(ftm), FTM_WCDMA_EXECUTE_TEMP_COMP_SIG );

      /* Set the timer = 10 sec for temp comp */
      (void)rex_set_timer(&ftm_wcdma_execute_temp_comp_timer, FTM_WCDMA_TEMP_COMP_FREQUENCY_MS);

      /* Call temp comp */
      ftm_wcdma_execute_temp_comp();
    }
#endif

#endif

#ifdef FTM_HAS_TDSCDMA
#ifdef FTM_HAS_TDSCDMA_BER
        if( sigs & FTM_TDSCDMA_BER_CMD_Q_SIG)
        {
            (void)rex_clr_sigs( TASK_HANDLE(ftm), FTM_TDSCDMA_BER_CMD_Q_SIG);
            ftm_tdscdma_ber_process_msgs();
        }
        if( sigs & FTM_TDSCDMA_BER_INT_CMD_Q_SIG)
        {
            (void)rex_clr_sigs( TASK_HANDLE(ftm), FTM_TDSCDMA_BER_INT_CMD_Q_SIG);
            ftm_tdscdma_ber_process_int_msgs();
        }
#endif /*FTM_HAS_TDSCDMA_BER*/
#endif /*FTM_HAS_TDSCDMA*/

    if( (sigs & FTM_SEQ_SIG) && (!CmdInProgressBlockSequencer))
    {
      (void) FTM_Sequencer_ProcessLoopIteration(0);
    }
  } /* for (;;) */
} /* ftm_task */

#if defined (FEATURE_LTE) || defined (FEATURE_GSM)
boolean ftm_is_task_running ( void )
{
  return ftm_task_info.is_task_running; 
}
#endif /* (FEATURE_LTE) || (FEATURE_GSM) */

#ifdef FEATURE_LTE
/*===========================================================================

  FUNCTION:  ftm_msgr_deregister

===========================================================================*/
/*!
  @brief
  De-Registers FTM with the message router

  @return
  
*/
/*=========================================================================*/
static void ftm_msgr_deregister
( 
  void
)
{
}

/*===========================================================================

  FUNCTION:  ftm_deinit

===========================================================================*/
/*!
  @brief
  FTM Tear down

  @return
  None.
*/
/*=========================================================================*/
static void ftm_deinit(void)
{
  // De-Register for all UMIDs the FTM LTE non-signaling module defines
  (void)ftm_lte_ns_msgr_deregister();
  // De-Register for all UMIDs the RF LTE FTM Calibration module defines
  (void)rf_lte_ftm_msgr_deregister(&(ftm_task_info.ftm_msgr_client));
  
  ftm_msgr_deregister();

  /* Release all mailboxes associated with FTM client */
  if (msgr_client_delete(&(ftm_task_info.ftm_msgr_client)) != E_SUCCESS)
  {
    FTM_MSG_FATAL("Mailboxes release associated with FTM client failed!",0,0,0);
  }

}
#endif /* FEATURE_LTE */

#ifdef FEATURE_GSM
static void ftm_gsm_deinit_rex_msgr_client( void )
{

  errno_enum_type msgr_error;

  /* Deregister all RF Task RF GSM message FTM Rex task has registered for. */
  msgr_error = msgr_deregister_block( MSGR_FTM_GSM,
                                      &ftm_rfgsm_rex_msgr_client,
                                      rfgsm_rex_ftm_umid_cmd_list,
                                      RF_GSM_REX_FTM_MSG_CNT);

  if( msgr_error != E_SUCCESS)
  {
     FTM_MSG_ERROR("Unable to deregister Rex GSM FTM MSGR register block.",0,0,0);  
  };

  /* Release mailbox associated with REX GSM FTM_MSGR client */
  if (msgr_client_delete( &ftm_rfgsm_rex_msgr_client ) != E_SUCCESS)
  {
	FTM_MSG_FATAL("Mailboxes release associated with REX GSM FTM_MSGR client failed!",0,0,0);
  }
}

static void ftm_gsm_deinit_msgr_client( void )
{

  errno_enum_type msgr_error;

  msgr_error = msgr_deregister_block( MSGR_FTM_GSM,
                                      &ftm_rfgsm_msgr_client,
                                      rfgsm_ftm_umid_cmd_list,
                                      RF_GSM_FTM_MSG_CNT);

  if( msgr_error != E_SUCCESS)
  {
     FTM_MSG_ERROR("Unable to deregister GSM FTM MSGR register block.",0,0,0);  
  };

  /* Release mailboxes associated with FTM_GSM client */
  if (msgr_client_delete( &ftm_rfgsm_msgr_client ) != E_SUCCESS)
  {
	FTM_MSG_FATAL("Mailboxes release associated with FTM_GSM client failed!",0,0,0);
  }
}
#endif /* FEATURE_GSM */

#if defined(FEATURE_L4) || defined(FEATURE_BLAST)
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set FTM Task Priority

  @details
  This function will set the FTM Task Priority.

  @param new_pri
  Proposed new Task Priority
 
  @return
  Success/Failure in setting FTM Task Priority.
*/
boolean
ftm_set_task_priority
(
  rex_priority_type new_pri       /* the new priority */
)
{
  boolean ret_val = FALSE; /* Return Value */

  /* This function should only be called under FTM Task Context */
  ASSERT ( rex_self () == TASK_HANDLE(ftm) );

  if ( new_pri <= AMSS_MAX_TASK_PRI )
  {
    /* Change the Priority */
    (void)rex_set_pri( new_pri );
    
    /* Flag the Reurn Value to be success */
    ret_val = TRUE;
  }

  return ret_val; /* return */

} /* ftm_set_task_priority */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get FTM Task Priority

  @details
  This function will query the FTM Task Priority.

  @return
  Present Task Priority
*/
rex_priority_type
ftm_get_task_priority
(
  void
)
{
  rex_priority_type present_pri; /* Present Priority */

  /* This function should only be called under FTM Task Context */
  ASSERT ( rex_self () == TASK_HANDLE(ftm) );

  /* Query the Priority */
  present_pri = rex_get_pri();

  return present_pri;

} /* ftm_get_task_priority */
#endif /* FEATURE_L4 */

#endif /* FEATURE_FACTORY_TESTMODE */

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        M A I N    U I M     T A S K

                  U S E R   I D E N T I T Y   M O D U L E

GENERAL DESCRIPTION
  This module is the server for all the commands to the User Identity Module
  in all the different modes of operation. The different modes being CDMA
  R-UIM, GSM SIM and WCDMA USIM. This module also does power management of
  the UIM to save battery power.

EXTERNALIZED FUNCTIONS

  uim_power_control
    Voting interface for power management of the UIM.

  uim_cmd
    External interface to queue a command for the UIM task.

  uim_task
    This is the entrance procedure for the UIM  task.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2001-2016 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimdrv/src/uim.c#1 $
$DateTime: 2016/12/13 08:00:05 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/21/16   gm      F3 Optimization
06/20/16   ks      Resetting the command transacted flag if the 
                   response is not received from card
06/07/16   gm      F3 optimization
06/06/16   dd      F3 Optimization on curr path
06/01/16   gm      Fix KW warning 
05/27/16   ks      Check for command transacted flag when the internal 
                   open channel command fails
05/23/16   na      F3 Optimization
05/18/16   ssr     Remove msg for malloc failure
05/12/16   dd      remove F3 msg
05/10/16   gm      Handling BTSAP connection unavailable signal
05/10/16   sam     Remove the logic to store timeout info in EFS
05/09/16   ks      Triggering extended recovery/new powerup on ICCID mismatch
05/09/16   sam     Added function to queue an internal open channel command
04/11/16   ks      Code changes needed for LPA support
12/03/15   ssr     Added MCFG API to query the power down reason
01/25/16   ssr     Delay local card power up in Modem SSR in BTSAP mode
12/23/15   sam     Fix for KW errors
12/09/15   hyo     Moving to QTF Dal implementation
12/01/15   ll      Removing compile time disable polling feature flag  
11/26/15   sam     Remove Redundant F3
11/05/15   ks      Rel12 polling enhancement
10/16/15   vr      Support to check the pending proactive command on card
09/15/15   sam     Explicit selection of ADF before Compute IP command if the 
                   current directory is anything other  than (3F00 7FFF)
09/08/15   ll      UIMDRV migrates to UIM COMMON EFS APIs for all EFS items
08/10/15   ks      Allow power_down_error notification when previous card error 
                   notification was CARD_REMOVED
07/21/15   ssr     Fix code to add the AID len on status request
07/17/15   ks      Prioritizing explicit MF selection than the POLL command
07/17/15   ks      Allow UIM to get the DF name in status request when the last
                   selected ef is child of ADF.
07/15/15   ks      Consolidating the F3s that print the directory path
06/29/15   na      Masking of interrupts before notifying an error of current command
06/16/15   ks      Selecting MF after 'terminate session - slect' on channel0
05/21/15   lm      UT framework changes
05/13/15   ks      Disabling SFI for the SEEK operation
05/13/15   sam     Clear channel info for extended recovery
05/04/15   sam     check if current directory is ADF, for terminate app status cmd
04/27/15   vr      Support to check the pending proactive command on card
03/24/15   ks      MF selection upon two consective technical problem errors
02/28/15   ks      Explicit MF selection logic revisited
02/16/15   sam     Fix for KW errors
02/03/15   xj      Add USB UICC and SAP CLIENT supported interface
01/23/15   ks      Reselect MF after Store ESN commands
12/15/14   ks      Invoking force recovery instead of setting timeout signal
12/09/14   na      Clearing of response buffer upon receipt of command
11/24/14   lm      UT framework changes for uim driver task init
11/20/14   na      Re setting of UIM_POLL_TIMER_SIG
11/17/14   nmb     Remove dead directory maintenance code
11/12/14   js      Fixed compiler warning
11/10/14   akv     Support for user to customize LDO settings
10/28/14   nmb     Clear current path before updating icc path
10/27/14   ll      KW
10/10/14   ssr     Optimization to move selects CDMA DF before telecom access
                   logic from MMGSDI to uimdrv
10/10/14   ll      Do not register for MCFG if slots are disabled
10/07/14   sam     perform ADF selection before pin/chv operation only
                   if the current path is not under ADF.
10/01/14   ll      KW errors
09/29/14   nmb     Break when intermediate sel function is found
09/22/14   akv     Code optimization
09/18/14   akv     UIMDRV UT enhancements
09/18/14   ks      Fixing the issue with intermediate selection of AID
09/10/14   ks      Allowing AUTH command even though the 7FFF selection fails
09/05/14   sam     Set command_requeued flag while requeueing RESET_F command
08/27/14   sam     Adding support to trigger recovery on reciept of bad status word
08/18/14   sam     Clearing TP cache on link establishment
08/14/14   ks      Hotswap enhancement for flaky mechanical switches
08/14/14   sam     Support for extended recovery
08/12/14   ak      Feature flags cleanup
08/12/14   ks      Updating UIM status while notifying POLL error
08/06/14   sam     updating current_path in respose of SELECT_FAILED_ST
08/05/14   lxu     deal with terminal profile command which has DF field
08/04/14   ll      Fix compile warnings
07/25/14   na      Allowing back to back silent recovery request
07/18/14   ks      AT commands over streaming APDU on default channel fixes
07/16/14   ssr     Fix to select the ADF before performing Pin operation
07/15/14   ll      NV refresh upon hotswap feature
07/15/14   ks      Update the task stop signal handling
07/11/14   sam     Donot perform intermediate  select for any command when UIM
                   is in passive mode(sap server mode)
07/04/14   sam     Workaround for buggy cards to select MF on channel 0 on
                   failure of AID selection on non-zero channel
07/03/14   ssr     Fix for 0x6FXX status word handling for read and select
                   request
06/25/14   ks      Resetting the static cmd count before internal select
06/20/14   sam     Clearing command_in_progress and static_buffer_used flags
                   on recieving UIM_ICCID_MISMATCH
06/12/14   ll      KW errors
06/11/14   ll      Add interruptible time delay and return value for update params
05/30/14   akv     UART_DM register logging
05/23/14   ks      Enabling software command response timer
05/14/14   na      Fix of compilation for offtarget
04/29/14   nmb     Re-enable AT+CRSM after directory maintenance changes
04/29/14   ks      FIX to handle select CDMA DF before TP appropriately
04/24/14   lxu     Fix wrong WWT of usb uicc given to link_est_data and
                   HOTSWAP_CARD_INS_F issue
04/21/14   ll      Listen to task stop sig during during pet and wait
04/15/14   ks      Reverting the changes of CR556030 that introduced sw wwr
04/11/14   na      Added support for SWP NV item to encode SWP in Terminal
                   Capability
04/08/14   lxu     Fix ISO card try to turn off USB ICCD interface
04/06/14   ks      Introducing software wwt running parallel to h/w wwt
04/01/14   av      NVRUIM headers cleanup
03/31/14   ll      Change stop at infinite NULL flag name for readability
03/31/14   lxu     Fix USB UICC recovery, hotswap and iso card issue
03/28/14   kb      Update function assert_on_crit_sect_owner
03/27/14   lxu     USB UICC modem implement
03/27/14   ll      SIM BUSY log reduction and remove dog rpt print
03/21/14   ll      Remove return_ptr and remove/change uim_free to UIM_FREE
03/20/14   sam     Fix to pet dog during power up wait/delay
03/11/14   kb      Added TASK_EXIT_SIG to synchronize exiting of all tasks
03/11/14   nmb     Correct intermediate selects for streaming APDUs
03/07/14   ks      Explicit MF/ADF selection changes due to new
                   directory maintainence
03/06/14   nmb     Directory maintenance I&T bug fixes
03/03/14   akv     Bip session in progress made slot specific
02/28/14   akv     Notifying correct error code during pup if card is absent
02/27/14   nmb     Remove debug code that had stability vulnerabilities
02/20/14   ks      Provision for explicit MF or ADF selction
                   And provision for inverse convention bootup
02/20/14   ak      Fix compile errors due to strict compiler on 9x35
02/15/14   sam     Usage of newer style diag macros
02/08/14   sam     Fix for Klocwork issues
02/07/14   akv     Recover within recovery for timeouts after ATR and PPS
01/29/14   kb      Disable function assert_on_crit_sect_owner
01/29/14   sj      ASSERT on Memoryleaks in UIMDRV
02/02/14   nmb     SFI changes
01/31/14   ak      Safety checks for disabled UIM instances
01/29/14   akv     RX ISR refactoring changes
02/27/14   na      Fix for session termination issue
01/27/14   ak      [dev branch] fixed qtf crash due to return from uim_task
01/14/14   ak      Uim task signal handling re-factor/cleanup
01/14/14   nmb     Move MANAGE CHANNEL command to generic cmd processing
01/14/14   nmb     Remove ACCESS_F and related unused functionality
01/09/14   ll/ak   Fix proactive polling interval value never set for slot3
01/07/14   akv/ak  Polling uim only at the expiry of the polling timer
12/21/13   am/ak   Silent Recover Feature Implementation
12/18/13   ll      Old NULL Byte timer logic clean up for SIM BUSY feature
12/12/13   ks      BTSAP in client mode, intial implementation
12/11/13   vv      Added support for the EF-PSISMSC in DF TELECOM
12/10/13   nmb     Fix for uimdrv testcase failures for QTFCRM
11/21/13   na      Fix for testcase failure for QTFCRM
11/19/13   nmb     Explicitly start from MF or ADF for proprietary files
11/19/13   ll      Clearing bad status word flag after sending error report
11/18/13   ll      Add disable poll to init_nv_globals_with_read_values_from_efs
11/13/13   js      MOB workaround for task stop processing.
11/11/13   js      Updated handling of task stop signal
10/28/13   kb      Add offtarget support to check mutex is owned or not
10/24/13   akv     Removed unnecessary feature flags
10/22/13   ll      Add UIM BUSY Response feature
10/22/13   ll      Check UIM GPIOs as inputs upon timeout w/recovery disabled
10/21/13   sam     Removed include file virtual_uimdrv.h -> file moved to
                   offtarget location
10/16/13   akv     Fixed KW errors
10/08/13   js      Prevent null ptr dereference during recovery
10/07/13   ll      Disable polling for internal power measurement
10/03/13   ks      F3 log reduction
                   Replaced UIM_FLUSH_FOR_SLOT with UIM_FLUSH_ALL
10/03/13   sam     SW1 SW2 check before reporting error to MMGSDI or GSTK
10/03/13   sam     Not retrying authentication command on recovery
10/01/13   vv      Added support for the EF-PWS
10/01/13   ak      Remove unnecessary feature flags
09/30/13   akv     Removal of if-else cases and unused code in hw enumeration
09/30/13   akv     Removal of multiple efs reads performed by uim tasks
09/27/13   ssr     Fixed to reset cmd count to 0 before start recovery
09/26/13   yk      Clean up signals
09/23/13   vr      qtf-crm MOB migration
09/24/13   nmb     Remove extra uim_free param
09/23/13   nmb     Remove GET_CURR_INSTANCE
09/20/13   ak      Hotswap debug message cleanup
09/20/13   js      uim_cmd() should return error status
09/20/13   js      Fix to accept commands after external RESET time out at 3 v
09/19/13   akv     Reference CLK changes to support 4.8MHz
09/16/13   yk      Removed the code for old targets and for old features
09/12/13   js      F3 clean up
09/11/13   akv     Fix to trigger recovery upon timeout during Interanl Fetch
09/11/13   sam     API to enable disabling of uim recovery during run time
09/10/13   na      Fix to handle Poll failure due to 6F 00 status words
08/12/13   ssr     Fix to clear response signal and timer before processing
                   new command request
08/21/13   hyo/js  Hotswap support on MOB
08/21/13   sam     Power down UIM on technical problems through NV control
08/21/13   na      Fix to prevent the resetting of the Poll Timer in middle
08/20/13   ssr     Set the channel information to channel 0 during card reset
08/20/13   na      Added Defensive code to block link establishment for error
                   state
08/19/13   akv     Changes in hotswap_init to read/write UIM_HOTSWAP_NV_EF
08/15/13   nmb     Re-porting Disabling caching of files due to merge error
08/14/13   ks      More time proactive command handling
08/13/31   js      UIMDRV Memory De-Couple changes
08/08/13   ks      Advanced Subscription Manager changes
08/05/13   js      Fix for hotswap card detection at power up
07/31/13   akv     Avoid resetting globals for UIM_POWER_DOWN_CMD_NOTIFICATION_S
07/26/13   sam     Setting imask signal to process new command in the queue,
                   before powering down UIM in case of NULL byte scenario
07/25/13   akv     Resetting necessary UIM globals after we notify our clients
                   of an error in uim_notify_error.
07/24/13   akv     Avoid double reporting while handling UIM_TRANSACTION_SIG
07/23/13   na      Fix for double pointer free for Hotswap
07/19/13   nmb     Avoid duplicating streaming apdu commands on prop. apps
07/15/13   ll      Remove uim_internal_cmd_buf_used for checking external command
                   except in uim_process_cmd_err
07/15/13   na      Introduction of new NULL_BYTES card error type
07/12/13   nmb     Global Reorganization
07/09/13   js      Fix for extra modem wake up while UE is camped on CDMA.
07/04/13   na      NULL Byte timer implementation
07/02/13   kb      Added support for memory leak detection
07/01/13   ks      stack memory optimization
06/27/13   spo     Added support for files under DF TELECOM
06/25/13   akv     Disabling caching of files
06/24/13   js      TSTS changes
06/20/13   akv     Clock activation when we process UIM_CMD_Q sig to avoid ahb
                   bus hang due to concurrency issues between UIM/GSDI/GSTK.
05/30/13   akv     Heart beat mechanism for Watchdog
05/28/13   rm      Time intervals of status commands exceeding polling period
05/24/13   abd     Featurize the code to poll in idle mode
05/21/13   akv     RESET_F command response timeout handling and reporting to
                   GSDI upon timeout in RESET_F command
05/16/13   na      Secure memory copy
05/09/13   nmb     Don't execute off target code on target
05/03/13   akv     Attempt to power up UIM only if the UIM instance is enabled
04/17/13   tl      Replace Featurization with NV
03/25/13   na      Call Disconnect event should not be sent to card during
                   proactive command is in progress.
03/21/13   ak      Notify GSTK when an error is returned for a FETCH cmd
03/19/13   na      Disable JCDMA service for multisim
03/15/13   js      Fix for double pointer free
03/13/13   js      UIM should not attempt recovery when external RESET times
                   out on card response and no card is present.
02/22/13   js      8x26 bring up changes
02/19/13   av      Merge jcdma conf file contents to uimdrv.conf
02/12/13   js      1) Fix for uim_fetch() where slot was hardcode to UIM_SLOT_1
                   2) General F3 macro cleanup
02/11/13   akv     UIM API for querying the available/enabled UIM slots
02/08/13   js      Updated UIM USB code for parallel processing
02/06/13   js      Fix issue with dangling ptr while re-select MF
                   after SW1 warnings
02/06/13   js      UIM need not try envelope command if it gets failure while
                   processing. Envelope retry logic is built in GSTK.
02/05/13   na      Not to retry SEARCH command on failure
02/04/13   akv     Fix for recovery amidst any pin related command
01/31/13   na      Select ADF before ADF termination for specific buggy card
                   scenario
01/28/13   js      Removing UIM Subscription Manager Code
01/24/13   akv     ICCID Cached and sent along with link est data to GSDI
01/22/13   kb      UIM will receive 0x6E00 and error status, at this time UIM
                   still send STATUS command
01/17/13   js      Fixed APDU logging.
12/17/12   js      Fixed KW error
12/12/12   av      Replace FEATURE_UIM_DEPRECATE_TMC with FEATURE_MODEM_RCINIT
12/05/12   js      UIM Parallel Processing changes
11/07/12   akv     Task stop and RCINIT registration changes for Dime
11/06/12   ms      Reducing the retrial command failure attempts to 2 from 3
                   for non-powering up commands
10/26/12   akv     HW Enumeration changes (without features)
10/26/12   akv     HW Enumeration changes
10/21/12   av      Update conf file only if necessary
10/19/12   ssr     Fix Stream APDU ST check for Generic Command only
10/13/12   ssr     Delay the card status query after unmasking the interrupt
09/28/12   ms      Setting imask for UIM_CMD_Q_SIG while handling max technical
                   error count
10/05/12   yt      Register with RCInit framework for termination signal
10/05/12   rm      Correction to pass the correct driver slot
09/26/12   av      Make UIM_MAX_NUM_NULL_PROC_BYTES count run time configurable
09/26/12   js      Dual SIM power up fixes
09/25/12   ms      Fix to reset me_power_up flag for no card scenario
09/25/12   ms      Fix to send the error response of current and pending command
                   before doing the power down for UIM_TRANSACTION_SIG.
09/21/12   js      Klocwork error fixed
09/21/12   ms      Added code for error handling during recovery
09/14/12   js      Fix for sending report for current command in progress
                   upon hotswap card removal.
09/06/12   js      Fix for sending UIM error rpt when SW 6F 00 is observed 3
                   times consecutively
08/31/12   ms      Pass correct slot information to mmgsdi in case of
                   FEATURE_UIM2 only enabled
08/28/12   js      Initial changes for UT framework
08/26/12   ssr     Moving Hotswap timer_def from timer expire cb to hotswap init
08/09/12   js/rm   Fixes for UIM controller hotswap and hardware WWT features
07/26/12   nmb     Fix to return the 6F 00 when not retrying the same command
06/12/12   nmb     Fix to initialize when Hotswap NV is activated
05/26/12   bcho    Power down the UIM task on reception of task stop sig if
                   TEST_FRAMEWORK feature is enabled.
05/21/12   nmb     DS Subscription Manager Changes
05/21/12   nmb     Initialize hotswap in the case of Dual SIM
05/02/12   av      Replaced FEATURE_UIM_SUPPORT_HOTSWAP with an nvitem
05/02/12   ak      Fixes for Dual SIM
04/27/12   yb      Fix to not update technical problem count in case of
                   cached select command
04/19/12   yb/rm   Fix to allow processing of commands after recovery count
                   reaches maximum
04/09/12   yt      Allow opening of non-prov session in ICC mode
04/07/12   ssr     Fixed card detection issue for buggy card which does not
                   send response in UICC mode
03/30/12   av      Remove feature flags with NVITEM
03/30/12   ssr     Empty AID selection support
03/20/12   rm      Hardware work waiting time changes
03/22/12   ms      Fixed on recovery getting stuck due to requeue_counter
                   overflow
03/15/12   rm      Changes for DIME
03/08/12   rm      Fix to avoid setting a signal on uim_tcb in error state
02/21/12   rm      Setting card detection polarity based on NV item
03/02/12   av      Removed feature FEATURE_MULTIMODE_ARCH
02/28/12   rm/ak   Fix for a crash issue during hotswap
02/28/12   ak      Fix a condition in which a command is free'd by GSDI before
                   it is requeued due to command response timeout.
02/18/12   av      Cleaned up remaining ALWAYS OFF/ON feature flags
02/06/12   av      Removed Always ON 1x feature flags
02/03/12   js      Reverting UART DM fix
02/01/12   yt      Bringing back change to non-deferrable timers
01/25/12   ms      Fix for KW warning errors
01/20/12   js      Changes needed for 8930 UIM bring up
01/20/12   ak      Platform ID/NV item logic for hotswap polarity
01/19/12   ssr     Fixed compilation error
01/11/12   rm      Fix to handle 6F C2 as a successful response
01/16/12   nmb     Removal of ALWAYS ON FEATURE_AUTH
01/06/12   ssr     Added delay before doing clock stop and after clock start
01/06/12   ms      Fix for polling happening in less than 28 sec after last command
01/04/12   ssr     Fix to prevent the RESET Command in card removed state
12/30/11   ssr     Manage channel for ICC mode
                   1) Fixed manage channel command response for empty response
                      and P2 data, where P2 contains the channel number
                   2) We should not allow Manage channel command in ICC mode
                   3) Reset the channel number to default channel whenever,
                      UIM is resetting the protocol to the ICC mode
12/30/11   shr     Removed featurization for ALWAYS ON features
12/12/11   ms      Enter into recovery in case of braf_values equal to zero
                   to avoid divide by 0 crash
12/23/11   ssr     Prevent the Status comparison for empty response data
12/21/11   ms      Fix added for Legacy GSDI Removal. Added template to support
                   addition of MMGSDI API interface
12/21/11   shr     Moved NVRUIM initization trigger to MMGSDI task
                   initialization
12/20/11   bcho    Added support for Get Operator Name API
12/20/11   yb      Adding new error code for technical problem
12/20/11   yb      Corrected initialization of data
11/30/11   ssr     Provide a new API to check the hotswap capability of given
                   slot
11/30/11   js      Revert support for file CPHS Get Operator Name Short String
11/28/11   js      Removed usage of sleep APIs
11/22/11   js      Add support for file CPHS Get Operator Name Short String
11/22/11   ssr     Revert the Power down fix of ¡®6F 00¡¯/¡¯6E 00¡¯case.
11/15/11   nmb     RPM Updates
11/14/11   js      Compiler Warning
11/11/11   av      Fixed a rex timer dog timeout issue
11/09/11   js      Fixed KW warning
11/09/11   yb      Fix for crash while processing card power down when no card
                   is present in dual slot and single slot scenario
11/3/11    ms      Fix to use small stale timeout value for default baud-rate
10/28/11   ssr     Requeue the Reset command to continue the Power up after
                   the Invalid INS or voltage switch
10/20/11   yt      Added support for RCInit
10/18/11   av      Removed features that are always OFF
10/17/11   vv      Fix for UT
10/13/11   ssr     Initializing the recovery parameter in uim_hotswap_reset_slot fun
10/13/11   ssr     Fixed compilation error
10/12/11   ssr     Power down the UIM in cases of ¡®6F 00¡¯/¡¯6E 00¡¯ receive
                   continuously from the card in normal mode.
10/07/11   rm      Fixes for T=1 protocol
10/04/11   yk      Change deferrable timers to non-deferrable timers
10/04/11   ak      Replace banned std_strlcat with strlcat
09/30/11   ssr     Removed uim_notify_sys_mode api
09/30/11   ssr     Rearranging the UIM_CONF_FILE enum
09/30/11   ssr     Added CMD pointer check before sending the error report
                   of current command
09/26/11   rm      Fix to bring the UART DM to direct convention upon a hot-swap
09/16/11   ssr     Fix the RESET command to reset the protocol and voltage
                   setting before UIM power up
09/16/11   ms      P1 UIM Fuzzing:Fix added to handle commands after file
                   access error report
09/15/11   nb      Reverting changes to block polling
09/14/11   ms      Fix added for UE sending AUTH to the card twice/thrice when
                   it  receives an error
09/05/11   ms      Added fix for overflow of efs logging buffer in case of DSDS
09/01/11   ak      GPIO hotswap support for 9x15
08/29/11   ssr     Fixed compilation warnings
08/24/11   ssr     Fixed NV Item support for hotswap configuration
08/22/11   yb      Fixed crash on card removal
                   Fix for not reporting response for UIM_RESET_F to MMGSDI if
                   recovery gets triggered during Refresh RESET.
                   Fix to report GSDI for reset command before entering recovery
                   Added support for complete recovery solution
08/19/11   ak      Un-featurize the call to uim_dev_init for 9x15 bring-up
08/16/11   ssr     Fix to not flush the pending commands on slot 2 during the
                   processing of slot 1 commands
                   Update the code to send the error report when card is in
                   removed state
08/16/11   ssr     Fixes for crashes during hot-swap after reducing the debounce
                   time interval used for querying.
                   Revert the IMASK changes and added more messages,
                   Update cmd_ptr while processing Hot Swap Events so that old
                   cmd_ptr is not re-used,
                   Re-queue UIM command on first slot will have higher priority
                   compare to Hotswap command on another slot
                   Do not process any command on another slot while processing
                   the hotswap command on first slot
08/09/11   ak      Merge "reset poll timer after Refresh Reset"
07/05/11   ak      Use accessor functions instead of referencing TCB members directly
07/18/11   rm      Catch-up with latest on Hot-swap
06/27/11   ssr     Re-instate command processing after failed command
06/17/11   ssr     Update the IMASK after card inserted and card removed evt
06/13/11   ssr     Optimize UIM notification code for card removed,
                   Send NO_ATR_RECEIVED_WITH_MAX_VOLTAGE during power up
                   without slot2 card,
                   Fixed Polling and Card removed signal in UIM task, where
                   polling is updating the slot information of static buffer,
                   due to this sometime we got the card removed notification
                   for Slot1.
06/06/11   ssr     Update the code to do the card insertion based on the
                   NV_UIM_FIRST_INST_CLASS_I NV item,
                   Remove Unnecessary power down for card inserted scenario
04/23/11   ssr     upmerge for hot-swap feature
07/05/11   ak      Use accessor functions instead of referencing TCB members directly
06/10/11   ssr     Never flush pending power down and reset
05/23/11   ssr     Fixed for QCN backup support for JCDMA
05/16/11   ssr     Added UIM thread safe support, removed uim_gsdi_event_cb_func
05/14/11   yb      Fixes to remove INTLOCK
04/29/11   nk      Remove use of deprecated api tmc_get_stored_esn_me()
04/27/11   yt      Included private GSDI header
04/26/11   yt      Replaced references to TMC heap with Modem heap
04/25/11   yb      Adding support for hotswap
04/22/11   yk      Fix for a card issue leading to card error during STATUS
                   command
02/28/11   ssr     Fixed Lint errors
02/03/11   ssr     Porting back FEATURE_UIM_JCDMA_RUIM_SUPPORT decouple changes
02/03/11   ssr     Removing FEATURE_UIM_JCDMA_RUIM_SUPPORT decouple changes
02/03/11   yb      Fixed crash on removing SIM from slot 2
01/31/11   ssr     Decoupled FEATURE_UIM_JCDMA_RUIM_SUPPORT feature
01/28/11   yb      Fix for compilation error
01/17/11   yb      Fix to acknowledge for TASK_STOP when
                   FEATURE_UIM_INTERFACE_NOT_USED is defined
01/07/11   ps      Fix to resolve a compile issue
01/04/11   rm      Fix to resolve a linker error
12/31/10   rm      Fix to remove TASKLOCK() and TASKFREE()
12/22/10   yt      Fixed linker error
12/17/10   ps      UIM memory reduction changes
12/14/10   ps      DEEPSEC: P1 UIM Fuzzing: UIM server stuck in reselecting  MF
11/30/10   yt      Including private GSTK header
11/25/10   yb      Removed unused private files
11/15/10   ps      Clearing transaction timer before setting CMD_RSP_SIG to
                   avoid transaction timer overwrite between ISR and TASK context
11/12/10   rm      Handle UIM_POWER_DOWN_F irrespective of uim mode
11/10/10   yt      Memory reduction changes
11/03/10   yb      Fixed compilation warning
10/27/10   yb      Fix for infinite null procedure byte issue
10/22/10   js      Make uim to malloc on modem heap always
10/19/10   vs      Fixed memory leak in uim_gsdi_link_est
10/11/10   yb      Fixed klockwork errors
09/24/10   yb      Fixed compilation error
09/21/10   ms      Added check to make sure that command is not posted to
                   the UIM queue when tcb is not active
09/17/10   yk      Added support dynamic registration on WDOG.
09/14/10   ms      Fixes for new UT Framework
09/10/10   ps      Merge UIM Driver DSDS changes
09/08/10   nb      Added API to start and stop card polling
08/16/10   yb      Added UIM Config NV parameters
07/18/10   ps      Fix to high Lint errors
07/14/10   ps      Fix to clear gsdi data before populating
                   link established message
07/09/10   ps      Fix for compiler warnings
07/07/10   yb      Removing dependency on rex from apps side
07/01/10   nmb     Updates for JCDMA CSIM support
06/30/10   rm      Resolving mob compilation issues
06/28/10   rm      Removing INTLOCKs in the code
06/06/10   kk      Fixed uim get enum from path util function
06/01/10   js      ZI memory reduction fixes
05/21/10   ps      UIM driver is sending slot2 enabled for single-SIM build
03/29/10   ps      Fixed slot info for GSTK send proactive command
03/25/10   yb      Fix to set pointer to NULL after freeing memory
03/23/10   yk      Added a feature for infinite NULL responses
03/17/10   yt      Corrected previous KW fix
03/15/10   ps      Reverted Klocwork fix
03/12/10   ps      Fix uim crash during BIP testing
03/02/10   jk      Added files for MMSS
03/04/10   yt      Fixed compilation error
03/02/10   yt      Klocwork fix
02/18/10   vs      Merged CSIM functionality
02/18/10   nb      Changed FEATURE_UIM_SUPPORT_CSIM to FEATURE_UIM_SUPPORT_ATCSIM
02/15/10   nb      Updated for GSTK Dual Slot Changes
01/13/10   rm      Fix for 40K ATR issue
12/17/09   yb      Fixed compilation warning
12/16/09   nb      Compilation Fixes
12/08/09   js      Added support for UIM_CSIM_ARR file
11/17/09   ssr     Fix for not to reattempt SSD command again in failure case
11/13/09   sk      Fixed Linker errors when FEATURE_VIRTUAL_SIM is ON.
11/05/09   ps      Added 9K target support
11/03/09   rm      Fix to not send link established message when uim_status
                   indicates error for dual-slot
10/28/09   yb      Fixed compilation error due to wrong FEATURE_UIM_ISIM
                   featurization.
10/28/09   mib     Added support for additional files
10/28/09   nb      Release 7 Enhanced Network Selection
10/27/09   ap      Marco UIM_CHECK_FOR_NULL_PTR does not check for NULL if
                   memory reduction feature is not defined
10/26/09   ap      KW error back_up_uim_rpt_buf_ptr may be NULL
10/14/09   rm      Fix to not send link established message when uim_status
                   indicates error
10/12/09   kp      Demand Paging Changes
10/09/09   yb      Klockwork fix
10/08/09   kp      Fix for the error fatal
10/06/09   yb      1. Fix to avoid clock stop during recovery
                   2. Fix to add delays before clock stop and after clock restart,
                   to give time to card for finishing any pending internal processing.
09/26/09   kp      ZI memory reduction changes
09/25/09   yb      Klockwork fix
09/24/09   vs      Removed shutdown and dev init calls when NV 896 is 0/1
09/01/09   mib     Removed deprecated fs.h
08/31/09   bd/yb   Fix POLLING OFF issue
08/20/09   mib     Added support for additional files in DF-HNB and DF-WLAN
08/10/09   rm      Error handling for T=1 protocol
08/10/09   rm      Fixes to have separate count for repeated technical problems
08/07/09   adp     Reverting back to using sys_stru.h instead of sys_stru_v.h
08/03/09   yb      Added support to read ENS ENABLED NV item
07/20/09   mib     Added support for additional LTE files and EF-UST extension
                   Replaced flag TEST_FRAMEWORK with FEATURE_UIM_TEST_FRAMEWORK
07/17/09   yb      Added support for BCSMS service in cdma service table
07/10/09   rn      Fixed Lint Errors
07/06/09   ps      Turn off the polling timer when the polling off proactive
                   command comes in
06/23/09   rm      Fixes to correct the featurization of the sleep APIs
06/04/09   rm      Fixes to handle technical problem reported by the card
06/01/09   ssr     Fixed compiler warnings
05/28/09   yk      Null ptr check before checking the contents
05/08/09   ps      Fixed 8200 compiler warnings
04/29/09   ps      Fixed Medium Lint error
04/29/09   kk      Updated EF file path info for non-spec defined telecom EFs
04/25/09   adp     Featurizing based on FEATURE_NO_COMMON_INTERFACE for CMI &
                   Dummy 1h09 target builds. NAS code for Dummy 1h09 &
                   MOB's are coming from different branches
04/24/09   tml     Fixed infinite loop issue with GSM Cingular file access
04/17/09   vs      Update to allow power down when already in power down state
04/15/09   ssr     Fixed limited Length TERMINAL PROFILE for non-spec
                   compliant cards
04/09/09   kk      Fixed updating path_to_be_sel for proprietary EF access
04/08/09   nb      Fix for multiple select on MF
04/07/09   js      Support for accessing EFs by path with proprietary DFs
03/26/09   rm      Fix to double the work waiting time for GSM cards
03/25/09   js      Fixed issue where uim_cmd_rsp_timer did not expire
03/23/09   sun     Added support for Virtuim in QTF
03/11/09   kk      Fixed re-queueing of external RESET command during a reset
03/02/09   sun     Added private header file inclusion
02/25/09   rm      Fix for RX break errors in Idle and Non-Idle
02/20/09   vs      De-featurized inclusion of fs_public.h header file
02/18/09   vs      Updated IC-USB logic to execute only if IC-USB is selected
02/17/09   ssr     Update latest EFS filesystem call
02/12/09   nd      Fixed linker error when FEATURE_VIRTUAL_SIM is defined
02/04/09   vs      Populated GSDI historical bytes from USB UICC ATR data
01/28/09   nb      Removed usage of GSDI global data
01/14/09   vs      Fix to indicate HCLK, MCLK restrictions during uim_init()
12/03/08   js      Support for accessing GSM Cingular DF by path
11/26/08   nb      Klockwork Fixes
11/19/08   vs      Moved sleep register call from uim_dev_init() to uim_init()
11/13/08   rm      Use timer callback for command response timer to handle the
                   work waiting timer expiry immediately in the timer context.
11/11/08   yb      Fixed compilation error when FEATURE_GSTK is not defined
11/05/08   vs      Update to allow UIM clock stop only after sending the first
                   instruction to the card in passive mode
10/07/08   vs      Fix to power up UIM LDO after card power down, vote TCXO off
                   permanently when IC-USB interface is selected, clear poll
                   timer before requesting ATR
10/06/08   yb      Sending BIP envelope after TR while BIP is working
09/19/08   tml     Re-select MF to force directory maintainance to MF when
                   received a sw1 warning with normal return
09/23/08   js      Added LTK Support for USB UICC
09/22/08   rm      Fix for parity errors in UART_DM
09/19/08   rm      Work waiting time fix for UART_DM
09/18/08   js      Fixed featurization for USB UICC.
09/17/08   vs      Updated UIM USB ICCD poll delay timer logic
09/17/08   js      Implemented UIM Card interface selection based on first
                   instruction class NV
09/15/08   vs      Added support for UIM USB ICCD pre-suspend timer
09/08/08   tml     Fixed Get ATR issue with USB featurization
09/05/08   vs      Moved fallback timer and signal clearance to UIM USB module
08/26/08   kk      Few comments from uim directory management review
08/22/08   vs      Added support for data re-poll request by USB UICC and ISO
                   fallback mechanism
08/08/08   vs      Removed featurization UIM_CLOCK_STOP_ALLOWED
08/05/08   vs      Renamed start and stop clock functions appropriately
07/22/08   js      Fixed UIM_MAX_PATH_ELEMENTS issue. Updated path length to 4
                   for all UIM commands. MAX path length in MMGSDI is set to 5.
07/24/08   vs      Fixed UIM USB UICC related featurization
07/17/08   vs      Added support for UIM USB UICC ICCD interface
06/21/08   kk      Added PKCS15 Support
04/30/08   kk      Added support for MMS EFs
03/20/08   nk      Added OMH EF and CST Support
01/02/08   jk      Added functions to check and remove memory allocs on cmd q
11/19/07   js      Radio Access Technoligy Balancing support
11/19/07   sun     Added file paths for ISIM and MFLO
11/08/07   vs/nk   Added EUIMID and OMH feature support
09/10/07   tml     Add MFLO support
08/16/07   jk      Rearranged featuization to support DATA Auth on 7K
07/02/07   sun     Added support for ODD INS Class
05/21/07   jk      Provide API to check if uim in recovery mode
05/22/07   nk      removed retry setting for jcdma feature to resolve power up
                   issue
05/21/07   sp      Fixed potential buffer overflow in uim_get_file path.
05/11/07   wli     Changes for JCDMA requirements
04/26/07   sp      Featurize for LTK
05/07/07   sun     Fixed Lint Error
05/02/07   sun     Added Support for MBMS
04/17/07   sp      Featurize for LTK
03/21/07   jk      Include tmc.h instead of mc.h if FEATURE_CDMA1X not defined
02/28/07   pv      Replace strncat with std_strlcat.
02/27/07   jk      Reduce Command queue size for memory reduction
02/22/07   pv      Revert NULL check for T=1 protocol.
02/15/07   pv      Featurize code using uim_current_protocol.
02/13/07   pv      When a recovery command was from uim_free_q, return
                   it back to the queue instead of just throwing it away.
02/13/07   pv      Add uim_clear_poll_timer and uim_set_poll_timer functions
                   to manage the poll timer.  These functions wrap the logic of
                   trying to be in synch with the DRX cycle while making sure
                   that we wake-up if there was no DRX on time.
02/12/07   pv      Do not ignore UIM_TRANSACTION_SIG even if there are
                   bytes in the FIFO since they could have been NULL bytes
                   that we might be receiving for a long time.
                   Clear the UIM_TRANSACTION_SIG when a time-out signal is
                   received.
02/05/07   pv      Avoid usage of cmd_ptr_in_use when NULL.
                   Avoid passing NULL cmd_ptr to uim_process_t_1_block.
01/25/07   jk      Fixed compile problem when FEATURE_UIM2 is defined instead of FEATURE_UIM1
01/16/07   nk      Append /apps/ to efsfilename when apps mount features defined
01/15/07   pv      Use strncat instead of strcat.
12/12/06   tml     Add uim_reset_dir_for_slot_channel to reset the ef/df/adf
                   info for a particular slot and channel
11/13/06   pv      Do not poll if in passive mode and protocol is unknown.
11/09/06   pv      Move logging of time-out information before resetting the
                   flag indicating that a command is in progress.
11/07/06   pv      Ignore timeout if we have a byte present even before PPS.
09/28/06   pv      Process the response for the external reset when internal
                   reset is issued as a result of that external reset command.
09/27/06   pv      Allow external select on ADF to use 0x7FFF if the
                   same application is already active.
09/20/06   jk      Added necessary lint suppresion due to featurization necessary
                   for 7200/7500 RPC Library convergance
09/11/06   jk      Changed featurization so that 7200 and 7500 generate identical
                   RPC Files
08/25/06   jar     Added handling for additional MMGSDI Card Error
                   enumerations in uim_gsdi_event_cb_func.
08/10/06   sp      Report poll error without retries for bad status words
                   when FEATURE_UIM_RPT_BAD_SW_ON_POLL is enabled
08/09/06   pv      Featurize the usage of members of nv_uim_config_params_type
                   under FEATURE_UIM_NO_ADDITIONAL_CONFIG_PARAMS for
                   targets not picking up the NV dependencies
08/01/06   sp      Changes to support when fetch fails.
07/28/06   jk      Fixed Error Fatal with No Card
07/27/06   jk      Lint fix
07/17/06   jk      Support for APDU logging to EFS
07/05/06   pv      Do not re-try authenticate or run gsm commands.
07/03/06   pv      Reset the uim_internal_command_used flag when uim_reset_uim
                   is called
06/28/06   pv      Do not switch instruction class on time-out if operating
                   in T=1 mode.
06/21/06   pv      Flush commands before notifying error.
06/14/06   pv      Do not ignore any max errors due to having overrun
                   or bad status words.
06/12/06   pv      Use uim_mode instead of uim_passive_mode.
                   Make all resets cold.
                   Initialize uim_internal_command_used when popping out a cmd.
                   Remove FEATURE_UIM_TRY_TO_RECOVER_FROM_ERRORS featurization.
                   Do not directly powerdown the interface when an internal
                   command times out while not doing a power-up.
                   Do not poll while in recovery mode.
                   Set and use uim_config_params.
                   Consider Transaction timer expiry as a regular time-out.
06/04/06   jar     Made local var back_ui_uim_rpt_buf a static to keep from
                   creating a var of 544 bytes on the stack
05/22/06   pv      Change timer_always_on_group to uim_always_on_timer_group
                   which is a locally defined group and is never disabled.
05/15/06   jk      Added extra NULL Param check in uim_log_timeout_info
04/24/06   pv      Added code to read uim_config_params from NV
04/21/06   pv      Featurize out code using tmc_heap_small based on
                   FEATURE_LOW_MEMORY_USAGE.
04/19/06   pv      Removed INLINE for uim_get_file_path
04/06/06   sun     Send the Error Report in case of error in Passive mode
04/11/06   pv      Change the call to UIM_READ_STATUS macro to a function call.
                   Add more information to uim_reset.txt
04/03/06   pv      Lint Fixes
03/29/06   pv      Set the poll timer followed by a poll error in
                   uim_process_cmd_err.
03/21/06   tml     added cfis support
03/14/06   pv      Added code to wait until uim_mv_init is done by tmc.
                   Merged missing HZI support.
03/09/06   jar     Merged in from Branched HZI
03/07/06   jk      Fixed featurization compile issue for SC-1X
02/16/06   pv      Featurized the code in uim_can_ignore_timeout based on
                   FEATURE_USE_TIME_VU.  Added support for UIM_USIM_EHPLMN
02/15/06   pv      Added function uim_can_ignore_timeout to check if a timeout
                   signal can be ignored and check if the timeout can be
                   ignored for T=1 protocol as well.
02/14/06   nk      Merge for: Use heap_small when the size of get response
                   allocation is less than 64 bytes; Remove uim rows from usim
                   path table for enums removed; and Added support to cache the
                   get response data and not do a get response if the item is
                   already cached.  Also, expose a new API to not send down the
                   select for the sake of getting the get response information
02/02/06   pv      Call the correct API to power down in order to do a
                   cold reset for dual slot.
01/30/06   pv      Remove code to switch instruction classes on a timeout
                   during power-up based on
                   FEATURE_UIM_NO_SWITCH_INST_ON_WWT_EXPIRY
01/30/06   pv      Do a cold reset instead of a warm reset when we timeout
                   during the power-up procedures
01/30/06   pv      Notify GSDI about the reason for power down.  Do not re-try
                   commands while in passive mode.
11/14/05   pv      Lint Fixes
10/27/05   pv      Code to turn off the interface if no ATR is received whthin
                   40,000 UIM clock cycles
10/26/05   pv      Allow uim_flush_command to be used without disturbing
                   the uim_rpt_buf
10/13/05   wli     Fixed comilation error when defining FEATURE_UIM2 alone.
09/13/05   jk      Added changes to keep interface powered down during voltage
                   switch for minimum time specified in ISO7816
05/26/05   jk      Added uim_get_uart_status() function to safely call
                   UIM_READ_STATUS() only when clock is still on
08/29/05   tml     Fixed compilation
08/29/05   jk      Added functions to get path to files from EF file ID
08/25/07   pv      Allow RESET command when in powered down state
08/04/05   wli     Added comments
08/02/05   wli     Added HOT SWAP feature.
07/28/05   pv      replaced UIM_MAX_NUM_CDD_ATTEMPTS with UIM_MAX_NUM_ATTEMPTS
                   where not necessary.
07/27/05   jk      Made work waiting time array for dual slots
07/26/05   pv      Internal polling message is now HIGH
07/22/05   pv      Poll with no data expected in response when not in a call.
07/22/05   pv      Define the polling timer to be in the timer_always_on_group
                   if FEATURE_USE_TIME_VU is defined
07/18/05   sun     Added support for USIM files in ENS
07/19/05   tml     Fixed switch slot issue
06/22/05   pv      Reset the transaction timer when we get resp from driver.
                   Allow power_up and power_down commands in passive mode
06/15/04   tml     Use correct type structure for new READ/WRITE cmd enums
06/13/05   pv      Code to support UIM commands with allocated memory.
06/02/05   pv      Code for FEATURE_UIM_ISIM & FEATURE_UIM_SESSION_MANAGEMENT
05/17/05   jk      Adjusted Fix for uim_clock_stop
05/13/05   jk      Only allow uim_clock_stop when uim_cmd_q is empty
05/10/05   pv      Code for BT sim access - Added checks to see if a
                   command is valid in the current status of UIM.  Allow
                   UIM_POWER_UP_UIM_PASSIVE_F when the UIM status is faulty.
05/10/05   sst     Lint fixes.
04/25/05   jk      Added FEATURE_UIM_RUIM around new HW ESN issue to fix compile
                   issue for non-RUIM
04/22/05   jk      Added function to return HW ESN for BREW
03/14/05   wli     Added 820B EFs
03/11/05   pv      Added code to select USIM ADF when authenticate
                   command comes in and a DF under USIM is currently selected
02/28/05   sun     Added file - EF MWIS
02/09/05   pv      Check if the CMD_RSP_SIG is received for the slot which
                   has an error to continue or ignore the signal.  Fix
                   compilation issue when both multiple voltage and dual_slots
                   is turned on.
01/04/05   sun     Added support for Cingular Wireless
02/03/05   pv      Added code to indicate that we have received a command_response even if
                   the uim_status is ERR if we have already received the task_stop signal
01/26/05   pv      Added additional check if atr is received if multiple
                   voltage support is on and we got a max_parity error
01/21/05   wli     Extend work waiting timer value for an out of spec RUIM card.
01/21/05   jk      Added gsdi_cmd_ext which checks GSDI CMD Q size to replace gsdi_cmd.
01/21/05   jar     Fixed Dual Slot Compile Error.
12/28/04   pv      Added code to handle timeout signaling from the driver based on
                   time stamp.  Also included more information in timeout_logging like
                   the ATR bytes, signal's pending in the UIM TCB etc.
                   Changed the timeout_logging function to be called every where
                   so that we will do a power down or reset before logging.
                   Added code to handle 27.11.2.6 (WWT exceeded during PPS phase)
                   when multiple voltage class support is turned on.
12/16/04   jk      Merged changes for Orange support from mainline
12/14/04   pv      Added check to make sure that cmd_ptr is not null
                   before invoking uim_process_card_response.
12/06/04   pv      Added code to log UIM driver/server status when a timeout occurs
11/22/04   jk      Added 3GPD support for CDMA service table check
11/21/04   wli     Dual slot merge and debug.
11/16/04   pv      Added code to power down the interface when a currupted
                   ATR is received and do not try a higher voltage if we
                   already have the voltage information from the ATR.
                   Cr. No. 51127
11/15/04   pv      Added definition for uim_t_1_bgt to be used as block gaurd time
11/01/04   pv      Include the work waiting report while sending the
                   link_established command to gsdi
10/08/04   jar     Updated Number of UIM Items for GSM SIM and USIM
10/06/04   jar     Merged in 4.2x Additional EFs
09/20/04   pv      Brought in support for dual slots when powering down the
                   interfaces because of maxing error count.  Also added
                   code to signal the requestor when the current command fails.
                   The new code checks if there is a next voltage class available
                   upon max rx break error and tries it instead of powering down.
09/01/04   jk      Added support for HRPD AN Authentication services in R-UIM
09/01/04   ck      Added support for CPHS files
09/01/04   ck      Added support for CSIM
08/30/04   pv      Take the protocol information from UIM variables instead of
                   requesting GSDI for the protocol information.
08/10/04   pv      Added more messages to debug timeout and cleared the error
                   flags conditionally.
08/02/04   ck      Reset the command_in_progress flag when the command was
                   rejected by the different modules.
07/19/04   pv      Added potential fix for a timeout issue by ignoring the time
                   out signal when a valid byte is present in the Rx FIFO.
07/14/04   wli     Changed Orange file path.
06/16/04   ck      Merged support for including protocol information in link
                   established message to GSDI from Cheetah branch
06/16/04   wli     Added ORANGE DF and its related EFs.
06/11/04   wli     Added EFs to support JCDMA.
06/03/04   ts      Fixed featurization misspelling -
                   FEATURE_UIM_SUPPORT_DUAL_SLOT(S).
05/17/04   ck      Moved the featurizations around to send DF presence info
                   for single slot in GSDI Link est message
                   Also added ATR information in GSDI Link est
                   Added support to indicate whether the command was transacted
                   with the card.
04/16/04   ts      Fixed problems with error condition processing due to bad
                   merge.
04/08/04   wli     Featurize for non-gsdi targets.
03/26/04   ts      Added support to bring down the interface for excessive null
                   procedure bytes.
                   Added processing to bring down the interface for excessive
                   receive error conditions.
03/15/04   ts      Change the instruction class to GSM if card times out on
                   USIM instruction class.
03/12/04   ck      Removed the check for uim_powerdown while starting and
                   stopping the clock for dual slots.
03/08/04   ck      Added suport to read specific CDMA files from both the slots.
02/25/04   ck      Fixed a bug where the second slot was also initialized when
                   the first slot timed out after initial power up.
02/18/04   ck      Fixed Lint errors.
02/09/04   ck      Changed the if-else to switch stmt in the function uim_get_
                   slot() for certain GSM files.
02/05/04   ck      Added support to access certain GSM files from either slot
                   irrespective of operational slot.
02/03/04   ts      Added a check to wait until the power down vote allows power
                   down or when power down is required until powering down the
01/29/03   ck      Removed UIM_MC from the CDMA traffic channel mask
                   Cleared CMD_Q_SIG from the I-mask upon cmd completion before
                   processing the power up of the second slot. Clearing the sig
                   after the function uim_process_slot_power_up resulted in
                   processing of another command before completing the power up.
01/21/04   ck      Fixed a bug where the poll timer was not being reset when a
                   91 xx (Fetch) was returned for a Status response.
12/19/03   jk      Added processing for R-UIM 3GPD Services.
12/15/03   wli     Fixed GSM ACCESS compilation error.
12/10/03   ck      Cleaned up polling for dual slots.
12/09/03   ck      Commands between polls do not reset the poll timer.
12/04/03   wli     Added FEATURE_UIM_TOOLKIT feature switch around toolkit
                   related varibles.
12/04/03   ck      Added a feature to force the first instruction class as GSM
                   to be used for dual slot targets.
                   Modified the aggregation of bits in CDMA_TC and GW_TC.
11/04/03   ck      Ensured that the CMD_Q_SIG was added to the mask after the
                   processing of the PREF_SLOT_CHANGE_F command.
10/03/03   ck      Added support to stop the clock irrespective of the vote and
                   turn it ON only when needed to process commands.
                   Merged in the support to use the flag uim_atr_pps_done
09/17/03   ck      Added checks to make sure that PREF_SLOT_CHANGE command is
                   not rejected when the status for the slot specified in the
                   command header is invalid.
09/12/03   ck      Changed the polling architecture to make sure that the
                   interval between STATUS command is less than 30s while in
                   traffic channel even if there are other accesses to the card
                   Also, poll the card as soon we get an indication of the
                   traffic channel setup.
09/02/03   ck      Added support to set the CDMA directory present in the nvruim
                   structure for dual slots.
08/20/03   ck      Use the slot in the command for Invalidate and Rehabilitate
                   commands.
08/15/03   ck      Changes from code review.
08/14/03   ck      Added support for dual slots.
08/04/03   ck      Reset the poll timer when the POLL_TIMER_SIGNAL was set and
                   UIM status was CLK_OFF_S and the card in use was not a
                   pro-active card.
                   Included UIM_TC to the check for arriving at the poll timer
                   delay when in traffic channel.
07/09/03   ck      Initialized the overrun error flag to FALSE.
                   Fixed a comment line that threw up a warning.
                   Removed the featurization around the funtion uim_nv_init().
06/25/03   ck      Added check to make sure that we select the USIM ADF even
                   when we are in the second level DF under USIM ADF before
                   processing the AUTHENTICATE command.
06/19/03   ck      Moved the code that handles command error into a function and
                   added support to handle overrun error.
06/05/03   ck      Reset the curr_app data structure in the function
                   uim_reset_dir().
06/03/03   ck      Added support to reset the UIM based on the status rsp
                   UIM_INSTRN_CHANGE and UIM_CONVENTION_CHANGE.
05/18/03   wli     Fixed compilation error when GSM ACCESS feature turned on.
05/01/03   ck      Removed the featurization around forcefully voting off the
                   different power control bits upon receiving TASK STOP.
04/28/03   ts      Added support for multi-Voltage classes.
04/28/03   ts      Added support for T=1 protocol.
04/28/03   ck      Moved the powerdown processing code as the last check in the
                   uim_task function. This greatly improves the standby time as a
                   result of stopping the clock right after the command(poll) is
                   completed.
04/25/03   wli     Declaired temp_uim_aid that was missing.
04/24/03   ck      Fixed an issue with directory maintenace that was causing an
                   issue with selection of Telecom PBR following a failure in
                   selecting USIM PBR.
02/26/03   jk      Added UTIL and WIM functionality
03/14/03   ck      Added a check to see if the card is a pro-active card
                   before polling the card when the status is CLK_OFF_S.
03/04/03   ck      Added the function uim_get_enum_value_for_path.
02/13/03   wli     Changed to support GSTK.
02/02/03   ck      Lint changes.
01/17/03   ck      Added support for CHV operations in USIM protocol to
                   internally select the ADF specified in the cmd if reqd
01/07/03   ts      Added EFrplmnat
11/21/02   ts      For 1x only builds, vote everyone off when receiving a task
                   stop.  Set status to ERR to ensure UIM does not power-up
                   after task stop.
11/15/02   ts      Ensure power down status indicates it is okay to power down when
                   powering down due to bad UIM link.
10/31/02   ts      Removed extraneous parenthesis in power down/clock stop
                   determination.
10/29/02   ck      Declared the boolean uim_atr_recd and use it to check if the
                   command count needs to be incremented for the internal
                   power up command so that the command is not tried again.
10/25/02   ck      Added support to indicate the poll error with a new status to
                   GSDI
10/24/02   ts      Added support for work waiting time
10/24/02   ts      Set command_in_progress to false when command times out.
10/14/02   ck      Report error for GSM ECC if one attempt fails.
10/11/02   ck      Added support for reading the first instruction class to be
                   used in the SELECT command from NV in the context of TMC.
10/02/02   ts      Wait for requests to be finished before powering down UIM
                   for task stop.  Acknowledge task stop after powering down.
10/02/02   ts      Changed the featurization around the GSDI interface.
09/20/02   ts      Set the cmd_ptr to NULL when finished processing the
                   command.
08/27/01   ck      Added support for multimode architecture to start and stop
                   the UIM task.
                   Added support for Multimode GSDI.
08/14/02   wli     Modified the feature defines around stop clock or power down.
07/25/02   ck      Modified the feature defines around polling while in idle
                   mode to be only applicable when the UIM is proactive.
                   Modified the input parameters to STATUS command for an
                   internal poll.
                   Fixed a path selection issue that did not include MF in the
                   path when just the AID was selected.
07/09/02   ck      Added support for AID selection by path.
                   Modified powerdown of UIM to be featurized out for USIM and
                   allowed for clock stop based on the return type.
05/21/02   plm     Removed conditional code for T_KOVU and T_MSM5200 for calling
                   tmc_task_start(). This code is not necessary for GSM or MSM5200
                   since they already redefine mc_task_start() to tmc_task_start()
                   in mc.h.
05/15/02   ts      Featurized else for clock stop code when toolkit is not
                   enabled.
05/01/02   ts      Need to set the poll timer with the new timer duration when
                   the duration changes between the proactive duration to the
                   traffic channel duration.
04/29/02   ck      Added UIM_USIM_SESSION to uim_ctl as default for USIM targets.
04/09/02   ck      Modified the #defines to implement Clock stop for USIM.
04/03/02   ts      Implemented polling control for Proactive commands.  Polling
                   duration depends on whether MC votes to keep the UIM power
                   on.  This indicates the phone is in the traffic channel.
                   Otherwise, the polling frequency is the Proactive frequency.
02/11/02   ts      Changed the code surrounding uim_get_file_path to include
                   processing for items (EFs) whose paths are defined by the
                   card.
02/01/02   ts      Process a new status from processing a UIM command.  The new
                   status indicates the card does not support 3V technology.
01/25/02   ts      Cleared the poll timer, poll timer signal and mask the poll
                   timer task signal from the UIM task to ensure the poll does
                   not happen when processing a command.
01/25/02   ts      Added support for Kovu.  Kovu is same as MSM5200.
01/22/02   ck      Added support for clock stop mode.
01/08/02   ck      Wrapped the uim_return_cdma_svc_availabililty function
                   call around the feature FEATURE_UIM_RUIM
12/18/01   ck      Added support to access GSM EFs when build for R-UIM and
                   modified the featurization around GSDI code.
                   Added support for EFs in DCS1800 DF.
13/12/01   ts      Cleaned out the last selected directory path when resetting
                   the UIM card.
12/13/01   ts      Added some comments and fixed format of code.
12/04/01   ts      Added support for UTK.
11/19/01   ts      Fixed a polling bug for USIM.
10/11/01   ck      Removed the featurization for DOG reporting for MSM5200.
                   Modified the path for EF HLPMN selector in USIM ADF.
                   Enhanced the SEEK interface to include path in the command
08/10/01   ck      Fixed the problem where the command header status was not
                   set to RECEIVED when the processing of the command had
                   begun.
                   Fixed the problem where the card was being reset even after
                   it was declared as faulty.
07/27/01   ts      Set the UIM_AUTH voter flag on startup so that the UIM stays
                   powered on until Auth gets initialized (reads call count).
07/23/01   ck      Added support for EF ECC,SPC STATUS and MAX_PRL.
                   Added a function to export the response to store esn_me.
                   Fixed a bug in which a second select to the same EF did not
                   fill the path to be selected array.
05/28/01   ck      Added support for R-UIM, GSM and USIM modules.
01/15/01   ck      Created module from AUTH module rev 1.5 from Common
                   archive.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "target.h"
#include "customer.h"
#include <stringl/stringl.h>
#include "timer.h"
#include "rex.h"
#include "task.h"
#include "dog.h"
#include "uimdrv_msg.h"
#include "err.h"
#include "mc.h"
#include "queue.h"
#include "uim.h"
#include "uimi.h"
#include "uimdrv.h"
#include "uimgen.h"
#include "stdio.h"
#include "uimcdma.h"
#include "uimgsm.h"
#include "uimusim.h"
#include "uimisim.h"
#include "mmgsdilib_p.h"
#include "gstk_exp.h"
#include "gstk_p.h"
#include "uimutil.h"
#include "fs_public.h"
#include "uimglobals.h"
#include "uimsub_manager.h"
#include "uim_sigs.h"
#include "uim_nvcfg.h"
#include "uim_logging.h"
#include "uim_hotswap.h"
#include "uim_polling.h"

#ifdef FEATURE_UIM_REMOTE_UIM
#include "uim_remote.h"
#endif /* FEATURE_UIM_REMOTE_UIM */
#if defined( FEATURE_UIM_T_1_SUPPORT )
#include "uimt1.h"  /* T=1 protocol support */
#endif /* FEATURE_UIM_T_1_SUPPORT */

#if !defined (FEATURE_UIM_RUIM_W_GSM_ACCESS)
#include "sys_stru.h"
#include "sys_cnst.h"
#include "sys_type.h"
#include "timers.h"
#include "gs.h"
#include "ms_timer.h"
#endif /*  !FEATURE_UIM_RUIM_W_GSM_ACCESS */
#include "estk.h"
#ifdef FEATURE_UIM_USB_UICC
#include "uimusb.h"
#endif /* FEATURE_UIM_USB_UICC */

#include "rcevt_rex.h"
#include "rcinit.h"

#include "uimdrv_main.h"
#include "uimdrv_gpio.h"
#include "uimdrv_uartdm.h"
#include "dog_hb_rex.h"

#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */


/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
#define UIM_JCDMA_POLL_INTERVAL 5000

#define UIM_MAX_COMMAND_RETRIES       0x03


/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                               Macros                                    */
/*                                                                         */
/*-------------------------------------------------------------------------*/

  /* Macro to identify if the command is allowed while UIM is in passive mode */
  #define UIM_IS_CMD_VALID_IN_MODE(cmdptr, uim_ptr)                                  \
  (((((int)((cmdptr)->hdr.slot)-1) < (int)UIM_NUM_DRV_SLOTS)                         \
    && (((int)((cmdptr)->hdr.slot)-1) >= 0))?                                        \
    ((uim_ptr->command.mode == UIM_PASSIVE_MODE)?                                    \
                       (((cmdptr)->hdr.command == UIM_RESET_F) ||                    \
                       ((cmdptr)->hdr.command == UIM_RESET_WITH_TP_F) ||             \
                       ((cmdptr)->hdr.command == UIM_RESET_SWITCH_UIM_PASSIVE_F) ||  \
                       ((cmdptr)->hdr.command == UIM_POWER_UP_UIM_PASSIVE_F) ||      \
                       ((cmdptr)->hdr.command == UIM_POWER_DOWN_F) ||                \
                       ((cmdptr)->hdr.command == UIM_STREAM_ISO7816_APDU_F)) :       \
                       (TRUE)) :                                                     \
                       (FALSE))

/* Macro to get the command mode type for a command */
#define UIM_GET_CMD_MODE(command) ((command >> 8) & 0x0F)

/*===========================================================================
              F U N C T I O N   P R O T O T Y P E S
===========================================================================*/
static boolean uim_need_intermediate_sel
(
  uim_instance_global_type     *uim_ptr,
  uim_channel_type              curr_ch,
  uim_intermediate_select_type *intermediate_select_ptr
);

static uim_cmd_status_type uim_command_response
(
  uim_rsp_buf_type         *rsp_ptr,
  uim_instance_global_type *uim_ptr
);

static void uim_fetch
(
  rex_sigs_type            *mask,
  uim_instance_global_type *uim_ptr
);

static rex_sigs_type uim_wait_filter_non_recovery_cmds
(
  rex_sigs_type             sigs_to_wait_for,
  uim_instance_global_type *uim_ptr
);

void uim_reset_poll_timer_val
(
  uim_instance_global_type *uim_ptr
);

static void uim_insert_cmd_at_head_of_queue
(
  uim_cmd_type             *cmd_ptr,
  uim_instance_global_type *uim_ptr
);

static void uim_wake_up_poll_timer_expiry_cb
(
  unsigned long uim_ptr
);

static void uim_update_swp_capability
(
  uim_instance_global_type *uim_ptr
);

static boolean uim_check_cdma_df_select_before_tp (void);

#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /*FEATURE_UIM_TEST_FRAMEWORK*/

boolean uim_check_explicit_select_df_cdma_before_telecom_access
(
  uim_instance_global_type     *uim_ptr
);
/*===========================================================================

FUNCTION UIM_SET_POLL_TIMER

DESCRIPTION
  This function should be used to set the time to trigger a poll.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  Sets the uim_poll_time_delay to the value passed in.
  Sets a second timer based on feature definition to ensure
  wake-up.

===========================================================================*/
void uim_set_poll_timer(
  rex_timer_cnt_type        time,
  rex_timer_cnt_type        wake_up_poll_offset,
  uim_instance_global_type *uim_ptr
)
{
  if ( TRUE == uim_ptr->flag.uim_disable_polling )
  {
    /* When polling is disabled, do nothing and return */
    return;
  }

  if(uim_ptr->poll.is_proactive_polling_enabled == FALSE) 
  {
    
    if((((int)uim_ptr->state.ctl & UIM_TRAF_CHAN) && 
   	   (uim_ptr->poll.presence_detection_state == UIM_PD_POLLING_SUSPENDED))||
          (!((int)uim_ptr->state.ctl & UIM_TRAF_CHAN)))
  
    { /* If there is no traffic channel, then no need to set POLL timer */
      UIMDRV_MSG_MED_1(uim_ptr->id, "Stopping poll timer , current state 0x%x", 
                                      uim_ptr->state.ctl);
      uim_clear_poll_timer(uim_ptr);
      return;
    }
  }
  
  /* Stop the poll timer if the timer value is UIM_MAX_POLL_DURATION */
  if (time == UIM_MAX_POLL_DURATION)
  {
     uim_clear_poll_timer(uim_ptr);
     UIMDRV_MSG_MED_0(uim_ptr->id, "Stop Proactive Polling Timer");
  }
  else
  {
    /* check if UIM_POLL_TIMER_SIG is already set in uim tcb then
       it needs to be restored because rex_set_timer clears it*/
    if(rex_get_sigs(uim_ptr->tcb_ptr) & UIM_POLL_TIMER_SIG)
    {
      /* Set the regular timer */
      (void) rex_set_timer(&uim_ptr->poll.no_wake_up_poll_timer, time);
      /* If this feature is defined, the timers which are not always on
       * would only be triggered when the ME wakes-up for any reason.
       * The always on timer would wake-up the ME when it goes off.
       * So we are giving UIM_WAKE_UP_POLL_OFFSET duration for a DRX
       * cycle so that the POLL could co-inside.  In the worst case,
       * we make sure that we wake-up within the UIM_WAKE_UP_POLL_OFFSET
       * duration of the given time (which is already reduced by 2 seconds
       * to compensate for timer inaccuracies)
       */
      (void) rex_set_timer(&uim_ptr->poll.wake_up_poll_timer, time +
                            wake_up_poll_offset);
      /* Re storing the UIM_POLL_TIMER_SIG so that STATUS COMMAND can be sent */
      (void) rex_set_sigs(uim_ptr->tcb_ptr, UIM_POLL_TIMER_SIG);
    }
    else
    {
      /* Set the regular timer */
      (void) rex_set_timer(&uim_ptr->poll.no_wake_up_poll_timer, time);
      /* If this feature is defined, the timers which are not always on
       * would only be triggered when the ME wakes-up for any reason.
       * The always on timer would wake-up the ME when it goes off.
       * So we are giving UIM_WAKE_UP_POLL_OFFSET duration for a DRX
       * cycle so that the POLL could co-inside.  In the worst case,
       * we make sure that we wake-up within the UIM_WAKE_UP_POLL_OFFSET
       * duration of the given time (which is already reduced by 2 seconds
       * to compensate for timer inaccuracies)
       */
      (void) rex_set_timer(&uim_ptr->poll.wake_up_poll_timer, time +
                            wake_up_poll_offset);
    }
  }
  uim_ptr->setting.poll_time_delay        = time;
  uim_ptr->setting.wakeup_poll_offset     = wake_up_poll_offset;
}/* uim_set_poll_timer */


/*===========================================================================

FUNCTION UIM_CLEAR_POLL_TIMER

DESCRIPTION
  This function clears any pending poll timer(s).

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  Clears all the timers relating to a poll.
===========================================================================*/
void uim_clear_poll_timer
(
  uim_instance_global_type *uim_ptr
)
{
  /* Clear the regular timer. */
  (void) rex_clr_timer(&uim_ptr->poll.no_wake_up_poll_timer);
  /* If this feature is defined, clear the second timer */
  (void) rex_clr_timer(&uim_ptr->poll.wake_up_poll_timer);
}/* uim_clear_poll_timer */

/*===========================================================================

FUNCTION UIM_RESET_POLL_TIMER_VAL

DESCRIPTION
  This function resets UIM POLL timer value to UIM_POLL_TIME.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void uim_reset_poll_timer_val
(
  uim_instance_global_type *uim_ptr
)
{
#ifdef FEATURE_UIM_USB_UICC
  /* Reset Poll Timers to default timer values */
  if (uim_is_usb_enabled(uim_ptr) && uim_usb_is_usb_interface_selected(uim_ptr))
  {
    uim_ptr->setting.poll_time_delay        = UIM_USB_POLL_TIME;
    uim_ptr->setting.proactive_time_delay   = UIM_USB_POLL_TIME;
  }
  else
#endif /* FEATURE_UIM_USB_UICC */
  {
    uim_ptr->setting.poll_time_delay        = UIM_POLL_TIME;
    uim_ptr->setting.proactive_time_delay   = UIM_POLL_TIME;
  }
  uim_ptr->setting.wakeup_poll_offset = UIM_WAKE_UP_POLL_OFFSET;
}/* uim_reset_poll_timer_val */


/*===========================================================================

FUNCTION UIM_GET_ESN_ME ( )

DESCRIPTION
  This function returns the HW ME ESN cached by TMC at power up.

DEPENDENCIES
  None

RETURN VALUE
  The HW ME ESN.  If the ESN was not cached properly, 0 is returned

SIDE EFFECTS
  None

===========================================================================*/
uint32 uim_get_esn_me ( void )
{
  /* "uim_get_esn_me is now deprecated because tmc_get_esn is deprecated" */
  /* Return 0 so that users of the api use nv function to get ESN */
  return 0;
} /* uim_get_esn_me */


/*===========================================================================

FUNCTION UIM_NOTIFY_ERRROR

DESCRIPTION
  This function notifies the higher layer (GSDI) if UIM encounters an
  unexpected error during power up or an internal wake up and passes the
  status of the UIM.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void uim_notify_error
(
  uim_status_type           uim_status_param,
  uim_instance_global_type *uim_ptr
)
{
  uim_slot_type slot;

  RETURN_IF_INSTANCE_INVALID(uim_ptr->id);

  slot = (uim_slot_type)uim_ptr->id + 1;

  /* UIM BUSY RESPONSE FEATURE: Call mmgsdi API to clear uim busy */
  uim_busy_ind_timer_state_handler(uim_ptr, UIM_BUSY_RESPONSE_EVT_NOTIFY_ERROR);
  uim_trans_timer_state_handler(uim_ptr, UIM_BUSY_RESPONSE_EVT_NOTIFY_ERROR);

  if( uim_hw_if.intctrl[uim_ptr->id].m_hs_enable == TRUE
#ifdef FEATURE_UIM_BTSAP_CLIENT
     || ( IS_BTSAP_HANDLE_ACTIVE(uim_ptr) )
#endif /* FEATURE_UIM_BTSAP_CLIENT */
    )
  {
    /*if card removed is already notifed , then no need to notfiy any error again
      unless it is a power_down notification*/
    if ((uim_ptr->hotswap.hotswap_notify_card_removed == TRUE) &&
        (UIM_POWER_DOWN_CMD_NOTIFICATION_S != uim_status_param))
    {
      UIMDRV_MSG_LOW_1(uim_ptr->id,"uim already notify card removed for this error 0x%x",
                       uim_status_param);
      uim_sm_notify_qmi_slot_status(uim_ptr);
      return;
    }
    if (uim_status_param == UIM_CARD_REMOVED_S)
    {
      uim_ptr->hotswap.hotswap_notify_card_removed = TRUE;
    }
  }

  UIMDRV_MSG_LOW_2(uim_ptr->id,"uim_notify_error status 0x%x for slot %d",
                   uim_status_param, slot);
  (void)uim_sm_mmgsdi_notify_card_error( uim_status_param, uim_ptr);

  if(uim_nv_is_feature_enabled(UIMDRV_FEATURE_LOG_TO_EFS,
                               uim_ptr) == TRUE
     && uim_ptr->debug.efslog_logging_in_progress)
  {
    uim_ptr->debug.efslog_logging_in_progress = FALSE;
    (void) rex_set_sigs( uim_ptr->tcb_ptr, UIM_EFSLOG_PURGE_SIG );
  }

   /*
      Reset the necessary UIM globals in the event of UIM notifying
      error.
      If UIM is notifying error due to UIM_POWER_DOWN_CMD_NOTIFICATION_S
      then there is no need to clear the globals as we have not hit any
      error scenario.
   */
  if (UIM_POWER_DOWN_CMD_NOTIFICATION_S != uim_status_param)
  {
    UIMDRV_MSG_HIGH_0(uim_ptr->id,"Resetting necessary UIM globals after notifying clients of error");
    uim_reset_globals(uim_ptr);
  }

} /* uim_notify_error */


/*===========================================================================

FUNCTION UIM_SEND_ERR_RPT

DESCRIPTION
  This function, based on the command mode calls the module that sends an
  error report.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The command buffer is released.

===========================================================================*/
void uim_send_err_rpt
(
  uim_cmd_type             *cmd_ptr,  /* the command to queue up */
  uim_instance_global_type *uim_ptr
)
{
  RETURN_IF_INSTANCE_INVALID(uim_ptr->id);

  /* This command has been completed */
  uim_ptr->flag.command_in_progress = FALSE;

  /* Reset the command re-queued flag */
  uim_ptr->flag.command_requeued = FALSE;

  uim_ptr->command.command_requeue_counter = 0;

  /* if SW1SW2 equal to success bytes then make it 0. so that
     we dont report 90X or 91X when uim is in error sate */
  if( uim_ptr->command.rpt_buf.sw1 == (byte) SW1_NORMAL_END ||
      uim_ptr->command.rpt_buf.sw1 == (byte) SW1_END_FETCH ||
      uim_ptr->command.rpt_buf.sw1 == (byte) SW1_END_RESP)
  {
    uim_ptr->command.rpt_buf.sw1 = (byte) SW1_NO_STATUS;
    uim_ptr->command.rpt_buf.sw2 = (byte) SW2_NORMAL_END;
  }


  switch (uim_ptr->state.cmd_mode)
  {
    case UIM_CDMA_CMD:
      uim_send_cdma_err_rpt ( cmd_ptr, uim_ptr);
      break;

    case UIM_GSM_CMD:
      uim_send_gsm_err_rpt (cmd_ptr, uim_ptr);
      break;

    case UIM_USIM_CMD:
      uim_send_usim_err_rpt ( cmd_ptr, uim_ptr);
      break;

    case UIM_ISIM_CMD:
      uim_send_isim_err_rpt ( cmd_ptr, uim_ptr);
      break;

    case UIM_GENERIC_CMD:
    default:
      uim_send_generic_err_rpt (cmd_ptr, uim_ptr);
      break;
  } /* end of switch */
} /* uim_send_err_rpt */


/*===========================================================================

FUNCTION UIM_FLUSH_COMMAND

DESCRIPTION
  This procedure sends an error report to the reqd tasks and releases the
  command buffer in some cases.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Releases the command buffer when uim_send_err_rpt() is not called.

===========================================================================*/
void uim_flush_command
(
  uim_flush_type            flush_type,
  uim_instance_global_type *uim_ptr
)
{
  uim_slot_type     uim_slot            = UIM_SLOT_NONE;
  word              i                   = 0;   /* index */
  int               count               = 0;   /* count */
  uim_cmd_type      *cmd_ptr            = NULL;
  int               total_removed_count = 0;

  /* Pointer to received command */
  uim_rpt_type *back_up_uim_rpt_buf_ptr = NULL;

  RETURN_IF_INSTANCE_INVALID(uim_ptr->id);

  uim_slot = (uim_slot_type)(uim_ptr->id + 1);

  back_up_uim_rpt_buf_ptr = uim_malloc(sizeof(uim_rpt_type));

  if (back_up_uim_rpt_buf_ptr == NULL)
  {
    return;
  }

  /* Back-up the current state of the uim_rpt_buf before flushing commands */
  uim_memscpy(back_up_uim_rpt_buf_ptr,sizeof(uim_rpt_type), &uim_ptr->command.rpt_buf,
         sizeof(uim_rpt_type));

  if (flush_type == UIM_FLUSH_ALL)
  {
    count = q_cnt(&uim_ptr->task.cmd_q);

    /* Command was not attempted when all the commands are flushed */
    uim_ptr->command.rpt_buf.cmd_transacted = FALSE;
  }
  else if (flush_type == UIM_FLUSH_ONLY)
  {
    count = 1;
  }


  for (i = 0; i < count; i++)
  {
    /* get the command from the queue */
    if ((cmd_ptr = (uim_cmd_type *)q_get( &uim_ptr->task.cmd_q )) != NULL)
    {
      /* Get the mode of the command */
      uim_ptr->state.cmd_mode = (uim_command_mode_type)
                     UIM_GET_CMD_MODE((int)cmd_ptr->hdr.command);

      /* Never flush power down and power up command */
      if((cmd_ptr->hdr.slot != uim_slot) ||
         (cmd_ptr->hdr.command == UIM_RESET_F || cmd_ptr->hdr.command == UIM_POWER_DOWN_F))
      {
         /* Check for slot specific command before removing, if it is not belonging
            to given slot then we put it back in queue.  We need to put a mutex here
            to maintain the command sequence for other slot, thru mutex we will block
            other clients to use command queue during this process. */
        /* init link */
        (void)q_link( cmd_ptr, &cmd_ptr->hdr.cmd_hdr.link);
        /* and queue it back */
        q_put(&uim_ptr->task.cmd_q, &cmd_ptr->hdr.cmd_hdr.link);
        continue;
      }

      total_removed_count++;

      /* if the command was queued again, it means that the processing
      of this was started and so we need to send an error report only
      for the first command in the queue */
      if (uim_ptr->flag.command_requeued)
      {
        uim_send_err_rpt(cmd_ptr, uim_ptr);
      }
      /* for the remaining commands, we need to only set the status
         as invalid and signal the task */
      else
      {
        /* return an invalid status in the header to all the commands */
        cmd_ptr->hdr.status = UIM_ERR_S;

        if (cmd_ptr->hdr.cmd_hdr.task_ptr != NULL)
        {
          (void) rex_set_sigs( cmd_ptr->hdr.cmd_hdr.task_ptr,
                               cmd_ptr->hdr.cmd_hdr.sigs );
        }

        /* If always reporting & have a report function, report failure */
        if (((int)cmd_ptr->hdr.options & (int)UIM_OPTION_ALWAYS_RPT) != 0)
        {
          uim_send_err_rpt(cmd_ptr, uim_ptr);
        }
        else
        {  /* return buffer to done queue */
          if ( cmd_ptr->hdr.cmd_hdr.done_q_ptr != NULL )
          {
            /* place command on requested queue */
            q_put( cmd_ptr->hdr.cmd_hdr.done_q_ptr,
                   &cmd_ptr->hdr.cmd_hdr.link );
          }
        } /* (cmd_ptr->hdr.options & UIM_OPTION_ALWAYS_RPT) */
      }  /*  (command_requeued) */
    } /* (cmd_ptr = (uim_cmd_type *)q_get( &uim_ptr->task.cmd_q )) != NULL) */
  }  /* for loop */

  /* Restore the state of the uim_rpt_buf before flushing commands */
  uim_memscpy(&uim_ptr->command.rpt_buf, sizeof(uim_rpt_type),back_up_uim_rpt_buf_ptr, sizeof(uim_rpt_type));

  UIM_FREE(back_up_uim_rpt_buf_ptr);

  /* Clear the command queue signal for the command */
  (void) rex_clr_sigs( uim_ptr->tcb_ptr, UIM_CMD_Q_SIG);
  UIMDRV_MSG_HIGH_4(uim_ptr->id,
                    "Flushing_Type 0x%x Flushing 0x%x commands After flushing Total CMD count 0x%x, Removed count 0x%x",
                     flush_type, count, q_cnt(& uim_ptr->task.cmd_q), total_removed_count);

} /* uim_flush command */


/*===========================================================================

FUNCTION UIM_COMMAND_RESPONSE_CALLBACK

DESCRIPTION
  This procedure is the call back function provided in the command to the
  driver and upon calling this it initiates the processing of the response
  that has been received from the UIM.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Sets the UIM_CMD_RSP_SIG.

===========================================================================*/
void uim_command_response_callback
(
  uim_instance_global_type *uim_ptr
)
{
  /*if it is a bad status word and Feature is enabled trigger recovery*/
  if(TRUE == uim_nv_is_feature_enabled(UIMDRV_FEATURE_RECOVERY_ON_BAD_STATUS_WORD, uim_ptr) &&
     TRUE == uim_ptr->flag.bad_status_words_error)
  {
    UIMDRV_MSG_ERR_0(uim_ptr->id,"recieved bad status word. forcing recovery");
    uim_force_recovery(uim_ptr);
    return;
  }

  uim_clear_cmd_rsp_timer(uim_ptr);

  /* clear the signal as well just in case if it was set */
  (void) rex_clr_sigs( uim_ptr->tcb_ptr, UIM_TRANSACTION_SIG );

  (void) rex_set_sigs( uim_ptr->tcb_ptr, UIM_CMD_RSP_SIG );
} /*  uim_command_response_callback */


/*===========================================================================

FUNCTION UIM_COMMAND_RESPONSE

DESCRIPTION
  This procedure processes the response that has been recd from the UIM.

DEPENDENCIES
  None

RETURN VALUE
  uim_cmd_status

SIDE EFFECTS
  None

===========================================================================*/
static uim_cmd_status_type uim_command_response
(
  uim_rsp_buf_type         *rsp_ptr,
  uim_instance_global_type *uim_ptr
)
{
  uim_cmd_status_type status = UIM_CMD_SUCCESS;

  if( (rsp_ptr == NULL) || (uim_ptr->command.cmd_ptr == NULL) )
  {
    return UIM_CMD_ERR;
  }
  UIMDRV_MSG_HIGH_4(uim_ptr->id,"cmd status 0x%X SW1 0x%x,SW2 0x%x, Response data length 0x%x ",
                    rsp_ptr->cmd_status,
                    rsp_ptr->sw1,
                    rsp_ptr->sw2,
                    rsp_ptr->cmd_rsp_size);

  switch (uim_ptr->state.cmd_mode)
  {
    case UIM_CDMA_CMD:
      status = uim_cdma_command_response( rsp_ptr, uim_ptr);
      break;

    case UIM_GSM_CMD:
      status = uim_gsm_command_response( rsp_ptr, uim_ptr);
      break;

    case UIM_USIM_CMD:
      status = uim_usim_command_response( rsp_ptr, uim_ptr);
      break;

    case UIM_ISIM_CMD:
      status = uim_isim_command_response( rsp_ptr, uim_ptr);
      break;

    case UIM_GENERIC_CMD:
    default:
      status = uim_generic_command_response (rsp_ptr, uim_ptr);
      break;
  }
  return(status);
} /* uim_command_response */


/*===========================================================================

FUNCTION UIM_PROCESS_COMMAND

DESCRIPTION
  This procedure dispatches the command to the respective module based on the
  type of the command if the path to execute the command is correct. If not,
  this function queues the external command back into the head of the command
  queue and issues an internal select to the required path.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void uim_process_command
(
  /* Pointer to received command */
  rex_sigs_type            *mask,
  /* rex signals type mask */
  uim_instance_global_type *uim_ptr
)
{
  /* Flag to indicate if the command processing modules have initiated any
     transactions with the UIM */
  boolean command_transacted = TRUE;
  uim_intermediate_select_type intermediate_select = {0};

  if (uim_ptr->command.cmd_ptr == NULL)
  {
    return;
  }
  (void)memset((void *)&uim_ptr->command.rsp_buf,
               (int)0x00,
               (size_t)sizeof(uim_ptr->command.rsp_buf));

  uim_ptr->command.rpt_buf.slot = uim_ptr->command.cmd_ptr->hdr.slot;
  uim_ptr->card_cmd.cur_channel = uim_ptr->command.cmd_ptr->hdr.channel;

  /* reset 'more time' global */
  if ( TRUE == uim_ptr->command.more_time_pcmd_fetched )
  {
    UIM_MSG_LOW_0("UIM: resetting MORE TIME from card");
    uim_ptr->command.more_time_pcmd_fetched = FALSE;
  }

/* A check is done to ensure the requested command carries the same
   protocol as that established by UIM during power up. This is
   necessary to avoid putting the card in a bad state and cause it to stop
   responding on the protocol negotiated at power up. Certain commands such as
   reset, power down/up and streaming apdu are excluded from this check */
  if ((uim_ptr->state.dfs_1.protocol != UIM_NO_SUCH_MODE)&&
      (uim_ptr->command.cmd_ptr->hdr.protocol != uim_ptr->state.dfs_1.protocol)&&
      (uim_ptr->command.cmd_ptr->hdr.command != UIM_INTERNAL_ME_PUP_F) &&
      (uim_ptr->command.cmd_ptr->hdr.command != UIM_INTERNAL_WAKE_UP_F) &&
      (uim_ptr->command.cmd_ptr->hdr.command != UIM_RESET_F) &&
      (uim_ptr->command.cmd_ptr->hdr.command != UIM_STREAM_APDU_F) &&
      (uim_ptr->command.cmd_ptr->hdr.command != UIM_STREAM_ISO7816_APDU_F) &&
      (uim_ptr->command.cmd_ptr->hdr.command != UIM_RESET_WITH_TP_F) &&
      (uim_ptr->command.cmd_ptr->hdr.command != UIM_RESET_SWITCH_UIM_PASSIVE_F) &&
      (uim_ptr->command.cmd_ptr->hdr.command != UIM_POWER_UP_UIM_PASSIVE_F) &&
      (uim_ptr->command.cmd_ptr->hdr.command != UIM_POWER_DOWN_F) &&
      (uim_ptr->command.cmd_ptr->hdr.command != UIM_HOTSWAP_CARD_REM_F) &&
      (uim_ptr->command.cmd_ptr->hdr.command != UIM_HOTSWAP_CARD_INS_F) &&
      (uim_ptr->command.cmd_ptr->hdr.command != UIM_PREF_SLOT_CHANGE_F) &&
      (uim_ptr->command.cmd_ptr->hdr.command != UIM_MANAGE_CHANNEL_F) &&
      !((uim_ptr->command.cmd_ptr->hdr.command == UIM_SELECT_F) &&
        (uim_ptr->command.cmd_ptr->select.file_type == UIM_ADF)))
  {
    uim_ptr->command.cmd_ptr->hdr.protocol = uim_ptr->state.dfs_1.protocol;
    if (uim_ptr->state.dfs_1.protocol == UIM_ICC)
    {
      uim_ptr->card_cmd.cur_channel = UIM_DEFAULT_CHANNEL;
    }
  }

  /* Set the command mode to Generic Command mode as default */
  uim_ptr->state.cmd_mode = UIM_GENERIC_CMD;

  /* Set the command in progress FLAG to TRUE */
  uim_ptr->flag.command_in_progress = TRUE;

  /* If the command was not requeued */
  if (!uim_ptr->flag.command_requeued)
  {
    /* Let the client know the command is being processed if the client
       provides it's tcb pointer. */
    uim_ptr->command.cmd_ptr->hdr.status = UIM_RECEIVED_S;

    if (uim_ptr->command.cmd_ptr->hdr.cmd_hdr.task_ptr != NULL)
    {
      (void) rex_set_sigs( uim_ptr->command.cmd_ptr->hdr.cmd_hdr.task_ptr,
                           uim_ptr->command.cmd_ptr->hdr.cmd_hdr.sigs );
    }
  }

  uim_need_intermediate_sel(uim_ptr, uim_ptr->card_cmd.cur_channel, &intermediate_select);

  /* If the current directory is current to process the command, then
     process the command */
  if(FALSE == intermediate_select.is_needed)
  {
    /* Set the requeued flag to FALSE since we are going to process the command,
       if the command is an external command */
    if (!uim_ptr->flag.static_buffer_used)
    {
      uim_ptr->flag.command_requeued = FALSE;
      uim_ptr->command.command_requeue_counter = 0;
    }

    /* Get the mode of the command */
    uim_ptr->state.cmd_mode =
    (uim_command_mode_type) UIM_GET_CMD_MODE((int)uim_ptr->command.cmd_ptr->hdr.command);

    switch (uim_ptr->state.cmd_mode)
    {
      case UIM_CDMA_CMD:
        command_transacted = uim_process_cdma_command( uim_ptr);
        break;

      case UIM_GSM_CMD:
        command_transacted = uim_process_gsm_command( uim_ptr);
        break;

      case UIM_USIM_CMD:
        command_transacted = uim_process_usim_command( uim_ptr);
        break;

      case UIM_ISIM_CMD:
        command_transacted = uim_process_isim_command( uim_ptr);
        break;

      case UIM_GENERIC_CMD:
      default:
        command_transacted = uim_process_generic_command (uim_ptr);
        break;
    }  /* end for switch */
  }
  else
  {
    /* Get the protocol from the command for an internal select */
    uim_ptr->command.static_cmd_buf.hdr.protocol = uim_ptr->command.cmd_ptr->hdr.protocol;

    /* Get the channel from the command for an internal select */
    uim_ptr->command.static_cmd_buf.hdr.channel = uim_ptr->command.cmd_ptr->hdr.channel;

    /* Put the command back in the head of the queue so that
       it can be processed again */
    uim_ptr->flag.command_requeued = TRUE;
    uim_ptr->command.command_requeue_counter++;

    if (uim_ptr->flag.command_requeued &&
        uim_ptr->command.command_requeue_counter > UIM_MAX_NUM_ATTEMPTS)
    {
      if (mask != NULL)
      {
        /* include the command Q sig to the mask */
        *mask |= UIM_CMD_Q_SIG;
      }
      uim_send_err_rpt(uim_ptr->command.cmd_ptr, uim_ptr);
      return;
    }

    /* Keeping track that an internal command is being used */
    uim_ptr->flag.internal_command_used = TRUE;

    if ( q_cnt( & uim_ptr->task.cmd_q ) == 0 )
    {
      q_put( & uim_ptr->task.cmd_q, & uim_ptr->command.cmd_ptr->hdr.cmd_hdr.link );
    }
    else
    {
      q_insert( & uim_ptr->task.cmd_q,
                & uim_ptr->command.cmd_ptr->hdr.cmd_hdr.link,
                & ((uim_cmd_type*) q_check( & uim_ptr->task.cmd_q ))->hdr.cmd_hdr.link );
    }

    /* Build an internal select command */
    uim_ptr->flag.static_buffer_used  = TRUE;

    uim_ptr->command.static_cmd_buf.hdr.command                = UIM_SELECT_F;
    uim_ptr->command.static_cmd_buf.hdr.cmd_hdr.task_ptr       = uim_ptr->tcb_ptr;
    uim_ptr->command.static_cmd_buf.hdr.cmd_hdr.sigs           = NULL;
    uim_ptr->command.static_cmd_buf.hdr.cmd_hdr.link.next_ptr  = NULL;
    uim_ptr->command.static_cmd_buf.hdr.cmd_hdr.done_q_ptr     = NULL;
    uim_ptr->command.static_cmd_buf.hdr.rpt_function           = NULL;

    uim_ptr->command.static_cmd_buf.select.is_intermediate_sel = TRUE;
    uim_ptr->command.static_cmd_buf.select.path                = intermediate_select.path;
    if ((intermediate_select.aid.aid_length > 0) &&
        (intermediate_select.path.len == 2) &&
        (intermediate_select.path.path[0] == UIM_MF_ID) &&
        (intermediate_select.path.path[1] == UIM_SEL_AID_BY_FILE_ID))
    {
      uim_ptr->command.static_cmd_buf.select.file_type        = UIM_ADF;
      uim_memscpy(
        &uim_ptr->command.static_cmd_buf.select.aid,
        sizeof(uim_aid_type),
        &intermediate_select.aid,
        sizeof(uim_aid_type));
    }
    else
    {
      uim_ptr->command.static_cmd_buf.select.file_type           = UIM_EF;
    }

    /* This is the first attempt for this command  */
    uim_ptr->command.static_cmd_buf.hdr.cmd_count = 1;
    /* Make command pointer point to static command buffer */
    uim_ptr->command.cmd_ptr = &uim_ptr->command.static_cmd_buf;

    command_transacted = uim_process_generic_command( uim_ptr);
  }

  /* If the return status from the command processing module is FALSE indicating
     that the command was rejected and the command has not been transacted with
     the UIM, it is assumed that an error report has been sent to the task and
     so we can start processing other commands from the command queue

     OR if the command is a PREF SLOT Change command which does not interact
     with the R-UIM card */

  if ((!command_transacted) || (uim_ptr->command.cmd_ptr->hdr.command == UIM_PREF_SLOT_CHANGE_F))
  {

    if (mask != NULL)
    {
      /* include the command Q sig to the mask */
      *mask |= UIM_CMD_Q_SIG;
    }
    /* Clear the flag */
    uim_ptr->flag.command_in_progress = FALSE;
  }

  /* Set the command transacted field in the report buffer */
  uim_ptr->command.rpt_buf.cmd_transacted = command_transacted;

} /* uim_process_command */


/*===========================================================================

FUNCTION UIM_GET_CDMA_SVC_TABLE_ITEM

DESCRIPTION
  This function checks availabilities for a specified service.

DEPENDENCIES
  The CDMA service table has to be read first.

RETURN VALUE
  TRUE :   The service is activated.
  FALSE:   The service is unavailable.

SIDE EFFECTS
  None.

===========================================================================*/

uim_svc_table_return_type uim_return_cdma_svc_availabililty
(
  uim_cdma_svc_table_enum_type item,  /* The service to be checked*/
  const byte                   *table /* Pointer to the service table */
)
{
  uim_svc_table_return_type svc;
  svc.activated = FALSE;
  svc.allocated = FALSE;

  switch (item)
  {
    case UIM_CDMA_SVC_CHV_DISABLE:
      /* CHV disable function */
      if (table[0] & UIM_CDMA_SVC_CHV_DISABLE_ALLOCATED_MASK)
      {
        svc.allocated = TRUE;
        if (table[0] & UIM_CDMA_SVC_CHV_DISABLE_ACTIVATED_MASK)
          svc.activated = TRUE;
      }
      break;

    case UIM_CDMA_SVC_ADN:
      /* Abbreviated Dialling Numbers (ADN) */
      if (table[0] & UIM_CDMA_SVC_ADN_ALLOCATED_MASK)
      {
        svc.allocated = TRUE;
        if (table[0] & UIM_CDMA_SVC_ADN_ACTIVATED_MASK)
          svc.activated = TRUE;
      }
      break;

    case UIM_CDMA_SVC_FDN:
      /* Fixed Dialling Numbers (FDN) */
      if (table[0] & UIM_CDMA_SVC_FDN_ALLOCATED_MASK)
      {
        svc.allocated = TRUE;
        if (table[0] & UIM_CDMA_SVC_FDN_ACTIVATED_MASK)
          svc.activated = TRUE;
      }
      break;

    case UIM_CDMA_SVC_SMS:
      /* Short Message Storage (SMS) */
      if (table[0] & UIM_CDMA_SVC_SMS_ALLOCATED_MASK)
      {
        svc.allocated = TRUE;
        if (table[0] & UIM_CDMA_SVC_SMS_ACTIVATED_MASK)
          svc.activated = TRUE;
      }
      break;

    case UIM_CDMA_SVC_EXT1:
      /* Extension 1 */
      if (table[2] & UIM_CDMA_SVC_EXT1_ALLOCATED_MASK)
      {
        svc.allocated = TRUE;
        if (table[2] & UIM_CDMA_SVC_EXT1_ACTIVATED_MASK)
          svc.activated = TRUE;
      }
      break;

    case UIM_CDMA_SVC_EXT2:
      /* Extension 2 */
      if (table[2] & UIM_CDMA_SVC_EXT2_ALLOCATED_MASK)
      {
        svc.allocated = TRUE;
        if (table[2] & UIM_CDMA_SVC_EXT2_ACTIVATED_MASK)
          svc.activated = TRUE;
      }
      break;

    case UIM_CDMA_SVC_SMS_PARAMS:
      /* SMS Parameters */
      if (table[2] & UIM_CDMA_SVC_SMS_PARAMS_ALLOCATED_MASK)
      {
        svc.allocated = TRUE;
        if (table[2] & UIM_CDMA_SVC_SMS_PARAMS_ACTIVATED_MASK)
          svc.activated = TRUE;
      }
      break;

    case UIM_CDMA_SVC_LND:
      /* Last Number Dialled (LND) */
      if (table[3] & UIM_CDMA_SVC_LND_ALLOCATED_MASK)
      {
        svc.allocated = TRUE;
        if (table[3] & UIM_CDMA_SVC_LND_ACTIVATED_MASK)
          svc.activated = TRUE;
      }
      break;

    case UIM_CDMA_SVC_HOME_SVC_PROVIDER_NAME:
      /* CDMA Home Service Provider Name */
      if (table[4] & UIM_CDMA_SVC_HOME_SVC_PVDR_NAME_ALLOCATED_MASK)
      {
        svc.allocated = TRUE;
        if (table[4] & UIM_CDMA_SVC_HOME_SVC_PVDR_NAME_ACTIVATED_MASK)
          svc.activated = TRUE;
      }
      break;

    case UIM_CDMA_SVC_SDN:
      /* Service Dialling Numbers */
      if (table[4] & UIM_CDMA_SVC_SDN_ALLOCATED_MASK)
      {
        svc.allocated = TRUE;
        if (table[4] & UIM_CDMA_SVC_SDN_ACTIVATED_MASK)
          svc.activated = TRUE;
      }
      break;

    case UIM_CDMA_SVC_EXT3:
      /* Extension 3 */
      if (table[4] & UIM_CDMA_SVC_EXT3_ALLOCATED_MASK)
      {
        svc.allocated = TRUE;
        if (table[4] & UIM_CDMA_SVC_EXT3_ACTIVATED_MASK)
          svc.activated = TRUE;
      }
      break;

#ifdef FEATURE_UIM_TOOLKIT_UTK
    case UIM_CDMA_SVC_SMS_PP_DOWNLOAD:
      /* Check CDMA SMS PP download service */
      if ( table[6] & UIM_CDMA_SVC_SMS_PP_DOWNLOAD_ALLOCATED_MASK )
      {
        svc.allocated = TRUE;
        if (table[6] & UIM_CDMA_SVC_SMS_PP_DOWNLOAD_ACTIVATED_MASK)
          svc.activated = TRUE;
      }
      break;

    case UIM_CDMA_SVC_MENU_SEL:
      /* Check menu selection service */
      if ( table[6] & UIM_CDMA_SVC_MENU_SEL_ALLOCATED_MASK )
      {
        svc.allocated = TRUE;
        if (table[6] & UIM_CDMA_SVC_MENU_SEL_ACTIVATED_MASK)
          svc.activated = TRUE;
      }
      break;

    case UIM_CDMA_SVC_PROACTIVE_UIM:
      /* Check proactive uim service */
      if ( table[7] & UIM_CDMA_SVC_PROACTIVE_UIM_ALLOCATED_MASK )
      {
        svc.allocated = TRUE;
        if (table[7] & UIM_CDMA_SVC_PROACTIVE_UIM_ACTIVATED_MASK)
          svc.activated = TRUE;
      }
      break;
#endif /* FEATURE_UIM_TOOLKIT_UTK */

    case UIM_CDMA_SVC_3GPD_SIP:
      /* Check 3GPD SIP service */
      if ( table[4] & UIM_CDMA_SVC_3GPD_SIP_ALLOCATED_MASK )
      {
        svc.allocated = TRUE;
        if (table[4] & UIM_CDMA_SVC_3GPD_SIP_ACTIVATED_MASK)
          svc.activated = TRUE;
      }
      break;

    case UIM_CDMA_SVC_3GPD_MIP:
      /* Check 3GPD MIP service */
      if ( table[9] & UIM_CDMA_SVC_3GPD_MIP_ALLOCATED_MASK )
      {
        svc.allocated = TRUE;
        if (table[9] & UIM_CDMA_SVC_3GPD_MIP_ACTIVATED_MASK)
          svc.activated = TRUE;
      }
      break;

    case UIM_CDMA_SVC_HRPD:
      /* Check HRPD service */
      if ( table[1] & UIM_CDMA_SVC_HRPD_ALLOCATED_MASK )
      {
        svc.allocated = TRUE;
        if (table[1] & UIM_CDMA_SVC_HRPD_ACTIVATED_MASK)
          svc.activated = TRUE;
      }
      break;

    case UIM_CDMA_SVC_3GPD_MSG_EXT:
      /* Check 3GPD / Messaging Extensions service */
      if ( table[3] & UIM_CDMA_SVC_3GPD_MSG_ALLOCATED_MASK )
      {
        svc.allocated = TRUE;
        if (table[3] & UIM_CDMA_SVC_3GPD_MSG_ACTIVATED_MASK)
          svc.activated = TRUE;
      }
      break;

    case UIM_CDMA_SVC_BCSMS:
      /* Check BCSMS service */
      if ( table[3] & UIM_CDMA_SVC_BCSMS_ALLOCATED_MASK )
      {
        svc.allocated = TRUE;
        if (table[3] & UIM_CDMA_SVC_BCSMS_ACTIVATED_MASK)
          svc.activated = TRUE;
      }
      break;

    case UIM_CDMA_SVC_RC:
      /* Check Root Certificates service */
      if ( table[3] & UIM_CDMA_SVC_RC_ALLOCATED_MASK )
      {
        svc.allocated = TRUE;
        if (table[3] & UIM_CDMA_SVC_RC_ACTIVATED_MASK)
          svc.activated = TRUE;
      }
      break;

    case UIM_CDMA_SVC_BROWSER:
      /* Check Browser service */
      if ( table[5] & UIM_CDMA_SVC_BROWSER_ALLOCATED_MASK )
      {
        svc.allocated = TRUE;
        if (table[5] & UIM_CDMA_SVC_BROWSER_ACTIVATED_MASK)
          svc.activated = TRUE;
      }
      break;

    case UIM_CDMA_SVC_JAVA:
      /* Check Java service */
      if ( table[5] & UIM_CDMA_SVC_JAVA_ALLOCATED_MASK )
      {
        svc.allocated = TRUE;
        if (table[5] & UIM_CDMA_SVC_JAVA_ACTIVATED_MASK)
          svc.activated = TRUE;
      }
      break;

    case UIM_CDMA_SVC_BREW:
      /* Check BREW service */
      if ( table[5] & UIM_CDMA_SVC_BREW_ALLOCATED_MASK )
      {
        svc.allocated = TRUE;
        if (table[5] & UIM_CDMA_SVC_BREW_ACTIVATED_MASK)
          svc.activated = TRUE;
      }
      break;

    case UIM_CDMA_SVC_LBS:
      /* Check LBS service */
      if ( table[5] & UIM_CDMA_SVC_LBS_ALLOCATED_MASK )
      {
        svc.allocated = TRUE;
        if (table[5] & UIM_CDMA_SVC_LBS_ACTIVATED_MASK)
          svc.activated = TRUE;
      }
      break;

    case UIM_CDMA_IPV6:
      /* Check IPv6 service */
      if ( table[7] & UIM_CDMA_IPV6_ALLOCATED_MASK )
      {
        svc.allocated = TRUE;
        if (table[7] & UIM_CDMA_IPV6_ACTIVATED_MASK)
          svc.activated = TRUE;
      }
      break;

     default:
      /* This function gets called in NON-UIM context always. UIM instance does
         not matter. Keeping instance as 0 */
      UIM_MSG_MED_0("No such service!");
      break;
  } /* end of switch */

  /* Return the result of the query */
  return svc;
} /* uim_return_cdma_svc_availabililty */

/*lint -e{715}
  uim_power_control API cannot be changed
*/
/* DUMMY API for external modules */
/*===========================================================================

FUNCTION UIM_POWER_CONTROL

DESCRIPTION
  This procedure is the voting interface for modules to indicate their pref as
  to whether they would like to keep the UIM powered ON or OFF.

DEPENDENCIES
  Poll time delay to be used.

RETURN VALUE
  None.

SIDE EFFECTS
  Internally sets the UIM_TOGLLE_STATE_SIG and could change the state of UIM
  as a result.

===========================================================================*/
dword uim_power_control
(
  uim_voter_type         mask,
  boolean                uim_reqd
)
{
  /* JS TODO this needs to be sorted out with MMOC and 1x-CP people.
     For now hard code the instance to 1 */
  if(uim_get_instance_ptr(UIM_INSTANCE_1))
  {
    return uim_get_instance_ptr(UIM_INSTANCE_1)->setting.poll_time_delay;
  }
  return UIM_POLL_TIME;
} /* uim_power_control */


/*===========================================================================

FUNCTION UIM_POWER_CONTROL_DS

DESCRIPTION
  This procedure is the voting interface for modules to indicate their pref as
  to whether they would like to keep the UIM powered ON or OFF.

DEPENDENCIES
  Poll time delay to be used.

RETURN VALUE
  None.

SIDE EFFECTS
  Internally sets the UIM_TOGLLE_STATE_SIG and could change the state of UIM
  as a result.
===========================================================================*/
dword uim_power_control_ds
(
  uim_slot_type           slot,
  uim_voter_type          mask,
  boolean                 uim_reqd,
  uint16                  uim_call_mask
)
{
  uim_instance_global_type *uim_ptr      = NULL;
  dword new_uim_poll_time_delay;

  /*This boolean is used to decide to keep running
     the poll timer or reset it in middle because of
     TRAFFIC CHANNEL COMMAND.
   */
  boolean uim_send_poll_signal = FALSE;
  uim_ptr = uim_get_instance_ptr((uim_instance_enum_type)(slot - 1));

  if(uim_ptr == NULL)
  {
    return (UIM_POLL_TIME);
  }


  if( UIM_ERR_S == uim_ptr->state.status )
  {
    if (uim_reqd)
    {	    
      UIMDRV_MSG_ERR_0(uim_ptr->id,"Returning from uim_power_control_ds:  slot is in UIM_ERR_S");
    }
    return uim_ptr->setting.poll_time_delay;
  }

  /* Is the vote to on or off */
  if (uim_reqd)
  {
    /* set the bit corresponding to the voter */
    uim_ptr->state.ctl = (uim_voter_type) ((int)uim_ptr->state.ctl | (int)mask );
  }
  else
  {
   if ((int)uim_ptr->state.ctl & UIM_TRAF_CHAN)
   {
     /* call type mask ZERO indicates no ongoing call  */
     uim_ptr->state.uim_call_info.call_mask = 0;
     uim_ptr->poll.presence_detection_state = UIM_PD_POLLING_OFF;
   }
    /* clear the bit corresponding to the voter */
    uim_ptr->state.ctl = (uim_voter_type) ((int)uim_ptr->state.ctl ^ ((int)uim_ptr->state.ctl & (int)mask ));
  }
  if (uim_ptr->state.ctl != UIM_NONE)
  {
    uim_ptr->flag.powerdown = FALSE;
  }
  else
  {
    uim_ptr->flag.powerdown = TRUE;
  }

  /* Check if MC or TC has voted the UIM on.  If so, it because the ME is in the
     traffic channel. */
  if ((int)uim_ptr->state.ctl & UIM_TRAF_CHAN)
  {
    if(uim_call_mask != UIM_CALL_TYPE_NOT_APPLICABLE)
    {
      uim_ptr->state.uim_call_info.call_mask = uim_call_mask;
      uim_ptr->poll.presence_detection_state = UIM_PD_POLLING_ACTIVE;
    }
    if (UIM_JCDMA_SERVICE_ENABLED == uim_ptr->setting.jcdma_mode.service)
    {
      /* 5 seconds interval for traffic channel */
      new_uim_poll_time_delay = UIM_JCDMA_POLL_INTERVAL;
    }
    else
    {
     /* Set the polling timer to be the lower of the proactive duration and the
          in traffic duration. */
#ifdef FEATURE_UIM_USB_UICC
      if (uim_is_usb_enabled(uim_ptr) && uim_usb_is_usb_interface_selected(uim_ptr))
      {
        new_uim_poll_time_delay = MIN( uim_ptr->setting.proactive_time_delay,
                                    UIM_USB_POLL_TIME );
      }
      else
#endif /* FEATURE_UIM_USB_UICC */
      {
        new_uim_poll_time_delay = MIN( uim_ptr->setting.proactive_time_delay,
                                     UIM_POLL_TIME );
      }
    }
  }
  else /* We are in the idle state and not in traffic channel */
  {
    /* Set the polling timer duration to be the proactive timer duration */
    new_uim_poll_time_delay = uim_ptr->setting.proactive_time_delay;
  }

  /* Determine if the poll interval has changed */
  if (new_uim_poll_time_delay != uim_ptr->setting.poll_time_delay)
  {
    /* Reset the wake up poll offset to 1500 */
    uim_ptr->setting.wakeup_poll_offset = UIM_WAKE_UP_POLL_OFFSET;
    /* Set the poll timer to the new delay */
    uim_set_poll_timer( new_uim_poll_time_delay,
                        uim_ptr->setting.wakeup_poll_offset,
                        uim_ptr);
    uim_send_poll_signal = TRUE;
    UIMDRV_MSG_MED_0(uim_ptr->id, "Setting UIM poll timer to new delay");
  }

  (void) rex_set_sigs(uim_ptr->tcb_ptr, UIM_STATE_TOGGLE_SIG);

  /* Start the polling as soon as we get to the traffic channel */
  if ( (int)uim_ptr->state.ctl & UIM_TRAF_CHAN )
  {
    if(uim_nv_is_feature_enabled(UIMDRV_FEATURE_UIM_POLLING_ONLY_AT_POLLING_TIMER_EXPIRY,
                                    uim_ptr) == TRUE)
    {
      if(!timer_is_active(&uim_ptr->poll.wake_up_poll_timer))
      {
        /* Set the poll timer to the new delay */
        uim_set_poll_timer( new_uim_poll_time_delay,
                            uim_ptr->setting.wakeup_poll_offset,
                            uim_ptr);
      }
    }
    else
    {
      (void) rex_set_sigs(uim_ptr->tcb_ptr, UIM_POLL_TIMER_SIG);
    }
  }
  UIMDRV_MSG_HIGH_3 ( uim_ptr->id,"uim power control ds return with call type mask 0x%x state.ctl 0x%x uim_send_poll_signal 0x%x",
                      uim_ptr->state.uim_call_info.call_mask,
                      uim_ptr->state.ctl,
                      uim_send_poll_signal);

  return uim_ptr->setting.poll_time_delay;
} /* uim_power_control_ds */


/*=====================================================================
FUNCTION UIM_SET_PROACTIVE_UIM_SLOT

DESCRIPTION
  This procedure is called by GSDI\GSTK to indicate to UIM if the underlying UIM is pro-active or not.

PARAMETERS
  boolean:    To Indicate the UIM is proactive or not.
  slot:       Slot for which the info is provided.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  Internally sets the UIM buffer.
======================================================================*/
void uim_set_proactive_uim_slot(
  boolean        proactive_uim,
  uim_slot_type  slot
)
{
  uim_instance_global_type *uim_ptr = uim_get_instance_ptr(((uim_instance_enum_type)slot-1));

  if ((uim_ptr == NULL) || (UIM_SLOT_NONE == slot) || ( slot >= UIM_SLOT_AUTOMATIC))
  {
    return;
  }
  uim_ptr->state.type.proactive_uim = proactive_uim;
  uim_ptr->state.type.init          = TRUE;
  return;
} /* uim_set_proactive_uim_slot */


/*======================================================================

FUNCTION UIM_GET_UIM_STATUS

DESCRIPTION
  This procedure retrieves the current state of UIM.

PARAMETERS
  slot:       Slot for which the info is provided.

DEPENDENCIES
  None

RETURN VALUE
  Returns uim_status.

SIDE EFFECTS
  None

======================================================================*/
uim_status_type uim_get_uim_slot_status(
  uim_slot_type  slot
)
{
  uim_instance_global_type *uim_ptr = uim_get_instance_ptr((uim_instance_enum_type)slot-1);

  if ((uim_ptr == NULL) || (UIM_SLOT_NONE == slot) || ( slot >= UIM_SLOT_AUTOMATIC))
  {
    UIM_MSG_ERR_1("Invalid Slot Reference 0x%x", slot);
    return UIM_NO_SUCH_STATUS_S;
  }

  return uim_ptr->state.status;
} /* uim_get_uim_slot_status */


/*===========================================================================

FUNCTION UIM_SET_PROACTIVE_UIM

DESCRIPTION
  This procedure is called by UI/any entity that read the CDMA servive table
  to indicate to UIM if the underlying UIM is pro-active or not.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  Internally sets the UIM buffer.
===========================================================================*/
void uim_set_proactive_uim
(
  boolean proactive_uim
)
{
  uim_set_proactive_uim_slot( proactive_uim, UIM_SLOT_1);
} /* uim_set_proactive_uim_info */

/*===========================================================================

FUNCTION UIM_SET_PROACTIVE_UIM_2

DESCRIPTION
  This procedure is called by UI/any entity that read the CDMA servive table
  to indicate to UIM if the underlying UIM is pro-active or not.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  Internally sets the UIM buffer.
===========================================================================*/
void uim_set_proactive_uim_2
(
  boolean proactive_uim
)
{
  uim_set_proactive_uim_slot( proactive_uim, UIM_SLOT_2);
} /* uim_set_proactive_uim_2 */


/*===========================================================================

FUNCTION UIM_RESET_DIR_FOR_SLOT_CHANNEL

DESCRIPTION
  This procedure resets all the members of the last selected directory
  to their defaults for the respective slot on the respective channel.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Resets the directory structure for the slot specified.

===========================================================================*/
void uim_reset_dir_for_slot_channel
(
  uim_channel_type          channel,
  uim_instance_global_type *uim_ptr
)
{
  if (channel >= UIM_MAX_CHANNELS)
  {
    return;
  }

  /* clears path specific info only */
  uim_clear_current_path(uim_ptr, channel);
  /* clear the app data in addition to path data */
  memset(uim_ptr->state.current_path[channel].curr_app.data, 0x00,
          UIM_AID_MAX_NAME_LENGTH);
  uim_ptr->state.current_path[channel].curr_app.aid_length  = 0;
  uim_ptr->state.current_path[channel].empty_aid_is_active  = FALSE;

} /* uim_reset_dir_for_slot_channel */


/*===========================================================================

FUNCTION UIM_RESET_DIR

DESCRIPTION
  This procedure resets all the members of the last selected directory
  to their defaults.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Resets the directory structure.

===========================================================================*/
void uim_reset_dir
(
  uim_instance_global_type *uim_ptr
)
{
  int i = 0;

  for(i = 0; i < (int) UIM_MAX_CHANNELS; i++)
  {
    uim_clear_current_path(uim_ptr, i);
    memset(uim_ptr->state.current_path[i].curr_app.data, 0x00, UIM_AID_MAX_NAME_LENGTH);
    uim_ptr->state.current_path[i].curr_app.aid_length = 0;
    uim_ptr->state.current_path[i].empty_aid_is_active = FALSE;
  }
} /* uim_reset_dir */


/*===========================================================================

FUNCTION UIM_DISABLE_RECOVERY

DESCRIPTION
  This procedure disables recovery in uim.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void uim_disable_recovery

(
   uim_slot_type  uim_slot,
   boolean  disable_recovery
)
{
  uim_instance_global_type *uim_ptr;
  if(UIM_SLOT_1 == uim_slot)
  {
    uim_ptr = uim_get_instance_ptr(UIM_INSTANCE_1);
  }
  else if (UIM_SLOT_2 == uim_slot)
  {
    uim_ptr = uim_get_instance_ptr(UIM_INSTANCE_2);
  }
  else if (UIM_SLOT_3 == uim_slot)
  {
    uim_ptr = uim_get_instance_ptr(UIM_INSTANCE_3);
  }
  else
  {
    return;
  }

  uim_ptr->flag.runtime_disable_recovery = disable_recovery;
  UIMDRV_MSG_HIGH_1(uim_ptr->id,"runtime_disable_recovery is 0x%x",
                    uim_ptr->flag.runtime_disable_recovery);
} /* uim_disable_recovery */


/*===========================================================================

FUNCTION UIM_RECOVERY_DISABLED

DESCRIPTION
  This procedure disables uim recovery. Sets uim_status to UIM_ERR_S and
  notifies clients of the error if its not done already. Powers down the
  uim interface and sets the UIM_CMD_Q_SIG in the i_mask.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void uim_recovery_disabled
(
  uim_instance_global_type *uim_ptr,
  rex_sigs_type            *mask
)
{
  if (mask == NULL)
  {
    return;
  }

  uim_ptr->flag.command_in_progress = FALSE;

  /* To take care of the other commands queued in our queue as well as
     of reporting err to the clients if needed */
  uim_flush_command(UIM_FLUSH_ALL, uim_ptr);

  if (uim_ptr->state.status == UIM_ERR_S)
  {
    UIMDRV_MSG_MED_0(uim_ptr->id,"Not notifying clients of error as uim_status already set to UIM_ERR_S");
  }
  else
  {
     uim_ptr->state.status = UIM_ERR_S;
     uim_notify_error(UIM_ERR_S, uim_ptr);
  }
  uim_power_down(uim_ptr);
  *mask |= UIM_CMD_Q_SIG;

} /* uim_recovery_disabled */


/*===========================================================================

FUNCTION UIM_RESET_UIM

DESCRIPTION
  This procedure builds the static command for power up and issues it. It
  issues different command based on whether the reset is for a wake up or
  a power up.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void uim_reset_uim
(
  rex_sigs_type            *mask,
  boolean                   me_powerup,
  uim_instance_global_type *uim_ptr
)
{
  gstk_slot_id_enum_type gstk_slot = GSTK_SLOT_AUTO;

  if ( mask == NULL )
  {
    return;
  }
  RETURN_IF_INSTANCE_INVALID(uim_ptr->id);
  UIMDRV_MSG_HIGH_1(uim_ptr->id,"Internal command to Reset the UIM for slot 0x%x",
                    uim_ptr->command.static_cmd_buf.hdr.slot);

  /* Set the count of technical problems to zero */
  uim_ptr->card_cmd.inc_tech_prob_count = 0;

  if ( TRUE == uim_ptr->flag.uim_not_allow_local_card_power_up)
  {
    /* No Need to power up the local cardt */
    UIMDRV_MSG_HIGH_0(uim_ptr->id,
                      "uim_reset_uim is not allowed on local card");
    if (TRUE == uim_ptr->flag.task_startup)
    {
      /* UIM need to reset the flag here As we are terminating internal
         ME power up command in middle for no card case */
      uim_ptr->flag.me_powerup = FALSE;
    }
    uim_ptr->state.status = UIM_ERR_S;
    uim_ptr->flag.command_in_progress = FALSE;
    uim_ptr->flag.uim_not_allow_local_card_power_up = FALSE;	  
    *mask |= UIM_CMD_Q_SIG;
    return;
  }

/* Before power up that slot first check card status, card is there or not */
  if (uim_ptr->hotswap.card_status != UIM_CARD_INSERTED &&
      uim_hw_if.intctrl[uim_ptr->id].m_hs_enable == TRUE)
  {
    /* If it is flaky hw, don't beleive on the card status and try power-up */
    if (uim_ptr->hotswap.hotswap_flaky_switch_support)

    {
      UIMDRV_MSG_HIGH_0(uim_ptr->id," Setting suspicious card removed signal during power-up ");
      (void) rex_set_sigs( uim_ptr->tcb_ptr, UIM_SUSPICIOUS_CARD_REM_SIG);
    }
    else
    {
      /* No Card, then no need to do the uim reset for that slot */
      UIMDRV_MSG_HIGH_0(uim_ptr->id,
                      "uim_reset_uim is not allowed, card is either not inserted or not detected on this slot");
      if (TRUE == uim_ptr->flag.task_startup)
      {
        /* UIM need to reset the flag here As we are terminating internal
           ME power up command in middle for no card case */
        uim_ptr->flag.me_powerup = FALSE;
      }
      if (uim_ptr->state.status == UIM_UNINITIALIZED_S)
      {
        uim_notify_error(UIM_CARD_REMOVED_S, uim_ptr);
      }
      uim_ptr->state.status = UIM_ERR_S;
      /* Check for card status */
      UIM_DETECT_CARD_STATUS(uim_ptr);
      /* In case Slot1 does not have the card, check for Slot2 */
      /* Single slot scenario where it does not have the card */
      /* This is back up code for Recovery,
         where recovery is end in middle, as card is removed */
      uim_ptr->flag.command_in_progress = FALSE;
      *mask |= UIM_CMD_Q_SIG;
      return;
    }
  }

  /* Indicate that we did not receive the ATR */
  uim_ptr->atr.atr_received = FALSE;

  /* reset the voltage_class_known_from_atr varialbe */
  uim_ptr->flag.voltage_class_known_from_atr = FALSE;
  if (me_powerup)
  {
    /* Always perform a cold reset */
    if (uim_hw_if.clkreg[uim_ptr->id].m_simClkFreq)
    {
      uim_power_down(uim_ptr);
#ifndef TEST_FRAMEWORK
      uim_timed_sleep(UIM_ISO7816_VOLTAGE_SWITCH_PWR_DOWN_DELAY,
                      uim_ptr,
                      UIM_ALL_ZERO_SIG);
#endif /* TEST_FRAMEWORK */
    }
    /* command to power up the UIM due to task start up */
    if (uim_ptr->command.static_cmd_buf.hdr.command == UIM_HOTSWAP_CARD_INS_F)
    {
      UIMDRV_MSG_LOW_1(uim_ptr->id,"HOTSWAP: uim_reset_uim due to card inserted command for slot 0x%x",
                       uim_ptr->command.static_cmd_buf.hdr.slot);
    }
    uim_ptr->command.static_cmd_buf.hdr.command = UIM_INTERNAL_ME_PUP_F;
  }
  else
  {
    /* command to power up the UIM due to power management */
    uim_ptr->command.static_cmd_buf.hdr.command = UIM_INTERNAL_WAKE_UP_F;
    if(TRUE == uim_ptr->flag.runtime_disable_recovery)
    {
      uim_recovery_disabled(uim_ptr, mask);
      return;
    }

    if(FALSE == uim_ptr->flag.recovery_triggered)
    {
      /* Set command count to 0 for first recovery attempt */
      uim_ptr->command.static_cmd_buf.hdr.cmd_count = 0;
    }

    uim_ptr->flag.recovery_triggered = TRUE;

    /* Always perform a cold reset */
    if (uim_hw_if.clkreg[uim_ptr->id].m_simClkFreq)
    {
      uim_power_down(uim_ptr);
#ifndef TEST_FRAMEWORK
      uim_timed_sleep(UIM_ISO7816_VOLTAGE_SWITCH_PWR_DOWN_DELAY,
                      uim_ptr,
                      UIM_ALL_ZERO_SIG);
#endif /* TEST_FRAMEWORK */
    }

    /* Recovery in progress - switch CLK back to 3.8MHz if CLK is currently 4.8MHz */
    if (UIM_IS_CLK_FREQ_AT_4_8_MHZ(uim_ptr->id))
    {
      UIM_SWITCH_CLK_FREQ_TO_3_8_MHZ(uim_ptr->id);
      SET_UIM_BAUD_RATE_SLOT ( uim_ptr->id, UIM_CRCF_372_1, UIM_BRAF_1);
    }

    if (uim_ptr->flag.poll_pending)
    {
      uim_set_poll_timer(uim_ptr->setting.poll_time_delay,
                         uim_ptr->setting.wakeup_poll_offset,
                         uim_ptr);
      /* Reset the flag */
      uim_ptr->flag.poll_pending = FALSE;
    }

    /* Notify GSTK about recovery */
    gstk_slot = (gstk_slot_id_enum_type)(uim_ptr->id + 1);

    uim_sm_gstk_uim_set_recovery_sig(gstk_slot);
  }

  /* clear the signal as well just in case if it was set */
  (void) rex_clr_sigs( uim_ptr->tcb_ptr, UIM_TRANSACTION_SIG );

  /* The status of UIM is set to Uninitialized since reset the UIM */
  uim_ptr->state.status = UIM_UNINITIALIZED_S;
  /* Clean out the last selected directory as we will reset the card. */
  uim_reset_dir(uim_ptr);
  /* !!!!! Do not clear the ATCSIM directory structure !!!!! */

  /* Remove the command q signal from the mask */
  if (mask != NULL)
    *mask &= ~UIM_CMD_Q_SIG;

  uim_ptr->flag.static_buffer_used  = TRUE;

  uim_ptr->flag.internal_command_used = FALSE;

  uim_ptr->command.static_cmd_buf.hdr.cmd_hdr.task_ptr = uim_ptr->tcb_ptr;
  uim_ptr->command.static_cmd_buf.hdr.cmd_hdr.sigs = NULL;
  uim_ptr->command.static_cmd_buf.hdr.cmd_hdr.link.next_ptr = NULL;
  uim_ptr->command.static_cmd_buf.hdr.cmd_hdr.done_q_ptr = NULL;
  uim_ptr->command.static_cmd_buf.hdr.rpt_function = NULL;
  uim_ptr->command.static_cmd_buf.hdr.channel = UIM_CHANNEL0;
  /* Make command pointer point to static command buffer */
  uim_ptr->command.cmd_ptr = &uim_ptr->command.static_cmd_buf;

  if(uim_ptr->command.static_cmd_buf.hdr.command == UIM_INTERNAL_WAKE_UP_F)
  {
    if((uim_ptr->command.static_cmd_buf.hdr.cmd_count != 0) &&
       (uim_ptr->command.static_cmd_buf.hdr.cmd_count < UIM_MAX_NUM_ATTEMPTS))
    {
      /*delay before next recovery attempt is retried*/
      uim_timed_sleep(((uim_ptr->extended_recovery.recovery_delay)*
                       (uim_ptr->command.static_cmd_buf.hdr.cmd_count)),
                        uim_ptr, UIM_ALL_ZERO_SIG);
    }
  }
  uim_process_command(mask, uim_ptr);

} /* uim_reset_uim() */


/*===========================================================================

FUNCTION UIM_GSDI_LINK_EST

DESCRIPTION
  This procedure sends a link established command to GSDI

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void uim_gsdi_link_est
(
  uim_instance_global_type *uim_ptr
)
{

  uim_slot_link_established_data_type link_est_data;
  uim_slot_type                       uim_slot = (uim_slot_type)(uim_ptr->id + 1);

  memset(&link_est_data, 0x00, sizeof(uim_slot_link_established_data_type));

  /* Pass the historical characters to GSDI task in the command
      which also indicates that the link has been established and
      this is done only on task start up */
  UIMDRV_MSG_LOW_1(uim_ptr->id,"Sending the LINK ESTABLISHED to GSDI for slot:0x%x",
                   uim_slot );

  /* Set the status to TRUE as link est is sent for single slot only when the
     card is present */
  link_est_data.slot_status = UIM_SLOT_PRESENT;

  link_est_data.cdma_df_present =
    uim_ptr->state.dfs_1.cdma_df_present;
  link_est_data.gsm_df_present =
    uim_ptr->state.dfs_1.gsm_df_present;

#ifdef FEATURE_UIM_USB_UICC
  /* Note: This code has been added only for the single slot solution
     FEATURE_UIM_USB_UICC does not support the dual slot solution */
  if (uim_is_usb_enabled(uim_ptr) && uim_usb_is_usb_interface_selected(uim_ptr))
  {
    uim_memscpy(link_est_data.atr_buf.data,
           sizeof(link_est_data.atr_buf.data),
           uim_usb_get_atr_data_ptr(uim_ptr),
           uim_usb_get_atr_data_size(uim_ptr));

    link_est_data.atr_buf.num_bytes =
      uim_usb_get_atr_data_size(uim_ptr);

    uim_memscpy(link_est_data.hist_char_buf.data,
           sizeof(link_est_data.hist_char_buf.data),
           uim_usb_get_atr_hist_char_ptr(uim_ptr),
           uim_usb_get_atr_hist_char_size(uim_ptr));

    link_est_data.hist_char_buf.length =
       uim_usb_get_atr_hist_char_size(uim_ptr);
    UIMDRV_MSG_HIGH_0(uim_ptr->id,
                      "UIM link established with card over USB UICC interface" );
  }
  else
#endif /* FEATURE_UIM_USB_UICC */
  {
    /* Copy the ATR characters */
    uim_memscpy(link_est_data.atr_buf.data,
           sizeof(link_est_data.atr_buf.data),
           uim_ptr->atr.atr_buf.data,
           uim_ptr->atr.atr_buf.num_bytes
          );

    /* Set the number of bytes copied */
    link_est_data.atr_buf.num_bytes =
      uim_ptr->atr.atr_buf.num_bytes;

    /* Copy the historichal characters */
    uim_memscpy(link_est_data.hist_char_buf.data,
           sizeof(link_est_data.hist_char_buf.data),
           uim_ptr->atr.hist_char_buf.data,
           uim_ptr->atr.hist_char_buf.length
          );

    /* Set the number of bytes copied */
    link_est_data.hist_char_buf.length =
      uim_ptr->atr.hist_char_buf.length;

    /* Copy the iccid characters */
    uim_memscpy( link_est_data.iccid_buf.data,
            sizeof(link_est_data.iccid_buf.data),
            uim_ptr->cache.iccid_buf.data,
            uim_ptr->cache.iccid_buf.num_bytes );

    /* Set the number of bytes copied */
    link_est_data.iccid_buf.num_bytes =
      uim_ptr->cache.iccid_buf.num_bytes;
    UIMDRV_MSG_HIGH_0(uim_ptr->id,
                      "UIM link established with card over legacy ISO interface" );
  }

  /* Set the protocol information for slot 1 */
  if (uim_ptr->state.dfs_1.protocol == UIM_ICC)
  {
    link_est_data.protocol = UIM_ICC;
  }
  else if (uim_ptr->state.dfs_1.protocol == UIM_UICC)
  {
    link_est_data.protocol= UIM_UICC;
  }
  else
  {
    link_est_data.protocol= UIM_NO_SUCH_MODE;
  }

#ifdef FEATURE_UIM_USB_UICC
  if (uim_is_usb_enabled(uim_ptr) && uim_usb_is_usb_interface_selected(uim_ptr))
  {
    link_est_data.work_waiting_time = uim_usb_get_work_waiting_time(uim_ptr);
  }
  else
#endif /* FEATURE_UIM_USB_UICC */
  {

#ifdef FEATURE_UIM_BTSAP_CLIENT
    if(IS_BTSAP_HANDLE_ACTIVE(uim_ptr))
    {
      link_est_data.work_waiting_time = uim_btsap_get_work_waiting_time(uim_ptr);
    }
    else
#endif /* FEATURE_UIM_BTSAP_CLIENT */
    {
      link_est_data.work_waiting_time =
        uim_ptr->card_cmd.work_waiting_time;
    }
  }

  link_est_data.is_euicc = uim_ptr->flag.is_euicc;

  /* Send the command to MMGSDI */
 (void)uim_sm_modify_link_est(link_est_data, uim_ptr);

} /* uim_gsdi_link_est */


/*===========================================================================

FUNCTION UIM_FETCH

DESCRIPTION
  This procedure builds the Internal Fetch command to retrieve a proactive
  command from the UIM.

DEPENDENCIES
  This function must be called from the UIM task after processing of a
  command is completed successfully.  The command queue mask must be
  disabled when this function is called as it sends an internal command
  to the UIM.

RETURN VALUE
  None

SIDE EFFECTS
  A Fetch command is sent to the UIM while other commands may still be in
  the command queue.

===========================================================================*/
static void uim_fetch
(
  rex_sigs_type            *mask,
  uim_instance_global_type *uim_ptr
)
{
  /* Send a FETCH command to the SIM */
  /* Pending fetch is set to false when card return 0x91XX */
  uim_ptr->flag.fetch_pending  = FALSE;

  /* Build an internal select command */
  uim_ptr->flag.static_buffer_used  = TRUE;

  uim_ptr->command.static_cmd_buf.fetch.hdr.command               = UIM_INTERNAL_FETCH_F;
  uim_ptr->command.static_cmd_buf.fetch.hdr.cmd_hdr.task_ptr      = uim_ptr->tcb_ptr;
  uim_ptr->command.static_cmd_buf.fetch.hdr.cmd_hdr.sigs          = NULL;
  uim_ptr->command.static_cmd_buf.fetch.hdr.cmd_hdr.link.next_ptr = NULL;
  uim_ptr->command.static_cmd_buf.fetch.hdr.cmd_hdr.done_q_ptr    = NULL;
  uim_ptr->command.static_cmd_buf.fetch.hdr.rpt_function          = NULL;

  /* Get the command size from the second status word. */
  uim_ptr->command.static_cmd_buf.fetch.command_size = uim_ptr->command.rsp_buf.sw2;

  uim_ptr->command.static_cmd_buf.hdr.protocol = uim_ptr->state.dfs_1.protocol;

  uim_ptr->command.static_cmd_buf.hdr.slot = (uim_slot_type) (uim_ptr->id + 1);

  /* Make command pointer point to static command buffer */
  uim_ptr->command.cmd_ptr = &uim_ptr->command.static_cmd_buf;

  uim_process_command( mask, uim_ptr);
} /* uim_fetch */


/*===========================================================================

FUNCTION UIM_INIT_CRITICAL_SECS

DESCRIPTION
  This procedure initializes the UIM critical sections for the UIM Task.
  It should be called only once during powerup time.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void uim_init_critical_secs( uim_instance_global_type *uim_ptr )
{
  /* Critical section use while turning clocks on and off */
  rex_init_crit_sect(&uim_ptr->crit_sect.clk_ctl);

#ifdef FEATURE_UIM_ENABLE_UARTDM_REGISTER_LOGGING
  /* Critical section used to gaurd accessing of uart register
     logging globals between ISR and UIM context */
  rex_init_crit_sect(&uim_ptr->crit_sect.uartdm_logging);
#endif /* FEATURE_UIM_ENABLE_UARTDM_REGISTER_LOGGING */

} /* uim_init_crit_secs */


/*===========================================================================

FUNCTION UIM_INIT

DESCRIPTION
  This procedure initializes the queues and timers for UIM Task.It should be
  called only once, at powerup time. It also initializes other UIM related
  variables.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void uim_init(
 uim_instance_enum_type id
)
{
  uim_instance_global_type *uim_ptr = NULL;
  uim_alloc_global_instance(id);
  uim_ptr = uim_get_instance_ptr(id);
  ASSERT(uim_ptr);


  /* -----------------
  ** Initialize queues
  ** ----------------- */

  (void) q_init( &uim_ptr->task.cmd_q );

  /* --------------------
  ** Initialize variables
  ** -------------------- */

  uim_ptr->flag.me_powerup                = TRUE;
  uim_ptr->flag.task_startup              = TRUE;

  uim_reset_dir(uim_ptr);

  uim_ptr->state.dfs_1.cdma_df_present    = FALSE;
  uim_ptr->state.dfs_1.gsm_df_present     = FALSE;
  uim_ptr->state.dfs_1.protocol           = UIM_NO_SUCH_MODE;

  uim_ptr->state.WI                       = 10;

  uim_ptr->state.last_connection_mode     = UIM_CONNECTION_ISO;
  uim_ptr->state.local_card_pup_delay     = UIM_MAX_WAIT_LOCAL_CARD_PUP;
  uim_ptr->flag.uim_not_allow_local_card_power_up = FALSE;

  uim_ptr->flag.me_powerup                = TRUE;

  uim_ptr->setting.poll_time_delay        = UIM_POLL_TIME;
  uim_ptr->setting.proactive_time_delay   = UIM_POLL_TIME;
  uim_ptr->setting.response_byte_delay    = UIM_RESPONSE_BYTE_DELAY_DEFAULT;
  uim_ptr->setting.wakeup_poll_offset     = UIM_WAKE_UP_POLL_OFFSET;

  uim_ptr->state.ctl =
   (uim_voter_type) ((int)UIM_NONE
                   | (int)UIM_MC
                   | (int)UIM_AUTH
                   | (int)UIM_UICC_SESSION
                     );

  uim_ptr->poll.is_proactive_polling_enabled = TRUE;
  uim_ptr->poll.presence_detection_state  = UIM_PD_POLLING_OFF;
  uim_ptr->state.FI_current               = UIM_CRCF_372_1;
  uim_ptr->state.DI_current               = UIM_BRAF_1;
  uim_ptr->state.is_ch_open[0]            = TRUE;
  uim_ptr->state.mf_reselect_count        = 1;
  uim_ptr->state.status                   = UIM_UNINITIALIZED_S;

  uim_ptr->card_cmd.instrn_toggle_count   = 1;
  uim_ptr->rxtx_state_machine.rx_sub_state = UIM_RX_PROCESS_IDLE;

  uim_ptr->card_cmd.uim_busy_ind_timer_state      = UIM_BUSY_RESPONSE_TIMER_NOT_USED;
  uim_ptr->card_cmd.uim_trans_timer_state         = UIM_BUSY_RESPONSE_TIMER_NOT_USED;
  uim_ptr->card_cmd.uim_simulate_null_timer_state = UIM_BUSY_RESPONSE_TIMER_NOT_USED;

  uim_ptr->debug.uim_feature_busy_response_simulate_null.uim_disable_simulate_null = TRUE;
  uim_ptr->debug.uim_feature_busy_response_simulate_null.uim_simulate_null_ins     = UIM_SIMULATE_NULL_INS_DEFAULT;
  uim_ptr->debug.uim_simulate_null_timer_val_s                                     = 0x0;

  uim_ptr->hotswap.card_status            = UIM_CARD_UNCHANGED;
  uim_ptr->hotswap.hotswap_card_status    = UIM_CARD_UNCHANGED;

  uim_ptr->extended_recovery.recovery_delay = UIM_RECOVERY_DELAY_FACTOR;

#ifdef FEATURE_UIM_T_1_SUPPORT
  uim_ptr->t1.nv_pref_protocol            = NV_UIM_PREF_PROTOCOL_T_1;
  uim_ptr->t1.t_1_wtx                     = 1;
#endif /* FEATURE_UIM_T_1_SUPPORT */

  uim_ptr->state.channel_info.manage_channel_allowed = TRUE;
  uim_ptr->state.channel_info.unallocated_channel    = 0xFF;
} /* uim_init() */

/*===========================================================================

FUNCTION UIM_INIT_COMMON_CRITICAL_SECTIONS

DESCRIPTION
  This procedure initializes the critical sections common to all UIM tasks
  before dev init gets called with the instance id of the specific tasks.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void uim_init_common_critical_sections(void)
{
  uim_shared_global_type   *shared_globals_ptr = uim_get_common_globals_ptr();

  /* Critical section used while performing nv reads */
  rex_init_crit_sect(&shared_globals_ptr->nv_read_crit_section);
  /* Critical section used while reading nv to initializing hw enumeration */
  rex_init_crit_sect(&uimdrv_hw_nv_config_info.uimdrv_hw_nv_crit_sect);
#ifdef FEATURE_UIM_DS_SUBSCRIPTION_MANAGER
  /* Critical section used by Subscription manager */
  rex_init_crit_sect(&uim_sm_data.uim_sm_data_crit_sect);
#endif
#ifdef FEATURE_UIM_POWERUP_LOGGING
  rex_init_crit_sect(&pup_log_crit_sec);
#endif

} /* uim_init_common_critical_sections */


/*===========================================================================

FUNCTION UIM_1_INIT

DESCRIPTION
  This procedure initializes the queues and timers for UIM Task.It should be
  called only once, at powerup time. It also initializes other UIM related
  variables.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void uim_1_init(void)
{
#ifdef FEATURE_UIM_TEST_FRAMEWORK
  #error code not present
#endif
  uim_init_common_globals();
  uim_init_common_critical_sections();
  uim_init(UIM_INSTANCE_1);
} /* uim_1_init */


/*===========================================================================

FUNCTION UIM_2_INIT

DESCRIPTION
  This procedure initializes the queues and timers for UIM Task.It should be
  called only once, at powerup time. It also initializes other UIM related
  variables.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void uim_2_init(void)
{
  uim_init(UIM_INSTANCE_2);
} /* uim_2_init */

/*===========================================================================

FUNCTION UIM_3_INIT

DESCRIPTION
  This procedure initializes the queues and timers for UIM Task.It should be
  called only once, at powerup time. It also initializes other UIM related
  variables.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void uim_3_init(void)
{
  uim_init(UIM_INSTANCE_3);
} /* uim_3_init */


/*===========================================================================

FUNCTION UIM_GET_UART_STATUS()

DESCRIPTION
  This procedure safely calls the uim_read_status() by first checking to make
  sure that the power and clock for the appropriate UIM is still on.
  If it isn't, it'll return 0 instead.  It always checks for the slot currently
  in use.

DEPENDENCIES
  None

RETURN VALUE
  Either Read Status or 0.

SIDE EFFECTS
  None

===========================================================================*/
dword uim_get_uart_status
(
  uim_instance_global_type *uim_ptr
)
{

#ifdef FEATURE_UIM_USB_UICC
  if (uim_is_usb_enabled(uim_ptr) && uim_usb_is_usb_interface_selected(uim_ptr))
  {
    return 0;
  }
#endif /* FEATURE_UIM_USB_UICC */

  if(UIM_IS_UIM_CLK_ON(uim_ptr))
  {
   /* Before checking UART status, ensure that Power and clock
      are still on */
    return uim_read_status(uim_ptr);
  }
  else
  {
    /* Otherwise, we can't reliably check the status, so just report 0 */
    return 0;
  }
} /* uim_get_uart_status() */


/*===========================================================================

FUNCTION UIM_PROCESS_CMD_ERR                                         INTERNAL

DESCRIPTION
  This procedure processes the error response received from the UIM card

DEPENDENCIES
  This function uses global variables defined for the UIM server to determine
  the proper processing of the card response.

RETURN VALUE
  cmd_ptr value.

SIDE EFFECTS
  The control variables of the UIM server are adjusted to reflect the change
  in the operation of the UIM server as a result of the processing on the
  response.

===========================================================================*/
void uim_process_cmd_err
(
  rex_sigs_type            *i_mask,   /* Mask of signals interested in */
  uim_instance_global_type *uim_ptr
)
{
  boolean uim_internal_cmd  = TRUE;
  uim_status_type  notification_reason = UIM_ERR_S;
  RETURN_IF_INSTANCE_INVALID(uim_ptr->id);
  if (i_mask == NULL)
  {
    return;
  }
  /* Set the uim status to UIM_ERR_S when
     sim reports technical problem */
  if( uim_ptr->setting.pdown_on_tech_prob.to_pdown &&
      uim_ptr->card_cmd.inc_tech_prob_count >= uim_ptr->setting.pdown_on_tech_prob.cnt_tech_problem &&
      uim_ptr->command.mode != UIM_PASSIVE_MODE )
  {
    /* Clear out any votes to keep UIM powered on */
    uim_ptr->state.ctl = UIM_NONE;
    uim_ptr->flag.powerdown = TRUE;
    uim_ptr->flag.command_in_progress = FALSE;
    /* Reset all the flags and include the imask */
    *i_mask |= UIM_CMD_Q_SIG;
    /* This command has been completed */
    uim_ptr->flag.command_in_progress = FALSE;
    /* Reset the internal command used flag */
    uim_ptr->flag.internal_command_used = FALSE;

    /* send an error report */
    if ((!uim_ptr->flag.static_buffer_used) && uim_ptr->command.cmd_ptr != NULL)
    {
      uim_send_err_rpt(uim_ptr->command.cmd_ptr, uim_ptr);
    }
    /* flush all the commands in the command queue */
    uim_flush_command(UIM_FLUSH_ALL, uim_ptr);
    /* Reset the static_buffer */
    uim_ptr->flag.static_buffer_used = FALSE;
    /* set the status to ERROR */
    uim_ptr->state.status = UIM_ERR_S;

    return;
  }

  /* Set the command count for external command to MAX in case of 6F 00
   * so that same command is not retried */
  if( uim_ptr->setting.pdown_on_tech_prob.to_pdown &&
      uim_ptr->command.rsp_buf.sw1 == (byte) SW1_PROBLEM &&
      uim_ptr->command.rsp_buf.sw2 == (byte) SW2_SW1_PROBLEM &&
      uim_ptr->flag.static_buffer_used != TRUE)
  {
    if(uim_ptr->command.cmd_ptr != NULL)
    {
      uim_ptr->command.cmd_ptr->hdr.cmd_count = UIM_MAX_NUM_CMD_ATTEMPTS(uim_ptr);
      uim_ptr->command.rpt_buf.sw1 = uim_ptr->command.rsp_buf.sw1;
      uim_ptr->command.rpt_buf.sw2 = uim_ptr->command.rsp_buf.sw2;
    }
  }

  if ((uim_ptr->flag.static_buffer_used) &&
      (uim_ptr->command.static_cmd_buf.hdr.command == UIM_INTERNAL_FETCH_F))
  {
    UIMDRV_MSG_HIGH_0(uim_ptr->id,"Ignore failure on an internal fetch");
    //reset all the flags and include the imask
    *i_mask |= UIM_CMD_Q_SIG;
    /* Set the flag to false since the command is completed */
    uim_ptr->flag.command_in_progress = FALSE;
    /* Reset the internal command used flag */
    uim_ptr->flag.internal_command_used = FALSE;
    /* Reset the static_buffer */
    uim_ptr->flag.static_buffer_used = FALSE;
    /* Reset the command requeued flag */
    uim_ptr->flag.command_requeued = FALSE;
    uim_ptr->command.command_requeue_counter = 0;
    uim_ptr->flag.bad_status_words_error = FALSE;
#ifdef FEATURE_GSTK
    /* Here we pass NULL and data length of 0 to GSTK to tell it to reset */
    /* gstk_proactive_cmd_in_progress to FALSE */
    (void)gstk_send_proactive_cmd(uim_ptr->command.static_cmd_buf.hdr.slot, 0, NULL);
#endif /* FEATURE_GSTK */
    return;

  }
   /* If the result is for an external command */
  if ((uim_ptr->flag.internal_command_used) || (!uim_ptr->flag.static_buffer_used))
  {
    UIMDRV_MSG_HIGH_0(uim_ptr->id,"Process cmd error in external command");
    uim_internal_cmd = FALSE;
    if (uim_ptr->command.cmd_ptr != NULL)
    {
      /* Avoid multiple attempts for ECC and authenticate as well as
         RUN_GSM_ALGO.  Also do not re-try while in passive mode */
      if (( !uim_ptr->flag.internal_command_used &&
            ( ( uim_ptr->command.cmd_ptr->hdr.command == UIM_AUTHENTICATE_F ) ||
              ( uim_ptr->command.cmd_ptr->hdr.command == UIM_AUTHENTICATE_ODD_INS_F ) ||
              ( uim_ptr->command.cmd_ptr->hdr.command == UIM_RUN_GSM_ALGO_F ) ||
              ( uim_ptr->command.cmd_ptr->hdr.command == UIM_ISIM_AUTHENTICATE_F ) ||
              ( uim_ptr->command.cmd_ptr->hdr.command == UIM_SSD_UPDATE_F   ) ||
              ( uim_ptr->command.cmd_ptr->hdr.command == UIM_SEEK_F   ) ||
              ( uim_ptr->command.cmd_ptr->hdr.command == UIM_CONFIRM_SSD_F ) ||
              ( uim_ptr->command.cmd_ptr->hdr.command == UIM_ENVELOPE_F   )
            )
          ) ||
          uim_ptr->command.mode == UIM_PASSIVE_MODE)
      {
        /* Any error in passive mode goes directly to the client
         * without any re-tries
         */
        uim_ptr->command.cmd_ptr->hdr.cmd_count = uim_ptr->setting.max_num_cmd_attempts;
      }

      /* If we have exceeded tbe max number of attempts */
      if (++uim_ptr->command.cmd_ptr->hdr.cmd_count > uim_ptr->setting.max_num_cmd_attempts)
      {
        UIMDRV_MSG_HIGH_1(uim_ptr->id,"cmd count maxed out",
                          uim_ptr->command.cmd_ptr->hdr.command);
        if (UIM_RESET_F == uim_ptr->command.cmd_ptr->hdr.command)
        {
           /* UIM need to power down the SIM on Reset failure, for that we need to change it to Internal cmd */
           uim_internal_cmd = TRUE;
           uim_ptr->command.static_cmd_buf.hdr.cmd_count = UIM_MAX_NUM_ATTEMPTS;
           uim_ptr->command.static_cmd_buf.hdr.slot = uim_ptr->command.cmd_ptr->hdr.slot;
           uim_ptr->command.static_cmd_buf.hdr.command = uim_ptr->command.cmd_ptr->hdr.command;
        }
        /* include the command Q sig to the mask as we are going
           to process the command again */
        *i_mask |= UIM_CMD_Q_SIG;

        if (uim_ptr->flag.internal_command_used)
        {
          /* When this flag is TRUE, do not flush the external command, though
             the intermediate selection failed*/
          if (uim_ptr->flag.uim_do_not_flush_ext_cmd == FALSE)
          {
            /* flush the external command */
            uim_flush_command(UIM_FLUSH_ONLY, uim_ptr);
          }
        }
        else
        {
          /* send an error report */
          uim_send_err_rpt(uim_ptr->command.cmd_ptr, uim_ptr);
        }

        /* Clearing the bad status word flag */
        uim_ptr->flag.bad_status_words_error = FALSE;
        /* reset the static buffer used flag */
        uim_ptr->flag.static_buffer_used = FALSE;

        /* reset the internal command used flag */
        uim_ptr->flag.internal_command_used = FALSE;
        if (uim_nv_is_feature_enabled(UIMDRV_FEATURE_MIN_TPL_ICCID_SUPPORT,
                                      uim_ptr)
            && (uim_ptr->command.cmd_ptr->hdr.command == UIM_TERMINAL_PROFILE_F)
            && (NULL != uim_ptr->state.iccid_tpl_ptr))
        {
          /* uim_ptr->state.iccid_tpl_ptr->terminal_profile_len is having the last TP length
             sent to card, If it is not set to minimum length, then set the
             terminal profile length to minimum length and update the NV Item.
             And reset the uim so on next power up,uim will send terminal
             profile with minimum length. */
          if( UIM_MINIMUM_TP_LENGTH < uim_ptr->state.iccid_tpl_ptr->terminal_profile_len)
          {
            uim_ptr->state.iccid_tpl_ptr->terminal_profile_len = UIM_MINIMUM_TP_LENGTH;
            if ( TRUE == uim_put_nv(
                          NV_UIM_ICCID_TPL_I,
                          (nv_item_type *)uim_ptr->state.iccid_tpl_ptr,
                          uim_ptr)
               )
            {
              /* Internal reset the uim on terminal profile response error */
              uim_reset_uim(i_mask, uim_ptr->flag.me_powerup, uim_ptr);
            }
          }

          /* After reset memory will be allocate in next terminal profile command */
          UIM_FREE(uim_ptr->state.iccid_tpl_ptr);
        }
      }
      else
      {
        /* Makes sure that we do not send the status again to
           the task that sent the command by setting the task pointer
           to NULL */
        uim_ptr->command.cmd_ptr->hdr.cmd_hdr.task_ptr = NULL;

        UIMDRV_MSG_HIGH_1(uim_ptr->id,"uim_command_requeued[%d] = %d",
                          uim_ptr->flag.command_requeued);
        if (!uim_ptr->flag.command_requeued)
        {
          /* Call the command processing function directly to retry
          the command */
          uim_process_command(i_mask, uim_ptr);
        }
        else
        {
         /* Since the command was re-queued, include the command Q sig
             to the mask to process the command again */
          *i_mask |= UIM_CMD_Q_SIG;
        }
      }
    } /* cmd_Ptr !=NULL */
    else
    {
      uim_reset_uim (i_mask, uim_ptr->flag.me_powerup, uim_ptr);
    }
  }
  /* if it was an internal command */
  if(TRUE == uim_internal_cmd)
  {
    UIMDRV_MSG_HIGH_0(uim_ptr->id,"Process cmd error in internal command");
    notification_reason = UIM_INT_CMD_ERR_EXCEED_MAX_ATTEMPTS_S;

    if ( uim_ptr->command.mode == UIM_RECOVERY_MODE )
    {
      UIMDRV_MSG_HIGH_0(uim_ptr->id,"Handle error in recovery via timeout signal");
      /* Let us not clear the timeout signal after processing resopnse */
      uim_ptr->flag.clear_timeout_on_response = FALSE;
      /* Clear out the command response timer as we are setting the timeout signal */
      uim_clear_cmd_rsp_timer(uim_ptr);
      /* To reset uart and mask intterupts before triggering recovery */
      uim_force_recovery(uim_ptr);
      return;
    }

    /* Determine if the ATR was corrupted */
    if (!uim_ptr->atr.atr_received)
    {
      /* Check if the ATR was processed enough */
      if (uim_ptr->command.static_cmd_buf.hdr.cmd_count == UIM_MAX_NUM_ATTEMPTS)
      {
        /* Check if there is a higher voltage class */
        if (uim_ptr->state.max_possible_voltage_class != uim_ptr->state.current_voltage_class)
        {
          /* Power down the interface */
          uim_power_down(uim_ptr);

          /* We will try the next voltage class only if the first_inst_class
          is GSM or we do not have the voltage class information from
          the ATR and we have a higher voltage available*/
          if ( ( uim_ptr->state.first_inst_class == NV_UIM_FIRST_INST_CLASS_GSM_SIM ) ||
               ( uim_ptr->flag.voltage_class_known_from_atr == FALSE )
             )
          {
            uim_timed_sleep(UIM_ISO7816_VOLTAGE_SWITCH_PWR_DOWN_DELAY, uim_ptr, UIM_ALL_ZERO_SIG);
            /* Try the next voltage class */
            uim_ptr->state.current_voltage_class++;
            /* Reset the error count */
            uim_ptr->command.static_cmd_buf.hdr.cmd_count = 0;
          }
        } /* end if - there are no more voltages to try */
        else
        {
            notification_reason = UIM_CORRUPT_ATR_RCVD_MAX_TIMES_S;
        }
      } /* end if - the ATR was processed enough */
      else
      {
        /* Let us power down the interface if the first instruction
           class is set to USIM */
        if (NV_UIM_FIRST_INST_CLASS_UMTS_SIM == uim_ptr->state.first_inst_class )
        {
          uim_power_down(uim_ptr);
        }
      } /* else for ATR was processed enough */
    } /* end if - the ATR was corrupted */

    /* Any error in passive mode for an internal command goes
     * directly to the client without re-tries
     */
    if ( uim_ptr->command.mode == UIM_PASSIVE_MODE )
    {
      /* could be a polling error or any other internal command
       * error
       */
      uim_ptr->command.static_cmd_buf.hdr.cmd_count = UIM_MAX_NUM_ATTEMPTS;
      notification_reason               = UIM_INT_CMD_ERR_IN_PASSIVE_MODE_S;
    }

    /* Report poll error without retries */
    if( uim_nv_is_feature_enabled(UIMDRV_FEATURE_RPT_BAD_SW_ON_POLL,
                                  uim_ptr) &&
        (uim_ptr->flag.static_buffer_used) &&
        (uim_ptr->command.static_cmd_buf.hdr.command == UIM_INTERNAL_POLL_F))
    {
       uim_ptr->command.static_cmd_buf.hdr.cmd_count = UIM_MAX_NUM_ATTEMPTS;
       notification_reason               = UIM_POLL_ERR_S;
    }

    /* If we have exceeded the number of attempts, then declare the
       UIM as faulty, flush the commands and indicate to the higher
       layer */
    if (++uim_ptr->command.static_cmd_buf.hdr.cmd_count > UIM_MAX_NUM_ATTEMPTS)
    {
      /* Setting the status to ERROR prior to invoking power down.
         While shutting the LDO down we evaluate the uim status to
         cater to the user customized LDO settings if needed. */
      uim_ptr->state.status = UIM_ERR_S;

      uim_power_down(uim_ptr);

      /* Clear out any votes to keep UIM powered on */
      uim_ptr->state.ctl = UIM_NONE;
      uim_ptr->flag.powerdown = TRUE;
      uim_ptr->flag.command_in_progress = FALSE;

      /* flush all the commands in the command queue */
      uim_flush_command(UIM_FLUSH_ALL, uim_ptr);

      /* Whenever we receive a command, send an error report as the
      SIM is to be powered down */
      *i_mask |= UIM_CMD_Q_SIG;

      /* Notify the higher layers that UIM is declared faulty */
      uim_notify_error(notification_reason, uim_ptr);

      /* reset the directories and send a power down to UIM */
      uim_reset_dir(uim_ptr);

      /* Reset the static buffer used flag */
      uim_ptr->flag.static_buffer_used = FALSE;

      /* Reset the static command count */
      uim_ptr->command.static_cmd_buf.hdr.cmd_count = 0;

      /* Reset recovery triggered flag */
      uim_ptr->flag.recovery_triggered = FALSE;
      if(uim_ptr->command.mode == UIM_RECOVERY_MODE)
      {
        UIMDRV_MSG_HIGH_0(uim_ptr->id,"Switching to NORMAL_MODE");
        uim_ptr->command.mode = UIM_NORMAL_MODE;
      }
    }
    /* else reset the UIM */
    else
    {
      if ((uim_ptr->flag.static_buffer_used) && (uim_ptr->command.static_cmd_buf.hdr.command == UIM_INTERNAL_POLL_F))
      {
        if ( uim_ptr->setting.pdown_on_tech_prob.to_pdown &&
            (uim_ptr->command.rsp_buf.sw1  == (byte) SW1_PROBLEM) &&
            (uim_ptr->command.rsp_buf.sw2   == (byte) SW2_SW1_PROBLEM))
        {
          UIMDRV_MSG_HIGH_0 ( uim_ptr->id,"Ignore failure on an internal poll for tech prob error");
          //reset all the flags and include the imask
          *i_mask |= UIM_CMD_Q_SIG;
          /* Set the flag to false since the command is completed */
          uim_ptr->flag.command_in_progress = FALSE;
          /* Reset the internal command used flag */
          uim_ptr->flag.internal_command_used = FALSE;
          /* Reset the static_buffer */
          uim_ptr->flag.static_buffer_used = FALSE;
          /* Reset the command requeued flag */
          uim_ptr->flag.command_requeued = FALSE;
          uim_ptr->command.command_requeue_counter = 0;
          uim_set_poll_timer(uim_ptr->setting.poll_time_delay,
                             uim_ptr->setting.wakeup_poll_offset,
                             uim_ptr);
          return;
        }
        UIMDRV_MSG_HIGH_0(uim_ptr->id,"Setting UIM POLL timer after a poll failure and reset the card" );
        uim_set_poll_timer(uim_ptr->setting.poll_time_delay,
                           uim_ptr->setting.wakeup_poll_offset,
                           uim_ptr);
      }
      uim_reset_uim (i_mask, uim_ptr->flag.me_powerup, uim_ptr);
    }
  }  /* else an internal command */
} /* uim_process_cmd_err */


/*===========================================================================

FUNCTION UIM_SW1_WARNINGS1_FREE         INTERNAL

DESCRIPTION
  This function is dummy callback. The function should only be used as the
  report function for commands queued in handle_sw1_warnings1

DEPENDENCIES
  UIM_HANDLE_SW1_WARNINGS1_NORMAL_END_RESELECT_MF has been called and a cmd
  was successfully queued

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
static void uim_sw1_warnings1_free(uim_rpt_type *rpt)
{
  /* Do nothing */
  (void)rpt;
}/* uim_sw1_warnings1_free */


/*===========================================================================

FUNCTION UIM_OPEN_CHANNEL_CB         INTERNAL

DESCRIPTION
  This function is a callback for the open channel command queued internally.

DEPENDENCIES
NONE

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
static void uim_open_channel_cb(uim_rpt_type *rpt)
{
  uim_instance_global_type  *uim_ptr  = NULL;
  uim_ptr =  uim_get_instance_ptr((uim_instance_enum_type)((rpt->slot) - 1));

  if(NULL == uim_ptr)
  {
    return;
  }
  if(rpt->rpt_status == UIM_PASS)
  {
    uim_ptr->state.channel_info.manage_channel_allowed = TRUE;
    uim_ptr->state.channel_info.unallocated_channel    = rpt->rpt.channel.channel_num;
  }
  else
  {
    if(TRUE == rpt->cmd_transacted)
    {
      uim_ptr->state.channel_info.manage_channel_allowed = FALSE;
      UIMDRV_MSG_HIGH_0(uim_ptr->id, "internl open channel command failed");
    }
  }
}/* uim_open_channel_cb */


/*===========================================================================

FUNCTION UIM_HANDLE_SW1_WARNINGS1_NORMAL_END_RESELECT_MF         INTERNAL

DESCRIPTION
  This procedure builds a UIM MF select command and inserts the command
  to the front of the uim cmd queue

DEPENDENCIES
  This function uses global variable uim_sw1_warnings1_normal_end_occured.

RETURN VALUE
  None.

SIDE EFFECTS
  A Select MF will occur, which will bring the uim_last_sel_dir to change to
  MF.

===========================================================================*/
static void uim_handle_sw1_warnings1_normal_end_reselect_mf
(
  uim_instance_global_type  *uim_ptr
)
{
  uim_cmd_type *uim_cmd_ptr = NULL;

  if (uim_ptr->flag.sw1_warnings1_normal_end_occured)
  {
   /*
    * Only try and Select MF two times if we received warning while selecting
    * MF. This will prevent code from entring into a loop trying to select MF
    * on receiving warnings during select.
    * If we have tried twice reset all the variables and move out.
    */
    if(uim_ptr->state.mf_reselect_count >= UIM_MAX_COMMAND_RETRIES)
    {
      uim_ptr->flag.sw1_warnings1_normal_end_occured = FALSE;
      uim_ptr->state.mf_reselect_count = 1;
      return;
    }

    if ((uim_ptr->command.cmd_ptr == NULL) ||
        (uim_cmd_ptr = (uim_cmd_type*)uim_malloc(sizeof(uim_cmd_type)))== NULL)
    {
      return;
    }
    /*---------------------------------------------------------------------------
    Populate UIM Cmd pointer data
    ---------------------------------------------------------------------------*/
    uim_cmd_ptr->hdr.command            = UIM_SELECT_F;
    uim_cmd_ptr->hdr.protocol           = uim_ptr->command.cmd_ptr->hdr.protocol;
    uim_cmd_ptr->hdr.slot               = uim_ptr->command.cmd_ptr->hdr.slot;
    uim_cmd_ptr->hdr.channel            = uim_ptr->command.cmd_ptr->hdr.channel;
    uim_cmd_ptr->hdr.cmd_hdr.task_ptr   = uim_ptr->tcb_ptr;
    uim_cmd_ptr->hdr.cmd_hdr.sigs       = 0;
    uim_cmd_ptr->hdr.options            = UIM_OPTION_ALWAYS_RPT;
    uim_cmd_ptr->hdr.cmd_hdr.done_q_ptr = NULL;
    uim_cmd_ptr->select.item            = UIM_NO_SUCH_ITEM;
    uim_cmd_ptr->select.dir             = (uim_dir_type)UIM_MF;
    uim_cmd_ptr->select.file_type       = UIM_MF;

    uim_cmd_ptr->select.aid.aid_length     = uim_ptr->command.cmd_ptr->select.aid.aid_length;

    if (uim_cmd_ptr->select.aid.aid_length > UIM_AID_MAX_NAME_LENGTH)
    {
      UIMDRV_MSG_ERR_1(uim_ptr->id,"Aid length too long 0x%x, cannot build reselect MF command",
                       uim_cmd_ptr->select.aid.aid_length);

      UIM_FREE(uim_cmd_ptr);
      return;
    }
    if (uim_cmd_ptr->select.aid.aid_length > 0)
    {
      (void) uim_memscpy(uim_cmd_ptr->select.aid.data,
                    sizeof(uim_cmd_ptr->select.aid.data),
                    uim_ptr->command.cmd_ptr->select.aid.data,
                    uim_ptr->command.cmd_ptr->select.aid.aid_length);
    }

    uim_cmd_ptr->hdr.rpt_function = uim_sw1_warnings1_free;
    uim_cmd_ptr->hdr.user_data    = (uint32)uim_cmd_ptr;

    memset(&uim_cmd_ptr->select.path, 0x00, sizeof(uim_path_type));

    /* if protocol == UICC, session should be set to UIM_ACTIVATION_OR_RESET (0)
       if protocol == ICC, session is not applicable, default to 0 */
    uim_cmd_ptr->select.session = UIM_ACTIVATION_OR_RESET;

    /* Since we are doing an explicit q_put / q_insert rather than using uim_cmd(),
       we will free the allocated memory either in uim_process_generic_response or
       uim_send_generic_err_rpt based on the result of SELECT, after invoking the
       response callback */
    if ( q_cnt( &uim_ptr->task.cmd_q ) == 0 )
    {
      q_put( &uim_ptr->task.cmd_q, &uim_cmd_ptr->hdr.cmd_hdr.link );
    }
    else
    {
      /* Insert the reselect MF to the beginning of the queue */
      q_insert( & uim_ptr->task.cmd_q,
                & uim_cmd_ptr->hdr.cmd_hdr.link,
                & ((uim_cmd_type*) q_check( & uim_ptr->task.cmd_q ))->hdr.cmd_hdr.link );
    }
  }
} /* uim_handle_sw1_warnings1_normal_end_reselect_mf */


/*===========================================================================

FUNCTION UIM_EXPLICIT_RESELECT_MF_AID         INTERNAL

DESCRIPTION
  This procedure builds a UIM MF select or ADF select command and inserts the command
  to the front of the uim cmd queue

DEPENDENCIES
  This function uses global variable uim_explicit_mf_selection
                      and uim_explicit_adf_selection.

RETURN VALUE
  None.

SIDE EFFECTS
  A Select MF/ADF will occur, which will bring the uim_last_sel_dir to change to
  MF/ADF.

===========================================================================*/
void uim_explicit_reselect_mf_aid(
  uim_instance_global_type  *uim_ptr

)
{
  uim_cmd_type *uim_cmd_ptr = NULL;
  if (uim_ptr->state.mf_selection.required 
     || uim_ptr->state.last_selected.required
     || uim_ptr->state.adf_selection.required)
  {
    UIMDRV_MSG_HIGH_3(uim_ptr->id,"Entering the explicit MF 0x%x or last_selected file 0x%x or ADF selection 0x%x",
                      uim_ptr->state.mf_selection.required,
                      uim_ptr->state.last_selected.required,
                      uim_ptr->state.adf_selection.required);
    if ((uim_cmd_ptr = (uim_cmd_type*)uim_malloc(sizeof(uim_cmd_type)))== NULL)
    {
      return;
    }
    memset(uim_cmd_ptr, 0x00, sizeof(uim_cmd_type));
    /*---------------------------------------------------------------------------
    Populate UIM Cmd pointer data
    ---------------------------------------------------------------------------*/
    uim_cmd_ptr->hdr.command            = UIM_SELECT_F;
    uim_cmd_ptr->hdr.protocol           = uim_ptr->card_cmd.uimdrv_protocol;
    uim_cmd_ptr->hdr.slot               = uim_ptr->id + 1 ;
    uim_cmd_ptr->hdr.cmd_hdr.task_ptr   = uim_ptr->tcb_ptr;
    uim_cmd_ptr->hdr.cmd_hdr.sigs       = 0;
    uim_cmd_ptr->hdr.options            = UIM_OPTION_ALWAYS_RPT;
    uim_cmd_ptr->hdr.cmd_hdr.done_q_ptr = NULL;
    uim_cmd_ptr->select.item            = UIM_NO_SUCH_ITEM;
    uim_cmd_ptr->select.aid.aid_length  = 0;

    if(uim_ptr->flag.poll_pending == TRUE)
    {
      uim_ptr->flag.poll_pending = FALSE;
      (void) rex_set_sigs(uim_ptr->tcb_ptr, UIM_POLL_TIMER_SIG);
    }

    if(uim_ptr->state.mf_selection.required)

    {
      uim_cmd_ptr->hdr.channel            = uim_ptr->state.mf_selection.channel;
      uim_cmd_ptr->select.dir             = (uim_dir_type)UIM_MF;
      uim_cmd_ptr->select.file_type       = UIM_MF;
      uim_cmd_ptr->select.path.path[0]    = 0x3F00;
      uim_cmd_ptr->select.path.len        = 0x1;
    }
    else if(uim_ptr->state.adf_selection.required)
    {
      uim_cmd_ptr->hdr.channel			= uim_ptr->state.adf_selection.channel;
      uim_cmd_ptr->select.dir 			= (uim_dir_type)UIM_DF;
      uim_cmd_ptr->select.file_type		= UIM_DF;
      uim_cmd_ptr->select.path.path[0]	= UIM_MF_ID;
      uim_cmd_ptr->select.path.path[1]	= UIM_SEL_AID_BY_FILE_ID;
      uim_cmd_ptr->select.path.len		= 0x2;
    }
    else if (uim_ptr->state.last_selected.required)
    {
      uim_cmd_ptr->hdr.channel   = uim_ptr->state.last_selected.channel;
	  
      if (uim_ptr->state.current_path[0].path.len  != 0 && 
          uim_ptr->state.current_path[0].path.len <= UIM_DIRMAIN_MAX_PATH_ELEMENTS)
      {
        uint32 i = 0;
        for(i = 0; i < uim_ptr->state.current_path[0].path.len; i++)
        {
          uim_cmd_ptr->select.path.path[i] = uim_ptr->state.current_path[0].path.path[i];
        }
        uim_cmd_ptr->select.path.len = uim_ptr->state.current_path[0].path.len;
        uim_cmd_ptr->select.file_type           = UIM_EF;
      }
    }
  
      uim_ptr->state.mf_selection.required    = 0;
      uim_ptr->state.mf_selection.channel     = 0;

      uim_ptr->state.adf_selection.required   = 0;
      uim_ptr->state.adf_selection.channel    = 0;

      /*If mf is being selected, then last selected is not required*/      
      uim_ptr->state.last_selected.required = FALSE;
      uim_ptr->state.last_selected.channel = 0;

    uim_cmd_ptr->hdr.rpt_function = uim_sw1_warnings1_free;
    uim_cmd_ptr->hdr.user_data    = (uint32)uim_cmd_ptr;
    /* if protocol == UICC, session should be set to UIM_ACTIVATION_OR_RESET (0)
       if protocol == ICC, session is not applicable, default to 0 */
    uim_cmd_ptr->select.session = UIM_ACTIVATION_OR_RESET;

    /* Since we are doing an explicit q_put / q_insert rather than using uim_cmd(),
       we will free the allocated memory either in uim_process_generic_response or
       uim_send_generic_err_rpt based on the result of SELECT, after invoking the
       response callback */
    if ( q_cnt( &uim_ptr->task.cmd_q ) == 0 )
    {
      q_put( &uim_ptr->task.cmd_q, &uim_cmd_ptr->hdr.cmd_hdr.link );
    }
    else
    {
      /* Insert the reselect MF to the beginning of the queue */
      q_insert( & uim_ptr->task.cmd_q,
      & uim_cmd_ptr->hdr.cmd_hdr.link,
      & ((uim_cmd_type*) q_check( & uim_ptr->task.cmd_q ))->hdr.cmd_hdr.link );
    }
  }
} /* uim_explicit_reselect_mf_aid */


/*===========================================================================

FUNCTION UIM_QUEUE_INTERNAL_OPEN_CHANNEL_COMMAND     INTERNAL

DESCRIPTION
  This procedure builds a UIM open manage channel command and inserts the command
  to the front of the uim cmd queue

DEPENDENCIES
  NONE

RETURN VALUE
  None.

SIDE EFFECTS
  NONE
===========================================================================*/
void uim_queue_internal_open_channel_command
(
  uim_instance_global_type  *uim_ptr
)
{
  uim_cmd_type *uim_cmd_ptr = NULL;
  {
    if ((uim_cmd_ptr = (uim_cmd_type*)uim_malloc(sizeof(uim_cmd_type)))== NULL)
    {
      return;
    }
    memset(uim_cmd_ptr, 0x00, sizeof(uim_cmd_type));
    /*---------------------------------------------------------------------------
    Populate UIM Cmd pointer data
    ---------------------------------------------------------------------------*/
    uim_cmd_ptr->hdr.command            = UIM_MANAGE_CHANNEL_F;
    uim_cmd_ptr->hdr.protocol           = UIM_UICC;
    uim_cmd_ptr->hdr.slot               = uim_ptr->id + 1 ;
    uim_cmd_ptr->hdr.cmd_hdr.task_ptr   = uim_ptr->tcb_ptr;
    uim_cmd_ptr->hdr.cmd_hdr.sigs       = 0;
    uim_cmd_ptr->hdr.options            = UIM_OPTION_ALWAYS_RPT;
    uim_cmd_ptr->hdr.cmd_hdr.done_q_ptr = NULL;

    uim_cmd_ptr->manage_channel.mc_action        = UIM_MC_OPEN;
    uim_cmd_ptr->manage_channel.op_close_channel = 0;

    uim_cmd_ptr->hdr.rpt_function = uim_open_channel_cb;
    uim_cmd_ptr->hdr.user_data    = (uint32)uim_ptr;

    UIMDRV_MSG_HIGH_0(uim_ptr->id, "queue an internal open channel command to the head of the queue");

    /* Since we are doing an explicit q_put / q_insert rather than using uim_cmd(),
       we will free the allocated memory either in uim_process_generic_response or
       uim_send_generic_err_rpt based on the result of SELECT, after invoking the
       response callback */
    if ( q_cnt( &uim_ptr->task.cmd_q ) == 0 )
    {
      q_put( &uim_ptr->task.cmd_q, &uim_cmd_ptr->hdr.cmd_hdr.link );
    }
    else
    {
      /* Insert the reselect MF to the beginning of the queue */
      q_insert( & uim_ptr->task.cmd_q,
      & uim_cmd_ptr->hdr.cmd_hdr.link,
      & ((uim_cmd_type*) q_check( & uim_ptr->task.cmd_q ))->hdr.cmd_hdr.link );
    }
  }
} /*uim_queue_internal_open_channel_command */


/*===========================================================================

FUNCTION UIM_PROCESS_CARD_RESPONSE                EXTERNAL

DESCRIPTION
  This procedure processes the response received from the UIM card and changes
  the UIM server control variables based upon the processing status.

DEPENDENCIES
  This function uses global variables defined for the UIM server to determine
  the proper processing of the card response.

RETURN VALUE
  None.

SIDE EFFECTS
  The control variables of the UIM server are adjusted to reflect the change
  in the operation of the UIM server as a result of the processing on the
  response.

===========================================================================*/
void uim_process_card_response
(
  rex_sigs_type            *i_mask,    /* Mask of signals interested in */
  uim_instance_global_type *uim_ptr
)
{
  uim_cmd_status_type status                  = UIM_CMD_SUCCESS; /* UIM result type */
  /* flag to bypass the usual command response handling */
  boolean             uim_special_handling    = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  uim_slot_type cmd_slot = UIM_SLOT_NONE;
  RETURN_IF_INSTANCE_INVALID(uim_ptr->id);
  UIMDRV_MSG_LOW_0(uim_ptr->id,"uim_process_card_response");

  if (i_mask == NULL)
  {
    ERR_FATAL("uim_process_card_response i_mask  is NULL",0,0,0);
  }

  /* This is a case where UIM is recvied card removed and a
     succesful response from the UART ISR */
  if ( uim_ptr->flag.static_buffer_used == TRUE &&
       uim_ptr->command.static_cmd_buf.hdr.command == UIM_HOTSWAP_CARD_REM_F &&
      (uim_hw_if.intctrl[uim_ptr->id].m_hs_enable == TRUE
#ifdef FEATURE_UIM_BTSAP_CLIENT
       || IS_BTSAP_HANDLE_ACTIVE(uim_ptr)
#endif /* FEATURE_UIM_BTSAP_CLIENT */
       ))
  {
    if (uim_ptr->command.rsp_buf.sw1 != SW1_NORMAL_END)
    {
      UIMDRV_MSG_HIGH_0(uim_ptr->id,"HOTSWAP: Need to ignore this response");
    }
    uim_ptr->command.rsp_buf.sw1 = SW1_NORMAL_END;
    uim_ptr->command.rsp_buf.sw2 = 0x00;
  }
  /* Pending fetch is set to false when card return 0x9000 status word */
  if ( ( TRUE == uim_ptr->flag.fetch_pending) &&
       ( SW1_NORMAL_END == uim_ptr->command.rsp_buf.sw1) )
  {
    uim_ptr->flag.fetch_pending = FALSE;
  }

  /* Set the special handling flag to TRUE, when NV ENS is set and
     the SIM returns 6F 00 (technical problem) */
  if( uim_ptr->setting.pdown_on_tech_prob.to_pdown )
  {
    /* Return command error immediately when SIM returned a
       technical problem */
    if( uim_ptr->command.rsp_buf.sw1 == (byte) SW1_PROBLEM &&
        uim_ptr->command.rsp_buf.sw2 == (byte) SW2_SW1_PROBLEM &&
        uim_ptr->command.mode != UIM_PASSIVE_MODE )
    {
      /* set flag to bypass the usual command response handling */
      uim_special_handling = TRUE;
      uim_ptr->card_cmd.inc_tech_prob_count++;
      UIMDRV_MSG_ERR_1(uim_ptr->id,"Technical problem count is %d",
                       uim_ptr->card_cmd.inc_tech_prob_count);
      /* Return an error status if the command failed. */
      status = UIM_CMD_ERR;
      if(uim_ptr->card_cmd.inc_tech_prob_count == (UIM_MAX_REPEATED_TECH_PROBLEMS - 1))
      {
        /* Select MF upon two consecutive tech problems */
        uim_ptr->state.mf_selection.required = TRUE; 
        uim_ptr->state.mf_selection.channel  = 0; 
      }
    }
    else
    {
      /* reset the count of technical problems */
      uim_ptr->card_cmd.inc_tech_prob_count = 0;
    }
  }

  /* Process the command response in the usual way if special handling
     is not set */
  if( !uim_special_handling)
  {
    /* If static buffer is used, use the static command pointer as a
       parameter to uim_command_response */
    if (uim_ptr->flag.static_buffer_used)
    {
      cmd_slot = uim_ptr->command.static_cmd_buf.hdr.slot;
      status = uim_command_response(&uim_ptr->command.rsp_buf, uim_ptr);
    }
    else
    {
      if (uim_ptr->command.cmd_ptr != NULL)
      {
        cmd_slot = uim_ptr->command.cmd_ptr->hdr.slot;
        status = uim_command_response(&uim_ptr->command.rsp_buf, uim_ptr);
      }
    }
  }

#ifdef FEATURE_GSTK
  /* Mimic the case of no-FETCH to UIM_CMD_COMPLETED case */
  if(status == UIM_CMD_FETCH &&
     !gstk_uim_fetch_allow((gstk_slot_id_enum_type)cmd_slot))
  {
    status = UIM_CMD_COMPLETED;
  }
#endif /* FEATURE_GSTK */
  /* If the ICCID does not match, declare the UIM as faulty and
     power down the UIM */
  if ((status == UIM_ICCID_MISMATCH) || (status == UIM_VOLTAGE_MISMATCH))
  {
    if((!uim_ptr->flag.static_buffer_used) && (uim_ptr->command.cmd_ptr != NULL)
         && (uim_ptr->command.cmd_ptr->hdr.command == UIM_RESET_F) && (status == UIM_VOLTAGE_MISMATCH))
    {
       UIMDRV_MSG_HIGH_0(uim_ptr->id,"Processing response for the reset command");
       uim_process_generic_response(&uim_ptr->command.rsp_buf, uim_ptr);
    }
    /* set the status to ERROR */
    uim_ptr->state.status = UIM_ERR_S;

    /* Clear out any votes to keep UIM powered on */
    uim_ptr->state.ctl = UIM_NONE;
    uim_ptr->flag.powerdown = TRUE;

    /* flush all the commands in the command queue */
    uim_flush_command(UIM_FLUSH_ALL, uim_ptr);

    /* Whenever we receive a command, send an error report as the
    SIM is to be powered down */
    *i_mask |= UIM_CMD_Q_SIG;
    
    if((uim_ptr->extended_recovery.extended_recovery_enabled == TRUE) &&
       (status == UIM_ICCID_MISMATCH))
    {
      uim_extended_recovery_event_handler(uim_ptr, UIM_EXTENDED_RECOVERY_EVT_TIMER_START);
    }

    /* Notify the higher layers that UIM is declared faulty */
    if(UIM_MAX_INSTANCES > uim_ptr->id)
    {
      uim_notify_error(UIM_VOLTAGE_MISMATCH_S, uim_ptr);
    }

    /* reset the directories and send a power down to UIM */
    uim_reset_dir(uim_ptr);
    uim_power_down(uim_ptr);

    /* Reset the static command count */
    uim_ptr->command.static_cmd_buf.hdr.cmd_count = 1;

    /* reset the sw1 warning flag to FALSE without the need to reselect MF */
    uim_ptr->flag.sw1_warnings1_normal_end_occured = FALSE;

    /* reset the static buffer used flag */
    uim_ptr->flag.static_buffer_used = FALSE;

    /* Reset the command in progress flag */
    uim_ptr->flag.command_in_progress = FALSE;

    /* Clear the global command pointer as we are done with this command. */
    uim_ptr->command.cmd_ptr = NULL;
  }
  /* else if the polling is not successful */
  else if (status == UIM_POLL_ERROR)
  {
    uim_notify_error(UIM_POLL_ERR_S, uim_ptr);

    uim_ptr->state.status = UIM_ERR_S;

    /* include the command Q sig to the mask */
    *i_mask |= UIM_CMD_Q_SIG;

    /* Reset the static command count */
    uim_ptr->command.static_cmd_buf.hdr.cmd_count = 1;

    /* reset the static buffer used flag */
    uim_ptr->flag.static_buffer_used = FALSE;

    /* Reset the command in progress flag */
    uim_ptr->flag.command_in_progress = FALSE;

    /* reset the sw1 warning flag to FALSE without the need to reselect MF */
    uim_ptr->flag.sw1_warnings1_normal_end_occured = FALSE;

    /* Clear the global command pointer as we are done with this command. */
    uim_ptr->command.cmd_ptr = NULL;

  }
  else if (status == UIM_INSTRN_CHANGE)
  {
    if(!uim_ptr->flag.static_buffer_used)
    {
      if (uim_ptr->command.cmd_ptr != NULL)
      {
        if(uim_ptr->command.cmd_ptr->hdr.command == UIM_RESET_F)
        {
          /* Requeue the RESET F command */
          UIMDRV_MSG_HIGH_0(uim_ptr->id,
                            "Requeue the Reset command due to UIM_INSTRN_CHANGE");
          uim_insert_cmd_at_head_of_queue(uim_ptr->command.cmd_ptr, uim_ptr);
        }
      }
      *i_mask |= UIM_CMD_Q_SIG;
    }
    else
    {
      uim_power_down(uim_ptr);
      if (UIM_IS_CLK_FREQ_AT_4_8_MHZ(uim_ptr->id))
      {
        UIM_SWITCH_CLK_FREQ_TO_3_8_MHZ(uim_ptr->id);
        SET_UIM_BAUD_RATE_SLOT ( uim_ptr->id, UIM_CRCF_372_1, UIM_BRAF_1 );
      }
      uim_reset_uim (i_mask, uim_ptr->flag.me_powerup, uim_ptr);
    }
    /* reset the sw1 warning flag to FALSE without the need to reselect MF */
    uim_ptr->flag.sw1_warnings1_normal_end_occured = FALSE;
  }
  else if (status == UIM_CLK_FREQ_CHANGE_NEEDED)
  {
    uim_reset_uim (i_mask, uim_ptr->flag.me_powerup, uim_ptr);
  }
  else if (status == UIM_CONVENTION_CHANGE)
  {
    if(!uim_ptr->flag.static_buffer_used)
    {
      if (uim_ptr->command.cmd_ptr != NULL)
      {
        if(uim_ptr->command.cmd_ptr->hdr.command == UIM_RESET_F)
        {
          /* Requeue the RESET F command */
          UIMDRV_MSG_HIGH_0(uim_ptr->id,
                            "Requeue the Reset command due to UIM_CONVENTION_CHANGE");
          uim_insert_cmd_at_head_of_queue(uim_ptr->command.cmd_ptr, uim_ptr);
        }
        else
        {
          UIMDRV_MSG_HIGH_1(uim_ptr->id,
                            "Error: UIM was expecting the Reset command here 0x%x",
                            uim_ptr->command.cmd_ptr->hdr.command);
        }
      }
      *i_mask |= UIM_CMD_Q_SIG;
    }
    else
    {
      uim_reset_uim (i_mask, uim_ptr->flag.me_powerup,uim_ptr);
    }
    /* reset the sw1 warning flag to FALSE without the need to reselect MF */
    uim_ptr->flag.sw1_warnings1_normal_end_occured = FALSE;
  }
  /* else if a proactive command is pending */
  else if (status == UIM_CMD_FETCH)
  {
    /* If the command completed is an internal poll, then reset the timer */
    if ((uim_ptr->flag.static_buffer_used) &&
        (uim_ptr->command.static_cmd_buf.hdr.command == UIM_INTERNAL_POLL_F))
    {
      UIMDRV_MSG_MED_0(uim_ptr->id, "Set UIM POLL timer following a poll bf FETCH" );
      uim_set_poll_timer( uim_ptr->setting.poll_time_delay,
                          uim_ptr->setting.wakeup_poll_offset,
                          uim_ptr);

    }

    if (!uim_ptr->flag.static_buffer_used)
    {
      /* Check if sw1 warnings1 with normal end was generated during this operation
       If so, built an internal command to reselect MF */
      uim_handle_sw1_warnings1_normal_end_reselect_mf(uim_ptr);
    }

    /* Send a FETCH command to the SIM */
    uim_fetch (i_mask, uim_ptr);
  }
  /* else if the command was completed successfully */
  else if (status == UIM_CMD_COMPLETED)
  {
    /* Set the flag to false since the command is completed */
    uim_ptr->flag.command_in_progress = FALSE;

    /* Reset the internal command used flag */
    uim_ptr->flag.internal_command_used = FALSE;

    /* include the command Q sig to the mask */
    *i_mask |= UIM_CMD_Q_SIG;

    if (uim_ptr->flag.me_powerup)
    {
      if(uim_ptr->command.cmd_ptr)
      {
        /* If cmd_ptr is not null and the external command is a reset
         * and the internal command is an INTERNAL PUP, we need
         * to send a response for the reset command
         */
        if (uim_ptr->command.cmd_ptr->hdr.command == UIM_RESET_F)
        {
          uim_ptr->state.status = UIM_INITIALIZED_S;
        }
      }

      /* Do not send LINK established message when status is UIM_ERR_S */
      if(UIM_ERR_S != uim_ptr->state.status)
      {
        /*clearing TP Cache as a fresh TP will be sent upon link establishment*/
        uim_ptr->cache.int_term_profile_data.num_bytes = 0;
        uim_sm_link_established(uim_ptr);
      }

      uim_ptr->flag.me_powerup = FALSE;

      /* Trigger the first poll */
      if(UIM_ERR_S != uim_ptr->state.status)
      {
        uim_set_poll_timer( uim_ptr->setting.poll_time_delay,
                            uim_ptr->setting.wakeup_poll_offset,
                            uim_ptr);
      }
    }

    if(uim_nv_is_feature_enabled(UIMDRV_FEATURE_UIM_POLLING_ONLY_AT_POLLING_TIMER_EXPIRY,
                                 uim_ptr) == TRUE)
    {
      /* If it is an internal poll command then set the poll timer */
      if (uim_ptr->command.static_cmd_buf.hdr.command == UIM_INTERNAL_POLL_F && uim_ptr->flag.static_buffer_used)
      {
        UIMDRV_MSG_HIGH_0(uim_ptr->id,"Setting UIM POLL timer following a poll");
        uim_set_poll_timer( uim_ptr->setting.poll_time_delay, uim_ptr->setting.wakeup_poll_offset, uim_ptr);
      }
    }
    else
    {
      UIMDRV_MSG_HIGH_0 ( uim_ptr->id,"Setting POLL timer upon successfull completion of the command");
      uim_set_poll_timer( uim_ptr->setting.poll_time_delay, uim_ptr->setting.wakeup_poll_offset, uim_ptr);
    }

    if (uim_ptr->flag.static_buffer_used)
    {
      /* Set the status as Initialized if the power up command is
         completed successfully */
      if ((uim_ptr->command.static_cmd_buf.hdr.command == UIM_INTERNAL_ME_PUP_F)||
               (uim_ptr->command.static_cmd_buf.hdr.command == UIM_HOTSWAP_CARD_INS_F) ||
               (uim_ptr->command.static_cmd_buf.hdr.command == UIM_INTERNAL_WAKE_UP_F)
              )
      {
        /* Reset the counters */
        uim_ptr->debug.convention_change_count = 1;

        uim_ptr->card_cmd.instrn_toggle_count = 1;

        if(UIM_ERR_S != uim_ptr->state.status)
        {
          uim_ptr->state.status = UIM_INITIALIZED_S;
        }
        /* Reset the static command count */
        uim_ptr->command.static_cmd_buf.hdr.cmd_count = 1;

        /* reset the static buffer used flag */
        uim_ptr->flag.static_buffer_used = FALSE;
      } /* end if - powerup command */
    }
    else
    {
      /* Check if sw1 warnings1 with normal end was generated during this operation
       If so, built an internal command to reselect MF */
      uim_handle_sw1_warnings1_normal_end_reselect_mf(uim_ptr);
    }
    /* Clear the global command pointer as we are done with this command. */
    uim_ptr->command.cmd_ptr = NULL;
  } /* end if - command completed with no errors */
  /* if the result was not something that we expected */
  else if (status == UIM_CMD_ERR)
  {
    uim_process_cmd_err(i_mask, uim_ptr);
  }  /* status == UIM_CMD_ERR */
  else if (status == UIM_CMD_ERR_WITH_SFI)
  {
    /* NOTE - uim_ptr->flag.command_in_progress flag is not set to FALSE.
       REASON - The commnand has failed using SFI and we are now uncertain
       about the current directory on the card until we retry the command
       after performing a SELECT from MF.
       If the polling timer expires and this flag is reset, then we attempt
       to poll the card at an incorrect directory. */

    /* Notify GSDI via a synchronous api of failed SFI so they can mark it dirty */
    UIMDRV_MSG_HIGH_0(uim_ptr->id, "SFI operation failed - Retrying command without using SFI");


    /* Reset the internal command used flag */
    uim_ptr->flag.internal_command_used = FALSE;

    /* Re-queue the command so its picked up and processed without SFI the next time round */
    if ( q_cnt( & uim_ptr->task.cmd_q ) == 0 )
    {
      q_put( & uim_ptr->task.cmd_q, & uim_ptr->command.cmd_ptr->hdr.cmd_hdr.link );
    }
    else
    {
      q_insert( & uim_ptr->task.cmd_q,
                & uim_ptr->command.cmd_ptr->hdr.cmd_hdr.link,
                & ((uim_cmd_type*) q_check( & uim_ptr->task.cmd_q ))->hdr.cmd_hdr.link );
    }

    /* Include the command Q sig to the mask */
    *i_mask |= UIM_CMD_Q_SIG;

    /* Clear the global command pointer as we are done with this command. */
    uim_ptr->command.cmd_ptr = NULL;
  }
  else if (status == UIM_SWITCH_VOLTAGE)
  {
    /* Reset the card - the interface should already be powered down and
       the voltage class selected */
    if(!uim_ptr->flag.static_buffer_used)
    {
      if (uim_ptr->command.cmd_ptr != NULL)
      {
        if(uim_ptr->command.cmd_ptr->hdr.command == UIM_RESET_F)
        {
          /* Requeue the RESET F command */
          UIMDRV_MSG_HIGH_0(uim_ptr->id,
                            "Requeue the Reset command due to UIM_SWITCH_VOLTAGE");
          uim_insert_cmd_at_head_of_queue(uim_ptr->command.cmd_ptr, uim_ptr);
        }
      }
      *i_mask |= UIM_CMD_Q_SIG;
    }
    else
    {
      /* Reset the card - the interface should already be powered down and
         the voltage class selected */
      uim_reset_uim (i_mask, uim_ptr->flag.me_powerup,uim_ptr);
    }
    /* reset the sw1 warning flag to FALSE without the need to reselect MF */
    uim_ptr->flag.sw1_warnings1_normal_end_occured = FALSE;
  }  /* status == UIM_SWITCH_VOLTAGE */

#ifdef FEATURE_UIM_TOOLKIT_UTK
  if (uim_ptr->command.cmd_ptr != NULL)
  {
    if (uim_ptr->command.cmd_ptr->hdr.command == UIM_TERMINAL_RESPONSE_F)
    {
      UIM_FREE(uim_ptr->command.cmd_ptr);
    }
  }
#endif /* FEATURE_UIM_TOOLKIT_UTK */

  uim_explicit_reselect_mf_aid(uim_ptr);
} /* uim_process_card_response */


/*===========================================================================

FUNCTION UIM_PET_AND_WAIT

DESCRIPTION
  This procedure accepts rex_sigs_type mask and keeps waiting for any of
  the signals in the mask specified.  Also, if it is time to report to
  the dog, the dog would be pet.

DEPENDENCIES
  None

RETURN VALUE
  rex_sigs_type

SIDE EFFECTS
  None

===========================================================================*/
rex_sigs_type uim_pet_and_wait
(
  rex_sigs_type             sigs_to_wait_for,
  uim_instance_global_type *uim_ptr
)
{
  rex_sigs_type sigs_received;

#ifdef FEATURE_UIM_TEST_FRAMEWORK
  #error code not present
#else
  do
  {
    /* Wait for the signals requested and also, Include dog report
     * signal to pet the dog. Also include task stop signal.
     */
    sigs_received = rex_wait( sigs_to_wait_for | UIM_DOG_HB_RPT_SIG | UIM_TASK_STOP_SIG);

    if ((sigs_received & UIM_DOG_HB_RPT_SIG) != 0)
    {
      (void) rex_clr_sigs(uim_ptr->tcb_ptr, UIM_DOG_HB_RPT_SIG);
      dog_hb_report(uim_ptr->task.dog_rpt_var);
    } /* if UIM_RPT_TIMER_SIG */
  } while( (sigs_received & ( sigs_to_wait_for | UIM_TASK_STOP_SIG ) ) == 0 );
  return sigs_received;
#endif /* FEATURE_UIM_TEST_FRAMEWORK */
} /* uim_pet_and_wait */


/*===========================================================================

FUNCTION UIM_WAIT_FILTER_NON_RECOVERY_CMDS

DESCRIPTION
This function waits for list of signals that we are interested in and if
we are interested in UIM_CMD_Q_SIG, we will send it out as a received
signal only if the command in the command queue is a recovery command.

DEPENDENCIES
  None

RETURN VALUE
  signals returned by rex_wait

SIDE EFFECTS
  Does not allow processing any other commands other than those commands
  in the uim_cmd_q which are marked to be recovery commands

===========================================================================*/
static rex_sigs_type uim_wait_filter_non_recovery_cmds(
  rex_sigs_type             sigs_to_wait_for,
  uim_instance_global_type *uim_ptr
)
{
  rex_sigs_type sigs_received = 0;
  uim_cmd_type *uim_ret_ptr   = NULL;
  do
  {
    /* Check if are interested in looking at the commands (UIM_CMD_Q_SIG) and
     * there are commads in the command queue
     */
    if (((sigs_to_wait_for & UIM_CMD_Q_SIG) != 0) && (q_cnt(& uim_ptr->task.cmd_q) != 0))
    {
      uim_ret_ptr = (uim_cmd_type *)q_check(&uim_ptr->task.cmd_q);
      if(NULL != uim_ret_ptr)
      {
        /* Check to make sure that head of the queue has a recovery command */
        if ( ((int)uim_ret_ptr->hdr.options & (int)UIM_OPTION_RECOVERY_CMD ) ||
             (UIM_POWER_DOWN_F == uim_ret_ptr->hdr.command)  )
        {
          /* The command at the head of the command queue is a recovery
           * command.  Return a command queued signal along with any other
           * signals received so that the command would be processed.
           */
          UIMDRV_MSG_HIGH_1(uim_ptr->id,"Received a recovery command returning %x",
                            ( UIM_CMD_Q_SIG | sigs_received ) );
          return ( UIM_CMD_Q_SIG | sigs_received );
        }
        else
        {
          UIMDRV_MSG_HIGH_0(uim_ptr->id,"Command at the tip not a recovery command" );
        }
      }
      else
      {
        UIMDRV_MSG_HIGH_0(uim_ptr->id,"q_check returned NULL");
      }
    }

    /* Wait for signals we are interested in */
    sigs_received = rex_wait(sigs_to_wait_for);

    if (sigs_received & UIM_CMD_Q_SIG &&

        (uim_ptr->command.mode == UIM_RECOVERY_MODE) )
    {
      /* We have received a command que signal, clear the signal and
       * continue.  If this command is indeed a recovery command then
       * the signal will be attached to the list of signals and returned
       * to the caller in the next iteration
       */
      (void) rex_clr_sigs(uim_ptr->tcb_ptr, UIM_CMD_Q_SIG);

      /* Remove the UIM_CMD_Q_SIG from the signals received */
      sigs_received &= ~UIM_CMD_Q_SIG;
    }
  } while ( (sigs_received & sigs_to_wait_for) == 0 );

  UIMDRV_MSG_LOW_1(uim_ptr->id,"Returning signals %x", sigs_received);
  return sigs_received;
}


/*===========================================================================
  FUNCTION uim_start_initial_powerup

  DESCRIPTION
    This function powers up the card for the first time by queuing a power
    up command.

  PARAMETERS
    uim_ptr -    Pointer to the current instance of UIM
    iMask_ptr -  Pointer to the mask of signals that the uim task is interested
                 in handling.   The mask may be changed by this function.

===========================================================================*/
void uim_start_initial_powerup(uim_instance_global_type *uim_ptr, rex_sigs_type *iMask_ptr)
{
#ifdef FEATURE_UIM_BTSAP_CLIENT
  rex_sigs_type  uim_remote_signal = UIM_ALL_ZERO_SIG;
#endif /* FEATURE_UIM_BTSAP_CLIENT */

  /* -------------------------------------------------------
  ** Check if the power up is due to a task power up and the
  ** associated UIM instance is enabled. If TRUE then start
  ** the Power up procedures for UIM
  ** ----------------------------------------------------- */
  if(TRUE == m_HWConfig.uim_slot_config[uim_ptr->id].disableUim)
  {
    /* Slot is disabled */
    return;
  }

  UIMDRV_MSG_LOW_0(uim_ptr->id, "This is the first attempt to reset the card" );

    /* Add delay to UIM power up */
  if(uim_ptr->setting.config_params.future_use[UIM_RFU_INDEX_POWER_UP_DELAY_VAL])
  {
    UIMDRV_MSG_LOW_1(uim_ptr->id, "Delay the powerup by 0x%x secs", uim_ptr->setting.config_params.future_use[UIM_RFU_INDEX_POWER_UP_DELAY_VAL] );
    uim_timed_sleep((uim_ptr->setting.config_params.future_use[UIM_RFU_INDEX_POWER_UP_DELAY_VAL]* 1000),
                    uim_ptr,
                    UIM_ALL_ZERO_SIG);
  }

#ifdef FEATURE_UIM_BTSAP_CLIENT
#ifndef FEATURE_UIM_TEST_FRAMEWORK
  /* Add delay to local power up card if last mode is remote mode */
  if ( (MCFG_PWRDOWN_RES_NONE != mcfg_pwr_down_reason()) && 
       (UIM_CONNECTION_BTSAP == uim_ptr->state.last_connection_mode))
  {
    UIMDRV_MSG_HIGH_1(uim_ptr->id,"Delay the powerup by %d secs", uim_ptr->state.local_card_pup_delay);
    uim_remote_signal = uim_timed_sleep_ext((uim_ptr->state.local_card_pup_delay * 1000),
                                            uim_ptr,
                                            UIM_REMOTE_CONNECTION_AVAILABLE_SIG);
    if (uim_remote_signal & UIM_REMOTE_CONNECTION_AVAILABLE_SIG)
    {
      (void)rex_clr_sigs(uim_ptr->tcb_ptr, UIM_REMOTE_CONNECTION_AVAILABLE_SIG);
      uim_ptr->flag.uim_not_allow_local_card_power_up = TRUE;
    }
    else
    {
      UIMDRV_MSG_HIGH_0(uim_ptr->id, "Remote Connect request is not received, Powering up local card");
      uim_btswap_save_current_mode_to_efs(uim_ptr,UIM_CONNECTION_ISO);
    }
  }
  else
  {
    uim_btswap_save_current_mode_to_efs(uim_ptr,UIM_CONNECTION_ISO);
  }
#endif /* FEATURE_UIM_TEST_FRAMEWORK */
#endif /* FEATURE_UIM_BTSAP_CLIENT */
  
  /* Not allow UIM to recevied any hotswap event if we have any wait in Task Startup */
  uim_hotswap_init(uim_ptr);
	
  /* This is the first attempt for this command  */
  uim_ptr->command.static_cmd_buf.hdr.cmd_count = 1;

  /* Reset the counters */
  uim_ptr->debug.convention_change_count = 1;
  uim_ptr->card_cmd.instrn_toggle_count = 1;
  /* Start the power up sequence for given slot*/
  uim_ptr->command.static_cmd_buf.hdr.slot =
    (uim_slot_type)(uim_ptr->id + 1);

  /* The power up mode will be set after parsing the historical characters
     from the ATR */
  uim_reset_uim(iMask_ptr, uim_ptr->flag.me_powerup, uim_ptr);
  uim_ptr->flag.task_startup = FALSE;

} /* uim_start_initial_powerup */


/*===========================================================================
  FUNCTION uim_timers_init

  DESCRIPTION
    Performs the one time power up initialization of timers.

  PARAMETERS
    uim_ptr - Pointer to the current instance of UIM

===========================================================================*/
void uim_timers_init(uim_instance_global_type *uim_ptr)
{
  /* -----------------
  ** Initialize timers
  ** ----------------- */
  timer_def( &uim_ptr->command.cmd_rsp_timer, &uim_ptr->task.always_on_timer_group,
             uim_ptr->tcb_ptr, 0,
             (timer_t1_cb_type)uim_swt_cmd_rsp_timer_expiry_cb, (timer_cb_data_type)uim_ptr);

  if(uim_nv_is_feature_enabled(UIMDRV_FEATURE_HANDLE_NO_ATR_IN_40000_CLK_CYCLES,
                               uim_ptr) == TRUE)
  {
    timer_def( &uim_ptr->atr.atr_40k_timer, &uim_ptr->task.always_on_timer_group,
               uim_ptr->tcb_ptr, 0,
               (timer_t1_cb_type)uim_40k_atr_timer_expiry_cb, (timer_cb_data_type)uim_ptr);
  }

  /* We want the poll timer to be on even-if the target is in sleep.. so
   * put it into the appropriate group
   */
  timer_def( &uim_ptr->poll.wake_up_poll_timer,
             &uim_ptr->task.always_on_timer_group,
             uim_ptr->tcb_ptr, 0,
             (timer_t1_cb_type)uim_wake_up_poll_timer_expiry_cb, (timer_cb_data_type)uim_ptr);

  rex_def_timer( &uim_ptr->poll.no_wake_up_poll_timer,
                 uim_ptr->tcb_ptr, UIM_POLL_TIMER_SIG );

#ifdef FEATURE_UIM_REMOTE_UIM
  timer_def( &uim_ptr->remote.remote_cmd_rsp_timer, &uim_ptr->task.always_on_timer_group,
             uim_ptr->tcb_ptr, 0,
             (timer_t1_cb_type)uim_remote_cmd_rsp_timer_expiry_cb, (timer_cb_data_type)uim_ptr);
#endif /* FEATURE_UIM_REMOTE_UIM */

} /* uim_timers_init */


/*===========================================================================
  FUNCTION uim_process_pending_poll

  DESCRIPTION
    This function is called if the poll_pending flag is set to TRUE.

  PARAMETERS
    uim_ptr - Pointer to the current instance of UIM
    iMask_ptr - Pointer to the mask of signals that the uim task is interested
                in handling.   The mask may be changed by this function.

===========================================================================*/
void uim_process_pending_poll(uim_instance_global_type *uim_ptr, rex_sigs_type *iMask_ptr)
{
  RETURN_IF_INSTANCE_INVALID(uim_ptr->id);

  if(TRUE == m_HWConfig.uim_slot_config[uim_ptr->id].disableUim)
  {
    /* Slot is disabled */
    return;
  }

  /* Do not allow the poll while a command is in progress are
   * recovery is in process
   */
  if (!uim_ptr->flag.command_in_progress &&
       !uim_ptr->flag.recovery_triggered &&
      (uim_ptr->command.mode != UIM_RECOVERY_MODE))
  {
    if ( (uim_ptr->command.mode == UIM_PASSIVE_MODE) &&
         ((int)uim_ptr->state.ctl & (int)UIM_PASSIVE) != (int)UIM_PASSIVE )
    {
      /* Restart the poll timer for the polling slot */
      uim_set_poll_timer( uim_ptr->setting.poll_time_delay,
                          uim_ptr->setting.wakeup_poll_offset,
                          uim_ptr);

      /* Reset the flag */
      uim_ptr->flag.poll_pending = FALSE;

      UIMDRV_MSG_MED_0(uim_ptr->id, "Protocol un-known in passive mode re-set poll timer");
    }
    else if (uim_ptr->state.status == UIM_INITIALIZED_S)
    {
      /* Poll the UIM only if we are already initialized */
      uim_process_polling(iMask_ptr, uim_ptr);
    }
    else if (uim_ptr->state.status == UIM_CLK_OFF_S)
    {
      if ((uim_ptr->state.ctl != UIM_NONE)
          || (uim_ptr->state.type.init && uim_ptr->state.type.proactive_uim)
          /* Sending POLL command to confirm the card status */
          || (uim_ptr->hotswap.hotswap_card_removed_suspicious == TRUE)
          )
      {
         /* Provide clock to UIM */
        uim_stop_power_save(uim_ptr);
        UIMDRV_MSG_MED_0(uim_ptr->id,"Clock started");

        /* Set the status */
        uim_ptr->state.status = UIM_INITIALIZED_S;

        /* Poll the card */
        uim_process_polling(iMask_ptr, uim_ptr);
      } /* if the card is a pro-active card */
      else
      {
        uim_set_poll_timer( uim_ptr->setting.poll_time_delay,
                            uim_ptr->setting.wakeup_poll_offset,
                            uim_ptr);
        uim_ptr->flag.poll_pending = FALSE;
        UIMDRV_MSG_MED_0(uim_ptr->id, "Setting UIM Poll timer for non pro-active scenario");
      }
    } /* status is CLK_OFF_S */
    else if (uim_ptr->state.status == UIM_ERR_S)
    {
      uim_ptr->flag.poll_pending = FALSE;
      /* Restart poll timer is not required for error state*/
      UIMDRV_MSG_HIGH_0 ( uim_ptr->id, "Clearing poll_pending flag as polling slot is invalid");
    }
  }
  else
  {
    UIMDRV_MSG_MED_2(uim_ptr->id, "Not processing the pending poll as cmd is in progress - in_recovery = %d cmd_progress = %d",
                     uim_ptr->flag.recovery_triggered,
                     uim_ptr->flag.command_in_progress);
    /* In case Polling is failing and there is no pending command then
       check the card status for that slot */
    if (uim_ptr->flag.command_in_progress == 0)
    {
      if(uim_hw_if.intctrl[uim_ptr->id].m_hs_enable == TRUE)
      {
        UIM_DETECT_CARD_STATUS(uim_ptr);
      }
    }
  }
} /* uim_process_pending_poll */


/**
*  This function is called if the fetch_pending flag is set to TRUE
*
*  @param uim_ptr Pointer to the current instance of the UIM global structure
*  @return void 
*/
void uim_process_pending_fetch(uim_instance_global_type *uim_ptr)
{
  if (uim_ptr->state.status == UIM_ERR_S || uim_ptr->command.mode != UIM_NORMAL_MODE)
  {
    uim_ptr->flag.fetch_pending = FALSE;
  }
  else if ((uim_ptr->flag.command_in_progress == FALSE) && 
            (q_cnt(&uim_ptr->task.cmd_q) == 0 ))
  {
    uim_ptr->state.last_selected.required = TRUE;
    uim_ptr->state.last_selected.channel  = 0;
    uim_explicit_reselect_mf_aid(uim_ptr);
    uim_ptr->flag.fetch_pending = FALSE;
  }
  UIMDRV_MSG_LOW_1(uim_ptr->id,"uim_process_pending_fetch status 0x%x",
                   uim_ptr->flag.fetch_pending);
}/* uim_process_pending_fetch*/


/**
*  This function confirms if the task power dwon is allowed currently or not
*
*  @param uim_ptr Pointer to the current instance of the UIM global structure
*  @return boolean TRUE    if task powered down allowed
*                           FALSE  if task power down not allowed due to some critical
*                                      on-going operation on the card.
*/
boolean uim_is_task_power_down_allowed(uim_instance_global_type *uim_ptr)
{
  boolean is_allowed = FALSE;
  uim_cmd_name_type  curr_cmd = UIM_NO_SUCH_COMMAND_F;

  if (!uim_ptr->flag.command_in_progress)
  {
    is_allowed = TRUE;
  }
  /* Immediately power down for POLL, internal select and recovery commands */
  else if ((uim_ptr->flag.static_buffer_used) || (uim_ptr->flag.recovery_triggered))
  {
    is_allowed = TRUE;
  }
  else if ( (uim_ptr->command.cmd_ptr != NULL) &&
            (!uim_ptr->flag.static_buffer_used) && (uim_ptr->flag.command_requeued == FALSE))
  {
    curr_cmd = uim_ptr->command.cmd_ptr->hdr.command;
    if (((int)uim_ptr->command.cmd_ptr->hdr.options & (int)UIM_OPTION_ALWAYS_RPT) != 0)
    {
      switch (uim_ptr->command.cmd_ptr->hdr.command)
      {
        /* For comands which do not have side impact on card when suddenly powered down, allow task power down
           For other commands (write,authentication) allow the command to be completed */
        case UIM_READ_BIN_CMD_F:
        case UIM_READ_REC_CMD_F:
        case UIM_SELECT_F:
        case UIM_ACCESS_F:
        case UIM_MANAGE_CHANNEL_F:
        case UIM_SEEK_F:
        case UIM_VERIFY_CHV_F:
        case UIM_DISABLE_CHV_F:
        case UIM_ENABLE_CHV_F:
        case UIM_STATUS_F:
        case UIM_INTERNAL_FETCH_F:
        case UIM_RESET_F:
        case UIM_SILENT_RECOVER_UICC_F:
          {
            is_allowed = TRUE;
          }
        default:
        {
          /* For other commands, power down task if there were NULL bytes */
          if (uim_ptr->card_cmd.infinite_null_count)
          {
            UIMDRV_MSG_HIGH_0(uim_ptr->id,"There are NULL byte on the external command,so power down task ");
            is_allowed = TRUE;
          }
        }
      }
    }
  }

  UIMDRV_MSG_HIGH_2(uim_ptr->id,
                    "Returning from uim_is_task_power_down_allowed with 0x%x, curr cmd 0x%x",
                     is_allowed, curr_cmd);
  
  return is_allowed;
}/* uim_is_task_power_down_allowed */


/**
*  This function performs the final cleanup of UIM state and
*  stops the UIM task.
*
*  @param uim_ptr Pointer to the current instance of the UIM global structure
*  @param i_mask_ptr The or'd values of signals that the UIM
*                    task is interested in handling.  This
*                    function may change the value of
*                     i_mask_ptr.
*  @param uim_group RCINIT Group number.
*  @param stop_sig_ptr Pointer to the stop sig information.
*  @return boolean TRUE if task powered down successfully
*/
boolean uim_task_power_down
(
  uim_instance_global_type *uim_ptr, rex_sigs_type *i_mask_ptr
  ,RCINIT_GROUP uim_group,
  RCEVT_SIGEX_SIGREX  *stop_sig_ptr
)
{
  if (uim_ptr->id >= UIM_MAX_INSTANCES)
  {
    return FALSE;
  }

  if(!uim_is_task_power_down_allowed(uim_ptr))
  {
    /* If we are off target, then power down even if there is a command in progress */
#ifndef FEATURE_UIM_TEST_FRAMEWORK
    /* Do not power down if there is a command in progress */
    return FALSE;
#endif
  }

  /* Setting the status to ERROR prior to invoking power down.
     While shutting the LDO down we evaluate the uim status to
     cater to the user customized LDO settings if needed. */
  uim_ptr->state.status = UIM_ERR_S;

  if(FALSE == m_HWConfig.uim_slot_config[uim_ptr->id].disableUim)
  {
    /* Processing of command (READ/SELSEC etc) is aborted upon receipt
      of UIM_TASK_STOP_SIG, While aborting the processing of current
      comamnd make sure that All interrupt is MASKED.
    */
    uim_power_down(uim_ptr);
    UIMDRV_MSG_HIGH_0(uim_ptr->id, "Task clean up completed");
    if(uim_ptr->flag.command_in_progress && !(uim_ptr->flag.static_buffer_used))
    {
      /*reset the command transacted flag as response 
      is not succesfully received from card*/
      uim_ptr->command.rpt_buf.cmd_transacted = FALSE;

      /* Set the status words to default before sending the rpt */
      uim_ptr->command.rpt_buf.sw1 = (uim_sw1_type) 0;
      uim_ptr->command.rpt_buf.sw2 = (uim_sw2_type) 0;
      uim_send_err_rpt(uim_ptr->command.cmd_ptr, uim_ptr);
    }
    uim_flush_command(UIM_FLUSH_ALL, uim_ptr);
    uim_reset_dir(uim_ptr);
  }

  /* Let us not be here time and again */
  uim_ptr->flag.powering_down_task = FALSE;
  uim_ptr->flag.task_dereg_from_rcinit = TRUE;

  /* Deregister from dog */
  dog_hb_deregister(uim_ptr->task.dog_rpt_var);

  rcinit_unregister_term_group(uim_group, RCEVT_SIGEX_TYPE_SIGREX, stop_sig_ptr);
  rcinit_handshake_term();

#ifndef FEATURE_UIM_TEST_FRAMEWORK
  /* Do not process any signals after this */
  *i_mask_ptr = 0x00;
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

  UIMDRV_MSG_HIGH_0(uim_ptr->id, "Task clean up completed");

  return TRUE;
} /* uim_task_power_down */


/**
*  This function determines whether we can stop the SIM clock,
*  and if the conditions are met the clock is stopped.
*
*  @param uim_ptr Pointer to the current instance of the UIM global structure
*  @param rex_signals_mask The or'd values of UIM task signals
*                          that are set.
*/
void uim_check_q_and_stop_clock(uim_instance_global_type *uim_ptr,
                                rex_sigs_type rex_signals_mask)
{

  RETURN_IF_INSTANCE_INVALID(uim_ptr->id);

  if(TRUE == m_HWConfig.uim_slot_config[uim_ptr->id].disableUim)
  {
    /* Slot is disabled */
    return;
  }

  /* We don't stop the clock if there is a command in progress or powering down */
  if (uim_ptr->flag.command_in_progress || uim_ptr->flag.powering_down_task)
  {
    return;
  }

  /* We don't stop the clock for these status values */
  if ((uim_ptr->state.status == UIM_POWERED_DOWN_S) ||
      (uim_ptr->state.status == UIM_ERR_S) ||
      (uim_ptr->state.status == UIM_CLK_OFF_S)
      )
  {
    return;
  }

  /* We don't stop the clock if there is any cmd in the queue */
  if(q_cnt(&uim_ptr->task.cmd_q) != 0)
  {
    return;
  }
  /* do not stop the clock if the card is requesting MORE TIME */
  if(uim_ptr->command.more_time_pcmd_fetched == TRUE)
  {
    return;
  }

  if (uim_start_power_save(uim_ptr))
  {
    /* Change the status if clock stop was successful */
    uim_ptr->state.status = UIM_CLK_OFF_S;
    UIMDRV_MSG_MED_0(uim_ptr->id,"Clock stopped");
  }
} /* uim_check_q_and_stop_clock */



/*===========================================================================
  FUNCTION uim_rcevt_signal_name

  DESCRIPTION
    Determines the signal event string for the UIM task startup based on the
    slot number.

  PARAMETERS
    uim_ptr - Pointer to the current instance of UIM

===========================================================================*/
void uim_rcevt_signal_name(uim_instance_global_type *uim_ptr)
{
#ifdef FEATURE_UIM_TEST_FRAMEWORK
  #error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */
} /* uim_rcevt_signal_name */


/*===========================================================================
  FUNCTION get_rex_sigs_mask

  DESCRIPTION
    Returns the mask of signals that the uim task needs to handle.
    This function blocks if there are no signals set since it calls rex_wait().

  PARAMETERS
    uim_ptr - Pointer to the current instance of UIM
    i_mask  - The mask of signals that the uim task is interested in

  SIDE EFFECTS
    Blocks if there are no signals set.

===========================================================================*/
rex_sigs_type get_rex_sigs_mask(uim_instance_global_type *uim_ptr, rex_sigs_type i_mask)
{
  rex_sigs_type rex_signals_mask = 0;

  if (uim_ptr->id >= UIM_MAX_INSTANCES)
  {
    return rex_signals_mask;
  }
  
  /* If we are in recovery mode, filter away non recovery commands */
  if ( uim_ptr->command.mode == UIM_RECOVERY_MODE )
  {
    rex_signals_mask =
      uim_wait_filter_non_recovery_cmds(i_mask, uim_ptr) & i_mask;
  }
  /* else if the command q sig is set in the mask and there is a command in
     the command queue, then set rex signals mask as command q sig */
  else if (((i_mask & UIM_CMD_Q_SIG) != 0) &&
           (q_cnt(& uim_ptr->task.cmd_q) != 0))
  {
    rex_signals_mask = UIM_CMD_Q_SIG;
  }
  /* else do the rex wait and mask the signals that we are interested in */
  else
  {
    rex_signals_mask = rex_wait(i_mask) & i_mask;
  }

 /* Replace with for loop */
  if( uim_hw_if.intctrl[uim_ptr->id].m_hs_enable == TRUE
#ifdef FEATURE_UIM_BTSAP_CLIENT
      || IS_BTSAP_HANDLE_ACTIVE(uim_ptr)
#endif /* FEATURE_UIM_BTSAP_CLIENT */
    )
  {
    /* We need to do the ORING of the hot swap signal as hot swap has a
       different queue and it is possible that UIM task receives the Hot
       Swap signal with other signals (CMD \ RSP and Response timeout signal).

       Hot swap signal is always have the high priority then the command signal,
       So in case where we receive a command signal with it then we drop command
       signal.

       In case where response signal and hotswap signal belong to the same slot,
       then hotswap signal will process and we drop the response signal.

       In case where response signal and hotswap signal do not belong to the same slot,
       then we process the response signal first and drop the hotswap signal.
    */
    if (((i_mask & UIM_HOTSWAP_CMD_CARD_REM_SIG) != 0) &&
        (q_cnt(& uim_ptr->hotswap.hotswap_rem_cmd_q) != 0))
    {
      rex_signals_mask  |= UIM_HOTSWAP_CMD_CARD_REM_SIG;
    }
    else if (((i_mask & UIM_HOTSWAP_CMD_CARD_INS_SIG) != 0) &&
             (q_cnt(& uim_ptr->hotswap.hotswap_ins_cmd_q) != 0))
    {
      rex_signals_mask  |= UIM_HOTSWAP_CMD_CARD_INS_SIG;
    }
  }

  return rex_signals_mask;
} /* get_rex_sigs_mask */

/**
*  This function returns the mask of signals that uim we need to
*  handle in uim_task.
*
*  Needs to be called after the m_HWConfig structure is
*  initialized
*
*  @param uim_ptr A pointer to the uim global structure
*
*  @return rex_sigs_type The mask of signals to handle
*/
rex_sigs_type uim_get_imask(uim_instance_global_type *uim_ptr)
{
  rex_sigs_type i_mask = 0;

  if(TRUE == m_HWConfig.uim_slot_config[uim_ptr->id].disableUim)
  {
    /* If the UIM slot is disabled, we only care about these sigs */
    i_mask = UIM_DOG_HB_RPT_SIG | UIM_TASK_STOP_SIG;
  }
  else
  {
    i_mask = UIM_DOG_HB_RPT_SIG |
             UIM_TASK_STOP_SIG |
#ifdef FEATURE_UIM_TEST_FRAMEWORK
             #error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */
             UIM_CMD_RSP_SIG |
             UIM_CMD_RSP_TIMEOUT_SIG |
             UIM_POLL_TIMER_SIG |
             UIM_STATE_TOGGLE_SIG |
             UIM_TRANSACTION_SIG |
#ifdef FEATURE_UIM_USB_UICC
             UIM_USB_FALLBACK_TO_ISO_SIG |
             UIM_USB_REMOTE_WAKEUP_SIG |
#endif /* FEATURE_UIM_USB_UICC */
             UIM_HOTSWAP_CMD_CARD_INS_SIG|
             UIM_SUSPICIOUS_CARD_REM_SIG|
             UIM_HOTSWAP_CMD_CARD_REM_SIG|
             UIM_EFSLOG_PURGE_SIG|
             UIM_BUSY_IND_TIMER_EXP_SIG|
             UIM_TRANS_TIMER_EXP_SIG|
             UIM_SIMULATE_NULL_TIMER_EXP_SIG|
             UIM_MCGF_NV_REFRESH_SIG|
             UIM_EXT_RECOVERY_TIMER_EXP_SIG|
#ifdef FEATURE_UIM_BTSAP_CLIENT
             UIM_REMOTE_CONNECTION_UNAVAILABLE_SIG|
#endif /* FEATURE_UIM_BTSAP_CLIENT */
             UIM_FETCH_PENDING_SIG;
  }

  return i_mask;
} /*uim_get_imask*/


#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */


#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */


/*===========================================================================

FUNCTION UIM_TASK_COMMON

DESCRIPTION
  This procedure is the entrance procedure for the UIM task.

DEPENDENCIES
  None

RETURN VALUE
  Does not return.

SIDE EFFECTS
  None

===========================================================================*/
void uim_task_common
(
  dword uim_global_ptr
)
{

  rex_sigs_type             rex_signals_mask;/* Mask of signals returned by rex */
  rex_sigs_type             i_mask;          /* Mask of signals interested in */
  uim_instance_global_type *uim_ptr = (uim_instance_global_type*)uim_global_ptr;
  boolean                   b_goto_top = FALSE;
  boolean                   b_task_powered_down = FALSE;

  #ifdef FEATURE_UIM_TEST_FRAMEWORK
  #error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

  RCEVT_SIGEX_SIGREX     stop_sig;
  RCINIT_GROUP uim_group;

  stop_sig.signal = rex_self();
  stop_sig.mask = UIM_TASK_STOP_SIG;

  uim_group = rcinit_lookup_group_rextask(rex_self());
  rcinit_register_term_group(uim_group, RCEVT_SIGEX_TYPE_SIGREX, &stop_sig);
  ASSERT(uim_ptr);

  #ifdef FEATURE_UIM_TEST_FRAMEWORK
  #error code not present
#endif/* FEATURE_UIM_TEST_FRAMEWORK */

  /* Handshake with RCInit framework */
  rcinit_handshake_startup();

  #ifdef FEATURE_UIM_TEST_FRAMEWORK
  #error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

  /* Initialization of Critical Sections */
  uim_init_critical_secs(uim_ptr);

  /* Initialization of Subscription Manager */
  uim_sm_init(uim_ptr);

  /* Initialization of UIMDRV NV items  */
  uim_nv_init(uim_ptr);

  /* -----------------
  ** Initialize timers
  ** ----------------- */
  uim_timers_init(uim_ptr);

  /* ------------------------------------------------------------------
  ** Register for Watchdog
  ** ------------------------------------------------------------------ */

  uim_ptr->task.dog_rpt_var = dog_hb_register_rex((rex_sigs_type) UIM_DOG_HB_RPT_SIG);

  uim_dev_init(uim_ptr);

  /* SWP(Single Wire Protocl) for NFC interface &
     SWP shall be slot based.
   */
  uim_update_swp_capability(uim_ptr);

  /* This needs to be after uim_dev_init because it depends on hw disable/enabled */
  i_mask = uim_get_imask(uim_ptr);

  /* This needs to be after uim_dev_init because it depends on UIM slot disable/enabled */
#ifdef FEATURE_MODEM_CONFIG_REFRESH
  if (FALSE == uimdrv_mcfg_refresh_register(uim_ptr))
  {
    UIMDRV_MSG_ERR_0(uim_ptr->id, "UIMDRV MCFG Registration failed");
  }
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

  UIMDRV_MSG_LOW_0(uim_ptr->id, "Acknowledged the Task start signal " );

  if(uim_nv_is_feature_enabled(UIMDRV_FEATURE_LOG_TO_EFS,
                               uim_ptr) == TRUE)
  {
    (void)uim_efslog_init(uim_ptr);
  }

#ifdef FEATURE_UIM_USB_UICC
  if(uim_is_usb_enabled(uim_ptr))
  {
    uim_usb_init(uim_ptr);
  }
#endif /* FEATURE_UIM_USB_UICC */

#ifdef FEATURE_UIM_BTSAP_CLIENT
  /* Register with the QMI UIM REMOTE client */
  uim_btsap_init(uim_ptr);
#endif /* FEATURE_UIM_BTSAP_CLIENT */

  /* Power up/reset the card */
  uim_start_initial_powerup(uim_ptr, &i_mask);

  uim_rcevt_signal_name(uim_ptr);

  for (;;)
  {
    /* Never exit this loop... */

    /* Perform the rex wait or check the q and set the signal mask */
    rex_signals_mask = get_rex_sigs_mask(uim_ptr, i_mask);

#ifdef FEATURE_UIM_TEST_FRAMEWORK
    #error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

    /* Handle the signals that are set */
    b_goto_top = uim_signal_handler(&rex_signals_mask, &i_mask, uim_ptr);
    if(TRUE == b_goto_top)
    {
      continue;
    }

    /*-------------------------
    ** Process the pending poll
    ** -----------------------*/
    if (uim_ptr->flag.poll_pending)
    {
      uim_process_pending_poll(uim_ptr, &i_mask);
    } /* uim_poll_pending */

    /*-------------------------
    ** Process the pending Fetch
    ** -----------------------*/
    if( uim_ptr->flag.fetch_pending)
    {
      uim_process_pending_fetch(uim_ptr);
    }/* uim_fetch_pending */

    /* Stop the clock if there are no commands progress or queued */
    uim_check_q_and_stop_clock(uim_ptr, rex_signals_mask);

    if(uim_ptr->flag.powering_down_task)
    {
      /* Power down and stop the uim task */
      b_task_powered_down = uim_task_power_down(uim_ptr, &i_mask
                            , uim_group, &stop_sig
                           ); /* call uim_task_power_down() */

      if(b_task_powered_down)
      {
#ifdef FEATURE_UIM_TEST_FRAMEWORK
        #error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */
      }
    }
  } /* end for (;;) */
#ifdef FEATURE_UIM_TEST_FRAMEWORK
  #error code not present
#endif /*FEATURE_UIM_TEST_FRAMEWORK*/
} /* end uim_task */


/*===========================================================================

FUNCTION UIM_3_TASK

DESCRIPTION
  This procedure is the entrance procedure for the UIM task.

DEPENDENCIES
  None

RETURN VALUE
  Does not return.

SIDE EFFECTS
  None

===========================================================================*/
void uim_3_task
(
  dword dummy
)
{
  uim_instance_global_type *uim_ptr = NULL;
  (void)dummy;
  uim_ptr = uim_get_instance_ptr(UIM_INSTANCE_3);
  uim_ptr->tcb_ptr = rex_self();
  uim_task_common((dword)uim_ptr);
} /* uim_3_task */


/*===========================================================================

FUNCTION UIM_2_TASK

DESCRIPTION
  This procedure is the entrance procedure for the UIM task.

DEPENDENCIES
  None

RETURN VALUE
  Does not return.

SIDE EFFECTS
  None

===========================================================================*/
void uim_2_task
(
  dword dummy
)
{
  uim_instance_global_type *uim_ptr = NULL;
  (void)dummy;
  uim_ptr = uim_get_instance_ptr(UIM_INSTANCE_2);
  uim_ptr->tcb_ptr = rex_self();
  uim_task_common((dword)uim_ptr);
} /* uim_2_task */


/*===========================================================================

FUNCTION UIM_TASK

DESCRIPTION
  This procedure is the entrance procedure for the UIM task.

DEPENDENCIES
  None

RETURN VALUE
  Does not return.

SIDE EFFECTS
  None

===========================================================================*/
void uim_task
(
  dword dummy
)
{
  uim_instance_global_type *uim_ptr = NULL;
  (void)dummy;
  uim_ptr = uim_get_instance_ptr(UIM_INSTANCE_1);
  uim_ptr->tcb_ptr = rex_self();
  uim_task_common((dword)uim_ptr);
} /* uim_task */


/*===========================================================================

FUNCTION UIM_COPY_CMD_DATA

DESCRIPTION
  This function will copy the data from the command ptr. It will do
  deep copy where necessary.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  = Copy operation successful
  FALSE = Copy operation failure

SIDE EFFECTS
  None

===========================================================================*/
static boolean uim_copy_cmd_data
(
  uim_cmd_type             *dest_cmd_ptr,
  uint32                    dest_cmd_size,
  const uim_cmd_type       *src_cmd_ptr,
  uim_instance_global_type *uim_ptr
)
{
  if(NULL == src_cmd_ptr || NULL == dest_cmd_ptr ||
     uim_ptr->id >= UIM_MAX_INSTANCES || (dest_cmd_size < sizeof(uim_cmd_type)))
  {
    UIM_MSG_LOW_0("Input data is invalid");
    return FALSE;
  }

  if(dest_cmd_size < sizeof(uim_cmd_type))
  {
    UIMDRV_MSG_ERR_1(uim_ptr->id,
                     "Destination Buffer Too Small Size = 0x%x", dest_cmd_size);
    return FALSE;
  }
  /* Default case is avoided on purpose here. We want to account for all enum
     values. If there is a missing enum then it will be caught through a
     compiler warning */
  switch(src_cmd_ptr->hdr.command)
  {
    case UIM_NO_SUCH_COMMAND_F:
    /* Memory De-coupling is not required for comamnd */
    case UIM_SILENT_RECOVER_UICC_F:
    case UIM_MAX_F:
      return FALSE;

    case UIM_INTERNAL_ME_PUP_F:
    case UIM_INTERNAL_WAKE_UP_F:
    case UIM_INTERNAL_POLL_F:
    case UIM_SELECT_F:
    case UIM_SEEK_F:
    case UIM_INVALIDATE_F:
    case UIM_REHABILITATE_F:
    case UIM_VERIFY_CHV_F:
    case UIM_CHANGE_CHV_F:
    case UIM_DISABLE_CHV_F:
    case UIM_ENABLE_CHV_F:
    case UIM_UNBLOCK_CHV_F:
    case UIM_STATUS_F:
    case UIM_TERMINAL_PROFILE_F:
    case UIM_ENVELOPE_F:
    case UIM_INTERNAL_FETCH_F:
    case UIM_TERMINAL_RESPONSE_F:
    case UIM_POLLING_INTERVAL_F:
    case UIM_RESET_F:
    case UIM_STREAM_APDU_F:
    case UIM_STREAM_ISO7816_APDU_F:
    case UIM_RESET_WITH_TP_F:
    case UIM_PREF_SLOT_CHANGE_F:
    case UIM_RESET_SWITCH_UIM_PASSIVE_F:
    case UIM_POWER_UP_UIM_PASSIVE_F:
    case UIM_POWER_DOWN_F:
    case UIM_READ_BIN_CMD_F:
    case UIM_READ_REC_CMD_F:
    case UIM_RECOVERY_COMPLETE_F:
    case UIM_HOTSWAP_CARD_INS_F:
    case UIM_HOTSWAP_CARD_REM_F:
    case UIM_RUN_GSM_ALGO_F:
    case UIM_STORE_ESN_ME_F:
    case UIM_SSD_UPDATE_F:
    case UIM_CONFIRM_SSD_F:
    case UIM_RUN_CAVE_F:
    case UIM_AKA_AUTH_F:
    case UIM_GENERATE_KEYS_F:
    case UIM_BS_CHAL_F:
    case UIM_MS_KEY_REQ_F:
    case UIM_KEY_GEN_REQ_F:
    case UIM_SERV_KEY_GEN_REQ_F:
    case UIM_COMMIT_F:
    case UIM_VALIDATE_F:
    case UIM_CONFIG_REQ_F:
    case UIM_DOWNLOAD_REQ_F:
    case UIM_SSPR_CONFIG_REQ_F:
    case UIM_SSPR_DOWNLOAD_REQ_F:
    case UIM_OTAPA_REQ_F:
    case UIM_COMPUTE_IP_AUTH_F:
    case UIM_STORE_ESN_MEID_ME_F:
    case UIM_AUTHENTICATE_F:
    case UIM_INC_CMD_F:
    case UIM_MANAGE_CHANNEL_F:
    case UIM_ISIM_AUTHENTICATE_F:
    case UIM_MFLO_INIT_ACT_PARAMS_F:
    case UIM_MFLO_GET_SUBSCRIBER_ID_F:
    case UIM_MFLO_GET_PUBLIC_KEY_F:
      uim_memscpy((void*)dest_cmd_ptr, dest_cmd_size, (void*)src_cmd_ptr,
                   sizeof(uim_cmd_type));
      return TRUE;

    case UIM_WRITE_BIN_CMD_F:
      uim_memscpy((void*)dest_cmd_ptr, dest_cmd_size, (void*)src_cmd_ptr,
                  sizeof(uim_cmd_type));

      dest_cmd_ptr->write_bin.data = (byte*)dest_cmd_ptr + sizeof(uim_cmd_type);

      if((dest_cmd_size - sizeof(uim_cmd_type)) >= src_cmd_ptr->write_bin.len)
      {
        uim_memscpy((void*)(dest_cmd_ptr->write_bin.data),
                    (dest_cmd_size - sizeof(uim_cmd_type)),
                    (void*)src_cmd_ptr->write_bin.data,
                    src_cmd_ptr->write_bin.len);
        return TRUE;
      }
      else
      {
        UIMDRV_MSG_ERR_2(uim_ptr->id,
                         "Not enough bytes in dest buf, Avail: 0x%x Needed: 0x%x",
                         (dest_cmd_size - sizeof(uim_cmd_type)), src_cmd_ptr->write_bin.len);
        return FALSE;
      }

     case UIM_WRITE_REC_CMD_F:
      uim_memscpy((void*)dest_cmd_ptr, dest_cmd_size, (void*)src_cmd_ptr,
                   sizeof(uim_cmd_type));

      dest_cmd_ptr->write_rec.data = (byte*)dest_cmd_ptr + sizeof(uim_cmd_type);

      if((dest_cmd_size - sizeof(uim_cmd_type)) >= src_cmd_ptr->write_rec.len)
      {
        uim_memscpy((void*)(dest_cmd_ptr->write_rec.data),
                    (dest_cmd_size - sizeof(uim_cmd_type)),
                    (void*)src_cmd_ptr->write_rec.data,
                    src_cmd_ptr->write_rec.len);
        return TRUE;
      }
      else
      {
        UIMDRV_MSG_ERR_2(uim_ptr->id,
                         "Not enough bytes in dest buf, Avail: 0x%x Needed: 0x%x",
                         (dest_cmd_size - sizeof(uim_cmd_type)), src_cmd_ptr->write_rec.len);
        return FALSE;
      }

    case UIM_CLEAR_CACHE_F:
      uim_memscpy((void*)dest_cmd_ptr, dest_cmd_size, (void*)src_cmd_ptr,
                  sizeof(uim_cmd_type));

      dest_cmd_ptr->clear_cache.item_list =
        (uim_items_enum_type*)((byte*)dest_cmd_ptr + sizeof(uim_cmd_type));

      if( ( dest_cmd_size - sizeof(uim_cmd_type) ) >=
          ( src_cmd_ptr->clear_cache.num_items * sizeof(uim_items_enum_type) ) )
      {
        uim_memscpy((void*)dest_cmd_ptr->clear_cache.item_list,
                    (dest_cmd_size - sizeof(uim_cmd_type)),
                    (void*)src_cmd_ptr->clear_cache.item_list,
                    (src_cmd_ptr->clear_cache.num_items * sizeof(uim_items_enum_type)));
        return TRUE;
      }
      else
      {
        UIMDRV_MSG_ERR_2(uim_ptr->id,
                         "Not enough bytes in dest buf, Avail: 0x%x Needed: 0x%x",
                         (dest_cmd_size - sizeof(uim_cmd_type)),
                         (src_cmd_ptr->clear_cache.num_items * sizeof(uim_items_enum_type)));
        return FALSE;
      }

    case UIM_AUTHENTICATE_ODD_INS_F:
      uim_memscpy((void*)dest_cmd_ptr, dest_cmd_size, (void*)src_cmd_ptr,
                   sizeof(uim_cmd_type));

      dest_cmd_ptr->autn_odd_ins.data = (byte*)dest_cmd_ptr + sizeof(uim_cmd_type);

      if((dest_cmd_size - sizeof(uim_cmd_type)) >= src_cmd_ptr->autn_odd_ins.len)
      {
        uim_memscpy((void*)dest_cmd_ptr->autn_odd_ins.data,
                    (dest_cmd_size - sizeof(uim_cmd_type)),
                    (void*)src_cmd_ptr->autn_odd_ins.data,
                    src_cmd_ptr->autn_odd_ins.len);
        return TRUE;
      }
      else
      {
        UIMDRV_MSG_ERR_2(uim_ptr->id,
                         "Not enough bytes in dest buf, Avail: 0x%x Needed: 0x%x",
                         (dest_cmd_size - sizeof(uim_cmd_type)), src_cmd_ptr->autn_odd_ins.len);
        return FALSE;
      }

    case UIM_BCAST_F:
      uim_memscpy((void*)dest_cmd_ptr, dest_cmd_size, (void*)src_cmd_ptr,
                  sizeof(uim_cmd_type));

      dest_cmd_ptr->bcast.data = (byte*)dest_cmd_ptr +  sizeof(uim_cmd_type);

      if((dest_cmd_size - sizeof(uim_cmd_type)) >= src_cmd_ptr->bcast.len)
      {
        uim_memscpy((void*)dest_cmd_ptr->bcast.data,
                    (dest_cmd_size - sizeof(uim_cmd_type)),
                    (void*)src_cmd_ptr->bcast.data,
                    src_cmd_ptr->bcast.len);
        return TRUE;
      }
      else
      {
        UIMDRV_MSG_ERR_2(uim_ptr->id,
                         "Not enough bytes in dest buf, Avail: 0x%x Needed: 0x%x",
                         (dest_cmd_size - sizeof(uim_cmd_type)), src_cmd_ptr->bcast.len);
        return FALSE;
      }

    case UIM_MFLO_SIGN_UCAST_F:
      uim_memscpy((void*)dest_cmd_ptr, dest_cmd_size, (void*)src_cmd_ptr,
                  sizeof(uim_cmd_type));

      dest_cmd_ptr->mflo_sign_ucast.ucast_msg_ptr =
        (byte*)dest_cmd_ptr + sizeof(uim_cmd_type);

      if((dest_cmd_size - sizeof(uim_cmd_type)) >=
         src_cmd_ptr->mflo_sign_ucast.ucast_msg_len)
      {
        uim_memscpy((void*)dest_cmd_ptr->mflo_sign_ucast.ucast_msg_ptr,
                    (dest_cmd_size - sizeof(uim_cmd_type)),
                    (void*)src_cmd_ptr->mflo_sign_ucast.ucast_msg_ptr,
                    src_cmd_ptr->mflo_sign_ucast.ucast_msg_len);
        return TRUE;
      }
      else
      {
        UIMDRV_MSG_ERR_2(uim_ptr->id,
                         "Not enough bytes in dest buf, Avail: 0x%x Needed: 0x%x",
                         (dest_cmd_size - sizeof(uim_cmd_type)),
                         src_cmd_ptr->mflo_sign_ucast.ucast_msg_len);
        return FALSE;
      }

    case UIM_MFLO_VERIFY_UCAST_F:
      uim_memscpy((void*)dest_cmd_ptr, dest_cmd_size, (void*)src_cmd_ptr,
                   sizeof(uim_cmd_type));

      dest_cmd_ptr->mflo_verify_ucast.ucast_msg_ptr =
        (byte*)dest_cmd_ptr + sizeof(uim_cmd_type);

      if((dest_cmd_size - sizeof(uim_cmd_type)) >=
         src_cmd_ptr->mflo_verify_ucast.ucast_msg_len)
      {
        uim_memscpy((void*)dest_cmd_ptr->mflo_verify_ucast.ucast_msg_ptr,
                    (dest_cmd_size - sizeof(uim_cmd_type)),
                    (void*)src_cmd_ptr->mflo_verify_ucast.ucast_msg_ptr,
                    src_cmd_ptr->mflo_verify_ucast.ucast_msg_len);
        return TRUE;
      }
      else
      {
        UIMDRV_MSG_ERR_2(uim_ptr->id,
                         "Not enough bytes in dest buf, Avail: 0x%x Needed: 0x%x",
                         (dest_cmd_size - sizeof(uim_cmd_type)),
                          src_cmd_ptr->mflo_verify_ucast.ucast_msg_len);
        return FALSE;
      }
    default:
      UIMDRV_MSG_ERR_1(uim_ptr->id,"Invalid command type enum 0x%x",
                       src_cmd_ptr->hdr.command);
      return FALSE;
  } /* switch */
  return FALSE;
} /* uim_copy_cmd_data */


/*===========================================================================

FUNCTION UIM_CACULATE_CMD_SIZE

DESCRIPTION
  This function calculated the amount of memory that needs to be allocated
  to the UIM CMD. It takes into consideration the dynamic allocations.

DEPENDENCIES
  None

RETURN VALUE
  uint32 size of the command.
  Return value will be zero in case of error.

SIDE EFFECTS
  None

===========================================================================*/
static uint32 uim_calculate_cmd_size
(
  const uim_cmd_type       *uim_cmd_ptr,
  uim_instance_global_type *uim_ptr
)
{
  uint32 cmd_size = 0;

  if(NULL == uim_cmd_ptr || (uim_cmd_ptr->hdr.command > UIM_MAX_F))
  {
    return cmd_size;
  }

  cmd_size = sizeof(uim_cmd_type);

  /* Default case is avoided on purpose here. We want to account for all enum
     values. If there is a missing enum then it will be caught through a
     compiler warning */
  switch(uim_cmd_ptr->hdr.command)
  {
    case UIM_NO_SUCH_COMMAND_F:
    /* Memory De-coupling is not required for this comamnd */
    case UIM_SILENT_RECOVER_UICC_F:
    case UIM_MAX_F:
      UIMDRV_MSG_ERR_1(uim_ptr->id,"Invalid command type enum 0x%x",
                       uim_cmd_ptr->hdr.command);
      return 0;
    case UIM_INTERNAL_ME_PUP_F:
    case UIM_INTERNAL_WAKE_UP_F:
    case UIM_INTERNAL_POLL_F:
    case UIM_SELECT_F:
    case UIM_SEEK_F:
    case UIM_INVALIDATE_F:
    case UIM_REHABILITATE_F:
    case UIM_VERIFY_CHV_F:
    case UIM_CHANGE_CHV_F:
    case UIM_DISABLE_CHV_F:
    case UIM_ENABLE_CHV_F:
    case UIM_UNBLOCK_CHV_F:
    case UIM_STATUS_F:
    case UIM_TERMINAL_PROFILE_F:
    case UIM_ENVELOPE_F:
    case UIM_INTERNAL_FETCH_F:
    case UIM_TERMINAL_RESPONSE_F:
    case UIM_POLLING_INTERVAL_F:
    case UIM_RESET_F:
    case UIM_STREAM_APDU_F:
    case UIM_STREAM_ISO7816_APDU_F:
    case UIM_RESET_WITH_TP_F:
    case UIM_PREF_SLOT_CHANGE_F:
    case UIM_RESET_SWITCH_UIM_PASSIVE_F:
    case UIM_POWER_UP_UIM_PASSIVE_F:
    case UIM_POWER_DOWN_F:
    case UIM_READ_BIN_CMD_F:
    case UIM_READ_REC_CMD_F:
    case UIM_RECOVERY_COMPLETE_F:
    case UIM_HOTSWAP_CARD_INS_F:
    case UIM_HOTSWAP_CARD_REM_F:
    case UIM_RUN_GSM_ALGO_F:
    case UIM_STORE_ESN_ME_F:
    case UIM_SSD_UPDATE_F:
    case UIM_CONFIRM_SSD_F:
    case UIM_RUN_CAVE_F:
    case UIM_AKA_AUTH_F:
    case UIM_GENERATE_KEYS_F:
    case UIM_BS_CHAL_F:
    case UIM_MS_KEY_REQ_F:
    case UIM_KEY_GEN_REQ_F:
    case UIM_SERV_KEY_GEN_REQ_F:
    case UIM_COMMIT_F:
    case UIM_VALIDATE_F:
    case UIM_CONFIG_REQ_F:
    case UIM_DOWNLOAD_REQ_F:
    case UIM_SSPR_CONFIG_REQ_F:
    case UIM_SSPR_DOWNLOAD_REQ_F:
    case UIM_OTAPA_REQ_F:
    case UIM_COMPUTE_IP_AUTH_F:
    case UIM_STORE_ESN_MEID_ME_F:
    case UIM_AUTHENTICATE_F:
    case UIM_INC_CMD_F:
    case UIM_MANAGE_CHANNEL_F:
    case UIM_ISIM_AUTHENTICATE_F:
    case UIM_MFLO_INIT_ACT_PARAMS_F:
    case UIM_MFLO_GET_SUBSCRIBER_ID_F:
    case UIM_MFLO_GET_PUBLIC_KEY_F:
      break;
    case UIM_WRITE_BIN_CMD_F:
      cmd_size += uim_cmd_ptr->write_bin.len;
      break;
    case UIM_WRITE_REC_CMD_F:
      cmd_size += uim_cmd_ptr->write_rec.len;
      break;
    case UIM_CLEAR_CACHE_F:
      cmd_size += (uim_cmd_ptr->clear_cache.num_items * sizeof(uim_items_enum_type));
      break;
    case UIM_AUTHENTICATE_ODD_INS_F:
      cmd_size += uim_cmd_ptr->autn_odd_ins.len;
      break;
    case UIM_BCAST_F:
      cmd_size += uim_cmd_ptr->bcast.len;
      break;
    case UIM_MFLO_SIGN_UCAST_F:
      cmd_size += uim_cmd_ptr->mflo_sign_ucast.ucast_msg_len;
      break;
    case UIM_MFLO_VERIFY_UCAST_F:
      cmd_size += uim_cmd_ptr->mflo_verify_ucast.ucast_msg_len;
      break;
   default:
     UIMDRV_MSG_ERR_1(uim_ptr->id,"Invalid command type enum 0x%x",uim_cmd_ptr->hdr.command);
  }
  return cmd_size;
} /* uim_calculate_cmd_size */


/*===========================================================================

FUNCTION UIM_REMOVE_SFI_FROM_CMD

DESCRIPTION
  This procedure removes the SFI parameter from a cmd from MMGSDI in case
  it is not needed or cannot be used

===========================================================================*/
void uim_remove_sfi_from_cmd(uim_cmd_type *cmd_ptr)
{
  switch(cmd_ptr->hdr.command)
  {
    case UIM_READ_BIN_CMD_F:
      cmd_ptr->read_bin.sfi = UIM_INVALID_SFI;
      break;

    case UIM_READ_REC_CMD_F:
      cmd_ptr->read_rec.sfi = UIM_INVALID_SFI;
       break;

    case UIM_WRITE_BIN_CMD_F:
      cmd_ptr->write_bin.sfi = UIM_INVALID_SFI;
      break;

    case UIM_WRITE_REC_CMD_F:
      cmd_ptr->write_rec.sfi = UIM_INVALID_SFI;
      break;

    case UIM_INC_CMD_F:
      cmd_ptr->inc.sfi = UIM_INVALID_SFI;
      break;

    case UIM_SEEK_F:
      cmd_ptr->seek.sfi = UIM_INVALID_SFI;
      break;

    default:
      UIM_MSG_ERR_0("CMD not recognized as SFI command");
      break;
  } /* switch command */
} /* uim_remove_sfi_from_cmd */


/*===========================================================================

FUNCTION UIM_CLEAR_CURRENT_PATH

DESCRIPTION
  Clears the current path.  Only the path is altered the AID being
  maintained for this channel is not altered.

===========================================================================*/
void uim_clear_current_path(uim_instance_global_type *uim_ptr, uim_channel_type curr_ch)
{
  memset(&uim_ptr->state.current_path[curr_ch].path, 0x00, sizeof(uim_path_type));
  uim_ptr->state.current_path[curr_ch].is_ef   = FALSE;
} /* uim_clear_current_path */


/*===========================================================================

FUNCTION UIM_GET_COMMAND_PATH_PTR

DESCRIPTION
  Each command in the uim_cmd_type union has it's own path member.  This
  function returns a pointer to that path member so that code common to
  paths - across commands - does not need to be duplicated.

RETURN VALUE
  A pointer to the path member in the command parameter.
  NULL if the command does not have a path member

===========================================================================*/
uim_path_type *uim_get_command_path_ptr
(
  uim_cmd_type *cmd_ptr
)
{
  uim_path_type *source_path_ptr = NULL;

  switch(cmd_ptr->hdr.command)
  {
    case UIM_SELECT_F:
      source_path_ptr = &cmd_ptr->select.path;
    break;

    case UIM_READ_BIN_CMD_F:
      source_path_ptr = &cmd_ptr->read_bin.path;
    break;

    case UIM_READ_REC_CMD_F:
      source_path_ptr = &cmd_ptr->read_rec.path;
      break;

    case UIM_WRITE_BIN_CMD_F:
      source_path_ptr = &cmd_ptr->write_bin.path;
      break;

    case UIM_WRITE_REC_CMD_F:
      source_path_ptr = &cmd_ptr->write_rec.path;
      break;

    case UIM_SEEK_F:
      source_path_ptr = &cmd_ptr->seek.path;
      break;

    case UIM_INC_CMD_F:
      source_path_ptr = &cmd_ptr->inc.path;
      break;

    case UIM_INVALIDATE_F:
      source_path_ptr = &cmd_ptr->invalidate.path;
      break;

    case UIM_REHABILITATE_F:
      source_path_ptr = &cmd_ptr->rehab.path;
      break;

    default:
      break;
  } /* switch command */

  return source_path_ptr;
} /* uim_get_command_path_ptr */


/*===========================================================================

FUNCTION UIM_GET_COMMAND_SFI_PTR

DESCRIPTION
  Each command in the uim_cmd_type union has it's own SFI member.  This
  function returns a pointer to that SFI member so that code common to
  SFI - across commands - does not need to be duplicated.

RETURN VALUE
  A pointer to the SFI member in the command parameter.
  NULL if the command does not have an SFI member

===========================================================================*/
uint8 *uim_get_command_sfi_ptr(uim_cmd_type *cmd_ptr)
{
  uint8 *sfi_ptr = NULL;

  switch(cmd_ptr->hdr.command)
  {
    case UIM_READ_BIN_CMD_F:
      sfi_ptr = &cmd_ptr->read_bin.sfi;
    break;

    case UIM_READ_REC_CMD_F:
      sfi_ptr = &cmd_ptr->read_rec.sfi;
      break;

    case UIM_WRITE_BIN_CMD_F:
      sfi_ptr = &cmd_ptr->write_bin.sfi;
      break;

    case UIM_WRITE_REC_CMD_F:
      sfi_ptr = &cmd_ptr->write_rec.sfi;
      break;

    case UIM_SEEK_F:
      sfi_ptr = &cmd_ptr->seek.sfi;

    default:
      break;
  }

  return sfi_ptr;
} /* uim_get_command_sfi_ptr */


/*===========================================================================

FUNCTION UIM_GET_COMMAND_AID_PTR

DESCRIPTION
  Each command in the uim_cmd_type union has it's own AID member.  This
  function returns a pointer to that AID member so that code common to
  AID - across commands - does not need to be duplicated.

RETURN VALUE
  A pointer to the AID member in the command parameter.
  NULL if the command does not have an AID member

===========================================================================*/
uim_aid_type *uim_get_command_aid_ptr(uim_cmd_type *cmd_ptr)
{
  uim_aid_type *aid_ptr = NULL;

  switch (cmd_ptr->hdr.command)
  {
    case UIM_SELECT_F:
      aid_ptr = &cmd_ptr->select.aid;
    break;

    case UIM_READ_BIN_CMD_F:
      aid_ptr = &cmd_ptr->read_bin.aid;
    break;

    case UIM_READ_REC_CMD_F:
      aid_ptr = &cmd_ptr->read_rec.aid;
      break;

    case UIM_WRITE_BIN_CMD_F:
      aid_ptr = &cmd_ptr->write_bin.aid;
      break;

    case UIM_WRITE_REC_CMD_F:
      aid_ptr = &cmd_ptr->write_rec.aid;
      break;

    case UIM_SEEK_F:
      aid_ptr = &cmd_ptr->seek.aid;
      break;

    case UIM_INC_CMD_F:
      aid_ptr = &cmd_ptr->inc.aid;
      break;

    case UIM_INVALIDATE_F:
      aid_ptr = &cmd_ptr->invalidate.aid;
      break;

    case UIM_REHABILITATE_F:
      aid_ptr = &cmd_ptr->rehab.aid;
      break;

    case UIM_VERIFY_CHV_F:
      aid_ptr = &cmd_ptr->verify_chv.aid;
      break;

    case UIM_CHANGE_CHV_F:
      aid_ptr = &cmd_ptr->change_chv.aid;
      break;

    case UIM_DISABLE_CHV_F:
      aid_ptr = &cmd_ptr->disable_chv.aid;
      break;

    case UIM_ENABLE_CHV_F:
      aid_ptr = &cmd_ptr->enable_chv.aid;
      break;

    case UIM_UNBLOCK_CHV_F:
      aid_ptr = &cmd_ptr->unblk_chv.aid;
      break;

    case UIM_AUTHENTICATE_F:
      aid_ptr = &cmd_ptr->autn.aid;
      break;

    case UIM_AUTHENTICATE_ODD_INS_F:
      aid_ptr = &cmd_ptr->autn_odd_ins.aid;
      break;

    case UIM_ISIM_AUTHENTICATE_F:
      aid_ptr = &cmd_ptr->isim_autn.aid;
      break;

    default:
      break;
  }

  return aid_ptr;
} /* uim_get_command_aid_ptr */


/*===========================================================================

TYPEDEF UIM_DIR_MAIN_CMD_TARGET

DESCRIPTION
  This typedef declares the function prototype used to determine whether an
  intermediate select is needed.  Each command that may need an intermediate
  select will have a function following this prototype in the directory
  maintenance command handling table

===========================================================================*/
typedef void (*uim_dir_main_cmd_target)(uim_instance_global_type     *uim_ptr,
                                        uint8                         curr_ch,
                                        uim_intermediate_select_type *intermediate_sel_ptr);


/*===========================================================================

TYPEDEF UIM_CURRENT_DIR_CMD_TABLE_TYPE

DESCRIPTION
  This type is used to map individual commands to the function which
  determines whether or not they need an intermediate select.

===========================================================================*/
typedef struct{
  uim_cmd_name_type cmd;
  uim_dir_main_cmd_target handler;
}uim_current_dir_cmd_table_type;


/*===========================================================================

FUNCTION UIM_CURRENT_DIR_EF_TARGET

DESCRIPTION
  This procedure determines if an intermediate select is needed for commands
  that operate specifically on an EF target.  It will also check if SFI can
  be used and appropriately remove the SFI param if it's not an appropriate
  time to execute an SFI command

===========================================================================*/
void uim_current_dir_ef_target
(
  uim_instance_global_type     *uim_ptr,
  uint8                         curr_ch,
  uim_intermediate_select_type *intermediate_sel_ptr
)
{
  uim_path_type *cmd_path_ptr     = NULL;
  uint8         *cmd_sfi_ptr      = NULL;

  if(uim_ptr->command.cmd_ptr->hdr.command == UIM_SEEK_F)
  {
    /* This is to support few non-spec compliant cards */
    UIMDRV_MSG_HIGH_0(uim_ptr->id, "Removed SFI pointer from SEEK command");
    uim_remove_sfi_from_cmd(uim_ptr->command.cmd_ptr);
  }

  cmd_path_ptr = uim_get_command_path_ptr(uim_ptr->command.cmd_ptr);
  cmd_sfi_ptr  = uim_get_command_sfi_ptr(uim_ptr->command.cmd_ptr);
  if (NULL == cmd_path_ptr)
  {
    return;
  }

  /* Is the target file already selected?
     Is SFI available?
       Are we in the correct directory for SFI?
     Select the target file ?                 */
  if(cmd_path_ptr->len == uim_ptr->state.current_path[curr_ch].path.len &&
     0 == memcmp(cmd_path_ptr->path, uim_ptr->state.current_path[curr_ch].path.path, cmd_path_ptr->len * 2))
  {
    /* Card current fp is pointing to the target. Remove SFI
       the command since we are pointing to the target EF */
    uim_remove_sfi_from_cmd(uim_ptr->command.cmd_ptr);
    intermediate_sel_ptr->is_needed = FALSE;
    return;
  }

  /* If there is an SFI included in the command see if we're in the
     correct directory to use it.  If so we don't need an intermediate
     select so we pass the command on with the sfi intact */
  if(cmd_sfi_ptr != NULL && UIM_INVALID_SFI != *cmd_sfi_ptr)
  {
    if(uim_ptr->state.current_path[curr_ch].is_ef == TRUE)
    {
      if(cmd_path_ptr->len == uim_ptr->state.current_path[curr_ch].path.len &&
         0 == memcmp(cmd_path_ptr->path, uim_ptr->state.current_path[curr_ch].path.path, (cmd_path_ptr->len - 1) * 2))
      {
        intermediate_sel_ptr->is_needed = FALSE;
        return;
      }
      else
      {
        uim_remove_sfi_from_cmd(uim_ptr->command.cmd_ptr);
      }
    }
    else /* the currently selected fp on the card is pointing to df/adf */
    {
      if((cmd_path_ptr->len - 1) == uim_ptr->state.current_path[curr_ch].path.len &&
          0 == memcmp(cmd_path_ptr->path, uim_ptr->state.current_path[curr_ch].path.path, (cmd_path_ptr->len - 1) * 2))
      {
        intermediate_sel_ptr->is_needed = FALSE;
        return;
      }
      else
      {
        uim_remove_sfi_from_cmd(uim_ptr->command.cmd_ptr);
      }
    }
  } /* check if SFI can be used */

  /* Support of buggy RUIM Card which need explicit CDMA Selection,
     before any telecomm access */

  if ( cmd_path_ptr->path[0] == UIM_MF_ID      &&
       cmd_path_ptr->path[1] == UIM_TELECOM_DF &&
       uim_ptr->command.command_requeue_counter == 0 &&
       uim_check_explicit_select_df_cdma_before_telecom_access(uim_ptr))
  {
    intermediate_sel_ptr->is_needed    = TRUE;
    intermediate_sel_ptr->path.path[0] = UIM_MF_ID;
    intermediate_sel_ptr->path.path[1] = UIM_CDMA_DF;
    intermediate_sel_ptr->path.len     = 2;
    return;
  }

  intermediate_sel_ptr->path = *cmd_path_ptr;

  intermediate_sel_ptr->is_needed = TRUE;
  return;
} /* uim_current_dir_ef_target */


/*===========================================================================

FUNCTION UIM_CURRENT_DIR_GSM_ALGO

DESCRIPTION
  This procedure determines if an intermediate select is needed for GSM
  authentication algorithm

===========================================================================*/
void uim_current_dir_gsm_algo
(
  uim_instance_global_type     *uim_ptr,
  uint8                         curr_ch,
  uim_intermediate_select_type *intermediate_sel_ptr
)
{
  if(uim_ptr->state.current_path[curr_ch].path.path[0] != UIM_MF_ID  ||
     uim_ptr->state.current_path[curr_ch].path.path[1] != UIM_GSM_DF ||
     uim_ptr->state.current_path[curr_ch].path.len != 2)
  {
    memset(&intermediate_sel_ptr->path, 0x00, sizeof(uim_path_type));
    intermediate_sel_ptr->is_needed    = TRUE;
    intermediate_sel_ptr->path.path[0] = UIM_MF_ID;
    intermediate_sel_ptr->path.path[1] = UIM_GSM_DF;
    intermediate_sel_ptr->path.len     = 2;
  }

} /* uim_current_dir_gsm_algo */


/*===========================================================================

FUNCTION UIM_CURRENT_DIR_CHV_CMD

DESCRIPTION
  This procedure determines if an intermediate select is needed CHV commands

===========================================================================*/
void uim_current_dir_chv_cmd
(
  uim_instance_global_type     *uim_ptr,
  uint8                         curr_ch,
  uim_intermediate_select_type *intermediate_sel_ptr
)
{
  uim_aid_type *cmd_aid_ptr  = uim_get_command_aid_ptr(uim_ptr->command.cmd_ptr);
  if (NULL == cmd_aid_ptr)
  {
    return;
  }

  if (uim_ptr->command.cmd_ptr->hdr.protocol != UIM_UICC)
  {
    intermediate_sel_ptr->is_needed = FALSE;
    return;
  }

  /* For PIN we may need to select the ADF prior to issuing
     the command.  If the ADF is already current on the channel select
     by using 7FFF otherwise select using full AID. Only attempt the select once per cmd */
  if(uim_ptr->command.command_requeue_counter == 0x00)
  {
    /* if there is not an active aid on this channel or if the aid selected is not
       the one associated with the chv command we need to select the aid in the cmd */
    if(uim_ptr->state.current_path[curr_ch].curr_app.aid_length != cmd_aid_ptr->aid_length ||
        (memcmp(uim_ptr->state.current_path[curr_ch].curr_app.data,
              cmd_aid_ptr->data,
              cmd_aid_ptr->aid_length) != 0))
    {
      memset(&intermediate_sel_ptr->path, 0x00, sizeof(uim_path_type));
      intermediate_sel_ptr->is_needed    = TRUE;
      intermediate_sel_ptr->aid          = *cmd_aid_ptr;
      intermediate_sel_ptr->path.path[0] = UIM_MF_ID;
      intermediate_sel_ptr->path.path[1] = UIM_SEL_AID_BY_FILE_ID;
      intermediate_sel_ptr->path.len     = 2;
    }
    else if(uim_ptr->state.current_path[curr_ch].path.path[0] != UIM_MF_ID  ||
            uim_ptr->state.current_path[curr_ch].path.path[1] != UIM_SEL_AID_BY_FILE_ID)
    {
      intermediate_sel_ptr->is_needed    = TRUE;
      intermediate_sel_ptr->path.path[0] = 0x3F00;
      intermediate_sel_ptr->path.path[1] = 0x7FFF;
      intermediate_sel_ptr->path.len     = 2;
    }
  }
  else
  {
    intermediate_sel_ptr->is_needed = FALSE;
  }
} /* uim_current_dir_chv_cmd */


/*===========================================================================

FUNCTION UIM_CURRENT_DIR_CDMA_CMD

DESCRIPTION
  This procedure determines if an intermediate select is needed for all
  CDMA commands

===========================================================================*/
void uim_current_dir_cdma_cmd
(
  uim_instance_global_type     *uim_ptr,
  uint8                         curr_ch,
  uim_intermediate_select_type *intermediate_sel_ptr
)
{
  /* if the CDMA DF is not active on the channel then we need to select it */
  if(uim_ptr->command.cmd_ptr->hdr.protocol == UIM_ICC)
  {
    if(uim_ptr->state.current_path[curr_ch].path.path[0] != UIM_MF_ID ||
       uim_ptr->state.current_path[curr_ch].path.path[1] != UIM_CDMA_DF)
    {
      intermediate_sel_ptr->is_needed    = TRUE;
      intermediate_sel_ptr->path.path[0] = UIM_MF_ID;
      intermediate_sel_ptr->path.path[1] = UIM_CDMA_DF;
      intermediate_sel_ptr->path.len     = 2;
    }
  }
  else /* finally some legacy code that makes sense .. if the adf is not the only thing selected select it */
  {
    if(uim_ptr->state.current_path[curr_ch].path.path[0] != UIM_MF_ID ||
       uim_ptr->state.current_path[curr_ch].path.path[1] != UIM_SEL_AID_BY_FILE_ID ||
       (UIM_COMPUTE_IP_AUTH_F == uim_ptr->command.cmd_ptr->hdr.command && 
        uim_ptr->state.current_path[curr_ch].path.len != 2))
    {
      intermediate_sel_ptr->is_needed    = TRUE;
      intermediate_sel_ptr->path.path[0] = UIM_MF_ID;
      intermediate_sel_ptr->path.path[1] = UIM_SEL_AID_BY_FILE_ID;
      intermediate_sel_ptr->path.len     = 2;
    }
  }

} /* uim_current_dir_cdma_cmd */

/*===========================================================================

FUNCTION UIM_CURRENT_DIR_TP_CMD

DESCRIPTION
  This procedure determines if an intermediate select is needed for
  terminal profile command

===========================================================================*/
void uim_current_dir_tp_cmd
(
  uim_instance_global_type     *uim_ptr,
  uint8                         curr_ch,
  uim_intermediate_select_type *intermediate_sel_ptr
)
{

  if (uim_ptr->command.cmd_ptr->hdr.protocol != UIM_ICC)
  {
    return;
  }
  if (uim_ptr->command.cmd_ptr->terminal_profile.df == UIM_DF_GSM_TYPE)
  {
     if(!(((uim_ptr->state.current_path[0].path.path[0] == UIM_MF_ID) &&
         (uim_ptr->state.current_path[0].path.len == 1))
         ||
         ((uim_ptr->state.current_path[0].path.path[1] == UIM_GSM_DF) &&
         (uim_ptr->state.current_path[0].path.len == 2))
        ))
    {
      intermediate_sel_ptr->is_needed    = TRUE;
      intermediate_sel_ptr->path.path[0] = UIM_MF_ID;
      intermediate_sel_ptr->path.len++;
      /* Select CDMA DF only if it was present while initial power-up
         else select MF alone */
      if ( uim_ptr->state.dfs_1.gsm_df_present == TRUE )
      {
        intermediate_sel_ptr->path.path[1] = UIM_GSM_DF;
        intermediate_sel_ptr->path.len++;
      }
    }
  }
  else if (uim_ptr->command.cmd_ptr->terminal_profile.df == UIM_DF_CDMA_TYPE
           || uim_check_cdma_df_select_before_tp())
  {
    /* Select CDMA DF before terminal profile based on mmgsdi NV */
    /* If the current path is MF or MF.CDMADF then ignore re-selection */
    /* Current path can be MF here only when intermediate select happened for TP once + cdma df was absent */
    if(!(((uim_ptr->state.current_path[0].path.path[0] == UIM_MF_ID) &&
         (uim_ptr->state.current_path[0].path.len == 1))
         ||
         ((uim_ptr->state.current_path[0].path.path[1] == UIM_CDMA_DF) &&
         (uim_ptr->state.current_path[0].path.len == 2))
        ))
    {
      intermediate_sel_ptr->is_needed    = TRUE;
      intermediate_sel_ptr->path.path[0] = UIM_MF_ID;
      intermediate_sel_ptr->path.len++;
      /* Select CDMA DF only if it was present while initial power-up
         else select MF alone */
      if ( uim_ptr->state.dfs_1.cdma_df_present == TRUE )
      {
        intermediate_sel_ptr->path.path[1] = UIM_CDMA_DF;
        intermediate_sel_ptr->path.len++;
      }
    }
  }/* uim_check_cdma_df_select_before_tp() */
} /* uim_current_dir_tp_cmd */

/*===========================================================================

FUNCTION UIM_CURRENT_DIR_STATUS_CMD

DESCRIPTION
  This procedure determines if an intermediate select is needed for
  status command

===========================================================================*/
void uim_current_dir_status_cmd
(
  uim_instance_global_type     *uim_ptr,
  uint8                         curr_ch,
  uim_intermediate_select_type *intermediate_sel_ptr
)
{
  intermediate_sel_ptr->is_needed = FALSE;
  if (uim_ptr->command.cmd_ptr->hdr.protocol != UIM_UICC)
  {
    return;
  }

  if((uim_ptr->state.current_path[curr_ch].curr_app.aid_length  != 0) && 
      ((uim_ptr->command.cmd_ptr->status.status == UIM_STATUS_P1_TERMN_OF_CURR_APP) ||
        (uim_ptr->command.cmd_ptr->status.status == UIM_STATUS_P1_CURR_APP_INIT)))
  {
    if(uim_ptr->command.command_requeue_counter == 0x00)
    {
      if(uim_ptr->state.current_path[curr_ch].path.path[0] != UIM_MF_ID  ||
          uim_ptr->state.current_path[curr_ch].path.path[1] != UIM_SEL_AID_BY_FILE_ID)
      {
        intermediate_sel_ptr->is_needed    = TRUE;
        intermediate_sel_ptr->path.path[0] = 0x3F00;
        intermediate_sel_ptr->path.path[1] = 0x7FFF;
        intermediate_sel_ptr->path.len     = 2;
      }
    }
  }
} /* uim_current_dir_status_cmd */


/*===========================================================================

FUNCTION UIM_CURRENT_DIR_SEL_CMD

DESCRIPTION
  For explicit selects this function prints the desired select path

===========================================================================*/
void uim_current_dir_sel_cmd
(
  uim_instance_global_type     *uim_ptr,
  uint8                         curr_ch,
  uim_intermediate_select_type *intermediate_sel_ptr
)
{
  UIMDRV_MSG_HIGH_0(uim_ptr->id, "Explicit SELECT command for path:");
  uim_print_curr_path_ext(&uim_ptr->command.cmd_ptr->select.path,uim_ptr);  
  if ( uim_ptr->command.cmd_ptr->select.path.path[0] == UIM_MF_ID      &&
       uim_ptr->command.cmd_ptr->select.path.path[1] == UIM_TELECOM_DF &&
       uim_ptr->command.command_requeue_counter      == 0              &&
       uim_check_explicit_select_df_cdma_before_telecom_access(uim_ptr))
  {
    intermediate_sel_ptr->is_needed    = TRUE;
    intermediate_sel_ptr->path.path[0] = UIM_MF_ID;
    intermediate_sel_ptr->path.path[1] = UIM_CDMA_DF;
    intermediate_sel_ptr->path.len     = 2;
  }
} /* uim_current_dir_sel_cmd */


/*===========================================================================

FUNCTION UIM_CURRENT_DIR_AUTH

DESCRIPTION
  This procedure determines if an intermediate
  select is needed for AUTH commands

===========================================================================*/
void uim_current_dir_auth
(
  uim_instance_global_type     *uim_ptr,
  uint8                         curr_ch,
  uim_intermediate_select_type *intermediate_sel_ptr
)
{
  uim_aid_type *aid_ptr = uim_get_command_aid_ptr(uim_ptr->command.cmd_ptr);

  if (NULL == aid_ptr)
  {
    return;
  }

  /* For authenticate we always want to select the ADF prior to issuing
     the command.  If the ADF is already current on the channel select
     by using 7FFF otherwise select using full AID. Only attempt the select once per cmd */
  if(uim_ptr->command.command_requeue_counter == 0x00)
  {
    if(aid_ptr->aid_length != uim_ptr->state.current_path[curr_ch].curr_app.aid_length ||
       memcmp(aid_ptr->data,
              uim_ptr->state.current_path[curr_ch].curr_app.data,
              aid_ptr->aid_length) != 0)
    {
      intermediate_sel_ptr->is_needed    = TRUE;
      intermediate_sel_ptr->path.path[0] = 0x3F00;
      intermediate_sel_ptr->path.path[1] = 0x7FFF;
      intermediate_sel_ptr->path.len     = 2;
      intermediate_sel_ptr->aid.aid_length = aid_ptr->aid_length;
      uim_memscpy(intermediate_sel_ptr->aid.data,
                  sizeof(intermediate_sel_ptr->aid.data),
                  aid_ptr->data,
                  aid_ptr->aid_length);
    }
    else
    {
      intermediate_sel_ptr->is_needed    = TRUE;
      intermediate_sel_ptr->path.path[0] = 0x3F00;
      intermediate_sel_ptr->path.path[1] = 0x7FFF;
      intermediate_sel_ptr->path.len     = 2;
    }
    uim_ptr->flag.uim_do_not_flush_ext_cmd  = TRUE;
  }
  else
  {
    /* if we have already attempted selecting for this command then
       we should not attempt again - regardless of the previous result */
    intermediate_sel_ptr->is_needed = FALSE;
    uim_ptr->flag.uim_do_not_flush_ext_cmd  = FALSE;
  }

} /* uim_current_dir_auth */


/*===========================================================================

FUNCTION UIM_CURRENT_DIR_EF_TARGET

DESCRIPTION
  When streaming APDUs on the default channel clients expect that the state of
  the card has not changed since their last APDU.  Since channel one is not
  exclusive but there are clients that use it to stream APDUs a stream command
  can be interleaved with other commands that could disrupt the streaming client's
  assumed state of the card.  For this reason we maintain ATCSIM last selected dir
  and will restore the card's current directory to ATCSIM when a 7816_STREAM_F cmd
  is received.

  Intermediate selects need to be performed when the cmd has a path different from
  the current ATCSIM directory and the cmd is not a select by DF_NAME or by path.
  DF_NAME and path selects are absolute and should be sent to the card directly.

===========================================================================*/
void uim_current_dir_stream
(
  uim_instance_global_type     *uim_ptr,
  uint8                         curr_ch,
  uim_intermediate_select_type *intermediate_sel_ptr
)
{
  byte class_byte = uim_ptr->command.cmd_ptr->stream_iso7816_apdu.cmd_data[UIM_7816_APDU_CLASS_OFFSET];

  if ( uim_ptr->command.mode == UIM_PASSIVE_MODE )
  {
    return;
  }
  /* Check if the command is streamed in default channel */
  if(!(class_byte & (UIM_LOGICAL_CHANNEL_MASK | UIM_UICC_EXTENDED_LC_CLASS) ))
  {
    if(uim_ptr->card_cmd.cur_channel != UIM_DEFAULT_CHANNEL )
    {
      UIMDRV_MSG_ERR_1(uim_ptr->id, "Stream on default with hdr->channel != 0 0x%x",
                       uim_ptr->card_cmd.cur_channel);
      /* Set the cur channel to the default channel just in case */
      uim_ptr->card_cmd.cur_channel = UIM_DEFAULT_CHANNEL;
    }
  }
} /* uim_current_dir_stream */


/*===========================================================================

UIM_CURRENT_DIR_CMD_TABLE

DESCRIPTION
  This table maps individual commands to the function that determines
  whether or not an intermediate select is needed.  Commands not included
  do not have any need for intermediate selects

===========================================================================*/
uim_current_dir_cmd_table_type uim_current_dir_cmd_table[] =
{
  {UIM_READ_BIN_CMD_F,      uim_current_dir_ef_target},
  {UIM_READ_REC_CMD_F,      uim_current_dir_ef_target},
  {UIM_WRITE_BIN_CMD_F,     uim_current_dir_ef_target},
  {UIM_WRITE_REC_CMD_F,     uim_current_dir_ef_target},
  {UIM_INC_CMD_F,           uim_current_dir_ef_target},
  {UIM_SEEK_F,              uim_current_dir_ef_target},
  {UIM_INVALIDATE_F,        uim_current_dir_ef_target},
  {UIM_REHABILITATE_F,      uim_current_dir_ef_target},

  {UIM_RUN_GSM_ALGO_F,      uim_current_dir_gsm_algo},

  {UIM_AUTHENTICATE_F,         uim_current_dir_auth},
  {UIM_AUTHENTICATE_ODD_INS_F, uim_current_dir_auth},
  {UIM_ISIM_AUTHENTICATE_F,    uim_current_dir_auth},

  {UIM_VERIFY_CHV_F,        uim_current_dir_chv_cmd},
  {UIM_CHANGE_CHV_F,        uim_current_dir_chv_cmd},
  {UIM_DISABLE_CHV_F,       uim_current_dir_chv_cmd},
  {UIM_ENABLE_CHV_F,        uim_current_dir_chv_cmd},
  {UIM_UNBLOCK_CHV_F,       uim_current_dir_chv_cmd},

  {UIM_STORE_ESN_ME_F,      uim_current_dir_cdma_cmd},
  {UIM_STORE_ESN_MEID_ME_F, uim_current_dir_cdma_cmd},
  {UIM_SSD_UPDATE_F,        uim_current_dir_cdma_cmd},
  {UIM_CONFIRM_SSD_F,       uim_current_dir_cdma_cmd},
  {UIM_RUN_CAVE_F,          uim_current_dir_cdma_cmd},
  {UIM_AKA_AUTH_F,          uim_current_dir_cdma_cmd},
  {UIM_GENERATE_KEYS_F,     uim_current_dir_cdma_cmd},
  {UIM_BS_CHAL_F,           uim_current_dir_cdma_cmd},
  {UIM_MS_KEY_REQ_F,        uim_current_dir_cdma_cmd},
  {UIM_KEY_GEN_REQ_F,       uim_current_dir_cdma_cmd},
  {UIM_SERV_KEY_GEN_REQ_F,  uim_current_dir_cdma_cmd},
  {UIM_COMMIT_F,            uim_current_dir_cdma_cmd},
  {UIM_VALIDATE_F,          uim_current_dir_cdma_cmd},
  {UIM_CONFIG_REQ_F,        uim_current_dir_cdma_cmd},
  {UIM_DOWNLOAD_REQ_F,      uim_current_dir_cdma_cmd},
  {UIM_SSPR_CONFIG_REQ_F,   uim_current_dir_cdma_cmd},
  {UIM_SSPR_DOWNLOAD_REQ_F, uim_current_dir_cdma_cmd},
  {UIM_OTAPA_REQ_F,         uim_current_dir_cdma_cmd},
  {UIM_COMPUTE_IP_AUTH_F,   uim_current_dir_cdma_cmd},
  {UIM_SELECT_F,            uim_current_dir_sel_cmd},

  {UIM_STREAM_ISO7816_APDU_F, uim_current_dir_stream},
  {UIM_TERMINAL_PROFILE_F,    uim_current_dir_tp_cmd},
  {UIM_STATUS_F,              uim_current_dir_status_cmd}
};


/*===========================================================================

FUNCTION UIM_NEED_INTERMEDIATE_SEL

DESCRIPTION
  For each command call the appropriate function to determine if we need
  an intermediate select before performing the command. The
  intermediate_sel_ptr parameter is populated appropriately reflecting
  if an intermediate select is_needed and the path/aid that needs to select

===========================================================================*/
boolean uim_need_intermediate_sel(uim_instance_global_type     *uim_ptr,
                                  uim_channel_type              curr_ch,
                                  uim_intermediate_select_type *intermediate_sel_ptr)
{
  int i = 0;

  for(i = 0; i < (sizeof(uim_current_dir_cmd_table)/
                  sizeof(uim_current_dir_cmd_table_type)); i++)
  {
    if(uim_current_dir_cmd_table[i].cmd == uim_ptr->command.cmd_ptr->hdr.command)
    {
      uim_current_dir_cmd_table[i].handler(uim_ptr, curr_ch, intermediate_sel_ptr);
      break;
    }
  }

  if(intermediate_sel_ptr->is_needed)
  {
    uim_print_curr_path(&intermediate_sel_ptr->path,uim_ptr);
    if(intermediate_sel_ptr->aid.aid_length > 0)
    {
      UIMDRV_MSG_HIGH_1(uim_ptr->id, "AID is present: aid_length:0x%d",
                       intermediate_sel_ptr->aid.aid_length);
    }
  }
  else
  {
    uim_print_curr_path_ext(&uim_ptr->state.current_path[curr_ch].path, uim_ptr);
  }
  return intermediate_sel_ptr->is_needed;

} /* uim_need_intermediate_sel */


/*===========================================================================

FUNCTION UIM_UPDATE_CURRENT_DIR_UICC

DESCRIPTION
  Update the current directory after a UICC select command

===========================================================================*/
void uim_update_current_dir_uicc
(
  uim_instance_global_type *uim_ptr,
  boolean                   is_ef,
  uim_select_cmd_type      *sel_cmd_ptr,
  uim_channel_type          curr_ch
)
{
  UIMDRV_MSG_HIGH_1(uim_ptr->id, "Updating the current directory for UICC on channel 0x%x. Path before:", curr_ch);
  uim_print_curr_path_ext(&uim_ptr->state.current_path[curr_ch].path, uim_ptr);

  uim_ptr->state.current_path[curr_ch].path  = sel_cmd_ptr->path;
  uim_ptr->state.current_path[curr_ch].is_ef = is_ef;

  UIMDRV_MSG_HIGH_0(uim_ptr->id, "Path after:");
  uim_print_curr_path(&uim_ptr->state.current_path[curr_ch].path, uim_ptr);

  if(UIM_ADF == uim_ptr->command.cmd_ptr->select.file_type)
  {
    if(uim_ptr->command.cmd_ptr->select.session == UIM_TERMINATION)
    {
      /* Clear the current path structure - intentionally overwriting the path we set above
         This returns each state(is_ef, aid, path and empty app) to its default/empty state */
      uim_clear_current_path(uim_ptr, curr_ch);
      if(0 == curr_ch)
      {
        /* This is to make sure MF is selected on channel0 
           after the previous path is cleared */        
        uim_ptr->state.mf_selection.required = TRUE; 
        uim_ptr->state.mf_selection.channel  = 0; 
      }
    }
    else
    {
      uim_ptr->state.current_path[curr_ch].curr_app = sel_cmd_ptr->aid;
      if(sel_cmd_ptr->aid.aid_length == 0)
      {
        uim_ptr->state.current_path[curr_ch].empty_aid_is_active = TRUE;
      }
    }
  }
} /* uim_update_current_dir_uicc */


/*===========================================================================

FUNCTION UIM_UPDATE_CURRENT_DIR_ICC

DESCRIPTION
  Update the current directory after an ICC select command

===========================================================================*/
void uim_update_current_dir_icc
(
  uim_instance_global_type *uim_ptr,
  boolean                   is_ef,
  uim_path_type            *path_ptr,
  uint8                     path_position
)
{
  int i = 0;

  uim_print_curr_path_ext(&uim_ptr->state.current_path[0].path,uim_ptr);

  memset(&uim_ptr->state.current_path[0].path, 0x00, sizeof(uim_path_type));

  uim_ptr->state.current_path[0].path.len = 0;
  for(i = 0; i < path_position; i++)
  {
    uim_ptr->state.current_path[0].path.path[i] = path_ptr->path[i];
    uim_ptr->state.current_path[0].path.len++;
  }
  uim_ptr->state.current_path[0].is_ef = is_ef;

  uim_print_curr_path_ext(&uim_ptr->state.current_path[0].path,uim_ptr);

} /* uim_update_current_dir_icc */


/*===========================================================================

FUNCTION UIM_UPDATE_CURRENT_DIR_SFI

DESCRIPTION
  Update the current directory after an SFI command

===========================================================================*/
void uim_update_current_dir_sfi
(
  uim_instance_global_type *uim_ptr,
  uim_path_type            *path_ptr,
  uim_channel_type          curr_ch
)
{
  uim_print_curr_path_ext(&uim_ptr->state.current_path[curr_ch].path,uim_ptr);

  uim_ptr->state.current_path[curr_ch].path  = *path_ptr;
  uim_ptr->state.current_path[curr_ch].is_ef = TRUE;

  uim_print_curr_path_ext(&uim_ptr->state.current_path[curr_ch].path,uim_ptr);
} /* uim_update_current_dir_sfi */


/*===========================================================================

FUNCTION UIM_CMD

DESCRIPTION
  The uim_cmd pointed to by the parameter passed in is queued up for UIM.

DEPENDENCIES
  None.

RETURN VALUE
  UIM_SUCCESS        :- Command queued to uim_cmd_q successfully.
  UIM_INVALID_PARAMS :- Invalid params sent by the client.
  UIM_MEM_PROBLEM    :- Error during malloc/memcpy operations.
  UIM_GENERIC_ERROR  :- Generic error

SIDE EFFECTS
  Sets the UIM_CMD_Q_SIG.
===========================================================================*/
uim_return_type uim_cmd
(
  uim_cmd_type *cmd_ptr      /* the command to queue up */
)
{
  uim_instance_global_type *uim_ptr           = NULL;
  uim_cmd_type             *uim_task_cmd_ptr  = NULL;
  uint32                    cmd_size          = 0;

  if ( (cmd_ptr == NULL) ||
       (cmd_ptr->hdr.channel >= UIM_MAX_CHANNELS) ||
       (NULL == cmd_ptr->hdr.rpt_function)
     )
  {
    UIM_MSG_LOW_0("Invalid parameter");
    return UIM_INVALID_PARAMS;
  }

  /* Always initialize the count to zero */
  cmd_ptr->hdr.cmd_count = 0;

  #ifdef FEATURE_UIM_DS_SUBSCRIPTION_MANAGER
  /* if necessary perform Card Manager manipulation on the slot info */
  uim_sm_translate_cmd_slot(cmd_ptr);

  /*Set the req_type to NONE for external command as
     behavior won't be changed */
  cmd_ptr->hdr.req_type = UIM_NO_ACTION;
   /*Send down power-down or power-up command for inactive slot
    in case of airplane(AP) mode on/off respectively.
    This is to mask hotswap interrupt in case of AP on
    and send latest card information for inactive slot
    during AP off
    Also, the newly created commands are queued first,
    as they are sent on inactive slot
  */
   if(UIM_RESET_F == cmd_ptr->hdr.command ||
      UIM_POWER_DOWN_F == cmd_ptr->hdr.command
     )
   {
     uim_instance_enum_type temp_instance = UIM_INSTANCE_1;
     if(UIM_POWER_DOWN_F == cmd_ptr->hdr.command)
     {
       uim_sm_data.sm_ap_mode_on = TRUE;
     }
     else
     {
       uim_sm_data.sm_ap_mode_on = FALSE;
     }
     UIM_MSG_HIGH_1("sub_mgr setting sm_ap_mode_on to 0x%x",
                     uim_sm_data.sm_ap_mode_on);

     rex_enter_crit_sect(&uim_sm_data.uim_sm_data_crit_sect);
     /*Send down RESET or POWER-DOWN command for all inactive slot as well*/
     for(temp_instance = UIM_INSTANCE_1; temp_instance < UIM_NUM_PHY_SLOTS; temp_instance++)
     {
       if(UIM_SLOT_STATE_INACTIVE == uim_sm_data.uim_slot_info[temp_instance].activity_status)
       {
         uim_ptr = uim_get_instance_ptr(temp_instance);
         UIM_MSG_HIGH_2(" Queuing %x command for inactive slot 0x%x",
                        cmd_ptr->hdr.command, temp_instance);
         uim_sm_build_command(cmd_ptr->hdr.command, uim_ptr, UIM_NO_ACTION);
       }

     }
     rex_leave_crit_sect(&uim_sm_data.uim_sm_data_crit_sect);
   }
  #endif /* FEATURE_UIM_DS_SUBSCRIPTION_MANAGER */

  if(cmd_ptr->hdr.slot == UIM_SLOT_1)
  {
    uim_ptr = uim_get_instance_ptr(UIM_INSTANCE_1);
  }
  else if(cmd_ptr->hdr.slot == UIM_SLOT_2)
  {
    uim_ptr = uim_get_instance_ptr(UIM_INSTANCE_2);
  }
  else if(cmd_ptr->hdr.slot == UIM_SLOT_3)
  {
    uim_ptr = uim_get_instance_ptr(UIM_INSTANCE_3);
  }

  if(uim_ptr == NULL)
  {
    UIM_MSG_ERR_1("Invalid slot value 0x%x", cmd_ptr->hdr.slot);
    return UIM_INVALID_PARAMS;
  }

  if(TRUE == uim_ptr->flag.task_dereg_from_rcinit)
  {
    UIMDRV_MSG_ERR_0(uim_ptr->id,"Task Is Terminated, Rejecting command");
    return UIM_GENERIC_ERROR;
  }

  if (!UIM_IS_CMD_VALID_IN_MODE(cmd_ptr, uim_ptr))
  {
    UIM_ERROR_MSG_0("Un expected command while UIM in passive mode");
  }

  cmd_size = uim_calculate_cmd_size(cmd_ptr,uim_ptr);

  if(cmd_size < sizeof(uim_cmd_type))
  {
    UIMDRV_MSG_ERR_1(uim_ptr->id,
                     "Incorrect cmd_size 0x%x. Can not queue uim cmd", cmd_size);
    return UIM_INVALID_PARAMS;
  }

  uim_task_cmd_ptr = uim_malloc(cmd_size);

  if(NULL == uim_task_cmd_ptr)
  {
    return UIM_MEM_PROBLEM;
  }

  if(FALSE == uim_copy_cmd_data(uim_task_cmd_ptr,cmd_size, cmd_ptr,uim_ptr))
  {
    UIM_FREE(uim_task_cmd_ptr);
    return UIM_MEM_PROBLEM;
  }

  /* For now set done_q_ptr to null. We will put cmd_ptr from GSTK back
     in its queue at the end of this function. Keeping the done_ptr
     in UIM's copy of command will result in double free */

  uim_task_cmd_ptr->hdr.cmd_hdr.done_q_ptr = NULL;

  /* init link */
  ( void )q_link( uim_task_cmd_ptr, &uim_task_cmd_ptr->hdr.cmd_hdr.link );

  /* and queue it */
  q_put( &uim_ptr->task.cmd_q, &uim_task_cmd_ptr->hdr.cmd_hdr.link );

  /* and signal the UIM task */
  (void)rex_set_sigs( uim_ptr->tcb_ptr, UIM_CMD_Q_SIG );

  /* Put command back in GSTK done queue */
  if (cmd_ptr->hdr.cmd_hdr.done_q_ptr != NULL )
  {
     q_put( cmd_ptr->hdr.cmd_hdr.done_q_ptr,
            &cmd_ptr->hdr.cmd_hdr.link );
  }
  return UIM_SUCCESS;
} /* uim_cmd */


/*===========================================================================
FUNCTION UIM_SILENT_RECOVERY_CB

DESCRIPTION
  The uim_cmd pointed to by the parameter passed in is queued up for UIM.

DEPENDENCIES
  None.

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE
===========================================================================*/
void uim_silent_recovery_cb
(
  uim_rpt_type *report_ptr
)
{
  if(NULL != report_ptr->user_data)
  {
    /* Free the allocated Memory */
    UIM_FREE(report_ptr->user_data);
  }
}/* uim_silent_recovery_cb */


/*===========================================================================
FUNCTION UIM_TRIGGER_SILENT_RECOVERY

DESCRIPTION
  This uim_recover_uim function prepares the comamnd onbehalf of client and
  Queue it in UIM command queue.If this exposed API is invoked again and again
  by cllient then for all subsequent invocation,It returns an error and does not
  Queue comamnd in command Queue.

DEPENDENCIES
  None.

RETURN VALUE
  UIM_SUCCESS        :- Command queued to uim_cmd_q successfully.
  UIM_INVALID_PARAMS :- Invalid params sent by the client.
  UIM_MEM_PROBLEM    :- Error during malloc/memcpy operations.
  UIM_GENERIC_ERROR  :- Generic error

SIDE EFFECTS
  Sets the UIM_CMD_Q_SIG.
===========================================================================*/
uim_return_type uim_trigger_silent_recovery
(
  uim_slot_type                 slot,
  const void                    *user_data,
  uim_silent_recovery_cb_type   user_callback
)
{
  uim_cmd_type               *cmd_ptr  = NULL;
  uim_instance_global_type   *uim_ptr  = NULL;

  if (((slot < UIM_SLOT_1) && (slot > UIM_SLOT_3) ) ||
      (NULL == (uim_ptr = uim_get_instance_ptr((uim_instance_enum_type)slot -1))) ||
      (NULL == (cmd_ptr = uim_malloc(sizeof(uim_cmd_type)))) )
  {
    /* Either Slot is out of range or
       Memory Allocation is failed */
    return (UIM_GENERIC_ERROR);
  }

  
  /* Remembering the call back so that response can be
  dispatched to client along with received user_data
  and recovery stage*/
  uim_ptr->silent_recover.uim_silent_recovery_callback = user_callback;
  /* Copying the user data */
  uim_ptr->silent_recover.user_data = user_data;

  /***********************************************
   * Prepare the command for SILENT RECOVERY.
   ***********************************************/

  cmd_ptr->hdr.command             = UIM_SILENT_RECOVER_UICC_F;
  cmd_ptr->hdr.channel             = UIM_CHANNEL0;
  cmd_ptr->hdr.protocol            = UIM_WCDMA;
  cmd_ptr->hdr.slot                = (uim_slot_type)slot;
  cmd_ptr->hdr.cmd_hdr.task_ptr    = NULL;
  cmd_ptr->hdr.cmd_hdr.sigs        = 0;
  cmd_ptr->hdr.options             = UIM_OPTION_ALWAYS_RPT;
  cmd_ptr->hdr.cmd_count           = 0;
  cmd_ptr->hdr.cmd_hdr.done_q_ptr  = NULL;
  /* This callback shall be used to free cmd_ptr assigned as user_data */
  cmd_ptr->hdr.rpt_function        = uim_silent_recovery_cb;

  /*This user_data shall be used to free in the callback */
  cmd_ptr->hdr.user_data          = (uint32)cmd_ptr;

  /* init link */
  ( void )q_link( cmd_ptr, &cmd_ptr->hdr.cmd_hdr.link );

  /* and queue it */
  q_put( &uim_ptr->task.cmd_q, &cmd_ptr->hdr.cmd_hdr.link );

  /* and signal the UIM task */
  (void)rex_set_sigs( uim_ptr->tcb_ptr, UIM_CMD_Q_SIG );


  return (UIM_SUCCESS);
} /* uim_trigger_silent_recovery */


/*===========================================================================

FUNCTION UIM_INSERT_CMD_AT_HEAD_OF_QUEUE

DESCRIPTION
 This function will put the command back in the head of the queue so that,
 it can be processed again .

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void uim_insert_cmd_at_head_of_queue
(
  uim_cmd_type             *cmd_ptr,
  uim_instance_global_type *uim_ptr
)
{
  uim_ptr->flag.command_requeued = TRUE;
  /* Put the command back in the head of the queue so that it can be
       processed again */
  if ( q_cnt( &uim_ptr->task.cmd_q ) == 0 )
  {
    q_put( &uim_ptr->task.cmd_q, &cmd_ptr->hdr.cmd_hdr.link );
  }
  else
  {
    q_insert( & uim_ptr->task.cmd_q,
              & cmd_ptr->hdr.cmd_hdr.link,
              & ((uim_cmd_type*)q_check( &uim_ptr->task.cmd_q ))->hdr.cmd_hdr.link);
  }
} /* uim_insert_cmd_at_head_of_queue */


/*===========================================================================

FUNCTION UIM_WAKE_UP_POLL_TIME_EXPIRY_CB

DESCRIPTION
  This procedure is registered as a call-back associated with the expiry of
  the wake up poll time. This call-back is called in the context of the
  high priority timer task as soon as the rex timer for the wake up poll
  timer expires


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  No wake up poll timer value may get updated.

===========================================================================*/
static void uim_wake_up_poll_timer_expiry_cb (unsigned long uim_ptr_param)
{
  rex_timer_cnt_type     new_poll_time   = 0;
  rex_timer_cnt_type     new_poll_offset = 0;
  uim_instance_global_type *uim_ptr =
    (uim_instance_global_type*)uim_ptr_param;

  RETURN_IF_INSTANCE_INVALID(uim_ptr->id);

  (void) rex_set_sigs(uim_ptr->tcb_ptr, UIM_POLL_TIMER_SIG);

  /* Non-deferrable poll timer should not expire normally if we are
     operating in G/W/L/TD mode.
     If it expired then it is an indication that we are offset from DRX cycle.
     For CDMA max DRX cycle is 5.12 second. So we need to set our
     deferrable poll timer to 29.5 - 5.12 seconds. We need to give 2 ms
     for software processing and modem wake up delay, similarly
     For W/L/TD DRX interval ranges from 1.28 to 5.12 sec
     For G      DRX interval is fixed to 1.17 sec

     We will reduce the no wake up poll interval by step size of
     1.28 + 2 ms if it does not align with the DRX wake up.

     The lower bound for deferrable poll interval should be 24.378 */

  if((uim_ptr->setting.poll_time_delay <= UIM_POLL_TIME) &&
     (uim_ptr->setting.poll_time_delay >= UIM_CDMA_POLL_TIME))
  {
    new_poll_offset = uim_ptr->setting.wakeup_poll_offset + 1282;
    new_poll_time   = uim_ptr->setting.poll_time_delay - 1282;

    if(new_poll_time <=  UIM_CDMA_POLL_TIME)
    {
      new_poll_time   = UIM_CDMA_POLL_TIME;
      new_poll_offset = UIM_CDMA_WAKE_UP_POLL_OFFSET;
    }
    UIMDRV_MSG_HIGH_2(uim_ptr->id,
                      "UIM will now poll with defferable time interval %d ms and deadline of %d ms",
                       new_poll_time, (new_poll_time+new_poll_offset));
    uim_set_poll_timer( new_poll_time,
                        new_poll_offset,
                        uim_ptr);
  }
} /* uim_wake_up_poll_timer_expiry_cb */


/*===========================================================================

FUNCTION UIM_BUSY_IND_TIMER_STATE_HANDLER

DESCRIPTION
  This function handles actions and change of state of uim_busy_ind_timer,
  when some event happens.


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void uim_busy_ind_timer_state_handler
(
  uim_instance_global_type          *uim_ptr,
  uim_busy_response_timer_event_type busy_timer_event
)
{
  if ( NULL == uim_ptr)
  {
    return;
  }

  switch (uim_ptr->card_cmd.uim_busy_ind_timer_state)
  {
    case UIM_BUSY_RESPONSE_TIMER_NOT_USED:
      if ( busy_timer_event == UIM_BUSY_RESPONSE_EVT_NV_INIT &&
           uim_ptr->setting.uim_feature_busy_response.uim_busy_ind_timer_val_ms > 0)
      {
        /* Called in uim_nv_init() */
        /* Define Timer */
        timer_def(&uim_ptr->card_cmd.uim_busy_ind_timer,
                  &uim_ptr->task.always_on_timer_group,
                  uim_ptr->tcb_ptr,
                  UIM_BUSY_IND_TIMER_EXP_SIG, 0, 0);
        /* Set the timer state to not started to indicate the timer is defined but not start yet */
        uim_ptr->card_cmd.uim_busy_ind_timer_state = UIM_BUSY_RESPONSE_TIMER_NOT_STARTED;
      }
      break;
    case UIM_BUSY_RESPONSE_TIMER_NOT_STARTED:
      if ( busy_timer_event == UIM_BUSY_RESPONSE_EVT_SEND_APDU ||
           busy_timer_event == UIM_BUSY_RESPONSE_EVT_T_1_SEND_I_BLOCK ||
           busy_timer_event == UIM_BUSY_RESPONSE_EVT_T_1_SEND_R_BLOCK ||
           busy_timer_event == UIM_BUSY_RESPONSE_EVT_T_1_SEND_S_BLOCK )
      {
        /* Called in uim_send_apdu()/uim_t1_send_%_block() */
        /* Start timer */
        timer_set( &uim_ptr->card_cmd.uim_busy_ind_timer,
                   uim_ptr->setting.uim_feature_busy_response.uim_busy_ind_timer_val_ms,
                   0, T_MSEC );
        /* Set the timer state to active to indicate the timer is running */
        uim_ptr->card_cmd.uim_busy_ind_timer_state = UIM_BUSY_RESPONSE_TIMER_ACTIVE;
      }
      break;
    case UIM_BUSY_RESPONSE_TIMER_ACTIVE:
      if ( busy_timer_event == UIM_BUSY_RESPONSE_EVT_CMD_RSP_SIG ||
           busy_timer_event == UIM_BUSY_RESPONSE_EVT_NOTIFY_ERROR )
      {
        /* Called in UIM_CMD_RSP_SIG handling in uim_task_common()/notify_error() */
        /* clear UIM_BUSY_IND_TIMER_EXP_SIG, in case uim gets response/notify_error and timeout at the same time */
        (void) rex_clr_sigs( uim_ptr->tcb_ptr, UIM_BUSY_IND_TIMER_EXP_SIG );
        /* Clear busy_ind_timer */
        rex_clr_timer(&uim_ptr->card_cmd.uim_busy_ind_timer);
        /* Set the timer state to active to indicate the timer is defined but not start yet */
        uim_ptr->card_cmd.uim_busy_ind_timer_state = UIM_BUSY_RESPONSE_TIMER_NOT_STARTED;
      }
      else if ( busy_timer_event == UIM_BUSY_RESPONSE_EVT_BUSY_IND_TIMER_EXP_SIG )
      {
        /* Called in UIM_BUSY_IND_TIMER_EXP_SIG handling in uim_task_common() */
        /* clear UIM_BUSY_IND_TIMER_EXP_SIG */
        (void) rex_clr_sigs( uim_ptr->tcb_ptr, UIM_BUSY_IND_TIMER_EXP_SIG );
        /* Notify mmgsdi of uim busy */
        UIMDRV_MSG_HIGH_0(uim_ptr->id,"uim_notify_busy(TRUE, slot)");
        (void) mmgsdi_notify_sim_busy( TRUE, (uim_slot_type)(uim_ptr->id + 1));
        /* Set the timer state from active to expired */
        uim_ptr->card_cmd.uim_busy_ind_timer_state = UIM_BUSY_RESPONSE_TIMER_EXPIRED;
      }
      break;
    case UIM_BUSY_RESPONSE_TIMER_EXPIRED:
      if ( busy_timer_event == UIM_BUSY_RESPONSE_EVT_CMD_RSP_SIG ||
           busy_timer_event == UIM_BUSY_RESPONSE_EVT_NOTIFY_ERROR )
      {
        /* Called in UIM_CMD_RSP_SIG handling in uim_task_common() */
        /* Notify mmgsdi of uim NOT busy */
        UIMDRV_MSG_HIGH_0(uim_ptr->id,"uim_notify_busy(FALSE, slot)");
        (void) mmgsdi_notify_sim_busy( FALSE, (uim_slot_type)(uim_ptr->id + 1));
        /* Clear busy_ind_timer (just in case)*/
        rex_clr_timer(&uim_ptr->card_cmd.uim_busy_ind_timer);
        /* Set the timer state to active to indicate the timer is defined but not start yet */
        uim_ptr->card_cmd.uim_busy_ind_timer_state = UIM_BUSY_RESPONSE_TIMER_NOT_STARTED;
      }
      break;
    default:
      break;
  }
}/* uim_busy_ind_timer_state_handler */


/*===========================================================================

FUNCTION UIM_TRANS_TIMER_STATE_HANDLER

DESCRIPTION
  This function handles actions and change of state of uim_trans_timer,
  when some event happens.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void uim_trans_timer_state_handler
(
  uim_instance_global_type *uim_ptr,
  uim_busy_response_timer_event_type trans_timer_event
)
{
  if ( NULL == uim_ptr )
  {
    return;
  }

  switch (uim_ptr->card_cmd.uim_trans_timer_state)
  {
    case UIM_BUSY_RESPONSE_TIMER_NOT_USED:
      if ( trans_timer_event == UIM_BUSY_RESPONSE_EVT_NV_INIT &&
           uim_ptr->setting.uim_feature_busy_response.uim_trans_timer_val_ms > 0 )
      {
        /* Define Timer */
        timer_def(&uim_ptr->card_cmd.uim_trans_timer,
                  &uim_ptr->task.always_on_timer_group,
                  uim_ptr->tcb_ptr,
                  UIM_TRANS_TIMER_EXP_SIG, 0, 0);
        /* Set the timer state to not started to indicate the timer is being used */
        uim_ptr->card_cmd.uim_trans_timer_state = UIM_BUSY_RESPONSE_TIMER_NOT_STARTED;
      }
      break;
    case UIM_BUSY_RESPONSE_TIMER_NOT_STARTED:
      if ( trans_timer_event == UIM_BUSY_RESPONSE_EVT_SEND_APDU ||
           trans_timer_event == UIM_BUSY_RESPONSE_EVT_T_1_SEND_I_BLOCK ||
           trans_timer_event == UIM_BUSY_RESPONSE_EVT_T_1_SEND_R_BLOCK ||
           trans_timer_event == UIM_BUSY_RESPONSE_EVT_T_1_SEND_S_BLOCK )
      {
        /* Called in uim_send_apdu()/uim_t1_send_%_block() */
        /* Start timer */
        timer_set( &uim_ptr->card_cmd.uim_trans_timer,
                   uim_ptr->setting.uim_feature_busy_response.uim_trans_timer_val_ms,
                   0, T_MSEC );
        /* Set the timer state to active to indicate the timer is running */
        uim_ptr->card_cmd.uim_trans_timer_state = UIM_BUSY_RESPONSE_TIMER_ACTIVE;
      }
      break;
    case UIM_BUSY_RESPONSE_TIMER_ACTIVE:
      if ( trans_timer_event == UIM_BUSY_RESPONSE_EVT_CMD_RSP_SIG ||
           trans_timer_event == UIM_BUSY_RESPONSE_EVT_NOTIFY_ERROR )
      {
        /* Called in UIM_CMD_RSP_SIG handling in uim_task_common() */
        /* clear UIM_TRANS_TIMER_EXP_SIG, in case uim gets response/notify_error/timeout at the same time */
        (void) rex_clr_sigs( uim_ptr->tcb_ptr, UIM_TRANS_TIMER_EXP_SIG );
        /* Clear uim_trans_timer */
        rex_clr_timer(&uim_ptr->card_cmd.uim_trans_timer);
        /* Set the timer state to active to indicate the timer is defined but not start yet */
        uim_ptr->card_cmd.uim_trans_timer_state = UIM_BUSY_RESPONSE_TIMER_NOT_STARTED;
      }
      else if ( trans_timer_event == UIM_BUSY_RESPONSE_EVT_TRANS_TIMER_EXP_SIG )
      {
        /* Called in UIM_TRANS_TIMER_SIG handling in uim_task_common() */
        /* Clear UIM_TRANS_TIMER_SIG */
        (void) rex_clr_sigs( uim_ptr->tcb_ptr, UIM_TRANS_TIMER_EXP_SIG );
        /* Set the timer state from active to expired */
        uim_ptr->card_cmd.uim_trans_timer_state = UIM_BUSY_RESPONSE_TIMER_EXPIRED;
        /* If UIMDRV_FEATURE_DISABLE_RECOVERY_UPON_INFINITE_NULL is enabled and infinite_null_count > 0, which means
           that this timeout is due to NULL bytes, uim will power down and inform the AP clients.
        */
        if ( uim_ptr->card_cmd.infinite_null_count >  0 &&
             uim_nv_is_feature_enabled(UIMDRV_FEATURE_DISABLE_RECOVERY_UPON_INFINITE_NULL, uim_ptr) == TRUE )
        {
          UIMDRV_MSG_ERR_0(uim_ptr->id,"DISABLE_RECOVERY_UPON_INFINITE_NULL enabled: Max NULL bytes received. UIM wll power down");
          (void)rex_set_sigs(uim_ptr->tcb_ptr, UIM_TRANSACTION_SIG);
        }
        else
        {
          UIMDRV_MSG_ERR_0(uim_ptr->id,"UIM will force recovery due to trans timer timeout");
          /* Force recovery */
          uim_force_recovery(uim_ptr);
        }
      }
      break;
    case UIM_BUSY_RESPONSE_TIMER_EXPIRED:
      if ( trans_timer_event == UIM_BUSY_RESPONSE_EVT_CMD_RSP_SIG ||
           trans_timer_event == UIM_BUSY_RESPONSE_EVT_NOTIFY_ERROR )
      {
        /* Called in UIM_CMD_RSP_SIG handling in uim_task_common() */
        /* Clear uim_trans_timer */
        rex_clr_timer(&uim_ptr->card_cmd.uim_trans_timer);
        /* Set the timer state to active to indicate the timer is defined but not start yet */
        uim_ptr->card_cmd.uim_trans_timer_state = UIM_BUSY_RESPONSE_TIMER_NOT_STARTED;
      }
      break;
    default:
      break;
  }/* end switch - uim_trans_timer_state case */
}/* uim_trans_timer_state_handler */


/*===========================================================================

FUNCTION UIM_SIMULATE_NULL_TIMER_STATE_HANDLER

DESCRIPTION
  This function handles actions and change of state of uim_simulate_null_timer,
  when some event happens. This function is only for SIM BUSY Feature testing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void uim_simulate_null_timer_state_handler
(
  uim_instance_global_type *uim_ptr,
  uim_busy_response_timer_event_type simulate_null_timer_event
)
{
  if ( NULL == uim_ptr ) {
    return;
  }

  switch (uim_ptr->card_cmd.uim_simulate_null_timer_state)
  {
    case UIM_BUSY_RESPONSE_TIMER_NOT_USED:
      if ( simulate_null_timer_event == UIM_BUSY_RESPONSE_EVT_NV_INIT &&
           uim_ptr->debug.uim_feature_busy_response_simulate_null.uim_disable_simulate_null == FALSE )
      {
        /* Called in uim_nv_init() if any of Busy respone timer in use */
        /* Define Timer */
        timer_def(&uim_ptr->card_cmd.uim_simulate_null_timer,
                  &uim_ptr->task.always_on_timer_group,
                  uim_ptr->tcb_ptr,
                  UIM_SIMULATE_NULL_TIMER_EXP_SIG, 0, 0);
        /* Set the timer state to not started to indicate the timer is defined but not start yet */
        uim_ptr->card_cmd.uim_simulate_null_timer_state = UIM_BUSY_RESPONSE_TIMER_NOT_STARTED;
      }
      break;
    case UIM_BUSY_RESPONSE_TIMER_NOT_STARTED:
      if (simulate_null_timer_event == UIM_BUSY_RESPONSE_EVT_CMD_RSP_SIG &&
           uim_ptr->debug.uim_simulate_null_timer_val_s > 0)
      {
        /* Called in uim_generic_command() UIM_SIMULATION_NULL_ST processing */
        /* Start timer */
        timer_set( &uim_ptr->card_cmd.uim_simulate_null_timer,
                   uim_ptr->debug.uim_simulate_null_timer_val_s,
                   0, T_SEC );

        /* Set the timer state to active to indicate the timer is running */
        uim_ptr->card_cmd.uim_simulate_null_timer_state = UIM_BUSY_RESPONSE_TIMER_ACTIVE;
      }
      break;
    case UIM_BUSY_RESPONSE_TIMER_ACTIVE:
      if ( simulate_null_timer_event == UIM_BUSY_RESPONSE_EVT_SIMULATE_NULL_TIMER_EXP_SIG )
      {
        /* Called in UIM_SIMULATE_NULL_TIMER_EXP_SIG handling in uim_task_common() */
        /* clear UIM_SIMULATE_NULL_TIMER_EXP_SIG */
        (void) rex_clr_sigs( uim_ptr->tcb_ptr, UIM_SIMULATE_NULL_TIMER_EXP_SIG );
        /* Handle busy response timers when get simulated response after "YY" sec of NULL byte */
        uim_busy_ind_timer_state_handler(uim_ptr, UIM_BUSY_RESPONSE_EVT_CMD_RSP_SIG);
        uim_trans_timer_state_handler(uim_ptr, UIM_BUSY_RESPONSE_EVT_CMD_RSP_SIG);
        /* Set CMD_RSP_SIG to trigger generic command state machine to move state to UIM_DONE_ST */
        (void) rex_set_sigs( uim_ptr->tcb_ptr, UIM_CMD_RSP_SIG );
        /* Clear uim_simulate_null_timer */
        rex_clr_timer(&uim_ptr->card_cmd.uim_simulate_null_timer);
        /* Set the timer state to active to indicate the timer is defined but not start yet */
        uim_ptr->card_cmd.uim_simulate_null_timer_state = UIM_BUSY_RESPONSE_TIMER_NOT_STARTED;
      }
      else if ( simulate_null_timer_event ==  UIM_BUSY_RESPONSE_EVT_POWER_DOWN )
      {
        /* Called in notify_error() */
        /* clear UIM_SIMULATE_NULL_TIMER_EXP_SIG, in case power down and timeout at the same time */
        (void) rex_clr_sigs( uim_ptr->tcb_ptr, UIM_SIMULATE_NULL_TIMER_EXP_SIG );
        /* Clear simulate_null_timer */
        rex_clr_timer(&uim_ptr->card_cmd.uim_simulate_null_timer);
        /* Set the timer state to active to indicate the timer is defined but not start yet */
        uim_ptr->card_cmd.uim_simulate_null_timer_state = UIM_BUSY_RESPONSE_TIMER_NOT_STARTED;
      }
      break;
    default:
      break;
  }
}/* uim_simulate_null_timer_state_handler */


/*===========================================================================

FUNCTION UIM_UPDATE_SWP_CAPABILITY

DESCRIPTION
  This function updates the global variable of UIM based on .
  the value set in NV-70210 (uimSwpCapabilitySupport) per-slot
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void uim_update_swp_capability
(
  uim_instance_global_type *uim_ptr
)
{
  if ( (NULL == uim_ptr) ||
       (uim_ptr->id >= UIM_INSTANCE_INVALID))
  {
    return;
  }

  if (FALSE == m_HWConfig.uimSwpCapabilitySupported[uim_ptr->id])
  {
    uim_ptr->flag.uim_swp_in_terminal_capability = FALSE;
  }
  else
  {
    uim_ptr->flag.uim_swp_in_terminal_capability = TRUE;
  }
  UIMDRV_MSG_HIGH_1(uim_ptr->id,"SWP %d ",uim_ptr->flag.uim_swp_in_terminal_capability);
}/* uim_update_swp_capability */

/*===========================================================================
FUNCTION uim_check_cdma_df_select_before_tp

DESCRIPTION
  This function checks weather to selct CDMA DF explicitly,
  based on the nv data taken from mmgsdi

PARAMETER
  None

DEPENDENCIES
  None

RETURN VALUE
  boolean

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/

LOCAL boolean uim_check_cdma_df_select_before_tp(void)
{
  mmgsdi_cached_nv_item_data nv_item;
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_SUCCESS;

  memset(&nv_item, 0x00, sizeof(mmgsdi_cached_nv_item_data));

  nv_item.nv_item_enum = MMGSDI_NV_CDMA_DF_SELECT_BEFORE_TP_I;
  mmgsdi_status = mmgsdi_get_cached_nv_item(&nv_item);
  if(mmgsdi_status == MMGSDI_SUCCESS &&
     nv_item.nv_item_data.cdma_df_select_before_tp == TRUE)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
} /* uim_check_cdma_df_select_before_tp */

/**
 * This function event handler for the extend recovery feature.
 *  (this is not a state machine)
 *
 *@param uim_ptr   Pointer to the UIM instance specific globals.
 *@param evt       Extended recovery event
 */
void uim_extended_recovery_event_handler
(
  uim_instance_global_type *uim_ptr,
  uim_enhanced_recovery_evt_type  evt
)
{
  uint8 num_ch=0;
  UIMDRV_MSG_HIGH_1(uim_ptr->id,"uim_extended_recovery_event_handler. evt 0x%x", evt);
  switch(evt)
  {
    case UIM_EXTENDED_RECOVERY_EVT_NV_INIT:
      /*initalises the extended recovery globals*/
      if(0 != uim_ptr->extended_recovery.recovery_config.no_of_extended_powerups)
      {
        timer_def(&uim_ptr->extended_recovery.extended_recovery_timer,
                  &uim_ptr->task.always_on_timer_group,
                  uim_ptr->tcb_ptr,
                  UIM_EXT_RECOVERY_TIMER_EXP_SIG, 0, 0);

        uim_ptr->extended_recovery.extended_recovery_enabled = TRUE;
      }
      else
      {
        uim_ptr->extended_recovery.extended_recovery_enabled = FALSE;
      }
      break;

    case UIM_EXTENDED_RECOVERY_EVT_TIMER_START:
      /*check for the remaining powerups and start the timer accordingly*/
      uim_ptr->extended_recovery.extended_recovery_in_progress = TRUE;
      if(++uim_ptr->extended_recovery.count <=
           uim_ptr->extended_recovery.recovery_config.no_of_extended_powerups)
      {
        UIMDRV_MSG_HIGH_1(uim_ptr->id,"starting timer. for powerup no 0x%x", uim_ptr->extended_recovery.count);
        if(uim_ptr->extended_recovery.recovery_config.incremental_delay)
        {
          timer_set( &uim_ptr->extended_recovery.extended_recovery_timer,
          (uim_ptr->extended_recovery.count)*(uim_ptr->extended_recovery.recovery_config.delay_value),
                   0, T_MSEC);
        }
        else
        {
          timer_set( &uim_ptr->extended_recovery.extended_recovery_timer,
                      uim_ptr->extended_recovery.recovery_config.delay_value,
                      0, T_MSEC);
        }
      }
      else
      {
        UIMDRV_MSG_HIGH_0(uim_ptr->id,"all powerups exhausted clearing relevant globals");
        uim_ptr->extended_recovery.extended_recovery_in_progress = FALSE;
        uim_ptr->extended_recovery.count = 0;
      }
      break;

    case UIM_EXTENDED_RECOVERY_EVT_TIMER_EXP:
      /*prepare UIM for a powerup*/
      uim_reset_globals(uim_ptr);
      uim_ptr->command.static_cmd_buf.hdr.cmd_count = 1;
      uim_ptr->debug.convention_change_count = 1;
      uim_ptr->card_cmd.instrn_toggle_count = 1;
      uim_ptr->command.static_cmd_buf.hdr.slot =
       (uim_slot_type)(uim_ptr->id + 1);
      uim_ptr->flag.me_powerup = TRUE;
      /* Reset Channel Open information
       *  Default channel is always open - not changing its status */
      for(num_ch=1; num_ch<UIM_MAX_CHANNELS; num_ch++)
      {
        uim_ptr->state.is_ch_open[num_ch] = FALSE;
      }
      break;

    case UIM_EXTENDED_RECOVERY_EVT_TERMINATE:
      /*abondon the extended recovery process as:
        1. extended powerup was succesful
        2. or need to stop the extended recovery process becuse of
             hotswap, AP mode ON/FF, Task stop signal */
      (void) rex_clr_sigs( uim_ptr->tcb_ptr, UIM_EXT_RECOVERY_TIMER_EXP_SIG);
      rex_clr_timer(&uim_ptr->extended_recovery.extended_recovery_timer);
      uim_ptr->extended_recovery.extended_recovery_in_progress = FALSE;
      uim_ptr->extended_recovery.count = 0;
      break;
    default:
      break;
  }
} /*uim_extended_recovery_event_handler*/

#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /*FEATURE_UIM_TEST_FRAMEWORK*/


/*===========================================================================

FUNCTION UIM_CHECK_EXPLICIT_SELECT_DF_CDMA_BEFORE_TELECOM_ACCESS

DESCRIPTION
  This function checks weather an explicit selection of CDMA DF is required,
  before telecom access for RUIM Card

DEPENDENCIES
  It is based on MMGSDI_NV_EXPLICIT_SELECT_DFCDMA_BEFORE_TELECOM_ADN_READ_I
  NV item.

RETURN VALUE
  boolean, It returns true when RUIM card is used and last selected directory
  was not the TELECOM or CDMA DF, false otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean uim_check_explicit_select_df_cdma_before_telecom_access
(
  uim_instance_global_type     *uim_ptr
)
{
  mmgsdi_cached_nv_item_data nv_item;
  mmgsdi_return_enum_type    mmgsdi_status = MMGSDI_SUCCESS;

  memset(&nv_item, 0x00, sizeof(mmgsdi_cached_nv_item_data));

  if( uim_ptr->command.cmd_ptr->hdr.protocol != UIM_ICC  ||
      uim_ptr->state.dfs_1.cdma_df_present != TRUE )
  {
    return FALSE;
  }

  if (uim_ptr->state.current_path[0].path.path[1] == UIM_TELECOM_DF ||
      uim_ptr->state.current_path[0].path.path[1] == UIM_CDMA_DF)
  {
    return FALSE;
  }

  nv_item.nv_item_enum = MMGSDI_NV_EXPLICIT_SELECT_DFCDMA_BEFORE_TELECOM_ADN_READ_I;
  mmgsdi_status = mmgsdi_get_cached_nv_item(&nv_item);
  if( (mmgsdi_status != MMGSDI_SUCCESS ) ||
      (nv_item.nv_item_data.cdma_df_select_before_telecom_adn != TRUE) )
  {
    return FALSE;
  }

  UIMDRV_MSG_HIGH_0(uim_ptr->id, "Specific handling of Telecom Access");
  return TRUE;
} /* uim_check_explicit_select_df_cdma_before_telecom_access */

/*===========================================================================

FUNCTION UIM_IS_USB_UICC_SUPPORTED

DESCRIPTION
  whether USB UICC is supported or not.

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None

===========================================================================*/
boolean uim_is_usb_uicc_supported(void)
{
  boolean flag = FALSE;
#ifdef FEATURE_UIM_USB_UICC
  uim_instance_enum_type uim_id = UIM_INSTANCE_1;
  for (uim_id = UIM_INSTANCE_1; uim_id <= UIM_MAX_INSTANCES; ++uim_id)
  {
    flag = flag || uim_is_usb_enabled_on_slot(uim_id);
  }
#endif /* FEATURE_UIM_USB_UICC */
  return flag;
} /* uim_is_usb_uicc_supported */

/*===========================================================================

FUNCTION UIM_IS_SAP_CLIENT_SUPPORTED

DESCRIPTION
  whether SAP CLIENT is supported or not.

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None

===========================================================================*/
boolean uim_is_sap_client_supported(void)
{
  boolean flag = FALSE;
#ifdef FEATURE_UIM_BTSAP_CLIENT
  flag = TRUE;
#endif /* FEATURE_UIM_BTSAP_CLIENT */
  return flag;
} /* uim_is_sap_client_supported */


/*===========================================================================

FUNCTION UIM_CHECK_PENDING_PROACTIVE_COMMAND

DESCRIPTION
  This procedure return true if check pending proactive request
  is successfully sent to the UIM task.

DEPENDENCIES
  None

RETURN VALUE
  Returns boolean.

SIDE EFFECTS
  None

===========================================================================*/
boolean uim_check_pending_proactive_command
(
  uim_slot_type uim_slot
)
{
  uim_instance_global_type *uim_ptr  = NULL;
  uim_ptr = uim_get_instance_ptr((uim_instance_enum_type)(uim_slot - 1));
  if(uim_ptr == NULL)
  {
    return FALSE;
  }
  if( UIM_ERR_S == uim_ptr->state.status || UIM_NORMAL_MODE != uim_ptr->command.mode )
  {
    UIMDRV_MSG_ERR_0(uim_ptr->id,"UIM is in erorr state or not in normal mode");
    return FALSE;
  }
  
  UIMDRV_MSG_LOW_0(uim_ptr->id, "uim_check_pending_proactive_command");
  (void) rex_set_sigs(uim_ptr->tcb_ptr, UIM_FETCH_PENDING_SIG);
  return TRUE;
}/* uim_check_pending_proactive_command*/



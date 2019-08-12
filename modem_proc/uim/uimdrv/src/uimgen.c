/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


              U I M   G E N E R I C   S T A T E   M A C H I N E

GENERAL DESCRIPTION
  This module contains the state machine for the generic protocol of the UIM
  interface. Generic module processes all the commands that are common to
  R-UIM, SIM and USIM.

EXTERNALIZED FUNCTIONS

  uim_process_generic_command
    Processes a generic UIM server command

  uim_process_generic_response
    Processes the responses for a generic UIM server command

  uim_send_generic_err_rpt
    Processes an error condition for a generic UIM server command

  uim_generic_command
    This function contains the state machine that processes APDUs
    for generic commands.

  uim_generic_command_response
    This function processes the UIM responses from APDUs.

Copyright(c) 2001,2007-2016 by Qualcomm Technologies, Inc. All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimdrv/src/uimgen.c#1 $
$DateTime: 2016/12/13 08:00:05 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/22/16   na      Initializing status as UIM_INITIALIZED_S
07/08/16   ks      Correcting the BWT calculation
07/01/16   ks      Fix to re-attempt select MF upon receiving 6F 00
06/27/16   sam     Clear recenty added UIM globals upon RESET_F command
06/24/16   sam     Avoid selection of 7FFF file on non-provisioning application
06/20/16   na      Do not retry manage channel request upon receipt of sw1 0x6E
06/10/16   na      Removed F3 traces
06/07/16   dd      Propoer parsing of ATR to extract eUICC information
06/06/16   dd      F3 Optimization on curr path
05/23/16   na      F3 Optimization
05/23/16   dd      Enhancing the voltage supply indication to QMI feature
05/18/16   ssr     Remove msg for malloc failure
05/17/16   dd      Reduce number of ATR prints under 20 F3s/ms
05/10/16   gm      Fix to allow only Telecomm CLA for Get Response in Passive mode
05/10/16   gm      Reducing the recovery attempts for the remote SIM
05/09/16   sam     Queue a open channel command internally after terminal profile
05/04/16   lm      Iccid length is being filled from file size received in FCP of EF-ICCID
04/29/16   ks      Fix issue wtih ATR parsing for eUICC information
04/13/16   ks      Send generic error report for the toolkit commands removed 
                   from uim cmd queue during recovery
04/11/16   ks      Code changes needed for LPA support
04/11/16   na      Corrected the calculation of cwt for T=1
03/22/16   na      Do not update current path for streaming apdu in ICC mode
03/09/16   na      Corrected the calculation of cwt for T=1
01/04/16   ks      Not parsing the voltage parameters in BTSAP client mode
12/23/15   sam     Fix for KW errors
11/26/15   ks      Do not attempt close channel command upon its failure
11/26/15   sp      write back EF-TST data to card from cache after recovery
11/23/15   ks      Recovery upon failure of close channel 
                   where ARAM app is selected
11/05/15   ks      Rel12 polling enhancement
11/11/15   sam     Explicit MF selection after call connect envelope command
10/16/15   sam     Added support to queue Explicit ADF selection
08/10/15   ks      Additional ATR validation before parsing the ATR
07/17/15   sam     Remove code to modufy P2 value of ICC stream select
07/15/15   sam     Stop get response for stream apdu command if the 
                   received data exceeds 64KB to avoid buffer overflow
07/14/15   ks      Incrementing the state pointer before sending APDU cmd
07/14/15   ks      Update stream select case for ICC 
06/30/15   sam     bound checks on offsets that are used to access data in arrays
06/29/15   ks      Checking .is_ch_valid flag before recovering the channel
06/03/15   ks      Select MF after EF-PSLOCI update for a buggy card
05/29/15   hh      Convert MANAGE CHANNEL APDU to MMGSDI requests
05/13/15   ks      Fix to ignore path position in case of UICC cards
05/13/15   ks      Adding silent recovery case during sending error report
05/11/15   hyo     Reset is_ef flag streaming select
04/27/15   vr      Support to check the pending proactive command on card
04/23/15   ssr     Fix for T=1 test card
03/26/15   hyo     Select MF after EF-LOCI update for a buggy card
02/28/15   ks      Explicit MF selection logic revisited
02/10/15   ks      Compilation fix when FEATURE_UIM_BTSAP_CLIENT is enabled
02/02/15   ks      Workaround for buggy cards to select MF on channel 0 after
                   succesful verify PIN operation
01/09/15   akv     Adding power supported indication to the TERMINAL_CAPABILITY
12/12/14   ks      Freeing toolkit command memory upon recovery
12/11/14   ks      Do not re-attempt close channel upon failure in first time
12/09/14   ll      KW_boolean TRUE check
11/17/14   nmb     Remove dead directory maintenance code
11/18/14   ks      Support for off-target
11/17/14   na      Support of le for stream 7816 apdu controlled with NV
11/13/14   ks      Fix issue with BTSAP crash during timeout on ATR
11/12/14   js      Fixed compiler warning
11/10/14   akv     Support for user to customize LDO settings
10/30/14   ks      Fix issue with BTSAP compilation
10/28/14   sam     Attempt ICC powerup if there is an MF select failure in
                   UICC powerup
10/22/14   na      Allow GET RESPONSE with Le if provided in command
10/16/14   nmb     Compare the ICC select RSP to EF to determine if it is an EF
10/01/14   ll      KW errors
09/29/14   ks      Attempting power-up with 3v after few recoveries
09/26/14   ks      Do not re-attempt failed seek command when SFI is used
09/18/14   akv     UIMDRV UT enhancements
09/10/14   ks      Reset the internal poll timer upon external status command
09/06/14   ks      Fixing data overflow issue
08/27/14   sam     reset_atcsim_last_sel_dir upon hotswap
08/25/14   ll      Fix compile warnings medium
08/21/14   na      Validation of aid length check is updated
08/15/14   js      Support for ATR / PPS in card emulator.
08/11/14   ak      Feature flags cleanup
08/06/14   sam     updating current_path in respose of SELECT_FAILED_ST
08/04/14   ll      Fix compile warnings
07/30/14   sam     Fix to send Status command when Apps are reopened during recovery
07/25/14   na      Allowing back to back silent recovery request
07/15/14   lxu     Fallback to iso when receiving RESET_F with ICC protocol
07/14/14   ssr     Allow GET RESPONSE upon warnings for GET STATUS
07/23/14   lxu     Power down the whole USB UICC card when doing UIM_POWER_DOWN_F
07/18/14   ks      Updating the stream apdu handling
07/18/14   ks      AT commands over streaming APDU on default channel fixes
07/15/14   ll      NV refresh upon hotswap feature
07/14/14   js      Update GET RESPONSE CLA byte value for streaming APDU
07/03/14   sam     1) Explicit MF selection after location status envelop when
                   NV 'UIMDRV_FEATURE_ENABLE_EXPLICIT_SELECTION_OF_MF_OR_ADF'is TRUE
                   2) when the NV is true- modify ADF selection after TR
                      for UICC cards to MF selection.
07/04/14   sam     Workaround for buggy cards to select MF on channel 0 on
                   failure of AID selection on non-zero channel
07/03/14   sam     preparing select for MF if the current_path variable is empty
07/03/14   xj      Added UIM_MAX_CLA_TOGGLE_ATTEMPTS to toggle CLA for 2 times
06/13/14   ssr     Removed slot based initialization of logical channel
06/11/14   ll      Add interruptible time delay and return value for update params
05/31/14   nmb     Stop initialization when MF selection fails
05/28/14   akv     Clearing the status words in the rsp buf while performing
                   USB UICC power up and power down.
05/23/14   ks      Enabling software command response timer
05/22/14   nmb     Optimize ICC selects to avoid reselecting from MF
05/22/14   sam     API call to inform QMI of successfull recovery execution
05/19/14   na      Support for select next for partial aid
04/29/14   nmb     Re-enable AT+CRSM after directory maintenance changes
04/29/14   ks      FIX to handle select CDMA DF before TP appropriately
04/28/14   akv     Clearing the usb init timer only upon its expiry or when
                   we have a remote connection available sig from QMI
04/24/14   lxu     Fix wrong WWT of usb uicc given to link_est_data and
                   HOTSWAP_CARD_INS_F issue
04/21/14   lxu     Clear usb related timers properly
04/17/14   ks      Ignoring voltage switch when BTSAP connection is active
04/16/14   akv     Fix pup issue with jcdma service enabled
04/16/14   lxu     Fix compile error on BO target
04/16/14   lxu     Add usb init timer for waiting connection available event
04/15/14   ks      Reverting the changes of CR556030 that introduced sw wwr
04/14/14   lxu     Fix hot-swap failed in slot2 when enable USB UICC for all slots
04/14/14   lxu     Fixes for Hotswap in USB mode when performed in quick succession
04/11/14   lxu     Unregister automatic UICC shutdown on card removal in USB mode
04/11/14   na      Added support for SWP NV item to encode SWP in Terminal Capability
04/11/14   lxu     Power down USB UICC when removed,otherwise power down ICCD
04/09/14   nmb     Set the protocol correctly for correct voltage selection
04/08/14   lxu     Fix ISO card try to turn off USB ICCD interface
04/08/14   lxu     Fix ISO fallback when ISO card inserted
04/06/14   ks      Introducing software wwt running parallel to h/w wwt
04/03/14   ks      Fixing error on poll command after CDMA selection failure
04/03/14   lxu     Power down ISO interface when pps can be skipped for usb uicc
04/02/14   av      NVRUIM headers cleanup
04/02/14   lxu     Send USB PPS request without PPS1 byte
04/01/14   lxu     Use new uim remote power down API to deal with card power
                   down command,set nv 70210 default version to 2
03/31/14   ll      polling with the correct AID length to avoid duplicate poll
03/31/14   lxu     Fix USB UICC recovery, hotswap and iso card issue
03/27/14   lxu     USB UICC modem implement
03/21/14   ll      Remove return_ptr and remove/change uim_free to UIM_FREE
03/20/14   nmb     Directory maintenance for streaming APDUs on default channel
03/19/14   ks      Updating the current channel appropriately
03/06/14   nmb     Correct the current directory for polling
03/03/14   nmb     Directory maintenance I&T bug fixes
02/25/14   na      Support of select next api for non-prov session
02/20/14   ks      Provision for explicit MF or ADF selction
                   And provision for inverse convention bootup
02/20/14   ak      Fix compile errors due to strict compiler on 9x35
01/29/14   sam     Usage of newer style diag macros
02/12/14   ks      Fix to initialize intermediate sw's for envelope command
02/08/14   sam     Fix for klocwork issues
02/07/14   ll      Add debug messages of GPIO configs, GPIO values and ATR
02/07/14   ks      Removal of mmgsdi NV dependency on ICC pup states
02/02/14   nmb     SFI initial engg revision
01/30/14   rm      Fix to correct the logic that wrongly selects CDMA DF
                   before GSM DF in RESET state
01/29/14   akv     RX ISR refactoring changes
01/14/14   nmb     Move MANAGE CHANNEL command to generic cmd processing
01/14/14   nmb     Remove ACCESS_F and related unused functionality
01/13/14   na      Fix for session deactivation after silent recovery
01/09/14   na      Resetting of directory hierarchy after ADF selection
01/07/14   akv     Polling uim only at the expiry of the polling timer
12/21/13   am      Silent Recover Feature Implementation
12/12/13   ks      BTSAP in client mode, intial implementation
12/11/13   js      Fix dont_skip_PPS flag for RESET response
12/11/13   na      skip_PPS is replaced with dont_skip_PPS and made to instance based
11/19/13   nmb     Remove parameter to q_linear_delete compare functions
11/19/13   nmb     Explicitly start from MF or ADF for proprietary files
11/11/2013 ks      Not sending error report for Envelope command and Terminal response
                   when card is removed.
11/1/2013  sam     Capping WI value to default 10, only if the WI value is
                   abnormally large or not set
10/24/13   akv     Removed unnecessary feature flags
10/24/13   ks      secure memory optimization to cater destination buffer overflow
10/22/13   ll      Add UIM BUSY Response feature
10/18/13   ks      FIX poll error due to response file ID mis-match
10/09/13   ssr     Add capping for WI value to prevent unreasonable value
10/08/13   ak      Remove dead code - uimtk.c, uimgnusr.c
10/08/13   ks      Support for clock off for BT SAP
10/08/13   sam     Explicit selection of CDMA_DF before TP
10/03/13   sam     F3 log removal
                   Replaced UIM_FLUSH_FOR_SLOT with UIM_FLUSH_ALL
09/26/13   ak      Remove unnecessary feature flags
09/24/13   nmb     Remove extra uim_free param
09/23/13   nmb     Remove GET_CURR_INSTANCE
09/19/13   akv     Reference CLK changes to support 4.8MHz
09/17/13   ssr     Added support for new recovery notification for GSTK
09/16/13   yk      Removed the code for old targets and for old features
09/16/13   sam     Fix to avoid mismatch of UIM mode (ICC\UICC Mode)
                   in recovery state.
09/13/13   sam     Reporting result for power down command
                   before notifying error to MMGSDI
09/12/13   js      F3 clean up
08/21/13   hyo/js  Hotswap support on MOB
08/14/13   ks      More time proactive command handling
08/13/31   js      UIMDRV Memory De-Couple changes
08/08/13   ks      Advanced Subscription Manager changes
07/29/13   ks      Fix for read file in chunks of 0xFF bytes instead of 256.
07/26/13   rm      Fix for TP response error blocking recovery
07/25/13   na      Fix of Poll Error
07/15/13   ssr     Fixed 62XX status byte for APP selection through
                   QMI_UIM_LOGICAL_CHANNE
07/12/13   nmb     Global Reorganization
07/09/13   js      Fix for extra modem wake up while UE is camped on CDMA.
06/24/13   js      TSTS changes
06/24/13   ks      Removal of F3 messages as part of memory optimization
06/19/13   js      Fixed issue with crash on ICC power up
06/07/13   js      Fix to prevent unintention memset of uim_rsp_buf
05/24/13   abd     Featurize the code to poll in idle mode
04/27/13   na      Secure memory copy
04/17/13   tl      Replace Featurization with NV
03/07/13   na      Clear all rsp buf data before powering up
02/25/13   ms      Fix to not overwrite 61 XX to 91 XX of streaming apdu’s as
                   part of GET RESPONSE processing
02/13/13   js      UIM should cache terminal profile when card returns error
                   for TP.
02/12/13   js      General F3 macro cleanup
02/08/13   js      Updated UIM USB code for parallel processing
02/07/13   na      Handling of extended logical channel add for GET RESPONSE
02/05/13   js      Fix issue where UIM halts initialization for ICCID not
                   present in UICC cards
01/31/13   akv     Uimgen state update upon failed stream apdu
01/28/13   js      Removing UIM Subscription Manager Code
01/24/13   akv     ICCID Cached and sent along with link est data to GSDI
12/18/12   nmb     Relay SW1=63 to client with additional data for case 4 ins
12/18/12   nmb     Clear DF information when setting ADF type
12/18/12   nmb     Don't copy the command AID into the curr_aid if NULL
12/17/12   js      Fixed KW error
12/15/12   js      Set extra BGT time to 10 ms for UART DM for T=1
12/13/12   ms      Select correct session control type during recovery
12/05/12   js      UIM Parallel Processing changes
11/06/12   ms      Reducing the retrial command failure attempts to 2 from 3
                   for non-powering up commands
10/26/12   akv     HW Enumeration changes (without features)
10/26/12   akv     HW Enumeration changes
10/19/12   ssr     Fix Stream APDU ST check for Generic Command only
10/18/12   ssr     Clear the recovery global at recovery ends
10/17/12   ssr     Fix to send the response when same APP is reselected on
                   default channel thru stream APDU.
10/17/12   ssr     Fix potential memory leak for stream APDU
10/16/12   ssr     Fix Power up failure for RESET and HOTSWAP Command when
                   NV_ENS_ENABLED_I is enabled and these command is sent in
                   uim power state due to 6F00 status byte
10/15/12   ms      Correcting instruction case for UIM_STREAM_APDU_ST
09/25/12   rm      WWT fixes over GCF test failures
09/18/12   ms      Fix to send 91XX byte to GSTK for enevelope command
                   if return by the card
09/21/12   ms      Added code for error handling during recovery
08/26/12   akv     Set P3 in Get Resp to 0 only when SW1 warnings are received
                   for streaming APDU
08/13/12   js      Fix for doing setting correct instruction case for stream APDU
08/13/12   js      Fix for supporting proprietary streaming APDU
08/09/12   js/rm   Fixes for UIM controller hotswap and hardware WWT features
08/09/12   ms      Fix to send correct instruction class in Get Response for
                   Stream APDUs
                   Fix to clear channel open information during RESET_F
                   Flushing manage channel commands for ICC recovery
                   Smart recovery for extended logical channels
                   Set the global flag which stores the channels being closed
                   to FALSE, after MMGSDI requests for the closure of a channel
08/07/12   akv     Sending status command after POLLING OFF
07/19/12   ssr     Add check for AID failure before making 0x7FFF app selection
07/24/12   nmb     Fix to reset the technical problem count in RESET_F command
06/07/12   ms      Resetting EF data in case CDMA CHECK fails
05/29/12   nmb     Removed dead code under ABSENT MANDATORY FILES featurization
05/22/12   yb      Robust fix to handle 61 XX chained response
                   Fix to prevent Poll Error after Select via STREAM APDU
05/21/12   nmb     Dual Sim Subscription Manager enablement
05/01/12   av      Replaced FEATURE_UIM_SUPPORT_HOTSWAP with an nvitem
04/26/12   rm      Correction in work waiting time value
03/30/12   av      Replace feature flags with NVITEM
03/30/12   ssr     Empty AID selection support
03/24/12   rm      Hardware work waiting time changes
03/24/12   js      Correct WWT offset for card deactivation
03/22/12   ms      Fix for selecting GSM DF with FEATURE_UIM_AUTH_CDMA_DF_FIRST
                   enabled
03/20/12   js      Fixed compilation error
03/20/12   js      Correct P2 param for MANAGE CHANNEL (Open) and fix
                   error handling in recovery
03/15/12   rm      Changes for DIME
03/12/12   vs      Always request FCP for EF Select
03/08/12   nmb     Notify card of logical channel support
03/07/12   js      Prevent divide by zero condition while computing WWT
03/07/12   js      Correct WWT for T=0 protocol
02/18/12   av      Cleaned up remaining ALWAYS OFF/ON feature flags
02/06/12   av      Removal of Always ON 1x features
02/03/12   js      Reverting UART DM fix
01/24/12   yb      Fixes to save power impact due to delay before and after
                   clock stop in idle mode
01/16/12   nmb     Removal of ALWAYS ON FEATURE_AUTH
11/23/11   ms      Fix for polling happening in less than 28 sec after last command
01/04/12   ssr     Fix to prevent the RESET Command in card removed state
12/30/11   shr     Removed featurization for ALWAYS ON features
12/12/11   ms      Enter into recovery in case of braf_value equal to zero
                   to avoid divide by 0 crash
12/23/11   ssr     Prevent the Status comparison for empty response data
12/21/11   ms      Fix added for Legacy GSDI Removal. Modified card error
                   notification call w.r.t MMGSDI API interface
11/10/11   rm      Fix for a compilation error
11/03/11   ms      Fix to use small stale timeout value for default baud-rate
11/01/11   ssr     Set the channel id to 0x00 while entering recovery within
                   recovery
10/18/11   av      Removed features that are always OFF
10/05/11   yk      Fixed compilation error for win mob
10/04/11   yk      Change deferrable timers to non-deferrable timers
09/09/11   ssr     Fix the RESET command to reset the protocol and voltage
                   setting before UIM power up
08/30/11   yb      Added support for complete recovery solution
                   Fix to avoid toggling instruction class during recovery
                   Fix for no TP after recovery
                   Fixed crash on card removal and inserting new card
                   Added support for caching channel information slot wise
                   Fixed compilation error for non recovery enabled builds
                   Fixed compile error for single slot builds
                   Fix to purge select cache when file is rehabilitated
08/18/11   rm      Fix for timing issues with stale timeouts between rx level
                   interrupts with a fastest card
08/16/11   ssr     Update the me_power_up for reset error
08/16/11   ssr     Added more message
08/11/11   vs      Use case 2 for streaming apdus without input/output
08/11/11   nmb     Expanded Logical Channels
08/09/11   ak      Merge "reset poll timer after Refresh Reset"
08/01/11   vs      Added support for session open with select response
07/08/11   rm      Catch-up with latest on Hot-swap
08/05/11   rm      Fix for the invalid card issue due to a {512, 64} card
06/14/11   nmb     Generalize RPM EF path lookups for RPM DF
04/27/11   yt      Included private GSDI and NVRUIM headers
04/25/11   yb      Adding support for hotswap
04/25/11   nmb     Fixed compilation error
04/25/11   nmb     Added RPM Files for USIM EF path lookups
04/22/11   ps      Fix for a card issue leading to card error during STATUS
                   command
03/08/11   ssr     Fixed UIM poll error
12/16/10   ps      Changes to support hot swap of two cards of different classes
02/22/11   js      Renaming fusion features
01/31/11   ssr     Decoupled FEATURE_UIM_JCDMA_RUIM_SUPPORT feature
12/22/10   yt      Removed FEATURE_UIM_ZI_MEMORY_REDUCTION
11/30/10   yt      Including private GSTK header
11/20/10   ps      Clearing delay timer before setting signal
11/10/10   rm      Fix to correct the instruction class
11/10/10   yt      Memory reduction changes
10/27/10   ms      Changes for C+G enablement
09/29/10   yb      Added support to send aid TLV length as p3 in status
                   command to card
09/28/10   rm      Set clock stop mode to LOW when the SELECT during
                   CHECK_CHARACTERISTICS state fails, to make sure we stop
                   the clock at LOW and vote for sleep
09/24/10   yb      Fixed crash due to UIM sending FETCH after Power Down
09/16/10   ms      Fixes for new DSDS UT Framework
09/10/10   ps      Removing instance of rex_timed_wait with 0 value
09/01/10   ps      Merge UIM server DSDS changes
08/24/10   yb      Removing dependency on clk.h
08/16/10   yb      Added UIM Config NV parameters
08/17/10   yb      Fixed updating sw1 and sw2 for get response
07/18/10   nmb     Added FEATURE_MMGSDI_ABSENT_MANDATORY_FILES_INIT
07/21/10   yb      Fixed compilation error with FEATURE_UIM_UICC disabled
07/14/10   ps      Add a feature to set P3=0 for status command in order to
                   fix an issue due to a faulty card.
07/13/10   sg      Forward proactive command to TAL
07/09/10   ps      Return SW1_P3_BAD for length mismatch for WRITE_REC command
07/01/10   nmb     Updates for JCDMA CSIM support
06/24/10   yt      Klocwork fixes
05/19/10   rm      Fix for divide-by-zero error in a no-card situation
05/11/10   rm      Fix for a 372/12 card to work at default baud-rate
03/24/10   rm      Switch instruction class when card responds with "technical
                   problem" during CHECK CHARACTERISTICS state
03/22/10   kp      Changing the macro from UIM_UICC_XXX to UIM_USIM_XXX
03/12/10   rm      Fix to handle TA4 in ATR for determining clock stop level
03/12/10   rm      Fixes for timing issues with UART_DM
02/18/10   vs      Merged CSIM functionality
02/18/10   nb      Changed FEATURE_UIM_SUPPORT_CSIM to FEATURE_UIM_SUPPORT_ATCSIM
02/17/10   rm      Workaround to send card error when
                   FEATURE_UIM_INTERFACE_NOT_USED is defined
16/02/10   kp      changing the macros from UIM_USIM_XXX to UIM_UICC_XXX
02/15/10   nb      Updated for GSTK Dual Slot Changes
01/18/10   rm      Fixes to reduce the delay in sending R-block
12/24/09   rm      Fixes to correct work-waiting time for UART_DM
10/29/09   yb      Fixed compilation error on disabling FEATURE_UIM_UICC feature
10/08/09   yb      In case of stream apdu:
                   1. Report bad p3 status words to the clients.
                   2. Do not perform resend apdu internally.
09/17/09   rn      Power down confirmation fix
09/26/09   kp      ZI memory reduction changes
09/11/09   rm      Handle TC1 byte in ATR for extra guard time
09/10/09   yb      Fix to select USIM ADF instead of MF when select on file
                   under ADF fails and NV item NV_ENS_ENABLED_I is enabled.
08/18/09   vs      Reverting UIM power down faking logic
08/10/09   rm      Fixes to have separate count for repeated technical problems
08/03/09   yb      Fix to do get response for seek command if a GCF test case
                   is running or NV_ENS_ENABLED_I NV item is enabled.
07/20/09   mib     Replaced flag TEST_FRAMEWORK with FEATURE_UIM_TEST_FRAMEWORK
07/16/09   vs      Added handling for ICCID read failure with MFLO CAS card
07/01/09   yb      1. Fix to do get response for envelope command when card
                   responds with warnings.
                   2. Fix to report warnings to GSTK on receiving warnings in
                   Envelope response.
06/17/09   sk      Fixed Issue with change_chv functionality
06/12/09   yk      Featurized the fixes to support cards that don't have CDMA
                   DF for only 1x builds.
06/10/09   yb      Fix to send status word as received in get response to AT COP
                   even if card returns error for envelope sent.
06/05/09   yb      Fixed handling of class C cards with FEATURE_UIM_USB_UICC on
06/04/09   rm      Fixes to handle technical problem reported by the card
06/01/09   ssr     Fixed compiler warnings
05/08/09   ps      Fixed 8200 compiler warnings
05/03/09   ps      Fixed High Lint error
05/06/09   vs      Fixed state processing of UIM_POWER_DOWN_F
04/17/09   js      Fixed misconfigured featurization
04/17/09   vs      Updated DI computation, faking power down if card is already
                   powered down, forcing T=0 protocol with feature, handling
                   absent DF CDMA
04/15/09   ssr     Fixed limited Length TERMINAL PROFILE for non-spec
                   compliant cards
04/08/09   rm      Fix for a non-compliant card in such a way that the
                   UIM_RESET_F handler is modified to power down and then
                   powerup the UIM interface at 1.8V
04/07/09   js      Support for accessing EF-by-path with proprietary DFs
03/23/09   sun     Virtuim Changes for QTF
03/13/09   vs      Fixed power up issue for MFLO cards without ICCID
03/02/09   sun     Added private header file inclusion
02/18/09   vs      Updated IC-USB logic to execute only if IC-USB is selected
01/29/09   sun     Removed MBMS Featurization
01/29/09   nb      Virtuim support in QTF
01/13/09   ssr     Fix to update record data when proessing UIM_WRITE_REC_CMD_F
12/17/08   nb      Common Modem Interface
12/03/08   js      Support for accessing GSM Cingular DF by path
11/05/08   vs      Update to allow UIM clock stop only after sending the first
                   instruction to the card in passive mode
10/08/08   rm      Cached select support for T=1 protocol
10/07/08   vs      Fix to update record data when proessing UIM_WRITE_REC_CMD_F
10/06/08   jk      Fixed featurization compile error
09/19/08   rm      Fix to set non-cached select for T=1 protocol temporarily
09/17/08   vs      Updated UIM USB ICCD poll delay timer logic
09/17/08   js      Implemented UIM Card interface selection based on first
                   instruction class NV
09/15/08   vs      Removed featurization FEATURE_UIM_USB_UICC_SUSPEND_RESUME
09/12/08   nb      Added support to handle warnings returned by Envelope Command
09/06/08   kk      Fixed Lint Error
09/05/08   vs      Moved fallback timer and signal clearance to UIM USB module
09/05/08   kk      Fixed Featurization Issue
08/26/08   kk      Few comments from uim directory management review
08/20/08   vs      Added support for ATR poll request by USB UICC, ICCD power
                   off when power down cmd is received and ISO fallback
08/14/08   kk      Added handling for SELECT EFs with non-generic file ids
08/08/08   vs      Removed featurization UIM_CLOCK_STOP_ALLOWED
08/08/08   ssr     Changed the order of FEATURE_UIM_RUIM for successful authentication
08/05/08   vs      Renamed start and stop clock functions appropriately
07/25/08   rm      Fix for ICCID read failure
07/17/08   vs      Added support for UIM USB UICC ICCD interface
07/17/08   kk      Added support for PKCS 15
05/28/08   rm      TA1=0x97 feature support.
05/06/08   sk      Do not retry command when "toolkit busy" response is
                   received for an envelope.
04/01/08   vs      Fixed compiler warning
03/25/08   rm      Removed changes which did not allow clock stop to happen
                   on LCU
03/18/08   vs      Fixed Klocwork errors
03/13/08   rm/jar  Disabling the clock stop temporarily in LCU
02/23/08   jar     Changed byte type to unit16 to avoid an infinite loop when
                   a 256 byte non spec conforming Select response is received.
12/17/07   rm      Fix to correct the instruction case in some situations
07/18/07   sp      Featurize uim_free_reset_auth_info()
07/02/07   sun     Added support for ODD INS Class
06/19/03   sp      Don't reselect MF for a select failure in streaming APDU mode
05/03/07   jk      Fixed compile warning
03/29/07   sp      Return no data for a select resp on App termination
03/07/07   sun     Added support for Disable with Replacement
02/13/07   pv      Use uim_set_poll_timer instead of rex_set_timer
01/11/07   tml     Fix compilation issue with 1x
12/11/06   tml     Clear the uim_last_sel_dir for the specific channel and slot
                   upon select request for app termination
11/13/06   pv      Store the protocol to be used for polling from the
                   streamed APDU header if in passive mode and start polling
                   only after the protocol is known.
10/09/06   pv      Allocate memory and assign values to uim_rd_wr_info
                   though not the first attempt.
                   Assign the offset properly for UIM_READ_BIN_CMD_F.
09/28/06   pv      Consider an external reset as a initial power-up rather
                   than internal reset which might perform TP-DL.
                   Do not send a link_established for external reset
                   since an internal power-up would send it out any way.
09/25/06   pv      Use uim_clk_busy_wait instead of clk_busy_wait.
08/18/06   pv      Consider ORANGE DF as level 1 df if df1 type is not an ADF.
08/16/06   sp      Added a check for protocol in pin operation commands
08/02/06   sun     Added case statement for UIM_RESET_SWITCH_UIM_PASSIVE_F in
                   uim_send_generic_err_rpt
07/13/06   pv      Compilation error fixes when FEATURE_UIM_SUPPORT_CSIM IS
                   not defined.
07/07/06   pv      Allow streaming of USIM commands on the default channel.
06/12/06   pv      Get rid of FEATURE_UIM_USIM_INTERNAL_VERIFY_PIN.
                   Notify internal reset to GSDI.
                   Add more debug messages.
                   Do not compare directory structure with stale date when poll
                   fails.
                   Internal reset would not to a TP DN any more.
06/07/06   jar     Terminate the UIM Paths to be selected with UIM_NO_FILE_SEL
                   in UIM_RESET_ST due to the possibility with some cards to
                   have a non 0xFFFF item as the second element of the array.
04/20/06   pv      Featurize out code using cmd in uim_update_last_sel_dir
                   based on FEATURE_UIM_UICC to compile when the feature is
                   not defined
04/03/06   pv      Lint Fixes
03/29/06   pv      Take down the interface and notify gsdi if bad/unknown
                   status words are received while polling if
                   FEATURE_UIM_RPT_BAD_SW_ON_POLL is defined.
                   Add message to print status words when un-known status
                   words are received.
                   Do not consider a select as an intermediate select while
                   in check-char state.
03/09/06   pv      Added additonal NULL check to avoid possible error messages.
                   Also, avoid re-execution of the clear cache code
                   on the item for which the cache was already cleared.
                   initializing select_rsp_cache_item_ptr
03/09/06   pv      Added new command to clear the response cache
                   and Load the status words to the report buffer even if we
                   errored out because of un-known status words
03/01/06   pv      Clear the get response cache on receiving an external RESET
                   command
02/15/06   pv      Select by path from MF if the path starts with 7FFF.
                   Reduce the additional buffer for CWT  and BWT.
                   time out values with additional checks in UIM task to
                   ignore spurious timeout signal.
02/14/06   nk      Merge for: Use heap_small when the size of get response
                   allocation is less than 64 bytes; Remove uim rows from usim
                   path table for enums removed; and Added support to cache the
                   get response data and not do a get response if the item is
                   already cached.  Also, expose a new API to not send down the
                   select for the sake of getting the get response information
02/03/06   pv      Revert the earlier change.
01/30/06   pv      Reduce the additional buffer for CWT  and BWT
                   time out values
01/30/06   pv      Do not perform internal fetch while in passive mode.
                   Notify GSDI about any reason for power down.
01/25/06   pv      Allow select by path from MF during the power-up procedure
                   by moving the setting of the flag from
                   uim_process_generic_command to uim_generic_command.
                   Featurized power down function call for the previous
                   check-in based on DUAL SLOT definition.
01/18/06   pv      Added code to do a cold reset instead of a warm reset
                   when a reset command is received to change the instruction
                   class to GSM.
11/16/05   wli     Changes to support no-iccid card.
11/14/05   pv      Lint Fixes
11/01/05   tml     Combine last sel EF with last sel Dir
11/01/05   jk      Corrected compile error on 7500 (sticter compliler)
10/21/05   pv      Delayed the decision to skip pps from RESET_ST to
                   UIM_DELAY_AFTER_ATR_ST
10/20/05   jk      Added changes to ensure that implicit status words are 0
                   before streaming 7816 apdu is sent
10/19/05   pv      Added code to introduce delay between the reception of the
                   ATR and sending the first byte of the PPS request.
10/06/05   pv      Code to allow select by path from MF for external select
09/13/05   jk      Added changes to keep interface powered down during voltage
                   switch for minimum time specified in ISO7816
09/14/05   wli     Fixed compilation error for feature
                   FEATURE_UIM_USIM_INTERNAL_VERIFY_PIN
08/25/05   pv      Code to notify GSDI upon pdown based on the option.
                   Notify client upon pdown failure.
                   Reset command sets uim_status to un_initialized so that the
                   response could be properly processed
08/15/05   tml     Treated UIM_INCORRECT_PARAM_IN_DATA_FIELD as an indication
                   of wrong instruction class in Check Characteristics state
07/28/05   pv      replaced UIM_MAX_NUM_CDD_ATTEMPTS with UIM_MAX_NUM_ATTEMPTS
                   where not necessary.
07/27/05   jk      Made work waiting time array for dual slots
07/26/05   pv      Check to see if command expects return data in poll
                   rather than checking the vote for traffic channel.
07/22/05   pv      Expect no data in return for poll while not in traffic.
07/18/05   sun     Added support for USIM files in ENS
07/13/05   pv      Fixes for STATUS with NO Data return p2 parameter
06/22/05   pv      changes to handle poll when in passive mode
06/13/05   pv      Code to support UIM commands with allocated memory.
06/08/05   wli     Replaced UIM_MAX_NUM_ATTEMPTS with UIM_MAX_NUM_CMD_ATTEMPTS
                   for command retry.
06/02/05   pv      Code for FEATURE_UIM_ISIM & FEATURE_UIM_SESSION_MANAGEMENT
05/10/05   pv      BT Sim access changes - Handle RST passive mode,
                   PUP and PDOWN
05/10/05   sst     Lint fixes
04/29/05   jk      Changes to set report data len=0 and sw1 and sw2 = 0 when
                   error report being sent for stream 7816 APDU
03/18/05   pv      Changes for Jaguar dual-slot multiplevoltage support
02/22/05   pv      changed uim_first_inst_class based on the protocol
                   supplied when a reset command is queued.
02/03/05   jk      Changed FEATURE_UIM_RUN_TIME_ENABLE to FEATURE_UIM_RUIM
                   around CHECK_FOR_CDMA_DF to allow CDMA to function okay
                   in 1x builds without RUN_TIME_ENABLE
01/28/05   pv      Changed the location where we set the atr_received flag
                   so that we will not set the flag if the ATR is corrupted
01/21/05   wli     Extend work waiting timer value for an out of spec RUIM card.
12/28/04   pv      computed uim_work_waiting_time_sleep_ticks in order to compare
12/27/04   jk      Fixed Compiler warnings.
12/16/04   jk      Merged changes for Orange support from mainline
11/21/04   wli     Dual slot merge and debug.
11/15/04   pv      Added computation for block guard time for T=1 protocol
11/01/04   pv      Change + 20 to CLK_MS_PER_TICK while computing
                   work_waiting time to be able to turn off the interface
                   within 960 ETUs
10/22/04   tml     Select MF immediately after a select failed
10/12/04   jar     Fixed Compile Error
10/11/04   pv      Added code to parse the ATR for voltage class information
09/25/04   jar     Added fix to zero out get resp status words for a
                   UIM_STREAM_ISO7816_APDU_F cmd.
09/23/04   jar     Added fix to keep UIM from an infinite loop due to hack.
09/09/04   ck      Added a hack to update last selected directory structure with
                   second level ORANGE DF which has an ID as first level DF.
09/01/04   ck      Added support for CSIM.
08/17/04   ck      Moved the setting of protocol information to the reset state.
08/16/04   pv      Saved protocol information irrespective of the last
                   selected DF when the protocol is GSM.  Without the change,
                   the protocol is un-known when reporting link_established
08/11/04   ck      Lint changes.
07/14/04   ck      Brought back resetting just EFs when SELECT failed to work
                   with cards that do not maintain their state upon failure
                   to select an EF.
06/16/04   ck      Removed resetting of directories upon a failed SELECT as it
                   could cause polling errors.
05/17/04   ck      Added support for ISO 7816 streaming API.
04/28/04   ts      Added support for > 256 byte writes to transparent files.
04/14/04   ts      Added an extra 2etu delay when turning off the UIM clock.
                   This is needed to compensate for getting the byte before
                   the stop bits.
04/08/04   wli     Featurize for non-gsdi targets.
03/18/04   ts      Back out the previous change because it caused the
                   instruction class switch to fail.
03/15/04   ts      Make sure instruction class is set to USIM on first power
                   up.  This is needed for the second slot if instruction class
                   is changed to GSM in the first slot bring up.
03/13/04   ck      Removed the featurization around gsdi_link_est in response
                   to a successful UIM_RESET_F
                   Merged in hack to toggle the instruction class to GSM for a
                   SIM card that accepted a UICC command but did not return FCP
                   template for the MF select that resulted in Voltage mismatch
02/20/04   ts      Fixed lint errors.
02/18/04   ck      Fixed Lint errors.
02/09/04   ck      Added checks for rec_len before dividing the record length
                   to arrive at the number of records for non transparent file
                   structures.
01/27/04   ck      Added support to send the link est command to GSDI upon
                   completing the UIM_RESET_F command for dual slots.
01/15/04   ck      Added support to reset the elements of the last selected dir
                   structure based on the file ID that was attempted.
01/13/04   ts      Fixed the multi-voltage code because of the multi-slot
                   changes.
01/12/04   ck      Removed duplicate case statements for UTIL commands in
                   the Stream APDU command processing.
12/29/03   ck      Fixed an issue with POLLING_OFF that was not returning the
                   correct value to GSTK in the report.
12/09/03   ck      Deleted resetting of the directories upon receiving an error
                   for Select
                   Set the internal timer after subtracting the Poll offset to
                   meet the deadline.
10/31/03   ts      Set P3 to maximum size for accesses greater than the maximum
                   transaction size.
10/23/03   ck      Merged in changes to support 18etus for certain cards that
                   require higher turnaround delay.
10/28/03   ck      Removed duplicate case for RUN Cave in APDU Stream processing
10/17/03   ck      Added support to stream APDU.
10/13/03   ts      Added processing to set guardtime to minimum for T=1.
10/10/03   ck      Fixed a compile issue with use of the flag reset_with_tp in
                   single slot builds.
10/08/03   ck      Fixed an issue where the toolkit was not switching when
                   the serving system (CDMA /GSM ) changed.
                   - Changed the order of CHECK_FOR_CDMA_DF and TERMINAL_
                     PROFILE_ST in internal wake up state.
09/19/03   ck      Merged in the fixes to pass GCF test cases
                   - Check for invalid Start character TS in ATR
                   - Make sure that PPS is resent twice following a time-out
                     with the third attempt using the default PPS.
09/11/03   ck      Fixed an issue with polling in UICC protocol whent the last
                   selected directory was MF. Also cleaned up polling to parse
                   the information in the response rather than relying on the
                   index for specific information.
09/02/03   ck      Added support to set the CDMA directory present in the nvruim
                   structure for dual slots after changing the preferred slots.
08/22/03   ck      Added a USIM error condition to work with a non-standard GSM
                   card for toggling the instruction class.
08/15/03   ck      Set the toolkit slot in the TP download state since a
                   FETCH could be in response to the download command.
08/15/03   ck      Code review comments checked in.
08/14/03   ck      Added support for dual slots
07/22/03   ts      Change the TA(1) parameters for the PPS to values the MSM
                   can support (due to a slow UART clock).
06/13/03   ck      Added featurization for message that displays the operating
                   protocol.
06/05/03   ck      Fixed an issue where a select by path reference from MF was
                   attempted when the ADF was part of the path causing failure.
06/03/03   ck      Added support to not count the convention change and
                   instruction change as attempts at power up.
05/23/03   ts      Added message to indicate transmission protocol for QXDM
                   messages.
05/01/03   ck      Set the protocol for the FETCH command based on the last
                   successful command.
04/28/03   ts      Added support for multi-Voltage classes.
04/28/03   ts      Added support for T=1 protocol.
04/22/03   wli     Rearranged GSTK/UTK feature.
04/22/03   jk      Added UTIL and WIM support
02/27/03   wli     Store ICCID if command is UIM_RESET_F.
02/19/03   ts      Fixed lint issues from GSTK support code.
02/14/03   ck      Fixed the ATR parsing error that resulted in incorrect
                   work waiting time.
02/13/03   wli     Added support to GSTK.
02/03/03   ck      Set the command count to MAX_NUM_ATTEMPTS to prevent the cmd
                   from being re-tried when a memory failure is recd for SMS
                   writes ( Update record state ) - to satisfy a GCF test case!
01/29/03   ck      Added additional checks for incorrect p1 and p2 before
                   toggling the instruction class in CHECK_CHAR_ST
12/20/02   ts      Fixed Lint issues
11/26/02   ts      Use only max UIM frequency supported.  No change during PPS.
11/21/02   ts      Added a new state to check for the presents of the CDMA DF
                   for RTRE.
11/21/02   ts      Use default PPS for TA1 values that are invalid.
10/29/02   ck      Removed setting of count to UIM_MAX_NUM_ATTEMPTS in the
                   CHECK_CHAR_ST. Added a flag to indicate that ATR has been
                   recd that can be used in the task to set the count.
                   Set the uim_atr_flag to TRUE upon receiving an ATR.
10/25/02   jar     Don't set cmd_ptr->hdr.cmd_count=UIM_MAX_NUM_ATTEMPTS
                   in UIM_CHECK_CHARACTERISTICS_ST to allow toggling - SIM/USIM
10/24/02   ts      Added support for work waiting time
10/24/02   ts      Only allow one attempt for power up after ATR.
10/24/02   ts      Do not set or clear response timeout.  Timeout ishandled in
                   the driver.
10/24/02   ts
10/14/02  ck/jar   Fixed Verify PIN Issue for USIMs.
10/11/02   ck      Added support to make use of the NV item first instn class
10/02/02   ts      Reject SIMs that request a guardtime between 0 and 255.
09/20/02   ts      Use a default max UIM clock frequency of 4MHz since we are
                   operating at 3V.  per spec.
09/19/02   ck      Added checks for UIM_UNKNOWN_INS before switching instrn
                   class
09/13/02   wli     Added feature to verify CHV2 after wake up.
08/12/02   ck      Added support for File selection by path reference.
07/28/02   sb      Merged changes made in rev1.20.1.0 made by pmarsh:
05/22/02   plm     Replaced reference to T_KOVU, T_MSM5200 and T_MSM6200 with
                   T_UIM_CLK_MAX_FREQ defined in targetXXXX.h.
07/18/02   wl      Verify pin after a ADF has been selected.
07/09/02   ck      Added support for AID selection by file ID.
05/15/02   ts      Fixed #ifdef's for using tcxo/4 for MSM5200, Kovu and
                   MSM6200.
05/09/02   ts      Included MSM5200, Kovu and MSM6200 to targets that can handle
                   a UIM clock of tcxo/4.
04/30/01   ck      Modified the Enhanced search to use the search offset in the
                   command while building the APDU.
04/09/02   ck      Modified the featuization around clock stop functions to
                   include FEATURE_UIM_USIM.
03/27/02   ck      Modified the Power up procedure to detect the protocol of the
                   card based on the protocol class in the first command sent to
                   the card.
                   and removed detection based on the historical characters.
02/01/02   ts      Process the file characteristics from CDMA or GSM to determine
                   if the card supports 3V technology.
01/22/02   ts      Added processing of file characteristics from CDMA or GSM DF
                   or MF. This is used to determine clock stop preferences from
                   the card.
01/08/02   ck      Fixed an issue where the num bytes in an access command was
                   compared with only the low word of the EF file size that
                   resulted in an incorrect PRL being read when the PRL was
                   greater that 255 bytes.
12/07/01   ck      Added caching of Terminal Profile data to send down the
                   TERMINAL PROFILE command after a RESET.
                   Added setting of instruction class for Verify CHV1 command.
                   Added RESET command processing.
12/04/01   ts      Added support for UIM toolkit.
11/19/01   ts      Fixed USIM detection for USIM/GSM code.
10/19/01   ck      Fixed the problem where VERIFY_CHV report was being sent for
                   a CHANGE CHV command.
10/11/01   ck      Added support to change the intruction class of SELECT command
                   while attempting to read the ICCID, when the historical chars
                   do not comply with ISO specs ( carrier specific hist chars in
                   a SIM Card ).
07/27/01   ts      Changed the operational parameters to use the parameters sent in
                   the ATR or PPS.
07/26/01   wli     Changed several "switch" statements to direct assignment. This
                   is to simplify the code and save rom space.
06/26/01   ts      Changed clock control feature switch names to have "FEATURE_"
                   prepended.  Changed clock control names to indicate the tcxo
                   relationship as opposed to the actual clock frequency.
06/12/01   ts      Fixed the PPS determination.  Code will now skip the PPS if
                   TA1 is not received.
05/28/01   ck      Added support for multi mode server.
01/16/01   ts      Created module.  This module contains the generic state
                   machine.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "intconv.h"
#include "target.h"
#include "customer.h"
#include "uimdrv_msg.h"
#include "err.h"
#include "task.h"
#include "uimdrv.h"
#include "uimi.h"
#include "uimgen.h"
#include <memory.h>
#include "uimutil.h"
#include "mmgsdilib_p.h"
#include "nvruim_p.h"
#include "uimglobals.h"
#include "uimsub_manager.h"
#include "mmgsdi_nv.h"
#include "uim_nvcfg.h"
#include "uim_hotswap.h"
#include "qmi_uim.h"
#include "lpalib.h"

#ifdef FEATURE_GSTK
#include "gstk_exp.h"
#include "gstk_p.h"
#endif /* FEATURE_GSTK */

#ifdef FEATURE_UIM_REMOTE_UIM
#include "uim_remote.h"
#endif /* FEATURE_UIM_REMOTE_UIM */

#include "uimdrv_main.h"
#include "uimdrv_pmic.h"

#if defined( FEATURE_UIM_T_1_SUPPORT )
#include "uimt1.h"  /* T=1 protocol support */
#endif /* FEATURE_UIM_T_1_SUPPORT */

#ifdef FEATURE_UIM_USB_UICC
#include "uimusb.h"
#endif /* FEATURE_UIM_USB_UICC */
/*===========================================================================

                Lint message inhibitions

===========================================================================*/
/*lint -esym(551, uim_toolkit_slot)
  uim_toolkit_slot is accessed in UIM
*/

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* This defines the amount of time to delay after receiving the ATR and
 * before sending out a PPS request to the CARD
 */
#define UIM_DELAY_TIME_AFTER_ATR    1

/* This defines the delay after powering the UIM and after PPS. */
#define UIM_CARD_DELAY_TIMEOUT 20
#define BGT_22_ETU             22

/* Milliseconds correction */
#define UIM_CLK_MS_PER_TICK 1

/* Macros for F=372 and D=12 baud-rate representation in TA1 byte */
#define UIM_F372_D12_COMB1     0x18
#define UIM_F372_D12_COMB2     0x08

/* This defines the size of the historical characters for buggy cards that need
     extra delay before the response */
  #define UIM_HIST_CHAR_CMP_SIZE 9
  static byte const UIM_MORE_DELAY_HIST_CHARS[] =
  {0x4D, 0x4D, 0x41, 0x52, 0xF2, 0x2B, 0x56, 0x32, 0x2E};
  #define UIM_MIN_MINOR_REV_MORE_DELAY '3'
  #define UIM_MAX_MINOR_REV_MORE_DELAY '4'

/*
  This is a conversion table that returns the number of bytes that follow the
  format byte.  Only the "presents of interface bytes" field is used in the
  index.  The table contains the number of bits set in the index.
 */
const byte uim_y_format_table[16] = {
/* 0    1    2    3    4    5    6    7                                      */
   0,   1,   1,   2,   1,   2,   2,   3,
/* 8    9   10   11   12   13   14    15                                     */
   1,   2,   2,   3,   2,   3,   3,    4
};

/* Bits set indicating SFI is used */
#define SFI_USED_IN_P1         0x80
#define SFI_USED_IN_P2         0xF8

/* Bits indicating Select parent or child or by path from current DF */
#define UN_SUPPORTED_SEL       0x01

/* Constants for processing RPM EFS under DF 0x5F50*/
#define UIM_RPM_DF_ID 0x5F00

/* Extra BGT time (in milli seconds) due to some unknown timing issues */
#define UIM_EXTRA_BGT_TIME    10

/* State configuration for the commands. */
/*  UIM_INTERNAL_ME_PUP_F    = 0x0100, POWER UP UIM due to task start up */
static const uim_generic_state_type UIM_INTERNAL_PUP_STATES[] =
{ UIM_POWER_UP_ST, UIM_RESET_ST,
  UIM_DELAY_AFTER_ATR_ST, UIM_PPS_ST, UIM_UPDATE_OP_PARAMS_ST,
#if defined( FEATURE_UIM_T_1_SUPPORT )
  UIM_IFS_NEG_ST,
#endif /* FEATURE_UIM_T_1_SUPPORT */
  UIM_SEND_STATUS_ST,
  UIM_CHECK_CHARACTERISTICS_ST,
  UIM_TERMINAL_CAPABILITY_ST,
  UIM_SELECT_ICCID_ST, UIM_READ_ICCID_ST,
  UIM_CHECK_FOR_CDMA_DF,
  UIM_DONE_ST };

#ifdef FEATURE_UIM_BTSAP_CLIENT
/*  UIM_BTSAP_CLIENT_PUP_F = 0x0100, POWER UP UIM due to BTSAP client connection request */
static const uim_generic_state_type UIM_BTSAP_CLIENT_PUP_STATES[] =
{ UIM_POWER_UP_ST, UIM_RESET_ST,
  UIM_SEND_STATUS_ST,
  UIM_CHECK_CHARACTERISTICS_ST,
  UIM_TERMINAL_CAPABILITY_ST,
  UIM_SELECT_ICCID_ST, UIM_READ_ICCID_ST,
  UIM_CHECK_FOR_CDMA_DF,
  UIM_DONE_ST };
#endif

/*  UIM_INTERNAL_WAKE_UP_F,         Power up due to Power management */
static const uim_generic_state_type UIM_INTERNAL_WAKE_UP_STATES[] =
{ UIM_POWER_UP_ST, UIM_RESET_ST,
  UIM_DELAY_AFTER_ATR_ST, UIM_PPS_ST, UIM_UPDATE_OP_PARAMS_ST,
#if defined( FEATURE_UIM_T_1_SUPPORT )
  UIM_IFS_NEG_ST,
#endif /* FEATURE_UIM_T_1_SUPPORT */
  UIM_SEND_STATUS_ST,
  UIM_CHECK_CHARACTERISTICS_ST,
  UIM_TERMINAL_CAPABILITY_ST,
  UIM_SELECT_ICCID_ST, UIM_READ_ICCID_ST,
  UIM_DONE_ST
};

#ifdef FEATURE_UIM_BTSAP_CLIENT
/*  UIM_BTSAP_INTERNAL_WAKE_UP_F,   Power up due to recovery/Power management */
static const uim_generic_state_type UIM_BTSAP_INTERNAL_WAKE_UP_STATES[] =
{ UIM_POWER_DOWN_ST,UIM_POWER_UP_ST, UIM_RESET_ST,
  UIM_SEND_STATUS_ST,
  UIM_CHECK_CHARACTERISTICS_ST,
  UIM_TERMINAL_CAPABILITY_ST,
  UIM_SELECT_ICCID_ST, UIM_READ_ICCID_ST,
  UIM_DONE_ST
};
#endif

/*  UIM_RESET_SWITCH_UIM_PASSIVE_F,         Power up making UIM passive */
static const uim_generic_state_type UIM_RST_SWITCH_UIM_PASSIVE_STATES[] =
{ UIM_POWER_UP_ST, UIM_RESET_ST,
  UIM_DELAY_AFTER_ATR_ST, UIM_PPS_ST, UIM_UPDATE_OP_PARAMS_ST,
#if defined( FEATURE_UIM_T_1_SUPPORT )
  UIM_IFS_NEG_ST,
#endif /* FEATURE_UIM_T_1_SUPPORT */
  UIM_DONE_ST
};

/*  Additional states to recover ICC card */
static const uim_generic_state_type UIM_RECOVER_ICC_SLOT_STATES[] =
{
  UIM_RECOVERY_FLUSH_MANAGE_CHANNEL_ST,
  UIM_CHECK_FOR_CDMA_DF,
  UIM_RECOVERY_VERIFY_CHV1_ST,
  UIM_RECOVERY_VERIFY_CHV2_ST,
  UIM_RECOVERY_PERFORM_TP_ST,
  UIM_RECOVERY_CHECK_REHAB_REQD_ST,
  UIM_SELECT_ST,
  UIM_RECOVERY_REHAB_ST,
  UIM_RECOVERY_CHECK_EF_TST_UPDATE_REQD_ST,
  UIM_RECOVERY_SELECT_EF_TST_ST,
  UIM_RECOVERY_WRITE_EF_TST_ST,
  UIM_DONE_ST
};

/* Additional states to recover UICC card */
static const uim_generic_state_type UIM_RECOVER_UICC_SLOT_STATES[] =
{
  UIM_RECOVERY_PERFORM_TP_ST,
  UIM_RECOVERY_MANAGE_CHANNEL_FOR_UICC_ST,
  UIM_RECOVERY_CLOSE_CHANNEL_FOR_UICC_ST,
  UIM_RECOVERY_CHECK_FOR_SELECT_AID_ST,
  UIM_SELECT_ST,
  UIM_RECOVERY_VERIFY_CHV1_ST,
  UIM_RECOVERY_VERIFY_CHV2_ST,
  UIM_RECOVERY_SEND_STATUS_COMMAND_ST,
  UIM_RECOVERY_CHECK_EF_TST_UPDATE_REQD_ST,
  UIM_RECOVERY_SELECT_EF_TST_ST,
  UIM_RECOVERY_WRITE_EF_TST_ST,
  UIM_RECOVERY_VERIFY_UPIN_FOR_UICC_ST ,
  UIM_DONE_ST
};

#ifdef FEATURE_UIM_USB_UICC
/* Array of states to traverse during power up of USB UICC
   due to task start up */
static const uim_generic_state_type UIM_USB_INTERNAL_PUP_STATES[] =
{ UIM_POWER_UP_ST,
  UIM_RESET_ST,
  UIM_DELAY_AFTER_ATR_ST,
  UIM_PPS_ST,
  UIM_USB_POWER_ON_ST,
  UIM_CHECK_CHARACTERISTICS_ST,
  UIM_TERMINAL_CAPABILITY_ST,
  UIM_SELECT_ICCID_ST,
  UIM_READ_ICCID_ST,
  UIM_CHECK_FOR_CDMA_DF,
  UIM_DONE_ST
 };

/* Array of states to traverse during power up of USB UICC
   due to power management */
static const uim_generic_state_type UIM_USB_INTERNAL_WAKE_UP_STATES[] =
{ UIM_USB_POWER_OFF_ST,
  UIM_USB_POWER_ON_ST,
  UIM_CHECK_CHARACTERISTICS_ST,
  UIM_TERMINAL_CAPABILITY_ST,
  UIM_SELECT_ICCID_ST,
  UIM_READ_ICCID_ST,
  UIM_VERIFY_CHV1_ST,
  UIM_DONE_ST
};

/*  Array of states to traverse during power up of USB UICC
    making UIM passive */
static const uim_generic_state_type UIM_USB_RST_SWITCH_UIM_PASSIVE_STATES[] =
{ UIM_USB_POWER_OFF_ST, UIM_USB_POWER_ON_ST,
  UIM_DONE_ST
};

static const uim_generic_state_type UIM_USB_INTERNAL_RESET_TO_ICC_STATES[] =
{ UIM_USB_POWER_OFF_ST,UIM_POWER_UP_ST, UIM_RESET_ST,
  UIM_DELAY_AFTER_ATR_ST, UIM_PPS_ST, UIM_UPDATE_OP_PARAMS_ST,
#if defined( FEATURE_UIM_T_1_SUPPORT )
  UIM_IFS_NEG_ST,
#endif /* FEATURE_UIM_T_1_SUPPORT */
  UIM_SEND_STATUS_ST,
  UIM_CHECK_CHARACTERISTICS_ST,
  UIM_TERMINAL_CAPABILITY_ST,
  UIM_SELECT_ICCID_ST, UIM_READ_ICCID_ST,
  UIM_CHECK_FOR_CDMA_DF,
  UIM_DONE_ST };
#endif /* FEATURE_UIM_USB_UICC */

/* Buffer to hold ICCID used in GCF test cases */
static byte const UIM_TEST_ICCID[UIM_ICCID_SIZE] = {0x98, 0x94, 0x00, 0x20, 0x20, 0x41, 0x00, 0x00, 0x40, 0xF5 };

/*===========================================================================
              F U N C T I O N   P R O T O T Y P E S
===========================================================================*/

static boolean uim_alloc_set_rd_wr_info
(
  word                   num_bytes_to_alloc,
  uim_instance_global_type *uim_ptr
);

static void uim_set_uicc_pin_param_p2
(
  uim_chv_type chv,
  byte *p2,
  uim_instance_global_type *uim_ptr
);

static void uim_parse_uicc_select_response
(
  uim_instance_global_type *uim_ptr,
  byte const            *data,
  word                   num_bytes,
  boolean                  *ef_rsp_ptr,
  byte                  *file_char,
  uint8                    *supported_cmd_present
);

static void uim_parse_uicc_status_response
(
  byte const               *data,
  word                      num_bytes,
  uim_dir_type             *file_id,
  uim_aid_type             *aid,
  uim_instance_global_type *uim_ptr
);

static void uim_free_reset_rd_wr_info
(
  uim_instance_global_type *uim_ptr
);

static int uim_compare_toolkit_cmds(void *item_ptr,void *param);

static void uim_remove_toolkit_cmds(void *item_ptr,void *param);

static int uim_compare_manage_ch_cmd(void *item_ptr,void *param);

static void uim_remove_manage_ch_cmd(
 void                   *item_ptr,
 void                   *param
);

void uim_clear_recovery_params
(
  uim_instance_global_type *uim_ptr
);

void uim_save_current_req_buf(uim_instance_global_type *uim_ptr);

void uim_restore_current_req_buf(uim_instance_global_type *uim_ptr);

void uim_silent_recovery_cb
(
  uim_rpt_type *report_ptr
);

/*===========================================================================*/

/* Releases the memory if allocated to alloc_mem member and resets all
 * other variables holding additional info about read/write
 */
static void uim_free_reset_rd_wr_info
(
  uim_instance_global_type *uim_ptr
)
{
  if ( uim_ptr->command.rd_wr_info.alloc_mem != NULL )
  {
    UIM_FREE(uim_ptr->command.rd_wr_info.alloc_mem);
  }

  uim_ptr->command.rd_wr_info.data_ptr      = NULL;
  uim_ptr->command.rd_wr_info.num_bytes_rsp = 0;
  uim_ptr->command.rd_wr_info.num_bytes     = 0;
  uim_ptr->command.rd_wr_info.offset        = 0;

}/* uim_free_reset_rd_wr_info */


/* Resets all the static variables holding additional read/write information,
 * allocates memory, assigns the allocated memory to alloc_mem and
 * data_ptr
 */
static boolean uim_alloc_set_rd_wr_info
(
  word                      num_bytes_to_alloc,
  uim_instance_global_type *uim_ptr
)
{
  uim_free_reset_rd_wr_info(uim_ptr);

  if (num_bytes_to_alloc == 0 )
  {
    return FALSE;
  }

  uim_ptr->command.rd_wr_info.alloc_mem =
    uim_malloc(num_bytes_to_alloc);

  uim_ptr->command.rd_wr_info.data_ptr =
    uim_ptr->command.rd_wr_info.alloc_mem;

  uim_ptr->command.rd_wr_info.num_bytes =
    num_bytes_to_alloc;

  return ( uim_ptr->command.rd_wr_info.alloc_mem != NULL );
}/* uim_alloc_set_rd_wr_info */


/*===========================================================================

FUNCTION UIM_SET_UICC_PIN_PARAM_P2

DESCRIPTION
  This procedure sets the parameter p2 in the apdu header based on the USIM
  PIN.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if a valid PIN type is recd. FALSE otherwise.

SIDE EFFECTS
  Fills the p2 in the apdu header.
===========================================================================*/

static void uim_set_uicc_pin_param_p2
(
  uim_chv_type chv,
  byte *p2,
  uim_instance_global_type *uim_ptr
)
{
  if (p2 == NULL)
  {
    return;
  }
  switch (chv)
  {
    case UIM_PIN1_APP1:
       *p2 = UIM_PIN1_APP1_VALUE;
    break;

    case UIM_PIN1_APP2:
       *p2 = UIM_PIN1_APP2_VALUE;
    break;

    case UIM_PIN1_APP3:
       *p2 = UIM_PIN1_APP3_VALUE;
    break;

    case UIM_PIN1_APP4:
       *p2 = UIM_PIN1_APP4_VALUE;
    break;

    case UIM_PIN1_APP5:
       *p2 = UIM_PIN1_APP5_VALUE;
    break;

    case UIM_PIN1_APP6:
       *p2 = UIM_PIN1_APP6_VALUE;
    break;

    case UIM_PIN1_APP7:
       *p2 = UIM_PIN1_APP7_VALUE;
    break;

    case UIM_PIN1_APP8:
       *p2 = UIM_PIN1_APP8_VALUE;
    break;

    case UIM_PIN2_APP1:
       *p2 = UIM_PIN2_APP1_VALUE;
    break;

    case UIM_PIN2_APP2:
       *p2 = UIM_PIN2_APP2_VALUE;
    break;

    case UIM_PIN2_APP3:
       *p2 = UIM_PIN2_APP3_VALUE;
    break;

    case UIM_PIN2_APP4:
       *p2 = UIM_PIN2_APP4_VALUE;
    break;

    case UIM_PIN2_APP5:
       *p2 = UIM_PIN2_APP5_VALUE;
    break;

    case UIM_PIN2_APP6:
       *p2 = UIM_PIN2_APP6_VALUE;
    break;

    case UIM_PIN2_APP7:
       *p2 = UIM_PIN2_APP7_VALUE;
    break;

    case UIM_PIN2_APP8:
       *p2 = UIM_PIN2_APP8_VALUE;
    break;

    case UIM_UNIVERSAL_PIN:
       *p2 = UIM_UNIVERSAL_PIN_VALUE;
    break;

    default:
       *p2 = 0;
       UIMDRV_MSG_ERR_1(uim_ptr->id,"Incorrect chv_type 0x%x", chv);
    break;
  }
} /* uim_set_uicc_pin_param_p2 */


/*===========================================================================

FUNCTION UIM_SEND_GENERIC_ERR_RPT

DESCRIPTION
  This function sends an error report for a generic command that is being
  requested.  We pass cmd_ptr explicitly here since we may be sending err
  report for am unprocessed command that was present in command queue while
  processing hotswap card removal or card error.

DEPENDENCIES
  This function queues an error report onto a client queue.

RETURN VALUE
  None

SIDE EFFECTS
  Releases the command buffer.

===========================================================================*/
void uim_send_generic_err_rpt
(
  uim_cmd_type             *cmd_ptr,
  uim_instance_global_type *uim_ptr
)
{
  if (cmd_ptr == NULL)
  {
    return;
  }

  /* Copy user data from received command to report buffer */
  uim_ptr->command.rpt_buf.user_data = cmd_ptr->hdr.user_data;

  /* We will reset the report options flag in the command to NONE for
     backward compatibility purposes */

  cmd_ptr->hdr.options = UIM_OPTION_NONE;

  /* Report back the status as UIM_FAIL indicating failure in the R-UIM */
  uim_ptr->command.rpt_buf.rpt_status = UIM_FAIL;

  UIMDRV_MSG_HIGH_1(uim_ptr->id, "Reporting an error for generic command 0x%x",
                      cmd_ptr->hdr.command );
  /* Check if the client task has supplied a report back function. */
  if (cmd_ptr->hdr.rpt_function != NULL)
  {
    
    /* Process the error report based on the command that failed. */
    switch(cmd_ptr->hdr.command)
    {
      case UIM_READ_REC_CMD_F:
      case UIM_READ_BIN_CMD_F:
        {
          if (cmd_ptr->hdr.command == UIM_READ_REC_CMD_F)
          {
            uim_ptr->command.rpt_buf.rpt_type = UIM_READ_REC_CMD_R;
          }
          else
          {
            uim_ptr->command.rpt_buf.rpt_type = UIM_READ_BIN_CMD_R;
          }

          uim_ptr->command.rpt_buf.rpt.read.num_bytes_read =
            uim_ptr->command.rd_wr_info.num_bytes_rsp;

          uim_ptr->command.rpt_buf.rpt.read.data =
            uim_ptr->command.rd_wr_info.alloc_mem;

          /* Fill in necessary fields of command block */
          uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );

          uim_free_reset_rd_wr_info(uim_ptr);
        }
        break;

      case UIM_WRITE_BIN_CMD_F:
      case UIM_WRITE_REC_CMD_F:
        {
          if (cmd_ptr->hdr.command == UIM_WRITE_BIN_CMD_F)
          {
            uim_ptr->command.rpt_buf.rpt_type = UIM_WRITE_BIN_CMD_R;
          }
          else
          {
            uim_ptr->command.rpt_buf.rpt_type = UIM_WRITE_REC_CMD_R;
          }

          uim_ptr->command.rpt_buf.rpt.write.num_bytes_written =
            uim_ptr->command.rd_wr_info.num_bytes_rsp;

          /* Fill in necessary fields of command block */
          uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );

          /* Clear all the static variables */
          uim_free_reset_rd_wr_info(uim_ptr);
        }
        break;

      case UIM_INC_CMD_F:
        {
          uim_ptr->command.rpt_buf.rpt_type = UIM_INC_CMD_R;

          uim_ptr->command.rpt_buf.rpt.inc.inc_rsp_len =
            uim_ptr->command.rd_wr_info.num_bytes_rsp;

          uim_ptr->command.rpt_buf.rpt.inc.data =
            uim_ptr->command.rd_wr_info.alloc_mem;

          /* Fill in necessary fields of command block */
          uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );

          uim_free_reset_rd_wr_info(uim_ptr);
        }
        break;

      case UIM_UNBLOCK_CHV_F:
        {
          uim_ptr->command.rpt_buf.rpt_type = UIM_UNBLOCK_CHV_R;

          /* Fill in necessary fields of command block */
          uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
        } /* end case - UIM_UNBLOCK_CHV_F */
        break;

      case UIM_STATUS_F:
        {
          uim_ptr->command.rpt_buf.rpt_type = UIM_STATUS_R;

          uim_ptr->command.rpt_buf.rpt.status.num_bytes = 0;

          /* Fill in necessary fields of command block */
          uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
        } /* end case - UIM_STATUS_F */
        break;

      case UIM_INTERNAL_ME_PUP_F:
      case UIM_INTERNAL_POLL_F:
          /* do nothing */
        break;
      case UIM_INTERNAL_WAKE_UP_F:        /* Power up due to Power management */
        uim_clear_recovery_params(uim_ptr);
        break;

      case UIM_SELECT_F:
        {
          /* Reset the flag */
          uim_ptr->command.rpt_buf.rpt_type                   = UIM_SELECT_R;
          uim_ptr->command.rpt_buf.rpt.select.num_data_bytes  = 0;
          /* Fill in necessary fields of command block */
          uim_ptr->command.rpt_buf.rpt_hdr.task_ptr           = NULL;
          uim_ptr->command.rpt_buf.rpt.select.get_resp_sw1    = (uim_sw1_type) 0;
          uim_ptr->command.rpt_buf.rpt.select.get_resp_sw2    = (uim_sw2_type) 0;

          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );          
        } /* end case - UIM_SELECT_F */
        break;

      case UIM_INVALIDATE_F:
        {
          uim_ptr->command.rpt_buf.rpt_type = UIM_INVALIDATE_R;

          /* Fill in necessary fields of command block */
          uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
        } /* end case - UIM_INVALIDATE_F */
        break;

      case UIM_REHABILITATE_F:
        {
          uim_ptr->command.rpt_buf.rpt_type = UIM_REHABILITATE_R;

          /* Fill in necessary fields of command block */
          uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
        } /* end case - UIM_REHABILITATE_F */
        break;

      case UIM_VERIFY_CHV_F:
        {
          uim_ptr->command.rpt_buf.rpt_type = UIM_VERIFY_CHV_R;

          /* Fill in necessary fields of command block */
          uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
        } /* end case - UIM_VERIFY_CHV_F */
        break;

      case UIM_CHANGE_CHV_F:
        {
          uim_ptr->command.rpt_buf.rpt_type = UIM_CHANGE_CHV_R;

          /* Fill in necessary fields of command block */
          uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
        } /* end case - UIM_CHANGE_CHV_F */
        break;

      case UIM_DISABLE_CHV_F:
        {
          uim_ptr->command.rpt_buf.rpt_type = UIM_DISABLE_CHV_R;

          /* Fill in necessary fields of command block */
          uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
        } /* end case - UIM_DISABLE_CHV_F */
        break;

      case UIM_ENABLE_CHV_F:
        {
          uim_ptr->command.rpt_buf.rpt_type = UIM_ENABLE_CHV_R;

          /* Fill in necessary fields of command block */
          uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
        } /* end case - UIM_ENABLE_CHV_F */
        break;

      case UIM_SEEK_F:
        {
          uim_ptr->command.rpt_buf.rpt_type = UIM_SEEK_R;

          uim_ptr->command.rpt_buf.rpt.seek.num_records = 0;

          /* Fill in necessary fields of command block */
          uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
        } /* end case - UIM_SEEK_F */
        break;

      case UIM_TERMINAL_PROFILE_F:        /* Send a terminal profile */
      {
        uim_ptr->command.rpt_buf.rpt_type = UIM_TERMINAL_PROFILE_R;

        /* Fill in necessary fields of command block */
        uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

        /* Report to requesting task */
        (*(cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
		
        if(uim_ptr->command.cmd_ptr->hdr.protocol == UIM_UICC)
        {
          uim_queue_internal_open_channel_command(uim_ptr);
        }
      } /* end case - UIM_TERMINAL_PROFILE_F */
      break;

      case UIM_ENVELOPE_F:                /* Send an envelope command */
      {
        uim_ptr->command.rpt_buf.rpt_type = UIM_ENVELOPE_R;

        /* Fill in necessary fields of command block */
        uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;
        if (uim_ptr->hotswap.card_status != UIM_CARD_REMOVED)
        {
          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
        }
        else
        {
          UIMDRV_MSG_HIGH_0(uim_ptr->id,"UIM not reporting error for ENVELOPE command as card is removed");
        }
      } /* end case - UIM_ENVELOPE_F */
      break;

      case UIM_INTERNAL_FETCH_F:          /* Fetch a proactive command */
      {
        /* If there is an error with fetching a proactive command, it
           indicates there is a bug in the code or in the card.  Do nothing */

      } /* end case - UIM_INTERNAL_FETCH_F */
      break;

      case UIM_TERMINAL_RESPONSE_F:       /* Send a proactive cmd response */
      {
        uim_ptr->command.rpt_buf.rpt_type = UIM_TERMINAL_RESPONSE_R;

        /* Fill in necessary fields of command block */
        uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;
        if (uim_ptr->hotswap.card_status != UIM_CARD_REMOVED)
        {/* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
        }
        else
        {
          UIMDRV_MSG_HIGH_0(uim_ptr->id,"UIM not reporting error for TERMINAL_RESPONSE as card is removed");
        }
      } /* end case - UIM_TERMINAL_RESPONSE_F */
      break;

      case UIM_POLLING_INTERVAL_F:        /* Polling interval or polling off */
        {
          uim_ptr->command.rpt_buf.rpt_type = UIM_POLLING_INTERVAL_R;

          /* Fill in necessary fields of command block */
          uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
        } /* end case - UIM_POLLING_INTERVAL_F */
        break;

      case UIM_HOTSWAP_CARD_INS_F:
      case UIM_RESET_F:                 /* Perform a warm reset and perform
                                           power up */
        {
          uim_ptr->command.rpt_buf.rpt_type = UIM_RESET_R;

          /* Fill in necessary fields of command block */
          uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;
          uim_ptr->flag.me_powerup = FALSE;
          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
        } /* end case - UIM_RESET_F */
        break;

      case UIM_STREAM_APDU_F:            /* Stream APDU to the card */
        {
          uim_ptr->command.rpt_buf.rpt_type = UIM_STREAM_APDU_R;

          /* Fill in necessary fields of command block */
          uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
        } /* end case - UIM_STREAM_APDU_F */
        break;


      case UIM_STREAM_ISO7816_APDU_F: /* Stream an ISO7816 APDU to the card */
        {
          uim_ptr->command.rpt_buf.rpt_type = UIM_STREAM_ISO7816_APDU_R;

          /* Fill in necessary fields of command block */
          uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;
          uim_ptr->command.rpt_buf.rpt.stream_iso7816_apdu.num_data_bytes =
            uim_ptr->command.rd_wr_info.num_bytes_rsp;

          uim_ptr->command.rpt_buf.rpt.stream_iso7816_apdu.get_resp_sw1 = (uim_sw1_type) 0;
          uim_ptr->command.rpt_buf.rpt.stream_iso7816_apdu.get_resp_sw2 = (uim_sw2_type) 0;

          uim_ptr->command.rpt_buf.rpt.stream_iso7816_apdu.data_ptr =
            uim_ptr->command.rd_wr_info.alloc_mem;

          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
          if (uim_ptr->command.rd_wr_info.num_bytes_rsp > 0)
          {
            uim_free_reset_rd_wr_info(uim_ptr);
          }

          uim_ptr->command.rpt_buf.rpt.stream_iso7816_apdu.data_ptr = NULL;
          uim_ptr->command.generic_state_ptr++;

        } /* end case - UIM_STREAM_ISO7816_APDU_F */
        break;
      case UIM_HOTSWAP_CARD_REM_F:
      case UIM_POWER_DOWN_F:
        {
          uim_ptr->command.rpt_buf.rpt_type = UIM_POWER_DOWN_R;

          /* Fill in necessary fields of command block */
          uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;
          if( uim_hw_if.intctrl[uim_ptr->id].m_hs_enable == TRUE
#ifdef FEATURE_UIM_BTSAP_CLIENT
              || IS_BTSAP_HANDLE_ACTIVE(uim_ptr)
#endif /* FEATURE_UIM_BTSAP_CLIENT */
            )
          {
            uim_ptr->hotswap.hotswap_me_power_down = FALSE;
          }

          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
        }
        break;

      case UIM_RESET_SWITCH_UIM_PASSIVE_F:
        {
          uim_ptr->command.rpt_buf.rpt_type = UIM_RESET_SWITCH_UIM_PASSIVE_R;

          /* Fill in necessary fields of command block */
          uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
        }
        break;

      case UIM_MANAGE_CHANNEL_F:
        {
          uim_ptr->command.rpt_buf.rpt_type = UIM_MANAGE_CHANNEL_R;

          /* Fill in necessary fields of command block */
          uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
        } /* end case - UIM_MANAGE_CHANNEL_F */
        break;

      case UIM_SILENT_RECOVER_UICC_F:
        {
          /* Notify the RECOVERY EVENT if callback is registered for SILENT RECOVERY */
          if(NULL != uim_ptr->silent_recover.uim_silent_recovery_callback)
          {
            uim_ptr->silent_recover.uim_silent_recovery_callback((uim_slot_type)(uim_ptr->id  +1),
                               uim_ptr->silent_recover.user_data,UIMDRV_RECOVERY_STAGE_END);
            /* Makesure It's not called again and again */
            uim_ptr->silent_recover.uim_silent_recovery_callback = NULL;
            uim_ptr->silent_recover.user_data = NULL;
          }
        }
     	break;
		
      default:
        {
          UIMDRV_MSG_ERR_1(uim_ptr->id, "***Unknown UIM Generic command: %d***",
                           cmd_ptr->hdr.command);
        } /* end case - default */
        break;

    }  /* switch */

    /* We only allocate cmd_ptr memory for the external commands that have
       valid callback. Internal UIM commands that use static buffers
       have callback set to NULL. */
    if (cmd_ptr == uim_ptr->command.cmd_ptr)
    {
      /* If cmd_ptr is same as current command in progress set global
         cmd_ptr to NULL as well */
      uim_ptr->command.cmd_ptr = NULL;
    }
    UIM_FREE(cmd_ptr);
  } /* end if - the client has supplied a report back function. */
} /* uim_send_generic_err_rpt */


/*===========================================================================

FUNCTION UIM_PARSE_UICC_SELECT_RESPONSE

DESCRIPTION
  This procedure parses the UICC select response for an EF and fills the
  required fields of last selected EF structure if required and fils the
  file characteristic byte.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Fills the file characteristic byte and elements of the last selected EF
  structure when the file selected is an EF that is under an ADF.
===========================================================================*/
static void uim_parse_uicc_select_response
(
  uim_instance_global_type *uim_ptr,
  byte const            *data,
  word                   num_bytes,
  boolean                  *ef_rsp_ptr,
  byte                  *file_char,
  uint8                    *supported_cmd_present
)
{
  uint16 i = 0; /* index variable */
  uint16 j = 0; /* Temp storage for length */
  uint16 k = 0; /* Innerloop variable for proprietary information */

  /* Process all the bytes in the receive FIFO */
  while (i < num_bytes)
  {
    if (data == NULL)
    {
      UIMDRV_MSG_LOW_0(uim_ptr->id,"uim_parse_uicc_select_response data is NULL");
      return;
    }
    switch (data[i++])
    {
      case UIM_FCP_TEMPLATE_TAG:
        i++;  /* Move past the length field */
        break;

      case UIM_FILE_DESCRIPTOR_TAG:
        if (i >= num_bytes)
        {
          return;
        }
        /* store the length to advance */
        j = data[i++];
        if ((i + j) >= num_bytes)
        {
          return;
        }
        switch (data[i] & UIM_UICC_EF_STRUCT_MASK)
        {
          case UIM_UICC_TRANSPARENT_EF:
          case UIM_UICC_LINEAR_EF:
          case UIM_UICC_CYCLIC_EF:
            if(ef_rsp_ptr != NULL)
            {
              *ef_rsp_ptr = TRUE;
            }
            break;
          default:
            break;
        }
        /* Skip j bytes */
        i+=j;
        break;

      case UIM_PROPRIETARY_TAG:
        if (i >= num_bytes)
        {
          return;
        }
        k = i + data[i];
        i++;
        while(i < k)
        {
          if (i >= num_bytes)
          {
            return;
          }
          switch(data[i++])
          {
            case UIM_CHARACTERISTICS_TAG:
            /* check here if i is a good index for "j" and the file_char */
            if ((i + 1) >= num_bytes)
            {
              return;
            }
            j = data[i++];
            /* Get the file characteristics byte */
            if(file_char != NULL)
            {
              *file_char = data[i];
            }
            else
            {
              UIMDRV_MSG_LOW_0(uim_ptr->id,"uim_parse_uicc_select_response data is NULL");
              return;
            }
            i+=j;
            break;

            case UIM_SUPPORTED_CMD_TAG:
              /* check here if i is a good index for "j" and supported_cmd_present */
              if ((i + 1) >= num_bytes)
              {
                return;
              }

              j = data[i++];
              if(supported_cmd_present != NULL)
              {
                *supported_cmd_present = data[i];
              }
              i+=j;
              break;

            default:
              if (i >= num_bytes)
              {
                return;
              }
              j = data[i++];
              i+=j;
              break;
          }
        }
        break;

      case UIM_FILE_ID_TAG:
      case UIM_FILE_SIZE_TAG:
      case UIM_DF_NAME_TAG:
      case UIM_SECURITY_ATTR_TAG1:
      case UIM_SECURITY_ATTR_TAG2:
      case UIM_SECURITY_ATTR_TAG3:
      case UIM_SECURITY_ATTR_TAG4:
      case UIM_LIFE_CYCLE_STATUS_TAG:
      case UIM_TOTAL_FILE_SIZE_TAG:
      case UIM_SFI_TAG:
      /* Ignore the tags that are not needed */
      default:
        if (i >= num_bytes)
        {
          return;
        }     /* Get the length */
        j = data[i++];
        /* Skip j bytes */
        i+=j;
        break;
    }
  }
} /* uim_parse_uicc_select_response */


/*===========================================================================

FUNCTION UIM_PARSE_STATUS_RESPONSE

DESCRIPTION
  This procedure parses the UICC response to a STATUS command

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Fills the data into the parameters provided.
===========================================================================*/
static void uim_parse_uicc_status_response
(
  byte const *data,
  word num_bytes,
  uim_dir_type *file_id,
  uim_aid_type *aid,
  uim_instance_global_type *uim_ptr
)
{
  uint16 i = 0; /* index variable */
  uint16 j = 0; /* Temp storage for length */


  if(file_id == NULL || aid == NULL)
  {
    UIMDRV_MSG_LOW_0(uim_ptr->id,"uim_parse_uicc_status_response file_id\aid is NULL");
    return;
  }
  /* Process all the bytes in the receive FIFO */
  while (i < num_bytes)
  {
     if (data == NULL)
     {
       UIMDRV_MSG_LOW_0(uim_ptr->id,"uim_parse_uicc_status_response data is NULL");
       return;
     }
     switch (data[i++])
     {
       case UIM_FCP_TEMPLATE_TAG:
         i++;  /* Move past the length field */
       break;

       case UIM_DF_NAME_TAG:
       {
         if (i >= num_bytes)
         {
           return;
         }
         /* Store the length of the aid data */
         aid->aid_length = data[i++];
         if((aid->aid_length + i) > num_bytes)
         {
           return;
         }
         uim_memscpy( aid->data, sizeof(aid->data),data+i, aid->aid_length);
         i+= aid->aid_length;
       }
       break;

       case UIM_FILE_ID_TAG:
       {
         if (i >= num_bytes)
         {
           return;
         }
         /* Move past the length field */
         j = data[i++];
         if ((i + j) > num_bytes)
         {
           return;
         }
         /* Get the file id */
         *file_id = (word)FLIPW(data+i);
         /* Move past the file id */
         i+=j;
       }
       break;

       /* Ignore the remaining tags */
       default:
       {
         if (i >= num_bytes)
         {
           return;
         }     /* Get the length */
         j = data[i++];
         /* Skip j bytes */
         i+=j;
       }
       break;
    } /* switch */
  }  /* while loop */
} /* uim_parse_status_response */


/*===========================================================================

FUNCTION UIM_PROCESS_GENERIC_COMMAND

DESCRIPTION
  This procedure processes a command sent to the UIM task. It sets the UIM
  state variable and calls the UIM state machine to process the command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  It sets the RUIM state variable amongst others.

===========================================================================*/
boolean uim_process_generic_command
(
  uim_instance_global_type *uim_ptr
)
{
  /* Boolean variable set if UIM power down initiated due to a card reset */
  boolean uim_powdown_initiated_due_to_reset = FALSE;

  /* This indicates that the command is processed by the state machine */
  boolean status = TRUE;

  /* Create a copy of current channel to increase code readability */
  uim_channel_type curr_ch = uim_ptr->card_cmd.cur_channel;

  if (uim_ptr->command.cmd_ptr == NULL)
  {
    UIMDRV_MSG_ERR_0(uim_ptr->id, "uim_process_generic_command pointer is NULL");
    return FALSE;
  }

  /* Copy user data from received command to report buffer */
  uim_ptr->command.rpt_buf.user_data = uim_ptr->command.cmd_ptr->hdr.user_data;

  if (uim_ptr->command.cmd_ptr->hdr.protocol == UIM_WCDMA)
  {
    uim_ptr->command.cmd_ptr->hdr.protocol = UIM_UICC;
  }

  /*For all power-up related commands retry attempts are not reduced.
    For all other commands retry attempts are reduced by 1
   */
  if(uim_ptr->command.cmd_ptr->hdr.command == UIM_INTERNAL_ME_PUP_F ||
     uim_ptr->command.cmd_ptr->hdr.command == UIM_INTERNAL_WAKE_UP_F ||
     uim_ptr->command.cmd_ptr->hdr.command == UIM_HOTSWAP_CARD_INS_F ||
     uim_ptr->command.cmd_ptr->hdr.command == UIM_RESET_F
    )
  {
    uim_ptr->setting.max_num_cmd_attempts = UIM_MAX_NUM_CMD_ATTEMPTS(uim_ptr);
  }
  else
  {
    uim_ptr->setting.max_num_cmd_attempts = UIM_MAX_NUM_CMD_ATTEMPTS(uim_ptr) - 1;
    UIMDRV_MSG_HIGH_1(uim_ptr->id,"Max command attempts allowed %x",
                      uim_ptr->setting.max_num_cmd_attempts);
  }
  UIMDRV_MSG_HIGH_1(uim_ptr->id,  "Received generic command 0x%x ",
                          uim_ptr->command.cmd_ptr->hdr.command);
  /* Process the generic command. */
  switch (uim_ptr->command.cmd_ptr->hdr.command)
  {
    case UIM_INTERNAL_ME_PUP_F:         /* POWER UP UIM due to task start up */
    {
#ifdef FEATURE_UIM_USB_UICC
      if (uim_is_usb_enabled(uim_ptr) &&
          uim_usb_has_remote_connection(uim_ptr) &&
          !uim_usb_iso_fallback_occurred(uim_ptr))
      {
        UIMDRV_MSG_HIGH_1(uim_ptr->id,  "Received internal USB UICC powerup command 0x%x ",
                          uim_ptr->command.cmd_ptr->hdr.command );
        /* Traverse through the states to power up the UIM from bootup */
        uim_ptr->command.generic_state_ptr = UIM_USB_INTERNAL_PUP_STATES;
      }
      else
#endif /* FEATURE_UIM_USB_UICC */
      {
        UIMDRV_MSG_HIGH_1(uim_ptr->id,  "Received internal Powerup command 0x%x ",
                          uim_ptr->command.cmd_ptr->hdr.command );
  /* Traverse through the states to power up the UIM from bootup */
#ifdef FEATURE_UIM_BTSAP_CLIENT
        if(IS_BTSAP_HANDLE_ACTIVE(uim_ptr))
        {
          uim_ptr->command.generic_state_ptr = UIM_BTSAP_CLIENT_PUP_STATES;
        }
        else
#endif /* FEATURE_UIM_BTSAP_CLIENT */
        {
          uim_ptr->command.generic_state_ptr = UIM_INTERNAL_PUP_STATES;
        }
      }

    } /* end case - UIM_INTERNAL_PUP_F */
      break;

    case UIM_INTERNAL_WAKE_UP_F:        /* Power up due to Power management */
    {
#ifdef FEATURE_UIM_USB_UICC
      if(uim_is_usb_enabled(uim_ptr) && uim_usb_is_usb_interface_selected(uim_ptr))
      {
        UIMDRV_MSG_HIGH_1(uim_ptr->id,  "Received internal USB UICC wakeup command 0x%x ",
                          uim_ptr->command.cmd_ptr->hdr.command );
        /* Traverse through the states to power up the UIM from sleep. */
        uim_ptr->command.generic_state_ptr = UIM_USB_INTERNAL_WAKE_UP_STATES;
      }
      else
#endif /* FEATURE_UIM_USB_UICC */
      {
        UIMDRV_MSG_HIGH_1(uim_ptr->id,  "Received internal Wakeup command, UIM Entering Recovery 0x%x ",
                          uim_ptr->command.cmd_ptr->hdr.command );
        /* Traverse through the states to power up the UIM from sleep. */
#ifdef FEATURE_UIM_BTSAP_CLIENT
       if(IS_BTSAP_HANDLE_ACTIVE(uim_ptr))
        {
          uim_ptr->command.generic_state_ptr = UIM_BTSAP_INTERNAL_WAKE_UP_STATES;
          if( uim_ptr->command.static_cmd_buf.hdr.cmd_count == 0 )
          {
            UIMDRV_MSG_HIGH_0(uim_ptr->id,"For BTSAP, setting cmd count to MAX - 1");
            uim_ptr->command.static_cmd_buf.hdr.cmd_count = uim_ptr->setting.max_num_cmd_attempts - 1;
          }
        }
        else
#endif /* FEATURE_UIM_BTSAP_CLIENT */
        {
          uim_ptr->command.generic_state_ptr = UIM_INTERNAL_WAKE_UP_STATES;
        }
        /* Save the last mode other than recovery mode */
        if ( uim_ptr->command.mode != UIM_RECOVERY_MODE )
        {
          UIMDRV_MSG_HIGH_1(uim_ptr->id,"Saving current mode %d and switching to recovery mode",
                            uim_ptr->command.mode);
          uim_ptr->state.save_mode = uim_ptr->command.mode;

          /* Put the UIM to check for recovery commands only */
          uim_ptr->command.mode = UIM_RECOVERY_MODE;
        }
        else
        {
          UIMDRV_MSG_MED_0(uim_ptr->id, "UIM Recovering within Recovery" );
          /* Reset old recovery data*/
          uim_ptr->state.recovery_data.ch_to_be_recovered = 0;
        }
      UIMDRV_MSG_HIGH_2(uim_ptr->id,"Operating mode of the UIM 0x%X and channel to be recovered 0x%X",
                          uim_ptr->command.mode,
                          uim_ptr->state.recovery_data.ch_to_be_recovered);

        /* Free any read write info it would be re-allocated as needed */
        uim_free_reset_rd_wr_info(uim_ptr);
        uim_free_reset_auth_info(uim_ptr);
      }

      /* Increment recovery count */
      uim_ptr->debug.tot_recovery_count++;

      if(uim_nv_is_feature_enabled(UIMDRV_FEATURE_ATTEMPT_PUP_3V_FROM_nTH_RECOVERY,uim_ptr)
         &&(uim_ptr->debug.tot_recovery_count >=
                    uim_ptr->setting.features_status_list.attempt_pup_3v_from_nth_recovery)
         &&(uim_ptr->flag.voltage_class_c_and_b_supported)
         &&(uim_ptr->state.current_voltage_class == UIM_VOLTAGE_CLASS_C)
         )
       {
         if (uim_ptr->hardware.uim_ldo.customize_ldo_setting &&
              uim_ptr->hardware.uim_ldo.desired_voltage_class != UIM_VOLTAGE_CLASS_INVALID)
         {
           UIMDRV_MSG_HIGH_0(uim_ptr->id,"LDO behaviour modified to work at specific voltage class");
           UIMDRV_MSG_HIGH_0(uim_ptr->id,"UIMDRV_FEATURE_ATTEMPT_PUP_3V_FROM_nTH_RECOVERY is invalidated");
         }
         else
         {
           UIMDRV_MSG_HIGH_1(uim_ptr->id,"Changing the voltage class to 3V during 0x%x recovery",
                                             uim_ptr->debug.tot_recovery_count );
           uim_ptr->state.current_voltage_class = UIM_VOLTAGE_CLASS_B;
         }
       }
    } /* end case - UIM_INTERNAL_WAKE_UP_F */
      break;

    case UIM_INTERNAL_POLL_F:           /* Internal command to Poll */
      {
        UIMDRV_MSG_HIGH_1(uim_ptr->id,  "Received internal Poll command 0x%x ",
                          uim_ptr->command.cmd_ptr->hdr.command );
        /* Build the Internal Poll list of states. */
        uim_ptr->mode.generic_states[0] = UIM_STATUS_ST;
        uim_ptr->mode.generic_states[1] = UIM_DONE_ST;
        uim_ptr->command.generic_state_ptr = uim_ptr->mode.generic_states;
      } /* end case - UIM_INTERNAL_POLL_F */
      break;

    case UIM_READ_BIN_CMD_F:
      {
        word num_bytes_to_alloc = 0;

        UIMDRV_MSG_HIGH_1(uim_ptr->id,  "Received UIM READ command 0x%x ",
                          uim_ptr->command.cmd_ptr->hdr.command );

        /* This is a read of a transparent file */
        uim_ptr->mode.generic_states[0] = UIM_READ_BIN_MEM_ST;
        uim_ptr->mode.generic_states[1] = UIM_DONE_ST;
        uim_ptr->command.generic_state_ptr = uim_ptr->mode.generic_states;

        num_bytes_to_alloc = uim_ptr->command.cmd_ptr->read_bin.len;

        /* Try to allocate mem and reset all the variables */
        if ( !uim_alloc_set_rd_wr_info(num_bytes_to_alloc, uim_ptr) )
        {
          uim_ptr->command.cmd_ptr->hdr.cmd_count = UIM_MAX_NUM_ATTEMPTS;
          status = FALSE;
          UIMDRV_MSG_ERR_1(uim_ptr->id, "Unable to allocate memory for data 0x%x",
                            uim_ptr->command.rd_wr_info.num_bytes);
          break;
        }

        uim_ptr->command.rd_wr_info.offset  = uim_ptr->command.cmd_ptr->read_bin.offset;
      }
      break;

    case UIM_READ_REC_CMD_F:
      {
          uim_ptr->mode.generic_states[0] = UIM_READ_REC_MEM_ST;
          uim_ptr->mode.generic_states[1] = UIM_DONE_ST;
          uim_ptr->command.generic_state_ptr = uim_ptr->mode.generic_states;

        if ( !uim_alloc_set_rd_wr_info(uim_ptr->command.cmd_ptr->read_rec.rec_len, uim_ptr) )
          {
            uim_ptr->command.cmd_ptr->hdr.cmd_count = UIM_MAX_NUM_ATTEMPTS;
            status = FALSE;
            UIMDRV_MSG_ERR_1(uim_ptr->id, "Unable to allocate memory for data 0x%x",
                             uim_ptr->command.rd_wr_info.num_bytes);
            break;
          }
        }
      break;

    case UIM_WRITE_BIN_CMD_F:
      {
        UIMDRV_MSG_HIGH_1(uim_ptr->id,  "Received UIM WRITE command 0x%x ",
                          uim_ptr->command.cmd_ptr->hdr.command );

          /* Workaround for a buggy card that loses its internal dir pointer
             after LOCI write */
          uim_ptr->state.mf_selection.required = uim_is_mf_selection_needed_after_update_bin_command(
                                                  &uim_ptr->command.cmd_ptr->write_bin.path);
          uim_ptr->state.mf_selection.channel   = 0;

        /* This is an update of a transparent file. */
        uim_ptr->mode.generic_states[0] = UIM_UPDATE_BIN_MEM_ST;
        uim_ptr->mode.generic_states[1] = UIM_DONE_ST;
        uim_ptr->command.generic_state_ptr = uim_ptr->mode.generic_states;

        /* Trust mmgsdi for the offset */
          uim_ptr->command.rd_wr_info.offset  = uim_ptr->command.cmd_ptr->write_bin.offset;

        /* Initialize the data pointer to read from to write to UIM */
        uim_ptr->command.rd_wr_info.data_ptr = uim_ptr->command.cmd_ptr->write_bin.data;

        /* If the number of bytes to be read is 0, do not continue */
        if (uim_ptr->command.cmd_ptr->write_bin.len  == 0 ||
            uim_ptr->command.cmd_ptr->write_bin.data == NULL)
        {
          uim_ptr->command.cmd_ptr->hdr.cmd_count = UIM_MAX_NUM_ATTEMPTS;
          status = FALSE;
          UIMDRV_MSG_ERR_0(uim_ptr->id, "Cannot Write 0 bytes");
          break;
        }

          /* Trust mmgsdi for offset. Offset being equal to file length has
           * already been ruled out above -- so len cannot be 0
           */
          uim_ptr->command.rd_wr_info.num_bytes = uim_ptr->command.cmd_ptr->write_bin.len;
        }
      break;

    case UIM_WRITE_REC_CMD_F:
      {
          /* This is an update of a record file. */
          uim_ptr->mode.generic_states[0] = UIM_UPDATE_REC_MEM_ST;
          uim_ptr->mode.generic_states[1] = UIM_DONE_ST;
          uim_ptr->command.generic_state_ptr = uim_ptr->mode.generic_states;

          uim_ptr->command.rd_wr_info.data_ptr      = uim_ptr->command.cmd_ptr->write_rec.data;
          uim_ptr->command.rd_wr_info.num_bytes_rsp = 0;
        uim_ptr->command.rd_wr_info.num_bytes     = uim_ptr->command.cmd_ptr->write_rec.len;

      }
      break;

    case UIM_INC_CMD_F:
      {
        UIMDRV_MSG_HIGH_1(uim_ptr->id,  "Received UIM INC command 0x%x ",
                          uim_ptr->command.cmd_ptr->hdr.command );

        /* This is an increment command. */
        uim_ptr->mode.generic_states[0] = UIM_INC_MEM_ST;
        uim_ptr->mode.generic_states[1] = UIM_DONE_ST;
        uim_ptr->command.generic_state_ptr = uim_ptr->mode.generic_states;

        uim_ptr->command.rd_wr_info.num_bytes = uim_ptr->command.cmd_ptr->inc.len;
        uim_ptr->command.rd_wr_info.data_ptr  = uim_ptr->command.cmd_ptr->inc.data;
        uim_ptr->command.rd_wr_info.num_bytes_rsp = 0;

      }
      break;

    case UIM_SELECT_F:           /* Select a file/directory from the UIM */
      {
          UIMDRV_MSG_HIGH_1(uim_ptr->id,  "Received UIM SELECT command 0x%x ",
                            uim_ptr->command.cmd_ptr->hdr.command );

        /* Init the status words */
        uim_ptr->card_cmd.get_resp_sw1 = (uim_sw1_type) 0;
        uim_ptr->card_cmd.get_resp_sw2 = (uim_sw2_type) 0;

        /* Check to see if we are trying to terminate a non current app */
        if ( (uim_ptr->command.cmd_ptr->select.file_type == UIM_ADF)       &&
             (uim_ptr->command.cmd_ptr->select.session == UIM_TERMINATION) &&
             (memcmp(uim_ptr->command.cmd_ptr->select.aid.data,
                     uim_ptr->state.current_path[curr_ch].curr_app.data,
                   uim_ptr->command.cmd_ptr->select.aid.aid_length) != 0))
        {
          status = FALSE;
        }
        else
        {
          /* Build the Select list of states. */
          uim_ptr->mode.generic_states[0] = UIM_SELECT_ST;
          uim_ptr->mode.generic_states[1] = UIM_DONE_ST;
          uim_ptr->command.generic_state_ptr = uim_ptr->mode.generic_states;

          uim_ptr->command.cmd_ptr->select.path_position = 0;
        }
      } /* end case - UIM_SELECT_F */
      break;

    case UIM_SEEK_F:                    /* Seek from the UIM */
      {
        UIMDRV_MSG_HIGH_1(uim_ptr->id,  "Received UIM Seek command 0x%x ",
                          uim_ptr->command.cmd_ptr->hdr.command );

        /* Build the Seek list of states. */
        uim_ptr->mode.generic_states[0] = UIM_SEEK_ST;
        uim_ptr->mode.generic_states[1] = UIM_DONE_ST;
        uim_ptr->command.generic_state_ptr = uim_ptr->mode.generic_states;
      } /* end case - UIM_SEEK_F */
      break;

    case UIM_INVALIDATE_F:              /* Invalidate an EF */
      {
        UIMDRV_MSG_HIGH_1(uim_ptr->id,  "Received UIM Invalidate command 0x%x ",
                          uim_ptr->command.cmd_ptr->hdr.command );

        /* Build the Invalidate list of states. */
        uim_ptr->mode.generic_states[0] = UIM_INVALIDATE_ST;
        uim_ptr->mode.generic_states[1] = UIM_DONE_ST;
        uim_ptr->command.generic_state_ptr = uim_ptr->mode.generic_states;
      } /* end case - UIM_INVALIDATE_F */
      break;

    case UIM_REHABILITATE_F:            /* Rehabilitate an EF */
      {
        UIMDRV_MSG_HIGH_1(uim_ptr->id,  "Received UIM Rehabilitate command 0x%x ",
                          uim_ptr->command.cmd_ptr->hdr.command );

        /* Build the Invalidate list of states. */
        uim_ptr->mode.generic_states[0] = UIM_REHABILITATE_ST;
        uim_ptr->mode.generic_states[1] = UIM_DONE_ST;
        uim_ptr->command.generic_state_ptr = uim_ptr->mode.generic_states;
      } /* end case - UIM_REHABILITATE_F */
      break;

    case UIM_VERIFY_CHV_F:              /* Verify CHV */
      {
        UIMDRV_MSG_HIGH_1(uim_ptr->id,  "Received Verify CHV command 0x%x ",
                          uim_ptr->command.cmd_ptr->hdr.command );
        /* Determine which CHV is to be verified. */
        if ((uim_ptr->command.cmd_ptr->verify_chv.chv == UIM_CHV1) &&
            (uim_ptr->command.cmd_ptr->hdr.protocol == UIM_ICC))
        {
          /* Copy the CHV to the cache buffer. The command processing will
             retrieve the CHV digits from the cache buffer. */
          uim_memscpy( uim_ptr->cache.chv1.chv,
                  sizeof(uim_ptr->cache.chv1.chv),
                  uim_ptr->command.cmd_ptr->verify_chv.chv_digits,
                  UIM_MAX_CHV_DIGITS);

          uim_ptr->cache.chv1.use_chv = TRUE;

          /* Format the CHV digits in the cache buffer. */
          uim_format_chv(uim_ptr->cache.chv1.chv,uim_ptr);

          /* Build the Verify CHV list of states. */
          uim_ptr->mode.generic_states[0] = UIM_VERIFY_CHV1_ST;
          uim_ptr->mode.generic_states[1] = UIM_DONE_ST;
          uim_ptr->command.generic_state_ptr = uim_ptr->mode.generic_states;
        }
        else if ((uim_ptr->command.cmd_ptr->verify_chv.chv == UIM_CHV2) &&
                 (uim_ptr->command.cmd_ptr->hdr.protocol == UIM_ICC))
        {
          /* Copy the CHV to the cache buffer. The command processing will
             retrieve the CHV digits from the cache buffer. */
          uim_memscpy( uim_ptr->cache.chv2.chv,
                  sizeof(uim_ptr->cache.chv2.chv),
                  uim_ptr->command.cmd_ptr->verify_chv.chv_digits,
                  UIM_MAX_CHV_DIGITS);
          uim_ptr->cache.chv2.use_chv = TRUE;

          /* Format the CHV digits in the cache buffer. */
          uim_format_chv(uim_ptr->cache.chv2.chv,uim_ptr);

          /* Build the Verify CHV list of states.  */
          uim_ptr->mode.generic_states[0] = UIM_VERIFY_CHV2_ST;
          uim_ptr->mode.generic_states[1] = UIM_DONE_ST;
          uim_ptr->command.generic_state_ptr = uim_ptr->mode.generic_states;
        }
        else if ((uim_ptr->command.cmd_ptr->verify_chv.chv > UIM_CHV2) &&
                 (uim_ptr->command.cmd_ptr->verify_chv.chv < UIM_CHV_MAX) &&
                 (uim_ptr->command.cmd_ptr->hdr.protocol == UIM_UICC))
        {
          uim_memscpy( uim_ptr->cache.chv2.chv,
                  sizeof(uim_ptr->cache.chv2.chv),
                  uim_ptr->command.cmd_ptr->verify_chv.chv_digits,
                  UIM_MAX_CHV_DIGITS);

          /* Format the CHV digits in the cache buffer. */
          uim_format_chv(uim_ptr->cache.chv2.chv,uim_ptr);

          /* Verify CHV2 state is used for Verifying PIN */
          uim_ptr->mode.generic_states[0] = UIM_VERIFY_CHV2_ST;
          uim_ptr->mode.generic_states[1] = UIM_DONE_ST;
          uim_ptr->command.generic_state_ptr = uim_ptr->mode.generic_states;
        }
        else
        {
          UIMDRV_MSG_HIGH_2(uim_ptr->id, "Wrong CHV/PIN 0x%x recd for 0x%x protocol",
                            uim_ptr->command.cmd_ptr->verify_chv.chv,
                            uim_ptr->command.cmd_ptr->hdr.protocol);
          status = FALSE;
        }
      } /* end case - UIM_VERIFY_CHV_F */
      break;

    case UIM_CHANGE_CHV_F:              /* Change CHV */
      {
        UIMDRV_MSG_HIGH_1(uim_ptr->id,  "Received Change CHV command 0x%x ",
                          uim_ptr->command.cmd_ptr->hdr.command );
        /* Determine which CHV is to be changed. */
        if ((uim_ptr->command.cmd_ptr->change_chv.chv == UIM_CHV1) &&
            (uim_ptr->command.cmd_ptr->hdr.protocol == UIM_ICC))
        {
          /* Build the Change CHV list of states. The new digits are NOT
             copied to the cache buffer unless the change completes
             successfully. */
          uim_ptr->mode.generic_states[0] = UIM_CHANGE_CHV1_ST;
          uim_ptr->mode.generic_states[1] = UIM_DONE_ST;
          uim_ptr->command.generic_state_ptr = uim_ptr->mode.generic_states;
        }
        else if ((uim_ptr->command.cmd_ptr->change_chv.chv == UIM_CHV2) &&
                 (uim_ptr->command.cmd_ptr->hdr.protocol == UIM_ICC))
        {
          /* Build the Verify CHV list of states. CHV2 is not cached since
             it is NOT automatically verified on wake up. */
          uim_ptr->mode.generic_states[0] = UIM_CHANGE_CHV2_ST;
          uim_ptr->mode.generic_states[1] = UIM_DONE_ST;
          uim_ptr->command.generic_state_ptr = uim_ptr->mode.generic_states;
        }
        else if ((uim_ptr->command.cmd_ptr->change_chv.chv > UIM_CHV2) &&
                 (uim_ptr->command.cmd_ptr->change_chv.chv < UIM_CHV_MAX) &&
                 (uim_ptr->command.cmd_ptr->hdr.protocol == UIM_UICC))
        {
          /* Verify CHV2 state is used for Verifying PIN */
          uim_ptr->mode.generic_states[0] = UIM_CHANGE_CHV2_ST;
          uim_ptr->mode.generic_states[1] = UIM_DONE_ST;
          uim_ptr->command.generic_state_ptr = uim_ptr->mode.generic_states;
        }
        else
        {
          UIMDRV_MSG_HIGH_2(uim_ptr->id, "Wrong CHV/PIN 0x%x recd for 0x%x protocol",
                            uim_ptr->command.cmd_ptr->verify_chv.chv,
                            uim_ptr->command.cmd_ptr->hdr.protocol);
          status = FALSE;
        }
      } /* end case - UIM_CHANGE_CHV_F */
      break;

    case UIM_DISABLE_CHV_F:             /* Disable CHV */
      {
        UIMDRV_MSG_HIGH_1(uim_ptr->id,  "Received Disable CHV command 0x%x ",
                          uim_ptr->command.cmd_ptr->hdr.command );
        /* Determine which CHV is to be disabled. */
        if ((uim_ptr->command.cmd_ptr->disable_chv.chv == UIM_CHV1) &&
            (uim_ptr->command.cmd_ptr->hdr.protocol == UIM_ICC))
        {
          /* Build the Change CHV list of states. */
          uim_ptr->mode.generic_states[0] = UIM_DISABLE_CHV1_ST;
          uim_ptr->mode.generic_states[1] = UIM_DONE_ST;
          uim_ptr->command.generic_state_ptr = uim_ptr->mode.generic_states;
        }
        else if ((uim_ptr->command.cmd_ptr->disable_chv.chv > UIM_CHV2) &&
                 (uim_ptr->command.cmd_ptr->disable_chv.chv < UIM_CHV_MAX) &&
                 (uim_ptr->command.cmd_ptr->hdr.protocol == UIM_UICC))
        {
          /* Verify CHV2 state is used for Verifying PIN */
          uim_ptr->mode.generic_states[0] = UIM_DISABLE_CHV1_ST;
          uim_ptr->mode.generic_states[1] = UIM_DONE_ST;
          uim_ptr->command.generic_state_ptr = uim_ptr->mode.generic_states;
        }
        else if ((uim_ptr->command.cmd_ptr->disable_chv.chv == UIM_CHV2) &&
                 (uim_ptr->command.cmd_ptr->hdr.protocol == UIM_ICC))
        {
          /* Build the Change CHV list of states. */
          uim_ptr->mode.generic_states[0] = UIM_DISABLE_CHV2_ST;
          uim_ptr->mode.generic_states[1] = UIM_DONE_ST;
          uim_ptr->command.generic_state_ptr = uim_ptr->mode.generic_states;
        }
        else
        {
          UIMDRV_MSG_HIGH_2(uim_ptr->id, "Wrong CHV/PIN 0x%x recd for 0x%x protocol",
                            uim_ptr->command.cmd_ptr->verify_chv.chv,
                            uim_ptr->command.cmd_ptr->hdr.protocol);
          status = FALSE;
        }
      } /* end case - UIM_DISABLE_CHV_F */
      break;

    case UIM_ENABLE_CHV_F:              /* Enable CHV */
      {
        UIMDRV_MSG_MED_0(uim_ptr->id,  "Received Enable CHV command " );
        /* Determine which CHV is to be changed. */
        if ((uim_ptr->command.cmd_ptr->enable_chv.chv == UIM_CHV1) &&
            (uim_ptr->command.cmd_ptr->hdr.protocol == UIM_ICC))
        {
          /* Build the Change CHV list of states. The enable digits are
             NOT copied to the cache buffer unless the comand completes
             successfully. */
          uim_ptr->mode.generic_states[0] = UIM_ENABLE_CHV1_ST;
          uim_ptr->mode.generic_states[1] = UIM_DONE_ST;
          uim_ptr->command.generic_state_ptr = uim_ptr->mode.generic_states;
        }
        else if ((uim_ptr->command.cmd_ptr->enable_chv.chv > UIM_CHV_NONE) &&
                 (uim_ptr->command.cmd_ptr->enable_chv.chv < UIM_CHV_MAX) &&
                 (uim_ptr->command.cmd_ptr->hdr.protocol == UIM_UICC))
        {
          /* Verify CHV2 state is used for Verifying PIN */
          uim_ptr->mode.generic_states[0] = UIM_ENABLE_CHV1_ST;
          uim_ptr->mode.generic_states[1] = UIM_DONE_ST;
          uim_ptr->command.generic_state_ptr = uim_ptr->mode.generic_states;
        }
        else if ((uim_ptr->command.cmd_ptr->enable_chv.chv == UIM_CHV2) &&
                 (uim_ptr->command.cmd_ptr->hdr.protocol == UIM_ICC))
        {
          /* Build the Change CHV list of states. */
          uim_ptr->mode.generic_states[0] = UIM_ENABLE_CHV2_ST;
          uim_ptr->mode.generic_states[1] = UIM_DONE_ST;
          uim_ptr->command.generic_state_ptr = uim_ptr->mode.generic_states;
        }
        else
        {
          UIMDRV_MSG_HIGH_2(uim_ptr->id, "Wrong CHV/PIN 0x%x recd for 0x%x protocol",
                            uim_ptr->command.cmd_ptr->verify_chv.chv,
                            uim_ptr->command.cmd_ptr->hdr.protocol);
          status = FALSE;
        }
      } /* end case - UIM_ENABLE_CHV_F */
      break;

    case UIM_UNBLOCK_CHV_F:             /* Unblock CHV */
      {
        UIMDRV_MSG_HIGH_1(uim_ptr->id,  "Received Unblock CHV command 0x%x ",
                          uim_ptr->command.cmd_ptr->hdr.command );
        /* Determine which CHV is to be unblocked. */
        if ((uim_ptr->command.cmd_ptr->unblk_chv.chv == UIM_CHV1) &&
            (uim_ptr->command.cmd_ptr->hdr.protocol == UIM_ICC))
        {
          /* Build the Unblock CHV list of states. */
          uim_ptr->mode.generic_states[0] = UIM_UNBLOCK_CHV1_ST;
          uim_ptr->mode.generic_states[1] = UIM_DONE_ST;
          uim_ptr->command.generic_state_ptr = uim_ptr->mode.generic_states;
        }
        else if ((uim_ptr->command.cmd_ptr->unblk_chv.chv == UIM_CHV2)&&
                 (uim_ptr->command.cmd_ptr->hdr.protocol == UIM_ICC))
        {
          /* Build the Unblock CHV list of states. */
          uim_ptr->mode.generic_states[0] = UIM_UNBLOCK_CHV2_ST;
          uim_ptr->mode.generic_states[1] = UIM_DONE_ST;
          uim_ptr->command.generic_state_ptr = uim_ptr->mode.generic_states;
        }
        else if ((uim_ptr->command.cmd_ptr->unblk_chv.chv > UIM_CHV_NONE) &&
                 (uim_ptr->command.cmd_ptr->unblk_chv.chv < UIM_CHV_MAX) &&
                 (uim_ptr->command.cmd_ptr->hdr.protocol == UIM_UICC))
        {
          /* Verify CHV2 state is used for Verifying PIN */
          uim_ptr->mode.generic_states[0] = UIM_UNBLOCK_CHV2_ST;
          uim_ptr->mode.generic_states[1] = UIM_DONE_ST;
          uim_ptr->command.generic_state_ptr = uim_ptr->mode.generic_states;
        }
        else
        {
          UIMDRV_MSG_HIGH_2(uim_ptr->id, "Wrong CHV/PIN 0x%x recd for 0x%x protocol",
                            uim_ptr->command.cmd_ptr->verify_chv.chv,
                            uim_ptr->command.cmd_ptr->hdr.protocol);
          status = FALSE;
        }
      } /* end case - UIM_UNBLOCK_CHV_F */
      break;

    case UIM_STATUS_F:                  /* Status of the Card */
      {
        UIMDRV_MSG_HIGH_1(uim_ptr->id,  "Received UIM Status command 0x%x ",
                          uim_ptr->command.cmd_ptr->hdr.command );

        if(uim_nv_is_feature_enabled(UIMDRV_FEATURE_UIM_POLLING_ONLY_AT_POLLING_TIMER_EXPIRY,
                                 uim_ptr) == FALSE)
        {
          uim_set_poll_timer( uim_ptr->setting.poll_time_delay,
                              uim_ptr->setting.wakeup_poll_offset, uim_ptr);
        }

        /* Build the Status list of states. */
        uim_ptr->mode.generic_states[0] = UIM_STATUS_ST;
        uim_ptr->mode.generic_states[1] = UIM_DONE_ST;
        uim_ptr->command.generic_state_ptr = uim_ptr->mode.generic_states;
      } /* end case - UIM_STATUS_F */
      break;

    case UIM_TERMINAL_PROFILE_F:        /* Send a terminal profile */
      {
        UIMDRV_MSG_HIGH_1(uim_ptr->id,  "Received Terminal Profile command 0x%x ",
                          uim_ptr->command.cmd_ptr->hdr.command );

        /* Build the Terminal Profile list of states. */
        uim_ptr->mode.generic_states[0] = UIM_TERMINAL_PROFILE_ST;
        uim_ptr->mode.generic_states[1] = UIM_DONE_ST;
        uim_ptr->command.generic_state_ptr = uim_ptr->mode.generic_states;
      } /* end case - UIM_TERMINAL_PROFILE_F */
      break;

    case UIM_ENVELOPE_F:                /* Send an envelope command */
      {
        UIMDRV_MSG_HIGH_1(uim_ptr->id,  "Received Envelope command 0x%x ",
                          uim_ptr->command.cmd_ptr->hdr.command );

        /* Build the Envelope list of states. */
        uim_ptr->mode.generic_states[0] = UIM_ENVELOPE_ST;
        uim_ptr->mode.generic_states[1] = UIM_DONE_ST;
        uim_ptr->command.generic_state_ptr = uim_ptr->mode.generic_states;

         /* Init the intermediate status words */
        uim_ptr->card_cmd.get_resp_sw1 = (uim_sw1_type) 0;
        uim_ptr->card_cmd.get_resp_sw2 = (uim_sw2_type) 0;
      } /* end case - UIM_ENVELOPE_F */
      break;

    case UIM_INTERNAL_FETCH_F:          /* Fetch a proactive command */
      {
        UIMDRV_MSG_HIGH_1(uim_ptr->id,  "Received Internal Fetch command 0x%x ",
                          uim_ptr->command.cmd_ptr->hdr.command );

        /* Build the Fetch list of states. */
        uim_ptr->mode.generic_states[0] = UIM_FETCH_ST;
        uim_ptr->mode.generic_states[1] = UIM_DONE_ST;
        uim_ptr->command.generic_state_ptr = uim_ptr->mode.generic_states;

      } /* end case - UIM_INTERNAL_FETCH_F */
      break;

    case UIM_TERMINAL_RESPONSE_F:       /* Send a proactive cmd response */
      {
        UIMDRV_MSG_HIGH_1(uim_ptr->id,  "Received Terminal Response command 0x%x ",
                          uim_ptr->command.cmd_ptr->hdr.command );

        /* Build the Terminal Response list of states. */
        uim_ptr->mode.generic_states[0] = UIM_TERMINAL_RESPONSE_ST;
        uim_ptr->mode.generic_states[1] = UIM_DONE_ST;
        uim_ptr->command.generic_state_ptr = uim_ptr->mode.generic_states;

        /* Initialize the flag */
        uim_ptr->command.rpt_buf.rpt.terminal_response.proactive_cmd_pending = FALSE;
      } /* end case - UIM_TERMINAL_RESPONSE_F */
      break;

    case UIM_POLLING_INTERVAL_F:
      {
        UIMDRV_MSG_HIGH_1(uim_ptr->id,  "Received TK polling command 0x%x ",
                          uim_ptr->command.cmd_ptr->hdr.command );

        /* Build the Terminal Response list of states. */
        uim_ptr->mode.generic_states[0] = UIM_POLLING_INTERVAL_ST;
        uim_ptr->mode.generic_states[1] = UIM_DONE_ST;
        uim_ptr->command.generic_state_ptr = uim_ptr->mode.generic_states;
      } /* end case - UIM_POLLING_INTERVAL_F */
      break;

    case UIM_HOTSWAP_CARD_INS_F:
    case UIM_RESET_F:   /* Perform a warm reset and perform power up */
      {
        uint8 num_ch=0;
        if(( uim_hw_if.intctrl[uim_ptr->command.cmd_ptr->hdr.slot - 1].m_hs_enable == TRUE
#ifdef FEATURE_UIM_BTSAP_CLIENT
             || IS_BTSAP_HANDLE_ACTIVE(uim_ptr)
#endif /* FEATURE_UIM_BTSAP_CLIENT */
           ) && uim_ptr->command.cmd_ptr->hdr.command == UIM_HOTSWAP_CARD_INS_F )
          {
            UIMDRV_MSG_HIGH_0(uim_ptr->id, "Received Reset command for UIM_HOTSWAP_CARD_INS_F");
        }
        else
        {
          UIMDRV_MSG_HIGH_1(uim_ptr->id, "Received Reset command 0x%x ",
                            uim_ptr->command.cmd_ptr->hdr.command );
        }

        /* The reset command shall have to bring - back uim to life
         * so let us change the uim_status accordingly
         */
        /* The status of UIM is set to Uninitialized since reset the UIM */
        uim_ptr->state.status = UIM_UNINITIALIZED_S;
        /* Clean out the last selected directory as we will reset the card. */
        uim_reset_dir(uim_ptr);
        
        /*clear out few UIM globals as it is a fresh card session*/
        uim_ptr->state.channel_info.manage_channel_allowed = TRUE;
        uim_ptr->state.channel_info.unallocated_channel    = 0xFF;

        uim_ptr->state.aram_app.select_aram_failed = FALSE;
        uim_ptr->state.aram_app.channel            = 0xFF;

        /* We are going to start fresh with the new instruction
           class. Set the uim_first_instn_class accordingly. Assumes
           that the caller would set the protocol appropriately.
        */
        switch(uim_ptr->command.cmd_ptr->hdr.protocol)
        {
          case UIM_ICC:
            {
              /* Perform a cold reset instead of a warm reset when
               * the reset is issued to change the instruction
               */
              if ( uim_ptr->state.first_inst_class != NV_UIM_FIRST_INST_CLASS_GSM_SIM )
              {
                if(uim_nv_is_feature_enabled(
                   UIMDRV_FEATURE_COLD_RESET_DUE_TO_CARD_SWITCH,
                   uim_ptr) == TRUE)
                {
                  uim_powdown_initiated_due_to_reset = TRUE;
                }
                uim_power_down(uim_ptr);
                uim_timed_sleep(UIM_ISO7816_VOLTAGE_SWITCH_PWR_DOWN_DELAY,
                                uim_ptr,
                                UIM_ALL_ZERO_SIG);
              }
              uim_ptr->state.first_inst_class = NV_UIM_FIRST_INST_CLASS_GSM_SIM;
            }
            break;
          case UIM_UICC:
            {
                  uim_ptr->state.first_inst_class = NV_UIM_FIRST_INST_CLASS_UMTS_SIM;
                }
            break;
          default:
            break;
        }

        /* Reset Channel Open information
         *  Default channel is always open - not changing its status */
        for(num_ch=1; num_ch<UIM_MAX_CHANNELS; num_ch++)
        {
          uim_ptr->state.is_ch_open[num_ch] = FALSE;
        }
        /* Traverse through the states to reset the UIM.
           Perform the same as a power-up.  This processing assumes
           the calling task will perform any other UIM commands after
           resetting the UIM. */
#ifdef FEATURE_UIM_USB_UICC
        if( uim_is_usb_enabled(uim_ptr) &&
            uim_usb_has_remote_connection(uim_ptr) &&
            !uim_usb_iso_fallback_occurred(uim_ptr))
        {
          if (uim_usb_is_usb_interface_selected(uim_ptr) &&
              (uim_ptr->command.cmd_ptr->hdr.command == UIM_RESET_F) &&
              (uim_ptr->command.cmd_ptr->hdr.protocol == UIM_ICC))
          {
            uim_ptr->usb.usb_interface_status.iso_fallback_occurred = TRUE;
            uim_ptr->command.generic_state_ptr = UIM_USB_INTERNAL_RESET_TO_ICC_STATES;
            UIMDRV_MSG_HIGH_0(uim_ptr->id, "USB UICC :Received reset command to icc");
          }
          else
          {
            uim_ptr->command.generic_state_ptr = UIM_USB_INTERNAL_PUP_STATES;
          }
        }
        else
#endif /* FEATURE_UIM_USB_UICC */
        {
#ifdef FEATURE_UIM_BTSAP_CLIENT
          if(IS_BTSAP_HANDLE_ACTIVE(uim_ptr))
          {
            uim_ptr->command.generic_state_ptr = UIM_BTSAP_CLIENT_PUP_STATES;
          }
          else
#endif /* FEATURE_UIM_BTSAP_CLIENT */
          {
            uim_ptr->command.generic_state_ptr = UIM_INTERNAL_PUP_STATES;
          }
        }

        /* Set the count of technical problems to zero */
        uim_ptr->card_cmd.inc_tech_prob_count = 0;

        /* Consider an external reset as a initial power-up */
        uim_ptr->flag.me_powerup = TRUE;
        /* Clear the poll timer and redefine for next card session */
        uim_clear_poll_timer(uim_ptr);
        uim_reset_poll_timer_val(uim_ptr);

        if(uim_nv_is_feature_enabled(
           UIMDRV_FEATURE_COLD_RESET_DUE_TO_CARD_SWITCH,
           uim_ptr) == TRUE)
        {
          if( !uim_powdown_initiated_due_to_reset )
          {
            uim_power_down(uim_ptr);
            uim_timed_sleep(UIM_ISO7816_VOLTAGE_SWITCH_PWR_DOWN_DELAY,
                            uim_ptr,
                            UIM_ALL_ZERO_SIG);
          }

          if (uim_ptr->hardware.uim_ldo.customize_ldo_setting &&
              uim_ptr->hardware.uim_ldo.desired_voltage_class != UIM_VOLTAGE_CLASS_INVALID)
          {
            uim_ptr->state.current_voltage_class = uim_ptr->hardware.uim_ldo.desired_voltage_class;
          }
          else
          {
            /* As the switched card may not be supporting the voltage in the
               last session (say 3V), power-up at 1.8V */
            uim_ptr->state.current_voltage_class = UIM_VOLTAGE_CLASS_C;
          }
        }
      } /* end case - UIM_RESET_F */
      break;

  case UIM_RESET_SWITCH_UIM_PASSIVE_F:
  case UIM_POWER_UP_UIM_PASSIVE_F:
  {
#ifdef FEATURE_UIM_BTSAP_CLIENT
    if(IS_BTSAP_HANDLE_ACTIVE(uim_ptr))
    {
      status = FALSE;
      UIMDRV_MSG_HIGH_0(uim_ptr->id,"UIM is in BTSAP client mode and cannot be switched to PASSIVE MODE. " );
      break;
    }
#endif /* FEATURE_UIM_BTSAP_CLIENT */

#ifdef FEATURE_UIM_USB_UICC
    if(uim_is_usb_enabled(uim_ptr) && uim_usb_is_usb_interface_selected(uim_ptr))
    {
      UIMDRV_MSG_HIGH_1(uim_ptr->id,  "Received reset to make UIM USB UICC passive 0x%x ",
                        uim_ptr->command.cmd_ptr->hdr.command );
      /* Traverse through the power off,on and get ATR states only */
      uim_ptr->command.generic_state_ptr = UIM_USB_RST_SWITCH_UIM_PASSIVE_STATES;
    }
    else
#endif /* FEATURE_UIM_USB_UICC */
    {
      UIMDRV_MSG_HIGH_1(uim_ptr->id,  "Received Reset to make UIM passive 0x%x ",
                        uim_ptr->command.cmd_ptr->hdr.command );
      /* Traverse through the atr and pps states only */
      uim_ptr->command.generic_state_ptr = UIM_RST_SWITCH_UIM_PASSIVE_STATES;
    }
    /* The status of UIM is set to Uninitialized since reset the UIM */
    uim_ptr->state.status = UIM_UNINITIALIZED_S;
    /* Clean out the last selected directory as we will reset the card. */
    uim_reset_dir(uim_ptr);

    /*clear out few UIM globals as it is a fresh card session*/
    uim_ptr->state.channel_info.manage_channel_allowed = TRUE;
    uim_ptr->state.channel_info.unallocated_channel    = 0xFF;

    uim_ptr->state.aram_app.select_aram_failed = FALSE;
    uim_ptr->state.aram_app.channel            = 0xFF;
  }
    break;

  case UIM_HOTSWAP_CARD_REM_F:
    case UIM_POWER_DOWN_F:
      {
        UIMDRV_MSG_HIGH_0(uim_ptr->id,  "Received power down command" );
        if(uim_ptr->flag.recovery_triggered)
        {
          uim_ptr->flag.recovery_triggered = FALSE;
        }

        /* In UIM power down we mask the HotSwap Interrupt and,
           In UIM power up we again unmask the hot swap interrupt */
        if (uim_ptr->command.cmd_ptr->hdr.command == UIM_POWER_DOWN_F &&
            uim_hw_if.intctrl[uim_ptr->id].m_hs_enable == TRUE)
        {
#ifdef FEATURE_UIM_DS_SUBSCRIPTION_MANAGER
          /*While powering-down inactive slot in case of SWITCH command
            Hotswap interrupts should not be masked
          */
          if(uim_ptr->command.cmd_ptr->hdr.req_type != UIM_NO_MASK_HOTSWAP_INT_REQ)
          {
#endif /* FEATURE_UIM_DS_SUBSCRIPTION_MANAGER */
          /* Disable the hot swap interrupt*/
            uim_hotswap_mask_interrupt(uim_ptr);
            uim_ptr->hotswap.hotswap_me_power_down = TRUE;
            UIMDRV_MSG_HIGH_0(uim_ptr->id,"Successfully mask hot swap interrupt for power down command");
#ifdef FEATURE_UIM_DS_SUBSCRIPTION_MANAGER
          }
#endif /* FEATURE_UIM_DS_SUBSCRIPTION_MANAGER */
        }

#ifdef FEATURE_UIM_USB_UICC
        if (uim_is_usb_enabled(uim_ptr) &&
            uim_usb_is_usb_mode_card_supported(uim_ptr) &&
            (uim_ptr->usb.usb_interface_status.uim_remote_client_connected == TRUE)&&
            !uim_usb_iso_fallback_occurred(uim_ptr))
        {
          uim_ptr->mode.generic_states[0] = UIM_USB_POWER_OFF_ST;
        }
        else
#endif /* FEATURE_UIM_USB_UICC */
        {
          uim_ptr->mode.generic_states[0] = UIM_POWER_DOWN_ST;
        }
        uim_ptr->mode.generic_states[1] = UIM_DONE_ST;
        uim_ptr->command.generic_state_ptr = uim_ptr->mode.generic_states;
      }
    break;
    case UIM_STREAM_APDU_F:       /* Send an APDU to the card */
      {
        UIMDRV_MSG_HIGH_1(uim_ptr->id,  "Received Stream APDU command 0x%x ",
                          uim_ptr->command.cmd_ptr->hdr.command );

        /* Build the Stream APDU list of states. */
        uim_ptr->mode.generic_states[0] = UIM_STREAM_APDU_ST;
        uim_ptr->mode.generic_states[1] = UIM_DONE_ST;
        uim_ptr->command.generic_state_ptr = uim_ptr->mode.generic_states;

        /* Check to make sure that the instruction does not match with
           commands for which we do directory maintenance */
        switch ((uim_instrns_enum_type)(uim_ptr->command.cmd_ptr->stream_apdu.instrn))
        {
          case SELECT:
          case STATUS:
          case READ_BINARY:
          case UPDATE_BINARY:
          case READ_RECORD:
          case UPDATE_RECORD:
          case SEEK:
          case INCREASE:
          case INVALIDATE:
          case REHABILITATE:
          /* The AUTHENTICATE case covers all supported authentication commands
             (RUN_GSM_ALGORITHM,AUTHENTICATE,RUN_CAVE,UIM_CDMA_AKA_AUTH) as
             their enum definitions are identical */
          case AUTHENTICATE:
          case SLEEP:
          case GET_RESPONSE:
          case TERMINAL_PROFILE:
          case ENVELOPE:
          case FETCH:
          case TERMINAL_RESPONSE:
          case UPDATE_SSD:
          case BS_CHALLENGE:
          case CONFIRM_SSD:
          case GEN_KEY_VPM:
          case MS_KEY_REQUEST:
          case KEY_GEN_REQUEST:
          case CONFIG_REQUEST:
          case DOWNLOAD_REQUEST:
          case SSPR_CONFIG_REQUEST:
          case SSPR_DOWNLOAD_REQUEST:
          case OTAPA_REQUEST:
          case COMMIT:
          case VALIDATE:
          case STORE_ESN_ME:
          case MANAGE_CHANNEL:
            status = FALSE;

          break;

          /* Allow the processing of commands only for PIN related commands and
             those that we do not recognize */
          default:
            status = TRUE;
          break;

        } /* end of switch */
      } /* end case - UIM_STREAM_APDU_F */
    break;

    case UIM_STREAM_ISO7816_APDU_F:     /* Stream an APDU - extended version */
    {
      /* Variables to hold the class (including channel) byte of the supplied
       * streaming APDU and the instruction supplied respectively.
       */
      byte class_byte;
      byte instrn;
      uim_ptr->card_cmd.stream_apdu_rpt_data_length  = 0xFFFF;

      UIMDRV_MSG_HIGH_1(uim_ptr->id,  "Received Stream 7816 APDU command 0x%x ",
                        uim_ptr->command.cmd_ptr->hdr.command );

      if(uim_ptr->command.cmd_ptr->stream_iso7816_apdu.num_cmd_bytes < UIM_ATCSIM_APDU_MIN_SIZE)
      {
        /* There are not enough bytes in the command */
        UIMDRV_MSG_HIGH_1(uim_ptr->id, "Insufficient number of bytes in the command 0x%x",
                          uim_ptr->command.cmd_ptr->stream_iso7816_apdu.num_cmd_bytes);
        status = FALSE;
        break;
      }

      UIMDRV_MSG_MED_7(uim_ptr->id, "APDU params class,ins,p1 p2,p3,d1 d2 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x",
                       uim_ptr->command.cmd_ptr->stream_iso7816_apdu.cmd_data[UIM_7816_APDU_CLASS_OFFSET],
                       uim_ptr->command.cmd_ptr->stream_iso7816_apdu.cmd_data[UIM_7816_APDU_INSTRN_OFFSET],
                       uim_ptr->command.cmd_ptr->stream_iso7816_apdu.cmd_data[UIM_7816_APDU_P1_OFFSET],
                       uim_ptr->command.cmd_ptr->stream_iso7816_apdu.cmd_data[UIM_7816_APDU_P2_OFFSET],
                       uim_ptr->command.cmd_ptr->stream_iso7816_apdu.cmd_data[UIM_CASE3_4_7816_LC_OFFSET],
                       uim_ptr->command.cmd_ptr->stream_iso7816_apdu.cmd_data[UIM_CASE2_7816_APDU_SIZE],
                       uim_ptr->command.cmd_ptr->stream_iso7816_apdu.cmd_data[UIM_CASE2_7816_APDU_SIZE+1]);

      /* Build the Stream 7816 APDU list of states. */
      uim_ptr->mode.generic_states[0]    = UIM_STREAM_APDU_ST;
      uim_ptr->mode.generic_states[1]    = UIM_DONE_ST;
      uim_ptr->command.generic_state_ptr = uim_ptr->mode.generic_states;

      /* Add one more state to the uim generic command state machine */
      if ( FALSE == uim_ptr->debug.uim_feature_busy_response_simulate_null.uim_disable_simulate_null &&
           uim_ptr->command.cmd_ptr->stream_iso7816_apdu.cmd_data[UIM_7816_APDU_INSTRN_OFFSET] == uim_ptr->debug.uim_feature_busy_response_simulate_null.uim_simulate_null_ins)
      {
          UIMDRV_MSG_HIGH_2(uim_ptr->id,  "Received Stream 7816 APDU 0x%x command with timer = 0x%x s",
                            uim_ptr->debug.uim_feature_busy_response_simulate_null.uim_simulate_null_ins,
                            uim_ptr->command.cmd_ptr->stream_iso7816_apdu.cmd_data[UIM_CASE2_7816_APDU_SIZE] );
        uim_ptr->mode.generic_states[0] = UIM_STREAM_APDU_ST;
        uim_ptr->mode.generic_states[1] = UIM_SIMULATE_NULL_ST;
        uim_ptr->mode.generic_states[2] = UIM_DONE_ST;

        /* Set the global value for the uim_simulate_null state machine */
        uim_ptr->debug.uim_simulate_null_timer_val_s = uim_ptr->command.cmd_ptr->stream_iso7816_apdu.cmd_data[UIM_CASE2_7816_APDU_SIZE];
        uim_ptr->command.generic_state_ptr           = uim_ptr->mode.generic_states;
        break;
      }

      /* Extract the class byte out of the APDU */
      class_byte =
        uim_ptr->command.cmd_ptr->stream_iso7816_apdu.cmd_data[UIM_7816_APDU_CLASS_OFFSET];

      /* Extract the instruction byte out of the APDU*/
      instrn =
         uim_ptr->command.cmd_ptr->stream_iso7816_apdu.cmd_data[UIM_7816_APDU_INSTRN_OFFSET];

      /* Let the commands be streamed in the default channel as well if in
       * passive mode
       */
      if ( uim_ptr->command.mode == UIM_PASSIVE_MODE )
      {
        /* When streaming in passive mode, let us store the protocol
         * to be used for internal poll from the header of the streamed
         * APDU on the default channel.  Ignore a manage channel
         * with UICC instruction class since it is acceptable
         * on a java card though an ICC.
         */
        if ( ((class_byte & (UIM_LOGICAL_CHANNEL_MASK |
                             UIM_UICC_EXTENDED_LC_CLASS)) == 0 )&&
            !((class_byte == UIM_UICC_INSTRUCTION_CLASS1)
              && (instrn == MANAGE_CHANNEL)) )
        {
          if (class_byte == UIM_INSTRUCTION_CLASS)
          {
            uim_ptr->state.dfs_1.protocol = UIM_ICC;
          }
          else
          {
            uim_ptr->state.dfs_1.protocol = UIM_UICC;
          }

          /* Start polling for presence detection once the
           * instruction class is known.  And reset the poll
           * timer if this is the first time.
           */
          if ( ( (int)uim_ptr->state.ctl & (int)UIM_PASSIVE ) != ((int)UIM_PASSIVE) )
          {
            /* Indicate that we can start the polling using the vote */
            uim_ptr->state.ctl = (uim_voter_type) ((int)uim_ptr->state.ctl | (int)UIM_PASSIVE);

            /* Just entered the passive mode.  Set the time to poll to the
             * default value and restart the timer to start counting
             * from the time we know the instruction class.
             */
            uim_set_poll_timer( UIM_POLL_TIME, UIM_WAKE_UP_POLL_OFFSET, uim_ptr);
          }
        }
        break;
      }

      /* Check if the command is streamed in default channel */
      if (!(class_byte & (UIM_LOGICAL_CHANNEL_MASK | UIM_UICC_EXTENDED_LC_CLASS) ))
      {
        /* Check if UICC command being streamed on the default channel */
        if ( (class_byte == UIM_UICC_INSTRUCTION_CLASS1) ||
             (class_byte == UIM_UICC_INSTRUCTION_CLASS2))
        {
          byte apdu_p1;
          byte *apdu_p2_ptr;

          UIMDRV_MSG_HIGH_0(uim_ptr->id, "Streaming UICC on default channel");

          uim_ptr->command.cmd_ptr->hdr.protocol = UIM_UICC;

          /* Get the value of P1 & P2 from the streamed APDU */
          apdu_p1     = uim_ptr->command.cmd_ptr->stream_iso7816_apdu.cmd_data[UIM_7816_APDU_P1_OFFSET];
          apdu_p2_ptr = &uim_ptr->command.cmd_ptr->stream_iso7816_apdu.cmd_data[UIM_7816_APDU_P2_OFFSET];

          if ( (uim_instrns_enum_type)(instrn) == SELECT )
          {
            /* Build the Stream 7816 APDU list of states. */
            uim_ptr->mode.generic_states[0]    = UIM_STREAM_SELECT_ST;
            uim_ptr->mode.generic_states[1]    = UIM_STREAM_APDU_ST;
            uim_ptr->mode.generic_states[2]    = UIM_DONE_ST;
            uim_ptr->command.generic_state_ptr = uim_ptr->mode.generic_states;

            uim_ptr->command.cmd_ptr->stream_iso7816_apdu.is_select_default_ch = TRUE;

            /* Check if P2 specifies to return FCP information if it doesn't
               change P2 so that we get an FCP.  We need this to parse in the
               response to determine if we've selected an EF, DF, or MF*/
            if (((*apdu_p2_ptr) & UIM_UICC_RETURN_NO_DATA) != UIM_UICC_RETURN_FCP_DATA)
            {
              /* clear the return "NO DATA" bits and set the return FCP bit */
              *apdu_p2_ptr &= (~UIM_UICC_RETURN_NO_DATA);
              *apdu_p2_ptr |= UIM_UICC_RETURN_FCP_DATA;
              UIMDRV_MSG_MED_0(uim_ptr->id, "Setting stream_apdu_rpt_data_length to 0");
              uim_ptr->card_cmd.stream_apdu_rpt_data_length = 0;
            }

            /* Select by DF name i.e SELECT ADF */
            if ( apdu_p1 & UIM_USIM_SELECT_BY_DF_NAME )
            {
              byte *stream_aid;
              byte *curr_aid;
              byte stream_aid_len;
              byte curr_aid_len;
              byte min_len;

              stream_aid = &(uim_ptr->command.cmd_ptr->stream_iso7816_apdu.cmd_data[UIM_CASE3_4_7816_LC_OFFSET+1]);

              stream_aid_len = uim_ptr->command.cmd_ptr->stream_iso7816_apdu.cmd_data[UIM_CASE3_4_7816_LC_OFFSET];

              curr_aid     = uim_ptr->state.current_path[curr_ch].curr_app.data;
              curr_aid_len = uim_ptr->state.current_path[curr_ch].curr_app.aid_length;

              /* Get the minimum of the supplied and the curr_app length */
              min_len = (curr_aid_len > stream_aid_len ) ?
                stream_aid_len : curr_aid_len;

             /* If the stream AID is not the same as the current aid reject the cmd */
              if(memcmp((void *)stream_aid, curr_aid, min_len) != 0)
              {
                  UIMDRV_MSG_ERR_0(uim_ptr->id, "Not allowed to active other app on default channel");
                  /* Fail the command */
                status = FALSE;
                break;
              }
            } /* select by df name */
            else if ( apdu_p1 & UN_SUPPORTED_SEL )
            {
              /* This is a select child or parent or by path from current DF
                 which are not supported */
              UIMDRV_MSG_ERR_0(uim_ptr->id, "Select child, parent or from current not supported");
              status = FALSE;
              break;
            } /* sel child, parent or path from current df */
            else if ( !(apdu_p1 & UIM_USIM_SELECT_BY_PATH_FROM_MF) &&
		      !(apdu_p1 == UIM_USIM_SELECT_BY_FILE_ID))
            {
              status = FALSE;
              break;
            }
          } /* if the instruction is select */
          else if ( ( ((uim_instrns_enum_type)(instrn) == READ_BINARY  ) ||
                      ((uim_instrns_enum_type)(instrn) == UPDATE_BINARY) ||
                      ((uim_instrns_enum_type)(instrn) == INCREASE ) ) &&
                    ( apdu_p1 & SFI_USED_IN_P1 )
                  )
          {
            /* The instruction is one of read bin, update bin and increase,
             * with usage of SFI which is not supported
             */
            UIMDRV_MSG_ERR_0(uim_ptr->id, "SFI selection is not supported");
            status = FALSE;
            break;
          }/* Read Bin, Update Bin or Increase*/
          else if ( ( ((uim_instrns_enum_type)(instrn) == READ_RECORD   ) ||
                      ((uim_instrns_enum_type)(instrn) == UPDATE_RECORD ) ||
                      ((uim_instrns_enum_type)(instrn) == SEEK ) ) &&
                    ( *apdu_p2_ptr & SFI_USED_IN_P2 )
                  )
          {
            /* The instruction is one of read rec, update rec or seek,
             * with usage of SFI which is not supported
             */
            UIMDRV_MSG_ERR_0(uim_ptr->id, "SFI selection is not supported");
            status = FALSE;
            break;
          } /* Read Rec, Update Rec or Search Rec */

          /* Any command other than select with the last selection
           * not the same as ATCSIM last selection and SFI not used
           * would have been selected as an intermediate select
           */
        } /* UICC instruction class */
        else if (class_byte == UIM_ICC_INSTRUCTION_CLASS)
        {
            UIMDRV_MSG_HIGH_1(uim_ptr->id, "Recd 7816 ATCSIM APDU command 0x%x ",
                              uim_ptr->command.cmd_ptr->hdr.command );

          /* Set the protocol for the command */
          uim_ptr->command.cmd_ptr->hdr.protocol = UIM_ICC;

          if ( (uim_instrns_enum_type)(instrn) == SELECT )
          {
            /* Build the Stream 7816 APDU list of states. */
            uim_ptr->mode.generic_states[0]    = UIM_STREAM_SELECT_ST;
            uim_ptr->mode.generic_states[1]    = UIM_DONE_ST;
            uim_ptr->command.generic_state_ptr = uim_ptr->mode.generic_states;
            uim_ptr->command.cmd_ptr->stream_iso7816_apdu.is_select_default_ch = TRUE;
          }
        }
      } /* Streamed on default channel */
    } /* end case - UIM_STREAM_ISO7816_APDU_F */
    break;

    case UIM_CLEAR_CACHE_F:     /* Clear uim cache */
      {
        UIMDRV_MSG_HIGH_0(uim_ptr->id,  "Received Clear Cache command" );
        status = FALSE;
      }
      break;

    case UIM_RECOVERY_COMPLETE_F: /* switch to normal mode */
      {
        UIMDRV_MSG_HIGH_0(uim_ptr->id, "Received recovery complete");
                        uim_ptr->mode.generic_states[0] = UIM_RECOVERY_COMPLETE_ST;
        uim_ptr->mode.generic_states[1] = UIM_DONE_ST;
        uim_ptr->command.generic_state_ptr = uim_ptr->mode.generic_states;
      }
      break;

    case UIM_SILENT_RECOVER_UICC_F:
      {
        uim_ptr->mode.generic_states[0] = UIM_SILENT_RECOVER_UICC_ST;
        uim_ptr->mode.generic_states[1] = UIM_DONE_ST;
        uim_ptr->command.generic_state_ptr = uim_ptr->mode.generic_states;
      }
      break;

    case UIM_MANAGE_CHANNEL_F:       /* Manage Channel */
      {
        UIMDRV_MSG_MED_0(uim_ptr->id, "Received Manage Channel command " );

        /* Build the Manage Channel list of states. */
        uim_ptr->mode.generic_states[0] = UIM_MANAGE_CHANNEL_ST;
        uim_ptr->mode.generic_states[1] = UIM_DONE_ST;
        uim_ptr->command.generic_state_ptr = uim_ptr->mode.generic_states;

      } /* end case - UIM_MANAGE_CHANNEL_F */
      break;

    default:
      UIMDRV_MSG_ERR_1(uim_ptr->id, "***Unknown UIM Generic command: %d***",
                       uim_ptr->command.cmd_ptr->hdr.command);
      break;

  } /* end switch (command_type) */

  if (status)
  {
    /* Call the state machine. */
    uim_generic_command (uim_ptr);
  }
  else
  {
    /* Send an error report to the client and release the buffer */
    uim_ptr->command.rpt_buf.sw1 = (uim_sw1_type) SW1_WRONG_PARAMS;
    uim_ptr->command.rpt_buf.sw2 = (uim_sw2_type) SW2_BAD_PARAMS_P1_P2;

    uim_send_generic_err_rpt(uim_ptr->command.cmd_ptr, uim_ptr);
  }

  return(status);

} /* uim_process_generic_command() */


/*===========================================================================

FUNCTION UIM_PROCESS_GENERIC_RESPONSE

DESCRIPTION
  This procedure processes the response that has been recd from the UIM
  at the completion of the command processing and releases the command
  buffer for a generic command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Releases the command buffer.

===========================================================================*/
void uim_process_generic_response
(
  uim_rsp_buf_type const   *rsp_ptr,
  uim_instance_global_type *uim_ptr
)
{

 if (uim_ptr->command.cmd_ptr == NULL || rsp_ptr == NULL)
  {
    return;
  }

  /* We will reset the report options flag in the command to NONE for
     backward compatibility purposes */

  uim_ptr->command.cmd_ptr->hdr.options = UIM_OPTION_NONE;

  /* Set the status to UIM_PASS unless something failed */
  uim_ptr->command.rpt_buf.rpt_status = UIM_PASS;

  UIMDRV_MSG_HIGH_1(uim_ptr->id, "Reporting UIM generic command 0x%x",
                    uim_ptr->command.cmd_ptr->hdr.command );

  switch (uim_ptr->command.cmd_ptr->hdr.command)
  {
    case UIM_INTERNAL_ME_PUP_F:         /* POWER UP UIM due to task start up */
    case UIM_INTERNAL_POLL_F:           /* Internal command to Poll */
        /* do nothing */
      break;

    case UIM_INTERNAL_WAKE_UP_F:        /* Power up due to Power management */
      {
        UIMDRV_MSG_HIGH_0(uim_ptr->id,"UIM Recovery Complete");
        /* notify QMI of Successfull recovery completion */
        qmi_uim_recovery_complete_ind((uim_slot_type)(uim_ptr->id + 1));
        uim_clear_recovery_params(uim_ptr);
      }
      break;

    case UIM_READ_REC_CMD_F:
    case UIM_READ_BIN_CMD_F:
      {
        if (uim_ptr->command.cmd_ptr->hdr.rpt_function != NULL)
        {
          if (uim_ptr->command.cmd_ptr->hdr.command == UIM_READ_REC_CMD_F)
          {
            uim_ptr->command.rpt_buf.rpt_type = UIM_READ_REC_CMD_R;
          }
          else
          {
            uim_ptr->command.rpt_buf.rpt_type = UIM_READ_BIN_CMD_R;
          }

          /* Fill in necessary fields of command block */
          uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

          uim_ptr->command.rpt_buf.rpt.read.num_bytes_read =
            uim_ptr->command.rd_wr_info.num_bytes_rsp;

          uim_ptr->command.rpt_buf.rpt.read.data =
            uim_ptr->command.rd_wr_info.alloc_mem;

          /* Report to the Requesting task */
          (*(uim_ptr->command.cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );

          /* Release the allocated memory for reads/inc commands */
          uim_free_reset_rd_wr_info(uim_ptr);

        } /* (uim_ptr->command.cmd_ptr->hdr.rpt_function != NULL) */
      }
      break;

    case UIM_WRITE_BIN_CMD_F:
    case UIM_WRITE_REC_CMD_F:
      {
        if (uim_ptr->command.cmd_ptr->hdr.command == UIM_WRITE_BIN_CMD_F)
        {
          uim_ptr->command.rpt_buf.rpt_type = UIM_WRITE_BIN_CMD_R;
        }
        else
        {
          uim_ptr->command.rpt_buf.rpt_type = UIM_WRITE_REC_CMD_R;
        }

        /* Fill in necessary fields of command block */
        uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

        uim_ptr->command.rpt_buf.rpt.write.num_bytes_written =
          uim_ptr->command.rd_wr_info.num_bytes_rsp;

        /* Report to the Requesting task */
        (*(uim_ptr->command.cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );

        /* Clear all the static variables */
        uim_free_reset_rd_wr_info(uim_ptr);
      }
      break;

    case UIM_INC_CMD_F:
      {
        uim_ptr->command.rpt_buf.rpt_type = UIM_INC_CMD_R;

        /* Fill in necessary fields of command block */
        uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

        if ( !uim_alloc_set_rd_wr_info(rsp_ptr->cmd_rsp_size,uim_ptr) )
        {
          uim_ptr->command.rpt_buf.rpt_status = UIM_FAIL;

          uim_ptr->command.rpt_buf.rpt.inc.inc_rsp_len = 0;

          uim_ptr->command.rpt_buf.rpt.inc.data        = NULL;
        }
        else
        {

          uim_ptr->command.rpt_buf.rpt.inc.inc_rsp_len =
            rsp_ptr->cmd_rsp_size;

          uim_memscpy(uim_ptr->command.rd_wr_info.alloc_mem,
                 rsp_ptr->cmd_rsp_size,
                 rsp_ptr->rsp.data,
                 sizeof(rsp_ptr->rsp.data));


          uim_ptr->command.rpt_buf.rpt.inc.data =
            uim_ptr->command.rd_wr_info.alloc_mem;
        }

        /* Report to the Requesting task */
        (*(uim_ptr->command.cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );

        /* Release the allocated memory for reads/inc commands */
        uim_free_reset_rd_wr_info(uim_ptr);
      }
      break;

    case UIM_SELECT_F:            /* Select a file/directory from the UIM */
      {
        /* Report back that Select was completed, if requested */
        if (uim_ptr->command.cmd_ptr->hdr.rpt_function != NULL)
        {
            uim_ptr->command.rpt_buf.rpt_type = UIM_SELECT_R;

          /* Fill in necessary fields of command block */
          uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

          if (uim_ptr->card_cmd.get_resp_sw1 == SW1_WARNINGS1 || uim_ptr->card_cmd.get_resp_sw1 == SW1_WARNINGS2)
          {
            /* copy the intermediate sw1 and sw2 */
            uim_ptr->command.rpt_buf.rpt.select.get_resp_sw1 = uim_ptr->card_cmd.get_resp_sw1;
            uim_ptr->command.rpt_buf.rpt.select.get_resp_sw2 = uim_ptr->card_cmd.get_resp_sw2;
          }

          /* Fill in the number of bytes in the report */
          uim_ptr->command.rpt_buf.rpt.select.num_data_bytes = rsp_ptr->cmd_rsp_size;

          /* copy the data into the report buffer  */
          uim_memscpy(uim_ptr->command.rpt_buf.rpt.select.data,
                 sizeof(uim_ptr->command.rpt_buf.rpt.select.data),
                 rsp_ptr->rsp.data,
                 rsp_ptr->cmd_rsp_size);

          /* Report to the Requesting task */
          (*(uim_ptr->command.cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );

        } /* (uim_ptr->command.cmd_ptr->hdr.rpt_function != NULL) */
      } /* end case - UIM_SELECT_F */
      break;

    case UIM_SEEK_F:                    /* Seek from the UIM */
      {
        /* Report back that Seek was completed, if requested */
        if (uim_ptr->command.cmd_ptr->hdr.rpt_function != NULL)
        {
          uim_ptr->command.rpt_buf.rpt_type = UIM_SEEK_R;

          /* Fill in necessary fields of command block */
          uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Fill in the number of bytes in the response that we got
            from UIM into the NV command */
          uim_ptr->command.rpt_buf.rpt.seek.num_records = rsp_ptr->cmd_rsp_size;

          /* copy the data into the report buffer */
          uim_memscpy(uim_ptr->command.rpt_buf.rpt.seek.data,
                 sizeof(uim_ptr->command.rpt_buf.rpt.seek.data),
                 rsp_ptr->rsp.data,
                 rsp_ptr->cmd_rsp_size);

          /* Report to the Requesting task */
          (*(uim_ptr->command.cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );

        } /* (uim_ptr->command.cmd_ptr->hdr.rpt_function != NULL) */
      } /* end case - UIM_SEEK_F */
      break;

    case UIM_INVALIDATE_F:              /* Invalidate an EF */
      {
        /* The item has been invalidated if it's get response has been
         * cached.... remove it from the cache so that the next select
         * on the item is forced to do a get response
         */

        if (uim_ptr->command.cmd_ptr->hdr.rpt_function != NULL)
        {
          uim_ptr->command.rpt_buf.rpt_type = UIM_INVALIDATE_R;

          /* Fill in necessary fields of command block */
          uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to the Requesting task */
          (*(uim_ptr->command.cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
        }
      } /* end case - UIM_INVALIDATE_F */
      break;

    case UIM_REHABILITATE_F:            /* Rehabilitate an EF */
      {
        /* The item has been rehabilitated if it's get response has been
         * cached.... remove it from the cache so that the next select
         * on the item is forced to do a get response
         */

        if (uim_ptr->command.cmd_ptr->hdr.rpt_function != NULL)
        {
          uim_ptr->command.rpt_buf.rpt_type = UIM_REHABILITATE_R;

          /* Fill in necessary fields of command block */
          uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to the Requesting task */
          (*(uim_ptr->command.cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
        }
      } /* end case - UIM_REHABILITATE_F */
      break;

    case UIM_VERIFY_CHV_F:              /* Verify CHV */
      {
        if (uim_ptr->command.cmd_ptr->hdr.rpt_function != NULL)
        {
          uim_ptr->command.rpt_buf.rpt_type = UIM_VERIFY_CHV_R;

          /* Fill in necessary fields of command block */
          uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

          if (rsp_ptr->cmd_status != UIM_DONE)
          {
             /* Set the status to UIM_FAIL since something failed */
             uim_ptr->command.rpt_buf.rpt_status = UIM_FAIL;
          }
          else
          {
            if( uim_nv_is_feature_enabled(UIMDRV_FEATURE_ENABLE_EXPLICIT_SELECTION_OF_MF_OR_ADF, uim_ptr) == TRUE )
            {
              if (uim_ptr->command.cmd_ptr != NULL) 
              {         
                /* This is for few buggy cards */
                /* which has lost EF pointer after verify PIN command */
                uim_ptr->state.mf_selection.required  = TRUE;
                uim_ptr->state.mf_selection.channel   = 0;
              }
            }
          }

          /* Report to the Requesting task */
          (*(uim_ptr->command.cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
        }
      } /* end case - UIM_VERIFY_CHV_F */
      break;

    case UIM_CHANGE_CHV_F:              /* Change CHV */
      {
        if (uim_ptr->command.cmd_ptr->hdr.rpt_function != NULL)
        {
          uim_ptr->command.rpt_buf.rpt_type = UIM_CHANGE_CHV_R;

          /* Fill in necessary fields of command block */
          uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

          if (rsp_ptr->cmd_status != UIM_DONE)
          {
             /* Set the status to UIM_FAIL since something failed */
             uim_ptr->command.rpt_buf.rpt_status = UIM_FAIL;
          }

          /* Report to the Requesting task */
          (*(uim_ptr->command.cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
        }
      } /* end case - UIM_CHANGE_CHV_F */
      break;

    case UIM_DISABLE_CHV_F:             /* Disable CHV */
      {
        if (uim_ptr->command.cmd_ptr->hdr.rpt_function != NULL)
        {
          uim_ptr->command.rpt_buf.rpt_type = UIM_DISABLE_CHV_R;

          /* Fill in necessary fields of command block */
          uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

          if (rsp_ptr->cmd_status != UIM_DONE)
          {
             /* Set the status to UIM_FAIL since something failed */
             uim_ptr->command.rpt_buf.rpt_status = UIM_FAIL;
          }

          /* Report to the Requesting task */
          (*(uim_ptr->command.cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
        }
      } /* end case - UIM_DISABLE_CHV_F */
      break;

    case UIM_ENABLE_CHV_F:              /* Enable CHV */
      {
        if (uim_ptr->command.cmd_ptr->hdr.rpt_function != NULL)
        {
          uim_ptr->command.rpt_buf.rpt_type = UIM_ENABLE_CHV_R;

          /* Fill in necessary fields of command block */
          uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

          if (rsp_ptr->cmd_status != UIM_DONE)
          {
             /* Set the status to UIM_FAIL since something failed */
             uim_ptr->command.rpt_buf.rpt_status = UIM_FAIL;
          }

          /* Report to the Requesting task */
          (*(uim_ptr->command.cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
        }
      } /* end case - UIM_ENABLE_CHV_F */
      break;

    case UIM_UNBLOCK_CHV_F:             /* Unblock CHV */
      {
        /* Report back the CHV unblock result, if requested */
        if (uim_ptr->command.cmd_ptr->hdr.rpt_function != NULL)
        {
          uim_ptr->command.rpt_buf.rpt_type = UIM_UNBLOCK_CHV_R;

          if (rsp_ptr->cmd_status != UIM_DONE)
          {
            /* Set the status as FAIL if the command has not succeeded */
            uim_ptr->command.rpt_buf.rpt_status = UIM_FAIL;
          }

          /* Fill in necessary fields of command block */
          uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to the Requesting task */
          (*(uim_ptr->command.cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
        } /* end if - is the report function defined */
      } /* end case - UIM_UNBLOCK_CHV_F */
      break;

    case UIM_STATUS_F:                  /* Status of the Card */
      {
        /* Report back the status as success */
        if (uim_ptr->command.cmd_ptr->hdr.rpt_function != NULL)
        {
          uim_ptr->command.rpt_buf.rpt_type = UIM_STATUS_R;

          /* Fill in necessary fields of command block */
          uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Fill in the number of bytes in the response that we got
            from UIM into the NV command */
          uim_ptr->command.rpt_buf.rpt.status.num_bytes = rsp_ptr->cmd_rsp_size;

          /* copy the data into the buffer from NV */
          uim_memscpy(uim_ptr->command.rpt_buf.rpt.status.data,
                 sizeof(uim_ptr->command.rpt_buf.rpt.status.data),
                 rsp_ptr->rsp.data,
                 rsp_ptr->cmd_rsp_size);

          /* Report to requesting task */
          (*(uim_ptr->command.cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
        } /* end if - is the report function defined. */
      } /* end case - UIM_STATUS_F */
      break;

    case UIM_TERMINAL_PROFILE_F:        /* Send a terminal profile */
      {
        /* Report back the status as success */
        if (uim_ptr->command.cmd_ptr->hdr.rpt_function != NULL)
        {
          uim_ptr->command.rpt_buf.rpt_type = UIM_TERMINAL_PROFILE_R;

          /* Fill in necessary fields of command block */
          uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to requesting task */
          (*(uim_ptr->command.cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );

          if(uim_ptr->command.cmd_ptr->hdr.protocol == UIM_UICC)
          {
            uim_queue_internal_open_channel_command(uim_ptr);
          }
		  
        } /* end if - is the report function defined. */
      } /* end case - UIM_TERMINAL_PROFILE_F */
      break;

    case UIM_ENVELOPE_F:                /* Send an envelope command */
      {
        /* Report back the status as success */
        if (uim_ptr->command.cmd_ptr->hdr.rpt_function != NULL)
        {
          uim_ptr->command.rpt_buf.rpt_type = UIM_ENVELOPE_R;

          /* Fill in necessary fields of command block */
          uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;
          if( uim_nv_is_feature_enabled(UIMDRV_FEATURE_ENABLE_EXPLICIT_SELECTION_OF_MF_OR_ADF, uim_ptr) == TRUE )
          {
            uim_ptr->state.mf_selection.required = uim_is_location_status_envelope(uim_ptr->command.cmd_ptr->envelope.data);
	      uim_ptr->state.mf_selection.channel   = 0;			

            if(TRUE != uim_ptr->state.mf_selection.required)
            {
              uim_ptr->state.mf_selection.required  = uim_is_call_connect_envelope(uim_ptr->command.cmd_ptr->envelope.data);
              uim_ptr->state.mf_selection.channel   = 0;
            }
          }

          /* Fill in the number of bytes in the response that we got
            from UIM into the NV command */
          uim_ptr->command.rpt_buf.rpt.envelope.data_length = rsp_ptr->cmd_rsp_size;

          /* copy the data into the buffer from NV */
          uim_memscpy(uim_ptr->command.rpt_buf.rpt.envelope.data,
                 sizeof(uim_ptr->command.rpt_buf.rpt.envelope.data),
                 rsp_ptr->rsp.data,
                 rsp_ptr->cmd_rsp_size);

          /* copy the intermediate sw1 and sw2 */
          uim_ptr->command.rpt_buf.rpt.envelope.get_resp_sw1 = uim_ptr->card_cmd.get_resp_sw1;
          uim_ptr->command.rpt_buf.rpt.envelope.get_resp_sw2 = uim_ptr->card_cmd.get_resp_sw2;

          /* Report to requesting task */
          (*(uim_ptr->command.cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
        } /* end if - is the report function defined. */
      } /* end case - UIM_ENVELOPE_F */
      break;

    case UIM_INTERNAL_FETCH_F:          /* Fetch a proactive command */
      {
        uim_ptr->command.more_time_pcmd_fetched = uim_is_more_time_pcmd_fetched(rsp_ptr->rsp.data);
#ifdef FEATURE_GSTK

#ifdef FEATURE_GSTK_TAL
        (void)gstk_tal_send_proactive_cmd(
                 uim_ptr->command.cmd_ptr->hdr.slot,
                 rsp_ptr->cmd_rsp_size,
                 rsp_ptr->rsp.data);
#else
        /* Pass raw data to GSTK */
        (void) uim_sm_send_proactive_cmd(
                 uim_ptr->command.cmd_ptr->hdr.slot,
                 rsp_ptr->cmd_rsp_size,
                 rsp_ptr->rsp.data);
#endif /* FEATURE_GSTK_TAL */

#endif /* FEATURE_GSTK */

      } /* end case - UIM_INTERNAL_FETCH_F */
      break;

    case UIM_TERMINAL_RESPONSE_F:       /* Send a proactive cmd response */
      {
        /* Report back the status as success */
        if (uim_ptr->command.cmd_ptr->hdr.rpt_function != NULL)
        {
          uim_ptr->command.rpt_buf.rpt_type = UIM_TERMINAL_RESPONSE_R;

          /* Fill in necessary fields of command block */
          uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to requesting task */
          (*(uim_ptr->command.cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );

          if( uim_nv_is_feature_enabled(UIMDRV_FEATURE_ENABLE_EXPLICIT_SELECTION_OF_MF_OR_ADF, uim_ptr) == TRUE )
          {
            UIMDRV_MSG_HIGH_0(uim_ptr->id, "Explicit MF selection set after Terminal Response");
            uim_ptr->state.mf_selection.required  = TRUE;
            uim_ptr->state.mf_selection.channel   = 0;
          }
        } /* end if - is the report function defined. */
      } /* end case - UIM_TERMINAL_RESPONSE_F */
      break;

    case UIM_POLLING_INTERVAL_F:
      {
        /* Report back the status as success */
        if (uim_ptr->command.cmd_ptr->hdr.rpt_function != NULL)
        {
          uim_ptr->command.rpt_buf.rpt_type = UIM_POLLING_INTERVAL_R;

          /* Fill in necessary fields of command block */
          uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to requesting task */
          (*(uim_ptr->command.cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
        } /* end if - is the report function defined. */
      } /* end case - UIM_POLLING_INTERVAL_F */
      break;

    case UIM_HOTSWAP_CARD_INS_F:
    case UIM_RESET_F:
      {
#ifdef FEATURE_UIM_BTSAP_CLIENT
        if(IS_BTSAP_HANDLE_ACTIVE(uim_ptr))
        {
          /* Flush all the existing commands to empty the queue  */
          uim_flush_command(UIM_FLUSH_ALL, uim_ptr);
        }
#endif /* FEATURE_UIM_BTSAP_CLIENT */
        /* Report back the status as success */
        if (uim_ptr->command.cmd_ptr->hdr.rpt_function != NULL)
        {
          uim_ptr->command.rpt_buf.rpt_type = UIM_RESET_R;

          /* Fill in necessary fields of command block */
          uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;
          uim_ptr->state.status = UIM_INITIALIZED_S;
          if ( uim_ptr->command.mode == UIM_PASSIVE_MODE )
          {
            /* Flush all the existing commands to empty the queue
            */
            uim_flush_command(UIM_FLUSH_ALL, uim_ptr);

            /* Make sure that we have switched the UIM mode to normal mode */
            uim_ptr->command.mode = UIM_NORMAL_MODE;

            /* Remove the requirement for presence detection in passive mode */
            uim_ptr->state.ctl = (uim_voter_type) ( ((int)uim_ptr->state.ctl) & ~((int)UIM_PASSIVE) );
          }

          /* Report to requesting task */
          (*(uim_ptr->command.cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
        } /* end if - is the report function defined. */
      } /* end case - UIM_RESET_F */
      break;
  case UIM_POWER_UP_UIM_PASSIVE_F:
  case UIM_RESET_SWITCH_UIM_PASSIVE_F:
    {
      /* Report back the status as success */
      if (uim_ptr->command.cmd_ptr->hdr.rpt_function != NULL)
      {
        if (uim_ptr->command.cmd_ptr->hdr.command == UIM_POWER_UP_UIM_PASSIVE_F)
        {
          uim_ptr->command.rpt_buf.rpt_type = UIM_POWER_UP_UIM_PASSIVE_R;
        }
        else
        {
          uim_ptr->command.rpt_buf.rpt_type = UIM_RESET_SWITCH_UIM_PASSIVE_R;
        }

        /* Fill in necessary fields of command block */
        uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

        uim_ptr->command.rpt_buf.rpt.rst_uim_passive.num_bytes =
          uim_ptr->atr.atr_buf.num_bytes;
        uim_memscpy(uim_ptr->command.rpt_buf.rpt.rst_uim_passive.data,
               sizeof(uim_ptr->command.rpt_buf.rpt.rst_uim_passive.data),
               uim_ptr->atr.atr_buf.data,
               uim_ptr->atr.atr_buf.num_bytes);

        /* Make sure that we have switched the UIM mode to passive */
        uim_ptr->command.mode = UIM_PASSIVE_MODE;

        /* Set clock stop allowed in passive mode flag to FALSE */
        uim_ptr->flag.passive_clk_stop_allowed = FALSE;

        /* Flush all the existing commands to empty the queue
        */
        uim_flush_command(UIM_FLUSH_ALL, uim_ptr);

        /* Report to requesting task */
        (*(uim_ptr->command.cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
      } /* end if - is the report function defined. */
    }
    break;
  case UIM_HOTSWAP_CARD_REM_F:
  case UIM_POWER_DOWN_F:
    {
      uim_status_type  uim_error_notificatoin;
      uim_cmd_name_type current_uim_command = uim_ptr->command.cmd_ptr->hdr.command;
      uim_pdown_option_type pdown_option = uim_ptr->command.cmd_ptr->pdown.pdown_option;
      uim_ptr->command.rpt_buf.rpt_type = UIM_POWER_DOWN_R;
      uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

      /* Switch the mode back to the saved previous mode */
      uim_ptr->command.mode = uim_ptr->state.save_mode;

      if (uim_ptr->command.cmd_ptr->hdr.rpt_function != NULL)
      {
        /* Report to requesting task */
        (*(uim_ptr->command.cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
      }
      /* Flush all the existing commands to empty the queue
      */
      if (current_uim_command == UIM_HOTSWAP_CARD_REM_F)
      {
        uim_flush_command(UIM_FLUSH_ALL,uim_ptr);
        uim_error_notificatoin= UIM_CARD_REMOVED_S;
      }
      else
      {
        uim_flush_command(UIM_FLUSH_ALL, uim_ptr);
        uim_error_notificatoin = UIM_POWER_DOWN_CMD_NOTIFICATION_S;
      }

      /* Notify GSDI about the card removal if the optionm is set */
      if (pdown_option == UIM_OPTION_NOTIFY_PDOWN)
      {
        if(uim_ptr->id < UIM_MAX_INSTANCES)
        {
          uim_notify_error(uim_error_notificatoin, uim_ptr);
        }
      }
      uim_reset_dir(uim_ptr);

      /*clear out few UIM globals as it is a fresh card session*/
      uim_ptr->state.channel_info.manage_channel_allowed = TRUE;
      uim_ptr->state.channel_info.unallocated_channel    = 0xFF;

      uim_ptr->state.aram_app.select_aram_failed = FALSE;
      uim_ptr->state.aram_app.channel            = 0xFF;

      /* set the status to ERROR */
      uim_ptr->state.status = UIM_ERR_S;

    }
    break;

    case UIM_STREAM_APDU_F:            /* Stream APDU to the card */
      {
        /* Report back the status as success */
        if (uim_ptr->command.cmd_ptr->hdr.rpt_function != NULL)
        {
          uim_ptr->command.rpt_buf.rpt_type = UIM_STREAM_APDU_R;

          /* Fill in necessary fields of command block */
          uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to requesting task */
          (*(uim_ptr->command.cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
        } /* end if - is the report function defined. */
      } /* end case - UIM_STREAM_APDU_F */
      break;

    case UIM_STREAM_ISO7816_APDU_F:            /* Stream APDU to the card */
      {
        /* Report back the status as success */
        if (uim_ptr->command.cmd_ptr->hdr.rpt_function != NULL)
        {
          uim_ptr->command.rpt_buf.rpt_type = UIM_STREAM_ISO7816_APDU_R;

          /* Fill in necessary fields of command block */
          uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

          uim_ptr->command.rpt_buf.rpt.stream_iso7816_apdu.num_data_bytes = 0;

          if (uim_ptr->command.rd_wr_info.num_bytes_rsp > 0)
          {
            /* copy the data into report buffer */
            uim_ptr->command.rpt_buf.rpt.stream_iso7816_apdu.num_data_bytes =
                                                   uim_ptr->command.rd_wr_info.num_bytes_rsp;

            uim_ptr->command.rpt_buf.rpt.stream_iso7816_apdu.data_ptr =
              uim_ptr->command.rd_wr_info.alloc_mem;

            uim_ptr->command.rpt_buf.rpt.stream_iso7816_apdu.get_resp_sw1 = uim_ptr->card_cmd.get_resp_sw1;
            uim_ptr->command.rpt_buf.rpt.stream_iso7816_apdu.get_resp_sw2 = uim_ptr->card_cmd.get_resp_sw2;
          }
          else if (rsp_ptr->cmd_rsp_size > 0)
          {
            /* copy the data into report buffer */
            /* Case, when default channel is selected */
            uim_ptr->command.rpt_buf.rpt.stream_iso7816_apdu.num_data_bytes =
              rsp_ptr->cmd_rsp_size;
            uim_ptr->command.rpt_buf.rpt.stream_iso7816_apdu.data_ptr =
              uim_malloc(rsp_ptr->cmd_rsp_size);
            if (uim_ptr->command.rpt_buf.rpt.stream_iso7816_apdu.data_ptr != NULL)
            {
              uim_memscpy(uim_ptr->command.rpt_buf.rpt.stream_iso7816_apdu.data_ptr,
                     rsp_ptr->cmd_rsp_size,
                     rsp_ptr->rsp.data,
                     rsp_ptr->cmd_rsp_size);
            }
            uim_ptr->command.rpt_buf.rpt.stream_iso7816_apdu.get_resp_sw1 = uim_ptr->card_cmd.get_resp_sw1;
            uim_ptr->command.rpt_buf.rpt.stream_iso7816_apdu.get_resp_sw2 = uim_ptr->card_cmd.get_resp_sw2;
          }
          else
          {
            uim_ptr->command.rpt_buf.rpt.stream_iso7816_apdu.get_resp_sw1 = (uim_sw1_type) 0;
            uim_ptr->command.rpt_buf.rpt.stream_iso7816_apdu.get_resp_sw2 = (uim_sw2_type) 0;
          }
          /* check If Truncation of response data with Le is enabled */
          if(uim_nv_is_feature_enabled(UIMDRV_FEATURE_LE_SUPPORT_FOR_7816_STREAM_APDU,
                               uim_ptr) == TRUE)
          {
            if(uim_ptr->card_cmd.stream_apdu_rpt_data_length < uim_ptr->command.rd_wr_info.num_bytes_rsp)
            {
              UIMDRV_MSG_MED_0(uim_ptr->id, "The rpt_length is less than the actual rsp");
              uim_ptr->command.rpt_buf.rpt.stream_iso7816_apdu.num_data_bytes = uim_ptr->card_cmd.stream_apdu_rpt_data_length;
                uim_ptr->command.rpt_buf.rpt.stream_iso7816_apdu.get_resp_sw2 = (uim_sw2_type)uim_ptr->card_cmd.stream_apdu_rpt_data_length;
              if(uim_ptr->card_cmd.stream_apdu_rpt_data_length == 0)
              {
                UIMDRV_MSG_MED_0(uim_ptr->id, "Setting intermediate status words to ZERO");
                uim_ptr->command.rpt_buf.rpt.stream_iso7816_apdu.get_resp_sw1 = (uim_sw1_type) 0;
                uim_ptr->command.rpt_buf.rpt.stream_iso7816_apdu.get_resp_sw2 = (uim_sw2_type) 0;
              }
            }
          }

          UIMDRV_MSG_HIGH_1(uim_ptr->id,"Total Response data length 0x%x",
                            uim_ptr->command.rpt_buf.rpt.stream_iso7816_apdu.num_data_bytes);
          /* Report to requesting task */
          (*(uim_ptr->command.cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
          if (uim_ptr->command.rd_wr_info.num_bytes_rsp > 0)
          {
            /* Release the allocated memory for stream apdu commands */
            uim_free_reset_rd_wr_info(uim_ptr);
          }
          else if (rsp_ptr->cmd_rsp_size > 0)
          {
            /* Case, when default channel is selected */
            if (uim_ptr->command.rpt_buf.rpt.stream_iso7816_apdu.data_ptr != NULL)
            {
              UIM_FREE(uim_ptr->command.rpt_buf.rpt.stream_iso7816_apdu.data_ptr);
            }
          }
          uim_ptr->command.rpt_buf.rpt.stream_iso7816_apdu.data_ptr = NULL;
        } /* end if - is the report function defined. */
      } /* end case - UIM_STREAM_ISO7816_APDU_F */
    break;

    case UIM_CLEAR_CACHE_F:
      {

        if (uim_ptr->command.cmd_ptr->hdr.rpt_function != NULL)
        {
          uim_ptr->command.rpt_buf.rpt_type = UIM_CLEAR_CACHE_R;

          /* Fill in necessary fields of command block */
          uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

          if ( rsp_ptr->cmd_status != UIM_DONE )
          {
            /* Set the status to UIM_PASS unless something failed */
            uim_ptr->command.rpt_buf.rpt_status = UIM_FAIL;
            UIMDRV_MSG_MED_0(uim_ptr->id,  "Reporting failure for Clear Cache");
          }
          else
          {
            UIMDRV_MSG_MED_0(uim_ptr->id,  "Reporting success for CLEAR CACHE");
          }

          /* Report to requesting task */
          (*(uim_ptr->command.cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
        } /* if report function is not null */
      } /* case UIM_CLEAR_CACHE_F */
    break;

    case UIM_RECOVERY_COMPLETE_F:
      {
        if (uim_ptr->command.cmd_ptr->hdr.rpt_function != NULL)
        {
          uim_ptr->command.rpt_buf.rpt_type = UIM_RECOVER_COMPLETE_R;

          /* Fill in necessary fields of command block */
          uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

          if ( rsp_ptr->cmd_status != UIM_DONE )
          {
            /* Set the status to UIM_PASS unless something failed */
            uim_ptr->command.rpt_buf.rpt_status = UIM_FAIL;
            UIMDRV_MSG_MED_0(uim_ptr->id,  "Reporting failure for RECOVERY_COMPLETE ");
          }
          /* Report to requesting task */
          (*(uim_ptr->command.cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
        } /* if report function is not null */
      } /* case UIM_RECOVERY_COMPLETE_F */
    break;

    case UIM_MANAGE_CHANNEL_F:            /* Manage Channel */
      {

        /* Report back that Channel Management was completed, if requested */
        if (uim_ptr->command.cmd_ptr->hdr.rpt_function != NULL)
        {
          uim_ptr->command.rpt_buf.rpt_type = UIM_MANAGE_CHANNEL_R;

          /* Fill in necessary fields of command block */
          uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

          /* This will check to ensure that a valid channel number has been
             received from the card */

          uim_ptr->command.rpt_buf.rpt.channel.channel_num = UIM_NO_LOGICAL_CHANNEL_SET;

          if(uim_ptr->command.cmd_ptr->manage_channel.mc_action == UIM_MC_OPEN)
          {
            /* open channel and P2 does not have the channel number */
            if(uim_ptr->command.cmd_ptr->manage_channel.op_close_channel == 0x00)
            {
              if (rsp_ptr->cmd_rsp_size == 0 )
              {
                /* Report back the status as UIM_FAIL indicating failure in the R-UIM */
                UIMDRV_MSG_ERR_0(uim_ptr->id,"UIM was not able to open the channel");
                uim_ptr->command.rpt_buf.rpt_status = UIM_FAIL;
              }
              else
              {
                uim_ptr->command.rpt_buf.rpt.channel.channel_num = rsp_ptr->rsp.data[0];
              }
            }
            else
            {
              /* Channel number is already defined in P2 parameter */
              uim_ptr->command.rpt_buf.rpt.channel.channel_num =
                uim_ptr->command.cmd_ptr->manage_channel.op_close_channel;
            }
            if(uim_ptr->command.rpt_buf.rpt.channel.channel_num < UIM_MAX_CHANNELS)
            {
              uim_ptr->state.is_ch_open[uim_ptr->command.rpt_buf.rpt.channel.channel_num] = TRUE;
            }
          }
          else
          {
            uim_ptr->command.rpt_buf.rpt.channel.channel_num = UIM_NO_LOGICAL_CHANNEL_SET;
            uim_ptr->state.is_ch_open[uim_ptr->command.cmd_ptr->manage_channel.op_close_channel] = FALSE;
            /* clean up the last directory global info for the current channel
               on the current slot if the close channel management is a successful
               operation */
            if (uim_ptr->command.rpt_buf.rpt_status == UIM_PASS)
            {
              uim_reset_dir_for_slot_channel(
                 (uim_channel_type)uim_ptr->command.cmd_ptr->manage_channel.op_close_channel,
                 uim_ptr);
            }
          }

          /* Report to the Requesting task */
          (*(uim_ptr->command.cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
        }
      } /* end case - UIM_MANAGE_CHANNEL_F */
      break;

    default:
      {
        UIMDRV_MSG_ERR_1(uim_ptr->id, "***Unknown UIM Generic command: %d***",
                         uim_ptr->command.cmd_ptr->hdr.command);
      } /* end case - default */
      break;

  } /* end switch - uim_ptr->command.cmd_ptr->hdr.command */

  /* We only allocate cmd_ptr memory for the external commands that have
     valid callback. Internal UIM commands that use static buffers
     have callback set to NULL. */
  if (uim_ptr->command.cmd_ptr->hdr.rpt_function != NULL)
  {
    UIM_FREE(uim_ptr->command.cmd_ptr);
  }
} /* uim_process_generic_response */


/*===========================================================================

FUNCTION UIM_CALCULATE_GUARD_TIME

DESCRIPTION
  This procedure calculates the guard time.

DEPENDENCIES

RETURN VALUE
  Guard time

SIDE EFFECTS

===========================================================================*/
uint8 uim_calculate_guard_time(uim_instance_global_type *uim_ptr)
{
  uint16 uim_guard_time     = 0;
  uint8  uim_min_guard_time = 0;

  uim_min_guard_time = UIM_MINIMUM_GUARD_TIME;

  /* TC1 byte is present in ATR */
  if(uim_ptr->atr.atr_buf.data[UIM_ATR_T0_BYTE] & (UIM_ATR_TC_PRESENT))
  {
    /* If TC1 byte is 0 or 255, only minimum guard time is possible */
    if( uim_ptr->atr.atr_tc1_byte == UIM_TC1_IS_0 ||
        uim_ptr->atr.atr_tc1_byte == UIM_TC1_IS_255 )
    {
      uim_guard_time = uim_min_guard_time;
    }
    else /* TC1 value is added to the minimum guard time */
    {
      uim_guard_time = uim_min_guard_time + uim_ptr->atr.atr_tc1_byte;
    }
  }
  else /* If tc1 byte is absent, only minimum guard time is possible */
  {
    uim_guard_time = uim_min_guard_time;
  }

  /* !! UART constraint: UART supports only 254 etus as guard time !! */
  if(uim_guard_time > UIM_MAX_POSSIBLE_GUARD_TIME)
  {
    uim_guard_time = UIM_MAX_POSSIBLE_GUARD_TIME;
  }

  return (uint8)uim_guard_time;
} /* uim_calculate_guard_time */


/*===========================================================================

FUNCTION UIM_STREAM_APDU_STATE

DESCRIPTION
  This procedure executes the stream apdu state

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void uim_stream_apdu_state
(
  uim_instance_global_type *uim_ptr,
  uim_req_buf_type         *uim_req_buf_static_ptr
)
{
  if (uim_ptr->command.cmd_ptr->hdr.command == UIM_STREAM_APDU_F)
  {
    /* Build an APDU to stream to the card */
    uim_req_buf_static_ptr->apdu_hdr.instrn = uim_ptr->command.cmd_ptr->stream_apdu.instrn;
    uim_req_buf_static_ptr->apdu_hdr.uim_class = uim_ptr->command.cmd_ptr->stream_apdu.uim_class;
    uim_req_buf_static_ptr->apdu_hdr.p1 = uim_ptr->command.cmd_ptr->stream_apdu.p1;
    uim_req_buf_static_ptr->apdu_hdr.p2 = uim_ptr->command.cmd_ptr->stream_apdu.p2;
    uim_req_buf_static_ptr->apdu_hdr.p3 = uim_ptr->command.cmd_ptr->stream_apdu.p3;

    /* We are ONLY going to deal with APDUs that have cmd data for now */
    uim_req_buf_static_ptr->instrn_case = UIM_INSTRN_CASE_3;

    if (uim_ptr->command.cmd_ptr->stream_apdu.p3 != 0)
    {
      uim_memscpy(uim_req_buf_static_ptr->req.data,
             sizeof(uim_req_buf_static_ptr->req.data),
             uim_ptr->command.cmd_ptr->stream_apdu.data,
             uim_ptr->command.cmd_ptr->stream_apdu.p3);
    }
  }
  else
  {
    /* Init the status words */
    uim_ptr->card_cmd.get_resp_sw1 = (uim_sw1_type) 0;
    uim_ptr->card_cmd.get_resp_sw2 = (uim_sw2_type) 0;

    /* Init the get resp status words */
    uim_ptr->command.rpt_buf.rpt.stream_iso7816_apdu.get_resp_sw1 = uim_ptr->card_cmd.get_resp_sw1;
    uim_ptr->command.rpt_buf.rpt.stream_iso7816_apdu.get_resp_sw2 = uim_ptr->card_cmd.get_resp_sw2;

    /* Build an APDU to stream to the card */
    uim_req_buf_static_ptr->apdu_hdr.uim_class =
      uim_ptr->command.cmd_ptr->stream_iso7816_apdu.cmd_data[UIM_7816_APDU_CLASS_OFFSET];
    uim_req_buf_static_ptr->apdu_hdr.instrn =
      uim_ptr->command.cmd_ptr->stream_iso7816_apdu.cmd_data[UIM_7816_APDU_INSTRN_OFFSET];
    uim_req_buf_static_ptr->apdu_hdr.p1 =
      uim_ptr->command.cmd_ptr->stream_iso7816_apdu.cmd_data[UIM_7816_APDU_P1_OFFSET];
    uim_req_buf_static_ptr->apdu_hdr.p2 =
      uim_ptr->command.cmd_ptr->stream_iso7816_apdu.cmd_data[UIM_7816_APDU_P2_OFFSET];

    /* Figure out the case for the commands */
    if (uim_ptr->command.cmd_ptr->stream_iso7816_apdu.num_cmd_bytes ==
                                               UIM_CASE1_7816_APDU_SIZE)
    {
      /* No input / no output */
      uim_req_buf_static_ptr->apdu_hdr.p3 = 0;
      /*  Marking case as case 2 because we always transmit p3 as 0x0 hence
          making it a de facto case 2. And in certain cases such as non-telecom
          apps it is possible that we get a 0x6C 0xXX response even for
          a case 1 command so for the response handling to occur correctly
          marking case as case 2*/
      uim_req_buf_static_ptr->instrn_case = UIM_INSTRN_CASE_2;

    }
    else if (uim_ptr->command.cmd_ptr->stream_iso7816_apdu.num_cmd_bytes ==
                                               UIM_CASE2_7816_APDU_SIZE)
    {
      /* Case 2 - no input / output of known or unknown length */
      uim_req_buf_static_ptr->apdu_hdr.p3 =
        uim_ptr->command.cmd_ptr->stream_iso7816_apdu.cmd_data[UIM_CASE2_7816_LE_OFFSET];
      uim_req_buf_static_ptr->instrn_case = UIM_INSTRN_CASE_2;
    }
    else if (uim_ptr->command.cmd_ptr->stream_iso7816_apdu.num_cmd_bytes ==
             ( UIM_CASE2_7816_APDU_SIZE +
              uim_ptr->command.cmd_ptr->stream_iso7816_apdu.cmd_data
                [UIM_CASE3_4_7816_LC_OFFSET]
              ))
    {
      /* Case 3 - input / no ouput */
      uim_req_buf_static_ptr->apdu_hdr.p3 = uim_ptr->command.cmd_ptr->stream_iso7816_apdu.cmd_data
                                             [UIM_CASE3_4_7816_LC_OFFSET];

      /* Copy the command bytes into the request buffer */
      uim_memscpy(uim_req_buf_static_ptr->req.data,
             sizeof(uim_req_buf_static_ptr->req.data),
             uim_ptr->command.cmd_ptr->stream_iso7816_apdu.cmd_data +
                                                UIM_CASE2_7816_APDU_SIZE,
             uim_ptr->command.cmd_ptr-> stream_iso7816_apdu.cmd_data
                                          [UIM_CASE3_4_7816_LC_OFFSET]);

      /* SELECT will be a case-4 instruction if it requests for
         FCP template */
      if(uim_req_buf_static_ptr->apdu_hdr.instrn == SELECT)
      {
        if (uim_req_buf_static_ptr->apdu_hdr.p2 == UIM_UICC_RETURN_NO_DATA)
        {
          /* Case 3 - input / output of known or unknown length */
          uim_req_buf_static_ptr->instrn_case = UIM_INSTRN_CASE_3;
        }
        else
        {
          /* Case 4 for select case */
          uim_req_buf_static_ptr->instrn_case = UIM_INSTRN_CASE_4;
        }
      } /* Non-SELECT APDUs */
      else if (STATUS == uim_req_buf_static_ptr->apdu_hdr.instrn)
      {
        /* Always allow GetStatus as Case4 command */
        uim_req_buf_static_ptr->instrn_case = UIM_INSTRN_CASE_4;
      }/* GET STATUS APDUs */
      else
      {
        uim_req_buf_static_ptr->instrn_case = UIM_INSTRN_CASE_3;
      }
    }
    else
    {
      /* Case 4 - input / output of known or unknown length */
      uim_req_buf_static_ptr->apdu_hdr.p3 = uim_ptr->command.cmd_ptr->stream_iso7816_apdu.cmd_data
                                             [UIM_CASE3_4_7816_LC_OFFSET];

      /* Copy the command bytes into the request buffer */
      uim_memscpy(uim_req_buf_static_ptr->req.data,
             sizeof(uim_req_buf_static_ptr->req.data),
             uim_ptr->command.cmd_ptr->stream_iso7816_apdu.cmd_data +
                                                UIM_CASE2_7816_APDU_SIZE,
             uim_ptr->command.cmd_ptr-> stream_iso7816_apdu.cmd_data
                                          [UIM_CASE3_4_7816_LC_OFFSET]);

      uim_req_buf_static_ptr->instrn_case = UIM_INSTRN_CASE_4;
      uim_ptr->card_cmd.stream_apdu_rpt_data_length = uim_ptr->command.cmd_ptr->stream_iso7816_apdu.cmd_data[
                    (uim_ptr->command.cmd_ptr->stream_iso7816_apdu.num_cmd_bytes - 1)] ;
      UIMDRV_MSG_MED_1(uim_ptr->id, "For instruction case 4 - value of the rpt_data_length is 0x%x",
                                             uim_ptr->card_cmd.stream_apdu_rpt_data_length );
      if(uim_ptr->card_cmd.stream_apdu_rpt_data_length == 0)
      {
        uim_ptr->card_cmd.stream_apdu_rpt_data_length  = 0xFFFF;
      }
    }
  }

  /* Set the protocol based on the class of the command */
  if (uim_req_buf_static_ptr->apdu_hdr.uim_class != UIM_INSTRUCTION_CLASS)
  {
    uim_req_buf_static_ptr->protocol = UIM_UICC;
    uim_ptr->command.cmd_ptr->hdr.protocol = UIM_UICC;
  }
  else
  {
    uim_req_buf_static_ptr->protocol = UIM_ICC;
    uim_ptr->command.cmd_ptr->hdr.protocol = UIM_ICC;
  }
  /* Set rd wr buffer length to zero */
  uim_ptr->command.rd_wr_info.num_bytes = 0;
  uim_ptr->command.rd_wr_info.num_bytes_rsp = 0;

  uim_req_buf_static_ptr->rsp_ptr = &uim_ptr->command.rsp_buf;

  uim_send_command(uim_req_buf_static_ptr, uim_ptr);
} /* uim_stream_apdu_state */


/*===========================================================================

FUNCTION UIM_GET_START_PATH_INDEX

DESCRIPTION
  This procedure gets the index we want to start at of the path to select

DEPENDENCIES
  Only used in ICC cases

RETURN VALUE
  The position of the new path to start selecting from

SIDE EFFECTS

===========================================================================*/
static uint8 uim_get_start_path_index
(
  uim_instance_global_type *uim_ptr,
  uim_path_type            *select_path_ptr
)
{
  uim_path_type curr_dir_path = {0};
  uim_path_type *curr_dir_path_ptr = &curr_dir_path;

  if(uim_ptr->state.current_path[0].is_ef)
  {
    uim_memscpy(curr_dir_path_ptr->path,
                sizeof(curr_dir_path_ptr->path),
                uim_ptr->state.current_path[0].path.path,
                (uim_ptr->state.current_path[0].path.len - 1) * 2);
    curr_dir_path_ptr->len = uim_ptr->state.current_path[0].path.len - 1;
  }
  else
  {
    uim_memscpy(curr_dir_path_ptr->path,
                sizeof(curr_dir_path_ptr->path),
                uim_ptr->state.current_path[0].path.path,
                uim_ptr->state.current_path[0].path.len * 2);
    curr_dir_path_ptr->len = uim_ptr->state.current_path[0].path.len;
  }


  /* if it's a select for MF or some path we don't
     recognize start at the beginning of the path */
  if(select_path_ptr->len <= 1 || curr_dir_path_ptr->len == 0)
  {
    return 0;
  }

  /* current directory is MF start selecting from
     the second select path element */
  if(curr_dir_path_ptr->len == 1)
  {
    return 1;
  }

  /* at this point the curr dir path length and the
     select path length are both at least two */

  /* is the target a child of the current directory
     or is it the current directory */
  if(memcmp(select_path_ptr->path,
            curr_dir_path_ptr->path,
            (curr_dir_path_ptr->len * 2)) ==0)
  {
    if(select_path_ptr->len > curr_dir_path_ptr->len)
    {
      return curr_dir_path_ptr->len;
    }
    else if(select_path_ptr->len == curr_dir_path_ptr->len)
    {
      return curr_dir_path_ptr->len - 1;
    }
    /* if we reach here the select path is shorter than the current directory
       path but somehow the memcmp returned true - should not be possible */
  }
  /* Is the next select a DF that is a child of the current
     selected DF's parent (sibling of the current DF)
     An explicit select for a sibling directory would be the same length
     as the current dir path length, however we don't know before issuing
     a select if it's a DF or EF - EF selection is not allowed this way
     so we can only issue the select this way if we know it's a DF(additional
     path elements after the sibling element assures us it's a DF)*/
  if(select_path_ptr->len > curr_dir_path_ptr->len &&
     memcmp(select_path_ptr->path,
            curr_dir_path_ptr->path,
            (curr_dir_path_ptr->len - 1) * 2) == 0)
  {
    return curr_dir_path_ptr->len - 1;
  }
  /* Does it make sense to select the parent directory or
     is the select explicitly for the parent dir */
  if((select_path_ptr->len == curr_dir_path_ptr->len ||
      select_path_ptr->len == curr_dir_path_ptr->len - 1) &&
     memcmp(select_path_ptr->path,
            curr_dir_path_ptr->path,
            (curr_dir_path_ptr->len - 1) *2) == 0)
  {
    return curr_dir_path_ptr->len - 2;
  }

  /* we found no clever way for selecting so select from MF */
  return 0;
} /* uim_get_start_path_index */


/*===========================================================================

FUNCTION UIM_SELECT_STATE

DESCRIPTION
  This procedure executes the select state

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void uim_select_state
(
  uim_instance_global_type *uim_ptr,
  uim_path_type            *target_path_ptr,
  uim_req_buf_type         *uim_req_buf_static_ptr
)
{
  uint8 session_type = UIM_ACTIVATION_WITH_NO_DATA;
  int i;
  int j;

  uim_req_buf_static_ptr->apdu_hdr.instrn = SELECT;
  uim_req_buf_static_ptr->instrn_case     = UIM_INSTRN_CASE_4;

/* ICC CODE */
  if(uim_ptr->command.cmd_ptr->hdr.protocol == UIM_ICC)
  {
    if(uim_ptr->command.cmd_ptr->select.path_position == 0)
    {
      uim_ptr->command.cmd_ptr->select.path_position = uim_get_start_path_index(uim_ptr, target_path_ptr);
    }
    uim_req_buf_static_ptr->apdu_hdr.uim_class     = UIM_INSTRUCTION_CLASS;
    uim_req_buf_static_ptr->apdu_hdr.p1            = 0;
    uim_req_buf_static_ptr->apdu_hdr.p2            = 0;
    uim_req_buf_static_ptr->apdu_hdr.p3            = 2;

    uim_req_buf_static_ptr->req.mf_df_ef.high_byte = WORD_HI (target_path_ptr->path[uim_ptr->command.cmd_ptr->select.path_position]);
    uim_req_buf_static_ptr->req.mf_df_ef.low_byte  = WORD_LO (target_path_ptr->path[uim_ptr->command.cmd_ptr->select.path_position]);

    uim_req_buf_static_ptr->rsp_ptr                = &uim_ptr->command.rsp_buf;

    uim_send_command(uim_req_buf_static_ptr, uim_ptr);
    return;
  }
/* ICC CODE since we return in the ICC code everything below only is executed for UICC case */

  uim_req_buf_static_ptr->apdu_hdr.uim_class =
    uim_util_get_uim_class(UIM_UICC_INSTRUCTION_CLASS1, uim_ptr->command.cmd_ptr->hdr.channel);

  /* if it's a select MF do this */
  if(target_path_ptr->path[0] == 0x3F00 &&
     target_path_ptr->len     == 1)
  {
    uim_req_buf_static_ptr->apdu_hdr.p1 = UIM_USIM_SELECT_BY_FILE_ID;
    uim_req_buf_static_ptr->apdu_hdr.p2 = UIM_UICC_RETURN_FCP_DATA;
    uim_req_buf_static_ptr->req.data[0] = WORD_HI(target_path_ptr->path[0]);
    uim_req_buf_static_ptr->req.data[1] = WORD_LO(target_path_ptr->path[0]);
    uim_req_buf_static_ptr->apdu_hdr.p3 = 2;
  } /* SELECT MF */
  else if(UIM_ADF == uim_ptr->command.cmd_ptr->select.file_type)
  {
    uim_req_buf_static_ptr->apdu_hdr.p1 = UIM_USIM_SELECT_BY_DF_NAME;

    session_type = uim_ptr->command.cmd_ptr->select.session;

    /* Bit 2, 3 and 4 of P2 determine the type of select response
       solicited from the card. Based on type of activation the
       bit pattern for P2 is setup */
    switch(session_type)
    {
      case UIM_ACTIVATION_WITH_NO_DATA:
        uim_req_buf_static_ptr->apdu_hdr.p2 = UIM_UICC_RETURN_NO_DATA;
        break;
      case UIM_ACTIVATION_WITH_FMD:
        uim_req_buf_static_ptr->apdu_hdr.p2 = UIM_UICC_RETURN_FMD_DATA;
        break;
      case UIM_ACTIVATION_WITH_FCI:
        uim_req_buf_static_ptr->apdu_hdr.p2 = UIM_UICC_RETURN_FCI_DATA;
        break;
      case UIM_ACTIVATION_WITH_FCI_INTERFACES:
        uim_req_buf_static_ptr->apdu_hdr.p2 = UIM_UICC_RETURN_FCI_W_INTERFACE;
        break;
      case UIM_ACTIVATION_OR_RESET:
        uim_req_buf_static_ptr->apdu_hdr.p2 = UIM_UICC_RETURN_FCP_DATA;
        break;
      case UIM_TERMINATION:
        uim_req_buf_static_ptr->apdu_hdr.p2 = UIM_UICC_RETURN_NO_DATA | UIM_USIM_SESSION_TERMINATION;
        break;
      /* No default case to ensure all types requested types are addressed */
    }
    /* Select Next on partial Aid */
    if(UIM_NEXT_OCCURRENCE == uim_ptr->command.cmd_ptr->select.adf_selection)
    {
      uim_req_buf_static_ptr->apdu_hdr.p2 |= UIM_NEXT_OCCURRENCE;
    }
    else if(UIM_LAST_OCCURRENCE == uim_ptr->command.cmd_ptr->select.adf_selection)
    {
      uim_req_buf_static_ptr->apdu_hdr.p2 |= UIM_LAST_OCCURRENCE;
    }
    else if(UIM_PREVIOUS_OCCURRENCE == uim_ptr->command.cmd_ptr->select.adf_selection)
    {
      uim_req_buf_static_ptr->apdu_hdr.p2 |= UIM_PREVIOUS_OCCURRENCE;
    }
    else
    {
      /* Bits 1 and 2 of P2 are setup for first occurrence */
      uim_req_buf_static_ptr->apdu_hdr.p2 |= UIM_FIRST_OR_ONLY_OCCURRENCE;
    }
    uim_memscpy(uim_req_buf_static_ptr->req.data,
                sizeof(uim_req_buf_static_ptr->req.data),
                uim_ptr->command.cmd_ptr->select.aid.data,
                uim_ptr->command.cmd_ptr->select.aid.aid_length);

    uim_req_buf_static_ptr->apdu_hdr.p3 = uim_ptr->command.cmd_ptr->select.aid.aid_length;

    /* Check for empty AID and  */
    if (uim_req_buf_static_ptr->apdu_hdr.p3 == 0)
    {
      uim_req_buf_static_ptr->instrn_case = UIM_INSTRN_CASE_2;
    }
  } /* Select by AID */
  else
  {
    uim_req_buf_static_ptr->apdu_hdr.p1 = UIM_USIM_SELECT_BY_PATH_FROM_MF;
    uim_req_buf_static_ptr->apdu_hdr.p2 = UIM_UICC_RETURN_FCP_DATA;
    uim_req_buf_static_ptr->apdu_hdr.p3 = 0;

    for(i = 1, j = 0; 
       (i < target_path_ptr->len) &&
       (j < (sizeof(uim_req_buf_static_ptr->req.data) - 1));
        i++)
    {
      uim_req_buf_static_ptr->req.data[j++] = WORD_HI(target_path_ptr->path[i]);
      uim_req_buf_static_ptr->req.data[j++] = WORD_LO(target_path_ptr->path[i]);

      /* Set the param3 as the total number of bytes */
      uim_req_buf_static_ptr->apdu_hdr.p3 += 2;
    }
  } /* UICC Select - performed by path */

  uim_req_buf_static_ptr->rsp_ptr = &uim_ptr->command.rsp_buf;

  uim_send_command(uim_req_buf_static_ptr, uim_ptr);

} /* uim_select_state */


/*===========================================================================

FUNCTION UIM_PREPARE_SELECT_FAILED_ST

DESCRIPTION
  This procedure saves the current select results(which will be
  restored in the SELECT_FAILED_ST response) and sets up a new
  select command to select the last successfully selected directory

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void uim_prepare_select_failed_st
(
  uim_instance_global_type *uim_ptr,
  uim_channel_type          curr_ch,
  uim_apdu_status_type      cmd_status
)
{
  int i = 0;

  /* For buggy cards a MF select on channel 0 is performed
     if there is a AID selection on non-zero channel*/
  if( uim_nv_is_feature_enabled(UIMDRV_FEATURE_ENABLE_EXPLICIT_SELECTION_OF_MF_OR_ADF, uim_ptr) == TRUE )
  {
    if ((UIM_ADF == uim_ptr->command.cmd_ptr->select.file_type) && ( 0 != curr_ch))
    {
      uim_ptr->state.mf_selection.required  = TRUE;
      uim_ptr->state.mf_selection.channel   = 0;
    }
  }

  
  if ((UIM_ADF == uim_ptr->command.cmd_ptr->select.file_type) && 
      (uim_is_aram_aid(uim_ptr->command.cmd_ptr->select.aid)))
  {
    UIMDRV_MSG_HIGH_0(uim_ptr->id,"aram_app_selected failed, Queueing ADF/MF selection on the other channels");

    for(i=0; i<UIM_MAX_CHANNELS; i++)
    {
      if((uim_ptr->state.is_ch_open[i]== TRUE) &&
         (i != curr_ch))
      {
        if(0 != uim_ptr->state.current_path[i].curr_app.aid_length)
        {
          if(uim_is_telecom_aid(uim_ptr->state.current_path[i].curr_app))
          {
            uim_ptr->state.adf_selection.required  = TRUE;
            uim_ptr->state.adf_selection.channel   = i;
            uim_explicit_reselect_mf_aid(uim_ptr);
          }
        }
        else
        {
          uim_ptr->state.mf_selection.required  = TRUE;
          uim_ptr->state.mf_selection.channel   = i;
          uim_explicit_reselect_mf_aid(uim_ptr);
        } 
      }
    }
  }

  if ((UIM_ADF == uim_ptr->command.cmd_ptr->select.file_type) && 
      (uim_is_aram_aid(uim_ptr->command.cmd_ptr->select.aid))
    )
  {
    uim_ptr->state.aram_app.select_aram_failed = TRUE;
    uim_ptr->state.aram_app.channel            = curr_ch;
  }

  if((1 == uim_ptr->command.cmd_ptr->select.path.len &&
     UIM_MF_ID ==uim_ptr->command.cmd_ptr->select.path.path[0]) ||
     (UIM_ADF != uim_ptr->command.cmd_ptr->select.file_type &&
      0 != uim_ptr->state.current_path[curr_ch].curr_app.aid_length &&
      (TRUE == uim_is_telecom_aid(uim_ptr->state.current_path[curr_ch].curr_app)) &&
      2 == uim_ptr->command.cmd_ptr->select.path.len &&
      UIM_MF_ID ==uim_ptr->command.cmd_ptr->select.path.path[0] &&
      UIM_SEL_AID_BY_FILE_ID == uim_ptr->command.cmd_ptr->select.path.path[1]))
  {
    UIMDRV_MSG_HIGH_0(uim_ptr->id,"triggering recovery as MF/7FFF selection failed");

    (void)uim_trigger_silent_recovery((uim_slot_type)(uim_ptr->id+1), 
                                      uim_ptr->command.cmd_ptr,
                                      NULL);
  }

  /* When a select fails try to reselect the last selected directory this
     is done in the select failed state - set up the select here and
     save the sw from the original failure to restore once we've reselected */
  uim_ptr->command.cmd_ptr->select.path_position = 0;

  for(i = 0; i < uim_ptr->state.current_path[curr_ch].path.len; i++)
  {
    uim_ptr->command.cmd_ptr->select.path.path[i] = uim_ptr->state.current_path[curr_ch].path.path[i];
  }
  uim_ptr->command.cmd_ptr->select.path.len = uim_ptr->state.current_path[curr_ch].path.len;
  uim_ptr->command.cmd_ptr->select.file_type           = UIM_EF;

  if(uim_ptr->state.current_path[curr_ch].path.len == 0)
  {
    uim_ptr->command.cmd_ptr->select.path.path[0]        = UIM_MF_ID;
    uim_ptr->command.cmd_ptr->select.path.len            = 1;
    uim_ptr->command.cmd_ptr->select.file_type           = UIM_MF;
    uim_ptr->command.cmd_ptr->select.aid.aid_length      = 0;
  }

  uim_ptr->command.cmd_ptr->select.reselect.is_failed_select_reselect = TRUE;
  uim_ptr->command.cmd_ptr->select.reselect.sw1        = uim_ptr->command.rpt_buf.sw1;
  uim_ptr->command.cmd_ptr->select.reselect.sw2        = uim_ptr->command.rpt_buf.sw2;
  uim_ptr->command.cmd_ptr->select.reselect.cmd_status = cmd_status;
  uim_ptr->command.cmd_ptr->select.is_intermediate_sel = TRUE;
} /* uim_prepare_select_failed_st */


/*===========================================================================

FUNCTION UIM_STREAM_APDU_SUCCESS

DESCRIPTION
  This procedure handles a successful stream apdu response

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void uim_stream_apdu_success
(
  uim_instance_global_type *uim_ptr,
  uim_req_buf_type         *uim_req_buf_static_ptr,
  uim_rsp_buf_type         *rsp_ptr
)
{
  word    num_bytes_to_alloc = 0;
  byte   *temp_data_ptr      = NULL;
  boolean temp_ef            = FALSE;

  num_bytes_to_alloc = rsp_ptr->cmd_rsp_size +
    uim_ptr->command.rd_wr_info.num_bytes_rsp;

  UIMDRV_MSG_HIGH_2(uim_ptr->id,"New data received = 0x%x, Data already present = 0x%x",
                    rsp_ptr->cmd_rsp_size,
                    uim_ptr->command.rd_wr_info.num_bytes_rsp);

  /*Now both of the size values added above are sizeof(word). 
   As a result, for large values, this can wrap and result in a 
   small value for num_bytes_to_alloc.
   If this happens then there will be a buffer overflow in the following processing. 
   So truncate the response to just 0xFFFF bytes*/
   if(0xFFFF < ((dword)(uim_ptr->command.rd_wr_info.num_bytes_rsp)
                       + (dword)(rsp_ptr->cmd_rsp_size)))
   {
     /*the num_bytes_to_alloc variable has overflown. set it to FFFF bytes*/
     num_bytes_to_alloc = 0xFFFF;

     /*truncate cmd_rsp_size*/ 
     rsp_ptr->cmd_rsp_size = num_bytes_to_alloc - uim_ptr->command.rd_wr_info.num_bytes_rsp;

     UIMDRV_MSG_HIGH_0(uim_ptr->id,"New data received + Data already present exceeds 0xFFFF, truncating it to 0xFFFF bytes");
   }

  /* Add received data to rd/wr buffer */
  if(num_bytes_to_alloc > 0)
  {
    /* Merge new data if data is already present in rd/wr buffer */
    if(uim_ptr->command.rd_wr_info.num_bytes_rsp > 0)
    {
      /* Allocate new buffer for total data */
      temp_data_ptr = uim_malloc(num_bytes_to_alloc);
      if(temp_data_ptr ==  NULL)
      {
        uim_ptr->command.cmd_ptr->hdr.cmd_count = UIM_MAX_NUM_ATTEMPTS;
        return;
      }

      /* Copy over the already present data to the new buffer */
      uim_memscpy(temp_data_ptr ,
             num_bytes_to_alloc,
             uim_ptr->command.rd_wr_info.data_ptr,
             uim_ptr->command.rd_wr_info.num_bytes_rsp);

      /* Increment the pointer to point to the next
       * location to copy the data
       */
      temp_data_ptr += uim_ptr->command.rd_wr_info.num_bytes_rsp;

      /* Copy over the already present data to the buffer */
      uim_memscpy(temp_data_ptr,
             (num_bytes_to_alloc - uim_ptr->command.rd_wr_info.num_bytes_rsp),
             rsp_ptr->rsp.data,
             rsp_ptr->cmd_rsp_size);

      /* Decrement pointer to original location */
      temp_data_ptr -= uim_ptr->command.rd_wr_info.num_bytes_rsp;
    }

    /* Try to allocate mem and reset all the variables */
    if ( !uim_alloc_set_rd_wr_info(num_bytes_to_alloc, uim_ptr) )
    {
      uim_ptr->command.cmd_ptr->hdr.cmd_count = UIM_MAX_NUM_ATTEMPTS;
      UIMDRV_MSG_ERR_2(uim_ptr->id, "Unable to allocate memory for data 0x%x, cmd_rsp_size=0x%x",
                       num_bytes_to_alloc, rsp_ptr->cmd_rsp_size);
      return;
    }

    /* Merge new data if data is already present in rd/wr buffer */
    if(num_bytes_to_alloc > rsp_ptr->cmd_rsp_size)
    {
      /* Copy over the merged data to the buffer provided each time we have
       * a peice of data returned
       */
      uim_memscpy(uim_ptr->command.rd_wr_info.data_ptr ,
             num_bytes_to_alloc,
             temp_data_ptr,
             num_bytes_to_alloc);
    }
    else
    {
      /* Copy over the data to the buffer provided each time we have
       * a peice of data returned
       */
      uim_memscpy(uim_ptr->command.rd_wr_info.data_ptr ,
             num_bytes_to_alloc,
             rsp_ptr->rsp.data,
             num_bytes_to_alloc);
    }

    uim_ptr->command.rd_wr_info.num_bytes_rsp = num_bytes_to_alloc;

    if ( temp_data_ptr != NULL )
    {
      UIM_FREE(temp_data_ptr);
    }
  }


  /* Send Get Response if intermediate SW1 is 0x61 */
  /*But ignore 0x61 if the data received has reached 0xFFFF*/
  if((rsp_ptr->sw1 == SW1_USIM_END_RESP) &&
     (0xFFFF > uim_ptr->command.rd_wr_info.num_bytes_rsp))
  {
    if(uim_req_buf_static_ptr->apdu_hdr.instrn != GET_RESPONSE)
    {
      uim_save_current_req_buf(uim_ptr);
    }
    /* Add state to do get response */
    uim_ptr->mode.generic_states[0] = UIM_GET_RESPONSE_ST;
    uim_ptr->mode.generic_states[1] = UIM_STREAM_APDU_ST;
    uim_ptr->mode.generic_states[2] = UIM_DONE_ST;
    uim_ptr->command.generic_state_ptr = uim_ptr->mode.generic_states;
  }
  else
  {
    /* If it is a select on the default channel we need to update curr path */
    if(uim_ptr->command.cmd_ptr->stream_iso7816_apdu.is_select_default_ch)
    {
      /* UICC and ICC have different response structure*/
      /* At this point, response from the card contains file type */
      if (uim_ptr->command.cmd_ptr->hdr.protocol == UIM_UICC)
      {
      uim_parse_uicc_select_response(
        uim_ptr,
        uim_ptr->command.rd_wr_info.data_ptr,
        uim_ptr->command.rd_wr_info.num_bytes_rsp,
       &temp_ef,
        NULL,
        NULL);

        uim_ptr->state.current_path[UIM_DEFAULT_CHANNEL].is_ef = temp_ef;
    }
      else if (uim_ptr->command.cmd_ptr->hdr.protocol == UIM_ICC)
      {
        temp_ef = rsp_ptr->rsp.ef.file_type == EF ? TRUE : FALSE;

#ifdef FEATURE_UIM_TEST_FRAMEWORK
        #error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */
        uim_ptr->state.current_path[UIM_DEFAULT_CHANNEL].is_ef = temp_ef;
      }
    }

    if(uim_req_buf_static_ptr->apdu_hdr.instrn == GET_RESPONSE)
    {
      uim_restore_current_req_buf(uim_ptr);
    }
    /* Skip to the next state. */
    ++uim_ptr->command.generic_state_ptr;
  }
} /* uim_stream_apdu_success */


/*===========================================================================

FUNCTION UIM_CHECK_CHARACTERISTICS_GET_NEXT_DF

DESCRIPTION
  We want to attempt to select GSM GSM_DCS_1800(if present) and CDMA  DFs
   (if CDMA_DF_SELECT_BEFORE_TP is active then we try to select CDMA first)
   If any of these selects are successful we'll stop attempting to select an
   ICC DF, but if they fail we want to keep selecting until one is a success

   This function takes the path that was just attempted and returns the next DF
   to select. In a normal case it would be called once and the first DF would
  return success - it is only called multiple times if there are failures

DEPENDENCIES

RETURN VALUE
  Next DF to select during ICC power-up

SIDE EFFECTS

===========================================================================*/
uint16 uim_check_characteristics_get_next_df(uim_instance_global_type *uim_ptr, uint16 curr_df)
{
  uint16                     dfs_to_select[3] = {0x0000, 0x0000, 0x0000};
  int                        index            = 0;
  int                        i                = 0;
  mmgsdi_return_enum_type    mmgsdi_status    = MMGSDI_SUCCESS;
  mmgsdi_cached_nv_item_data nv_item;

  memset(&nv_item, 0x00, sizeof(mmgsdi_cached_nv_item_data));

  if (uim_ptr->flag.uim_auth_cdma_df_first)
  {
    dfs_to_select[index++] = UIM_CDMA_DF;
  }

  if(!(uim_ptr->setting.config_params.future_use[UIM_RFU_INDEX_BOOLEAN_FLAGS] &
      UIM_RFU_MASK_NOT_SELECT_GSM_DF))
  {
    dfs_to_select[index++] = UIM_GSM_DF;
  }

  nv_item.nv_item_enum = MMGSDI_NV_UIM_GSM_DCS_1800_I;
  mmgsdi_status        = mmgsdi_get_cached_nv_item_ext(&nv_item, (uint8)uim_ptr->id);

  if(mmgsdi_status == MMGSDI_SUCCESS &&
     nv_item.nv_item_data.uim_gsm_dcs_1800 == TRUE)
  {
    dfs_to_select[index++] = UIM_DCS_1800_DF;
  }

  if(uim_ptr->flag.uim_auth_cdma_df_first == FALSE)
  {
    dfs_to_select[index++] = UIM_CDMA_DF;
  }

  if( index == 0 || curr_df == dfs_to_select[index-1])
  {
    /* if index is 0
       or the current df was the last in the list to select
       we don't have a next df return an invalid ID */
    return 0x0000;
  }

  for(i = 0; i < index-1; i++)
  {
    if(i < 2 && curr_df == dfs_to_select[i])
    {
      return dfs_to_select[i + 1];
    }
  }
  return dfs_to_select[0];
} /* uim_check_characteristics_get_next_df */


/*===========================================================================

FUNCTION UIM_GENERIC_COMMAND

DESCRIPTION
  This procedure is the main state machine for Generic UIM command processing.

DEPENDENCIES
  This function uses the UIM generic state pointer to determine what to process.

RETURN VALUE
  None

SIDE EFFECTS
  Sends a command to the UIM.

===========================================================================*/
void uim_generic_command
(
  uim_instance_global_type *uim_ptr
)
{
  byte FI = 0, DI = 0; /* Transmission factors from ATR */
  boolean  is_lpd_supported = FALSE;
  boolean  is_lui_supported = FALSE;

  /* Pointer to the Global UIM Request buffer */
  uim_req_buf_type *uim_req_buf_static_ptr = NULL;

#ifdef FEATURE_UIM_USB_UICC
  rex_sigs_type     sigs_received          = (rex_sigs_type)NULL;
#endif /* FEATURE_UIM_USB_UICC */

  uim_ptr->card_cmd.cur_channel = uim_ptr->command.cmd_ptr->hdr.channel;
  uim_req_buf_static_ptr = &uim_ptr->command.req_buf;

  if (uim_ptr->command.cmd_ptr == NULL)
  {
    UIMDRV_MSG_ERR_0(uim_ptr->id, "uim_generic_command cmd_ptr is NULL");
    return;
  }

  /* Set the protocol from the command to the UIM request buffer */
  uim_req_buf_static_ptr->protocol = uim_ptr->command.cmd_ptr->hdr.protocol;

  if(uim_nv_is_feature_enabled(UIMDRV_FEATURE_UIM_POLLING_ONLY_AT_POLLING_TIMER_EXPIRY,
                                 uim_ptr) == FALSE)
  {
    uim_set_poll_timer( uim_ptr->setting.poll_time_delay, uim_ptr->setting.wakeup_poll_offset, uim_ptr);
  }

  /* Build an APDU based on the UIM generic state */
  switch(*uim_ptr->command.generic_state_ptr)
  {
    case UIM_POWER_DOWN_ST:
      {
        UIMDRV_MSG_ERR_0(uim_ptr->id, "Turning off UIM because of POWER down cmd");
        uim_power_down(uim_ptr);
#ifdef FEATURE_UIM_BTSAP_CLIENT
        if(IS_BTSAP_HANDLE_ACTIVE(uim_ptr))
        {
          UIMDRV_MSG_HIGH_0(uim_ptr->id,"Calling QMI remote API to power down the remote SIM");
          /*cmd_rsp_sig is set in the error event which is generated due to power down */
          uim_remote_card_power_down(uim_ptr);
        }
#endif /* FEATURE_UIM_BTSAP_CLIENT */

#ifdef FEATURE_UIM_USB_UICC
        if(uim_is_usb_enabled(uim_ptr))
        {
          /* Reset usb related globals when power down ISO card,
             since after power down it is possible a USB UICC inserted */
          uim_usb_reset_globals(uim_ptr, FALSE);
        }
#endif /* FEATURE_UIM_USB_UICC */

        /* We are done, notify UIM */
        (void)rex_set_sigs(uim_ptr->tcb_ptr, UIM_CMD_RSP_SIG);
        /* Set the response command status as done */
        uim_ptr->command.rsp_buf.cmd_status = UIM_DONE;
      }
      break;

    case UIM_POWER_UP_ST:                     /* Power Up state */
      {
        /* Initialize the warm reset count to zero for each powerup */
        uim_ptr->debug.warm_reset_count = 0;
        uim_ptr->command.rsp_buf.sw1 = SW1_NORMAL_END;
        uim_ptr->command.rsp_buf.sw2 = SW2_NORMAL_END;
        uim_ptr->flag.uim_auth_cdma_df_first = FEATURE_UIM_AUTH_CDMA_DF_FIRST_DEFAULT;

        /* The path elements are used by the reentrant UIM_CHECK_CHARACTERISTICS_ST
           clear the path here so that it's ready when we enter this state. */
        memset(&uim_ptr->command.cmd_ptr->select.path, 0x00, sizeof(uim_path_type));

        memset(&uim_ptr->command.rsp_buf, 0x00, sizeof(uim_ptr->command.rsp_buf));

        /* If this is the first time we are executing this command and if the
           uim_first_instn_class is set to GSM, then toggle the flag */
        if (uim_ptr->command.cmd_ptr->hdr.cmd_count == 1)
        {
          if (uim_ptr->state.first_inst_class == NV_UIM_FIRST_INST_CLASS_GSM_SIM)
          {
            uim_ptr->flag.toggle_instrn_class = TRUE;
            /* Set baud rate and clk to 3.8MHz for GSM INST class if CLK is at 4.8MHz */
            UIMDRV_MSG_HIGH_0(uim_ptr->id, "GSM instruction class  - evaluate clk freq change");
            if (UIM_IS_CLK_FREQ_AT_4_8_MHZ(uim_ptr->id))
            {
              UIMDRV_MSG_HIGH_0(uim_ptr->id, "GSM instruction class  - clk freq change required to 3.8MHz");
              uim_power_down(uim_ptr);
              UIM_SWITCH_CLK_FREQ_TO_3_8_MHZ(uim_ptr->id);
              SET_UIM_BAUD_RATE_SLOT ( uim_ptr->id, UIM_CRCF_372_1, UIM_BRAF_1 );
            }
          }
        }
        uim_power_up(uim_ptr);
#if defined( FEATURE_UIM_T_1_SUPPORT )
        /* Set the current protocol to T=0 during power up */
        uim_ptr->state.current_protocol = UIM_T_0_PROTOCOL;
#endif /* FEATURE_UIM_T_1_SUPPORT */
        /* Signal is set internally for proper operation of state machine */

#ifdef FEATURE_UIM_BTSAP_CLIENT
        if(IS_BTSAP_HANDLE_ACTIVE(uim_ptr))
        {
          (void)rex_set_sigs(uim_ptr->tcb_ptr,UIM_CMD_RSP_SIG);
           break;
        }
#endif /* FEATURE_UIM_BTSAP_CLIENT */
        /* This intentional wait is to delay the reset by 20ms. The delay
           timer is associated with the UIM_CMD_RSP_SIG. */
        if(uim_nv_is_feature_enabled(UIMDRV_FEATURE_INTERFACE_NOT_USED,
                                     uim_ptr) == FALSE)
        {
          uim_timed_sleep(UIM_CARD_DELAY_TIMEOUT, uim_ptr, UIM_ALL_ZERO_SIG);
        (void)rex_set_sigs(uim_ptr->tcb_ptr, UIM_CMD_RSP_SIG);
        }
      } /* end case - UIM_POWER_UP_ST */
      break;

    case UIM_RESET_ST:                        /* Reset State */
      {
        /* Indicate that we have not yet received an ATR */
        uim_ptr->atr.atr_received = FALSE;
        uim_ptr->flag.uicc_supports_swp = FALSE;
        /* Set the initial work waiting time */
        /* Compute a new work waiting time */
        if(uim_hw_if.clkreg[uim_ptr->id].m_simClkFreq == 0)
        {
          UIMDRV_MSG_ERR_0(uim_ptr->id, "uim_clk_freq can not be zero");
          return;
        }
        if(uim_is_fi_di_supported(uim_ptr, uim_ptr->state.op_params_buf.FI,uim_ptr->state.op_params_buf.DI))
        {
          uim_ptr->card_cmd.work_waiting_time = ((960 * 10) *
                                crcf_values[UIM_CRCF_372_1]) /
                               (uim_hw_if.clkreg[uim_ptr->id].m_simClkFreq / 1000) +
            (480 * crcf_values[UIM_CRCF_372_1]) /
            ( uim_hw_if.clkreg[uim_ptr->id].m_simClkFreq / 1000);
          uim_ptr->card_cmd.work_waiting_time_etus = 960*10 + 480;
        }
        else
        {
          uim_ptr->card_cmd.work_waiting_time = ((960 * 10) *
                                crcf_values[UIM_CRCF_372_1]) /
                               (uim_hw_if.clkreg[uim_ptr->id].m_simClkFreq / 1000) +
                                UIM_CLK_MS_PER_TICK;
          uim_ptr->card_cmd.work_waiting_time_etus = 960*10;
        }

#if defined(FEATURE_UIM_T_1_SUPPORT)
        /* for T=1 protocol, Block Guard Time is 22 Etu's */
        uim_ptr->t1.t_1_bgt = (BGT_22_ETU * crcf_values[UIM_CRCF_372_1]) /
                       (uim_hw_if.clkreg[uim_ptr->id].m_simClkFreq / 1000) + UIM_CLK_MS_PER_TICK;
#endif
        /* Set the flag to FALSE */
        uim_ptr->atr.atr_pps_done= FALSE;
        uim_ptr->flag.invalid_pps_received = FALSE;

        if (uim_nv_is_feature_enabled(UIMDRV_FEATURE_HANDLE_TC1_BYTE_FOR_EXTRA_GUARD_TIME, uim_ptr) == FALSE)
        {
          /* Do not use the minimum guardtime.  This is determined in the
             ATR */
          uim_ptr->t1.use_minimum_guardtime_size = FALSE;
        }

#ifdef FEATURE_UIM_BTSAP_CLIENT
        if(IS_BTSAP_HANDLE_ACTIVE(uim_ptr))
        {
        UIMDRV_MSG_HIGH_0(uim_ptr->id,"UIM in BTSAP client mode.Call BTSAP API's to power-up remote card");
          if(!(uim_ptr->remote.btsap.handle & UIM_BTSAP_CARD_CONNCECTTION_ENABLE_MASK) )
          {
            /* Call card_connect API for the first time */
            uim_remote_card_connect(uim_ptr);
          }
          else if(uim_ptr->command.cmd_ptr->hdr.command == UIM_HOTSWAP_CARD_INS_F)
          {
            /* Call card_reset API during HOTSWAP INSERTED command */
            uim_remote_card_reset(uim_ptr);
          }
          else
          {
            /* Call card_power_up API from second time onwards */
            uim_remote_card_power_up(uim_ptr);
          }
        }
        else
#endif /* FEATURE_UIM_BTSAP_CLIENT */
        {
          /* Reset the UIM card */
          uim_reset( &uim_ptr->command.rsp_buf, uim_ptr);
        }

        /* The protocol is used in one case for voltage control
           while parsing the ATR in the UIM_RESET_ST(response)
           set it properly here */
        if ( !uim_ptr->flag.toggle_instrn_class )
        {
          /* Set the protocol to UICC */
          uim_ptr->command.cmd_ptr->hdr.protocol = UIM_UICC;
          uim_ptr->state.dfs_1.protocol          = UIM_UICC;
        }
        else
        {
          /* Set the protocol to ICC */
          uim_ptr->command.cmd_ptr->hdr.protocol = UIM_ICC;
          uim_ptr->state.dfs_1.protocol          = UIM_ICC;
        }
      } /* end case - UIM_RESET_ST */
      break;
    case UIM_DELAY_AFTER_ATR_ST:              /* Introduce delay after ATR */
      {
        /* This intentional wait is to delay sending the PPS request after
         * the reception of the ATR.  To avoid sending the first PPS byte
         * within 12 ETU of the last ATR byte received.  This could happen
         * based on when the last byte is received.. if the byte is already
         * present in the FIFO by the time the ISR is reading bytes out
         */
        uim_timed_sleep(UIM_DELAY_TIME_AFTER_ATR, uim_ptr, UIM_ALL_ZERO_SIG);
        (void)rex_set_sigs(uim_ptr->tcb_ptr,UIM_CMD_RSP_SIG);

      }
      break;
    case UIM_PPS_ST:                          /* PPS State */
      {
#ifdef FEATURE_UIM_USB_UICC
        UIMDRV_MSG_HIGH_3(uim_ptr->id, "usb enabled 0x%x usb mode supported 0x%x fallback occured 0x%x",
                     uim_is_usb_enabled(uim_ptr),
                     uim_ptr->usb.usb_mode_card_supported,
                     uim_usb_iso_fallback_occurred(uim_ptr));
        UIMDRV_MSG_HIGH_1(uim_ptr->id, "usb has remote connection 0x%x",
                     uim_usb_has_remote_connection(uim_ptr));
        if (uim_is_usb_enabled(uim_ptr) &&
            uim_usb_has_remote_connection(uim_ptr) &&
            uim_usb_is_usb_mode_card_supported(uim_ptr) &&
            !uim_usb_iso_fallback_occurred(uim_ptr))
        {
          UIMDRV_MSG_HIGH_0(uim_ptr->id, "Sending USB PPS");
          uim_ptr->atr.pps_req_buf.size_of_pps_request = UIM_PPS_REQ_SIZE;
          uim_ptr->atr.pps_char_buf[0] = UIM_PPS_DEFAULT_PPSS;
          uim_ptr->atr.pps_char_buf[1] = UIM_PPS_USB_PPS0;
          uim_ptr->atr.pps_char_buf[2] = UIM_PPS_USB_PPS2;
          uim_ptr->atr.pps_char_buf[3] =
            (uim_ptr->atr.pps_char_buf[0] ^ uim_ptr->atr.pps_char_buf[1] ^ uim_ptr->atr.pps_char_buf[2]);
        }
        else
#endif /* FEATURE_UIM_USB_UICC */
        {
          UIMDRV_MSG_HIGH_0(uim_ptr->id, "Sending Default PPS");
          /* This uses TA(1) from the ATR to negotiate the PPS. */
          uim_ptr->atr.pps_char_buf[0] = UIM_PPS_DEFAULT_PPSS;
          /* Separate FI from DI */
          FI = uim_ptr->command.rsp_buf.rsp.data[UIM_TA1_INDEX] >> UIM_FI_SHIFT_OF_TA1;
          DI = uim_ptr->command.rsp_buf.rsp.data[UIM_TA1_INDEX] & UIM_DI_MASK_OF_TA1;
          /* Check if TA1 is valid */
          /* Default baud-rate for a card with F=372 and D=12 */
          if (((uim_nv_is_feature_enabled(
                UIMDRV_FEATURE_TRY_DEFAULT_BAUD_RATE_FOR_F372_D12_CARD,
                uim_ptr) == TRUE)
                && ((braf_values[DI] == 0) || (crcf_values[FI] == 0) ||
                   (UIM_F372_D12_COMB1 == uim_ptr->command.rsp_buf.rsp.data[UIM_TA1_INDEX]) ||
                   (UIM_F372_D12_COMB2 == uim_ptr->command.rsp_buf.rsp.data[UIM_TA1_INDEX])) )
              || ((uim_nv_is_feature_enabled(
                  UIMDRV_FEATURE_TRY_DEFAULT_BAUD_RATE_FOR_F372_D12_CARD,
                  uim_ptr) == FALSE)
                 && ((braf_values[DI] == 0) || (crcf_values[FI] == 0))) )
          {
            uim_ptr->atr.pps_req_buf.size_of_pps_request = UIM_PPS_DEFAULT_REQ_SIZE;
            uim_ptr->atr.pps_char_buf[1] = UIM_PPS_DEFAULT_PPS0;
            uim_ptr->atr.pps_char_buf[2] = UIM_PPS_DEFAULT_PCK;
          }
          else
          {
            /* If we are in the third reset attempt */
            if (uim_ptr->command.cmd_ptr->hdr.cmd_count > (UIM_MAX_NUM_ATTEMPTS - 1))
            {
              uim_ptr->atr.pps_req_buf.size_of_pps_request = UIM_PPS_DEFAULT_REQ_SIZE;
              uim_ptr->atr.pps_char_buf[1] = UIM_PPS_DEFAULT_PPS0;
              uim_ptr->atr.pps_char_buf[2] = UIM_PPS_DEFAULT_PCK;
            }
            else
            {
              uim_ptr->atr.pps_req_buf.size_of_pps_request = UIM_PPS_REQ_SIZE;
              uim_ptr->atr.pps_char_buf[1] = (int)UIM_PPS_PPS1_PPS0
#if defined( FEATURE_UIM_T_1_SUPPORT )
                | (int)uim_ptr->state.operating_protocol
#endif /* FEATURE_UIM_T_1_SUPPORT */
              ;
              uim_ptr->atr.pps_char_buf[2] = uim_ptr->command.rsp_buf.rsp.data[UIM_TA1_INDEX];

#if defined( FEATURE_UIM_T_1_SUPPORT )
              if (UIM_T_1_PROTOCOL == (int)uim_ptr->state.operating_protocol)
              {
                if (!(uim_ptr->atr.pps_char_buf[2] == UIM_NO_PPS_VALUE1 ||
                      uim_ptr->atr.pps_char_buf[2] == UIM_NO_PPS_VALUE2))
                {
                  /* CWT value should be greater than or equal to 2ms to support the current design with Max Clock */
                  uint32 cwt_in_ms = (uint32)( ((crcf_values[FI] * 1000) * (11 + (1 << uim_ptr->t1.t_1_cwi))) /
                                                (uim_hw_if.clkreg[uim_ptr->id].m_simClkFreq * braf_values[DI]));
                  if ( cwt_in_ms < 2)
                  {
                    uim_ptr->atr.pps_char_buf[2] = UIM_NO_PPS_VALUE1;
                    UIMDRV_MSG_HIGH_1(uim_ptr->id,
                       "Move to default baud rate for T1 protocol due to less value of cwt 0x%x in ms", cwt_in_ms);
                  }
                }
              }
#endif /* FEATURE_UIM_T_1_SUPPORT */

              uim_ptr->atr.pps_char_buf[3] =
                (uim_ptr->atr.pps_char_buf[0] ^ uim_ptr->atr.pps_char_buf[1] ^ uim_ptr->atr.pps_char_buf[2]);
            }
          }
        }
        uim_ptr->atr.pps_req_buf.request_buf = uim_ptr->atr.pps_char_buf;
        uim_ptr->atr.pps_req_buf.rsp_ptr = &uim_ptr->command.rsp_buf;
        uim_send_pps(&uim_ptr->atr.pps_req_buf, uim_ptr);
      } /* end case - UIM_PPS_ST */
      break;

    case UIM_UPDATE_OP_PARAMS_ST:  /* Update Operational Parameters State */
      {
        /* Fill out the operational parameters as default values */
        uim_ptr->state.op_params_buf.change_baud_rate = TRUE;
        uim_ptr->state.op_params_buf.change_clk_freq = TRUE;
        uim_ptr->state.op_params_buf.change_guardtime = TRUE;
        uim_ptr->state.op_params_buf.FI = UIM_CRCF_372_1;
        uim_ptr->state.op_params_buf.DI = UIM_BRAF_1;
        if (uim_hw_if.clkreg[uim_ptr->id].m_simClkFreq ==
                       uim_clock_frequency[UIMDRV_CLK_FREQ_3_84_MHZ])
        {
          uim_ptr->state.op_params_buf.clock_frequency =
                                              UIMDRV_CLK_FREQ_3_84_MHZ;
        }
        else if(uim_hw_if.clkreg[uim_ptr->id].m_simClkFreq ==
                       uim_clock_frequency[UIMDRV_CLK_FREQ_4_8_MHZ])
        {
          uim_ptr->state.op_params_buf.clock_frequency =
                                              UIMDRV_CLK_FREQ_4_8_MHZ;
        }
        /* Determine if there is an interface byte to process.  This code
           operates on the ATR as well as the PPS response. */
        if (uim_ptr->command.rsp_buf.rsp.data[UIM_ATR_T0_BYTE] & (UIM_ATR_TA_PRESENT))
        {
          /* Set the op parameters from the ATR/PPS response */
          uim_ptr->state.op_params_buf.FI = (uim_clock_rate_conversion_factor_type)
            (uim_ptr->command.rsp_buf.rsp.data[UIM_TA1_INDEX] >> UIM_FI_SHIFT_OF_TA1);
          uim_ptr->state.op_params_buf.DI = (uim_baud_rate_adjustment_factor_type)
            (uim_ptr->command.rsp_buf.rsp.data[UIM_TA1_INDEX] & UIM_DI_MASK_OF_TA1);
        } /* end if - TA(1) does exist in the ATR/PPS. */

        if (uim_nv_is_feature_enabled(UIMDRV_FEATURE_HANDLE_TC1_BYTE_FOR_EXTRA_GUARD_TIME, uim_ptr) == FALSE)
        {
          /* Use the default guardtime size. */
          uim_ptr->state.op_params_buf.guardtime_bits = UIM_DEFLT_GUARDTIME_BITS;
        }

#if defined( FEATURE_UIM_T_1_SUPPORT )
        if( braf_values[uim_ptr->state.op_params_buf.DI] != 0 &&
            uim_hw_if.clkreg[uim_ptr->id].m_simClkFreq != 0 )
        {
          /* Set the waiting times for T=1 protocol */
          /* Setting Block Waiting Time */
          uim_ptr->t1.t_1_bwt =  (rex_timer_cnt_type) (( (11 +
                             (uim_ptr->setting.config_params.future_use[UIM_RFU_INDEX_EXTRA_BWT]*
                              UIM_EXTRA_BWT_MUL_FACTOR)
                             )* crcf_values[uim_ptr->state.op_params_buf.FI] * 1000) /
                          ( braf_values[uim_ptr->state.op_params_buf.DI] *
                             uim_hw_if.clkreg[uim_ptr->id].m_simClkFreq ))
                          + (rex_timer_cnt_type) (  ((1 << uim_ptr->t1.t_1_bwi) * 960 *
                              crcf_values[UIM_CRCF_372_1] ) /
                            (uim_hw_if.clkreg[uim_ptr->id].m_simClkFreq / 1000) )
                          + UIM_CLK_MS_PER_TICK + UIM_EXTRA_BGT_TIME;
          uim_ptr->t1.t_1_bwt_etus = 11 + (((1 << uim_ptr->t1.t_1_bwi) * 960 *
                                 braf_values[uim_ptr->state.op_params_buf.DI] * crcf_values[UIM_CRCF_372_1]) / crcf_values[uim_ptr->state.op_params_buf.FI]);

          /* Setting Character Waiting Time */
          uim_ptr->t1.t_1_cwt = ((11 + (1 << uim_ptr->t1.t_1_cwi) +
                          (uim_ptr->setting.config_params.future_use[UIM_RFU_INDEX_EXTRA_CWT]*
                           UIM_EXTRA_CWT_MUL_FACTOR)
                         ) * crcf_values[uim_ptr->state.op_params_buf.FI]) /
                         ((uim_hw_if.clkreg[uim_ptr->id].m_simClkFreq / 1000) *
                          braf_values[uim_ptr->state.op_params_buf.DI]) + UIM_CLK_MS_PER_TICK;

          uim_ptr->t1.t_1_cwt_etus = 11 + (1 << uim_ptr->t1.t_1_cwi);

            /* for T=1 protocol, Block Guard Time is 22 Etu's */
          uim_ptr->t1.t_1_bgt = ((BGT_22_ETU +
                          (uim_ptr->setting.config_params.future_use[UIM_RFU_INDEX_EXTRA_BGT]*
                          UIM_EXTRA_BGT_MUL_FACTOR)
                         )* crcf_values[uim_ptr->state.op_params_buf.FI]) /
                         ((uim_hw_if.clkreg[uim_ptr->id].m_simClkFreq / 1000) *
                          braf_values[uim_ptr->state.op_params_buf.DI]) + UIM_CLK_MS_PER_TICK;
         }
         else
         {
           UIMDRV_MSG_ERR_1(uim_ptr->id,"Either Invalid DI value 0x%x or uim_clk_freq is zero", 
                                     braf_values[uim_ptr->state.op_params_buf.DI]);
           uim_ptr->flag.clear_timeout_on_response = FALSE;
           (void) rex_set_sigs( uim_ptr->tcb_ptr, UIM_CMD_RSP_TIMEOUT_SIG );
           uim_ptr->flag.invalid_pps_received = TRUE;
           break;
         }

         if (uim_nv_is_feature_enabled(UIMDRV_FEATURE_HANDLE_TC1_BYTE_FOR_EXTRA_GUARD_TIME, uim_ptr) == FALSE)
         {
           /* Determine if the guard bit time in the minimum for T=1 */
           if ((UIM_T_1_PROTOCOL == uim_ptr->state.operating_protocol) &&
               (uim_ptr->t1.use_minimum_guardtime_size))
           {
             /* Use the default guardtime size. */
             uim_ptr->state.op_params_buf.guardtime_bits = UIM_MINIMUM_T_1_GUARDTIME;
           }
         }

#endif /* FEATURE_UIM_T_1_SUPPORT */

         if (uim_nv_is_feature_enabled(UIMDRV_FEATURE_HANDLE_TC1_BYTE_FOR_EXTRA_GUARD_TIME, uim_ptr) == TRUE)
         {
            /* Determine the guard-time based on the ATR */
            uim_ptr->state.op_params_buf.guardtime_bits = uim_calculate_guard_time(uim_ptr);
         }

        /* Add extra guard time from uim config params NV 4205 */
        uim_ptr->state.op_params_buf.guardtime_bits +=
          uim_ptr->setting.config_params.future_use[UIM_RFU_INDEX_EXTRA_GUARD_TIME];

        /* Send the operational parameters */
        uim_update_op_params( &uim_ptr->state.op_params_buf, uim_ptr);

        if (uim_ptr->setting.config_params.future_use[UIM_RFU_INDEX_EXTRA_WWT] == 0)
        {
          /* Initialize Extra Work Waiting Time to 1 to prevent multiplication by zero */
          uim_ptr->setting.config_params.future_use[UIM_RFU_INDEX_EXTRA_WWT] = 1;
        }
        /* Compute a new work waiting time */
        if(uim_hw_if.clkreg[uim_ptr->id].m_simClkFreq == 0)
        {
          UIMDRV_MSG_ERR_0(uim_ptr->id, "uim_clk_freq can not be zero");
          return;
        }
        if(braf_values[uim_ptr->state.op_params_buf.DI] != 0)
        {
          uim_ptr->card_cmd.work_waiting_time = ((960 * uim_ptr->state.WI) *
                                uim_ptr->setting.config_params.future_use[UIM_RFU_INDEX_EXTRA_WWT] *
                                crcf_values[uim_ptr->state.op_params_buf.FI]) /
                               (uim_hw_if.clkreg[uim_ptr->id].m_simClkFreq / 1000) +
            (480 * crcf_values[uim_ptr->state.op_params_buf.FI]) /
            (braf_values[uim_ptr->state.op_params_buf.DI] * ( uim_hw_if.clkreg[uim_ptr->id].m_simClkFreq / 1000));
          uim_ptr->card_cmd.work_waiting_time_etus = (960 * uim_ptr->state.WI * braf_values[uim_ptr->state.op_params_buf.DI]) + 480;

        }
        else
        {
          uim_ptr->card_cmd.work_waiting_time = ((960 * uim_ptr->state.WI) *
                                uim_ptr->setting.config_params.future_use[UIM_RFU_INDEX_EXTRA_WWT] *
                                crcf_values[uim_ptr->state.op_params_buf.FI]) /
                               (uim_hw_if.clkreg[uim_ptr->id].m_simClkFreq / 1000) + UIM_CLK_MS_PER_TICK;
          uim_ptr->card_cmd.work_waiting_time_etus = (960 * uim_ptr->state.WI) + 480;
        }

        /* Extend work waiting time for some out of spec RUIM cards */
        uim_ptr->card_cmd.original_work_waiting_time =
                                        uim_ptr->card_cmd.work_waiting_time;
        uim_ptr->card_cmd.original_work_waiting_time_etus =
                                        uim_ptr->card_cmd.work_waiting_time_etus;
        uim_ptr->card_cmd.extended_work_waiting_time =
                                    2 * uim_ptr->card_cmd.work_waiting_time;
        uim_ptr->card_cmd.extended_work_waiting_time_etus =
                                    2 * uim_ptr->card_cmd.work_waiting_time_etus;

        /* Signal is set internally for proper operation of state machine */
        uim_timed_sleep(UIM_CARD_DELAY_TIMEOUT, uim_ptr, UIM_ALL_ZERO_SIG);
        (void)rex_set_sigs(uim_ptr->tcb_ptr, UIM_CMD_RSP_SIG);


      } /* end case - UIM_UPDATE_OP_PARAMS_ST */
      break;

#if defined( FEATURE_UIM_T_1_SUPPORT )
    case UIM_IFS_NEG_ST:
      {
        /* This command does not use these parameters.  Set them to a benign
           command, just in case */
        uim_req_buf_static_ptr->instrn_case = UIM_INSTRN_IFS_BLOCK;

        uim_req_buf_static_ptr->rsp_ptr = &uim_ptr->command.rsp_buf;

        /* This is the IFS parameter that indicates the maximum number of bytes
           the MSM can accept in a block */
        uim_req_buf_static_ptr->req.data[0] =
          int32touint8(UIM_MAX_TRANSACTION_SIZE -
                       (UIM_T_1_NUM_BYTES_PROLOGUE + uim_ptr->t1.epilogue_field_size));

        uim_send_command(uim_req_buf_static_ptr, uim_ptr);
      } /* end case - UIM_IFS_NEG_ST */
      break;
#endif /* FEATURE_UIM_T_1_SUPPORT */

     /* For now, we are assuming that reads and writes to the UIM are
       not going to exceed 255 bytes */

    case UIM_READ_ICCID_ST:
      {
        if(uim_nv_is_feature_enabled(UIMDRV_FEATURE_SUPPORT_NO_ICCID,
                                     uim_ptr) == TRUE)
        {
          /* Skip read iccid if select failed. */
          if (uim_ptr->flag.iccid_select_failed)
          {
            UIMDRV_MSG_HIGH_0(uim_ptr->id,"uim_ptr->flag.iccid_select_failed is TRUE");
            uim_ptr->flag.iccid_select_failed = FALSE;

            uim_ptr->command.generic_state_ptr++;
            if (uim_ptr->command.cmd_ptr->hdr.command == UIM_INTERNAL_WAKE_UP_F)
            {
              if(MMGSDI_SUCCESS ==
                 uim_sm_mmgsdi_get_recovery_data_sync(&uim_ptr->state.recovery_data,
                                               (uim_slot_type)(uim_ptr->id + 1)))
              {
                uim_ptr->command.cmd_ptr->hdr.protocol = uim_ptr->state.recovery_data.protocol;
                if (uim_ptr->command.cmd_ptr->hdr.protocol == UIM_UICC)
                {
                  UIMDRV_MSG_HIGH_0(uim_ptr->id,"Recovering UICC");
                  /* Proceed to recover UICC card */
                  uim_ptr->command.generic_state_ptr = UIM_RECOVER_UICC_SLOT_STATES;
                }
                else
                {
                  UIMDRV_MSG_HIGH_0(uim_ptr->id,"Recovering ICC");
                  /* Proceed to recover ICC card */
                  uim_ptr->command.generic_state_ptr = UIM_RECOVER_ICC_SLOT_STATES;
                }
              }
              else
              {
                UIMDRV_MSG_ERR_0(uim_ptr->id,"mmgsdi_get_recovery_data_sync returned Error");
              }
            }
            /* go to process next command */
            uim_generic_command (uim_ptr);
            break;
          }
        }

        uim_req_buf_static_ptr->apdu_hdr.instrn = READ_BINARY;
        uim_req_buf_static_ptr->instrn_case = UIM_INSTRN_CASE_2;
        uim_req_buf_static_ptr->apdu_hdr.p1 = UIM_ICCID_OFFSET;
        uim_req_buf_static_ptr->apdu_hdr.p2 = UIM_ICCID_OFFSET;
        uim_req_buf_static_ptr->apdu_hdr.p3 = (uim_ptr->cache.iccid_buf.num_bytes > 0) ? 
                                              uim_ptr->cache.iccid_buf.num_bytes : 
                                              UIM_ICCID_SIZE;

        /* Specifically select the ICCID EF. */
        if (uim_ptr->command.cmd_ptr->hdr.protocol == UIM_UICC)
        {
           uim_req_buf_static_ptr->apdu_hdr.uim_class = UIM_UICC_INSTRUCTION_CLASS1;
        }
        else
        {
           uim_req_buf_static_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;
        }

        uim_req_buf_static_ptr->rsp_ptr = &uim_ptr->command.rsp_buf;

        uim_send_command(uim_req_buf_static_ptr, uim_ptr);

      } /* end case - UIM_READ_ICCID_ST */
      break;

    /* Try to select the CDMA DF.  Use this for RTRE */
    case UIM_CHECK_FOR_CDMA_DF:
      {
        if (uim_ptr->command.cmd_ptr->hdr.protocol == UIM_UICC)
        {
          /* Skip the CDMA DF check if UICC protocol has been chosen */
          ++uim_ptr->command.generic_state_ptr;
          /* Process the next state. */
          uim_generic_command (uim_ptr);
          return;
        }

        /* Reset the select state counter */
        uim_ptr->command.cmd_ptr->select.path_position = 0;

        if (uim_ptr->flag.uim_auth_cdma_df_first == FALSE)
        {
          uim_ptr->command.cmd_ptr->select.path.path[0] = UIM_CDMA_DF;
        }
        else
        {
          uim_ptr->command.cmd_ptr->select.path.path[0] = UIM_GSM_DF;
        }

        uim_ptr->command.cmd_ptr->select.path.len = 1;

        /* Reset select state control to start selecting from the first DF */
        uim_ptr->command.cmd_ptr->select.path_position = 0;

        uim_select_state(uim_ptr, &uim_ptr->command.cmd_ptr->select.path, uim_req_buf_static_ptr);
      } /* end case - UIM_CHECK_FOR_CDMA_DF*/
          break;

    /* Select a DF to determine characteristics. This state may be entered multiple
       times based on what df is selected and whether or not it was a success. It
       uses the previous path element in the command pointer to determine the next
       df to select.  Currently the path is reset before this state in UIM_POWER_UP_ST */
    case UIM_CHECK_CHARACTERISTICS_ST:
      {
        if ( !uim_ptr->flag.toggle_instrn_class )
        {
          /* Save the protocol as UICC - drivers layer uses the req_buf protocol
             to make decisions on apdu settings.  It must be correct from here on */
          uim_ptr->state.dfs_1.protocol                 = UIM_UICC;
          uim_req_buf_static_ptr->protocol              = UIM_UICC;
          uim_ptr->command.cmd_ptr->hdr.protocol        = UIM_UICC;

          uim_ptr->command.cmd_ptr->select.path.path[0] = 0x3F00;
          uim_ptr->command.cmd_ptr->select.path.len     = 1;
        }
        else
        {
          /* Set the protocol to ICC - drivers layer uses the req_buf protocol
             to make decisions on apdu settings.  It must be correct from here on */
          uim_ptr->command.cmd_ptr->hdr.protocol         = UIM_ICC;
          uim_ptr->command.static_cmd_buf.hdr.protocol   = UIM_ICC;
          uim_req_buf_static_ptr->protocol               = UIM_ICC;
          uim_ptr->state.dfs_1.protocol                  = UIM_ICC;

          uim_ptr->command.cmd_ptr->select.path_position = 0;

          uim_ptr->command.cmd_ptr->select.path.path[0] =
            uim_check_characteristics_get_next_df(uim_ptr, uim_ptr->command.cmd_ptr->select.path.path[0]);
        }
        uim_select_state(uim_ptr, &uim_ptr->command.cmd_ptr->select.path, uim_req_buf_static_ptr);
      } /* end case - UIM_CHECK_CHARACTERISTICS_ST */
      break;

    case UIM_SELECT_ICCID_ST:
      /* path position is not used in the power-up states up until this point.
         For ICC we select mf/iccid incrementally - this state is reentered
         after MF is selected.  If it's ICC and we just selected MF we should
         proceed selecting ICCID otherwise we should set the path to be selected */
      if((uim_ptr->command.cmd_ptr->select.path_position != 1)
         ||(uim_ptr->command.cmd_ptr->hdr.protocol == UIM_UICC))
      {
        uim_ptr->command.cmd_ptr->select.file_type    = UIM_EF;
        uim_ptr->command.cmd_ptr->select.path.path[0] = UIM_MF_ID;
        uim_ptr->command.cmd_ptr->select.path.path[1] = UIM_ICCID_EF_ID;
        uim_ptr->command.cmd_ptr->select.path.len     = 2;
      }
      uim_select_state(uim_ptr, &uim_ptr->command.cmd_ptr->select.path, uim_req_buf_static_ptr);
      break;

    case UIM_SELECT_FAILED_ST:
      uim_select_state(uim_ptr, &uim_ptr->command.cmd_ptr->select.path, uim_req_buf_static_ptr);
      break;

    case UIM_STREAM_SELECT_ST:

      if(uim_ptr->command.cmd_ptr->stream_iso7816_apdu.cmd_data[UIM_7816_APDU_P1_OFFSET] & UIM_USIM_SELECT_BY_DF_NAME)
      {
        /* We would simply force a select on 7fff and return
         * the result.  The response would be similar to that of
         * selection by AID */
        uim_ptr->command.cmd_ptr->select.path.path[0] = 0x3F00;
        uim_ptr->command.cmd_ptr->select.path.path[1] = 0x7FFF;
        uim_ptr->command.cmd_ptr->select.path.len     = 2;

        uim_select_state(uim_ptr, &uim_ptr->command.cmd_ptr->select.path, uim_req_buf_static_ptr);
      }
      else
      {
        /* This can be a select by path or a select by file id any
           other p1 is filtered out in the 7816_STREAM_F handling */
        uim_stream_apdu_state(uim_ptr, uim_req_buf_static_ptr);
      }
      break;

    case UIM_SELECT_ST:
      uim_select_state(uim_ptr, &uim_ptr->command.cmd_ptr->select.path, uim_req_buf_static_ptr);
      break;

     case UIM_TERMINAL_CAPABILITY_ST:
       uim_req_buf_static_ptr->apdu_hdr.instrn    = UIM_TERMINAL_CAPABILITY;
       uim_req_buf_static_ptr->apdu_hdr.uim_class = UIM_UICC_INSTRUCTION_CLASS2;
       uim_req_buf_static_ptr->instrn_case        = UIM_INSTRN_CASE_3;
       uim_req_buf_static_ptr->apdu_hdr.p1        = 0;
       uim_req_buf_static_ptr->apdu_hdr.p2        = 0;

       /* Initialize p3 length */
       uim_req_buf_static_ptr->apdu_hdr.p3        = 0x00;

       /* Tag - Terminal capability constructed */
       uim_req_buf_static_ptr->req.data[uim_req_buf_static_ptr->apdu_hdr.p3++]   = 0xA9;
       /* Length - Initialize length of proprietary data to 0x00 */
       uim_req_buf_static_ptr->req.data[uim_req_buf_static_ptr->apdu_hdr.p3++]   = 0x00;

       /* Tag - Power supply support indication */
       uim_req_buf_static_ptr->req.data[uim_req_buf_static_ptr->apdu_hdr.p3++]   = 0x80;
       /* Length - as defined by ETSI 102 221 */
       uim_req_buf_static_ptr->req.data[uim_req_buf_static_ptr->apdu_hdr.p3++]   = 0x03;
       /* Value 1 - for indicating the used voltage class.
                    bits b1 to b6 - xxxx1x - Class B
                    bits b1 to b6 - xxx1xx - Class C */
       /* Value 2 - for indicating the max power capability.
                    Per 102 221, 0x3C (60mA) is the max possible value.
                    Value set to 60mA as our PMIC's are capable of supporting this load.*/
       /* Value 3 - for indicating the used clk freq.
                    0xFF implies that no clk freq is indicated */
       if(uim_ptr->state.current_voltage_class == UIM_VOLTAGE_CLASS_C)
       {
         uim_req_buf_static_ptr->req.data[uim_req_buf_static_ptr->apdu_hdr.p3++] = 0x04;
       }
       else
       {
         uim_req_buf_static_ptr->req.data[uim_req_buf_static_ptr->apdu_hdr.p3++] = 0x02;
       }

       uim_req_buf_static_ptr->req.data[uim_req_buf_static_ptr->apdu_hdr.p3++] = 0x3C;
       uim_req_buf_static_ptr->req.data[uim_req_buf_static_ptr->apdu_hdr.p3++] = 0xFF;

       /* Tag - Extended logical channels support indication */
       uim_req_buf_static_ptr->req.data[uim_req_buf_static_ptr->apdu_hdr.p3++] = 0x81;
       /* Length - as defined by ETSI 102 221 */
       uim_req_buf_static_ptr->req.data[uim_req_buf_static_ptr->apdu_hdr.p3++] = 0x00;

       /* Tag - Additional interface support indication */
       uim_req_buf_static_ptr->req.data[uim_req_buf_static_ptr->apdu_hdr.p3++] = UIM_ADDITIONAL_INTERFACE_SUPPORTED_TAG;
       /* Length - as defined by ETSI 102 221 */
       uim_req_buf_static_ptr->req.data[uim_req_buf_static_ptr->apdu_hdr.p3++] = UIM_ADDITIONAL_INTERFACE_SUPPORTED_LENGTH;
       /* Value */
       if( TRUE == uim_ptr->flag.uim_swp_in_terminal_capability)
       {
         if(uim_ptr->flag.uicc_supports_swp)
         {
           uim_ptr->setting.voltage_indication_to_qmi.is_ack_needed = TRUE;
         }
         uim_req_buf_static_ptr->req.data[uim_req_buf_static_ptr->apdu_hdr.p3++] = UIM_ADDITIONAL_INTERFACE_SUPPORTED;
       }
       else
       {
         /* This value is default FALSE */
         uim_ptr->setting.voltage_indication_to_qmi.is_ack_needed = FALSE;
         uim_req_buf_static_ptr->req.data[uim_req_buf_static_ptr->apdu_hdr.p3++] = UIM_ADDITIONAL_INTERFACE_NOT_SUPPORTED;
       }

       if(uim_ptr->flag.is_euicc)
       {
          /* Tag - eUICC support */
          uim_req_buf_static_ptr->req.data[uim_req_buf_static_ptr->apdu_hdr.p3++]   = 0x83;
          /* Length - as defined by SGP22 */
          uim_req_buf_static_ptr->req.data[uim_req_buf_static_ptr->apdu_hdr.p3++]   = 0x01;

          /* Value 1 - for indicating LUI and LPD support.
            -   -   -   -   -   -   -   1   Local User Interface (LUI) for eUICC supported
            -   -   -   -   -   -   -   0   Local User Interface (LUI) for eUICC not supported
            -   -   -   -   -   -   1   -   Local Profile Download (LPD) for eUICC supported
            -   -   -   -   -   -   0   -   Local Profile Download (LPD) for eUICC not supported
            x   x   x   x   x   x   -   -   RFU
          */
          if(LPA_SUCCESS == lpa_is_lpa_supported(&is_lpd_supported, &is_lui_supported))
          {
	     uim_req_buf_static_ptr->req.data[uim_req_buf_static_ptr->apdu_hdr.p3++] =
		 	( ((uint8)is_lpd_supported << 0x01)  | (uint8)is_lui_supported );
          }
          else
          {
            uim_req_buf_static_ptr->req.data[uim_req_buf_static_ptr->apdu_hdr.p3++] = 0x0;
          }
        }
       /* Adjusting the length of proprietary data */
       uim_req_buf_static_ptr->req.data[1]        = uim_req_buf_static_ptr->apdu_hdr.p3 - 
                                                      UIM_TERM_CAPABILITY_LEN_BYTE_OFFSET;

       uim_req_buf_static_ptr->rsp_ptr            = &uim_ptr->command.rsp_buf;

       uim_send_command(uim_req_buf_static_ptr, uim_ptr);
       break;

    case UIM_SEND_STATUS_ST:  /* Send Status to determine ICC or UICC card */
      if(UIM_JCDMA_SERVICE_ENABLED == uim_ptr->setting.jcdma_mode.service)
      {
        if (uim_ptr->state.dfs_1.protocol == UIM_ICC)
        {
          uim_ptr->command.cmd_ptr->hdr.protocol = UIM_ICC;
        }
        else
        {
          uim_ptr->command.cmd_ptr->hdr.protocol = UIM_UICC;
        }
        if(uim_ptr->command.cmd_ptr->hdr.protocol == UIM_UICC)
        {
          /* Send STATUS command to card to determine if its truly a UICC */
          uim_req_buf_static_ptr->apdu_hdr.instrn    = STATUS;
          uim_req_buf_static_ptr->instrn_case        = UIM_INSTRN_CASE_1;
          uim_req_buf_static_ptr->apdu_hdr.uim_class = UIM_UICC_INSTRUCTION_CLASS2;
          uim_req_buf_static_ptr->apdu_hdr.p1        = UIM_STATUS_P1_NO_INDICATION;
          uim_req_buf_static_ptr->apdu_hdr.p2        = UIM_STATUS_P2_RETURN_NONE;
          /* No response expected. So response length is 0 */
          uim_req_buf_static_ptr->apdu_hdr.p3        = 0x00;
          uim_req_buf_static_ptr->rsp_ptr            = &uim_ptr->command.rsp_buf;

          uim_send_command(uim_req_buf_static_ptr, uim_ptr);
        }
        else
        {
          /* If its not a UICC card skip to the next state */
          ++uim_ptr->command.generic_state_ptr;
          uim_generic_command( uim_ptr);
        }
      }
      else
      {
        /* If its not a JCDMA card skip to the next state */
        ++uim_ptr->command.generic_state_ptr;
        uim_generic_command(uim_ptr);
      }
      break;

    case UIM_STATUS_ST:                       /* Status State */
      {
        /* Get the status (last selected DF) of the UIM card */
        uim_req_buf_static_ptr->apdu_hdr.instrn = STATUS;
        uim_req_buf_static_ptr->instrn_case = UIM_INSTRN_CASE_2;

        if (uim_ptr->command.cmd_ptr->hdr.protocol == UIM_UICC)
        {
          uim_req_buf_static_ptr->apdu_hdr.uim_class =
            uim_util_get_uim_class(UIM_UICC_INSTRUCTION_CLASS2, uim_ptr->command.cmd_ptr->hdr.channel);

          uim_req_buf_static_ptr->apdu_hdr.p1 = (int)uim_ptr->command.cmd_ptr->status.status;
          uim_req_buf_static_ptr->apdu_hdr.p2 = (int)uim_ptr->command.cmd_ptr->status.return_data;

          if ( uim_ptr->command.cmd_ptr->status.return_data == UIM_RETURN_NONE )
          {
            /* expect nothing in the response */
            uim_req_buf_static_ptr->apdu_hdr.p3 = 0;
            uim_req_buf_static_ptr->instrn_case = UIM_INSTRN_CASE_1;
          }
          else if((uim_ptr->command.cmd_ptr->status.return_data == UIM_STATUS_P2_DF_NAME_OF_CURR_APP) &&
                  (uim_ptr->command.cmd_ptr->status.aid.aid_length != 0))
          {
            uim_req_buf_static_ptr->apdu_hdr.p3 = uim_ptr->command.cmd_ptr->status.aid.aid_length + UIM_AID_TLV_LEN_OFFSET;
          }
          else
          {
            /* Maximum length of data expected in the response */
            if(uim_ptr->setting.config_params.future_use[UIM_RFU_INDEX_BOOLEAN_FLAGS] &
               UIM_RFU_MASK_STATUS_MAX_LEN_RETURN_EXP)
            {
              uim_req_buf_static_ptr->apdu_hdr.p3 = 255;
            }
            else
            {
              uim_req_buf_static_ptr->apdu_hdr.p3 = 0;
            }
          }
        }
        else
        {
           uim_req_buf_static_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;

           uim_req_buf_static_ptr->apdu_hdr.p1 = 0;
           uim_req_buf_static_ptr->apdu_hdr.p2 = 0;
           uim_req_buf_static_ptr->apdu_hdr.p3 = 22;
           /* 22 bytes are mandatory and shall be returned by the R-UIM for
           the DF */

        }

        uim_req_buf_static_ptr->rsp_ptr = &uim_ptr->command.rsp_buf;

        uim_send_command(uim_req_buf_static_ptr , uim_ptr);

      }
      break;
    case UIM_RECOVERY_SEND_STATUS_COMMAND_ST:
      {
        if(TRUE == uim_ptr->state.recovery_data.app_data[uim_ptr->state.recovery_data.ch_to_be_recovered].send_status)
        {
          UIMDRV_MSG_HIGH_0 ( uim_ptr->id, "UIM_RECOVERY_SEND_STATUS_COMMAND_ST");
          uim_req_buf_static_ptr->apdu_hdr.instrn = STATUS;
          uim_req_buf_static_ptr->instrn_case = UIM_INSTRN_CASE_2;

          uim_req_buf_static_ptr->apdu_hdr.uim_class =
          uim_util_get_uim_class(UIM_UICC_INSTRUCTION_CLASS2, uim_ptr->state.recovery_data.ch_to_be_recovered);

          uim_req_buf_static_ptr->apdu_hdr.p1 = UIM_STATUS_P1_CURR_APP_INIT;
          uim_req_buf_static_ptr->apdu_hdr.p2 = UIM_STATUS_P2_DF_NAME_OF_CURR_APP;
          uim_req_buf_static_ptr->apdu_hdr.p3 = uim_ptr->state.recovery_data.app_data[uim_ptr->state.recovery_data.ch_to_be_recovered].aid.aid_length + UIM_AID_TLV_LEN_OFFSET;
          uim_req_buf_static_ptr->rsp_ptr = &uim_ptr->command.rsp_buf;

          uim_send_command(uim_req_buf_static_ptr, uim_ptr);
        }
        else
        {
          /* Do not send status comamnd and skip to next state*/
          ++uim_ptr->command.generic_state_ptr;
          uim_generic_command (uim_ptr);
        }
      }
      break;
    case UIM_READ_BIN_MEM_ST:                  /* Read Binary State */
      {
        /* The selected file is TRANSPARENT.  Use a binary read to read
           from the file. */
        uim_req_buf_static_ptr->apdu_hdr.instrn = READ_BINARY;
        uim_req_buf_static_ptr->instrn_case = UIM_INSTRN_CASE_2;

        /* Offset into the EF - high byte, low byte */
        uim_req_buf_static_ptr->apdu_hdr.p1 = WORD_HI(uim_ptr->command.rd_wr_info.offset);
        uim_req_buf_static_ptr->apdu_hdr.p2 = WORD_LO(uim_ptr->command.rd_wr_info.offset);

        if (uim_ptr->command.cmd_ptr->read_bin.sfi != UIM_INVALID_SFI &&
            uim_req_buf_static_ptr->apdu_hdr.p1 == 0x00)
        {
          /* Can there be a case where the offset is > 256 that would need P1? If yes then we cannot use SFI but use the regular method. */
          /* SFI encoded in P1. Bits B8,B7,B6 need to be 1,0,0 respectively for SFI operation and bits B1-B5 hold the SFI.
             Offset into the EF in P2. */
          uim_req_buf_static_ptr->apdu_hdr.p1 = ( UIM_SFI_USED_BITS_B6_TO_B8 |
                                                  (uim_ptr->command.cmd_ptr->read_bin.sfi & UIM_SFI_BITS_B1_TO_B5_MASK) );
          uim_req_buf_static_ptr->apdu_hdr.p2 = WORD_LO(uim_ptr->command.rd_wr_info.offset);
          UIMDRV_MSG_HIGH_1(uim_ptr->id, "UIM_READ_BIN_MEM_ST - SFI used - 0x%x",
                            uim_ptr->command.cmd_ptr->read_bin.sfi);

        }
        else
        {
           /* SFI referencing is not used and so clear the MSB of p1 */
           uim_req_buf_static_ptr->apdu_hdr.p1 = uim_req_buf_static_ptr->apdu_hdr.p1 ^
               (uim_req_buf_static_ptr->apdu_hdr.p1 & UIM_SFI_NOT_USED_BR_MASK);
        }

        if (UIM_MAX_TRANSACTION_SIZE <= uim_ptr->command.rd_wr_info.num_bytes)
        {
          /* This is to read file in chunks of 255bytes */
          uim_req_buf_static_ptr->apdu_hdr.p3 = 0xFF;
        }
        else
        {
          uim_req_buf_static_ptr->apdu_hdr.p3 = WORD_LO (uim_ptr->command.rd_wr_info.num_bytes);
        }

        if (uim_ptr->command.cmd_ptr->hdr.protocol == UIM_UICC)
        {
           uim_req_buf_static_ptr->apdu_hdr.uim_class =
             uim_util_get_uim_class(UIM_UICC_INSTRUCTION_CLASS1, uim_ptr->command.cmd_ptr->hdr.channel);
        }
        else
        {
           uim_req_buf_static_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;
        }

        uim_req_buf_static_ptr->rsp_ptr = &uim_ptr->command.rsp_buf;

        uim_send_command(uim_req_buf_static_ptr, uim_ptr);

      } /* end case - UIM_READ_BIN_MEM_ST */
      break;

    case UIM_UPDATE_BIN_MEM_ST:                /* Update Binary State */
      {
        /* The selected file is TRANSPARENT.  Use a binary update to write
           to the file. */
        uim_req_buf_static_ptr->apdu_hdr.instrn = UPDATE_BINARY;
        uim_req_buf_static_ptr->instrn_case = UIM_INSTRN_CASE_3;

        /* Offset into the EF -- high byte, low byte */
        uim_req_buf_static_ptr->apdu_hdr.p1 = WORD_HI(  uim_ptr->command.rd_wr_info.offset );
        uim_req_buf_static_ptr->apdu_hdr.p2 = WORD_LO(  uim_ptr->command.rd_wr_info.offset );

        if (uim_ptr->command.cmd_ptr->write_bin.sfi != UIM_INVALID_SFI &&
            uim_req_buf_static_ptr->apdu_hdr.p1 == 0x00)
        {
          /* Can there be a case where the offset is > 256 that would need P1?
             If yes then we cannot use SFI but use the regular method */
          /* SFI encoded in P1. Bits B8,B7,B6 need to be 1,0,0 respectively for SFI operation and bits B1-B5 hold the SFI.
             Offset into the EF in P2 */
          uim_req_buf_static_ptr->apdu_hdr.p1 = ( UIM_SFI_USED_BITS_B6_TO_B8 |
                                                  (uim_ptr->command.cmd_ptr->write_bin.sfi & UIM_SFI_BITS_B1_TO_B5_MASK) );
          uim_req_buf_static_ptr->apdu_hdr.p2 = WORD_LO(uim_ptr->command.rd_wr_info.offset);
          UIMDRV_MSG_HIGH_1(uim_ptr->id, "UIM_UPDATE_BIN_MEM_ST - SFI used - 0x%x",
                            uim_ptr->command.cmd_ptr->write_bin.sfi);

        }
        else
        {
           /* SFI referencing is not used and so clear the MSB of p1 */
           uim_req_buf_static_ptr->apdu_hdr.p1 &= ~UIM_SFI_NOT_USED_BR_MASK;
        }

        if (UIM_MAX_TRANSACTION_SIZE <= uim_ptr->command.rd_wr_info.num_bytes)
        {
          uim_req_buf_static_ptr->apdu_hdr.p3 = UIM_MAX_TRANS_SIZE_OUT;
        }
        else
        {
          uim_req_buf_static_ptr->apdu_hdr.p3 =
            WORD_LO (uim_ptr->command.rd_wr_info.num_bytes);
        }

        if (uim_ptr->command.cmd_ptr->hdr.protocol == UIM_UICC)
        {
           uim_req_buf_static_ptr->apdu_hdr.uim_class =
             uim_util_get_uim_class(UIM_UICC_INSTRUCTION_CLASS1, uim_ptr->command.cmd_ptr->hdr.channel);
        }
        else
        {
           uim_req_buf_static_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;
        }

        uim_memscpy(uim_req_buf_static_ptr->req.data,
               sizeof(uim_req_buf_static_ptr->req.data),
               uim_ptr->command.rd_wr_info.data_ptr,
               uim_req_buf_static_ptr->apdu_hdr.p3);

        uim_req_buf_static_ptr->rsp_ptr = &uim_ptr->command.rsp_buf;

        uim_send_command(uim_req_buf_static_ptr, uim_ptr);

      } /* end case - UIM_UPDATE_BIN_MEM_ST */
      break;

    case UIM_READ_REC_MEM_ST:                  /* Read Record State */
      {
        /* The selected file is a record format.  Use a record read to read
           from the file. */
        uim_req_buf_static_ptr->apdu_hdr.instrn = READ_RECORD;
        uim_req_buf_static_ptr->instrn_case = UIM_INSTRN_CASE_2;

        if (uim_ptr->command.cmd_ptr->hdr.protocol == UIM_UICC)
        {
           uim_req_buf_static_ptr->apdu_hdr.uim_class =
             uim_util_get_uim_class(UIM_UICC_INSTRUCTION_CLASS1, uim_ptr->command.cmd_ptr->hdr.channel);
        }
        else
        {
           uim_req_buf_static_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;
        }

        if (uim_ptr->command.cmd_ptr->read_rec.sfi != UIM_INVALID_SFI)
        {
           UIMDRV_MSG_MED_2(uim_ptr->id, "UIM_READ_REC_MEM_ST - UIM_NEXT - SFI used - 0x%x for record mode 0x%x",
                     uim_ptr->command.cmd_ptr->read_rec.sfi,uim_ptr->command.cmd_ptr->read_rec.recmode );
        }

        /* Fill out APDU parameters based on the request mode */
        switch (uim_ptr->command.cmd_ptr->read_rec.recmode)
        {
          case UIM_CURRENT:
            {
              uim_req_buf_static_ptr->apdu_hdr.p1 = 0;
              uim_req_buf_static_ptr->apdu_hdr.p2 = UIM_P2_ABS_RECORD_MODE;
              /* SFI referencing cannot be used if we are trying to read a record
                 from the currently selected EF, so clear the MS 5 bits of p2 */
              uim_req_buf_static_ptr->apdu_hdr.p2 = uim_req_buf_static_ptr->apdu_hdr.p2 ^
              (uim_req_buf_static_ptr->apdu_hdr.p2 & UIM_SFI_NOT_USED_RR_MASK);

            } /* end case - UIM_CURRENT */
            break;

          case UIM_NEXT:
            {
              uim_req_buf_static_ptr->apdu_hdr.p1 = 0;
              if (uim_ptr->command.cmd_ptr->read_rec.sfi != UIM_INVALID_SFI)
              {
                /* SFI encoded in P2. Bits B8,B7,B6,B5,B4 denote the SFI and bits B1-B3 hold the mode for READ_RECORD. */
                uim_req_buf_static_ptr->apdu_hdr.p2 = ( UIM_P2_NEXT_RECORD_MODE |
                                                        ((uim_ptr->command.cmd_ptr->read_rec.sfi << UIM_SHIFT_SFI_BITS_TO_B4_TO_B8)
                                                         & UIM_SFI_BITS_B4_TO_B8_MASK) );
              }
              else
              {
              uim_req_buf_static_ptr->apdu_hdr.p2 = UIM_P2_NEXT_RECORD_MODE;
              }
            } /* end case - UIM_NEXT */
            break;

          case UIM_PREVIOUS:
            {
              uim_req_buf_static_ptr->apdu_hdr.p1 = 0;
              if (uim_ptr->command.cmd_ptr->read_rec.sfi != UIM_INVALID_SFI)
              {
                /* SFI encoded in P2. Bits B8,B7,B6,B5,B4 denote the SFI and bits B1-B3 hold the mode for READ_RECORD. */
                uim_req_buf_static_ptr->apdu_hdr.p2 = ( UIM_P2_PREV_RECORD_MODE |
                                                        ((uim_ptr->command.cmd_ptr->read_rec.sfi << UIM_SHIFT_SFI_BITS_TO_B4_TO_B8)
                                                          & UIM_SFI_BITS_B4_TO_B8_MASK) );
              }
              else
              {
              uim_req_buf_static_ptr->apdu_hdr.p2 = UIM_P2_PREV_RECORD_MODE;
            }
            }
            break;

          case UIM_ABSOLUTE:
            {
              uim_req_buf_static_ptr->apdu_hdr.p1 = uim_ptr->command.cmd_ptr->read_rec.rec_num;
              if (uim_ptr->command.cmd_ptr->read_rec.sfi != UIM_INVALID_SFI)
              {
                /* SFI encoded in P2. Bits B8,B7,B6,B5,B4 denote the SFI and bits B1-B3 hold the mode for READ_RECORD. */
                uim_req_buf_static_ptr->apdu_hdr.p2 = ( UIM_P2_ABS_RECORD_MODE |
                                                        ((uim_ptr->command.cmd_ptr->read_rec.sfi << UIM_SHIFT_SFI_BITS_TO_B4_TO_B8)
                                                         & UIM_SFI_BITS_B4_TO_B8_MASK) );
              }
              else
              {
              uim_req_buf_static_ptr->apdu_hdr.p2 = UIM_P2_ABS_RECORD_MODE;
            }
            }
            break;

          default:
            break;

        } /* end switch - uim_ptr->command.cmd_ptr->read_rec.rec_mode */

        /* Number of bytes to be read */
        uim_req_buf_static_ptr->apdu_hdr.p3 = uint16touint8(uim_ptr->command.rd_wr_info.num_bytes);

        uim_req_buf_static_ptr->rsp_ptr = &uim_ptr->command.rsp_buf;

        uim_send_command(uim_req_buf_static_ptr, uim_ptr);

      } /* end case - UIM_READ_REC_MEM_ST */
      break;

    case UIM_UPDATE_REC_MEM_ST:                /* Update Record State */
      {
        /* The last selected file is a record structure.  Use update record
           to write to this file */
        uim_req_buf_static_ptr->apdu_hdr.instrn = UPDATE_RECORD;
        uim_req_buf_static_ptr->instrn_case = UIM_INSTRN_CASE_3;

        if (uim_ptr->command.cmd_ptr->hdr.protocol == UIM_UICC)
        {
           uim_req_buf_static_ptr->apdu_hdr.uim_class =
             uim_util_get_uim_class(UIM_UICC_INSTRUCTION_CLASS1, uim_ptr->command.cmd_ptr->hdr.channel);
        }
        else
        {
           uim_req_buf_static_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;
        }

        if (uim_ptr->command.cmd_ptr->write_rec.sfi != UIM_INVALID_SFI)
        {
          UIMDRV_MSG_HIGH_2(uim_ptr->id, "UIM_UPDATE_REC_MEM_ST - UIM_NEXT - SFI used - 0x%x  for rec mode 0x%x",
                            uim_ptr->command.cmd_ptr->write_rec.sfi , uim_ptr->command.cmd_ptr->write_rec.recmode);
        }

        /* Fill out APDU parameters based on the request mode */
        switch (uim_ptr->command.cmd_ptr->write_rec.recmode)
        {
          case UIM_CURRENT:
            {
              uim_req_buf_static_ptr->apdu_hdr.p1 = 0;
              uim_req_buf_static_ptr->apdu_hdr.p2 = UIM_P2_ABS_RECORD_MODE;
              /* SFI referencing cannot be used if we are trying to update a record
                 from the currently selected EF, so clear the MS 5 bits of p2 */
              uim_req_buf_static_ptr->apdu_hdr.p2 = uim_req_buf_static_ptr->apdu_hdr.p2 ^
              (uim_req_buf_static_ptr->apdu_hdr.p2 & UIM_SFI_NOT_USED_RR_MASK);

            } /* end case - UIM_CURRENT */
            break;

          case UIM_NEXT:
            {
              uim_req_buf_static_ptr->apdu_hdr.p1 = 0;
              if (uim_ptr->command.cmd_ptr->write_rec.sfi != UIM_INVALID_SFI)
              {
                /* SFI encoded in P2. Bits B8,B7,B6,B5,B4 denote the SFI and bits B1-B3 hold the mode for UPDATE_RECORD. */
                uim_req_buf_static_ptr->apdu_hdr.p2 = ( UIM_P2_NEXT_RECORD_MODE |
                                                        ((uim_ptr->command.cmd_ptr->write_rec.sfi << UIM_SHIFT_SFI_BITS_TO_B4_TO_B8)
                                                          & UIM_SFI_BITS_B4_TO_B8_MASK) );
              }
              else
              {
              uim_req_buf_static_ptr->apdu_hdr.p2 = UIM_P2_NEXT_RECORD_MODE;
              }
            } /* end case - UIM_NEXT */
            break;

          case UIM_PREVIOUS:
            {
              uim_req_buf_static_ptr->apdu_hdr.p1 = 0;
                            if (uim_ptr->command.cmd_ptr->write_rec.sfi != UIM_INVALID_SFI)
              {
                /* SFI encoded in P2. Bits B8,B7,B6,B5,B4 denote the SFI and bits B1-B3 hold the mode for UPDATE_RECORD. */
                uim_req_buf_static_ptr->apdu_hdr.p2 = ( UIM_P2_PREV_RECORD_MODE |
                                                        ((uim_ptr->command.cmd_ptr->write_rec.sfi << UIM_SHIFT_SFI_BITS_TO_B4_TO_B8)
                                                          & UIM_SFI_BITS_B4_TO_B8_MASK) );
              }
              else
              {
              uim_req_buf_static_ptr->apdu_hdr.p2 = UIM_P2_PREV_RECORD_MODE;
              }
            } /* end case - UIM_PREVIOUS */
            break;

          case UIM_ABSOLUTE:
            {
              uim_req_buf_static_ptr->apdu_hdr.p1 = uim_ptr->command.cmd_ptr->write_rec.rec_num;
                            if (uim_ptr->command.cmd_ptr->write_rec.sfi != UIM_INVALID_SFI)
              {
                /* SFI encoded in P2. Bits B8,B7,B6,B5,B4 denote the SFI and bits B1-B3 hold the mode for UPDATE_RECORD. */
                uim_req_buf_static_ptr->apdu_hdr.p2 = ( UIM_P2_ABS_RECORD_MODE |
                                                        ((uim_ptr->command.cmd_ptr->write_rec.sfi << UIM_SHIFT_SFI_BITS_TO_B4_TO_B8)
                                                         & UIM_SFI_BITS_B4_TO_B8_MASK) );
              }
              else
              {
              uim_req_buf_static_ptr->apdu_hdr.p2 = UIM_P2_ABS_RECORD_MODE;
              }
            } /* end case - UIM_ABSOLUTE */
            break;

          default:
            break;

        } /* end switch - uim_ptr->command.cmd_ptr->write_rec.rec_mode */

        /* Number of bytes to write */
        uim_req_buf_static_ptr->apdu_hdr.p3 = uint16touint8(uim_ptr->command.rd_wr_info.num_bytes);

        uim_memscpy(uim_req_buf_static_ptr->req.data,
               sizeof(uim_req_buf_static_ptr->req.data),
               uim_ptr->command.rd_wr_info.data_ptr,
               uim_req_buf_static_ptr->apdu_hdr.p3);

        uim_req_buf_static_ptr->rsp_ptr = &uim_ptr->command.rsp_buf;

        uim_send_command(uim_req_buf_static_ptr, uim_ptr);

      } /* end case - UIM_UPDATE_REC_MEM_ST */
      break;

    case UIM_INC_MEM_ST:                    /* Increment State */
      {
        /* Build an increase APDU */
        uim_req_buf_static_ptr->apdu_hdr.instrn = INCREASE;
        uim_req_buf_static_ptr->instrn_case = UIM_INSTRN_CASE_4;

        if (uim_ptr->command.cmd_ptr->hdr.protocol == UIM_UICC)
        {
          uim_req_buf_static_ptr->apdu_hdr.uim_class =
            uim_util_get_uim_class(UIM_UICC_INSTRUCTION_CLASS2, uim_ptr->command.cmd_ptr->hdr.channel);
        }
        else
        {
          uim_req_buf_static_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;
        }

        uim_req_buf_static_ptr->apdu_hdr.p3 = uint16touint8(uim_ptr->command.rd_wr_info.num_bytes);

        uim_memscpy( uim_req_buf_static_ptr->req.data,
                 sizeof(uim_req_buf_static_ptr->req.data),
                 uim_ptr->command.rd_wr_info.data_ptr,
                 uim_ptr->command.rd_wr_info.num_bytes);

        if (uim_ptr->command.cmd_ptr->inc.sfi != UIM_INVALID_SFI)
        {
          /* SFI encoded in P1. Bits B8,B7,B6 need to be 1,0,0 respectively for SFI operation and bits B1-B5 hold the SFI. */
          uim_req_buf_static_ptr->apdu_hdr.p1 = ( UIM_SFI_USED_BITS_B6_TO_B8 |
                                                  (uim_ptr->command.cmd_ptr->inc.sfi & UIM_SFI_BITS_B1_TO_B5_MASK) );
          UIMDRV_MSG_HIGH_1(uim_ptr->id, "UIM_INC_MEM_ST - SFI used - 0x%x",
                            uim_ptr->command.cmd_ptr->inc.sfi);
        }
        else
        {
        uim_req_buf_static_ptr->apdu_hdr.p1 = 0;
        }

        uim_req_buf_static_ptr->apdu_hdr.p2 = 0;

        uim_req_buf_static_ptr->rsp_ptr = &uim_ptr->command.rsp_buf;

        uim_send_command(uim_req_buf_static_ptr, uim_ptr);

      } /* end case - UIM_INC_MEM_ST */
      break;

    case UIM_VERIFY_CHV1_ST:                  /* Verify CHV 1 State */
      {
        /* Check if CHV1 has been cache'd or this is a command to verify CHV */
        if (uim_ptr->cache.chv1.use_chv)
        {
          /* Build a verify CHV APDU */
          uim_req_buf_static_ptr->apdu_hdr.instrn = VERIFY_CHV;
          uim_req_buf_static_ptr->instrn_case = UIM_INSTRN_CASE_3;

          uim_req_buf_static_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;

          uim_req_buf_static_ptr->apdu_hdr.p1 = 0;

          uim_req_buf_static_ptr->apdu_hdr.p2 = (byte) UIM_CHV1;

          /* bytes of data sent in the command */
          uim_req_buf_static_ptr->apdu_hdr.p3 = UIM_MAX_CHV_DIGITS;

          uim_req_buf_static_ptr->rsp_ptr = &uim_ptr->command.rsp_buf;

          /* The CHV digits are copied from the cache'd buffer so that CHV1
             can be verified on wake up of the UIM (no explicit command). */
          uim_memscpy( (byte*) uim_req_buf_static_ptr->req.verify_chv.chv,
                  sizeof(uim_req_buf_static_ptr->req.verify_chv.chv),
                  uim_ptr->cache.chv1.chv, UIM_MAX_CHV_DIGITS);

          uim_send_command(uim_req_buf_static_ptr, uim_ptr);
        }
        else /* CHV 1 value is not cache'd */
        {
          /* Skip this state */
          ++uim_ptr->command.generic_state_ptr;
          /* Call this function to process the next state. */
          uim_generic_command (uim_ptr);
          return; /* exit immediately */
        } /* end if - the CHV 1 value is cache'd */
      } /* end case - UIM_VERIFY_CHV1_ST */
      break;

    case UIM_VERIFY_CHV2_ST:                  /* Verify CHV 2 State */
      {
        /* Build a verify CHV or a VERIFY PIN APDU */
        uim_req_buf_static_ptr->apdu_hdr.instrn = VERIFY_CHV;
        uim_req_buf_static_ptr->instrn_case = UIM_INSTRN_CASE_3;

        uim_req_buf_static_ptr->apdu_hdr.p1 = 0;

        if (uim_ptr->command.cmd_ptr->hdr.protocol != UIM_UICC)
        {
          if (!uim_ptr->cache.chv2.use_chv)
          {
            /* Do not verify CHV2 and skip this state */
            ++uim_ptr->command.generic_state_ptr;
            /* Call this function to process the next state. */
            uim_generic_command (uim_ptr);
            return; /* exit immediately */
          }

          uim_req_buf_static_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;
          uim_req_buf_static_ptr->apdu_hdr.p2 = (byte) UIM_CHV2;  /* CHV2 */
        }
        else
        {
          uim_req_buf_static_ptr->apdu_hdr.uim_class =
            uim_util_get_uim_class(UIM_UICC_INSTRUCTION_CLASS1, uim_ptr->command.cmd_ptr->hdr.channel);

          uim_set_uicc_pin_param_p2( uim_ptr->command.cmd_ptr->verify_chv.chv,
                                    (byte*) &(uim_req_buf_static_ptr->apdu_hdr.p2),
                                     uim_ptr);
        }

        /* If the command is being sent just to get the number of re-tries
           remaining, do not include the data */
        if ((uim_ptr->command.cmd_ptr->verify_chv.chv_digits[0] == '\0') &&
          (uim_ptr->command.cmd_ptr->hdr.command == UIM_VERIFY_CHV_F))
        {
          uim_req_buf_static_ptr->apdu_hdr.p3 = 0;
          uim_req_buf_static_ptr->instrn_case = UIM_INSTRN_CASE_1;
        }
        else
        {
          uim_req_buf_static_ptr->apdu_hdr.p3 = UIM_MAX_CHV_DIGITS;

          uim_memscpy( (byte*) uim_req_buf_static_ptr->req.verify_chv.chv,
                          sizeof(uim_req_buf_static_ptr->req.verify_chv.chv),
                          uim_ptr->cache.chv2.chv, UIM_MAX_CHV_DIGITS);
        }

        uim_req_buf_static_ptr->rsp_ptr = &uim_ptr->command.rsp_buf;

        uim_send_command(uim_req_buf_static_ptr, uim_ptr);
      } /* end case - UIM_VERIFY_CHV2_ST */
      break;

    case UIM_UNBLOCK_CHV1_ST:                 /* Unblock CHV 1 State */
      {
        /* Build an unblock CHV APDU */
        uim_req_buf_static_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;
        uim_req_buf_static_ptr->apdu_hdr.instrn = UNBLOCK_CHV;
        uim_req_buf_static_ptr->instrn_case = UIM_INSTRN_CASE_3;
        uim_req_buf_static_ptr->apdu_hdr.p1 = 0;
        uim_req_buf_static_ptr->apdu_hdr.p2 = 0;     /* weird but TRUE,!= UIM_CHV1*/
        /* bytes of data sent in cmd */
        uim_req_buf_static_ptr->apdu_hdr.p3 = UIM_MAX_CHV_DIGITS * 2;

        /* The unblock digits are provided in the command buffer by the
           client task. */
        uim_memscpy( (byte*) uim_req_buf_static_ptr->req.unblk_chv.chv,
                sizeof(uim_req_buf_static_ptr->req.unblk_chv.chv),
                uim_ptr->command.cmd_ptr->unblk_chv.chv_digits, UIM_MAX_CHV_DIGITS);

        /* The new CHV1 digits are provided in the command buffer by the
           client task. These digits are sent in the APDU.  The digits
           are copied to the cache buffer if the command completes
           successfully. */
        uim_memscpy( (byte*) uim_req_buf_static_ptr->req.unblk_chv.new_chv,
                sizeof(uim_req_buf_static_ptr->req.unblk_chv.new_chv),
                uim_ptr->command.cmd_ptr->unblk_chv.new_chv_digits, UIM_MAX_CHV_DIGITS);

        /* Format the CHV digits in the request buffer */
        uim_format_chv( (byte*) uim_req_buf_static_ptr->req.unblk_chv.new_chv,
                        uim_ptr);

        /* Format the CHV digits in the request buffer */
        uim_format_chv( (byte*) uim_req_buf_static_ptr->req.unblk_chv.chv,
                        uim_ptr);

        uim_req_buf_static_ptr->rsp_ptr = &uim_ptr->command.rsp_buf;

        uim_send_command(uim_req_buf_static_ptr, uim_ptr);

      } /* end case - UIM_UNBLOCK_CHV1_ST */
      break;

    case UIM_UNBLOCK_CHV2_ST:                 /* Unblock CHV 2 State */
      {
        /* Build an unblock CHV or Unblock PIN APDU */
        uim_req_buf_static_ptr->apdu_hdr.instrn = UNBLOCK_CHV;
        uim_req_buf_static_ptr->instrn_case = UIM_INSTRN_CASE_3;

        uim_req_buf_static_ptr->apdu_hdr.p1 = 0;

        if (uim_ptr->command.cmd_ptr->unblk_chv.chv == UIM_CHV2)
        {
           uim_req_buf_static_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;

           uim_req_buf_static_ptr->apdu_hdr.p2 = (byte) UIM_CHV2;  /* CHV2 */
        }
        else
        {
           uim_req_buf_static_ptr->apdu_hdr.uim_class =
             uim_util_get_uim_class(UIM_UICC_INSTRUCTION_CLASS1, uim_ptr->command.cmd_ptr->hdr.channel);

           uim_set_uicc_pin_param_p2( uim_ptr->command.cmd_ptr->unblk_chv.chv,
                                      (byte*) &(uim_req_buf_static_ptr->apdu_hdr.p2),
                                      uim_ptr);
        }

        /* If the command is being sent just to get the number of re-tries
           remaining, do not include the data */
        if (uim_ptr->command.cmd_ptr->unblk_chv.chv_digits[0] == '\0')
        {
            uim_req_buf_static_ptr->apdu_hdr.p3 = 0;
            uim_req_buf_static_ptr->instrn_case = UIM_INSTRN_CASE_1;
        }
        else
        {
           /* bytes of data sent in cmd */
           uim_req_buf_static_ptr->apdu_hdr.p3 = UIM_MAX_CHV_DIGITS * 2;

           /* The unblock digits are supplied in the command buffer by the
              client task. */
           uim_memscpy( (byte*) uim_req_buf_static_ptr->req.unblk_chv.chv,
                  sizeof(uim_req_buf_static_ptr->req.unblk_chv.chv),
                  uim_ptr->command.cmd_ptr->unblk_chv.chv_digits, UIM_MAX_CHV_DIGITS);

           /* Get the CHV digits from the command buffer.  The digits are not
              saved in a cache buffer for CHV2 since CHV2 is only verified
              by an explicit call from a client (not on wakeup of the UIM). */
           uim_memscpy( (byte*) uim_req_buf_static_ptr->req.unblk_chv.new_chv,
                   sizeof(uim_req_buf_static_ptr->req.unblk_chv.new_chv),
                   uim_ptr->command.cmd_ptr->unblk_chv.new_chv_digits, UIM_MAX_CHV_DIGITS);

           /* Format the CHV digits for the new CHV */
           uim_format_chv((byte*) uim_req_buf_static_ptr->req.unblk_chv.new_chv,
                          uim_ptr);

           /* Format the CHV digits for Unblock CHV */
           uim_format_chv((byte*) uim_req_buf_static_ptr->req.unblk_chv.chv,
                          uim_ptr);
        }

        uim_req_buf_static_ptr->rsp_ptr = &uim_ptr->command.rsp_buf;

        uim_send_command(uim_req_buf_static_ptr, uim_ptr);

      } /* end case - UIM_UNBLOCK_CHV2_ST */
      break;

    case UIM_CHANGE_CHV1_ST:                  /* Change CHV 1 State */
      {
        /* Build an change CHV APDU */
        uim_req_buf_static_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;
        uim_req_buf_static_ptr->apdu_hdr.instrn = CHANGE_CHV;
        uim_req_buf_static_ptr->instrn_case = UIM_INSTRN_CASE_3;
        uim_req_buf_static_ptr->apdu_hdr.p1 = 0;
        uim_req_buf_static_ptr->apdu_hdr.p2 = (int)UIM_CHV1; /* CHV1 */
        /* bytes of data sent in cmd */
        uim_req_buf_static_ptr->apdu_hdr.p3 = UIM_MAX_CHV_DIGITS * 2;

        /* The old CHV1 digits are supplied in the command buffer by the
           client task.  In this case, we do not use the cache buffer as
           the cache buffer may not contain the proper digits. */
        uim_memscpy( (byte*) uim_req_buf_static_ptr->req.change_chv.chv,
                sizeof(uim_req_buf_static_ptr->req.change_chv.chv),
                uim_ptr->command.cmd_ptr->change_chv.chv_digits, UIM_MAX_CHV_DIGITS);

        /* The client task supplies the new CHV digits in the command buffer.
           The digits will be copied to the CHV1 cache buffer if the command
           completes successfully. */
        uim_memscpy( (byte*) uim_req_buf_static_ptr->req.change_chv.new_chv,
                sizeof(uim_req_buf_static_ptr->req.change_chv.new_chv),
                uim_ptr->command.cmd_ptr->change_chv.new_chv_digits, UIM_MAX_CHV_DIGITS);

        /* Format the CHV digits */
        uim_format_chv((byte*) uim_req_buf_static_ptr->req.change_chv.chv,
                          uim_ptr);
        uim_format_chv((byte*) uim_req_buf_static_ptr->req.change_chv.new_chv,
                          uim_ptr);

        uim_req_buf_static_ptr->rsp_ptr = &uim_ptr->command.rsp_buf;

        uim_send_command(uim_req_buf_static_ptr, uim_ptr);
      } /* end case - UIM_CHANGE_CHV1_ST */
      break;

    case UIM_CHANGE_CHV2_ST:                  /* Change CHV 2 State */
      {
        /* Build an change CHV APDU or CHANGE PIN APDU */
        uim_req_buf_static_ptr->apdu_hdr.instrn = CHANGE_CHV;
        uim_req_buf_static_ptr->instrn_case = UIM_INSTRN_CASE_3;

        uim_req_buf_static_ptr->apdu_hdr.p1 = 0;

        if (uim_ptr->command.cmd_ptr->change_chv.chv == UIM_CHV2)
        {
           uim_req_buf_static_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;

           uim_req_buf_static_ptr->apdu_hdr.p2 = (byte) UIM_CHV2;  /* CHV2 */
        }
        else
        {
           uim_req_buf_static_ptr->apdu_hdr.uim_class =
             uim_util_get_uim_class(UIM_UICC_INSTRUCTION_CLASS1, uim_ptr->command.cmd_ptr->hdr.channel);

           uim_set_uicc_pin_param_p2( uim_ptr->command.cmd_ptr->change_chv.chv,
                                      (byte*) &(uim_req_buf_static_ptr->apdu_hdr.p2),
                                      uim_ptr);
        }

        /* bytes of data sent in cmd */
        uim_req_buf_static_ptr->apdu_hdr.p3 = UIM_MAX_CHV_DIGITS * 2;

        /* The old CHV2 digits are supplied in the command buffer by the
           client task.  The CHV2 digits are not kept in a cache buffer. */
        uim_memscpy( (byte*) uim_req_buf_static_ptr->req.change_chv.chv,
                sizeof(uim_req_buf_static_ptr->req.change_chv.chv),
                uim_ptr->command.cmd_ptr->change_chv.chv_digits, UIM_MAX_CHV_DIGITS);

        /* The client task supplies the new CHV digits in the command buffer.
           The digits are not copied to a cache buffer as there is no need
           for a cache buffer for CHV2. */
        uim_memscpy( (byte*) uim_req_buf_static_ptr->req.change_chv.new_chv,
                sizeof(uim_req_buf_static_ptr->req.change_chv.new_chv),
                uim_ptr->command.cmd_ptr->change_chv.new_chv_digits, UIM_MAX_CHV_DIGITS);

        /* Format the CHV digits */
        uim_format_chv((byte*) uim_req_buf_static_ptr->req.change_chv.chv,
                          uim_ptr);
        uim_format_chv((byte*) uim_req_buf_static_ptr->req.change_chv.new_chv,
                          uim_ptr);

        uim_req_buf_static_ptr->rsp_ptr = &uim_ptr->command.rsp_buf;

        uim_send_command(uim_req_buf_static_ptr, uim_ptr);
      } /* end case - UIM_CHANGE_CHV2_ST */
      break;

    case UIM_ENABLE_CHV1_ST:                  /* Enable CHV 1 State */
      {
        /* Build an enable CHV APDU */
        uim_req_buf_static_ptr->apdu_hdr.instrn = ENABLE_CHV;
        uim_req_buf_static_ptr->instrn_case = UIM_INSTRN_CASE_3;

        uim_req_buf_static_ptr->apdu_hdr.p1 = 0;

        if (uim_ptr->command.cmd_ptr->enable_chv.chv == UIM_CHV1)
        {
           uim_req_buf_static_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;

           uim_req_buf_static_ptr->apdu_hdr.p2 = (byte) UIM_CHV1;  /* CHV2 */
        }
        else
        {
           uim_req_buf_static_ptr->apdu_hdr.uim_class =
             uim_util_get_uim_class(UIM_UICC_INSTRUCTION_CLASS1, uim_ptr->command.cmd_ptr->hdr.channel);

           uim_set_uicc_pin_param_p2( uim_ptr->command.cmd_ptr->enable_chv.chv,
                                      (byte*) &(uim_req_buf_static_ptr->apdu_hdr.p2),
                                      uim_ptr);
        }

        uim_req_buf_static_ptr->apdu_hdr.p3 = UIM_MAX_CHV_DIGITS;
                      /* bytes of data sent in the command */

        uim_req_buf_static_ptr->rsp_ptr = &uim_ptr->command.rsp_buf;

        /* The CHV1 digits are provided in the command buffer by the
           client task. These digits are sent in the APDU.  The digits
           are copied to the cache buffer if the command completes
           successfully. */
        uim_memscpy( (byte*) uim_req_buf_static_ptr->req.enable_chv.chv,
                sizeof(uim_req_buf_static_ptr->req.enable_chv.chv),
                uim_ptr->command.cmd_ptr->enable_chv.chv_digits, UIM_MAX_CHV_DIGITS);

        uim_format_chv((byte*) uim_req_buf_static_ptr->req.enable_chv.chv,
                          uim_ptr);

        uim_send_command(uim_req_buf_static_ptr, uim_ptr);
      } /* end case - UIM_ENABLE_CHV1_ST */
      break;

    case UIM_ENABLE_CHV2_ST:                  /* Enable CHV 2 State */
      {
        uim_req_buf_static_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;
        uim_req_buf_static_ptr->apdu_hdr.instrn = ENABLE_CHV;
        uim_req_buf_static_ptr->instrn_case = UIM_INSTRN_CASE_3;
        uim_req_buf_static_ptr->apdu_hdr.p1 = 0;
        uim_req_buf_static_ptr->apdu_hdr.p2 = (byte) UIM_CHV2;
        uim_req_buf_static_ptr->apdu_hdr.p3 = UIM_MAX_CHV_DIGITS;
                      /* bytes of data sent in the command */

        uim_req_buf_static_ptr->rsp_ptr = &uim_ptr->command.rsp_buf;

        uim_memscpy( (byte*) uim_req_buf_static_ptr->req.enable_chv.chv,
                sizeof(uim_req_buf_static_ptr->req.enable_chv.chv),
                uim_ptr->command.cmd_ptr->enable_chv.chv_digits, UIM_MAX_CHV_DIGITS);

        uim_format_chv((byte*) uim_req_buf_static_ptr->req.enable_chv.chv,
                          uim_ptr);

        uim_send_command(uim_req_buf_static_ptr, uim_ptr);
      } /* end case - UIM_ENABLE_CHV2_ST */
      break;

    case UIM_DISABLE_CHV1_ST:                 /* Disable CHV 1 State */
      {
        /* Build a disable CHV APDU */

        uim_req_buf_static_ptr->apdu_hdr.instrn = DISABLE_CHV;
        uim_req_buf_static_ptr->instrn_case = UIM_INSTRN_CASE_3;

        uim_req_buf_static_ptr->apdu_hdr.p1 = 0;

        if (uim_ptr->command.cmd_ptr->disable_chv.chv == UIM_CHV1)
        {
           uim_req_buf_static_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;

           uim_req_buf_static_ptr->apdu_hdr.p2 = (byte) UIM_CHV1;  /* CHV2 */
        }
        else
        {
           uim_req_buf_static_ptr->apdu_hdr.uim_class =
             uim_util_get_uim_class(UIM_UICC_INSTRUCTION_CLASS1, uim_ptr->command.cmd_ptr->hdr.channel);

           uim_set_uicc_pin_param_p2( uim_ptr->command.cmd_ptr->disable_chv.chv,
                                      (byte*) &(uim_req_buf_static_ptr->apdu_hdr.p2),
                                      uim_ptr);

           if(uim_ptr->command.cmd_ptr->disable_chv.param_p2 != 0)
           {
              uim_req_buf_static_ptr->apdu_hdr.p1 = 0x80 | UIM_UNIVERSAL_PIN_VALUE;
           }
        }

        uim_req_buf_static_ptr->apdu_hdr.p3 = UIM_MAX_CHV_DIGITS;
                      /* bytes of data sent in the command */

        uim_req_buf_static_ptr->rsp_ptr = &uim_ptr->command.rsp_buf;

        /* The CHV1 digits are provided in the command buffer by the
           client task. These digits are sent in the APDU.  The digits
           are copied to the cache buffer if the command completes
           successfully. */
        uim_memscpy( (byte*) uim_req_buf_static_ptr->req.disable_chv.chv,
                sizeof(uim_req_buf_static_ptr->req.disable_chv.chv),
                uim_ptr->command.cmd_ptr->disable_chv.chv_digits, UIM_MAX_CHV_DIGITS);

        uim_format_chv((byte*) uim_req_buf_static_ptr->req.disable_chv.chv,
                          uim_ptr);

        uim_send_command(uim_req_buf_static_ptr, uim_ptr);
      } /* end case - UIM_DISABLE_CHV1_ST */
      break;

    case UIM_DISABLE_CHV2_ST:                 /* Disable CHV 2 State */
      {
        /* Build a disable CHV APDU */
        uim_req_buf_static_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;
        uim_req_buf_static_ptr->apdu_hdr.instrn = DISABLE_CHV;
        uim_req_buf_static_ptr->instrn_case = UIM_INSTRN_CASE_3;
        uim_req_buf_static_ptr->apdu_hdr.p1 = 0;
        uim_req_buf_static_ptr->apdu_hdr.p2 = (byte) UIM_CHV2;
        uim_req_buf_static_ptr->apdu_hdr.p3 = UIM_MAX_CHV_DIGITS;
                      /* bytes of data sent in the command */

        uim_req_buf_static_ptr->rsp_ptr = &uim_ptr->command.rsp_buf;

        uim_memscpy( (byte*) uim_req_buf_static_ptr->req.disable_chv.chv,
                sizeof(uim_req_buf_static_ptr->req.disable_chv.chv),
                uim_ptr->command.cmd_ptr->disable_chv.chv_digits, UIM_MAX_CHV_DIGITS);

        uim_format_chv((byte*) uim_req_buf_static_ptr->req.disable_chv.chv,
                          uim_ptr);

        uim_send_command(uim_req_buf_static_ptr, uim_ptr);
      } /* end case - UIM_DISABLE_CHV2_ST */
      break;

    case UIM_INVALIDATE_ST:                   /* Invalidate State */
      {
        /* Build an invalidate APDU */
        uim_req_buf_static_ptr->apdu_hdr.instrn = INVALIDATE;
        uim_req_buf_static_ptr->instrn_case = UIM_INSTRN_CASE_1;

        /* P1 = P2 = 0 and data field is empty in UICC implies that the
           command applies on the current EF */
        uim_req_buf_static_ptr->apdu_hdr.p1 = 0;
        uim_req_buf_static_ptr->apdu_hdr.p2 = 0;
        uim_req_buf_static_ptr->apdu_hdr.p3 = 0;
                    /* bytes of data sent in the command */

        if (uim_ptr->command.cmd_ptr->hdr.protocol == UIM_UICC)
        {
          uim_req_buf_static_ptr->apdu_hdr.uim_class =
            uim_util_get_uim_class(UIM_UICC_INSTRUCTION_CLASS1, uim_ptr->command.cmd_ptr->hdr.channel);
        }
        else
        {
          uim_req_buf_static_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;
        }

        uim_req_buf_static_ptr->rsp_ptr = &uim_ptr->command.rsp_buf;

        uim_send_command(uim_req_buf_static_ptr, uim_ptr);
      } /* end case - UIM_INVALIDATE_ST */
      break;

    case UIM_REHABILITATE_ST:                 /* Rehab state */
      {
        /* Build a rehabilitate APDU */
        uim_req_buf_static_ptr->apdu_hdr.instrn = REHABILITATE;
        uim_req_buf_static_ptr->instrn_case = UIM_INSTRN_CASE_1;

        /* P1 = P2 = 0 and data field is empty in UICC implies that the
           command applies on the current EF */
        uim_req_buf_static_ptr->apdu_hdr.p1 = 0;
        uim_req_buf_static_ptr->apdu_hdr.p2 = 0;
        uim_req_buf_static_ptr->apdu_hdr.p3 = 0;
                    /* bytes of data sent in the command */

        if (uim_ptr->command.cmd_ptr->hdr.protocol == UIM_UICC)
        {
          uim_req_buf_static_ptr->apdu_hdr.uim_class =
            uim_util_get_uim_class(UIM_UICC_INSTRUCTION_CLASS1, uim_ptr->command.cmd_ptr->hdr.channel);
        }
        else
        {
          uim_req_buf_static_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;
        }

        uim_req_buf_static_ptr->rsp_ptr = &uim_ptr->command.rsp_buf;

        uim_send_command(uim_req_buf_static_ptr, uim_ptr);
      } /* end case - UIM_REHABILITATE_ST */
      break;

    case UIM_SEEK_ST:
      {
        /* Build a seek APDU */
        uim_req_buf_static_ptr->apdu_hdr.instrn = SEEK;
        uim_req_buf_static_ptr->instrn_case = UIM_INSTRN_CASE_4;

        if (uim_ptr->command.cmd_ptr->hdr.protocol == UIM_UICC)
        {
          uim_req_buf_static_ptr->apdu_hdr.uim_class =
            uim_util_get_uim_class(UIM_UICC_INSTRUCTION_CLASS1, uim_ptr->command.cmd_ptr->hdr.channel);
	  
         if (uim_ptr->command.cmd_ptr->seek.sfi != UIM_INVALID_SFI)
         {
           UIMDRV_MSG_HIGH_2(uim_ptr->id, "UIM_SEEK_ST - UIM_SIMPLE_SRCH - SFI used - 0x%x for Search mode 0x%x",
                             uim_ptr->command.cmd_ptr->seek.sfi, uim_ptr->command.cmd_ptr->seek.srch_mode);
         }

          /* Simple search or enhanced search */
          switch (uim_ptr->command.cmd_ptr->seek.srch_mode)
          {
            case  UIM_SIMPLE_SRCH:

              uim_req_buf_static_ptr->apdu_hdr.p1 = uim_ptr->command.cmd_ptr->seek.rec_number;
              if (uim_ptr->command.cmd_ptr->seek.sfi != UIM_INVALID_SFI)
              {
                /* SFI encoded in P2. Bits B8,B7,B6,B5,B4 denote the SFI and bits B1-B3 hold the mode for SEARCH_RECORD. */
                uim_req_buf_static_ptr->apdu_hdr.p2 = ( uim_ptr->command.cmd_ptr->seek.srch_dir |
                                                        ((uim_ptr->command.cmd_ptr->seek.sfi << UIM_SHIFT_SFI_BITS_TO_B4_TO_B8)
                                                         & UIM_SFI_BITS_B4_TO_B8_MASK) );
              }
              else
              {
              uim_req_buf_static_ptr->apdu_hdr.p2 = (int)uim_ptr->command.cmd_ptr->seek.srch_dir;
              }

              /* Number of bytes of data sent in the command */
              uim_req_buf_static_ptr->apdu_hdr.p3 = uim_ptr->command.cmd_ptr->seek.num_bytes;

              /* Copy the seek patern from the command */
              uim_memscpy( uim_req_buf_static_ptr->req.data,
                      sizeof(uim_req_buf_static_ptr->req.data),
                      uim_ptr->command.cmd_ptr->seek.srch_pattern,
                      uim_ptr->command.cmd_ptr->seek.num_bytes );

            break; /* UIM_SIMPLE_SRCH  */

            case UIM_ENHANCED_SRCH:

              /* See if search is based on  rec # in P1 or current rec */
              if ((int)uim_ptr->command.cmd_ptr->seek.srch_rec_mode_type & UIM_SRCH_MODE_MASK )
                uim_req_buf_static_ptr->apdu_hdr.p1 = 0;
              else
                uim_req_buf_static_ptr->apdu_hdr.p1 = uim_ptr->command.cmd_ptr->seek.rec_number;

                            if (uim_ptr->command.cmd_ptr->seek.sfi != UIM_INVALID_SFI)
              {
                /* SFI encoded in P2. Bits B8,B7,B6,B5,B4 denote the SFI and bits B1-B3 hold the mode for SEARCH_RECORD. */
                uim_req_buf_static_ptr->apdu_hdr.p2 = ( UIM_P2_SRCH_ENHANCED |
                                                        ((uim_ptr->command.cmd_ptr->seek.sfi << UIM_SHIFT_SFI_BITS_TO_B4_TO_B8)
                                                         & UIM_SFI_BITS_B4_TO_B8_MASK) );
              }
              else
              {
              uim_req_buf_static_ptr->apdu_hdr.p2 = UIM_P2_SRCH_ENHANCED;
              }

              /* Number of bytes of data sent in the command */
              uim_req_buf_static_ptr->apdu_hdr.p3 = uim_ptr->command.cmd_ptr->seek.num_bytes +
                                              UIM_SRCH_IND_BYTES;

              /* Search indicator bytes */
              uim_req_buf_static_ptr->req.data[0] = (int)uim_ptr->command.cmd_ptr->seek.offset_type
                                           | (int)uim_ptr->command.cmd_ptr->seek.srch_rec_mode_type;

              uim_req_buf_static_ptr->req.data[1] = uim_ptr->command.cmd_ptr->seek.offset;

              /* Copy the seek patern from the command */
              uim_memscpy( &uim_req_buf_static_ptr->req.data[UIM_SRCH_IND_BYTES],
                      (sizeof(uim_req_buf_static_ptr->req.data) - UIM_SRCH_IND_BYTES),
                      uim_ptr->command.cmd_ptr->seek.srch_pattern, uim_ptr->command.cmd_ptr->seek.num_bytes );

            break; /* UIM_ENHANCED_SRCH */

            default:
            break;
          } /* switch (uim_ptr->command.cmd_ptr->seek.srch_mode) */
        } /* if (uim_ptr->command.cmd_ptr->hdr.protocol == UIM_UICC) */

        if (uim_ptr->command.cmd_ptr->hdr.protocol == UIM_ICC)
        {
          uim_req_buf_static_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;

          uim_req_buf_static_ptr->apdu_hdr.p1 = 0;

          uim_req_buf_static_ptr->apdu_hdr.p2 = (int)uim_ptr->command.cmd_ptr->seek.seek_type
                                       | (int)uim_ptr->command.cmd_ptr->seek.mode;

          /* Number of bytes of data sent in the command */
          uim_req_buf_static_ptr->apdu_hdr.p3 = uim_ptr->command.cmd_ptr->seek.num_bytes;

          /* Copy the seek patern from the command */
          uim_memscpy( uim_req_buf_static_ptr->req.data,
                  sizeof(uim_req_buf_static_ptr->req.data),
                  uim_ptr->command.cmd_ptr->seek.srch_pattern,
                  uim_ptr->command.cmd_ptr->seek.num_bytes );
        }

        uim_req_buf_static_ptr->rsp_ptr = &uim_ptr->command.rsp_buf;

        uim_send_command(uim_req_buf_static_ptr, uim_ptr);

      } /* end case for UIM_SEEK_ST */
      break;

    case UIM_TERMINAL_PROFILE_ST:             /* Term profile State */
      {
        if ( ((uim_ptr->command.cmd_ptr->hdr.command == UIM_INTERNAL_WAKE_UP_F) &&
              (uim_ptr->cache.int_term_profile_data.num_bytes != 0)) ||
             (uim_ptr->command.cmd_ptr->hdr.command == UIM_TERMINAL_PROFILE_F)
           )
        {

          /* Build a Terminal Profile APDU */
          uim_req_buf_static_ptr->apdu_hdr.instrn = TERMINAL_PROFILE;
          uim_req_buf_static_ptr->instrn_case = UIM_INSTRN_CASE_3;

          /* P1 = P2 = 0 and data field is empty in UICC implies that the
          command applies on the current EF */
          uim_req_buf_static_ptr->apdu_hdr.p1 = 0;
          uim_req_buf_static_ptr->apdu_hdr.p2 = 0;

          if (uim_ptr->command.cmd_ptr->hdr.protocol == UIM_UICC)
          {
            uim_req_buf_static_ptr->apdu_hdr.uim_class = UIM_UICC_INSTRUCTION_CLASS2;
          }
          else
          {
            uim_req_buf_static_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;
          }

          if (uim_ptr->command.cmd_ptr->hdr.command == UIM_INTERNAL_WAKE_UP_F)
          {
            /* Number of bytes of Terminal Profile */
            uim_req_buf_static_ptr->apdu_hdr.p3 = uim_ptr->cache.int_term_profile_data.num_bytes;

            /* Copy the data from the internal buffer */
            uim_memscpy (uim_req_buf_static_ptr->req.data,
                    sizeof(uim_req_buf_static_ptr->req.data),
                    uim_ptr->cache.int_term_profile_data.data,
                    uim_ptr->cache.int_term_profile_data.num_bytes );
          }
          else
          {
            /* Number of bytes of Terminal Profile */
            uim_req_buf_static_ptr->apdu_hdr.p3 = uim_ptr->command.cmd_ptr->terminal_profile.num_bytes;

            /* Copy the data from the command */
            uim_memscpy(uim_req_buf_static_ptr->req.data,
                   sizeof(uim_req_buf_static_ptr->req.data),
                   uim_ptr->command.cmd_ptr->terminal_profile.data,
                   uim_ptr->command.cmd_ptr->terminal_profile.num_bytes );
            /* Clear TP data cached by UIM */
            uim_ptr->cache.int_term_profile_data.num_bytes = 0;
          }

          uim_req_buf_static_ptr->rsp_ptr = &uim_ptr->command.rsp_buf;

          if(uim_nv_is_feature_enabled(UIMDRV_FEATURE_MIN_TPL_ICCID_SUPPORT,
                                       uim_ptr) == TRUE)
          {
            /* Get the ICCID value and TPL from NV  */
            if (NULL == uim_ptr->state.iccid_tpl_ptr)
            {
              /* Allocate memory */
              uim_ptr->state.iccid_tpl_ptr = uim_malloc(sizeof(nv_uim_iccid_tpl_type));
              if(NULL != uim_ptr->state.iccid_tpl_ptr)
              {
                /* Read NV Item */
                if (FALSE == uim_get_nv(
                              NV_UIM_ICCID_TPL_I,
                              (nv_item_type *)uim_ptr->state.iccid_tpl_ptr,
                              uim_ptr)
                    )
                {
                  /* NV item is not supported, freeing memory */
                  UIM_FREE(uim_ptr->state.iccid_tpl_ptr);
                }
              }
            }
            /* Compare the uim iccid and nv iccid value, if both are same then
               update the uim terminal profile length to nv terminal profile
               length if it is minimum, that will lead the uim to send the
               terminal profile length with minmum value. */
            if ((uim_ptr->command.cmd_ptr->hdr.command == UIM_TERMINAL_PROFILE_F) && (NULL != uim_ptr->state.iccid_tpl_ptr))
            {
              if ((0 == memcmp(uim_ptr->cache.iccid_buf.data ,uim_ptr->state.iccid_tpl_ptr->iccid_data,
                               uim_ptr->cache.iccid_buf.num_bytes)) &&
                  (uim_ptr->command.req_buf.apdu_hdr.p3 > uim_ptr->state.iccid_tpl_ptr->terminal_profile_len))
              {
                uim_ptr->command.req_buf.apdu_hdr.p3 = (byte)uim_ptr->state.iccid_tpl_ptr->terminal_profile_len;
              }
              else
              {
                /* If uim iccid and nv iccid is not same then take the uim iccid
                   and uim terminal profile length that will be write
                   back to NV if we get a error on terminal profile response */
                uim_memscpy(uim_ptr->state.iccid_tpl_ptr->iccid_data,
                       sizeof(uim_ptr->state.iccid_tpl_ptr->iccid_data),
                       uim_ptr->cache.iccid_buf.data ,
                       uim_ptr->cache.iccid_buf.num_bytes);
                uim_ptr->state.iccid_tpl_ptr->terminal_profile_len =
                                             uim_ptr->command.cmd_ptr->terminal_profile.num_bytes;
              }
            }
          } /* endif uim_nv_is_feature_enabled */

          uim_send_command(uim_req_buf_static_ptr, uim_ptr);
        } /* end if command needs to be sent */
        else
        {
          /* Signal is set internally for proper operation of state machine */
          uim_timed_sleep(UIM_CARD_DELAY_TIMEOUT, uim_ptr, UIM_ALL_ZERO_SIG);
          (void)rex_set_sigs(uim_ptr->tcb_ptr,UIM_CMD_RSP_SIG);
        }
      } /* end case - UIM_TERMINAL_PROFILE_ST */
      break;

    case UIM_ENVELOPE_ST:                     /* Envelope State */
      {
        /* Build an Envelope APDU */
        uim_req_buf_static_ptr->apdu_hdr.instrn = ENVELOPE;
        uim_req_buf_static_ptr->instrn_case = UIM_INSTRN_CASE_4;

        /* P1 = P2 = 0 and data field is empty in UICC implies that the
           command applies on the current EF */
        uim_req_buf_static_ptr->apdu_hdr.p1 = 0;
        uim_req_buf_static_ptr->apdu_hdr.p2 = 0;

        if (uim_ptr->command.cmd_ptr->hdr.protocol == UIM_UICC)
        {
          uim_req_buf_static_ptr->apdu_hdr.uim_class = UIM_UICC_INSTRUCTION_CLASS2;
        }
        else
        {
          uim_req_buf_static_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;
        }

        /* Number of bytes of Envelope command */
        uim_req_buf_static_ptr->apdu_hdr.p3 = uim_ptr->command.cmd_ptr->envelope.num_bytes;

        uim_memscpy(uim_req_buf_static_ptr->req.data,
               sizeof(uim_req_buf_static_ptr->req.data),
               uim_ptr->command.cmd_ptr->envelope.data + uim_ptr->command.cmd_ptr->envelope.offset,
               uim_ptr->command.cmd_ptr->envelope.num_bytes );

        uim_req_buf_static_ptr->rsp_ptr = &uim_ptr->command.rsp_buf;

        uim_send_command(uim_req_buf_static_ptr, uim_ptr);
      } /* end case - UIM_ENVELOPE_ST */
      break;

    case UIM_FETCH_ST:                        /* Fetch State */
      {
        /* Build a Fetch APDU */
        uim_req_buf_static_ptr->apdu_hdr.instrn = FETCH;
        uim_req_buf_static_ptr->instrn_case = UIM_INSTRN_CASE_2;

        /* P1 = P2 = 0 and data field is empty in UICC implies that the
           command applies on the current EF */
        uim_req_buf_static_ptr->apdu_hdr.p1 = 0;
        uim_req_buf_static_ptr->apdu_hdr.p2 = 0;

        if (uim_ptr->command.cmd_ptr->hdr.protocol == UIM_UICC)
        {
          uim_req_buf_static_ptr->apdu_hdr.uim_class = UIM_UICC_INSTRUCTION_CLASS2;
        }
        else
        {
          uim_req_buf_static_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;
        }

        /* Number of bytes of Fetch command */
        uim_req_buf_static_ptr->apdu_hdr.p3 = uim_ptr->command.cmd_ptr->fetch.command_size;

        uim_req_buf_static_ptr->rsp_ptr = &uim_ptr->command.rsp_buf;

        uim_send_command(uim_req_buf_static_ptr, uim_ptr);
      } /* end case - UIM_FETCH_ST */
      break;

    case UIM_TERMINAL_RESPONSE_ST:            /* Term response State */
      {
        /* Build a Terminal Response APDU */
        uim_req_buf_static_ptr->apdu_hdr.instrn = TERMINAL_RESPONSE;
        uim_req_buf_static_ptr->instrn_case = UIM_INSTRN_CASE_3;

        /* P1 = P2 = 0 and data field is empty in UICC implies that the
           command applies on the current EF */
        uim_req_buf_static_ptr->apdu_hdr.p1 = 0;
        uim_req_buf_static_ptr->apdu_hdr.p2 = 0;

        if (uim_ptr->command.cmd_ptr->hdr.protocol == UIM_UICC)
        {
          uim_req_buf_static_ptr->apdu_hdr.uim_class = UIM_UICC_INSTRUCTION_CLASS2;
        }
        else
        {
          uim_req_buf_static_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;
        }

        /* Number of bytes of Terminal Response */
        uim_req_buf_static_ptr->apdu_hdr.p3 = uim_ptr->command.cmd_ptr->terminal_response.num_bytes;

        uim_memscpy(uim_req_buf_static_ptr->req.data,
               sizeof(uim_req_buf_static_ptr->req.data),
               uim_ptr->command.cmd_ptr->terminal_response.data,
               uim_ptr->command.cmd_ptr->terminal_response.num_bytes );

        uim_req_buf_static_ptr->rsp_ptr = &uim_ptr->command.rsp_buf;

        uim_send_command(uim_req_buf_static_ptr, uim_ptr);

      } /* end case - UIM_TERMINAL_RESPONSE_ST */
      break;

    case UIM_POLLING_INTERVAL_ST:
      {
        switch (uim_ptr->command.cmd_ptr->polling.command_type)
        {
          case POLLING_INTERVAL:
            {
              uim_ptr->poll.is_proactive_polling_enabled = TRUE;
              uim_ptr->setting.proactive_time_delay =
                uim_ptr->command.cmd_ptr->polling.polling_interval_in_ms;

              if (uim_ptr->setting.proactive_time_delay
                  < UIM_MIN_POLL_DURATION)
              {
                uim_ptr->setting.proactive_time_delay =
                  UIM_MIN_POLL_DURATION;
              }

              /* Subtract UIM_POLL_OFFSET from the time_delay that is used by
                 the server */
              uim_ptr->setting.proactive_time_delay -= UIM_POLL_OFFSET;
            }
            break;

          case POLLING_OFF:
            {
              /* Set the proactive polling duration to the maximum allowed */
              uim_ptr->poll.is_proactive_polling_enabled = FALSE;              	
              uim_ptr->setting.proactive_time_delay
                = (dword)UIM_MAX_POLL_DURATION;
              if(uim_nv_is_feature_enabled(UIMDRV_FEATURE_UIM_POLLING_ONLY_AT_POLLING_TIMER_EXPIRY,
                                             uim_ptr) == TRUE)
              {
                /* Set the poll timer to the default polling time */
                uim_set_poll_timer( UIM_POLL_TIME, uim_ptr->setting.wakeup_poll_offset, uim_ptr);
              }
              uim_reset_poll_timer_val(uim_ptr);
            }
            break;
        } /* switch */

        /* Call the UIM power control function to use the new Proactive
           polling value.  Do not affect the power on voting. */
        uim_ptr->command.rpt_buf.rpt.polling_interval.actual_uim_polling_interval =
        uim_power_control_ds((uim_slot_type)(uim_ptr->id + 1), UIM_NONE, TRUE, UIM_CALL_TYPE_NOT_APPLICABLE );

          /* Add the UIM_POLL_OFFSET back so that the negotiated time does not
             include the poll_offset as poll_offset is for internal purposes
             only */
          if (uim_ptr->command.cmd_ptr->polling.command_type != POLLING_OFF)
          {
            uim_ptr->command.rpt_buf.rpt.polling_interval.actual_uim_polling_interval +=
                                                               UIM_POLL_OFFSET;
          }
      }
      /* Signal is set internally for proper operation of state machine */
      uim_timed_sleep(UIM_CARD_DELAY_TIMEOUT, uim_ptr, UIM_ALL_ZERO_SIG);
      (void)rex_set_sigs(uim_ptr->tcb_ptr, UIM_CMD_RSP_SIG);


      break;

    case UIM_STREAM_APDU_ST:
        uim_stream_apdu_state(uim_ptr, uim_req_buf_static_ptr);
      break;

    case UIM_RECOVERY_COMPLETE_ST:
      {
        /* Switch the mode back to the saved previous mode */
        uim_ptr->command.mode = uim_ptr->state.save_mode;

        /* set the response as done */
        uim_ptr->command.rsp_buf.cmd_status = UIM_DONE;

        /* Nothing is to be done set the response signal */
        (void)rex_set_sigs(uim_ptr->tcb_ptr, UIM_CMD_RSP_SIG);
      }
      break;

    case UIM_SIMULATE_NULL_ST:
      {
        /* Call handler to start uim_simulation_null_timer */
        uim_simulate_null_timer_state_handler( uim_ptr, UIM_BUSY_RESPONSE_EVT_CMD_RSP_SIG);
      }
      break;

    case UIM_DONE_ST:                         /* Done State */
    {
      /* Process the response of the command. */
      uim_process_generic_response(&uim_ptr->command.rsp_buf, uim_ptr);
    } /* end case - UIM_DONE_ST */
      break;

#ifdef FEATURE_UIM_USB_UICC
    case UIM_USB_POWER_OFF_ST:
    {
      uim_ptr->command.rsp_buf.sw1 = SW1_NORMAL_END;
      uim_ptr->command.rsp_buf.sw2 = SW2_NORMAL_END;

#ifdef FEATURE_UIM_REMOTE_UIM
      if (uim_ptr->command.cmd_ptr->hdr.command == UIM_HOTSWAP_CARD_REM_F ||
          uim_ptr->command.cmd_ptr->hdr.command == UIM_POWER_DOWN_F ||
          uim_ptr->command.cmd_ptr->hdr.command == UIM_RESET_F)
      {
        uim_usb_iccd_power_off(uim_ptr,QMI_UIM_REMOTE_POWER_DOWN_CARD);
        uim_usb_reset_globals(uim_ptr,FALSE);
        UIMDRV_MSG_HIGH_0(uim_ptr->id,
            "Turning off UIM USB UICC with QMI_UIM_REMOTE_POWER_DOWN_CARD");
      }
      else
      {
        uim_usb_iccd_power_off(uim_ptr,QMI_UIM_REMOTE_POWER_DOWN_TELECOM_INTERFACE);
        uim_usb_reset_globals(uim_ptr,TRUE);
        UIMDRV_MSG_HIGH_0(uim_ptr->id,
            "Turning off UIM USB UICC with QMI_UIM_REMOTE_POWER_DOWN_TELECOM_INTERFACE");
      }
#else
      UIMDRV_MSG_HIGH_0(uim_ptr->id, "FEATURE_UIM_REMOTE_UIM undefined, fallback to iso");
      uim_usb_fallback_to_iso(uim_ptr);
#endif /* FEATURE_UIM_REMOTE_UIM */
    }
      break;

    case UIM_USB_POWER_ON_ST:
    {
      uim_ptr->command.rsp_buf.sw1 = SW1_NORMAL_END;
      uim_ptr->command.rsp_buf.sw2 = SW2_NORMAL_END;

      if ((uim_ptr->command.cmd_ptr->hdr.command == UIM_INTERNAL_ME_PUP_F) &&
          (uim_ptr->usb.usb_interface_status.uim_remote_client_connected == FALSE))
      {
        UIMDRV_MSG_HIGH_0(uim_ptr->id, "Waiting for AP client to connect....");
        sigs_received = uim_pet_and_wait(UIM_USB_CONNECT_AVAILABLE_SIG |
                                         UIM_USB_FALLBACK_TO_ISO_SIG,
                                         uim_ptr
                                         );
        if (sigs_received == UIM_USB_CONNECT_AVAILABLE_SIG)
        {
          (void) rex_clr_sigs(uim_ptr->tcb_ptr, UIM_USB_CONNECT_AVAILABLE_SIG );
        }
        else
        {
          /* Received UIM_USB_FALLBACK_TO_ISO_SIG here indicate expiry of usb_init_timer.
             break from the block to avoid call uim_usb_fallback_to_iso() again */
          break;
        }
      }
      if(uim_ptr->usb.usb_interface_status.uim_remote_client_connected)
      {
        UIMDRV_MSG_HIGH_0(uim_ptr->id, "Turning on UIM USB UICC");
        if((uim_is_usb_enabled(uim_ptr) == TRUE)&&
           (uim_usb_iso_fallback_occurred(uim_ptr) == FALSE))
        {
          /* Save the protocol as UICC */
          uim_ptr->command.cmd_ptr->hdr.protocol = UIM_UICC;
          uim_ptr->state.dfs_1.protocol = UIM_UICC;
          uim_ptr->flag.toggle_instrn_class = FALSE;
        }
  #ifdef FEATURE_UIM_REMOTE_UIM
      uim_usb_iccd_power_on(uim_ptr);
  #else
        UIMDRV_MSG_HIGH_0(uim_ptr->id, "FEATURE_UIM_REMOTE_UIM undefined, fallback to iso");
        uim_usb_fallback_to_iso(uim_ptr);
  #endif /* FEATURE_UIM_REMOTE_UIM */
    }
      else
    {
        UIMDRV_MSG_HIGH_0(uim_ptr->id, "remote client doesn't connected, fallback to iso");
        uim_usb_fallback_to_iso(uim_ptr);
      }
    }
    break;
#endif /* FEATURE_UIM_USB_UICC */

   case UIM_RECOVERY_FLUSH_MANAGE_CHANNEL_ST:
     {
       /* Check the command queue for any Manage Channel command
          Flush the command if found. Proceed to next state pointer
       */
       q_linear_delete(&uim_ptr->task.cmd_q,
                       uim_compare_manage_ch_cmd,
                       uim_ptr,
                       uim_remove_manage_ch_cmd);
       UIMDRV_MSG_HIGH_0(uim_ptr->id,"Flushing manage channel command");
       ++uim_ptr->command.generic_state_ptr;
       uim_generic_command(uim_ptr);
       return;
     }
    break;

    case UIM_RECOVERY_VERIFY_CHV1_ST:                  /* Verify CHV 1 State during recovery */
      {
        if(0 == uim_ptr->state.recovery_data.num_channels)
        {
          /* GSDI has already cleared its cache */
          UIMDRV_MSG_HIGH_0(uim_ptr->id,"No information in GSDI cache, move to Done state");
          /* Finish recovery, move to done state */
          uim_ptr->mode.generic_states[0] = UIM_DONE_ST;
          uim_ptr->command.generic_state_ptr = uim_ptr->mode.generic_states;

          /* Call this function to process the next state. */
          uim_generic_command (uim_ptr);
          return; /* exit immediately */
        }
        /* Determine which CHV is to be verified. */
        if ((uim_ptr->state.recovery_data.app_data[uim_ptr->state.recovery_data.ch_to_be_recovered].pin1_data.chv == UIM_CHV1) &&
                 (uim_ptr->state.recovery_data.protocol == UIM_ICC))
        {
          UIMDRV_MSG_MED_0(uim_ptr->id,"Verifying PIN1 on ICC");
          /* Format the CHV digits in the cache buffer. */
          uim_format_chv(uim_ptr->state.recovery_data.app_data[uim_ptr->state.recovery_data.ch_to_be_recovered].pin1_data.chv_digits,
                         uim_ptr);

          /* Set the instruction class for the APDU */
          uim_req_buf_static_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;

          /* Set the CHV type */
          uim_req_buf_static_ptr->apdu_hdr.p2 = (byte) UIM_CHV1;  /* CHV1 */

        }
        else if ((uim_ptr->state.recovery_data.app_data[uim_ptr->state.recovery_data.ch_to_be_recovered].pin1_data.chv > UIM_CHV2) &&
                 (uim_ptr->state.recovery_data.app_data[uim_ptr->state.recovery_data.ch_to_be_recovered].pin1_data.chv < UIM_CHV_MAX) &&
                 (uim_ptr->state.recovery_data.protocol == UIM_UICC))
        {
          UIMDRV_MSG_MED_1(uim_ptr->id,"Verifying PIN1 on UICC for channel- %d",
                           uim_ptr->state.recovery_data.ch_to_be_recovered);
          /* Format the CHV digits in the cache buffer. */
          uim_format_chv(uim_ptr->state.recovery_data.app_data[uim_ptr->state.recovery_data.ch_to_be_recovered].pin1_data.chv_digits,
                         uim_ptr);

          /* Set the instruction class for the APDU */
          uim_req_buf_static_ptr->apdu_hdr.uim_class =
             uim_util_get_uim_class(UIM_UICC_INSTRUCTION_CLASS1,
               uim_ptr->state.recovery_data.app_data[uim_ptr->state.recovery_data.ch_to_be_recovered].channel);

          /* Set the CHV type */
          uim_set_uicc_pin_param_p2( uim_ptr->state.recovery_data.app_data[uim_ptr->state.recovery_data.ch_to_be_recovered].pin1_data.chv,
                                    (byte*) &(uim_req_buf_static_ptr->apdu_hdr.p2),
                                     uim_ptr);

        }
        else
        {
          UIMDRV_MSG_HIGH_2(uim_ptr->id,"Wrong CHV1/PIN1 %d recd for %d protocol",
                   uim_ptr->state.recovery_data.app_data[uim_ptr->state.recovery_data.ch_to_be_recovered].pin1_data.chv,
                            uim_ptr->state.recovery_data.protocol);
          /* Do not verify CHV1 and skip this state */
          ++uim_ptr->command.generic_state_ptr;
          /* Call this function to process the next state. */
          uim_generic_command (uim_ptr);
          return; /* exit immediately */
        }

        /* Build a verify CHV APDU */
        uim_req_buf_static_ptr->apdu_hdr.instrn = VERIFY_CHV;
        uim_req_buf_static_ptr->instrn_case = UIM_INSTRN_CASE_3;

        uim_req_buf_static_ptr->apdu_hdr.p1 = 0;

        /* bytes of data sent in the command */
        uim_req_buf_static_ptr->apdu_hdr.p3 = UIM_MAX_CHV_DIGITS;

        uim_req_buf_static_ptr->rsp_ptr = &uim_ptr->command.rsp_buf;

        uim_memscpy( (byte*) uim_req_buf_static_ptr->req.verify_chv.chv,
                          sizeof(uim_req_buf_static_ptr->req.verify_chv.chv),
                          uim_ptr->state.recovery_data.app_data[uim_ptr->state.recovery_data.ch_to_be_recovered].pin1_data.chv_digits,
                          UIM_MAX_CHV_DIGITS);

        uim_send_command(uim_req_buf_static_ptr, uim_ptr);
      } /* end case - UIM_RECOVERY_VERIFY_CHV1_ST */
      break;

    case UIM_RECOVERY_VERIFY_CHV2_ST:                  /* Verify CHV2 State during recovery */
      {
        /* Determine which CHV is to be verified. */
        if ((uim_ptr->state.recovery_data.app_data[uim_ptr->state.recovery_data.ch_to_be_recovered].pin2_data.chv == UIM_CHV2) &&
                 (uim_ptr->state.recovery_data.protocol == UIM_ICC))
        {
          UIMDRV_MSG_MED_0(uim_ptr->id,"Verifying PIN2 on ICC");
          /* Format the CHV digits in the cache buffer. */
          uim_format_chv(uim_ptr->state.recovery_data.app_data[uim_ptr->state.recovery_data.ch_to_be_recovered].pin2_data.chv_digits,
                         uim_ptr);

          /* Set the instruction class for the APDU */
          uim_req_buf_static_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;

          /* Set the CHV type */
          uim_req_buf_static_ptr->apdu_hdr.p2 = (byte) UIM_CHV2;  /* CHV2 */

        }
        else if ((uim_ptr->state.recovery_data.app_data[uim_ptr->state.recovery_data.ch_to_be_recovered].pin2_data.chv > UIM_CHV2) &&
                 (uim_ptr->state.recovery_data.app_data[uim_ptr->state.recovery_data.ch_to_be_recovered].pin2_data.chv < UIM_CHV_MAX) &&
                 (uim_ptr->state.recovery_data.protocol == UIM_UICC))
        {
          UIMDRV_MSG_MED_1(uim_ptr->id,"Verifying PIN2 on UICC for channel- %d",
                           uim_ptr->state.recovery_data.ch_to_be_recovered);
          /* Format the CHV digits in the cache buffer. */
          uim_format_chv(uim_ptr->state.recovery_data.app_data[uim_ptr->state.recovery_data.ch_to_be_recovered].pin2_data.chv_digits,
                          uim_ptr);

          /* Set the instruction class for the APDU */
          uim_req_buf_static_ptr->apdu_hdr.uim_class =
             uim_util_get_uim_class(UIM_UICC_INSTRUCTION_CLASS1,
               uim_ptr->state.recovery_data.app_data[uim_ptr->state.recovery_data.ch_to_be_recovered].channel);

          /* Set the CHV type */
          uim_set_uicc_pin_param_p2( uim_ptr->state.recovery_data.app_data[uim_ptr->state.recovery_data.ch_to_be_recovered].pin2_data.chv,
                                    (byte*) &(uim_req_buf_static_ptr->apdu_hdr.p2),
                                     uim_ptr);

        }
        else
        {
          UIMDRV_MSG_HIGH_2(uim_ptr->id,"Wrong CHV2/PIN2 %d recd for %d protocol",
                   uim_ptr->state.recovery_data.app_data[uim_ptr->state.recovery_data.ch_to_be_recovered].pin2_data.chv,
                            uim_ptr->state.recovery_data.protocol);

         /* Do not verify CHV2 and skip this state */
         ++uim_ptr->command.generic_state_ptr;

          /* Call this function to process the next state. */
          uim_generic_command (uim_ptr);
          return; /* exit immediately */
        }

        /* Build a verify CHV or a VERIFY PIN APDU */
        uim_req_buf_static_ptr->apdu_hdr.instrn = VERIFY_CHV;

        uim_req_buf_static_ptr->instrn_case = UIM_INSTRN_CASE_3;

        uim_req_buf_static_ptr->apdu_hdr.p1 = 0;

        uim_req_buf_static_ptr->apdu_hdr.p3 = UIM_MAX_CHV_DIGITS;

        /* Copy the CHV digits from data provided by MMGSDI */
        uim_memscpy( (byte*) uim_req_buf_static_ptr->req.verify_chv.chv,
                          sizeof(uim_req_buf_static_ptr->req.verify_chv.chv),
                          uim_ptr->state.recovery_data.app_data[uim_ptr->state.recovery_data.ch_to_be_recovered].pin2_data.chv_digits,
                          UIM_MAX_CHV_DIGITS);

        uim_req_buf_static_ptr->rsp_ptr = &uim_ptr->command.rsp_buf;

        uim_send_command(uim_req_buf_static_ptr, uim_ptr);
      } /* end case - UIM_RECOVERY_VERIFY_CHV2_ST */
      break;

    case UIM_RECOVERY_PERFORM_TP_ST:
      {
        if(0 == uim_ptr->cache.int_term_profile_data.num_bytes)
        {
          /* No Terminal Profile in UIM cache, move to done state */
          UIMDRV_MSG_HIGH_0(uim_ptr->id,"No Terminal Profile data in UIM cache");
          /* Do not verify CHV2 and skip this state */
          uim_ptr->mode.generic_states[0] = UIM_DONE_ST;
          uim_ptr->command.generic_state_ptr = uim_ptr->mode.generic_states;

          /* Call this function to process the next state. */
          uim_generic_command (uim_ptr);
          return; /* exit immediately */
        }

        /* Remove Terminal Response commands on this slot from UIM queue */
        q_linear_delete(&uim_ptr->task.cmd_q,
                        uim_compare_toolkit_cmds,
                        uim_ptr,
                        uim_remove_toolkit_cmds);

        /* Build a Terminal Profile APDU */
        uim_req_buf_static_ptr->apdu_hdr.instrn = TERMINAL_PROFILE;
        uim_req_buf_static_ptr->instrn_case = UIM_INSTRN_CASE_3;

        /* P1 = P2 = 0 and data field is empty in UICC implies that the
        command applies on the current EF */
        uim_req_buf_static_ptr->apdu_hdr.p1 = 0;
        uim_req_buf_static_ptr->apdu_hdr.p2 = 0;

        if (uim_ptr->state.recovery_data.protocol == UIM_UICC)
        {
          UIMDRV_MSG_MED_0(uim_ptr->id,"Sending Terminal Profile for UICC");
          uim_req_buf_static_ptr->apdu_hdr.uim_class = UIM_UICC_INSTRUCTION_CLASS2;
        }
        else
        {
          UIMDRV_MSG_MED_0(uim_ptr->id,"Sending Terminal Profile for ICC");
          uim_req_buf_static_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;
        }

        /* Number of bytes of Terminal Profile */
        uim_req_buf_static_ptr->apdu_hdr.p3 = uim_ptr->cache.int_term_profile_data.num_bytes;

        /* Copy the data from the internal buffer */
        uim_memscpy (uim_req_buf_static_ptr->req.data,
               sizeof(uim_req_buf_static_ptr->req.data),
               uim_ptr->cache.int_term_profile_data.data,
               uim_ptr->cache.int_term_profile_data.num_bytes );

        uim_req_buf_static_ptr->rsp_ptr = &uim_ptr->command.rsp_buf;

        uim_send_command(uim_req_buf_static_ptr, uim_ptr);
      }
      break; /* UIM_RECOVERY_PERFORM_TP_ST */

    case UIM_RECOVERY_CHECK_REHAB_REQD_ST:
      {
        if((UIM_NUM_REHAB_FILES == uim_ptr->state.recovery_data.rehab_file_index) ||
           (!uim_ptr->state.recovery_data.rehab_data[uim_ptr->state.recovery_data.rehab_file_index].rehab_reqd))
        {
          /* No more file to Rehabilitate */
          UIMDRV_MSG_HIGH_0(uim_ptr->id,"No more Files to Rehabilitate");
          /* Do not Rehabilitate any more file */
          uim_ptr->command.generic_state_ptr += 3;

          /* Call this function to process the next state. */
          uim_generic_command (uim_ptr);
          return; /* exit immediately */
        }

        /* path position and path are all that is needed for ICC select populate
           command here and transition to the select state to handle the select */
        uim_ptr->command.cmd_ptr->select.path_position = 0;

        uim_ptr->command.cmd_ptr->select.path =
          uim_ptr->state.recovery_data.rehab_data[uim_ptr->state.recovery_data.rehab_file_index].path;

          /* Move to select state */
          ++uim_ptr->command.generic_state_ptr;
          /* Call this function to process the next state. */
          uim_generic_command (uim_ptr);
        }
      break;

    case UIM_RECOVERY_REHAB_ST:
      {
        /* Build a rehabilitate APDU */
        uim_req_buf_static_ptr->apdu_hdr.instrn = REHABILITATE;
        uim_req_buf_static_ptr->instrn_case = UIM_INSTRN_CASE_1;

        /* P1 = P2 = 0 and data field is empty in UICC implies that the
           command applies on the current EF */
        uim_req_buf_static_ptr->apdu_hdr.p1 = 0;
        uim_req_buf_static_ptr->apdu_hdr.p2 = 0;
        uim_req_buf_static_ptr->apdu_hdr.p3 = 0;

        uim_req_buf_static_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;

        uim_req_buf_static_ptr->rsp_ptr = &uim_ptr->command.rsp_buf;

        uim_send_command(uim_req_buf_static_ptr, uim_ptr);
      }/* UIM_RECOVERY_REHAB_ST */
      break;

    case UIM_RECOVERY_CHECK_EF_TST_UPDATE_REQD_ST:
      {
        if(0 == uim_ptr->state.recovery_data.app_data[uim_ptr->state.recovery_data.ch_to_be_recovered].ef_tst_data.length)
        {
          /* EF-TST update is not required */
          UIMDRV_MSG_HIGH_0(uim_ptr->id,"EF-tst update not required");
          if(uim_ptr->state.recovery_data.protocol == UIM_UICC)
          {
            /* Increment channel index */
            ++uim_ptr->state.recovery_data.ch_to_be_recovered;
            /* Move to next channel*/
            uim_ptr->command.generic_state_ptr -= 6;
          }
          else
          {
            uim_ptr->mode.generic_states[0] = UIM_DONE_ST;
            uim_ptr->command.generic_state_ptr = uim_ptr->mode.generic_states;
          }

          /* Call this function to process the next state. */
          uim_generic_command (uim_ptr);
          return; /* exit immediately */
        }

        /* path position and path needs to be populated for select
        command here and transition to the select state to handle the select */
        if(uim_ptr->command.cmd_ptr->hdr.protocol == UIM_UICC)
        {
          uim_ptr->command.cmd_ptr->select.path.path[0]        = UIM_MF_ID;
          uim_ptr->command.cmd_ptr->select.path.path[1]        = 0x7FFF;
          uim_ptr->command.cmd_ptr->select.path.path[2]        = 0x7F66;
          uim_ptr->command.cmd_ptr->select.path.path[3]        = 0x6FD2;

          uim_ptr->command.cmd_ptr->select.path.len            = 4;
        }
        else
        {
          uim_ptr->command.cmd_ptr->select.path.path[0]        = UIM_MF_ID;
          uim_ptr->command.cmd_ptr->select.path.path[1]        = 0x7F66;
          uim_ptr->command.cmd_ptr->select.path.path[2]        = 0x6FD2;

          uim_ptr->command.cmd_ptr->select.path.len            = 3;
        }
        uim_ptr->command.cmd_ptr->select.path_position       = 0;
        uim_ptr->command.cmd_ptr->select.file_type           = UIM_EF;
        uim_ptr->command.cmd_ptr->select.aid.aid_length      = 0;

        uim_ptr->command.cmd_ptr->hdr.channel                = uim_ptr->state.recovery_data.ch_to_be_recovered;

        /* Move to select EF-TST state */
        ++uim_ptr->command.generic_state_ptr;
        /* Call this function to process the next state. */
        uim_generic_command (uim_ptr);
      }
      break;

    case UIM_RECOVERY_SELECT_EF_TST_ST:
      {
        uim_select_state(uim_ptr, &uim_ptr->command.cmd_ptr->select.path, uim_req_buf_static_ptr);
      }
      break;

    case UIM_RECOVERY_WRITE_EF_TST_ST:
      {
        /* Build a update binary APDU */
        uim_req_buf_static_ptr->apdu_hdr.instrn = UPDATE_BINARY;
        uim_req_buf_static_ptr->instrn_case = UIM_INSTRN_CASE_3;

        /* Offset into the EF -- high byte, low byte */
        uim_req_buf_static_ptr->apdu_hdr.p1 = 00;
        uim_req_buf_static_ptr->apdu_hdr.p2 = 00;

        uim_req_buf_static_ptr->apdu_hdr.p3 = uim_ptr->state.recovery_data.app_data[uim_ptr->state.recovery_data.ch_to_be_recovered].ef_tst_data.length;

        if (uim_ptr->command.cmd_ptr->hdr.protocol == UIM_UICC)
        {
          uim_req_buf_static_ptr->apdu_hdr.uim_class =
          uim_util_get_uim_class(UIM_UICC_INSTRUCTION_CLASS1, uim_ptr->state.recovery_data.ch_to_be_recovered);
        }
        else
        {
          uim_req_buf_static_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;
        }

        uim_memscpy(uim_req_buf_static_ptr->req.data,
        sizeof(uim_req_buf_static_ptr->req.data),
        uim_ptr->state.recovery_data.app_data[uim_ptr->state.recovery_data.ch_to_be_recovered].ef_tst_data.data,
        uim_ptr->state.recovery_data.app_data[uim_ptr->state.recovery_data.ch_to_be_recovered].ef_tst_data.length);

        uim_send_command(uim_req_buf_static_ptr, uim_ptr);
      }
      break;

    case UIM_RECOVERY_MANAGE_CHANNEL_FOR_UICC_ST:
      {
        /*All the channels till the last channel which was opened before recovery needs to be opened*/
        if(uim_ptr->state.recovery_data.ch_to_be_recovered > uim_max_ch_num_opened(uim_ptr))
        {
          UIMDRV_MSG_HIGH_0(uim_ptr->id,"No more channels to recover");
          uim_ptr->state.recovery_data.ch_to_be_recovered=0;
          uim_ptr->command.generic_state_ptr ++;

          /* Call this function to process the same state for next channel. */
          uim_generic_command (uim_ptr);
          return; /* exit immediately */
        }
        else if(!uim_ptr->state.recovery_data.ch_to_be_recovered)
        {
          /* Skip Manage Channel for default channel */
          UIMDRV_MSG_HIGH_1(uim_ptr->id,"Skip Manage Channel open for default Channel=%d",
                            uim_ptr->state.recovery_data.ch_to_be_recovered);
          ++uim_ptr->state.recovery_data.ch_to_be_recovered;
          /* Call this function to process the same state(Manage Channel)
             for next channel. */
          uim_generic_command (uim_ptr);
          return; /* exit immediately */
        }

        /* Build a Manage Channel APDU to open channel */
        uim_req_buf_static_ptr->apdu_hdr.instrn = MANAGE_CHANNEL;
        uim_req_buf_static_ptr->apdu_hdr.uim_class = UIM_UTIL_INSTRUCTION_CLASS2;

        /* For open channel P2 = 0 */
        uim_req_buf_static_ptr->apdu_hdr.p2 = 0;
        uim_req_buf_static_ptr->apdu_hdr.p1 = 0;
        uim_req_buf_static_ptr->apdu_hdr.p3 = 1;
        uim_req_buf_static_ptr->instrn_case = UIM_INSTRN_CASE_2;

        uim_req_buf_static_ptr->rsp_ptr = &uim_ptr->command.rsp_buf;
        uim_send_command(uim_req_buf_static_ptr, uim_ptr);
      }
      break;

  case UIM_RECOVERY_CLOSE_CHANNEL_FOR_UICC_ST:
    {
      if(uim_ptr->state.recovery_data.ch_to_be_recovered > uim_max_ch_num_opened(uim_ptr))
      {
        /* All channels recovered, move to UPIN state */
        uim_ptr->command.generic_state_ptr += 9;
        UIMDRV_MSG_HIGH_1(uim_ptr->id,"No more channels to be closed. Processing state 0x%ld",
                          *uim_ptr->command.generic_state_ptr);
        /* Call this function to process the same state for next channel. */
        uim_generic_command (uim_ptr);
        return; /* exit immediately */
      }
      else if(!uim_ptr->state.recovery_data.ch_to_be_recovered)
        {
          /* Skip Manage Channel for default channel */
        UIMDRV_MSG_HIGH_1(uim_ptr->id,"Process further states for default channel =%d ",
                          uim_ptr->state.recovery_data.ch_to_be_recovered);
          /* Move to check for select state */
          ++uim_ptr->command.generic_state_ptr;
        /* Call this function to process the same state for next channel. */
          uim_generic_command (uim_ptr);
          return; /* exit immediately */
        }
      else if(TRUE == (uim_ptr->state.is_ch_open[uim_ptr->state.recovery_data.ch_to_be_recovered] &&
               uim_ptr->state.recovery_data.app_data[uim_ptr->state.recovery_data.ch_to_be_recovered].is_ch_valid &&
              !uim_ptr->state.recovery_data.app_data[uim_ptr->state.recovery_data.ch_to_be_recovered].do_not_recover))
      {
        UIMDRV_MSG_HIGH_1(uim_ptr->id,"Process further states for Channel=%d",
                          uim_ptr->state.recovery_data.ch_to_be_recovered);
        /* Move to next state */
        ++uim_ptr->command.generic_state_ptr;
        uim_generic_command (uim_ptr);
        return; /* exit immediately */
      }
      else
      {
        if (TRUE == uim_ptr->state.is_ch_open[uim_ptr->state.recovery_data.ch_to_be_recovered] && uim_ptr->state.recovery_data.app_data[uim_ptr->state.recovery_data.ch_to_be_recovered].do_not_recover)
        {
          UIMDRV_MSG_HIGH_1(uim_ptr->id,"Channel=%d is being closed, as per MMGSDI request",
                            uim_ptr->state.recovery_data.ch_to_be_recovered);
        }
        else
        {
          UIMDRV_MSG_HIGH_1(uim_ptr->id,"Channel=%d is being closed now",
                            uim_ptr->state.recovery_data.ch_to_be_recovered);
        }
         /* Build a Manage Channel APDU to Close channel */
        uim_req_buf_static_ptr->apdu_hdr.instrn = MANAGE_CHANNEL;
        uim_req_buf_static_ptr->apdu_hdr.uim_class = UIM_UTIL_INSTRUCTION_CLASS2;

        uim_req_buf_static_ptr->apdu_hdr.p1 = 0x80;
        uim_req_buf_static_ptr->apdu_hdr.p2 =
                   uim_ptr->state.recovery_data.ch_to_be_recovered;
        uim_req_buf_static_ptr->apdu_hdr.p3 = 0x0;
        uim_req_buf_static_ptr->instrn_case = UIM_INSTRN_CASE_1;

        uim_req_buf_static_ptr->rsp_ptr = &uim_ptr->command.rsp_buf;
        uim_send_command(uim_req_buf_static_ptr, uim_ptr);
      }
    }
      break;

  case UIM_RECOVERY_CHECK_FOR_SELECT_AID_ST:
    {
      if(uim_ptr->state.recovery_data.app_data[uim_ptr->state.recovery_data.ch_to_be_recovered].is_aid_valid == FALSE)
      {
        /* No AID present */
        UIMDRV_MSG_HIGH_0(uim_ptr->id,"AID is invalid, moving to next channel");
        /* Move to next channel */
        ++uim_ptr->state.recovery_data.ch_to_be_recovered;
        /* Move to Manage Channel state for next channel */
        --uim_ptr->command.generic_state_ptr;
        /* Call this function to process the same state for next channel. */
        uim_generic_command (uim_ptr);
        return; /* exit immediately */
      }
      UIMDRV_MSG_HIGH_1(uim_ptr->id,"Send Select AID for channel=%d",
                        uim_ptr->state.recovery_data.ch_to_be_recovered);

      uim_ptr->command.cmd_ptr->select.file_type    = UIM_ADF;
      uim_ptr->command.cmd_ptr->select.path.path[0] = 0x3F00;
      uim_ptr->command.cmd_ptr->select.path.path[1] = 0x7FFF;
      uim_ptr->command.cmd_ptr->select.path.len     = 2;
      uim_ptr->command.cmd_ptr->hdr.channel         = uim_ptr->state.recovery_data.ch_to_be_recovered;

      uim_ptr->command.cmd_ptr->select.session =
        uim_ptr->state.recovery_data.app_data[uim_ptr->state.recovery_data.ch_to_be_recovered].session_control;
      uim_ptr->command.cmd_ptr->select.aid     =
        uim_ptr->state.recovery_data.app_data[uim_ptr->state.recovery_data.ch_to_be_recovered].aid;

      /* Move to Select State */
      ++uim_ptr->command.generic_state_ptr;
      /* Call this function to process the select state. */
      uim_generic_command (uim_ptr);
    }
    break;

  case UIM_RECOVERY_VERIFY_UPIN_FOR_UICC_ST:
    {
      if ((uim_ptr->state.recovery_data.upin_data.chv == UIM_UNIVERSAL_PIN) &&
          (uim_ptr->state.recovery_data.protocol == UIM_UICC))
      {
        UIMDRV_MSG_HIGH_0(uim_ptr->id,"Send UPIN verification on default channel");
        /* Format the CHV digits in the cache buffer. */
        uim_format_chv(uim_ptr->state.recovery_data.upin_data.chv_digits,
                       uim_ptr);

        /* Set the instruction class for the APDU */
        uim_req_buf_static_ptr->apdu_hdr.uim_class =  UIM_UICC_INSTRUCTION_CLASS1 ;

        /* Set the CHV type */
        uim_req_buf_static_ptr->apdu_hdr.p2 = UIM_UNIVERSAL_PIN_VALUE;

        /* Build a verify CHV APDU */
        uim_req_buf_static_ptr->apdu_hdr.instrn = VERIFY_CHV;
        uim_req_buf_static_ptr->instrn_case = UIM_INSTRN_CASE_3;

        uim_req_buf_static_ptr->apdu_hdr.p1 = 0;

        /* bytes of data sent in the command */
        uim_req_buf_static_ptr->apdu_hdr.p3 = UIM_MAX_CHV_DIGITS;

        uim_req_buf_static_ptr->rsp_ptr = &uim_ptr->command.rsp_buf;

        uim_memscpy( (byte*) uim_req_buf_static_ptr->req.verify_chv.chv,
                          sizeof(uim_req_buf_static_ptr->req.verify_chv.chv),
                          uim_ptr->state.recovery_data.upin_data.chv_digits,
                          UIM_MAX_CHV_DIGITS);

        uim_send_command(uim_req_buf_static_ptr, uim_ptr);
      }
      else
      {
        /* UPIN not available, move to next state*/
        UIMDRV_MSG_HIGH_0(uim_ptr->id,"UPIN not available");
        /* Move to select state */
        ++uim_ptr->command.generic_state_ptr;
        /* Call this function to process the next state. */
        uim_generic_command (uim_ptr);
        return;
      }
    }
    break;

    case UIM_GET_RESPONSE_ST:
      {
        UIMDRV_MSG_HIGH_0(uim_ptr->id,"Sending Get Response");
        if ((uim_ptr->command.cmd_ptr->hdr.protocol == UIM_UICC) ||
            (uim_ptr->command.cmd_ptr->hdr.protocol == UIM_WCDMA))
        {
          /*
            In case of streaming APDU do not mask bit 8 and bit 5 as some proprietary application
            do want GET RESPONSE with exact same CLA byte value as the one in original
            C-APDU header. If CLA is 0xFF which is a reserved value then print error and
            let card generate the error for transaction
          */
          if(uim_ptr->command.cmd_ptr->stream_iso7816_apdu.cmd_data[UIM_7816_APDU_CLASS_OFFSET] == 0xFF)
          {
            UIMDRV_MSG_ERR_1(uim_ptr->id,"Invalid CLA byte 0x%x",
              uim_ptr->command.cmd_ptr->stream_iso7816_apdu.cmd_data[UIM_7816_APDU_CLASS_OFFSET]);
          }
          uim_req_buf_static_ptr->apdu_hdr.uim_class =
            uim_ptr->command.cmd_ptr->stream_iso7816_apdu.cmd_data[UIM_7816_APDU_CLASS_OFFSET];

          if ( UIM_PASSIVE_MODE == uim_ptr->command.mode)
          {
            /* In passive mode, UIM will support only ETSI CLA mask */
            uim_req_buf_static_ptr->apdu_hdr.uim_class = 
               uim_req_buf_static_ptr->apdu_hdr.uim_class & UIM_GET_RESPONSE_CLA_MASK;
          }
        }
        else
        {
          uim_req_buf_static_ptr->apdu_hdr.uim_class = 0xA0;
        }
        uim_req_buf_static_ptr->apdu_hdr.instrn = GET_RESPONSE;
        uim_req_buf_static_ptr->instrn_case = UIM_INSTRN_CASE_2;
        uim_req_buf_static_ptr->apdu_hdr.p1 = 0x00;
        uim_req_buf_static_ptr->apdu_hdr.p2 = 0x00;

        /* When we get SW 62 XX for Case 4 instruction, we are supposed to force
           GET RESPONSE with P3 = 0. For 62 83 we force uim_get_resp_sw1 to
           value SW1_END_RESP. Hence same value is used here for sake of comparison */
        if (uim_ptr->card_cmd.get_resp_sw1 == SW1_WARNINGS1)
        {
          uim_req_buf_static_ptr->apdu_hdr.p3 = UIM_MAX_TRANSACTION_P3;
        }
        else if(uim_ptr->card_cmd.get_resp_sw1 == SW1_WARNINGS2)
        {
          uim_req_buf_static_ptr->apdu_hdr.p3 = UIM_MAX_TRANSACTION_P3;
        }
        else
        {
          uim_req_buf_static_ptr->apdu_hdr.p3 = uim_ptr->card_cmd.get_resp_sw2;
        }

        /* Move to Stream APDU state to process the response */
        ++uim_ptr->command.generic_state_ptr;
        uim_send_command(uim_req_buf_static_ptr, uim_ptr);
      }
      break;

    case UIM_SILENT_RECOVER_UICC_ST:
      {
        /* Clear out the command response timer as we are setting the timeout signal */
        uim_ptr->flag.clear_timeout_on_response = FALSE;

        /* Set the command response timeout signal so that UIM task can initiate
           the powerdown procedure */
        (void) rex_set_sigs( uim_ptr->tcb_ptr, UIM_CMD_RSP_TIMEOUT_SIG );
      }
      break;

    case UIM_MANAGE_CHANNEL_ST:            /* Manage Channel State */
      {
        /* Build a Manage Channel APDU */
        uim_ptr->command.req_buf.apdu_hdr.instrn = MANAGE_CHANNEL;
        /* This should always be 0 from mmgsdi but pass what they send along */
        uim_ptr->command.req_buf.apdu_hdr.uim_class =
          uim_util_get_uim_class(UIM_UTIL_INSTRUCTION_CLASS2,
                                 uim_ptr->command.cmd_ptr->manage_channel.channel);

        if(uim_ptr->command.cmd_ptr->manage_channel.mc_action == UIM_MC_OPEN)
        {
          if( TRUE == uim_ptr->state.channel_info.manage_channel_allowed &&
              0xFF != uim_ptr->state.channel_info.unallocated_channel)
          {
            UIMDRV_MSG_HIGH_1(uim_ptr->id, "Donot send the command to card.Channel 0x%x already opened internally.", uim_ptr->state.channel_info.unallocated_channel);
            uim_ptr->command.rsp_buf.cmd_status = UIM_DONE;
            uim_ptr->command.rsp_buf.cmd_rsp_size = 1;
            uim_ptr->command.rsp_buf.rsp.data[0] = uim_ptr->state.channel_info.unallocated_channel;
            uim_ptr->command.rsp_buf.sw1 = SW1_NORMAL_END;
            uim_ptr->command.rsp_buf.sw2 = SW2_NORMAL_END;
            uim_ptr->state.channel_info.unallocated_channel = 0xFF;
            (void) rex_set_sigs( uim_ptr->tcb_ptr, UIM_CMD_RSP_SIG );
            return;
          }
          uim_ptr->command.req_buf.apdu_hdr.p1 = 0;
          uim_ptr->command.req_buf.apdu_hdr.p2 =
            uim_ptr->command.cmd_ptr->manage_channel.op_close_channel;
          if(uim_ptr->command.cmd_ptr->manage_channel.op_close_channel == 0x0)
          {
            uim_ptr->command.req_buf.apdu_hdr.p3 = 1;
            uim_ptr->command.req_buf.instrn_case = UIM_INSTRN_CASE_2;
          }
          else
          {
            uim_ptr->command.req_buf.apdu_hdr.p3 = 0;
            uim_ptr->command.req_buf.instrn_case = UIM_INSTRN_CASE_1;
          }
        }
        else
        {
          uim_ptr->command.req_buf.apdu_hdr.p1 = 0x80;
          uim_ptr->command.req_buf.apdu_hdr.p2 =
          uim_ptr->command.cmd_ptr->manage_channel.op_close_channel;
          uim_ptr->command.req_buf.apdu_hdr.p3 = 0x0;
          uim_ptr->command.req_buf.instrn_case = UIM_INSTRN_CASE_1;
        }

        uim_ptr->command.req_buf.rsp_ptr = &uim_ptr->command.rsp_buf;
        uim_send_command(&uim_ptr->command.req_buf, uim_ptr);
      } /* end case - UIM_MANAGE_CHANNEL_ST */
      break;

    default:
      {
        UIMDRV_MSG_ERR_1(uim_ptr->id, "***Unknown UIM Generic state: %d***",
                         *uim_ptr->command.generic_state_ptr);
      } /* end case - default */
      break;

  } /* end of main switch */

  UIMDRV_MSG_LOW_1(uim_ptr->id, "UIM generic state in uim_command %ld",
                   *uim_ptr->command.generic_state_ptr);
} /* uim_generic_command */


/*===========================================================================

FUNCTION UIM_GENERIC_COMMAND_RESPONSE

DESCRIPTION
  This procedure processes the response to a generic comamnd that has been
  recd from the UIM.

DEPENDENCIES
  None

RETURN VALUE
  Status which is of apdu_status_type

SIDE EFFECTS
  Changes the UIM state variable uim_ptr->command.generic_state_ptr.

===========================================================================*/

uim_cmd_status_type uim_generic_command_response
(
  uim_rsp_buf_type      *rsp_ptr,
  uim_instance_global_type *uim_ptr
)
{
  uim_cmd_status_type status = UIM_CMD_SUCCESS;
  /* check to see what the status from the UIM is */
  byte i = 0;
  byte   j = 0;
  uint32 tempATR = 0;
  /* Index into the ATR buffer */
  byte check_byte = 0;
  /* Check byte */
  byte rx_value = 0;
  /* temp storage for recd byte while parsing ATR */
  byte num_hist_chars = 0;
  /* number of historical characters */
  int num_bytes_to_skip = 0;
  /* number of bytes to skip */
  uim_atr_pps_rx_mode_type receive_mode = UIM_WAITING_FOR_T0;
  /* ATR Receieve mode */
  boolean check_byte_expected = FALSE;
  /* Flag set to TRUE if we would receive a check byte */
  boolean td_present = FALSE;
  /* Index to TC1 byte of ATR */
  int uim_proc_atr_byte_index = 0;
  /* Flag to indicate if TD character was present */
  uim_generic_state_type curr_uim_generic_state = UIM_DONE_ST;
  /* Normally, the status when a command is completed is UIM_CMD_COMPLETED
     This is changed if there is a proactive command pending. */
  uim_cmd_status_type completed_status = UIM_CMD_COMPLETED;

  /* Aid for status response */
  uim_aid_type status_adf = {{0}};
  /* File ID for status response */
  uim_dir_type status_file_id = 0;
  /* Holds index in the ATR buffer of the TD byte */
  int uim_atr_td_index = 0;

  uim_clock_rate_conversion_factor_type  FI = UIM_CRCF_372_1; /* CRCF index */
  uim_baud_rate_adjustment_factor_type   DI = UIM_BRAF_1; /* BRAF index */
  /* Variable to store the TD1 byte position in the ATR.
     TD1 byte indicates if TA2 byte is present in the ATR or not.
     The TA2 byte would indicates the mode of the SIM. */
  byte uim_atr_td1_index = 0;
  /* Variable to store the TA2 byte.
     The TA2 byte indicates the mode of the SIM */
  byte uim_atr_ta2_byte = 0;
  /* Boolean variable to specific mode check */
  boolean skip_specific_mode_check = FALSE;
  /* Boolean variable to check for mode changeability */
  boolean skip_mode_changeability_check = FALSE;
  /* Boolean variable to indicate if TA2 byte is present in ATR */
  boolean uim_atr_ta2_byte_present = FALSE;
  /* Boolean variable to indicate the Bit 8 of the TA2 byte.
     If b8 bit of TA2 is set (TRUE), the SIM doesn't allow to change
     the mode.
     If b8 bit of TA2 is not set (FALSE), the SIM allows to change
     the mode. */
  boolean uim_atr_b8_bit_ta2 = FALSE;
 /*  Boolean variable to indicate the Bit 5 of the TA2 byte.
     If b5 bit of TA2 is set (TRUE), the baud-rate of the SIM is
     implicitly defined (default baud-rate).
     If b5 bit of TA2 is not set (FALSE), the baud-rate of the SIM is
     defined by the interface bytes. */
  boolean uim_atr_b5_bit_ta2 = FALSE;
  /* Indicates whether we have the voltage class information present */
  boolean voltage_class_indicator_scan = FALSE;
  /* Maintains the number of the TDs that we have processed */
  byte number_of_td = 0;
  /* Holds the UI (voltage class indicator) value found in TA after T=15 */
  byte voltage_class_indicator;

  /* Does the card support the terminal capability command */
  uint8 uim_send_terminal_capability = 0x00;

  /* Byte to store the clock stop bits extracted from TA4 byte */
  byte uim_ta4_clock_stop_bits = 0;
  byte mnc1 = 0, mnc2 = 0;
  byte mcc1 = 0, mcc2 = 0, mcc3 = 0;
  int idx = 0;
  /* Pointer to the Global UIM Request buffer */
  uim_req_buf_type *uim_req_buf_static_ptr = NULL;

 /* Create a ptr to current channel to increase code readability */
  uim_channel_type curr_ch = 0;

  static boolean reattempted_select_mf_upon_tech_prob = FALSE;

  /* To make sure the card_cmd has the same channel present in the command  */
  uim_ptr->card_cmd.cur_channel = uim_ptr->command.cmd_ptr->hdr.channel;
  curr_ch = uim_ptr->card_cmd.cur_channel;

  uim_req_buf_static_ptr = &uim_ptr->command.req_buf;

  if(uim_ptr->command.cmd_ptr == NULL || rsp_ptr == NULL)
  {
    return UIM_CMD_ERR;
  }

  if ((*uim_ptr->command.generic_state_ptr != UIM_POWER_UP_ST) &&
         (*uim_ptr->command.generic_state_ptr != UIM_UPDATE_OP_PARAMS_ST))
  {
    /* UIM_PARITY is used to indicate an error from the UIM */
    if ( rsp_ptr->cmd_status == UIM_PARITY )
    {
       /* Fill in the status words that we did not understand.. the
       * client can look into it
       */

      uim_ptr->command.rpt_buf.sw1 = (uim_sw1_type) rsp_ptr->sw1;
      uim_ptr->command.rpt_buf.sw2 = (uim_sw2_type) rsp_ptr->sw2;

      if (uim_nv_is_feature_enabled(UIMDRV_FEATURE_RPT_BAD_SW_ON_POLL,
                                    uim_ptr) &&
          uim_ptr->command.cmd_ptr->hdr.command == UIM_INTERNAL_POLL_F)
      {
        UIMDRV_MSG_ERR_2(uim_ptr->id, "Unknown status words while polling - SW1, SW2",
                         rsp_ptr->sw1, rsp_ptr->sw2);
        return(UIM_POLL_ERROR);
      }

      UIMDRV_MSG_ERR_2(uim_ptr->id, "Received unknown status words - SW1, SW2",
                       rsp_ptr->sw1, rsp_ptr->sw2);

       /* If we get a parity error, we would like to treat it as a command
          error */
      UIMDRV_MSG_ERR_0(uim_ptr->id, "Returning error since cmd_status = UIM_PARITY");
      /* If we get a parity error, we would like to treat it as a command
         error */
      return(UIM_CMD_ERR);
    } /* end if - cmd_status == UIM_PARITY */
  } /* (uim_ptr->command.generic_state_ptr != UIM_POWER_UP_ST) &&
       (uim_generic_state_ptr != UIM_UPDATE_OP_PARAMS_ST) */

  /* Fill the status words into the report buffer. This could be sent for
     both passed and failed reports. If the command is not sent to the
     card, then the status words are cleared before sending the report */
  uim_ptr->command.rpt_buf.sw1 = (uim_sw1_type) rsp_ptr->sw1;
  uim_ptr->command.rpt_buf.sw2 = (uim_sw2_type) rsp_ptr->sw2;

  /* Change fetch responses to normal end responses only if we are not in
   * passive mode.  In passive mode, it is the client's responsibility
   * to do a fetch based on the status words reported
   * Let us not waste time trying to fetch while recovering.
   */
  if ( (SW1_END_FETCH == uim_ptr->command.rpt_buf.sw1) &&
       !(uim_ptr->command.mode == UIM_PASSIVE_MODE) &&
       !(uim_ptr->command.mode == UIM_RECOVERY_MODE) )
  {
    /* Store the words for streaming result */
    /* Do not change the response to fetch response in case of warnings for envelope
       command or in case of streaming apdus, to notify warnings to gstk */
    if(!(((uim_ptr->card_cmd.get_resp_sw1 == SW1_WARNINGS1) &&
            (*uim_ptr->command.generic_state_ptr == UIM_ENVELOPE_ST)) ||
         ((uim_ptr->card_cmd.get_resp_sw1 == SW1_WARNINGS2) &&
            (*uim_ptr->command.generic_state_ptr == UIM_ENVELOPE_ST))||
          uim_processing_stream_apdu(uim_ptr)))
    {
      uim_ptr->card_cmd.get_resp_sw1 = uim_ptr->command.rpt_buf.sw1;
      uim_ptr->card_cmd.get_resp_sw2 = uim_ptr->command.rpt_buf.sw2;
    }

    if(UIM_ENVELOPE_ST != *uim_ptr->command.generic_state_ptr)
    {
      /* For envelope command UIM need to send the 0x91XX status byte,
         This status byte used by the GSTK for their internal purpose.
         Case: some card return MORE Time procactive command, That means
         Card is still processing the envelope command, so GSTK can not
         terminate the STK session. It needs to wait for the actual 0x9000
         status byte
      */
      uim_ptr->command.rpt_buf.sw1 = SW1_NORMAL_END;
      uim_ptr->command.rpt_buf.sw2 = SW2_NORMAL_END;
    }
    /* If the command is completed, indicate there is a proactive command
       pending. */
    completed_status = UIM_CMD_FETCH;

    /* Set the protocol for the Internal FETCH command based on the last
       successful command */
    uim_ptr->command.static_cmd_buf.hdr.protocol = uim_ptr->command.cmd_ptr->hdr.protocol;

  } /* end if - fetch response */
  else if(uim_ptr->command.mode == UIM_RECOVERY_MODE)
  {
    uim_ptr->command.rpt_buf.sw1 = SW1_NORMAL_END;
    uim_ptr->command.rpt_buf.sw2 = SW2_NORMAL_END;
  }

  /* Necessary so that we do not change the uim state and switch into that
     case also */
  curr_uim_generic_state = *uim_ptr->command.generic_state_ptr;

  UIMDRV_MSG_HIGH_1(uim_ptr->id,"Processsing uim_generic_command_response for state 0x%x",
               curr_uim_generic_state);
  switch (curr_uim_generic_state)
  {
    case UIM_POWER_DOWN_ST:
      {
        ++uim_ptr->command.generic_state_ptr;
        completed_status = UIM_CMD_COMPLETED;
      }
      break;
    case UIM_POWER_UP_ST:                     /* Power Up state */
      {
        /* Get the next state. */
        ++uim_ptr->command.generic_state_ptr;
      } /* end case - UIM_POWER_UP_ST */
      break;

    case UIM_RESET_ST:                        /* Reset State */
      {
#ifdef FEATURE_UIM_BTSAP_CLIENT
        if(IS_BTSAP_HANDLE_ACTIVE(uim_ptr))
        {
          /* Need not parse ATR as we need only historical char data
             which is obtained from uim_btsap_send_atr() */
          ++uim_ptr->command.generic_state_ptr;
          break;
        }
#endif /* FEATURE_UIM_BTSAP_CLIENT */
        /* variable is static so re-initialize */
        uim_ptr->flag.dont_skip_PPS = FALSE;
        uim_ptr->flag.is_euicc      = FALSE;

        /* Print any data received from the card before it's examined */
        for ( i = 0; i < rsp_ptr->cmd_rsp_size; i+=4)
        {
          tempATR = 0;
          //Grouping 4 bytes together to print to reduce F3s
          for ( j = 0; (j < 4)&&(i+j < rsp_ptr->cmd_rsp_size); j++)
          {
            tempATR = (tempATR<<8)|(rsp_ptr->rsp.data[i+j]);
          }
          UIMDRV_MSG_MED_3(uim_ptr->id, "atr byte[%d-%d]:%x",
                           i, i+j-1, tempATR);
        }

        /* Check to see if there was a convention error */
        if (rsp_ptr->cmd_status == UIM_CONVENTION_ERROR)
        {
            /* If the number of times we are switching between direct & inverse
               convention is greater than the max number of attempts */
            if (++uim_ptr->debug.convention_change_count > UIM_MAX_NUM_ATTEMPTS)
            {
              /* Return a cmd error after setting the command count to MAX */
              uim_ptr->command.cmd_ptr->hdr.cmd_count = UIM_MAX_NUM_ATTEMPTS;
              return(UIM_CMD_ERR);
            }
            else
            {
              /* Return a convention changes error that will not increment the
              wake-up or power-up command count */
              return(UIM_CONVENTION_CHANGE);
            }
        }
        /* Check if there was a problem with the Reset
           also return error if by some chance the response size becomes 0 */
        if (rsp_ptr->cmd_status != UIM_DONE || 
            0 == rsp_ptr->cmd_rsp_size)
        {
          return(UIM_CMD_ERR);
        } /* end if - there was a problem with the Reset */

        uim_ptr->atr.atr_buf.num_bytes = (byte)rsp_ptr->cmd_rsp_size;
        /*if the atr is being recieved from RX_ISR (local sim) 
          then there is no issue, as there are already checks present there for max length.
          the check hereis for precaution, if the ATR is recieved from a different source (like remote sim)*/
        if(UIM_MAX_ATR_CHARS < uim_ptr->atr.atr_buf.num_bytes)
        {
          uim_ptr->atr.atr_buf.num_bytes = UIM_MAX_ATR_CHARS;
        }
        uim_memscpy(uim_ptr->atr.atr_buf.data,
               sizeof(uim_ptr->atr.atr_buf.data),
               rsp_ptr->rsp.data,
               rsp_ptr->cmd_rsp_size);

        /* Validate the ATR here */
        /* Check for an intelligible start byte */
        if ((uim_ptr->atr.atr_buf.data[UIM_ATR_TS_BYTE_INDEX] !=
                                                    UIM_ATR_TS_DIRECT_CONV) &&
            (uim_ptr->atr.atr_buf.data[UIM_ATR_TS_BYTE_INDEX] !=
                                                    UIM_ATR_TS_INVERSE_CONV))
        {
          return(UIM_CMD_ERR);
        }

        receive_mode = UIM_WAITING_FOR_T0;

        num_bytes_to_skip = 2;

        /* Assume there are no historical characters */
        num_hist_chars = 0;
        uim_ptr->atr.hist_char_buf.length = num_hist_chars;

        /* Initialize the check byte to TS */
        check_byte = uim_ptr->atr.atr_buf.data[0];

        /* Reset the protocols */
        uim_ptr->state.prot_sup_by_card.t_0 = FALSE;
        uim_ptr->state.prot_sup_by_card.t_1 = FALSE;

        /* Process all the bytes in the receive FIFO */
        for (i = 0;i < uim_ptr->atr.atr_buf.num_bytes;i++)
        {
          if(UIM_END == receive_mode)
          {
            /*All the expected ATR bytes are recieved,but there are still bytees
             in the buffer. ignore these bytes and modify the atr length 
              to not parse these extra bytes*/
             uim_ptr->atr.atr_buf.num_bytes = i;
             break;
          }

          /* Get the next received byte. */
          rx_value = uim_ptr->atr.atr_buf.data[i];

          /* Keep a rolling value of the check byte */
          check_byte ^= rx_value;

          num_bytes_to_skip--;

          if ( voltage_class_indicator_scan == TRUE)
          {
            voltage_class_indicator_scan = FALSE;
            voltage_class_indicator = ( rx_value & UIM_UI_MASK );
            uim_ptr->flag.voltage_class_known_from_atr = TRUE;

            if(((voltage_class_indicator & UIM_UI_1_8_UIM_UICC)
              &&(voltage_class_indicator & UIM_UI_3V_UIM_UICC)))
            {
              uim_ptr->flag.voltage_class_c_and_b_supported = TRUE;
            }
            /* Determine the current voltage on the interface */
            switch (uim_ptr->state.current_voltage_class)
            {
              case UIM_VOLTAGE_CLASS_C:
                {
                  /* Check if the card does not support this voltage */
                  if ((voltage_class_indicator & UIM_UI_1_8_UIM_UICC) !=
                      UIM_UI_1_8_UIM_UICC)
                  {
                    UIMDRV_MSG_LOW_0(uim_ptr->id, "Card does not support 1.8V");
                    /* Check if card supports other supported voltages */
                    if ((voltage_class_indicator & UIM_UI_3V_UIM_UICC) ==
                        UIM_UI_3V_UIM_UICC)
                    {
                      if (uim_ptr->hardware.uim_ldo.customize_ldo_setting &&
                          uim_ptr->hardware.uim_ldo.desired_voltage_class == UIM_VOLTAGE_CLASS_C)
                      {
                        UIMDRV_MSG_HIGH_0(uim_ptr->id,"LDO behaviour modified to work at specific voltage class");
                        return(UIM_VOLTAGE_MISMATCH);
                      }
                      /* Switch to different voltage */
                      uim_power_down(uim_ptr);
                      uim_timed_sleep(UIM_ISO7816_VOLTAGE_SWITCH_PWR_DOWN_DELAY,
                                      uim_ptr,
                                      UIM_ALL_ZERO_SIG);
                      /* Try the indicated voltage class */
                      uim_ptr->state.current_voltage_class = UIM_VOLTAGE_CLASS_B;
                      return(UIM_SWITCH_VOLTAGE);
                    }
                    else /* No voltages supported */
                    {
                      UIMDRV_MSG_MED_0(uim_ptr->id, "Card does not support 3V mode either");
                      return(UIM_VOLTAGE_MISMATCH);
                    } /* end if - card supports other voltage */
                  } /* end if - card does not support this voltage */

                  /* Voltage class of 1.8V is supported
                     Check if FI value is > 0 and if clk freq needs to be changed */
                  if ( UIM_IS_CLK_FREQ_AT_4_8_MHZ(uim_ptr->id) &&
                       UIM_ATR_FI_VALUE_IS_ZERO_OR_INVALID(uim_ptr->id) )
                  {
                    UIMDRV_MSG_LOW_0(uim_ptr->id,"UIM_RESET_ST FI is 0");
                    uim_power_down(uim_ptr);
                    uim_hotswap_reset_slot(uim_ptr);
                    UIM_SWITCH_CLK_FREQ_TO_3_8_MHZ(uim_ptr->id);
                    SET_UIM_BAUD_RATE_SLOT ( uim_ptr->id, UIM_CRCF_372_1, UIM_BRAF_1 );
                    return(UIM_CLK_FREQ_CHANGE_NEEDED);
                  }

                } /* end case - UIM_VOLTAGE_1_8V */
                break;

              case UIM_VOLTAGE_CLASS_B:
                {
                  /* Check if the card does not support this voltage */
                  if ((voltage_class_indicator & UIM_UI_3V_UIM_UICC) !=
                      UIM_UI_3V_UIM_UICC)
                  {
                    UIMDRV_MSG_LOW_0(uim_ptr->id, "Card does not support 3V");
                    return(UIM_VOLTAGE_MISMATCH);
                  } /* end if - card does not support this voltage */
                  /* Voltage class of 3V is supported
                     Check if FI value is > 0 and if clk freq needs to be changed */
                  if (UIM_IS_CLK_FREQ_AT_4_8_MHZ(uim_ptr->id) &&
                      UIM_ATR_FI_VALUE_IS_ZERO_OR_INVALID(uim_ptr->id))
                  {
                    UIMDRV_MSG_LOW_0(uim_ptr->id,"UIM_RESET_ST FI is 0");
                    uim_power_down(uim_ptr);
                    uim_hotswap_reset_slot(uim_ptr);
                    UIM_SWITCH_CLK_FREQ_TO_3_8_MHZ(uim_ptr->id);
                    SET_UIM_BAUD_RATE_SLOT ( uim_ptr->id, UIM_CRCF_372_1, UIM_BRAF_1 );
                    return(UIM_CLK_FREQ_CHANGE_NEEDED);
                  }
                } /* end case - UIM_VOLTAGE_3V*/
                break;

              default:
                {
                  UIMDRV_MSG_ERR_0(uim_ptr->id, "Bad UIM voltage class" );
                  return(UIM_VOLTAGE_MISMATCH);
                } /* end case - default */
            } /* end switch - voltage class */
          } /* voltage_class_indicator_scan */

          /* Determine if this byte should be processed. */
          if (num_bytes_to_skip == 0)
          {
            /* Process the byte based on what is expected by this function */
            switch (receive_mode)
            {
              case UIM_WAITING_FOR_T0:    /* Expecting the T0 character */
                {
                  /* Save the number of historical bytes */
                  num_hist_chars = (rx_value & UIM_K_FIELD);

                  /* Determine how many bytes follow this format byte. */
                  num_bytes_to_skip =
                    uim_y_format_table[rx_value >> UIM_Y_FIELD_SHIFT];

                  /* Determine if there will be global interface bytes.*/
                  if (rx_value & UIM_TD_PRESENT)
                  {
                    receive_mode = UIM_WAITING_FOR_TD;
                  }
                  else
                  {
                    /* The number of bytes to skip include the number of
                    bytes that follow this format character and the
                    number of historical characters. */
                    num_bytes_to_skip += num_hist_chars;

                    /* There will NOT be a TCK.  Determine if any bytes
                       follow in this ATR.*/
                    if (num_bytes_to_skip > 0)
                    {
                      /* Wait for the rest of the bytes of the ATR.*/
                      receive_mode = UIM_WAITING_FOR_END;
                    } /* end if - more bytes to skip. */
                    else
                    {
                      receive_mode = UIM_END;
                    }
                  } /* end if - TD byte is present in following bytes */
                } /* end case - UIM_WAITING_FOR_T0 */
                break;

              case UIM_WAITING_FOR_TD:    /* Expecting a TD character */
                {
                  td_present = TRUE;
                  number_of_td++;

                  /* Determine how many bytes follow this format byte.*/
                  num_bytes_to_skip =
                    uim_y_format_table[rx_value >> UIM_Y_FIELD_SHIFT];

                  /* Determine if there is a protocol other than 0. */
                  if ((rx_value & UIM_T_FIELD) == (int)UIM_T_0_PROTOCOL)
                  {
                    uim_ptr->state.prot_sup_by_card.t_0 = TRUE;
                    check_byte_expected = FALSE;
                  }
#if defined( FEATURE_UIM_T_1_SUPPORT )
                  else if ((rx_value & UIM_T_FIELD) == (int)UIM_T_1_PROTOCOL)
                  {
                    uim_ptr->state.prot_sup_by_card.t_1 = TRUE;
                    check_byte_expected = TRUE;
                  }
#endif /* FEATURE_UIM_T_1_SUPPORT */
                  else
                  {
                    /* This indicates the TCK byte is expected */
                    check_byte_expected = TRUE;
                  } /* end if - a protocol other than 0 */

                  /* check if we have TA(i) after the first occurrence of T=15 in TD(i-1)
                  for i > 2 (which means we have XI and UI ) */
                  if ( ((rx_value & UIM_T_FIELD) == UIM_T_FIELD) &&
                       (rx_value & UIM_ATR_TA_PRESENT) &&
                       (number_of_td >= 2) &&
                       (uim_ptr->command.cmd_ptr->hdr.protocol == UIM_UICC) )
                  {
                    /* The next TA byte has the voltage class information we
                       need to process it */
                    voltage_class_indicator_scan = TRUE;
                  }

                  /* Determine if there will be another interface byte.*/
                  if (rx_value & UIM_TD_PRESENT)
                  {
                    receive_mode = UIM_WAITING_FOR_TD;
                  }
                  else
                  {
                    /* Determine if the check byte is expected */
                    if (check_byte_expected)
                    {
                      /* The number of bytes to skip include the
                      number of bytes that follow this format char
                      and the number of historical characters.The
                      check byte is also included.*/
                      num_bytes_to_skip += num_hist_chars + 1;

                      /* Wait for the check byte.*/
                      receive_mode = UIM_WAITING_FOR_TCK;
                    }
                    else /* No check byte is expected */
                    {
                      /* The number of bytes to skip include the num of
                      bytes that follow this format character and the
                      number of historical characters.*/
                      num_bytes_to_skip += num_hist_chars;

                      /* There will NOT be a TCK.  Determine if any
                         bytes follow in this ATR.*/
                      if (num_bytes_to_skip > 0)
                      {
                        /* Wait for the rest of bytes of the ATR.*/
                        receive_mode = UIM_WAITING_FOR_END;
                      }
                      else /* There are no more bytes in the ATR. */
                      {
                        receive_mode = UIM_END;
                      } /* end if - more bytes to come.*/
                    } /* end if - more bytes to come.*/
                  } /* end if - TD byte is present in following bytes */

                } /* end case - UIM_WAITING_FOR_TD */
                break;

              case UIM_WAITING_FOR_TCK:
                {
                  /* Expecting the TCK character.  The ATR should have
                     been completely received.  No other bytes are
                     expected at this time */

                  /* Determine if the ATR was received with an error */
                  if (check_byte != 0)
                  {
                    /* We have recd an incorrect ATR */
                    return (UIM_CMD_ERR);
                  } /* end if - received an ATR with an error. */

                  /* Copy the historical characters to the historical characters
                     buffer. */
                  uim_ptr->atr.hist_char_buf.length = num_hist_chars;

                  /* This addresses the first historical character.
                     This takes into account the TCK byte at the end of
                     the buffer. */
                  uim_memscpy (uim_ptr->atr.hist_char_buf.data,
                          sizeof(uim_ptr->atr.hist_char_buf.data),
                          (byte*) &(uim_ptr->atr.atr_buf.
                                                    data[(i - num_hist_chars)]),
                          num_hist_chars);
                  receive_mode = UIM_END;
                } /* end case - UIM_WAITING_FOR_TCK */
                break;

              case UIM_WAITING_FOR_END:
                {
                  /* Expecting the last of the ATR with NO TCK char expected
                     The ATR should have been completely received. No other
                     bytes are expected at this time */

                  /* Copy the historical characters to the historical char
                     buffer. */
                  uim_ptr->atr.hist_char_buf.length = num_hist_chars;

                  /* This addresses the first historical character. The index
                     of the first historical character is adjusted by one since
                     i is the index of the last historical character. */
                  uim_memscpy (uim_ptr->atr.hist_char_buf.data,
                          sizeof(uim_ptr->atr.hist_char_buf.data),
                          (byte*) &(uim_ptr->atr.atr_buf.
                                                    data[(i-num_hist_chars)+1]),
                          num_hist_chars);
                  receive_mode = UIM_END;
                }
                break;

              default:
                {

                }
                break;
            } /* end switch - receive_mode */
          } /* end if - this function should process this byte */
        } /* end while - process all the bytes in the receive FIFO. */

        if(UIM_END != receive_mode)
        {
          /*We have received an invalid ATR*/
          return(UIM_CMD_ERR);
        }
#if defined( FEATURE_UIM_T_1_SUPPORT )
        /* If the preferred protocol is T = 1 */
        if (uim_ptr->t1.nv_pref_protocol == NV_UIM_PREF_PROTOCOL_T_1)
        {
          /* If the card supports T=1 protocol */
          if (uim_ptr->state.prot_sup_by_card.t_1)
          {
            /* Set the current protocol to T = 1 */
            uim_ptr->state.operating_protocol = UIM_T_1_PROTOCOL;
            /* Determine if T=0 is supported also */
            if (uim_ptr->state.prot_sup_by_card.t_0)
            {
              /* Do not skip the PPS since there are two protocols supported
                 and we will use T=1 */
              uim_ptr->flag.dont_skip_PPS = TRUE;
            }/* end if - T=0 is also supported */
          }
          else if ((uim_ptr->state.prot_sup_by_card.t_0) || (!td_present))
          {
            /* Set the current protocol to T = 0 */
            uim_ptr->state.operating_protocol = UIM_T_0_PROTOCOL;
          }
          else
          {
            /* Return an error */
            uim_ptr->atr.atr_buf.num_bytes = 0;
            return (UIM_CMD_ERR);
          }
        }
        /* Else if the preferred protocol is T=0 or No preference */
        else if ((uim_ptr->t1.nv_pref_protocol == NV_UIM_PREF_PROTOCOL_T_0) ||
                 (uim_ptr->t1.nv_pref_protocol == NV_UIM_PREF_PROTOCOL_NONE))
        {
          if ((uim_ptr->state.prot_sup_by_card.t_0) || (!td_present))
          {
            /* Set the current protocol to T = 0 */
            uim_ptr->state.operating_protocol = UIM_T_0_PROTOCOL;
          }
          else if (uim_ptr->state.prot_sup_by_card.t_1)
          {
            /* Set the current protocol to T = 1 */
            uim_ptr->state.operating_protocol = UIM_T_1_PROTOCOL;
          }
          else
          {
            /* Return an error for ATR */
            uim_ptr->atr.atr_buf.num_bytes = 0;
            return (UIM_CMD_ERR);
          }
        }
#else
        if ((td_present) && (!uim_ptr->state.prot_sup_by_card.t_0))
        {
          uim_ptr->atr.atr_buf.num_bytes = 0;
          return (UIM_CMD_ERR);
        }
        else
#endif /* FEATURE_UIM_T_1_SUPPORT */
        {
          /* Check for TD1 byte */
          if (uim_ptr->atr.atr_buf.data[UIM_ATR_T0_BYTE] &
              (UIM_TD_PRESENT))
          {
            /* Calculate the TD1 index in the ATR */
            uim_atr_td1_index = UIM_ATR_T0_BYTE +
             uim_y_format_table[uim_ptr->atr.atr_buf.data[UIM_ATR_T0_BYTE]
               >> UIM_Y_FIELD_SHIFT];

            /* TA2 is present */
            if(uim_ptr->atr.atr_buf.data[uim_atr_td1_index]
               & UIM_ATR_TA_PRESENT)
            {
              /* Indicate that the TA2 is present */
              uim_atr_ta2_byte_present = TRUE;

              /* Get the TA2 byte */
              uim_atr_ta2_byte =
                 uim_ptr->atr.atr_buf.data[uim_atr_td1_index + 1];

              /* Is B8 bit in TA2 is set */
              if(uim_atr_ta2_byte & UIM_BIT8_CHECK)
              {
                /* B8 bit in TA2 is set */
                uim_atr_b8_bit_ta2 = TRUE;
              }

              /* Is B5 bit in TA2 is set */
              if(uim_atr_ta2_byte & UIM_BIT5_CHECK)
              {
                /* B5 bit in TA2 is set */
                uim_atr_b5_bit_ta2 = TRUE;
              }
            }
          }

          /* The TC1 byte may be in the first byte after T0 */
          uim_proc_atr_byte_index = UIM_ATR_TA1_BYTE;

          /* If TA1 is present, we want to do PPS, only if TA1 is not equal to
            '11' or '01' */
          if (uim_ptr->atr.atr_buf.data[UIM_ATR_T0_BYTE] &
                                                         (UIM_ATR_TA_PRESENT))
          {
            /* Count the number of bytes to TC1 */
            uim_proc_atr_byte_index++;

            /* Check TA1 for default values */
            if (((uim_ptr->atr.atr_buf.data[UIM_ATR_TA1_BYTE]) !=
                                                         UIM_NO_PPS_VALUE1) &&
                ((uim_ptr->atr.atr_buf.data[UIM_ATR_TA1_BYTE]) !=
                                                         UIM_NO_PPS_VALUE2))
            {
              /* Do not skip PPS.  Use these parameters */
              uim_ptr->flag.dont_skip_PPS = TRUE;

              /* Check if we can handle these parameters */
              FI = (uim_clock_rate_conversion_factor_type)
                (uim_ptr->command.rsp_buf.rsp.data[UIM_TA1_INDEX]
                   >> UIM_FI_SHIFT_OF_TA1);
              DI = (uim_baud_rate_adjustment_factor_type)
                (uim_ptr->command.rsp_buf.rsp.data[UIM_TA1_INDEX]
                   & UIM_DI_MASK_OF_TA1);

              /* Place parameters back into the ATR for processing of the
                 PPS */
              uim_ptr->command.rsp_buf.rsp.data[UIM_TA1_INDEX] =
                (uim_ptr->command.rsp_buf.rsp.data[UIM_TA1_INDEX] & (~UIM_DI_MASK_OF_TA1))
                | (int)DI;
            } /* Check if TA1 values need a PPS */
            else
            {
              /* Skip the PPS */
              uim_ptr->flag.dont_skip_PPS = FALSE;
              /* Skip the subsequent specific mode check */
              skip_specific_mode_check = TRUE;
            }
          } /* check for TA1 */
          else
          {
            /* Skip the PPS */
            uim_ptr->flag.dont_skip_PPS = FALSE;
            /* Skip the subsequent specific mode check */
            skip_specific_mode_check = TRUE;
          }

          /* Enter the specific mode check */
          if( skip_specific_mode_check == FALSE)
          {
            /* Is SIM in specific mode? */
            if( uim_atr_ta2_byte_present == TRUE )
            {
              /* Don't perform PPS as the SIM is in specific mode */
              uim_ptr->flag.dont_skip_PPS = FALSE;

              /* Is TA1 supported by the platform? */
              if( uim_is_fi_di_supported(uim_ptr, FI, DI) )
              {
                /* Check if b5 bit is set in TA2 */
                if(uim_atr_b5_bit_ta2 == FALSE)
                {
                  skip_mode_changeability_check = TRUE;
                }
                else
                {
                  /* Use implicit values for F and D.  Cause this by
                  indicating TA1 is not in the ATR */
                  uim_ptr->command.rsp_buf.rsp.data[UIM_ATR_T0_BYTE] &= ~UIM_ATR_TA_PRESENT;
                }
              }

              /* Enter the mode changeability check */
              if(skip_mode_changeability_check != TRUE)
              {
                /* Check if mode change is possible */
                if(uim_atr_b8_bit_ta2 == FALSE)
                {
                  /* Check if we had done warm reset earlier within the same powerup */
                  if(uim_ptr->debug.warm_reset_count > 0)
                  {
                    /* Set this to 0 as this is a static variable */
                    uim_ptr->debug.warm_reset_count = 0;

                    /* Is Implicit baud-rate possible? */
                    if(uim_atr_b5_bit_ta2 == TRUE)
                    {
                      /* Use implicit values for F and D.  Cause this by
                      indicating TA1 is not in the ATR */
                      uim_ptr->command.rsp_buf.rsp.data[UIM_ATR_T0_BYTE] &= ~UIM_ATR_TA_PRESENT;
                    }
                    else
                    {
                      /* Return error as this SIM is not useable */
                      /* Convert this to MSG_ERROR, if need be */
                      UIMDRV_MSG_HIGH_0(uim_ptr->id, "THIS SIM IS NOT USEABLE BY OUR TERMINAL DEVICE");
                      uim_ptr->atr.atr_buf.num_bytes = 0;
                      return (UIM_CMD_ERR);
                    }
                  }
                  else
                  {
                    if(uim_nv_is_feature_enabled(UIMDRV_FEATURE_ENABLE_SIM_MODE_CHANGE_VIA_WARM_RESET,
                                                 uim_ptr))
                    {
                      uim_ptr->debug.warm_reset_count++;
                    }
                    else
                    {
                      if(uim_atr_b5_bit_ta2 == TRUE)
                      {
                        /* Use implicit values for F and D.  Cause this by
                           indicating TA1 is not in the ATR */
                        uim_ptr->command.rsp_buf.rsp.data[UIM_ATR_T0_BYTE] &= ~UIM_ATR_TA_PRESENT;
                      }
                      else
                      {
                        /* Return error as this SIM is not useable */
                        UIMDRV_MSG_HIGH_0(uim_ptr->id,"THIS SIM IS NOT USEABLE BY OUR TERMINAL DEVICE");
                        uim_ptr->atr.atr_buf.num_bytes = 0;
                        return (UIM_CMD_ERR);
                      }
                    } /* UIMDRV_FEATURE_ENABLE_SIM_MODE_CHANGE_VIA_WARM_RESET */
                  }
                }
                else /* Mode change is not possible */
                {
                  /* Reset the warm reset count to 0 */
                  uim_ptr->debug.warm_reset_count = 0;

                  /* Is B5 bit in TA2 byte set? */
                  if(uim_atr_b5_bit_ta2 == TRUE)
                  {
                    /* Use implicit values for F and D.  Cause this by
                    indicating TA1 is not in the ATR */
                    uim_ptr->command.rsp_buf.rsp.data[UIM_ATR_T0_BYTE] &= ~UIM_ATR_TA_PRESENT;
                  }
                  else
                  {
                    /* Return error as this SIM is not useable */
                    UIMDRV_MSG_HIGH_0(uim_ptr->id,"THIS SIM IS NOT USEABLE BY OUR TERMINAL DEVICE");
                    uim_ptr->atr.atr_buf.num_bytes = 0;
                    return (UIM_CMD_ERR);
                  }
                }
              } /* skip_mode_changeability_check */
              else
              {
                uim_ptr->debug.warm_reset_count = 0;
              }
            }
            else /* SIM is not in specific mode */
            {
              uim_ptr->debug.warm_reset_count = 0;
              uim_ptr->flag.dont_skip_PPS = TRUE;

              /* Is TA1 supported by the platform? */
              if( uim_is_fi_di_supported(uim_ptr, FI, DI) )
              {
                uim_ptr->flag.dont_skip_PPS = TRUE;
              }
              else /* TA1 not supported by the platform */
              {
                /* Fall back to 0x96 */
                DI = UIM_BRAF_32;
                /* Place parameters back into the ATR for processing of the
                   PPS */
                uim_ptr->command.rsp_buf.rsp.data[UIM_TA1_INDEX] =
                  (uim_ptr->command.rsp_buf.rsp.data[UIM_TA1_INDEX] & (~UIM_DI_MASK_OF_TA1))
                  | (int)DI;

                /* Perform PPS */
                uim_ptr->flag.dont_skip_PPS = TRUE;
              }

            } /* else */
          } /* if skip_specific_mode_check ... */
          else
          {
            /* set the warm reset count to 0 */
            uim_ptr->debug.warm_reset_count = 0;
          }

          /* Now check if TB1 is present */
          if (uim_ptr->atr.atr_buf.data[UIM_ATR_T0_BYTE] &
                                                          (UIM_ATR_TB_PRESENT))
          {
            /* Count the number of bytes to TC1 */
            uim_proc_atr_byte_index++;
          }

          if (uim_nv_is_feature_enabled(UIMDRV_FEATURE_HANDLE_TC1_BYTE_FOR_EXTRA_GUARD_TIME, uim_ptr) == FALSE)
          {
            /* If TC1 is present, check to see if it is within acceptable
               values */
            if (uim_ptr->atr.atr_buf.data[UIM_ATR_T0_BYTE] & (UIM_ATR_TC_PRESENT))
            {
              /* Check if TC1 is acceptable */
              if ((uim_ptr->atr.atr_buf.data[uim_proc_atr_byte_index]) !=
                                                                  UIM_TC1_IS_0)
              {
                if ((uim_ptr->atr.atr_buf.data[uim_proc_atr_byte_index]) !=
                     UIM_TC1_IS_255)
                {
                  /* Fail the ATR from the SIM */
                  uim_ptr->atr.atr_buf.num_bytes = 0;
                  return (UIM_CMD_ERR);
                }
                else /* TC1 is the minimum value */
                {
                  /* Use the minimum guardtime size */
                  uim_ptr->t1.use_minimum_guardtime_size = TRUE;
                } /* end if - TC1 is not within the acceptable range */
              } /* end if - TC1 is not 0 */
            } /* end if - TC1 is present */
          }
          else
          {
            /* Check for TC1 byte presence in ATR */
            if (uim_ptr->atr.atr_buf.data[UIM_ATR_T0_BYTE] &
                                                         (UIM_ATR_TC_PRESENT))
            {
              /* store the TC1 byte value */
              uim_ptr->atr.atr_tc1_byte =
              uim_ptr->atr.atr_buf.data[uim_proc_atr_byte_index];
            } /* end if - TC1 is present */
          }

          /* Check if TD1 is present */
          if (uim_ptr->atr.atr_buf.data[UIM_ATR_T0_BYTE] & (UIM_TD_PRESENT))
          {
            /* Determine the TD1 index */
            uim_atr_td_index = UIM_ATR_T0_BYTE +
              uim_y_format_table[uim_ptr->atr.atr_buf.data
                [UIM_ATR_T0_BYTE] >> UIM_Y_FIELD_SHIFT];

            /* Move the TD2 index to process the bytes after TD2 */
            uim_proc_atr_byte_index = uim_atr_td_index;

            /* Check if TA2 is present by checking TD1 */
            if (uim_ptr->atr.atr_buf.data[uim_atr_td_index] &
                 UIM_ATR_TA_PRESENT)
            {
              /* TA2 is the specific mode byte */
              ++uim_proc_atr_byte_index;
#if defined( FEATURE_UIM_T_1_SUPPORT )
              /* Set the current protocol to the specific mode */
              uim_ptr->state.operating_protocol =
                (uim_transport_protocol_type)
                (uim_ptr->atr.atr_buf.data[uim_proc_atr_byte_index] &
                                                            UIM_T_FIELD);
#endif /* FEATURE_UIM_T_1_SUPPORT */
            } /* end if - TA2 is present */

            /* Check if TB2 is present by checking TD1 */
            if (uim_ptr->atr.atr_buf.data[uim_atr_td_index] &
                 UIM_ATR_TB_PRESENT)
            {
              /* Do not process TB2.  Skip to next byte */
              ++uim_proc_atr_byte_index;
            } /* end if - TB2 is present */

            /* Check if TC2 is present by checking TD1 */
            if (uim_ptr->atr.atr_buf.data[uim_atr_td_index] &
                 (UIM_ATR_TC_PRESENT))
            {
              /* Determine the TC2 index.  Consider only those bytes to TC2 */
              /* Save the work waiting time */
              uim_ptr->state.WI =
                uim_ptr->atr.atr_buf.data[++uim_proc_atr_byte_index];
              /* Capping the WI value to 10 to prevent unreasonable long WWT time */
              if (uim_ptr->state.WI == 0 || uim_ptr->state.WI == 0xFF)
              {
                uim_ptr->state.WI = 10;
              }
            } /* end if - TC2 is present */

#if defined( FEATURE_UIM_T_1_SUPPORT )
            /* Set initial values of send sequence numbers */
            uim_ptr->t1.me_send_sequence_num = UIM_T_1_INIT_SEQ_NUM;
            uim_ptr->t1.card_send_sequence_num = UIM_T_1_INIT_SEQ_NUM;

            /* Set default values of IFS */
            uim_ptr->t1.t_1_ifsc = UIM_T_1_DEFAULT_IFS;
            uim_ptr->t1.t_1_ifsd = UIM_T_1_DEFAULT_IFS;

            /* Set the default waiting times indeces for T=1 protocol */
            uim_ptr->t1.t_1_bwi = UIM_T_1_DEFAULT_BWI;
            uim_ptr->t1.t_1_cwi = UIM_T_1_DEFAULT_CWI;

            /* Set the default EDC controls */
            uim_ptr->t1.t_1_edc = UIM_T_1_EDC_LRC;
            uim_ptr->t1.epilogue_field_size = UIM_T_1_LRC_SIZE;
            /* Check if TD2 is present by checking TD1 */
            if (uim_ptr->atr.atr_buf.data[uim_atr_td_index] &
                                                              (UIM_TD_PRESENT))
            {
              /* Determine the TD2 index. Consider only those bytes to TD2*/
              uim_atr_td_index +=
              uim_y_format_table[(uim_ptr->atr.atr_buf.data
                [uim_atr_td_index] & 0xF0) >> UIM_Y_FIELD_SHIFT];

              /* Check to see if T=1 support is indicated by TD2 */
              if ((uim_ptr->atr.atr_buf.data[uim_atr_td_index] &
                  (UIM_T_FIELD)) == (int)UIM_T_1_PROTOCOL)
              {
                /* Move the TD2 index to process the bytes after TD2 */
                uim_proc_atr_byte_index = uim_atr_td_index;

                /* Get the protocol specific parameters */
                if (uim_ptr->atr.atr_buf.data[uim_atr_td_index] &
                     UIM_ATR_TA_PRESENT)
                {
                  /* Store the Information field size for ICC */
                  uim_ptr->t1.t_1_ifsc =
                    uim_ptr->atr.atr_buf.data[++uim_proc_atr_byte_index];
                } /* end if - TA(i) is present */

                if (uim_ptr->atr.atr_buf.data[uim_atr_td_index] &
                     UIM_ATR_TB_PRESENT)
                {
                  /* Store the Character waiting time index */
                  uim_ptr->t1.t_1_cwi =
                    (uim_ptr->atr.atr_buf.data[++uim_proc_atr_byte_index] &
                      UIM_T_1_CWI_FIELD);
                  /* Store the block waiting time index */
                  uim_ptr->t1.t_1_bwi =
                    ((uim_ptr->atr.atr_buf.data[uim_proc_atr_byte_index] &
                      UIM_T_1_BWI_FIELD) >> UIM_T_1_BWI_FIELD_SHIFT);
                } /* end if - TB(i) is present */

                if (uim_ptr->atr.atr_buf.data[uim_atr_td_index] &
                      UIM_ATR_TC_PRESENT)
                {
                  if (uim_ptr->atr.atr_buf.data[++uim_proc_atr_byte_index]&
                        UIM_T_1_EDC_FIELD)
                  {
                    /* Check to see if CRC is used for error detection */
                    uim_ptr->t1.t_1_edc = UIM_T_1_EDC_CRC;
                    uim_ptr->t1.epilogue_field_size = UIM_T_1_CRC_SIZE;
                  } /* The CRC indication bit is set */
                } /* end if - TC(i) is present */
              } /* end if - T=1 is supported */
              /* Check to see if  T=15 is indicated by TD2 */
              else if ((uim_ptr->atr.atr_buf.data[uim_atr_td_index] &
                  (UIM_T_FIELD)) == (int)UIM_T_FIELD &&
                  (!uim_ptr->flag.toggle_instrn_class))
              {
                /* Move the TD2 index to process the bytes after TD2 */
                uim_proc_atr_byte_index = uim_atr_td_index;
                if (uim_ptr->atr.atr_buf.data[uim_atr_td_index] &
                     UIM_ATR_TA_PRESENT)
                {
                  uim_proc_atr_byte_index++;
                }
                /* Check to see if TB3 is present */
                if (uim_ptr->atr.atr_buf.data[uim_atr_td_index] &
                     UIM_ATR_TB_PRESENT)
                {
                  if( UIM_UICC_CLF_INTERFACE_SUPPORTED == 
                      (uim_ptr->atr.atr_buf.data[++uim_proc_atr_byte_index] 
                                       & UIM_UICC_CLF_INTERFACE_SUPPORTED)
                    )
                  {
                    uim_ptr->flag.uicc_supports_swp = TRUE;
                  }
#ifdef FEATURE_UIM_USB_UICC
                  /* Check to see if USB UICC is supported */
                  if (uim_ptr->atr.atr_buf.data[uim_proc_atr_byte_index] == UIM_USB_SUPPORTED)
                  {
                    /* set a flag to indicate that card support usb mode.*/
                    uim_ptr->usb.usb_mode_card_supported = TRUE;
                    UIMDRV_MSG_HIGH_0(uim_ptr->id,"The card supports USB UICC");
                  }
#endif /* FEATURE_UIM_USB_UICC */
                  /* Check to see if this is eUICC */
                  if ( UIM_EUICC_SUPPORTED == 
                        (uim_ptr->atr.atr_buf.data[uim_proc_atr_byte_index] & UIM_EUICC_SUPPORTED) )
                  {
                    /* set a flag to indicate that this is eUICC */
                    uim_ptr->flag.is_euicc = TRUE;
                    UIMDRV_MSG_HIGH_0(uim_ptr->id,"This is an eUICC card");
                  }/* end if - if this is eUICC */
                } /* end if - TB3 is present */
              }/* end if - T=15 in TD2 */

              /* Check if TD3 is present */
              if (uim_ptr->atr.atr_buf.data[uim_atr_td_index] &
                      UIM_ATR_TD_PRESENT)
              {
                /* Determine the TD3 index. */
                uim_atr_td_index +=
                uim_y_format_table[(uim_ptr->atr.atr_buf.data
                  [uim_atr_td_index] & 0xF0) >> UIM_Y_FIELD_SHIFT];
                  uim_proc_atr_byte_index = uim_atr_td_index;

                /* Check if TA4 is present */
                if (uim_ptr->atr.atr_buf.data[uim_atr_td_index] &
                      UIM_ATR_TA_PRESENT)
                {
                  uim_proc_atr_byte_index = uim_atr_td_index;

                  /* extract the clock stop bits from TA4 */
                  uim_ta4_clock_stop_bits =
                      (uim_ptr->atr.atr_buf.
                         data[++uim_proc_atr_byte_index])>>
                                 UIM_TA4_CLOCK_STOP_FIELD_SHFT;

                  /* Check if the UIM clock can be stopped */
                  if(uim_ta4_clock_stop_bits == UIM_TA4_CLOCK_STOP_NOT_SUPPORTED)
                  {
                    /* UIM clock stop not allowed */
                    uim_ptr->state.clock_stop_cntrl = UIM_CLOCK_STOP_NOT_ALLOWED;
                  }
                  else if((uim_ta4_clock_stop_bits == UIM_TA4_CLOCK_STOP_LOW) ||
                           (uim_ta4_clock_stop_bits == UIM_TA4_CLOCK_STOP_NO_PREF))
                  {
                    /* UIM clock stop at LOW */
                    uim_ptr->state.clock_stop_cntrl = UIM_CLOCK_STOP_LOW;
                  }
                  else
                  {
                    /* UIM clock stop at HIGH */
                    uim_ptr->state.clock_stop_cntrl = UIM_CLOCK_STOP_HIGH;
                  }
                }
                /* Check to see if  T=15 is indicated by TD3 */
                if ((uim_ptr->atr.atr_buf.data[uim_atr_td_index] &
                   (UIM_T_FIELD)) == (int)UIM_T_FIELD &&
                   (!uim_ptr->flag.toggle_instrn_class))
                {
                  /* Check to see if TB4 is present */
                  if (uim_ptr->atr.atr_buf.data[uim_atr_td_index] &
                      UIM_ATR_TB_PRESENT)
                  {
                    if( UIM_UICC_CLF_INTERFACE_SUPPORTED == 
                        (uim_ptr->atr.atr_buf.data[++uim_proc_atr_byte_index] 
                                          & UIM_UICC_CLF_INTERFACE_SUPPORTED)
                      )
                    {
                      uim_ptr->flag.uicc_supports_swp = TRUE;
                    }
#ifdef FEATURE_UIM_USB_UICC
                    /* Check to see if USB UICC is supported */
                    if (uim_ptr->atr.atr_buf.data[uim_proc_atr_byte_index] == UIM_USB_SUPPORTED)
                    {
                      /* set a flag to indicate that card support usb mode.*/
                      uim_ptr->usb.usb_mode_card_supported = TRUE;
                      UIMDRV_MSG_HIGH_0(uim_ptr->id,"The card supports USB UICC");
                    }/* end if - USB UICC is supported */
#endif /* FEATURE_UIM_USB_UICC */
                    /* Check to see if this is eUICC */
                  if ( UIM_EUICC_SUPPORTED == 
                        (uim_ptr->atr.atr_buf.data[uim_proc_atr_byte_index] & UIM_EUICC_SUPPORTED) )
                    {
                      /* set a flag to indicate that this is eUICC */
                      uim_ptr->flag.is_euicc = TRUE;
                      UIMDRV_MSG_HIGH_0(uim_ptr->id,"This is an eUICC card");
                    }/* end if - if this is eUICC */
                  } /* end if - TB4 is present */
                }/* end if - T=15 in TD3 */
              }/* end if TD3 is present */
            } /* end if TD2 is present */
#endif /* FEATURE_UIM_T_1_SUPPORT */
          } /* end if - TD1 is present */
        } /* end if - T=0 is supported */

        /* Check if this is a card that needs more delay for response bytes */
        if (uim_ptr->atr.hist_char_buf.length > UIM_HIST_CHAR_CMP_SIZE)
        {
          if (!memcmp(uim_ptr->atr.hist_char_buf.data,
                      UIM_MORE_DELAY_HIST_CHARS,
                      UIM_HIST_CHAR_CMP_SIZE))
          {
            if ((uim_ptr->atr.hist_char_buf.data[UIM_HIST_CHAR_CMP_SIZE] >=
                 UIM_MIN_MINOR_REV_MORE_DELAY) &&
                (uim_ptr->atr.hist_char_buf.data[UIM_HIST_CHAR_CMP_SIZE] <=
                 UIM_MAX_MINOR_REV_MORE_DELAY))
            {
              /* Increase the response byte delay in the driver */
              uim_ptr->setting.response_byte_delay = UIM_RESPONSE_BYTE_DELAY_MORE;
            } /* end if - check the minor revision */
          } /* end if - the historical bytes match the delay more test
               string */
        } /* end if - there are enough historical bytes to check for more
             delay for the response byte */

        /* Indicate that we have already received the ATR so that we
         * will not try next voltage leavel when MULTIPLE_VOLTAGE is supported*/
        /* Move to the adjacent state if only we did not require a warm reset */
        if(uim_ptr->debug.warm_reset_count == 0)
        {
          uim_ptr->atr.atr_received = TRUE;
          ++uim_ptr->command.generic_state_ptr;
        }
      } /* end case - UIM_RESET_ST */
      break;
    case UIM_DELAY_AFTER_ATR_ST:              /* Introduce delay after ATR */
      {
        /* This is just to create a delay.  The delay has been done just
         * Get the next state as nothing needs to be done here
         */
        /* Adjust the generic state pointer according */
        if (FALSE == uim_ptr->flag.dont_skip_PPS)
        {
          /* Set the flag to TRUE */
          uim_ptr->atr.atr_pps_done = TRUE;

#ifdef FEATURE_UIM_USB_UICC
          if (uim_is_usb_enabled(uim_ptr))
          {
            if(uim_usb_has_remote_connection(uim_ptr) &&
               uim_usb_is_usb_mode_card_supported(uim_ptr) &&
               !uim_usb_iso_fallback_occurred(uim_ptr))
            {
              /* Uicc usb is enabled and the uicc card support usb mode,
                 close the iso interface and go to init usb interface */
              uim_iso_power_down_before_usb_attach(uim_ptr);
              /* Process the next state. */
              uim_ptr->command.generic_state_ptr += 2;
            }
            else
            {
              /* Clear USB UICC related timers and set iso_fallback_occurred to TRUE */
              uim_usb_clear_usb_timers(uim_ptr);
              uim_ptr->usb.usb_interface_status.iso_fallback_occurred = TRUE;
              /* Card doesn't support usb mode, then we goes on with ISO interface
                 power up states machine.*/
              uim_ptr->command.generic_state_ptr  = UIM_INTERNAL_PUP_STATES;
              uim_ptr->command.generic_state_ptr += 4;
            }
          }
          else
#endif /* FEATURE_UIM_USB_UICC */
          {
            /* Process the next state. */
            uim_ptr->command.generic_state_ptr += 2;
          }
        }
        else
        {
          ++uim_ptr->command.generic_state_ptr;
        } /* end if - skip the PPS */
      }
      break;

    case UIM_PPS_ST:                          /* PPS State */
      {
        /* Check if there was a problem with the Reset */
        if (rsp_ptr->cmd_status != UIM_DONE)
        {
          return(UIM_CMD_ERR);
        } /* end if - there was a problem with the Reset */

#if defined( FEATURE_UIM_T_1_SUPPORT )
        /* Indicate which protocol is running */
        UIMDRV_MSG_MED_1(uim_ptr->id, "The UIM is operating under T=0x%x",
                         uim_ptr->state.operating_protocol );
#endif /* FEATURE_UIM_T_1_SUPPORT */
        /* Set the flag to TRUE */
        uim_ptr->atr.atr_pps_done = TRUE;

#ifdef FEATURE_UIM_USB_UICC
        if (uim_is_usb_enabled(uim_ptr))
        {
          /*USB UICC is enabled and the uicc card support usb mode,
            close the iso interface and go to init usb interface*/
          if (uim_usb_has_remote_connection(uim_ptr) &&
              uim_usb_is_usb_mode_card_supported(uim_ptr) &&
              !uim_usb_iso_fallback_occurred(uim_ptr))
          {
            uim_iso_power_down_before_usb_attach(uim_ptr);
            /* Process the next state. */
            ++uim_ptr->command.generic_state_ptr;
          }
          else
          {
            /* Clear USB UICC related timers and set iso_fallback_occurred to TRUE */
            uim_usb_clear_usb_timers(uim_ptr);
            uim_ptr->usb.usb_interface_status.iso_fallback_occurred = TRUE;
            /* Card doesn't support usb mode, then we goes on with ISO interface
               power up states machine.*/
            uim_ptr->command.generic_state_ptr = UIM_INTERNAL_PUP_STATES;
            uim_ptr->command.generic_state_ptr += 4;
          }
        }
        else
#endif /* FEATURE_UIM_USB_UICC */
        {
          /* Process the next state. */
            ++uim_ptr->command.generic_state_ptr;
        }

      } /* end case - UIM_PPS_ST */
      break;

    case UIM_UPDATE_OP_PARAMS_ST:  /* Update Operational Parameters State */
      {
#if defined( FEATURE_UIM_T_1_SUPPORT )
        /* Set the current protocol to the operating protocol determined
           in the ATR processing */
        uim_ptr->state.current_protocol = uim_ptr->state.operating_protocol;

        /* Check if the current protocol is not T=1 */
        if (UIM_T_1_PROTOCOL != uim_ptr->state.current_protocol)
        {
          /* Skip the IFS negotiation */
          ++uim_ptr->command.generic_state_ptr;
        } /* end if - the protocol is not T=1 */
#endif /* FEATURE_UIM_T_1_SUPPORT */

        /* Process the next state. */
        ++uim_ptr->command.generic_state_ptr;
      } /* end case - UIM_UPDATE_OP_PARAMS_ST */
      break;

#if defined( FEATURE_UIM_T_1_SUPPORT )
    case UIM_IFS_NEG_ST:
      {
        /* Process the next state. */
        ++uim_ptr->command.generic_state_ptr;
      } /* end case - UIM_IFS_NEG_ST */
      break;
#endif /* FEATURE_UIM_T_1_SUPPORT */

    case UIM_TERMINAL_CAPABILITY_ST:
          /* We sent the terminal capability and printed
             the card response. Go to the next state */
          ++uim_ptr->command.generic_state_ptr;
      break;

    case UIM_CHECK_CHARACTERISTICS_ST:
      {
        if((UIM_PROBLEM == rsp_ptr->cmd_status) &&
           (FALSE == reattempted_select_mf_upon_tech_prob))
        {
          /* Do not increment state pointer incase 6F 00 is received
             select MF will be attempted again */  
          reattempted_select_mf_upon_tech_prob = TRUE;
          break;
        }
        else
        {
          reattempted_select_mf_upon_tech_prob = FALSE; 
        }
        /* If the class of the command was incorrect, set the flag to change
        the instruction class and return a CMD ERROR */
        if ((rsp_ptr->cmd_status == UIM_WRONG_CLASS) ||
            (rsp_ptr->cmd_status == UIM_UNKNOWN_INS) ||
            (rsp_ptr->cmd_status == UIM_PROBLEM &&
             (uim_ptr->setting.config_params.future_use[UIM_RFU_INDEX_BOOLEAN_FLAGS] &
              UIM_RFU_MASK_CHANGE_INST_CLASS_IF_TECH_PROB)
            ) ||
            (rsp_ptr->cmd_status == UIM_INCORRECT_PARAM_P3) ||
            (rsp_ptr->cmd_status == UIM_INCORRECT_PARAM_P1_2) ||
            (rsp_ptr->cmd_status == UIM_INCORRECT_PARAMS_P1_P2) ||
            (rsp_ptr->cmd_status == UIM_INCORRECT_PARAM_IN_DATA_FIELD) ||
            ((rsp_ptr->cmd_status == UIM_DONE) &&
             (uim_ptr->command.cmd_ptr->hdr.protocol == UIM_UICC) &&
             (rsp_ptr->rsp.data[0] != UIM_FCP_TEMPLATE_TAG))  ||
             /* It's possible this was a failure on the MF it's
             unlikely but we handle it by switching to ICC.
             As some buggy cards can work well with ICC protocol*/
            ((rsp_ptr->cmd_status != UIM_DONE) &&
             (uim_ptr->command.cmd_ptr->select.path.path[0] == UIM_MF_ID))
           )
        {
          /* Toggle the instruction class if not in recovery mode */
          if(!uim_ptr->flag.recovery_triggered)
          {
            uim_ptr->flag.toggle_instrn_class = !uim_ptr->flag.toggle_instrn_class;
          }

          if (++uim_ptr->card_cmd.instrn_toggle_count > UIM_MAX_CLA_TOGGLE_ATTEMPTS)
          {
             uim_ptr->command.cmd_ptr->hdr.cmd_count = UIM_MAX_NUM_ATTEMPTS;
             return(UIM_CMD_ERR);
          }
          else
          {
             uim_ptr->flag.bad_status_words_error = FALSE;
             return(UIM_INSTRN_CHANGE);
          }
        }
        else
        /* Check if there was a problem with the select */
        if (rsp_ptr->cmd_status != UIM_DONE)
        {
          /* Check to see if there are more directories to attempt.  If there are stay in the
             check_characteristics_st which will try to select the next DF */
          if(0x0000 == uim_check_characteristics_get_next_df(uim_ptr, uim_ptr->command.cmd_ptr->select.path.path[0]))
          {
            /* Since the SELECT failed, we do not know the preferred clock
               stop mode for this card. But, let us set it to LOW, as most
               cards supports it, to make sure the driver stops the clock
               and votes for sleep. */
            uim_ptr->state.clock_stop_cntrl = UIM_CLOCK_STOP_LOW;
            /* No more DFs to try.  Continue with the power-up */

            /* Skip the terminal capability state and continue to select and read the ICCID */
            uim_ptr->command.generic_state_ptr+=2;
          } /* end if - no more directories to select */
        }
        else /* The select was success.  Check the characteristics. */
        {
          /* Initialize the file characteristics byte */
          uim_ptr->state.file_char_byte = 0;

          if (uim_ptr->command.cmd_ptr->hdr.protocol == UIM_ICC)
          {
            /* update the path directly instead of calling uim_update_current_dir_icc
               use current_path[0] for default channel for ICC */
            uim_ptr->state.current_path[0].path.path[0] = 0x3F00;
            uim_ptr->state.current_path[0].path.path[1] = uim_ptr->command.cmd_ptr->select.path.path[0];
            uim_ptr->state.current_path[0].is_ef        = FALSE;
            uim_ptr->state.current_path[0].path.len     = 2;

            if (uim_ptr->command.cmd_ptr->select.path.path[0] == UIM_GSM_DF)
            {
              uim_ptr->state.dfs_1.gsm_df_present = TRUE;
            }
            else if(uim_ptr->command.cmd_ptr->select.path.path[0]== UIM_CDMA_DF)
            {
              uim_ptr->state.dfs_1.cdma_df_present = TRUE;
              nvruim_dir_present |= NVRUIM_CDMA_SUBSCRIP_PRESENT;
            }

            /*Extract the file characteristic byte from the response */
            uim_ptr->state.file_char_byte = rsp_ptr->rsp.data[UIM_FILE_CHAR_INDEX];

#ifdef FEATURE_UIM_BTSAP_CLIENT
            if(!IS_BTSAP_HANDLE_ACTIVE(uim_ptr))
#endif /* FEATURE_UIM_BTSAP_CLIENT */
            {
              if(((uim_ptr->state.file_char_byte & UIM_1_8_SIM_GSM)
                &&(uim_ptr->state.file_char_byte & UIM_3V_SIM_GSM)))
              {
                uim_ptr->flag.voltage_class_c_and_b_supported = TRUE;
              }

              /* Determine the current voltage on the interface */
              switch (uim_ptr->state.current_voltage_class)
              {
                case UIM_VOLTAGE_CLASS_C:
                  {
                    /* Check if the card does not support this voltage */
                    if ((uim_ptr->state.file_char_byte & UIM_1_8_SIM_GSM) !=
                        UIM_1_8_SIM_GSM)
                    {
                      /* Check if card supports other supported voltages */
                      if ((uim_ptr->state.file_char_byte & UIM_3V_SIM_GSM) ==
                          UIM_3V_SIM_GSM)
                      {
                        if (uim_ptr->hardware.uim_ldo.customize_ldo_setting &&
                            uim_ptr->hardware.uim_ldo.desired_voltage_class == UIM_VOLTAGE_CLASS_C)
                        {
                          UIMDRV_MSG_HIGH_0(uim_ptr->id,"LDO behaviour modified to work at specific voltage class");
                          return(UIM_VOLTAGE_MISMATCH);
                        }
                        /* Switch to different voltage */
                        uim_power_down(uim_ptr);
                        uim_timed_sleep(UIM_ISO7816_VOLTAGE_SWITCH_PWR_DOWN_DELAY,
                                        uim_ptr,
                                        UIM_ALL_ZERO_SIG);
                        /* Try the indicated voltage class */
                        uim_ptr->state.current_voltage_class = UIM_VOLTAGE_CLASS_B;
                        return(UIM_SWITCH_VOLTAGE);
                      }
                      else /* No voltages supported */
                      {
                        return(UIM_VOLTAGE_MISMATCH);
                      } /* end if - card supports other voltage */
                    } /* end if - card does not support this voltage */
                  } /* end case - UIM_VOLTAGE_1_8V */
                  break;

                case UIM_VOLTAGE_CLASS_B:
                  {
                    /* Check if the card does not support this voltage */
                    if ((uim_ptr->state.file_char_byte & UIM_3V_SIM_GSM) !=
                        UIM_3V_SIM_GSM)
                    {
                      return(UIM_VOLTAGE_MISMATCH);
                    } /* end if - card does not support this voltage */
                  } /* end case - UIM_VOLTAGE_3V*/
                  break;

                default:
                  {
                    UIMDRV_MSG_ERR_0(uim_ptr->id, "Bad UIM voltage class" );
                    return(UIM_VOLTAGE_MISMATCH);
                  } /* end case - default */
              } /* end switch - voltage class */
            }

            /* Skip the send capability state for ICC cards */
            uim_ptr->command.generic_state_ptr++;
          }/* protocol is GSM */
          else if (uim_ptr->command.cmd_ptr->hdr.protocol == UIM_UICC)
          {
            /* this is a response for select of MF */
            uim_ptr->state.current_path[uim_ptr->card_cmd.cur_channel].path = uim_ptr->command.cmd_ptr->select.path;

            /* call the function to parse the response from the UICC
               select for an MF select */
            uim_parse_uicc_select_response(
              uim_ptr,
              rsp_ptr->rsp.data,
              rsp_ptr->cmd_rsp_size,
              NULL,
              &uim_ptr->state.file_char_byte,
              &uim_send_terminal_capability);

            if(!(uim_send_terminal_capability & UIM_TERMINAL_CAP_BYTE_MASK))
            {
              /* Skip sending terminal capability */
              ++uim_ptr->command.generic_state_ptr;
            }
#ifdef FEATURE_UIM_USB_UICC
            if(!uim_usb_is_usb_interface_selected(uim_ptr))
#endif /* FEATURE_UIM_USB_UICC */
#ifdef FEATURE_UIM_BTSAP_CLIENT
            if(!IS_BTSAP_HANDLE_ACTIVE(uim_ptr))
#endif /* FEATURE_UIM_BTSAP_CLIENT */
            {
              if(((uim_ptr->state.file_char_byte & UIM_1_8_UIM_UICC)
                &&(uim_ptr->state.file_char_byte & UIM_3V_UIM_UICC)))
              {
                uim_ptr->flag.voltage_class_c_and_b_supported = TRUE;
              }
              /* Determine the current voltage on the interface */
              switch (uim_ptr->state.current_voltage_class)
              {
                case UIM_VOLTAGE_CLASS_C:
                  {
                    /* Check if the card does not support this voltage */
                    if ((uim_ptr->state.file_char_byte & UIM_1_8_UIM_UICC) !=
                        UIM_1_8_UIM_UICC)
                    {
                      /* Check if card supports other supported voltages */
                      if ((uim_ptr->state.file_char_byte & UIM_3V_UIM_UICC) ==
                          UIM_3V_UIM_UICC)
                      {
                        if (uim_ptr->hardware.uim_ldo.customize_ldo_setting &&
                            uim_ptr->hardware.uim_ldo.desired_voltage_class == UIM_VOLTAGE_CLASS_C)
                        {
                          UIMDRV_MSG_HIGH_0(uim_ptr->id,"LDO behaviour modified to work at specific voltage class");
                          return(UIM_VOLTAGE_MISMATCH);
                        }
                        /* Switch to different voltage */
                        uim_power_down(uim_ptr);
                        uim_timed_sleep(UIM_ISO7816_VOLTAGE_SWITCH_PWR_DOWN_DELAY,
                                        uim_ptr,
                                        UIM_ALL_ZERO_SIG);
                        /* Try the indicated voltage class */
                        uim_ptr->state.current_voltage_class = UIM_VOLTAGE_CLASS_B;
                        return(UIM_SWITCH_VOLTAGE);
                      }
                      else /* No voltages supported */
                      {
                        return(UIM_VOLTAGE_MISMATCH);
                      } /* end if - card supports other voltage */
                    } /* end if - card does not support this voltage */
                  } /* end case - UIM_VOLTAGE_1_8V */
                  break;

                case UIM_VOLTAGE_CLASS_B:
                  {
                    /* Check if the card does not support this voltage */
                    if ((uim_ptr->state.file_char_byte & UIM_3V_UIM_UICC) !=
                        UIM_3V_UIM_UICC)
                    {
                      return(UIM_VOLTAGE_MISMATCH);
                    } /* end if - card does not support this voltage */
                  } /* end case - UIM_VOLTAGE_3V*/
                  break;

                default:
                  {
                    UIMDRV_MSG_ERR_0(uim_ptr->id, "Bad UIM voltage class" );
                    return(UIM_VOLTAGE_MISMATCH);
                  } /* end case - default */
              } /* end switch - voltage class */
            }
          } /* protocol is UICC */
          /* Check if the UIM clock can be stopped */
          if ((uim_ptr->state.file_char_byte & UIM_CLOCK_STOP_ALL_BITS) == 0)
          {
            /* Do not allow the UIM clock to be stopped */
            uim_ptr->state.clock_stop_cntrl = UIM_CLOCK_STOP_NOT_ALLOWED;
          }
          else /* the card supports stopping the UIM clock */
          {
            /* Determine the recommended level to stop the clock */
            if (uim_ptr->state.file_char_byte & UIM_CLOCK_STOP_B3)
            {
              /* Clock must be stopped at high level */
              uim_ptr->state.clock_stop_cntrl = UIM_CLOCK_STOP_HIGH;
            }
            else
            {
              /* Clock must be stopped at low level */
              uim_ptr->state.clock_stop_cntrl = UIM_CLOCK_STOP_LOW;
            } /* end if - determine clock stop level */
          } /* end if - check UIM clock stop. */

          /* A DF was processed.  Continue with the power-up */
          ++uim_ptr->command.generic_state_ptr;

        } /* end if - there was a problem with the APDU */

      } /* end case - UIM_CHECK_CHARACTERISTICS_ST */
      break;

    case UIM_SEND_STATUS_ST:
      if(UIM_JCDMA_SERVICE_ENABLED == uim_ptr->setting.jcdma_mode.service)
      {
        if ((rsp_ptr->cmd_status != UIM_DONE) &&
            (uim_ptr->command.cmd_ptr->hdr.protocol == UIM_UICC))
        {
          /* Toggle the instruction class */
          if(!uim_ptr->flag.recovery_triggered)
          {
            UIMDRV_MSG_HIGH_0(uim_ptr->id,"Status failure in UICC mode,toggling inst class");
            uim_ptr->flag.toggle_instrn_class = !uim_ptr->flag.toggle_instrn_class;
          }
          return(UIM_INSTRN_CHANGE);
        }
      }
      ++uim_ptr->command.generic_state_ptr;
      break;

    case UIM_CHECK_FOR_CDMA_DF:
      {
        /* Check if there was a problem with selecting the CDMA DF */
        if (rsp_ptr->cmd_status == UIM_DONE)
        {
          /* update the path directly instead of calling uim_update_current_dir_icc
             use current_path[0] (default channel) for ICC */
          uim_ptr->state.current_path[0].path.path[0] = 0x3F00;
          uim_ptr->state.current_path[0].path.path[1] = uim_ptr->command.cmd_ptr->select.path.path[0];
          uim_ptr->state.current_path[0].is_ef        = FALSE;
          uim_ptr->state.current_path[0].path.len     = 2;

          /* uim_ptr->command.cmd_ptr->select.path.path[0] is set in the beginning of this state
             according to MMGSDI_FEATURE_CDMA_DF_SELECT_BEFORE_TP if it is true path[0] would be
             GSM_DF if it is false path[0] would be CDMA_DF */
          if (uim_ptr->flag.uim_auth_cdma_df_first == FALSE)
          {
            uim_ptr->state.dfs_1.cdma_df_present = TRUE;
            nvruim_dir_present |= NVRUIM_CDMA_SUBSCRIP_PRESENT;
          }
          else
          {
            uim_ptr->state.dfs_1.gsm_df_present = TRUE;
          }

          /* Due to some out of spec, commercialized RUIM cards we have to
             extend our work waiting timer value */
          uim_ptr->card_cmd.work_waiting_time =
            uim_ptr->card_cmd.extended_work_waiting_time;
          uim_ptr->card_cmd.work_waiting_time_etus =
            uim_ptr->card_cmd.extended_work_waiting_time_etus;
        } /* end if - the CDMA DF was selected */
         /*clearing of EF is required as some cards are clearing their EF pointer after the
           CDMA DF failure*/
        else if (rsp_ptr->cmd_status != UIM_DONE &&
                 uim_ptr->command.rpt_buf.sw1 == SW1_REFERENCE &&
                 uim_ptr->command.rpt_buf.sw2 == SW2_NOT_FOUND
                )
        {
          UIMDRV_MSG_HIGH_1(uim_ptr->id,"Select CDMA failed Command status: %x",
                            rsp_ptr->cmd_status);
        }
        /* Go to the next state.The current path points to previous selected path. */
        /* 3F00 2FE2 is the last selected file at this point during power-up or recovery */
        ++uim_ptr->command.generic_state_ptr;
      } /* end case - UIM_CHECK_FOR_CDMA_DF */
      break;

    case UIM_READ_ICCID_ST:
      {
        /* Check if there was a problem with the read */
        if (rsp_ptr->cmd_status != UIM_DONE)
        {
          if (uim_ptr->command.cmd_ptr->hdr.command == UIM_INTERNAL_WAKE_UP_F)
          {
            UIMDRV_MSG_HIGH_0(uim_ptr->id,"ICCID read failure during recovery");
            uim_clear_recovery_params(uim_ptr);
          }

          if(uim_nv_is_feature_enabled(UIMDRV_FEATURE_HANDLE_ICCID_READ_FAILURE,
                                       uim_ptr) == TRUE)
          {
            if(uim_ptr->command.cmd_ptr->hdr.protocol == UIM_UICC)
            {
              if(!uim_ptr->flag.recovery_triggered)
              {
                /* Toggle the instruction class */
                UIMDRV_MSG_HIGH_0(uim_ptr->id, "ICCID read failure, toggling the inst class ");
                uim_ptr->flag.toggle_instrn_class = !uim_ptr->flag.toggle_instrn_class;
              }
              return(UIM_INSTRN_CHANGE);
            }
            else if(uim_ptr->command.cmd_ptr->hdr.protocol == UIM_ICC)
            {
             /* If there was a read error for ICCID
              * and the class is already in GSM return
              * error
              */
              return(UIM_CMD_ERR);
            }
          }
          else
          {
            return(UIM_CMD_ERR);
          }
        } /* end if - there was a problem with the APDU */

        /* Process the ICCID data according to the command received. */
        if ( (uim_ptr->command.cmd_ptr->hdr.command == UIM_INTERNAL_ME_PUP_F) ||
             (uim_ptr->command.cmd_ptr->hdr.command == UIM_HOTSWAP_CARD_INS_F)||
             (uim_ptr->command.cmd_ptr->hdr.command == UIM_RESET_F)
           )
        {
          /* Store the ICCID */
          for (i = 0; i < uim_ptr->cache.iccid_buf.num_bytes; ++i)
          {
            uim_ptr->cache.iccid_buf.data[i] = rsp_ptr->rsp.data[i];
          } /* end for - store the ICCID */
          /* Get the next state. */
          ++uim_ptr->command.generic_state_ptr;

        }
        else if (uim_ptr->command.cmd_ptr->hdr.command == UIM_INTERNAL_WAKE_UP_F)
        {
          /* Compare the ICCID with the stored ICCID */
          for (i = 0; i < uim_ptr->cache.iccid_buf.num_bytes; ++i)
          {
            if (uim_ptr->cache.iccid_buf.data[i] != rsp_ptr->rsp.data[i])
            {
              /* Indicate an ICCID mismatch */
              UIMDRV_MSG_HIGH_0(uim_ptr->id,"ICCID MISMATCH");
              uim_clear_recovery_params(uim_ptr);
              return (UIM_ICCID_MISMATCH);
            }
          } /* end for - compare the ICCID */
          /* Move to recovery states */
          if(MMGSDI_SUCCESS ==
              uim_sm_mmgsdi_get_recovery_data_sync(&uim_ptr->state.recovery_data,
                                            (uim_slot_type)(uim_ptr->id + 1)))
          {
            uim_ptr->command.cmd_ptr->hdr.protocol = uim_ptr->state.recovery_data.protocol;
            if (uim_ptr->command.cmd_ptr->hdr.protocol == UIM_UICC)
            {
              UIMDRV_MSG_HIGH_0(uim_ptr->id,"Recovering UICC");
              /* Proceed to recover UICC card */
              uim_ptr->command.generic_state_ptr = UIM_RECOVER_UICC_SLOT_STATES;
            }
            else
            {
              UIMDRV_MSG_HIGH_0(uim_ptr->id,"Recovering ICC");
             /* Proceed to recover ICC card */
               uim_ptr->command.generic_state_ptr = UIM_RECOVER_ICC_SLOT_STATES;
            }
          }
          else
          {
            UIMDRV_MSG_ERR_0(uim_ptr->id,"mmgsdi_get_recovery_data_sync returned Error");
          }
        }
        else
        {
          /* Something is wrong.  We should not be in this state
             while processing this command. */
          UIMDRV_MSG_LOW_0(uim_ptr->id,"UIM_READ_ICCID_ST outside of PUP or WAKE_UP");
          return (UIM_CMD_ERR);
        } /* end if - command is a power-up command. */
      } /* end case - UIM_READ_ICCID_ST */
      break;

    case UIM_SELECT_ICCID_ST:
      if(uim_nv_is_feature_enabled(UIMDRV_FEATURE_SUPPORT_NO_ICCID, uim_ptr) &&
         (rsp_ptr->cmd_status != UIM_DONE))
      {
        if(uim_ptr->command.cmd_ptr->hdr.protocol == UIM_UICC)
        {
         uim_ptr->flag.iccid_select_failed = TRUE;
         rsp_ptr->cmd_status = UIM_DONE;
        }
        else if((uim_ptr->command.cmd_ptr->hdr.protocol == UIM_ICC) &&
                (uim_ptr->command.cmd_ptr->select.path_position == 1))
        {
          /* For an ICC we set the select path to 3F00/2FE2.  If we get a response
             and the path position is 1 then we know we failed selecting 2FE2(ICCID) */
          uim_ptr->flag.iccid_select_failed = TRUE;
          rsp_ptr->cmd_status               = UIM_DONE;
          }
      } /* support no iccid*/

      if(UIM_ICC == uim_ptr->command.cmd_ptr->hdr.protocol  &&
         0 == uim_ptr->command.cmd_ptr->select.path_position &&
         UIM_DONE == rsp_ptr->cmd_status)
      {
        /* For ICC we select incrementally.  If the path position is 0
           this is the response for MF increment path position and
           select again */
        uim_ptr->command.cmd_ptr->select.path_position++;
        break;
      }
      uim_ptr->cache.iccid_buf.num_bytes = (byte)uim_util_get_ef_file_size (uim_ptr,rsp_ptr);
      UIMDRV_MSG_HIGH_1(uim_ptr->id,"ICCID Length 0x%x",uim_ptr->cache.iccid_buf.num_bytes);

      uim_ptr->state.current_path[0].path.path[0] = UIM_MF_ID;
      uim_ptr->state.current_path[0].path.path[1] = UIM_ICCID_EF_ID;
      uim_ptr->state.current_path[0].is_ef        = TRUE;
      uim_ptr->state.current_path[0].path.len     = 2;
      ++uim_ptr->command.generic_state_ptr;
      break;

    case UIM_STREAM_SELECT_ST:
      {
        /* this state is only entered when streaming selects on the default channel */
        byte *apdu_data_ptr = uim_ptr->command.cmd_ptr->stream_iso7816_apdu.cmd_data;
        byte  apdu_p1       = apdu_data_ptr[UIM_7816_APDU_P1_OFFSET];
        uint8 cmd_path_len  = apdu_data_ptr[UIM_CASE3_4_7816_LC_OFFSET];
        uint8 i             = 0;

        uim_current_path_type *default_ch_curr_path_ptr = &uim_ptr->state.current_path[UIM_DEFAULT_CHANNEL];

        /* cmd_path_len is in bytes - each path entry is two bytes so divide by two */
        cmd_path_len /= 2;

        if (rsp_ptr->cmd_status != UIM_DONE)
        {
          uim_prepare_select_failed_st(uim_ptr, 0, rsp_ptr->cmd_status);

          uim_ptr->mode.generic_states[0]    = UIM_SELECT_FAILED_ST;
          uim_ptr->mode.generic_states[1]    = UIM_DONE_ST;
          uim_ptr->command.generic_state_ptr = uim_ptr->mode.generic_states;
          break;
        } /* end of error handling */
        if ( ((UIM_ICC  == uim_ptr->state.dfs_1.protocol) ||
              (UIM_UICC == uim_ptr->state.dfs_1.protocol)) &&
             ((SW1_NORMAL_END    != rsp_ptr->sw1) &&
              (SW1_END_FETCH     != rsp_ptr->sw1) &&
              (SW1_END_RESP      != rsp_ptr->sw1) &&
              (SW1_UTIL_END_RESP !=rsp_ptr->sw1))
           )
        {
          /* Do not update the current path as Streaming command for (U)ICC is not executed successfully
           * If current PATH is updated then subsequent POLL would fail
           */
          uim_stream_apdu_success(uim_ptr,
                                  uim_req_buf_static_ptr,
                                  rsp_ptr);
          UIMDRV_MSG_HIGH_3(uim_ptr->id,
               "STREAMING Protocol %d SW1 %d SW2 %d",
                uim_ptr->state.dfs_1.protocol,
                rsp_ptr->sw1,
                rsp_ptr->sw2);
          break;
        }
        if ( apdu_p1 & UIM_USIM_SELECT_BY_DF_NAME )
        {
          default_ch_curr_path_ptr->path.path[0] = 0x3F00;
          default_ch_curr_path_ptr->path.path[1] = 0x7FFF;
          default_ch_curr_path_ptr->path.len     = 2;
          default_ch_curr_path_ptr->is_ef        = FALSE;
        }
        else if(apdu_p1 & UIM_USIM_SELECT_BY_PATH_FROM_MF &&
                cmd_path_len < UIM_DIRMAIN_MAX_PATH_ELEMENTS)
        {
          default_ch_curr_path_ptr->path.path[0] = 0x3F00;
          default_ch_curr_path_ptr->path.len     = 1;

          /* set the data pointer equal to the path */
          apdu_data_ptr = &apdu_data_ptr[UIM_CASE3_4_7816_LC_OFFSET + 1];
          for(i = 1; i <= cmd_path_len; i++)
          {
            default_ch_curr_path_ptr->path.path[i] = (apdu_data_ptr[0] << 8) | apdu_data_ptr[1];
            apdu_data_ptr += 2;
            default_ch_curr_path_ptr->path.len++;
          }
          /* The type of selected file is unknown at this point.
             We need to get file type returned from card by GET RESPONSE*/
        }
        else if(apdu_p1 == UIM_USIM_SELECT_BY_FILE_ID &&
                default_ch_curr_path_ptr->path.len < UIM_DIRMAIN_MAX_PATH_ELEMENTS)
        {
          if(((apdu_data_ptr[UIM_CASE3_4_7816_LC_OFFSET + 1] << 8) |
                apdu_data_ptr[UIM_CASE3_4_7816_LC_OFFSET + 2]) == 0x3F00)
          {
            default_ch_curr_path_ptr->path.len     = 1;
            default_ch_curr_path_ptr->path.path[0] = 0x3F00;
            default_ch_curr_path_ptr->is_ef        = FALSE;
          }
          else
          {
            /* The command was successful. The apdu data has the file id selected as the
               first two command data bytes.  Take them from this apdu and store them */
            default_ch_curr_path_ptr->path.path[default_ch_curr_path_ptr->path.len] =
              (apdu_data_ptr[UIM_CASE3_4_7816_LC_OFFSET + 1] << 8) | apdu_data_ptr[UIM_CASE3_4_7816_LC_OFFSET + 2];

            default_ch_curr_path_ptr->path.len++;

            /* The type of selected file is unknown at this point.
             We need to get file type returned from card by GET RESPONSE*/
          }
        }

        /* this copies apdu response data and increments the generic state ptr
           it's possible that it will build new states for get rsp if needed */
        uim_stream_apdu_success(uim_ptr, uim_req_buf_static_ptr, rsp_ptr);
      }
      break;

    case UIM_SELECT_FAILED_ST:
      {
        if(rsp_ptr->cmd_status == UIM_DONE)
        {
          if(uim_ptr->command.cmd_ptr->hdr.protocol == UIM_ICC)
          {
            uim_ptr->command.cmd_ptr->select.path_position++;

            /* Select the next file ID by incrementing the sel state counter */
            if(uim_ptr->command.cmd_ptr->select.path_position < uim_ptr->command.cmd_ptr->select.path.len)
            {
              uim_update_current_dir_icc(uim_ptr,
                                       FALSE,
                                      &uim_ptr->command.cmd_ptr->select.path,
                                       uim_ptr->command.cmd_ptr->select.path_position);
              break;
            }
            else if(uim_ptr->command.cmd_ptr->select.path_position == uim_ptr->command.cmd_ptr->select.path.len)
            {
              boolean temp_ef = rsp_ptr->rsp.ef.file_type == EF ? TRUE : FALSE;

#ifdef FEATURE_UIM_TEST_FRAMEWORK
            #error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

              uim_update_current_dir_icc(uim_ptr,
                                       temp_ef,
                                      &uim_ptr->command.cmd_ptr->select.path,
                                       uim_ptr->command.cmd_ptr->select.path_position);
            }
          }
          else if (uim_ptr->command.cmd_ptr->hdr.protocol == UIM_UICC)
          {
            boolean temp_ef = FALSE;
            uim_parse_uicc_select_response(
              uim_ptr,
             &rsp_ptr->rsp.data[0],
              rsp_ptr->cmd_rsp_size,
             &temp_ef,
              NULL,
              NULL);

            uim_update_current_dir_uicc(uim_ptr, temp_ef, &uim_ptr->command.cmd_ptr->select, curr_ch);
          }
        }
        else
        {
          /* We failed the last selected dir select attempt. Clear the current
             path and report - Something is very wrong with the card*/
          uim_clear_current_path(uim_ptr, curr_ch);
        }

        uim_ptr->command.rpt_buf.sw1 = uim_ptr->command.cmd_ptr->select.reselect.sw1;
        uim_ptr->command.rpt_buf.sw2 = uim_ptr->command.cmd_ptr->select.reselect.sw2;
        rsp_ptr->cmd_status          = uim_ptr->command.cmd_ptr->select.reselect.cmd_status;
        uim_ptr->command.cmd_ptr->select.reselect.is_failed_select_reselect = FALSE;
        uim_ptr->command.cmd_ptr->select.is_intermediate_sel = FALSE;

        uim_ptr->command.cmd_ptr->hdr.cmd_count = UIM_MAX_NUM_ATTEMPTS;
        return UIM_CMD_ERR;
      }
      break;

    case UIM_SELECT_ST:                       /* Select State */
      {
        /* Did the command execute properly. */
        if (rsp_ptr->cmd_status != UIM_DONE)
        {
          if (uim_ptr->command.cmd_ptr->hdr.command == UIM_INTERNAL_WAKE_UP_F)
          {
            UIMDRV_MSG_HIGH_0(uim_ptr->id,"Select failure during Recovery");
            /* If internal wake_up_f, try to recover */
            (void) rex_set_sigs( uim_ptr->tcb_ptr, UIM_CMD_RSP_TIMEOUT_SIG );
            /* Go to the next state so that we are not stuck in infinite
               SELECT */
            ++uim_ptr->command.generic_state_ptr;
            break;
          }
          else if( (UIM_SELECT_F             == uim_ptr->command.cmd_ptr->hdr.command)          &&
                  ((UIM_NEXT_OCCURRENCE      == uim_ptr->command.cmd_ptr->select.adf_selection) ||
                   (UIM_LAST_OCCURRENCE      == uim_ptr->command.cmd_ptr->select.adf_selection) ||
                   (UIM_PREVIOUS_OCCURRENCE  == uim_ptr->command.cmd_ptr->select.adf_selection)) )
         {
           /* Do not clear the last selected ADF because
              Upon un-successful selection of given partial AID for given channel the
              already selected Applet stays selected.
               Therefore no need to issue MF Selection upon Failure.
             */
             uim_ptr->command.cmd_ptr->hdr.cmd_count = UIM_MAX_NUM_CMD_ATTEMPTS(uim_ptr);
             uim_ptr->command.rpt_buf.sw1 = uim_ptr->command.rsp_buf.sw1;
             uim_ptr->command.rpt_buf.sw2 = uim_ptr->command.rsp_buf.sw2;
             UIMDRV_MSG_HIGH_3(uim_ptr->id,"Select for Next Occurrence Failed and not going to re-attempt"
                          " sw1 0x%x sw2 0x%x cmd_count 0x%x",
                          uim_ptr->command.rpt_buf.sw1,uim_ptr->command.rpt_buf.sw2,
                          uim_ptr->command.cmd_ptr->hdr.cmd_count);
             return (UIM_CMD_ERR);
         }

          uim_prepare_select_failed_st(uim_ptr, curr_ch, rsp_ptr->cmd_status);

          uim_ptr->mode.generic_states[0]    = UIM_SELECT_FAILED_ST;
          uim_ptr->mode.generic_states[1]    = UIM_DONE_ST;
          uim_ptr->command.generic_state_ptr = uim_ptr->mode.generic_states;
          break;
        } /* end of error handling */


        if (uim_ptr->command.cmd_ptr->hdr.protocol == UIM_UICC)
        {
          boolean temp_ef = FALSE;

          uim_parse_uicc_select_response(
            uim_ptr,
           &rsp_ptr->rsp.data[0],
            rsp_ptr->cmd_rsp_size,
           &temp_ef,
            NULL,
            NULL);

          uim_update_current_dir_uicc(uim_ptr, temp_ef, &uim_ptr->command.cmd_ptr->select, curr_ch);
          ++uim_ptr->command.generic_state_ptr;
        }
        else if(uim_ptr->command.cmd_ptr->hdr.protocol == UIM_ICC)
        {
          uim_ptr->command.cmd_ptr->select.path_position++;

          /* Select the next file ID by incrementing the sel state counter */
          if(uim_ptr->command.cmd_ptr->select.path_position < uim_ptr->command.cmd_ptr->select.path.len)
          {
            uim_update_current_dir_icc(uim_ptr,
                                       FALSE,
                                      &uim_ptr->command.cmd_ptr->select.path,
                                       uim_ptr->command.cmd_ptr->select.path_position);
          }
          else
          {
            boolean temp_ef = rsp_ptr->rsp.ef.file_type == EF ? TRUE : FALSE;

#ifdef FEATURE_UIM_TEST_FRAMEWORK
            #error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

            UIMDRV_MSG_ERR_2(uim_ptr->id, "updating current directory icc - temp_ef:0x%x - file type:0x%x",temp_ef,rsp_ptr->rsp.ef.file_type);

            uim_update_current_dir_icc(uim_ptr,
                                       temp_ef,
                                      &uim_ptr->command.cmd_ptr->select.path,
                                       uim_ptr->command.cmd_ptr->select.path_position);

              /* The file selection is finished.  Get to the next state. */
            ++uim_ptr->command.generic_state_ptr;
          }
        }
      } /* end case - UIM_SELECT_ST */
      break;

    case UIM_STATUS_ST:                       /* Status State */
      {
        uint16 current_dir = 0x0000;
        /* Response to status is the same as Select MF/DF */
        /* Determine which command got us here. */
        if (uim_ptr->command.cmd_ptr->hdr.command == UIM_INTERNAL_POLL_F)
        {
          if (rsp_ptr->cmd_status != UIM_DONE)
          {
            if(uim_nv_is_feature_enabled(UIMDRV_FEATURE_RPT_BAD_SW_ON_POLL,
                                         uim_ptr) == TRUE)
            {
              if ( uim_ptr->command.mode != UIM_PASSIVE_MODE )
              {
                UIMDRV_MSG_ERR_3(uim_ptr->id, "Un-successful polling - SW1, SW2, cmd_status",
                                 rsp_ptr->sw1,
                                 rsp_ptr->sw2,
                                 rsp_ptr->cmd_status);
                return(UIM_POLL_ERROR);
              }
            }
            else
            {
              UIMDRV_MSG_ERR_1(uim_ptr->id, "Polling failed while uim_mode is 0x%x, ignoring",
                               uim_ptr->command.mode);
              /* Internal poll command failed for some odd reason
               * let us continue instead of comparing data with the
               * stale data in the response pointer
               */
              ++uim_ptr->command.generic_state_ptr;
              break;
            }
          }
          if ( uim_ptr->command.mode == UIM_PASSIVE_MODE ||
               uim_ptr->command.cmd_ptr->status.return_data == UIM_RETURN_NONE  || rsp_ptr->cmd_rsp_size == 0)
          {
            /* We are either in in passive mode or not in traffic.  It is
             * enough if we get a response, we need not compare the
             * directory maintainence just process the next state
             */
            ++uim_ptr->command.generic_state_ptr;
            break;
          }
          status_adf.aid_length = 0;

          /* get the currently select file id */
          if (uim_ptr->command.cmd_ptr->hdr.protocol == UIM_UICC)
          {
            uim_parse_uicc_status_response ( rsp_ptr->rsp.data,
                                           rsp_ptr->cmd_rsp_size,
                                           &status_file_id,
                                           &status_adf,
                                           uim_ptr);
          }
            else
              {
            status_file_id = FLIPW(rsp_ptr->rsp.mf_df.file_id);
              }

          /* Get the last selected directory to compare
             against the currently selected FID from above*/
          if(uim_ptr->state.current_path[curr_ch].is_ef)
                 {
            current_dir = uim_ptr->state.current_path[curr_ch].path.path[uim_ptr->state.current_path[curr_ch].path.len - 2];
              }
              else
              {
            current_dir = uim_ptr->state.current_path[curr_ch].path.path[uim_ptr->state.current_path[curr_ch].path.len - 1];
              }

          /* Compare the two values - the current directory
             and the directory identified by the status*/
          if(current_dir == 0x7FFF)
          {
              if (memcmp(status_adf.data,
                      uim_ptr->state.current_path[curr_ch].curr_app.data,
                      uim_ptr->state.current_path[curr_ch].curr_app.aid_length) != 0)
              {
                UIMDRV_MSG_HIGH_0(uim_ptr->id, "Mismatch in ADF - UICC protocol");
                return(UIM_POLL_ERROR);
              }
          }
          else if(current_dir != status_file_id)
              {
            /* if the current DF doesn't match the one returned by the
               status it is only ok if it's UICC, the current dir is MF, and
               the status returned the AID currently selected on this channel*/
            if(uim_ptr->command.cmd_ptr->hdr.protocol == UIM_UICC &&
               current_dir == UIM_MF_ID  &&
               status_adf.aid_length > 0 &&
                    status_adf.aid_length <= UIM_AID_MAX_NAME_LENGTH )
                {
                  if (memcmp(status_adf.data,
                 uim_ptr->state.current_path[curr_ch].curr_app.data,
                      status_adf.aid_length ) != 0)
                  {
                    UIMDRV_MSG_HIGH_0 ( uim_ptr->id, "Mismatch in AID - UICC protocol");
                    /* Log shall be printed */
                    for (idx = 0; idx < status_adf.aid_length; idx++)
                    {
                      UIMDRV_MSG_HIGH_2 ( uim_ptr->id, " status_adf.data[%d] 0x%x",
                                          idx, status_adf.data[idx]);
                    }
                    return(UIM_POLL_ERROR);
                   }
              /* if the aid matches we break out here and increment to the next state*/
                }
            else
                {
                  UIMDRV_MSG_ERR_1(uim_ptr->id, "Recd file ID in STATUS: 0x%x Current Path:",
                               status_file_id);
                  uim_print_curr_path(&uim_ptr->state.current_path[curr_ch].path,uim_ptr);
                  /* The selected file ID has changed */
                  return(UIM_POLL_ERROR);
                }
              }
        } /* end if - the command was a poll command */
        else
        {
          if (rsp_ptr->cmd_status != UIM_DONE)
          {
            return(UIM_CMD_ERR);
          }
        }
        /* Process the next state */
        ++uim_ptr->command.generic_state_ptr;
      } /* end case - UIM_STATUS_ST */
      break;

    case UIM_RECOVERY_SEND_STATUS_COMMAND_ST:
      {
        /* Restart recovery if command failed. */
        if (rsp_ptr->cmd_status != UIM_DONE)
        {
          UIMDRV_MSG_MED_0 ( uim_ptr->id,"app init status command during recovery");
          return (UIM_CMD_ERR);
        }
        else
        {
          /*Move to next state*/
          ++uim_ptr->command.generic_state_ptr;
        }
      }
      break;

    case UIM_READ_BIN_MEM_ST:                  /* Read Binary State */
      {
        /* Return an error status if the command failed. */
        if (rsp_ptr->cmd_status != UIM_DONE)
        {
                    /* Check if SFI was used and the command failed.
             If yes then retry the command without using SFI. */
          if (uim_ptr->command.cmd_ptr->read_bin.sfi != UIM_INVALID_SFI)
          {
            /* 1. Blank out the SFI so its not used while re-attempting the command.
               2. Blank out the directory structure as we are uncertain of the current directory
                  pointer in the card and while re-attempting the command we would like to
                  start from MF */
            uim_ptr->command.cmd_ptr->read_bin.sfi = UIM_INVALID_SFI;
            uim_clear_current_path(uim_ptr, curr_ch);
            return UIM_CMD_ERR_WITH_SFI;
          }
          return (UIM_CMD_ERR);
        }
        else
        {
          /* Temporary variable to hold the number of bytes actually read */
          word tmp_num_bytes_read;

          if ( uim_req_buf_static_ptr->apdu_hdr.p3 == 0 )
          {
            tmp_num_bytes_read = (word) UIM_MAX_TRANSACTION_SIZE;
          }
          else
          {
            tmp_num_bytes_read = (word) uim_req_buf_static_ptr->apdu_hdr.p3;
          }

          /* decrement the number of bytes left to read by the number read*/
          uim_ptr->command.rd_wr_info.num_bytes -= tmp_num_bytes_read;


          /* store the total number of bytes read up-to now */
          uim_ptr->command.rd_wr_info.num_bytes_rsp += tmp_num_bytes_read;

          /* Increment the offset so that next read would get
           * the right data
           */
          uim_ptr->command.rd_wr_info.offset += tmp_num_bytes_read;

          /* Copy over the data to the buffer provided each time we have
           * a peice of data returned
           */
          uim_memscpy(uim_ptr->command.rd_wr_info.data_ptr ,tmp_num_bytes_read, rsp_ptr->rsp.data,
                 tmp_num_bytes_read);

          /* Increment the pointer to point to the next
           * location to copy the data
           */
          uim_ptr->command.rd_wr_info.data_ptr = uim_ptr->command.rd_wr_info.data_ptr +
            tmp_num_bytes_read;

          /* if no more bytes to read go to the next step*/
          if (uim_ptr->command.rd_wr_info.num_bytes == 0)
          {
            if ((uim_ptr->command.cmd_ptr->hdr.command == UIM_READ_BIN_CMD_F) &&
                (uim_ptr->command.cmd_ptr->read_bin.item == UIM_USIM_IMSI))
            {
              /* Check MCC and MNC */
              /* -----------------------------------------------------------
                Parse MCC from the IMSI:
                B0     B1       B2       B3       B4     B5   B6   B7    B8
                LEN  MCC1PB   MCC3MCC2 MNC2MNC1 D1MNC3  D3D2 D5D4 D7D6  D9D8
              -------------------------------------------------------------*/
              mcc1 = rsp_ptr->rsp.data[1] & UIM_USIM_IMSI_HIGH_BYTE_MASK;
              mcc1 = mcc1 >> 4;
              mcc2 = rsp_ptr->rsp.data[2] & UIM_USIM_IMSI_LOW_BYTE_MASK;
              mcc3 = rsp_ptr->rsp.data[2] & UIM_USIM_IMSI_HIGH_BYTE_MASK;
              mcc3 = mcc3 >> 4;

              mnc1 = rsp_ptr->rsp.data[3] & UIM_USIM_IMSI_LOW_BYTE_MASK;
              mnc2 = rsp_ptr->rsp.data[3] & UIM_USIM_IMSI_HIGH_BYTE_MASK;
              mnc2 = mnc2 >> 4;

              if ((mcc1 == UIM_USIM_CARD_IMSI_MCC1_VAL) &&
                  (mcc2 == UIM_USIM_CARD_IMSI_MCC2_VAL) &&
                  (mcc3 == UIM_USIM_CARD_IMSI_MCC3_VAL) &&
                  (mnc1 == UIM_USIM_CARD_IMSI_MNC1_VAL) &&
                  (mnc2 == UIM_USIM_CARD_IMSI_MNC2_VAL))
              {
                uim_ptr->flag.check_card_problem = TRUE;
              }
              else
              {
                uim_ptr->flag.check_card_problem = FALSE;
              }
              UIMDRV_MSG_HIGH_1(uim_ptr->id,"MCC/MNC match. It is a problematic card 0x%x", 
                                uim_ptr->flag.check_card_problem);
            }
            /*
             * This workaround is under the assumption that
             * IMSI is read before IMSI_M
             */
            if ((uim_ptr->command.cmd_ptr->hdr.command == UIM_READ_BIN_CMD_F) &&
                (uim_ptr->command.cmd_ptr->read_bin.item == UIM_CSIM_IMSI_M) &&
                (uim_ptr->flag.check_card_problem))
            {
                /* Check if IMSI programmed */
                if((rsp_ptr->rsp.data[UIM_CSIM_IMSI_M_ADDR_OFFSET] & 0x80) == 0)
                {
                  /* Disable return FCP for STATUS */
                  uim_ptr->flag.disable_return_df_fcp_inpoll = TRUE;
                }
                else
                {
                  uim_ptr->flag.disable_return_df_fcp_inpoll = FALSE;
                }
                UIMDRV_MSG_HIGH_1(uim_ptr->id,"IMSI_M_PROGRAMMED bit is 0x%x", 
                                  uim_ptr->flag.disable_return_df_fcp_inpoll );
            }
            /* Skip to the next state. */

            /* To update the last_sel_dir and the last_sel_ef if SFI was used */
            if(uim_ptr->command.cmd_ptr->read_bin.sfi != UIM_INVALID_SFI)
            {
              UIMDRV_MSG_MED_0(uim_ptr->id,"UIM_READ_BIN_MEM_ST - SFI used successfully");
              uim_update_current_dir_sfi(uim_ptr, &uim_ptr->command.cmd_ptr->read_bin.path, curr_ch);
            }

            ++uim_ptr->command.generic_state_ptr;
          }
        }
      } /* end case - UIM_READ_BIN_MEM_ST */
      break;

    case UIM_UPDATE_BIN_MEM_ST:                /* Update Binary State */
      {
        /* Return an error status if the command failed. */
        if (rsp_ptr->cmd_status != UIM_DONE)
        {
          /* Check if SFI was used and the command failed.
             If yes then retry the command without using SFI. */
          if (uim_ptr->command.cmd_ptr->write_bin.sfi != UIM_INVALID_SFI)
          {
            /* 1. Blank out the SFI so its not used while re-attempting the command.
               2. Blank out the directory structure as we are uncertain of the current directory
                  pointer in the card and while re-attempting the command we would like to
                  start from MF */
            uim_ptr->command.cmd_ptr->write_bin.sfi = UIM_INVALID_SFI;
            uim_clear_current_path(uim_ptr, curr_ch);
            return UIM_CMD_ERR_WITH_SFI;
          }

          return (UIM_CMD_ERR);
        }
        else
        {
          /* Decrement the number of bytes sent in the transaction */
          uim_ptr->command.rd_wr_info.num_bytes -= uim_req_buf_static_ptr->apdu_hdr.p3;

          /* store the total number of bytes read up-to now */
          uim_ptr->command.rd_wr_info.num_bytes_rsp  += uim_req_buf_static_ptr->apdu_hdr.p3;

          /* Check if we are done sending all the bytes
           * If the number of bytes requested is greater than the file size, then
           *  P3 is filled with the file size as the number of bytes to write
           */
          if (uim_ptr->command.rd_wr_info.num_bytes)
          {
            /* Change the offset for the next write */
            uim_ptr->command.rd_wr_info.offset += uim_req_buf_static_ptr->apdu_hdr.p3;

            /* Change the data pointer for the next write */
            uim_ptr->command.rd_wr_info.data_ptr += uim_req_buf_static_ptr->apdu_hdr.p3;
          }
          else
          {
            /* Skip to the next state. */


            /* To update the last_sel_dir and the last_sel_ef if SFI was used */
            if(uim_ptr->command.cmd_ptr->write_bin.sfi != UIM_INVALID_SFI)
            {
              UIMDRV_MSG_MED_0(uim_ptr->id,"UIM_UPDATE_BIN_MEM_ST - SFI used successfully");
              uim_update_current_dir_sfi(uim_ptr, &uim_ptr->command.cmd_ptr->write_bin.path, curr_ch);
            }

            ++uim_ptr->command.generic_state_ptr;
          }
        }
      } /* end case - UIM_UPDATE_BIN_MEM_ST */
      break;

    case UIM_READ_REC_MEM_ST:                  /* Read Record State */
      {
        /* Return an error status if the command failed. */
        if (rsp_ptr->cmd_status != UIM_DONE)
        {
                    /* Check if SFI was used and the command failed.
             If yes then retry the command without using SFI. */
          if (uim_ptr->command.cmd_ptr->read_rec.sfi != UIM_INVALID_SFI)
          {
            /* 1. Blank out the SFI so its not used while re-attempting the command.
               2. Blank out the directory structure as we are uncertain of the current directory
                  pointer in the card and while re-attempting the command we would like to
                  start from MF */
            uim_ptr->command.cmd_ptr->read_rec.sfi = UIM_INVALID_SFI;
            uim_clear_current_path(uim_ptr, curr_ch);
            return UIM_CMD_ERR_WITH_SFI;
          }

          return (UIM_CMD_ERR);
        }
        else
        {
          /* Need to copy the data here */
          word tmp_num_bytes_read;

          if ( uim_req_buf_static_ptr->apdu_hdr.p3 == 0 )
          {
            tmp_num_bytes_read = (word) UIM_MAX_TRANSACTION_SIZE;
          }
          else
          {
            tmp_num_bytes_read = (word) uim_req_buf_static_ptr->apdu_hdr.p3;
          }

          /* Copy over the data to the buffer provided each time we have
           * a peice of data returned
           */
          uim_memscpy(uim_ptr->command.rd_wr_info.data_ptr ,
                 tmp_num_bytes_read,
                 rsp_ptr->rsp.data,
                 tmp_num_bytes_read);

          /* Set the number of response bytes received */
          uim_ptr->command.rd_wr_info.num_bytes_rsp = tmp_num_bytes_read;

          /* To update the last_sel_dir and the last_sel_ef if SFI was used */
          if(uim_ptr->command.cmd_ptr->read_rec.sfi != UIM_INVALID_SFI)
          {
            UIMDRV_MSG_MED_0(uim_ptr->id,"UIM_READ_REC_MEM_ST - SFI used successfully");
            uim_update_current_dir_sfi(uim_ptr, &uim_ptr->command.cmd_ptr->read_rec.path, curr_ch);
          }

          /* Skip to the next state. */
          ++uim_ptr->command.generic_state_ptr;
        }
      } /* end case - UIM_READ_REC_MEM_ST */
      break;

    case UIM_UPDATE_REC_MEM_ST:                /* Update Record State */
      {
        /* Return an error status if the command failed. */
        if (rsp_ptr->cmd_status != UIM_DONE)
        {
          if (rsp_ptr->cmd_status == UIM_MEMORY_PROBLEM)
          {
             /* Set the command count to MAX so as to not repeat this cmd as a
                GCF test case fails when repeated */
             uim_ptr->command.cmd_ptr->hdr.cmd_count = UIM_MAX_NUM_ATTEMPTS;
          }

          /* Check if SFI was used and the command failed.
             If yes then retry the command without using SFI. */
          if (uim_ptr->command.cmd_ptr->write_rec.sfi != UIM_INVALID_SFI)
          {
            /* 1. Blank out the SFI so its not used while re-attempting the command.
               2. Blank out the directory structure as we are uncertain of the current directory
                  pointer in the card and while re-attempting the command we would like to
                  start from MF */
            uim_ptr->command.cmd_ptr->write_rec.sfi = UIM_INVALID_SFI;
            uim_clear_current_path(uim_ptr, curr_ch);
            return UIM_CMD_ERR_WITH_SFI;
          }

          return (UIM_CMD_ERR);
        }
        else
        {
          uim_ptr->command.rd_wr_info.num_bytes_rsp  += uim_req_buf_static_ptr->apdu_hdr.p3;
          /* Skip to the next state. */
          /* To update the last_sel_dir and the last_sel_ef if SFI was used */
          if(uim_ptr->command.cmd_ptr->write_rec.sfi != UIM_INVALID_SFI)
          {
            UIMDRV_MSG_MED_0(uim_ptr->id,"UIM_UPDATE_REC_MEM_ST - SFI used successfully");
            uim_update_current_dir_sfi(uim_ptr, &uim_ptr->command.cmd_ptr->write_rec.path, curr_ch);
          }

          ++uim_ptr->command.generic_state_ptr;
        }
      } /* end case - UIM_UPDATE_REC_MEM_ST */
      break;

    case UIM_INC_MEM_ST:                    /* Increment State */
      {
        /* Return an error status if the command failed. */
        if (rsp_ptr->cmd_status != UIM_DONE)
        {
          /* Check if SFI was used and the command failed.
             If yes then retry the command without using SFI. */
          if (uim_ptr->command.cmd_ptr->inc.sfi != UIM_INVALID_SFI)
          {
            /* 1. Blank out the SFI so its not used while re-attempting the command.
               2. Blank out the directory structure as we are uncertain of the current directory
                  pointer in the card and while re-attempting the command we would like to
                  start from MF */
            uim_ptr->command.cmd_ptr->inc.sfi = UIM_INVALID_SFI;
            uim_clear_current_path(uim_ptr, curr_ch);
            return UIM_CMD_ERR_WITH_SFI;
          }

          return (UIM_CMD_ERR);
        }
        else
        {
          /* Skip to the next state. */
          /* To update the last_sel_dir and the last_sel_ef if SFI was used */
          if(uim_ptr->command.cmd_ptr->inc.sfi != UIM_INVALID_SFI)
          {
            UIMDRV_MSG_MED_0(uim_ptr->id,"UIM_INCREMENT_ST - SFI used successfully");
            uim_update_current_dir_sfi(uim_ptr, &uim_ptr->command.cmd_ptr->inc.path, curr_ch);
          }

          ++uim_ptr->command.generic_state_ptr;
        }
      } /* end case - UIM_INCREMENT_ST */
      break;

    case UIM_VERIFY_CHV1_ST:                  /* Verify CHV 1 State */
      {
        /* If the response is good, then set the use flag to TRUE */
        if (rsp_ptr->cmd_status == UIM_DONE)
        {
          uim_ptr->cache.chv1.use_chv = TRUE;
        }
        else
        {
          /* Do not verify CHV1 until the verify succeeds. */
          uim_ptr->cache.chv1.use_chv = FALSE;
        } /* end if - the UIM command completed successfully. */

        /* Skip to the next state. */
        ++uim_ptr->command.generic_state_ptr;
      } /* end case - UIM_VERIFY_CHV1_ST */
      break;

    case UIM_VERIFY_CHV2_ST:                  /* Verify CHV 2 State */
      {
        /* If the response is good, then set the use flag to TRUE */
        if (rsp_ptr->cmd_status == UIM_DONE)
        {
          uim_ptr->cache.chv2.use_chv = TRUE;
        }
        else
        {
          /* Do not verify CHV1 until the verify succeeds. */
          uim_ptr->cache.chv2.use_chv = FALSE;

        } /* end if - the UIM command completed successfully. */
        /* Skip to the next state. */
        ++uim_ptr->command.generic_state_ptr;
      } /* end case - UIM_VERIFY_CHV2_ST */
      break;

    case UIM_UNBLOCK_CHV1_ST:                 /* Unblock CHV 1 State */
      {
        /* If the response is good, then set the use flag to TRUE */
        if (rsp_ptr->cmd_status == UIM_DONE)
        {
          /* Keep the new CHV value in the cache buffer.  The cache buffer
             is used to verify CHV1 on wake up. */
          uim_memscpy( uim_ptr->cache.chv1.chv,
                  sizeof(uim_ptr->cache.chv1.chv),
                  uim_ptr->command.cmd_ptr->unblk_chv.new_chv_digits,
                  UIM_MAX_CHV_DIGITS);

          /* Format the CHV digits in the cache buffer. */
          uim_format_chv(uim_ptr->cache.chv1.chv,uim_ptr);

          uim_ptr->cache.chv1.use_chv = TRUE;
        }
        else
        {
          /* Do not verify CHV1 again until a CHV command succeeds. */
          uim_ptr->cache.chv1.use_chv = FALSE;
        } /* end if - the UIM command completed successfully. */

        /* Skip to the next state. */
        ++uim_ptr->command.generic_state_ptr;
      } /* end case - UIM_UNBLOCK_CHV1_ST */
      break;

    case UIM_UNBLOCK_CHV2_ST:                 /* Unblock CHV 2 State */
      {
        /* Skip to the next state. */
        ++uim_ptr->command.generic_state_ptr;
      } /* end case - UIM_UNBLOCK_CHV2_ST */
      break;

    case UIM_CHANGE_CHV1_ST:                  /* Change CHV 1 State */
      {
        /* If the response is good, then set the use flag to TRUE */
        if (rsp_ptr->cmd_status == UIM_DONE)
        {
          /* Keep the new CHV value in the cache buffer.  The cache buffer
             is used to verify CHV1 on wake up. */
          uim_memscpy( uim_ptr->cache.chv1.chv,
                  sizeof(uim_ptr->cache.chv1.chv),
                  uim_ptr->command.cmd_ptr->change_chv.new_chv_digits,
                  UIM_MAX_CHV_DIGITS);

          /* Format the CHV digits in the cache buffer. */
          uim_format_chv(uim_ptr->cache.chv1.chv, uim_ptr);

          uim_ptr->cache.chv1.use_chv = TRUE;
        }
        else
        {
          /* Do not verify CHV1 again until the verify succeeds. */
          uim_ptr->cache.chv1.use_chv = FALSE;
        } /* end if - the UIM command completed successfully. */

        /* Skip to the next state. */
        ++uim_ptr->command.generic_state_ptr;
      } /* end case - UIM_CHANGE_CHV1_ST */
      break;

    case UIM_CHANGE_CHV2_ST:                  /* Change CHV 2 State */
      {
        /* Since we are not caching the CHV2, Skip to the next state. */
        ++uim_ptr->command.generic_state_ptr;
      } /* end case - UIM_CHANGE_CHV2_ST */
      break;

    case UIM_ENABLE_CHV1_ST:                  /* Enable CHV 1 State */
      {
        /* If the response is good, then set the use flag to TRUE */
        if (rsp_ptr->cmd_status == UIM_DONE)
        {
          /* Keep the new CHV value in the cache buffer.  The cache buffer
             is used to verify CHV1 on wake up. */
          uim_memscpy( uim_ptr->cache.chv1.chv,
                  sizeof(uim_ptr->cache.chv1.chv),
                  uim_ptr->command.cmd_ptr->enable_chv.chv_digits,
                  UIM_MAX_CHV_DIGITS);

          /* Format the CHV digits in the cache buffer. */
          uim_format_chv(uim_ptr->cache.chv1.chv, uim_ptr);

          uim_ptr->cache.chv1.use_chv = TRUE;
        }
        else
        {
          /* Do not verify CHV1 again until a CHV command succeeds. */
          uim_ptr->cache.chv1.use_chv = FALSE;
        } /* end if - the UIM command completed successfully. */

        /* Skip to the next state. */
        ++uim_ptr->command.generic_state_ptr;
      } /* end case - UIM_ENABLE_CHV1_ST */
      break;

    case UIM_ENABLE_CHV2_ST:                  /* Enable CHV 2 State */
      {
        /* Since we are not caching the CHV2, Skip to the next state. */
        ++uim_ptr->command.generic_state_ptr;
      } /* end case - UIM_ENABLE_CHV2_ST */
      break;

    case UIM_DISABLE_CHV1_ST:                 /* Disable CHV 1 State */
      {
        /* If this command was successful, then the CHV was disabled
           successfully, so we do not have to verify CHV upon power up.

           If the command failed with SW1 as 98, then the CHV presented
           is incorrect or already disabled or blocked, then the use
           flag is set to FALSE */

        /* If the response is good, then copy the CHV1 digits to the cache
           buffer. */
        if (rsp_ptr->cmd_status == UIM_DONE)
        {
          /* Keep the new CHV value in the cache buffer.  The cache buffer
             is used to verify CHV1 on wake up. */
          uim_memscpy( uim_ptr->cache.chv1.chv,
                  sizeof(uim_ptr->cache.chv1.chv),
                  uim_ptr->command.cmd_ptr->unblk_chv.new_chv_digits,
                  UIM_MAX_CHV_DIGITS);

          /* Format the CHV digits in the cache buffer. */
          uim_format_chv(uim_ptr->cache.chv1.chv, uim_ptr);

        }
        /* Whether the command succeeds, there is no reason the verify CHV1.
           If the command fails, the CHV1 digits do not match the card.  In
           either case, CHV1 is not to be verified. */
        uim_ptr->cache.chv1.use_chv = FALSE;

        /* if the sw indicates that there is some other error, then we
           would like to continue verifying CHV on power up with the
           old CHV and not from the CHV in the disable command */
        if (rsp_ptr->sw1 != UIM_SW1_SECURITY)
        {
           uim_ptr->cache.chv1.use_chv = TRUE;
        } /* end if - sw1 indicates a problem other than security management.*/

        /* Skip to the next state. */
        ++uim_ptr->command.generic_state_ptr;
      } /* end case - UIM_DISABLE_CHV1_ST */
      break;

    case UIM_DISABLE_CHV2_ST:                 /* Disable CHV 2 State */
      {
        /* Since we are not caching the CHV2, Skip to the next state. */
        ++uim_ptr->command.generic_state_ptr;
      } /* end case - UIM_DISABLE_CHV2_ST */
      break;

    case UIM_INVALIDATE_ST:                   /* Invalidate State */
      {
        /* Check if there was a problem with Invalidation */
        if (rsp_ptr->cmd_status != UIM_DONE)
        {
          return (UIM_CMD_ERR);
        } /* end if - there was a problem with the APDU */

        /* Get the next state. */
        ++uim_ptr->command.generic_state_ptr;
      } /* end case - UIM_INVALIDATE_ST */
      break;

    case UIM_REHABILITATE_ST:                 /* Rehab state */
      {
        /* Check if there was a problem with Rehab */
        if (rsp_ptr->cmd_status != UIM_DONE)
        {
          return (UIM_CMD_ERR);
        } /* end if - there was a problem with the APDU */

        /* Get the next state. */
        ++uim_ptr->command.generic_state_ptr;
      } /* end case - UIM_REHABILITATE_ST */
      break;

    case UIM_SEEK_ST:
      {
        /* Return an error status if the command failed. */
        if (rsp_ptr->cmd_status != UIM_DONE)
        {
          /* Check if SFI was used and the command failed.
             If yes then retry the command without using SFI. */
          if (uim_ptr->command.cmd_ptr->seek.sfi != UIM_INVALID_SFI)
          {
            if(uim_ptr->card_cmd.resp_buf_ptr->cmd_status != UIM_RECORD_NOT_FOUND)
            {
              /* 1. Blank out the SFI so its not used while re-attempting the command.
                 2. Blank out the directory structure as we are uncertain of the current directory
                    pointer in the card and while re-attempting the command we would like to
                    start from MF */
              uim_ptr->command.cmd_ptr->seek.sfi = UIM_INVALID_SFI;
              uim_clear_current_path(uim_ptr, curr_ch);
              return UIM_CMD_ERR_WITH_SFI;
            }
            else
            {
              /* If seek failed due to 6A 83 then do not re-attempt
                  Update the last_sel_ef and return UIM_CMD_ERR */
              uim_update_current_dir_sfi(uim_ptr, &uim_ptr->command.cmd_ptr->seek.path, curr_ch);
            }
          }
          return (UIM_CMD_ERR);
        }
        else
        {
          /* Skip to the next state. */
          /* To update the last_sel_dir and the last_sel_ef if SFI was used */
          if(uim_ptr->command.cmd_ptr->seek.sfi != UIM_INVALID_SFI)
          {
            UIMDRV_MSG_HIGH_0(uim_ptr->id,"UIM_SEEK_ST - SFI used successfully");
            uim_update_current_dir_sfi(uim_ptr, &uim_ptr->command.cmd_ptr->seek.path, curr_ch);
          }

          ++uim_ptr->command.generic_state_ptr;
        }
      } /* end case - UIM_SEEK_ST */
      break;

    case UIM_TERMINAL_PROFILE_ST:             /* Term profile State */
      {
        /* We need to cache the terminal profile value even if there was
           error in the transaction. GSTK will not send terminal profile
           again if we hit recovery during terminal profile APDU */
	      
        if (uim_ptr->command.cmd_ptr->terminal_profile.num_bytes <= UIM_TERMINAL_PROFILE_DATA_LENGTH)
        {
          UIMDRV_MSG_HIGH_0(uim_ptr->id,"Cache Terminal Profile");
          /* Store the number of bytes that was sent in the command */
          uim_ptr->cache.int_term_profile_data.num_bytes =
                                   uim_ptr->command.cmd_ptr->terminal_profile.num_bytes;
          /* Copy the data from the command into internal buffer */
          uim_memscpy(uim_ptr->cache.int_term_profile_data.data,
                     sizeof(uim_ptr->cache.int_term_profile_data.data),
                     uim_ptr->command.cmd_ptr->terminal_profile.data,
                     uim_ptr->command.cmd_ptr->terminal_profile.num_bytes);
        }
	      
        /* Return an error status if the command failed. */
        if (rsp_ptr->cmd_status != UIM_DONE)
        {
          return (UIM_CMD_ERR);
        }
        else
        {
          /* Skip to the next state. */
          ++uim_ptr->command.generic_state_ptr;
          if (uim_nv_is_feature_enabled(UIMDRV_FEATURE_MIN_TPL_ICCID_SUPPORT,
                                        uim_ptr) &&
              NULL != uim_ptr->state.iccid_tpl_ptr)
          {
            /* Terminal Profile is working fine, No need to update ICCID and TPL */
            /* No need to continue uim_ptr->state.iccid_tpl_ptr further, freeing memory here */
            UIM_FREE(uim_ptr->state.iccid_tpl_ptr);
            /* After that memory will be allocate in next terminal profile
               command */
          }
        }
      } /* end case - UIM_TERMINAL_PROFILE_ST */
      break;

    case UIM_ENVELOPE_ST:                     /* Envelope State */
      {
        /* Return an error status if the command failed. */
        if ((rsp_ptr->cmd_status != UIM_DONE) &&
            (rsp_ptr->cmd_status != UIM_TOOLKIT_BUSY))
        {
          return (UIM_CMD_ERR);
        }
        else
        {
          /* Skip to the next state. */
          ++uim_ptr->command.generic_state_ptr;
        }
      } /* end case - UIM_ENVELOPE_ST */
      break;

    case UIM_FETCH_ST:                        /* Fetch State */
      {
        /* Return an error status if the command failed. */
        if (rsp_ptr->cmd_status != UIM_DONE)
        {
          return (UIM_CMD_ERR);
        }
        else
        {
          /* Skip to the next state. */
          ++uim_ptr->command.generic_state_ptr;
        }
      } /* end case - UIM_FETCH_ST */
      break;

    case UIM_TERMINAL_RESPONSE_ST:            /* Term response State */
      {
        /* Return an error status if the command failed. */
        if (rsp_ptr->cmd_status != UIM_DONE)
        {
          return (UIM_CMD_ERR);
        }
        else
        {

          if ( completed_status == UIM_CMD_FETCH)
          {
            /* Tell GSTK a proactive command pending */
            uim_ptr->command.rpt_buf.rpt.terminal_response.proactive_cmd_pending = TRUE;
          }

          /* Skip to the next state. */
          ++uim_ptr->command.generic_state_ptr;
        }
      } /* end case - UIM_TERMINAL_RESPONSE_ST */
      break;

    case UIM_POLLING_INTERVAL_ST:
      {
        /* Skip to the next state. */
        ++uim_ptr->command.generic_state_ptr;
      } /* end case - UIM_POLLING_INTERVAL_ST */
      break;

    case UIM_STREAM_APDU_ST:
      {
        /* Return an error status if the command failed. */
        if (rsp_ptr->cmd_status != UIM_DONE)
        {
          uim_ptr->command.cmd_ptr->hdr.cmd_count = UIM_MAX_NUM_ATTEMPTS;
          return (UIM_CMD_ERR);
        }
        uim_stream_apdu_success(uim_ptr, uim_req_buf_static_ptr, rsp_ptr);

      } /* end case - UIM_STREAM_APDU_ST */
      break;

    case UIM_RECOVERY_COMPLETE_ST:
      {
        /* Just skip to the next state */
        ++uim_ptr->command.generic_state_ptr;
      }
      break;

    case UIM_SIMULATE_NULL_ST:
      {
        /* Just skip to the next state */
        ++uim_ptr->command.generic_state_ptr;
      }
      break;

    case UIM_DONE_ST:                         /* Done State */
    {
      /* Do nothing */
    } /* end case - UIM_DONE_ST */
      break;

#ifdef FEATURE_UIM_USB_UICC
    case UIM_USB_POWER_OFF_ST:
    {
      UIMDRV_MSG_MED_0(uim_ptr->id, "UIM USB UICC ICCD POWER OFF complete");
      ++uim_ptr->command.generic_state_ptr;
    }
      break;

    case UIM_USB_POWER_ON_ST:
    {
      UIMDRV_MSG_MED_0(uim_ptr->id, "UIM USB UICC ICCD POWER ON complete");
      uim_reset_poll_timer_val(uim_ptr);
      ++uim_ptr->command.generic_state_ptr;
    }
      break;
#endif /* FEATURE_UIM_USB_UICC */

    case UIM_RECOVERY_PERFORM_TP_ST:
      {
        if (rsp_ptr->cmd_status != UIM_DONE)
        {
          UIMDRV_MSG_ERR_1(uim_ptr->id,"CMD Status = 0x%x, Continuing with recovery !!",
                           rsp_ptr->cmd_status);
        }
        /* Move to the next state. */
        ++uim_ptr->command.generic_state_ptr;
      }
      break;

    case UIM_RECOVERY_FLUSH_MANAGE_CHANNEL_ST:
      {
        UIMDRV_MSG_MED_0(uim_ptr->id,"Should not reach here!!");
      }

    case UIM_RECOVERY_VERIFY_CHV1_ST:
      {
        /* Restart recovery if command failed. */
        if (rsp_ptr->cmd_status != UIM_DONE)
        {
          UIMDRV_MSG_MED_0(uim_ptr->id,"Pin1 Verification failed during recovery");
          return (UIM_CMD_ERR);
        }
        else
        {
          /* Move to the next state. */
          ++uim_ptr->command.generic_state_ptr;
        }
      }
      break;

    case UIM_RECOVERY_VERIFY_CHV2_ST:
      {
        /* Restart recovery if command failed. */
        if (rsp_ptr->cmd_status != UIM_DONE)
        {
          UIMDRV_MSG_MED_0(uim_ptr->id,"Pin2 Verification failed during recovery");
          return (UIM_CMD_ERR);
        }
            ++uim_ptr->command.generic_state_ptr;
          }
      break;

    case UIM_RECOVERY_REHAB_ST:
      {
        /* Restart recovery if command failed. */
        /* The item has been rehabilitated if it's get response has been
         * cached.... remove it from the cache so that the next select
         * on the item is forced to do a get response
         */

        if (rsp_ptr->cmd_status != UIM_DONE)
        {
          UIMDRV_MSG_MED_1(uim_ptr->id,"Recovery Rehabilitate on file_index= %d returned error",
                           uim_ptr->state.recovery_data.rehab_file_index);
          return (UIM_CMD_ERR);
        } /* end if - there was a problem with the APDU */
        /* Increment file index */
        ++uim_ptr->state.recovery_data.rehab_file_index;
        /* Move to check for rehab state for next file */
        uim_ptr->command.generic_state_ptr -=2;
      }
      break;

    case UIM_RECOVERY_CHECK_EF_TST_UPDATE_REQD_ST:
      {
        UIMDRV_MSG_ERR_0(uim_ptr->id, "RECOVERY CHECK EF_TST_UPDATE REQD is not a valid response state");
      }
      break;

    case UIM_RECOVERY_SELECT_EF_TST_ST:
      {
        if(rsp_ptr->cmd_status != UIM_DONE)
        {
          UIMDRV_MSG_HIGH_0(uim_ptr->id,"select of EF-TST failed. skip write EF-tst and move to next state");
          rsp_ptr->cmd_status               = UIM_DONE;
    
          /*select failed there might be a mismatch in directory.
          to avoid that queue a select MF command to reset the directory*/
          uim_ptr->state.mf_selection.required  = TRUE;
          uim_ptr->state.mf_selection.channel   = uim_ptr->state.recovery_data.ch_to_be_recovered;
          uim_explicit_reselect_mf_aid(uim_ptr);

          if(uim_ptr->state.recovery_data.protocol == UIM_UICC)
          {
            /* Increment channel index */
            ++uim_ptr->state.recovery_data.ch_to_be_recovered;
            /* Move to next channel*/
            uim_ptr->command.generic_state_ptr -= 7;
          }
          else
          {
            uim_ptr->command.generic_state_ptr +=2;
          }
          break;
        }

        if(UIM_ICC == uim_ptr->command.cmd_ptr->hdr.protocol  &&
           (uim_ptr->command.cmd_ptr->select.path_position < 2) &&
           UIM_DONE == rsp_ptr->cmd_status)
        {
          /* For ICC we select incrementally*/
          uim_ptr->command.cmd_ptr->select.path_position++;
          break;
        }

        /*update the current path to EF-tst*/
        if(uim_ptr->command.cmd_ptr->hdr.protocol == UIM_UICC)
        {
          uim_ptr->state.current_path[uim_ptr->state.recovery_data.ch_to_be_recovered].path.path[0] = UIM_MF_ID;
          uim_ptr->state.current_path[uim_ptr->state.recovery_data.ch_to_be_recovered].path.path[1] = 0x7FFF;
          uim_ptr->state.current_path[uim_ptr->state.recovery_data.ch_to_be_recovered].path.path[2] = 0x7F66;
          uim_ptr->state.current_path[uim_ptr->state.recovery_data.ch_to_be_recovered].path.path[3] = 0x6FD2;

          uim_ptr->state.current_path[uim_ptr->state.recovery_data.ch_to_be_recovered].path.len     = 4;
          uim_ptr->state.current_path[uim_ptr->state.recovery_data.ch_to_be_recovered].is_ef        = TRUE;
        }
        else
        {
          uim_ptr->state.current_path[0].path.path[0] = UIM_MF_ID;
          uim_ptr->state.current_path[0].path.path[1] = 0x7F66;
          uim_ptr->state.current_path[0].path.path[2] = 0x6FD2;

          uim_ptr->state.current_path[0].path.len     = 3;
          uim_ptr->state.current_path[0].is_ef        = TRUE;
        }
        ++uim_ptr->command.generic_state_ptr;
      }
      break;

    case UIM_RECOVERY_WRITE_EF_TST_ST:
      {
        if(rsp_ptr->cmd_status != UIM_DONE)
        {
          UIMDRV_MSG_HIGH_0(uim_ptr->id,"write of EF-TST failed. ignore the failure");
          rsp_ptr->cmd_status               = UIM_DONE;
        }

        if(uim_ptr->state.recovery_data.protocol == UIM_UICC)
        {
          /* Increment channel index */
          ++uim_ptr->state.recovery_data.ch_to_be_recovered;
          /* Move to next channel*/
          uim_ptr->command.generic_state_ptr -= 8;
        }
        else
        {
          ++uim_ptr->command.generic_state_ptr;
        }
      }
      break;

    case UIM_RECOVERY_MANAGE_CHANNEL_FOR_UICC_ST:
      {
        /* Restart recovery if command failed. */
        if (rsp_ptr->cmd_status != UIM_DONE)
        {
          UIMDRV_MSG_MED_1(uim_ptr->id,"Channel=%d failed to open during Recovery",
                           uim_ptr->state.recovery_data.ch_to_be_recovered);
          return (UIM_CMD_ERR);
        } /* end if - there was a problem with the APDU */
        else
        {
          UIMDRV_MSG_MED_1(uim_ptr->id,"Channel=%d opened during Recovery",
                           uim_ptr->state.recovery_data.ch_to_be_recovered);
          ++uim_ptr->state.recovery_data.ch_to_be_recovered;
        }
      }
      break;

    case UIM_RECOVERY_CLOSE_CHANNEL_FOR_UICC_ST:
      {
        if (rsp_ptr->cmd_status != UIM_DONE)
        {
          UIMDRV_MSG_MED_1(uim_ptr->id,"Channel=%d failed to close during Recovery",
                           uim_ptr->state.recovery_data.ch_to_be_recovered);
          /* Clear out the command response timer as we are setting the timeout signal */
          (void) rex_clr_timer( &uim_ptr->command.cmd_rsp_timer);
          /* UIM should try to recover */
          (void) rex_set_sigs( uim_ptr->tcb_ptr, UIM_CMD_RSP_TIMEOUT_SIG );
        } /* end if - there was a problem with the APDU */
        else
        {
           /*Set this flag to FASLE for every closed channel*/
           uim_ptr->state.is_ch_open[uim_ptr->state.recovery_data.ch_to_be_recovered] = FALSE;
           UIMDRV_MSG_MED_1(uim_ptr->id,"Channel=%d closed during Recovery",
                            uim_ptr->state.recovery_data.ch_to_be_recovered);
          /* Process next channel */
          ++uim_ptr->state.recovery_data.ch_to_be_recovered;
        }
      }
      break;

    case UIM_RECOVERY_VERIFY_UPIN_FOR_UICC_ST:
      {
        /* Restart recovery if command failed. */
        if (rsp_ptr->cmd_status != UIM_DONE)
        {
           UIMDRV_MSG_MED_0(uim_ptr->id,"UPIN verification failed");
           return (UIM_CMD_ERR);
        } /* end if - there was a problem with the APDU */
        /* Move to next state */
        ++uim_ptr->command.generic_state_ptr;
      }
      break;

    case UIM_MANAGE_CHANNEL_ST:
      {
        /* Force uim status to success when card returns 62 00 */
        if(rsp_ptr->cmd_status == UIM_NO_INFO_AND_NV_UNCHANGED)
        {
          rsp_ptr->cmd_status = UIM_DONE;
        }
        /* Determine if the UIM command responded with a problem. */
        if (rsp_ptr->cmd_status != UIM_DONE)
        {
          if( ( UIM_WRONG_CLASS == rsp_ptr->cmd_status &&
                SW1_CLA_BAD     == rsp_ptr->sw1    &&
                SW2_NORMAL_END  == rsp_ptr->sw2 ) ||
              (uim_ptr->command.cmd_ptr->manage_channel.mc_action == UIM_MC_CLOSE))
          {
            uim_ptr->command.cmd_ptr->hdr.cmd_count = UIM_MAX_NUM_CMD_ATTEMPTS(uim_ptr);
          }

          if ( (uim_ptr->command.cmd_ptr->manage_channel.mc_action == UIM_MC_CLOSE) &&
               (TRUE == uim_ptr->state.aram_app.select_aram_failed) &&
               (uim_ptr->command.cmd_ptr->manage_channel.op_close_channel == uim_ptr->state.aram_app.channel) )
          {
            (void)uim_trigger_silent_recovery((uim_slot_type)(uim_ptr->id+1), 
                                               uim_ptr->command.cmd_ptr,
                                               NULL);
            uim_ptr->state.aram_app.select_aram_failed = FALSE;
            uim_ptr->state.aram_app.channel            = 0xFF;
          }
          /* Indicate a problem with the command. */
          return (UIM_CMD_ERR);
        }
        else /* The command completed successfully. */
        {
          if ((uim_ptr->command.cmd_ptr->manage_channel.mc_action == UIM_MC_CLOSE) &&
              (TRUE == uim_ptr->state.aram_app.select_aram_failed) &&
              (uim_ptr->command.cmd_ptr->manage_channel.op_close_channel == uim_ptr->state.aram_app.channel))
          {
            uim_ptr->state.aram_app.select_aram_failed = FALSE;
            uim_ptr->state.aram_app.channel            = 0xFF;
          }
          /* Get the next state. */
          uim_ptr->command.generic_state_ptr++;
        } /* end if - the UIM command responded with a problem. */
      }
      break;

    default:
      {
        UIMDRV_MSG_ERR_1(uim_ptr->id, "***Unknown UIM Generic state: %d***",
                         curr_uim_generic_state);
      } /* end case - default */
      break;
  } /* end switch - curr_uim_generic_state */

  /* Continue processing the command only if the response indicates success. */
  if (status == UIM_CMD_SUCCESS)
  {
    /* Process the next state of this command. */
    uim_generic_command(uim_ptr);
  } /* end if - command is still in progress */

  /* Determine if the command has completed */
  if (UIM_DONE_ST == *uim_ptr->command.generic_state_ptr)
  {
    /* Indicate the command has completed successfully. */
    return (completed_status);
  } /* end if - uim_ptr->command.generic_state_ptr is UIM_DONE_ST */

  return(status);

} /* uim_generic_command_response */


/*===========================================================================

FUNCTION UIM_PROCESSING_ENVELOPE_APDU

DESCRIPTION
  This function informs whether UIM is processing an envelope command.

DEPENDENCIES
  None

RETURN VALUE
  TRUE : UIM is processing an envelope command
  FALSE: UIM is not processing an envelope command

SIDE EFFECTS
  None

===========================================================================*/
boolean uim_processing_envelope_apdu
(
  uim_instance_global_type *uim_ptr
)
{
  /* Check if we are processing ENVELOPE command */
  if( UIM_ENVELOPE_ST == uim_ptr->mode.generic_states[0] )
  {
    return TRUE;
  }

  return FALSE;
} /* uim_processing_envelope_apdu */


/*===========================================================================
FUNCTION UIM_IS_TEST_ICCID

DESCRIPTION
  This function checks if the current ICCID is GCF ICCID or not.

RETURN VALUE
  Boolean
    TRUE : If current iccid is same as iccid used in GCF test cases
    FALSE: If current iccid is NOT same as iccid used in GCF test cases

DEPENDENCIES
  None
===========================================================================*/
boolean uim_is_test_iccid
(
  uim_instance_global_type *uim_ptr
)
{
  if(memcmp(uim_ptr->cache.iccid_buf.data, UIM_TEST_ICCID, UIM_ICCID_SIZE) == 0)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}/* uim_is_test_iccid */


/*===========================================================================
FUNCTION UIM_COMPARE_TOOLKIT_CMDS

DESCRIPTION
  UIM Action function callback that is called by q_linear_delete when the
  compare callback function returns TRUE. This function will return TRUE only
  when the command id is that of a Terminal Response.
  These commands need to be removed from UIM queue before starting recovery.

PARAMETER
  item_ptr  [Input]:  ptr to queue item
  param_ptr [Input]:  no use as of now. This will be populated as NULL
                      by q_linear_delete()

DEPENDENCIES
  None

RETURN VALUE
  TRUE - if there is a match
  FALSE - if there is no match

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
int uim_compare_toolkit_cmds
(
  void *item_ptr,
  void *param_ptr
)
{
  uim_cmd_type             *cmd_ptr          = NULL;

  if(item_ptr == NULL)
  {
    return 0;
  }
  cmd_ptr = (uim_cmd_type*)item_ptr;

  /* Clear Terminal Response commands sent by GSTK before recovery */

  switch((uim_cmd_name_type)cmd_ptr->hdr.command)
  {
    case UIM_TERMINAL_RESPONSE_F:
    case UIM_ENVELOPE_F:
    case UIM_SILENT_RECOVER_UICC_F:
      return 1;
    default:
      return 0;
  }/* switch */

  return 0;
} /* uim_compare_toolkit_cmds */


/*===========================================================================
FUNCTION UIM_REMOVE_TOOLKIT_CMDS

DESCRIPTION
  UIM Action function callback that is called by q_linear_delete when the
  compare callback function returns TRUE. This function puts the command
  removed from the command queue into the free queue.

PARAMETER
  item_ptr  [Input]:  ptr to queue item
  param_ptr [Input]:  no use as of now. This will be populated as NULL
                      by q_linear_delete()

DEPENDENCIES
  None

RETURN VALUE
  void

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void uim_remove_toolkit_cmds
(
  void *item_ptr,
  void *param_ptr
)
{
  uim_cmd_type          *cmd_ptr      = NULL;
  uim_instance_global_type *uim_ptr   = NULL;

  if(item_ptr == NULL || param_ptr == NULL)
  {
    UIM_MSG_HIGH_0("item_ptr is NULL");
    return;
  }

  cmd_ptr = (uim_cmd_type*)item_ptr;
  uim_ptr = (uim_instance_global_type*)param_ptr;

  UIMDRV_MSG_HIGH_1(uim_ptr->id,"Removing command=%d from UIM queue",
                    cmd_ptr->hdr.command);

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
    UIMDRV_MSG_HIGH_0(uim_ptr->id, "Sending  error report for toolkit command");
    uim_send_generic_err_rpt(cmd_ptr, uim_ptr);
  }
  else
  {  /* return buffer to free queue */
    if ( cmd_ptr->hdr.cmd_hdr.done_q_ptr != NULL )
  {
      /* place command on requested queue */
      q_put( cmd_ptr->hdr.cmd_hdr.done_q_ptr,
             &cmd_ptr->hdr.cmd_hdr.link );
     }
  }
} /* uim_remove_toolkit_cmds */


/*===========================================================================
FUNCTION UIM_CLEAR_RECOVERY_PARAMS

DESCRIPTION
  This function clears the global parameters used for recovery when recovery
  is complete .

PARAMETER
  None

DEPENDENCIES
  None

RETURN VALUE
  void

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void uim_clear_recovery_params
(
  uim_instance_global_type *uim_ptr
)
{
  RETURN_IF_INSTANCE_INVALID(uim_ptr->id);
  if (uim_ptr->flag.recovery_triggered)
  {
    /* Reset recovery triggered flag */
    uim_ptr->flag.recovery_triggered = FALSE;
    uim_ptr->flag.fetch_pending  = TRUE;
    /* Switch the mode back to the saved previous mode */
    uim_ptr->command.mode = uim_ptr->state.save_mode;
    memset(&uim_ptr->state.recovery_data, 0x00, sizeof (uim_recovery_data_type));
    gstk_send_recovery_cmd((uim_ptr->id + 1), UIMDRV_RECOVERY_STAGE_END);

    /* Notify the RECOVERY EVENT if callback is registered for SILENT RECOVERY */
    if(NULL != uim_ptr->silent_recover.uim_silent_recovery_callback)
    {
      uim_ptr->silent_recover.uim_silent_recovery_callback((uim_slot_type)(uim_ptr->id  +1),
                               uim_ptr->silent_recover.user_data,UIMDRV_RECOVERY_STAGE_END);
      /* Makesure It's not called again and again */
      uim_ptr->silent_recover.uim_silent_recovery_callback = NULL;
      uim_ptr->silent_recover.user_data = NULL;
    }
  }
}/* uim_clear_recovery_params */


/*===========================================================================
FUNCTION UIM_SAVE_CURRENT_REQ_BUF

DESCRIPTION
  This function saves the current request buffer.

PARAMETER
  None

DEPENDENCIES
  None

RETURN VALUE
  void

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void uim_save_current_req_buf(uim_instance_global_type *uim_ptr)
{
  uim_ptr->state.saved_req_buf.apdu_hdr.uim_class = uim_ptr->command.req_buf.apdu_hdr.uim_class;
  uim_ptr->state.saved_req_buf.apdu_hdr.instrn    = uim_ptr->command.req_buf.apdu_hdr.instrn;
  uim_ptr->state.saved_req_buf.instrn_case        = uim_ptr->command.req_buf.instrn_case;
  uim_ptr->state.saved_req_buf.apdu_hdr.p1        = uim_ptr->command.req_buf.apdu_hdr.p1;
  uim_ptr->state.saved_req_buf.apdu_hdr.p2        = uim_ptr->command.req_buf.apdu_hdr.p2;
  uim_ptr->state.saved_req_buf.apdu_hdr.p3        = uim_ptr->command.req_buf.apdu_hdr.p3;
} /* uim_save_current_req_buf */


/*===========================================================================
FUNCTION UIM_RESTORE_CURRENT_REQ_BUF

DESCRIPTION
  This function restores the current request buffer.

PARAMETER
  None

DEPENDENCIES
  None

RETURN VALUE
  void

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void uim_restore_current_req_buf(uim_instance_global_type *uim_ptr)
{
  uim_ptr->command.req_buf.apdu_hdr.uim_class = uim_ptr->state.saved_req_buf.apdu_hdr.uim_class;
  uim_ptr->command.req_buf.apdu_hdr.instrn    = uim_ptr->state.saved_req_buf.apdu_hdr.instrn;
  uim_ptr->command.req_buf.instrn_case        = uim_ptr->state.saved_req_buf.instrn_case;
  uim_ptr->command.req_buf.apdu_hdr.p1        = uim_ptr->state.saved_req_buf.apdu_hdr.p1;
  uim_ptr->command.req_buf.apdu_hdr.p2        = uim_ptr->state.saved_req_buf.apdu_hdr.p2;
  uim_ptr->command.req_buf.apdu_hdr.p3        = uim_ptr->state.saved_req_buf.apdu_hdr.p3;
} /* uim_restore_current_req_buf */

/*===========================================================================
FUNCTION UIM_COMPARE_MANAGE_CH_CMD

DESCRIPTION
  UIM Action function callback that is called by q_linear_delete when the
  compare callback function returns TRUE. This function will return TRUE only
  when the command id is that of a MANAGE CHANNEL in case of Processing
Recovery while in ICC mode
  These commands need to be removed from UIM queue before starting recovery.

PARAMETER
  item_ptr  [Input]:  ptr to queue item
  param_ptr [Input]:  no use as of now. This will be populated as NULL
                      by q_linear_delete()

DEPENDENCIES
  None

RETURN VALUE
  TRUE - if there is a match
  FALSE - if there is no match

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
int uim_compare_manage_ch_cmd
(
  void *item_ptr,
  void *param_ptr
)
{
  uim_cmd_type          *cmd_ptr      = NULL;

  cmd_ptr = (uim_cmd_type*)item_ptr;

  if (!cmd_ptr ||
      cmd_ptr->hdr.slot == UIM_SLOT_NONE ||
      cmd_ptr->hdr.slot >= UIM_SLOT_AUTOMATIC)
  {
    return 0;
  }

  switch((uim_cmd_name_type)cmd_ptr->hdr.command)
  {
    case UIM_MANAGE_CHANNEL_F:
      UIMDRV_MSG_MED_0((cmd_ptr->hdr.slot - 1), "MANAGE CHANNEL command matched");
      return 1;
    default:
      return 0;
  }/* switch */

} /* uim_compare_toolkit_cmds */


/*===========================================================================
FUNCTION UIM_REMOVE_MANAGE_CH_CMD

DESCRIPTION
  UIM Action function callback that is called by q_linear_delete when the
  compare callback function returns TRUE. This function puts the command
  removed from the command queue into the free queue.

PARAMETER
  item_ptr  [Input]:  ptr to queue item
  param_ptr [Input]:  no use as of now. This will be populated as NULL
                      by q_linear_delete()

DEPENDENCIES
  None

RETURN VALUE
  void

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None

===========================================================================*/
void uim_remove_manage_ch_cmd
(
  void  *item_ptr,
  void  *param_ptr
)
{
  uim_cmd_type             *cmd_ptr          = NULL;
  uim_instance_global_type *uim_ptr = (uim_instance_global_type*)param_ptr;

  if (uim_ptr == NULL || item_ptr == NULL)
  {
    return;
  }

  cmd_ptr = (uim_cmd_type*)item_ptr;

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
    uim_send_generic_err_rpt(cmd_ptr, uim_ptr);
  }
  else
  {  /* return buffer to free queue */
    UIMDRV_MSG_HIGH_1(uim_ptr->id, "Not Sending message error report 0x%x", 
                      cmd_ptr->hdr.command);
    if ( cmd_ptr->hdr.cmd_hdr.done_q_ptr != NULL )
    {
      /* place command on requested queue */
      q_put( cmd_ptr->hdr.cmd_hdr.done_q_ptr,
             &cmd_ptr->hdr.cmd_hdr.link );
     }
  }
} /* uim_remove_toolkit_cmds */


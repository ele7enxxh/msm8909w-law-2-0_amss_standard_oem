/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                D A T A   S E R V I C E S

                A T   C O M M A N D   
                
                ( M O B I L E  E Q U I P M E N T  C O M M A N D S )
                
                P R O C E S S I N G

GENERAL DESCRIPTION
  This module executes the AT commands. It mainly executes the 
  Mobile Equipment commands.

EXTERNALIZED FUNCTIONS
  dsat_exec_cpls_cmd
    This function takes the result from the command line parser
    and executes it. It executes +CPLS command.
  dsat_send_cme_error
    Send Mobile Equipment error to TE or PS protocol
    stack based on response routing. Use for +CMEE error reporting
    during async command.

EXTERNALIZED FUNCTIONS INTERNAL TO DSAT UNIT

  dsatetsime_exec_cpas_cmd
    This function takes the result from the command line parser
    and executes it. It executes +CPAS command.
    The result is in res_buff_ptr, reporting phone activity status

  dsatetsime_exec_csq_cmd
    This function takes the result from the command line parser
    and executes it. It executes +CSQ command.
    The result is in res_buff_ptr, reporting received signal strength 
    indication and channel bit error rate.

  dsatetsime_exec_cbc_cmd
    This function takes the result from the command line parser
    and executes it. It executes +CBC command.
    The result is in res_buff_ptr, reporting battery condition status
    and charge level.

  dsatetsime_exec_cpin_cmd
    This function takes the result from the command line parser
    and executes it. It executes +CPIN command.
    This set commands sends to the ME a password which is necessary before 
    operation.

  dsatetsime_mmgsdi_perso_event_handler
    This function is the handler function for the MMGSDI personalization events

  dsatetsime_init_me
    ME initialization function. This function does the following:
    - Initializes ATCOP as one of the clients to CM.
    - Registers the CM client call back functions.
    - Initializes the unsolicited response event queue.
    - Sets initial phone functionality level.

  dsatetsime_get_pin_status
    This function provides an interface to query SIM PIN.

  dsatetsime_change_pin
    This function provides an interface to change a specific SIM PIN.

  dsatetsime_exec_cind_cmd
    This function takes the result from the command line parser and
    executes it. It executes +CIND command.  This command reports the
    status of ME indicators.
    
  dsatetsime_change_indicator_state
    This function updates the specified indicator to the specified
    state.  It provides a wrapper for indicator state tracking.  The
    state of the indicators is reported by +CIND.
    
  dsatetsime_exec_cmec_cmd
    This function takes the result from the command line parser
    and executes it. It executes +CMEC command.
    This command selects the equipment which operates the keypad,
    writes to the display, and sets indicators.
    
  dsatetsime_exec_cimi_cmd
    This function takes the result from the command line parser
    and executes it. It executes +CIMI command.
    This command returns the international mobile subscriber identy (IMSI).
    
  dsatetsime_exec_qcpinstat_cmd
    This function takes the result from the command line parser
    and executes it. It executes $QCPINSTAT command.
    This command sends to the ME the status of all PINs for all cards.

  dsatetsime_exec_csim_cmd
    This function takes the result from the command line parser
    and executes it. It executes +CSIM command.
    This command allows generic SIM access.

  dsatetsime_exec_crsm_cmd
    This function takes the result from the command line parser
    and executes it. It executes +CRSM command.
    This command allows restricted SIM access.

  dsatetsime_sim_read
    This function provides an interface to read SIM EF's.  The input
    parameter dictates the caller which maps to a specific EF.
    
  dsatetsime_sim_write
    This function provides an interface to write SIM EF's.  The input
    parameter req_submitter dictates the caller which maps to a
    specific EF.  The buffer and buffer lenght are also specified.

  dsatetsime_exec_clac_cmd
    This function takes the result from the command line parser 
    and executes it. It executes at+clac command.
    at+clac is used to list all supported at commands.

INITIALIZATION AND SEQUENCING REQUIREMENTS

   Copyright (c) 2001 - 2015 by Qualcomm Technologies Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  L:/src/asw/MSM5200/data/vcs/dsatetsime.c_v   1.6   12 Jul 2002 10:08:04   randrew  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsatetsime.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/24/14   tk      ATCoP changes for dynamic memory optimizations.
08/27/14   pg      Fixed CLCK query issue.
06/27/14   tk/pg   Added support for Dynamic ATCoP.
06/12/14   pg      Depricated mmgsdi_session_perso APIs.
04/14/14   tk      Fixed issues in MMGSDI events handling.
03/14/14   tk/sc   Optimized debug macros usage in ATCoP.
12/30/13   sc      Fixed static code bugs.
12/17/13   sc      Fixed +CSIM APDU response size issue.
11/22/13   sc      Fixed Bolt MOB3.0 compiler warnings.
11/18/13   sc      Fixed ATCoP KW errors.
10/28/13   pg      Fixed CPOL issue on DSDS.
09/13/13   sc      Fixed KW errors.
08/30/13   pg      Fixed predictable random number generation.
08/01/13   sc      Fixed KW errors.
07/16/13   pg      Fixed ^SYSINFO o/p for "SIM is not inserted" 
07/02/13   pg      Migrated to MMGSDI API to find PLMN info in SE13 table
05/28/13   tk      Deprecated battery driver API call for charge level.
04/08/13   tk      Fixed issue with restricted mode in few scenarios.
03/27/13   tk      Fixed issues in dual stack commands.
02/28/13   tk      Fixed CPIN issue when a PUK blocked SIM is inserted.
02/28/13   tk      Fixed stack overflow issue.
01/17/13   tk      Fixed KW errors.
01/04/13   tk      Fixed +CPAS and +CEER issues.
01/04/13   tk      ATCoP changes for DSDX C/G+G with True World Mode Support.
12/03/12   tk      Mainlined the PMIC RTC features.
11/30/12   tk      ATCoP changes for Triton DSDA.
11/02/12   tk      Migrated to mmgsdi_session_get_file_attr_ext().
09/13/12   sk      Fixed CSQ command.
08/22/12   sk      Mainlining Nikel Changes.
08/10/12   tk      Deprecated ATCoP - DIAG interface.
07/31/12   sk      C-EONS Feature Support.
07/31/12   sb      Fixed KW Errors.
07/26/12   sb      Fixed KW Errors.
06/20/12   tk      Fixed issues related to ^HDRCSQ and ^SYSINFO commands.
05/18/12   tk      Migrated to MSG 2.0 macros
02/27/12   sb      Fixed compiler warnings.
04/20/12   sk      DSAT_NIKEL featurization changes.
02/17/12   sk      Migrating Data related AT commands interface to unified MH.
02/06/12   ad      Added CSTF/CEN/CGPIAF/CVMOD command support.
01/19/12   sk      Feature cleanup.
01/04/12   sb      Fixed KW Warnings.
11/03/11   sb      Fixed KW Warnings.
10/25/11   ua      Enhanced EONS to read EFHPLMN to check if a PLMN is registered
                   in HPLMN. 
10/20/11   nc      Added support for CUAD/CEAP/CERP commands.
09/27/11   ht      Included dsut header to support offtarget stubs.
07/18/11   bs      Fixed +CRSM command to use a different cb_expected.
02/24/11   bs      Fixed Klocwork errors.
02/04/11   nc      Featurising CKPD under FEATURE_HS.
01/11/11   ad      Removed pending variable .
01/11/11   ad      Remove extern usage.used get/set API for command associated  
                   value pointers.
10/25/10   ad      Init Changes for DSDS. 
08/24/10   ad      Fixed CCLK issue.
07/27/10   bs      Fixed MMGSDI commands queuing in RPC context.
07/14/10   ua      Fixed race condition in processing REFRESH for 
                   SIM files against regular SIM file operations.
07/20/10   ad      Fixed compiler warning.
07/14/10   ad      Fixed $CSQ issue.
07/12/10   ad      Fixed +CPOL issue.
07/06/10   ad      Removing extern declaration from time function .
06/04/10   ua      Removing support of CKPD and HS services for all 
                   THIN UI targets.
06/04/10   bs      MMGSDI SESSION Migration.
05/31/10   ad      Fixed Klocwork errors. 
05/26/10   sa      Fixed +CLCK query command.
05/10/10   kk      Adding support for +ICCID.
01/18/10   bs      Added support for +PACSP.
02/08/10   ua      Removing support for CLVL and CMUT. 
01/20/10   kk      Moved +CFUN to CommonATCoP.
12/15/09   nc      Featurisation changes for LTE.
12/07/09   nc      Added support for <E-UTRAN_AcT> parameter for +CPOL in LTE.
11/02/09   sa      Replacing snprintf with std_snprintf.
07/15/09   sa      Added support for $CSQ command.
07/14/09   bs      Added support for $CCLK command.
09/10/09   nc      Fixed Klocwork errors. 
09/09/09   ua      Added support for EONS (spec 22.101).
                   Removing COPS_GSDI_SIM_READ as it is not valid anymore.
05/16/09   ua      Fixed sending junk data in CPOL delete operation and also
                   resetting AcT data to 0.
07/10/09   vg      Fixed +CSQ to report BER(2G only).
04/20/09   ua      Fixed issues related with call commands followed by +CIND query.
06/29/09   vg      Replacing depricated MSG_XXX macro with MSG_SPRINTF_X.
06/28/09   vg      Fixed lint medium.
04/29/09   ua      Fixed compiler warnings. 
04/07/09   ms      CMI Integration fix.
03/13/09   nc      Fixed Compilation issue for Thin_ui builds.
04/20/09   sa      Updated +CLAC to include forwarded command list also.
01/28/09   sa      Fixed CLCK issue for SP personalization.
01/20/09   nc      Fixed +CPIN lock issue found when pin locked with 
                   CLCK with wrong password.
07/17/08   sa      Removed the support of +CKPD for thin-UI 8K builds.
04/14/08   ua      Removing support for +CLVL, +CMUT commands in 8K targets due
                   to non availabiltiy of SND support to ATCOP.
12/12/08   ua      Fixed Off target lint errors.
12/05/08   nc      Fixed +CPIN Query command when multiple pins are enabled.
12/02/08   ss      Off target Lint fix.
10/23/08   bs      Added support for 1X AT Phonebook commands.
09/03/08   bs      Fixed Alignment issues with MDM8200.
08/05/08   nc      Fixing compilation issue.
07/16/08   nc      Added support for <path_id> parameter for +CRSM command
03/24/08   bs      Fixed Klocwork criticals.
02/02/08   sa      Fixing +CPBR issue when the total record size is maximum.
01/30/08   ua      Handling MMGSDI_REFRESH_SUCCESS in CFUN command.
01/22/08   sa      Fixed +CLCK and +CPIN issues related with SIM PIN2.
01/21/08   bs      Added new command $QCSIMSTAT for sim status.
01/09/08   ua      Fixing +CPBR issue for emergency phone book (EN).
11/12/07   sa      Replaced pbm_find_number() function call with its UCS2 equivalent.
10/25/07   sa      Removed multiple calls for mmgsdi_get_all_pin_status during 
                   initialization.
                   Added FEATURE_DSAT_DEFAULT_CHANNEL to stream APDU always on 
                   default channel for USIM's.
                   Fixed initial CFUN query command.
10/17/07   pp      Fixed AT->SIO TX watermark issues with +CPBR amd +CPBF cmds.
09/25/07   ss      Fixed lint medium errors.
08/24/07   ss      Fixing memory leaks in CPBR command.
08/23/07   ss      Fixed issues with last dial entry in phone book for PS data call.
08/10/07   ss      Fixed compiler warning.
08/06/07   ss      Fixed CPIN to accept only digits as passwords.
07/18/07   pp      Moved +CLVL, +CMUT commands to mainline.
07/05/07   ss      Full UCS2 character support for phone book commands.
06/21/07   ua      Enabling +CKPD command for single processor THIN UI builds.
06/08/07   ua      Updating CPIN status variables when SIM ERROR is encountered. 
01/06/07   pp      Fixed RVCT compiler warnings.
05/29/07   ss      Check added for maximum PLMN list size while reading SIM.
05/17/07   ss      Moved to MMGSDI APIs for pin status operations.
                   Added new function dsatetsime_mmgsdi_req_cb.
05/02/07   ua      Fixed lint medium errors. 
04/14/07   pkp     Added support for +CCLK (Clock).
04/05/07   sa      Fixed +CPIN issue due to incorrect password usage by +CLCK 
                   and +CPWD in combination
03/05/07   sa      Added support for +CRSM to send unknown file-id to UIM.
02/13/07   sa      Fixed issues related to MULTIPORT support.
02/15/07   ua      Added new phone book "FD" support. 
                   Phone book commands (CPBS,CPBR,CPBW,CPBF) now doesnt not 
                   rely on the staus of the ADN phone book at initialization. 
02/15/07   ss      Fixed lint high errors
02/14/07   ph      dsatutil_reset_password() now requires total buffer size
02/14/07   pkp     Fixed Klocwork (NULL dereference) errors.
02/12/07   ua      Removing support for +CKPD in THIN_UI builds.
02/13/07   pkp     Fixed RVCT compiler warnings.
01/30/07   ss      Replaced banned string API calls.
12/06/06   ua      Correcting +CLCK query response.
11/03/06   snb     Use functions other than gsdi_cmd() that are exported in 
                   dual-processor architecture.
10/20/06   sa      Modfication of +CRSM/+CSIM using MMGSDI for AID retrieval.
10/09/06   ua      Correcting Feature wrap
09/15/06   sa      Fall back to default channel for +CSIM/+CRSM.
09/11/06   ua      Fix in CPIN PERM_BLOCKED for PIN2
09/07/06   ua      Correcting setting of dsat_cpol_res_type variable.
08/31/06   snb     Correct +CSIM/+CRSM behavior with USIMs.
07/29/06   ua      Modifications supporting modified MMGSDI API for mmgsdi_callback_type
07/05/06   sa      Corrected +CSIM for USIM.
06/13/06   ua      Corrected +CRSM for all USIM's.
06/08/06   ua      Eliminating usage of CM internal function cmph_info_get
06/08/06   sa      Add support for +CLAC and modified +CTZU command
05/24/06   ua      Fixed critical Lint errors and corrected build issues concerned with 
                   mmgsdilib.h
05/12/06   ua      CPLS, CPOL commands wrapped under FEATURE_MMGSDI
05/08/06   ua      Eliminating the use of gsdi interface for CPOL command. 
                   dsatetsime_get_num_pplmn_rec is removed. 
03/13/06   ua      CPOL command updated to use +CPLS command for sim file selection.
02/27/06   snb     Allow for 2 or 3 digit MNC in PLMN.
02/21/06   sa      Merged changes from Sirius Branch
                   -Fix +CPOL write command for other formats
                   -Added cpbs_process_test_cmd to display only the supported 
                   phone book.
                   -Correct +CCLK for secure time feature in RTC. 
                   -Correct +CBC battery charge level indication.                     
                   to avoid making duplicate entries in call history.
02/11/06   ua      Removed feautre FEATURE_DSAT_CFUN_CARD_POWER_CTL for mmgsdi interfaces
                   to implement CPLS, CPOL using mmgsdi interfaces.
01/12/06   snb     Use PBM 8bit alphabet in PBM interactions.
01/09/06   ua      Add support CPLS command.
01/06/06   snb     Display SIM PIN2/PUK2 in +CPIN response only when last 
                   related command failed.                  
12/17/05   snb     Correct call history entry and stop spurious response on UIM
                   failure during card power control.
11/09/05   snb     Lint errors.
11/07/05   snb     Changes to implement a second layer of flow control, 
                   ATCOP to USB, to avoid exhaustion of DSM large items and
                   allow +CIND read and test command in restricted mode.
11/02/05   hap     Added cpbs_process_test_cmd to display only the supported 
                   phone books
10/18/05   snb     Lint cleanup and get current operating mode if needed on 
                   +CFUN write.
10/13/05   snb     Don't allow +CFUN command until after CM has made first
                   operating mode report.
10/06/05   snb     Fix 6500-GSM build issues.
09/23/05   snb     Corrections for +CSIM/+CRSM USIM support.
09/20/05   hap     Modified dsatetsime_get_num_pplmn_rec to add support for 
                   +CPOL,^SPLR,^SPLW test commands for USIM
09/13/05   snb     Use LPM for +CFUN=4 mode, removed assert since SIM init can
                   occur at time other than power up.
08/29/05   snb     Add SIM card power control to +CFUN.
08/15/05   snb     Add support for reset via +CFUN command.
08/08/05   snb     Don't allow attempt to use USIM in +CSIM command.
08/03/05   hap     Changed dsatetsime_ld_save to avoid making duplicate entries 
                   in call history
07/29/05   ar      Use cached RSSI for +CSQ reporting. Lint fixes.
05/26/05   hap     Fixes for +CNUM
05/24/05   hap     Modifications for handling spurious responses from UI
05/19/05   iv      Removed function 'dsatetsime_crmp_handle_get_profile' and 
                   fixed bugs in +CRMP & ^SSET commands.
05/12/05   ar      Code cleanup for IMSI reporting.
04/29/05   tkk     Added missing terminating quote for +CCLK command.
04/19/05   tkk     Added support to retrieve NV items for +CVIB/+CRSL etc.
04/19/05   iv      Modified phone book commands to support "CS" (sorted list)
                   phone book.
04/19/05   hap     Changed design for +CNUM command &
                   corrections for ^SDBR
04/15/05   ar      Added used of dsatcmdp_set_restricted_mode().
04/14/05   iv      +CPBW test command number plan range change and lint
                   corrections.
04/05/05   ar      Add support for +CIND command.
04/04/05   snb     Open channel before and close channel after sending channel
                   data only for USIM card.
03/30/05   iv      Added index range checking for +CPOL command.
03/29/05   dvp     Fixing bug in ^SCID.
03/15/05   snb     Remove +CSIM command from &V response, handle all processing
                   in command handler.
03/11/05   snb     Add +CRSM restricted SIM access command.
03/08/05   iv      Added support for +CRMP command.
03/07/05   ar      Extend +CFUN capability for testing purposes.
03/04/05   snb     Fix PBM interface when storage is "LD" last dialed.
03/02/05   dvp     Fixed bugs in ^SMG[LRO]
02/23/05   ar      Rework keypad emulation.  Lint corrections.
02/22/05   iv      Renamed function "cpbr_parse_arguments" to
                   "dsatetsime_cpbr_parse_arguments".
02/14/05   tkk     Added support for +CVIB and corrected 
                   dsatetsime_snd_cmd_handler function.
02/14/05   hap     Added support for ^SDBR command.
02/14/05   iv      Modified +CPBS, +CPBW & +CPBR commands to support more 
                   phone book types.
02/07/05   tkk     Renamed is_date_valid to conform to coding conventions.
                   Modified CLIP handler to generic SS abort event handler.
02/09/05   hap     Changed etsime_report_cnum
02/03/05   iv      Added support for +CPOL command.
01/31/05   iv      Added support for +COPN command.
01/31/05   iv      Added support for +CLCC command.
01/27/05   pdv     Merge from Sirius-ATCoP development.
01/27/05   pdv     Rebase from Mainline.
01/20/05   tkk     Added sound callback handlers & installed +CLIP abort 
                   handler.
01/05/05   hap     Added support for +CNUM command.
12/30/04   tkk     Added dsatetsime_get_alphanumeric_name function.
12/29/04   snb     Lint corrections.
12/21/04   tkk     Added support for +CCLK command.
12/07/04   ar      Add support for +COPS command.
11/19/04   tkk     Added support for +CLVL and +CMUT commands.
11/18/04   snb     Add TE-ME alphabet conversion per +CSCS setting for PB
                   commands +CPBF, +CPBR, +CPBW.
11/09/04   ar      Add support for +CPUC command and dsatetsime_sim_read,
                   dsatetsime_sim_write.
10/22/04   ar      Add Advice of Charge support.
10/21/04   snb     Add TE-ME alphabet conversion per +CSCS setting for PB
                   commands +CPBF, +CPBR, +CPBW. Correct +CSIM response when
                   MMGSDI returns GET RESPONSE APDU
10/21/04   snb     Correct build warning for inventing cmph function and lint
                   MMGSDI returns GET RESPONSE APDU
10/13/04   dvp     Adding ^SCID, ^SCKA, ^SCKS command.
09/17/04   snb     Allow PB entries with empty number field to be read by +CPBR
                   Fix +CFUN response when power up in LPM.
09/03/04   snb     Add +CHLD and +CSIM support.
07/06/04   snb     Add support for personalization PINs for ATCOP and return
                   +CME ERROR if PBM not ready.
05/24/04   snb     Implement DSAT to SIO flow control for PB entries and 
                   correct same for SMS.
01/22/04   snb     Correct +CFUN response for <fun> parms not allowed by spec.
01/21/04   snb     Correct pin state when +CLCK with incorrect password blocks
                   PIN1, correct similar problem for +CPBS, lint cleanup.
01/06/04   snb     Move registration for pin event with GSDI and handling 
                   callback for pin events to common file and add protocol
                   stack re-initialization under feature define.
11/19/03   ar      Use Call Manager query for +CPAS reporting. Add +CME 
                   errors for pkt traffic flow template support. Lint cleanup
11/07/03   snb     Fix out of order responses due to local formatted response
                   buffers.
11/03/03   snb     Check GSM operation slot on IMSI read via +CIMI
10/29/03   snb     Added support for $QCPINSTAT command and for async IMSI 
                   read from either card slot by +CIMI command.
10/13/03   ar      Added support for the +CFUN command.
10/08/03   snb     Set +CMEE power-on value to 2. On power-up return SIM busy
                   in response to AT+CPIN? until GSDI responds with pin state
09/16/03   ar      Adjust wrapper on battery connection status query.
09/05/03   ar      Adjust interface to dsatetsicall_report_clck().
08/19/03   ar      Bypass two params check in +CPIN when PIN1 expected.
08/08/03   ar      Add support for dual slot UIM/GSDI interface.
07/23/03   sb      Thin UI build merge. 
07/22/03   snb     Handle LD record init attempt without dial number reset 
                   ld_rec state.
07/11/03   snb     Use correct PBM device for last dialed list.
06/27/03   snb     Do not unnecesarily allocate dsm buffer in pbm cb.
06/11/03   ar      Change interface for dsatutil_strcmp_pkt_dial_str().
06/09/03   ar      Added dsatetsime_exec_cmer_cmd().
06/02/03   snb     Changed +CBC supported battery connection status values.
05/28/03   sb      Use the old way of getting the battery status for Leopard
05/27/03   snb     Correct response on attempt to delete PB entry from 
                   already empty index.
05/23/03   sb      Use vbatt API (vbatt_connection_status) to read the battery
                   status
04/29/03   snb     Err on attempt to change ME facility pin password when
                   pin is disabled.
03/27/03   ar      Add support for +CKPD and ME unsoliticed result codes.
03/26/03   sb      Don't give MSG_HI on unsupported GSDI event.
03/24/03   ar      Assume missing SIM for PIN initialization
03/17/03   ar      Verify +CPIN parameters are not null.
03/07/03   ar      Remove FEATURE_DATA_ETSI_SUPSERV wrappers
02/26/03   wx      Put dialed number into last dial ph book.
02/19/03   ar      Rearrange FEATURE_ETSI_PBM placements for DSPE build.
02/14/03   wx      Add ABORTING state to the abortable command to 
                   handle the aborting in async command processing.
01/17/03   wx      Added support for dial string modifiers
12/31/02   wx      Added direct dialing implementations
11/21/01   ar      Added support for +CLCK command PIN functions
09/10/02   ar      Wrap GSDI callback change under FEATURE_MMGSDI_<mode>
06/11/02   ar      Report +CSQ bit error rate as not known
03/14/02   ar      Added +CSQ, +CBC, +CPAS, +CPIN support
02/07/02   wx      Use async call to get pin status and to verify pin
11/13/01   wx      Created module.


===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include <stringl/stringl.h>


/* this file should be included only in GSM or WCDMA mode */
#ifdef FEATURE_DSAT_ETSI_MODE

#include <stdio.h>
#include <math.h>
#include <string.h>
#include "dsati.h"
#include "dsatcmif.h"
#include "dsatetsicmif.h"
#include "dsatetsime.h"
#include "dsatetsictab.h"
#include "dsatctab.h"
#include "dsatparm.h"
#include "dsatclienti.h"
#include "vbatt.h"
#include "cm.h"
#include "amssassert.h"
#include "msg.h"

#if defined(T_WINNT)
#error code not present
#endif /* WINNT */

#include "dsm.h"
#include "time_svc.h"
#include "time_tod.h"

#include "dsatvend.h"
#ifdef FEATURE_DATA_PS_EAP
#include "intconv.h"
#include "ps_eap.h"
#include "ps_eap_sim_aka.h"
#include "ps_eap_aka.h"
#include "ps_eap_sim.h"
#endif /* FEATURE_DATA_PS_EAP */
#ifdef FEATURE_MMGSDI_ATCSIM
#include "uim.h"
#endif /* FEATURE_MMGSDI_ATCIM */

#include "dsatme.h"
#include "dsatetsicall.h"

#ifdef FEATURE_DSAT_ETSI_DATA
#include "dsatetsipkt.h"
#endif /* FEATURE_DSAT_ETSI_DATA */

#include "sys_v.h"

#include "uim_v.h"
#include "ps_utils.h"

/* To store alpha numeric equalent of the given number when
  dsatetsime_get_call_alphanumeric_num function is called */

#define DSAT_QCSIMSTAT_URC_ENABLE 1

#define UPPER_BCD(x)  (((x) & 0xF0) >> 4)
#define LOWER_BCD(x)  ( (x) & 0x0F)

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/

/*--------------------------------------------------------------------------
  Debug macro for this module.
---------------------------------------------------------------------------*/

#define DEBUG( x )

/* Macros to ease use of GSDI functions that change name and parms 
   for dual slot use */

#ifdef  FEATURE_MMGSDI   

  #define SET_PIN_STATUS( enable, pin_id, pin_data, client_ref ) \
    if ( enable ) \
    { \
      mmgsdi_result = mmgsdi_session_enable_pin \
                           (dsat_get_current_gw_session_id(FALSE), \
                            pin_id, \
                            pin_data, \
                            dsatme_mmgsdi_req_cb, \
                            (mmgsdi_client_data_type)client_ref); \
    } \
    else \
    { \
      mmgsdi_result = mmgsdi_session_disable_pin \
                           (dsat_get_current_gw_session_id(FALSE), \
                            pin_id, \
                            MMGSDI_PIN_NOT_REPLACED_BY_UNIVERSAL, \
                            pin_data, \
                            dsatme_mmgsdi_req_cb, \
                            (mmgsdi_client_data_type)client_ref); \
    }

  #define UNBLOCK_PIN( slot, pin_id, old_pin_data, new_pin_data, client_ref ) \
    mmgsdi_result = mmgsdi_session_unblock_pin \
                           (dsat_get_current_gw_session_id(FALSE), \
                            pin_id, \
                            old_pin_data, \
                            new_pin_data, \
                            dsatme_mmgsdi_req_cb, \
                            (mmgsdi_client_data_type)client_ref); 

  #define CHANGE_PIN(pin_id, old_pin_data, new_pin_data, client_ref ) \
    mmgsdi_result = mmgsdi_session_change_pin \
                           (dsat_get_current_gw_session_id(FALSE), \
                            pin_id, \
                            old_pin_data, \
                            new_pin_data, \
                            dsatme_mmgsdi_req_cb, \
                            (mmgsdi_client_data_type)client_ref); 

#endif /* FEATURE_MMGSDI */

/* Local variable which stores whether FDN needs to
 * be enabled or disabled after SIM PIN2 verification.
 */
LOCAL boolean dsat_fdn_enable_disable = FALSE;

/*-------------------------------------------------------------------------
    import definitions:
-------------------------------------------------------------------------*/

/*SIM status enable/disable */

extern cb_cmd_e_type cb_expected;

/* variable which stores whether listing of preffered PLMN or
   PLMN list stored on ME need to be aborted(TRUE) or not(FALSE) */
extern boolean dsat_abort_plmn_listing;
/* Hold the variables of the command in execution */
extern dsat_cmd_hdlr_state_s_type dsat_curr_cmd_var;

LOCAL cb_cmd_e_type cb_csim_pending = CB_NONE;

/*-------------------------------------------------------------------------
    Local definitions:
-------------------------------------------------------------------------*/
#define DSAT_TIME_FORMAT_24_HOUR  1
#define DSAT_TIME_FORMAT_12_HOUR  2
#define DSAT_TIME_FORMAT_MAX  7

#define DSAT_DATE_AUX_MODE_YY    1
#define DSAT_DATE_AUX_MODE_YYYY  2

#define DSAT_EAP_AKA 23
#define DSAT_EAP_SIM 18

/* Bit Error Rate Status */
#define DSAT_BER_UNKNOWN     99   /* per 3GPP 27.007  */

/* CBC status codes */
typedef enum {
    BATTSTAT_POWERED = 0,
    BATTSTAT_CHARGING = 1,
    BATTSTAT_NOT_CONNECTED = 2,
    BATTSTAT_MAX
} cbc_code_e_type;


/* CPAS return codes */
typedef enum
{
  CPAS_READY = 0,
  CPAS_RINGING = 3,
  CPAS_CALL_IN_PROGRESS = 4,
  CPAS_MAX
} cpas_code_e_type;

#define CMEC_ACCESS_EMULATION_ONLY 1
#define CMEC_ACCESS_KEYPAD_ONLY    0

#ifdef FEATURE_MMGSDI
/* Supported PLMN  */
#define DSAT_SUPP_PLMN      1
#define DSAT_SUPP_PLMNWACT  1
#define DSAT_SUPP_OPLMNWACT 2
#define DSAT_SUPP_HPLMNWACT 4

#define MMGSDI_IS_FILE_EXIST(sim_filename,file_length) \
  mmgsdi_session_read_cache_file_size(dsat_get_current_gw_session_id(FALSE),sim_filename, file_length)
#endif /* FEATURE_MMGSDI */

/* +CFUN definitions */
/* Allowed syntactically per 27.007 section 8.2, we support a subset */
#define CFUN_MIN_ALLOWED_FUN 0
#define CFUN_MAX_ALLOWED_FUN DSAT_ME_FUNC_MAX  /* spec says upto 127 */

LOCAL dsat_cmd_hdlr_state_s_type dsatetsime_cind_hdlr_state;
boolean dsatetsime_cind_pending[DSAT_SYS_MODEM_AS_ID_MAX];

/*-------------------------------------
  Local variables
---------------------------------------*/
LOCAL cb_cmd_e_type send_cb_pending = CB_NONE;

/* Flag user-initiated GSDI interaction underway */
#define GSDI_STATUS_MASK  0xFF

#ifdef FEATURE_DATA_ETSI_PIN
/*---------------------------------------------------------------------------
  Personal Identity Number definitions
---------------------------------------------------------------------------*/
/* Required code table according to Section 8.3 of 3GPP TS 27.007. */
/* Order of records dictates precedence of reporting.              */
static const struct {
  uint32  req_code;
  char   *pin_text;
  char   *puk_text; 
} cpin_req_table[] =
{
  { (uint32)DSAT_CPIN_SIM_PIN_REQUIRED,         "SIM PIN",       "SIM PUK" },
  { (uint32)DSAT_CPIN_PH_SIM_PIN_REQUIRED,      "PH-SIM PIN",    "PH-SIM PIN" },

#ifdef FEATURE_MMGSDI_PERSONALIZATION
  /* Personalization PIN codes for card in slot1 */
  { (uint32)DSAT_CPIN_PH_FSIM_PIN_REQUIRED,     "PH-FSIM PIN",   "PH-FSIM PUK" },
  { (uint32)DSAT_CPIN_NET_PIN_REQUIRED,         "PH-NET PIN",    "PH-NET PUK" },
  { (uint32)DSAT_CPIN_NETSUB_PIN_REQUIRED,      "PH-NETSUB PIN", "PH-NETSUB PUK" },
  { (uint32)DSAT_CPIN_SP_PIN_REQUIRED,          "PH-SP PIN",     "PH-SP PUK" },
  { (uint32)DSAT_CPIN_CORP_PIN_REQUIRED,        "PH-CORP PIN",   "PH-CORP PUK" },
#endif /* FEATURE_MMGSDI_PERSONALIZATION */
  { (uint32)DSAT_CPIN_SIM_PIN2_REQUIRED,        "SIM PIN2",      "SIM PUK2" },
  { (uint32)DSAT_CPIN_NONE_REQUIRED,            "READY",         "READY" },
  { (uint32)DSAT_CPIN_MAX,              NULL,            NULL }
};
  

/* Indicates if SIM PIN2 status should be reported by +CPIN command:
   per recommendation in 3GPP TS 27.007 section 8.3 "SIM PIN2" and "SIM PUK2"
   codes should be returned only when the last executed command resulted in 
   PIN2 authentication failure */
#else
  /* Allow consistent interfaces in non-PIN builds */
  #define gsdi_slot_id_type  byte

  #define GSDI_GSM_SLOT_1_MASK MMGSDI_SLOT_1
#endif /* FEATURE_DATA_ETSI_PIN */

#ifdef FEATURE_MMGSDI_ATCSIM
#define INVALID_CHAN_ID -1
#define DEFAULT_CHAN_ID 0
/* turn off lint info "local macro not referenced",
   macros not currently used are defined for future convenience */
/* lint -e750 */
/*--- +CSIM constants and variables ---*/
/* APDU sizes as octets */
#define CSIM_MAX_CMD_APDU_SIZE 260
#define CSIM_MAX_RSP_APDU_SIZE 257
/* +CSIM Command */
/* Command APDU can be 5..260 octets in length. Each octet requires two hex
   characters to specify. In command line "AT+CSIM=<len>,\"<hex_string>\"\x00"
   where len is 2 or 3 chars this requires 535 MAX_LINE_SIZE */
/* Command APDU limits when expressed as HEX char pairs */
#define MAX_CSIM_APDU_LEN 520
#define MIN_CSIM_APDU_LEN 10

/* Byte offsets within APDU */
#define CSIM_APDU_CLASS_OFS 0
#define CSIM_APDU_INSTR_OFS 1
#define CSIM_APDU_P1_OFS    2
#define CSIM_APDU_P2_OFS    3
#define CSIM_APDU_P3_OFS    4
#define CSIM_APDU_DATA_OFS  5

/* SW1 return codes for commands correctly executed */
#define CSIM_SW1_NORMAL           0x90
#define CSIM_SW1_NORMAL_CMD       0x91
#define CSIM_SW1_SIM_DATA_DL_ERR  0x9E
#define CSIM_SW1_RESP_DATA        0x9F
#define CSIM_SW1_USIM_RESP_DATA   0x61
#define CSIM_SW1_USIM_RESP_DATA_IMPLICIT   0x6C

/* SW1 return codes for commands postponed */
#define CSIM_SW1_SATK_BUSY        0x93

/* SW return codes, memory management */
#define CSIM_SW1_MEM_MGT          0x92
#define CSIM_SW2_MEM_PROBLEM      0x40

/* SW return codes, referencing management */
#define CSIM_SW1_REF_MGT          0x94
#define CSIM_SW2_REF_NO_EF        0x00
#define CSIM_SW2_REF_OOR          0x02
#define CSIM_SW2_REF_NOT_FOUND    0x04
#define CSIM_SW2_REF_X_CMD        0x08

/* SW return codes, security management */
#define CSIM_SW1_SEC_MGT          0x98
#define CSIM_SW2_SEC_NO_CHV       0x02
#define CSIM_SW2_SEC_X_ACCESS     0x04
#define CSIM_SW2_SEC_X_CHV_STAT   0x08
#define CSIM_SW2_SEC_X_INVAL_STAT 0x10
#define CSIM_SW2_SEC_X_VER_BLK    0x40
#define CSIM_SW2_SEC_MAX          0x50

/* SW return codes, application independent */
#define CSIM_SW1_ERR_P3           0x67
#define CSIM_SW1_ERR_P1_P2        0x6B
#define CSIM_SW1_ERR_INSTR_CODE   0x6D
#define CSIM_SW1_ERR_INSTR_CLASS  0x6E
#define CSIM_SW1_ERR_UNKNOWN      0x6F
/* lint +e750 */
#endif /* FEATURE_MMGSDI_ATCSIM */

/*-------------------------------------------------------------------------
    Protypes for local functions:
-------------------------------------------------------------------------*/

/*--------            gsdi function return status handlers       --------*/

LOCAL void me_copy_pin
(
  byte * array,
  byte * string
);

#ifdef FEATURE_DATA_ETSI_PIN
#ifdef FEATURE_MMGSDI
LOCAL dsat_result_enum_type me_process_cpin_state
(
  const tokens_struct_type *tok_ptr,
  dsm_item_type *res_buff_ptr
);

LOCAL dsat_result_enum_type me_cpin_unblock_pin
(
  const tokens_struct_type *tok_ptr,
  dsm_item_type *res_buff_ptr,
  const mmgsdi_pin_enum_type pin_id,
  const mmgsdi_slot_id_enum_type slot_id
);
#endif /* FEATURE_MMGSDI */

#endif /* FEATURE_DATA_ETSI_PIN */

LOCAL boolean req_mmgsdi_sim_read
(
  cb_cmd_e_type cb_cmd
);

LOCAL boolean req_mmgsdi_sim_write
(
  cb_cmd_e_type cb_cmd,
  byte * buffer,
  uint8  buflen
);

LOCAL dsat_result_enum_type etsime_decode_imsi
(
  ds_at_sim_info_type * imsi_ptr,       /* Structure of GSDI data */
  byte * imsi_buf                 /* Output string buffer */
);

LOCAL dsat_result_enum_type etsime_msisdn_read
(
  dsm_item_type *res_buff_ptr
);

#ifdef FEATURE_MMGSDI_ATCSIM

LOCAL dsat_result_enum_type etsime_report_csim
(
  uint32 num_bytes,
  byte *resp_apdu
);

LOCAL dsat_result_enum_type etsime_report_crsm
(
  uint8 sw1,
  uint8 sw2,
  uint32 num_bytes,
  byte *resp_apdu
);
#endif /* FEATURE_MMGSDI_ATCSIM */

LOCAL word etsime_get_battery_level
(
  word scale
);

/* local function to get Real Time Clock settings */
LOCAL dsat_result_enum_type etsime_get_real_time_clock
(
  dsm_item_type *res_buff_ptr,
  const byte    *cmd_name      /*  Ptr to cmd name  */
);


extern boolean dsatetsime_set_time_zone_into_nv(sys_time_zone_type time_zone);
boolean dsatetsime_get_time_zone_from_nv(sys_time_zone_type *time_zone);

#ifdef FEATURE_DATA_PS_EAP
LOCAL dsat_result_enum_type etsime_send_eap_packet
(
  dsat_num_item_type eap_method, 
  dsm_item_type* eap_pkt_data 
);

LOCAL dsat_result_enum_type etsime_start_and_send_eap_packet
(
  dsat_num_item_type eap_method,
  dsm_item_type* eap_pkt_data
);


LOCAL boolean dsatetsime_eap_task_sw_cb
(
  eap_sim_aka_task_srv_req_cb_type req_cb,
  void * user_data
);
LOCAL void dsatetsime_eap_result_ind_cb
(
  eap_handle_type       handle, 
  void                 *result_ind_user_data,
  eap_result_enum_type  result,
  uint8                *pre_master_key,
  uint16                pre_master_key_len
);

LOCAL void dsatetsime_eap_supp_result_ind_cb
(
  eap_handle_type       handle,
    /**< EAP handle */
  void                 *supp_result_ind_user_data,
    /**< Supplementary result indication user data */
  eap_result_enum_type  result,
    /**< Indicates auth failure/success */
  eap_sim_aka_supp_auth_mask_type
                        supp_auth_info_mask,
    /**< Supplementary auth info mask */
  uint16                supp_auth_info_size,
    /**< Supplementary auth info size */
  void                 *supp_auth_info_ptr
    /**< Supplementary auth info */
);

LOCAL void dsatetsime_eap_trp_tx_cb
(
  void           * user_data, 
  dsm_item_type ** eap_resp_pkt
);
#endif /* FEATURE_DATA_PS_EAP */
/*-------------------------------------------------------------------------
            Constants:
-------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------
            Function Definitions: Public functions
-------------------------------------------------------------------------*/

/*--------          Functions exported outside of ATCOP          --------*/

/*===========================================================================

FUNCTION DSATETSIME_EXEC_CIND_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +CIND command.
  This command reports the status of ME indicators.
  
  Note: +CIND read & test syntax is permitted in RESTRICTED mode.
  
DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :      if there was any problem in executing the command
    DSAT_OK : if it is a success.
    DSAT_CMD_ERR_RSP: if +CME ERROR is being generated

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatetsime_exec_cind_cmd
(
  dsat_mode_enum_type mode,                /*  AT command mode:            */
  const dsati_cmd_type *parse_table,       /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,       /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr              /*  Place to put response       */
)
{
  dsat_result_enum_type    result  = DSAT_OK;
  sys_modem_as_id_e_type   subs_id = dsat_get_current_subs_id(FALSE);

  /* process the WRITE command */
  if( tok_ptr->op == (NA|EQ|AR) )
  {
    if ( FALSE == dsatcmdp_restricted_commands[subs_id] )
    {
      /* Report +CME error on unsupported write syntax */
      dsatme_set_cme_error (DSAT_CME_OP_NOT_SUPPORTED, res_buff_ptr);
      result = DSAT_CMD_ERR_RSP;
    }
    else
    {
      result = DSAT_ERROR;
    }
  }
      
  /* process the READ command */
  else if (tok_ptr->op == (NA|QU))
  {
    /* Preserve command handler state */
    DSAT_SAVE_HDLR_STATE( dsatetsime_cind_hdlr_state );
    dsatetsime_cind_pending[subs_id] = TRUE;
    
    /* Kickoff asynchronous processing */
    result = dsatetsicmif_get_cm_call_state ();
  }
  
  /* process the TEST command */
  else if (tok_ptr->op == (NA|EQ|QU))
  {
    result = dsatparm_exec_param_cmd ( mode,
                                       parse_table,
                                       tok_ptr,
                                       res_buff_ptr);
  }

  /* command does not make sense */
  else
  {
    result = DSAT_ERROR;
  }
  
  return result;
} /* dsatetsime_exec_cind_cmd */



/*===========================================================================

FUNCTION DSATETSIME_GENERATE_CIND_RESPONSE

DESCRIPTION
  This function generates the response for the +CIND command on return from
  asynchronous processing.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK :         if it is a success.
    DSAT_CMD_ERR_RSP: if +CME ERROR is being generated

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatetsime_generate_cind_response
(
  ds_at_cm_call_event_type *call_event_ptr, /* Call event pointer */
  sys_modem_as_id_e_type    subs_id
)
{
  dsat_result_enum_type result = DSAT_OK;
  dsm_item_type *res_buff_ptr = NULL;

  /* Update battey level */
  (void)dsatetsime_change_indicator_state(
    DSAT_CIND_INDEX_BATTCHG,
    (uint8)etsime_get_battery_level( DSAT_CIND_MAX_BATTCHG ),
    subs_id );

  /* Update call indicator */
  (void)dsatetsime_change_indicator_state(
    DSAT_CIND_INDEX_CALL,
    ((call_event_ptr->event_info.dsat_cm_call_info_list.number_of_active_calls > 0)? 1 : 0),
    subs_id );

  /* Generate response */
  res_buff_ptr = dsat_dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL, FALSE);
  result = dsatparm_exec_param_cmd (dsatetsime_cind_hdlr_state.mode,
                                    dsatetsime_cind_hdlr_state.parse_table,
                                    dsatetsime_cind_hdlr_state.tok_ptr,
                                    res_buff_ptr);
  
  if ( DSAT_OK != result )
  {
    DS_AT_MSG0_ERROR("Could not generate +CIND response");
    dsatme_set_cme_error(DSAT_CME_PHONE_FAILURE,
                             res_buff_ptr);
    result = DSAT_CMD_ERR_RSP;
  }
  dsatetsime_cind_pending[subs_id] = FALSE;
  /* Send response to TE (no URC supported) */
  dsatcmdp_handle_async_cmd_rsp (res_buff_ptr,
                                 DSAT_COMPLETE_RSP);

  memset ((void*)&dsatetsime_cind_hdlr_state, 0x0,
          sizeof(dsatetsime_cind_hdlr_state));
  return result;
} /* dsatetsime_generate_cind_response */

/*===========================================================================

FUNCTION DSATETSIME_REPORT_CIEV_RESULT

DESCRIPTION
  This function reports +CIEV URC.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatetsime_report_ciev_result
(
  uint8                   indicator,
  dsat_ind_val_type       value,
  sys_modem_as_id_e_type  subs_id
)
{
  dsat_num_item_type cmer_val0;
  dsat_num_item_type cmer_val3;

  if (!IS_VALID_SUBS_ID(subs_id))
  {
    return;
  }

  /*----------------------------------------------------------------------- 
  Check if a free buffer is avalable. If none, require a 
  new buffer space. 
  -----------------------------------------------------------------------*/ 
  /*Update local CMER vals*/
  cmer_val0 = (dsat_num_item_type)
        dsatutil_get_val(DSATETSI_EXT_ACT_CMER_ETSI_IDX,0,0,NUM_TYPE);
  cmer_val3 = (dsat_num_item_type)
         dsatutil_get_val(DSATETSI_EXT_ACT_CMER_ETSI_IDX,0,3,NUM_TYPE);

  if ((dsatetsime_cind_pending[subs_id] == FALSE)&&
                     (1 == cmer_val3) && (0 != cmer_val0) )
  { 
    dsm_item_type *ind_ptr = NULL; 
    ind_ptr = dsat_dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL, TRUE);
    if (NULL != ind_ptr)
    { 
      /*----------------------------------------------------------------------- 
      Stores the echo character into the buffer. 
      -----------------------------------------------------------------------*/ 
      ind_ptr->used = (word)snprintf ( (char*)ind_ptr->data_ptr,ind_ptr->size, 
      "+CIEV: %d,%d",indicator,value); 
      dsatcmdp_send_urc_per_subs(subs_id, ind_ptr, DSAT_COMPLETE_RSP); 
    }
  }

  return;
} /* dsatetsime_report_ciev_result */


/*===========================================================================

FUNCTION DSATETSIME_EXEC_CMEC_CMD

DESCRIPTION

  This function takes the result from the command line parser and
  executes it. It executes the +CMEC command.  This command selects
  the equipment which operates the keypad, writes to the display, and
  sets indicators.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :      if there was any problem in executing the command
    DSAT_OK :         if it is a success.
    DSAT_CMD_ERR_RSP: if +CME ERROR is being generated

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsime_exec_cmec_cmd
(
  dsat_mode_enum_type mode,                /*  AT command mode:            */
  const dsati_cmd_type *parse_table,       /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,       /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr              /*  Place to put response       */
)
{

  dsat_result_enum_type result = DSAT_OK;

  /* process the WRITE command */
  if(tok_ptr->op == (NA|EQ|AR))
  {
    if (DSAT_OK !=
        dsatparm_exec_param_cmd (mode, parse_table, tok_ptr, res_buff_ptr))
    {
      result = DSAT_ERROR;
    }
  }

  /* process the READ or TEST command */
  else if ( (tok_ptr->op == (NA|EQ|QU)) ||
            (tok_ptr->op == (NA|QU)) )
  {
    if (DSAT_OK !=
        dsatparm_exec_param_cmd (mode, parse_table, tok_ptr, res_buff_ptr))
    {
      result = DSAT_ERROR;
    }
  }
      
  /* command does not make sense */
  else
  {
    result = DSAT_ERROR;
  }
    
  return result;

} /* dsatetsime_exec_cmec_cmd () */

/*===========================================================================

FUNCTION DSATETSIME_EXEC_CMER_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +CMER command.
  This command control sending of unsolicited result codes from TA to TE
  in the case of key pressings.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :      if there was any problem in executing the command
    DSAT_OK :         if it is a success.
    DSAT_CMD_ERR_RSP: if +CME ERROR is being generated

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsime_exec_cmer_cmd
(
  dsat_mode_enum_type mode,                /*  AT command mode:            */
  const dsati_cmd_type *parse_table,       /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,       /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr              /*  Place to put response       */
   )
{
  dsat_result_enum_type result = DSAT_OK;

  /* process the WRITE, READ, TEST syntax */
  if ((tok_ptr->op == (NA|EQ|AR)) ||
      (tok_ptr->op == (NA|EQ|QU)) ||
      (tok_ptr->op == (NA|QU)) ||
      (tok_ptr->op == (NA))) 
  {
     if (DSAT_OK !=
         dsatparm_exec_param_cmd (mode, parse_table, tok_ptr, res_buff_ptr))
     {
        dsatme_set_cme_error (DSAT_CME_OP_NOT_SUPPORTED, res_buff_ptr);
        result = DSAT_CMD_ERR_RSP;
     }
  }

  /* command does not make sense */
  else
  {
    result = DSAT_ERROR;
  }

  return result;
} /* dsatetsime_exec_cmer_cmd() */

/*--------     AT mobile equipment cmd handler functions         --------*/

/*===========================================================================

FUNCTION DSATETSIME_EXEC_CPAS_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +CPAS command.
  The result is in res_buff_ptr, reporting phone activity status

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :    if there was any problem in executing the command
    DSAT_OK :         if it is a success and execution completed.
    DSAT_ASYNC_CMD :  if it is a success and awaiting completion.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatetsime_exec_cpas_cmd
(
  dsat_mode_enum_type mode,                /*  AT command mode:            */
  const dsati_cmd_type *parse_table,       /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,       /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr              /*  Place to put response       */
)
{
  dsat_result_enum_type result  = DSAT_OK;
  sys_modem_as_id_e_type subs_id = dsat_get_current_subs_id(FALSE);

  /* process command */
  if(tok_ptr->op == NA)
  {
    /* Check call status */
    if ( DSAT_ONLINE_CMD == mode )
    {
      result = dsatetsime_report_cpas (mode, NULL, res_buff_ptr, subs_id);
    }
    else
    {
      /* Launch asynchronous CM query for call information.     */
      SET_PENDING(DSATETSI_EXT_ACT_CPAS_ETSI_IDX ,0, DSAT_PENDING_TRUE)
      if ( DSAT_ASYNC_CMD != (result = dsatetsicmif_get_cm_call_state()) )
      {
        SET_PENDING(DSATETSI_EXT_ACT_CPAS_ETSI_IDX ,0, DSAT_PENDING_FALSE);
        DS_AT_MSG0_ERROR("Failed to query CM call info list");
        dsatme_set_cme_error(DSAT_CME_PHONE_FAILURE, res_buff_ptr);
        result = DSAT_CMD_ERR_RSP;
      }
    }
  }
  /* process the TEST command */
  else if (tok_ptr->op == (NA|EQ|QU))
  {
    /* Generate supported values response */
    res_buff_ptr->used +=
      (word)snprintf((char*)&res_buff_ptr->data_ptr[res_buff_ptr->used], 
                           res_buff_ptr->size - res_buff_ptr->used,
                           "+CPAS: (0,3,4)" );
  }
    
  /* command does not make sense */
  else
  {
    result = DSAT_ERROR;
  }
  
  return result;
} /* dsatetsime_exec_cpas_cmd */


/*===========================================================================

FUNCTION  DSATETSIME_REPORT_CPAS

DESCRIPTION
  This function generates the query response for the +CPAS read command.
  Data is extracted from the data returned from Call Manager and formatted.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK :       if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsime_report_cpas
(
  const dsat_mode_enum_type  mode,          /*  AT command mode:            */
  ds_at_cm_call_event_type  *call_event_ptr, /* Call event pointer */
  dsm_item_type             *res_buff_ptr,  /*  Place to put response       */
  sys_modem_as_id_e_type     subs_id
)
{
  dsat_result_enum_type result = DSAT_OK;
  cpas_code_e_type call_state = CPAS_READY;
  boolean async_rsp = FALSE;

  /* Check for asych response */
  if ( NULL == res_buff_ptr )
  {
    res_buff_ptr = dsat_dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL, FALSE);
    async_rsp = TRUE;
  }
    
  /* Check for data from CM; variable initialized in cmd handler */
  if ((NULL != call_event_ptr) && (0 != call_event_ptr->event_info.dsat_cm_call_info_list.number_of_active_calls))
  {
    /* Look for state of active calls.                */
    /* Report "call in process" state over "ringing". */
    uint8 i;
    boolean done = FALSE;
        
    for (i=0; (i < call_event_ptr->event_info.dsat_cm_call_info_list.number_of_active_calls) && !done; i++)
    {
      switch (call_event_ptr->event_info.dsat_cm_call_info_list.info[i].call_state)
      {
      case CM_CALL_STATE_ORIG:
      case CM_CALL_STATE_CONV:
        call_state = CPAS_CALL_IN_PROGRESS;
        done = TRUE;
        break;    

      case CM_CALL_STATE_INCOM:
        call_state = CPAS_RINGING;
        break;    

      default:
        break; /* Make Lint happy */
      }
    } /* for */
  } /* if */
  else
  {
    /* If in online-data mode, report call in progress */
    if ( DSAT_ONLINE_CMD == mode )
    {
      call_state = CPAS_CALL_IN_PROGRESS;
    }
  }

  res_buff_ptr->used +=
    (word) snprintf ( (char*)&res_buff_ptr->data_ptr[res_buff_ptr->used], 
                            res_buff_ptr->size - res_buff_ptr->used,"+CPAS: %d",
                            call_state );

  if ( async_rsp )
  {
    dsatcmdp_handle_async_cmd_rsp(res_buff_ptr,DSAT_COMPLETE_RSP);
  }
  
  return result;
}  /* dsatetsime_report_cpas */

/*===========================================================================

FUNCTION DSATETSIME_EXEC_CSQ_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +CSQ command.
  The result is in res_buff_ptr, reporting received signal strength 
  indication and channel bit error rate.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :    if there was any problem in executing the command
    DSAT_OK :       if it is a success.
    DSAT_ASYNC_CMD: if this is function invokes an async command

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatetsime_exec_csq_cmd
(
  dsat_mode_enum_type mode,                /*  AT command mode:            */
  const dsati_cmd_type *parse_table,       /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,       /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr              /*  Place to put response       */
)
{

  dsat_result_enum_type result = DSAT_OK;
  uint8 rssi_code = 0;
  uint8 ber_code = DSAT_BER_UNKNOWN;
  dsatcmif_servs_state_ms_info  *ph_ss_ms_val = NULL;
#if defined (FEATURE_DSAT_DEV_CMDS) && defined (FEATURE_UMTS_REPORT_ECIO)
  dsati_mode_e_type     current_mode;

  current_mode = dsatcmdp_get_current_mode();
#endif /* defined (FEATURE_DSAT_DEV_CMDS) && defined (FEATURE_UMTS_REPORT_ECIO) */
  
   ph_ss_ms_val = dsat_get_base_addr_current_subs(DSAT_SS_PH_MS_VALS);

  /* process command */
  if(tok_ptr->op == NA)
  {
    /* Convert RSSI value to reporting range */
    if(dsatcmdp_get_current_mode() == DSAT_MODE_TDS)
    {
       uint16 rscp = 0;
       if(ph_ss_ms_val->dsatcmif_signal_reporting.rscp < 0)
       {
         rscp = ph_ss_ms_val->dsatcmif_signal_reporting.rscp * -1;
       }
       else
       {
          rscp = ph_ss_ms_val->dsatcmif_signal_reporting.rscp;
       }
       rssi_code = dsatetsime_convert_rssi(rscp,
                         RSSI_TDS_TOOHI_CODE );   
    }
    else
    {
      rssi_code = dsatetsime_convert_rssi(
                         ph_ss_ms_val->dsatcmif_signal_reporting.rssi,
                         DSAT_CSQ_MAX_SIGNAL );
    }
    

    if ( DSAT_BER_NO_SIGNAL != ph_ss_ms_val->dsatcmif_signal_reporting.bit_err_rate)
    {
      ber_code = ph_ss_ms_val->dsatcmif_signal_reporting.bit_err_rate;
    }

    /* Generate supported values response */
    res_buff_ptr->used = (word) snprintf ((char*)res_buff_ptr->data_ptr, 
                                                res_buff_ptr->size,
                                                "%s: %d,%d", 
                                                tok_ptr->name, 
                                                rssi_code,
                                                ber_code
                                                );
#if defined (FEATURE_DSAT_DEV_CMDS) && defined (FEATURE_UMTS_REPORT_ECIO)
    /* ECIO value will be reported by higher layer only 
     * when FEATURE_UMTS_REPORT_ECIO is enabled. ECIO is not valid
     * for GSM and CM will report a MAX value equivalent to 63.
     */
    if ( (  DSAT_MODE_WCDMA == current_mode )
         && ( 0 == dsatutil_strcmp_ig_sp_case((byte*)"$CSQ", (const byte *)parse_table->name) )
         && ( ph_ss_ms_val->dsatcmif_signal_reporting.ecio < DSAT_ECIO_NO_SIGNAL ) )
    {
      res_buff_ptr->used = (word) snprintf (
                                    (char*)res_buff_ptr->data_ptr, 
                                    res_buff_ptr->size,
                                    "%s,%d", 
                                    res_buff_ptr->data_ptr,
                                    (-1)*(ph_ss_ms_val->dsatcmif_signal_reporting.ecio/2)
                                    ); 
    }
#endif /* defined (FEATURE_DSAT_DEV_CMDS) && defined (FEATURE_UMTS_REPORT_ECIO) */
  }

  /* process the TEST command */
  else if (tok_ptr->op == (NA|EQ|QU))
  {
    /* Generate supported values response */
    if(dsatcmdp_get_current_mode() == DSAT_MODE_TDS)
    {
      res_buff_ptr->used = (word) snprintf ((char*)res_buff_ptr->data_ptr, 
                                                 res_buff_ptr->size,
                                                 "%s: %s,%s", 
                                                 parse_table->name, 
                                                 "(100-191,199)",
                                                 "(0-7,99)"
                                                 );
    }
    else
    {
    res_buff_ptr->used = (word) snprintf ((char*)res_buff_ptr->data_ptr, 
                                                 res_buff_ptr->size,
                                                 "%s: %s,%s", 
                                                 parse_table->name, 
                                                 "(0-31,99)",
                                                 "(0-7,99)"
                                                 );
    }
#if defined (FEATURE_DSAT_DEV_CMDS) && defined (FEATURE_UMTS_REPORT_ECIO)

    if ( 0 == dsatutil_strcmp_ig_sp_case((byte*)"$CSQ", (const byte *)parse_table->name) )
    {
      res_buff_ptr->used = (word) snprintf ((char*)res_buff_ptr->data_ptr, 
                                                   res_buff_ptr->size,
                                                   "%s,%s", 
                                                   res_buff_ptr->data_ptr,
                                                   "(-31 - 0)"
                                                   ); 

    }
#endif /* defined (FEATURE_DSAT_DEV_CMDS) && defined (FEATURE_UMTS_REPORT_ECIO) */
  }

  /* command does not make sense */
  else
  {
    result = dsat_send_cme_error(DSAT_CME_OP_NOT_ALLOWED);
  }

  return result;
} /* dsatetsime_exec_csq_cmd */


/*===========================================================================

FUNCTION DSATETSIME_EXEC_CBC_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +CBC command.
  The result is in res_buff_ptr, reporting battery condition status
  and charge level.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :    if there was any problem in executing the command
    DSAT_OK :       if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatetsime_exec_cbc_cmd
(
  dsat_mode_enum_type mode,                /*  AT command mode:            */
  const dsati_cmd_type *parse_table,       /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,       /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr              /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  cbc_code_e_type battstat = BATTSTAT_MAX;  /* make LINT happy */
  word battlvl = etsime_get_battery_level( 100 );

  /* process command */
  if(tok_ptr->op == NA)
  {
    /* Use the old approach in Leopard builds */
    if (VBATT_SCALED_MIN < battlvl)
    {
      battstat = BATTSTAT_POWERED;
    }
    else
    {
      battstat = BATTSTAT_NOT_CONNECTED;
    }
    res_buff_ptr->used = (word) snprintf ((char*)res_buff_ptr->data_ptr, 
                                                res_buff_ptr->size,
                                                "%s: %d,%d",
                                                "+CBC",
                                                battstat,
                                                battlvl);
  }

  /* process the TEST command */
  else if (tok_ptr->op == (NA|EQ|QU))
  {
    /* Generate supported values response */
    res_buff_ptr->used = (word) snprintf ((char*)res_buff_ptr->data_ptr,
                                                res_buff_ptr->size,
                                                "%s: %s,%s",
                                                "+CBC",
                                                "(0-3)",
                                                "(0-100)");
  }

  /* command does not make sense */
  else
  {
    result = DSAT_ERROR;
  }

  return result;
} /* dsatetsime_exec_cbc_cmd */

#ifdef FEATURE_DATA_ETSI_PIN

/*===========================================================================

FUNCTION DSATETSIME_EXEC_CPIN_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +CPIN command.
  This set commands sends to the ME a password which is necessary before 
  operation.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :      if there was any problem in executing the command
    DSAT_OK :         if it is a success.
    DSAT_CMD_ERR_RSP: if +CME ERROR is being generated
    DSAT_ASYNC_CMD :  excuting the action command and waiting for the
                      callback function be called.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatetsime_exec_cpin_cmd
(
  dsat_mode_enum_type mode,                /*  AT command mode:            */
  const dsati_cmd_type *parse_table,       /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,       /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr              /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  char  * code_string = NULL;
  boolean hit = FALSE;
  word index;
  char *c_ptr;
  dsat_apps_id_e_type apps_id = dsat_get_current_gw_apps_id(TRUE);
  dsatme_mmgsdi_state_ms_slot_info *me_ms_slot_val = NULL;
  dsatme_mmgsdi_state_ms_info  *me_ms_val = NULL;

  /* process command */
  if(tok_ptr->op == (NA|EQ|AR))
  {
    if ( (!IS_VALID_GW_APPS_ID(apps_id)) ||
         (!IS_VALID_SLOT_ID(dsat_qcsimapp_info[apps_id].slot_id)) )
    {
      me_ms_slot_val = dsat_get_base_addr_current_slot();
      dsatme_set_cme_error(me_ms_slot_val->dsat_pin_error, res_buff_ptr);
      return DSAT_CMD_ERR_RSP;
    }

    me_ms_val = dsat_get_base_addr_current_session();

    /* Verify first string provided & not empty */
    if (!VALID_TOKEN(0) ||
        ('"' == tok_ptr->arg[0][0] && '"' == tok_ptr->arg[0][1]))
    {
      DS_AT_MSG0_HIGH("Null PIN string invalid");
      return DSAT_ERROR;
    }

    /* Verify second string provided & not empty when appropriate: */
    /* if SIM PIN is blocked OR */
    if ( IS_PUK_STATE(DSAT_CPIN_SIM_PIN_REQUIRED) ||

    /* if SIM PIN2 is blocked AND not awaiting SIM PIN,SIM2 PIN,PERS PIN OR */
        (IS_PUK_STATE(DSAT_CPIN_SIM_PIN2_REQUIRED) && 
         !(me_ms_val->dsat_pin_required & (  (uint32)DSAT_CPIN_SIM_PIN_REQUIRED 
#ifdef FEATURE_MMGSDI_PERSONALIZATION
                                | (uint32)DSAT_CPIN_NET_PIN_REQUIRED 
                                | (uint32)DSAT_CPIN_NETSUB_PIN_REQUIRED 
                                | (uint32)DSAT_CPIN_SP_PIN_REQUIRED 
                                | (uint32)DSAT_CPIN_CORP_PIN_REQUIRED 
                                | (uint32)DSAT_CPIN_PH_FSIM_PIN_REQUIRED
#endif /* FEATURE_MMGSDI_PERSONALIZATION */
        ))) )
    {
      if ( !VALID_TOKEN(1) ||
           ('"' == tok_ptr->arg[1][0] && '"' == tok_ptr->arg[1][1]) )
      {
        DS_AT_MSG0_HIGH("Null new PIN string invalid");
        return DSAT_ERROR;
      }
    }
    
    /*---------------------------------------------------------------
    Processing for a command with an argument (WRITE command)
    ---------------------------------------------------------------*/
    result = dsatparm_exec_param_cmd(
                                     mode,
                                     parse_table,
                                     tok_ptr,
                                     res_buff_ptr
                                    );

    if (DSAT_OK == result)
    {
      /* Report error if no PIN required */
      /* Must use exact match to enum here due to bitfield */
      if ((uint32)DSAT_CPIN_NONE_REQUIRED == me_ms_val->dsat_pin_required)
      {
        dsatme_set_cme_error(DSAT_CME_OP_NOT_ALLOWED, res_buff_ptr);
        result = DSAT_CMD_ERR_RSP;
      }
      else
      {
        /* Verify allowed digits in PIN and PUK */
        for(index = 0; index < 2 && VALID_TOKEN(index); index++)
        {
          c_ptr = (char *)dsatutil_get_val(
            DSATETSI_EXT_ACT_CPIN_ETSI_IDX,apps_id,index,MIX_STR_TYPE);
          while ( '\0' != *c_ptr )
          {
            /* Report error if char is not numeric */
            if (!(('0' <= *c_ptr) && ('9' >= *c_ptr)))
            { 
              DS_AT_MSG0_HIGH("PIN char outside range [0-9]");
              return DSAT_ERROR;
            }
            c_ptr++;
          }
        }
        c_ptr = NULL;
        /* Process arguments based on PIN state bit array */
        result = me_process_cpin_state (tok_ptr, res_buff_ptr);
      }
    }
  }

  /* process the READ command */
  else if (tok_ptr->op == (NA|QU))
  {
    if ( (!IS_VALID_GW_APPS_ID(apps_id)) ||
         (!IS_VALID_SLOT_ID(dsat_qcsimapp_info[apps_id].slot_id)) )
    {
      me_ms_slot_val = dsat_get_base_addr_current_slot();
      dsatme_set_cme_error(me_ms_slot_val->dsat_pin_error, res_buff_ptr);
      return DSAT_CMD_ERR_RSP;
    }

    me_ms_val = dsat_get_base_addr_current_session();

    /* Check for SIM/PIN error */
    if (DSAT_CME_NO_ERROR != me_ms_val->dsat_pin_error)
    {
      dsatme_set_cme_error(me_ms_val->dsat_pin_error, res_buff_ptr);
      me_ms_val->dsat_mask_pin2_state = TRUE;
      result = DSAT_CMD_ERR_RSP;
    }
    else
    {
      /* To mask bits related to SIM PIN2 */

  #define PIN2_MASK DSAT_CPIN_SIM_PIN2_REQUIRED

      /* To save actual pin2 state during manipulation */
      uint32 previous_pin_reqd[] = {
                 (uint32)DSAT_CPIN_NONE_REQUIRED,
                 (uint32)DSAT_CPIN_NONE_REQUIRED,
                 (uint32)DSAT_CPIN_NONE_REQUIRED
      };
  uint32 previous_pin_blkd[] = {
                 (uint32)DSAT_CPIN_NONE_REQUIRED,
                 (uint32)DSAT_CPIN_NONE_REQUIRED,
                 (uint32)DSAT_CPIN_NONE_REQUIRED
      };

      /* Check for SIM_INIT_COMPLETE if there in no other PIN reqd*/
      if((!me_ms_val->dsat_sim_init_comp) && 
              (me_ms_val->dsat_pin_required == (uint32)DSAT_CPIN_NONE_REQUIRED) )
      {
        DS_AT_MSG0_HIGH("Sim Init Complete has not happened");
	me_ms_val->dsat_mask_pin2_state = TRUE;
        return dsat_send_cme_error(DSAT_CME_SIM_BUSY);
      }
      /* Save actual pin2 state and mask any pin2 bits set */
      if ( me_ms_val->dsat_mask_pin2_state )
      {
        previous_pin_reqd[apps_id] = me_ms_val->dsat_pin_required;
        previous_pin_blkd[apps_id] = me_ms_val->dsat_pin_blocked;
        me_ms_val->dsat_pin_required &= ~(uint32)PIN2_MASK;
        me_ms_val->dsat_pin_blocked  &= ~(uint32)PIN2_MASK;
      }
      /* Search the table for the response code/text */
      index = 0;
      do
      {
        hit = (cpin_req_table[index].req_code ==
             ((uint32)me_ms_val->dsat_pin_required & cpin_req_table[index].req_code));

        /* If PIN is blocked use PUK text from table else PIN text */
        code_string = ((uint32)me_ms_val->dsat_pin_blocked & 
                       cpin_req_table[index].req_code) ?
          cpin_req_table[index].puk_text : cpin_req_table[index].pin_text;

        index++;
      } while (!hit && (code_string != NULL));

      if ( me_ms_val->dsat_mask_pin2_state )
      {
        /* Restore actual pin state */
        me_ms_val->dsat_pin_required = previous_pin_reqd[apps_id];
        me_ms_val->dsat_pin_blocked = previous_pin_blkd[apps_id];
      }
      else
      {
        /* 3GPP TS 27.007 section 3.8 recommendeds "SIM PIN2" and "SIM PUK2" 
           be returned only when the last executed command resulted in PIN2 
           authentication failure: Once PIN2 required info has been displayed
           once, don't show it any more */
        me_ms_val->dsat_mask_pin2_state = TRUE;
      }
      if(code_string != NULL)
      {
        res_buff_ptr->used = (word) snprintf ((char*)res_buff_ptr->data_ptr,
                                                  res_buff_ptr->size,
                                                  "%s: %s",
                                                  "+CPIN",
                                                  code_string);
      }else
      {
        DS_AT_MSG0_HIGH("Code string is NULL");
        return DSAT_ERROR;
      }
    }
  }

  /* process the TEST command */
  else if (tok_ptr->op == (NA|EQ|QU))
  {
    /* Do nothing; return success */
  }

  /* command does not make sense */
  else
  {
    result = DSAT_ERROR;
  }
  return result;
} /* dsatetsime_exec_cpin_cmd */


/*===========================================================================

FUNCTION DSATETSIME_EXEC_QCPINSTAT_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes $QCPINSTAT command.
  This command sends to the ME the status of all PINs for all cards.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR | 
    DSAT_CMD_ERR_RSP : if there was any problem in executing the command
    DSAT_OK :          if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatetsime_exec_qcpinstat_cmd
(
  dsat_mode_enum_type mode,                /*  AT command mode:            */
  const dsati_cmd_type *parse_table,       /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,       /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr              /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  char  * code_string = NULL;
  boolean hit = FALSE;
  uint8 idx, idx2;
  dsatme_mmgsdi_state_ms_info  *me_ms_val = NULL;
  
  /* Masks to extract the pertinent information for each pin:
     for each pin QCPINSTAT is to report on, we'll need to know if it's ready,
     enabled or blocked */
  dsat_cpin_required_e_type pin_mask[] =
  {
    (dsat_cpin_required_e_type)((uint32)DSAT_CPIN_NONE_REQUIRED | (uint32)DSAT_CPIN_SIM_PIN_REQUIRED),

#ifdef FEATURE_MMGSDI_PERSONALIZATION
    /* Personalization PINs for slot1 */
    (dsat_cpin_required_e_type)((uint32)DSAT_CPIN_NONE_REQUIRED | (uint32)DSAT_CPIN_PH_FSIM_PIN_REQUIRED),
    (dsat_cpin_required_e_type)((uint32)DSAT_CPIN_NONE_REQUIRED | (uint32)DSAT_CPIN_NET_PIN_REQUIRED),
    (dsat_cpin_required_e_type)((uint32)DSAT_CPIN_NONE_REQUIRED | (uint32)DSAT_CPIN_NETSUB_PIN_REQUIRED),
    (dsat_cpin_required_e_type)((uint32)DSAT_CPIN_NONE_REQUIRED | (uint32)DSAT_CPIN_SP_PIN_REQUIRED),
    (dsat_cpin_required_e_type)((uint32)DSAT_CPIN_NONE_REQUIRED | (uint32)DSAT_CPIN_CORP_PIN_REQUIRED),
#endif /* FEATURE_MMGSDI_PERSONALIZATION */

    (dsat_cpin_required_e_type)((uint32)DSAT_CPIN_NONE_REQUIRED | (uint32)DSAT_CPIN_SIM_PIN2_REQUIRED),

    /* NULL entry terminates table search */
    (dsat_cpin_required_e_type) 0
  };
  uint32 pin_reqd = (uint32)DSAT_CPIN_NONE_REQUIRED;
  me_ms_val = dsat_get_base_addr_current_session();

  /*-------------------------------------------------------
  Read command should return the 
  information requested in the form of a string
  ---------------------------------------------------------*/
  if (tok_ptr->op == (NA|QU))
  {
    /* Check for SIM/PIN error */
    if (DSAT_CME_NO_ERROR != me_ms_val->dsat_pin_error)
    {
      dsatme_set_cme_error(me_ms_val->dsat_pin_error, res_buff_ptr);
      result = DSAT_CMD_ERR_RSP;
    }
    else
    {
      res_buff_ptr->used = (word)snprintf((char*)res_buff_ptr->data_ptr,
                                                res_buff_ptr->size,
                                                "$QCPINSTAT: ");
      /* Use a mask specific to the pin we're dealing with */
      /* to extract the correct bits from dsat_pin_required then... */
      for ( idx=0; (idx < ARR_SIZE(pin_mask)) && 
            ((pin_reqd = (me_ms_val->dsat_pin_required & (uint32)pin_mask[idx])) != 0); 
            idx++)
      {
        /* Search the table for the proper response code/text */
        idx2 = 0;
        do
        {
          hit = (cpin_req_table[idx2].req_code ==
                    ((uint32)pin_reqd & cpin_req_table[idx2].req_code));

          /* If PIN is blocked use PUK text from table else PIN text */
          code_string = ((uint32)me_ms_val->dsat_pin_blocked & 
                         cpin_req_table[idx2].req_code) ?
            cpin_req_table[idx2].puk_text : cpin_req_table[idx2].pin_text;

          idx2++;
        } while (!hit && (code_string != NULL));
        if(code_string != NULL)
        {
          res_buff_ptr->used += (word)snprintf(
                           (char*)&res_buff_ptr->data_ptr[res_buff_ptr->used],
                           res_buff_ptr->size - res_buff_ptr->used,
                           "%s,", code_string);
       }
     }

      res_buff_ptr->data_ptr[--res_buff_ptr->used] = '\0';
    }
  }

  /*-------------------------------------------------------
  Test command should return only OK
  ---------------------------------------------------------*/
  else if (tok_ptr->op == (NA|EQ|QU))
  {
    /* this command is valid, but do nothing */
  }

  else
  {
    result = DSAT_ERROR;
  }

  return result;
} /* dsatetsime_exec_qcpinstat_cmd */

#endif /* FEATURE_DATA_ETSI_PIN */

/*--------     Other public functions inside ATCOP                 --------*/
#ifdef FEATURE_DATA_ETSI_PIN
#ifdef FEATURE_MMGSDI 

/*===========================================================================

FUNCTION DSATETSIME_SET_PIN_STATUS

DESCRIPTION
  This function provides an interface to enable/disable a specific SIM PIN.
  
DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of execution.
  possible values:
    DSAT_ASYNC_CMD :  excuting the action command and waiting for the
                      callback function be called.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsime_set_pin_status
(
  mmgsdi_pin_enum_type pin_id,          /* PIN identifier */
  boolean              pin_enabled,           /* Enabled flag   */
  byte *               pin,                    /* PIN password */
  dsat_apps_id_e_type  apps_id
)
{
  mmgsdi_return_enum_type  mmgsdi_result;
  mmgsdi_data_type pin_data;
  dsatme_mmgsdi_state_ms_info  *me_ms_val = NULL;

  me_ms_val = dsat_get_base_addr_current_session();

  if ( pin == NULL )
  {
    DS_AT_MSG0_ERROR("PIN value is NULL in dsatetsime_set_pin_status");
    return DSAT_ERROR;
  }
  
  pin_data.data_ptr = (uint8 *)pin;
#ifdef FEATURE_DSAT_CUST
  #error code not present
#else
  pin_data.data_len = (mmgsdi_len_type) strlen((char*) pin);
#endif /* FEATURE_DSAT_CUST */

  /* Invoke state change */
  SET_PIN_STATUS( pin_enabled, pin_id, pin_data, CLCK_MMGSDI_PIN1_VERIFY);

  if (mmgsdi_result == MMGSDI_SUCCESS) 
  {
    /* Flag that the PIN state is being changed */
    me_ms_val->dsat_pin_required |= (uint32)DSAT_CPIN_STATE_CHANGE_PENDING;
    dsatme_pending_mmgsdi_command[apps_id] = TRUE;
    return DSAT_ASYNC_CMD;
  }
  else
  {
    DS_AT_MSG1_ERROR("MMGSDI returned failure %d",(int)mmgsdi_result);
    return dsat_send_cme_error(DSAT_CME_SIM_FAILURE);
  }
} /* dsatetsime_set_pin_status */


/*===========================================================================

FUNCTION DSATETSIME_VERIFY_PIN

DESCRIPTION
  This function provides an interface to verify a specific SIM PIN.
  
DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of execution.
  possible values:
    DSAT_ASYNC_CMD :  excuting the action command and waiting for the
                      callback function be called.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsime_verify_pin
(
  mmgsdi_pin_enum_type pin_id,          /* PIN identifier */
  byte *         pin,                   /* PIN password */
  cb_cmd_e_type  req_submitter          /* Callback return identifier */
)
{
  mmgsdi_return_enum_type mmgsdi_status;
  mmgsdi_data_type pin_data;

  if ( pin == NULL )
  {
    DS_AT_MSG0_ERROR("PIN value is NULL in dsatetsime_verify_pin");
    return DSAT_ERROR;
  }

  pin_data.data_ptr = (uint8 *)pin;

#ifdef FEATURE_DSAT_CUST
  #error code not present
#else
  pin_data.data_len = (mmgsdi_len_type) strlen((char*) pin);
#endif /* FEATURE_DSAT_CUST */

  /* Submit request to MMGSDI to verify PIN */
  VERIFY_PIN( dsat_get_current_gw_session_id(FALSE),
              pin_id, pin_data, req_submitter );

  if (mmgsdi_status == MMGSDI_SUCCESS )
  {
    return DSAT_ASYNC_CMD;
  }
  else
  {
    /* MMGSDI command queue was full */
    return DSAT_ERROR;
  }
} /* dsatetsime_verify_pin */

#endif /* FEATURE_MMGSDI */

/*===========================================================================

FUNCTION DSATETSIME_SIM_READ

DESCRIPTION
  This function provides an interface to read SIM EF's.  The input
  parameter dictates the caller which maps to a specific EF.
  
DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of execution.
  possible values:
    DSAT_ASYNC_CMD :  excuting the action command and waiting for the
                      callback function be called.
    DSAT_ERROR :  an error occurred.
                      
SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsime_sim_read
(
  cb_cmd_e_type  req_submitter          /* Callback return identifier */
)
{
  /* Submit request to GSDI to read EF */
  if (TRUE == req_mmgsdi_sim_read( req_submitter ) )
  {
    dsatme_pending_mmgsdi_command[dsat_get_current_gw_apps_id(FALSE)] = TRUE;
    return DSAT_ASYNC_CMD;
  }
  else
  {
    return DSAT_ERROR;
  }
} /* dsatetsime_sim_read */


/*===========================================================================

FUNCTION DSATETSIME_SIM_WRITE

DESCRIPTION
  This function provides an interface to write SIM EF's.  The input
  parameter req_submitter dictates the caller which maps to a
  specific EF.  The buffer and buffer lenght are also specified.
  
DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of execution.
  possible values:
    DSAT_ASYNC_CMD :  excuting the action command and waiting for the
                      callback function be called.
    DSAT_ERROR :  an error occurred.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsime_sim_write
(
  cb_cmd_e_type  req_submitter,         /* Callback return identifier */
  byte * buffer,                        /* EF buffer */
  uint8  buflen                         /* Buffer length */
)
{
  /* Submit request to GSDI to write EF */
  if (TRUE == req_mmgsdi_sim_write( req_submitter, buffer, buflen  ) )
  {
    dsatme_pending_mmgsdi_command[dsat_get_current_gw_apps_id(FALSE)] = TRUE;
    return DSAT_ASYNC_CMD;
  }
  else
  {
    return DSAT_ERROR;
  }
} /* dsatetsime_sim_write */

#ifdef FEATURE_MMGSDI 

/*===========================================================================

FUNCTION DSATETSIME_GET_PIN_STATUS

DESCRIPTION
  This function provides an interface to query SIM PIN.
  
DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of execution.
  possible values:
    DSAT_ASYNC_CMD :  excuting the action command and waiting for the
                      callback function be called.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatetsime_get_pin_status
(
  mmgsdi_pin_enum_type  pin_id                /* PIN identifier */
)
{
  mmgsdi_return_enum_type mmgsdi_status;

  GET_PIN_STATUS(dsat_get_current_gw_session_id(FALSE), CLCK_MMGSDI_PIN1_STATUS );

  if (mmgsdi_status == MMGSDI_SUCCESS) 
  {
    /* Flag start of user-initiated MMGSDI action */
    dsatme_pending_mmgsdi_command[dsat_get_current_gw_apps_id(FALSE)] = TRUE;
    return DSAT_ASYNC_CMD;
  }
  else
  {
    return DSAT_ERROR;
  }
} /* dsatetsime_get_pin_status */



/*===========================================================================

FUNCTION DSATETSIME_CHANGE_PIN

DESCRIPTION
  This function provides an interface to change a specific SIM PIN.
  
DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of execution.
  possible values:
    DSAT_ASYNC_CMD :  excuting the action command and waiting for the
                      callback function be called.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsime_change_pin
(
  mmgsdi_pin_enum_type pin_id,          /* PIN identifier */
  dsat_string_item_type * pin,          /* PIN password */
  dsat_string_item_type * new_pin       /* PIN new password */
)
{
  mmgsdi_return_enum_type mmgsdi_result;
  mmgsdi_data_type old_pin_data, new_pin_data;

  cb_cmd_e_type req_submitter = (MMGSDI_PIN1 == pin_id)?
      CPWD_MMGSDI_PIN1_CHANGE : CPWD_MMGSDI_PIN2_CHANGE;

  if ( pin == NULL )
  {
    DS_AT_MSG0_ERROR("PIN value is NULL in dsatetsime_change_pin");
    return DSAT_ERROR;
  }
  
  old_pin_data.data_ptr = (uint8 *)pin;
#ifdef FEATURE_DSAT_CUST
  #error code not present
#else
  old_pin_data.data_len = (mmgsdi_len_type)strlen((char *) pin);
#endif /* FEATURE_DSAT_CUST */

  if ( new_pin == NULL )
  {
    DS_AT_MSG0_ERROR("New PIN value is NULL in dsatetsime_change_pin");
    return DSAT_ERROR;
  }
  
  new_pin_data.data_ptr = (uint8 *)new_pin;

#ifdef FEATURE_DSAT_CUST
  #error code not present
#else
  new_pin_data.data_len = (mmgsdi_len_type)strlen((char *) new_pin);
#endif /* FEATURE_DSAT_CUST */

  CHANGE_PIN(pin_id, old_pin_data, new_pin_data, req_submitter);

  if (mmgsdi_result == MMGSDI_SUCCESS)
  {
    /* Flag start of user-initiated MMGSDI action */
    dsatme_pending_mmgsdi_command[dsat_get_current_gw_apps_id(FALSE)] = TRUE;
    return DSAT_ASYNC_CMD;
  }
  else
  {
    return DSAT_ERROR;
  }
} /* dsatetsime_change_pin */
#endif /* FEATURE_MMGSDI */

/*===========================================================================

FUNCTION  DSAT_PIN_MMGSDI_EVENT_HANDLER

DESCRIPTION
  This function is the handler function for the GSDI events

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ASYNC_EVENT : async event, contine processing  

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsat_pin_mmgsdi_event_handler
(
  ds_at_mmgsdi_event_type *mmgsdi_event_ptr /* MMGSDI event pointer */
)
{
  dsat_result_enum_type    result = DSAT_ASYNC_EVENT;
  mmgsdi_slot_id_enum_type slot_id;
  dsat_apps_id_e_type      apps_id;
  mmgsdi_events_enum_type  mmgsdi_event;
  dsatme_mmgsdi_state_ms_slot_info *me_ms_slot_val = NULL;
  dsatme_mmgsdi_state_ms_info  *me_ms_val = NULL;
  dsatme_mmgsdi_state_ss_info  *me_ss_val = NULL;
  /* To get the present status */

#define GENERATE_QCSIMSTAT_RESPONSE(slot_id) \
  { \
    if (me_ms_slot_val->is_qcsimstat_updated == TRUE) \
    { \
      me_ms_slot_val->is_qcsimstat_updated = FALSE; \
      if (DSAT_QCSIMSTAT_URC_ENABLE == (dsat_num_item_type)dsatutil_get_val(\
                              DSAT_VENDOR_QCSIMSTAT_IDX,0,0,NUM_TYPE)) \
      { \
        dsatme_report_qcsimstat_result(slot_id); \
      } \
    } \
  }

  slot_id = mmgsdi_event_ptr->slot_id;
  apps_id = mmgsdi_event_ptr->apps_id;
  mmgsdi_event = mmgsdi_event_ptr->mmgsdi_event;
  me_ms_slot_val = dsat_get_base_addr_per_slot(slot_id, TRUE);
  me_ss_val = dsat_get_base_addr(DSAT_MMGSDI_SS_VALS,TRUE);

  /* Convert the detailed error to general error. If it is not an error
     then the event remains the same */
  if( mmgsdi_event != MMGSDI_CARD_INSERTED_EVT &&
      mmgsdi_event != MMGSDI_CARD_REMOVED_EVT &&
      mmgsdi_event != MMGSDI_CARD_ERROR_EVT)
  {
    me_ms_val = dsat_get_base_addr_per_session(apps_id, TRUE);
    if (NULL == me_ms_val)
    {
      return DSAT_ASYNC_EVENT;
    }
  }
  else
  {
    if (NULL == me_ss_val)
    {
      return DSAT_ASYNC_EVENT;
    }
  }

  switch ( mmgsdi_event )
  {
    case MMGSDI_CARD_INSERTED_EVT:
#ifdef FEATURE_DSAT_CFUN_CARD_POWER_CTL
      /* Check for report requirement:
         If +CFUN has set FULL functionality and SIM card has been powered 
         up, the reception of card inserted event will signify successful end of
         processing. This case deliberately falls through to processing below. */
         
      if(CHECK_PENDING(DSAT_EXT_CFUN_IDX,0,DSAT_PENDING_CFUN_WRITE_CARD_PUP) )
      {
        SET_PENDING(DSAT_EXT_CFUN_IDX,0,DSAT_PENDING_CFUN_NONE);
        me_ss_val->dsat_card_error_event_count = 0;
        result = DSAT_OK;
      }
#endif /* FEATURE_DSAT_CFUN_CARD_POWER_CTL */
      break;

    case MMGSDI_SUBSCRIPTION_READY_EVT:
      /*Update the flag to indicate SIM INIT COMPLETED*/
      if (!IS_VALID_GW_APPS_ID(apps_id))
      {
        return DSAT_ERROR;
      }

      if (NULL != me_ms_slot_val)
      {
        GENERATE_QCSIMSTAT_RESPONSE(slot_id);
      }

  case MMGSDI_PIN1_EVT:
  case MMGSDI_PIN2_EVT:
    /* Check for manual state change flag */
    if (0 == (me_ms_val->dsat_pin_required & (uint32)DSAT_CPIN_STATE_CHANGE_PENDING))
    {
      /* Process PIN state change */
      if (DSAT_OK != cpin_track_pin_status (mmgsdi_event_ptr->mmgsdi_event,
             mmgsdi_event_ptr->pin.pin_info.status,apps_id))
      {
        DS_AT_MSG0_ERROR("Problem updating PIN status");
        me_ms_val->dsat_pin_required &= (uint32)(~(uint32)( DSAT_CPIN_STATE_CHANGE_PENDING ));
        return DSAT_ERROR;
      }
    }
    
    /* Clear flag for manual state change and return */
    if (0 != (me_ms_val->dsat_pin_required & (uint32)DSAT_CPIN_STATE_CHANGE_PENDING))
    {
      me_ms_val->dsat_pin_required &= (uint32)(~(uint32)( DSAT_CPIN_STATE_CHANGE_PENDING ));
    }    
    break;

  case MMGSDI_SESSION_CHANGED_EVT:
  {
    if (FALSE == mmgsdi_event_ptr->activated)
    {
      if (TRUE == dsatme_pending_mmgsdi_command[apps_id])
      {
        dsatme_pending_mmgsdi_command[apps_id] = FALSE;
        DS_AT_MSG0_HIGH("Session De-activated, clearing pending commands");
        return DSAT_ERROR;
      }
    }
    break;
  }

  case MMGSDI_CARD_REMOVED_EVT:
  case MMGSDI_CARD_ERROR_EVT:
    if (NULL != me_ms_slot_val)
    {
      if (me_ms_slot_val->dsat_qcsimstat_curr != DSAT_QCSIMSTAT_SIM_ERROR)
      {
        me_ms_slot_val->dsat_qcsimstat_curr  = DSAT_QCSIMSTAT_SIM_ERROR;
        me_ms_slot_val->is_qcsimstat_updated = TRUE;
      }

      GENERATE_QCSIMSTAT_RESPONSE(slot_id);
    }

#ifdef FEATURE_DSAT_CFUN_CARD_POWER_CTL

    /* Check for report requirement:
       If +CFUN has set MIN functionality and SIM card has been powered 
       down, the reception of card error event will signify successful end of
       processing. This case deliberately falls through to processing below. */
    
    if(CHECK_PENDING(DSAT_EXT_CFUN_IDX,0,DSAT_PENDING_CFUN_WRITE_CARD_DOWN) )
    {
       SET_PENDING(DSAT_EXT_CFUN_IDX,0,DSAT_PENDING_CFUN_NONE)
      result = DSAT_OK;
    }
    if(CHECK_PENDING(DSAT_EXT_CFUN_IDX,0,DSAT_PENDING_CFUN_WRITE_CARD_PUP) )
    {
      me_ss_val->dsat_card_error_event_count++;
      if (me_ss_val->dsat_card_error_event_count < me_ss_val->dsat_num_of_card_operations)
      {
        return DSAT_ASYNC_EVENT;
      }
      else
      {
        me_ss_val->dsat_card_error_event_count = 0;
        SET_PENDING(DSAT_EXT_CFUN_IDX,0,DSAT_PENDING_CFUN_NONE)
        result = DSAT_OK;
      }
    }

#endif /* FEATURE_DSAT_CFUN_CARD_POWER_CTL */
    break;

  default:
    DS_AT_MSG1_LOW("Unsupported MMGSDI event: %d",
            mmgsdi_event_ptr->mmgsdi_event);
    break;
  }
  
  return result;
} /* dsat_pin_mmgsdi_event_handler */

#endif /* FEATURE_DATA_ETSI_PIN */


/*===========================================================================

FUNCTION DSATETSIME_EXEC_CIMI_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +CIMI command.
  This command returns the international mobile subscriber identy (IMSI)
  from the SIM card in the slot specified by $QCSLOT command.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR:        syntax error.
    DSAT_CMD_ERR_RSP:  if attempt was made to read IMSI from a slot that is
                       not the GSM operationa slot.
    DSAT_OK          : if it is a success.
    DSAT_ASYNC_CMD   : if this function invokes an async command.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatetsime_exec_cimi_cmd
(
  dsat_mode_enum_type  mode,              /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr             /*  Place to put response       */
)
{
  dsat_result_enum_type         result = DSAT_OK;
  mmgsdi_app_enum_type          app_type = dsat_get_current_gw_app_type(FALSE);
  ds_at_sim_info_type           sim_imsi_info;
  dsat_mmgsdi_imsi_info_s_type *imsi_data = NULL;
  byte imsi_buf[32];
  
#ifdef FEATURE_DSAT_GOBI_MAINLINE
  boolean show_imsi = TRUE;
  uint32 uqcn_ver = 0;
#endif /* FEATURE_DSAT_GOBI_MAINLINE */

  /*-------------------------------------------------------
  General commands with no arguments should return the 
  information requested in the form of a string
  ---------------------------------------------------------*/
  if (tok_ptr->op == NA)
  {
#ifdef FEATURE_DSAT_GOBI_MAINLINE
    uqcn_ver = dsatutil_get_uqcn_version();
    show_imsi = ( ( uqcn_ver & UNDP_CARRIER_MAJOR_MASK ) != UNDP_CARRIER_DOCOMO_MAJOR );
#endif /* FEATURE_DSAT_GOBI_MAINLINE */

    if ( 
#ifdef FEATURE_DSAT_GOBI_MAINLINE
        show_imsi && 
#endif /* FEATURE_DSAT_GOBI_MAINLINE */
        ( (app_type == MMGSDI_APP_SIM) || (app_type == MMGSDI_APP_USIM) ) )
    {
      memset(&sim_imsi_info ,0,sizeof(ds_at_sim_info_type));
      imsi_data = dsatme_get_imsi_info();
      
      if(NULL == imsi_data)
      {
        dsatme_set_cme_error(DSAT_CME_OP_NOT_ALLOWED, res_buff_ptr);
        result = DSAT_CMD_ERR_RSP;
      }
      else
      {
        /* If IMSI read the first byte indicates the length */
        sim_imsi_info.length = imsi_data->data[0];
        
        if( sim_imsi_info.length < MMGSDI_IMSI_LEN)
        {
          (void) dsatutil_memscpy((void*)&sim_imsi_info.data[0],
                  DSAT_MAX_MMGSDI_BUFFER_LEN,(void*)&imsi_data->data[1],
                  MIN(sim_imsi_info.length,MMGSDI_IMSI_LEN - 1));
          
          if ( DSAT_OK == etsime_decode_imsi(&sim_imsi_info, imsi_buf) )
          {
            res_buff_ptr->used = (word)snprintf((char*)res_buff_ptr->data_ptr,
                                                       res_buff_ptr->size,"%s",
                                                      (char*)imsi_buf);
            result = DSAT_OK;
          }
          else
          {
            /* Empty string returned indicates error in SIM programming */
            dsatme_set_cme_error(DSAT_CME_SIM_WRONG, res_buff_ptr);
            result = DSAT_CMD_ERR_RSP;
          }
        }
        else
        {
          DS_AT_MSG2_ERROR("Incorrect IMSI len  %d from buffer %d from attr ",sim_imsi_info.length,imsi_data->len);
          result = DSAT_ERROR;
        }
      }
    }
    /* Won't attempt to read IMSI from non-GSM operational slot */
    else
    {
      dsatme_set_cme_error(DSAT_CME_OP_NOT_ALLOWED, res_buff_ptr);
      result = DSAT_CMD_ERR_RSP;
    }
  }
      
  /*-------------------------------------------------------
  General commands with a query should produce no response
  ---------------------------------------------------------*/
  else if (tok_ptr->op == (NA|EQ|QU))
  {
    /* this command is valid, but do nothing */
  }

  /*-------------------------------------------------------
  Any argument with the command should result in an error
  ---------------------------------------------------------*/
  else
  {
    result = DSAT_ERROR;
  }

  return result;
} /* dsatetsime_exec_cimi_cmd() */


/*===========================================================================

FUNCTION DSATETSIME_EXEC_CNUM_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +CNUM command.
  This command returns the MSISDN from PBM for the SIM card in 
  the operational slot.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_CMD_ERR_RSP : if there was any problem in executing the command
    DSAT_ERROR : if the command is not supported
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatetsime_exec_cnum_cmd
(
  dsat_mode_enum_type  mode,              /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr             /*  Place to put response       */
)
{

  dsat_result_enum_type result = DSAT_OK;

  /*-------------------------------------------------------
  General commands with no arguments should return the 
  information requested in the form of a string
  ---------------------------------------------------------*/
  if(tok_ptr->op == NA)
  {
#if defined(FEATURE_ETSI_PBM)
      result = etsime_msisdn_read(res_buff_ptr);
#endif /* defined(FEATURE_ETSI_PBM) */
  }

  /*-------------------------------------------------------
  General commands with a query should produce no response
  ---------------------------------------------------------*/
  else if(tok_ptr->op == (NA | EQ | QU))
  {
    /* this command is valid, but do nothing */
  }

  /*-------------------------------------------------------
  Any argument with the command should result in an error
  ---------------------------------------------------------*/
  else
  {
    result = DSAT_ERROR;
  }

  return result;

} /* dsatetsime_exec_cnum_cmd */

#ifdef FEATURE_MMGSDI

/*===========================================================================

FUNCTION DSATETSIME_EXEC_CPOL_CMD

DESCRIPTION
  This function takes the result from the command line parser and
  executes it. It executes +CPOL command, which read and write entries into
  preferred Network operators list.

DEPENDENCIES
  None
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ASYNC_CMD : If list of preferred networks need to be retrieved 
                     from Call Manager
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success for local execution
    DSAT_CMD_ERR_RSP : If SIM initialization is failed

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsime_exec_cpol_cmd
(
  dsat_mode_enum_type       mode,          /*  AT command mode:            */
  const dsati_cmd_type     *parse_table,   /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,       /*  Command tokens from parser  */
  dsm_item_type            *res_buff_ptr   /*  Place to put response       */
)
{
#define NUM_OPER_SUBFIELDS 2
#define OPER_LEN_W_MNC3    6
#define OPER_LEN_W_MNC2    5

  dsat_result_enum_type result = DSAT_ASYNC_CMD;
  dsat_num_item_type index;
  dsat_num_item_type cpol_format = (dsat_num_item_type)DSAT_CPOL_NUMERIC;
  mmgsdi_client_data_type  client_ref;
  mmgsdi_access_type  file_access;
  dsatme_mmgsdi_state_ss_info  *me_ss_val = NULL;
  dsatme_mmgsdi_state_ms_info  *me_ms_val = NULL;
  dsat_me_cpol_read_param_s_type *read_params_ptr;
  dsat_error_info_s_type    err_info;

  err_info.errval = DSAT_ERR_NO_ERROR;
  err_info.arg_num = 0;

  me_ss_val = dsat_get_base_addr(DSAT_MMGSDI_SS_VALS, FALSE);
  me_ms_val = dsat_get_base_addr_current_session();

  /* Allocate command memory pointer for +CPOL command */
  read_params_ptr = dsat_alloc_cmd_mem_ptr(sizeof(dsat_me_cpol_read_param_s_type));

  /* Reset abort flag */
  dsat_abort_plmn_listing = FALSE;
  memset(&file_access , 0 ,sizeof(mmgsdi_access_type));
  /* Check if sim file is initialized */
  if (me_ms_val->dsat_sim_filename_plmn_selector == MMGSDI_NO_FILE_ENUM )
  {
    if (dsatme_init_sim_filename() == DSAT_ERROR )
    {
      return dsat_send_cme_error(DSAT_CME_SIM_FAILURE);
    }
  }
  /* Update file access by sim initilized values */
  file_access.access_method = MMGSDI_EF_ENUM_ACCESS;
  file_access.file.file_enum = me_ms_val->dsat_sim_filename_plmn_selector;
  if(tok_ptr->op == (NA|EQ|QU))
  {
    client_ref = (mmgsdi_client_data_type)CPOL_MMGSDI_SIM_FILE_ATTR;
    SET_PENDING(DSATETSI_EXT_ACT_CPOL_ETSI_IDX ,0, DSAT_PENDING_CPOL_TEST)
    /* Using the mmgsdi_get_file_attr function to get the number of records */ 
    if (MMGSDI_SUCCESS == mmgsdi_session_get_file_attr_ext(
            dsat_get_current_gw_session_id(FALSE),
            file_access, 
            (mmgsdi_callback_type) dsatme_mmgsdi_get_file_attr_cb,
            client_ref,
            TRUE ))
    {
      result = DSAT_ASYNC_CMD;
    }
    else
    {
      SET_PENDING(DSATETSI_EXT_ACT_CPOL_ETSI_IDX ,0, DSAT_PENDING_CPOL_NONE)
      return dsat_send_cme_error(DSAT_CME_SIM_FAILURE);
    }
  }
  else if (tok_ptr->op == (NA|EQ|AR))
  {
    me_ms_val->dsat_cpol_write_params.no_of_args = (int16)tok_ptr->args_found;

    /* check whether the argument is present or NULL */
    if (VALID_TOKEN(0))
    {
      if ( dsatutil_atoi( &index, 
                         tok_ptr->arg[0], 10) != ATOI_OK )
      {
        /* if index1 can not be converted into a legal number,
                this is invalid parameter */
        err_info.errval = DSAT_ERR_ATOI_CONVERSION_FAILURE;
        err_info.arg_num = 0;
        goto send_error;
      }
      /* get the index */
      me_ms_val->dsat_cpol_write_params.index = (int16)index;
    }
    else
    {
      me_ms_val->dsat_cpol_write_params.index = CPOL_INVALID_INDEX;
    }

    if (tok_ptr->args_found > 1)
    {
      /* If argument is NULL then send an error */
      if (!VALID_TOKEN(1))
      {
        return DSAT_ERROR;
      }

        /* get the format */
      if ( dsatutil_atoi( &cpol_format, 
                         tok_ptr->arg[1], 10) != ATOI_OK )
      {
        /* if format can not be converted into a legal number,
               this is invalid parameter */
        err_info.errval = DSAT_ERR_ATOI_CONVERSION_FAILURE;
        err_info.arg_num = 1;
        goto send_error;
      }

      /* Check the validity of format field */
      if (cpol_format > (dsat_num_item_type)DSAT_CPOL_NUMERIC)
      {
        dsatme_set_cme_error(DSAT_CME_INVALID_INDEX, res_buff_ptr);
        return DSAT_CMD_ERR_RSP;
      }

    }
  
    result = dsatparm_exec_param_cmd(mode, parse_table, tok_ptr, res_buff_ptr);
    if (result != DSAT_OK)
    {
      return result;
    }

    if (tok_ptr->args_found == 2)
    {
      if (me_ms_val->dsat_cpol_write_params.index == CPOL_INVALID_INDEX)
      {
        /* only format is configured */
        return DSAT_OK;
      }
      else
      {
        /* As it is equal to delete the PLMN in the 
        given index, make no of args as 1 */
        me_ms_val->dsat_cpol_write_params.no_of_args = 1;
      }
    }
  
    /* Get the PLMN */
    if (tok_ptr->args_found >= 3)
    {
      dsat_num_item_type cpol_act;
      uint8 num_fields_scanned = 0;

      me_ms_val->dsat_cpol_write_params.act = CPOL_NO_ACT;	
#ifdef FEATURE_DSAT_LTE
      if ( me_ms_val->dsat_plmn_wact_supported && !(tok_ptr->args_found == 7))
      {
        err_info.errval = DSAT_ERR_INVALID_NUM_OF_ARGS;
        goto send_error;
      }
#else
      if ( me_ms_val->dsat_plmn_wact_supported && !(tok_ptr->args_found == 6))
      {
        err_info.errval = DSAT_ERR_INVALID_NUM_OF_ARGS;
        goto send_error;
      }       
#endif /* FEATURE_DSAT_LTE */
      me_ms_val->dsat_cpol_write_params.mcc = 0;
      me_ms_val->dsat_cpol_write_params.mnc = 0;

      /* If argument is NULL or is not double quoted 
         then send an error */
      if ( !VALID_TOKEN(2) ||
           !dsatutil_strip_quotes_out( 
                                      tok_ptr->arg[2],
                                      tok_ptr->arg[2],
                                      (uint16)strlen( (char *)tok_ptr->arg[2] )
                                     ) 
         )
      {
        err_info.errval = DSAT_ERR_QUOTE_STRIP_FAILURE;
        err_info.arg_num = 2;
        goto send_error;
      }
      /* Get the Access Technology (Tokens 3, 4 and 5) */	  
      if ( VALID_TOKEN(3))
      {
        if ( dsatutil_atoi(&cpol_act, 
                         tok_ptr->arg[3], 10) != ATOI_OK )
        {
            /* if format can not be converted into a legal number,
             this is invalid parameter */
          err_info.errval = DSAT_ERR_ATOI_CONVERSION_FAILURE;
          err_info.arg_num = 3;
          goto send_error;
        }
        else if (cpol_act == 1)
        {
          me_ms_val->dsat_cpol_write_params.act |= SYS_ACT_GSM_MASK;
        }
      }
      if ( VALID_TOKEN(4))
      {
        if ( dsatutil_atoi(&cpol_act, 
                         tok_ptr->arg[4], 10) != ATOI_OK )
        {
            /* if format can not be converted into a legal number,
                     this is invalid parameter */
          err_info.errval = DSAT_ERR_ATOI_CONVERSION_FAILURE;
          err_info.arg_num = 4;
          goto send_error;
        }
        else if (cpol_act == 1)
        {
          me_ms_val->dsat_cpol_write_params.act |= SYS_ACT_GSM_COMPACT_MASK;
        }
      }
      if ( VALID_TOKEN(5))
      {
        if ( dsatutil_atoi(&cpol_act, 
                         tok_ptr->arg[5], 10) != ATOI_OK )
        {
          /* if format can not be converted into a legal number,
                    this is invalid parameter */
          err_info.errval = DSAT_ERR_ATOI_CONVERSION_FAILURE;
          err_info.arg_num = 5;
          goto send_error;
        }
        else if (cpol_act == 1)
        {
          me_ms_val->dsat_cpol_write_params.act |= SYS_ACT_UMTS_MASK;
        }
      }
#ifdef FEATURE_DSAT_LTE
      if ( VALID_TOKEN(6))
      {
        if ( dsatutil_atoi(&cpol_act, 
                         tok_ptr->arg[6], 10) != ATOI_OK )
        {
          /* if format can not be converted into a legal number,
                    this is invalid parameter */
          err_info.errval = DSAT_ERR_ATOI_CONVERSION_FAILURE;
          err_info.arg_num = 6;
          goto send_error;
        }
        else if (cpol_act == 1)
        {
          me_ms_val->dsat_cpol_write_params.act |= SYS_ACT_LTE_MASK;
        }
      }
#endif /* FEATURE_DSAT_LTE */

      switch ( cpol_format )
      {
      case DSAT_CPOL_LONG_ALPHA_NUMERIC:
      case DSAT_CPOL_SHORT_ALPHA_NUMERIC:
      {
        mmgsdi_data_type longname;
        mmgsdi_data_type shortname;
        mmgsdi_rat_enum_type rat = MMGSDI_RAT_NONE;
        mmgsdi_return_enum_type status = MMGSDI_MAX_RETURN_ENUM;
        longname.data_ptr = NULL;
        longname.data_len = 0;
        shortname.data_ptr = NULL;
        shortname.data_len = 0;
        if(DSAT_CPOL_LONG_ALPHA_NUMERIC == cpol_format)
        {
        longname.data_len = strlen((char *)tok_ptr->arg[2]);
        longname.data_ptr = ( uint8 *)(char *)tok_ptr->arg[2];
    }
        else
        {
        shortname.data_len = strlen((char *)tok_ptr->arg[2]);
        shortname.data_ptr = ( uint8 *)(char *)tok_ptr->arg[2];
    }
                   status = mmgsdi_get_se13_plmn_info_by_name(
                                      me_ss_val->dsat_mmgsdi_client_id,
                                      longname,
                                      shortname,
                                      rat,
                                      dsatetsime_cpol_cmd_cb_func,
                                      (mmgsdi_client_data_type)CPOL_MMGSDI_PLMN_INFO);
                   if( MMGSDI_SUCCESS == status )
                   {
                      return DSAT_ASYNC_CMD;
                   }
                   else
                   {
                     /* Name lookup failed so return error  */
                     DS_AT_MSG0_ERROR("CPOL network name lookup failed");
                     dsatme_set_cme_error(DSAT_CME_NOT_FOUND, res_buff_ptr);
                     return DSAT_CMD_ERR_RSP;
                   }
                   break;
       }/* DSAT_CPOL_SHORT_ALPHA_NUMERIC:*/

      case DSAT_CPOL_NUMERIC:
        /* If mnc doesn't includes pcs digit */
        if ( strlen(((char *)tok_ptr->arg[2])) == OPER_LEN_W_MNC2 )
    {
          num_fields_scanned = (uint8)sscanf(((char *)tok_ptr->arg[2]),
                                      "%03lu%02lu",
                                      &(me_ms_val->dsat_cpol_write_params.mcc),
                                      &(me_ms_val->dsat_cpol_write_params.mnc));
          me_ms_val->dsat_cpol_write_params.mnc_includes_pcs_digit = FALSE;
        }
        else if ( strlen(((char *)tok_ptr->arg[2])) == OPER_LEN_W_MNC3 )
        {
          num_fields_scanned = (uint8)sscanf(((char *)tok_ptr->arg[2]),
                                      "%03lu%03lu",
                                      &(me_ms_val->dsat_cpol_write_params.mcc),
                                      &(me_ms_val->dsat_cpol_write_params.mnc));
          me_ms_val->dsat_cpol_write_params.mnc_includes_pcs_digit = TRUE;
    }
    else
    {
          /* <oper> parm must be 5 or 6 character quoted string */
          return DSAT_ERROR;
    }

        if ( NUM_OPER_SUBFIELDS != num_fields_scanned )
        {
          /* Must successfully scan MCC and MNC subfields from <oper> parm */
          return DSAT_ERROR;
        }
        break;

      default:
        DS_AT_MSG1_ERROR("Invalid +CPOL format: %d", cpol_format);
        return DSAT_ERROR;
      }

    }

    result = dsatetsime_process_plmn_info();
  }
  else if (tok_ptr->op == (NA|QU))
  {
    read_params_ptr->start_index = 1;
    (void)strlcpy(read_params_ptr->display_str,
                      "+CPOL",
                      sizeof(read_params_ptr->display_str));
    read_params_ptr->format =(dsat_cpol_dis_frmt_e_type)
     (dsat_num_item_type)dsatutil_get_val(DSATETSI_EXT_ACT_CPOL_ETSI_IDX,dsat_get_current_subs_id(FALSE),1,MIX_NUM_TYPE);
    SET_PENDING(DSATETSI_EXT_ACT_CPOL_ETSI_IDX ,0, DSAT_PENDING_CPOL_READ)
    client_ref = (mmgsdi_client_data_type)CPOL_MMGSDI_SIM_READ;
    
    if ( MMGSDI_SUCCESS == mmgsdi_session_read_transparent(
        dsat_get_current_gw_session_id(FALSE),
             file_access,
             0,
             0, 
             (mmgsdi_callback_type)dsatme_mmgsdi_read_transparent_cb,
             client_ref ))
    {
      result = DSAT_ASYNC_CMD;
    }
    else
    {
      SET_PENDING(DSATETSI_EXT_ACT_CPOL_ETSI_IDX ,0, DSAT_PENDING_CPOL_NONE)
      return dsat_send_cme_error(DSAT_CME_SIM_FAILURE);
    }
  }
  else
  {
    /* other commands are illegal */
    err_info.errval = DSAT_ERR_INVALID_SYNTAX;
    goto send_error;
  }

  return result;

send_error:
  if( err_info.errval == DSAT_ERR_INVALID_SYNTAX ||
      err_info.errval == DSAT_ERR_INVALID_NUM_OF_ARGS )
  {
    DS_AT_MSG1_ERROR("Error:%d",err_info.errval);
  }
  else
  {
    DS_AT_MSG2_ERROR("Error:%d, arg_num:%d",err_info.errval,err_info.arg_num);
  }
  return DSAT_ERROR;

} /* dsatetsime_exec_cpol_cmd() */


/*===========================================================================

FUNCTION DSATETSIME_CPOL_WRITE_PLMN_HANDLER

DESCRIPTION
  This function is used to write entries into preferred Network operators list.

DEPENDENCIES
  None
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ASYNC_CMD : If preffered plmn list update status need to be retrieved 
                     from Call Manager
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success for local execution

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type
dsatetsime_cpol_write_plmn_handler
(
  sys_user_pref_plmn_list_s_type  *pref_networks
)
{
#ifdef FEATURE_DSAT_LTE
#define ACT_EUTRAN          0x40
#endif /* FEATURE_DSAT_LTE */

  dsat_result_enum_type result = DSAT_ASYNC_CMD;
  boolean               mnc_includes_pcs_digit = FALSE;
  sys_plmn_id_s_type    inv_plmn_id;
  int8                  index;
  mmgsdi_client_data_type  client_ref;
  mmgsdi_write_data_type write_data;
  mmgsdi_offset_type     offset;
  mmgsdi_len_type        data_len;
  mmgsdi_access_type  file_access;
  dsatme_mmgsdi_state_ms_info  *me_ms_val = NULL;

  me_ms_val = dsat_get_base_addr_current_session();

  file_access.access_method = MMGSDI_EF_ENUM_ACCESS;
  file_access.file.file_enum = me_ms_val->dsat_sim_filename_plmn_selector;

  inv_plmn_id.identity[0] = INVALID_PLMN_DIGIT;
  inv_plmn_id.identity[1] = INVALID_PLMN_DIGIT;
  inv_plmn_id.identity[2] = INVALID_PLMN_DIGIT;

  if (dsat_abort_plmn_listing == TRUE)
  {
    /* User aborted the command */
    dsat_abort_plmn_listing = FALSE;

    /* Abort handler responsible for sending error */
    return DSAT_ASYNC_EVENT;
  }

  if ((me_ms_val->dsat_cpol_write_params.no_of_args == 1) && 
    (me_ms_val->dsat_cpol_write_params.index != CPOL_INVALID_INDEX) &&
    (me_ms_val->dsat_cpol_write_params.index > 0 ))
  {
    pref_networks->info[me_ms_val->dsat_cpol_write_params.index - 1].plmn = inv_plmn_id;
    /* The actual data that needs to send to MMGSDI is copied to 
    write_data.data_ptr below */
    pref_networks->info[me_ms_val->dsat_cpol_write_params.index -1].access_tech = 0;
  }
  else
  {
    sys_plmn_id_s_type    plmn_id;

    memset(&plmn_id,0x0,sizeof(plmn_id));
    if ( me_ms_val->dsat_cpol_write_params.mnc > MAX_2_DIGIT_DEC_NUM ||
         me_ms_val->dsat_cpol_write_params.mnc_includes_pcs_digit )
      mnc_includes_pcs_digit = TRUE;

    /* Form the PLMN id to be added */
    if (sys_plmn_set_mcc_mnc(mnc_includes_pcs_digit, 
      me_ms_val->dsat_cpol_write_params.mcc, 
      me_ms_val->dsat_cpol_write_params.mnc,
      &plmn_id) == FALSE)
    {
      return DSAT_ERROR;
    }

    /* find whether the PLMN is a duplicate and also the 
       free position where PLMN can be added */
    for (index = 0; index <= pref_networks->length -1; index++)
    {
      if ((plmn_id.identity[0] == 
            pref_networks->info[index].plmn.identity[0]) &&
        (plmn_id.identity[1] == 
            pref_networks->info[index].plmn.identity[1]) &&
        (plmn_id.identity[2] == pref_networks->info[index].plmn.identity[2]))
      {
        /* Duplicate PLMN id */
        DS_AT_MSG0_ERROR("Attempt to write duplicate PLMN denied");
        return dsat_send_cme_error(DSAT_CME_OP_NOT_ALLOWED);
      }

      if ((me_ms_val->dsat_cpol_write_params.index == CPOL_INVALID_INDEX) && 
        ((sys_user_pref_plmn_list_info_is_valid(
          pref_networks->info[index]) == FALSE) 
      || (sys_plmn_id_is_undefined(pref_networks->info[index].plmn))))
      {
        /* If the entry is invalid entry then and user has not given
           the index then update the index */
        me_ms_val->dsat_cpol_write_params.index = index + 1;
      }

    }
   
    pref_networks->info[me_ms_val->dsat_cpol_write_params.index - 1].plmn = plmn_id;
  }
  
  write_data.data_ptr = (uint8 *)
   &(pref_networks->info[me_ms_val->dsat_cpol_write_params.index - 1]);
  /* PLMN with Access Technologies have the data length of 5 else it is 3 */  
  if ( me_ms_val->dsat_plmn_wact_supported )
  {
    data_len = PLMN_W_ACT_LEN;
  }
  else
  {
    data_len = PLMN_LEN;
  }
  /* Filling up the Access Technology information provided */
  if ( !sys_plmn_id_is_undefined( pref_networks->info[me_ms_val->dsat_cpol_write_params.index - 1 ].plmn) ) 
  {
    if ( me_ms_val->dsat_cpol_write_params.act & SYS_ACT_UMTS_MASK )
    {
      write_data.data_ptr[3] = ACT_UMTS;
    }
    else
    {
      write_data.data_ptr[3] = ACT_NONE;
    }
#ifdef FEATURE_DSAT_LTE
    if ( me_ms_val->dsat_cpol_write_params.act & SYS_ACT_LTE_MASK )
    {
      write_data.data_ptr[3] = write_data.data_ptr[3] | ACT_EUTRAN;
    }
#endif /* FEATURE_DSAT_LTE */
    if ( me_ms_val->dsat_cpol_write_params.act & SYS_ACT_GSM_MASK )
    {
      write_data.data_ptr[4] = ACT_GSM;
    }
    else
    {
      write_data.data_ptr[4] = ACT_NONE;
    }

    if ( me_ms_val->dsat_cpol_write_params.act & SYS_ACT_GSM_COMPACT_MASK )
    {
      write_data.data_ptr[4] = write_data.data_ptr[4] | ACT_GSM_COMPACT;
    }
  }
  else if ( me_ms_val->dsat_cpol_write_params.no_of_args == 1) /* For Delete operation*/
  {
    /* To adjust the compiler memory alignment we have to manually copy
    the data. Though info is in 3 bytes of PLMN and 2 bytes of AcT, compiler
    actually assigns 4 bytes of PLMN and 2 bytes of AcT so we are actually
    sending junk data only in delete operation. The above section handles 
    correctly for regular writing*/

    write_data.data_ptr[3]=0;
    write_data.data_ptr[4]=0;
  }
  
  write_data.data_len = data_len;
  offset = (me_ms_val->dsat_cpol_write_params.index - 1) * data_len;
  client_ref = (mmgsdi_client_data_type)CPOL_MMGSDI_SIM_WRITE;

  if (dsat_abort_plmn_listing == TRUE)
  {
    /* User aborted the command */
    dsat_abort_plmn_listing = FALSE;

    /* Abort handler responsible for sending error */
    return DSAT_ASYNC_EVENT;
  }

  if(MMGSDI_SUCCESS != mmgsdi_session_write_transparent(             
                        dsat_get_current_gw_session_id(FALSE),
                           file_access, 
                           offset, write_data, 
                           (mmgsdi_callback_type)dsatme_mmgsdi_write_transparent_cb,
                           client_ref))
  {
    result = DSAT_ERROR;
  }
  
  return result;
  
}/* dsatetsime_cpol_write_plmn_handler */



/*===========================================================================

FUNCTION DSATETSIME_EXEC_CPLS_CMD

DESCRIPTION
  This function takes the result from the command line parser and
  executes it. It executes +CPLS command, which selects a  PLMN selector with 
  access technology based on the card mode. 

DEPENDENCIES
  None
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
                        
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success for local execution
    DSAT_CMD_ERR_RSP : If SIM initialization is failed

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type 
dsatetsime_exec_cpls_cmd
(
  dsat_mode_enum_type       mode,          /*  AT command mode:            */
  const dsati_cmd_type     *parse_table,   /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,       /*  Command tokens from parser  */
  dsm_item_type            *res_buff_ptr   /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  unsigned int supp_plmn_selectors = 0; /* Initialized with Zero */
  byte buffer[(uint8)DSAT_MAX_SUPP_PLMNWACT * 2] = { 0 };
  uint8 index = 0;
  dsat_cpls_plmn_selector plmn_selector;
  dsat_num_item_type cpls_val = 0;
  dsatme_mmgsdi_state_ms_info  *me_ms_val = NULL;

  me_ms_val = dsat_get_base_addr_current_session();

  memset( buffer, 0, sizeof(buffer) );

  if (tok_ptr->op == (NA|EQ|QU))
  { 
    /* Test command */
    result=dsatetsime_get_supp_plmn_selectors(&supp_plmn_selectors);
    if ( result == DSAT_OK )
    {
      /* This condition handles both the DSAT_SUPP_PLMN *
      ** as well as DSAT_SUPP_PLMNWACT                 */
      if ( supp_plmn_selectors & DSAT_SUPP_PLMN)
      {
        buffer[index++] = (byte)DSAT_PLMN + '0'; 
        buffer[index++] = ',';
      }

      if ( supp_plmn_selectors & DSAT_SUPP_OPLMNWACT )
      {
        buffer[index++] = (byte)DSAT_OPLMNWACT + '0';
        buffer[index++] = ',';
      }
                       
      if ( supp_plmn_selectors & DSAT_SUPP_HPLMNWACT )
      {
        buffer[index++] = (byte)DSAT_HPLMNWACT + '0'; 
        buffer[index] = '\0';
      }
      if( index != 0 && ( buffer[index - 1] == ',' ))
      {
        buffer[index - 1] = '\0';
      }

      res_buff_ptr->used = 
       (word)snprintf((char*)res_buff_ptr->data_ptr,
                      (res_buff_ptr->size - res_buff_ptr->used), 
                      "+CPLS: %s", 
                      buffer
                     );
    }
  }
  else if ( tok_ptr->op == (NA|QU) )
  {
    /* Read Command */
    if (me_ms_val->dsat_sim_filename_plmn_selector == MMGSDI_NO_FILE_ENUM )
    {
      if (dsatme_init_sim_filename() == DSAT_ERROR )
      {
        return dsat_send_cme_error(DSAT_CME_SIM_FAILURE);
      }
    }

    result = dsatetsime_get_plmn_selector(&plmn_selector);

    if ( result == DSAT_OK )
    {
      res_buff_ptr->used = 
      (word)snprintf((char*)res_buff_ptr->data_ptr,
                     (res_buff_ptr->size - res_buff_ptr->used), 
                     "+CPLS: %d", 
                      plmn_selector
                     );
    }

  }
  else if ( tok_ptr->op == (NA|EQ|AR) )
  {
    result = dsatparm_exec_param_cmd(mode,
                                     parse_table,
                                     tok_ptr,
                                     res_buff_ptr);

    if ( result == DSAT_OK )
    {
      cpls_val = (dsat_num_item_type)
                  dsatutil_get_val(DSATETSI_EXT_ACT_CPLS_ETSI_IDX,dsat_get_current_subs_id(FALSE),0,NUM_TYPE);

      result = dsatetsime_get_supp_plmn_selectors(&supp_plmn_selectors);

      if( result == DSAT_OK )
      {       
        if( (1 << cpls_val ) & supp_plmn_selectors )
        {
          result = dsatetsime_set_plmn_selector(cpls_val);
        }
         else
        {
          result = dsat_send_cme_error( DSAT_CME_PLMN_NOT_ALLOWED );        
          DS_AT_MSG0_ERROR("Invalid PLMN Selector with access technology");
        }
      }
    }
    /* Execute Command */ 
  }
  else 
  {
    result = DSAT_ERROR;
  }

  return result;

} /* dsatetsime_exec_cpls_cmd */


/*===========================================================================

FUNCTION DSATETSIME_GET_SUPP_PLMN_SELECTORS

DESCRIPTION
  This function is used to get the number of valid entries in
  preferred Network operators list.
  A read request to different EF's is made and if they return success, then 
  the EF is added into the supported PLMN selector list.

DEPENDENCIES
  None
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success for local execution

SIDE EFFECTS
  None
  
===========================================================================*/
 dsat_result_enum_type dsatetsime_get_supp_plmn_selectors
(
  unsigned int *supp_plmn_selectors
)
{
  mmgsdi_len_type              file_length = 0;
  mmgsdi_return_enum_type      mmgsdi_status = MMGSDI_MAX_RETURN_ENUM;
  dsatme_mmgsdi_state_ms_info *me_ms_val = NULL;
  mmgsdi_app_enum_type         app_type = dsat_get_current_gw_app_type(FALSE);

  me_ms_val = dsat_get_base_addr_current_session();
  me_ms_val->dsat_plmn_wact_supported = TRUE;
  *supp_plmn_selectors = 0;

  if (app_type == MMGSDI_APP_SIM)
  {
    mmgsdi_status = (mmgsdi_return_enum_type)MMGSDI_IS_FILE_EXIST
    (
      MMGSDI_GSM_PLMNWACT,
      &file_length
    );

    if ( mmgsdi_status == MMGSDI_SUCCESS )
    {
      *supp_plmn_selectors |= DSAT_SUPP_PLMNWACT;
    }
    else
    {
      *supp_plmn_selectors |= DSAT_SUPP_PLMN;
      me_ms_val->dsat_plmn_wact_supported = FALSE;
    }  
    mmgsdi_status = (mmgsdi_return_enum_type)MMGSDI_IS_FILE_EXIST
    (
      MMGSDI_GSM_OPLMNWACT,
      &file_length
     );

    if ( mmgsdi_status == MMGSDI_SUCCESS  )
    {
      *supp_plmn_selectors|=DSAT_SUPP_OPLMNWACT;
    }

    mmgsdi_status = (mmgsdi_return_enum_type)MMGSDI_IS_FILE_EXIST
   	(
      MMGSDI_GSM_HPLMNACT,
   	  &file_length
    );
    if ( mmgsdi_status == MMGSDI_SUCCESS  )
    {
      *supp_plmn_selectors|=DSAT_SUPP_HPLMNWACT;
    }
  }
  else if (app_type == MMGSDI_APP_USIM)
  {
    mmgsdi_status = (mmgsdi_return_enum_type)MMGSDI_IS_FILE_EXIST
    (
      MMGSDI_USIM_PLMNWACT,
      &file_length
    );

    if ( mmgsdi_status == MMGSDI_SUCCESS )
    {
      *supp_plmn_selectors|=DSAT_SUPP_PLMNWACT;
    }
    else
    {
      *supp_plmn_selectors|=DSAT_SUPP_PLMN;
      me_ms_val->dsat_plmn_wact_supported = FALSE;
    }

    mmgsdi_status = (mmgsdi_return_enum_type)MMGSDI_IS_FILE_EXIST
    (
      MMGSDI_USIM_OPLMNWACT,
      &file_length
    );

    if ( mmgsdi_status == MMGSDI_SUCCESS )
    {
      *supp_plmn_selectors|=DSAT_SUPP_OPLMNWACT;
    }

    mmgsdi_status = (mmgsdi_return_enum_type)MMGSDI_IS_FILE_EXIST
    (
      MMGSDI_USIM_HPLMNWACT,
      &file_length
    );

    if ( mmgsdi_status == MMGSDI_SUCCESS )
    {
      *supp_plmn_selectors|=DSAT_SUPP_HPLMNWACT;
    }
  }
  else
  {
    return DSAT_ERROR;
  }

  return DSAT_OK;
 
}/* dsatetsime_get_supp_plmn_selectors */

/*===========================================================================

FUNCTION DSATETSIME_GET_PLMN_SELECTOR

DESCRIPTION
  This function is used to get the selected PLMN selctor.

DEPENDENCIES
  None
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success for local execution

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsime_get_plmn_selector
(
    dsat_cpls_plmn_selector *plmn_selector
)
{
  dsat_result_enum_type result= DSAT_OK;
  dsatme_mmgsdi_state_ms_info  *me_ms_val = NULL;

  me_ms_val = dsat_get_base_addr_current_session();

  switch(me_ms_val->dsat_sim_filename_plmn_selector)
  {
  case MMGSDI_GSM_PLMN     :
  case MMGSDI_USIM_UPLMNSEL:
   *plmn_selector = DSAT_PLMN;
   break;

  case MMGSDI_GSM_PLMNWACT : 
  case MMGSDI_USIM_PLMNWACT:
   *plmn_selector = DSAT_PLMNWACT;
   break;

  case MMGSDI_GSM_OPLMNWACT: 
  case MMGSDI_USIM_OPLMNWACT: 
   *plmn_selector = DSAT_OPLMNWACT;
   break;

  case MMGSDI_GSM_HPLMNACT: 
  case MMGSDI_USIM_HPLMNWACT: 
   *plmn_selector = DSAT_HPLMNWACT;
   break;
   
  default: 
   result = DSAT_ERROR;
  }

  return result;

}/* dsatetsime_get_plmn_selector */

/*===========================================================================

FUNCTION DSATETSIME_SET_PLMN_SELECTORS

DESCRIPTION
  This function is used to set with the user provided PLMN selctor in the SIM.

DEPENDENCIES
  None
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success for local execution

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsime_set_plmn_selector
(
 dsat_num_item_type cpls_val
)
{
  dsat_result_enum_type        result = DSAT_OK;
  dsatme_mmgsdi_state_ms_info *me_ms_val = NULL;
  mmgsdi_app_enum_type         app_type = dsat_get_current_gw_app_type(FALSE);

  me_ms_val = dsat_get_base_addr_current_session();

  if (app_type == MMGSDI_APP_SIM)
  {
    switch(cpls_val)
    {
      case DSAT_PLMN: 
        if ( me_ms_val->dsat_plmn_wact_supported )
        {
          me_ms_val->dsat_sim_filename_plmn_selector = MMGSDI_GSM_PLMNWACT;
        }
        else
        {
          me_ms_val->dsat_sim_filename_plmn_selector = MMGSDI_GSM_PLMN;
        }
        break;

      case DSAT_OPLMNWACT: 
        me_ms_val->dsat_sim_filename_plmn_selector = MMGSDI_GSM_OPLMNWACT;
        break;

      case DSAT_HPLMNWACT: 
        me_ms_val->dsat_sim_filename_plmn_selector = MMGSDI_GSM_HPLMNACT;
        break;

      default: 
      result = DSAT_ERROR;
    }

  }
  else if (app_type == MMGSDI_APP_USIM)
  {
    switch(cpls_val)
    {
      case DSAT_PLMN:
        if ( me_ms_val->dsat_plmn_wact_supported )
        {
          me_ms_val->dsat_sim_filename_plmn_selector = MMGSDI_USIM_PLMNWACT;
        }
        else
        {
          me_ms_val->dsat_sim_filename_plmn_selector = MMGSDI_USIM_UPLMNSEL;
        }
        break;

      case DSAT_OPLMNWACT: 
        me_ms_val->dsat_sim_filename_plmn_selector = MMGSDI_USIM_OPLMNWACT;
        break;

      case DSAT_HPLMNWACT: 
        me_ms_val->dsat_sim_filename_plmn_selector = MMGSDI_USIM_HPLMNWACT;
        break;

      default:
        result = DSAT_ERROR;
    }
  }

  DS_AT_MSG3_MED("PLMN Selector %d App type %d CPLS value %d",
                 me_ms_val->dsat_sim_filename_plmn_selector,
                 app_type,
                 cpls_val);

  return result;
} /* dsatetsime_set_plmn_selector */
#endif /*FEATURE_MMGSDI */

/*===========================================================================

FUNCTION ETSIME_MSISDN_READ

DESCRIPTION
  This function puts a read MSISDN numbers from PBM for the operational slot 
  of the SIM
  
DEPENDENCIES
  None

RETURN VALUE
  returns and enum that describes the result of execution.
  possible values:
    DSAT_CMD_ERR_RSP : if any error
    DSAT_OK  : if it is a success.
              
SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
LOCAL dsat_result_enum_type etsime_msisdn_read
(
  dsm_item_type *res_buff_ptr
)
{
#define MSISDN_REC_SIZE (PBM_MAX_NUM_LENGTH + PBM_MAX_TEXT_LENGTH + 24) 

  pbm_return_type pbm_return;    
  uint16 size=0;
  uint8 i = 0;
  dsat_pbm_rec_id_type record_id;
  dsat_pbm_record_s_type *msisdn_data_ptr;  
  char text[PBM_MAX_TEXT_LENGTH+1];   
  dsat_cme_error_e_type cme_error;    
  dsat_num_item_type cscs_val ;
  uint8 dsat_handler;
  uint8 is_handler_acquired = FALSE;
  pbm_phonebook_type pb_id_local;
  
  char temp_buff[MSISDN_REC_SIZE];
  msisdn_data_ptr = (dsat_pbm_record_s_type *) dsat_alloc_memory(sizeof(dsat_pbm_record_s_type),FALSE);
  memset(&pb_id_local,0,sizeof (pbm_phonebook_type));
  memset(text,0,(PBM_MAX_TEXT_LENGTH+1));

  dsat_get_phonebook_id(&pb_id_local); 
  pb_id_local.device_type = PBM_MSISDN;

  pbm_return =  pbm_session_enum_rec_init_ext( pb_id_local,
                                           (uint16)PBM_CAT_NONE,
                                           PBM_FIELD_NONE,
                                           (uint8 *)NULL,
                                           0,
                                           PBM_SEARCHTYPE_EXACT|PBM_STRTYPE_UCS2,
                                           &dsat_handler);
  if(pbm_return == PBM_SUCCESS)
  {
    is_handler_acquired = TRUE;
  }

  for(i=0; pbm_return == PBM_SUCCESS; i++)
  {
    if (pbm_return == PBM_SUCCESS )
    {
      pbm_return = pbm_session_enum_next_rec_id_ext(&record_id,dsat_handler);
    }

    if(pbm_return != PBM_SUCCESS)
    {
      /* No records of MSISDN */
      DS_AT_MSG1_HIGH("Number of records in MSISDN %d",i);
      pbm_session_enum_rec_init_ext_free_handle(dsat_handler);
      dsatutil_free_memory((void *) msisdn_data_ptr);
      return DSAT_OK;
    }
    dsatme_cpb_pbm_read_record((dsat_pbm_rec_id_type)record_id, msisdn_data_ptr, &pbm_return);

    if(pbm_return == PBM_SUCCESS)
    {
      ASSERT( res_buff_ptr != NULL );
     /* Convert <text> parm from internal IRA alphabet 
        to TE alphabet if needed */
      cscs_val =(dsat_num_item_type)dsatutil_get_val(
                                             DSATETSI_EXT_CSCS_IDX,0,0,NUM_TYPE);
      if(dsatutil_convert_chset(msisdn_data_ptr->text, 
                                ALPHA_PBM_UCS2,
                                text,
                                (dsati_chset_type)cscs_val,
                                PBM_MAX_TEXT_LENGTH+1,
                                TRUE) == NULL)
      {
        dsatme_set_cme_error(DSAT_CME_UNKNOWN, res_buff_ptr);
        pbm_session_enum_rec_init_ext_free_handle(dsat_handler);
        dsatutil_free_memory((void *) msisdn_data_ptr);
        return DSAT_CMD_ERR_RSP;
      }

     if( strlen((char *)text) )
     {
      size = (word)snprintf((temp_buff),
                           MSISDN_REC_SIZE,
                             "+CNUM: \"%s\",\"%s\",%d\n",
                             text,
                             (msisdn_data_ptr->number),
                             (msisdn_data_ptr->type));
     }
     else
     {
        size = (word)snprintf((temp_buff),
                             MSISDN_REC_SIZE,
                           "+CNUM: %s,\"%s\",%d\n",
                           text,
                           (msisdn_data_ptr->number),
                           (msisdn_data_ptr->type));
     }

      dsat_dsm_pushdown_tail(&res_buff_ptr,
                             temp_buff,
                             size,
                             DSM_ITEM_POOL(res_buff_ptr),
                             FALSE
                            );
    }      
      
    else
    {
      cme_error = dsatme_cpb_cme_error_lookup(pbm_return);

      if (cme_error == DSAT_CME_MAX)
      {
        cme_error = DSAT_CME_UNKNOWN;
      }
    
      ASSERT( res_buff_ptr != NULL );
      dsatme_set_cme_error(cme_error, res_buff_ptr);
      pbm_session_enum_rec_init_ext_free_handle(dsat_handler);
      dsatutil_free_memory((void *) msisdn_data_ptr);
      return DSAT_CMD_ERR_RSP;    
    }
  }
  if(is_handler_acquired == TRUE)
  {
    pbm_session_enum_rec_init_ext_free_handle(dsat_handler);
  }
  DS_AT_MSG1_HIGH("Total records found %d",i);
  dsatutil_free_memory((void *) msisdn_data_ptr);
  return DSAT_OK;
} /* etsime_msisdn_read */

#ifdef FEATURE_MMGSDI
/*===========================================================================

FUNCTION DSATETSIME_REPORT_ICCID

DESCRIPTION
  Handles the asynchronous response from the +ICCID initiated SIM read of the 
  ICCID.

DEPENDENCIES
  None

RETURN VALUE
  Returns the command running status.
    DSAT_CMD_ERR_RSP:  if there was any problem in executing the command 
    DSAT_OK :          if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatetsime_report_iccid
(
  ds_at_mmgsdi_sim_info_type *sim_info_ptr /* MMGSDI SIM info pointer */
)
{
  dsat_result_enum_type   result        = DSAT_OK;
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_SUCCESS;

  mmgsdi_status = sim_info_ptr->cmd_status;

  /* Flag completion of user-initiated GSDI action */
  dsatme_pending_mmgsdi_command[sim_info_ptr->apps_id] = FALSE;

  if (MMGSDI_SUCCESS == mmgsdi_status)
  {
    dsm_item_type *res_buff_ptr;
    byte  *iccid = sim_info_ptr->data;
    byte   tmp_iccid[MMGSDI_ICCID_LEN * 2 + 1];
    uint8  idx1, idx;
    byte   low_iccid = 0;
    byte   high_iccid = 0;

    memset( (void *)tmp_iccid, 0, sizeof(tmp_iccid) );
    for(idx = 0, idx1 = 0;idx < MMGSDI_ICCID_LEN && idx1 < (MMGSDI_ICCID_LEN * 2) ;idx++) 
    {
      if (idx >= sim_info_ptr->data_len)
      {
        DS_AT_MSG0_HIGH("Truncated ICCID as MMGSDI data bound is reached");
        break;
      }

      low_iccid = LOWER_BCD(iccid[idx]);
      high_iccid = UPPER_BCD(iccid[idx]);

      if(low_iccid > 9)
      {
        tmp_iccid[idx1] = 0;
        DS_AT_MSG0_HIGH("Truncated ICCID as a unsupported Char is found ");
        break;
      }
      else if(high_iccid > 9)
      {
        tmp_iccid[idx1++] = '0' + low_iccid;
        tmp_iccid[idx1] = 0;
        DS_AT_MSG0_HIGH("Truncated ICCID as a unsupported Char is found ");
        break;
      }
      else
      {
        tmp_iccid[idx1++] = '0' + low_iccid;
        tmp_iccid[idx1++] = '0' + high_iccid;
      }
    }
    tmp_iccid[MMGSDI_ICCID_LEN * 2] = 0;
    res_buff_ptr = dsat_dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL, FALSE);
    res_buff_ptr->used = (word)snprintf((char*)res_buff_ptr->data_ptr,
                                              res_buff_ptr->size,
                                              "%s%s",
                                              "ICCID: ",
                                              tmp_iccid);
    dsatcmdp_handle_async_cmd_rsp( res_buff_ptr, DSAT_COMPLETE_RSP );
    result = DSAT_OK;
  }/* (MMGSDI_SUCCESS == mmgsdi_status) */
  else
  {
    result = dsat_send_cme_error(DSAT_CME_SIM_FAILURE);
  }

  return result;
}
 
#ifdef FEATURE_DSAT_CFUN_CARD_POWER_CTL

/*===========================================================================

FUNCTION DSATETSIME_MMGSDI_CARD_POWER_CTL_CB

DESCRIPTION
  This function is the callback function for the GSDI to return SIM card
  power control request status: Only posts unsuccessful results to DS task so
  that ERROR result code can be given by cfun_process_card_pwr_ctl_status().

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatetsime_mmgsdi_card_power_ctl_cb
(
  mmgsdi_return_enum_type status, 
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type        *cnf_ptr
)
{

  ds_at_cmd_status_type *status_cmd_ptr = NULL;
  ds_cmd_type           *cmd_buf = NULL;

  DS_AT_MSG2_HIGH("cnf %d, status %d",cnf,status);

  if ( MMGSDI_CARD_PDOWN_CNF == cnf ||
       MMGSDI_CARD_PUP_CNF == cnf )
  {
    /* Only post unsuccessful results to DS task so that ERROR result code can
       be given */
    if ( ( status == MMGSDI_SUCCESS ) || ( status == MMGSDI_REFRESH_SUCCESS ) ||
         (( cnf == MMGSDI_CARD_PUP_CNF)  && ( status == MMGSDI_CAN_NOT_REFRESH)) )
    {
      return;
    }
    
    cmd_buf  = dsat_get_cmd_buf(sizeof(ds_at_cmd_status_type), FALSE);
    status_cmd_ptr = cmd_buf->cmd_payload_ptr;
    /* send the message to ATCOP */
    cmd_buf->hdr.cmd_id    = DS_CMD_ATCOP_STATUS_CMD;
    status_cmd_ptr->cmd_id     = (uint16)CFUN_MMGSDI_CARD_PWR_CTL;
    status_cmd_ptr->cmd_status = status;
      /* cmd_data is byte and client_data allows uint32:
         the field is unused right now but must be restricted to 0-255
         in future */
    status_cmd_ptr->cmd_data = (byte)(cnf_ptr->response_header.client_data);
    ds_put_cmd(cmd_buf);
  }
  else
  {
    DS_AT_MSG1_ERROR("Incorrect CNF type rcvd: %d", cnf);
  }
}
#endif /* FEATURE_DSAT_CFUN_CARD_POWER_CTL */

#ifdef FEATURE_MMGSDI_ATCSIM
/*===========================================================================

FUNCTION DSATETSIME_EXEC_CSIM_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +CSIM command.
  This command allows generic SIM access.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR:        syntax error.
    DSAT_OK          : if it is a success.
    DSAT_ASYNC_CMD   : if this function invokes an async command.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatetsime_exec_csim_cmd
(
  dsat_mode_enum_type  mode,              /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr             /*  Place to put response       */
)
{

  dsat_result_enum_type result = DSAT_OK;
  nv_item_type    nv_item;
  uint8 csim_cmd_enabled = 0;
  mmgsdi_option_type          dsat_opt;       /* Required for sending apdu */
  mmgsdi_send_apdu_data_type  dsat_apdu_data; /* Fill Length & Data for APDU*/
  dsatme_mmgsdi_state_ss_info  *me_ss_val = NULL;

  me_ss_val = dsat_get_base_addr(DSAT_MMGSDI_SS_VALS, FALSE);

  memset(&dsat_opt,0,sizeof(mmgsdi_option_type));
  if(dsatutil_get_nv_item(NV_CSIM_SUPPORT_I, &nv_item) == NV_DONE_S)
  {
    /* NV storage values are different from what ATCOP expects.
       So offsetting by that value. */
    csim_cmd_enabled = nv_item.csim_support;
  }
  
  if(!csim_cmd_enabled)
  {
    dsatme_set_cme_error(DSAT_CME_OP_NOT_SUPPORTED, res_buff_ptr);
    return DSAT_CMD_ERR_RSP;
  }    
     
  /*----------------------------  
   Processing WRITE command :
   -----------------------------*/
  if ( tok_ptr->op == (NA|EQ|AR) &&
       tok_ptr->args_found == 2 )
  {
    /* Both parameters <length> and <command> are mandatory */
    if ( VALID_TOKEN(0) && VALID_TOKEN(1) )
    {
      dsat_num_item_type len;

      /* Length of APDU expressed as hex char pairs must be 10-520 characters, 
         and its length w/o quotes must equal <length> parameter. */
      if ( ATOI_OK != dsatutil_atoi(&len, tok_ptr->arg[0], 10) ||
           len < MIN_CSIM_APDU_LEN || len > MAX_CSIM_APDU_LEN ||
           (len + 2) != strlen((char *)tok_ptr->arg[1]) )
      {
        /* Parameter error */
        return DSAT_ERROR;
      }

      /* APDU must be provided as quoted string */
      if ( !dsatutil_strip_quotes_out( tok_ptr->arg[1], 
                                       tok_ptr->arg[1],
                                       (uint16)(len + 2) ) )
      {
        /* Syntax error */
        return DSAT_ERROR;
      }

        /* convert from HEX char pair string to octets:
           this call will fail and return NULL if other than HEX chars input,
           HEX chars are not upper case or there are not an even number of HEX
           chars. */
        if ( dsatutil_convert_chset( (const char *)tok_ptr->arg[1], 
                                     ALPHA_HEX,
                                     (const char *)&me_ss_val->csim_apdu_data[0],
                                     ALPHA_8BIT,
                                     CSIM_MAX_CMD_APDU_SIZE + 1,
                                     FALSE ) == NULL )
        {
          /* parameter error */
          return DSAT_ERROR;
        }
        me_ss_val->csim_apdu_data_len = (len >> 1);
#ifdef FEATURE_MMGSDI
         dsat_apdu_data.data_len = me_ss_val->csim_apdu_data_len;
         dsat_apdu_data.data_ptr = &me_ss_val->csim_apdu_data[0];
         cb_csim_pending = CSIM_MMGSDI_SEND_APDU;
          result = DSAT_ASYNC_CMD;
         if (MMGSDI_SUCCESS != mmgsdi_send_apdu( me_ss_val->dsat_mmgsdi_client_id,
                               dsat_get_current_slot_id(),
                               dsat_apdu_data,
                               dsat_opt,
                               etsime_mmgsdi_send_apdu_cb,
                              (mmgsdi_client_data_type)cb_csim_pending))
            {
              cb_csim_pending = CB_NONE;
              result = dsat_send_cme_error(DSAT_CME_MEMORY_FAILURE);
            }
#endif /* FEATURE_MMGSDI */
    }
    else
    {
      /* Syntax error */
      result = DSAT_ERROR;
    }
  }

  /*----------------------------  
   Processing TEST command :
   -----------------------------*/
  else if (tok_ptr->op == (NA|EQ|QU))
  {
    result = DSAT_OK;
  }

  /*-------------------------------------------------------
  Any other command should result in an error
  ---------------------------------------------------------*/
  else
  {
    result = DSAT_ERROR;
  }

  return result;
} /* dsatetsime_exec_csim_cmd() */


/*===========================================================================

FUNCTION DSATETSIME_EXEC_CRSM_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +CRSM command.
  This command allows restricted SIM access.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR:        syntax error.
    DSAT_CMD_ERR_RSP:  if SELECT command implied by presence of <file_id>
                       parameter failed.
    DSAT_OK          : if it is a success.
    DSAT_ASYNC_CMD   : if this function invokes an async command.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatetsime_exec_crsm_cmd
(
  dsat_mode_enum_type  mode,              /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr             /*  Place to put response       */
)
{
#define MAX_PARM_VAL 255

  dsat_result_enum_type                  result          = DSAT_OK;
  dsat_num_item_type                     cmd             = 0; 
  dsat_num_item_type                     file_id         = 0xFFFF;
  dsat_num_item_type                     temp            = 0;

  mmgsdi_apdu_params_type                apdu_params;
  mmgsdi_data_type                       data;
  mmgsdi_path_type                       file_path;
  word                                   path_id[MMGSDI_MAX_PATH_LEN]= {0} ;
  dsat_error_info_s_type                 err_info;
  dsatme_mmgsdi_state_ss_info           *me_ss_val       = NULL;

  /*----------------------------  
   Processing WRITE command :
  -----------------------------*/
  if ( tok_ptr->op == (NA|EQ|AR) )
  {
    me_ss_val = dsat_get_base_addr(DSAT_MMGSDI_SS_VALS, FALSE);

    /*memset all the required param types*/
    memset(&apdu_params,0, sizeof(mmgsdi_apdu_params_type));
    memset(&data       ,0, sizeof(mmgsdi_data_type));
    memset(&file_path  ,0, sizeof(mmgsdi_path_type));
    memset(&err_info   ,0, sizeof(dsat_error_info_s_type));

    /* verify command parameter is valid and convertible to one of:
       176 READ BINARY, 178     READ RECORD, 192 GET RESPONSE,214 UPDATE BINARY,
       220 UPDATE RECORD or 242 STATUS. */
    if ( !VALID_TOKEN(0) ||
         ATOI_OK != dsatutil_atoi(&cmd, tok_ptr->arg[0], 10) ||
         ( cmd != CRSM_CMD_RD_BIN  && 
           cmd != CRSM_CMD_RD_REC  &&   
           cmd != CRSM_CMD_GET_RSP &&
           cmd != CRSM_CMD_UPD_BIN &&
           cmd != CRSM_CMD_UPD_REC &&
           cmd != CRSM_CMD_STATUS ) )
    {
      /* parameter error */
      err_info.errval  = DSAT_ERR_ATOI_CONVERSION_FAILURE;
      err_info.arg_num = 0;
      goto send_error;
    }

    /* verify file id if present specifies an elementary file (2F,6F,4F) as 
       specified in 27.007, file id may only be absent for STATUS command. */
    if ( VALID_TOKEN(1) )
    {
      if ( ATOI_OK != dsatutil_atoi(&file_id, tok_ptr->arg[1], 10) )
      {
        /* parameter error */
        err_info.errval  = DSAT_ERR_ATOI_CONVERSION_FAILURE;
        err_info.arg_num = 1;
        goto send_error;
      }
    }
    else
    {
      if ( cmd != CRSM_CMD_STATUS )
      {
        /* syntax error */
        err_info.errval  = DSAT_ERR_PARAMETER_MANDATORY;
        err_info.arg_num = 1;
        goto send_error;
      }
    }

    /* verify per 27.007 that if any of P1..3 are present all of them are and
       that if all are present they are all valid values. */
    if ( VALID_TOKEN(2) || VALID_TOKEN(3) || VALID_TOKEN(4) ) 
    {
      apdu_params.params_valid = TRUE;

      if ( VALID_TOKEN(2) && VALID_TOKEN(3) && VALID_TOKEN(4) )
      {
        dsatutil_atoi(&temp, tok_ptr->arg[2], 10);
        apdu_params.p1 = (uint8)temp;

        dsatutil_atoi(&temp, tok_ptr->arg[3], 10);
        apdu_params.p2 = (uint8)temp;

        dsatutil_atoi(&temp, tok_ptr->arg[4], 10);
        apdu_params.p3 = (uint8)temp;
      }
      else
      {
        /* <P1>, <P2>, <P3>: integer type; parameters passed on by the MT to the SIM. 
           These parameters are mandatory for every command, except GET RESPONSE  */

        if(cmd != CRSM_CMD_GET_RSP && cmd != CRSM_CMD_STATUS)
        {
          err_info.errval  = DSAT_ERR_PARAMETER_OUT_OF_RANGE;
          err_info.arg_num = 2;
          goto send_error;
        }
        else
        {
          if(VALID_TOKEN(2))
          {
            dsatutil_atoi(&temp, tok_ptr->arg[2], 10);
            apdu_params.p1 = (uint8)temp;
          }
          if(VALID_TOKEN(3))
          {
            dsatutil_atoi(&temp, tok_ptr->arg[3], 10);
            apdu_params.p2 = (uint8)temp;
          }
          if(VALID_TOKEN(4))
          {
            dsatutil_atoi(&temp, tok_ptr->arg[4], 10);
            apdu_params.p3 = (uint8)temp;
          }
        }
      }
    }

    if ( VALID_TOKEN(6) )
    {
      memset(path_id, 0, sizeof(path_id));

      file_path.path_len = (uint8)strlen((char *)tok_ptr->arg[6]);
      if(((file_path.path_len - 2) <= 0)  || 
         ((file_path.path_len - 2) % FILE_ID_LEN != 0) ||
        (file_path.path_len/FILE_ID_LEN >= MMGSDI_MAX_PATH_LEN ))
      {
         err_info.errval  = DSAT_ERR_PARAMETER_OUT_OF_RANGE;
         err_info.arg_num = 6;
         goto send_error;
      }
       /* strip quotes from data parameter in place */
      if ( !dsatutil_strip_quotes_out( 
            tok_ptr->arg[6], tok_ptr->arg[6],(word) file_path.path_len ) )
      {
         /* parameter error */
         err_info.errval  = DSAT_ERR_QUOTE_STRIP_FAILURE;
         err_info.arg_num = 6;
         goto send_error;
      }
      
      /*update path_len after removing quotes */
      file_path.path_len  = (file_path.path_len - 2)/FILE_ID_LEN;

      /* We convert the path_id charecter string to octets */
      if(dsatutil_convert_chset( 
          (const char *)tok_ptr->arg[6], 
          ALPHA_HEX,
          (const char *)&path_id[0],
          ALPHA_8BIT,
          sizeof(path_id),
          FALSE ) == NULL )
      {
         /* parameter error */
         err_info.errval  = DSAT_ERR_INVALID_OPERATION;
         err_info.arg_num = 6;
         goto send_error;
      }

      /* Swap the MSB and LSB of all the */
      /* individual words of the converted path_id */
      /* For example if path_id is "003F,207F", after */
      /* swaping it would be "3F00,7F20" */
      if( FALSE == dsatutil_swapn_word_msb_lsb((word *)&path_id,
                                                 (word *)&file_path.path_buf,
                                                  file_path.path_len,
                                                  MMGSDI_MAX_PATH_LEN) )
      {
        err_info.errval  = DSAT_ERR_INVALID_SYNTAX;
        err_info.arg_num = 6;
        goto send_error;
      }
    }

    if ( VALID_TOKEN(5) )
    {
      /* strip quotes from data parameter in place */
      if ( !dsatutil_strip_quotes_out( 
          tok_ptr->arg[5], tok_ptr->arg[5],
         (word)strlen((char *)tok_ptr->arg[5]) ) )
      {
        /* parameter error */
        err_info.errval  = DSAT_ERR_QUOTE_STRIP_FAILURE;
        err_info.arg_num = 5;
        goto send_error;
      }

      /*Copy the buffer from local char * of octects to data pointer*/
      data.data_ptr = (uint8 *)dsat_alloc_memory(strlen((char *)tok_ptr->arg[5] + 1), FALSE);
      data.data_len  = strlen((char *)tok_ptr->arg[5]);

      /* convert from HEX to octets */
      if(dsatutil_convert_chset( (const char *)tok_ptr->arg[5], 
                                            ALPHA_HEX,
                                            (const char *)&data.data_ptr[0],
                                            ALPHA_8BIT,
                                            data.data_len + 1,
                                            FALSE ) == NULL)
      {
        /* parameter error */
        err_info.errval  = DSAT_ERR_INVALID_OPERATION;
        err_info.arg_num = 5;
        dsatutil_free_memory_ext((void **) &data.data_ptr);
        goto send_error;
      }
      data.data_len = data.data_len/2;

      DS_AT_MSG1_HIGH("dsatetsime_exec_crsm_cmd() data_len: %d\n" , data.data_len);
    }

    /*Add file id to path ID after the path_id is inserted*/
    if ( VALID_TOKEN(1) && file_path.path_len < MMGSDI_MAX_PATH_LEN )
    {
      file_path.path_buf[file_path.path_len] = (uint16) file_id;
      file_path.path_len++;
    }

    result = DSAT_ASYNC_CMD;

    DS_AT_MSG4_HIGH("dsatetsime_exec_crsm_cmd() APDU = P1:%d, P2:%d, P3: %d\n, SLOT_ID: " , 
                    apdu_params.p1,
                    apdu_params.p2,
                    apdu_params.p3,
                    dsat_get_current_slot_id());

    /* Call into MMGSDI api for Slot based +CRSM command processing */
    if (MMGSDI_SUCCESS != mmgsdi_restricted_sim_access(me_ss_val->dsat_mmgsdi_client_id,
                                                       dsat_get_current_slot_id(),
                                                       (uint8)cmd,
                                                       apdu_params,
                                                       data,
                                                       file_path,
                                                       etsime_mmgsdi_crsm_cb,
                                                       NULL))
    {
      result = dsat_send_cme_error(DSAT_CME_MEMORY_FAILURE);
    }
    /*Free the allocated data buffer*/
    if (data.data_ptr != NULL)
    {
      dsatutil_free_memory_ext((void **) &data.data_ptr);
    }
  }
  /*----------------------------  
   Processing TEST command :
   -----------------------------*/
  else if (tok_ptr->op == (NA|EQ|QU))
  {
    result = DSAT_OK;
  }

  /*-------------------------------------------------------
  Any other command should result in an error
  ---------------------------------------------------------*/
  else
  {
    result = DSAT_ERROR;
  }

  return result;

send_error:
  DS_AT_MSG2_ERROR ("dsatetsime_exec_crsm_cmd() Err_val: %d, arg_num: %d",
                    err_info.errval,
                    err_info.arg_num);
  return DSAT_ERROR;

} /* dsatetsime_exec_crsm_cmd() */

#endif /* FEATURE_MMGSDI_ATCSIM */


/*===========================================================================

FUNCTION DSATETSIME_GET_NEXT_NETWK_PLMN

DESCRIPTION
  This function is used to get the next PLMN stored on the ME

DEPENDENCIES
  None

RETURN VALUE
  DSAT_ERROR : If unable to get the next PLMN details
  DSAT_ASYNC_CMD: If PLMN details are posted to the DS task.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsime_get_next_netwk_plmn(void)
{
  dsatcmif_servs_state_ss_info       *ph_ss_val   = NULL;
  dsatetsicall_call_state_ss_info    *call_ss_val = NULL;
  dsatme_mmgsdi_state_ss_info        *me_ss_val   = NULL;
  ds_at_mmgsdi_sim_info_type         *sim_info_ptr = NULL;

  ph_ss_val = dsat_get_base_addr(DSAT_SS_PH_SS_VALS, FALSE);
  me_ss_val = dsat_get_base_addr(DSAT_MMGSDI_SS_VALS, FALSE);
  call_ss_val = dsat_get_base_addr(DSAT_CALL_SS_VALS, FALSE);

  cb_expected = COPN_UI_NETWK_BY_INDEX;
  if ( MMGSDI_SUCCESS == mmgsdi_get_se13_network_info_by_index_sync (
                  me_ss_val->dsat_mmgsdi_client_id,
                  (uint16)call_ss_val->dsatetsicall_copn_plmn_index,
                 &ph_ss_val->dsatetsicall_copn_plmn_info) )
  {
    ds_cmd_type *cmd_ptr;

      /* allocate command buffer */
    cmd_ptr = dsat_get_cmd_buf(sizeof(ds_at_mmgsdi_sim_info_type), FALSE);
    sim_info_ptr = cmd_ptr->cmd_payload_ptr;
    /* Send PLMN details to process further */
    cmd_ptr->hdr.cmd_id = DS_CMD_ATCOP_MMGSDI_SIM_INFO;
    sim_info_ptr->client_ref =  (uint32)COPN_UI_NETWK_BY_INDEX;
    sim_info_ptr->session_id = dsat_qcsimapp_info[dsat_get_qcsimapp_val()].session_id;
    ds_put_cmd (cmd_ptr);
    cb_expected = CB_NONE;
  }
  else
  {
    cb_expected = CB_NONE;
    return DSAT_ERROR;
  }

  return DSAT_ASYNC_CMD;
}/* dsatetsime_get_next_netwk_plmn */


/*-------------------------------------------------------------------------
            Function Definitions: Internal functions
-------------------------------------------------------------------------*/

/*--------             pin processing function             --------*/

/*===========================================================================

FUNCTION DSATETSIME_MMGSDI_FILE_CB

DESCRIPTION
  This function is the callback function for the MMGSDI to return
  pin status.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatetsime_mmgsdi_file_attr_rsp_cb
(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type       cnf,
  const mmgsdi_cnf_type     *cnf_ptr
)
{
  ds_cmd_type *cmd_ptr;
  ds_at_mmgsdi_sim_info_type *cmd_rd_ptr;

  ASSERT(cnf_ptr != NULL);

  DS_AT_MSG3_HIGH("cnf = %d session_id = %d client data = %d",
    cnf,cnf_ptr->response_header.session_id,cnf_ptr->response_header.client_data);

  if(status != MMGSDI_SUCCESS)
  {
    DS_AT_MSG1_ERROR("mmgsdi status = %d",status);
    return;
  }

  if(MMGSDI_GET_FILE_ATTR_CNF == cnf)
  {
    if(cnf_ptr->get_file_attr_cnf.file_attrib.file_type == MMGSDI_TRANSPARENT_FILE)
    {
      if(MMGSDI_SUCCESS != mmgsdi_session_read_transparent(
          dsat_get_current_gw_session_id(FALSE),
                 cnf_ptr->get_file_attr_cnf.access,
                 0, cnf_ptr->get_file_attr_cnf.file_attrib.file_size,
                 dsatetsime_mmgsdi_file_attr_rsp_cb,
                 cnf_ptr->response_header.client_data))
      {
        DS_AT_MSG0_ERROR("Mmgsdi get file attr failed");
        return;
      }
    }
    else
    {
      if(MMGSDI_SUCCESS != mmgsdi_session_read_record(
          dsat_get_current_gw_session_id(FALSE),
                 cnf_ptr->get_file_attr_cnf.access,
                 1, cnf_ptr->get_file_attr_cnf.file_attrib.file_size,
                 dsatetsime_mmgsdi_file_attr_rsp_cb,
                 cnf_ptr->response_header.client_data))
      {
        DS_AT_MSG0_ERROR("Mmgsdi get file attr failed");
        return;
      }
    }
  }
  else if((MMGSDI_READ_CNF == cnf)||(MMGSDI_WRITE_CNF == cnf))
  {
       
    /* allocate command buffer */
    cmd_ptr = dsat_get_cmd_buf(sizeof(ds_at_mmgsdi_sim_info_type), FALSE);
    cmd_rd_ptr = cmd_ptr->cmd_payload_ptr;

    cmd_rd_ptr->cmd_status = cnf_ptr->response_header.mmgsdi_status;
    cmd_ptr->hdr.cmd_id    = DS_CMD_ATCOP_MMGSDI_SIM_INFO;
    cmd_rd_ptr->client_ref = cnf_ptr->response_header.client_data;
    cmd_rd_ptr->session_id = cnf_ptr->response_header.session_id;
    
    switch(cnf_ptr->response_header.client_data)
    {
      case CPUC_MMGSDI_SIM_READ:
      case ICCID_MMGSDI_SIM_READ:
        if(NULL != cnf_ptr->read_cnf.read_data.data_ptr)
        {
          cmd_rd_ptr->data_len = cnf_ptr->read_cnf.read_data.data_len;
        (void) dsatutil_memscpy((void*)&cmd_rd_ptr->data[0],
                MAX_MMGSDI_BUFFER_LEN,(void*)&cnf_ptr->read_cnf.read_data.data_ptr[0],
                cmd_rd_ptr->data_len );
        }
        else
        {
          DS_AT_MSG1_ERROR("Received data NULL pointer ,Status %d",
                          cnf_ptr->response_header.mmgsdi_status);
          cmd_rd_ptr->data_len = 0;
        }
        break;
      case CPUC_MMGSDI_SIM_WRITE:
      default:
        break;
    }
    ds_put_cmd(cmd_ptr);
  }
  else
  {
    DS_AT_MSG1_HIGH("Unknown MMGSDI confirmation type: %d",cnf);
  }
}

/*===========================================================================

FUNCTION REQ_MMGSDI_SIM_READ

DESCRIPTION
  This function sends MMGSDI a message to read the EF from the card.
  The input parameter maps to a specific EF.
                  

DEPENDENCIES
  None

RETURN VALUE
  returns boolean:
    FALSE: if dsat_qcslot_val is not GSM operational slot
    TRUE:  executing the GSDI async call and waiting for the callback function
           to be called.

SIDE EFFECTS
  If TRUE return value message is sent to GSDI, etsime_mmgsdi_send_apdu_cb will
  be called with IMSI info.

===========================================================================*/
boolean req_mmgsdi_sim_read(cb_cmd_e_type cb_cmd)
{
  mmgsdi_app_enum_type app_type = dsat_get_current_gw_app_type(FALSE);
  mmgsdi_access_type   file_name;

  DS_AT_MSG1_HIGH("command_id = %d", cb_cmd);

  switch(cb_cmd)
  {
  case COPS_MMGSDI_SIM_READ:
    if (app_type == MMGSDI_APP_USIM)
    {
      file_name.file.file_enum = MMGSDI_USIM_IMSI;
    }
    else if (app_type == MMGSDI_APP_SIM)
    {
      file_name.file.file_enum = MMGSDI_GSM_IMSI;
    }
    else 
    {
      DS_AT_MSG1_ERROR("NO APPS AVAILABLE %d", app_type);
      /* Default to USIM IMSI */
      file_name.file.file_enum = MMGSDI_USIM_IMSI;
    }
    break;
    
  case CPUC_MMGSDI_SIM_READ:
    if (app_type == MMGSDI_APP_USIM)
    {
      file_name.file.file_enum = MMGSDI_USIM_PUCT;
    }
    else if (app_type == MMGSDI_APP_SIM)
    {
      file_name.file.file_enum = MMGSDI_GSM_PUCT;
    }
    else 
    {
      DS_AT_MSG0_ERROR("NO APPS AVAILABLE");
      /* Default to USIM IMSI */
      file_name.file.file_enum = MMGSDI_GSM_PUCT;
    }
    break;

  case ICCID_MMGSDI_SIM_READ:
    file_name.file.file_enum = MMGSDI_ICCID;
    break;
    
  case CUAD_MMGSDI_SIM_READ:
    file_name.file.file_enum = MMGSDI_DIR;
    break;
	
  default:
    DS_AT_MSG0_ERROR("UNKNOWN SIM FILE");
    file_name.file.file_enum = MMGSDI_MAX_FILE_ENUM;
    break;
  }
  file_name.access_method = MMGSDI_EF_ENUM_ACCESS;

  if(MMGSDI_SUCCESS == mmgsdi_session_get_file_attr_ext(
      dsat_get_current_gw_session_id(FALSE),
                 file_name,
                 dsatetsime_mmgsdi_file_attr_rsp_cb,
                 (mmgsdi_client_data_type)cb_cmd,
                 TRUE ))
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
} /* req_mmgsdi_sim_read*/

/*===========================================================================

FUNCTION REQ_MMGSDI_SIM_WRITE

DESCRIPTION
  This function sends MMGSDI a message to write the EF on the card,
  The input parameter maps to a specific EF.
                  

DEPENDENCIES
  None

RETURN VALUE
  returns boolean:
    FALSE: if dsat_qcslot_val is not GSM operational slot
    TRUE:  executing the GSDI async call and waiting for the callback function
           to be called.

SIDE EFFECTS
  If TRUE return value message is sent to GSDI, etsime_mmgsdi_send_apdu_cb will
  be called with IMSI info.

===========================================================================*/
boolean req_mmgsdi_sim_write( cb_cmd_e_type cb_cmd,
                            byte * buffer,
                            uint8  buflen )
{
  mmgsdi_access_type     file_name;
  mmgsdi_write_data_type write_data;
  mmgsdi_app_enum_type   app_type = dsat_get_current_gw_app_type(FALSE);

  write_data.data_len = buflen;
  write_data.data_ptr = (uint8 *)buffer;

  DS_AT_MSG1_HIGH("command_id = %d",cb_cmd);
  switch(cb_cmd)
  {
    case CPUC_MMGSDI_SIM_WRITE:
      if (app_type == MMGSDI_APP_USIM)
      {
        file_name.file.file_enum = MMGSDI_USIM_PUCT;
      }
      else if (app_type == MMGSDI_APP_SIM)
      {
        file_name.file.file_enum = MMGSDI_GSM_PUCT;
      }
      else 
      {
        DS_AT_MSG0_ERROR("NO APPS AVAILABLE");
          /* Default to USIM IMSI */
          file_name.file.file_enum = MMGSDI_GSM_PUCT;
      }
      break;
    
    default:
      DS_AT_MSG0_ERROR("UNKNOWN SIM FILE");
      file_name.file.file_enum = MMGSDI_MAX_FILE_ENUM;
    break;
  }
  file_name.access_method = MMGSDI_EF_ENUM_ACCESS;

  if(MMGSDI_SUCCESS == mmgsdi_session_write_transparent(
      dsat_get_current_gw_session_id(FALSE),
             file_name,
             0, write_data,
             dsatetsime_mmgsdi_file_attr_rsp_cb,
             (mmgsdi_client_data_type)cb_cmd))
  {
    DS_AT_MSG0_ERROR("Mmgsdi get file attr failed");
    return FALSE;
  }
  else
  {
    return TRUE;
  }
} /* req_mmgsdi_sim_write*/


/*===========================================================================

FUNCTION ETSIME_DECODE_IMSI

DESCRIPTION
  This function decodes the IMSI data from the GSDI into a string
  representation.  The IMSI type consists of a length and data array.
  Each array element represents the BCD form of two digits in reversed order.
  The first byte is special in that the lower nibble is ignored.

  Example:
       Length =  8
       Data   =  0x09 0x10 0x10 0x10 0x32 0x54 0x76 0x98

       Decoded = 001010123456789
  
DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the execution.
  possible values:
    DSAT_OK :    if successful
    DSAT_ERROR : if there was any problem

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
LOCAL dsat_result_enum_type etsime_decode_imsi
(
  ds_at_sim_info_type * imsi_ptr,    /* Structure of GDSI data */
  byte * imsi_buf               /* Output string buffer */
)
{
  int index;
  byte odd_digits;
  byte * s_ptr = imsi_buf;
 
  ASSERT(( NULL != imsi_ptr ) && ( NULL != imsi_buf ));
  
  /* Validate the input IMSI data */
  if( (0 == imsi_ptr->length) ||
      (MMGSDI_IMSI_NOT_PROVISIONED == imsi_ptr->length) ||
      (MMGSDI_MIN_VALID_IMSI_LEN > imsi_ptr->length) ||
      (MMGSDI_IMSI_LEN < imsi_ptr->length) )
  {
    DS_AT_MSG1_ERROR("No valid IMSI present to convert - length: %d",
              imsi_ptr->length);
    return DSAT_ERROR;
  }

  odd_digits = imsi_ptr->data[0] & 0x08;
  
  /* Decode IMSI value from nibbles */
  for(index = 0; index < imsi_ptr->length; index++)
  {
    if( index == 0) /* first byte, ignore lower nibble */
    {
      *s_ptr++ = UPPER_BCD( imsi_ptr->data[index] ) + '0';
    }
    else if( index == imsi_ptr->length -1) /* last byte */
    {
      *s_ptr++ = LOWER_BCD( imsi_ptr->data[index] ) + '0';
      if(odd_digits) /* count the last one if odd digits */
        *s_ptr++ = UPPER_BCD( imsi_ptr->data[index] ) + '0';
    }
    else
    {
      *s_ptr++ = LOWER_BCD( imsi_ptr->data[index] ) + '0';
      *s_ptr++ = UPPER_BCD( imsi_ptr->data[index] ) + '0';
    }
  }
  
  /* Terminate string */
  *s_ptr = '\0';
  return DSAT_OK;
} /* etsime_decode_imsi */


/*===========================================================================

FUNCTION ME_COPY_PIN

DESCRIPTION
  This function copies the PIN string to a 0xFF padded array

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Message is sent to GSDI.  dsatetsime_gsdi_req_cb will be called with
  pin status.

===========================================================================*/
LOCAL void me_copy_pin
(
  byte * array,                       /* PIN padded array */
  byte * string                       /* PIN string */
)
{
  byte c;
  int count = 0;

  /* Initialize array */
  memset(array, 0xFF, PIN_MAX_LEN);

  /* Transfer characters to array */
  while (( (c = *string++) != '\0') && (PIN_MAX_LEN >= count++))
  {
    *array++ = c;
  }
  return;
} /* me_copy_pin */

#if defined(FEATURE_8960_SGLTE_FUSION) || defined(FEATURE_8960_DSDA_FUSION)

/*===========================================================================
FUNCTION DSAT_GET_PROVISIONED_VAL

DESCRIPTION
  This function will return state of apps provisioning.

DEPENDENCIES
  None

RETURN VALUE
value of dsat_app_provisioned

SIDE EFFECTS
  NONE

===========================================================================*/

dsat_mmgsdi_ready_state dsat_get_provision_state()
{
  dsatme_mmgsdi_state_ss_info  *me_ss_val = NULL;
  me_ss_val = dsat_get_base_addr(DSAT_MMGSDI_SS_VALS, FALSE);

  return me_ss_val->dsat_apps_provisioned;
}/*dsat_get_provision_state*/

/*===========================================================================
FUNCTION DSAT_SET_PROVISIONED_VAL

DESCRIPTION
  This function will set state of apps provisioning.

DEPENDENCIES
  None

RETURN VALUE
value of dsat_app_provisioned

SIDE EFFECTS
  NONE

===========================================================================*/


void dsat_set_provision_state(dsat_mmgsdi_ready_state provisioned_val)
{
  dsatme_mmgsdi_state_ss_info  *me_ss_val = NULL;

  me_ss_val = dsat_get_base_addr(DSAT_MMGSDI_SS_VALS, FALSE);
  me_ss_val->dsat_apps_provisioned = provisioned_val;
}/*dsat_set_provision_state*/

#endif /*defined(FEATURE_8960_SGLTE_FUSION) || defined(FEATURE_8960_DSDA_FUSION)*/

#ifdef FEATURE_DATA_ETSI_PIN

#ifdef FEATURE_MMGSDI
/*===========================================================================

FUNCTION  ME_PROCESS_CPIN_STATE

DESCRIPTION
  This function acts as the state machine for the +CPIN command handler.
  Based on the PIN status bitfield array, the appropriate request to MMGSDI is
  initiated.  All call to MMGSDI are done asychronously.

  The state machine uses the following PIN precendence based on processing
  order:
      SLOT1 PUK1  
      SLOT1 PIN1
      SLOT2 PUK1  (if FEATURE_DUAL_SLOTS defined)
      SLOT2 PIN1  (if FEATURE_DUAL_SLOTS defined)
      Any Personalization PIN/PUK
      SLOT1 PUK2
      SLOT1 PIN2
      SLOT2 PUK2  (if FEATURE_DUAL_SLOTS defined)
      SLOT2 PIN2  (if FEATURE_DUAL_SLOTS defined)
  
DEPENDENCIES
  None

RETURN VALUE
  DSAT_ASYNC_CMD : if async call successfully made
  DSAT_ERROR     : if async call failed due to bad parameters

SIDE EFFECTS
  None

===========================================================================*/
LOCAL dsat_result_enum_type me_process_cpin_state
(
  const tokens_struct_type *tok_ptr,       /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr              /*  Place to put response       */
)
{
  dsat_result_enum_type  result = DSAT_ASYNC_CMD; 
  mmgsdi_return_enum_type mmgsdi_status;
  mmgsdi_data_type pin_data;
  dsat_mixed_param_val_type *temp0_mix_type;
  dsat_mixed_param_val_type *temp1_mix_type;
  dsat_apps_id_e_type apps_id;
  dsatme_mmgsdi_state_ms_info  *me_ms_val = NULL;
  simlock_unlock_device_msg_req_type  req_data;
  dsat_perso_user_type user_data;

  memset (&req_data, 0, sizeof(simlock_unlock_device_msg_req_type));

  apps_id   = dsat_get_current_gw_apps_id(FALSE);
  me_ms_val = dsat_get_base_addr_per_session(apps_id, FALSE);

  /* Macro to verify PIN/PUK max length of GSDI_MAX_PIN_LEN:
     If a personalization PIN/PUK is being entered a max length of 
     MMGSDI_PERSO_MAX_CK is allowable and will already have been verified in
     the command handler by the default parameter command handler, but if it 
     is not a personalization PIN/PUK it must be limited to GSDI_MAX_PIN_LEN 
     characters. */
#define PIN_LEN_OK(pinstr)\
  (strlen((char *)pinstr->string_item) <= MMGSDI_PIN_MAX_LEN)
  
  /* State machine based on PIN/PUK state bit arrays: */
  /* SLOT1 PUK1 */
  if ( IS_PUK_STATE(DSAT_CPIN_SIM_PIN_REQUIRED) )
  {
    temp0_mix_type  = (dsat_mixed_param_val_type *)dsatutil_get_val(
            DSATETSI_EXT_ACT_CPIN_ETSI_IDX,apps_id,0,MIX_INDEX_TYPE);
    temp1_mix_type  =(dsat_mixed_param_val_type *)dsatutil_get_val(
            DSATETSI_EXT_ACT_CPIN_ETSI_IDX,apps_id,1,MIX_INDEX_TYPE);
    if ( PIN_LEN_OK(temp0_mix_type) && PIN_LEN_OK(temp1_mix_type) )
    {
      result = me_cpin_unblock_pin ( tok_ptr, res_buff_ptr,
                                     MMGSDI_PIN1, MMGSDI_SLOT_1 );
    }
    else
    {
      result = DSAT_ERROR;
    }
  }

  /* SLOT1 PIN1 */
  else if ( IS_PIN_STATE(DSAT_CPIN_SIM_PIN_REQUIRED) )
  {
    temp0_mix_type  = (dsat_mixed_param_val_type *)dsatutil_get_val(
            DSATETSI_EXT_ACT_CPIN_ETSI_IDX,apps_id,0,MIX_INDEX_TYPE);
    if ( PIN_LEN_OK(temp0_mix_type) ) 
    {
      pin_data.data_ptr = (uint8 *)temp0_mix_type->string_item;
#ifdef FEATURE_DSAT_CUST
      #error code not present
#else
      pin_data.data_len = (mmgsdi_len_type)strlen((char*)temp0_mix_type->string_item);
#endif /* FEATURE_DSAT_CUST */

      /* Verify PIN password */
      VERIFY_PIN( dsat_get_current_gw_session_id(FALSE),
                  MMGSDI_PIN1, 
                  pin_data, 
                  CPIN_MMGSDI_PIN1_VERIFY );

      if (mmgsdi_status == MMGSDI_SUCCESS)
      {
        result = DSAT_ASYNC_CMD;
      }
      else
      {
        result = DSAT_ERROR;
      }
    }
    else
    {
      result = DSAT_ERROR;
    }
  }

#ifdef FEATURE_MMGSDI_PERSONALIZATION
  /* Any personalization PUK either slot blocked:
     only 1 should be active at a time according to GSDI */
  else if ( me_ms_val->dsat_pin_blocked & ((uint32)DSAT_CPIN_NET_PIN_REQUIRED 
                              | (uint32)DSAT_CPIN_NETSUB_PIN_REQUIRED 
                              | (uint32)DSAT_CPIN_SP_PIN_REQUIRED 
                              | (uint32)DSAT_CPIN_CORP_PIN_REQUIRED 
                              | (uint32)DSAT_CPIN_PH_FSIM_PIN_REQUIRED
          ) )
  {
    /* Don't support clearing of personalization PUKs */
    result = dsat_send_cme_error(DSAT_CME_OP_NOT_SUPPORTED);
  }

  /* Any personalization PIN slot 1 required:
     only 1 should be active at a time according to GSDI */
  else if ( me_ms_val->dsat_pin_required & ((uint32)DSAT_CPIN_NET_PIN_REQUIRED    |
                                 (uint32)DSAT_CPIN_NETSUB_PIN_REQUIRED |
                                 (uint32)DSAT_CPIN_SP_PIN_REQUIRED     |
                                 (uint32)DSAT_CPIN_CORP_PIN_REQUIRED   |
                                 (uint32)DSAT_CPIN_PH_FSIM_PIN_REQUIRED) )
  {
    user_data.session_id = dsat_get_current_gw_session_id(FALSE);
    user_data.perso_cmd = CPIN_MMGSDI_PERSO_DEACT;
    switch (dsat_get_current_slot_id())
    {
      case MMGSDI_SLOT_1:
        req_data.slot = SIMLOCK_SLOT_1;
        break;
      case MMGSDI_SLOT_2:
        req_data.slot = SIMLOCK_SLOT_2;
        break;
      case MMGSDI_SLOT_3:
        req_data.slot = SIMLOCK_SLOT_3;
        break;
      default:
        return DSAT_ERROR;
        break;
    }
    user_data.slot = req_data.slot;
    if ( me_ms_val->dsat_pin_required & (uint32)DSAT_CPIN_NET_PIN_REQUIRED )
    {
      req_data.category = SIMLOCK_CATEGORY_3GPP_NW;
    }
    else if ( me_ms_val->dsat_pin_required & (uint32)DSAT_CPIN_NETSUB_PIN_REQUIRED )
    {
      req_data.category = SIMLOCK_CATEGORY_3GPP_NS;
    }
    else if ( me_ms_val->dsat_pin_required & (uint32)DSAT_CPIN_SP_PIN_REQUIRED )
    {
      req_data.category = SIMLOCK_CATEGORY_3GPP_SP;
    }
    else if ( me_ms_val->dsat_pin_required & (uint32)DSAT_CPIN_CORP_PIN_REQUIRED  )
    {
      req_data.category = SIMLOCK_CATEGORY_3GPP_CP;
    }
    else if ( me_ms_val->dsat_pin_required & (uint32)DSAT_CPIN_PH_FSIM_PIN_REQUIRED  )
    {
      req_data.category = SIMLOCK_CATEGORY_3GPP_SIM;
    }
    
    req_data.ck.simlock_ck_length     = (uint8)strlen((char *) dsatutil_get_val(
              DSATETSI_EXT_ACT_CPIN_ETSI_IDX,apps_id,0,MIX_STR_TYPE));
    dsatutil_memscpy(req_data.ck.simlock_ck_data,SIMLOCK_CK_MAX,
        (void *) dsatutil_get_val(DSATETSI_EXT_ACT_CPIN_ETSI_IDX,apps_id,0,MIX_STR_TYPE),req_data.ck.simlock_ck_length);
    if(SIMLOCK_SUCCESS != simlock_unlock(req_data,dsatme_sim_mmgsdi_perso_req_cb,&user_data))
    {
      result = DSAT_ERROR;
    }
  }

#endif /* FEATURE_MMGSDI_PERSONALIZATION */


  /* SLOT1 PUK2 */
  else if ( IS_PUK_STATE(DSAT_CPIN_SIM_PIN2_REQUIRED) )
  {
    temp0_mix_type  = (dsat_mixed_param_val_type *)dsatutil_get_val(
            DSATETSI_EXT_ACT_CPIN_ETSI_IDX,apps_id,0,MIX_INDEX_TYPE);
    temp1_mix_type  =(dsat_mixed_param_val_type *)dsatutil_get_val(
            DSATETSI_EXT_ACT_CPIN_ETSI_IDX,apps_id,1,MIX_INDEX_TYPE);
    if ( PIN_LEN_OK(temp0_mix_type) && PIN_LEN_OK(temp1_mix_type) ) 
    {
      result = me_cpin_unblock_pin ( tok_ptr, res_buff_ptr,
                                     MMGSDI_PIN2, MMGSDI_SLOT_1 );
    }
    else
    {
      result = DSAT_ERROR;
    }
  }
  /* SLOT1 PIN2 */
  else if ( IS_PIN_STATE(DSAT_CPIN_SIM_PIN2_REQUIRED) )
  {
    temp0_mix_type  = (dsat_mixed_param_val_type *)dsatutil_get_val(
            DSATETSI_EXT_ACT_CPIN_ETSI_IDX,apps_id,0,MIX_INDEX_TYPE);
    if ( PIN_LEN_OK(temp0_mix_type) ) 
    {
      pin_data.data_ptr = (uint8 *)temp0_mix_type->string_item;
      pin_data.data_len = (mmgsdi_len_type)strlen((char*) temp0_mix_type->string_item);

      /* Verify PIN password */
      VERIFY_PIN( dsat_get_current_gw_session_id(FALSE),
                  MMGSDI_PIN2, 
                  pin_data, 
                  CPIN_MMGSDI_PIN2_VERIFY );

      if (mmgsdi_status == MMGSDI_SUCCESS)
      {
        result = DSAT_ASYNC_CMD;
      }
      else
      {
        result = DSAT_ERROR;
      }
    }
    else
    {
      result = DSAT_ERROR;
    }
  }
  
  /* ERROR CONDITION */
  else
  {
    DS_AT_MSG1_MED("Unrecognized PIN state: %d",me_ms_val->dsat_pin_required);
    result = DSAT_ERROR;
  }
  
  /* Clear the passwords from memory to be safe */
  dsatutil_reset_password(dsatutil_get_val(
    DSATETSI_EXT_ACT_CPIN_ETSI_IDX,apps_id,0,MIX_STR_TYPE), PIN_MAX_LEN+1);
  dsatutil_reset_password(dsatutil_get_val(
    DSATETSI_EXT_ACT_CPIN_ETSI_IDX,apps_id,1,MIX_STR_TYPE), PIN_MAX_LEN+1);
  
  if ( result == DSAT_ASYNC_CMD )
  {
    /* Flag command-mode GSDI action underway */ 
    dsatme_pending_mmgsdi_command[apps_id] = TRUE;
  }
  
  return result; 
} /* me_process_cpin_state */


/*===========================================================================

FUNCTION  ME_CPIN_UNBLOCK_PIN

DESCRIPTION
  This function verifies the unblocking code and new PIN code are specified
  then initiates interaction with GSDI to unblock specified SIM PIN.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the execution.
  possible values:
    DSAT_CMD_ERR_RSP: if +CME ERROR is being generated
    DSAT_ASYNC_CMD :  excuting the action command and waiting for the
                      callback function be called.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
LOCAL dsat_result_enum_type me_cpin_unblock_pin
(
  const tokens_struct_type *tok_ptr,       /* Command tokens from parser  */
  dsm_item_type *res_buff_ptr,             /* Place to put response       */
  const mmgsdi_pin_enum_type pin_id,       /* PIN identifier              */
  const mmgsdi_slot_id_enum_type slot_id   /* SIM slot identifier         */
)
{
  dsat_result_enum_type  result = DSAT_ASYNC_CMD;
  mmgsdi_return_enum_type       mmgsdi_result;
  mmgsdi_data_type old_pin_data, new_pin_data;
  dsat_apps_id_e_type apps_id = dsat_get_current_gw_apps_id(FALSE);

  /* Verify user entered newpin */
  if (2 == tok_ptr->args_found)
  {
    /* Initiate request to GSDI */
    cb_cmd_e_type req_submitter = (MMGSDI_PIN1 == pin_id)?
      CPIN_MMGSDI_PIN1_VERIFY : CPIN_MMGSDI_PIN2_VERIFY;

    old_pin_data.data_ptr =(uint8 *)dsatutil_get_val(
                               DSATETSI_EXT_ACT_CPIN_ETSI_IDX,apps_id,0,MIX_STR_TYPE);

#ifdef FEATURE_DSAT_CUST
    #error code not present
#else
    old_pin_data.data_len = (int32)strlen((char*)dsatutil_get_val(
                              DSATETSI_EXT_ACT_CPIN_ETSI_IDX,apps_id,0,MIX_STR_TYPE));
#endif /* FEATURE_DSAT_CUST */

    new_pin_data.data_ptr =(uint8 *)dsatutil_get_val(
                               DSATETSI_EXT_ACT_CPIN_ETSI_IDX,apps_id,1,MIX_STR_TYPE);
#ifdef FEATURE_DSAT_CUST
    #error code not present
#else
    new_pin_data.data_len = (int32)strlen((char*)dsatutil_get_val(
                              DSATETSI_EXT_ACT_CPIN_ETSI_IDX,apps_id,1,MIX_STR_TYPE));
#endif /* FEATURE_DSAT_CUST */

    UNBLOCK_PIN(slot_id,
                pin_id, 
                old_pin_data, 
                new_pin_data, 
                req_submitter );

    if (mmgsdi_result == MMGSDI_SUCCESS)
    {
      result = DSAT_ASYNC_CMD;
    }
    else
    {
      /* MMGSDI command queue is full */
      dsatme_set_cme_error(DSAT_CME_MEMORY_FULL, res_buff_ptr);
      result = DSAT_CMD_ERR_RSP;
    }
  }
  else
  {
    /* Report error */
    dsat_cme_error_e_type code = DSAT_CME_SIM_PUK_REQUIRED;
    dsatme_set_cme_error(code, res_buff_ptr);
    result = DSAT_CMD_ERR_RSP;
  }

  return result;
} /* me_cpin_unblock_pin */
#endif /* FEATURE_MMGSDI */
#endif /* FEATURE_DATA_ETSI_PIN */

#ifdef FEATURE_MMGSDI_ATCSIM

#ifdef FEATURE_MMGSDI
/*===========================================================================

FUNCTION  DSATETSIME_MMGSDI_SEND_APDU_HANDLER

DESCRIPTION
  This function is the handler function for the CSIM/CRSM  events

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_CMD_ERR_RSP:  if there was any problem in executing the command 
    DSAT_OK :          if it is a success.
    DSAT_ASYNC_EVENT : async event, contine processing  

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatetsime_mmgsdi_send_apdu_handler
(
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
)
{
  dsat_result_enum_type        result        = DSAT_ASYNC_CMD;
  cb_cmd_e_type                client_ref    = CB_NONE;
  dsatme_mmgsdi_state_ss_info  *me_ss_val    = NULL;

  ds_at_mmgsdi_apdu_resp_type *mmgsdi_apdu_resp_ptr = NULL;
   
  me_ss_val            = dsat_get_base_addr(DSAT_MMGSDI_SS_VALS, FALSE);
  mmgsdi_apdu_resp_ptr = dsat_get_cmd_payload_ptr(cmd_ptr);
  client_ref           = (cb_cmd_e_type)mmgsdi_apdu_resp_ptr->client_ref;

  DS_AT_MSG1_HIGH("Client ref = %d",client_ref);

  switch (client_ref)
  {
     case CSIM_MMGSDI_SEND_APDU:
      if ( (mmgsdi_apdu_resp_ptr->cmd_status  == MMGSDI_SUCCESS) && 
           (NULL != mmgsdi_apdu_resp_ptr->csim_apdu_data_ptr)
         )
      {
          /* The data in csim_apdu_data[] is valid GET_RESPONSE data */
        me_ss_val->csim_apdu_data_len = mmgsdi_apdu_resp_ptr->apdu_len;
          /* SW2 cannot indicate more response data is available than was
             actually received */
          /* Output response APDU created as +CSIM command response */
        if ( (uint32)CSIM_MMGSDI_SEND_APDU == mmgsdi_apdu_resp_ptr->client_ref )
        {
          result = etsime_report_csim( mmgsdi_apdu_resp_ptr->apdu_len, 
                                       (byte*)mmgsdi_apdu_resp_ptr->csim_apdu_data_ptr);
         }
      }
      else
      {
         /* Report +CME error */
        result = dsat_send_cme_error(DSAT_CME_PHONE_FAILURE);
      }
    break;
    
    default:
    {
      DS_AT_MSG1_LOW("Unsupported MMGSDI SIM read event: %d",
                     mmgsdi_apdu_resp_ptr->client_ref);
      result = dsat_send_cme_error(DSAT_CME_UNKNOWN);
    }
    break;
  }
  dsatutil_free_memory( (void*) mmgsdi_apdu_resp_ptr->csim_apdu_data_ptr);
  return result;
} /* dsatetsime_mmgsdi_send_apdu_handler */

/*===========================================================================

FUNCTION  DSATETSIME_MMGSDI_CRSM_HANDLER

DESCRIPTION
  This function is the handler function for the CRSM response handler

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_CMD_ERR_RSP:  if there was any problem in executing the command 
    DSAT_OK :          if it is a success.
    DSAT_ASYNC_EVENT : async event, contine processing  

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsime_mmgsdi_crsm_handler
(
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
)
{
  dsat_result_enum_type         result         = DSAT_ASYNC_CMD;
  
  ds_at_crsm_info_type *crsm_info = dsat_get_cmd_payload_ptr(cmd_ptr);

  if(cmd_ptr->hdr.cmd_id != DS_CMD_ATCOP_MMGSDI_CRSM_RESP)
  {
    return DSAT_ERROR;
  }

  DS_AT_MSG3_HIGH("dsatetsime_mmgsdi_crsm_handler(): %d, SW1:%d SW2: %d",
                  crsm_info->cmd_status,
                  crsm_info->sw1,
                  crsm_info->sw2);

  if ( crsm_info->cmd_status == MMGSDI_SUCCESS)
  {
    /*report +CRSM here*/
    result = etsime_report_crsm(crsm_info->sw1,
                                crsm_info->sw2,
                                (uint32)crsm_info->data.data_len,
                                (byte *)crsm_info->data.data_ptr);
  }
  else
  {
    if (crsm_info->sw1 || crsm_info->sw2)
    {
      /*report +CRSM here*/
      result = etsime_report_crsm(crsm_info->sw1,
                                  crsm_info->sw2,
                                  0,
                                  NULL);
    }
    else
    {
      /* Report +CME error */
      result = dsat_send_cme_error(DSAT_CME_UNKNOWN);
    }
  }

  if (crsm_info->data.data_ptr != NULL)
  {
    dsatutil_free_memory_ext( (void**) &crsm_info->data.data_ptr);
  }
  return result;
} /* dsatetsime_mmgsdi_crsm_handler */

/*===========================================================================
FUNCTION ETSIME_MMGSDI_CRSM_CB

DESCRIPTION
  This function handles the Callback for +CRSM response callback.  

DEPENDENCIES
  None


RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
LOCAL void etsime_mmgsdi_crsm_cb
(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type       cnf,
  const mmgsdi_cnf_type     *cnf_ptr
)
{
  ds_cmd_type                                 *cmd_ptr           = NULL;
  const mmgsdi_response_header_type           *cnf_hdr_ptr       = NULL;
  const mmgsdi_restricted_sim_access_cnf_type *sim_info          = NULL;
  ds_at_crsm_info_type                        *dsat_crsm_info    = NULL;
/*------------------------------------------------------------------------*/
  
  ASSERT(cnf_ptr != NULL);

  cnf_hdr_ptr = &cnf_ptr->restricted_sim_access_cnf.response_header;
  sim_info    = &cnf_ptr->restricted_sim_access_cnf;
  DS_AT_MSG3_HIGH("MMGSDI CRSM CB  cnf=%d  status=%d Client data = %d",
                                    cnf,status,cnf_hdr_ptr->client_data);
  
  DS_AT_MSG1_HIGH("MMGSDI CRSM response type = %d",cnf_hdr_ptr->response_type);

  switch (cnf_hdr_ptr->response_type)
  {
   case MMGSDI_RESTRICTED_SIM_ACCESS_CNF:
   {
   /* Allocate command buffer */
     cmd_ptr                       =  dsat_get_cmd_buf(sizeof(ds_at_crsm_info_type), FALSE);
     dsat_crsm_info                =  cmd_ptr->cmd_payload_ptr;
     cmd_ptr->hdr.cmd_id           =  DS_CMD_ATCOP_MMGSDI_CRSM_RESP;

     dsat_crsm_info->client_ref    = cnf_hdr_ptr->client_data;
     dsat_crsm_info->cmd_status    = (mmgsdi_return_enum_type)cnf_hdr_ptr->mmgsdi_status;

     if ( cnf_hdr_ptr->mmgsdi_status == MMGSDI_SUCCESS )
     {
       if (sim_info->response_data.data_len != 0 &&
           sim_info->response_data.data_ptr != NULL)
       {
         dsat_crsm_info->data.data_ptr = dsat_alloc_memory(sim_info->response_data.data_len, FALSE);
         /* Copy the response into our heap buffer. */
        dsat_crsm_info->data.data_len = dsatutil_memscpy((void*)dsat_crsm_info->data.data_ptr,
                                                         sim_info->response_data.data_len,
                                                         (void*)sim_info->response_data.data_ptr,
                                                         sim_info->response_data.data_len);
       }
     }
     /*Check if the status word is present and copy it to the buffer*/
     if (cnf_hdr_ptr->status_word.present)
     {
       dsat_crsm_info->sw1     = sim_info->response_header.status_word.sw1;
       dsat_crsm_info->sw2     = sim_info->response_header.status_word.sw2;
     }
     DS_AT_MSG1_HIGH(" etsime_mmgsdi_crsm_cb - data_len-%d " ,
                      dsat_crsm_info->data.data_len);

      /*Only post when data is valid */
     ds_put_cmd(cmd_ptr);
   }
   break;
   default:
      DS_AT_MSG1_ERROR("Not a valid mmgsdi CRSM cb type: %d", 
                           cnf_hdr_ptr->response_type);
      break;
  }
}/* etsime_mmgsdi_crsm_cb */

/*===========================================================================
FUNCTION ETSIME_MMGSDI_SEND_APDU_CB

DESCRIPTION
  This function handles the Send APDU response callback.  


DEPENDENCIES
  None


RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
LOCAL void etsime_mmgsdi_send_apdu_cb
(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type       cnf,
  const mmgsdi_cnf_type     *cnf_ptr
)
{
  ds_cmd_type *cmd_ptr = NULL;
  const mmgsdi_response_header_type *cnf_hdr_ptr = NULL;
  const mmgsdi_send_apdu_cnf_type   *cnf_apdu_ptr = NULL;
  ds_at_mmgsdi_apdu_resp_type *cmd_apdu_ptr = NULL;

/*--------------------------------------------------------------*/
  
  ASSERT(cnf_ptr != NULL);
  cnf_hdr_ptr = &cnf_ptr->send_apdu_cnf.response_header;
  cnf_apdu_ptr = &cnf_ptr->send_apdu_cnf;  
  ASSERT(cb_csim_pending == cnf_hdr_ptr->client_data) ; 
  DS_AT_MSG3_HIGH("MMGSDI APDU CB  cnf=%d  status=%d Client data = %d",
                                    cnf,status,cnf_hdr_ptr->client_data);
  
  DS_AT_MSG1_HIGH("MMGSDI APDU response type = %d",cnf_hdr_ptr->response_type);
  switch (cnf_hdr_ptr->response_type)
  {
   case MMGSDI_SEND_APDU_CNF:
   {
       /* Allocate command buffer */
     cmd_ptr                  =  dsat_get_cmd_buf(sizeof(ds_at_mmgsdi_apdu_resp_type), FALSE);
     cmd_apdu_ptr             = cmd_ptr->cmd_payload_ptr;

     cmd_ptr->hdr.cmd_id      =  DS_CMD_ATCOP_MMGSDI_APDU_RESP;
     cmd_apdu_ptr->cmd_status = (mmgsdi_return_enum_type)cnf_hdr_ptr->mmgsdi_status;
     cmd_apdu_ptr->client_ref = cnf_hdr_ptr->client_data;
     cmd_apdu_ptr->channel_id = cnf_apdu_ptr->channel_id;
     cmd_apdu_ptr->apdu_len = cnf_apdu_ptr->apdu_data.data_len;
     cmd_apdu_ptr->csim_apdu_data_ptr = NULL;
       /* Copy the response APDU into the save buffer 
          when it won't overwrite. */
         

     if ( cnf_hdr_ptr->mmgsdi_status == MMGSDI_SUCCESS )
     {
        if (cmd_apdu_ptr->apdu_len != 0)
        {
          cmd_apdu_ptr->csim_apdu_data_ptr = dsat_alloc_memory(cmd_apdu_ptr->apdu_len, FALSE);
        }
        if( NULL != cnf_apdu_ptr->apdu_data.data_ptr )
        {
       /* Copy the response APDU into the save buffer 
          when it won't overwrite. */
        (void) dsatutil_memscpy((void*)cmd_apdu_ptr->csim_apdu_data_ptr,
                   cmd_apdu_ptr->apdu_len ,(void*)cnf_apdu_ptr->apdu_data.data_ptr,
                  cmd_apdu_ptr->apdu_len);
        } 
     }
     else
     {
       cmd_apdu_ptr->apdu_len = 0;
     }
     /*Only put when it have something */
      DS_AT_MSG1_HIGH(" mmgsdi_send apdu cb - apdu_len-%d",
                      cmd_apdu_ptr->apdu_len);
     ds_put_cmd(cmd_ptr);
   }
   
   break;
   default:
      DS_AT_MSG1_ERROR("Not a valid mmgsdi sim cb rsp: %d", 
                           cnf_hdr_ptr->response_type);
      break;
  }
  
}/* etsime_mmgsdi_send_apdu_cb */

#endif /* FEATURE_MMGSDI */

/*===========================================================================

FUNCTION  ETSIME_REPORT_CSIM

DESCRIPTION
  This function gives the response for +CSIM write command. It converts a 
  length and byte array into a hex response string and outputs it.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_CMD_ERR_RSP :    if there was any problem in executing the command
    DSAT_OK :       if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
LOCAL dsat_result_enum_type etsime_report_csim
(
  uint32 num_bytes,
  byte *resp_apdu
)
{
  dsat_result_enum_type result = DSAT_OK;
  dsm_item_type * res_buff_ptr = NULL;
  byte *curr_byte;
  byte *end_byte;
  byte *curr_char;
  uint16 num_chars = (uint16)(num_bytes << 1);
  dsm_item_type *curr_buff_ptr = NULL;
  boolean check_append = FALSE;

  ASSERT(NULL != resp_apdu);

  res_buff_ptr = dsat_dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL, FALSE);
  curr_buff_ptr = res_buff_ptr;
  /* Generate response up to hex string representation of response
     APDU */
  curr_buff_ptr->used = (word)snprintf( (char*)curr_buff_ptr->data_ptr, 
                                              curr_buff_ptr->size, 
                                              "+CSIM: %d,\"", num_chars );

  /* If adding the APDU hex string or terminating '"\0' will 
     overflow the DSM item check for when append is needed 
     while stuffing characters */
  if ( (curr_buff_ptr->used + (num_chars + 2)) > curr_buff_ptr->size )
  {
    check_append = TRUE;
  };

  /* Convert response APDU into hex chars adding to res_buff_ptr->data_ptr
     as we go, each byte of response APDU becomes 2 hex characters.
     Must check available length as we go */
  for ( curr_byte = resp_apdu, 
          end_byte = curr_byte + num_bytes,
          curr_char = &curr_buff_ptr->data_ptr[curr_buff_ptr->used];
        curr_byte < end_byte;
        curr_byte++ )
  {
    /* Check to see if a new DSM item is required */
    if ( check_append )
    {
      /* Check that room for 4 more characters exists, chose 4 to save 
         having to check when '"' and terminating NULL are added later */
      curr_buff_ptr = dsatutil_append_dsm_item( 
                        curr_buff_ptr,
                        curr_buff_ptr->size - 4);
      /* If buffer appended... */
      if ( curr_buff_ptr != res_buff_ptr )
      {
        /* Point curr_char to beginning of its data array */
        curr_buff_ptr->used = 0;
        curr_char = &curr_buff_ptr->data_ptr[curr_buff_ptr->used];
      }

      /* Appending 2 DSM items will never be required */
      check_append = FALSE;
    }

    if( *curr_byte < 16 )
    {
      *curr_char++ = '0';
    }
    curr_char = dsatutil_itoa( (uint32)*curr_byte, curr_char, 16 );
    curr_buff_ptr->used += 2;
  }

  *curr_char++ = '\"';
  curr_buff_ptr->used++;

  *curr_char++ = '\0';

  /* Send the response to the user */
  dsatcmdp_handle_async_cmd_rsp( res_buff_ptr, DSAT_COMPLETE_RSP );

  res_buff_ptr = NULL;
  result = DSAT_OK;

  return result;
} /* etsime_report_csim */


/*===========================================================================

FUNCTION  ETSIME_REPORT_CRSM

DESCRIPTION
  This function gives the response for +CRSM write command. It outputs the 
  SW1 and SW2 codes then converts a length and byte array into a hex response
  string and outputs it.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_CMD_ERR_RSP :    if there was any problem in executing the command
    DSAT_OK :       if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
LOCAL dsat_result_enum_type etsime_report_crsm
(
  uint8 sw1,
  uint8 sw2,
  uint32 num_bytes,
  byte *resp_apdu
)
{
  /*Local Variables*/
  dsat_result_enum_type result = DSAT_OK;
  dsm_item_type * res_buff_ptr = NULL;
  char                   buffer[65];
  uint16                 size;
  uint32                 num_bytes_rem = num_bytes;
  char                  *resp_rem = (char *)resp_apdu;

  if ( num_bytes )
  {
    ASSERT(resp_apdu != NULL);
  }

  /* Generate response up to hex string representation of response
     APDU */
  size = (uint16)snprintf ( buffer,sizeof(buffer),
                                  "+CRSM: %d,%d,\"", sw1, sw2 );

  res_buff_ptr = dsat_dsm_create_packet(buffer, size, FALSE);

  while ( num_bytes_rem )
  {
    char *tmp_chr_ptr = NULL;

    /* process response data one chunk at a time,
       could be 255 bytes = 510 characters */
    if ( ((num_bytes_rem << 1) + 1) < sizeof(buffer) )
    {
      /* do remaining bytes, + 1 for terminating NULL */
      size = (uint16)((num_bytes_rem << 1) + 1);
    }
    else
    {
      /* fill the buffer */
      size = sizeof(buffer);
    }

    /* convert to HEX character pairs in buffer */
    tmp_chr_ptr = dsatutil_convert_chset( resp_rem, 
                                          ALPHA_8BIT,
                                          &buffer[0], 
                                          ALPHA_HEX,
                                          size,
                                          TRUE );
    if ( tmp_chr_ptr == NULL )
    {
      DS_AT_MSG0_ERROR("Error converting APDU to HEX chars");
      return DSAT_ERROR;
    }

    /* calculate size of hex char string and place in result buffer. */
    size = (uint16)( tmp_chr_ptr - &buffer[0] );
    dsat_dsm_pushdown_tail(&res_buff_ptr, buffer, size,
                           DSM_ITEM_POOL(res_buff_ptr), FALSE);

    /* convert size from number of hex chars added to response buffer to 
       number of octets removed from data buffer,
       update count and pointer */
    size = size >> 1;
    num_bytes_rem -= size;
    resp_rem += size;
  }
  /* push terminating quote mark onto result buffer. */
  dsat_dsm_pushdown_tail(&res_buff_ptr, "\"", 1,
                         DSM_ITEM_POOL(res_buff_ptr), FALSE);

  /* Send the response to the user */
  dsatcmdp_handle_async_cmd_rsp( res_buff_ptr, DSAT_COMPLETE_RSP );

  res_buff_ptr = NULL;
  result = DSAT_OK;

  return result;
} /* etsime_report_crsm */

#endif /* FEATURE_MMGSDI_ATCSIM */

/*===========================================================================

FUNCTION  ETSIME_GET_BATTERY_LEVEL

DESCRIPTION
  This function queries the battery driver for the charge level,
  converts to fraction of total charge then scales value using
  paramater.  This allows multiple change ranges to be used.  The
  return value is rounded to the nearest integer.

DEPENDENCIES
  None

RETURN VALUE
  Battery level as integer, scaled based on parameter.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
LOCAL word etsime_get_battery_level
(
  word scale
)
{
  /* Battery driver API for charge level is deprecated */
  return 0;
} /* etsime_get_battery_level */



/*===========================================================================
FUNCTION: dsatetsime_exec_cclk_cmd

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at+cclk command.
  at+cclk is used to set the real time clock.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatetsime_exec_cclk_cmd
(
  dsat_mode_enum_type mode,               /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr             /*  Place to put response       */
)
{
  time_julian_type  rt_clk;
  byte delimiter = '/';  /* Initial delimiter for time string parsing */
  byte cclk_tok_str[MAX_CCLK_STRTOK_LENGTH+2];
  byte *str, str2[MAX_CCLK_STRING_LENGTH + 3];
  dsat_cclk_tok_e_type i = FINDING_CCLK_YEAR;
  sys_time_zone_type time_zone = 0;
  uint32 x = 0;
  boolean cclk_flag = FALSE;
  dsat_num_item_type  csdf_aux_val;

  csdf_aux_val = (dsat_num_item_type)dsatutil_get_val( DSATETSI_EXT_ACT_CSDF_ETSI_IDX,0,0,NUM_TYPE);

  memset(&rt_clk, 0, sizeof(time_julian_type));

  /* AT WRITE COMMAND SYNTAX */
  if(tok_ptr->op == (NA|EQ|AR))
  {
    if(tok_ptr->args_found == 1)
    {
      /* Ignore the quotes surrounding the string */
      if(dsatutil_strip_quotes_out(tok_ptr->arg[0], str2, sizeof(str2)) 
                                                                == FALSE)
      {
        return DSAT_ERROR;
      }

      x = strlen((char *) str2);

      if((1 == csdf_aux_val && (x == MAX_CCLK_STRING_LENGTH || 
                                x == MAX_CCLK_STRING_LENGTH - 3)) ||
         (2 == csdf_aux_val && (x == MAX_CCLK_STRING_LENGTH + 2 ||
                                x == MAX_CCLK_STRING_LENGTH - 1 ))) /* optional time-zone */
      {
        str2[MAX_CCLK_STRING_LENGTH] = '\0';
        str = str2;

        /* This loop terminates when valid +CCLK format is given or an error 
           condition is reached in parsing */
        while(cclk_flag != TRUE)
        {
          if(dsatutil_strtok(str, ((csdf_aux_val == 2 && i == FINDING_CCLK_YEAR)? 
               MAX_CCLK_STRTOK_LENGTH + 2 : MAX_CCLK_STRTOK_LENGTH), delimiter, cclk_tok_str) != DSAT_ERROR)
          {
            if(((cclk_tok_str[0] >= '0') && (cclk_tok_str[0] <= '9')) &&
               ((cclk_tok_str[1] >= '0') && (cclk_tok_str[1] <= '9')))
            {
              switch(i)
              {
              case FINDING_CCLK_YEAR:
                if(csdf_aux_val == 2)
                {
                  rt_clk.year = (cclk_tok_str[0] - '0')*1000 +
                                (cclk_tok_str[1] - '0')*100 +
                                (cclk_tok_str[2] - '0')*10 +
                                (cclk_tok_str[3] - '0') ;
                  str +=  MAX_CCLK_STRTOK_LENGTH + 3;
                }
                else
                {
                rt_clk.year = (cclk_tok_str[0] - '0')*10 +
                              (cclk_tok_str[1] - '0') + DSAT_TIMEZONE_MIN_YEAR;
                str += MAX_CCLK_STRTOK_LENGTH + 1;
                }
                i = FINDING_CCLK_MONTH;

                break;

              case FINDING_CCLK_MONTH:
                rt_clk.month = (cclk_tok_str[0] - '0')*10 +
                               (cclk_tok_str[1] - '0');
                i = FINDING_CCLK_DAY;
                delimiter = ',';
                str += MAX_CCLK_STRTOK_LENGTH + 1;
                break;

              case FINDING_CCLK_DAY:
                rt_clk.day = (cclk_tok_str[0] - '0')*10 +
                             (cclk_tok_str[1] - '0');
                i = FINDING_CCLK_HOUR;            
                delimiter = ':';
                str += MAX_CCLK_STRTOK_LENGTH + 1;
                break;

              case FINDING_CCLK_HOUR:
                rt_clk.hour = (cclk_tok_str[0] - '0')*10 +
                              (cclk_tok_str[1] - '0');
                i = FINDING_CCLK_MINUTE;
                str += MAX_CCLK_STRTOK_LENGTH + 1;
                break;

              case FINDING_CCLK_MINUTE:
                rt_clk.minute = (cclk_tok_str[0] - '0')*10 +
                                (cclk_tok_str[1] - '0');
                str += MAX_CCLK_STRTOK_LENGTH + 1;

                /* str now points to start of *seconds* token */
                if(((*(char *)str >= '0') && (*(char *)str <= '9')) &&
                   ((*(char *)(str + 1) >= '0') && (*(char *)(str + 1) <= '9')))
                {
                  rt_clk.second = ((*(char *)str) - '0')*10 +
                                  ((*(char *)(str + 1)) - '0');
                  str += 2;
                }
                else
                {
                  return DSAT_ERROR;
                }

                /* Check if the optional timezone is specified or not */
                if(x == MAX_CCLK_STRING_LENGTH)
                { 
                  if(*(char *)str == '+')
                  {
                    str++;
                    if(((*(char *)str >= '0') && (*(char *)str <= '9')) &&
                         ((*(char *)(str + 1) >= '0') && 
                         (*(char *)(str + 1)<= '9')))
                    {
                      time_zone = ((*(char *)str) - '0')*10 +
                                  ((*(char *)(str + 1)) - '0');
                      str += 2;
                      if(time_zone > DSAT_TIMEZONE_HIGH)
                      {
                        return DSAT_ERROR;
                      }
                    }
                    else
                    {
                      return DSAT_ERROR;
                    }
                  }
                  else if(*(char *)str == '-')
                  {
                    str++;
                    if(((*(char *)str >= '0') && (*(char *)str <= '9')) &&
                             ((*(char *)(str + 1) >= '0')   && 
                             (*(char *)(str + 1)<= '9')))
                    {
                      time_zone = ((*(char *)str) - '0')*10 +
                                  ((*(char *)(str + 1)) - '0');
                      cclk_flag = TRUE;
                      str += 2; 
                      if(time_zone > DSAT_TIMEZONE_LOW)
                      {                    
                        return DSAT_ERROR;
                      }
                      time_zone = -time_zone;
                    }
                    else
                    {
                      return DSAT_ERROR;
                    }
                  }
                  else
                  {
                    return DSAT_ERROR;
                  }
                  (void)dsatetsime_set_time_zone_into_nv(time_zone);
                     /* Set Time Zone value into NV items */
                }/* if (x == MAX_CCLK_STRING_LENGTH )*/
                cclk_flag = TRUE;
                break;

              default:
                /* Should not come here */
                return DSAT_ERROR;
              } /* end switch */
            } /* end ((cclk_tok_str[0]... */
            else
            {
              return DSAT_ERROR;
            }
          }
          else
          {
            return DSAT_ERROR;
          }
        } /* end while */

        if ((rt_clk.year < DSAT_TIMEZONE_MIN_YEAR)
            || (rt_clk.year > DSAT_TIMEZONE_MAX_YEAR)
            || (rt_clk.month > DSAT_TIMEZONE_MAX_MONTH)
            || (rt_clk.day > DSAT_TIMEZONE_MAX_DAY)
            || (rt_clk.hour > DSAT_TIMEZONE_MAX_HOUR)
            || (rt_clk.minute > DSAT_TIMEZONE_MAX_MINUTES)
            || (rt_clk.second > DSAT_TIMEZONE_MAX_SECONDS)
            || (dsatutil_is_date_valid(rt_clk.year, rt_clk.month, rt_clk.day)
               == FALSE))
        {
          return DSAT_ERROR;
        }
        else
        {
           /* Do Real Time Clock update */
           time_set_julian(&rt_clk);
        }
      }
      else
      {
        return DSAT_ERROR;
      }
    }
    else
    {
      return DSAT_ERROR;
    }
  }
  /* AT TEST COMMAND SYNTAX */
  else if (tok_ptr->op == (NA|EQ|QU))
  {
    /* Do nothing */
  }
  /* AT READ COMMAND SYNTAX */
  else if (tok_ptr->op == (NA|QU))
  {
    /* get the Real Time Clock current setting */
    return etsime_get_real_time_clock(res_buff_ptr,tok_ptr->name);
  }
  else
  {
    return DSAT_ERROR;
  }
  
  return DSAT_OK;
} /* dsatetsime_exec_cclk_cmd */
/*===========================================================================
FUNCTION: DSATETSIME_EXEC_CSTF_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. This command sets the time format of the time information 
  presented to the user,which is specified by use of the <mode> parameter. 
  The <mode> affects the time format on the phone display and doesn't affect the 
  time format of the AT command serial interface(+CCLK),
  so it is not actually used.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsime_exec_cstf_cmd
(
  dsat_mode_enum_type mode,               /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr             /*  Place to put response       */
)
{
  dsat_result_enum_type result    = DSAT_OK;
  dsat_num_item_type    cstf_mode = DSAT_TIME_FORMAT_MAX;
  nv_item_type          nv_item;
  nv_stat_enum_type     nv_stat = NV_DONE_S;

  memset ((void*)&nv_item, 0, sizeof(nv_item));

  if (tok_ptr->op == (NA|EQ|AR))
  {
    if ( tok_ptr->args_found != 1 )
    {
      DS_AT_MSG1_ERROR("+CSTF: Invalid Number of arguements  %d", tok_ptr->args_found);
      return(DSAT_ERROR);
    }
    if((VALID_TOKEN(0))&& 
       (ATOI_OK == dsatutil_atoi(&cstf_mode,tok_ptr->arg[0],10))&&
       ((cstf_mode == DSAT_TIME_FORMAT_24_HOUR) 
          || (cstf_mode == DSAT_TIME_FORMAT_12_HOUR))
      )
    {
      nv_item.set_time_format = (nv_set_time_format_enum_type) cstf_mode;
      nv_stat = dsatutil_put_nv_item( NV_SET_TIME_FORMAT_I, &nv_item);
      if (nv_stat != NV_DONE_S)
      {
        return DSAT_ERROR;
      }
      result = DSAT_OK;
    }
    else 
    { 
      DS_AT_MSG1_ERROR("+CSTF: mode updation to %d failed", cstf_mode);
      result = DSAT_ERROR;
      return result;
    }
  }else if(tok_ptr->op == (NA|QU))
  {
    nv_stat = dsatutil_get_nv_item( NV_SET_TIME_FORMAT_I, &nv_item );
    
    if (nv_stat != NV_DONE_S)
    {
      return DSAT_ERROR;
    }
    
    cstf_mode =(dsat_num_item_type) nv_item.set_time_format;
    
    if(cstf_mode == DSAT_TIME_FORMAT_24_HOUR || cstf_mode == DSAT_TIME_FORMAT_12_HOUR )
    {
      res_buff_ptr->used = (word) snprintf((char *)res_buff_ptr->data_ptr,
                                            res_buff_ptr->size,
                                            "+CSTF: %d",cstf_mode);
      result = DSAT_OK;
    }
    else
    {
      DS_AT_MSG1_HIGH("Time format not supported %d", cstf_mode);
      result = DSAT_ERROR;
    }
  }else if(tok_ptr->op == (NA|EQ|QU))
  {
     res_buff_ptr->used = (word) snprintf((char *)res_buff_ptr->data_ptr,
                                            res_buff_ptr->size,
                                            "+CSTF: (1,2)");
    result = DSAT_OK;
  }else if(tok_ptr->op == (NA))
  {
   /* Do nothing: No default value mentioned in SPEC*/
    result = DSAT_OK;
  }
  else
  {
    /* wrong operation */
    result = DSAT_ERROR;
  }
  return result;
}/* dsatetsime_exec_cstf_cmd */
/*===========================================================================
FUNCTION: DSATETSIME_EXEC_CSDF_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. This command sets the date format of the date information 
  presented to the user,which is specified by use of the <mode> parameter. 
  The <mode> affects the date format on the phone display and doesn't affect the 
  date format of the AT command serial interface(+CCLK),
  so it is not actually used.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsime_exec_csdf_cmd
(
  dsat_mode_enum_type mode,               /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr             /*  Place to put response       */
)
{
  dsat_result_enum_type result        = DSAT_OK;
  dsat_num_item_type    csdf_mode     = DSAT_DATE_AUX_MODE_YY; /* Default value*/
  dsat_num_item_type    csdf_aux_mode = DSAT_DATE_AUX_MODE_YY; /* Default value*/
  nv_item_type          nv_item;
  nv_stat_enum_type     nv_stat       = NV_DONE_S;
  dsat_error_info_s_type    err_info;

  err_info.errval = DSAT_ERR_NO_ERROR;
  err_info.arg_num = 0;
    
  memset ((void*)&nv_item, 0, sizeof(nv_item));
 
  if (tok_ptr->op == (NA|EQ|AR) || 
      tok_ptr->op == (NA|EQ)||
      tok_ptr->op == (NA) )
  {
    if ( tok_ptr->args_found > 2 )
    {
      err_info.errval = DSAT_ERR_INVALID_NUM_OF_ARGS;
      goto send_error;
    }
    /*3GPP 27.007 6.22. If the parameter is omitted ("+CSDF=","+CSDF=<mode>","+CSDF=,<auxmode>")
      Default values  are 1
       */
    if(VALID_TOKEN(0) &&
      ((ATOI_OK != dsatutil_atoi(&csdf_mode,tok_ptr->arg[0],10))||
      (( csdf_mode < 1) || (csdf_mode > 7))))
    {
      err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
      err_info.arg_num = 0;
      goto send_error;
    }
    nv_item.set_date_format = (nv_set_time_format_enum_type) csdf_mode;
    nv_stat = dsatutil_put_nv_item( NV_SET_DATE_FORMAT_I, &nv_item);
    
    if (nv_stat != NV_DONE_S)
    {
      return DSAT_ERROR;
    }
    if(VALID_TOKEN(1)&&
      ((ATOI_OK != dsatutil_atoi(&csdf_aux_mode,tok_ptr->arg[1],10))||
       ((csdf_aux_mode < DSAT_DATE_AUX_MODE_YY ) || ( DSAT_DATE_AUX_MODE_YYYY < csdf_aux_mode ))))
    {
      err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
      err_info.arg_num = 1;
      goto send_error;
    }
    DSATUTIL_SET_VAL(DSATETSI_EXT_ACT_CSDF_ETSI_IDX,0,0,0,csdf_aux_mode,NUM_TYPE)
        
    result = DSAT_OK;
    DS_AT_MSG2_HIGH("+CSDF value update: Mode %d Aux mode %d",csdf_mode,csdf_aux_mode);
    
  }else if(tok_ptr->op == (NA|QU))
  {
    nv_stat = dsatutil_get_nv_item( NV_SET_DATE_FORMAT_I, &nv_item );
    
    if (nv_stat != NV_DONE_S)
    {
      return DSAT_ERROR;
    }
    
    csdf_mode =(dsat_num_item_type) nv_item.set_date_format;
    csdf_aux_mode = (dsat_num_item_type)dsatutil_get_val( DSATETSI_EXT_ACT_CSDF_ETSI_IDX,0,0,NUM_TYPE);
        
    if(0 < csdf_mode && csdf_mode < 8)
    {
      res_buff_ptr->used = (word) snprintf((char *)res_buff_ptr->data_ptr,
                                            res_buff_ptr->size,
                                            "+CSDF: %d,%d",csdf_mode, csdf_aux_mode);
      result = DSAT_OK;
    }
    else
    {
      DS_AT_MSG1_HIGH("Time format not supported %d", csdf_mode);
      result = DSAT_ERROR;
    }
  }else if(tok_ptr->op == (NA|EQ|QU))
  {
     res_buff_ptr->used = (word) snprintf((char *)res_buff_ptr->data_ptr,
                                            res_buff_ptr->size,
                                            "+CSDF: (1-7),(1-2)");
    result = DSAT_OK;
  }
  else
  {
    /* wrong operation */
    err_info.errval = DSAT_ERR_INVALID_SYNTAX;
    goto send_error;
  }
  return result;

send_error:
  if( err_info.errval == DSAT_ERR_INVALID_SYNTAX ||
      err_info.errval == DSAT_ERR_INVALID_NUM_OF_ARGS )
  {
    DS_AT_MSG1_ERROR("Error:%d",err_info.errval);
  }
  else
  {
    DS_AT_MSG2_ERROR("Error:%d, arg_num:%d",err_info.errval,err_info.arg_num);
  }
  return DSAT_ERROR;
}/* dsatetsime_exec_cstf_cmd */

/*===========================================================================

FUNCTION ETSIME_GET_REAL_TIME_CLOCK

DESCRIPTION
  This function is used to get the real time clock settings. 

DEPENDENCIES
  None
  
RETURN VALUE
  Returns dsat_result_enum_type 
  Possible values:
    DSAT_OK : Getting current clock setting is successful.
    DSAT_ERROR : otherwise.

SIDE EFFECTS
  None.
  
===========================================================================*/
LOCAL dsat_result_enum_type etsime_get_real_time_clock
(
  dsm_item_type *res_buff_ptr,
  const byte    *cmd_name       /*  Ptr to cmd name  */
)
{
#define MAX_THRESHOLD_TIME_ZONE   65536
sys_time_zone_type temp_tz;
boolean time_zone_avail = FALSE;
  time_julian_type      rt_clk;
dsat_num_item_type      csdf_aux_val;

csdf_aux_val = (dsat_num_item_type)dsatutil_get_val( DSATETSI_EXT_ACT_CSDF_ETSI_IDX,0,0,NUM_TYPE);

  memset(&rt_clk, 0, sizeof(time_julian_type));

  time_get_julian(&rt_clk);

  /* RTC supports last two digits display of the year field */
  if(rt_clk.year < DSAT_CLK_MIN_YEAR || rt_clk.year > DSAT_CLK_MAX_YEAR)
  {
    return DSAT_ERROR;
  }
  else if(csdf_aux_val == 1)
  {
    if(rt_clk.year >= DSAT_CLK_MIN_YEAR && 
          rt_clk.year < DSAT_TIMEZONE_MIN_YEAR)
  {
    rt_clk.year -= DSAT_YEAR_BASE_1900;
  }
  else if(rt_clk.year >= DSAT_TIMEZONE_MIN_YEAR && 
          rt_clk.year < DSAT_TIMEZONE_MAX_YEAR)
  {
    rt_clk.year -= DSAT_YEAR_BASE_2000;
  }
  else
  {
    rt_clk.year -= DSAT_YEAR_BASE_2100;
  }
  }

  res_buff_ptr->used = (word)snprintf((char*) res_buff_ptr->data_ptr, 
          res_buff_ptr->size,
          "%s: \"%2.2d/%2.2d/%2.2d,%2.2d:%2.2d:%2.2d", 
          cmd_name,
          rt_clk.year,
          rt_clk.month, rt_clk.day,
          rt_clk.hour, rt_clk.minute,
          rt_clk.second);

  time_zone_avail = dsatetsime_get_time_zone_from_nv(&temp_tz);
    /*For UNDP targets displaying the timezone in hourly format
      as against quarters of an hour described in 27.007 spec */
  if(time_zone_avail == TRUE)
  {
    res_buff_ptr->used = (word)snprintf( (char *)res_buff_ptr->data_ptr,
                                          res_buff_ptr->size,
                                          "%s%c%2.2d",
                                         res_buff_ptr->data_ptr,
                                         (temp_tz & 0xFF00 )? '-':'+',
                                         (temp_tz & 0xFF00 )? (MAX_THRESHOLD_TIME_ZONE-temp_tz):(temp_tz));
  }
  res_buff_ptr->used = (word)snprintf( (char *)res_buff_ptr->data_ptr,
                                            res_buff_ptr->size,
                                            "%s\"",
                                           res_buff_ptr->data_ptr);
  return DSAT_OK;
}/* etsime_get_real_time_clock */


/*===========================================================================
FUNCTION: DSATETSIME_GET_TIME_ZONE_FROM_NV

DESCRIPTION
  This function get time zone value from NV items.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean dsatetsime_get_time_zone_from_nv(sys_time_zone_type *time_zone)
{

  nv_item_type nv_item;
  nv_stat_enum_type  nv_stat = NV_DONE_S;
  memset ((void*)&nv_item, 0, sizeof(nv_item));      
  nv_stat = dsatutil_get_nv_item( NV_TIMEZONE_I, &nv_item );
  if (nv_stat != NV_DONE_S)
  {
    return FALSE;
  }   
  else
  {
    *time_zone = nv_item.timezone;
    return TRUE;
  }
} /* dsatetsime_get_time_zone_from_nv */

/*===========================================================================

FUNCTION  DSATETSIME_GET_ALPHANUMERIC_NAME

DESCRIPTION
  This function takes number and it's length as input and return equalent
  alphanumeric text stored on the phone book. If the function returns 
  PBM_SUCCESS then the equalent alphanumeric text can be accesed from 
  "dsat_alpha_text" variable.
  
DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
  PBM_SUCCESS: if equivalent alphanumeric text is found on the phone book
               else return value of "pbm_enum_rec_init/
               pbm_enum_next_rec_id/PBM_ERROR" function. 

  SIDE EFFECTS
  None

===========================================================================*/
pbm_return_type
dsatetsime_get_alphanumeric_name(byte *num_buf, byte num_len)
{
  pbm_return_type         pbm_return_status; /* pbm return value */
  dsat_pbm_record_s_type *record = NULL;     /* record */
  dsat_pbm_rec_id_type    rec_id;            /* record id */
  byte                    find_num[PBM_MAX_NUM_LENGTH+1];/* plus null char */
  dsat_num_item_type cscs_val ;
  uint8 dsat_handler;
  pbm_phonebook_type pb_id_local;
  dsatme_mmgsdi_state_ss_info  *me_ss_val = NULL;

  me_ss_val = dsat_get_base_addr(DSAT_MMGSDI_SS_VALS, FALSE);

  memset(&pb_id_local,0,sizeof (pbm_phonebook_type));
  if ( num_len == 0 || num_len > PBM_MAX_NUM_LENGTH )
  {
    return PBM_ERROR;
  }

  dsat_get_phonebook_id(&pb_id_local);
  pb_id_local.device_type = PBM_ADN;
  record = (dsat_pbm_record_s_type *)dsat_alloc_memory(sizeof(dsat_pbm_record_s_type), TRUE);
  if (NULL == record)
  {
    return PBM_ERROR;
  }

  memset(record, 0, sizeof(dsat_pbm_record_s_type));
  memset(find_num, 0, sizeof(find_num));
  (void) dsatutil_memscpy((void*)find_num,
          PBM_MAX_NUM_LENGTH+1,(void*)num_buf,num_len);
  
  /* Making two attempts to extract the number 
     One - PBM_ADN, Two - PBM_EFS */
  /* 1: PBM_ADN */
  /* Initialize search of the address book */
  pbm_return_status = pbm_session_enum_rec_init_ext(pb_id_local,
                                        (uint16)PBM_CAT_NONE,
                                        PBM_FIELD_PHONE_GENERIC,
                                        find_num,
                                        (uint16)(num_len + 1),
                                        PBM_SEARCHTYPE_EXACT | PBM_DATATYPE_STR | 
                                        PBM_FIELDID_PHONE | PBM_STRTYPE_PBM8,
                                        &dsat_handler);

  if( PBM_SUCCESS == pbm_return_status )
  {
    /* Get the next record from PBM which matches the search criteria */
    pbm_return_status = pbm_session_enum_next_rec_id_ext(&rec_id, dsat_handler);
    if( PBM_SUCCESS == pbm_return_status )
    {
      /* read the record  */ 
      dsatme_cpb_pbm_read_record(rec_id, record, &pbm_return_status);
    }
    pbm_session_enum_rec_init_ext_free_handle(dsat_handler);
  }
  
  /* 2:PBM_EFS, if not sucess in retrieving from PBM_ADN */
  if ( PBM_SUCCESS != pbm_return_status )
  {
    /* Initialize search of the address book */
    pb_id_local.device_type = PBM_EFS;
    pbm_return_status = pbm_session_enum_rec_init_ext(pb_id_local,
                                          (uint16)PBM_CAT_NONE,
                                          PBM_FIELD_PHONE_GENERIC,
                                          find_num,
                                          (uint16)(num_len + 1),
                                          PBM_SEARCHTYPE_EXACT | PBM_DATATYPE_STR | 
                                          PBM_FIELDID_PHONE | PBM_STRTYPE_PBM8,
                                          &dsat_handler);

    if( PBM_SUCCESS == pbm_return_status )
    {
      /* Get the next record from PBM which matches the search criteria */
      pbm_return_status = pbm_session_enum_next_rec_id_ext(&rec_id,dsat_handler);
      if( PBM_SUCCESS == pbm_return_status )
      {
        /* read the record  */ 
        dsatme_cpb_pbm_read_record(rec_id, record, &pbm_return_status);
      }
      pbm_session_enum_rec_init_ext_free_handle(dsat_handler);
    }
  }

  /* 3:PBM_FDN, if not sucess in retrieving from PBM_ADN */
  if ( PBM_SUCCESS != pbm_return_status )
  {
    /* Initialize search of the address book */
    pb_id_local.device_type = PBM_FDN;
    pbm_return_status = pbm_session_enum_rec_init_ext(pb_id_local,
                                          (uint16)PBM_CAT_NONE,
                                          PBM_FIELD_PHONE_GENERIC,
                                          find_num,
                                          (uint16)(num_len + 1),
                                          PBM_SEARCHTYPE_EXACT | PBM_DATATYPE_STR | 
                                          PBM_FIELDID_PHONE | PBM_STRTYPE_PBM8,
                                          &dsat_handler);

    if( PBM_SUCCESS == pbm_return_status )
    {
      DS_AT_MSG0_HIGH("PBM record init failed");
      /* Get the next record from PBM which matches the search criteria */
      pbm_return_status = pbm_session_enum_next_rec_id_ext(&rec_id,dsat_handler);
      if( PBM_SUCCESS == pbm_return_status )
      {
        /* read the record  */ 
        dsatme_cpb_pbm_read_record(rec_id, record, &pbm_return_status);
      }
      pbm_session_enum_rec_init_ext_free_handle(dsat_handler);
    }
  }

  if( pbm_return_status == PBM_SUCCESS )
  {
    cscs_val =(dsat_num_item_type)dsatutil_get_val(
                                             DSATETSI_EXT_CSCS_IDX,0,0,NUM_TYPE);
    if( dsatutil_convert_chset(record->text,
                               ALPHA_PBM_UCS2,
                               me_ss_val->dsat_alpha_text,
                               (dsati_chset_type)cscs_val,
                               DSAT_MAX_DISPLAY_ALPHA_LEN,
                               TRUE) == NULL )
    {
      pbm_return_status = PBM_ERROR;
    }
  }

  dsatutil_free_memory(record);

  return pbm_return_status;
} /* dsatetsime_get_alphanumeric_name */

/*===========================================================================
FUNCTION: DSATETSIME_EXEC_CTZU_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at+ctzu command.
  at+ctzu is used to NITZ update.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsime_exec_ctzu_cmd
(
  dsat_mode_enum_type mode,               /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr             /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;

  if(tok_ptr->op == (NA|QU|EQ) || tok_ptr->op == (NA|EQ|AR))
  {
    result = dsatparm_exec_param_cmd(
                                    mode,
                                    parse_table,
                                    tok_ptr,
                                    res_buff_ptr);

    if(result == DSAT_OK && (tok_ptr->op == (NA|EQ|AR)))
    { 
      nv_item_type nv_item;
      memset ((void*)&nv_item, 0, sizeof(nv_item));
      nv_item.auto_time_enable = (dsat_num_item_type)dsatutil_get_val(DSATETSI_EXT_ACT_CTZU_ETSI_IDX,0,0,NUM_TYPE);
  
      (void)dsatutil_put_nv_item(NV_SET_AUTO_TIME_I,&nv_item);
    }
  }
  else if(tok_ptr->op == (NA|QU))
  {
    res_buff_ptr->used = (word)snprintf((char*) res_buff_ptr->data_ptr,
                                              res_buff_ptr->size,
                                              "+CTZU: %d",
                                              (dsat_num_item_type)dsatutil_get_val(
                                              DSATETSI_EXT_ACT_CTZU_ETSI_IDX,0,0,NUM_TYPE));
  }
  else
  {
    result = DSAT_ERROR;
  }

  return result;
} /* dsatetsime_exec_ctzu_cmd */

/*===========================================================================

FUNCTION DSATETSIME_EXEC_CLAC_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +CLAC command.
  The result is in res_buff_ptr, reporting all supported AT commands.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :    if there was any problem in executing the command
    DSAT_OK :         if it is a success and execution completed.
SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatetsime_exec_clac_cmd
(
  dsat_mode_enum_type mode,                /*  AT command mode:            */
  const dsati_cmd_type *parse_table,       /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,       /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr              /*  Place to put response       */
)
{
  /* process command */
  if (tok_ptr->op == NA)
  {
    unsigned int i=0;
    dsati_mode_e_type operating_mode;
    char  buffer[MAX_CMD_SIZE];
    word size = 0;

    for (i=0;i<dsat_basic_table_size;i++)
    {
      size = (word)snprintf(&(buffer[0]),
                             MAX_CMD_SIZE,
                             "%s\n",
                             dsat_basic_table[i].name);

      dsat_dsm_pushdown_tail(&res_buff_ptr,
                             buffer, size,
                             DSM_ITEM_POOL(res_buff_ptr), FALSE);
    }
    for (i=0;i<dsat_basic_action_table_size;i++)
    {
      size = (word)snprintf(&(buffer[0]),
                            MAX_CMD_SIZE,
                            "%s\n",
                            dsat_basic_action_table[i].name);

      dsat_dsm_pushdown_tail(&res_buff_ptr,
                             buffer, size,
                             DSM_ITEM_POOL(res_buff_ptr), FALSE);
    }
    for (i=0;i<dsat_sreg_table_size;i++)
    {
      size = (word)snprintf(&(buffer[0]),
                            MAX_CMD_SIZE,
                            "%s\n",
                            dsat_sreg_table[i].name);

      dsat_dsm_pushdown_tail(&res_buff_ptr,
                             buffer, size,
                             DSM_ITEM_POOL(res_buff_ptr), FALSE);
    }
    for (i=0;i<dsat_ext_table_size;i++)
    {
      size = (word)snprintf(&(buffer[0]),
                           MAX_CMD_SIZE,
                           "%s\n",
                           dsat_ext_table[i].name);

      dsat_dsm_pushdown_tail(&res_buff_ptr,
                             buffer, size,
                             DSM_ITEM_POOL(res_buff_ptr), FALSE);
    }
    for(i=0;i<dsatetsi_ext_table_size;i++)
    {
      size = (word)snprintf(&(buffer[0]),
                            MAX_CMD_SIZE,
                            "%s\n",
                            dsatetsi_ext_table[i].name);

      dsat_dsm_pushdown_tail(&res_buff_ptr,
                             buffer, size,
                             DSM_ITEM_POOL(res_buff_ptr), FALSE);
    }
    operating_mode = dsatcmdp_get_current_mode();
    if ( ( DSAT_MODE_WCDMA == operating_mode) ||
         ( DSAT_MODE_GSM == operating_mode ) )
    {
      for (i=0;i<dsatetsi_ext_action_table_size;i++)
      {
        char *name[6]= {"+FTS","+FTH","+FTM","+FRS","+FRH","+FRM"};
        int j=0;
        int found_flag=0;
        for (j=0;((j<6) &&(found_flag==0));j++)
        {
          if ((DSAT_MODE_GSM != operating_mode) && 
          (strcmp(name[j],(char *)dsatetsi_ext_action_table[i].name)==0) )
          {
            found_flag = 1;
          }
        }
        if (!found_flag)
        {
          size = (word)snprintf(&(buffer[0]),
                                MAX_CMD_SIZE,
                                "%s\n",
                                dsatetsi_ext_action_table[i].name);

          dsat_dsm_pushdown_tail(&res_buff_ptr,
                                 buffer, size,
                                 DSM_ITEM_POOL(res_buff_ptr), FALSE);
          }
        }
      
    }
    /* Loop through the entire forwared command list to 
       display the supported command list 
    */
    for ( i=0; i< MAX_CMD_LIST; i++)
    {
      /* Check if there is any valid command forwarded to client. */
      /* Note:  If any UMTS commands are forwarded then it should
         be feature wrapped correctly in the internal static table 
         to avoid displaying twice.
      */
      if( ( dsat_fwd_at_cmds[i].client_id != DSAT_INVALID_CLIENT_ID ) && 
          ( dsat_fwd_at_cmds[i].cmd_name[0] != '\0' ) )
      {
        size = (word)snprintf( &(buffer[0]),
                               MAX_CMD_SIZE,
                               "%s\n",
                               dsat_fwd_at_cmds[i].cmd_name);

        dsat_dsm_pushdown_tail(&res_buff_ptr,
                               buffer, size,
                               DSM_ITEM_POOL(res_buff_ptr), FALSE);
      }
    }

    for(i=0;i<dsat_vendor_table_size;i++)
    {
      size = (word)snprintf(&(buffer[0]),
                           MAX_CMD_SIZE,
                            "%s\n",
                            dsat_vendor_table[i].name);

      dsat_dsm_pushdown_tail(&res_buff_ptr,
                             buffer, size,
                             DSM_ITEM_POOL(res_buff_ptr), FALSE);
    }
    return DSAT_NO_RESULT_CODE;
  }
  else if (tok_ptr->op == (NA|EQ|QU))
  {
    return DSAT_OK;
  }
  else
  {
    return DSAT_ERROR;
  }
}

/*===========================================================================

FUNCTION DSATETSIME_EXEC_ICCID_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +ICCID command.
  This command returns the ICCID of the SIM card.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR:        syntax error.
    DSAT_CMD_ERR_RSP:  if attempt was made to read IMSI from a slot that is
                       not the GSM operationa slot.
    DSAT_OK          : if it is a success.
    DSAT_ASYNC_CMD   : if this function invokes an async command.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatetsime_exec_iccid_cmd
(
  dsat_mode_enum_type  mode,              /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr             /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  dsat_apps_id_e_type   apps_id = dsat_get_current_gw_apps_id(FALSE);

  /*-------------------------------------------------------
  General commands with no arguments should return the 
  information requested in the form of a string
  ---------------------------------------------------------*/
  if (tok_ptr->op == NA)
  {
    /* GSDI async call made */
    if ( TRUE == dsatme_req_mmgsdi_sim_read( ICCID_MMGSDI_SIM_READ,
                                             MMGSDI_TRANSPARENT_FILE,
                                             0 ,apps_id) )
    {
      dsatme_pending_mmgsdi_command[apps_id] = TRUE;
      result = DSAT_ASYNC_CMD;
    }
    /* Won't attempt to read IMSI from non-GSM operational slot */
    else
    {
      dsatme_set_cme_error(DSAT_CME_OP_NOT_ALLOWED, res_buff_ptr);
      result = DSAT_CMD_ERR_RSP;
    }
  }
      
  /*-------------------------------------------------------
  General commands with a query should produce no response
  ---------------------------------------------------------*/
  else if (tok_ptr->op == (NA|EQ|QU))
  {
    /* this command is valid, but do nothing */
  }

  /*-------------------------------------------------------
  Any argument with the command should result in an error
  ---------------------------------------------------------*/
  else
  {
    result = DSAT_ERROR;
  }

  return result;
} /* dsatetsime_exec_iccid_cmd() */
#endif /* FEATURE_MMGSDI */
#ifdef FEATURE_DATA_PS_EAP
/*===========================================================================

FUNCTION  DSATETSIME_CEAP_TIMER_EXPIRED_HANDLER 

DESCRIPTION
  This function is the handler function for the +CEAP command
  reporting timer expired event.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_ASYNC_EVENT

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsime_ceap_timer_expired_handler
(
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
)
{
  DS_AT_MSG0_HIGH(" ceap_timer_expired ");
  
  if ( CHECK_PENDING(DSATETSI_EXT_ACT_CEAP_ETSI_IDX,0,DSAT_PENDING_CEAP_PROCESS) )
  {
    SET_PENDING(DSATETSI_EXT_ACT_CEAP_ETSI_IDX ,0, DSAT_PENDING_CEAP_NONE);
    return DSAT_ERROR;
  }
  else
  {
    return DSAT_ASYNC_EVENT;
  }
} /* dsatetsime_ceap_timer_expired_handler */

/*===========================================================================

FUNCTION  DSATETSIME_CEAP_PROCESS_TIMER_EXPIRED_HANDLER 

DESCRIPTION


DEPENDENCIES
  None

RETURN VALUE
  DSAT_ASYNC_EVENT

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsime_ceap_process_timer_expired_handler
(
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
)
{
  dsatme_mmgsdi_state_ss_info  *me_ss_val = NULL;

  me_ss_val = dsat_get_base_addr(DSAT_MMGSDI_SS_VALS, FALSE);

  DS_AT_MSG0_HIGH(" ceap_process_timer_expired ");
  
  if(me_ss_val->dsat_eap_info.eap_state == DSAT_EAP_STATE_AUTHENTICATING)
  {
    me_ss_val->dsat_eap_info.eap_state = DSAT_EAP_STATE_HELD;
    eap_delete_instance(me_ss_val->dsat_eap_info.eap_handle);
  }

  if ( CHECK_PENDING(DSATETSI_EXT_ACT_CEAP_ETSI_IDX,0,DSAT_PENDING_CEAP_PROCESS) )
  {
    SET_PENDING(DSATETSI_EXT_ACT_CEAP_ETSI_IDX ,0, DSAT_PENDING_CEAP_NONE);
    return DSAT_ERROR;
  }
  else
  {
    return DSAT_ASYNC_EVENT;
  }
} /* dsatetsime_ceap_process_timer_expired_handler */

/*===========================================================================
  FUNCTION DSATETSIME_EAP_TASK_SRV_FCT

  DESCRIPTION
    Cback function registered with EAP SIM/AKA for task switch

  PARAMETERS
    req_cb - cback to be called after switching tasks
    user_data

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL boolean dsatetsime_eap_task_sw_cb
(
  eap_sim_aka_task_srv_req_cb_type req_cb,
  void * user_data
)
{
  ds_cmd_type            *cmd_buf  = NULL;
  ds_at_eap_task_sw_info *eap_task_sw_info_ptr = NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  cmd_buf  = dsat_get_cmd_buf(sizeof(ds_at_eap_task_sw_info), FALSE);
  eap_task_sw_info_ptr = cmd_buf->cmd_payload_ptr;

  cmd_buf->hdr.cmd_id = DS_CMD_ATCOP_EAP_SIM_AKA_TASK_SWITCH_CMD;
  eap_task_sw_info_ptr->req_cb = req_cb;
  eap_task_sw_info_ptr->user_data = user_data;

  ds_put_cmd(cmd_buf);

  return TRUE;
}/* dsatetsime_eap_task_sw_cb */
/*===========================================================================

FUNCTION  DSATETSIME_EAP_TASK_SW_HANDLER

DESCRIPTION
  Handler for switching the task context from EAP module context to DS task.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_CMD_ERR_RSP:  if there was any problem in executing the command 
    DSAT_OK :          if it is a success.
    DSAT_ASYNC_EVENT : async event, contine processing  

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatetsime_eap_task_sw_handler
(
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
)
{
  ds_at_eap_task_sw_info *eap_task_sw_info_ptr = dsat_get_cmd_payload_ptr(cmd_ptr);

  if (NULL != eap_task_sw_info_ptr->req_cb)
  {
    eap_task_sw_info_ptr->req_cb(eap_task_sw_info_ptr->user_data);
  }
  else
  {
    DS_AT_MSG0_ERROR("Invalid EAP request callback");
  }

  return DSAT_ASYNC_EVENT;
}/* dsatetsime_eap_task_sw_handler */
/*===========================================================================
  FUNCTION DSATETSIME_EAP_RESULT_IND_CB

  DESCRIPTION
    Process the task switch req

  PARAMETERS
    handle - eap handle
    user data
    result - result of EAP 
    pre_master_key - MSK
    pre_master_key_len - MSK len

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void dsatetsime_eap_result_ind_cb
(
  eap_handle_type       handle, 
  void                 *result_ind_user_data,
  eap_result_enum_type  result,
  uint8                *pre_master_key,
  uint16                pre_master_key_len
)
{
  ds_cmd_type            *cmd_buf;
  ds_at_authi_eap_result_cmd_type *eap_result_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_buf  = dsat_get_cmd_buf(sizeof(ds_at_authi_eap_result_cmd_type), FALSE);
  eap_result_ptr = cmd_buf->cmd_payload_ptr;

  cmd_buf->hdr.cmd_id = DS_CMD_ATCOP_EAP_SIM_AKA_RESULT_IND_CMD;

  eap_result_ptr->handle      = handle;
  eap_result_ptr->result      = result;
  eap_result_ptr->pmk_len     = pre_master_key_len;
  eap_result_ptr->user_data   = result_ind_user_data;

  (void) dsatutil_memscpy((void*)eap_result_ptr->pmk,
          DS_EAP_MAX_PRE_MASTER_KEY_SIZE,(void*)pre_master_key,pre_master_key_len);
  ds_put_cmd(cmd_buf);
}/* dsatetsime_eap_result_ind_cb */

/*===========================================================================

FUNCTION  DSATETSIME_EAP_RESULT_IND_HANDLER

DESCRIPTION
  Function handler to store the PMK key from EAP for a successfull EAP session 

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_CMD_ERR_RSP:  if there was any problem in executing the command 
    DSAT_OK :          if it is a success.
    DSAT_ASYNC_EVENT : async event, contine processing  

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatetsime_eap_result_ind_handler
(
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
)
{
  dsatme_mmgsdi_state_ss_info  *me_ss_val = NULL;
  ds_at_authi_eap_result_cmd_type *eap_result_ptr = dsat_get_cmd_payload_ptr(cmd_ptr);

  me_ss_val = dsat_get_base_addr(DSAT_MMGSDI_SS_VALS, FALSE);

  DS_AT_MSG1_HIGH("eap_result_ind_handler %d result",
                              eap_result_ptr->result);

  if(me_ss_val->dsat_eap_info.eap_handle != eap_result_ptr->handle)
  {
    DS_AT_MSG1_HIGH("Result for a differnt handle %d",
                              eap_result_ptr->handle);
    return DSAT_ASYNC_EVENT;
  }

  if( eap_result_ptr->result == EAP_RESULT_SUCCESS && me_ss_val->dsat_eap_info.eap_key != NULL)
  {
    me_ss_val->dsat_eap_info.eap_key->pmk_len =MIN(DS_EAP_MAX_PRE_MASTER_KEY_SIZE, eap_result_ptr->pmk_len);
    (void) dsatutil_memscpy((void*)&me_ss_val->dsat_eap_info.eap_key->pmk[0],
            DS_EAP_MAX_PRE_MASTER_KEY_SIZE,(void*)
            &eap_result_ptr->pmk[0],
            MIN(DS_EAP_MAX_PRE_MASTER_KEY_SIZE,me_ss_val->dsat_eap_info.eap_key->pmk_len));
    
    DS_AT_MSG1_HIGH("copied %d length of key",eap_result_ptr->pmk_len);
  }

  return DSAT_ASYNC_EVENT;
}/* dsatetsime_eap_result_ind_handler */
/*===========================================================================
  FUNCTION DSATETSIME_EAP_SUPP_RESULT_IND_CB

  DESCRIPTION
    Process the task switch req

  PARAMETERS
    handle - eap handle
    user data
    result - result of EAP 
    pre_master_key - MSK
    pre_master_key_len - MSK len

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void dsatetsime_eap_supp_result_ind_cb
(
  eap_handle_type       handle,
    /**< EAP handle */
  void                 *supp_result_ind_user_data,
    /**< Supplementary result indication user data */
  eap_result_enum_type  result,
    /**< Indicates auth failure/success */
  eap_sim_aka_supp_auth_mask_type
                        supp_auth_info_mask,
    /**< Supplementary auth info mask */
  uint16                supp_auth_info_size,
    /**< Supplementary auth info size */
  void                 *supp_auth_info_ptr
    /**< Supplementary auth info */
)
{
  ds_cmd_type            *cmd_buf;
  ds_at_authi_supp_eap_result_cmd_type *result_cmd_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_buf = dsat_get_cmd_buf(sizeof(ds_at_authi_supp_eap_result_cmd_type), FALSE);
  result_cmd_ptr = cmd_buf->cmd_payload_ptr;

  cmd_buf->hdr.cmd_id = DS_CMD_ATCOP_EAP_SIM_AKA_SUPP_RESULT_IND_CMD;
  result_cmd_ptr->handle                = handle;
  result_cmd_ptr->result                = result;
  result_cmd_ptr->supp_auth_len         = supp_auth_info_size;
  result_cmd_ptr->supp_auth_info_mask   = supp_auth_info_mask;
  result_cmd_ptr->user_data             = supp_result_ind_user_data;

  (void) dsatutil_memscpy((void*)result_cmd_ptr->supp_auth,
          DS_EAP_MAX_PRE_MASTER_KEY_SIZE,(void*)supp_auth_info_ptr,supp_auth_info_size);

  ds_put_cmd(cmd_buf);
}/* dsatetsime_eap_supp_result_ind_cb */

/*===========================================================================

FUNCTION  DSATETSIME_EAP_SUPP_RESULT_IND_HANDLER

DESCRIPTION
  Function handler to store the EMSK key from EAP for a successfull EAP session 

  This function call completes a successful EAP session subjective to correct data passed through. 

  This also clears the timers - DSAT_TIMER_CEAP_REPORT_TIMEOUT, DSAT_TIMER_CEAP_EAP_TIMEOUT if found 
  running.Once the Keys(EMSK here and PMK in dsatetsime_eap_result_ind_handler) are copied, the EAP instance
  is deleted.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK :       If it is a success.
    DSAT_ASYNC_EVENT : If there is an unexpected input is received. 

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatetsime_eap_supp_result_ind_handler
(
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
)
{
  dsat_timer_s_type *t_ptr = NULL;
  dsatme_mmgsdi_state_ss_info  *me_ss_val = NULL;
  ds_at_authi_supp_eap_result_cmd_type *result_cmd_ptr = dsat_get_cmd_payload_ptr(cmd_ptr);

  me_ss_val = dsat_get_base_addr(DSAT_MMGSDI_SS_VALS, FALSE);

  DS_AT_MSG1_HIGH("eap_supp_result_ind_handler %d result",
                              result_cmd_ptr->result);

  if(me_ss_val->dsat_eap_info.eap_handle != result_cmd_ptr->handle)
  {
    DS_AT_MSG1_HIGH(" Result for a differnt handle %d",
                              result_cmd_ptr->handle);
    return DSAT_ASYNC_EVENT;
  }

  if( result_cmd_ptr->supp_auth_info_mask != EAP_SUPP_AUTH_INFO_EMSK)
  {
    DS_AT_MSG1_HIGH(" Result for a differnt mask %d",
                       result_cmd_ptr->supp_auth_info_mask);
    return DSAT_ASYNC_EVENT;
  }

  if( result_cmd_ptr->result == EAP_RESULT_SUCCESS)
  {
    me_ss_val->dsat_eap_info.eap_state = DSAT_EAP_STATE_AUTHENTICATED;
    if(me_ss_val->dsat_eap_info.eap_key != NULL)
    {
      me_ss_val->dsat_eap_info.eap_key->emsk_len = MIN(DS_EAP_MAX_PRE_MASTER_KEY_SIZE,result_cmd_ptr->supp_auth_len);
      (void) dsatutil_memscpy((void*)&me_ss_val->dsat_eap_info.eap_key->emsk[0],
            DS_EAP_MAX_PRE_MASTER_KEY_SIZE,(void*)&result_cmd_ptr->supp_auth[0],
            MIN(DS_EAP_MAX_PRE_MASTER_KEY_SIZE,me_ss_val->dsat_eap_info.eap_key->emsk_len));  
    }
  }
  else
  {
    me_ss_val->dsat_eap_info.eap_state = DSAT_EAP_STATE_HELD;
  }

  t_ptr = &dsatutil_timer_table[DSAT_TIMER_CEAP_REPORT_TIMEOUT];

  if(t_ptr->is_running)
  {
   (void)rex_clr_timer(t_ptr->timer_ptr);
    t_ptr->is_running = FALSE;
  }

  t_ptr = &dsatutil_timer_table[DSAT_TIMER_CEAP_EAP_TIMEOUT];

  if(t_ptr->is_running)
  {
   (void)rex_clr_timer(t_ptr->timer_ptr);
    t_ptr->is_running = FALSE;
  }

  eap_delete_instance(me_ss_val->dsat_eap_info.eap_handle);
 
  if ( CHECK_PENDING(DSATETSI_EXT_ACT_CEAP_ETSI_IDX,0,DSAT_PENDING_CEAP_PROCESS) )
  {
    SET_PENDING(DSATETSI_EXT_ACT_CEAP_ETSI_IDX ,0, DSAT_PENDING_CEAP_NONE);
    return DSAT_OK;
  }

  return DSAT_ASYNC_EVENT;
}/* dsatetsime_eap_supp_result_ind_handler */


/*===========================================================================
  FUNCTION DSATETSIME_EAP_TRP_TX_CB

  DESCRIPTION
    Process the task switch req

  PARAMETERS
    handle - eap handle
    user data
    result - result of EAP 
    pre_master_key - MSK
    pre_master_key_len - MSK len

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void dsatetsime_eap_trp_tx_cb
(
  void           * user_data, 
  dsm_item_type ** eap_resp_pkt
)
{
  ds_cmd_type            *cmd_buf;
  ds_at_eap_resp         *eap_resp_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_buf  = dsat_get_cmd_buf(sizeof(ds_at_eap_resp), FALSE);
  eap_resp_ptr = cmd_buf->cmd_payload_ptr;

  cmd_buf->hdr.cmd_id                   = DS_CMD_ATCOP_EAP_SIM_AKA_TRP_TX_CMD;
  eap_resp_ptr->user_data = user_data;
  eap_resp_ptr->pkt       = *eap_resp_pkt;
  
  ds_put_cmd(cmd_buf);
}/* dsatetsime_eap_trp_tx_cb */

/*===========================================================================

DESCRIPTION
  Function handler to forward the EAP packet response towards the terminal

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR:  If incorrect packet data is recieved 
    DSAT_OK :       If it is a success.
    DSAT_ASYNC_EVENT : If there is an unexpected input is received. 

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatetsime_eap_trp_tx_handler
(
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
)
{
  dsm_item_type * res_buff_ptr;
  dsm_item_type * pkt_resp = NULL;
  dsat_timer_s_type *t_ptr = NULL;
  uint32 pkt_len = 0;
  dsatme_mmgsdi_state_ss_info  *me_ss_val = NULL;
  ds_at_eap_resp *eap_resp_ptr = dsat_get_cmd_payload_ptr(cmd_ptr);

  me_ss_val = dsat_get_base_addr(DSAT_MMGSDI_SS_VALS, FALSE);

  pkt_len = dsm_length_packet(eap_resp_ptr->pkt);
  
  DS_AT_MSG2_HIGH("Recv'd %d bytes in trp_tx_handler, %d eap_state ",pkt_len,
                             me_ss_val->dsat_eap_info.eap_state);

  if ( CHECK_PENDING(DSATETSI_EXT_ACT_CEAP_ETSI_IDX,0,DSAT_PENDING_CEAP_PROCESS) )
  {
    SET_PENDING(DSATETSI_EXT_ACT_CEAP_ETSI_IDX,0,DSAT_PENDING_CEAP_NONE)	

    t_ptr = &dsatutil_timer_table[DSAT_TIMER_CEAP_REPORT_TIMEOUT];
 
    if(t_ptr->is_running)
    {
     (void)rex_clr_timer(t_ptr->timer_ptr);
      t_ptr->is_running = FALSE;
    }

    pkt_resp = dsat_dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL, FALSE);

    if(pkt_len > pkt_resp->size)
    {
      DS_AT_MSG1_HIGH("Not supporting %d Large responses",pkt_len);
      dsm_free_packet(&(eap_resp_ptr->pkt));
      dsm_free_buffer(pkt_resp);
      return DSAT_ERROR;
    }

    if(pkt_len != dsm_extract( (eap_resp_ptr->pkt),0,&pkt_resp->data_ptr[0],(uint16) pkt_len))
    {
      DS_AT_MSG0_HIGH("Could not Pull up the data ");
      dsm_free_packet(&(eap_resp_ptr->pkt));
      dsm_free_buffer(pkt_resp);
      return DSAT_ERROR;
    }
    pkt_resp->used = (uint16) pkt_len;	
    if(me_ss_val->dsat_eap_info.eap_state == DSAT_EAP_STATE_AUTHENTICATING)
    {
      res_buff_ptr = dsat_dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL, FALSE);
      res_buff_ptr->used = (uint16)snprintf((char*)res_buff_ptr->data_ptr,
                                                res_buff_ptr->size,"%s%d,\"",
                                                (char*)"+CEAP: ",me_ss_val->dsat_eap_info.eap_session_id);

      if(dsatme_format_cmd_rsp(&pkt_resp->data_ptr[0],
                                pkt_resp->used,
                                res_buff_ptr) == DSAT_ERROR)
      {
        return DSAT_ERROR;
      }
      dsatcmdp_handle_async_cmd_rsp( res_buff_ptr, DSAT_COMPLETE_RSP );
      dsm_free_buffer(pkt_resp);
      return DSAT_OK;
    }

  }

  dsm_free_buffer(pkt_resp);
  dsm_free_packet(&(eap_resp_ptr->pkt));
  return DSAT_ASYNC_EVENT;


}/* dsatetsime_eap_trp_tx_handler */

/*===========================================================================

FUNCTION  ETSIME_SEND_EAP_PACKET

DESCRIPTION
  This function send the packet data across for processing for an already created instance.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR:        syntax error.
    DSAT_ASYNC_CMD   : if this function invokes an async command.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
LOCAL dsat_result_enum_type etsime_send_eap_packet
(
  dsat_num_item_type eap_method, 
  dsm_item_type* eap_pkt_data 
)
{
  dsat_timer_s_type *t_ptr = NULL;
  dsatme_mmgsdi_state_ss_info  *me_ss_val = NULL;

  me_ss_val = dsat_get_base_addr(DSAT_MMGSDI_SS_VALS, FALSE);

  DS_AT_MSG2_HIGH("etsime_send_eap_packet %d eap_method %d eap_handle",
                me_ss_val->dsat_eap_info.eap_method,me_ss_val->dsat_eap_info.eap_handle);

  if((eap_method != me_ss_val->dsat_eap_info.eap_method) || 
     (me_ss_val->dsat_eap_info.eap_handle == EAP_INVALID_HANDLE))
  {
    DS_AT_MSG0_ERROR("Invalid EAP state");
    return DSAT_ERROR;
  }

  SET_PENDING(DSATETSI_EXT_ACT_CEAP_ETSI_IDX ,0, DSAT_PENDING_CEAP_PROCESS)

  eap_input(me_ss_val->dsat_eap_info.eap_handle,&eap_pkt_data);

  t_ptr = &dsatutil_timer_table[DSAT_TIMER_CEAP_REPORT_TIMEOUT];
  (void) rex_set_timer( t_ptr->timer_ptr, t_ptr->duration ); 
  t_ptr->is_running = TRUE;
  return DSAT_ASYNC_CMD;

}/* etsime_send_eap_packet */

/*===========================================================================

FUNCTION  ETSIME_START_AND_SEND_EAP_PACKET

DESCRIPTION
  This function creates a new EAP instance and send the packet data across for processing.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR:        syntax error.
    DSAT_ASYNC_CMD   : if this function invokes an async command.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
LOCAL dsat_result_enum_type etsime_start_and_send_eap_packet
(
  dsat_num_item_type eap_method,
  dsm_item_type* eap_pkt_data
)
{
  eap_info_type                * eap_req_params = NULL;
  dsat_timer_s_type *t_ptr = NULL;
  dsat_mmgsdi_imsi_info_s_type *imsi_ptr = NULL;
  int index;
  byte odd_digits;
  byte imsi_buff[32];
  byte * s_ptr = imsi_buff;
  dsatme_mmgsdi_state_ss_info  *me_ss_val = NULL;

  me_ss_val = dsat_get_base_addr(DSAT_MMGSDI_SS_VALS, FALSE);

  eap_req_params = (eap_info_type *)dsat_alloc_memory(sizeof(eap_info_type), FALSE);

  memset((void *)(eap_req_params), 0, sizeof(eap_info_type));

  DS_AT_MSG1_HIGH("etsime_start_and_send_eap_packet %d method",eap_method);
  
  if(eap_method == DSAT_EAP_SIM) 
  {
    eap_req_params->eap_auth_prot_mask = (0x1 << EAP_SIM_METHOD);
  }
  else if(eap_method == DSAT_EAP_AKA) 
  {
    eap_req_params->eap_auth_prot_mask = (0x1 << EAP_AKA_METHOD);
  }
  else
  {
    DS_AT_MSG0_HIGH("Invalid EAP method");
    dsatutil_free_memory(eap_req_params);
    return DSAT_ERROR;
  }

  imsi_ptr = dsatme_get_imsi_info(); 

  if(imsi_ptr == NULL)
  {
    DS_AT_MSG0_ERROR("Invalid ID");
    return DSAT_ERROR;
  }
  if( (0 == imsi_ptr->len) ||(imsi_ptr->len > MMGSDI_IMSI_LEN))
  {
    DS_AT_MSG0_HIGH("Bad IMSI data");
    dsatutil_free_memory(eap_req_params);
    return DSAT_ERROR;
  }

  odd_digits = imsi_ptr->data[0] & 0x08;
  memset((void *)(&imsi_buff[0]), 0, 32);

  /* Decode IMSI value from nibbles */
  for(index = 1; index <= imsi_ptr->len; index++)
  {
    if( ( index >= 0 ) && ( index < MMGSDI_IMSI_LEN ) )
    {
      if( index == 1) /* first byte, ignore lower nibble */
      {
        *s_ptr++ = UPPER_BCD( imsi_ptr->data[index] ) + '0';
      }
      else if( index == imsi_ptr->len ) /* last byte */
      {
        *s_ptr++ = LOWER_BCD( imsi_ptr->data[index] ) + '0';
        if(odd_digits) /* count the last one if odd digits */
          *s_ptr++ = UPPER_BCD( imsi_ptr->data[index] ) + '0';
      }
      else
      {
        *s_ptr++ = LOWER_BCD( imsi_ptr->data[index] ) + '0';
        *s_ptr++ = UPPER_BCD( imsi_ptr->data[index] ) + '0';
      }
    }
  }

  eap_req_params->authenticator_rsp_required = FALSE;
  eap_req_params->raw_mode = FALSE;
  eap_req_params->support_eap_ext = FALSE;
  eap_req_params->user_id.len = (uint16)( s_ptr - imsi_buff - 2);

  (void) dsatutil_memscpy((void*)&eap_req_params->user_id.name[0],
          EAP_IDENTITY_MAX_CHAR,(void*)&imsi_buff[0],MIN(32,eap_req_params->user_id.len));


  eap_req_params->result_ind_f_ptr = dsatetsime_eap_result_ind_cb;
  eap_req_params->result_ind_user_data = NULL;
  eap_req_params->supp_result_ind_f_ptr = dsatetsime_eap_supp_result_ind_cb;
  eap_req_params->supp_result_ind_user_data = NULL;

  eap_req_params->trp_tx_f_ptr = dsatetsime_eap_trp_tx_cb;
  eap_req_params->trp_tx_user_data = NULL ;


/*-------------------------------------------------------------------------
    CLEAR our internal state                              
  -------------------------------------------------------------------------*/
  me_ss_val->dsat_eap_info.eap_method = 0;
  me_ss_val->dsat_eap_info.eap_state = DSAT_EAP_STATE_NONE;
  me_ss_val->dsat_eap_info.eap_handle = EAP_INVALID_HANDLE;

  if(me_ss_val->dsat_eap_info.eap_key == NULL)
  {
    me_ss_val->dsat_eap_info.eap_key = (me_eap_key *)dsat_alloc_memory(sizeof(me_eap_key), FALSE);
    me_ss_val->dsat_eap_meta_info_ptr =(eap_sim_aka_meta_info_type *)
             dsat_alloc_memory(sizeof(eap_sim_aka_meta_info_type), FALSE); 
  }
  memset((void *)(me_ss_val->dsat_eap_meta_info_ptr), 0, sizeof(eap_sim_aka_meta_info_type));

/*-------------------------------------------------------------------------
    Create an EAP instance                              
  -------------------------------------------------------------------------*/

  me_ss_val->dsat_eap_info.eap_handle = eap_create_instance(eap_req_params);
  ps_utils_generate_rand_num(&(me_ss_val->dsat_eap_info.eap_session_id),
                              sizeof(me_ss_val->dsat_eap_info.eap_session_id));
  if(me_ss_val->dsat_eap_info.eap_handle == EAP_INVALID_HANDLE)
  {
    DS_AT_MSG0_ERROR("EAP create instance failed ");
    dsatutil_free_memory(eap_req_params);
    return DSAT_ERROR;
  }
  

/*-------------------------------------------------------------------------
    Initialise state to the method input                      
  -------------------------------------------------------------------------*/
  me_ss_val->dsat_eap_info.eap_method = eap_method;

  me_ss_val->dsat_eap_info.eap_state = DSAT_EAP_STATE_AUTHENTICATING;

  if(eap_method == DSAT_EAP_SIM) 
  {
    if(dsat_get_current_gw_app_type(FALSE) == MMGSDI_APP_USIM)
    {
      me_ss_val->dsat_eap_meta_info_ptr->sim_aka_algo_type = EAP_SIM_ALGO_USIM_GSM; 
    }
    else
    {
      me_ss_val->dsat_eap_meta_info_ptr->sim_aka_algo_type = EAP_SIM_ALGO_GSM; 
    }
  }
  else if(eap_method == DSAT_EAP_AKA) 
  {
    me_ss_val->dsat_eap_meta_info_ptr->sim_aka_algo_type = EAP_AKA_ALGO_MILENAGE;
  }

/*-------------------------------------------------------------------------
    Populate meta Info
  -------------------------------------------------------------------------*/

  me_ss_val->dsat_eap_meta_info_ptr->notification_rcv_cb_fct = NULL;

/*-------------------------------------------------------------------------
    Populate IMSI
  -------------------------------------------------------------------------*/
  me_ss_val->dsat_eap_meta_info_ptr->id.provided_id_len = (uint8) eap_req_params->user_id.len;

  (void) dsatutil_memscpy((void*)&me_ss_val->dsat_eap_meta_info_ptr->id.provided_id[0],
          EAP_SIM_AKA_MAX_ID_LEN,(void*)&imsi_buff[0],MIN(32,eap_req_params->user_id.len));
  /*-------------------------------------------------------------------------
     Fill the task_srv_fct----->   Function to switch the task context     
  -------------------------------------------------------------------------*/
  me_ss_val->dsat_eap_meta_info_ptr->task_srv_fct = dsatetsime_eap_task_sw_cb;

  /*-------------------------------------------------------------------------
      Fill supp type   
  -------------------------------------------------------------------------*/

  me_ss_val->dsat_eap_meta_info_ptr->supp_result_auth_info_mask = EAP_SUPP_AUTH_INFO_EMSK;

  /*-------------------------------------------------------------------------
    do EAP-AKA in software                
  -------------------------------------------------------------------------*/
  me_ss_val->dsat_eap_meta_info_ptr->eap_aka_in_sw = FALSE;

  /*-------------------------------------------------------------------------
    DS is a new_client therefore set to true
  -------------------------------------------------------------------------*/
  me_ss_val->dsat_eap_meta_info_ptr->new_client = TRUE;
  
  SET_PENDING(DSATETSI_EXT_ACT_CEAP_ETSI_IDX ,0, DSAT_PENDING_CEAP_PROCESS);

  eap_set_meta_info(me_ss_val->dsat_eap_info.eap_handle, (void *) me_ss_val->dsat_eap_meta_info_ptr);

  eap_input(me_ss_val->dsat_eap_info.eap_handle,&eap_pkt_data);

  t_ptr = &dsatutil_timer_table[DSAT_TIMER_CEAP_REPORT_TIMEOUT];
  (void) rex_set_timer( t_ptr->timer_ptr, t_ptr->duration ); 
  t_ptr->is_running = TRUE;


  t_ptr = &dsatutil_timer_table[DSAT_TIMER_CEAP_EAP_TIMEOUT];
  (void) rex_set_timer( t_ptr->timer_ptr, t_ptr->duration ); 
  t_ptr->is_running = TRUE;

  dsatutil_free_memory(eap_req_params);

  return DSAT_ASYNC_CMD;

}/* etsime_start_and_send_eap_packet */

/*===========================================================================

FUNCTION DSATETSIME_EXEC_CUAD_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +CUAD command.
  This command asks the MT to discover what applications are available for selection on the UICC. 

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR:        syntax error.
    DSAT_ASYNC_CMD   : if this function invokes an async command.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatetsime_exec_cuad_cmd
(
  dsat_mode_enum_type  mode,              /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr             /*  Place to put response       */
)
{
  dsat_apps_id_e_type apps_id = dsat_get_current_gw_apps_id(FALSE);

  if( tok_ptr->op == (NA))
  {
    SET_PENDING(DSATETSI_EXT_ACT_CUAD_ETSI_IDX ,0, DSAT_PENDING_CUAD_READ)
    if ( FALSE == dsatme_req_mmgsdi_get_file_attr( CUAD_MMGSDI_SIM_FILE_ATTR, apps_id ) )
    {
      SET_PENDING(DSATETSI_EXT_ACT_CUAD_ETSI_IDX ,0, DSAT_PENDING_CUAD_NONE)
      DS_AT_MSG1_ERROR("Initial Get File attr failed for file %d",CUAD_MMGSDI_SIM_FILE_ATTR);
      return DSAT_ERROR;
    }
  }
  else
  {
    return DSAT_ERROR;
  }
  return DSAT_ASYNC_CMD;
}/* dsatetsime_exec_cuad_cmd */

/*===========================================================================
FUNCTION DSATETSIME_EXEC_CEAP_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +CEAP command. This command allows a TE 
  to exchange EAP packets with the UICC or the ME.


DEPENDENCIES
  Only EAP AKA and EAP SIM are supported for now.

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR:        syntax error.
    DSAT_CMD_ERR_RSP:  if attempt was made to read IMSI from a slot that is
                       not the GSM operationa slot.
    DSAT_OK          : if it is a success.
    DSAT_ASYNC_CMD   : if this function invokes an async command.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatetsime_exec_ceap_cmd
(
  dsat_mode_enum_type  mode,              /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr             /*  Place to put response       */
)
{
  dsat_num_item_type  curr_eap_method = 0;
  dsat_num_item_type  len = 0;
  dsm_item_type *eap_packet_data;	
  dsatme_mmgsdi_state_ss_info  *me_ss_val = NULL;
  dsat_error_info_s_type    err_info;

  err_info.errval = DSAT_ERR_NO_ERROR;
  err_info.arg_num = 0;
  
  me_ss_val = dsat_get_base_addr(DSAT_MMGSDI_SS_VALS, FALSE);

  if ( tok_ptr->op == (NA|EQ|AR) )
  {
    if(tok_ptr->args_found > parse_table->compound)
    {
      err_info.errval = DSAT_ERR_INVALID_NUM_OF_ARGS;
      goto send_error;
    }
    if(VALID_TOKEN(0))
    {
      len = (uint16)strlen( (char *)tok_ptr->arg[0] );
      if((len - 2) > 16)
      {
        err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
        err_info.arg_num = 0;
        goto send_error;
      }
      if ('"' == tok_ptr->arg[0][0])
      {
        if (!dsatutil_strip_quotes_out(tok_ptr->arg[0],
                                       tok_ptr->arg[0],
                                       (word)strlen((char*)tok_ptr->arg[0])))
        {
          err_info.errval = DSAT_ERR_QUOTE_STRIP_FAILURE;
          err_info.arg_num = 0;
          goto send_error;
        }
      }
    }
   /* Process EAP method */
    if(!VALID_TOKEN(1))
    {
      return DSAT_ERROR;
    }
    else
    {
      if ('"' == tok_ptr->arg[1][0])
      {
        if (!dsatutil_strip_quotes_out(tok_ptr->arg[1],
                                       tok_ptr->arg[1],
                                       (word)strlen((char*)tok_ptr->arg[1])))
        {
          err_info.errval = DSAT_ERR_QUOTE_STRIP_FAILURE;
          err_info.arg_num = 1;
          goto send_error;
        }
      }
	
      if ( dsatutil_atoi( &curr_eap_method, 
                         tok_ptr->arg[1], 16) != ATOI_OK )
      {
        err_info.errval = DSAT_ERR_ATOI_CONVERSION_FAILURE;
        err_info.arg_num = 1;
        goto send_error;
      }

      if((curr_eap_method != DSAT_EAP_AKA )&&(curr_eap_method != DSAT_EAP_SIM ))
      {
        dsatme_set_cme_error(DSAT_CME_EAP_NOT_SUPPORTED, res_buff_ptr);
        return DSAT_CMD_ERR_RSP;
      }
    }

    /* Copy EAP DATA */
    if((!VALID_TOKEN(2)) || ('"' != tok_ptr->arg[2][0]))
    {
      err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
      err_info.arg_num = 2;
      goto send_error;
    }
    else
    {
      len = (uint16)strlen( (char *)tok_ptr->arg[2] );
     /* Process EAP method */
      if (!dsatutil_strip_quotes_out(tok_ptr->arg[2],
                                     tok_ptr->arg[2],
                                     len))
      {
        err_info.errval = DSAT_ERR_QUOTE_STRIP_FAILURE;
        err_info.arg_num = 2;
        goto send_error;
      }
      len = len - 2;
      
      eap_packet_data = dsat_dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL, FALSE);
      if((len % 2) != 0)
      {
        DS_AT_MSG0_ERROR("Invalid packet data");
        dsm_free_buffer(eap_packet_data);
        return DSAT_ERROR;
      }

      if(dsatutil_convert_chset((const char *)tok_ptr->arg[2],
                                ALPHA_HEX,
                                (const char *)&eap_packet_data->data_ptr[0],
                                ALPHA_8BIT,
                                MIN(len,eap_packet_data->size),
                                TRUE)== NULL)
      {
        dsm_free_buffer(eap_packet_data);
        dsatme_set_cme_error(DSAT_CME_UNKNOWN, res_buff_ptr);
        return DSAT_CMD_ERR_RSP;
      }
      eap_packet_data->used = MIN(len,eap_packet_data->size);

    }

    if( me_ss_val->dsat_eap_info.eap_state == DSAT_EAP_STATE_AUTHENTICATING )
    {
      if( me_ss_val->dsat_eap_info.eap_method != curr_eap_method )
      {
        dsm_free_buffer(eap_packet_data);
        DS_AT_MSG1_ERROR("EAP input %d specified while a different one in progress",curr_eap_method);
        dsatme_set_cme_error(DSAT_CME_OP_NOT_SUPPORTED, res_buff_ptr);
        return DSAT_CMD_ERR_RSP;
      }
      else
      {
        if( etsime_send_eap_packet(curr_eap_method,eap_packet_data) == DSAT_ERROR)
        {
          dsm_free_buffer(eap_packet_data);
          DS_AT_MSG0_ERROR("Invalid EAP state");
          return DSAT_ERROR;
        }
      }
    }
    else 
    {
      if( etsime_start_and_send_eap_packet(curr_eap_method,eap_packet_data) == DSAT_ERROR )
      {
        dsm_free_buffer(eap_packet_data);
        DS_AT_MSG0_ERROR("Invalid EAP state");
        return DSAT_ERROR;
      }
    }

    return DSAT_ASYNC_CMD;

  }
  else
  {
    err_info.errval = DSAT_ERR_INVALID_SYNTAX;
    goto send_error;
  }

send_error:
  if( err_info.errval == DSAT_ERR_INVALID_SYNTAX || 
      err_info.errval == DSAT_ERR_INVALID_NUM_OF_ARGS )
  {
    DS_AT_MSG1_ERROR("Error:%d",err_info.errval);
  }
  else
  {
    DS_AT_MSG2_ERROR("Error:%d, arg_num:%d",err_info.errval,err_info.arg_num);
  }
  return DSAT_ERROR; 
}/* dsatetsime_exec_ceap_cmd */


/*===========================================================================

FUNCTION DSATETSIME_EXEC_CERP_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +CERP command.
  This command allows a TE to retrieve EAP session parameters / derived keys after a run of the +CEAP command. 


DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR:        syntax error.
    DSAT_CMD_ERR_RSP:  if attempt was made to read IMSI from a slot that is
                       not the GSM operationa slot.
    DSAT_OK          : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatetsime_exec_cerp_cmd
(
  dsat_mode_enum_type  mode,              /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr             /*  Place to put response       */
)
{
#define KEY_PARAM 1
#define STATUS_PARAM 2
#define IDENTITY_PARAM 3
#define PSUEDONYM_PARAM 4

  dsat_num_item_type eap_session_id;
  dsat_num_item_type eap_param;
  dsatme_mmgsdi_state_ss_info  *me_ss_val = NULL;
  dsat_error_info_s_type    err_info;

  err_info.errval = DSAT_ERR_NO_ERROR;
  
  me_ss_val = dsat_get_base_addr(DSAT_MMGSDI_SS_VALS, FALSE);

  /*                                                                                                     */
  
  if(tok_ptr->op == (NA|EQ|AR))
  {
    if(tok_ptr->args_found > parse_table->compound)
    {
      err_info.errval = DSAT_ERR_INVALID_NUM_OF_ARGS;
      goto send_error;
    }
    if ( (!VALID_TOKEN(0)) || dsatutil_atoi( &eap_session_id, 
                      tok_ptr->arg[0], 10) != ATOI_OK )
    {
      err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
      err_info.arg_num = 0;
      goto send_error;
    }

    if ((!VALID_TOKEN(1)) || dsatutil_atoi( &eap_param, 
                      tok_ptr->arg[1], 10) != ATOI_OK )
    {
      err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
      err_info.arg_num = 1;
      goto send_error;
    }
	
    if(eap_session_id == me_ss_val->dsat_eap_info.eap_session_id)
    {
      if( eap_param == STATUS_PARAM )
      {
        res_buff_ptr->used = (word)snprintf((char*)res_buff_ptr->data_ptr,
                                                  res_buff_ptr->size,"%s\"0%d\"",
                                                  (char*)"+CERP: ",me_ss_val->dsat_eap_info.eap_state);
        return DSAT_OK;
      }
      else if( eap_param == KEY_PARAM )
      {
        if(me_ss_val->dsat_eap_info.eap_state == DSAT_EAP_STATE_AUTHENTICATED )
        {
          if(me_ss_val->dsat_eap_info.eap_key == NULL)
          {
            return DSAT_ERROR;
          }
          res_buff_ptr->used = (uint16)snprintf((char*)res_buff_ptr->data_ptr,
                                                  res_buff_ptr->size,"%s\"%x%x",
                                                  (char*)"+CERP: ",0x80,(int)me_ss_val->dsat_eap_info.eap_key->pmk_len);

          if(dsatme_format_cmd_rsp(&me_ss_val->dsat_eap_info.eap_key->pmk[0],
                                   me_ss_val->dsat_eap_info.eap_key->pmk_len,
                                   res_buff_ptr) == DSAT_ERROR)
          {
            return DSAT_ERROR;
          }

          res_buff_ptr->used--;
          res_buff_ptr = dsatutil_append_dsm_item(
                           res_buff_ptr,
                           res_buff_ptr->size - 4);

          res_buff_ptr->used += (uint16)snprintf((char*)&res_buff_ptr->data_ptr[res_buff_ptr->used],
                                                  res_buff_ptr->size - res_buff_ptr->used,"%x%x",
                                                  0x81,(int)me_ss_val->dsat_eap_info.eap_key->emsk_len);

          if(dsatme_format_cmd_rsp(&me_ss_val->dsat_eap_info.eap_key->emsk[0],
                                   me_ss_val->dsat_eap_info.eap_key->emsk_len,
                                   res_buff_ptr) == DSAT_ERROR)
          {
            return DSAT_ERROR;
          }
          return DSAT_OK;
        }
        else
        {
          DS_AT_MSG0_ERROR("Invalid EAP STATE");
          dsatme_set_cme_error(DSAT_CME_EAP_INVALID_PARAM, res_buff_ptr);
          return DSAT_CMD_ERR_RSP;
        }
      }
      else if(eap_param == IDENTITY_PARAM)
      {  
        res_buff_ptr->used = (word)snprintf((char*)res_buff_ptr->data_ptr,
                                                  res_buff_ptr->size,"+CERP: \"");

        if( (me_ss_val->dsat_eap_meta_info_ptr == NULL) ||
            (dsatme_format_cmd_rsp(&me_ss_val->dsat_eap_meta_info_ptr->id.provided_id[0],
                                 me_ss_val->dsat_eap_meta_info_ptr->id.provided_id_len,
                                 res_buff_ptr) == DSAT_ERROR))
        {
          return DSAT_ERROR;
        }
        return DSAT_OK;
      }
      else
      {
        DS_AT_MSG0_ERROR("Unsupported param");
        dsatme_set_cme_error(DSAT_CME_OP_NOT_SUPPORTED, res_buff_ptr);
        return DSAT_CMD_ERR_RSP;
      }

    }
    else
    {
      DS_AT_MSG0_ERROR("Invalid Session ID");
      dsatme_set_cme_error(DSAT_CME_EAP_INVALID_PARAM, res_buff_ptr);
      return DSAT_CMD_ERR_RSP;

    }

  }
  return DSAT_ERROR;

send_error:
  if( err_info.errval == DSAT_ERR_INVALID_NUM_OF_ARGS )
  {
    DS_AT_MSG1_ERROR("Error:%d",err_info.errval);
  }
  else
  {
    DS_AT_MSG2_ERROR("Error:%d, arg_num:%d",err_info.errval,err_info.arg_num);
  }
  return DSAT_ERROR; 
}/* dsatetsime_exec_cerp_cmd */
#endif /* FEATURE_DATA_PS_EAP */

#ifdef FEATURE_MMGSDI

/*===========================================================================
FUNCTION: DSATETSIME_CPOL_MMGSDI_CMD_HANDLER

DESCRIPTION
  This function handles at+cops command.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsime_cpol_mmgsdi_handler
(
  ds_cmd_type *cmd_ptr
)
{
  dsat_result_enum_type result = DSAT_ERROR;
  dsatme_mmgsdi_state_ms_info  *me_ms_val = NULL;
  ds_at_mmgsdi_plmn_info_type *plmn_info_ptr = dsat_get_cmd_payload_ptr(cmd_ptr);

  me_ms_val = dsat_get_base_addr_current_session();

  DS_AT_MSG2_HIGH("Status : %d and Number of PLMN IDs: %d",plmn_info_ptr->mmgsdi_status,
  	plmn_info_ptr->num_of_plmn_ids);
  if( MMGSDI_SUCCESS != plmn_info_ptr->mmgsdi_status )
  {
      return result;
  }
  if( !(plmn_info_ptr->num_of_plmn_ids) )
  {
    return result;
  }
  else
  {
     me_ms_val->dsat_cpol_write_params.mcc = plmn_info_ptr->plmn_info.mcc;
     me_ms_val->dsat_cpol_write_params.mnc = plmn_info_ptr->plmn_info.mnc;
     /* Process PLMN data */
     result = dsatetsime_process_plmn_info();
  }
  return result;
}

/*===========================================================================
FUNCTION: DSATETSIME_CPOL_CMD_CB_FUNC

DESCRIPTION
  This function handles at+cops command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatetsime_cpol_cmd_cb_func
(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type  *cnf_ptr
)
{
  ds_cmd_type           *cmd_buf;
  ds_at_mmgsdi_plmn_info_type *plmn_info_ptr = NULL;

  if(cnf_ptr == NULL)
  { 
    /*Message error*/
    DS_AT_MSG0_HIGH("CONF PTR is NULL");
    return;
  }
  DS_AT_MSG2_HIGH("Number of PLMN ids %d status %d",
                  cnf_ptr->get_se13_plmn_info_by_name_cnf.num_of_plmn_ids,status);
  
  if(NULL != cnf_ptr->get_se13_plmn_info_by_name_cnf.se13_plmn_info_ptr)
  {
     DS_AT_MSG3_HIGH("PLMN info mcc %d mnc %d Net Mask %d",
       cnf_ptr->get_se13_plmn_info_by_name_cnf.se13_plmn_info_ptr->mcc,
       cnf_ptr->get_se13_plmn_info_by_name_cnf.se13_plmn_info_ptr->mnc,
       cnf_ptr->get_se13_plmn_info_by_name_cnf.se13_plmn_info_ptr->network_mask
       );
  }

  cmd_buf  = dsat_get_cmd_buf(sizeof(ds_at_mmgsdi_plmn_info_type), FALSE);
  plmn_info_ptr = cmd_buf->cmd_payload_ptr;

  cmd_buf->hdr.cmd_id              = DS_CMD_ATCOP_CPOL_AT_CMD;
  plmn_info_ptr->mmgsdi_status = status;
  plmn_info_ptr->num_of_plmn_ids = 0;
  memset ((void *)&plmn_info_ptr->plmn_info,
                0x0,
                sizeof(mmgsdi_se13_plmn_info_type));
  if( MMGSDI_SUCCESS == status )
  {
    plmn_info_ptr->client_ref = 
      cnf_ptr->get_se13_plmn_info_by_name_cnf.response_header.client_data;
    plmn_info_ptr->num_of_plmn_ids = 
      cnf_ptr->get_se13_plmn_info_by_name_cnf.num_of_plmn_ids;

    if( cnf_ptr->get_se13_plmn_info_by_name_cnf.num_of_plmn_ids &&
         cnf_ptr->get_se13_plmn_info_by_name_cnf.se13_plmn_info_ptr )
    {
	  (void)dsatutil_memscpy((void*)&plmn_info_ptr->plmn_info,
			                 sizeof(mmgsdi_se13_plmn_info_type),
			                (void*)cnf_ptr->get_se13_plmn_info_by_name_cnf.se13_plmn_info_ptr,
			                sizeof(mmgsdi_se13_plmn_info_type));	   
    }
  }
  ds_put_cmd(cmd_buf);
}/* dsatetsime_cpol_cmd_cb_func */

#endif /*FEATURE_MMGSDI*/

/*===========================================================================
FUNCTION: DSATETSIME_PROCESS_PLMN_INFO

DESCRIPTION
  This function handles at+cops command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsime_process_plmn_info(void)
{
  dsat_result_enum_type result = DSAT_ERROR;
  mmgsdi_client_data_type  client_ref;
  mmgsdi_access_type  file_access;
  dsatme_mmgsdi_state_ms_info  *me_ms_val = NULL;
  me_ms_val = dsat_get_base_addr_current_session();

  memset(&file_access , 0 ,sizeof(mmgsdi_access_type));
  file_access.access_method = MMGSDI_EF_ENUM_ACCESS;
  file_access.file.file_enum = me_ms_val->dsat_sim_filename_plmn_selector;
  SET_PENDING(DSATETSI_EXT_ACT_CPOL_ETSI_IDX ,0, DSAT_PENDING_CPOL_WRITE)
  /* Validity of the index is done by getting the number of records present
    in the file. */
   client_ref = (mmgsdi_client_data_type)CPOL_MMGSDI_SIM_FILE_ATTR;
  /* Using the mmgsdi_get_file_attr function to get the number of records */ 
  if (MMGSDI_SUCCESS == mmgsdi_session_get_file_attr_ext(
      dsat_get_current_gw_session_id(FALSE),
          file_access, 
          (mmgsdi_callback_type) dsatme_mmgsdi_get_file_attr_cb,
          client_ref,
          TRUE ))
  {
    result = DSAT_ASYNC_CMD;
  }
  else
  {
    SET_PENDING(DSATETSI_EXT_ACT_CPOL_ETSI_IDX ,0, DSAT_PENDING_CPOL_NONE)
    return dsat_send_cme_error(DSAT_CME_SIM_FAILURE);
  }
  return result;
}/*dsatetsime_process_plmn_info*/

#endif /* FEATURE_DSAT_ETSI_MODE */

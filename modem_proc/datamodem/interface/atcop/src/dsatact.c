/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                D A T A   S E R V I C E S

                A T   C O M M A N D   
                ( C O M M O N   A C T I O N  
                  C O M M A N D S )

                P R O C E S S I N G

GENERAL DESCRIPTION
  This module executes the AT commands. It mainly executes the common 
  (across all modes) action commands.

EXTERNALIZED FUNCTIONS
  dsatact_exec_amps_cmd
    This function takes the result from the command line parser
    and executes it. This function executes configuration (AT&S)
    command. AT&S is used to manage the DSR control. 

  dsatact_exec_ampf_cmd
    This function takes the result from the command line parser
    and executes it. This function executes configuration (AT&F)
    command. AT&F is used to set to factory defined configuration. 

  dsatact_exec_atz_cmd
    This function takes the result from the command line parser
    and executes it. This function executes configuration (ATZ) 
    command. ATZ is used to reset to default configuration.

  dsatact_exec_ampw_cmd
    This function takes the result from the command line parser
    and executes it. It executes at&w command.
    AT&W is used to store S-registers and V.250 registers into NV.

  dsatact_exec_slashs_cmd
    This function takes the result from the command line parser
    and executes it. It executes at\s command.
    AT\S is used to print AT command settings.

  dsatact_exec_slash_q_cmd
    This function takes the result from the command line parser
    and executes it. This function executes AT\Q command.
    AT\Q is used to set serial port flow control.

  dsatact_exec_atd_cmd
    This function takes the result from the command line parser
    and executes it. It executes ATD command.
    ATD command is used to dial a call (ASYNC, PACKET, VOICE) using AT 
    commands

  dsatact_exec_ata_cmd
    This function takes the result from the command line parser
    and executes it. It executes ATA command.
    ATA command is used to answer a incoming call using AT commands

  dsatact_exec_ath_cmd
    This function takes the result from the command line parser
    and executes it. It executes ATH command.
    ATH command is used to hangupall using AT commands.
    Will not end a voice call.

  dsatact_exec_gmi_cmd
    This function takes the result from the command line parser
    and executes it. It executes +CGMI/+GMI command.
    This command returns manufacturer information.

  dsatact_exec_gmm_cmd
    This function takes the result from the command line parser
    and executes it. It executes +CGMM/+GMM command.
    This command returns model information.

  dsatact_exec_gmr_cmd
    This function takes the result from the command line parser
    and executes it. It executes +CGMR/+GMR command.
    This command returns revision information.

  dsatact_exec_gsn_cmd
    This function takes the result from the command line parser
    and executes it. It executes +CGSN command.
    This command returns serial number information.

  dsatact_exec_gcap_cmd
    This function takes the result from the command line parser
    and executes it. It executes +CGSN command.
    This command returns complete capabilities list information.

  dsatact_exec_ws46_cmd
    This function takes the result from the command line parser
    and executes it. It executes +WS46 command.
    This command manages wireless data service (WDS) side-stack selection.

  dsatact_call_abort_handler
    This function does nothing but return a result to indicate the
    call should be aborted.

  dsatact_exec_ato_cmd
    This function takes the result from the command line parser
    and executes it. It executes ATO command.
    ATO command is used to return from command to online data mode.
    
  dsatact_exec_ati_cmd
    This function takes the result from the command line parser
    and executes it. It executes I command.
    This command returns identification information.

  dsatact_exec_ate_cmd
    This function takes the result from the command line parser
    and executes it. It executes E command.
    This command controls echoing of the command line to TE.

  dsat_get_imei
    This function queries the IMEI value in NV and
    decodes it into an ASCII string.  The result is
    sent back to the caller in the passed buffer pointer.

INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2001 - 2014 by Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsatact.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/24/14   pg      Fixed ws46 featurization issue.
07/31/14   pg      Fixed Async data call on 3gpp2.
07/29/14   sc      Added support for NV Refresh.
06/27/14   tk      Added support for Dynamic ATCoP.
03/14/14   tk/sc   Optimized debug macros usage in ATCoP.
10/19/13   tk      Fixed issues in +CMUX command.
09/13/13   sc      Fixed KW errors.
05/08/13   tk      Renamed 3GPP2 +CMUX command to +QCMUX command.
02/22/13   tk      ATCoP - CM interface changes for Triton DSDA.
01/17/13   tk      Fixed KW errors.
01/04/13   tk      ATCoP changes for DSDX C/G+G with True World Mode Support.
11/30/12   tk      ATCoP changes for Triton DSDA.
08/10/12   sk      Fixed CLCC issue when PS call state is not valid.
08/22/12   sk      Mainlining Nikel Changes.
06/12/12   tk      Fixed +CGMM issue.
05/18/12   tk      Migrated to MSG 2.0 macros
03/29/12   sk      Fixed errors & warnings when FEATURE_DSAT_EXTENDED_CMD is enabled.
03/26/12   sk      Removed check for emergency numbers.
03/07/12   sk      Made CLCC common for all modes.
05/03/12   sk      Dun call failure fix on LTE.
02/17/12   sb      Fixed Compiler and KW Warnings.
02/17/12   sk      Migrating Data related AT commands interface to unified MH.
01/19/12   sk      Feature cleanup.
01/13/12   sa      Added support for CSVT for CSFB targets.
11/03/11   sb      Fixed KW Warnings.
10/25/11   nc      Adopted prototype change for the usage of 'ds_cnedsd_if_get_preferred_sys'.
09/02/11   ad      Having SYS mode check only for Data call.
05/26/11   dvk     Global Variables Cleanup
04/27/11   bs      Added NIKEL Phase I Support
03/04/11   ad      Reverted RIM changes for this file .
02/17/11   nc      Ignoring ending Waiting calls for ATH when CVHU is 1.
01/11/11   ad      Remove extern usage.used get/set API for command associated  
                   value pointers.
12/24/10   nc      Ignoring CM Mode check for ATD for DSDS.
10/25/10   ad      Init Changes for DSDS. 
09/28/10   ad      Added LTE support in +WS46.
08/22/10   vk      Using DSD for command mode check for Fusion
06/29/10   kk      Removing bbver.h dependency for BOOT_BLOCK_VERSION.
05/31/10   ad      Added support for S7 command.
05/10/10   kk      Fixes for CT-specific formatting in ATI.
01/06/10   ua      Extending CGMI, CGMM, CGMR for GOBI in CDMA mode. 
09/22/09   vrk	   Merged LTE changes.
06/11/09   nc      Modified the waiting call end using ATH to 
                   reflect the correct cause code.
06/14/09   nc      Fixed the border case of PDP activation for cid equals 16.
06/29/09   vg	   Replacing MSG_XXX with MSG_SPRINTF_X.
04/20/09   ua      Fixed lint errors.
01/30/09   ua      Added support of CGEV for DUN calls.
12/19/08   sa      Added check to dis-allow ATD call if there is mode mismatch
                   in case of Multi-mode target.
11/24/08   ss      Off target Lint fix.
10/23/08   bs      Added support for 1X AT Phonebook commands.
04/16/08   sa      Allow voice call in ETSI mode when a data call is present.
02/18/08   sa      Added modification for Passport Feature support.
12/20/07   sa      Fixed feature wrap issues.
11/24/07   ua      Fixed Lint-lows. 
10/26/07   ua      Added support for AT&S command. 
10/09/07   ss      Correcting the behaviour for GSM+1x targets.
09/27/07   sa      Added support for concurrent registration for CS data
                   and MT PDP handlers.
09/18/07   ss      Added support for ATDI and ATDL commands.
09/13/07   sa      Feature wrapped S2 register in FEATURE_DATA_S2_REG.
08/22/07   sa      Added new register function dsat_register_pdp_handlers() and  
                   modified existing dsat_register_handlers(),ATA and ATH command
                   handling functions for MT PDP support. 
08/06/07   pp      Added support for AT\Q command.
02/07/07   pp      New commands: AT&W, AT\S. Also, modified ATZ command.
06/20/07   ua      Fixes lint errors. 
06/12/07   ss      Fixes for +WS46 read on power up.
05/30/07   ua      Correcting +WS46 report result.
05/09/07   az      Changed FEATURE_JCDMA_2 to FEATURE_JCDMA_1X
05/02/07   ua      Fixed lint medium errors. 
04/11/07   ua      Modifications as per KDDI requirements. 
01/30/07   ss      Replaced banned string API calls
10/11/06   rsj     Modified dsati_dial_table for JCDMA_2 - ignore async circuit
                   origination.
08/03/06   ua      Adding 911 to the list of Emergency numbers
02/21/06   sa      Merged from Sirius Branch.
                   -Use global strings for manufacturer and model
                   -Added +CLIR support                                 
01/17/06   snb     Correct direct dial to accept quoted strings.
11/11/05   ar      Accomodate interface change to dsatvoice_is_voice_call().
10/14/05   ar      Set the SIOLOB modem port on ATA.
11/04/05   snb     Correct ATH command handler's setting of 
                   etsicall_cmd_pending.
11/03/05   ar      Make +WS46 responses consistent with spec CR, Lint fixes.
09/12/05   snb     Make get_imei() external function dsat_get_imei().
05/25/05   gr      Unfeaturized the ATA command handling so that it also 
                   works for CDMA 
04/21/05   snb     If CDMA-only build and no hangup CB registered ATH should
                   not end voice call(s).
04/13/05   snb     Add +DS to +GCAP response in WCDMA mode, correct +GCAP
                   response in Sirius builds to include +CGSM instead of 
                   +CGSMS.
04/05/05   ar      Add +CIND support for ATD command.
03/31/05   sb      Fixes from testing the serializer changes.
03/30/05   tkk     Featurized ATB command.
03/15/05   sb      Fixes from testing the serializer changes.
03/03/05   tkk     Added interdependency information for ATB command.
03/02/05   tkk     Added support for ATB command.
02/18/05   dvp     Removed the port check when ATD is executed.
02/18/05   dvp     Changed +CGMM command response to SXG75.
02/15/05   snb     Fix CDMA-only build issues.
02/14/05   dvp     Removed compilation warning.
02/10/05   sb      Accessories related changes.
01/17/05   sb      Merge from development branch
01/11/05   ar      Add support for +WS46 command.
01/05/05   pdv     Correcting +CGMM response.
12/15/04   snb     Allow ATH to hangup voice calls under +CVHU control.
12/29/04   snb     Correct behavior when packet dial strings contains
                   multiple CIDs.
10/22/04   ar      Stop timers on reset commands.  Lint cleanup.
10/21/04   snb     Correct behavior when packet dial strings contains
                   multiple CIDs.
09/13/04   snb     Fix 6500 build warning, add ATE command handler to allow
                   echo enable as default.
09/03/04   snb     Add +CHLD support.
04/20/04   snb     Reset PDP context info and PDP call setup delay tolerance 
                   on AT&F only if FEATURE_DATA_AMPF_RESET defined.
02/24/04   snb     Allow ATZ command to terminate call when its callback in 
                   dial_handler is not defined.
10/31/03   rsl     Increased info array size in +gcap processing. Fixed
                   bug where cap_list_2000 was overwriting +GCAP:
10/29/03   snb     Added support for $QCPINSTAT command and for async IMSI 
                   read from either card slot by +CIMI command.
09/02/03   ar      Added further validation to IMSI data processing.
08/24/03   sb      Range check for optional argument in ATI command
08/22/03   sb      Fixed compile issue
08/21/03   sb      Removed : AT&F should behave exactly like AT&Z.
08/20/03   ar      Adjust I cmd handler to accept but ignore arguments.
08/13/03   snb     Added support for $QCPDPLT command and for reading IMSI
                   from UIM card
07/31/03   ar      Added dial string dependence on +CSTA value.
06/27/03   snb     Only init LD record on successful call origination.
06/10/03   ar      Change interface to proc_dial_string.  Relocate packet 
                   call detection.
06/09/03   ar      Adjust dial string modifier test for CLIR.
05/01/03   sb      new return code (DSAT_ATH_CMD) when we are processing 
                   ATH command. Mainly to indicate that it is a delayed 
                   result code (from DSMGR) and we need to process the 
                   command line after we are doen executing ATH command.
04/23/03   dgy     Merged GSM and WCDMA CS Data mode-specific handlers.
04/18/03   vsk     changed to use the dsumts_rmsm interface rather than 
                   gen_rmsm 
03/12/03   ar      Scan entire dial string for I/G modifiers
02/26/03   wx      Put dialed number into last dial ph book.
02/25/03   rsl     Fixed typo., use mode instead of current_mode.
02/25/03   rsl     Execute atd only when in AT_CMD mode, otherwise return ERROR.
02/14/03   wx      Add ABORTING state to the abortable command to 
                   handle the aborting in async command processing.
02/03/03   ar      Add +ES in +GCAP list in WCDMA mode.
01/24/03   ar      Add dial string I/G/CID modifier support.
01/22/03   ar      Changed +GCAP to display +FCLASS only in GSM mode.
01/16/03   ar      AT&F now calls dsatetsipkt_reset_pdp_context_profiles()
11/22/02   sb      ATH Should not end Voice call.
11/14/02   jd      Simplified/optimized strcmp_pkt_dial_str, cleanup
11/11/02   sb      Restricted +GCAP command based on the current mode.
11/08/02   sb      Concurrent call changes.
11/08/02   sb      Return NO_CARRIER when Answer Call Back Handler is NULL.
11/07/02   ar      Incorporated code review comments
11/07/02   ar      Added FEATURE_GSM_GPRS to FEATURE_DATA_WCDMA_PS wrappers
10/29/02   sb      Fixed a merge issue that occured due to Voice changes
10/28/02   sb      Opened Voice call related handlers (moved changes to APIs)
10/23/02   ar      Added FEATURE_DATA_MM wrapper around packet rmsm items 
10/09/02   sb      Populated the dial table (GSM CS, WCDMA CS, WCDMA PS )
09/18/02   ar      Added/adjusted FEATURE_DATA_WCDMA_PS wrapper for GSM build
09/10/02   ak      Compilation for 1x changes.
09/10/02   atp     Added 707 async & pkt dial cb functions to dial table. 
09/10/02   ar      Wrap GSDI interface change under FEATURE_MMGSDI_<mode>
09/09/02   ar      Move dial character validation to utility function
                   dsatutil_validate_dial_char().
09/06/02   sb      Fix atd_cmd handler bugs.
09/05/02   sb      ATCOP changes based on new Multi mode Data services design
07/22/02   ak      made Proc_dial_string non-LOCAL
06/05/02   ar      Relocated dsatact_enter_online_data_mode from DS3GMGR
04/17/02   rc      Added stack_type as parameter to function call
                   ds3g_initiate_call().
08/27/01   sb      ATCOP code is now partitioned into multiple VUs. As a 
                   result of the partitioning, some of the files have to be
                   renamed. So making the file/interface name changes.
08/10/01   sjd     Initial release to MSM5200 archives.
06/01/01   sb      Created module.
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include <stringl/stringl.h>


#include <stdio.h>
#include <string.h>

#include "dsati.h"
#include "dsatact.h"
#include "dsatctab.h"
#include "dsatparm.h"
#include "dsatvoice.h"
#include "dsatcmif.h"
#include "ds_cap.h" /* ITC_SPEECH */
#include "ds3gmgr.h"
#include "dstask.h"
#include "dstaski.h"
#include "err.h"
#include "mobile.h"
#include "msg.h"
#include "ds3gsiolib_ex.h"

#ifdef FEATURE_DSAT_ETSI_MODE
#include "dsatetsime.h"
#include "dsatetsicall.h"
#include "dsatetsicmif.h"
#include "dsatetsictab.h"

#ifdef FEATURE_DSAT_ETSI_DATA
#include "dsumts_rmsm.h"
#include "dsatetsipkt.h"
#endif /* FEATURE_DSAT_ETSI_DATA*/
#endif /* FEATURE_DSAT_ETSI_MODE */

#if defined(FEATURE_DATA_GCSD) || defined(FEATURE_DATA_WCDMA_CS) 
#include "dsucsdhdlr.h"
#endif /* defined(FEATURE_DATA_GCSD) || defined(FEATURE_DATA_WCDMA_CS) */

#ifdef FEATURE_DATA_IS707
#include "dsat707util.h"
#include "dsat707extctab.h"
#include "ds707_rmsm.h"
#include "ds707_async_mgr.h"
#include "snm.h"
#include "cai.h"
#include "ds707_roaming.h"
#include "ds707_pkt_mgr.h"
#endif /* FEATURE_DATA_IS707 */

#ifdef FEATURE_DATA_MUX
#include "ds_mux_ext_defs.h"
#endif /* FEATURE_DATA_MUX */
#include "ds3gdsdif.h"

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/
/*-------------------------------------------------------------------------
            Constants:
-------------------------------------------------------------------------*/

#ifdef FEATURE_DSAT_ETSI_MODE
/*-------------------------------------
  Dial string for ETSI Emergency calls.
---------------------------------------*/
/* Allowing both 112 and 911 as Emergency numbers */
LOCAL const byte dsat_emergency_call_str_1[] = "112";
LOCAL const byte dsat_emergency_call_str_2[] = "911";

#endif /* FEATURE_DSAT_ETSI_MODE */

typedef enum dsat_clcc_err_code
{
  DSAT_CLCC_ERR_NONE = -1,
    /* No error */

  DSAT_CLCC_ERR_SEQ_NUM = 0,
    /* Sequence number error */

  DSAT_CLCC_ERR_CALL_DIRECTION,
    /* Call direction error */

  DSAT_CLCC_ERR_CONV_STATE,
    /* Conversation state error */

  DSAT_CLCC_ERR_CALL_STATE,
    /* Call state error */

  DSAT_CLCC_ERR_CALL_MODE,
    /* Call mode error */

  DSAT_CLCC_ERR_CALL_TYPE,
    /* Call type error */

  DSAT_CLCC_ERR_MAX
} dsat_clcc_err_code_e_type;

/*-------------------------------------
  Local Variables
---------------------------------------*/
/* ATCOP and PS split up support */

/* ATD Dial Table */
/* This table has different call handlers in different mode        */
/* Row is for different mode ( GSM, WCDMA, CDMA in that order)     */
/* Column is for different calls ( Circuit, Packet in that order ) */
/* Example:                                                        */
/*  { gsm_ckt_f,   gsm_pkt_f   },                                  */
/*  { wcdma_ckt_f, wcdma_pkt_f },                                  */
/*  { cdma_ckt_f,  cdma_pkt_f  }                                   */
/*                                                                 */ 
/* gsm_ckt_f will have the following prototype                     */
/* dsat_call_abort_cb_type gsm_ckt_f                               */
/* (                                                               */
/*  const byte            *dial_string,                            */
/*  boolean                digit_mode                              */
/* );                                                              */

dsat_dial_type dsati_dial_table[DSAT_MODE_MAX][DSAT_MAX_DIAL_TYPE]
= 
{

  { 
    /* GSM Circuit Call handler */
    #ifdef FEATURE_DATA_GCSD
    ds_ucsd_dial_string_handler
    #else
    NULL
    #endif /* (FEATURE_DATA_GCSD) */
    ,  
    /* GSM Packet Call handler   */
    #ifdef FEATURE_GSM_GPRS
    dsumts_rmsm_dial_str_cb 
    #else
    NULL   
    #endif /* (FEATURE_GSM_GPRS) */
  },  
  {
    /* WCDMA Circuit Call handler */   
    #ifdef FEATURE_DATA_WCDMA_CS
    ds_ucsd_dial_string_handler
    #else
    NULL
    #endif /* FEATURE_DATA_WCDMA_CS */
    ,
    /* WCDMA Packet Call handler */
    #ifdef FEATURE_DATA_WCDMA_PS
    dsumts_rmsm_dial_str_cb 
    #else
    NULL 
    #endif /* FEATURE_DATA_WCDMA_PS */

  },
  {
    /* Circuit Switch data Handler */
 
    #if (defined(FEATURE_3GPP_CSFB) && (defined(FEATURE_DATA_GCSD) || defined(FEATURE_DATA_WCDMA_CS) ))
    ds_ucsd_dial_string_handler 
    #else
    NULL 
    #endif /* FEATURE_3GPP_CSFB && (FEATURE_DATA_GCSD || FEATURE_DATA_WCDMA_CS)*/
    ,
    /* LTE Packet call handler */
    #ifdef FEATURE_DATA_LTE
    dsumts_rmsm_dial_str_cb
    #else
    NULL
    #endif
  },

  { 
    /* TDS Circuit Call handler */   
    #ifdef FEATURE_DATA_WCDMA_CS
    ds_ucsd_dial_string_handler
    #else
    NULL
    #endif /* FEATURE_DATA_WCDMA_CS */
    ,
    /* TDS Packet Call handler */
    #ifdef FEATURE_TDSCDMA
    dsumts_rmsm_dial_str_cb 
    #else
    NULL 
    #endif 
  },

  { 
    /* CDMA Circuit Call handler */
#ifdef FEATURE_DATA_IS707_ASYNC
    ds707_async_dial_str_cb
#else
    NULL
#endif /* (FEATURE_DATA_IS707_ASYNC) */
    ,
    /*  CDMA Packet Call handler  */
    #ifdef FEATURE_DATA_IS707
    ds707_rmsm_dial_str_cb
    #else
    NULL
    #endif /* (FEATURE_DATA_IS707) */
  }  
};

/*-------------------------------------
  Regional Variables
---------------------------------------*/
/* indicates the current abort handler */
dsat_call_abort_cb_type dsat_curr_abort_handler = NULL;
#ifdef FEATURE_MDM_FUSION
/* indicates current Rm-Sm proxy abort handler */
dsat_proxy_call_abort_cb_type dsat_proxy_curr_abort_handler = NULL;
#endif /* FEATURE_MDM_FUSION */

#ifdef FEATURE_DSAT_ETSI_MODE
extern const def_list_type dsat_ws46_list[];
#endif /* FEATURE_DSAT_ETSI_MODE */

#ifdef FEATURE_DSAT_GOBI_MAINLINE
#if defined (FEATURE_CDMA_800) || defined (FEATURE_CDMA_1900)
#if defined (FEATURE_OTASP)
/*----------------------------------
  Dial string for OTASP calls
------------------------------------*/
LOCAL const byte dsat_otasp_call_str[]="*22899";
extern cm_call_id_type   voice_call_id;
#endif/*defined (FEATURE_OTASP)*/
#endif /*defined (FEATURE_CDMA_800) || defined (FEATURE_CDMA_1900)*/
#endif /*FEATURE_DSAT_GOBI_MAINLINE*/

#ifdef FEATURE_DATA_MUX
static ds_mux_cmux_info_type dsat_cmux_info ;
static ds3g_siolib_port_e_type dsat_curr_cmux_port = DS3G_SIOLIB_PORT_MAX ;
#endif /* FEATURE_DATA_MUX */
/*-------------------------------------------------------------------------
    Protypes for local functions:
-------------------------------------------------------------------------*/
LOCAL void enter_online_data_mode
(
  boolean flush_tx      /* Flag to flush SIO TX fifo prior to switch */
);

LOCAL dsat_result_enum_type process_amps_cmd
(
  void
);
LOCAL boolean proc_dial_str
(
  /* cmd token containing the atd argument       */
  const tokens_struct_type  *   tok_ptr,    

  /* dial string contain the parsed information
     This is output                              */
  dsati_dial_str_type       *   dial_ptr,

  /* current system operating mode               */
  dsati_mode_e_type         *   cur_mode_ptr
);

#ifdef FEATURE_DATA_MUX
LOCAL void dsatact_sio_flush_tx_cb(void);

LOCAL void dsatact_cmux_notify_cb
(
  int port_state, 
  int port_operation_state
);

LOCAL void dsatact_rdm_notify_cb
(
  rdm_assign_status_type status,
  rdm_service_enum_type service,
  rdm_device_enum_type device
);
#endif /* FEATURE_DATA_MUX */

#ifdef FEATURE_DSAT_GOBI_MAINLINE
#if defined (FEATURE_CDMA_800) || defined (FEATURE_CDMA_1900)
#if defined (FEATURE_OTASP)
LOCAL dsat_result_enum_type dsatact_exec_atd_otasp_call
(
  dsati_dial_str_type* dial_str_ptr
);
#endif/*defined (FEATURE_OTASP)*/
#endif /*defined (FEATURE_CDMA_800) || defined (FEATURE_CDMA_1900)*/
#endif /*FEATURE_DSAT_GOBI_MAINLINE*/

LOCAL int dsatact_callid_to_seqnum
(
  sys_modem_as_id_e_type   subs_id,
  cm_call_id_type          call_id
);

/*-------------------------------------------------------------------------
            Function Definitions:
-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION DSATCMDP_IS_PREF_PKT_MODE

DESCRIPTION
  This function checks and returns TRUE if the provided sys mode is also 
  the preferred sys mode for packet data service

DEPENDENCIES
  None
  
RETURN VALUE
  returns boolean
    TRUE : if the indicated sys mode is the preferred mode for data service
    FALSE : otherwise
  
SIDE EFFECTS
  None.

===========================================================================*/
LOCAL boolean dsatcmdp_is_pref_pkt_mode
( 
  sys_sys_mode_e_type mode,
  dsat_num_item_type subs_id
)
{
#ifdef FEATURE_DATA_DSD

  sys_sys_mode_e_type pref_mode; 

 //NOTE ruishi check with ATCOP on how to pass sub id
  pref_mode = ds3gdsdif_get_preferred_sys(
                        ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id));

  DS_AT_MSG2_HIGH("%d mode input, %d dsd_pref_mode",mode,pref_mode);

  if (pref_mode == SYS_SYS_MODE_HDR) 
  {
    pref_mode = SYS_SYS_MODE_CDMA;
  }
  
  if (pref_mode == SYS_SYS_MODE_NO_SRV) 
  {
    return ds3g_is_pref_pkt_mode_ex(mode,subs_id);
  }
  return (pref_mode == mode);

#else
  return ds3g_is_pref_pkt_mode_ex(mode,subs_id);
#endif /* FEATURE_DATA_DSD */

}

/*===========================================================================

FUNCTION DSATACT_EXEC_AMPC_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. This function executes AT&C command. 
  AT&C is used to set the behaviour of circuit 109 relates
  to the recieved line signal from the distant end.
  
DEPENDENCIES
  NONE

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  None

===========================================================================*/
/*ARGSUSED*/
dsat_result_enum_type dsatact_exec_ampc_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{

#ifdef FEATURE_DATA_IS707
  dsat_num_item_type  old_amp_c_val = (dsat_num_item_type)dsatutil_get_val(
                                      DSAT_BASIC_N_C_IDX,0,0,NUM_TYPE);
#endif /* FEATURE_DATA_IS707 */
  /*------------------------------------------------
              Standard Processing 
  --------------------------------------------------*/
  if ( DSAT_OK != dsatparm_exec_param_cmd(
                                   mode,
                                   parse_table,
                                   tok_ptr,
                                   res_buff_ptr
                                  ))
  {
    return DSAT_ERROR;
  }

#ifdef FEATURE_DATA_IS707
  if(IS_JCDMA_MODE())
  {
    /* only 0 and 1 are valid values, rest will return ERROR */
    if( (dsat_num_item_type)dsatutil_get_val(
                                  DSAT_BASIC_N_C_IDX,0,0,NUM_TYPE) > 1 )
    {
      DSATUTIL_SET_VAL(DSAT_BASIC_N_C_IDX,0,0,0,old_amp_c_val,NUM_TYPE)
      return DSAT_ERROR;
    }
  }
#endif /* FEATURE_DATA_IS707 */
  return DSAT_OK;
}/* dsatact_exec_ampc_cmd */

/*===========================================================================

FUNCTION DSATACT_EXEC_ICF_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. This function executes AT+ICF command. 
  AT+ICF is used to set the TE TA character framing .
  
DEPENDENCIES
  NONE

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  None

===========================================================================*/
/*ARGSUSED*/
dsat_result_enum_type dsatact_exec_icf_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
#ifdef FEATURE_DATA_IS707
  dsat_num_item_type    old_dsat_icf_val[2];
(void) dsatutil_memscpy((void*)old_dsat_icf_val,sizeof(old_dsat_icf_val),
          (void*)dsatutil_get_val(DSAT_EXT_ICF_IDX,0,0,STR_TYPE),
         sizeof(old_dsat_icf_val));

#endif /* FEATURE_DATA_IS707 */
  /*------------------------------------------------
              Standard Processing 
  --------------------------------------------------*/
  if ( DSAT_OK != dsatparm_exec_param_cmd(
                                   mode,
                                   parse_table,
                                   tok_ptr,
                                   res_buff_ptr
                                  ))
  {
    return DSAT_ERROR;
  }
#ifdef FEATURE_DATA_IS707
  if(IS_JCDMA_MODE() )
  {
    /* only value of 3 is VALID for parity in JCDMA mode. So 
     * return ERROR for rest of the values and restore old values.
     */
    if( (dsat_num_item_type)dsatutil_get_val(
                                  DSAT_EXT_ICF_IDX,0,1,NUM_TYPE) != 3)
    {
      (void) dsatutil_memscpy((void*)dsatutil_get_val(DSAT_EXT_ICF_IDX,0,0,STR_TYPE),
                 sizeof(old_dsat_icf_val),
                 (void*)old_dsat_icf_val,
             sizeof(old_dsat_icf_val));
      
      return DSAT_ERROR;
    }
  }
#endif /* FEATURE_DATA_IS707 */
  return DSAT_OK;
}/* dsatact_exec_icf_cmd */

/*===========================================================================

FUNCTION DSATACT_EXEC_IFC_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. This function executes AT+IFC command. 
  AT+IFC is used to set TE-TA local flow control .
  
DEPENDENCIES
  NONE

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  None

===========================================================================*/
/*ARGSUSED*/
dsat_result_enum_type dsatact_exec_ifc_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
#ifdef FEATURE_DATA_IS707
  dsat_num_item_type    old_dsat_ifc_val[2];
  
  (void) dsatutil_memscpy((void*)old_dsat_ifc_val,sizeof(old_dsat_ifc_val),
            (void*)dsatutil_get_val(DSAT_EXT_IFC_IDX,0,0,STR_TYPE),
         sizeof(old_dsat_ifc_val));
#endif /* FEATURE_DATA_IS707 */
  /*------------------------------------------------
              Standard Processing 
  --------------------------------------------------*/
  if ( DSAT_OK != dsatparm_exec_param_cmd(
                                   mode,
                                   parse_table,
                                   tok_ptr,
                                   res_buff_ptr
                                  ))
  {
    return DSAT_ERROR;
  }
#ifdef FEATURE_DATA_IS707
  if(IS_JCDMA_MODE() )
  {
    /* only value of 2 is VALID for TE TA local flow control 
     * in JCDMA mode. So return ERROR for rest of the values 
     * and restore old values.
     */
    if( ( (dsat_num_item_type)dsatutil_get_val(
          DSAT_EXT_IFC_IDX,0,0,NUM_TYPE) != 2 ) || 
        ( (dsat_num_item_type)dsatutil_get_val(
          DSAT_EXT_IFC_IDX,0,1,NUM_TYPE) != 2 ) )
    {
      (void) dsatutil_memscpy((void*)dsatutil_get_val(DSAT_EXT_IFC_IDX,0,0,STR_TYPE),
                               sizeof(old_dsat_ifc_val),
                              (void*)old_dsat_ifc_val,
             sizeof(old_dsat_ifc_val));
      return DSAT_ERROR;
    }
  }
#endif /* FEATURE_DATA_IS707 */
  return DSAT_OK;
}/* dsatact_exec_ifc_cmd */

/*===========================================================================

FUNCTION DSATACT_EXEC_AMPS_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. This function executes AT&S command. 
  AT&S is used to set the management of  DSR control.
  
DEPENDENCIES
  NONE

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  If the DUN fails, check the value of &C command. This has been verified
  on UART and USB which worked with &C set to 1 and 2. If different DUN 
  application is used and the call doesn't go through then try different 
  options of &C command. 

===========================================================================*/
/*ARGSUSED*/
dsat_result_enum_type dsatact_exec_amps_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr       /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  /*------------------------------------------------
              Standard Processing 
  --------------------------------------------------*/
  if ( DSAT_OK != dsatparm_exec_param_cmd(
                                   mode,
                                   parse_table,
                                   tok_ptr,
                                   res_buff_ptr
                                  ))
  {
    return DSAT_ERROR;
  }
  result = process_amps_cmd();

  return result;
}/* dsatact_exec_amps_cmd */

/*===========================================================================

FUNCTION DSATACT_EXEC_AMPF_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. This function executes AT&F command. 
  AT&F is used to set to factory defined configuration. 

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  Sets parameter value for "SET"  command.

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatact_exec_ampf_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result;
  dsat_num_item_type temp_ipr_val;
  
  result = dsatparm_exec_param_cmd(
                                   mode,
                                   parse_table,
                                   tok_ptr,
                                   res_buff_ptr
                                  );

  if ( result == DSAT_OK )
  {

    /* Initialize all AT parameters to their default values */
#ifdef FEATURE_DATA_IS707
    ds707_pkt_mgr_nv_sync();
    ds707_pkt_mgr_qcmip_set_crm_val();
    ds707_pkt_mgr_qcmip_set_mdr_val();
    ds707_so_pkt_recal();
#endif /*FEATURE_DATA_IS707*/
    dsatcmdp_init_config ();
    temp_ipr_val = (dsat_num_item_type)dsatutil_get_val(DSAT_EXT_IPR_IDX,0,0,NUM_TYPE);
    (void) ds3g_siolib_change_baud_rate((sio_bitrate_type)temp_ipr_val , FALSE );

#if defined FEATURE_DATA_AMPF_RESET
    /* Reset all persistent PDP context profiles to default */
    if (DSAT_OK != dsatetsipkt_reset_pdp_context_profiles())
    {
      result = DSAT_ERROR;
    }

    /* Reset PDP call setup delay tolerance to default */
    dsatetsipkt_reset_tolerance();
#endif /* defined FEATURE_DATA_AMPF_RESET */
  }
  else 
  {
    result = DSAT_ERROR;
  }
  return result;
}/* dsatact_exec_ampf_cmd */



/*===========================================================================

FUNCTION DSATACT_EXEC_ATZ_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. This function executes ATZ, command. 
  ATZ is used to reset to default configuration. 

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  Sets parameter value for "SET"  command.

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatact_exec_atz_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_num_item_type retained_ipr_val;
  dsat_result_enum_type result = DSAT_OK;
#ifdef FEATURE_DS_IS707A_CMUX
  word  fwd_mux;  /* Forward multiplex option */
  uint8 temp_val;
#endif /* FEATURE_DS_IS707A_CMUX */

  result = dsatparm_exec_param_cmd(
                                   mode,
                                   parse_table,
                                   tok_ptr,
                                   res_buff_ptr
                                  );

  if ( result == DSAT_OK )
  {
    if ( mode != DSAT_ONLINE_DATA )
    {
      nv_stat_enum_type    nv_status;
      /* Data Port's Baud Rate should not be changed in ATZ command */
      /* Retain the IPR value and restore it later for ATZ command  */
      retained_ipr_val = (dsat_num_item_type)dsatutil_get_val(
                                        DSAT_EXT_IPR_IDX,0,0,NUM_TYPE);

      /* Initialize all AT parameters to their default values */
#ifdef FEATURE_DATA_IS707
      ds707_pkt_mgr_nv_sync();
      ds707_pkt_mgr_qcmip_set_crm_val();
      ds707_pkt_mgr_qcmip_set_mdr_val();
      ds707_so_pkt_recal();
      dsatcmdp_init_config ();
#endif /*FEATURE_DATA_IS707*/

      DSATUTIL_SET_VAL(DSAT_EXT_IPR_IDX,0,0,0,retained_ipr_val,NUM_TYPE)
      /* Restore S registers & V.250 registers setting from NV */

      /*-----------------------------------------------------------------------
      Read the stored V.250 registers from the NV.
      -----------------------------------------------------------------------*/
      nv_status = dsatutil_get_nv_item(NV_DS_AT_V250_REGISTERS_I, &ds_nv_item);

      if( nv_status == NV_DONE_S)
      {
#ifdef FEATURE_ATCOP_ECHO_CTL_MULTI_PORTS
        /*each modem port uses 1 bit of the e_reg_val, modem port 1 uses bit 0, modem port 2 uses bit 1*/
        DSATUTIL_SET_VAL(DSAT_BASIC_E_IDX,0,0,0,ds_nv_item.ds_at_v250_registers.e_reg_val &0x01,NUM_TYPE)
        DSATUTIL_SET_VAL(DSAT_BASIC_E_IDX,0,1,0,ds_nv_item.ds_at_v250_registers.e_reg_val &0x02) >> 1,NUM_TYPE)
#else
        DSATUTIL_SET_VAL(DSAT_BASIC_E_IDX,0,0,0,ds_nv_item.ds_at_v250_registers.e_reg_val,NUM_TYPE)
#endif /*FEATURE_ATCOP_ECHO_CTL_MULTI_PORTS*/
        DSATUTIL_SET_VAL(DSAT_BASIC_Q_IDX,0,0,0,ds_nv_item.ds_at_v250_registers.q_reg_val,NUM_TYPE)
        DSATUTIL_SET_VAL(DSAT_BASIC_V_IDX,0,0,0,ds_nv_item.ds_at_v250_registers.v_reg_val,NUM_TYPE)
        DSATUTIL_SET_VAL(DSAT_BASIC_X_IDX,0,0,0,ds_nv_item.ds_at_v250_registers.x_reg_val,NUM_TYPE)
        DSATUTIL_SET_VAL(DSAT_BASIC_N_C_IDX,0,0,0,ds_nv_item.ds_at_v250_registers.amp_c_reg_val,NUM_TYPE)
        DSATUTIL_SET_VAL(DSAT_BASIC_N_D_IDX,0,0,0,ds_nv_item.ds_at_v250_registers.amp_d_reg_val,NUM_TYPE)
        DSATUTIL_SET_VAL(DSAT_BASIC_N_S_IDX,0,0,0,ds_nv_item.ds_at_v250_registers.amp_s_reg_val,NUM_TYPE)
        DSATUTIL_SET_VAL(DSAT_EXT_IFC_IDX,0,0,0,ds_nv_item.ds_at_v250_registers.ifc_reg_val[0],NUM_TYPE)
        DSATUTIL_SET_VAL(DSAT_EXT_IFC_IDX,0,1,0,ds_nv_item.ds_at_v250_registers.ifc_reg_val[1],NUM_TYPE)
        DSATUTIL_SET_VAL(DSAT_BASIC_DS_V_IDX,0,0,0,ds_nv_item.ds_at_v250_registers.slash_v_reg_val,NUM_TYPE)
      }

      /*-----------------------------------------------------------------------
      Read the stored S registers from the NV.
      -----------------------------------------------------------------------*/
      nv_status = dsatutil_get_nv_item(NV_DS_AT_S_REGISTERS_I, &ds_nv_item);
  
      if( nv_status == NV_DONE_S)
      {
        DSATUTIL_SET_VAL(DSAT_SREG_S0_IDX,0,0,0,ds_nv_item.ds_at_s_registers.s0_reg_val,NUM_TYPE)
        DSATUTIL_SET_VAL(DSAT_SREG_S7_IDX,0,0,0,ds_nv_item.ds_at_s_registers.s7_reg_val,NUM_TYPE)
        DSATUTIL_SET_VAL(DSAT_SREG_S10_IDX,0,0,0,ds_nv_item.ds_at_s_registers.s10_reg_val,NUM_TYPE)

      }
      /* AT&S is restored. Process &S value */
      if ( DSAT_OK != process_amps_cmd())
      {
        return DSAT_ERROR;
      }

#ifdef FEATURE_DS_IS707A_CMUX
      /* Get the forward multiplex option */
      ds707_pkt_mgr_get_1x_profile_val(DS707_CMUX_VAL,(void *)&temp_val,1);
      fwd_mux = dsat707_cmux_fwd_table[(dsat_num_item_type)temp_val];

      /* Set the maximum multiplex option for service option 22 */
      if( fwd_mux & 0x01 )
      {
        /* Set the maximum number of forward supplementals as per the
           AT+QCMUX default */
        (void)snm_set_max_for_sups( CAI_SO_MDR_PKT_DATA_FRS1_RRS1,
                            (byte)((fwd_mux-1)/2) );
      }
      else
      {
        /* Set the maximum number of forward supplementals as per the
                     CAI default */
        (void)snm_set_max_for_sups( CAI_SO_MDR_PKT_DATA_FRS1_RRS1,
                            (byte)((CAI_MAX_USABLE_MUX_RS1-1)/2) );
      }

      /* Set the maximum multiplex option for service option 25 */
      if( !(fwd_mux & 0x01) )
      {
        /* Set the maximum number of forward supplementals as per the
           AT+QCMUX default */
        (void)snm_set_max_for_sups( CAI_SO_MDR_PKT_DATA_FRS2_RRS2,
                            (byte)((fwd_mux-2)/2) );
      }
      else
      {
        /* Set the maximum number of forward supplementals as per the
                     CAI default */
        (void)snm_set_max_for_sups( CAI_SO_MDR_PKT_DATA_FRS2_RRS2,
                            (byte)((CAI_MAX_USABLE_MUX_RS2-2)/2) );
      }
#endif  /* FEATURE_DS_IS707A_CMUX */

    }

    /* If ATZ handler available use it, 
       assuming here that it will correctly terminate call */
    if (  dial_string_handler.atz_cb != NULL )
    {
      dial_string_handler.atz_cb();
    }    
    /* Else rely on hangup handler */
    else if ( dial_string_handler.hangup_cb != NULL )
    {
      dial_string_handler.hangup_cb();
      result = DSAT_ATH_CMD;
    }
  }
  else 
  {
    result = DSAT_ERROR;
  }

  return result;

}/* dsatact_exec_atz_cmd */

/*===========================================================================

FUNCTION DSATACT_EXEC_AMPW_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at&w command.
  at&w is used to store S-registers and V.250 registers into NV.

DEPENDENCIES
  None.

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed.
    DSAT_ERROR : if there was any problem in executing the command.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatact_exec_ampw_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{

  dsat_result_enum_type     result;
  dsat_num_item_type                     temp1;
#ifdef FEATURE_ATCOP_ECHO_CTL_MULTI_PORTS
  dsat_num_item_type                    temp2;
#endif /* FEATURE_ATCOP_ECHO_CTL_MULTI_PORTS */
  /*----------------------------------------------------------------
  Do standard processing first.
  -----------------------------------------------------------------*/
  result = dsatparm_exec_param_cmd( mode,
                                    parse_table,
                                    tok_ptr,
                                    res_buff_ptr
                                  );
  if ( result == DSAT_OK )
  {
    /*-----------------------------------------------------------------------
    DSAT OK, now store S-registers & V.250 settings into NV.
    -----------------------------------------------------------------------*/
    DS_AT_MSG0_MED("Storing S registers & V.250 registers into NV");

    /* V.250 */
    memset ((void*)&ds_nv_item, 0, sizeof(ds_nv_item));
#ifdef FEATURE_ATCOP_ECHO_CTL_MULTI_PORTS
    /*each modem port uses 1 bit of the e_reg_val, modem port 1 uses bit 0, modem port 2 uses bit 1*/
    temp1 = (dsat_num_item_type)dsatutil_get_val(DSAT_BASIC_E_IDX,0,0,NUM_TYPE);
    temp2 = ((dsat_num_item_type)dsatutil_get_val(DSAT_BASIC_E_IDX,0,1,NUM_TYPE))<<1;
    ds_nv_item.ds_at_v250_registers.e_reg_val = (uint8)( temp1 | temp2);
#else
    temp1 = (dsat_num_item_type)dsatutil_get_val(DSAT_BASIC_E_IDX,0,0,NUM_TYPE);
    ds_nv_item.ds_at_v250_registers.e_reg_val =(uint8) temp1;
#endif /*FEATURE_ATCOP_ECHO_CTL_MULTI_PORTS*/
    temp1 =(dsat_num_item_type)dsatutil_get_val(DSAT_BASIC_Q_IDX,0,0,NUM_TYPE);
    ds_nv_item.ds_at_v250_registers.q_reg_val = (uint8)temp1;
    temp1 =(dsat_num_item_type)dsatutil_get_val(DSAT_BASIC_V_IDX,0,0,NUM_TYPE);
    ds_nv_item.ds_at_v250_registers.v_reg_val = (uint8)temp1;
    temp1 =(dsat_num_item_type)dsatutil_get_val(DSAT_BASIC_X_IDX,0,0,NUM_TYPE);
    ds_nv_item.ds_at_v250_registers.x_reg_val = (uint8)temp1;
    temp1 = (dsat_num_item_type)dsatutil_get_val(DSAT_BASIC_N_C_IDX,0,0,NUM_TYPE);
    ds_nv_item.ds_at_v250_registers.amp_c_reg_val = (uint8)temp1;
    temp1 = (dsat_num_item_type)dsatutil_get_val(DSAT_BASIC_N_D_IDX,0,0,NUM_TYPE);
    ds_nv_item.ds_at_v250_registers.amp_d_reg_val = (uint8)temp1;
    temp1 = (dsat_num_item_type)dsatutil_get_val(DSAT_BASIC_N_S_IDX,0,0,NUM_TYPE);
    ds_nv_item.ds_at_v250_registers.amp_s_reg_val = (uint8)temp1;
    temp1 = (dsat_num_item_type)dsatutil_get_val(DSAT_EXT_IFC_IDX,0,0,NUM_TYPE);
    ds_nv_item.ds_at_v250_registers.ifc_reg_val[0] = (uint8)temp1;
    temp1 = (dsat_num_item_type)dsatutil_get_val(DSAT_EXT_IFC_IDX,0,1,NUM_TYPE);
    ds_nv_item.ds_at_v250_registers.ifc_reg_val[1] = (uint8)temp1;
    temp1 = (dsat_num_item_type)dsatutil_get_val(DSAT_BASIC_DS_V_IDX,0,0,NUM_TYPE);
    ds_nv_item.ds_at_v250_registers.slash_v_reg_val = (uint8)temp1;
    (void)dsatutil_put_nv_item(NV_DS_AT_V250_REGISTERS_I, &ds_nv_item);

    /* S Registers */
    memset ((void*)&ds_nv_item, 0, sizeof(ds_nv_item));
    temp1 =(dsat_num_item_type)dsatutil_get_val(DSAT_SREG_S0_IDX,0,0,NUM_TYPE);
    ds_nv_item.ds_at_s_registers.s0_reg_val = (uint8)temp1;
    temp1 = (dsat_num_item_type)dsatutil_get_val(DSAT_SREG_S7_IDX,0,0,NUM_TYPE);
    ds_nv_item.ds_at_s_registers.s7_reg_val = (uint8)temp1;
    temp1 = (dsat_num_item_type)dsatutil_get_val(DSAT_SREG_S10_IDX,0,0,NUM_TYPE);
    ds_nv_item.ds_at_s_registers.s10_reg_val = (uint8)temp1;
    (void)dsatutil_put_nv_item(NV_DS_AT_S_REGISTERS_I, &ds_nv_item);
  }

  return result;
}/* dsatact_exec_ampw_cmd */

/*===========================================================================

FUNCTION DSATACT_EXEC_SLASHS_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at\s command.
  at\s is used to print AT command settings.

DEPENDENCIES
  None.

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed.
    DSAT_ERROR : if there was any problem in executing the command.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatact_exec_slashs_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type     result;
#ifdef FEATURE_ATCOP_ECHO_CTL_MULTI_PORTS
  dsat_num_item_type                     temp1;
  dsat_num_item_type                     temp2;
#endif /* FEATURE_ATCOP_ECHO_CTL_MULTI_PORTS*/
  char format_str[] = "E%d Q%d V%d X%d &C%d &D%d &S%d &E%d \\V%d\n"
                      "S000=%03d S002=%03d S003=%03d S004=%03d S005=%03d\n"
                      "S006=%03d S007=%03d S008=%03d S010=%03d S030=%03d\n"
                      "S103=%03d S104=%03d";
  /*----------------------------------------------------------------
  Do standard processing first.
  -----------------------------------------------------------------*/
  result = dsatparm_exec_param_cmd ( mode,
                                     parse_table,
                                     tok_ptr,
                                     res_buff_ptr
                                   );

  if (result == DSAT_OK)
  {
    /* Print V.250 and S registers */
#ifdef FEATURE_ATCOP_ECHO_CTL_MULTI_PORTS

    temp1 = (dsat_num_item_type)dsatutil_get_val(DSAT_BASIC_E_IDX,0,0,NUM_TYPE);
    temp2 = (dsat_num_item_type)dsatutil_get_val(DSAT_BASIC_E_IDX,0,1,NUM_TYPE)<< 1;
#endif /* FEATURE_ATCOP_ECHO_CTL_MULTI_PORTS */
    res_buff_ptr->used = (word)snprintf ((char*)res_buff_ptr->data_ptr,
                           res_buff_ptr->size,
                           format_str,
#ifdef FEATURE_ATCOP_ECHO_CTL_MULTI_PORTS
  /*each modem port uses 1 bit of the e_reg_val, modem port 1 uses bit 0, modem port 2 uses bit 1*/
                           (dsat_num_item_type)( temp1 | temp2 ),
#else
                                                            
                           (dsat_num_item_type)dsatutil_get_val(
                                        DSAT_BASIC_E_IDX,0,0,NUM_TYPE), 
#endif /*FEATURE_ATCOP_ECHO_CTL_MULTI_PORTS*/
                           (dsat_num_item_type)dsatutil_get_val(
                                      DSAT_BASIC_Q_IDX,0,0,NUM_TYPE), 
                           (dsat_num_item_type)dsatutil_get_val(
                                      DSAT_BASIC_V_IDX,0,0,NUM_TYPE),
                           (dsat_num_item_type)dsatutil_get_val(
                                      DSAT_BASIC_X_IDX,0,0,NUM_TYPE), 
                           (dsat_num_item_type)dsatutil_get_val(
                                      DSAT_BASIC_N_C_IDX,0,0,NUM_TYPE),
                           (dsat_num_item_type)dsatutil_get_val(
                                      DSAT_BASIC_N_D_IDX,0,0,NUM_TYPE),
                           (dsat_num_item_type)dsatutil_get_val(
                                      DSAT_BASIC_N_S_IDX,0,0,NUM_TYPE),
                           (dsat_num_item_type)dsatutil_get_val(
                                      DSAT_BASIC_N_E_IDX,0,0,NUM_TYPE),
                           (dsat_num_item_type)dsatutil_get_val(
                                     DSAT_BASIC_DS_V_IDX,0,0,NUM_TYPE),
                           (dsat_num_item_type)dsatutil_get_val(
                                     DSAT_SREG_S0_IDX,0,0,NUM_TYPE), 
                           (dsat_num_item_type)dsatutil_get_val(
                                     DSAT_SREG_S2_IDX,0,0,NUM_TYPE), 
                           (dsat_num_item_type)dsatutil_get_val(
                                     DSAT_SREG_S3_IDX,0,0,NUM_TYPE),
                           (dsat_num_item_type)dsatutil_get_val(
                                     DSAT_SREG_S4_IDX,0,0,NUM_TYPE),
                           (dsat_num_item_type)dsatutil_get_val(
                                     DSAT_SREG_S5_IDX,0,0,NUM_TYPE), 
                           (dsat_num_item_type)dsatutil_get_val(
                                     DSAT_SREG_S6_IDX,0,0,NUM_TYPE),
                           (dsat_num_item_type)dsatutil_get_val(
                                     DSAT_SREG_S7_IDX,0,0,NUM_TYPE),
                           (dsat_num_item_type)dsatutil_get_val(
                                     DSAT_SREG_S8_IDX,0,0,NUM_TYPE),
                           (dsat_num_item_type)dsatutil_get_val(
                                     DSAT_SREG_S10_IDX,0,0,NUM_TYPE),
                           (dsat_num_item_type)dsatutil_get_val(
                                     DSAT_SREG_S30_IDX,0,0,NUM_TYPE),
                           (dsat_num_item_type)dsatutil_get_val(
                                    DSAT_SREG_S103_IDX,0,0,NUM_TYPE), 
                           (dsat_num_item_type)dsatutil_get_val(
                                    DSAT_SREG_S104_IDX,0,0,NUM_TYPE));
  }

  return result;
}/* dsatact_exec_slashs_cmd */

/*===========================================================================

FUNCTION DSATACT_EXEC_SLASH_Q_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. This function executes AT\Q command. 
  AT\Q is used to set serial port flow control.

DEPENDENCIES
  None
  
RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command
  
SIDE EFFECTS
  Modifies flow control settings of a serial port.

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatact_exec_slash_q_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  /* Values taken as per flow_ctl_type defined in dsatcmdp.c */
#define DSATQ_FCTL_OFF              0    /* No flow control */
#define DSATQ_XONXOFF_STRIP_FCTL    1    /* Xon-Xoff software flow control */
#define DSATQ_CTSRFR_FCTL           2    /* RTS/CTS Hardware flow control */ 
#define DSATQ_XONXOFF_NSTRIP_FCTL   3    /* Xon-Xoff software flow control */ 

  dsat_result_enum_type result;
#ifdef FEATURE_DATA_IS707
  dsat_num_item_type old_dsat_slash_q_val = 
                  (dsat_num_item_type)dsatutil_get_val(DSAT_BASIC_DS_Q_IDX,
                                                            0,
                                                            0,
                                                            NUM_TYPE);
#endif /* FEATURE_DATA_IS707 */

  result = dsatparm_exec_param_cmd ( mode,
                                     parse_table,
                                     tok_ptr,
                                     res_buff_ptr
                                   );
#ifdef FEATURE_DATA_IS707
  if( (result == DSAT_OK) && (IS_JCDMA_MODE() ) )
  {
    if( (dsat_num_item_type)dsatutil_get_val(
                             DSAT_BASIC_DS_Q_IDX,0,0,NUM_TYPE) != 3 ) /* Bi-directional hardware */
    {                           /* (CTS/RTS) flow control. */
      DSATUTIL_SET_VAL(DSAT_BASIC_DS_Q_IDX,0,0,0,old_dsat_slash_q_val,NUM_TYPE)
      result = DSAT_ERROR;
    }
  }
#endif /* FEATURE_DATA_IS707 */
  if ( result == DSAT_OK )
  {
    if( (dsat_num_item_type)dsatutil_get_val(
                              DSAT_BASIC_DS_Q_IDX,0,0,NUM_TYPE) == 0 ) /* Disable flow control */
    {
      DSATUTIL_SET_VAL(DSAT_EXT_IFC_IDX,0,0,0,DSATQ_FCTL_OFF,NUM_TYPE)
      DSATUTIL_SET_VAL(DSAT_EXT_IFC_IDX,0,1,0,DSATQ_FCTL_OFF,NUM_TYPE)
    }
    else if( (dsat_num_item_type)dsatutil_get_val(
                               DSAT_BASIC_DS_Q_IDX,0,0,NUM_TYPE) == 1 ) /* Bi-directional software */
    {                                /* (XON/XOFF) flow control */

      DSATUTIL_SET_VAL(DSAT_EXT_IFC_IDX,0,0,0,DSATQ_XONXOFF_NSTRIP_FCTL,NUM_TYPE)
      DSATUTIL_SET_VAL(DSAT_EXT_IFC_IDX,0,1,0,DSATQ_XONXOFF_STRIP_FCTL,NUM_TYPE)

    }
    /* dsat_slash_q_val = 2 is not taken care here as 
       CTS only Hardware flow control is not supported */
    else if( (dsat_num_item_type)dsatutil_get_val(
                               DSAT_BASIC_DS_Q_IDX,0,0,NUM_TYPE) == 3 ) /* Bi-directional hardware */
    {                               /* (CTS/RTS) flow control. */
      DSATUTIL_SET_VAL(DSAT_EXT_IFC_IDX,0,0,0,DSATQ_CTSRFR_FCTL,NUM_TYPE)
      DSATUTIL_SET_VAL(DSAT_EXT_IFC_IDX,0,1,0,DSATQ_CTSRFR_FCTL,NUM_TYPE)

    }
  }
  return result;
} /* dsatact_exec_slash_q_cmd */

/*===========================================================================

FUNCTION DSATACT_EXEC_ATD_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes ATD command.
  ATD command is used to dial a call (ASYNC, PACKET, VOICE, Direct Dial) 
  using AT commands

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR          : if there was any problem in executing the command
    DSAT_NO_RSP         : if ds3g_initiate_call() is called.
    DSAT_ASYNC_CMD      : if voice call is made.

SIDE EFFECTS
  Data dial handler or voice call handler invoked to init a call.

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatact_exec_atd_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type         result;
  dsati_dial_str_type           dial_str;
  dsati_mode_e_type             current_mode;
  dsat_dial_str_enum_type       call;
  dsat_dial_modifier_info_type  modifier_info;
  ds3g_siolib_port_e_type port_count = DS3G_SIOLIB_DATA_PORT;
  boolean dtr_status = FALSE;
  ds3g_siolib_state_info_type *state = NULL;
  uint8  flag = FALSE;
#ifdef FEATURE_DSAT_ETSI_MODE
  dsat_num_item_type csta_val ;
#endif /*FEATURE_DSAT_ETSI_MODE*/
  sys_modem_as_id_e_type subs_id = dsat_get_current_subs_id(FALSE);
  dsatetsicall_call_state_ss_info *call_ss_val = NULL;
  uint8 index = 0;
  call_ss_val = dsat_get_base_addr(DSAT_CALL_SS_VALS, FALSE);

/*------------------------------------------------------------------*/

  current_mode = dsatcmdp_get_current_mode();

  if (mode == DSAT_CMD) 
  {

    /* process the dial string tok_ptr->arg[0],
       Get the dial number's attribute in dial_str 
    */
    if (!proc_dial_str(tok_ptr, &dial_str, &current_mode))
    {
    /* illegal dial string */
      return DSAT_ERROR;
    }

    /* error for empty dial number */
    if (dial_str.len == 0)
    {
      DS_AT_MSG0_HIGH("Dial number empty.");
      return DSAT_ERROR;
    }

#ifdef FEATURE_GOBI
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
    if(!dsatutil_strcmp_ig_sp_case(dial_str.num,
                                         dsat_emergency_call_str_1) ||
       !dsatutil_strcmp_ig_sp_case(dial_str.num,
                                         dsat_emergency_call_str_2))
    {
      DS_AT_MSG0_HIGH("Gobi doesn't support Emergency Call");
      return DSAT_ERROR;
    }
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */
#endif /* FEATURE_GOBI */
    if(dsatcmdp_block_indications() == TRUE)
    { 
       while( port_count < DS3G_SIOLIB_PORT_MAX )
       { 
         (void)ds3g_siolib_ex_is_dtr_asserted(&dtr_status, port_count); 
         state = ds3g_siolib_get_ds3g_siolib_state(port_count); 
         if(state->serial_info.serial_state == DS3G_SIOLIB_AUTODETECT_SERIAL_STATE && dtr_status == TRUE) 
         {
           flag = TRUE;
           break;
         }\
         port_count++;
       }
       if(flag == FALSE)
       { /* If there is an existing DATA call
      **  -- Do not allow DATA calls in ETSI mode,
      **  -- Do not allow ANY calls in case of CDMA mode
      */
        if ( ( IS_CDMA_MODE(current_mode) ) || 
          !( dial_str.attrib & DIAL_ATTRIB_SEMICOLON ) )
        {
          DS_AT_MSG0_HIGH("Data call issued while a data call is already in session");
          return DSAT_ERROR;
        }
      }
    }

#ifdef FEATURE_DSAT_GOBI_MAINLINE
#if defined (FEATURE_CDMA_800) || defined (FEATURE_CDMA_1900)
#if defined (FEATURE_OTASP) 
  if( 0 == dsatutil_strcmp_ig_sp_case(dial_str.num,
                                       dsat_otasp_call_str))
  {
    return dsatact_exec_atd_otasp_call(&dial_str);
  }
#endif /*defined (FEATURE_OTASP)*/
#endif /*defined (FEATURE_CDMA_800) || defined (FEATURE_CDMA_1900)*/
#endif /*FEATURE_DSAT_GOBI_MAINLINE*/

#ifdef FEATURE_DSAT_ETSI_MODE
    /* do special dial checking in ETSI modes,
       1. Check restricted mode.
       2. Check if the call is a direct dial.
       3. Check if the call is a voice call.
    */
    if ( IS_ETSI_MODE(current_mode) )
    {
      /* Override address type if +CSTA set to international calls. */
      /* Skip step for packet calls and phonebook direct dial.                  */
      csta_val =(dsat_num_item_type)dsatutil_get_val(
                                             DSATETSI_EXT_CSTA_IDX,0,0,NUM_TYPE);
      if ( (DSAT_TA_INTERNATIONAL == csta_val) &&
           !(dial_str.attrib & DIAL_ATTRIB_START_PLUS) && 
           !(dial_str.attrib & DIAL_ATTRIB_GREAT) &&
           !(dial_str.attrib & DIAL_ATTRIB_PKT) )
      {
        /* Prepend number with international call prefix. */
        /* String buffer can be extended as its already been saved in
         * previous command buffer and there is at least 'ATD' before
         * dial string */
        *(--dial_str.num) = DSAT_TA_INTER_PREFIX;
        dial_str.len++;
      }
      
      if (dial_str.attrib & DIAL_ATTRIB_GREAT)
      {
        /* if DIAL_ATTRIB_GREAT is set in ETSI mode, 
           we do direct dialing process                     */

        /* Allocate command memory pointer for ATD command for direct dial */
        (void) dsat_alloc_cmd_mem_ptr(sizeof(etsime_dd_type));

#ifdef FEATURE_ETSI_PBM
        return dsatme_direct_dial(&dial_str, res_buff_ptr);
#else
        /* we don't suppurt direct dial */
        return DSAT_ERROR;
#endif /* FEATURE_ETSI_PBM */
      }
      else if (dial_str.attrib & DIAL_ATTRIB_SEMICOLON)
      {
        /* if DIAL_ATTRIB_SEMICOLON is set in ETSI mode,
           this is a voice call.                            */
        result = dsatetsicall_voice_dial_cmd(&dial_str);

#ifdef FEATURE_ETSI_PBM
        /* prepare the last dial number for saving to the last dial ph book */
        if(result == DSAT_ASYNC_CMD)
        {
          /* the number was dialed, init the ld_rec */
          dsatme_ld_init_record((char *)dial_str.num, -1, NULL);

          /* Save last dial record to phone book */
          dsatme_ld_save(subs_id);
        }
#endif /* defined(FEATURE_ETSI_PBM) */

        return result;
      }

    }
#endif /* FEATURE_DSAT_ETSI_MODE */

    /* dial through the dial table, depending on mode, CS or PKT call
     */
    if ( dial_str.attrib & DIAL_ATTRIB_PKT )
    {
      call = DSAT_PKT_DIAL_TYPE;
    }
    else
    {
      call = DSAT_DEFAULT_DIAL_TYPE;
    }

    memset(&modifier_info, 0, sizeof(dsat_dial_modifier_info_type));
#ifdef FEATURE_DSAT_ETSI_MODE
    if ( IS_ETSI_MODE(current_mode) )
    {
      /* Fill out dial string modifiers */
      if (DSAT_OK != dsatutil_fill_modifier_info(&dial_str, &modifier_info))
      {
        /* Problem with modifier setup */
        return DSAT_ERROR;
      }
    }

    /* Update +CIND callsetup indicator */
    /* Updatation  only for GSM/WCDMA Subscription */
    if ( IS_ETSI_MODE(current_mode) )
    {
      (void)dsatetsime_change_indicator_state( DSAT_CIND_INDEX_CALLSETUP,
                                               DSAT_CIND_CSETUP_MOINIT,
                                               subs_id );
    }
#endif /* FEATURE_DSAT_ETSI_MODE */

#ifdef FEATURE_MSM_FUSION
    if(call != DSAT_PKT_DIAL_TYPE)
    {
      DS_AT_MSG0_HIGH("Not a Packet Data Call");
      return DSAT_ERROR;
    }
#endif /* FEATURE_MSM_FUSION */

   DS_AT_MSG1_HIGH("current_mode is %d",current_mode);

   switch(current_mode)
   {
     case DSAT_MODE_1XDO:
       current_mode = DSAT_MODE_CDMA;
       break;
     case DSAT_MODE_1XLTE:
       if(call == DSAT_PKT_DIAL_TYPE)
       {
         current_mode = DSAT_MODE_LTE;
       }
       else
       {
         current_mode = DSAT_MODE_CDMA;
       }
       break;
     default:
       break;
   }
    DS_AT_MSG1_HIGH("current_mode updated to %d",current_mode);
    if ( dsati_dial_table[current_mode][call] == NULL )
    {
      DS_AT_MSG0_HIGH("unregistered call handler");
      result = DSAT_ERROR;
    }
    else
    {
#ifdef FEATURE_DSAT_ETSI_DATA
      if ( (IS_ETSI_MODE(current_mode)) && ( call == DSAT_PKT_DIAL_TYPE ) )
      {
        byte temp_cid = 1; /* default profile id*/

        /* Update CID if provided  */
        if ( ( modifier_info.modifier_present ) &&
             ( modifier_info.modifier_info.cid.cid_val > 0) )
        {
          temp_cid  =  modifier_info.modifier_info.cid.cid_val;
          if ( !DSAT_IS_PROFILE_ID_WITHIN_RANGE(temp_cid) )
          {
            DS_AT_MSG1_ERROR("Invalid CID %d",temp_cid);
            return DSAT_ERROR;
          }
        }
      }
#endif /* FEATURE_DSAT_ETSI_DATA */
    /* if there is a mode mismatch between current ATCoP 
       operating mode and current network mode,return DSAT_ERROR 
    */
#ifndef FEATURE_DUAL_SIM
    if ( ( call == DSAT_PKT_DIAL_TYPE) &&
         ( ( (IS_CDMA_MODE(current_mode)) && 
           ! dsatcmdp_is_pref_pkt_mode(SYS_SYS_MODE_CDMA,subs_id) )||
         ( (IS_ETSI_MODE(current_mode)) && 
             dsatcmdp_is_pref_pkt_mode(SYS_SYS_MODE_CDMA,subs_id) ) ) )
    {
      DS_AT_MSG1_ERROR("Mode mismatch,ATCoP mode =%d",(int)current_mode);
      return DSAT_ERROR;
    }
#endif /* FEATURE_DUAL_SIM */
      dsat_curr_abort_handler =
        dsati_dial_table[current_mode][call](dial_str.num,
                                     (dial_str.attrib & DIAL_ATTRIB_ASCII),
                                             &modifier_info,
                                             subs_id);
#ifdef FEATURE_ETSI_PBM
      if ( dsat_curr_abort_handler != NULL )
      {
        dsatme_ld_init_record((char *) dial_str.num, -1, NULL);

        /* Save last dial record to phone book */
        dsatme_ld_save(subs_id);
      }
#endif /* defined(FEATURE_ETSI_PBM) */

      result = DSAT_NO_RSP;
/* Copy the curr_abort_handler for using it when S7 timer expires.
   This is required for PDP-IP calls where CONNECT is sent to TE before 
   CALL_EVENT_CONNECT from CM is received and we clear the curr_abort_handler.*/
      for(index = 0; index < DSAT_MAX_DATA_CALLS; index++)
      {
        if(NULL == call_ss_val->dsatetsicall_s7_info[index].data_s7_abort_handler)
        {
          call_ss_val->dsatetsicall_s7_info[index].data_s7_abort_handler = dsat_curr_abort_handler;
          if(modifier_info.modifier_present)
          {
            call_ss_val->dsatetsicall_s7_info[index].is_profile_id_valid = TRUE;
          }
        }
      }
    }
  }
  else
  {
    result = DSAT_ERROR;
  }

  return result;
} /* dsatact_exec_atd_cmd */


/*===========================================================================

FUNCTION DSATACT_EXEC_ATA_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes ATA command.
  ATA command is used to answer a incoming call using AT commands

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_NO_RSP : if ds3g_answer_call() is called.

SIDE EFFECTS
  Calls ds3g_answer_call() function to answer a call

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatact_exec_ata_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result;
#ifdef FEATURE_DATA_TE_MT_PDP
  dsat_pdp_answer_data_type pdp_answer_data;
#endif /* FEATURE_DATA_TE_MT_PDP */
  sys_modem_as_id_e_type subs_id = dsat_get_current_subs_id(FALSE);

  if ( mode == DSAT_CMD )
  {
     /* ATA preference is in the following order:
        1. Voice call
        2. CS data call
        3. PS data call */
    if( dsatvoice_is_voice_call( subs_id, DSAT_CALLID_ALL ) )
    {
      result = dsatvoice_answer_cmd(subs_id);
    }
    else
    {
      if ( dial_string_handler.answer_cb == NULL
#ifdef FEATURE_DATA_TE_MT_PDP
           && (dial_string_handler.pdp_answer_cb == NULL)
#endif /* FEATURE_DATA_TE_MT_PDP */
    )
      {
        result = DSAT_NO_CARRIER;
        DS_AT_MSG0_HIGH("unregistered incoming_cb handler");
      }
      else 
      {
        /* either answer_cb != NULL or pdp_answer_cb != NULL */
        result = DSAT_NO_RSP;
        
#ifdef FEATURE_DATA_TE_MT_PDP
        /* This check should be done before slamming the modem port */
        if ( ((dsat_num_item_type)dsatutil_get_val(DSATETSI_EXT_CGAUTO_IDX,
            0,0,NUM_TYPE) == DSAT_CGAUTO_AUTO_ANS_OFF) &&
             (dial_string_handler.pdp_answer_cb != NULL) )
        {
          DS_AT_MSG0_HIGH("ATA cmd not allowed when +CGAUTO=0");
          return DSAT_NO_CARRIER;
        }
#endif /* FEATURE_DATA_TE_MT_PDP */

        /* Slam the modem port to the current active port */
        ds3g_siolib_set_answer_port ( DS3G_SIOLIB_ANSWER_MODE_MANUAL );

        if( dial_string_handler.answer_cb != NULL)
        {
          dsat_curr_abort_handler = dial_string_handler.answer_cb();
        }
#ifdef FEATURE_DATA_TE_MT_PDP
        else /* (dial_string_handler.pdp_answer_cb != NULL) */
        {
          memset ((void*)&pdp_answer_data, 0, sizeof(pdp_answer_data));
          dsat_curr_abort_handler = 
            dial_string_handler.pdp_answer_cb(&pdp_answer_data );
        }
#endif /* FEATURE_DATA_TE_MT_PDP */
        if ( dsat_curr_abort_handler == NULL )
        {
          DS_AT_MSG0_HIGH("unregistered ATA Abort handler");
        }
      } /* Answer callbacks are not NULL */
    } /* Not voice call */
  }
  else
  {
    result = DSAT_ERROR;
  }

  return result;

}/* dsatact_exec_ata_cmd () */

/*===========================================================================

FUNCTION DSATACT_EXEC_ATH_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes ATH command.
  ATH command is used to hangup both data and voice calls
  using AT commands.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_ATH_CMD : if hangup pointer is invoked.
    DSAT_OK : if CDMA-only and no hangup CB registered

SIDE EFFECTS
  Calls ds3g_hangup_call() function to terminate a call

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatact_exec_ath_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result  = DSAT_OK;
  sys_modem_as_id_e_type subs_id = dsat_get_current_subs_id(FALSE);
  dsatetsicall_call_state_da_info  *call_da_val = NULL;

  call_da_val = dsat_get_base_addr_per_subs(DSAT_CALL_DA_VALS, subs_id, FALSE);
  /*---------------------------------------------  
    Processing for WRITE syntax
  ----------------------------------------------*/
  if ((tok_ptr->op == NA) || (tok_ptr->op == (NA|EQ|AR)))
  {
    if ((tok_ptr->op == (NA|EQ|AR)) && ( *tok_ptr->arg[0] != '0'))
    {
      /* check to see if the argument is valid */
      /* for ATH only 0 is defined */
      /* any other value would cause an error */
      DS_AT_MSG_SPRINTF_1_LOW( 
                     "ATH parameter not supported: %s",*tok_ptr->arg);
      return DSAT_ERROR;
    }
     /* ATH preference is in the following order:
        1. CS data call
        2. PS data call
        3. Voice call (if +CVHU=0) */
    if ( dial_string_handler.hangup_cb != NULL )
    {
      dial_string_handler.hangup_cb ();
      result = DSAT_ATH_CMD;
    }
#ifdef FEATURE_DATA_TE_MT_PDP
    else if( dial_string_handler.pdp_hangup_cb != NULL)
    {
      if ((dsat_num_item_type)dsatutil_get_val(DSATETSI_EXT_CGAUTO_IDX,
                                    0,0,NUM_TYPE) == DSAT_CGAUTO_AUTO_ANS_OFF)
      {
        DS_AT_MSG0_HIGH("ATH cmd not allowed when +CGAUTO=0");
        result = DSAT_OK;
      }
      else
      {
        dial_string_handler.pdp_hangup_cb();
        result = DSAT_ATH_CMD;
      }
    }
#endif /* FEATURE_DATA_TE_MT_PDP */
    else
    {
#ifdef FEATURE_DSAT_ETSI_MODE
      dsati_mode_e_type current_mode = dsatcmdp_get_current_mode();
      if ( IS_ETSI_MODE(current_mode) )
      {
        uint8 num_calls = 0;
        cm_call_id_type call_ids[CM_CALL_ID_MAX];
        uint8 index;
        cm_call_mode_info_e_type call_mode = CM_CALL_MODE_INFO_GW_CS;

#ifdef FEATURE_DUAL_ACTIVE
        if (FALSE == dsatvoice_validate_call_cmd(subs_id, DSAT_CALL_CMD_END))
        {
          return dsat_send_cme_error(DSAT_CME_OP_NOT_ALLOWED);
        }
#endif /* FEATURE_DUAL_ACTIVE */

        memset ((void*)call_ids, CM_CALL_ID_INVALID, sizeof(call_ids));
          /* Waiting calls need to end(Only if +CVHU is 0) using call_cmd_answer 
             with reply as false */
        if( (call_da_val->etsicall_wait_call_present) && (((dsat_num_item_type)
                 dsatutil_get_val(DSATETSI_EXT_CVHU_IDX,0,0,NUM_TYPE)) == 0 ) )
        {
          etsicall_cmd_pending[subs_id] = CMD_ATH;
          for ( index = 0; 
                index < CM_CALL_ID_MAX; 
                index++ )
          {
            if(DSAT_VOICE_STATE_WAITING == voice_state[subs_id][index].state)
            {
              call_mode = voice_state[subs_id][index].call_mode;
              break;
            }
          }
          if( dsatcmif_answer_voice_call(subs_id,call_da_val->etsicall_wait_call_id,FALSE,call_mode) )
          {
            result = DSAT_ASYNC_CMD;
          }
          else
          {
            etsicall_cmd_pending[subs_id] = CMD_NONE;
            DS_AT_MSG0_ERROR("End call request failed");
            result = DSAT_ERROR;
            return result;
          }
        }
        /* search for calls that should be ended... */
        for ( index = 0; 
              index < CM_CALL_ID_MAX; 
              index++ )
        {
          /* in ETSI builds active and held call lists are maintained for +CHLD
             command but these types must be searched for */
          if ( voice_state[subs_id][index].state == DSAT_VOICE_STATE_CONNECTING )
          {
            call_ids[num_calls++] = index;
          }
        }

        if ( (dsat_num_item_type)dsatutil_get_val(DSATETSI_EXT_CVHU_IDX,0,0,NUM_TYPE) == 0 )
        {
          /* Terminate any (voice) call in progress per v.25 ter */
          if ( num_calls > 0 ||
               call_da_val->etsicall_num_active_calls != 0 ||
               call_da_val->etsicall_num_held_calls != 0 ) 
          {
            /* Fill call_ids[] with all active & held voice calls that 
               should be ended by ATH */
            if ( call_da_val->etsicall_num_active_calls )
            {
              for ( index = 0; (index < call_da_val->etsicall_num_active_calls && 
                               num_calls < CM_CALL_ID_MAX); index++ )
              {
                call_ids[num_calls++] = call_da_val->etsicall_active_call_ids[index];
              }
            }

            if ( call_da_val->etsicall_num_held_calls )
            {
              for ( index = 0; (index < call_da_val->etsicall_num_held_calls &&
                               num_calls < CM_CALL_ID_MAX); index++ )
              {
                call_ids[num_calls++] = call_da_val->etsicall_held_call_ids[index];
              }
            }

            etsicall_cmd_pending[subs_id] = CMD_ATH;

            /* Make CM call to release them all */
            if ( dsatcmif_end_calls(num_calls, &call_ids[0]) )
            {
              /* the command callback function will be called */
              result = DSAT_ASYNC_CMD;
            }
            else
            {
              etsicall_cmd_pending[subs_id] = CMD_NONE;
              DS_AT_MSG0_ERROR("End call request failed");
              result = DSAT_ERROR;
             }
          }
          else
          {
            /* ATH is intended to hangup voice call.  If there is no
               active call return ok. */
            result = DSAT_OK;
          }

        }
        else
        {
          result = DSAT_OK;
          DS_AT_MSG0_HIGH("ATH cmd not in online_cmd_mode and +CVHU != 1");
        }
      }
      else
#endif /* FEATURE_DSAT_ETSI_MODE */
      {
        /* CDMA-only builds should not release voice call(s) on ATH */
        result = DSAT_OK;
      }
    }
  }
  else 
  {
    result = DSAT_ERROR;
  }

  return result;

}/* dsatact_exec_ath_cmd () */



/*===========================================================================

FUNCTION DSATACT_EXEC_ATO_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes O command.
  This command causes the DCE to return to online data state.
  
DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_NO_RSP : if ds3g_answer_call() is called.

SIDE EFFECTS
  None  

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatact_exec_ato_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result;

  /*---------------------------------------------  
    Processing for WRITE syntax
  ----------------------------------------------*/
  if ((tok_ptr->op == NA) || (tok_ptr->op == (NA|EQ|AR)))
  {
    if ( tok_ptr->op == (NA|EQ|AR) )
    {
      /* check to see if the argument is valid  */
      if( DSAT_OK != dsatparm_exec_param_cmd(mode,
                                                 parse_table,
                                                 tok_ptr,
                                                 res_buff_ptr) )
      {
        DS_AT_MSG_SPRINTF_1_LOW( 
          "ATO parameter not supported: %s",*tok_ptr->arg);
        return DSAT_ERROR;
      }
      if ( (dsat_num_item_type)dsatutil_get_val(
                               DSAT_BASIC_ACT_O_IDX,0,0,NUM_TYPE) > 0 )
      {
#ifdef FEATURE_DATA_IS707
        if ( IS_JCDMA_MODE() )
        {
          /* for ATO values till 2 is defined for JCDMA mode */
          return DSAT_OK;
        }
        else
#endif /* FEATURE_DATA_IS707 */
        {
          /* for ATO only 0 is defined at this time */
          /* any other value would cause an error   */
          DS_AT_MSG_SPRINTF_1_LOW(
            "ATO parameter not supported: %s",*tok_ptr->arg);
          return DSAT_ERROR;
        }
      }/* dsat_o_val > 0 . */
    }
    /* Check mode for correct state */
    if ( DSAT_ONLINE_CMD == mode )
    {
      /* Currently in ONLINE-COMMAND mode */
      enter_online_data_mode(TRUE);
      result = DSAT_CONNECT;
    }
    else
    {
      /* Currently in COMMAND mode */
      result = DSAT_NO_CARRIER;
    }
  }
  else
  {
    result = DSAT_ERROR;
  }

  return result;

} /* dsatact_exec_ato_cmd () */



/*===========================================================================

FUNCTION DSATACT_EXEC_GMI_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +GMI/+CGMI command.
  This command returns manufacturer information.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatact_exec_gmi_cmd
(
  dsat_mode_enum_type  mode,              /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr             /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  static char info[32] = "QUALCOMM INCORPORATED";


  /*-------------------------------------------------------
  General commands with no arguments should return the 
  information requested in the form of a string
  ---------------------------------------------------------*/
  if (tok_ptr->op == NA)
  {
#ifdef FEATURE_DSAT_EXTENDED_CMD
    /*-----------------------------------------------------------------------
    Read the stored MFR. info  from the NV.
    -----------------------------------------------------------------------*/
    if (NV_DONE_S == dsatutil_get_nv_item(NV_UNDP_HSU_MFGSTR_I, &ds_nv_item))
    {
      res_buff_ptr->used =(word) strlcpy((char*)res_buff_ptr->data_ptr,
                                       (const char*)ds_nv_item.undp_hsu_mfgstr,
                                       res_buff_ptr->size);
    }
    else
#endif /* FEATURE_DSAT_EXTENDED_CMD */
    {
      res_buff_ptr->used =(word) strlcpy((char*)res_buff_ptr->data_ptr,
                                             (const char*)info,
                                             res_buff_ptr->size);
    }
    result = DSAT_OK;
    if( !(strncmp((const char *)parse_table->name,
        (const char *)"+CGMI" ,strlen((const char *)"+CGMI"))))
    {
      result = DSAT_NO_RESULT_CODE;
    }
  }

  /*-------------------------------------------------------
  General commands with a query should produce no response
  ---------------------------------------------------------*/
  else if (tok_ptr->op == (NA|EQ|QU))
  {
    /* this command is valid, but do nothing */
    result = DSAT_OK;
  }

  /*-------------------------------------------------------
  Any argument with the command should result in an error
  ---------------------------------------------------------*/
  else
  {
    result = DSAT_ERROR;
  }

  return result;
}  /* dsatact_exec_gmi_cmd () */


/*===========================================================================

FUNCTION DSATACT_EXEC_GMM_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +GMM/+CGMM command.
  This command returns model information.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatact_exec_gmm_cmd
(
 dsat_mode_enum_type mode,               /*  AT command mode:            */
 const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table   */
 const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
 dsm_item_type *res_buff_ptr             /*  Place to put response       */
 )
{
  dsat_result_enum_type result = DSAT_OK;
#ifdef FEATURE_DSAT_EXTENDED_CMD
  nv_stat_enum_type nv_status;
#endif /* FEATURE_DSAT_EXTENDED_CMD */
  byte info[11];

  /*-------------------------------------------------------
  General commands with no arguments should return the 
  information requested in the form of a string
  ---------------------------------------------------------*/
  if (tok_ptr->op == NA)
  {
    /* Query NV for model identification */
#ifdef FEATURE_DSAT_EXTENDED_CMD
    nv_status = dsatutil_get_nv_item(NV_UNDP_HSU_PRODSTR_I, &ds_nv_item);
    if( ( nv_status == NV_DONE_S ) &&
         ( 0 != strlen((char*)ds_nv_item.undp_hsu_prodstr)))
    {
      (void) strlcpy((char*)res_buff_ptr->data_ptr,
                         (const char*)ds_nv_item.undp_hsu_prodstr,
                         res_buff_ptr->size);
      res_buff_ptr->used += (word)strlen((char*)res_buff_ptr->data_ptr);
      result = DSAT_OK;
    }
    else
#endif /* FEATURE_DSAT_EXTENDED_CMD */
    {
#ifdef CUST_MOB_MODEL_EXT
      (void) dsatutil_itoa(mob_model_ext, info, 10);
#else
      (void) dsatutil_itoa((uint32)mob_model, info, 10);
#endif /* CUST_MOB_MODEL_EXT */

      (void) strlcpy((char*)res_buff_ptr->data_ptr,
                     (const char*)info,
                     res_buff_ptr->size);
      res_buff_ptr->used += (word)strlen((char*)res_buff_ptr->data_ptr);
      result = DSAT_OK;
    }/* NV_UNDP_HSU_PRODSTR_I = !NV_DONE_S */
    if( !(strncmp((const char *)parse_table->name,
        (const char *)"+CGMM" ,strlen((const char *)"+CGMM"))))
    {
      result = DSAT_NO_RESULT_CODE;
    }

  }/* NAME */
 
  
  /*-------------------------------------------------------
  General commands with a query should produce no response
  ---------------------------------------------------------*/
  else if (tok_ptr->op == (NA|EQ|QU))
  {
    /* this command is valid, but do nothing */
    result = DSAT_OK;
  }

  /*-------------------------------------------------------
  Any argument with the command should result in an error
  ---------------------------------------------------------*/
  else
  {
    result = DSAT_ERROR;
  }
  return result;
}  /* dsatact_exec_gmm_cmd () */


/*===========================================================================

FUNCTION DSATACT_EXEC_GMR_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +CGMR/+GMR command.
  This command returns revision information.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatact_exec_gmr_cmd
(
  dsat_mode_enum_type  mode,              /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr             /*  Place to put response       */
)
{
/* boot_block_version isn't defined for NAND-based (6k and later) targets.
 * We need to send the default NOR boot block version in our response, for
 * backward compatibility. */
#define DSAT_BOOT_BLOCK_VERSION 0x1
  dsat_result_enum_type result = DSAT_OK;

  /*-------------------------------------------------------
  General commands with no arguments should return the 
  information requested in the form of a string
  ---------------------------------------------------------*/
  if (tok_ptr->op == NA)
  {
#ifdef FEATURE_DSAT_EXTENDED_CMD
    if( !strncmp((const char *)parse_table->name,
        (const char *)"+CGMR" ,strlen((const char *)"+CGMR")))
    {
      res_buff_ptr->used =
        (word)snprintf ((char*)res_buff_ptr->data_ptr,res_buff_ptr->size,
                             "%s: %s", parse_table->name, mob_sw_rev);
    }
    else
#endif/* FEATURE_DSAT_EXTENDED_CMD */
    {
      res_buff_ptr->used =
        (word)snprintf ((char*)res_buff_ptr->data_ptr,res_buff_ptr->size,
                              "%s  %d  [%s %s]", mob_sw_rev,
                              DSAT_BOOT_BLOCK_VERSION, rel_date, rel_time);
    }
    if( !(strncmp((const char *)parse_table->name,
        (const char *)"+CGMR" ,strlen((const char *)"+CGMR"))))
    {
      result = DSAT_NO_RESULT_CODE;
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
} /* dsatact_exec_gmr_cmd() */



/*===========================================================================

FUNCTION DSATACT_EXEC_GSN_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +GSN/+CGSN command.
  This command returns serial number information.  In ETSI mode,
  the IMEI value is reported.  Otherwise, the ESN is reported.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatact_exec_gsn_cmd
(
  dsat_mode_enum_type  mode,              /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr             /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  nv_stat_enum_type nv_status;
  static byte info[32];

#ifdef FEATURE_DATA_IS707
  /* This command is not valid for JCDMA */
  NOT_SUPPORTED_IN_JCDMA()
#endif /* FEATURE_DATA_IS707 */

  memset(info, 0, 32);
  /*-------------------------------------------------------
  General commands with no arguments should return the 
  information requested in the form of a string
  ---------------------------------------------------------*/
  if (tok_ptr->op == NA)
  {
#ifdef FEATURE_DSAT_ETSI_MODE
    /*-------------------------------------------------------
    Determine the response based on the operating mode.
    Report IMEI for ETSI mode, ESN otherwise
    ---------------------------------------------------------*/
    if (ETSI_CMD_MODE == dsatcmdp_get_operating_cmd_mode())
    {
      if (DSAT_OK == dsat_get_imei (info,sizeof(info)))
      {
        (void) strlcpy((char*)res_buff_ptr->data_ptr,
                           (const char*)info,res_buff_ptr->size);
        res_buff_ptr->used += (word)strlen((const char*)info);
        result = DSAT_OK;
      }
      else
      {
        dsatme_set_cme_error(DSAT_CME_MEMORY_FAILURE, res_buff_ptr);
        result = DSAT_CMD_ERR_RSP;
      }
    }
    else
#endif /* FEATURE_DSAT_ETSI_MODE */
    {
      /* ESN Reporting */
      nv_status = dsatutil_get_nv_item (NV_ESN_I, &ds_nv_item);
      if( nv_status == NV_DONE_S )
      {
        /* Convert the 32 bit unsigned number to hex */
        (void)dsatutil_itoa(ds_nv_item.esn.esn, info, 16);
        res_buff_ptr->used = 
                   (word)snprintf((char*)res_buff_ptr->data_ptr,
                                         res_buff_ptr->size,"0x%s", info);
        result = DSAT_OK;
      }
      else
      {
        result = DSAT_ERROR;
      }
    }
    if( !(strncmp((const char *)parse_table->name,
        (const char *)"+CGSN" ,strlen((const char *)"+CGSN"))))
    {
      result = DSAT_NO_RESULT_CODE;
    }
  }

  /*-------------------------------------------------------
  General commands with a query should produce no response
  ---------------------------------------------------------*/
  else if (tok_ptr->op == (NA|EQ|QU))
  {
    /* this command is valid, but do nothing */
    result = DSAT_OK;
  }

  /*-------------------------------------------------------
  Any argument with the command should result in an error
  ---------------------------------------------------------*/
  else
  {
    result = DSAT_ERROR;
  }
  return result;
} /* dsatact_exec_gsn_cmd() */



/*===========================================================================

FUNCTION DSATACT_EXEC_GCAP_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +GCAP command.
  This command returns complete capabilities list information.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatact_exec_gcap_cmd
(
  dsat_mode_enum_type  mode,              /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr             /*  Place to put response       */
)
{
  #define SIZE_OF_INFO 65   /* Size of info array */
  dsat_result_enum_type result = DSAT_OK;
  static char info[SIZE_OF_INFO]; 
  word cnt = 0;
  
#if defined(FEATURE_HDR) && defined(FEATURE_DSAT_EXTENDED_CMD)
  const char cap_list_2000 [] = "+CIS707-A, CIS-856, CIS-856-A, CIS707,+MS, +ES, +DS, +FCLASS";
#elif defined(FEATURE_HDR)
  const char cap_list_2000 [] = "+CIS707-A, CIS-856, +MS, +ES, +DS, +FCLASS";
#else
const char cap_list_2000 [] = "+CIS707-A, CIS707, +MS, +ES, +DS, +FCLASS";
#endif
  /*-------------------------------------------------------
  General commands with no arguments should return the 
  information requested in the form of a string
  ---------------------------------------------------------*/
  if (tok_ptr->op == NA)
  {
    dsati_mode_e_type curr_mode = dsatcmdp_get_current_mode();
    dsati_operating_cmd_mode_type oper_cmd_mode = 
                            dsatcmdp_get_operating_cmd_mode();

    /*--------------------------------------
    Setup response based on operating mode
    ----------------------------------------*/
    /* Initialize response */
 
    (void) strlcpy((char*)info, (char*)"+GCAP: ", SIZE_OF_INFO);

    if (ETSI_CMD_MODE == oper_cmd_mode)
    {

      (void)snprintf (info, SIZE_OF_INFO,"%s%s%s", info, 
                            (!cnt++) ? "" : ",", "+CGSM");
    }
#ifndef FEATURE_MDM_FUSION
    /* For Fusion Targets cap_list_2000 should be present by default  */
    else if (CDMA_CMD_MODE == oper_cmd_mode)
#endif/* FEATURE_MDM_FUSION */
    {
      (void) strlcat(info, cap_list_2000, SIZE_OF_INFO); 
    }

#ifdef FEATURE_DATA_GCSD_FAX
    if ( DSAT_MODE_GSM == curr_mode )
    {
      (void)snprintf (info, SIZE_OF_INFO, "%s%s%s", info, 
                            (!cnt++) ? "" : "," ,"+FCLASS");
    }
#endif /* FEATURE_DATA_GCSD_FAX */

    if ( DSAT_MODE_GSM == curr_mode ||
         DSAT_MODE_WCDMA == curr_mode )
    {
      (void)snprintf (info, SIZE_OF_INFO, "%s%s%s", info, 
                            (!cnt++) ? "" : ",",
                            "+DS");
    }

#ifdef FEATURE_WCDMA
    if ( DSAT_MODE_WCDMA == curr_mode )
    {
      (void)snprintf (info, SIZE_OF_INFO, "%s%s%s", info, 
                            (!cnt++) ? "" : ",","+ES");
    }
#endif /* FEATURE_WCDMA */

    (void) strlcpy((char*)res_buff_ptr->data_ptr, (const char*)info,
                       res_buff_ptr->size);
    res_buff_ptr->used += (word)strlen((const char*)info);
    result = DSAT_OK;
  }

  /*-------------------------------------------------------
  General commands with a query should produce no response
  ---------------------------------------------------------*/
  else if (tok_ptr->op == (NA|EQ|QU))
  {
    /* this command is valid, but do nothing */
    result = DSAT_OK;
  }

  /*-------------------------------------------------------
  Any argument with the command should result in an error
  ---------------------------------------------------------*/
  else
  {
    result = DSAT_ERROR;
  }

  return result;
} /* dsatact_exec_gcap_cmd() */


#ifdef FEATURE_DSAT_ETSI_MODE
/* Exclude CDMA (for now, may be required later) */

/*===========================================================================

FUNCTION DSATACT_EXEC_WS46_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +WS46 command.
  This command manages wireless data service (WDS) side-stack selection.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.
    DSAT_ASYNC_CMD : if this function invokes an async command

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatact_exec_ws46_cmd
(
  dsat_mode_enum_type  mode,              /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr             /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  dsatcmif_servs_state_ms_info  *ph_ss_ms_val = NULL;
  sys_modem_as_id_e_type subs_id = dsat_get_current_subs_id(FALSE);

  ph_ss_ms_val = dsat_get_base_addr_per_subs(DSAT_SS_PH_MS_VALS, subs_id, FALSE);
  
  /*-------------------------------------------------------
    WRITE & TEST command syntax
  ---------------------------------------------------------*/
  if( (tok_ptr->op == (NA|EQ|AR)) ||
      (tok_ptr->op == (NA|EQ|QU)) )      
  {
    /* For write syntax, validate parameter but do nothing else. RAT
     * preference is currently set via +COPS command. */
    result = dsatparm_exec_param_cmd( mode,
                                      parse_table,
                                      tok_ptr,
                                      res_buff_ptr );
  }
  /*-------------------------------------------------------
    READ command syntax
  ---------------------------------------------------------*/
  else if (tok_ptr->op == (NA|QU))
  {
    /* If functionality unknown, query CM */
    if ( DSAT_WS46_FUNC_MAX == ph_ss_ms_val->dsat_ws46_curr_val )
    {
       ph_ss_ms_val->dsat_ws46_res_type = DSAT_WS46_READ;
      /* The return value is ignored as both success and failure cases
       * are handled by the call back function */
#ifdef FEATURE_DUAL_SIM
      (void)cm_ph_cmd_get_ph_info_per_subs (dsatcmif_ph_cmd_cb_func,
                                            NULL,
                                            dsatcm_client_id,
                                            subs_id);
#else
      (void)cm_ph_cmd_get_ph_info (dsatcmif_ph_cmd_cb_func,
                                   NULL,
                                   dsatcm_client_id
                                   );
#endif /* FEATURE_DUAL_SIM */
      result = DSAT_ASYNC_CMD;
    }
    else
    {
      /* Report the +WS46 side stack value based on current RAT mode
       * preference. */
      result = dsatact_process_ws46_cmd( &ph_ss_ms_val->dsat_net_reg_state.cmph_pref );
    }
  }
  else
  {
    result = DSAT_ERROR;
  }

  return result;
} /* dsatact_exec_ws46_cmd() */


/*===========================================================================

FUNCTION DSATACT_PROCESS_WS46_CMD

DESCRIPTION
  This function process the response for the +WS46 command on return from
  asynchronous processing / synchronous function call. 
 
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
dsat_result_enum_type dsatact_process_ws46_cmd
(
  ds_at_cm_ph_pref_type  *pref_mode    /* Network preference mode */
)
{
  #define SYS_MODE_GERAN  0
  #define SYS_MODE_UTRAN  1
  #define SYS_MODE_3GPP   2
#ifdef FEATURE_DSAT_LTE
  #define SYS_MODE_EUTRAN          3
  #define SYS_MODE_GERAN_UTRAN     4
/* G+L or W+L combination is not supported by lower layers */
  #define SYS_MODE_GERAN_EUTRAN    5
  #define SYS_MODE_UTRAN_EUTRAN    6
#endif /* defined (FEATURE_DSAT_LTE) */
  dsm_item_type * res_buff_ptr;
  const dsat_string_item_type (*list_ptr)[8] = dsat_ws46_list[0].list_v;
  dsatcmif_servs_state_ms_info  *ph_ss_ms_val = NULL;
  sys_modem_as_id_e_type  subs_id = dsat_get_current_subs_id(FALSE);

  ph_ss_ms_val = dsat_get_base_addr_per_subs(DSAT_SS_PH_MS_VALS, subs_id, FALSE);

  /* Process the READ command */
  if ( ph_ss_ms_val->dsat_ws46_res_type == DSAT_WS46_READ )
  {
    ph_ss_ms_val->dsat_ws46_res_type = DSAT_WS46_NONE;
    ph_ss_ms_val->dsat_net_reg_state.cmph_pref.network_rat_mode_pref = 
                                               pref_mode->network_rat_mode_pref;
  }

  res_buff_ptr = dsat_dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL, FALSE);
  /* Only the functionality value is output */
  /* Report the +WS46 side stack value based on current RAT mode
   * preference. dsat_ws46_curr_val is assigned the index of the 
   * actual network mode preference */
  switch( pref_mode->network_rat_mode_pref)
  {
  case CM_MODE_PREF_GSM_ONLY:
    ph_ss_ms_val->dsat_ws46_curr_val = SYS_MODE_GERAN;
    break;
  case CM_MODE_PREF_WCDMA_ONLY:
    ph_ss_ms_val->dsat_ws46_curr_val = SYS_MODE_UTRAN;
    break;
#ifdef FEATURE_DSAT_LTE

  case CM_MODE_PREF_GWL:
    ph_ss_ms_val->dsat_ws46_curr_val = SYS_MODE_3GPP;
  break;
  case CM_MODE_PREF_LTE_ONLY:
    ph_ss_ms_val->dsat_ws46_curr_val = SYS_MODE_EUTRAN;
  break;
  case CM_MODE_PREF_GSM_WCDMA_ONLY:
    ph_ss_ms_val->dsat_ws46_curr_val = SYS_MODE_GERAN_UTRAN;
  break;
/* G+L or W+L combination is not supported by lower layers */
#endif /* defined (FEATURE_DSAT_LTE) */
  /* The cases CM_MODE_PREF_AUTOMATIC or CM_MODE_PREF_GSM_WCDMA_ONLY are taken 
   * care by the default case */
  default:
    ph_ss_ms_val->dsat_ws46_curr_val = SYS_MODE_3GPP;
    break;
  }

  res_buff_ptr->used = (word)snprintf ( (char*)res_buff_ptr->data_ptr,
                                              res_buff_ptr->size,
                                              "%s",
                                              list_ptr[ph_ss_ms_val->dsat_ws46_curr_val]);
  dsatcmdp_handle_async_cmd_rsp(res_buff_ptr, DSAT_COMPLETE_RSP);
  return DSAT_OK;
} /* dsatact_process_ws46_cmd */

#endif /* FEATURE_DSAT_ETSI_MODE */

/*===========================================================================

FUNCTION DSATACT_EXEC_ATI_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes I command.
  This command returns identification information.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatact_exec_ati_cmd  
(
  dsat_mode_enum_type  mode,              /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr             /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  dsat_num_item_type ati_val;
  mmgsdi_slot_id_enum_type slot_id = dsat_get_current_slot_id();

  /*-------------------------------------------------------
  General commands with no arguments should return the 
  information requested in the form of a string.
  Arguments are accepted but ignored (to accomodate
  external software).
  ---------------------------------------------------------*/
  if ( tok_ptr->op == (NA|EQ|AR) )
  {
    if ( dsatutil_atoi(&ati_val, tok_ptr->arg[0], 10) )
    {
      /* error in converting the number */
      return DSAT_ERROR;
    }

    /* Even if it is an optional parameter check the range */
    if ( ati_val > 255 )
    {
      return DSAT_ERROR;
    }
  }

  if ( (tok_ptr->op == NA) ||
       (tok_ptr->op == (NA|EQ|AR)) )
  {
    dsm_item_type * temp_buff_ptr;
    unsigned int *attrib_ptr = (unsigned int *)&tok_ptr->op;
#ifndef FEATURE_DSAT_EXTENDED_CMD
    nv_stat_enum_type nv_status;
#endif /* FEATURE_DSAT_EXTENDED_CMD */
    *attrib_ptr = NA;
    
    /*-------------------------------------------------------
      Allocate a temporary buffer to hold composite response
    ---------------------------------------------------------*/
    temp_buff_ptr = dsat_dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL, FALSE);
    temp_buff_ptr->data_ptr[temp_buff_ptr->used] = '\0';

    /* Manufacturer information */
    if (DSAT_OK == dsatact_exec_gmi_cmd (mode, parse_table, 
                                         tok_ptr, res_buff_ptr))
    {
#ifdef FEATURE_DSAT_EXTENDED_CMD
      temp_buff_ptr->used = (word)snprintf((char*)temp_buff_ptr->data_ptr,
                                          temp_buff_ptr->size,
                                          "%s: %s: %s",
                                          "Manufacturer",
                                          "+GMI",
                                          res_buff_ptr->data_ptr);
#else
      temp_buff_ptr->used = (word)snprintf((char*)temp_buff_ptr->data_ptr,
                                          temp_buff_ptr->size,
                                          "%s: %s",
                                          "Manufacturer",
                                          res_buff_ptr->data_ptr);
#endif/* FEATURE_DSAT_EXTENDED_CMD */
    }

    /* Model information */
    if (DSAT_OK == dsatact_exec_gmm_cmd (mode, parse_table, 
                                         tok_ptr, res_buff_ptr))
    {
      temp_buff_ptr->used = (word)snprintf((char*)temp_buff_ptr->data_ptr,
                                          temp_buff_ptr->size,
                                          "%s\n%s: %s",
                                          temp_buff_ptr->data_ptr,
                                          "Model",
                                          res_buff_ptr->data_ptr);
    }

    /* Revision information */
    if (DSAT_OK == dsatact_exec_gmr_cmd (mode, parse_table, 
                                         tok_ptr, res_buff_ptr))
    {
      temp_buff_ptr->used = (word)snprintf((char*)temp_buff_ptr->data_ptr,
                                          temp_buff_ptr->size,
                                          "%s\n%s: %s",
                                          temp_buff_ptr->data_ptr,
                                          "Revision",
                                          res_buff_ptr->data_ptr);
    }
#ifdef FEATURE_DSAT_EXTENDED_CMD
    temp_buff_ptr->used = (word)snprintf((char*)temp_buff_ptr->data_ptr,
                                        temp_buff_ptr->size,
                                        "%s\n%s: ",
                                        temp_buff_ptr->data_ptr,
                                        (ETSI_CMD_MODE ==
                                        dsatcmdp_get_operating_cmd_mode()) ? 
                                        "IMEI" : "ESN: +GSN" );
#else
    /* SVN information */
    nv_status = dsatutil_get_nv_item_per_slot(NV_UE_IMEISV_SVN_I, &ds_nv_item, slot_id);
    if (nv_status == NV_DONE_S)
    {
       temp_buff_ptr->used = (word)snprintf((char*)temp_buff_ptr->data_ptr,
                                          temp_buff_ptr->size,
                                          "%s\n%s: %02d",
                                          temp_buff_ptr->data_ptr,
                                          "SVN",
                                          ds_nv_item.ue_imeisv_svn);
    }
    /* ESN/IMEI information */
    temp_buff_ptr->used = (word)snprintf((char*)temp_buff_ptr->data_ptr,
                                        temp_buff_ptr->size,
                                        "%s\n%s: ",
                                        temp_buff_ptr->data_ptr,
                                        (ETSI_CMD_MODE ==
                                        dsatcmdp_get_operating_cmd_mode()) ? 
                                        "IMEI" : "ESN" );
#endif/* FEATURE_DSAT_EXTENDED_CMD */    
    if (DSAT_OK == dsatact_exec_gsn_cmd (mode, parse_table, 
                                         tok_ptr, res_buff_ptr))
    {
      temp_buff_ptr->used = (word)snprintf((char*)temp_buff_ptr->data_ptr,
                                          temp_buff_ptr->size,
                                          "%s%s",
                                          temp_buff_ptr->data_ptr,
                                          res_buff_ptr->data_ptr);
    }

    /* Capabilities information (heading already in response buffer) */
    if (DSAT_OK == dsatact_exec_gcap_cmd (mode, parse_table, 
                                          tok_ptr, res_buff_ptr))
    {
      temp_buff_ptr->used = (word)snprintf((char*)temp_buff_ptr->data_ptr,
                                          temp_buff_ptr->size,
                                          "%s\n%s",
                                          temp_buff_ptr->data_ptr,
                                          res_buff_ptr->data_ptr);
    }

    /* Flag if one DSM item not enough  (should never happen) */
    if (DSM_BUFFER_THRESHOLD < temp_buff_ptr->used)
    {
      DS_AT_MSG0_ERROR("Response too long for one DSM item");
    }

    /* Put composite response in result buffer */
    (void) dsatutil_memscpy((void*)res_buff_ptr->data_ptr,
                                   res_buff_ptr->size,
                            (void*)temp_buff_ptr->data_ptr,
                                   temp_buff_ptr->size);

    res_buff_ptr->used = temp_buff_ptr->used;
    (void) dsm_free_buffer( temp_buff_ptr );
  }

  /*-------------------------------------------------------
  General commands with a query should produce no response
  ---------------------------------------------------------*/
  else if (tok_ptr->op == (NA|EQ|QU))
  {
    /* this command is valid, but do nothing */
    result = DSAT_OK;
  }

  /*-------------------------------------------------------
  Any argument with the command should result in an error
  ---------------------------------------------------------*/
  else
  {
    result = DSAT_ERROR;
  }

  return result;
}  /* dsatact_exec_ati_cmd() */



/*===========================================================================

FUNCTION DSATACT_EXEC_ATE_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes E command.
  This command controls echoing of the command line to TE.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatact_exec_ate_cmd  
(
  dsat_mode_enum_type  mode,              /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr             /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  
#ifdef FEATURE_ATCOP_ECHO_CTL_MULTI_PORTS
  char* arg=(char*) tok_ptr->arg[0];
  ds3g_siolib_port_e_type open_port = ds3g_siolib_get_active_port();
  dsat_modem_port_e_type modem_port;

  if( open_port == DS3G_SIOLIB_PORT_NONE )
  {
    return DSAT_ERROR;
  }
  modem_port = dsatctab_get_modem_port_id(open_port);
  if( modem_port == DSAT_MODEM_PORT_NONE )
  {
    return DSAT_ERROR;
  }
#endif /*FEATURE_ATCOP_ECHO_CTL_MULTI_PORTS*/

  /*-------------------------------------------------------
  Set command with a value
  ---------------------------------------------------------*/
  if ( tok_ptr->op == (NA|EQ|AR) )
  {
#ifdef FEATURE_ATCOP_ECHO_CTL_MULTI_PORTS
    if( (tok_ptr->arg != NULL) && (tok_ptr->args_found !=0) )
    {
      if( (strlen(arg)== 1) && ( arg[0] == '0' || arg[0] == '1' ) )
      {
        DSATUTIL_SET_VAL(DSAT_BASIC_E_IDX,0,modem_port,0,arg[0] - '0',NUM_TYPE)
      }
      else
      {
        /*ignore garbage input data*/
        result = DSAT_ERROR;
      }
    }
    else
    {
      result = DSAT_ERROR;
    }
#else
    result = dsatparm_exec_param_cmd(mode,
                                     parse_table,
                                     tok_ptr,
                                     res_buff_ptr);
#endif /*FEATURE_ATCOP_ECHO_CTL_MULTI_PORTS*/
  }

  /*-------------------------------------------------------
  Set default value command
  ---------------------------------------------------------*/
  else if ( tok_ptr->op == NA )
  {
#ifdef FEATURE_ATCOP_ECHO_CTL_MULTI_PORTS
    DSATUTIL_SET_VAL(DSAT_BASIC_E_IDX,0,modem_port,0,1,NUM_TYPE)
#else
    DSATUTIL_SET_VAL(DSAT_BASIC_E_IDX,0,0,0,1,NUM_TYPE)
#endif /*FEATURE_ATCOP_ECHO_CTL_MULTI_PORTS*/
  }

  /*-------------------------------------------------------
  Anything else
  ---------------------------------------------------------*/
  else
  {
    result = DSAT_ERROR;
  }

  return result;
}  /* dsatact_exec_ate_cmd() */

/*===========================================================================

FUNCTION DSATACT_EXEC_ATX_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes X command.
  This command sends a CONNECT message when a connection is established by blind
  dialing and enables the additional result codes.
  
DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : If it is a success. 
SIDE EFFECTS
  None  

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatact_exec_atx_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
#ifdef FEATURE_DATA_IS707
  dsat_num_item_type old_dsat_x_val = (dsat_num_item_type)dsatutil_get_val(
                                        DSAT_BASIC_X_IDX,0,0,NUM_TYPE);
#endif /* FEATURE_DATA_IS707 */

  result = dsatparm_exec_param_cmd(mode,
                                     parse_table,
                                     tok_ptr,
                                     res_buff_ptr);

#ifdef FEATURE_DATA_IS707
  if( (result == DSAT_OK) && (IS_JCDMA_MODE() ) )
  {
    if ( ( tok_ptr->op == NA ) || ( (dsat_num_item_type)dsatutil_get_val(
                                  DSAT_BASIC_X_IDX,0,0,NUM_TYPE) == 0))
    {
      DSATUTIL_SET_VAL(DSAT_BASIC_X_IDX,0,0,0,old_dsat_x_val,NUM_TYPE)
      result= DSAT_ERROR;
    }
  }
#endif /* FEATURE_DATA_IS707 */
  return result; 
}/* dsatact_exec_atx_cmd */


#ifdef FEATURE_DATA_MUX
/*===========================================================================

FUNCTION DSATACT_EXEC_CMUX_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +CMUX command.
  This command is used to enable/disable the 3GPP TS 27.010 multiplexing protocol control channel

+CMUX=<mode>[,<subset>[,<port_speed>[,<N1>[,<T1>[,<N2>[,<T2>[,<T3>[,<k>]]]]]]]]

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatact_exec_cmux_cmd
(
  dsat_mode_enum_type  mode,              /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr  /*  Place to put response       */
)
{
#define T1_T2_FACTOR 10

  dsat_num_item_type temp;
  dsm_item_type * res_buff = NULL;
  dsat_error_info_s_type    err_info;

  err_info.errval = DSAT_ERR_NO_ERROR;
  err_info.arg_num = 0;

/* Write command */
  if(tok_ptr->op == (NA|EQ|AR))
  {
    ds3g_siolib_state_info_type* state_info;
    ds3g_siolib_port_e_type active_port = ds3g_siolib_get_active_port();

    if(ds3g_siolib_is_client_mux_port(active_port))
    {
      return dsat_send_cme_error(DSAT_CME_OP_NOT_ALLOWED);
    }

      /* Assign the default values */

      dsat_cmux_info.subset = DS_MUX_SUBSET_UIH;
      dsat_cmux_info.port_speed = DS_MUX_PHY_PORT_SPEED_1;
      dsat_cmux_info.max_frame_size_N1 = DS_MUX_CMUX_BASIC_MODE_DEFAULT_FRAME_N1;
      dsat_cmux_info.response_timer_T1 = DS_MUX_CMUX_DEFAULT_CMD_TIMER_T1;
      dsat_cmux_info.max_cmd_num_tx_times_N2 = DS_MUX_CMUX_DEFAULT_MAX_TX_N2;
      dsat_cmux_info.response_timer_T2 = DS_MUX_CMUX_DEFAULT_DLCI0_TIMER_T2;
      dsat_cmux_info.response_timer_T3 = DS_MUX_CMUX_DEFAULT_TIMER_T3;
      dsat_cmux_info.window_size_k = 2;

    /* <operation>: 0 Basic option ; 1 Advanced option */
    if(!VALID_TOKEN(0) || (ATOI_OK != dsatutil_atoi(&temp, tok_ptr->arg[0], 10)))
    {
      err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
      err_info.arg_num = 0;
      goto send_error;
    }
    if((temp != DS_MUX_MODE_BASIC) && (temp != DS_MUX_MODE_ADVANCED))
    {
      DS_AT_MSG0_ERROR ( "Invalid MUX mode of operation");
      return dsat_send_cme_error(DSAT_CME_OP_NOT_ALLOWED);
    }
    dsat_cmux_info.operating_mode = temp;

    /* <subset>: 0 UIH frames used only ;1 UI frames used only;2 I frames used only */
    if(VALID_TOKEN(1))
    {
      if(ATOI_OK != dsatutil_atoi(&temp, tok_ptr->arg[1], 10))
      {
        err_info.errval = DSAT_ERR_ATOI_CONVERSION_FAILURE;
        err_info.arg_num = 1;
        goto send_error;
      }
      if(temp > DS_MUX_SUBSET_I)
      {
        DS_AT_MSG0_ERROR ( "Invalid subset");
        return dsat_send_cme_error(DSAT_CME_OP_NOT_ALLOWED);
      }
      dsat_cmux_info.subset = temp;
    }

    /*  <port_speed> 1 - 9600 bit/s;2 - 19200 bit/s;3 - 38400 bit/s;4 - 57600 bit/s;5 - 115200 bit/s;6 - 230400 bits/s */
    if(VALID_TOKEN(2))
    {
      if(ATOI_OK != dsatutil_atoi(&temp, tok_ptr->arg[2], 10))
      {
        err_info.errval = DSAT_ERR_ATOI_CONVERSION_FAILURE;
        err_info.arg_num = 2;
        goto send_error;
      }
      if((temp < DS_MUX_PHY_PORT_SPEED_1 ) || (temp > DS_MUX_PHY_PORT_SPEED_6))
      {
        DS_AT_MSG0_ERROR ( "Invalid Port speed");
        return dsat_send_cme_error(DSAT_CME_OP_NOT_ALLOWED);
      }
      dsat_cmux_info.port_speed = temp;
    }

    /* <N1>: 1- 32768 ;31 is default for Basic option and 64 is default for Advanced option   */
    if(VALID_TOKEN(3))
    {
      if(ATOI_OK != dsatutil_atoi(&temp, tok_ptr->arg[3], 10))
      {
        err_info.errval = DSAT_ERR_ATOI_CONVERSION_FAILURE;
        err_info.arg_num = 3;
        goto send_error;
      }
      if((temp < 1 ) || (temp > DS_MUX_CMUX_MAX_FRAME_N1))
      {
        DS_AT_MSG0_ERROR ( "Invalid Max frame size");
        return dsat_send_cme_error(DSAT_CME_OP_NOT_ALLOWED);
      }
      dsat_cmux_info.max_frame_size_N1 = temp;
    }
    else
    {
      if(dsat_cmux_info.operating_mode == DS_MUX_MODE_ADVANCED)
      {
        dsat_cmux_info.max_frame_size_N1 = DS_MUX_CMUX_ADVANCED_MODE_DEFAULT_FRAME_N1;
      }
    }

    /* <T1>: 1-255; where 10 is default (100 ms)  */
    if(VALID_TOKEN(4))
    {
      if(ATOI_OK != dsatutil_atoi(&temp, tok_ptr->arg[4], 10))
      {
        err_info.errval = DSAT_ERR_ATOI_CONVERSION_FAILURE;
        err_info.arg_num = 4;
        goto send_error;
      }
      if((temp < 1 ) || (temp > 255))
      {
        DS_AT_MSG0_ERROR ( "Invalid Ack timer");
        return dsat_send_cme_error(DSAT_CME_OP_NOT_ALLOWED);
      }
      dsat_cmux_info.response_timer_T1 = T1_T2_FACTOR * temp;
    }

    /* <N2>: 0-100; where 3 is default */
    if(VALID_TOKEN(5))
    {
      if(ATOI_OK != dsatutil_atoi(&temp, tok_ptr->arg[5], 10))
      {
        err_info.errval = DSAT_ERR_ATOI_CONVERSION_FAILURE;
        err_info.arg_num = 5;
        goto send_error;
      }
      if(temp > 100)
      {
        DS_AT_MSG0_ERROR ( "Invalid Max re-transmissions");
        return dsat_send_cme_error(DSAT_CME_OP_NOT_ALLOWED);
      }
      dsat_cmux_info.max_cmd_num_tx_times_N2 = temp;
    }

    /* <T2>: 2-255; where 30 is default (300 ms); T2 must be longer than T1. */
    if(VALID_TOKEN(6))
    {
      if(ATOI_OK != dsatutil_atoi(&temp, tok_ptr->arg[6], 10))
      {
        err_info.errval = DSAT_ERR_ATOI_CONVERSION_FAILURE;
        err_info.arg_num = 6;
        goto send_error;
      }
      if((temp < 2 ) || (temp > 255))
      {
        DS_AT_MSG0_ERROR ( "Invalid response timer");
        return dsat_send_cme_error(DSAT_CME_OP_NOT_ALLOWED);
      }
      dsat_cmux_info.response_timer_T2 = T1_T2_FACTOR * temp;

      if(dsat_cmux_info.response_timer_T2 < dsat_cmux_info.response_timer_T1 )
      {
        DS_AT_MSG0_ERROR ( "T2 must be longer than T1");
        return DSAT_ERROR;
      }
    }

    /* <T3>: 1-255; where 10 is default  */
    if(VALID_TOKEN(7))
    {
      if(ATOI_OK != dsatutil_atoi(&temp, tok_ptr->arg[7], 10))
      {
        err_info.errval = DSAT_ERR_ATOI_CONVERSION_FAILURE;
        err_info.arg_num = 7;
        goto send_error;
      }
      if((temp < 1 ) || (temp > 255))
      {
        DS_AT_MSG0_ERROR ( "Invalid wake up timer");
        return dsat_send_cme_error(DSAT_CME_OP_NOT_ALLOWED);
      }
      dsat_cmux_info.response_timer_T3 = temp;
    }

    /* <k>: 1-7; where 2 is default */
    if(VALID_TOKEN(8))
    {
      if(ATOI_OK != dsatutil_atoi(&temp, tok_ptr->arg[8], 10))
      {
        err_info.errval = DSAT_ERR_ATOI_CONVERSION_FAILURE;
        err_info.arg_num = 8;
        goto send_error;
      }
      if((temp < 1 ) || (temp > 7))
      {
        DS_AT_MSG0_ERROR ( "Invalid window size");
        return dsat_send_cme_error(DSAT_CME_OP_NOT_ALLOWED);
      }
      dsat_cmux_info.window_size_k = temp;
    }

    res_buff = dsat_dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL, FALSE);

/* 
    Ensure to send OK to the terminal and close the Physical PORT on which the command is received 
    Hence we use sio_transmit and call service_ports after SIO notifies of closure.
 */

    dsat_curr_cmux_port = active_port;

    SET_PENDING(DSATETSI_EXT_ACT_CMUX_ETSI_IDX,0,DSAT_PENDING_TRUE);

    dsatrsp_fmt_result_code(DSAT_OK,&res_buff);

    state_info = ds3g_siolib_get_ds3g_siolib_state(active_port);

    sio_transmit(state_info->serial_info.stream_id,res_buff);

    ds3g_siolib_ex_flush_tx(dsatact_sio_flush_tx_cb, active_port);

    return DSAT_NO_RSP;
  }

/* Test command */
  if(tok_ptr->op == (NA|EQ|QU))
  {
    res_buff_ptr->used = (word)snprintf((char *)res_buff_ptr->data_ptr,
                                 res_buff_ptr->size,
                                 "+CMUX: (0-1),(0-2),(1-6),(1-32768),(1-255),(0-100),(2-255),(1-255),(1-7)\n");

    return DSAT_OK;
  }

/* Read command */
  if(tok_ptr->op == (NA|QU))
  {
    res_buff_ptr->used = (word)snprintf((char *)res_buff_ptr->data_ptr,
                                 res_buff_ptr->size,
                                 "+CMUX: %d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                                 dsat_cmux_info.operating_mode,
                                 dsat_cmux_info.subset,
                                 dsat_cmux_info.port_speed,
                                 dsat_cmux_info.max_frame_size_N1,
                                 (dsat_num_item_type)dsat_cmux_info.response_timer_T1/T1_T2_FACTOR,
                                 dsat_cmux_info.max_cmd_num_tx_times_N2,
                                 (dsat_num_item_type)dsat_cmux_info.response_timer_T2/T1_T2_FACTOR,
                                 (dsat_num_item_type)dsat_cmux_info.response_timer_T3,
                                 dsat_cmux_info.window_size_k
                                 );

    return DSAT_OK;
  }

  return DSAT_ERROR;

send_error:
  if( err_info.errval == DSAT_ERR_INVALID_SYNTAX)
  {
    DS_AT_MSG1_ERROR("Error:%d",err_info.errval);
  }
  else
  {
    DS_AT_MSG2_ERROR("Error:%d, arg_num:%d",err_info.errval,err_info.arg_num);
  }
  return DSAT_ERROR;

}/* dsatact_exec_cmux_cmd */

/*===========================================================================

FUNCTION DSATACT_SIO_FLUSH_TX_CB

DESCRIPTION
  sio_flush_tx() callback function in +CMUX command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Adds command in DS command buffer

===========================================================================*/
LOCAL void dsatact_sio_flush_tx_cb(void)
{
  DS_AT_MSG0_MED("In dsatact_sio_flush_tx_cb()");

  rdm_assign_port_tmp(RDM_DATA_SRVC, RDM_NULL_DEV, NULL);
} /* dsatact_sio_flush_tx_cb */

/*===========================================================================

FUNCTION DSAT_GET_CMUX_PORT

DESCRIPTION
 Function to retrive the previous Ds3g Siolib port on which CMUX command was issued.

DEPENDENCIES
  None

RETURN VALUE
  The previous Ds3g Siolib port on which CMUX command was issued

SIDE EFFECTS
  None

===========================================================================*/
uint8 dsat_get_cmux_port(void)
{
  return dsat_curr_cmux_port;
}/* dsat_get_cmux_port */

/*===========================================================================

FUNCTION DSATACT_CMUX_HANDLER

DESCRIPTION
Handler function which initializes MUX module.

DEPENDENCIES
  Physical port passed on to MUX should not be in open state.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatact_cmux_handler(void)
{
  sio_port_id_type sio_port;
  
  if(CHECK_PENDING(DSATETSI_EXT_ACT_CMUX_ETSI_IDX,0,DSAT_PENDING_TRUE))
  {
    SET_PENDING(DSATETSI_EXT_ACT_CMUX_ETSI_IDX,0,DSAT_PENDING_FALSE);

    sio_port = ds3g_siolib_map_ds_port(dsat_curr_cmux_port);

    ds_mux_ext_cli_init(dsat_cmux_info,sio_port,dsatact_cmux_notify_cb);

    ds3g_siolib_service_ports();
  }

  return;
}/* dsatact_cmux_handler */

/*===========================================================================

FUNCTION DSATACT_CMUX_NOTIFY_CB

DESCRIPTION
  MUX notify call back on closure of the physical port.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Adds command in DS command buffer

===========================================================================*/
LOCAL void dsatact_cmux_notify_cb
(
  int port_state, 
  int port_operation_state
)
{
  ds_cmd_type             *cmd_buf  = NULL;
  ds_at_mux_notify_info_type *mux_info = NULL;

  cmd_buf  = dsat_get_cmd_buf(sizeof(ds_at_mux_notify_info_type), FALSE);
  mux_info = cmd_ptr->cmd_payload_ptr;

  /* send the message to ATCOP */
  cmd_buf->hdr.cmd_id            = DS_CMD_ATCOP_MUX_NOTIFY_CMD;
  mux_info->port_state           = port_state;
  mux_info->port_operation_state = port_operation_state;
  ds_put_cmd(cmd_buf);

}/* dsatact_cmux_notify_cb */

/*===========================================================================

FUNCTION DSATACT_CMUX_NOTIFY_HANDLER

DESCRIPTION
Handler function to handle MUX notification. Upon a failure to open the physical port or a close noification
of MUX, ATCoP would need to re-open the Physical port.

DEPENDENCIES
  Physical port to be opened in case of failures, need to be closed.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatact_cmux_notify_handler
(
  ds_cmd_type  * cmd_ptr   /* DS Command pointer		   */
)
{
  int fd;
  ds_at_mux_notify_info_type *mux_info = dsat_get_cmd_payload_ptr(cmd_ptr);

  DS_AT_MSG2_MED("Recvd MUX notification, %d port_oper_state %d port_state",
                        mux_info->port_operation_state,mux_info->port_state);

/* If MUX notifies of a CLOSE operation or a Failed OPEN operation, ATCoP needs to re-open the physical port */

  if( (mux_info->port_state == DS_MUX_PHY_PORT_OPERATION_CLOSE) || 
      ( (mux_info->port_state == DS_MUX_PHY_PORT_OPERATION_OPEN) &&
        (mux_info->port_operation_state == DS_MUX_PHY_PORT_OPERATION_STATUS_FAILED) ) )
  {
    rdm_assign_port(RDM_DATA_SRVC,ds3g_siolib_convert_ds_to_rdm_port(dsat_curr_cmux_port),dsatact_rdm_notify_cb);
    dsat_curr_cmux_port = DS3G_SIOLIB_PORT_MAX;
  }

/* Store the port info in EFS to re-open the same on power-up */

  fd = efs_open("atcop_persistent.txt", O_CREAT| O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR);
  if (fd < 0)
  {
    DS_AT_MSG1_ERROR("efs_open: errcode %d", efs_errno);
  }
  else
  {
    efs_write(fd,&dsat_curr_cmux_port, sizeof(dsat_curr_cmux_port));
    efs_close(fd);
  }

  return DSAT_ASYNC_EVENT;
}/* dsatact_cmux_notify_handler */
#endif /* FEATURE_DATA_MUX */
/*===========================================================================

FUNCTION DSATACT_CALL_ABORT_HANDLER

DESCRIPTION
  This function does nothing but return a result to indicate the
  call should be aborted.  It should be installed in the command table
  as the abort handler function for abortable AT commands which bring
  up a call.

DEPENDENCIES
  None

RETURN VALUE
  Value indicating whether or not call should be aborted:
      TRUE if call should be aborted, FALSE otherwise.

SIDE EFFECTS
  Call Manager is requested to end voice calls.

===========================================================================*/
/* ARGSUSED */
boolean dsatact_call_abort_handler
(
  const dsati_cmd_type *cmd_table_entry_ptr  /* Ptr to cmd entry in table. */
)
{
  dsati_mode_e_type  current_mode = dsatcmdp_get_current_mode();
  sys_modem_as_id_e_type subs_id = dsat_get_current_subs_id(FALSE);

  if ( IS_ETSI_MODE(current_mode) )
  {

    cm_call_id_type call_id;

    /* look for DSAT_VOICE_STATE_CONNECTING call ID... */
    for ( call_id = 0; call_id < CM_CALL_ID_MAX; call_id++ )
    {
      /* If DSAT_VOICE_STATE_CONNECTING call ID is found 
          (there should be only one)... */
      if ( voice_state[subs_id][call_id].state == DSAT_VOICE_STATE_CONNECTING )
      {
        dsatvoice_voice_call_abort_handler(subs_id);
        return FALSE;
      }
    }
  }

  if ( dsat_curr_abort_handler == NULL )
  {
    /* the abort handler hasn't been set and the user 
       directs to abort the call.
    */
    dsatcmdp_abortable_state = ABORTING;
  }
  else
  {
    (*dsat_curr_abort_handler)();
  }

  return TRUE;
} /* dsatact_call_abort_handler */


/*===========================================================================

FUNCTION DSATACT_PROC_DIAL_STR

DESCRIPTION
  Parses a dial string, looking for allowed non-digits, and
  illegal non-digits.  Digits, and allowed non-digits are copied
  to the output.  Illegal characters terminate processing.  All
  other characters are ignore.  Upper and lower case are treated
  as being the same.

  Since the output will be no longer than the input, the output
  string may be the same as the input string.

DEPENDENCIES
  None

RETURN VALUE
  Returns enum value indicating dial string content or error

SIDE EFFECTS
  None

===========================================================================*/
dsat_dial_val_e_type dsatact_proc_dial_str
(
  const byte * in_ptr,          /*  Input string, null terminated   */
  byte * out_ptr                /*  Output string, null terminated  */
)
{
  dsat_dial_val_e_type result = DSAT_DIAL_EMPTY;
  byte c;
  
  while (((c = *in_ptr++) != '\0') &&
         (result != DSAT_DIAL_ERROR))
  {
    dsat_dial_val_e_type vresult = dsatutil_validate_dial_char(&c);
    switch (vresult)
    {
      case DSAT_DIAL_DIGIT:
      case DSAT_DIAL_POUND:
      case DSAT_DIAL_STAR:
        *out_ptr++ = c;
        if (result == DSAT_DIAL_EMPTY)
        {
          result = DSAT_DIAL_DIGIT;
        }
        break;

      case DSAT_DIAL_ASCII_ABC:
        /* Allowed ASCII characters */
        *out_ptr++ = c;
        result = DSAT_DIAL_ASCII;
        break;

      case DSAT_DIAL_ASCII_D:
      case DSAT_DIAL_STRIP:
        /* Stripped characters */
        result = DSAT_DIAL_ASCII;
        break;

      case DSAT_DIAL_SEMICOLON:
        /* Do not pass semicolon to output */
        result = (ETSI_CMD_MODE == dsatcmdp_get_operating_cmd_mode()) ?
                  vresult : DSAT_DIAL_ERROR;
        break;

      case DSAT_DIAL_ASCII_E:
      case DSAT_DIAL_IGNORE:
        break;

      default:
        result = DSAT_DIAL_ERROR;
        break;
        
    } /* switch */
  } /* while */
  *out_ptr = '\0';
  return result;

} /* dsatact_proc_dial_str () */


#ifdef FEATURE_DSAT_ETSI_MODE
/*===========================================================================

FUNCTION DSAT_GET_IMEI

DESCRIPTION
  This function queries the IMEI value in NV and
  decodes it into an ASCII string.  The result is
  sent back to the caller in the passed buffer pointer.

DEPENDENCIES
  NV task must be available

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsat_get_imei
(
  byte * rb_ptr,                  /* Pointer to return buffer */
  dsat_num_item_type  len         /* Buffer MAX len*/
)
{
  dsat_result_enum_type result = DSAT_OK;
  byte imei_bcd_len = 0;
  byte n = 0;
  byte digit;
  char imei_ascii[(NV_UE_IMEI_SIZE-1)*2];
  mmgsdi_slot_id_enum_type slot_id = dsat_get_current_slot_id();
  nv_stat_enum_type nv_status;

  rb_ptr[0] = '\0';

  /*  Read the IMEI  from NV */
  nv_status = dsatutil_get_nv_item_per_slot(NV_UE_IMEI_I, &ds_nv_item, slot_id);
  if (nv_status == NV_DONE_S)
  {
    /* Convert it to ASCII */
    imei_bcd_len = ds_nv_item.ue_imei.ue_imei[0];

    if( imei_bcd_len <= (NV_UE_IMEI_SIZE-1) )
    {
      /* This is a valid IMEI */
      memset(imei_ascii, 0, (NV_UE_IMEI_SIZE-1)*2);

      for( n = 1; n <= imei_bcd_len; n++ )
      {
        digit = ds_nv_item.ue_imei.ue_imei[n] & 0x0F;
        if( ( digit <= 9 ) || ( n <= 1 ) )
        {
          imei_ascii[ (n - 1) * 2 ] = digit + '0';
        }
        else
        {
          imei_ascii[ (n - 1) * 2 ] = '\0';
          break;
        }
        
        digit = ds_nv_item.ue_imei.ue_imei[n] >> 4;
        if( ( digit <= 9 ) || ( n <= 1 ) )
        {
          imei_ascii[ ((n - 1) * 2) + 1 ] = digit + '0';
        }
        else
        {
          imei_ascii[ ((n - 1) * 2) + 1 ] = '\0';
          break;
        }
      }
      
      /* Lose the first byte because it is just the ID */
       (void) dsatutil_memscpy((void*)rb_ptr,
                                      len,
                               (void*)(imei_ascii + 1),
                                     (NV_UE_IMEI_SIZE-1)*2-1);
      
    }
    else
    {
      /* This is an invalid IMEI */
      DS_AT_MSG0_ERROR("Invalid IMEI value from NV");
      result = DSAT_ERROR;
    }
  }
  else
  {
    result = DSAT_ERROR;
  }
  return result;
}

#endif /* FEATURE_DSAT_ETSI_MODE */



/*===========================================================================

FUNCTION PROC_DIAL_STR

DESCRIPTION
  Parses a dial string in atd cmd token.  The number is in 
  tok_ptr->arg[0].

  Upon function returns, tok_ptr->arg[0] contains the processed/altered
  number.  The addtional information from the parsing process are in
  dial_ptr.

  The parser looks for chars of different category, see 
  dsat_dial_val_e_type.  It stripps out chars not needed depending on
  operating mode and on whether it is for voice call.
  It set the corresponding attrib bit on when encounters special chars
  or find a packet call pattern.

  Illegal characters terminate processing.  All other characters are ignore.

DEPENDENCIES
  None

RETURN VALUE
  TRUE          Processing finished without problem.
  FALSE         There is illegal char in the dial string, 
                or the mode is not right.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL boolean proc_dial_str
(
  /* cmd token containing the atd argument       */
  const tokens_struct_type  *   tok_ptr,    

  /* dial string contain the parsed information
     This is output                              */
  dsati_dial_str_type       *   dial_ptr,

  /* current system operating mode               */
  dsati_mode_e_type         *   cur_mode_ptr
)
{
#ifdef FEATURE_DSAT_ETSI_MODE
  /*-------------------------------------------------------
    char sets in ETSI mode
    -------------------------------------------------------*/
  /* Illegal characters, currently no illegal chars.  
     Non-legal chars will be stripped out */
  static const byte illegal_etsi[]      = "";

  /* Cause DIAL_ATTRIB_ASCII be on */
  static const byte ascii_etsi[]        = "ABC";

  /* The chars the subsystem needs for dialing, + is kept
     only if it is at the beginning of the dial string */
  /* If PS data call add ',' to allowed ETSI dial characters as 
     multiple <cid>s in dial string can be separated by commas.
     Multiple <cid>s in dial string not currently supported and 
     this will exceed supported number of options in 
     dsatutil_check_opt_dial_str() and err. */
  static const byte keep_etsi_pkt[]     = "0123456789*#+ABC,";
  static const byte keep_etsi[]         = "0123456789*#+ABC";

  /*-------------------------------------------------------
    char sets in ETSI mode for voice call numbers
    -------------------------------------------------------*/
  /* Illegal characters, currently no illegal chars.  
     Non-legal chars will be stripped out */
  static const byte illegal_voice[]      = "";

  /* Cause DIAL_ATTRIB_ASCII be on */
  static const byte ascii_voice[]        = "";

  /* The chars the subsystem needs for dialing, + is kept
     only if it is at the beginning of the dial string */
  static const byte keep_voice[]        = "0123456789*#";

#endif /* FEATURE_DSAT_ETSI_MODE */


#ifdef FEATURE_DATA_IS707
  /*-------------------------------------------------------
    char sets in CDMA mode
    -------------------------------------------------------*/
    /* Illegal characters, currently no illegal chars.  
     Non-legal chars will be stripped out */
  static const byte illegal_cdma[]      = "";

  /* Cause DIAL_ATTRIB_ASCII be on */
  static const byte ascii_cdma[]        = "ABCDTP,W@!$;";

  /* This the set of legal dial chars */
  static const byte keep_cdma[]        = "0123456789*#ABCDTP,W@!$;";

#endif /* FEATURE_DATA_IS707 */

  const byte  *       illegal_set;
  const byte  *       ascii_set;
  const byte  *       keep_set;

  /* wrt_ptr points to the destination of processing
     rd_ptr  points to the source of processing
  */
  byte  *       wrt_ptr;
  byte  *       rd_ptr;
  byte          tmp_c;

  /* eol_ptr is writable end of line pointer */
  byte  * eol_ptr;

/*------------------------------------------------------------------*/

  /* init attrib */
  dial_ptr->attrib = 0;
  dial_ptr->num    = tok_ptr->arg[0];

  ASSERT(*tok_ptr->end_of_line == '\0');

  eol_ptr = tok_ptr->end_of_line;

  DS_AT_MSG1_HIGH("Curr mode = %d",*cur_mode_ptr);

  if(*cur_mode_ptr == DSAT_MODE_1XLTE)
  {
    *cur_mode_ptr = DSAT_MODE_LTE;
  }
  else if(*cur_mode_ptr == DSAT_MODE_1XDO)
  {
    *cur_mode_ptr = DSAT_MODE_CDMA;
  }
  if ( dsatutil_strcmp_pkt_dial_str( dial_ptr->num, cur_mode_ptr) )
  {
    dial_ptr->attrib |= DIAL_ATTRIB_PKT;
  }
  
#ifdef FEATURE_DSAT_ETSI_MODE
  /* In ETSI mode, check for num[len-1,2,3] for ; [G|g], [I|i],
     set attrib accordingly strip them out and update len */
  if ( IS_ETSI_MODE(*cur_mode_ptr) )
  {
    /* if the last char of dial string (pointed by tok_ptr->end_of_line)
       is ;.  Turn on attrib DIAL_ATTRIB_SEMICOLON bit.
       Set ; be null and move eol_ptr one char */
    if ( (eol_ptr - dial_ptr->num > 0)
        && *(eol_ptr-1) == ';')
    {
      dial_ptr->attrib |= DIAL_ATTRIB_SEMICOLON;
      *(--eol_ptr) = '\0';
    }

    /* Check if Redial  i.e if the first char in dial string is L 
       then 'ATDL' is converted to its equivalent 'ATD>LD1'.
       i.e Dial the first entry from LD phone book
    */
    if (*dial_ptr->num == 'L')
    {
      DS_AT_MSG0_HIGH("Processing command -- ATDL");
      if (*(dial_ptr->num + 1)!= '\0')
      {
        return FALSE;
      }
      (void)strlcpy( (char *)dial_ptr->num, "LD1", 4);
      dial_ptr->len = 3;
      dial_ptr->attrib |= DIAL_ATTRIB_GREAT;
      return TRUE;
    }

    /* Check if ISDN dial  i.e if the first char in dial string is I */
    if (*dial_ptr->num == 'I')
    {
       DS_AT_MSG0_HIGH("Processing command -- ATDI");
       /* 'ATDI<dial-string>' is converted to its equivalent 'ATD<dial-string>' */
       dial_ptr->num++;
    }

    /* Check if this is a direct dialing number
       i.e if the first char in dial string is >
    */
    if (*dial_ptr->num == '>')
    {
      dial_ptr->attrib |= DIAL_ATTRIB_GREAT;
      dial_ptr->num++;
      if ( dsatutil_strip_quotes_out(dial_ptr->num, dial_ptr->num, 
                                     (uint16)(eol_ptr - dial_ptr->num)) )
      {
        dial_ptr->len = (uint16)strlen( (const char *)dial_ptr->num );
      }
      else
      {
        dial_ptr->len = (uint16)(eol_ptr - dial_ptr->num);
      }

      return TRUE;
    }
  }

#endif /* FEATURE_DSAT_ETSI_MODE */

  /* process chars one by one
     1. if encoutners non digit # * turn on DIAL_ATTRIB_ASCII
     2. strip out chars according to ETSI, CDMA, or voice */

#ifdef FEATURE_DSAT_ETSI_MODE

  if ( IS_ETSI_MODE(*cur_mode_ptr) && 
       (dial_ptr->attrib & DIAL_ATTRIB_SEMICOLON) )
  {
    illegal_set =       illegal_voice;
    ascii_set   =       ascii_voice;
    keep_set    =       keep_voice;
  }
  else if ( IS_ETSI_MODE(*cur_mode_ptr) )
  {
    illegal_set = illegal_etsi;
    ascii_set   = ascii_etsi;
    keep_set    = (dial_ptr->attrib & DIAL_ATTRIB_PKT) ?
                    keep_etsi_pkt : keep_etsi;
  }
#endif /* FEATURE_DSAT_ETSI_MODE */

#ifdef FEATURE_DATA_IS707

#ifdef FEATURE_DSAT_ETSI_MODE
  else
#endif /* FEATURE_DSAT_ETSI_MODE */
  if (IS_CDMA_MODE(*cur_mode_ptr))
  {
    illegal_set =       illegal_cdma;
    ascii_set   =       ascii_cdma;
    keep_set    =       keep_cdma;
  }
#endif /* FEATURE_DATA_IS707 */
#if defined(FEATURE_DSAT_ETSI_MODE) || defined(FEATURE_DATA_IS707)
  else
#endif /* FEATURE_DSAT_ETSI_MODE || FEATURE_DATA_IS707 */
  {
    /* Unexpected mode, don't know how to process the dial string */
    return FALSE;
  }    

  wrt_ptr = dial_ptr->num;
  rd_ptr = dial_ptr->num;

  /* process '+' for DIAL_ATTRIB_START_PLUS attribute*/
  if (*rd_ptr == '+')
  {
    dial_ptr->attrib |= DIAL_ATTRIB_START_PLUS;
    rd_ptr++;
    wrt_ptr++;
  }

  /* Dial string scan loop */
  while (*rd_ptr != '\0')
  {
    /* In ETSI mode, check for I/G modifiers */
    if ( IS_ETSI_MODE(*cur_mode_ptr) )
    {
      /* Check for CUG control */
      if ('G' == *rd_ptr)
      {
        dial_ptr->attrib |= DIAL_ATTRIB_UP_G;  /* set bit */
        dial_ptr->attrib &= ~DIAL_ATTRIB_DN_G; /* clear bit */
      }
      else if ('g' == *rd_ptr)
      {
        dial_ptr->attrib |= DIAL_ATTRIB_DN_G;  /* set bit */
        dial_ptr->attrib &= ~DIAL_ATTRIB_UP_G; /* clear bit */
      }
      /* Check for CLIR control */
      else if ('I' == *rd_ptr)
      {
        dial_ptr->attrib |= DIAL_ATTRIB_UP_I;  /* set bit */
        dial_ptr->attrib &= ~DIAL_ATTRIB_DN_I; /* clear bit */
      }
      else if ('i' == *rd_ptr)
      {
        dial_ptr->attrib |= DIAL_ATTRIB_DN_I;  /* set bit */
        dial_ptr->attrib &= ~DIAL_ATTRIB_UP_I; /* clear bit */
      }
    }
    
    tmp_c = UPCASE(*rd_ptr);

    /* keep chars */
    if (strchr((char *)keep_set, (char) tmp_c) != NULL)
    {
      /* see if the char is in ascii_set */
      if (strchr((char *)ascii_set, (char) tmp_c) != NULL)
      {
        dial_ptr->attrib |= DIAL_ATTRIB_ASCII;
      }

      /* the *rd_ptr is in the keep_set */
      *wrt_ptr = *rd_ptr;
      wrt_ptr++;
      rd_ptr++;
    }
    else if (strchr((char *)illegal_set, (char) *rd_ptr) != NULL)
    {
    /* current rd_ptr points to an illegal chars */
      return FALSE;
    }
    else
    {
      /* strip this char out */
      rd_ptr++;
    }
  }

#ifdef FEATURE_DSAT_ETSI_MODE

  /* Verify the attributes of the call.
     If atleast one of them is set; 
     it becomes per call CLIR invocation/suppression
     Don't apply CLIR (dsat_clir_val) settings 
     Otherwise apply the appropriate CLIR settings */
  if ( IS_ETSI_MODE(*cur_mode_ptr) )
  {
    if(((dial_ptr->attrib & DIAL_ATTRIB_UP_I) == FALSE) &&
       ((dial_ptr->attrib & DIAL_ATTRIB_DN_I) == FALSE))
    {
      if((dsat_num_item_type)dsatutil_get_val(DSATETSI_EXT_ACT_CLIR_ETSI_IDX,
               0,0,NUM_TYPE) == (dsat_num_item_type)DSAT_CLIR_INVOCATION_OPTION)
      {
        dial_ptr->attrib |= DIAL_ATTRIB_UP_I;  /* set bit */
        dial_ptr->attrib &= ~DIAL_ATTRIB_DN_I; /* clear bit */
      }
      else if((dsat_num_item_type)dsatutil_get_val(DSATETSI_EXT_ACT_CLIR_ETSI_IDX,
                 0,0,NUM_TYPE) == (dsat_num_item_type)DSAT_CLIR_SUPPRESSION_OPTION)
      {
        dial_ptr->attrib |= DIAL_ATTRIB_DN_I;  /* set bit */
        dial_ptr->attrib &= ~DIAL_ATTRIB_UP_I; /* clear bit */
      }
    }
  }

#endif /* FEATURE_DSAT_ETSI_MODE*/

  /* null terminate the dial_str */
  *wrt_ptr = '\0';
  eol_ptr = wrt_ptr;
  dial_ptr->len = (uint16) (eol_ptr - dial_ptr->num);
    
  return TRUE;
} /* proc_dial_str */


/*===========================================================================

FUNCTION ENTER_ONLINE_DATA_MODE

DESCRIPTION
  This function is used to initiate an asynchronous SIO/ATCOP processing mode
  change to online data mode. The flush_tx parameter is used to indicate
  whether the SIO TX fifo should be flushed prior to switching to online
  data mode as follows:

    If flush_tx == TRUE, the SIO TX fifo will be flushed.
    If flush_tx == FALSE, the SIO TX fifo will not be flushed, any data 
      currently in the queue will be lost.

  The function sends either a DS_ENTER_ONLINE_DATA_TX_FLUSH_CMD or 
  DS_ENTER_ONLINE_DATA_NO_TX_FLUSH_CMD depending on the value of flush_tx to
  3G Dsmgr to initiate the mode change.  If no TX flushing is required, the
  mode change will occur immediately, otherwise, 3G Dsmgr will schedule the 
  SIO TX fifo to be flushed.  When the flush completes, the switch to online 
  data mode will be made.

DEPENDENCIES
  Must be able to allocate a DS Task command buffer.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void enter_online_data_mode
(
  boolean flush_tx
)
{
  ds_cmd_type * cmd_ptr;

  /* Allocate the command buffer and queue the command to the DS Task */
  cmd_ptr = dsat_get_cmd_buf(DS_CMD_PAYLOAD_SIZE_ZERO, FALSE);
  /* Need to determine the appropriate command based on whether or
  ** not the TX buffer should be flushed.
  */
  if (flush_tx)
  {
    cmd_ptr->hdr.cmd_id = DS_CMD_ENTER_ONLINE_DATA_TX_FLUSH;
  }
  else
  {
    cmd_ptr->hdr.cmd_id = DS_CMD_ENTER_ONLINE_DATA_NO_TX_FLUSH;
  }

  /* Send the command on its way */
  ds_put_cmd(cmd_ptr);
} /* enter_online_data_mode() */
/*===========================================================================

FUNCTION PROCESS_AMPS_CMD

DESCRIPTION

DEPENDENCIES


RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL dsat_result_enum_type process_amps_cmd ( void )
{
  dsat_num_item_type  current_amp_s = (dsat_num_item_type)dsatutil_get_val(
                                      DSAT_BASIC_N_S_IDX,0,0,NUM_TYPE);
  ds3g_siolib_port_e_type open_port = ds3g_siolib_get_active_port();

  /*-------------------------------------------------------
          DSR should be always Asserted
  -------------------------------------------------------*/
  /* Change from "ON during Connection" to "Always ON" */
  if ( (dsat_num_item_type)dsatutil_get_val(
               DSAT_BASIC_N_S_IDX,0,0,NUM_TYPE) == DS3G_DSR_ALWAYS_ON )
  {
    if ( ds3g_siolib_dsr_assert( open_port ) != DS3G_SIOLIB_OK )
    {
      DS_AT_MSG0_ERROR("AT&S0 setting failed");
      /* Re-setting dsat_amp_s_val */

      DSATUTIL_SET_VAL(DSAT_BASIC_N_S_IDX,0,0,0,current_amp_s,NUM_TYPE)
      return DSAT_ERROR;
    }
  }
  /*---------------------------------------------------------------
     DSR should be De-asserted and assert at CONNECT 
  ----------------------------------------------------------------*/
  /* Change from "Always ON" to "ON during Connection"*/
  else /* DS3G_DSR_CONNECT_ON */
  {
    if ( ds3g_siolib_dsr_deassert ( open_port ) != DS3G_SIOLIB_OK)
    {
      DS_AT_MSG0_ERROR("AT&S1 setting failed");
      /* Re-setting dsat_amp_s_val */
      DSATUTIL_SET_VAL(DSAT_BASIC_N_S_IDX,0,0,0,current_amp_s,NUM_TYPE)
      return DSAT_ERROR;
    }
  }
  DS_AT_MSG1_LOW("Changed AT&S to %d",(dsat_num_item_type)dsatutil_get_val(
                                 DSAT_BASIC_N_S_IDX,0,0,NUM_TYPE));
  return DSAT_OK;
}

#ifdef FEATURE_DSAT_GOBI_MAINLINE
#if defined (FEATURE_CDMA_800) || defined (FEATURE_CDMA_1900)
#if defined (FEATURE_OTASP)
/*===========================================================================

FUNCTION DSATACT_EXEC_ATD_OTASP_CALL

DESCRIPTION
  This function execute OTASP call

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR          : if there was any problem in executing the command
    DSAT_ASYNC_CMD      : if voice call is made.

SIDE EFFECTS
  Data dial handler or voice call handler invoked to init a call.

===========================================================================*/
LOCAL dsat_result_enum_type dsatact_exec_atd_otasp_call
(
  dsati_dial_str_type* dial_str_ptr
)
{
  cm_num_s_type                 dial_num;
  cm_cdma_orig_params_s_type    call_orig_params;
  cm_orig_alpha_s_type          alpha_buf;
  sys_modem_as_id_e_type        subs_id = dsat_get_current_subs_id(FALSE);

  if( dsatvoice_is_voice_call (subs_id, voice_call_id))
  {
    DS_AT_MSG0_ERROR("In voice call already");
    return DSAT_ERROR;
  }

#ifdef FEATURE_DUAL_ACTIVE
  if (FALSE == dsatvoice_validate_call_cmd(subs_id, DSAT_CALL_CMD_ORIG))
  {
    return dsat_send_cme_error(DSAT_CME_OP_NOT_ALLOWED);
  }
#endif /* FEATURE_DUAL_ACTIVE */

   /* default the CM call origination parameters */
  memset(&dial_num, 0, sizeof(cm_num_s_type));
  memset(&alpha_buf, 0, sizeof(cm_orig_alpha_s_type));
  memset(&call_orig_params, CM_CALL_CMD_PARAM_DEFAULT_VALUE, 
                            sizeof(call_orig_params));
  DS_AT_MSG0_MED("OTASP call orig");
  cm_num_fill(&dial_num, 
              (uint8 *) dial_str_ptr->num, 
              dial_str_ptr->len, 
              CM_DIGIT_MODE_8BIT_ASCII);

  /* create the CM call origination request */
  if ( cm_mm_call_cmd_orig(dsatcmif_call_cmd_cb_func,
                  NULL,
                  dsatcm_client_id,
                  CM_CALL_TYPE_VOICE,
                  CM_SRV_TYPE_AUTOMATIC,
                  NULL,
                  &dial_num,
                  &alpha_buf,
                  &call_orig_params, /* cdma orig params */
                  NULL, /* GW CS parameters */
                  NULL, /* GW PS parameters */
                  &voice_call_id
                  ) )
  {
    dsatvoice_go_to_connecting(subs_id, voice_call_id);
    /* the command callback function will be called */
    DS_AT_MSG0_MED("call status successful");
    return DSAT_ASYNC_CMD;
  }
  else
  {
    DS_AT_MSG0_ERROR("call status error");
    voice_call_id = CM_CALL_ID_INVALID;
    return DSAT_ERROR;
  }
} /*dsatact_exec_atd_otasp_call*/
#endif /*defined (FEATURE_OTASP)*/
#endif /*defined (FEATURE_CDMA_800) || defined (FEATURE_CDMA_1900)*/
#endif /*FEATURE_DSAT_GOBI_MAINLINE*/

#ifndef FEATURE_DSAT_CUST
/*===========================================================================

FUNCTION DSATACT_EXEC_CLCC_CMD

DESCRIPTION
  This is the command handler function for +CLCC.

  This function sends request to CM to get the all calls details

  Return OK for test command.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_ASYNC_CMD : if the action command succeeds.
  DSAT_OK        : if this is a test command.
  DSAT_ERROR     : if the action command fails or other than test or 
                   execute command is received.

SIDE EFFECTS
  None
  
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatact_exec_clcc_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_ASYNC_CMD;
  boolean               cm_result;
  sys_modem_as_id_e_type subs_id = dsat_get_current_subs_id(FALSE);
  dsatetsicall_call_state_da_info  *call_da_val = NULL;

  call_da_val = dsat_get_base_addr_per_subs( DSAT_CALL_DA_VALS, subs_id, FALSE);
  /* Reset +CLCC abort flag */
  call_da_val->dsat_abort_call_info_listing = FALSE;
  if(tok_ptr->op == (NA|EQ|QU))
  {
    /* Test command is valid - we support this command */
    result = DSAT_OK;
  }
  else if (tok_ptr->op == NA)
  {

    SET_PENDING(DSAT_EXT_CLCC_IDX,0,DSAT_PENDING_TRUE)
    /* Get call ids of all CS calls */
#ifdef FEATURE_DUAL_SIM
    cm_result = cm_mm_call_cmd_get_call_info_list_per_subs(dsatcmif_call_cmd_cb_func,
                    NULL,
                    dsatcm_client_id,
                    subs_id);
#else
    cm_result = cm_mm_call_cmd_get_call_info_list(dsatcmif_call_cmd_cb_func,
                    NULL, 
                    dsatcm_client_id);
#endif /* FEATURE_DUAL_SIM */
    if (!cm_result)
    {
      DS_AT_MSG0_ERROR("Failed to query CM call info list");
      SET_PENDING(DSAT_EXT_CLCC_IDX,0,DSAT_PENDING_FALSE)
      return DSAT_ERROR;
    }

  }
  else
  {
    /* other commands are illegal */
    result = DSAT_ERROR;
  }

#ifdef FEATURE_DSAT_MDM_FUSION
  /* CLCC is an action COMMON_CMD; So set to append the MSM response  */
  DS_AT_MSG1_HIGH("Result before append set ,%d",result);
  if(result == DSAT_ASYNC_CMD)
  {
    dsat_set_append_rsp_state( TRUE );
  }
#endif /* FEATURE_DSAT_MDM_FUSION */

  return result;

} /* dsatact_exec_clcc_cmd */


/*===========================================================================

FUNCTION DSATACT_CALL_INFO_ABORT_HANDLER

DESCRIPTION
  This function generates an abort command to terminate asynchronous 
  processing for +CLCC command.  It should be
  installed in the command table as the abort handler function for
  +CLCC command.

DEPENDENCIES
  None

RETURN VALUE
  Value indicating whether or not call should be aborted:
      TRUE if call should be aborted, FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
boolean dsatact_call_info_abort_handler
(
  const dsati_cmd_type *cmd_table_entry_ptr  /* Ptr to cmd entry in table. */
)
{
  ds_cmd_type                      *cmd_ptr     = NULL;
  sys_modem_as_id_e_type            subs_id = dsat_get_current_subs_id(FALSE);
  dsatetsicall_call_state_da_info  *call_da_val = NULL;
  ds_at_cmd_status_type            *status_cmd_ptr  = NULL;

  call_da_val = dsat_get_base_addr_per_subs(DSAT_CALL_DA_VALS, subs_id, FALSE);

  call_da_val->dsat_abort_call_info_listing = TRUE;

  cmd_ptr  = dsat_get_cmd_buf(sizeof(ds_at_cmd_status_type), FALSE);
  status_cmd_ptr = cmd_ptr->cmd_payload_ptr;

  /* Send abort event to restore normal command processing */
  cmd_ptr->hdr.cmd_id = DS_CMD_ATCOP_STATUS_CMD;
  status_cmd_ptr->cmd_id = (uint16)CLCC_CM_CMD_ABORT;
  status_cmd_ptr->cmd_status = MMGSDI_SUCCESS;
  status_cmd_ptr->cmd_data = 0;
  ds_put_cmd (cmd_ptr);

  return TRUE;
} /* dsatact_call_info_abort_handler */
#endif /* FEATURE_DSAT_CUST */


/*===========================================================================

FUNCTION  DSATACT_CALL_INFO_LIST_CLCC_CMD_HANDLER

DESCRIPTION
  This function processes the Call Manager active call ids list and send 
  a request to CM with Call id of each active CS call to get complete 
  information about that Call Id
  
DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ASYNC_CMD: If a request is sent to CM to retrieve the given call id 
                    information
    DSAT_ERROR :    if there was any problem in executing the command
    DSAT_OK :       if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatact_call_info_list_clcc_cmd_handler
(
  const ds_at_cm_call_event_type *call_event_ptr, /* Call event pointer */
  sys_modem_as_id_e_type          subs_id
)
{
  dsat_result_enum_type             result = DSAT_ASYNC_CMD;
  const dsat_cm_call_state_info_list_s_type *call_info;
  int index = 0;
  boolean cm_result;
  dsatetsicall_call_state_da_info  *call_da_val = NULL;

  call_da_val = dsat_get_base_addr_per_subs(DSAT_CALL_DA_VALS,subs_id, FALSE);
    
  call_info = &call_event_ptr->event_info.dsat_cm_call_info_list;
  call_da_val->dsatact_num_clcc_calls = 0;
  call_da_val->dsatact_clcc_mpty_calls = 0;

  if (call_da_val->dsat_abort_call_info_listing == TRUE)
  {
    /* user aborted the command */
    call_da_val->dsat_abort_call_info_listing = FALSE;	
	
    SET_PENDING(DSAT_EXT_CLCC_IDX ,0, DSAT_PENDING_FALSE)

    /* Abort handler responsible for sending result code */
    return DSAT_ASYNC_EVENT;
  }
  if (call_info->number_of_active_calls == 0)
  {
    /* No active calls */
    SET_PENDING(DSAT_EXT_CLCC_IDX ,0, DSAT_PENDING_FALSE)
    return DSAT_OK;
  }
    
  for (index = call_info->number_of_active_calls - 1; (index >= 0) && 
                 (call_da_val->dsatact_num_clcc_calls < CM_CALL_ID_MAX) ; index--)
  {
    /* Get Call id s of CS data/voice calls */
    if ((call_info->info[index].call_type == CM_CALL_TYPE_VOICE) ||
        (call_info->info[index].call_type == CM_CALL_TYPE_CS_DATA)||
        (call_info->info[index].call_type == CM_CALL_TYPE_PS_DATA))
    {
      call_da_val->dsatcmif_clcc_no_call_ids[call_da_val->dsatact_num_clcc_calls].call_id = 
        call_info->info[index].call_id;
      call_da_val->dsatcmif_clcc_no_call_ids[call_da_val->dsatact_num_clcc_calls].sub_state = 
        call_info->info[index].call_subst;
      call_da_val->dsatact_num_clcc_calls++;

      if ((call_info->info[index].call_type == CM_CALL_TYPE_VOICE)
         && (call_info->info[index].call_state == CM_CALL_STATE_CONV) &&
         (call_info->info[index].call_subst.conv == CM_CALL_CONV_CALL_ACTIVE))
      {
        /* If call is voice call and in conversation then increment */
        call_da_val->dsatact_clcc_mpty_calls++;
      }
    }
  }

  if (call_da_val->dsat_abort_call_info_listing == TRUE)
  {
    /* user aborted the command */
    call_da_val->dsat_abort_call_info_listing = FALSE;
    call_da_val->dsatact_num_clcc_calls = 0;
    /* Abort handler responsible for sending result code */
    return DSAT_ASYNC_EVENT;
  }
  
  if (call_da_val->dsatact_num_clcc_calls == 0)
  {
    /* No active CS calls */
    result = DSAT_OK;
  }
  else
  {
     cm_result = cm_mm_call_cmd_get_call_info(
                   dsatcmif_call_cmd_cb_func, 
                   NULL, 
                   dsatcm_client_id, 
                   call_da_val->dsatcmif_clcc_no_call_ids[call_da_val->dsatact_num_clcc_calls - 1].call_id);

    if (!cm_result)
    {
      call_da_val->dsatact_num_clcc_calls = 0;
      DS_AT_MSG0_HIGH("Failed to query CM call info");
      result = dsat_send_cme_error(DSAT_CME_UNKNOWN);
    }

  }

  SET_PENDING(DSAT_EXT_CLCC_IDX ,0, DSAT_PENDING_FALSE)
  return result;
    
} /* dsatact_call_info_list_clcc_cmd_handler */


/*===========================================================================

FUNCTION  DSATACT_CALL_INFO_CLCC_CMD_HANDLER

DESCRIPTION
  This function processes the Call Manager call id information and send a 
  request to CM with remaining active CS calls call id to get complete 
  information about that Call Id
  
DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ASYNC_CMD: If a request is sent to CM to retrieve the given call id 
                    information
    DSAT_ERROR :    if there was any problem in executing the command
    DSAT_OK :       if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type
	dsatact_call_info_clcc_cmd_handler
(
  ds_at_cm_call_event_type *call_event_ptr, /* Call event pointer */
  sys_modem_as_id_e_type    subs_id
)
{
  dsat_result_enum_type  result = DSAT_ASYNC_CMD;
  boolean cm_result;
  dsatetsicall_call_state_da_info  *call_da_val = NULL;
  
  call_da_val = dsat_get_base_addr_per_subs(DSAT_CALL_DA_VALS,subs_id, FALSE);

  /* display the Call id information */
  if( !( ( call_da_val->dsatact_num_clcc_calls <= CM_CALL_ID_MAX )&&
        (  call_da_val->dsatact_num_clcc_calls > 0 ) ) )
  {
    return DSAT_ERROR;
  }
  result = dsatact_report_clcc(subs_id, &call_event_ptr->event_info, 
             call_da_val->dsatcmif_clcc_no_call_ids[call_da_val->dsatact_num_clcc_calls - 1].sub_state);
  if (call_da_val->dsat_abort_call_info_listing == TRUE)
  {
    /* user aborted the command */
    call_da_val->dsatact_num_clcc_calls = 0;
    call_da_val->dsat_abort_call_info_listing = FALSE;
    /* Abort handler responsible for sending result code */
    return DSAT_ASYNC_EVENT;
  }

  if ((result == DSAT_ASYNC_CMD) && (call_da_val->dsatact_num_clcc_calls > 0))
  {
    /* If more call ids are present, then get Call id information from CM */
    cm_result = cm_mm_call_cmd_get_call_info(
                   dsatcmif_call_cmd_cb_func, 
                   NULL, 
                   dsatcm_client_id, 
                   call_da_val->dsatcmif_clcc_no_call_ids[call_da_val->dsatact_num_clcc_calls - 1].call_id);
    if (!cm_result)
    {
      call_da_val->dsatact_num_clcc_calls = 0;
      DS_AT_MSG0_HIGH("Failed to query CM call info");
      result = dsat_send_cme_error(DSAT_CME_UNKNOWN);
    }

  }
    
  return result;
    
}/* dsatact_call_info_clcc_cmd_handler */

/*===========================================================================

FUNCTION  DSATACT_REPORT_CLCC

DESCRIPTION
  This function displays the required details of given Call id
  
DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ASYNC_CMD: If more call ids whose information need to be 
                    retrieved from CM are there
    DSAT_ERROR :    if there was any problem in executing the command
    DSAT_OK :       if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatact_report_clcc
(
  sys_modem_as_id_e_type  subs_id,
  ds_at_call_info_s_type *call_info,
  cm_call_substate_type   call_sub_state
)
{
  dsat_result_enum_type     result = DSAT_ASYNC_CMD;
  dsat_clcc_err_code_e_type err_code = DSAT_CLCC_ERR_NONE;
  dsm_item_type            *res_buff_ptr;
  static dsat_rsp_enum_type type_of_rsp = DSAT_START_OF_RSP;
  int    seq_num;
  byte num_type;
  dsatetsicall_call_state_da_info  *call_da_val = NULL;
  dsatme_mmgsdi_state_ss_info      *me_ss_val = NULL;

  me_ss_val = dsat_get_base_addr(DSAT_MMGSDI_SS_VALS, FALSE);
  call_da_val = dsat_get_base_addr_per_subs(DSAT_CALL_DA_VALS, subs_id, FALSE);
  
  if ((call_da_val->dsatact_num_clcc_calls > 0) && 
    (call_da_val->dsat_abort_call_info_listing == FALSE))
  {
    uint8  dir;
    uint8  state = (uint8)DSAT_CLCC_STATE_MAX;
    uint8  mode;
    uint8  mpty = 0;

    res_buff_ptr = dsat_dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL, FALSE);

    seq_num = dsatact_callid_to_seqnum(subs_id, call_info->call_id);
    if (seq_num == CALL_SEQNUM_INVALID)
    {
      call_da_val->dsatact_num_clcc_calls = 0;
      err_code = DSAT_CLCC_ERR_SEQ_NUM;
      goto report_clcc_cme_unknown;
    }

    if (call_info->call_direction == CM_CALL_DIRECTION_MO)
    {
      dir = (uint8)DSAT_CLCC_DIRECTION_MO;
    }
    else if (call_info->call_direction == CM_CALL_DIRECTION_MT)
    {
      dir = (uint8)DSAT_CLCC_DIRECTION_MT;
    }
    else if(call_info->call_type == CM_CALL_TYPE_PS_DATA)
    {
      dir = (uint8)DSAT_CLCC_DIRECTION_MO;
    }
    else
    {
      call_da_val->dsatact_num_clcc_calls = 0;
      err_code = DSAT_CLCC_ERR_CALL_DIRECTION;
      goto report_clcc_cme_unknown;
    }

    switch (call_info->call_state)
    {
      case CM_CALL_STATE_INCOM:
        if (call_da_val->dsatact_clcc_mpty_calls > 0) 
        {
          /* At least one call is in conversation */
          state = (uint8)DSAT_CLCC_STATE_WAITING;
        }
        else
        {
          state = (uint8)DSAT_CLCC_STATE_INCOMING;
        }
        break;
            
      case CM_CALL_STATE_ORIG:
        if (dsat_mo_dailing_state_get(subs_id) == TRUE)
        {
          state = (uint8)DSAT_CLCC_STATE_DIALING;
        }
        else
        {
          state = (uint8)DSAT_CLCC_STATE_ALERTING;
        }
        break;
            
      case   CM_CALL_STATE_CONV:
        if (call_sub_state.conv == CM_CALL_CONV_CALL_ACTIVE)
        {
          state = (uint8)DSAT_CLCC_STATE_ACTIVE;
        }
        else if (call_sub_state.conv == CM_CALL_CONV_CALL_ON_HOLD)
        {
          state = (uint8)DSAT_CLCC_STATE_HELD;
        }
        else
        {
          if ((call_info->sys_mode == SYS_SYS_MODE_CDMA) || (call_info->sys_mode == SYS_SYS_MODE_HDR))
          {
            /* Assume default the state is active in CONVSERSATION */
            state = (uint8)DSAT_CLCC_STATE_ACTIVE;      
          }
          else
          {
            call_da_val->dsatact_num_clcc_calls = 0;
            err_code = DSAT_CLCC_ERR_CONV_STATE;
            goto report_clcc_cme_unknown;
          }
        }
        
        if (call_da_val->dsatact_clcc_mpty_calls > 1)
        {
          mpty = DSAT_CLCC_MPTY_CALL;
        }
        break;
            
      default:
        if( call_info->call_type == CM_CALL_TYPE_VOICE)
        {
          call_da_val->dsatact_num_clcc_calls = 0;
          err_code = DSAT_CLCC_ERR_CALL_STATE;
          goto report_clcc_cme_unknown;
        }
        else
        {
          call_da_val->dsatact_num_clcc_calls--;
          DS_AT_MSG1_HIGH("Invalid state Ignore data call ,Calls : %d",call_da_val->dsatact_num_clcc_calls);
          if (call_da_val->dsatact_num_clcc_calls == 0)
          {
            if (type_of_rsp != DSAT_START_OF_RSP)
            {
              dsatcmdp_handle_async_cmd_rsp(res_buff_ptr, DSAT_INTERMEDIATE_RSP);
            }
            result = DSAT_OK;
          }
          return result;
       }
    }
        
    if (call_info->call_type == CM_CALL_TYPE_VOICE)
    {
      mode = (uint8)DSAT_CLCC_MODE_VOICE;
    }
    else if (call_info->call_type == CM_CALL_TYPE_CS_DATA)
    {
#ifdef FEATURE_DATA_IS707
      if ((call_info->sys_mode == SYS_SYS_MODE_CDMA) || (call_info->sys_mode == SYS_SYS_MODE_HDR))
      {
        DS_AT_MSG1_MED("service opt %d", call_info->dsat_mode_info.info.cdma_call.srv_opt);
        if (call_info->dsat_mode_info.info.cdma_call.srv_opt == CAI_SO_G3_FAX_IS707 ||
        call_info->dsat_mode_info.info.cdma_call.srv_opt == CAI_SO_ANALOG_FAX_RS1 ||
        call_info->dsat_mode_info.info.cdma_call.srv_opt == CAI_SO_ANALOG_FAX_RS2 ||
        call_info->dsat_mode_info.info.cdma_call.srv_opt == CAI_SO_G3_FAX_PRE707 ||
        call_info->dsat_mode_info.info.cdma_call.srv_opt == CAI_SO_G3_FAX_13K_PRE707 )
        {
          mode = (uint8)DSAT_CLCC_MODE_FAX;
        }
        else 
        {
          mode = (uint8)DSAT_CLCC_MODE_DATA;
        }
   	  }
	  else
#endif /* FEATURE_DATA_IS707 */
	  {
        if (call_info->dsat_mode_info.info.gw_cs_call.bearer_capability_1.\
              information_transfer_capability == ITC_FAX_G3)
        {
          mode = (uint8)DSAT_CLCC_MODE_FAX;
        }
        else if ((call_info->dsat_mode_info.info.gw_cs_call.bearer_capability_1.\
                  information_transfer_capability !=  ITC_SPEECH) && 
                  (call_info->dsat_mode_info.info.gw_cs_call.bearer_capability_1.\
                  information_transfer_capability != ITC_AUX_SPEECH))
        {
          mode = (uint8)DSAT_CLCC_MODE_DATA;
        }
        else
        {
          call_da_val->dsatact_num_clcc_calls = 0;
          err_code = DSAT_CLCC_ERR_CALL_MODE;
          goto report_clcc_cme_unknown;
        }
      }
      
    }
    else if (call_info->call_type == CM_CALL_TYPE_PS_DATA)
    {
      mode = (uint8)DSAT_CLCC_MODE_DATA;
    }
    else
    {
      call_da_val->dsatact_num_clcc_calls = 0;
      err_code = DSAT_CLCC_ERR_CALL_TYPE;
      goto report_clcc_cme_unknown;
    }

    res_buff_ptr->used += (word)snprintf(
                          (char*)res_buff_ptr->data_ptr + res_buff_ptr->used, 
                          res_buff_ptr->size - res_buff_ptr->used, 
                          "+CLCC: %d,%d,%d,%d,%d,", seq_num, dir, 
                          state, mode, mpty);

    if (call_info->num.buf[0]  == DSAT_TA_INTER_PREFIX)
    {
      num_type = DSAT_TA_INTERNATIONAL;
    }
    else
    {
      /* Type bits - bit7 = 1, bits 6-4 = number type, bits 3-0 = number plan */
      num_type = 0x80 | ((call_info->num.number_type & 0x07) << 4) | 
                   (call_info->num.number_plan & 0x0F); 
    }
    
    res_buff_ptr->used += (word)snprintf(
                        (char*)&res_buff_ptr->data_ptr[res_buff_ptr->used],
                        res_buff_ptr->size - res_buff_ptr->used,"\"");
        
    /* dsat_incom_call_num.buf is not null terminated */
    (void) dsatutil_memscpy((void*)&res_buff_ptr->data_ptr[res_buff_ptr->used],
                                    DSAT_BUF_SIZE(res_buff_ptr->size,res_buff_ptr->used),
                             (void*)call_info->num.buf,call_info->num.len);
    
    res_buff_ptr->used += call_info->num.len;

    res_buff_ptr->used += (word)snprintf(
                          (char*)&res_buff_ptr->data_ptr[res_buff_ptr->used],
                          res_buff_ptr->size - res_buff_ptr->used, "\"");
                                     
    /* Number type */
    res_buff_ptr->used += (word)snprintf(
                            (char*)&res_buff_ptr->data_ptr[res_buff_ptr->used],
                            res_buff_ptr->size - res_buff_ptr->used,
                            ",%d",num_type);

#ifdef FEATURE_ETSI_PBM
    if (dsatetsime_get_alphanumeric_name(call_info->num.buf, 
        MIN(CM_MAX_NUMBER_CHARS,call_info->num.len)) == PBM_SUCCESS)
    {
      res_buff_ptr->used += (word)snprintf(
                            (char*)&res_buff_ptr->data_ptr[res_buff_ptr->used],
                            res_buff_ptr->size - res_buff_ptr->used,
                            ",\"%s\"",me_ss_val->dsat_alpha_text);
    }
#endif /* FEATURE_ETSI_PBM */

    call_da_val->dsatact_num_clcc_calls--;

    if (call_da_val->dsat_abort_call_info_listing == TRUE)
    {
      /* user aborted the command */
      call_da_val->dsat_abort_call_info_listing = FALSE;
      call_da_val->dsatact_num_clcc_calls = 0;
      type_of_rsp = DSAT_START_OF_RSP;

      /* Abort handler responsible for sending result code */
      return DSAT_ASYNC_EVENT;
    }

    if (call_da_val->dsatact_num_clcc_calls == 0)
    {
      if (type_of_rsp == DSAT_START_OF_RSP)
      {
        type_of_rsp = DSAT_COMPLETE_RSP;
      }
      else
      {
        type_of_rsp = DSAT_END_OF_RSP;
      }

      dsatcmdp_handle_async_cmd_rsp(res_buff_ptr, type_of_rsp);
      result = DSAT_OK;
      type_of_rsp = DSAT_START_OF_RSP;
    }
    else
    {
      res_buff_ptr->used += (word)snprintf(
                            (char*)&res_buff_ptr->data_ptr[res_buff_ptr->used],
                            res_buff_ptr->size - res_buff_ptr->used,
                             "\n");

      dsatcmdp_handle_async_cmd_rsp(res_buff_ptr, type_of_rsp);
      if (type_of_rsp == DSAT_START_OF_RSP)
      {
        type_of_rsp = DSAT_INTERMEDIATE_RSP;
      }
    }

  }
  else
  {
    type_of_rsp = DSAT_START_OF_RSP;
  }

  return result;

report_clcc_cme_unknown:
  DS_AT_MSG1_ERROR("CLCC error code = %d", err_code);
  dsatme_set_cme_error(DSAT_CME_UNKNOWN, res_buff_ptr);
  dsatcmdp_handle_async_cmd_rsp (res_buff_ptr, DSAT_COMPLETE_RSP);
  return DSAT_CMD_ERR_RSP;
} /* dsatact_report_clcc */

/*===========================================================================

FUNCTION DSATACT_CALLID_TO_SEQNUM

DESCRIPTION
  Given a call ID it returns the associated sequence number as described in 
  3GPP TS 22.030 section 6.5.5.1.

DEPENDENCIES
  None
  
RETURN VALUE
  Sequence number on success or
  CALL_SEQNUM_INVALID on out-of-range call ID or failure to find.

SIDE EFFECTS
  None
  
===========================================================================*/
LOCAL  int dsatact_callid_to_seqnum
(
  sys_modem_as_id_e_type  subs_id,
  cm_call_id_type         call_id
)
{
  int sequence_number = 0;
  dsatetsicall_call_state_da_info  *call_da_val = NULL;

  call_da_val = dsat_get_base_addr_per_subs(DSAT_CALL_DA_VALS, subs_id, FALSE);

  if ( call_id <= CM_CALL_ID_MAX )
  {
    while ((sequence_number < CM_CALL_ID_MAX) &&
      (call_da_val->dsat_seqnum_callid[sequence_number].call_id != call_id))
    {
      sequence_number++;
    }
  
    if (sequence_number != CM_CALL_ID_MAX)
    {
      return(sequence_number + 1);
    }
  }

  return(CALL_SEQNUM_INVALID);
  
}/* dsatact_callid_to_seqnum */

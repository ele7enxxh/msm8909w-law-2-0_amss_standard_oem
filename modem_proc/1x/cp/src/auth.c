/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


          M A I N    A U T H E N T I C A T I O N    T A S K

GENERAL DESCRIPTION
  This module performs all authentication functions required by IS-95A and is
  also the server for the R-UIM card ( Removable User Identity Module) when
  FEATURE_UIM_RUIM is defined. In the presence of FEATURE_UIM_RUIM, the Authentication
  is performed by the RUIM card and this task acts as the server for the RUIM.

EXTERNALIZED FUNCTIONS
  auth_validate_a_key
    Determines if a given A-key is valid.

  auth_private_lcm_available
    Tells whether or not a private long code mask is available.

  auth_get_count
    Returns the current value of COUNTsp.

  auth_calculate_digits
    Packs the origination or sms destination digits .

  auth_encryption_possible
    Tells if message encryption is possible at the current time.

  auth_encrypt_cdma
    Encrypts a CDMA message to be transmitted on the traffic channel.

  auth_decrypt_cdma
    Decrypts a CDMA message received on the traffic channel.

  auth_encrypt_analog
    Encrypts an analog message to be transmitted on the voice channel.

  auth_decrypt_analog
    Decrypts an analog message received on the voice channel.

  auth_cmd
    External interface to send a command to the authentication task.

  auth_task
    This is the entrance procedure for the Authentication/ RUIM  task.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  The AUTH_INIT_F command must be sent before successful authentication
  can be performed.

  Copyright (c) 1995 - 2014 Qualcomm Technologies, Inc. 
  All Rights Reserved Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/auth.c_v   1.12   03 Sep 2002 22:10:42   azafer  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/src/auth.c#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/29/14   agh     Replace card_reader_status_cnf with status_cnf
05/06/14   gga     1X_CP F3 redistribution
02/21/14   ssh     Workaround to correct the PLCM to overcome a compiler bug
01/08/14   agh     FR 15489: UIM Clients to Handle SIM BUSY Indications
08/29/13   ppr     Added break stmt after MMGSDI_READ_CNF switch case
07/23/13   srk     Use TCB_PTR MACRO instead of rex_self() or tcb_ptr to make
                   the code compatible with RCINIT.
07/12/13   ppr     Changes to do deep copy of MMGSDI report
07/19/13   fg      Reorganize the nv read/write functions
03/05/13   srk     Replace existing watchdog implementation with the new
                   Dog Heartbeat Mechanism.
03/05/13   bb      Set auth_cnf to TRUE for default cases in auth_mmgsdi_cb()
02/27/13   nee     Set auth_cnf=FALSE in auth_mmgsdi_cb() for memcpy error case.
01/03/13   nee     Replacement of memcpy() to memscpy() Phase 2
10/19/12   ppr     Added defensive checks
09/27/12   jtm     Replace auth_tcb with AUTH_TCB_PTR.
09/25/12   ppr     Added defencive checks for NULL ptr access
09/15/12   ppr     FR#2370 - 1x memory optimization changes
03/07/12   srk     Replaced MODEM_MSG_LEGACY_STARTED with RCINIT_RCEVT_INITGROUPS.
02/13/12   srk     Moved init fucntions after task start.
02/04/12   ppr     Feature Cleanup
01/17/12   ppr     Feature Cleanup: Mainlined Always ON features
01/06/12   ppr     1X CP - Feature clean up: Mainline Always On features,
                   Remove Always Off Features
12/20/11   srk     Added RCInit Framework.
12/13/11   srk     Feature clean up: Mainlined FEATURE_1X_WDOG_DYNAMIC
07/13/11   jj      Fix to send auth report to MC even if Auth_signature
                   fails.
02/24/11   ssh     Removed the inclusion of unused header files.
01/27/11   azh     Lint and KW fixes.
01/13/10   ssh     Included the modem_1x_defs.h file explicitly for dynamic
                   watchdog support.
01/04/11   ag      Fixed compiler and lint warnings.
12/17/10   ssh     Changed the watchdog timout value for AUTH task to
                   DOG_DEFAULT_TIMEOUT.
11/15/10   jtm     Initial SU API split for CP.
11/02/10   ag      Fix to initialize WDOG related variables after task starts.
10/03/10   jj      CSIM: Added changes to access CALL_COUNT with a corresponding
                   record accessing MMGSDI API.
09/30/10   ag      Changed the scope of subscription check to only accessing
                   of NV/card.
09/15/10   ag      Added support for dynamic DOG.
09/13/10   ag      Fix to prevent reading call count when phone subscription
                   is not available.
08/19/10   jj      CSIM: Added changes to update CAVE VPM keys correctly.
06/24/10   ssh     Added EFS sync support.
05/12/10   jj      CSIM P2 changes: updated to read/write call count.
03/04/10   jj      Replaced deprecated task_xxx APIs with tmc_xxx APIs.
02/15/10   jj      CSIM P2: Fixed a merging issue.
02/11/10   ag      Fix to read NV_COUNT_I only when subscription is available.
01/12/10   jtm     Lint Fixes.
01/10/10   jj      CSIM P2 Changes - Replaced UIM command based interface
                   with MMGSDI API based interfaces.
09/10/09   pk      Fixing a possible message buffer overflow error in
                   auth_encrypt_info_recs and auth_encrypt_flash
08/30/09   jj      Added include of uim_v.h as part of CMI.
08/21/09   jtm     Lint fixes.
07/14/07   jj      Fixed a link error because of not including err.h incase
                   FEATURE_AUTH is not enabled.
07/03/07   jj      CMI Phase-I update
05/03/09   jj      CMI Phase-II update.
03/26/09   adw     Added ulpn.h include for ulpn_type.
03/04/09   adw     Added auth_v.h include.
06/07/07   pg      Lint clean up.
05/14/07   pg      Use ESN ME for RUN CAVE and other authentication commands
                   regardless of UIM ID usage indicator provided by card
                   under FEATURE_UIM_JCDMA_RUIM_SUPPORT.
04/26/06   an      Fixed Lint Errors
01/19/06   fc      Merged the following :
           fh      Added AKA and MI support.
11/30/05   va      Added new function auth_send_update_a_key_cmd  which is
                   for calling from SVCPRG module.
11/30/05   an      Mainlined FEATURE_IS2000
10/10/05   fc      Removed including of enc.h
05/03/05   fc      Mainlined FEATURE_IS2000.
12/13/04   az      Fixed a variable name
12/10/04   sb      Fixed lint issue.
12/05/04   az      Submission done for Wes Li. This is for CR 53350 where
                   a word is written to a pointer pointed to a byte.
10/20/04   an      Replaced inclusion of srch.h with srchmc.h
08/17/04   yll     For burst type 0x3e and 0x3f of a data burst message, only
                   encrypt the CHARi after the first two bytes.
03/19/04   sb      Removed auth_switch_to_private_lcm.
03/09/04   ph      Change slot to AUTOMATIC in command to UIM.
06/20/03   bkm     Clear T64 timer when SSD Update is received to prevent
                   timer expiration from aborting a second SSD Update attempt
                   after an Idle Handoff.
01/28/03   ts      Make sure UIM can power off whether RTRE control is NV or
                   UIM.
09/03/02   ts      Added support for Run Time R-UIM Enable.
08/26/02   ck      Featurised the processing of CAI_EFWIM_MSG around the feature
                   FEATURE_IS2000_REL_A.
07/25/02   jqi     Updated with task operation interface.
04/12/02   lh      Added support for subaddress in Orig_C.
03/12/02   lh      Added CMEA support for release A messages.
08/09/01   sb      Merged following from common archive:
                   07/27/01   ts      Allow UIM to power off after initializing
                                      (reading call count).
05/25/01   ts      Removed RUIM server code from auth and added interface to
                   new UIM server.
04/12/01   fc      Renamed mcc_report_event_timer to mclog_report_event_timer.
03/12/01   fas     Backed out call to MSG_LOW.
03/09/01   fas     Added includes for comdef.h and customer.h.
                   Added call to MSG_LOW to inhibit compiler warning.
03/08/01   fc      Fixed timer ID of T64m and moved "#include mccdma,h"
                   oustide feature #ifdef to support events report.
03/06/01   fc      Added support of events report: Timer expired.
02/16/01   ck      Fixed the issue where 8 digits of CHV was incorrectly
                   formatted by the CHV formatting routine.
02/07/01   wli     Split RUIM_UPDATE_RECORD_ST to two states:
                   RUIM_UPDATE_LINEAR_FIXED_RECORD_ST and
                   RUIM_UPDATE_CYCLIC_RECORD_ST.
1/27/01    wli     Added new RUIM commands, INVALIDATE and REHABILITATE.
01/11/01   ts      Added a call to flush the nvruim cache when resetting the
                   RUIM.
12/18/00   ck      Fixed the problem where the bytes of the request offset
                   in the SSPR Config request were swapped causing the R-UIM
                   to reject the request with 'Data Size Mismatch'.
11/28/00   ck      Added support for IS-683A with the R-UIM.
                   Added support for STORE_ESN_ME and STATUS command.
                   Added the debug feature for Unblocking the CHV.
                   Added paths for new EFs in the table.
                   Fixed the issue with zeroing 3 extra bytes in the RUN_CAVE.
                   Added a different time-out for the Commit command.
                   Changed the RAND_SEED_F command to BS_CHAL_F.
                   Fixed the problem where an external command that got xlated
                   to an internal command caused a buffer leak.
11/14/00   ts      Added access to the MSISDN in the CDMA directory.  Added
                   support for a faster RUIM clock frequency.  Fixed the APDU
                   for the status command.
10/11/00   ts      Include the RUIM status with auth reports.
07/11/00   ms      Added functionality to return just the size of an EF in records,
07/16/00   ck      Added support to select the CDMA DF before sending any
                   authentication related commands to the card.
                   Fixed the bug where the disabled CHV was causing the phone
                   to go offline.
06/13/00   ck      Added a debug feature to not do PPS since the cards do not do
                   PPS and the ATR tells us that they do.
06/12/00   ck      Added functionality to parse the ATR to determine if PPS can
                   be sent.
                   Added the functionality of reading and writing record from
                   RUIM.
06/06/00   ck      Cleaned up the warning caused by a bad merge for the prev fix.
06/06/00   ck      Fixed the problem where NEW_CHV command followed by any
                   command before powering down the RUIM rsulted in the cmd
                   being ignored.
06/01/00   ck      Added debug feature for ESN in the command RUN CAVE to flip
                   the byte ordering and changed the polling of RUIM based on
                   the status of the RUIM.
05/30/00   ck      Changed instances of RUIM_UI_CHV_SIG to RUIM_CHV_SIG.
05/01/00   ck      Added support for RUIM software. AUTH code has been
                   featurised such that the basic task functionality is
                   obtained by defining either FEATURE_AUTH or FEATURE_RUIM.
                   Authentication functionality performed by CAVE is defined
                   under FEATURE_AUTH and is ifdefed out when FEATURE_RUIM is
                   defined.
                   Various RUIM functions have been added.
04/06/00   va      Merged the following from MSM3100_CP.03.00.17
           ry      Added OTAPA support
08/13/99   jq      Changed the return mechanism of auth_get_nv_item() so the
                   function also returns.
06/17/99   ck      Fixed the problem where the CMEA and VPM keys were computed
                   based on the old SSD during re-authentication.
05/06/99   kmp     Changed T_ACP and T_AUTH to FEATURE_ACP and FEATURE_AUTH
                   respectively.
03/18/99   ck      Merged in the changes for A-Key exchange and
                   Re-Authentication.
09/15/98   pms     Backed out _near pointers
09/04/98   pms     Neared the variables to save ROM space.
07/28/98   dna     Retain message encryption on CDMA->AMPS handoff by
                   separating auth_reset from auth_resync.
06/21/96   dna     Removed AUTH_BAD_SSD_S status.  Always consider SSD valid.
05/15/96   dna     CMEA key generation status now set ASAP.
11/17/95   dna     Report back when keys are generated.
11/16/95   jca     Removed INTLOCK/INTFREE pair in function auth_cmd().
10/05/95   dna     re-initialize after A_Key update
08/03/95   dna     Added support for message encryption
06/07/95   dna     Created module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/*lint -e766*/
#include "1x_variation.h"
#include "comdef.h"
#include "target.h"
#include "customer.h"
#include "modem_1x_utils.h"
#include "modem_1x_defs.h"

#include "msg.h"
#include "m1x_diag.h"
#include "auth.h"
#include "err.h"
#include "rex.h"
#include "task.h"
#include "dog.h"
#include "dog_hb_rex.h"
#include "onex_nv_rd_wt.h"
#include "queue.h"
#include "rcinit.h"

#if defined (FEATURE_RUIM) || defined (FEATURE_CSIM)
#include "mc_v.h"
#include "mccdma_v.h"
#include "mccdma_i.h"
#include "cai_v.h"
#include "auth_v.h"
#include "auth_i.h"

#include "authi.h"
#include "cave.h"

#include "srchmc.h"
#include "bit.h"

#include "memory.h"

#ifdef FEATURE_UIM_RUIM
#include "uim.h"
#include "uim_v.h"
#include "nvruimi.h"
#include "otaspe.h"
#endif /* FEATURE_UIM_RUIM */

#include "mclog_v.h"
#include "ulpn.h"
#include "fs_public.h"
/*lint -e818 */

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* Create TCB pointer */
rex_tcb_type* auth_tcb_ptr = NULL;


#define AUTH_THREE_BITS  3
#define AUTH_FOUR_BITS   4
#define AUTH_FIVE_BITS   5
#define AUTH_EIGHT_BITS  8
#define AUTH_TWO_BYTES   2

/* Suppress the following lint messages:
   e413: Likely use of null pointer in left argument to operator '->',
         which is heavily used in FPOS and FSIZ macros.
   e734:  Loss of precision (assignment).
*/
/*lint -e734 -e413 */

#define AUTH_RECORD_TYPE_SIZE FSIZ( cai_rec_hdr_type, record_type )
#define AUTH_RECORD_LEN_SIZE  FSIZ( cai_rec_hdr_type, record_len )

/* Buffer to hold data to be encrypted */
byte auth_temp_buf[CAI_REV_TC_MAX_MSG_BYTES];

/* Buffers to be placed on the auth_free_q */
#define                      AUTH_NUM_CMD_BUFS 3

auth_cmd_type auth_cmd_pool[ AUTH_NUM_CMD_BUFS ];

/* Queue to hold commands for auth */
q_type          auth_cmd_q;

/* Queue to hold free buffers to be placed on the auth_cmd_q */
q_type auth_free_q;

/* Buffer to hold report to be returned to task issuing command */
auth_rpt_type auth_rpt_buf;

/* DOG report ID for AUTH task */
dog_report_type auth_dog_rpt_id = 0;

rex_timer_type auth_ssd_timer;  /* SSD timeout timer */

/* Boolean to indicate if we are in the middle of performing SSD update */
LOCAL boolean auth_ssd_update_in_progress = FALSE;

/* Indicates if CMEA key has been generated */
boolean auth_valid_CMEA_key = FALSE;

/* Indicates if Private Long Code Mask has been generated */
boolean auth_valid_PLCM = FALSE;

/* Indicates if auth_signature() has been told to save_registers */
boolean auth_saved_registers = FALSE;

/* Status of auth initialization */
auth_init_status_type auth_init_status = AUTH_UNINITIALIZED;

/* COUNTsp */
byte auth_count = 0;

/* NAM index used in last auth init, used for updating auth_count. */
byte auth_stored_NAM_index;

/* A_KEY_TEMP stores the newly generated A-Key  */
qword a_key_temp;

/* Indicates if a new A_KEY_TEMP has been generated */
boolean a_key_temp_valid = FALSE;

/* Indicates if new SSD has been generated */
boolean new_ssd_valid = FALSE;

/* Indicates that the new ssd has been used for generating the AUTHR in the
   Re-Authentication Response. This flag is primarily used to choose the
   correct SSD during key generation(CMEA and VPM). We cannot use new_ssd_valid
   as there is a chance that we might need to use the old SSD when new SSD is
   valid( If for some reason, encryption is turned on after SSD update and
   before Re-Authentication) */
LOCAL boolean used_new_ssd = FALSE;



#if defined(FEATURE_CSIM)
mmgsdi_return_enum_type auth_mmgsdi_rpt_status;
#ifdef FEATURE_1X_CP_MEM_OPT
mmgsdi_cnf_type  *auth_mmgsdi_rpt_ptr = NULL;
#else /* !FEATURE_1X_CP_MEM_OPT */
mmgsdi_cnf_type  auth_mmgsdi_rpt;
mmgsdi_cnf_type  *auth_mmgsdi_rpt_ptr = &auth_mmgsdi_rpt;
#endif /* FEATURE_1X_CP_MEM_OPT */
byte auth_mmgsdi_data[CAVE_VPM_SIZE];
boolean auth_cnf;

/* CallBack Function */
void auth_mmgsdi_cb
(
  mmgsdi_return_enum_type  status,
                     mmgsdi_cnf_enum_type     cnf,
                     const mmgsdi_cnf_type    *cnf_ptr
                    );

#elif defined(FEATURE_UIM_RUIM)
/* Response buffer */
#ifdef FEATURE_1X_CP_MEM_OPT
uim_rpt_type *auth_uim_rsp_buf_ptr = NULL;
#else /* !FEATURE_1X_CP_MEM_OPT */
uim_rpt_type auth_uim_rsp_buf;
uim_rpt_type *auth_uim_rsp_buf_ptr = &auth_uim_rsp_buf;
#endif /* FEATURE_1X_CP_MEM_OPT */

#endif /* FEATURE_UIM_RUIM */

#if defined(FEATURE_CSIM) || defined(FEATURE_UIM_RUIM)

/* Temporary COUNTsp */
word temp_auth_count = 0;


ulpn_type auth_plcm;
/* plcm returned by RUIM */

dword auth_randbs;
/* Randbs returned by RUIM */


/* Constants for origination message translation */

#define AUTH_DIAL_0                0xA
#define AUTH_DIAL_STAR             0xB
#define AUTH_DIAL_PND              0xC
#define AUTH_DIAL_NULL             0x0

/*===========================================================================

            FUNCTION PROTOTYPES

This section contains local function prototypes for the RUIM interface code.

===========================================================================*/
LOCAL rex_sigs_type  auth_wait
(
  /* Mask of signals to wait for */
  rex_sigs_type sigs
);

LOCAL void auth_uim_response_callback ( uim_rpt_type *uim_rpt_buf_ptr );
#endif /* FEATURE_CSIM || FEATURE_UIM_RUIM */

/*===========================================================================

FUNCTION AUTH_DOG_REPORT

DESCRIPTION
  This function sends report to Dog heartbeat mechanism and clears the
  dog report signal.

DEPENDENCIES
  Depends on previously initialized DOG report ID.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void auth_dog_report( void )
{
  dog_hb_report(auth_dog_rpt_id);
  (void) rex_clr_sigs( AUTH_TCB_PTR, AUTH_DOG_RPT_SIG );

} /* auth_dog_report */


/*===========================================================================

FUNCTION AUTH_GET_NV_ITEM

DESCRIPTION
  This function gets a specific item from NV.

DEPENDENCIES
  None.

RETURN VALUE
  Status returned from the NV read request.  Status can be either:
    NV_DONE_S       - request done
    NV_NOTACTIVE_S  - item was not active
  Any other status causes ERR_FATAL and the function does not return.

SIDE EFFECTS
  While this function is running all other task activities are
  suspended except for watchdog kicking, and until the NV item is
  read in.  If any of read status other then the one described above
  is returned then an ERR_FATAL call is made.

===========================================================================*/

nv_stat_enum_type auth_get_nv_item
(
  nv_items_enum_type  item_code,       /* Item to get */
  nv_item_type        *item_ptr        /* Pointer where to put the item */
)
{
  static nv_cmd_type  nv_cmd_buf;
    /* Command buffer to NV */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!onex_nv_read_write(item_code, item_ptr, NV_RD | NV_AUTH, &rex_wait,
                          &nv_cmd_buf))
  {
    ERR_FATAL("Bad NV read status %d", (word) nv_cmd_buf.status, 0, 0);
  }
  return (nv_cmd_buf.status);
} /* auth_get_nv_item */

/*===========================================================================

FUNCTION AUTH_PUT_NV_ITEM

DESCRIPTION
  This function puts a specific item to NV.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  While this function is running all other task activities are
  suspended except for watchdog kicking, and until the NV item is
  written.

===========================================================================*/

void auth_put_nv_item
(
  nv_items_enum_type  item_code,       /* Item to put */
  nv_item_type        *item_ptr        /* Pointer where to get the item */
)
{

  static nv_cmd_type  nv_cmd_buf;   /* Command buffer to NV */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  onex_nv_read_write(item_code, item_ptr, NV_WRT | NV_AUTH,
                     &rex_wait, &nv_cmd_buf);

  if (nv_cmd_buf.status != NV_DONE_S)
  {
    ERR_FATAL("Failed to write item to NV %d", (int) item_code, 0, 0);
  }
} /* auth_put_nv_item */

/*===========================================================================

FUNCTION AUTH_VAR_INIT

DESCRIPTION
  This procedure initializes the authentication task variables after a
  powerup, NAM change, or A_Key change.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

LOCAL void auth_var_init
(
  byte nam
    /* The nam active during this initialization */
)
{
  nv_item_type nv_item;
    /* Item to hold values retrieved from NV */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize boolean indicators */
  auth_saved_registers = FALSE;
  auth_ssd_update_in_progress = FALSE;
  auth_valid_CMEA_key = FALSE;
  auth_valid_PLCM = FALSE;

  /* Store NAM index */
  auth_stored_NAM_index = nam;

  /* Get value of COUNTsp from NV */
  if(cdma.is_cdma_subs_avail)
  {
    nv_item.count.nam = auth_stored_NAM_index;
    if (auth_get_nv_item( NV_COUNT_I, &nv_item ) == NV_NOTACTIVE_S)
    {
      /* Write a zero in for the COUNTsp if no count has ever been used */
      nv_item.count.nam = auth_stored_NAM_index;
      nv_item.count.count = 0;
      auth_put_nv_item( NV_COUNT_I, &nv_item );
      auth_count = 0;
    }
    else
    {
      auth_count = nv_item.count.count;
    }
  }
  else
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "Subscription not available. Call Count can not be read");
    auth_count = 0;
  }
} /* auth_var_init() */


#if !defined(FEATURE_CSIM) && defined (FEATURE_UIM_RUIM)
/*===========================================================================

FUNCTION AUTH_SEND_UIM_COMMAND

DESCRIPTION
  This function sends a UIM command top the UIM server.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  This function waits for the response before returning to the calling
  function.

===========================================================================*/
void auth_send_uim_command
(
  uim_cmd_type *uim_cmd_ptr
)
{
  /* Indicate command completion is to be signaled:
   *    Do not signal the Auth task upon receipt of command
   *    Use no signal for receipt of command
   *    No "done" queue
   *    Status call-back function
   *    Always Report status
   */

  uim_cmd_ptr->hdr.cmd_hdr.task_ptr = NULL;
  uim_cmd_ptr->hdr.cmd_hdr.sigs = 0;
  uim_cmd_ptr->hdr.cmd_hdr.done_q_ptr = &uim_free_q;
  uim_cmd_ptr->hdr.options = UIM_OPTION_ALWAYS_RPT;
  uim_cmd_ptr->hdr.protocol = UIM_CDMA;
  uim_cmd_ptr->hdr.slot = UIM_SLOT_AUTOMATIC;
  uim_cmd_ptr->hdr.rpt_function = auth_uim_response_callback;

  /* Send the command to the UIM server:
   *    Clear the "command done signal"
   *    Send the command
   *    Wait for the command to be done
   */
  (void) rex_clr_sigs( AUTH_TCB_PTR, AUTH_UIM_CMD_RSP_SIG );

  /* Send the command to the UIM server. */
  uim_cmd ( uim_cmd_ptr );
  /* Wait for the response.  Process the watchdog, stop, and offline
     signals. */
  (void) auth_wait( AUTH_UIM_CMD_RSP_SIG );

} /* auth_send_uim_command */

/*===========================================================================

FUNCTION AUTH_UIM_RESPONSE_CALLBACK

DESCRIPTION
  This procedure initiates the processing of the response that has been recd
  from the RUIM.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Sets the RUIM_CMD_RSP_SIG.

===========================================================================*/

LOCAL void auth_uim_response_callback ( uim_rpt_type *uim_rpt_buf_ptr )
{
  /* Variable to store value of memscpy() */
  size_t copy_size;
  /* Copy the report into the local buffer. */
  if( (auth_uim_rsp_buf_ptr != NULL) && (uim_rpt_buf_ptr != NULL) )
  {
    copy_size = memscpy(auth_uim_rsp_buf_ptr, sizeof(uim_rpt_type),
                        uim_rpt_buf_ptr, sizeof(uim_rpt_type));
    if( copy_size != sizeof(uim_rpt_type) )
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
        copy_size,
        sizeof(uim_rpt_type),
        sizeof(uim_rpt_type));
    }
  }
  else
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "NULL pointer, auth_uim_rsp_buf_ptr:0x%x, uim_rpt_buf_ptr:0x%x",
      auth_uim_rsp_buf_ptr,
      uim_rpt_buf_ptr);
  }

  /* set the command response signal */
  M1X_MSG( DCP, LEGACY_LOW,
    " Recd Command Response Signal");
  (void) rex_set_sigs(AUTH_TCB_PTR, AUTH_UIM_CMD_RSP_SIG);

} /*  auth_uim_response_callback */

#endif /* !defined(FEATURE_CSIM) && defined (FEATURE_UIM_RUIM) */

/*===========================================================================

FUNCTION AUTH_PROCESS_COMMAND

DESCRIPTION
  This procedure processes a command sent to the Authentication task. If
  the FEATURE_UIM_RUIM is defined, it sets the RUIM state variable and does
  not actually perform the AUTH computations.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  If the command is AUTH_GENERATE_KEY_F, lots of CPU time will be taken
  to do this huge calculation.
  If FEATURE_UIM_RUIM is defined, it sets the RUIM state variable amongst others.

===========================================================================*/

LOCAL void auth_process_command
(
  auth_cmd_type *cmd_ptr
    /* Pointer to received command */
)
{
#if (!defined(FEATURE_UIM_RUIM) || defined(FEATURE_UIM_RUN_TIME_ENABLE))
  static dword authbs;
    /* Value computed internally during SSD Update, compared to BS version */
  nv_item_type nv_item;
    /* Item to hold values retrieved from NV */
  boolean return_status;
    /* Status to be returned for Finish SSD Update or Update A Key */
  dword auth_sig;
    /* Auth signature value returned from CAVE */
#endif /* (!FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE) */


#ifdef FEATURE_CSIM
  mmgsdi_access_type       access_type;
  mmgsdi_return_enum_type  status;
  mmgsdi_len_type          req_len;
  qword                    randssd, run_cave_esn;
  byte                     process_control;
  dword                    ssd_esn;
  mmgsdi_write_data_type   write_data;
  byte mmgsdi_rec_data[2];

#elif defined(FEATURE_UIM_RUIM)

  uim_cmd_type *uim_cmd_ptr;
    /* UIM command pointer points to UIM command buffer for sending commands
       to the UIM server. */
  uim_rpt_type *uim_rpt_ptr = auth_uim_rsp_buf_ptr;
    /* Pointer to UIM report */
#endif /* FEATURE_UIM_RUIM */

#ifdef EFS_HAVE_SYNC
  int res;
#endif /* EFS_HAVE_SYNC */

#ifndef FEATURE_UIM_RUIM
size_t copy_size;
  /* Return value of memscpy function */
#endif  /* FEATURE_UIM_RUIM */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command to local buffer */
#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
  /* Determine if the R-UIM is available for this NAM */
  if (!nv_rtre_use_ruim_for_nam(auth_stored_NAM_index))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
  {
    if ((auth_init_status == AUTH_UNINITIALIZED) &&
        ((cmd_ptr->hdr.command != AUTH_INIT_F) &&
         (cmd_ptr->hdr.command != AUTH_UPDATE_A_KEY_F) &&
         (cmd_ptr->hdr.command != AUTH_RESYNC_F) &&
         (cmd_ptr->hdr.command != AUTH_RESET_F)))
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "AUTH not initialized");
      cmd_ptr->hdr.status = AUTH_INVALID_S;
      if (cmd_ptr->hdr.cmd_hdr.task_ptr != NULL)
      {
        (void) rex_set_sigs( cmd_ptr->hdr.cmd_hdr.task_ptr,
                            cmd_ptr->hdr.cmd_hdr.sigs );
      }
      return;
    }
  }
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */

/* Check if Sim Busy status is set, if yes and the auth command to be 
** processed requires MMGSDI access, then return directly without  
** processing the command.
*/

#if defined(MMOC_API_PH_STAT_CNF_WITH_STATUS) && defined( FEATURE_UIM_RUIM )
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
  /* Determine if the R-UIM is available for this NAM */
  if (nv_rtre_use_ruim_for_nam(auth_stored_NAM_index))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
  {
    #ifdef FEATURE_CSIM
    if (mc_get_sim_busy_status())
    {
      switch(cmd_ptr->hdr.command)
      {
        /* Only the below 8 auth commands require MMGSDI access */
        case AUTH_INIT_F:
        case AUTH_SSD_UPDATE_F:
        case AUTH_FINISH_SSD_UPDATE_F:
        case AUTH_SIGNATURE_F:
        case AUTH_GENERATE_KEY_F:
        case AUTH_INC_COUNT_F:
        case AUTH_BS_CHAL_F:
        case AUTH_RUIM_STATUS_F:
          {
            M1X_MSG( DCP, LEGACY_HIGH, "SIM BUSY...ignoring AUTH cmd = %d",
                     cmd_ptr->hdr.command);
            cmd_ptr->hdr.status = AUTH_INVALID_S;
            /* Keep track of the error rsn returned by auth task as sim busy */
            mc_set_auth_err_status(TRUE);
            if (cmd_ptr->hdr.cmd_hdr.task_ptr != NULL)
            {
              (void) rex_set_sigs( cmd_ptr->hdr.cmd_hdr.task_ptr,
                                  cmd_ptr->hdr.cmd_hdr.sigs );
            }
            return;    
          }
          break;

        default:
          /* Cmd doesn't require MMGSDI access. Go ahead with processing it */
          M1X_MSG( DCP, LEGACY_HIGH,
                   "SIM BUSY...auth cmd doesn't req MMGSDI access, cmd = %d",
                    cmd_ptr->hdr.command );

      }
    } /* if (mc_get_sim_busy_status()) */
    #endif /* FEATURE_CSIM */
  }
#endif /* defined(MMOC_API_PH_STAT_CNF_WITH_STATUS) && defined( FEATURE_UIM_RUIM ) */


#ifdef FEATURE_1X_CP_MEM_OPT
#if defined(FEATURE_UIM_RUIM) && defined(FEATURE_UIM_RUN_TIME_ENABLE)
  /* Determine if the R-UIM is available for this NAM */
  if (nv_rtre_use_ruim_for_nam(cdma.curr_nam))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
  {
#ifdef FEATURE_CSIM
    if( (auth_mmgsdi_rpt_ptr = (mmgsdi_cnf_type*) modem_mem_alloc
                    ( sizeof(mmgsdi_cnf_type), MODEM_MEM_CLIENT_1X ) ) != NULL)
    {
      memset( auth_mmgsdi_rpt_ptr, 0, sizeof(mmgsdi_cnf_type) );
    }
    else
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "Memory allocation failed for auth_mmgsdi_rpt_ptr");
      return;
    }
#else
    if( (auth_uim_rsp_buf_ptr = (uim_rpt_type*) modem_mem_alloc
                  ( sizeof(uim_rpt_type), MODEM_MEM_CLIENT_1X ) ) != NULL)
    {
      memset( auth_uim_rsp_buf_ptr, 0, sizeof(uim_rpt_type) );
    }
    else
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "Memory allocation failed for auth_uim_rsp_buf_ptr");
      return;
    }
#endif /* FEATURE_CSIM */
  }
#endif /* FEATURE_1X_CP_MEM_OPT */

  switch (cmd_ptr->hdr.command)
  {

    case AUTH_INIT_F:
      {
        cmd_ptr->hdr.status = AUTH_RECEIVED_S;

        if (cmd_ptr->hdr.cmd_hdr.task_ptr != NULL)
        {
          (void) rex_set_sigs( cmd_ptr->hdr.cmd_hdr.task_ptr,
                              cmd_ptr->hdr.cmd_hdr.sigs );
        }
#ifdef FEATURE_UIM_RUIM
        /* Allow the RUIM to power off for AUTH. */
        (void) uim_power_control ( UIM_AUTH, FALSE );

#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
        /* Determine if the R-UIM is available for this NAM */
        if (nv_rtre_use_ruim_for_nam(auth_stored_NAM_index))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
        {
          auth_saved_registers = FALSE;
          auth_ssd_update_in_progress = FALSE;
          auth_valid_CMEA_key = FALSE;
          auth_valid_PLCM = FALSE;

          /* Store NAM index */
          auth_stored_NAM_index = cmd_ptr->init.nam;

          auth_init_status = AUTH_INIT_OK;

          if (cdma.is_cdma_subs_avail)
          {
            /* Treat this command as a read from Call Count */
            /* Retrieve a UIM command buffer from the queue. */
            #ifdef FEATURE_CSIM
            (void) rex_clr_sigs( AUTH_TCB_PTR, AUTH_UIM_CMD_RSP_SIG );

            access_type.access_method = MMGSDI_EF_ENUM_ACCESS;
            if ( mcmmgsdi_app_type == MMGSDI_APP_CSIM )
            {
              access_type.file.file_enum = MMGSDI_CSIM_CALL_COUNT;
            }
            else if ( mcmmgsdi_app_type == MMGSDI_APP_RUIM )
            {
              access_type.file.file_enum =  MMGSDI_CDMA_CC;
            }
            else
            {
              M1X_MSG( DCP, LEGACY_MED,
                "Reading invalid app_type=%d expect errors",
                mcmmgsdi_app_type );
            }
            req_len = 2;
            /* Read from MMGSDI and wait for the response. */
            /* The record type for CALL_COUNT is hard coded to 1
            ** since the record number starts with 1.
            */
            status = mmgsdi_session_read_record(cdma.session_id,
                                                access_type,
                                                1,
                                                req_len,
                                                auth_mmgsdi_cb ,
                                                0);
            if( status != MMGSDI_SUCCESS )
            {
              M1X_MSG( DCP, LEGACY_ERROR,
                "MMGSDI read failed for the item = %d ",
                access_type.file.file_enum );
            }
            else
            {
              (void) auth_wait( AUTH_UIM_CMD_RSP_SIG );
              if ( auth_cnf == TRUE )
              {
                if( auth_mmgsdi_rpt_ptr != NULL )
                {
                  auth_mmgsdi_rpt_status =
                    auth_mmgsdi_rpt_ptr->read_cnf.response_header.mmgsdi_status;
                }
                else
                {
                  M1X_MSG( DCP, LEGACY_ERROR,
                    "auth_mmgsdi_rpt_ptr is NULL");
                  return;
                }
              }
              else
              {
                auth_mmgsdi_rpt_status = MMGSDI_ERROR;
              }
              if ( auth_mmgsdi_rpt_status == MMGSDI_SUCCESS )
              {
                M1X_MSG( DCP, LEGACY_HIGH,
                  "MMGSDI read success for the item = %d",
                  access_type.file.file_enum);

                /* Copy the new call count to the call count buffer. */
                /* As per CSIM spec, call count will be contained in
                ** lower 6 bits of the higher order byte , hence
                ** move the data ptr by 1 position
                */
                if( auth_mmgsdi_rpt_ptr != NULL )
                {
                  temp_auth_count = *(auth_mmgsdi_rpt_ptr->read_cnf.read_data.data_ptr+1);
                }
                else
                {
                  M1X_MSG( DCP, LEGACY_ERROR,
                    "auth_mmgsdi_rpt_ptr is NULL");
                  return;
                }
                M1X_MSG( DCP, LEGACY_MED,
                  "temp_auth_count = %d",
                  temp_auth_count);
                auth_count = temp_auth_count & RUIM_CC_MASK;
                M1X_MSG( DCP, LEGACY_MED,
                  "auth_count = %d",
                  auth_count);
              }
            }
            #else
            if ((uim_cmd_ptr = (uim_cmd_type*) q_get( &uim_free_q )) != NULL)
            {
              uim_cmd_ptr->access_uim.hdr.command = UIM_ACCESS_F;
              uim_cmd_ptr->access_uim.num_bytes = 2;  /* One byte in size */
              uim_cmd_ptr->access_uim.offset = 1;     /* First byte */
              uim_cmd_ptr->access_uim.rec_mode = UIM_ABSOLUTE;
              uim_cmd_ptr->access_uim.data_ptr = (byte *)&(temp_auth_count);
              uim_cmd_ptr->access_uim.item = UIM_CDMA_CC;       /* EF is "Call Count" */
              uim_cmd_ptr->access_uim.access = UIM_READ_F;     /* "Read" command        */

              /* Send the command and wait for the response. */
              auth_send_uim_command(uim_cmd_ptr);

              /* Determine if the command passed. */
              if (uim_rpt_ptr->rpt_status == UIM_PASS)
              {
                /* Copy the new call count to the call count buffer. */
                temp_auth_count = temp_auth_count >> RUIM_CC_SHIFT;
                auth_count = temp_auth_count & RUIM_CC_MASK;
              } /* end if - the command passed */
            }
            else /* No UIM command buffers on the free queue. */
            {
              M1X_MSG( DCP, LEGACY_ERROR,
                "No free buffers on auth_free_q" );
            } /* Get a UIM command buffer from the UIM free queue. */
            #endif
          }
          else
          {
            M1X_MSG( DCP, LEGACY_HIGH,
              "Subscription not available. Call Count can not be read");
            auth_count = 0;
          }
          auth_init_status = AUTH_INIT_OK;
        }
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
        else /* R-UIM is not available */
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
#endif /* FEATURE_UIM_RUIM */
#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
        {
          auth_var_init(cmd_ptr->init.nam);
          auth_init_status = CAVE_init( cmd_ptr->init.nam );
        }
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */
      } /* end case - AUTH_INIT_F */
      break;

    case AUTH_SSD_UPDATE_F:
      {
        /* Clear the T64 timer to handle the case where an SSD Update
        is in progress and an Idle HO occurs.  The new BS will initiate
        SSD Update and we don't want the old timer expiration to abort
        the new attempt */
        (void) rex_clr_timer( &auth_ssd_timer );
        (void) rex_clr_sigs( AUTH_TCB_PTR, AUTH_SSD_TIMER_SIG );

        /* Inform the client that the command is being processed. */
        cmd_ptr->hdr.status = AUTH_RECEIVED_S;

        if (cmd_ptr->hdr.cmd_hdr.task_ptr != NULL)
        {
          (void) rex_set_sigs( cmd_ptr->hdr.cmd_hdr.task_ptr,
                              cmd_ptr->hdr.cmd_hdr.sigs );
        }
        /* Indicate the SSD update has started */
        auth_ssd_update_in_progress = TRUE;

#if  defined (FEATURE_CSIM) || defined (FEATURE_UIM_RUIM)
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
        /* Determine if the R-UIM is available for this NAM */
        if (nv_rtre_use_ruim_for_nam(auth_stored_NAM_index))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE || FEATURE_CSIM */
        {
          #ifdef FEATURE_CSIM
          (void) rex_clr_sigs( AUTH_TCB_PTR, AUTH_UIM_CMD_RSP_SIG );
          randssd[0] = cmd_ptr->ssd.randssd[0];
          randssd[1] = cmd_ptr->ssd.randssd[1];
          /* Include the process control bit if the card has
            generated new A-Key */
          if (a_key_temp_valid)
          {
            cmd_ptr->ssd.process_control |= RUIM_COMMIT_NEW_SSD;
          }
          process_control = cmd_ptr->ssd.process_control;
          #ifdef FEATURE_UIM_JCDMA_RUIM_SUPPORT
          ssd_esn = tmc_get_stored_esn_me();
          #else
          ssd_esn = cmd_ptr->ssd.esn;
          #endif

          status =  mmgsdi_session_ssd_update( cdma.session_id,
                                               randssd,
                                               process_control,
                                               ssd_esn,
                                               auth_mmgsdi_cb ,
                                               0 );
          if ( status != MMGSDI_SUCCESS )
          {
            M1X_MSG( DCP, LEGACY_ERROR,
              "MMGSDI returned failure" );
          }
          else
          {
            (void) auth_wait( AUTH_UIM_CMD_RSP_SIG );
            if ( auth_cnf == TRUE )
            {
              if( auth_mmgsdi_rpt_ptr != NULL )
              {
                auth_mmgsdi_rpt_status =
                  auth_mmgsdi_rpt_ptr->session_ssd_update_cnf.response_header.mmgsdi_status;
              }
              else
              {
                M1X_MSG( DCP, LEGACY_ERROR,
                  "auth_mmgsdi_rpt_ptr is NULL");
                return;
              }
            }
            else
            {
              auth_mmgsdi_rpt_status = MMGSDI_ERROR;
            }
            M1X_MSG( DCP, LEGACY_MED,
              "MMGSDI- AUTH : session_ssd_update status=%d",
              auth_mmgsdi_rpt_status );

            /* Do NOT check the response.  There is no use for a response
              for this command. */
          }
          #elif defined (FEATURE_UIM_RUIM)
          /* Retrieve a UIM command buffer from the queue. */
          if ((uim_cmd_ptr = (uim_cmd_type*) q_get( &uim_free_q )) != NULL)
          {
            /* Build a SSD update command. */
            uim_cmd_ptr->hdr.command = UIM_SSD_UPDATE_F;
            uim_cmd_ptr->ssd_update.randssd[0] = cmd_ptr->ssd.randssd[0];
            uim_cmd_ptr->ssd_update.randssd[1] = cmd_ptr->ssd.randssd[1];
            /* Include the process control bit if the card has
               generated new A-Key */
            if (a_key_temp_valid)
            {
                cmd_ptr->ssd.process_control |= RUIM_COMMIT_NEW_SSD;
            }
            /* Fill in the rest of the command parameters. */
            uim_cmd_ptr->ssd_update.process_control = cmd_ptr->ssd.process_control;

#ifdef FEATURE_UIM_JCDMA_RUIM_SUPPORT
            uim_cmd_ptr->ssd_update.esn = tmc_get_stored_esn_me();
#else
            uim_cmd_ptr->ssd_update.esn = cmd_ptr->ssd.esn;
#endif

            /* Send the command and wait for the response. */
            auth_send_uim_command(uim_cmd_ptr);

            /* Do NOT check the response.  There is no use for a response
               for this command. */
          }
          else /* No UIM command buffers on the free queue. */
          {
            M1X_MSG( DCP, LEGACY_ERROR,
              "No free buffers on auth_free_q" );
          } /* Get a UIM command buffer from the UIM free queue. */
        #endif
        }
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
        else /* R-UIM is not available */
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
#endif /* FEATURE_UIM_RUIM */
#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
        {
          CAVE_generate_SSD( cmd_ptr->ssd.randssd );
          authbs = CAVE_auth_signature( cmd_ptr->ssd.randbs,
                     cmd_ptr->ssd.auth_data, TRUE, FALSE

                     , FALSE  // Not for SPASM

                           );
        }
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */

        M1X_MSG( DCP, LEGACY_MED,
          "SSD update command recd" );
      } /* end case - AUTH_SSD_UPDATE_F */
      break;

    case AUTH_FINISH_SSD_UPDATE_F:
      {
        /* Cancel the SSD timer now that the FINISH_SSD command has
           been received. */
        (void) rex_clr_sigs( AUTH_TCB_PTR, AUTH_SSD_TIMER_SIG );
        (void) rex_clr_timer( &auth_ssd_timer );

        /* Determine if the SSD update has started. */
        if (!auth_ssd_update_in_progress)
        {
          M1X_MSG( DCP, LEGACY_MED,
            "No SSD Update in progress" );
          cmd_ptr->hdr.status = AUTH_INVALID_S;
          if (cmd_ptr->hdr.cmd_hdr.task_ptr != NULL)
          {
            (void) rex_set_sigs( cmd_ptr->hdr.cmd_hdr.task_ptr,
                                cmd_ptr->hdr.cmd_hdr.sigs );
          }
          /* We do not need to process this command and the buffer
             needs to be released */
        }
        else /* We can proceed and finish the SSD Update */
        {
          /* Inform the client the command was received. */
          cmd_ptr->hdr.status = AUTH_RECEIVED_S;
          if (cmd_ptr->hdr.cmd_hdr.task_ptr != NULL)
          {
            (void) rex_set_sigs( cmd_ptr->hdr.cmd_hdr.task_ptr,
                                cmd_ptr->hdr.cmd_hdr.sigs );
          }
          /* Indicate the SSD is finished. */
          auth_ssd_update_in_progress = FALSE;
#ifdef FEATURE_UIM_RUIM
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
          /* Determine if the R-UIM is available for this NAM */
          if (nv_rtre_use_ruim_for_nam(auth_stored_NAM_index))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
          {
          #if defined( FEATURE_CSIM )
            (void) rex_clr_sigs( AUTH_TCB_PTR, AUTH_UIM_CMD_RSP_SIG );
            status = mmgsdi_session_confirm_ssd( cdma.session_id,
                                                 cmd_ptr->f_ssd.authbs,
                                                 auth_mmgsdi_cb ,
                                                 0 );
            if( status != MMGSDI_SUCCESS )
            {
              M1X_MSG( DCP, LEGACY_ERROR,
                "MMGSDI retuned error = %d for SSD Update",
                status );
              auth_mmgsdi_rpt_status = status;
              /* Set the value for SSD Update to FALSE, because in this case
                 we hanvent really queued any thing to MMGSDI. */
              if( auth_mmgsdi_rpt_ptr != NULL )
              {
                auth_mmgsdi_rpt_ptr->session_ssd_confirm_cnf.ssd_update_ok = FALSE;
              }
              else
              {
                M1X_MSG( DCP, LEGACY_ERROR,
                  "auth_mmgsdi_rpt_ptr is NULL");
                return;
              }
            }
            else
            {
              (void) auth_wait( AUTH_UIM_CMD_RSP_SIG );
              if ( auth_cnf == TRUE )
              {
                if( auth_mmgsdi_rpt_ptr != NULL )
                {
                  auth_mmgsdi_rpt_status =
                    auth_mmgsdi_rpt_ptr->session_ssd_confirm_cnf.response_header.mmgsdi_status;
                }
                else
                {
                  M1X_MSG( DCP, LEGACY_ERROR,
                    "auth_mmgsdi_rpt_ptr is NULL");
                  return;
                }
              }
              else
              {
                auth_mmgsdi_rpt_status = MMGSDI_ERROR;
              }
            }

            M1X_MSG( DCP, LEGACY_HIGH,
              "MMGSDI - AUTH session_confirm_ssd status = %d",
              auth_mmgsdi_rpt_status );

            /* -------------------------------------
            ** Now, if required, report back whether
            ** or not SSD update was successful.
            ** ------------------------------------- */
            if ( cmd_ptr->f_ssd.rpt_function != NULL )
            {
              /* Retrieve the UIM response from the UIM report buffer. */
              auth_rpt_buf.rpt_type = AUTH_FINISH_SSD_R;
              if ( auth_mmgsdi_rpt_status == MMGSDI_SUCCESS )
              {
                auth_rpt_buf.rpt_status = UIM_PASS;
              }
              else
              {
                auth_rpt_buf.rpt_status = UIM_FAIL;
              }
              if( auth_mmgsdi_rpt_ptr != NULL )
              {
                /* Copy the MMGSDI response to the report buffer. */
                M1X_MSG( DCP, LEGACY_HIGH,
                  "MMGSDI - AUTH session_confirm_ssd status = %d",
                  auth_mmgsdi_rpt_ptr->session_ssd_confirm_cnf.ssd_update_ok );
                auth_rpt_buf.rpt.ssd_update_ok =
                           auth_mmgsdi_rpt_ptr->session_ssd_confirm_cnf.ssd_update_ok;
              }
              else
              {
                M1X_MSG( DCP, LEGACY_ERROR,
                  "auth_mmgsdi_rpt_ptr is NULL");
                return;
              }
              /* Fill in necessary fields of command block */
              auth_rpt_buf.rpt_hdr.task_ptr = NULL;

              /* Report to requesting task */
              (*(cmd_ptr->f_ssd.rpt_function))( &auth_rpt_buf );
              M1X_MSG( DCP, LEGACY_MED,
                "Reporting SSD update complete" );
            }
            #elif defined( FEATURE_UIM_RUIM )
            /* Retrieve a UIM command buffer from the queue. */
            if ((uim_cmd_ptr = (uim_cmd_type*) q_get( &uim_free_q )) != NULL)
            {
              /* Build a confirm SSD command. */
              uim_cmd_ptr->confirm_ssd.hdr.command = UIM_CONFIRM_SSD_F;
              uim_cmd_ptr->confirm_ssd.authbs = cmd_ptr->f_ssd.authbs;

              /* Send the command and wait for the response. */
              auth_send_uim_command(uim_cmd_ptr);

              /* -------------------------------------
              ** Now, if required, report back whether
              ** or not SSD update was successful.
              ** ------------------------------------- */
              if (cmd_ptr->f_ssd.rpt_function != NULL)
              {
                /* Retrieve the UIM response from the UIM report buffer. */
                auth_rpt_buf.rpt_type = AUTH_FINISH_SSD_R;
                auth_rpt_buf.rpt_status = uim_rpt_ptr->rpt_status;

                /* Copy the UIM response to the report buffer. */
                auth_rpt_buf.rpt.ssd_update_ok = uim_rpt_ptr->rpt.ssd_update_ok;

                /* Fill in necessary fields of command block */
                auth_rpt_buf.rpt_hdr.task_ptr = NULL;

                /* Report to requesting task */
                (*(cmd_ptr->f_ssd.rpt_function))( &auth_rpt_buf );
                M1X_MSG( DCP, LEGACY_MED,
                  "Reporting SSD update complete" );
              }
            }
            else /* No UIM command buffers on the free queue. */
            {
              M1X_MSG( DCP, LEGACY_ERROR,
                "No free buffers on auth_free_q" );
            } /* Get a UIM command buffer from the UIM free queue. */
            /* The SSD is no longer in progress.  Allow the UIM to be powered
               down */
          #endif /* CSIM*/
            (void) uim_power_control ( UIM_AUTH, FALSE );
          }
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
          else /* R-UIM is not available */
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
#endif /* FEATURE_UIM_RUIM */
#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
          {
            M1X_MSG( DCP, LEGACY_MED, "MS authbs %ld, Received authbs %ld",
                     authbs, cmd_ptr->f_ssd.authbs );

            /* If the base station value is correct, report SSD Update success */
            if (authbs == cmd_ptr->f_ssd.authbs)
            {
              return_status = TRUE;
            }
            else
            {
              return_status = FALSE;
            }

            /* -------------------------------------
            ** Now, if required, report back whether
            ** or not SSD update was successful.
            ** ------------------------------------- */
            if (cmd_ptr->f_ssd.rpt_function != NULL)
            {
              auth_rpt_buf.rpt_type = AUTH_FINISH_SSD_R;
              auth_rpt_buf.rpt.ssd_update_ok = return_status;

              /* Fill in necessary fields of command block */
              auth_rpt_buf.rpt_hdr.task_ptr = NULL;

              /* Queue report for requesting task */
              (*(cmd_ptr->f_ssd.rpt_function))( &auth_rpt_buf );
              M1X_MSG( DCP, LEGACY_MED,
                "Reporting SSD update complete" );
            }

            /* ----------------------------------------------------------------
            ** Tell CAVE to update the SSD (Write it to NV) after reporting
            ** the success.  There is a 750 ms timeout for analog voice channel
            ** operation, and we take too long otherwise.  this is OK since the
            ** NV is guaranteed to write properly or else cause a fatal error.
            ** The NV writes take almost two seconds.
            ** ---------------------------------------------------------------- */

            /* ------------------------------------------------------------------
            ** If a_key_temp_valid is true, we do not want to write SSD to NV.
            ** The new_ssd_valid flag is set to true to indicate that ssd has
            ** been generated from the new A-Key. Also if we got a commit request,
            ** by looking at this flag we can decide if we need to commit the ssd
            ** ---------------------------------------------------------------- */

            if (a_key_temp_valid) {
              /* change the flag to indicate that new A-Key has been used
              ** to generate the SSDs  */
              new_ssd_valid = TRUE;
            }
            else
            {
              if (return_status == TRUE)
              {
                (void) CAVE_update_SSD();
              }
            }
          }
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */
        }
        /* Send a indicating what was done. */
        M1X_MSG( DCP, LEGACY_MED,
          "Received SSD update command " );
      } /* end case - AUTH_FINISH_SSD_UPDATE_F */
      break;

    case AUTH_SIGNATURE_F:
      {
        M1X_MSG( DCP, LEGACY_MED,
          "Received AUTH_SIGNATURE_F command");
        cmd_ptr->hdr.status = AUTH_RECEIVED_S;
        if (cmd_ptr->hdr.cmd_hdr.task_ptr != NULL)
        {
           (void) rex_set_sigs( cmd_ptr->hdr.cmd_hdr.task_ptr,
                             cmd_ptr->hdr.cmd_hdr.sigs );
        }

#ifdef FEATURE_UIM_RUIM
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
        /* Determine if the R-UIM is available for this NAM */
        if (nv_rtre_use_ruim_for_nam(auth_stored_NAM_index))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
        {
        #ifdef FEATURE_CSIM
          (void) rex_clr_sigs( AUTH_TCB_PTR, AUTH_UIM_CMD_RSP_SIG );
        #ifdef FEATURE_UIM_JCDMA_RUIM_SUPPORT
          run_cave_esn[0] = tmc_get_stored_esn_me();
        #else
          run_cave_esn[0] = cmd_ptr->sig.esn;
        #endif
          run_cave_esn[1] = 0;
          status = mmgsdi_session_run_cave_algo( cdma.session_id,
                    (mmgsdi_cave_rand_type_type)cmd_ptr->sig.rand_type,
                    (mmgsdi_cave_rand_chal_type)cmd_ptr->sig.rand_chal,
                    (mmgsdi_cave_dig_len_type) cmd_ptr->sig.dig_len,
                    cmd_ptr->sig.digits,
                    (mmgsdi_cave_process_control_type) cmd_ptr->sig.process_control,
                    run_cave_esn,
                    auth_mmgsdi_cb ,
                    0 );
          if( status != MMGSDI_SUCCESS )
          {
            M1X_MSG( DCP, LEGACY_ERROR,
              "AUTH_SIGNATURE_F - MMGSDI retuned error = %d ",
              status);
            /* Report error to the calling function if there is a handler
             * registered.
             */
            auth_mmgsdi_rpt_status = status;
          }
          else
          {
            (void) auth_wait( AUTH_UIM_CMD_RSP_SIG );
             /* Determine if the run CAVE command passed. */
            if ( auth_cnf == TRUE )
            {
              if( auth_mmgsdi_rpt_ptr != NULL )
              {
                auth_mmgsdi_rpt_status =
                  auth_mmgsdi_rpt_ptr->run_cave_cnf.response_header.mmgsdi_status;
              }
              else
              {
                M1X_MSG( DCP, LEGACY_ERROR,
                  "auth_mmgsdi_rpt_ptr is NULL");
                return;
              }
            }
            else
            {
              auth_mmgsdi_rpt_status = MMGSDI_ERROR;
            }
          }
          M1X_MSG( DCP, LEGACY_HIGH,
            "MMGSDI- AUTH: auth_mmgsdi_rpt_status = %d",
            auth_mmgsdi_rpt_status);
          if (auth_mmgsdi_rpt_status == MMGSDI_SUCCESS)
          {
            /* Determine whether the registers were saved based on the
               command control. */
            if ( cmd_ptr->sig.process_control & RUIM_SAVE_REGS_ON )
            {
              auth_saved_registers = TRUE;
            }
            else
            {
              auth_saved_registers = FALSE;
            }
          }
          else /* The run CAVE command failed. */
          {
            /* The registers were NOT saved. */
            auth_saved_registers = FALSE;
          }

          /* -------------------------------------
          ** Now, if required, send the signature
          ** back
          ** ------------------------------------- */
          if ( cmd_ptr->sig.rpt_function != NULL )
          {
            /* Assign the report type */
            auth_rpt_buf.rpt_type = AUTH_SIGNATURE_R;

            /* Copy the MMGSDI response to the report buffer. */
            if ( auth_mmgsdi_rpt_status == MMGSDI_SUCCESS )
            {
              auth_rpt_buf.rpt_status = UIM_PASS;
            }
            else
            {
              auth_rpt_buf.rpt_status = UIM_FAIL;
            }
            if( auth_mmgsdi_rpt_ptr != NULL )
            {
              auth_rpt_buf.rpt.auth =
                     (dword) auth_mmgsdi_rpt_ptr->run_cave_cnf.run_cave_response_data;
            }
            else
            {
              M1X_MSG( DCP, LEGACY_ERROR,
                "auth_mmgsdi_rpt_ptr is NULL");
              return;
            }
            /* Fill in necessary fields of command block */
            auth_rpt_buf.rpt_hdr.task_ptr = NULL;

            /* Report to requesting task */
            (*(cmd_ptr->sig.rpt_function))( &auth_rpt_buf );
            M1X_MSG( DCP, LEGACY_MED,
              "Reporting auth Signature %ld",
              auth_rpt_buf.rpt.auth );
          } /* end if - the client supplied a reportback function. */
          #else
          /* Retrieve a UIM command buffer from the queue. */
          if ((uim_cmd_ptr = (uim_cmd_type*) q_get( &uim_free_q )) != NULL)
          {
            uim_cmd_ptr->run_cave.hdr.command = UIM_RUN_CAVE_F;
            uim_cmd_ptr->run_cave.rand_type = cmd_ptr->sig.rand_type;
            uim_cmd_ptr->run_cave.rand_chal = cmd_ptr->sig.rand_chal;
            uim_cmd_ptr->run_cave.dig_len = cmd_ptr->sig.dig_len;

            copy_size = memscpy( uim_cmd_ptr->run_cave.digits,
                                 UIM_RUN_CAVE_DIGITS*sizeof(byte),
                     cmd_ptr->sig.digits,
                                 RUIM_RUN_CAVE_DIGITS
                               );

            if( copy_size != RUIM_RUN_CAVE_DIGITS )
            {
              M1X_MSG( DCP, LEGACY_ERROR,
                "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
                copy_size,
                RUIM_RUN_CAVE_DIGITS,
                UIM_RUN_CAVE_DIGITS*sizeof(byte));
            }
            uim_cmd_ptr->run_cave.process_control = cmd_ptr->sig.process_control;
#ifdef FEATURE_UIM_JCDMA_RUIM_SUPPORT
            uim_cmd_ptr->run_cave.esn[0] = tmc_get_stored_esn_me();
#else
            uim_cmd_ptr->run_cave.esn[0] = cmd_ptr->sig.esn;
#endif
            uim_cmd_ptr->run_cave.esn[1] = 0; /* Zero out the MSbits of the ESN
                                                 since the command takes 7 bytes
                                                 and our ESN is only 4 bytes.*/

            /* Send the command and wait for the response. */
            auth_send_uim_command(uim_cmd_ptr);

            /* Determine if the run CAVE command passed. */
            if (uim_rpt_ptr->rpt_status == UIM_PASS)
            {
              /* Determine whether the registers were saved based on the
                 command control. */
              if (cmd_ptr->sig.process_control & RUIM_SAVE_REGS_ON)
              {
                 auth_saved_registers = TRUE;
              }
              else
              {
                 auth_saved_registers = FALSE;
              }
            }
            else /* The run CAVE command failed. */
            {
              /* The registers were NOT saved. */
              auth_saved_registers = FALSE;
            }

            /* -------------------------------------
            ** Now, if required, send the signature
            ** back
            ** ------------------------------------- */
            if (cmd_ptr->sig.rpt_function != NULL)
            {
              /* Assign the report type */
              auth_rpt_buf.rpt_type = AUTH_SIGNATURE_R;

              /* Copy the UIM response to the report buffer. */
              auth_rpt_buf.rpt_status = uim_rpt_ptr->rpt_status;
              auth_rpt_buf.rpt.auth = uim_rpt_ptr->rpt.run_cave.auth;

              /* Fill in necessary fields of command block */
              auth_rpt_buf.rpt_hdr.task_ptr = NULL;

              /* Report to requesting task */
              (*(cmd_ptr->sig.rpt_function))( &auth_rpt_buf );
              M1X_MSG( DCP, LEGACY_MED,
                "Reporting auth Signature %ld",
                auth_rpt_buf.rpt.auth );
            } /* end if - the client supplied a reportback function. */
          }
          else /* No UIM command buffers on the free queue. */
          {
            M1X_MSG( DCP, LEGACY_ERROR,
              "No free buffers on auth_free_q" );
          } /* Get a UIM command buffer from the UIM free queue. */
          #endif
        }
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
        else /* R-UIM is not available */
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
#endif /* FEATURE_UIM_RUIM */
#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
        {
          /* indicate that the new ssd values need to be used if they
          ** have been generated  using the new a-key*/
          if (new_ssd_valid)
          {
             auth_sig = CAVE_auth_signature( cmd_ptr->sig.rand_chal,
                cmd_ptr->sig.auth_data, TRUE, cmd_ptr->sig.save_reg

            , FALSE  // Not for SPASM

            );

             /* set to TRUE since new ssd values have been used to compute
                AUTHR */
             used_new_ssd = TRUE;
          }
          else
          {
             auth_sig = CAVE_auth_signature( cmd_ptr->sig.rand_chal,
                cmd_ptr->sig.auth_data, FALSE, cmd_ptr->sig.save_reg

            , FALSE  // Not for SPASM

            );

            used_new_ssd = FALSE;
          }

          /* ------------------------------------------------------------
          ** If we saved the register values, set saved_registers to TRUE
          ** to indicate we are ready to generate CMEA key and VPM.
          ** ------------------------------------------------------------ */
          if (cmd_ptr->sig.save_reg)
          {
             auth_saved_registers = TRUE;
          }

          /* Report back auth signature, if requested */
          if (cmd_ptr->sig.rpt_function != NULL)
          {
            /* Retrieve the UIM response from the UIM report buffer. */
             auth_rpt_buf.rpt_type = AUTH_SIGNATURE_R;
             auth_rpt_buf.rpt.auth = auth_sig;

             /* Fill in necessary fields of command block */
             auth_rpt_buf.rpt_hdr.task_ptr = NULL;

             /* --------------------------------
             ** Queue report for requesting task
             ** -------------------------------- */
             (*(cmd_ptr->sig.rpt_function))( &auth_rpt_buf );
             M1X_MSG( DCP, LEGACY_MED,
               "Reporting auth %ld",
               auth_rpt_buf.rpt.auth );
          }
        }
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */

        M1X_MSG( DCP, LEGACY_MED,
          "Received Auth Signature command " );
      } /* end case - AUTH_SIGNATURE_F */
      break;

    case AUTH_GENERATE_KEY_F:
      {
        /* Determine if the auth registers were saved from the last
           RUN CAVE command. */
        if (!auth_saved_registers)
        {
          /* The registers were NOT saved.  Generate keys will fail.
             Inform the client the command failed. */
          M1X_MSG( DCP, LEGACY_ERROR,
            "Need to call auth_signature first");
          cmd_ptr->hdr.status = AUTH_INVALID_S;
          if (cmd_ptr->hdr.cmd_hdr.task_ptr != NULL)
          {
            (void) rex_set_sigs( cmd_ptr->hdr.cmd_hdr.task_ptr,
                                cmd_ptr->hdr.cmd_hdr.sigs );
          }
          /* Indicate the keys are no longer valid. */
          auth_valid_CMEA_key = FALSE;
          auth_valid_PLCM = FALSE;
        }
        else
        {
          /* Inform the client that the command was received */
          cmd_ptr->hdr.status = AUTH_RECEIVED_S;
          if (cmd_ptr->hdr.cmd_hdr.task_ptr != NULL)
          {
            (void) rex_set_sigs( cmd_ptr->hdr.cmd_hdr.task_ptr,
                                cmd_ptr->hdr.cmd_hdr.sigs );
          }

#ifdef FEATURE_UIM_RUIM
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
          /* Determine if the R-UIM is available for this NAM */
          if (nv_rtre_use_ruim_for_nam(auth_stored_NAM_index))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
          {
          #ifdef FEATURE_CSIM
           (void) rex_clr_sigs( AUTH_TCB_PTR, AUTH_UIM_CMD_RSP_SIG );
            status = mmgsdi_session_generate_key_vpm(
                       cdma.session_id,
                       (mmgsdi_single_vpm_octet_type) cmd_ptr->key.vpm_first_octet,
                       (mmgsdi_single_vpm_octet_type)  cmd_ptr->key.vpm_last_octet,
                       auth_mmgsdi_cb ,
                       0 );

            if( status != MMGSDI_SUCCESS )
            {
              M1X_MSG( DCP, LEGACY_ERROR,
                "MMGSDI retuned error = %d for Auth Generate Key",
                status);
              auth_mmgsdi_rpt_status = status;
            }
            else
            {
              (void) auth_wait( AUTH_UIM_CMD_RSP_SIG );
              /* Determine if the run CAVE command passed. */
              if( auth_cnf == TRUE )
              {
                if( auth_mmgsdi_rpt_ptr != NULL )
                {
                  auth_mmgsdi_rpt_status =
                    auth_mmgsdi_rpt_ptr->generate_key_vpm_cnf.response_header.mmgsdi_status;
                }
                else
                {
                  M1X_MSG( DCP, LEGACY_ERROR,
                    "auth_mmgsdi_rpt_ptr is NULL");
                  return;
                }
              }
              else
              {
                auth_mmgsdi_rpt_status = MMGSDI_ERROR;
              }
            }
            if (auth_mmgsdi_rpt_status == MMGSDI_SUCCESS)
            {
              /* With the new interface, we always compute the VPM */
              /* Indocate that the keys are now valid. */
              auth_valid_PLCM = TRUE;
              auth_valid_CMEA_key = TRUE;

              /* Update CAVE with CMEA and VPM keys */
              if( auth_mmgsdi_rpt_ptr != NULL )
              {
                CAVE_update_keys(auth_mmgsdi_rpt_ptr->generate_key_vpm_cnf.key_data,
                               auth_mmgsdi_data);
              }
              else
              {
                M1X_MSG( DCP, LEGACY_ERROR,
                  "auth_mmgsdi_rpt_ptr is NULL");
                return;
              }
            }
            else /* The run CAVE command failed. */
            {
              /* The command failed, so the keys are NOT valid. */
              auth_valid_PLCM = FALSE;
              auth_valid_CMEA_key = FALSE;
            } /* end if the report is success */

            if ( cmd_ptr->key.rpt_function != NULL )
            {
             /* Assign the report type */
              auth_rpt_buf.rpt_type = AUTH_GENERATE_KEY_R;

              /* Retrieve the UIM response from the UIM report buffer. */
              if ( auth_mmgsdi_rpt_status == MMGSDI_SUCCESS )
              {
                auth_rpt_buf.rpt_status = UIM_PASS;
              }
              else
              {
                auth_rpt_buf.rpt_status = UIM_FAIL;
              }

              /* Fill in necessary fields of command block */
              auth_rpt_buf.rpt_hdr.task_ptr = NULL;

              /* Report to requesting task */
              (*(cmd_ptr->key.rpt_function))( &auth_rpt_buf );
              M1X_MSG( DCP, LEGACY_MED,
                "Reporting keys generated" );
            }
             #else
            /* Retrieve a UIM command buffer from the queue. */
            if ((uim_cmd_ptr = (uim_cmd_type*) q_get( &uim_free_q )) != NULL)
            {
              uim_cmd_ptr->hdr.command = UIM_GENERATE_KEYS_F;
              uim_cmd_ptr->gen_keys.vpm_first_octet = cmd_ptr->key.vpm_first_octet;
              uim_cmd_ptr->gen_keys.vpm_last_octet = cmd_ptr->key.vpm_last_octet;

              /* Send the command and wait for the response. */
              auth_send_uim_command(uim_cmd_ptr);

              /* Determine if the run CAVE command passed. */
              if (uim_rpt_ptr->rpt_status == UIM_PASS)
              {
                /* With the new interface, we always compute the VPM */
                /* Indocate that the keys are now valid. */
                auth_valid_PLCM = TRUE;
                auth_valid_CMEA_key = TRUE;

                /* Update CAVE with CMEA and VPM keys */
                CAVE_update_keys(uim_rpt_ptr->rpt.generate_keys.data);
              }
              else /* The run CAVE command failed. */
              {
                /* The command failed, so the keys are NOT valid. */
                auth_valid_PLCM = FALSE;
                auth_valid_CMEA_key = FALSE;
              } /* end if - the command passed */

              /* -------------------------------------
              ** Now, if required, send the signature
              ** back
              ** ------------------------------------- */
              if (cmd_ptr->key.rpt_function != NULL)
              {
                /* Assign the report type */
                auth_rpt_buf.rpt_type = AUTH_GENERATE_KEY_R;

                /* Retrieve the UIM response from the UIM report buffer. */
                auth_rpt_buf.rpt_status = uim_rpt_ptr->rpt_status;

                /* Fill in necessary fields of command block */
                auth_rpt_buf.rpt_hdr.task_ptr = NULL;

                /* Report to requesting task */
                (*(cmd_ptr->key.rpt_function))( &auth_rpt_buf );
                M1X_MSG( DCP, LEGACY_MED,
                  "Reporting keys generated" );
              } /* end if - the client supplied a reportback function. */
            }
            else /* No UIM command buffers on the free queue. */
            {
              M1X_MSG( DCP, LEGACY_ERROR,
                "No free buffers on auth_free_q" );
            } /* Get a UIM command buffer from the UIM free queue. */
          #endif
          }
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
          else /* R-UIM is not available */
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
#endif /* FEATURE_UIM_RUIM */
#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
          {
            /* Full key generation takes 100+ms of CPU time */
            /* It sets auth_valid_CMEA_key as soon as the key is valid */
            /* Variable used_new_ssd is sent as a parameter */
            if (used_new_ssd)
            {
              CAVE_generate_key( cmd_ptr->key.do_vpm, &auth_valid_CMEA_key, TRUE );
              M1X_MSG( DCP, LEGACY_MED,
                "Keys generated with new SSD");
            }
            else
            {
              CAVE_generate_key( cmd_ptr->key.do_vpm, &auth_valid_CMEA_key, FALSE );
              M1X_MSG( DCP, LEGACY_MED,
                "Keys generated");
            }

            /* If we were told to calculate the VPM, then our PLCM is valid */
            auth_valid_PLCM = cmd_ptr->key.do_vpm;

            /* Report back that keys are generated, if requested */
            if (cmd_ptr->key.rpt_function != NULL)
            {
              auth_rpt_buf.rpt_type = AUTH_GENERATE_KEY_R;

              /* Fill in necessary fields of command block */
              auth_rpt_buf.rpt_hdr.task_ptr = NULL;

              /* --------------------------------
              ** Queue report for requesting task
              ** -------------------------------- */
              (*(cmd_ptr->key.rpt_function))( &auth_rpt_buf );
              M1X_MSG( DCP, LEGACY_MED,
                "Reporting keys generated" );
            }
          }
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */
        } /* end if - the auth registers were saved. */
      } /* end case - AUTH_GENERATE_KEY_F */
      break;

#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )

    case AUTH_UPDATE_A_KEY_F:
      {

        cmd_ptr->hdr.status = AUTH_RECEIVED_S;

        if (cmd_ptr->hdr.cmd_hdr.task_ptr != NULL)
        {
          (void) rex_set_sigs( cmd_ptr->hdr.cmd_hdr.task_ptr,
                                cmd_ptr->hdr.cmd_hdr.sigs );
        }

        if (CAVE_update_A_key( cmd_ptr->a_key.a_key,
                              cmd_ptr->a_key.nam ))
        {
          /* Re-initialize everything following A_key change */
          auth_var_init( cmd_ptr->a_key.nam );
          auth_init_status = CAVE_init( cmd_ptr->a_key.nam );
          return_status = TRUE;
        }
        else
        {
          return_status = FALSE;
        }

        /* ------------------------------------------
        ** Now if a report was requested, report back
        ** whether or not A-KEY update was successful.
        ** ------------------------------------------ */
        if (cmd_ptr->a_key.rpt_function != NULL)
        {
          auth_rpt_buf.rpt_type = AUTH_A_KEY_R;
          auth_rpt_buf.rpt.a_key_update_ok = return_status;

          /* Fill in necessary fields of command block */
          auth_rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Queue report for requesting task */
          (*(cmd_ptr->a_key.rpt_function))( &auth_rpt_buf );
          M1X_MSG( DCP, LEGACY_MED,
            "Reporting A-key update complete" );
        }
      } /* end case - AUTH_UPDATE_A_KEY_F */
      break;
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */

    case AUTH_INC_COUNT_F:
      {
        /* Inform the client that the command was received */
        cmd_ptr->hdr.status = AUTH_RECEIVED_S;
        if (cmd_ptr->hdr.cmd_hdr.task_ptr != NULL)
        {
          (void) rex_set_sigs( cmd_ptr->hdr.cmd_hdr.task_ptr,
                                cmd_ptr->hdr.cmd_hdr.sigs );
        }

#ifdef FEATURE_UIM_RUIM
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
        /* Determine if the R-UIM is available for this NAM */
        if (nv_rtre_use_ruim_for_nam(auth_stored_NAM_index))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
        {
          /* Treat this command as a write to Call Count */
          /* Use a temporary buffer to hold the call count. */
          temp_auth_count = (auth_count + 1) & RUIM_CC_MASK;
        #ifdef FEATURE_CSIM
          (void) rex_clr_sigs( AUTH_TCB_PTR, AUTH_UIM_CMD_RSP_SIG );
          access_type.access_method = MMGSDI_EF_ENUM_ACCESS;

          if ( mcmmgsdi_app_type == MMGSDI_APP_CSIM )
          {
            access_type.file.file_enum = MMGSDI_CSIM_CALL_COUNT;
          }
          else if ( mcmmgsdi_app_type == MMGSDI_APP_RUIM )
          {
            access_type.file.file_enum =  MMGSDI_CDMA_CC;
          }
          else
          {
            M1X_MSG( DCP, LEGACY_MED,
              "Reading invalid app_type=%d expect errors",
              mcmmgsdi_app_type );
          }
          /* As per CSIM spec, the data for CALL_COUNT should be written
          ** in byte array format, the lower byte being 0.
          */
          write_data.data_len = 2;
          mmgsdi_rec_data[0] = 0;
          mmgsdi_rec_data[1] = (uint8)(temp_auth_count);
          write_data.data_ptr = mmgsdi_rec_data;

          /* The record type for CALL_COUNT is hard coded to 1
          ** since the record number starts with 1.
          */
          status = mmgsdi_session_write_record(cdma.session_id,
                                               access_type,
                                               MMGSDI_CYCLIC_FILE,
                                               1,
                                               write_data,
                                               auth_mmgsdi_cb,
                                               0);
          if( status != MMGSDI_SUCCESS )
          {
            M1X_MSG( DCP, LEGACY_ERROR,
              "MMGSDI read failed for the item  " );
          }
          else
          {
            (void) auth_wait( AUTH_UIM_CMD_RSP_SIG );
            if ( auth_cnf == TRUE )
            {
              if( auth_mmgsdi_rpt_ptr != NULL )
              {
                auth_mmgsdi_rpt_status =
                  auth_mmgsdi_rpt_ptr->write_cnf.response_header.mmgsdi_status;
              }
              else
              {
                M1X_MSG( DCP, LEGACY_ERROR,
                  "auth_mmgsdi_rpt_ptr is NULL");
                return;
              }
            }
            if ( auth_mmgsdi_rpt_status == MMGSDI_SUCCESS )
            {
              /* Copy the new call count to the call count buffer. */
              auth_count = temp_auth_count & RUIM_CC_MASK;
              M1X_MSG( DCP, LEGACY_MED,
                " written auth_count=%d",
                auth_count);
            } /* end if - the command passed */
          }
          #else
          temp_auth_count = temp_auth_count << RUIM_CC_SHIFT;
          /* Retrieve a UIM command buffer from the queue. */
          if ((uim_cmd_ptr = (uim_cmd_type*) q_get( &uim_free_q )) != NULL)
          {
            uim_cmd_ptr->access_uim.hdr.command = UIM_ACCESS_F;
            uim_cmd_ptr->access_uim.num_bytes = 2;  /* One byte in size */
            uim_cmd_ptr->access_uim.offset = 1;     /* Second byte */
            uim_cmd_ptr->access_uim.rec_mode = UIM_PREVIOUS;
            uim_cmd_ptr->access_uim.data_ptr = (byte *)&(temp_auth_count);
            uim_cmd_ptr->access_uim.item = UIM_CDMA_CC;       /* EF is "Call Count" */
            uim_cmd_ptr->access_uim.access = UIM_WRITE_F;     /* "Write" command        */

            /* Send the command and wait for the response. */
            auth_send_uim_command(uim_cmd_ptr);

            /* Determine if the run CAVE command passed. */
            if (uim_rpt_ptr->rpt_status == UIM_PASS)
            {
              /* Copy the new call count to the call count buffer. */
              temp_auth_count = temp_auth_count >> RUIM_CC_SHIFT;
              auth_count = temp_auth_count & RUIM_CC_MASK;
            } /* end if - the command passed */
          }
          else /* No UIM command buffers on the free queue. */
          {
            M1X_MSG( DCP, LEGACY_ERROR,
              "No free buffers on auth_free_q" );
          } /* Get a UIM command buffer from the UIM free queue. */
        #endif
        }
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
        else /* R-UIM is not available */
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
#endif /* FEATURE_UIM_RUIM */
#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
        {
          auth_count++;
          /* Put new COUNTsp in NV */
          nv_item.count.nam = auth_stored_NAM_index;
          nv_item.count.count = auth_count;
          auth_put_nv_item( NV_COUNT_I, &nv_item );

#ifdef EFS_HAVE_SYNC
          /* -----------------------------
          ** Kick watchdog and reset timer
          ** ----------------------------- */
          auth_dog_report();

          M1X_MSG( DCP, LEGACY_MED,
            "Synchronizing the EFS");
          res = efs_sync("/");
          if (res != 0)
          {
            ERR_FATAL( "EFS sync failed",0,0,0);
          }
          M1X_MSG( DCP, LEGACY_MED,
            "EFS sync after incrementing count is successful");
#endif /* EFS_HAVE_SYNC */

        }
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */
      } /* end case - AUTH_INC_COUNT_F */
      break;

    case AUTH_START_TIMER_F:
      {
        /* Inform the client that the command has been received. */
        cmd_ptr->hdr.status = AUTH_RECEIVED_S;
        if (cmd_ptr->hdr.cmd_hdr.task_ptr != NULL)
        {
          (void) rex_set_sigs( cmd_ptr->hdr.cmd_hdr.task_ptr,
                                cmd_ptr->hdr.cmd_hdr.sigs );
        }

        /* Set timer in case the Base Station leaves us hanging */
        M1X_MSG( DCP, LEGACY_MED,
          "Setting SSD timer" );
        (void) rex_set_timer( &auth_ssd_timer, CAI_T64M );
      } /* end case - AUTH_START_TIMER_F */
      break;

    case AUTH_RESET_F:
      {
        /* Inform the client that the command has been received. */
        cmd_ptr->hdr.status = AUTH_RECEIVED_S;
        if (cmd_ptr->hdr.cmd_hdr.task_ptr != NULL)
        {
          (void) rex_set_sigs( cmd_ptr->hdr.cmd_hdr.task_ptr,
                                cmd_ptr->hdr.cmd_hdr.sigs );
        }

        /* -------------------------------------------------------------
        ** Initialize boolean indicators.  This allows us to detect when
        ** operations are done out of order i.e. a key generation before
        ** keys are saved or an encryption request before keys are
        ** generated.
        ** ------------------------------------------------------------- */
        auth_saved_registers = FALSE;
        auth_valid_CMEA_key = FALSE;
        auth_valid_PLCM = FALSE;
        M1X_MSG( DCP, LEGACY_MED,
          "AUTH state reset" );
      } /* end case - AUTH_RESET_F */
      break;

    case AUTH_RESYNC_F:
      {
        /* Inform the client that the command has been received. */
        cmd_ptr->hdr.status = AUTH_RECEIVED_S;
        if (cmd_ptr->hdr.cmd_hdr.task_ptr != NULL)
        {
          (void) rex_set_sigs( cmd_ptr->hdr.cmd_hdr.task_ptr,
                                cmd_ptr->hdr.cmd_hdr.sigs );
        }
        if (cmd_ptr->resync.rpt_function != NULL)
        {
          auth_rpt_buf.rpt_type = AUTH_RESYNC_R;

          /* Fill in necessary fields of command block */
          auth_rpt_buf.rpt_hdr.task_ptr = NULL;

          /* --------------------------------
          ** Queue report for requesting task
          ** -------------------------------- */
          (*(cmd_ptr->resync.rpt_function))( &auth_rpt_buf );
          M1X_MSG( DCP, LEGACY_MED,
            "AUTH synchronized" );
        }
      } /* end case - AUTH_RESYNC_F */
      break;

#ifdef FEATURE_UIM_RUIM
    case AUTH_BS_CHAL_F:
      {
        /* We do not want the RUIM to power down as the SSD update is in
           progress and RANDBS is in temporary memory in the SIM */
        (void) uim_power_control ( UIM_AUTH, TRUE );
        cmd_ptr->hdr.status = AUTH_RECEIVED_S;
        if (cmd_ptr->hdr.cmd_hdr.task_ptr != NULL)
        {
           (void) rex_set_sigs( cmd_ptr->hdr.cmd_hdr.task_ptr,
                                          cmd_ptr->hdr.cmd_hdr.sigs );
        }
        #ifdef FEATURE_CSIM
        (void) rex_clr_sigs( AUTH_TCB_PTR, AUTH_UIM_CMD_RSP_SIG );
        /* Read from MMGSDI and wait for the response. */
        status = mmgsdi_session_bs_chal( cdma.session_id,
                                         cmd_ptr->chal.randseed,
                                         auth_mmgsdi_cb ,
                                         0 );
        if( status != MMGSDI_SUCCESS )
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "MMGSDI read failed for AUTH_BS_CHAL_F, status = %d",
            status );
          auth_mmgsdi_rpt_status = status;
        }
        else
        {
          (void) auth_wait( AUTH_UIM_CMD_RSP_SIG );
          if( auth_cnf == TRUE )
          {
            if( auth_mmgsdi_rpt_ptr != NULL )
            {
              auth_mmgsdi_rpt_status =
                auth_mmgsdi_rpt_ptr->session_bs_chal_cnf.response_header.mmgsdi_status;
            }
            else
            {
              M1X_MSG( DCP, LEGACY_ERROR,
                "auth_mmgsdi_rpt_ptr is NULL");
              return;
            }
          }
        }
        if (cmd_ptr->chal.rpt_function != NULL)
        {
          auth_rpt_buf.rpt_type = AUTH_BS_CHAL_R;
          /* Copy the UIM response to the report buffer. */
          if ( auth_mmgsdi_rpt_status == MMGSDI_SUCCESS )
          {
            auth_rpt_buf.rpt_status = UIM_PASS;
          }
          else
          {
            auth_rpt_buf.rpt_status = UIM_FAIL;
          }
          if( auth_mmgsdi_rpt_ptr != NULL )
          {
            auth_rpt_buf.rpt.randbs = auth_mmgsdi_rpt_ptr->session_bs_chal_cnf.randbs;
          }
          else
          {
            M1X_MSG( DCP, LEGACY_ERROR,
              "auth_mmgsdi_rpt_ptr is NULL");
            return;
          }
          /* Fill in necessary fields of command block */
          auth_rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to the Requesting task */
          (*( cmd_ptr->chal.rpt_function ))( &auth_rpt_buf );
          M1X_MSG( DCP, LEGACY_MED,
            "Reporting RUIM Access" );

        } /* (cmd_ptr->access_ruim.rpt_function != NULL) */
        #elif defined (FEATURE_UIM_RUIM)
        /* Retrieve a UIM command buffer from the queue. */
        if ((uim_cmd_ptr = (uim_cmd_type*) q_get( &uim_free_q )) != NULL)
        {
          uim_cmd_ptr->hdr.command = UIM_BS_CHAL_F;
          uim_cmd_ptr->bs_chal.randseed = cmd_ptr->chal.randseed;

          /* Send the command and wait for the response. */
          auth_send_uim_command(uim_cmd_ptr);

          /* Report back that Access was completed, if requested */
          if (cmd_ptr->chal.rpt_function != NULL)
          {
             auth_rpt_buf.rpt_type = AUTH_BS_CHAL_R;

             /* Copy the UIM response to the report buffer. */
             auth_rpt_buf.rpt_status = uim_rpt_ptr->rpt_status;
             auth_rpt_buf.rpt.randbs = uim_rpt_ptr->rpt.bs_chal.randbs;

             /* Fill in necessary fields of command block */
             auth_rpt_buf.rpt_hdr.task_ptr = NULL;

             /* Report to the Requesting task */
             (*(cmd_ptr->chal.rpt_function))( &auth_rpt_buf );
             M1X_MSG( DCP, LEGACY_MED,
               "Reporting RUIM Access" );

          } /* (cmd_ptr->access_ruim.rpt_function != NULL) */
        }
        else /* No UIM command buffers on the free queue. */
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "No free buffers on uim_free_q" );
        } /* Get a UIM command buffer from the UIM free queue. */
        #endif
        M1X_MSG( DCP, LEGACY_MED,
          "Received Base station Challenge command " );
      } /* end case - AUTH_BS_CHAL_F */
    break;
#endif /* FEATURE_UIM_RUIM */

#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
    case AUTH_UPDATE_A_KEY_TEMP_F:

      cmd_ptr->hdr.status = AUTH_RECEIVED_S;
      if (cmd_ptr->hdr.cmd_hdr.task_ptr != NULL)
      {
          (void) rex_set_sigs( cmd_ptr->hdr.cmd_hdr.task_ptr,
                        cmd_ptr->hdr.cmd_hdr.sigs );
      }

      /* copy the contents of temp_a_key in the command to a_key_temp */
      qw_equ(a_key_temp, cmd_ptr->temp.a_key);

      a_key_temp_valid = TRUE;

      if (cmd_ptr->temp.rpt_function != NULL)
      {
        auth_rpt_buf.rpt_type = AUTH_UPDATE_A_KEY_TEMP_R;

        /* Fill in necessary fields of command block */
        auth_rpt_buf.rpt_hdr.task_ptr = NULL;

        /* --------------------------------
        ** Queue report for requesting task
        ** -------------------------------- */
        (*(cmd_ptr->temp.rpt_function))( &auth_rpt_buf );
        M1X_MSG( DCP, LEGACY_MED,
          "Updating A_KEY_TEMP" );
      }
      break;

    case AUTH_COMMIT_KEYS_F:

      cmd_ptr->hdr.status = AUTH_RECEIVED_S;

      /* update the SSD */
      if (new_ssd_valid)
         (void) CAVE_update_SSD();

      return_status = CAVE_update_A_key_683( a_key_temp,
                              cmd_ptr->commit_keys.nam_index );

      /* We use the report in otasp to check the result of A-Key commit */
      if (cmd_ptr->commit_keys.rpt_function != NULL)
      {
          auth_rpt_buf.rpt_type = AUTH_COMMIT_KEYS_R;

          /* Fill in necessary fields of command block */
          auth_rpt_buf.rpt_hdr.task_ptr = NULL;

          auth_rpt_buf.rpt.commit_keys_ok = return_status;

          /* --------------------------------
          ** Queue report for requesting task
          ** -------------------------------- */
          (*(cmd_ptr->commit_keys.rpt_function))( &auth_rpt_buf );
          M1X_MSG( DCP, LEGACY_MED,
            "Reporting keys committed" );
      }

      /* If specified in request then signal the requesting task. */
      /* signal MC that committing has been done */
      if (cmd_ptr->hdr.cmd_hdr.task_ptr != NULL)
      {
         (void) rex_set_sigs( cmd_ptr->hdr.cmd_hdr.task_ptr,
                               cmd_ptr->hdr.cmd_hdr.sigs );
      }

      break;


    case AUTH_SIGNATURE_SPASM_F:

      M1X_MSG( DCP, LEGACY_MED,
        "Process AUTH_SIG_SPASM_F");

      cmd_ptr->hdr.status = AUTH_RECEIVED_S;
      if (cmd_ptr->hdr.cmd_hdr.task_ptr != NULL)
      {
         (void) rex_set_sigs( cmd_ptr->hdr.cmd_hdr.task_ptr,
                           cmd_ptr->hdr.cmd_hdr.sigs );
      }

      /* ------------------------------------------------------------
      ** We use the SSD_A from NV, so use_new_ssd is set to FALSE.
      ** Save_registers parameter is FALSE. See IS-683A 3.3.7.
      ** ------------------------------------------------------------ */
      auth_sig = CAVE_auth_signature( cmd_ptr->sig.rand_chal,
            cmd_ptr->sig.auth_data, FALSE, FALSE, TRUE );

      M1X_MSG( DCP, LEGACY_MED,
        "rand %lx, auth_data %lx",
        cmd_ptr->sig.rand_chal,
        cmd_ptr->sig.auth_data);

      M1X_MSG( DCP, LEGACY_MED,
        "new_ssd FALSE, save_reg FALSE, val_spasm TRUE");

      /* Report back auth signature, if requested */
      if (cmd_ptr->sig.rpt_function != NULL)
      {
         auth_rpt_buf.rpt_type = AUTH_SIGNATURE_SPASM_R;
         auth_rpt_buf.rpt.auth = auth_sig;

         /* Fill in necessary fields of command block */
         auth_rpt_buf.rpt_hdr.task_ptr = NULL;

         /* --------------------------------
         ** Queue report for requesting task
         ** -------------------------------- */
         (*(cmd_ptr->sig.rpt_function))( &auth_rpt_buf );
         M1X_MSG( DCP, LEGACY_MED,
           "Reporting auth %ld",
           auth_rpt_buf.rpt.auth );
      }

      break;


#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */


#if ( !defined (FEATURE_CSIM) ) && ( defined (FEATURE_UIM_RUIM) )
    case AUTH_ACCESS_RUIM_F:
      {
        /* Inform the client that the command has been received. */
        cmd_ptr->hdr.status = AUTH_RECEIVED_S;
        if (cmd_ptr->hdr.cmd_hdr.task_ptr != NULL)
        {
          (void) rex_set_sigs( cmd_ptr->hdr.cmd_hdr.task_ptr,
                          cmd_ptr->hdr.cmd_hdr.sigs );
        }
        /* Retrieve a UIM command buffer from the queue. */
        if ((uim_cmd_ptr = (uim_cmd_type*) q_get( &uim_free_q )) != NULL)
        {
          uim_cmd_ptr->access_uim.hdr.command = UIM_ACCESS_F;
          uim_cmd_ptr->access_uim.item = cmd_ptr->access_ruim.item;
          uim_cmd_ptr->access_uim.access = cmd_ptr->access_ruim.access;
          uim_cmd_ptr->access_uim.rec_mode = UIM_ABSOLUTE;
          uim_cmd_ptr->access_uim.num_bytes = cmd_ptr->access_ruim.num_bytes;
          uim_cmd_ptr->access_uim.offset = cmd_ptr->access_ruim.offset;
          uim_cmd_ptr->access_uim.data_ptr = cmd_ptr->access_ruim.data_ptr;

          /* Send the command and wait for the response. */
          auth_send_uim_command(uim_cmd_ptr);

          /* Report back that Access was completed, if requested */
          if (cmd_ptr->access_ruim.rpt_function != NULL)
          {
             auth_rpt_buf.rpt_type = AUTH_ACCESS_RUIM_R;

             /* Copy the UIM response to the report buffer. */
             auth_rpt_buf.rpt_status = uim_rpt_ptr->rpt_status;

             /* Fill in the number of bytes in the response that we got
                from RUIM into the NV command */
             cmd_ptr->access_ruim.num_bytes_rsp =
               uim_cmd_ptr->access_uim.num_bytes_rsp;

             /* The data is already copied to the client buffer as a result
                of copying the data_ptr from the client auth command to the
                UIM command above.  The UIM server copies the data to the
                client buffer. */

             /* Fill in necessary fields of command block */
             auth_rpt_buf.rpt_hdr.task_ptr = NULL;

             /* Report to the Requesting task */
             (*(cmd_ptr->access_ruim.rpt_function))( &auth_rpt_buf );
             M1X_MSG( DCP, LEGACY_MED,
               "Reporting RUIM Access" );

          } /* (cmd_ptr->access_ruim.rpt_function != NULL) */
        }
        else /* No UIM command buffers on the free queue. */
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "No free buffers on auth_free_q" );
        } /* Get a UIM command buffer from the UIM free queue. */
        M1X_MSG( DCP, LEGACY_MED,
          "Received RUIM Access command " );
      } /* end case - AUTH_ACCESS_RUIM_F */
      break;
#endif
#if  defined (FEATURE_UIM_RUIM) || defined (FEATURE_CSIM)
    case AUTH_RUIM_STATUS_F:
      {
        /* Inform the client that the command has been received. */
        cmd_ptr->hdr.status = AUTH_RECEIVED_S;
        if (cmd_ptr->hdr.cmd_hdr.task_ptr != NULL)
        {
           (void) rex_set_sigs( cmd_ptr->hdr.cmd_hdr.task_ptr,
              cmd_ptr->hdr.cmd_hdr.sigs );
        }
       #ifdef FEATURE_CSIM
        (void) rex_clr_sigs( AUTH_TCB_PTR, AUTH_UIM_CMD_RSP_SIG );
        /* Read from MMGSDI and wait for the response. */
        status = mmgsdi_session_send_card_status( cdma.session_id,
                                                  MMGSDI_STATUS_APP_INITIALIZED,
                                                  MMGSDI_STATUS_TLV_DATA,
                                                  auth_mmgsdi_cb,
                                                  0 );
        if(status != MMGSDI_SUCCESS)
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "MMGSDI read failed for the item AUTH_RUIM_STATUS_F, err_code = %d ",
            status);
          auth_mmgsdi_rpt_status = status;
        }
        else
        {
          (void) auth_wait( AUTH_UIM_CMD_RSP_SIG );
          if( auth_cnf == TRUE )
          {
            if( auth_mmgsdi_rpt_ptr != NULL )
            {
              auth_mmgsdi_rpt_status =
                auth_mmgsdi_rpt_ptr->status_cnf.response_header.mmgsdi_status;
            }
            else
            {
              M1X_MSG( DCP, LEGACY_ERROR,
                "auth_mmgsdi_rpt_ptr is NULL");
              return;
            }
          }
        }
        if ( cmd_ptr->status.rpt_function != NULL )
        {
          auth_rpt_buf.rpt_type = AUTH_RUIM_STATUS_R;

         /* Copy the UIM response to the report buffer. */
          if ( auth_mmgsdi_rpt_status == MMGSDI_SUCCESS )
          {
            auth_rpt_buf.rpt_status = UIM_PASS;
          }
          else
          {
            auth_rpt_buf.rpt_status = UIM_FAIL;
          }

          /* Fill in necessary fields of command block */
          auth_rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to the Requesting task */
          (*(cmd_ptr->status.rpt_function))( &auth_rpt_buf );
          M1X_MSG( DCP, LEGACY_MED,
            "Reporting RUIM/MMGSDI Access" );
        }
        #else
        /* Retrieve a UIM command buffer from the queue. */
        if ((uim_cmd_ptr = (uim_cmd_type*) q_get( &uim_free_q )) != NULL)
        {
          uim_cmd_ptr->status.hdr.command = UIM_STATUS_F;

          /* Send the command and wait for the response. */
          auth_send_uim_command(uim_cmd_ptr);

          /* Report back that Access was completed, if requested */
          if (cmd_ptr->status.rpt_function != NULL)
          {
            auth_rpt_buf.rpt_type = AUTH_RUIM_STATUS_R;

            /* Copy the UIM response to the report buffer. */
            auth_rpt_buf.rpt_status = uim_rpt_ptr->rpt_status;

            /* Fill in necessary fields of command block */
            auth_rpt_buf.rpt_hdr.task_ptr = NULL;

            /* Report to the Requesting task */
            (*(cmd_ptr->status.rpt_function))( &auth_rpt_buf );
            M1X_MSG( DCP, LEGACY_MED,
              "Reporting RUIM Access" );

          } /* (cmd_ptr->access_ruim.rpt_function != NULL) */
        }
        else /* No UIM command buffers on the free queue. */
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "No free buffers on auth_free_q" );
        } /* Get a UIM command buffer from the UIM free queue. */
        M1X_MSG( DCP, LEGACY_MED,
          "Received RUIM STATUS command " );
      #endif
      } /* end case - AUTH_RUIM_STATUS_F */
      break;

#endif /* FEATURE_UIM_RUIM */
    default:
      ERR_FATAL( "Unknown command to AUTH %d",
                           (int) cmd_ptr->hdr.command, 0, 0);
    break;

  } /* end switch (command_type) */


#ifdef FEATURE_1X_CP_MEM_OPT
#if defined(FEATURE_UIM_RUIM) && defined(FEATURE_UIM_RUN_TIME_ENABLE)
  /* Determine if the R-UIM is available for this NAM */
  if (nv_rtre_use_ruim_for_nam(cdma.curr_nam))
#endif /* FEATURE_UIM_RUIM && FEATURE_UIM_RUN_TIME_ENABLE */
  {
#ifdef FEATURE_CSIM
    if( auth_mmgsdi_rpt_ptr != NULL )
    {
      /* Free the dynamically allocated memory for the deep copy of
       * MMGSDI Rpt if required */
      if(auth_mmgsdi_rpt_ptr->read_cnf.read_data.data_len > 0)
      {
        if(auth_mmgsdi_rpt_ptr->read_cnf.read_data.data_ptr != NULL)
        {
          M1X_MSG( DCP, LEGACY_MED,
            "Auth MMGSDI_READ_CNF rpt, free mem for data_ptr:0X%x",
            auth_mmgsdi_rpt_ptr->read_cnf.read_data.data_ptr);
          modem_mem_free(
           auth_mmgsdi_rpt_ptr->read_cnf.read_data.data_ptr,
           MODEM_MEM_CLIENT_1X
                        );
          auth_mmgsdi_rpt_ptr->read_cnf.read_data.data_ptr = NULL;
        } /* End if(data_ptr != NULL) */
        else
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Data ptr is NULL for auth MMGSDI read cnf rpt and data_len:%d",
            auth_mmgsdi_rpt_ptr->read_cnf.read_data.data_len);
        }
      } /* End if (data_len > 0) */

      modem_mem_free( auth_mmgsdi_rpt_ptr, MODEM_MEM_CLIENT_1X );
      auth_mmgsdi_rpt_ptr = NULL;
    }
#else
    if( auth_uim_rsp_buf_ptr != NULL )
    {
      modem_mem_free( auth_uim_rsp_buf_ptr, MODEM_MEM_CLIENT_1X );
      auth_uim_rsp_buf_ptr = NULL;
    }
#endif /* FEATURE_CSIM */
  }
#endif /* FEATURE_1X_CP_MEM_OPT */

} /* auth_process_command() */



/*===========================================================================

FUNCTION AUTH_PRIVATE_LCM_AVAILABLE

DESCRIPTION
  This procedure checks the private lcm to make sure it can be activated.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the new lcm is available, FALSE othewise.

SIDE EFFECTS
  None

===========================================================================*/
boolean auth_private_lcm_available( void )
{
  boolean plcm_available;
    /* Flag if the private long code mask is available */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (auth_init_status == AUTH_UNINITIALIZED)
  {
    M1X_MSG( DCP, LEGACY_MED,
      "Need to initialize AUTH" );
    plcm_available = FALSE;
  }
  else if (!auth_valid_PLCM)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Need to generate plcm first");
    plcm_available = FALSE;
  }
  else
  {
    plcm_available = TRUE;
  }
  M1X_MSG( DCP, LEGACY_MED,
    "PLCM is ready? %d",
    plcm_available);

  return (plcm_available);
} /* auth_private_lcm_available() */

/*===========================================================================

FUNCTION AUTH_GET_PRIVATE_LCM

DESCRIPTION
  This procedure gets the private lcm.

DEPENDENCIES
  None

RETURN VALUE
  PLCM .

SIDE EFFECTS
  None

===========================================================================*/
void auth_get_private_lcm( qword qplcm )
{
  ulpn_type *plcm_ptr;
    /* Private long code mask returned by CAVE */
	
  plcm_ptr = (ulpn_type *)qplcm;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  CAVE_private_lcm(plcm_ptr);
} /* auth_get_private_lcm */

/*===========================================================================

FUNCTION AUTH_GET_COUNT

DESCRIPTION
  This procedure retrieves the current value of COUNTsp.

DEPENDENCIES
  None

RETURN VALUE
  Returns COUNTsp, or zero if no value is available.

SIDE EFFECTS
  None

===========================================================================*/
byte auth_get_count( void )
{
  return (auth_count);
} /* auth_get_count() */



#ifdef FEATURE_UIM_RUIM


/*===========================================================================

FUNCTION AUTH_CALCULATE_DIGITS

DESCRIPTION
  This function packs the dialed digits that need to be sent to the RUIM. If
  the number of digits is less than 6, the leading digits are zero padded.
  A max of 6 digits are packed in a 3 byte array.

DEPENDENCIES
  The digits passed in must be in a byte array and be encoded in BCD.

RETURN VALUE
  The computed digits

SIDE EFFECTS
  None

===========================================================================*/
void auth_calculate_digits
(
  byte digits[],
    /* Dialed digits or destination address digits */
  byte array_len,
    /* The number of digits in the above array */
  boolean digit_mode,
    /* TRUE if digits are ASCII, FALSE if digits are DTMF */
  byte output_digits[]
    /* output array */
)
{
  byte last_digit;
    /* Index of last dialed digit in cdma.dial.digits array */
  byte num_digits;
    /* The number of digits to be used in auth_data */
  byte i;
    /* Index into cdma.dial.digits array */
  byte converted_digit;
    /* Digit converted to 4 bit DTMF code */
  byte j;
    /* Index into the output array */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  last_digit = array_len - 1;

  /* compute how many digits we are to use */
  num_digits = (array_len < 6) ? array_len : 6;

 /* initialize the digit array */
  for (j = 0; j < RUIM_RUN_CAVE_DIGITS; j++ )
  {
     output_digits[j]  = 0;
  }
  j = 2;
  /* j is left to 2 as we start filling the array from the end */

  /* Then replace however much is necessary with supplied digits */
  for (i=0; i < num_digits; i++)
  {
    /* Get the next digit, which can be ascii or DTMF already */
    converted_digit = digits[last_digit - i];

    /* If digit is not DTMF, convert it to DTMF as per 6.3.12.1.6 */
    if (digit_mode)
    {
      /* Digits, star, and pound are encoded as real DTMF codes */
      if (converted_digit == '*')
      {
        converted_digit = AUTH_DIAL_STAR;
      }
      else if (converted_digit == '#')
      {
        converted_digit = AUTH_DIAL_PND;
      }
      else if (converted_digit == '0')
      {
        converted_digit = AUTH_DIAL_0;
      }
      else if (converted_digit >= '1' && converted_digit <= '9')
      {
        converted_digit -= '0';
      }
      else
      {
        /* All others are encoded as DTMF version of last decimal digit
         * of ASCII character code.  That is, the ASCII code modulo 10.
         */
        converted_digit %= 10;
        if (converted_digit == 0)
        {
          /* A zero is encoded as '1010' */
          converted_digit = AUTH_DIAL_0;
        }
      }
    }  /* digit mode */
    if ((i+1) % 2 == 0)
    {
       output_digits[j--] |= ((converted_digit & 0x0FL) << 4);
    }
    else
    {
       output_digits[j] = (converted_digit & 0x0FL);
    }
  }  /* for loop */
} /* auth_calculate_digits () */

#endif /*  FEATURE_UIM_RUIM */


/*===========================================================================

FUNCTION AUTH_ENCRYPT_INFO_RECS

DESCRIPTION
  This procedure encrypts type specific fields in reverse traffic channel
  message.  To accomplish this I first make a temporary
  copy of the packed message body.  Then I unpack each set of type
  specific fields from the copy, encrypt them, and copy them back
  into the original message to be transmitted.  If a record_len is
  only one byte, another byte is added.  Once the message to be
  transmitted is lengthened it is necessary to copy all remaining
  fields (still encrypting the type specific fields, of course).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The length of Traffic Channel message to transmit may be increased.

===========================================================================*/
void auth_encrypt_info_recs
(
  word *len_ptr,
    /* Length of message in bits */
  byte *buf_ptr,
    /* Pointer to body of packed external-format message */
  caii_tx_msg_type *int_ptr,
    /* Pointer to internal message to be transmitted */
  word rec_start_pos,
    /* Start position of the info record after num_rec field */
  byte num_recs
    /* Number of info records to be processed */
)
{

  byte msg_buf[CAI_REV_TC_MAX_MSG_BYTES];
    /* Buffer to hold copy of message body */
  word crypt_pos;
    /* Position of end of message to encrypt */
  word msg_pos;
    /* Pointer into packed external message */
  word num_bytes;
    /* Number of bytes in flash with info message */
  byte num_data_bytes;
    /* Number of data bytes to be encrypted */
  word i, j;
    /* Loop counters */
  boolean bytes_added = FALSE;
    /* Flag if bytes have been added to the message */
  byte *buf_index = buf_ptr;
    /* Index into packed external message body, used to copy it */
  byte rec_type;
    /* The type of an information record in this message */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Pay the price to copy the entire packed message body */
  num_bytes = (*len_ptr + AUTH_EIGHT_BITS - 1) / AUTH_EIGHT_BITS;
  for (i = 0; i < num_bytes; i++)
  {
    msg_buf[i] = *buf_index;
    buf_index++;
  }

  /* Check if the num of bytes are within the limit */
  if (num_bytes > CAI_REV_TC_MAX_MSG_BYTES)
  {
     M1X_MSG( DCP, LEGACY_ERROR,
       "Message buffer overflow");
     return;
  }

  /* ----------------------------------------------------------------------
  ** Now step through each info record, extract the type specific fields,
  ** add a byte if necessary, encrypt it, and place it back in the message.
  ** ---------------------------------------------------------------------- */
  crypt_pos = msg_pos = rec_start_pos;

  for (i = 0; i < num_recs; i++)
  {
    if (bytes_added)
    {
      /* We need to copy the record type to the encrypted message */
      rec_type = b_unpackb( msg_buf, msg_pos, AUTH_RECORD_TYPE_SIZE);
      b_packb( rec_type, buf_ptr, crypt_pos, AUTH_RECORD_TYPE_SIZE);
    }
    msg_pos += AUTH_RECORD_TYPE_SIZE;
    crypt_pos += AUTH_RECORD_TYPE_SIZE;

    num_data_bytes = b_unpackb( msg_buf, msg_pos, AUTH_RECORD_LEN_SIZE);
    if (bytes_added)
    {
      /* We need to copy the record length to the encrypted message */
      b_packb( num_data_bytes, buf_ptr, crypt_pos, AUTH_RECORD_LEN_SIZE);
    }
    msg_pos += AUTH_RECORD_LEN_SIZE;
    crypt_pos += AUTH_RECORD_LEN_SIZE;

    /* Unpack the type specific fields */
    for (j = 0; j < num_data_bytes; j++)
    {
      auth_temp_buf[j] = b_unpackb( msg_buf, msg_pos, AUTH_EIGHT_BITS);
      msg_pos += AUTH_EIGHT_BITS;
    }

    if (num_data_bytes == 1)
    {
      /* Add an extra zero byte */
      bytes_added = TRUE;
      (*len_ptr) += AUTH_EIGHT_BITS;
      num_data_bytes = 2;
      auth_temp_buf[1] = 0;
    }

    CAVE_encrypt( auth_temp_buf, num_data_bytes );

    for (j = 0; j < num_data_bytes; j++)
    {
      b_packb( auth_temp_buf[j], buf_ptr, crypt_pos, AUTH_EIGHT_BITS );
      crypt_pos += AUTH_EIGHT_BITS;
    }
  }
} /* auth_encrypt_info_recs() */ /*lint !e715 */

/*===========================================================================

FUNCTION AUTH_ENCRYPT_FLASH

DESCRIPTION
  This procedure encrypts CDMA reverse traffic channel Flash with
  Information message.  To accomplish this I first make a temporary
  copy of the packed message body.  Then I unpack each set of type
  specific fields from the copy, encrypt them, and copy them back
  into the original message to be transmitted.  If a record_len is
  only one byte, another byte is added.  Once the message to be
  transmitted is lengthened it is necessary to copy all remaining
  fields (still encrypting the type specific fields, of course).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The length of Traffic Channel message to transmit may be increased.

===========================================================================*/
void auth_encrypt_flash
(
  word *len_ptr,
    /* Length of message in bits */
  byte *buf_ptr,
    /* Pointer to body of packed external-format message */
  caii_tx_msg_type *int_ptr
    /* Pointer to internal message to be transmitted */
)
{

  byte msg_buf[CAI_REV_TC_MAX_MSG_BYTES];
    /* Buffer to hold copy of message body */
  word crypt_pos;
    /* Position of end of message to encrypt */
  word msg_pos;
    /* Pointer into packed external message */
  word num_bytes;
    /* Number of bytes in flash with info message */
  byte num_data_bytes;
    /* Number of data bytes to be encrypted */
  word i, j;
    /* Loop counters */
  boolean bytes_added = FALSE;
    /* Flag if bytes have been added to the message */
  byte *buf_index = buf_ptr;
    /* Index into packed external message body, used to copy it */
  byte rec_type;
    /* The type of an information record in this message */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Pay the price to copy the entire packed message body */
  num_bytes = (*len_ptr + AUTH_EIGHT_BITS - 1) / AUTH_EIGHT_BITS;
  for (i = 0; i < num_bytes; i++)
  {
    msg_buf[i] = *buf_index;
    buf_index++;
  }

  /* Check if the num of bytes are within the limit */
  if (num_bytes > CAI_REV_TC_MAX_MSG_BYTES)
  {
     M1X_MSG( DCP, LEGACY_ERROR,
       "Message buffer overflow");
     return;
  }

  /* ----------------------------------------------------------------------
  ** Now step through each info record, extract the type specific fields,
  ** add a byte if necessary, encrypt it, and place it back in the message.
  ** ---------------------------------------------------------------------- */
  msg_pos = crypt_pos = FPOS( cai_flash_fix_type, hdr.encryption ) +
                        FSIZ( cai_flash_fix_type, hdr.encryption );

  for (i = 0; i < int_ptr->flash.num_recs; i++)
  {
    if (bytes_added)
    {
      /* We need to copy the record type to the encrypted message */
      rec_type = b_unpackb( msg_buf, msg_pos, AUTH_RECORD_TYPE_SIZE);
      b_packb( rec_type, buf_ptr, crypt_pos, AUTH_RECORD_TYPE_SIZE);
    }
    msg_pos += AUTH_RECORD_TYPE_SIZE;
    crypt_pos += AUTH_RECORD_TYPE_SIZE;

    num_data_bytes = b_unpackb( msg_buf, msg_pos, AUTH_RECORD_LEN_SIZE);
    if (bytes_added)
    {
      /* We need to copy the record length to the encrypted message */
      b_packb( num_data_bytes, buf_ptr, crypt_pos, AUTH_RECORD_LEN_SIZE);
    }
    msg_pos += AUTH_RECORD_LEN_SIZE;
    crypt_pos += AUTH_RECORD_LEN_SIZE;

    /* Unpack the type specific fields */
    for (j = 0; j < num_data_bytes; j++)
    {
      auth_temp_buf[j] = b_unpackb( msg_buf, msg_pos, AUTH_EIGHT_BITS);
      msg_pos += AUTH_EIGHT_BITS;
    }

    if (num_data_bytes == 1)
    {
      /* Add an extra zero byte */
      bytes_added = TRUE;
      (*len_ptr) += AUTH_EIGHT_BITS;
      num_data_bytes = 2;
      auth_temp_buf[1] = 0;
    }

    CAVE_encrypt( auth_temp_buf, num_data_bytes );

    for (j = 0; j < num_data_bytes; j++)
    {
      b_packb( auth_temp_buf[j], buf_ptr, crypt_pos, AUTH_EIGHT_BITS );
      crypt_pos += AUTH_EIGHT_BITS;
    }
  }
} /* auth_encrypt_flash() */

/*===========================================================================

FUNCTION AUTH_ENCRYPT_ORIG_C

DESCRIPTION
  This procedure encrypts CDMA reverse traffic channel origination
  continuation message.  I copy the packed digit data into a temporary
  buffer, encrypt it, and then put it back.

DEPENDENCIES
  The Origination Continuation Message must have at least one digit in it.

RETURN VALUE
  None

SIDE EFFECTS
  The length of Traffic Channel message to transmit may be increased.

===========================================================================*/
void auth_encrypt_orig_c
(
  word *len_ptr,
    /* Length of message in bits */
  byte *buf_ptr,
    /* Pointer to body of packed external-format message */
  caii_tx_msg_type *int_ptr
    /* Pointer to internal message to be transmitted */
)
{
  word end_pos;
    /* Position of end of message to encrypt */
  word data_pos;
    /* Position of data in packed external message */
  boolean eight_bit_digits;
    /* Whether or not the digits are eight bits (as opposed to four) */
  byte num_digits;
    /* Number of digits in origination continuation */
  byte num_data_bytes;
    /* Number of data bytes to be encrypted */
  word i;
    /* Loop counter */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy some values to make the code more readable */
  eight_bit_digits = (int_ptr->orig_c.digit_mode == 1);
  num_digits = int_ptr->orig_c.num_fields;
  end_pos = *len_ptr;

  /* --------------------------------------------------------------------
  ** Determine how many bytes of data we have to encrypt, and add bits
  ** to ensure we have an integral number of bytes greater than one.
  ** -------------------------------------------------------------------- */
  num_data_bytes = (eight_bit_digits) ? num_digits : ((num_digits + 1) / 2);

  if ((!eight_bit_digits) && ((num_digits % 2) != 0))
  {
    /* ------------------------------------------------
    ** We have an odd number of 4 bit digits. Fill with
    ** four zero bits to allign to a byte boundry.
    ** ------------------------------------------------ */
    b_packb( 0, buf_ptr, end_pos, AUTH_FOUR_BITS );
    end_pos += AUTH_FOUR_BITS;
  }

  if (((eight_bit_digits) && (num_digits == 1)) ||
      ((!eight_bit_digits) && ((num_digits == 1) || (num_digits == 2))))
  {
    /* We have only one byte of data. Fill with eight more zero bits. */
    b_packb( 0, buf_ptr, end_pos, AUTH_EIGHT_BITS );
    end_pos += AUTH_EIGHT_BITS;
    num_data_bytes++;
  }

  /* Update total length to reflect if it has been increased */
  *len_ptr = end_pos;

  /* ------------------------------------------
  ** Extract data to be encrypted, which is now
  ** an integral number of bytes greater than 1
  ** ------------------------------------------ */
  data_pos = FPOS( cai_orig_c_type, var );

  for (i=0; i < num_data_bytes; i++)
  {
    auth_temp_buf[i] = b_unpackb( buf_ptr, data_pos, AUTH_EIGHT_BITS );
    data_pos += AUTH_EIGHT_BITS;
  }

  CAVE_encrypt( auth_temp_buf, num_data_bytes );

  /* Now write encrypted data back over original data */
  data_pos = FPOS( cai_orig_c_type, var );

  for (i=0; i < num_data_bytes; i++)
  {
    b_packb( auth_temp_buf[i], buf_ptr, data_pos, AUTH_EIGHT_BITS );
    data_pos += AUTH_EIGHT_BITS;
  }

} /* auth_encrypt_orig_c() */

/*===========================================================================

FUNCTION AUTH_CRYPT_BURST_DTMF

DESCRIPTION
  This procedure encrypts/decrypts CDMA Burst DTMF message.  I copy the
  packed digit data into a temporary buffer, encrypt/decrypt it, and
  then put it back.

DEPENDENCIES
  The Burst DTMF Message must have at least one digit in it.

RETURN VALUE
  None

SIDE EFFECTS
  The length of the message may increase if encrypting or decrease if
  decrypting.

===========================================================================*/
void auth_crypt_burst_dtmf
(
  word *len_ptr,
    /* Length of message in bits */
  byte *buf_ptr,
    /* Pointer to body of packed external-format message */
  boolean encrypting
    /* Flag if we are encrypting or decrypting tye message */
)
{
  word end_pos;
    /* Position of end of message to encrypt */
  word data_pos;
    /* Position of data in packed external message */
  byte num_dgts;
    /* Number of digits in origination continuation */
  byte num_data_bytes;
    /* Number of data bytes to be encrypted */
  word i;
    /* Loop counter */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy some values to make the code more readable */
  num_dgts = b_unpackb( buf_ptr, FPOS( cai_sbdtmf_fix_type, num_digits ),
                                 FSIZ( cai_sbdtmf_fix_type, num_digits ));
  end_pos = *len_ptr;

  /* --------------------------------------------------------------------
  ** Determine how many bytes of data we have to encrypt/decrypt, and add
  ** to ensure we have an integral number of bytes greater than one.
  ** -------------------------------------------------------------------- */
  num_data_bytes = (num_dgts + 1) / 2;

  if (encrypting)
  {
    if ((num_dgts % 2) != 0)
    {
      /* ------------------------------------------------
      ** We have an odd number of 4 bit digits. Fill with
      ** four zero bits to allign to a byte boundry.
      ** ------------------------------------------------ */
      b_packb( 0, buf_ptr, end_pos, AUTH_FOUR_BITS );
      end_pos += AUTH_FOUR_BITS;
    }

    if ((num_dgts == 1) || (num_dgts == 2))
    {
      /* We have only one byte of data. Fill with eight more zero bits. */
      b_packb( 0, buf_ptr, end_pos, AUTH_EIGHT_BITS );
      end_pos += AUTH_EIGHT_BITS;
      num_data_bytes++;
    }

    /* Update total length to reflect if it has been increased */
    *len_ptr = end_pos;
  }
  else /* decrypting */
  {
    if (num_data_bytes == 1)
    {
      /* -------------------------------------------
      ** If there is only one bytes worth of digits,
      ** there are two bytes of data to decrypt.
      ** ------------------------------------------- */
      num_data_bytes = AUTH_TWO_BYTES;
      /* Adjust length to reflect we will be removing a byte. */
      *len_ptr -= AUTH_EIGHT_BITS;
    }

    if ((num_dgts % 2) != 0)
    {
      /* ----------------------------------------------
      ** We have an odd number of 4 bit digits.  Adjust
      ** length to reflect we will be removing 4 bits.
      ** ---------------------------------------------- */
      *len_ptr -= AUTH_FOUR_BITS;
    }
  }

  /* -------------------------------------------------
  ** Extract data to be encrypted/decrypted.  The data
  ** is now an integral number of bytes greater than 1
  ** ------------------------------------------------- */
  data_pos = FPOS( cai_sbdtmf_type, var );

  for (i=0; i < num_data_bytes; i++)
  {
    auth_temp_buf[i] = b_unpackb( buf_ptr, data_pos, AUTH_EIGHT_BITS );
    data_pos += AUTH_EIGHT_BITS;
  }

  CAVE_encrypt( auth_temp_buf, num_data_bytes );

  /* Now write encrypted data back over original data */
  data_pos = FPOS( cai_sbdtmf_type, var );

  for (i=0; i < num_data_bytes; i++)
  {
    b_packb( auth_temp_buf[i], buf_ptr, data_pos, AUTH_EIGHT_BITS );
    data_pos += AUTH_EIGHT_BITS;
  }
} /* auth_crypt_burst_dtmf() */

/*===========================================================================

FUNCTION AUTH_CRYPT_DTMF_ORD

DESCRIPTION
  This procedure encrypts/decrypts a CDMA Continuous DTMF Tone Order.
  I copy the data to be encrypted/decrypted into a temporary buffer,
  encrypt/decrypt it, and then put it back.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void auth_crypt_dtmf_ord
(
  word *len_ptr,
    /* Length of message in bits */
  byte *buf_ptr,
    /* Pointer to body of packed external-format message */
  boolean encrypting
    /* Flag if we are encrypting or decrypting */
)
{
  word data_pos;
    /* Position of data in packed external message */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* -----------------------------
  ** Extract data to be encrypted.
  ** ----------------------------- */
  if (encrypting)
  {
    data_pos = FPOS( cai_rtc_ord_type, ordq.add_record_len );
  }
  else /* decrypting */
  {
    data_pos = FPOS( cai_ftc_ord_type, ordq.add_record_len );
  }

  /* First byte is 3 bit ADD_RECORD_LEN field and 5 bits of ORDQ */
  auth_temp_buf[0] = b_unpackb( buf_ptr, data_pos, AUTH_EIGHT_BITS );
  data_pos += AUTH_EIGHT_BITS;

  if (encrypting)
  {
    /* ----------------------------------------------------------------
    ** Second byte is last 3 bits of ORDQ.  Shift these so they are the
    ** most significant bits of the byte, and fill the rest with zeros.
    ** ---------------------------------------------------------------- */
    auth_temp_buf[1] = (byte) (b_unpackb( buf_ptr, data_pos,
                                     AUTH_THREE_BITS ) << 5);

    /* ---------------------------------------------------------------------
    ** If we are encrypting, 5 'reserved' bits of the message are encrypted.
    ** Since the reserved bits are added after the encryption, increasing
    ** the message length by 5 bits effectively decreases the number of
    ** reserved bits (added later in order to allign the message to a byte
    ** boundary) by 5.
    ** --------------------------------------------------------------------- */
    *len_ptr += AUTH_FIVE_BITS;
  }
  else /* decrypting */
  {
    /* ---------------------------------------------------------------------
    ** If we are decrypting, 5 'reserved' bits of the message are decrypted.
    ** Since these bits were never really part of the message, decreasing
    ** the length by 5 bits effectively removes them.
    ** --------------------------------------------------------------------- */
    auth_temp_buf[1] = b_unpackb( buf_ptr, data_pos, AUTH_EIGHT_BITS );
    *len_ptr -= AUTH_FIVE_BITS;
  }

  CAVE_encrypt( auth_temp_buf, AUTH_TWO_BYTES );

  if (encrypting)
  {
    data_pos = FPOS( cai_rtc_ord_type, ordq.add_record_len );
  }
  else /* decrypting */
  {
    data_pos = FPOS( cai_ftc_ord_type, ordq.add_record_len );
  }
  b_packb( auth_temp_buf[0], buf_ptr, data_pos, AUTH_EIGHT_BITS );
  data_pos += AUTH_EIGHT_BITS;
  b_packb( auth_temp_buf[1], buf_ptr, data_pos, AUTH_EIGHT_BITS );

} /* auth_crypt_dtmf_ord() */

/*===========================================================================

FUNCTION AUTH_CRYPT_DATA_BURST

DESCRIPTION
  This procedure encrypts/decrypts CDMA reverse traffic channel Data Burst
  Message.  I copy the packed chari data into a temporary buffer, encrypt/
  decrypt it, and then put it back.

DEPENDENCIES
  The Data Burst must have at least two chari fields.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void auth_crypt_data_burst
(
  byte *buf_ptr,
    /* Pointer to body of packed external-format message */
  boolean encrypting
    /* Flag if we are encrypting or decrypting */
)
{
  word data_pos;
    /* Position of data in packed external message */
  byte num_fields;
    /* Number of bytes in the un-encrypted data burst message */
  byte num_data_bytes;
    /* Number of bytes of data to be encrypted/decrypted */
  word i;
    /* Loop counter */

  byte burst_type;
    /* Burst type of the Data Burst Message */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  burst_type = b_unpackb( buf_ptr, FPOS( cai_burst_tc_fix_type, burst_type ),
                                   FSIZ( cai_burst_tc_fix_type, burst_type ));

  /* Copy some values to make the code more readable */
  num_fields = b_unpackb( buf_ptr, FPOS( cai_burst_tc_fix_type, num_fields ),
                                   FSIZ( cai_burst_tc_fix_type, num_fields ));

  /* ------------------------------------------
  ** Extract data to be encrypted, which is an
  ** integral number of bytes greater than 1.
  ** ------------------------------------------ */
  data_pos = FPOS( cai_burst_tc_fix_type, num_fields ) +
             FSIZ( cai_burst_tc_fix_type, num_fields );

  /* If BURST_TYPE is equal to '111110' or '111111', */
  /* all CHARi fields after the first two shall be   */
  /* encrypted; otherwise, all CHARi fields shall be */
  /* encrypted.                                      */
  if ((burst_type == CAI_EXTENDED_BURST_TYPE_INTL)
      ||
      (burst_type == CAI_EXTENDED_BURST_TYPE)
     )
  {
    if (num_fields <= 2)
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        " <= 2 bytes of payload for burst %d",
        burst_type);
      return;
    }
    else
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "skip two bytes for burst %d",
        burst_type);
      num_fields -= 2;
      data_pos += ( 2 * AUTH_EIGHT_BITS ) ;
    }
  }

  /* These are the same unless we are dealing with a one-byte data burst */
  num_data_bytes = num_fields;

  if (!encrypting) /* decrypting */
  {
    if (num_fields == 1)
    {
      /* --------------------------------------------
      ** If there is only one byte in the data burst,
      ** there are two bytes of data to decrypt.
      ** -------------------------------------------- */
      num_data_bytes = AUTH_TWO_BYTES;
    }
  }

  for (i=0; i < num_data_bytes; i++)
  {
    auth_temp_buf[i] = b_unpackb( buf_ptr, data_pos, AUTH_EIGHT_BITS );
    data_pos += AUTH_EIGHT_BITS;
  }

  if (encrypting)
  {
    if (num_fields == 1)
    {
      /* We have only one byte of data.  Add another zero byte. */
      auth_temp_buf[1] = 0;
      num_data_bytes = AUTH_TWO_BYTES;
    }
  }

  CAVE_encrypt( auth_temp_buf, num_data_bytes );

  /* Now write encrypted data back over original data */
  data_pos = FPOS( cai_burst_tc_fix_type, num_fields ) +
             FSIZ( cai_burst_tc_fix_type, num_fields );

  if ((burst_type == CAI_EXTENDED_BURST_TYPE_INTL)
      ||
      (burst_type == CAI_EXTENDED_BURST_TYPE)
     )
  {
    data_pos += ( 2 * AUTH_EIGHT_BITS ) ;
  }

  for (i=0; i < num_data_bytes; i++)
  {
    b_packb( auth_temp_buf[i], buf_ptr, data_pos, AUTH_EIGHT_BITS );
    data_pos += AUTH_EIGHT_BITS;
  }
} /* auth_crypt_data_burst() */

#ifdef FEATURE_IS2000_REL_A
/*===========================================================================

FUNCTION AUTH_ENCRYPT_EFWIM

DESCRIPTION
  This procedure encrypts CDMA reverse traffic channel Extended Flash with
  Information message.  To accomplish this I first make a temporary
  copy of the packed message body.  Then I unpack each set of type
  specific fields from the copy, encrypt them, and copy them back
  into the original message to be transmitted.  If a record_len is
  only one byte, another byte is added.  Once the message to be
  transmitted is lengthened it is necessary to copy all remaining
  fields (still encrypting the type specific fields, of course).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The length of Traffic Channel message to transmit may be increased.

===========================================================================*/
void auth_encrypt_efwim
(
  word *len_ptr,
    /* Length of message in bits */
  byte *buf_ptr,
    /* Pointer to body of packed external-format message */
  caii_tx_msg_type *int_ptr
    /* Pointer to internal message to be transmitted */
)
{

  word msg_pos;
    /* Pointer into packed external message */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* ----------------------------------------------------------------------
  ** Now step through each info record, extract the type specific fields,
  ** add a byte if necessary, encrypt it, and place it back in the message.
  ** ---------------------------------------------------------------------- */
  msg_pos = FPOS( cai_efwim_type, con_ref_incl ) +
              FSIZ(cai_efwim_type, con_ref_incl );

  if (int_ptr->efwim.con_ref_incl)
  {
    msg_pos = FPOS(cai_efwim_type, num_rec) + FSIZ(cai_efwim_type, num_rec);
  }
  else
  {
    msg_pos += FSIZ(cai_efwim_type, num_rec);
  }

  auth_encrypt_info_recs(len_ptr, buf_ptr, int_ptr, msg_pos,
                         int_ptr->efwim.num_rec);

} /* auth_encrypt_efwim() */

/*===========================================================================

FUNCTION AUTH_ENCRYPT_EOM_DIGITS

DESCRIPTION
  This procedure encrypts CDMA reverse traffic channel enhanced origination
  message.  I copy the packed digit data into a temporary
  buffer, encrypt it, and then put it back.

DEPENDENCIES
  The Enhanced Origination Message must have at least one digit in it.

RETURN VALUE
  None

SIDE EFFECTS
  The length of Traffic Channel message to transmit may be increased.

===========================================================================*/
void auth_encrypt_eom_digits
(
  word *len_ptr,
    /* Length of message in bits */
  byte *buf_ptr,
    /* Pointer to body of packed external-format message */
  caii_tx_msg_type *int_ptr,
    /* Pointer to internal message to be transmitted */
  word digit_start_pos
    /* Start location of digits */
)
{
  word end_pos;
    /* Position of end of message to encrypt */
  word data_pos;
    /* Position of data in packed external message */
  boolean eight_bit_digits;
    /* Whether or not the digits are eight bits (as opposed to four) */
  byte num_digits;
    /* Number of digits in origination continuation */
  byte num_data_bytes;
    /* Number of data bytes to be encrypted */
  word i;
    /* Loop counter */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy some values to make the code more readable */
  eight_bit_digits = (int_ptr->eom.digit_mode == 1);
  num_digits = int_ptr->eom.num_fields;
  end_pos = *len_ptr;

  /* --------------------------------------------------------------------
  ** Determine how many bytes of data we have to encrypt, and add bits
  ** to ensure we have an integral number of bytes greater than one.
  ** -------------------------------------------------------------------- */
  num_data_bytes = (eight_bit_digits) ? num_digits : ((num_digits + 1) / 2);

  if ((!eight_bit_digits) && ((num_digits % 2) != 0))
  {
    /* ------------------------------------------------
    ** We have an odd number of 4 bit digits. Fill with
    ** four zero bits to allign to a byte boundry.
    ** ------------------------------------------------ */
    b_packb( 0, buf_ptr, end_pos, AUTH_FOUR_BITS );
    end_pos += AUTH_FOUR_BITS;
  }

  if (((eight_bit_digits) && (num_digits == 1)) ||
      ((!eight_bit_digits) && ((num_digits == 1) || (num_digits == 2))))
  {
    /* We have only one byte of data. Fill with eight more zero bits. */
    b_packb( 0, buf_ptr, end_pos, AUTH_EIGHT_BITS );
    end_pos += AUTH_EIGHT_BITS;
    num_data_bytes++;
  }

  /* Update total length to reflect if it has been increased */
  *len_ptr = end_pos;

  /* ------------------------------------------
  ** Extract data to be encrypted, which is now
  ** an integral number of bytes greater than 1
  ** ------------------------------------------ */
  data_pos = digit_start_pos;

  for (i=0; i < num_data_bytes; i++)
  {
    auth_temp_buf[i] = b_unpackb( buf_ptr, data_pos, AUTH_EIGHT_BITS );
    data_pos += AUTH_EIGHT_BITS;
  }

  CAVE_encrypt( auth_temp_buf, num_data_bytes );

  /* Now write encrypted data back over original data */
  data_pos = digit_start_pos;

  for (i=0; i < num_data_bytes; i++)
  {
    b_packb( auth_temp_buf[i], buf_ptr, data_pos, AUTH_EIGHT_BITS );
    data_pos += AUTH_EIGHT_BITS;
  }

} /* auth_encrypt_eom_digits() */

/*===========================================================================

FUNCTION AUTH_DECRYPT_EXTENDED_INFO

DESCRIPTION
  This procedure decrypts CDMA forward traffic channel Extended Flash with
  Information and Extended Alert with information messages.  To accomplish
  this I unpack each set of type specific fields, encrypt them,
  and copy them back into the original received packed message.
  If a record_len is only one byte, the extra byte added for
  encryption is removed and the overall length is decreased.
  Once the received packed message is shortened it is necessary
  to copy all remaining fields (still decrypting the type specific
  fields, of course).

DEPENDENCIES
  None

RETURN VALUE
  TURE if successful. Otherwise return FALSE.

SIDE EFFECTS
  The length of Traffic Channel message may be increased.

===========================================================================*/
boolean auth_decrypt_extended_info
(
  word *len_ptr,
    /* Length of message in bits */
  byte *buf_ptr,
    /* Pointer to body of packed external-format message */
  word start_pos,
    /* Start point of Information Record */
  byte num_recs
    /* Numer of records in this message */
)
{

  word msg_pos;
    /* Current position in encrypted external message */
  byte num_data_bytes;
    /* Number of data bytes to be decrypted */
  word i, j;
    /* Loop counter */
  boolean bytes_removed = FALSE;
    /* Flag if bytes have been removed from the message */
  byte rec_type;
    /* The type of an information record in this message */
  byte rec_len;
    /* The original length of an information record before encryption */
  word total_length = *len_ptr;
    /* The overall length of the encrypted message body */
  boolean status = TRUE;
    /* return status */
  word decrypt_pos;
    /* Pointer to decrypt position */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* ----------------------------------------------------------------------
  ** Now step through each info record, extract the type specific fields,
  ** decrypt it, remove a byte if necessary, and place it back in the message.
  ** ---------------------------------------------------------------------- */
  msg_pos = decrypt_pos = start_pos;

  for (i=0; i<num_recs; i++)
  {
    if (bytes_removed)
    {
      /* We need to slide the record type field up due to removed bytes */
      rec_type = b_unpackb( buf_ptr, msg_pos, AUTH_RECORD_TYPE_SIZE);
      b_packb( rec_type, buf_ptr, decrypt_pos, AUTH_RECORD_TYPE_SIZE);
    }
    msg_pos += AUTH_RECORD_TYPE_SIZE;
    decrypt_pos += AUTH_RECORD_TYPE_SIZE;

    rec_len = b_unpackb( buf_ptr, msg_pos, AUTH_RECORD_LEN_SIZE);
    if (bytes_removed)
    {
      /* We need to slide the record length field up due to removed bytes */
      b_packb( rec_len, buf_ptr, decrypt_pos, AUTH_RECORD_LEN_SIZE);
    }
    msg_pos += AUTH_RECORD_LEN_SIZE;
    decrypt_pos += AUTH_RECORD_LEN_SIZE;

    if (rec_len == 1)
    {
      /* Add an extra zero byte */
      bytes_removed = TRUE;
      (*len_ptr) -= AUTH_EIGHT_BITS;
      num_data_bytes = 2;
    }
    else
    {
      num_data_bytes = rec_len;
    }

    if ((num_data_bytes * AUTH_EIGHT_BITS + msg_pos) > total_length)
    {
      /* --------------------------------------------------------
      ** The info record length would cause us to read beyond the
      ** end of the received message buffer.
      ** -------------------------------------------------------- */
      M1X_MSG( DCP, LEGACY_ERROR,
        "Message Length invalid");
      status = FALSE;
      break;
    }

    /* Unpack the (encrypted) type specific fields */
    for (j = 0; j < num_data_bytes; j++)
    {
      auth_temp_buf[j] = b_unpackb( buf_ptr, msg_pos, AUTH_EIGHT_BITS);
      msg_pos += AUTH_EIGHT_BITS;
    }

    CAVE_encrypt( auth_temp_buf, num_data_bytes );

    for (j = 0; j < rec_len; j++)
    {
      b_packb( auth_temp_buf[j], buf_ptr, decrypt_pos, AUTH_EIGHT_BITS );
      decrypt_pos += AUTH_EIGHT_BITS;
    }
  }

  return status;

} /* auth_decrypt_extended_info() */

#endif /* FEATURE_IS2000_REL_A */

/*===========================================================================

FUNCTION AUTH_DECRYPT_INFO

DESCRIPTION
  This procedure decrypts CDMA reverse traffic channel Flash with
  Information and alert with information messages.  To accomplish
  this I unpack each set of type specific fields, encrypt them,
  and copy them back into the original received packed message.
  If a record_len is only one byte, the extra byte added for
  encryption is removed and the overall length is decreased.
  Once the received packed message is shortened it is necessary
  to copy all remaining fields (still decrypting the type specific
  fields, of course).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The length of Traffic Channel message to transmit may be increased.

===========================================================================*/
void auth_decrypt_info
(
  word *len_ptr,
    /* Length of message in bits */
  byte *buf_ptr
    /* Pointer to body of packed external-format message */
)
{

  word msg_pos;
    /* Current position in encrypted external message */
  word decrypt_pos;
    /* Current position in decrypted message (decrypt_pos <= msg_pos) */
  byte num_data_bytes;
    /* Number of data bytes to be decrypted */
  word j;
    /* Loop counter */
  boolean bytes_removed = FALSE;
    /* Flag if bytes have been removed from the message */
  byte rec_type;
    /* The type of an information record in this message */
  byte rec_len;
    /* The original length of an information record before encryption */
  word total_length = *len_ptr;
    /* The overall length of the encrypted message body */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* ----------------------------------------------------------------------
  ** Now step through each info record, extract the type specific fields,
  ** add a byte if necessary, encrypt it, and place it back in the message.
  ** ---------------------------------------------------------------------- */
  msg_pos = decrypt_pos = FPOS( cai_gen_tc_type, hdr.encryption ) +
                        FSIZ( cai_gen_tc_type, hdr.encryption );

  while ((total_length - msg_pos) > sizeof( cai_rec_hdr_type )) /*lint !e574 */
  {
    if (bytes_removed)
    {
      /* We need to slide the record type field up due to removed bytes */
      rec_type = b_unpackb( buf_ptr, msg_pos, AUTH_RECORD_TYPE_SIZE);
      b_packb( rec_type, buf_ptr, decrypt_pos, AUTH_RECORD_TYPE_SIZE);
    }
    msg_pos += AUTH_RECORD_TYPE_SIZE;
    decrypt_pos += AUTH_RECORD_TYPE_SIZE;

    rec_len = b_unpackb( buf_ptr, msg_pos, AUTH_RECORD_LEN_SIZE);
    if (bytes_removed)
    {
      /* We need to slide the record length field up due to removed bytes */
      b_packb( rec_len, buf_ptr, decrypt_pos, AUTH_RECORD_LEN_SIZE);
    }
    msg_pos += AUTH_RECORD_LEN_SIZE;
    decrypt_pos += AUTH_RECORD_LEN_SIZE;

    if (rec_len == 1)
    {
      /* Add an extra zero byte */
      bytes_removed = TRUE;
      (*len_ptr) -= AUTH_EIGHT_BITS;
      num_data_bytes = 2;
    }
    else
    {
      num_data_bytes = rec_len;
    }

    if ((num_data_bytes * AUTH_EIGHT_BITS + msg_pos) > total_length)
    {
      /* --------------------------------------------------------
      ** The info record length would cause us to read beyond the
      ** end of the received message buffer.
      ** -------------------------------------------------------- */
      M1X_MSG( DCP, LEGACY_ERROR,
        "Message Length invalid");
      break;
    }

    /* Unpack the (encrypted) type specific fields */
    for (j = 0; j < num_data_bytes; j++)
    {
      auth_temp_buf[j] = b_unpackb( buf_ptr, msg_pos, AUTH_EIGHT_BITS);
      msg_pos += AUTH_EIGHT_BITS;
    }

    CAVE_encrypt( auth_temp_buf, num_data_bytes );

    for (j = 0; j < rec_len; j++)
    {
      b_packb( auth_temp_buf[j], buf_ptr, decrypt_pos, AUTH_EIGHT_BITS );
      decrypt_pos += AUTH_EIGHT_BITS;
    }
  }
} /* auth_decrypt_info() */

/*===========================================================================

FUNCTION AUTH_ENCRYPTION_POSSIBLE

DESCRIPTION
  This procedure determines if message encryption is possible.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if auth could successfully encrypt messages.
  FALSE if auth is not initialized properly to do encryption.

SIDE EFFECTS
  None

===========================================================================*/
boolean auth_encryption_possible( void )
{
  boolean possible;
    /* Flag if encryption is possible */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (auth_init_status == AUTH_UNINITIALIZED)
  {
    M1X_MSG( DCP, LEGACY_MED,
      "Need to initialize AUTH" );
    possible = FALSE;
  }
  else if (!auth_valid_CMEA_key)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Need to generate keys first" );
    possible = FALSE;
  }
  else
  {
    possible = TRUE;
  }
  return (possible);
}

/*===========================================================================

FUNCTION AUTH_ENCRYPT_CDMA

DESCRIPTION
  This procedure encrypts CDMA reverse traffic channel messages.
  If any fields require encryption they are encrypted.

DEPENDENCIES
  The function auth_encryption_possible needs to be called first to ensure
  message encryption is possible.

RETURN VALUE
  None

SIDE EFFECTS
  The num_digits, num_fields, record_len, etc. fields may not accurately
  represent the size of the field(s) if bits are added for encryption.

===========================================================================*/
void auth_encrypt_cdma
(
  word *len_ptr,
    /* Length of message in bits */
  byte *buf_ptr,
    /* Pointer to body of already packed traffic channel message */
  caii_tx_msg_type *int_ptr
    /* Pointer to internal message to be transmitted */
)
{

  switch (int_ptr->gen.msg_type)
  {
    case CAI_REV_FLASH_MSG:
      if (int_ptr->flash.num_recs !=0)
      {
        auth_encrypt_flash( len_ptr, buf_ptr, int_ptr );
      }
      break;

    case CAI_SEND_DTMF_MSG:
      if (int_ptr->sbdtmf.num_digits != 0)
      {
        /* There is some data to encrypt */
        auth_crypt_burst_dtmf( len_ptr, buf_ptr, TRUE );
      }
      break;

    case CAI_ORIG_C_MSG:
      if (int_ptr->orig_c.num_fields != 0)
      {
        /* There is some data to encrypt */
        auth_encrypt_orig_c( len_ptr, buf_ptr, int_ptr );
      }
      break;

    case CAI_TC_REV_ORD_MSG:
      if (int_ptr->tc_ord.gen.order == CAI_DTMF_ORD)
      {
        auth_crypt_dtmf_ord( len_ptr, buf_ptr, TRUE );
      }
      break;

    case CAI_TC_REV_BURST_MSG:
      if (int_ptr->tc_burst.num_fields != 0)
      {
        auth_crypt_data_burst( buf_ptr, TRUE );
      }
      break;

#ifdef FEATURE_IS2000_REL_A
    case CAI_EFWIM_MSG:
      if (int_ptr->efwim.num_rec != 0)
      {
        auth_encrypt_efwim( len_ptr, buf_ptr, int_ptr );
      }
      break;
#endif

    default:
      /* The message doesn't require any encryption */
      break;
  } /* end switch */
} /* auth_encrypt_cdma() */

/*===========================================================================

FUNCTION AUTH_DECRYPT_CDMA

DESCRIPTION
  This procedure decrypts CDMA reverse traffic channel messages.
  If any fields require decryption they are decrypted.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if there are no problems encountered.
  FALSE if fields of the given message that should be decrypted are not.

SIDE EFFECTS
  None

===========================================================================*/
boolean auth_decrypt_cdma
(
  word *len_ptr,
    /* Length of message in bits */
  byte *buf_ptr
    /* Pointer to received message to be translated */
)
{
  boolean success;
    /* Flag if decryption is successful */
#ifdef FEATURE_IS2000_REL_A
  boolean con_ref_incl;
    /* Flag to remember whetehr con_ref is included */
  byte num_recs;
    /* Number of info records in the message */
  word msg_pos;
    /* Keeps track of the current location in the packed msg */
#endif /* FEATURE_IS2000_REL_A */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (auth_init_status == AUTH_UNINITIALIZED)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Need to initialize AUTH" );
    success = FALSE;
  }
  else
  {
    switch ( b_unpackb( buf_ptr, FPOS( cai_gen_type, msg_type ),
                                 FSIZ( cai_gen_type, msg_type )))
    {
      case CAI_FWD_FLASH_MSG:
      case CAI_ALERT_MSG:
        if (!auth_valid_CMEA_key)
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Need to generate keys first" );
          success = FALSE;
        }
        else
        {
          /* Make sure we have at least one information record */
          if ((sizeof( cai_gen_tc_type ) +
              sizeof( cai_rec_hdr_type )) < *len_ptr)
          {
            auth_decrypt_info( len_ptr, buf_ptr );
          }
          success = TRUE;
        }
        break;

      case CAI_SEND_BURST_MSG:
        if (!auth_valid_CMEA_key)
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Need to generate keys first" );
          success = FALSE;
        }
        else
        {
          if ((b_unpackb( buf_ptr, FPOS( cai_sbdtmf_fix_type, num_digits ),
                          FSIZ( cai_sbdtmf_fix_type, num_digits ))) != 0)
          {
            /* There is some data to decrypt */
            auth_crypt_burst_dtmf( len_ptr, buf_ptr, FALSE );
          }
          success = TRUE;
        }
        break;

      case CAI_TC_FWD_ORD_MSG:
        if ((b_unpackb( buf_ptr, FPOS( cai_ftc_ord_type, gen.order ),
                    FSIZ( cai_ftc_ord_type, gen.order )))  == CAI_DTMF_ORD)
        {
          if (!auth_valid_CMEA_key)
          {
             M1X_MSG( DCP, LEGACY_ERROR,
               "Need to generate keys first" );
             success = FALSE;
          }
          else
          {
            auth_crypt_dtmf_ord( len_ptr, buf_ptr, FALSE );
            success = TRUE;
          }
        }
        else
        {
          /* There is no encryption necessary for any other orders */
          success = TRUE;
        }
        break;

      case CAI_TC_FWD_BURST_MSG:
        if (!auth_valid_CMEA_key)
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Need to generate keys first" );
          success = FALSE;
        }
        else
        {
          if ((b_unpackb( buf_ptr, FPOS( cai_burst_tc_fix_type, num_fields ),
              FSIZ( cai_burst_tc_fix_type, num_fields )))  != 0)
          {
            auth_crypt_data_burst( buf_ptr, FALSE );
          }
          success = TRUE;
        }
        break;

#ifdef FEATURE_IS2000_REL_A
      case CAI_EXT_ALERT_W_INFO_MSG:
      case CAI_EXT_FLASH_W_INFO_MSG:
        if (!auth_valid_CMEA_key)
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Need to generate keys first" );
          success = FALSE;
        }
        else
        {
          /* If this function is called, that means encryption
          ** is set to '01'. There should be no extended
          ** encryption fields. L3 fields starts right after
          ** encryption.
          */
          msg_pos = FPOS( cai_gen_tc_type, hdr.encryption)
                     + FSIZ( cai_gen_tc_type, hdr.encryption);

          con_ref_incl = b_unpackb( buf_ptr, msg_pos,
                          FSIZ(cai_extended_alert_w_info_type, con_ref_incl));
          msg_pos += FSIZ(cai_extended_alert_w_info_type, con_ref_incl);

          if (con_ref_incl)
          {
            msg_pos += FSIZ(cai_extended_alert_w_info_type, con_ref);
          }

          num_recs =
            b_unpackb( buf_ptr, msg_pos,
                       FSIZ( cai_extended_alert_w_info_type, num_recs ));
          msg_pos += FSIZ(cai_extended_alert_w_info_type, num_recs);


          /* Make sure we have at least one information record */
          if (num_recs > 0)
          {
            success = auth_decrypt_extended_info( len_ptr, buf_ptr,
                                                  msg_pos, num_recs );
          }
          else
          {
            success = TRUE;
          }
        }
        break;

#endif /* FEATURE_IS2000_REL_A */

      default:
        /* The message doesn't require any decryption */
        success = TRUE;
        break;
    }
  }

  return (success);
} /* auth_decrypt_cdma() */

/*===========================================================================

FUNCTION AUTH_INIT

DESCRIPTION
  This procedure initializes the queues and timers for Authentication Task.
  It should be called only once, at powerup time. It also initializes RUIM
  related variables.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void auth_init( void )
{
  byte i; /* Index into free buffer pool */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* -----------------
  ** Initialize queues
  ** ----------------- */

  (void) q_init( &auth_cmd_q );
  (void) q_init( &auth_free_q );

  /* Fill auth_free_q */
  for (i = 0; i < AUTH_NUM_CMD_BUFS; i++)
  {
    auth_cmd_pool[i].hdr.cmd_hdr.done_q_ptr = &auth_free_q;
    q_put( &auth_free_q, q_link( &auth_cmd_pool[i],
           &auth_cmd_pool[i].hdr.cmd_hdr.link ) );
  }


  /* -----------------
  ** Initialize flags
  ** ----------------- */

  new_ssd_valid = FALSE;
  used_new_ssd = FALSE;
  a_key_temp_valid = FALSE;


} /* auth_init() */


/*===========================================================================

FUNCTION AUTH_TASK_INIT

DESCRIPTION
  This function initializes the tcb pointer and other pre initializations that
  needs to be done only after TCB_PTR is updated.

DEPENDENCIES
  Must be called from "auth" task context.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void auth_task_init(void)
{
  /* Update the AUTH TCB pointer */
  AUTH_TCB_PTR = rex_self();

  /* -----------------
  ** Initialize timers
  ** ----------------- */
  rex_def_timer( &auth_ssd_timer, AUTH_TCB_PTR, AUTH_SSD_TIMER_SIG );

  /* Initialize DOG report ID and DOG report timeout */
  auth_dog_rpt_id = dog_hb_register_rex( AUTH_DOG_RPT_SIG );

  /* ------------------------------------------------------------------
  ** Initially kick watchdog
  ** ------------------------------------------------------------------ */
  auth_dog_report();

} /* auth_task_init */


#ifdef FEATURE_UIM_RUIM
/* <EJECT> */
/*===========================================================================

FUNCTION AUTH_WAIT

DESCRIPTION
      This function just performs a REX wait as specified by the
      caller, except that it automatically kicks the watchdog timer every
      so often while waiting. It also acknowledges the TASK_STOP_SIG and
      TASK_OFFLINE_SIG when they are set.

DEPENDENCIES
      Relies on AUTH_RPT_TIMER_SIG being raised often enough to
      avoid watchdog alarms, so should not be called until auth_init has
      run.

RETURN VALUE
      Returns the value returned by rex_wait.

SIDE EFFECTS
      The watchdog gets kicked periodically.

===========================================================================*/

LOCAL rex_sigs_type  auth_wait
(
  /* Mask of signals to wait for */
  rex_sigs_type sigs
)
{
  /* Return value from REX: bitmap of signals found raised */
  rex_sigs_type  rex_signals_mask;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Keep waiting and kicking the watchdog as long as the return
     value has the kick timer signal set and none of the user's
     desired signals are set. Also acknowledge the TASK_OFFLINE_
     SIG or TASK_STOP_SIG if they are set*/

  do
  {
    /* Wait for the caller's conditions OR our kick timer */
    rex_signals_mask = rex_wait( sigs | AUTH_DOG_RPT_SIG | TASK_STOP_SIG |
                                TASK_OFFLINE_SIG );

    if ((rex_signals_mask & TASK_STOP_SIG) != 0)
    {
      M1X_MSG( DCP, LEGACY_MED,
        "TASK_STOP_SIG received" );
      (void) rex_clr_sigs( AUTH_TCB_PTR, TASK_STOP_SIG );
    }
    else if ((rex_signals_mask & TASK_OFFLINE_SIG) != 0)
    {
      M1X_MSG( DCP, LEGACY_MED,
        "TASK_OFFLINE_SIG received" );
      (void) rex_clr_sigs( AUTH_TCB_PTR, TASK_OFFLINE_SIG );
    }
    else if ( (rex_signals_mask & AUTH_DOG_RPT_SIG) != 0 )
    {
      /* -----------------------------
      ** Kick watchdog and reset timer
      ** ----------------------------- */
      auth_dog_report();
    }
  } while ( (rex_signals_mask & sigs ) == 0x0000 );
  /* Keep looping as long as user's conditions are un-met */

  /* Return the rex_wait return value with user condition(s) set */
  return ( rex_signals_mask );

} /* auth_wait */
#endif /* FEATURE_UIM_RUIM */

/*===========================================================================

FUNCTION AUTH_CMD

DESCRIPTION
  The auth_cmd pointed to by the parameter passed in is queued up for AUTH.

DEPENDENCIES
  None.
RETURN VALUE
  None.
SIDE EFFECTS
  None.
===========================================================================*/
void auth_cmd (
  auth_cmd_type *cmd_ptr                    /* the command to queue up */
)
{
  #if defined(FEATURE_UIM_RUIM)
    /* init link */
  ( void )q_link( cmd_ptr, &cmd_ptr->hdr.cmd_hdr.link );

  q_put( &auth_cmd_q, &cmd_ptr->hdr.cmd_hdr.link ); /* and queue it */
  M1X_MSG( DCP, LEGACY_MED, "Auth_cmd_q count = %d", q_cnt(&auth_cmd_q) );

  (void)rex_set_sigs( AUTH_TCB_PTR, AUTH_CMD_Q_SIG );  /* signal the Auth task */
  #else
  (void) cmd_ptr;
  #endif /* FEATURE_UIM_RUIM  */
}

#ifdef FEATURE_CSIM
/*=========================================================================
MMGSDI CALL BACKS
=========================================================================*/


/*===========================================================================

FUNCTION auth_mmgsdi_cb ()

DESCRIPTION

DEPENDENCIES
  None.
RETURN VALUE
  None.
SIDE EFFECTS
  None.
===========================================================================*/
 void auth_mmgsdi_cb
 (
   mmgsdi_return_enum_type  status,
                      mmgsdi_cnf_enum_type      cnf,
                      const mmgsdi_cnf_type     *cnf_ptr
                     )
{
   size_t copy_size;
   /* Return value of memscpy function */
   auth_mmgsdi_rpt_status = MMGSDI_MAX_RETURN_ENUM;
   auth_cnf = FALSE;
   M1X_MSG( DCP, LEGACY_MED,
     "In auth_mmgsdi_cb , cnf = %d",
     cnf );

   if ( (cnf_ptr != NULL) && (auth_mmgsdi_rpt_ptr != NULL) )
   {
      copy_size = memscpy(auth_mmgsdi_rpt_ptr, sizeof(mmgsdi_cnf_type),
                          cnf_ptr, sizeof(mmgsdi_cnf_type));
      if( copy_size != sizeof(mmgsdi_cnf_type) )
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
          copy_size,
          sizeof(mmgsdi_cnf_type),
          sizeof(mmgsdi_cnf_type));
      }
     auth_cnf = TRUE;
     switch(cnf)
     {
       case MMGSDI_READ_CNF:
       {
         /* Do the deep copy of MMGSDI Rpt if required */
         if(auth_mmgsdi_rpt_ptr->read_cnf.read_data.data_len > 0)
         {
            if( (auth_mmgsdi_rpt_ptr->read_cnf.read_data.data_ptr =
                (uint8 *) modem_mem_alloc(
                 auth_mmgsdi_rpt_ptr->read_cnf.read_data.data_len,
                 MODEM_MEM_CLIENT_1X ) ) != NULL )
            {
              M1X_MSG( DCP, LEGACY_MED,
                "Auth MMGSDI_READ_CNF, mem allocated for data_ptr:0x%x",
                auth_mmgsdi_rpt_ptr->read_cnf.read_data.data_ptr);
              memset(auth_mmgsdi_rpt_ptr->read_cnf.read_data.data_ptr,
                     0, auth_mmgsdi_rpt_ptr->read_cnf.read_data.data_len);
              copy_size = memscpy(
                          auth_mmgsdi_rpt_ptr->read_cnf.read_data.data_ptr,
                          auth_mmgsdi_rpt_ptr->read_cnf.read_data.data_len,
                          cnf_ptr->read_cnf.read_data.data_ptr,
                          cnf_ptr->read_cnf.read_data.data_len);

              if( copy_size != cnf_ptr->read_cnf.read_data.data_len )
              {
                M1X_MSG( DCP, LEGACY_ERROR,
                  "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
                  copy_size,
                  cnf_ptr->read_cnf.read_data.data_len,
                  auth_mmgsdi_rpt_ptr->read_cnf.read_data.data_len);
                auth_cnf = FALSE;
              }
            } /* End if(modem_mem_alloc() != NULL) */
            else
            {
              ERR_FATAL( "Memory allocation failed", 0, 0,0);
            }
         } /* End if(data_len > 0) */

         break;
       } /* MMGSDI_READ_CNF */

       case MMGSDI_GENERATE_KEY_VPM_CNF:
       {
         if( cnf_ptr->generate_key_vpm_cnf.octet_data.data_len <= CAVE_VPM_SIZE )
         {
           /* Copy the data from cnf pointer, which contains the VPM mask */

           copy_size = memscpy(auth_mmgsdi_data,
                               CAVE_VPM_SIZE*sizeof(byte),
                  cnf_ptr->generate_key_vpm_cnf.octet_data.data_ptr,
                  cnf_ptr->generate_key_vpm_cnf.octet_data.data_len);

           if( copy_size != cnf_ptr->generate_key_vpm_cnf.octet_data.data_len )
           {
             M1X_MSG( DCP, LEGACY_ERROR,
               "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
               copy_size,
               cnf_ptr->generate_key_vpm_cnf.octet_data.data_len,
               CAVE_VPM_SIZE*sizeof(byte));
             auth_cnf = FALSE;
           }
         }
         else
         {
           M1X_MSG( DCP, LEGACY_ERROR,
             "Unexpected data_len:%d > auth_mmgsdi_data size:CAVE_VPM_SIZE",
             cnf_ptr->generate_key_vpm_cnf.octet_data.data_len);
           auth_cnf = FALSE;
         }
         break;
       }
       default:
         break;
     }
   }
   else
   {
     M1X_MSG( DCP, LEGACY_ERROR,
       "NULL pointer, cnf_ptr:0x%x, auth_mmgsdi_rpt_ptr:0x%x",
       cnf_ptr,
       auth_mmgsdi_rpt_ptr);
   }
   (void) rex_set_sigs( AUTH_TCB_PTR, AUTH_UIM_CMD_RSP_SIG );

   MODEM_1X_UNUSED(status);

}

#endif /* FEATURE_CSIM */
#endif /* FEATURE_UIM_RUIM || FEATURE_CSI */

/*===========================================================================

FUNCTION AUTH_TASK

DESCRIPTION
  This procedure is the entrance procedure for the Authentication task.

DEPENDENCIES
  None

RETURN VALUE
  Does not return.

SIDE EFFECTS
  None

===========================================================================*/

void auth_task
(
  dword dummy
    /* Parameter required for REX.  Tell lint to ignore it. */
    /*lint -esym(715,dummy) */
)
{
#if defined (FEATURE_RUIM)

  rex_sigs_type rex_signals_mask;  /* Mask of signals returned by rex */
  auth_cmd_type *cmd_ptr = NULL;          /* Pointer to received command */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*---------------------*/
  /* Task initialization */
  /*---------------------*/
  auth_task_init();

  /* Start handshake with RCINIT to start the init process */
  rcinit_handshake_startup();


  for (;;)
  {
    /* Never exit this loop... */

    /* If there is already a command on our command queue,  don't wait. */
    if (q_cnt( &auth_cmd_q ) != 0)
    {
      rex_signals_mask = AUTH_CMD_Q_SIG;
    }
    else
    {
      rex_signals_mask =
        rex_wait( AUTH_DOG_RPT_SIG | AUTH_CMD_Q_SIG | AUTH_SSD_TIMER_SIG |
                  TASK_OFFLINE_SIG | TASK_STOP_SIG );
    }

    if ( (rex_signals_mask & AUTH_DOG_RPT_SIG) != 0 )
    {
      /* -----------------------------
      ** Kick watchdog and reset timer
      ** ----------------------------- */
      auth_dog_report();

    }

    /* -------------------------------------------------------
    ** Check if powerdown command signal was set.  If set then
    ** clear signal, and process task stop procedure.
    ** ------------------------------------------------------- */
    if ((rex_signals_mask & TASK_STOP_SIG) != 0)
    {
      M1X_MSG( DCP, LEGACY_MED,
        "TASK_STOP_SIG received" );
      (void) rex_clr_sigs( AUTH_TCB_PTR, TASK_STOP_SIG );
    }

    /* -----------------------------------------------------
    ** Check if offline command signal was set.  If set then
    ** clear signal, and process task offline procedure.
    ** ----------------------------------------------------- */
    if ((rex_signals_mask & TASK_OFFLINE_SIG) != 0)
    {
      M1X_MSG( DCP, LEGACY_MED,
        "TASK_OFFLINE_SIG received" );
      (void) rex_clr_sigs( AUTH_TCB_PTR, TASK_OFFLINE_SIG );
    }


    /* ---------------------
    ** The SSD Timer expired
    ** --------------------- */
    if ((rex_signals_mask & AUTH_SSD_TIMER_SIG) != 0)
    {
      (void) rex_clr_sigs( AUTH_TCB_PTR, AUTH_SSD_TIMER_SIG );
      if (auth_ssd_update_in_progress)
      {
        /* ------------------------------------------
        ** The base station took too long to complete
        ** the SSD update so abort it.
        ** ------------------------------------------ */
        M1X_MSG( DCP, LEGACY_MED,
          "SSD timer expired - Aborting");
#ifdef FEATURE_UIM_RUIM
        (void) uim_power_control ( UIM_AUTH, FALSE );
#endif /* FEATURE_UIM_RUIM */
        auth_ssd_update_in_progress = FALSE;

       /* Event report: Timer T64m expired */
       mclog_report_event_timer(CAI_TIMER_T64M);

      }
    }


    /*-----------------------------
    ** We have a command to process
    ** ---------------------------- */
    if ((rex_signals_mask & AUTH_CMD_Q_SIG) != 0)
    {
      (void) rex_clr_sigs( AUTH_TCB_PTR, AUTH_CMD_Q_SIG );
      if ((cmd_ptr = (auth_cmd_type *)q_get( &auth_cmd_q )) != NULL)
      {
        auth_process_command( cmd_ptr );

        /* return buffer to free queue */
        if( cmd_ptr->hdr.cmd_hdr.done_q_ptr != NULL )
        {
          /* place command on requested queue */
           q_put( cmd_ptr->hdr.cmd_hdr.done_q_ptr,
                &cmd_ptr->hdr.cmd_hdr.link );
        }
        else
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "done_q_ptr was NULL!");
        }
      }  /* (cmd_ptr != NULL) */
    } /* end if ((rex_signals_mask & AUTH_CMD_Q_SIG) != 0) */
  } /* end for (;;) */
#else
  M1X_MSG( DCP, LEGACY_HIGH,
    "Feature AUTH not enabled, invalid Client rqst ..");
#endif /* FEATURE_RUIM */
} /* end auth_task */



/*===========================================================================

FUNCTION AUTH_VALIDATE_A_KEY

DESCRIPTION
  This procedure tests if the given A key is valid.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the A key is valid
  FALSE if the A key is invalid

SIDE EFFECTS
  None

===========================================================================*/
boolean auth_validate_a_key
(
  byte a_key[AUTH_A_KEY_DIGITS]
    /* The A key to be validated */
)
{
#if (!defined(FEATURE_UIM_RUIM) || defined (FEATURE_UIM_RUN_TIME_ENABLE))
  if (CAVE_validate_A_key( a_key ))
  {
    return (TRUE);
  }
  else
  {
    return (FALSE);
  }
#else
  M1X_MSG( DCP, LEGACY_MED,
    "Feature AUTH not enabled, invalid Client rqst ..");
  return FALSE;
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */
} /* auth_validate_a_key() */

/*===========================================================================

FUNCTION AUTH_SEND_UPDATE_A_KEY_CMD

DESCRIPTION
  Gets an available auth_cmd_type from the auth_free_q and constructs
  an update A_KEY cmd to AUTH task. This is used by the SVCPRG module.
  This function is remoted for use by the SVCPRG module in some targets.

DEPENDENCIES
  None.

RETURN VALUE
  True if auth cmd was successfully queued, False otherwise

SIDE EFFECTS
  Queue signal is set for the AUTH task
===========================================================================*/

boolean auth_send_update_a_key_cmd
(
  byte * a_key,
  int num_a_key_digits,
  int nam
)
{
  size_t copy_size;
  /* Return value of memscpy function */
#if defined (FEATURE_RUIM)

  auth_cmd_type *auth_ptr;

  MCC_CHK_NULL_PTR (a_key);

  if( ( auth_ptr = q_get(&auth_free_q)) == NULL )
  {
    /* could not get a buffer */
   M1X_MSG( DCP, LEGACY_ERROR,
     " couldn't get buffer for update AKEY cmd" );
   return FALSE;
  }
  if (num_a_key_digits > AUTH_A_KEY_DIGITS)
  {
    /* Too many digits in AKEY */
    M1X_MSG( DCP, LEGACY_ERROR,
      "Too many digits in AKEY %d",
      num_a_key_digits );
    return FALSE;
  }
  /* tell auth to update AKEY  */
  auth_ptr->hdr.command = AUTH_UPDATE_A_KEY_F;
  auth_ptr->hdr.cmd_hdr.task_ptr = NULL;

  copy_size = memscpy( auth_ptr->a_key.a_key, AUTH_A_KEY_DIGITS*sizeof(byte),
                       a_key , num_a_key_digits ); /*lint !e732 */

  if( copy_size != num_a_key_digits )
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
      copy_size,
      num_a_key_digits,
      AUTH_A_KEY_DIGITS*sizeof(byte));
  }

  auth_ptr->a_key.nam = nam;
  auth_ptr->a_key.rpt_function = NULL;
  auth_cmd(auth_ptr );
  return TRUE;
#else
  M1X_MSG( DCP, LEGACY_MED,
    "Feature AUTH not enabled, invalid Client rqst ..");
  return FALSE;
#endif /* FEATURE_RUIM */
}

/*lint +e818 */

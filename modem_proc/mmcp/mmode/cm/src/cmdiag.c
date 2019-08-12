/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

 C A L L   M A N A G E R   D I A G N O S T I C S   P A C K E T   M O D U L E

GENERAL DESCRIPTION
  This module contains function to support diagnostic packet processing.

  These functions support
  1. Call origination (CDMA subsystem).
  2. Call end (CDMA subsystem).
  3. Change of operating mode.


EXTERNALIZED FUNCTIONS

  cmdiag_init
    Registers the callback functions for call origination, call end and
    change of operating mode with the diagnostic monitor.



INITIALIZATION AND SEQUENCING REQUIREMENTS
  cmdiag_init() must be called to initialize this module before any other
  function declared in this module is being called.


Copyright (c) 1991 - 2013 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                           Edit History

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmdiag.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/15/14   jvo     Fix compilation warning in cmdiag_cm_set_system_selection()
                   by casting both sides of comparison
05/17/13   dk      CR470025 stack reduction effort
06/14/13   jqi     Add of TTL markers to prfile the boot up time. 
03/27/13   dk      providing a new API cm_mm_call_cmd_end_all_calls() in CR 447701                   
                   verified in cmdiag, using QXDM command window to end call send_data 54 
09/27/12   vm      Removing DIME featurization and cleanup
09/26/12   vm      Fixing Lint errors
03/23/12   gm      DIME integration fix: TMC deprecation and others.
01/06/12   gm      Data synchronization changes for freefloating platform
10/10/11   chl     Added support to answer call and select mode_pref with LTE
07/01/11   sg      fixing compilation issue for e-calll build.
04/29/11   rm      Adding req/resp packet for CDMA info
04/21/11   sv      Correct Lint Errors
04/11/11   am      Cleaning stubs and runnning qtf SUTE
02/17/11   cl      Add support FEATURE_MMODE_REMOVE_1X for FUSION-MDM
01/17/10   ak      Decoupling SO73 from 1x_Adv feature.
01/13/11   ak      Assigning SO75 to call type test.
12/23/10   sv      Integrate DSDS feature
11/02/10   am      Replacing FEATURE_MOB_MMODE_CUTE
09/22/10   xs      Fixed target compiler warning
08/10/10   aj      update diag IRAT meas req API
06/15/10   aj      support for IRAT measurement support
06/04/10   ak      Added support for 1X Advanced SO75
05/20/10   ak      Removing SO75 support due to dependancies in MOB.
05/14/10   ak      Added support for 1X Advanced SO75
10/30/09   ak      Added support for 1X Advanced service options
08/07/09   rm      CMI phase-2: Removing featurisation from /api files
11/04/08   cl      Adding DIAG support for SUPS Command.
09/15/08   np      Added test code of Assisted dialing
08/20/08   st      Resolve Klockwork defects
12/07/07   ra      Added support for IMS VCC in HOMER files
11/20/07   sv      Fixed CR#130667 - Reverted back to using reg->cnt for
                   the dailed_digits length
08/07/07   ic      Lint clean up
05/23/07   yc/ic   Fixed compile error
05/11/06   sk      Added support for originating 4GV_NB calls.
01/12/06   ic      For specific calls / walks Lint warnings, replaced global
                   -fsp comment with -e{error#} comments where needed
01/09/06   ic      Adjusted for cm_num_init() / cm_num_copy() changes in cm.h
01/06/06   ic      Lint cleanup
11/03/05   ic      Added CM_ASSERT() for ptr function parameters and lint
                   comments for 613 Lint error
10/21/05   sk      Added "+" as a valid number for calls made through the
                   diag interface.
06/02/05   ic      Mainlined FEATURE_FACTORY_TESTMODE
04/26/05   pk      Added CMDIAG_MODE_POWER_OFF_F to the diag mode. This mode
                   powers down the phone. Command to be used in diag is
                   send_data 41 6 0
07/26/04   ic      Changed include file order to customer.h, then cm.h then
                   cmi.h
03/29/04   ic      Added support for FEATURE_36_DIGIT_DIAL_STRING.
                   In cmdiag_orig_req() replaced NV_MAX_DIAL_DIGITS with
                   CMDIAG_NV_MAX_DIAL_DIGITS.
                   NV_MAX_DIAL_DIGITS may be greater than 32 if
                   FEATURE_36_DIGIT_DIAL_STRING is defined and we want to make
                   sure we use 32 in cmdiag_orig_req()
03/15/04   ka      Merged from 6250. Fixed Lint errors.
02/06/04   ws      Initial jaguar/mainline merge.
11/04/03   ka      Fixed a problem in call end where the list
                   of calls to be ended was not being filled
                   in sequentially.
10/31/03   ws      Removed T_RF_ZRF6200 feature.
09/29/03   vt      Added logic to end all active calls instead of just one.
07/24/03   vt      removed FEATURE_SPECIAL_MDR
04/28/03   AT      General Lint Cleanup.
02/27/03   prk     Updated file header and removed compiler warnings.
02/20/03   vt      Compiles orig & end functions for CDMA only.
02/14/03   vt      Fixed a compile problem.
02/14/03   lad     Moved mode change functionality from DIAG.  CM handles all
                   mode information.
08/29/02   lad     Created file from former diagp.c.
===========================================================================*/

#include "mmcp_variation.h"
#ifdef __cplusplus
extern "C"
{
#endif

#include "customer.h"
#include "cm.h"
#include "cm_v.h"
#include "comdef.h"
#include "cmcall.h"
#include "cmclient.h"       /* for event notifications     */
#include "cmdiag.h"
#include "diagcmd.h"
#include "diagpkt.h"
#include "msg.h"
#include "task.h"
#include "ftm.h"
#include "cmdbg.h"
#include "cmutil.h"
#include "cmtaski.h"

#include "cm_dualsim.h"
#include "cmlog.h"
#include "cmph.h"
#include "cmlog_v.h"

#ifdef CM_DEBUG
#error code not present
#endif

#ifdef CM_GW_SUPPORTED
#include "cmwaoc.h"
#endif

#ifdef FEATURE_ECALL_APP
#include "ecall_diag.h"
#endif /* FEATURE_ECALL_APP */

/* Header file Time Test Lite (TTL) framework for profiles modem boot up time
*/
#include "ttl_map.h"

#if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
/* Call Id made Local to access from orig & end. */
static cm_call_id_type call_id = 0;
#endif /* (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)) */

#ifdef CM_GW_SUPPORTED
/* Variables to store cug information */
static boolean  use_cug               = FALSE;
static boolean  cug_suppress_oa       = FALSE;
static uint16   cug_group_id          = 0;
static boolean  cug_suppress_pref_cug = FALSE;
LOCAL static cm_call_id_type wcdma_call_id = 0;
LOCAL static cm_call_id_type gsm_call_id = 0;
#endif

/*===========================================================================

FUNCTION CMDIAG_CONTROL

DESCRIPTION
  This procedure processes a request to change modes of operation in the
  DMSS.

RETURN VALUE
  Pointer to response packet.

============================================================================*/
PACKED void *cmdiag_control (PACKED void *req_pkt, uint16 pkt_len)
/*lint -esym(765,cmdiag_control)
** Can't be static, called by DIAG via cmdiag_tbl
*/
{

  cmdiag_mode_control_req_type *req = NULL;
  cmdiag_mode_control_rsp_type *rsp;
  const int rsp_len = sizeof (cmdiag_mode_control_rsp_type);

  sys_oprt_mode_e_type oprt_mode = SYS_OPRT_MODE_NONE;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(req_pkt != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  req = (cmdiag_mode_control_req_type *) req_pkt;

  CM_MSG_HIGH_1 ("Received mode change to %d", req->mode);

  switch (req->mode)
  {
    case CMDIAG_MODE_OFFLINE_A_F:
    case CMDIAG_MODE_OFFLINE_D_F:

      oprt_mode = SYS_OPRT_MODE_OFFLINE_CDMA;

      #if defined(FEATURE_HWTC) || defined(FEATURE_WCDMA_PLT)
      #error code not present
#endif
      break;

    case CMDIAG_MODE_FTM_F:
      oprt_mode = SYS_OPRT_MODE_FTM;
      break;

    case CMDIAG_MODE_ONLINE_F:
      oprt_mode = SYS_OPRT_MODE_ONLINE;
      break;

    case CMDIAG_MODE_LPM_F:
      oprt_mode = SYS_OPRT_MODE_LPM;
      break;

    case CMDIAG_MODE_RESET_F:
      oprt_mode = SYS_OPRT_MODE_RESET;
      break;

    case CMDIAG_MODE_POWER_OFF_F:
      oprt_mode = SYS_OPRT_MODE_PWROFF;
      break;

    default:
      CM_MSG_HIGH_1 ("Received invalid mode control %d", req->mode);
  }

  if (oprt_mode != SYS_OPRT_MODE_NONE)
  {
    (void) cm_ph_cmd_oprt_mode (NULL, NULL, CM_CLIENT_ID_ANONYMOUS, oprt_mode);

    rsp = (cmdiag_mode_control_rsp_type *)
      diagpkt_alloc (DIAG_CONTROL_F, rsp_len);

    if(rsp==NULL)
    {
        CM_MSG_LOW_0( "rsp_NULL");
        return NULL;
    }
    rsp->mode = req->mode;
  }
  else
  {
    rsp = (cmdiag_mode_control_rsp_type *)
      diagpkt_err_rsp (DIAG_BAD_PARM_F, req_pkt, pkt_len);
  }

  return (rsp);
}

/*===========================================================================
FUNCTION CMDIAG_ORIG_REQ

DESCRIPTION
  Processes a request to originate a call.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void *cmdiag_orig_req (PACKED void *req_pkt, word pkt_len)
/*lint -esym(715,req_pkt,pkt_len) */
/*lint -esym(818, req_pkt) */
/*lint -esym(765,cmdiag_orig_req)
** Can't be static / const, called by DIAG via cmdiag_tbl
*/
{
  #ifndef FEATURE_HWTC

  #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
  word i;           /* loop index */
  cm_call_type_e_type eCM_Call_Type;

  word cai_so;      /* The IS2000 service option */
  #endif /* (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)) */
  #endif

  const int rsp_len = sizeof (cmdiag_orig_rsp_type);

  #if !defined(FEATURE_HWTC)
  #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))

  cmdiag_orig_req_type *req = NULL;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(req_pkt != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  req = (cmdiag_orig_req_type *) req_pkt;

  /* Check the service option */
  switch (req->so)
  {
    case CMDIAG_SO_VOICE_96A:
      cai_so = CAI_SO_VOICE_IS96A;
      eCM_Call_Type = CM_CALL_TYPE_VOICE;
      break;

    case CMDIAG_SO_VOICE_4GV_NB:
      cai_so = CAI_SO_VOICE_4GV_NB;
      eCM_Call_Type = CM_CALL_TYPE_VOICE;
      break;

    case CMDIAG_SO_VOICE13:cai_so = CAI_SO_VOICE_13K;
      eCM_Call_Type = CM_CALL_TYPE_VOICE;
      break;

    case CMDIAG_SO_LOOPBACK:cai_so = CAI_SO_LOOPBACK;
      eCM_Call_Type = CM_CALL_TYPE_TEST;
      break;

    case CMDIAG_SO_LOOPBACK_13K:cai_so = CAI_SO_LOOPBACK_13K;
      eCM_Call_Type = CM_CALL_TYPE_TEST;
      break;

    #ifdef FEATURE_IS2000
    case CMDIAG_SO_LOOPBACK_SO55:cai_so = CAI_SO_LOOPBACK_SO55;
      eCM_Call_Type = CM_CALL_TYPE_TEST;
      break;
    #endif

    case CMDIAG_SO_MARKOV:cai_so = CAI_SO_MARKOV;
      eCM_Call_Type = CM_CALL_TYPE_TEST;
      break;

    case CMDIAG_SO_MARKOV_13K:cai_so = CAI_SO_MARKOV_13K;
      eCM_Call_Type = CM_CALL_TYPE_TEST;
      break;

    case CMDIAG_SO_RS2_MARKOV:cai_so = CAI_SO_RS2_MARKOV;
      eCM_Call_Type = CM_CALL_TYPE_TEST;
      break;

    case CMDIAG_SO_RS1_MARKOV:cai_so = CAI_SO_RS1_MARKOV;
      eCM_Call_Type = CM_CALL_TYPE_TEST;
      break;

    #ifdef FEATURE_IS2000
    case CMDIAG_SO_MARKOV_SO54:cai_so = CAI_SO_MARKOV_SO54;
      eCM_Call_Type = CM_CALL_TYPE_TEST;
      break;
    #endif

    #ifdef FEATURE_IS2000_1X_ADV
    case CAI_SO_MARKOV_SO74:cai_so = CAI_SO_MARKOV_SO74;
      eCM_Call_Type = CM_CALL_TYPE_TEST;
      break;
    #endif

    case CMDIAG_SO_EVRC:cai_so = CAI_SO_VOICE_EVRC;
      eCM_Call_Type = CM_CALL_TYPE_VOICE;
      break;

    #ifdef FEATURE_VOC_4GV_NW
    case CAI_SO_VOICE_EVRC_NW:cai_so = CAI_SO_VOICE_EVRC_NW;
      eCM_Call_Type = CM_CALL_TYPE_VOICE;
      break;
    #endif  /* FEATURE_VOC_4GV_NW */

    #if (defined(FEATURE_IS2000_1X_ADV) && !defined(FEATURE_MMODE_REMOVE_1X))
    case CAI_SO_LOOPBACK_SO75:cai_so = CAI_SO_LOOPBACK_SO75;
      eCM_Call_Type = CM_CALL_TYPE_TEST;
      break;
    #endif  /* FEATURE_IS2000_1X_ADV */

    #if defined(FEATURE_IS95B_MDR)
    case CMDIAG_SO_MDR_PKT_DATA_FRS1_RRS1:
      cai_so = CAI_SO_MDR_PKT_DATA_FRS1_RRS1;
      eCM_Call_Type = CM_CALL_TYPE_DATA;
      break;

    case CMDIAG_SO_MDR_PKT_DATA_FRS1_RRS2:
      cai_so = CAI_SO_MDR_PKT_DATA_FRS1_RRS2;
      eCM_Call_Type = CM_CALL_TYPE_DATA;
      break;

    case CMDIAG_SO_MDR_PKT_DATA_FRS2_RRS1:
      cai_so = CAI_SO_MDR_PKT_DATA_FRS1_RRS2;
      eCM_Call_Type = CM_CALL_TYPE_DATA;
      break;

    case CMDIAG_SO_MDR_PKT_DATA_FRS2_RRS2:
      cai_so = CAI_SO_MDR_PKT_DATA_FRS2_RRS2;
      eCM_Call_Type = CM_CALL_TYPE_DATA;
      break;

    case CMDIAG_SO_MDR_DPT_FRS1_RRS1:
      cai_so = CAI_SO_MDR_DPT_FRS1_RRS1;
      eCM_Call_Type = CM_CALL_TYPE_DATA;
      break;

    case CMDIAG_SO_MDR_DPT_FRS2_RRS2:
      cai_so = CAI_SO_MDR_DPT_FRS2_RRS2;
      eCM_Call_Type = CM_CALL_TYPE_DATA;
      break;
    #endif              /* defined(FEATURE_IS95B_MDR) */

    #if defined(FEATURE_IS2000_TDSO)
    /* Currently, DIAG can only originate a TDSO call via CM. */
    case CAI_SO_TDSO:case CAI_SO_FULL_TDSO:cai_so = req->so;
      eCM_Call_Type = CM_CALL_TYPE_TEST;
      break;
    #endif

    case CMDIAG_SO_SMS:cai_so = CAI_SO_SMS;
      eCM_Call_Type = CM_CALL_TYPE_SMS;
      break;

    case CMDIAG_SO_RS2_SMS:cai_so = CAI_SO_RS2_SMS;
      eCM_Call_Type = CM_CALL_TYPE_SMS;
      break;

    case CMDIAG_SO_DATA:
    case CMDIAG_SO_NONE:

    default:
      CM_MSG_HIGH_1 ("%d: Unsupported orig service option!", req->so);
      return (diagpkt_err_rsp (DIAG_BAD_PARM_F, req_pkt, pkt_len));
  }

  /* Check the dial string for valid number of digits */
  if (req->cnt > CMDIAG_NV_MAX_DIAL_DIGITS )
  {
    CM_MSG_HIGH_1 ("%d: Invalid digit count!", req->cnt);
    return (diagpkt_err_rsp (DIAG_BAD_PARM_F, req, pkt_len));
  }

  /* Check the dial string for valid digits */
  for (i = 0; i < req->cnt; i++)
  {
    /* Check for dial digits (0 - 9, *, #) */
    if ( !INRANGE(req->dialed_digits[i], '0', '9') &&
                 (req->dialed_digits[i] != '*') &&
                 (req->dialed_digits[i] != '#') &&
                 (req->dialed_digits[i] != '+'))
    {

      /* Was an invalid digit (not 0 - 9, *, #) */
      return (diagpkt_err_rsp (DIAG_BAD_PARM_F, req_pkt, pkt_len));
    }
  }             /* for each key in the string */

  /* Set up parameters to call CM with orig request.
     We have validated the call digits and the SO. */

  /* Send request to CM */
  {
    cm_cdma_orig_params_s_type orig_params;
    cm_num_s_type called_number;
    cm_orig_alpha_s_type alpha;

  /*-----------------------------------------------------------------------*/

    orig_params.activate_code = CM_OTASP_ACT_CODE_NONE;
    orig_params.srv_opt = cai_so;
    #ifdef FEATURE_IS2000
    orig_params.drs_bit = FALSE;
    #endif
    #ifdef FEATURE_IS2000_REL_A
    orig_params.sr_id_included = FALSE;
    orig_params.qos_parms_incl = FALSE;
    #endif
    #ifdef FEATURE_HDR_HANDOFF
    orig_params.last_act_data_net = SYS_SYS_MODE_NO_SRV;
    #endif /* FEATURE_HDR_HANDOFF */

    cm_num_init (&called_number);
    cm_num_fill (&called_number,
                 (const byte *) req->dialed_digits,
                 MIN(req->cnt,ARR_SIZE(req->dialed_digits)),
                 CM_DIGIT_MODE_4BIT_DTMF);

    alpha.len = memscpy( alpha.buf, sizeof(alpha.buf), "DIAG CALL", strlen ("DIAG CALL") );

   (void) cm_mm_call_cmd_orig_exten3(NULL,
                         NULL,
                         CM_CLIENT_ID_ANONYMOUS,
                         eCM_Call_Type,
                         CM_SRV_TYPE_CDMA_SPECIFIC,
                         NULL,
                         &called_number,
                         &alpha,
                         &orig_params,
                         NULL,
                         NULL,
                         CM_CALL_ID_INVALID,
                         CM_ORIG_CLIENT_CATEG_USER,
                         &call_id);

  }
  #endif /* (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)) */


  #else               /* FEATURE_HWTC defined */
  #error code not present
#endif              /* FEATURE_HWTC */

  return diagpkt_alloc (DIAG_ORIG_F, rsp_len);


}
/*lint +esym(715,req_pkt,pkt_len) */
/*lint +esym(818, req_pkt) */

/*===========================================================================
FUNCTION CMDIAG_END_REQ

DESCRIPTION
  Processes a request to end a call.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void *cmdiag_end_req (PACKED void *req_pkt, word pkt_len)
/*lint -esym(715,pkt_len)*/
/*lint -esym(715,req_pkt)*/
/*lint -esym(818,req_pkt) nope as diagpkt_user_table_entry_type phohibits that */
/*lint -esym(765,cmdiag_end_req)
** Can't be static, called by DIAG via cmdiag_tbl
*/
{
  cmdiag_end_rsp_type *rsp;
  const int rsp_len = sizeof (cmdiag_end_rsp_type);

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #if !defined(FEATURE_HWTC)
  #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
  {
    cm_mm_call_cmd_end_all_calls();
  }
  #endif /* (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)) */
  #endif /* FEATURE_HWTC */

  rsp = (cmdiag_end_rsp_type *) diagpkt_alloc (DIAG_END_F, rsp_len);

  return (rsp);
}               /* cmdiag_end_req */
/*lint +esym(715,pkt_len)*/
/*lint +esym(715,req_pkt)*/
/*lint +esym(818,req_pkt) nope as diagpkt_user_table_entry_type phohibits that */

/*===========================================================================

FUNCTION CMDIAG_CNTRY_INFO_REQ

DESCRIPTION
  This procedure processes a request to country network information for
  Assisted dialing. This is just test purpose. Not necessary to check in.

RETURN VALUE
  Pointer to response packet.

============================================================================*/
PACKED void *cmdiag_cntry_info_req (PACKED void *req_pkt, uint16 pkt_len)
/*lint -esym(715,req_pkt,pkt_len) */
/*lint -esym(818,req_pkt) */
/*lint -esym(765,cmdiag_cntry_info_req)*/
/*Can't be static, called by DIAG via cmdiag_tbl */
{
  cmdiag_cntry_info_rsp_type  *rsp;

  const int rsp_len = sizeof (cmdiag_cntry_info_rsp_type);

#if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)

  cmdiag_cntry_info_req_type  *req = NULL;

  cmutil_country_specific_record_s country_info;

   /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(req_pkt != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  req = (cmdiag_cntry_info_req_type *) req_pkt;

  CM_MSG_HIGH_1 ("Received mcc for cntry info req= %d", req->mcc);


  rsp = (cmdiag_cntry_info_rsp_type *)
      diagpkt_alloc (DIAG_CNTRY_INFO_F, rsp_len);

  if(rsp==NULL)
  {
        CM_MSG_LOW_0( "rsp_NULL");
        return NULL;
  }
  if ( cm_util_country_info_for_dialing(req->mcc,&country_info)==
    CMUTIL_COUNTRY_INFO_RESULT_FOUND)
  {
      rsp->sid1 = country_info.sid1;
      rsp->sid2 = country_info.sid2;
      rsp->country_code = country_info.country_code;
      memscpy( (char *)rsp->inter_dialed_digits, sizeof(rsp->inter_dialed_digits), (char *)country_info.inter_dialed_digits, 6 ); /* idd digits start after last F */
      memscpy( (char *)rsp->special_prefix_code, sizeof(rsp->special_prefix_code), (char *)country_info.special_prefix_code, 6 ); /* spc */
      memscpy( (char *)rsp->national_direct_dial, sizeof(rsp->national_direct_dial), (char *)country_info.national_direct_dial, 6 ); /* ndd */
      rsp->is_network_soln =(byte) country_info.is_network_soln; /* network solution */
      rsp->mobile_country_code = country_info.mobile_country_code ;   /* Mobile country code */
  }
#else
  rsp = (cmdiag_cntry_info_rsp_type *)
   diagpkt_alloc (DIAG_CNTRY_INFO_F, rsp_len);
#endif
  return (rsp);

}
/*lint -esym(715,pkt_len)*/
/*lint +esym(715,req_pkt)*/

#ifdef FEATURE_CM_SS_MEAS
#error code not present
#endif /* FEATURE_CM_SS_MEAS */



/*===========================================================================
FUNCTION CMDIAG_SUPS_REQ

DESCRIPTION
  Processes a Suplementary Service Request

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void *cmdiag_sups_req (PACKED void *req_pkt, word pkt_len)
/*lint -esym(715,req_pkt,pkt_len) */
/*lint -esym(818,req_pkt) */
/*lint -esym(765,cmdiag_sups_req)
** Can't be static / const, called by DIAG via cmdiag_tbl
*/
{
  const int rsp_len = sizeof (cmdiag_sups_rsp_type);
  byte i;
  cmdiag_sups_req_type *req                    = NULL;
  cm_call_sups_params_s_type        sups_param = {0};
  cmcall_s_type         *call_ptr;
  cm_call_sups_type_e_type sups_type;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(req_pkt != NULL);
  req = (cmdiag_sups_req_type *) req_pkt;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Get Call ID */
  call_ptr = cmcall_get_call_obj_in_call_state(CM_CALL_STATE_CONV);
  if (call_ptr != NULL)
  {
    /* Supplementary Service is originated based on an existing call id */
    sups_param.call_id = call_ptr->call_id;
  }
  else
  {
    /* In case of there's no existing call, Initialize Call Id = default value */
    sups_param.call_id = 0;
  }
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* ------------Initialize the number field ----------*/
  /* Check the dial string for valid number of digits */
  if (req->cnt > CMDIAG_NV_MAX_DIAL_DIGITS )
  {
    CM_MSG_HIGH_1 ("%d: Invalid digit count!", req->cnt);
    return (diagpkt_err_rsp (DIAG_BAD_PARM_F, req, pkt_len));
  }

  /* Check the dial string for valid digits */
  for (i = 0; i < req->cnt; i++)
  {
    /* Check for dial digits (0 - 9, *, #) */
    if ( !INRANGE(req->dialed_digits[i], '0', '9') &&
                 (req->dialed_digits[i] != '*') &&
                 (req->dialed_digits[i] != '#') &&
                 (req->dialed_digits[i] != '+'))
    {

      /* Was an invalid digit (not 0 - 9, *, #) */
      return (diagpkt_err_rsp (DIAG_BAD_PARM_F, req_pkt, pkt_len));
    }
  }             /* for each key in the string */

  /* Copy the numbers from diag packets over */
  cm_num_init(&sups_param.sups_number);
  cm_num_fill (&sups_param.sups_number,
                 (const byte *) req->dialed_digits,
                 MIN(req->cnt,ARR_SIZE(req->dialed_digits)),
                 CM_DIGIT_MODE_4BIT_DTMF);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Get sups type */
  sups_type = (cm_call_sups_type_e_type)req->sups_type;

  switch (sups_type)
  {

    case CM_CALL_SUPS_TYPE_HELD_CALL_REL_OR_UDUB:
    case CM_CALL_SUPS_TYPE_ACTIVE_CALL_REL:
    case CM_CALL_SUPS_TYPE_ACTIVE_CALL_X_REL:
    case CM_CALL_SUPS_TYPE_HOLD_CALL:
    case CM_CALL_SUPS_TYPE_SELECT_CALL_X:
    case CM_CALL_SUPS_TYPE_MULTIPARTY_CALL:
    case CM_CALL_SUPS_TYPE_CALL_DEFLECTION:
    case CM_CALL_SUPS_TYPE_EXPLICIT_CALL_TRANSFER:
    case CM_CALL_SUPS_TYPE_CALL_FORWARD:
    case CM_CALL_SUPS_TYPE_EMERGENCY_CALL:
    case CM_CALL_SUPS_TYPE_ACT_ANSWER_HOLD:
    case CM_CALL_SUPS_TYPE_DEACT_ANSWER_HOLD:
    case CM_CALL_SUPS_TYPE_SIMPLE_FLASH:
    case CM_CALL_SUPS_TYPE_ACT_CCBS_REQ:

      (void) cm_mm_call_cmd_sups(NULL, NULL, CM_CLIENT_ID_ANONYMOUS,
        sups_type,&sups_param);
      break;

    case CM_CALL_SUPS_TYPE_NONE:
    case CM_CALL_SUPS_TYPE_MAX:
    default:
      CM_MSG_HIGH_1(" Invalid options sups_type = %d", sups_type);
      break;
  }

  return diagpkt_alloc (DIAG_SUPS_REQ_F, rsp_len);


}              /*cmdiag_sups_req*/
/*lint +esym(715,req_pkt,pkt_len) */
/*lint +esym(818, req_pkt) */

/*===========================================================================
Function: cmdiag_check_request_length
Description:

       Function checks the length of packet with its associated type and returns TRUE/FALSE.
       It is important to modify this function depending on changes made to packet structures
       of WCDMA/GSM call origination, call end, set closed user group, set password and send
       USSD reply.
============================================================================*/
static boolean cmdiag_check_request_length
(  int pkt_len,
   PACKED const void* req_ptr,
   int packet_type
)
{
  int minimum_length;
  int expected_length;

  if (req_ptr == NULL)
  {
    return FALSE;
  }

  switch(packet_type)
  {
    case CM_WCDMA_CALL_ORIGINATION:
      /* num_digits and amr_rate and at least one digit */
      minimum_length = sizeof(diagpkt_subsys_header_type)
                       + sizeof (((DIAG_SUBSYS_CM_CM_WCDMA_CALL_ORIGINATION_req_type *)req_ptr)->num_digits)
                       + sizeof (((DIAG_SUBSYS_CM_CM_WCDMA_CALL_ORIGINATION_req_type *)req_ptr)->amr_rate)
                       + sizeof (((DIAG_SUBSYS_CM_CM_WCDMA_CALL_ORIGINATION_req_type *)req_ptr)->digits[0]);

      if (pkt_len < minimum_length)
        return FALSE;

      /* digits[] should have as many digits indicated by num_digits */
      expected_length = sizeof(diagpkt_subsys_header_type)
                       + sizeof (((DIAG_SUBSYS_CM_CM_WCDMA_CALL_ORIGINATION_req_type *)req_ptr)->num_digits)
                       + sizeof (((DIAG_SUBSYS_CM_CM_WCDMA_CALL_ORIGINATION_req_type *)req_ptr)->amr_rate)
                       + ((DIAG_SUBSYS_CM_CM_WCDMA_CALL_ORIGINATION_req_type *)req_ptr)->num_digits *
                         sizeof (((DIAG_SUBSYS_CM_CM_WCDMA_CALL_ORIGINATION_req_type *)req_ptr)->digits[0]);

      if ((pkt_len < expected_length) ||
           (pkt_len > (int) sizeof(DIAG_SUBSYS_CM_CM_WCDMA_CALL_ORIGINATION_req_type)))

      {
        return FALSE;
      }
      else
      {
        return TRUE;
      }

    case CM_GSM_CALL_ORIGINATION:
      /* num_digits and at least one digit */
      minimum_length = sizeof(diagpkt_subsys_header_type)
                       + sizeof(((DIAG_SUBSYS_CM_CM_GSM_CALL_ORIGINATION_req_type *)req_ptr)->num_digits)
                       + sizeof(((DIAG_SUBSYS_CM_CM_GSM_CALL_ORIGINATION_req_type *)req_ptr)->digits[0]);

      if (pkt_len < minimum_length)
        return FALSE;

      /* digits[] should have as many digits indicated by num_digits */
      expected_length = sizeof(diagpkt_subsys_header_type)
                        + sizeof(((DIAG_SUBSYS_CM_CM_GSM_CALL_ORIGINATION_req_type *)req_ptr)->num_digits)
                        + ((DIAG_SUBSYS_CM_CM_GSM_CALL_ORIGINATION_req_type *)req_ptr)->num_digits *
                          sizeof(((DIAG_SUBSYS_CM_CM_GSM_CALL_ORIGINATION_req_type *)req_ptr)->digits[0]);

      if ((pkt_len < expected_length) ||
           (pkt_len > (int) sizeof(DIAG_SUBSYS_CM_CM_GSM_CALL_ORIGINATION_req_type)))
      {
        return FALSE;
      }
      else
      {
        return TRUE;
      }

    case CM_CALL_END:
      /* call_id_count and at one parameter value*/
      minimum_length = sizeof(diagpkt_subsys_header_type) +
                        + sizeof(((DIAG_SUBSYS_CM_CM_CALL_END_req_type *)req_ptr)->call_id_count)
                        + sizeof(((DIAG_SUBSYS_CM_CM_CALL_END_req_type *)req_ptr)->end_params[0]);

      if (pkt_len < minimum_length)
      {
        return FALSE;
      }

      /* digits[] should have as many digits indicated by num_digits */
      expected_length = sizeof(diagpkt_subsys_header_type)
                        + sizeof(((DIAG_SUBSYS_CM_CM_CALL_END_req_type *)req_ptr)->call_id_count)
                        + ((DIAG_SUBSYS_CM_CM_CALL_END_req_type *)req_ptr)->call_id_count *
                          sizeof(((DIAG_SUBSYS_CM_CM_CALL_END_req_type *)req_ptr)->end_params[0]);

      if ((pkt_len < expected_length) ||
            (pkt_len > (int) sizeof(DIAG_SUBSYS_CM_CM_CALL_END_req_type)))
      {
        return FALSE;
      }
      else
      {
        return TRUE;
      }

    case CM_SET_CLOSED_USER_GROUP_PREFERENCE:
      if (pkt_len != (int) sizeof(DIAG_SUBSYS_CM_CM_SET_CLOSED_USER_GROUP_PREFERENCE_req_type))
      {
        return FALSE;
      }
      else
      {
        return TRUE;
      }

    case CM_SET_PASSWORD:
      if (pkt_len != (int) sizeof(DIAG_SUBSYS_CM_CM_SET_PASSWORD_req_type))
      {
        return FALSE;
      }
      else
      {
        return TRUE;
      }

    case CM_SEND_USSD_REPLY:
      /* call_id_count and at one parameter value */
      minimum_length = sizeof(diagpkt_subsys_header_type)
                       + sizeof(((DIAG_SUBSYS_CM_CM_SEND_USSD_REPLY_req_type *)req_ptr)->invoke_id)
                       + sizeof(((DIAG_SUBSYS_CM_CM_SEND_USSD_REPLY_req_type *)req_ptr)->ussd_type)
                       + sizeof(((DIAG_SUBSYS_CM_CM_SEND_USSD_REPLY_req_type *)req_ptr)->reply_data_size)
                       + sizeof(((DIAG_SUBSYS_CM_CM_SEND_USSD_REPLY_req_type *)req_ptr)->reply_data[0]);

      if (pkt_len < minimum_length)
        return FALSE;

       /* reply_data[] should have as many bytes indictaed by reply_data_size */
      expected_length = sizeof(diagpkt_subsys_header_type)
                        + sizeof(((DIAG_SUBSYS_CM_CM_SEND_USSD_REPLY_req_type *)req_ptr)->invoke_id)
                        + sizeof(((DIAG_SUBSYS_CM_CM_SEND_USSD_REPLY_req_type *)req_ptr)->ussd_type)
                        + sizeof(((DIAG_SUBSYS_CM_CM_SEND_USSD_REPLY_req_type *)req_ptr)->reply_data_size)
                        + ((DIAG_SUBSYS_CM_CM_SEND_USSD_REPLY_req_type *)req_ptr)->reply_data_size *
                          sizeof(((DIAG_SUBSYS_CM_CM_SEND_USSD_REPLY_req_type *)req_ptr)->reply_data[0]);

      if ((pkt_len < expected_length) ||
           (pkt_len > (int) sizeof(DIAG_SUBSYS_CM_CM_SEND_USSD_REPLY_req_type)))
      {
        return FALSE;
      }
      else
      {
        return TRUE;
      }

    case CM_SYSTEM_SELECTION_PREFERENCE2:
      if (pkt_len != (int) sizeof(DIAG_SUBSYS_CM_CM_SYSTEM_SELECTION_PREFERENCE2_req_type))
      {
        return FALSE;
      }
      else
      {
        return TRUE;
      }

    case CM_GET_CELL_SERVICE_IND:
      if ( pkt_len != (int)sizeof(DIAG_SUBSYS_CM_CM_GET_CELL_SERVICE_IND_req_type))
      {
        return FALSE;
      }
      else
      {
        return TRUE;
      }

    #ifdef FEATURE_MMODE_DUAL_SIM
    case CM_WCDMA_CALL_ORIGINATION_PER_SUBS:
      /* num_digits and amr_rate and at least one digit */
      minimum_length = sizeof(diagpkt_subsys_header_type)
                       + sizeof (((DIAG_SUBSYS_CM_CM_WCDMA_CALL_ORIGINATION_PER_SUBS_req_type *)req_ptr)->asubs_id)
                       + sizeof (((DIAG_SUBSYS_CM_CM_WCDMA_CALL_ORIGINATION_PER_SUBS_req_type *)req_ptr)->num_digits)
                       + sizeof (((DIAG_SUBSYS_CM_CM_WCDMA_CALL_ORIGINATION_PER_SUBS_req_type *)req_ptr)->amr_rate)
                       + sizeof (((DIAG_SUBSYS_CM_CM_WCDMA_CALL_ORIGINATION_PER_SUBS_req_type *)req_ptr)->digits[0]);

      if (pkt_len < minimum_length)
        return FALSE;

      /* digits[] should have as many digits indicated by num_digits */
      expected_length = sizeof(diagpkt_subsys_header_type)
                       + sizeof (((DIAG_SUBSYS_CM_CM_WCDMA_CALL_ORIGINATION_PER_SUBS_req_type *)req_ptr)->asubs_id)
                       + sizeof (((DIAG_SUBSYS_CM_CM_WCDMA_CALL_ORIGINATION_PER_SUBS_req_type *)req_ptr)->num_digits)
                       + sizeof (((DIAG_SUBSYS_CM_CM_WCDMA_CALL_ORIGINATION_PER_SUBS_req_type *)req_ptr)->amr_rate)
                       + ((DIAG_SUBSYS_CM_CM_WCDMA_CALL_ORIGINATION_PER_SUBS_req_type *)req_ptr)->num_digits *
                         sizeof (((DIAG_SUBSYS_CM_CM_WCDMA_CALL_ORIGINATION_PER_SUBS_req_type *)req_ptr)->digits[0]);

      if ((pkt_len < expected_length) ||
           (pkt_len > (int) sizeof(DIAG_SUBSYS_CM_CM_WCDMA_CALL_ORIGINATION_PER_SUBS_req_type)))

      {
        return FALSE;
      }
      else
      {
        return TRUE;
      }

    case CM_GSM_CALL_ORIGINATION_PER_SUBS:
      /* num_digits and at least one digit */
      minimum_length = sizeof(diagpkt_subsys_header_type)
                       + sizeof(((DIAG_SUBSYS_CM_CM_GSM_CALL_ORIGINATION_PER_SUBS_req_type *)req_ptr)->asubs_id)
                       + sizeof(((DIAG_SUBSYS_CM_CM_GSM_CALL_ORIGINATION_PER_SUBS_req_type *)req_ptr)->num_digits)
                       + sizeof(((DIAG_SUBSYS_CM_CM_GSM_CALL_ORIGINATION_PER_SUBS_req_type *)req_ptr)->digits[0]);

      if (pkt_len < minimum_length)
        return FALSE;

      /* digits[] should have as many digits indicated by num_digits */
      expected_length = sizeof(diagpkt_subsys_header_type)
                        + sizeof(((DIAG_SUBSYS_CM_CM_GSM_CALL_ORIGINATION_PER_SUBS_req_type *)req_ptr)->asubs_id)
                        + sizeof(((DIAG_SUBSYS_CM_CM_GSM_CALL_ORIGINATION_PER_SUBS_req_type *)req_ptr)->num_digits)
                        + ((DIAG_SUBSYS_CM_CM_GSM_CALL_ORIGINATION_PER_SUBS_req_type *)req_ptr)->num_digits *
                          sizeof(((DIAG_SUBSYS_CM_CM_GSM_CALL_ORIGINATION_PER_SUBS_req_type *)req_ptr)->digits[0]);

      if ((pkt_len < expected_length) ||
           (pkt_len > (int) sizeof(DIAG_SUBSYS_CM_CM_GSM_CALL_ORIGINATION_PER_SUBS_req_type)))
      {
        return FALSE;
      }
      else
      {
        return TRUE;
      }

    case CM_SET_PASSWORD_PER_SUBS:
      if (pkt_len != (int) sizeof(DIAG_SUBSYS_CM_CM_SET_PASSWORD_PER_SUBS_req_type))
      {
        return FALSE;
      }
      else
      {
        return TRUE;
      }

    case CM_SEND_USSD_REPLY_PER_SUBS:
      /* call_id_count and at one parameter value */
      minimum_length = sizeof(diagpkt_subsys_header_type)
                       + sizeof(((DIAG_SUBSYS_CM_CM_SEND_USSD_REPLY_PER_SUBS_req_type *)req_ptr)->asubs_id)
                       + sizeof(((DIAG_SUBSYS_CM_CM_SEND_USSD_REPLY_PER_SUBS_req_type *)req_ptr)->invoke_id)
                       + sizeof(((DIAG_SUBSYS_CM_CM_SEND_USSD_REPLY_PER_SUBS_req_type *)req_ptr)->ussd_type)
                       + sizeof(((DIAG_SUBSYS_CM_CM_SEND_USSD_REPLY_PER_SUBS_req_type *)req_ptr)->reply_data_size)
                       + sizeof(((DIAG_SUBSYS_CM_CM_SEND_USSD_REPLY_PER_SUBS_req_type *)req_ptr)->reply_data[0]);

      if (pkt_len < minimum_length)
        return FALSE;

       /* reply_data[] should have as many bytes indictaed by reply_data_size */
      expected_length = sizeof(diagpkt_subsys_header_type)
                        + sizeof(((DIAG_SUBSYS_CM_CM_SEND_USSD_REPLY_PER_SUBS_req_type *)req_ptr)->asubs_id)
                        + sizeof(((DIAG_SUBSYS_CM_CM_SEND_USSD_REPLY_PER_SUBS_req_type *)req_ptr)->invoke_id)
                        + sizeof(((DIAG_SUBSYS_CM_CM_SEND_USSD_REPLY_PER_SUBS_req_type *)req_ptr)->ussd_type)
                        + sizeof(((DIAG_SUBSYS_CM_CM_SEND_USSD_REPLY_PER_SUBS_req_type *)req_ptr)->reply_data_size)
                        + ((DIAG_SUBSYS_CM_CM_SEND_USSD_REPLY_PER_SUBS_req_type *)req_ptr)->reply_data_size *
                          sizeof(((DIAG_SUBSYS_CM_CM_SEND_USSD_REPLY_PER_SUBS_req_type *)req_ptr)->reply_data[0]);

      if ((pkt_len < expected_length) ||
           (pkt_len > (int) sizeof(DIAG_SUBSYS_CM_CM_SEND_USSD_REPLY_PER_SUBS_req_type)))
      {
        return FALSE;
      }
      else
      {
        return TRUE;
      }

    case CM_SYSTEM_SELECTION_PREFERENCE_PER_SUBS:
      if (pkt_len != (int) sizeof(DIAG_SUBSYS_CM_CM_SYSTEM_SELECTION_PREFERENCE_PER_SUBS_req_type))
      {
        return FALSE;
      }
      else
      {
        return TRUE;
      }
    case CM_SYSTEM_SELECTION_PREFERENCE2_PER_SUBS:
      if (pkt_len != (int) sizeof(DIAG_SUBSYS_CM_CM_SYSTEM_SELECTION_PREFERENCE2_PER_SUBS_req_type))
      {
        return FALSE;
      }
      else
      {
        return TRUE;
      }

    case CM_GET_CELL_SERVICE_IND_PER_SUBS:
      if ( pkt_len != (int)sizeof(DIAG_SUBSYS_CM_CM_GET_CELL_SERVICE_IND_PER_SUBS_req_type))
      {
        return FALSE;
      }
      else
      {
        return TRUE;
      }
    #endif

    case CM_SYSTEM_SELECTION_PREFERENCE_WITH_LTE:
      if (pkt_len != (int) sizeof(DIAG_SUBSYS_CM_CM_SYSTEM_SELECTION_PREFERENCE_WITH_LTE_req_type))
      {
        return FALSE;
      }
      else
      {
        return TRUE;
      }

    case CM_CALL_ANSWER:
      if (pkt_len != (int) sizeof(DIAG_SUBSYS_CM_CM_CALL_ANSWER_req_type))
      {
        return FALSE;
      }
      else
      {
        return TRUE;
      }

    case CM_GW_CALL_SETUP_RESPONSE:
      if (pkt_len != (int) sizeof(DIAG_SUBSYS_CM_CM_GW_CALL_SETUP_RESPONSE_req_type))
      {
        return FALSE;
      }
      else
      {
        return TRUE;
      }

    default:
      return FALSE;

  } /* switch (packet_type) */


} /* cmdiag_check_request_length (int pkt_len, PACKED void* req_pkt, int packet_type) */

/*===========================================================================
Function: cmdiag_cm_state_info
Description:

       This function processes the umts specific call manager state
       information request packet from QXDM and sends the response back.

============================================================================*/
/*lint -esym(765, cmdiag_cm_state_info)
** Can't be static, to be called by DIAG via COMMAND DISPATCH TABLE
*/
PACKED void *cmdiag_cm_state_info
(
  PACKED void* req_pkt,

  uint16       pkt_len
)
/*lint -esym(818, req_pkt) diagpkt_user_table_entry_type prohibits that */
/*lint -esym(715,req_pkt)*/
/*lint -esym(715,pkt_len)*/
{
  DIAG_SUBSYS_CM_CM_STATE_INFO_rsp_type *rsp_ptr ;

  cmph_s_type             *ph_ptr     = cmph_ptr();

  const int rsp_len = sizeof(DIAG_SUBSYS_CM_CM_STATE_INFO_rsp_type);

  cmlog_client_s_type* diag_client_ptr = cmlog_get_diag_client();

  /*lint -save -e641*/
  /* Allocate the memory for this */
  rsp_ptr = (DIAG_SUBSYS_CM_CM_STATE_INFO_rsp_type *)diagpkt_subsys_alloc(DIAG_SUBSYS_CM, CM_STATE_INFO,
                                 rsp_len );

  CM_MSG_HIGH_2("cmdiag_cm_state_info standby_pref %d, active_subs %d",\
    ph_ptr->standby_pref, ph_ptr->active_subs);
  CM_MSG_HIGH_3("cmdiag_cm_state_info main %d, h2 %d, h3 %d",\
    ph_ptr->is_gwl_subscription_available, ph_ptr->is_hybr_gw_subscription_available, ph_ptr->is_hybr_gw3_subscription_available);

  if (rsp_ptr != NULL)
  {
    /* Fill the response packet */
    /* Query Call Manager for State information */
    if (ph_ptr->active_subs == BM(ph_ptr->main_stack_info.asubs_id))
    {
      CM_MSG_HIGH_1("cmdiag_cm_state_info main_first %d", diag_client_ptr->network_sel_mode_pref);
      rsp_ptr->call_state            = cmcall_overall_call_state_per_subs(CM_SS_MAIN);
      rsp_ptr->oprt_mode             = diag_client_ptr->oprt_mode;
      rsp_ptr->system_mode           = diag_client_ptr->sys_mode;
      rsp_ptr->mode_pref             = diag_client_ptr->mode_pref;
      rsp_ptr->acq_order_pref        = diag_client_ptr->acq_order_pref;
      rsp_ptr->srv_domain_pref       = diag_client_ptr->srv_domain_pref;
      rsp_ptr->band_pref             = (int)diag_client_ptr->band_pref;
      rsp_ptr->roam_pref             = diag_client_ptr->roam_pref;
      rsp_ptr->hybr_pref             = diag_client_ptr->hybr_pref;
      rsp_ptr->network_sel_mode_pref = diag_client_ptr->network_sel_mode_pref;
      rsp_ptr->srv_status            = diag_client_ptr->srv_status;
    }
    else
    #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
    if (ph_ptr->active_subs == BM(ph_ptr->hybr_3_stack_info.asubs_id))
    {
      rsp_ptr->call_state            = cmcall_overall_call_state_per_subs(CM_SS_HYBR_3);
      rsp_ptr->oprt_mode             = diag_client_ptr->oprt_mode;
      rsp_ptr->system_mode           = diag_client_ptr->gw3_sys_mode;
      rsp_ptr->mode_pref             = diag_client_ptr->gw3_mode_pref;
      rsp_ptr->acq_order_pref        = diag_client_ptr->gw3_acq_order_pref;
      rsp_ptr->srv_domain_pref       = diag_client_ptr->gw3_srv_domain_pref;
      rsp_ptr->band_pref             = (int)diag_client_ptr->gw3_band_pref;
      rsp_ptr->roam_pref             = diag_client_ptr->gw3_roam_pref;
      rsp_ptr->hybr_pref             = diag_client_ptr->gw3_hybr_pref;
      rsp_ptr->network_sel_mode_pref = diag_client_ptr->gw3_network_sel_mode_pref;
      rsp_ptr->srv_status            = diag_client_ptr->gw3_srv_status;
    }
    else
    #endif
    #ifdef FEATURE_MMODE_DUAL_SIM
    if (!(ph_ptr->active_subs & BM(ph_ptr->main_stack_info.asubs_id))
        &&
        (ph_ptr->active_subs & BM(ph_ptr->hybr_2_stack_info.asubs_id))
        &&
        !cmph_is_sxlte_any())
    {
      rsp_ptr->call_state            = cmcall_overall_call_state_per_subs(CM_SS_HYBR_2);
      rsp_ptr->oprt_mode             = diag_client_ptr->oprt_mode;
      rsp_ptr->system_mode           = diag_client_ptr->gw_sys_mode;
      rsp_ptr->mode_pref             = diag_client_ptr->gw_mode_pref;
      rsp_ptr->acq_order_pref        = diag_client_ptr->gw_acq_order_pref;
      rsp_ptr->srv_domain_pref       = diag_client_ptr->gw_srv_domain_pref;
      rsp_ptr->band_pref             = (int)diag_client_ptr->gw_band_pref;
      rsp_ptr->roam_pref             = diag_client_ptr->gw_roam_pref;
      rsp_ptr->hybr_pref             = diag_client_ptr->gw_hybr_pref;
      rsp_ptr->network_sel_mode_pref = diag_client_ptr->gw_network_sel_mode_pref;
      rsp_ptr->srv_status            = diag_client_ptr->gw_srv_status;
    }
    else
    #endif
    {
      rsp_ptr->call_state            = cmcall_overall_call_state_per_subs(CM_SS_MAIN);
      rsp_ptr->oprt_mode             = diag_client_ptr->oprt_mode;
      rsp_ptr->system_mode           = diag_client_ptr->sys_mode;
      rsp_ptr->mode_pref             = diag_client_ptr->mode_pref;
      rsp_ptr->acq_order_pref        = diag_client_ptr->acq_order_pref;
      rsp_ptr->srv_domain_pref       = diag_client_ptr->srv_domain_pref;
      rsp_ptr->band_pref             = (int)diag_client_ptr->band_pref;
      rsp_ptr->roam_pref             = diag_client_ptr->roam_pref;
      rsp_ptr->hybr_pref             = diag_client_ptr->hybr_pref;
      rsp_ptr->network_sel_mode_pref = diag_client_ptr->network_sel_mode_pref;
      rsp_ptr->srv_status            = diag_client_ptr->srv_status;
    }
  }
  /*lint -restore */

  return rsp_ptr ;
}
/*lint +esym(715,req_pkt)*/
/*lint +esym(715,pkt_len)*/
/*lint +esym(818, req_pkt) */

/*===========================================================================
Function: cmdiag_cm_plmn_list
Description:

       This function processes the call manager plmn list request
       packet from QXDM and sends the response back.

============================================================================*/
/*lint -esym(765, cmdiag_cm_get_plmn_list)
** Can't be static, to be called by DIAG via COMMAND DISPATCH TABLE
*/
PACKED void *cmdiag_cm_get_plmn_list
(
  PACKED void* req_pkt, uint16 pkt_len
)
/*lint -esym(818, req_pkt) diagpkt_user_table_entry_type prohibits that */
/*lint -esym(715,req_pkt) */
/*lint -esym(715,pkt_len) */
{
    DIAG_SUBSYS_CM_CM_PLMN_SELECTION_MODE_rsp_type *rsp_ptr ;

    cmlog_client_s_type* diag_client_ptr = cmlog_get_diag_client();

    /*lint -save -e641*/
    /* Allocate the memory for this */
    rsp_ptr = (DIAG_SUBSYS_CM_CM_PLMN_SELECTION_MODE_rsp_type *)diagpkt_subsys_alloc(DIAG_SUBSYS_CM, CM_PLMN_SELECTION_MODE,
                                   sizeof(DIAG_SUBSYS_CM_CM_PLMN_SELECTION_MODE_rsp_type) );
    /*lint -restore */

    if (rsp_ptr != NULL)
    {   /* Fill the response packet */
        rsp_ptr->status = cm_ph_cmd_get_networks(
            NULL,
            NULL,
            diag_client_ptr->client,
            CM_NETWORK_LIST_AVAILABLE,
            CM_MODE_PREF_GSM_WCDMA_ONLY
            );
    }

    return rsp_ptr ;
}
/*lint +esym(715,req_pkt) */
/*lint +esym(715,pkt_len) */
/*lint +esym(818, req_pkt) diagpkt_user_table_entry_type prohibits that */


/*===========================================================================
Function: cmdiag_cm_system_selection
Description:

       This function processes the call manager system selection request
       packet from QXDM and sends the response back.

============================================================================*/
/*lint -esym(765, cmdiag_cm_set_system_selection)
** Can't be static, to be called by DIAG via COMMAND DISPATCH TABLE
*/
PACKED void *cmdiag_cm_set_system_selection (PACKED void* req_pkt, uint16 pkt_len)
{
    DIAG_SUBSYS_CM_CM_SYSTEM_SELECTION_PREFERENCE_req_type *req_ptr
            = (DIAG_SUBSYS_CM_CM_SYSTEM_SELECTION_PREFERENCE_req_type*)req_pkt;
    DIAG_SUBSYS_CM_CM_SYSTEM_SELECTION_PREFERENCE_rsp_type *rsp_ptr;

    /*lint -save -e641*/

    cm_network_sel_mode_pref_e_type  nw_sel_mode_pref;
    sys_plmn_id_s_type               plmn_struct;

    cm_mode_pref_e_type               mode_pref;
    cm_gw_acq_order_pref_e_type       acq_order_pref;
    cm_band_pref_e_type               band_pref;
    cm_roam_pref_e_type               roam_pref;
    cm_hybr_pref_e_type               hybr_pref;
    cm_srv_domain_pref_e_type         srv_domain_pref;

    cmlog_client_s_type* diag_client_ptr = cmlog_get_diag_client();

    /* Allocate the memory for this */
    rsp_ptr = (DIAG_SUBSYS_CM_CM_SYSTEM_SELECTION_PREFERENCE_rsp_type *)diagpkt_subsys_alloc(DIAG_SUBSYS_CM, CM_SYSTEM_SELECTION_PREFERENCE,
                                   sizeof(DIAG_SUBSYS_CM_CM_SYSTEM_SELECTION_PREFERENCE_rsp_type) );

    /* Ensure response packet allocation */
    if (!rsp_ptr)
    {
        return NULL;
    }
    /* Sanity check request packet */
    if (!req_ptr || pkt_len < sizeof(DIAG_SUBSYS_CM_CM_SYSTEM_SELECTION_PREFERENCE_req_type) )
    {
        rsp_ptr->status = FALSE;

    } else {
        /* Get params from Request Packet */
        /* Convert from little endian notation */
        plmn_struct.identity[0] = req_ptr->PLMN[2];
        plmn_struct.identity[1] = req_ptr->PLMN[1];
        plmn_struct.identity[2] = req_ptr->PLMN[0];

        /*lint -save -e650 -e737
        **            Loss of sign in promotion from int to unsigned int
        **            Constant out of range for operator '!='
        **            AST picked 0xFFFFFFFF to mean "no change"
        **            so we just disable those two here
        */
        if (((req_ptr->mode_pref >= CM_MODE_PREF_MAX) &&
              (req_ptr->mode_pref != 0xFFFFFFFF))                         ||

            ((req_ptr->acq_order_pref >= CM_GW_ACQ_ORDER_PREF_MAX) &&
             (req_ptr->acq_order_pref != 0xFFFFFFFF ))                     ||

            ((req_ptr->roam_pref >= CM_ROAM_PREF_MAX) &&
             (req_ptr->roam_pref != 0xFFFFFFFF))                          ||

            ((req_ptr->hybr_pref >= CM_HYBR_PREF_MAX) &&
             (req_ptr->hybr_pref != 0xFFFFFFFF))                          ||

            ((req_ptr->srv_domain_pref >= CM_SRV_DOMAIN_PREF_MAX) &&
             (req_ptr->srv_domain_pref != 0xFFFFFFFF))                    ||

            ((req_ptr->network_sel_mode_pref >= CM_NETWORK_SEL_MODE_PREF_MAX) &&
             (req_ptr->network_sel_mode_pref != 0xFF)))
        {
          CM_ERR_0("One or more request parameters had illegal values");
        }
        /*lint -restore */

        if( req_ptr->network_sel_mode_pref < CM_NETWORK_SEL_MODE_PREF_MAX )
        {
          nw_sel_mode_pref = (cm_network_sel_mode_pref_e_type) req_ptr->network_sel_mode_pref;
        }
        else
        {
          nw_sel_mode_pref = CM_NETWORK_SEL_MODE_PREF_NO_CHANGE;
        }

        if( req_ptr->mode_pref < CM_MODE_PREF_MAX )
        {
            mode_pref = (cm_mode_pref_e_type) req_ptr->mode_pref;
        }
        else
        {
            mode_pref = CM_MODE_PREF_NO_CHANGE;
        }

        if( req_ptr->acq_order_pref < CM_GW_ACQ_ORDER_PREF_MAX )
        {
            acq_order_pref = (cm_gw_acq_order_pref_e_type) req_ptr->acq_order_pref;
        }
        else
        {
            acq_order_pref = CM_GW_ACQ_ORDER_PREF_NO_CHANGE;
        }

        /*lint -save -e650 Constant '-2147483647' out of range for operator '<'
        ** Band pref went to 64 bits.
        ** Can't modify req packet so just suppress it and accept that we'll
        ** have garbage there
        */
        /* This CM ss pref dispatch command/resp uses 32bit band pref.
        ** The value of CM_BAND_PREF_NO_CHANGE is unchanged when
        ** band pref is increased from 32bit to 64bit.
        */
        if( req_ptr->band_pref < CM_BAND_PREF_NO_CHANGE )
        {
            band_pref = (cm_band_pref_e_type) req_ptr->band_pref;
        }
        else
        {
            band_pref = CM_BAND_PREF_NO_CHANGE;
        }
        /*lint -restore */

        if( req_ptr->roam_pref < CM_ROAM_PREF_MAX )
        {
            roam_pref = (cm_roam_pref_e_type) req_ptr->roam_pref;
        }
        else
        {
            roam_pref = CM_ROAM_PREF_NO_CHANGE;
        }

        if( req_ptr->hybr_pref < CM_HYBR_PREF_MAX )
        {
            hybr_pref = (cm_hybr_pref_e_type) req_ptr->hybr_pref;
        }
        else
        {
            hybr_pref = CM_HYBR_PREF_NO_CHANGE;
        }

        if( req_ptr->srv_domain_pref < CM_SRV_DOMAIN_PREF_MAX )
        {
            srv_domain_pref = (cm_srv_domain_pref_e_type) req_ptr->srv_domain_pref;
        }
        else
        {
            srv_domain_pref = CM_SRV_DOMAIN_PREF_NO_CHANGE;
        }


        /* Issue SYS_SEL_PREF & Fill the response packet */
        rsp_ptr->status = cm_ph_cmd_sys_sel_pref_new
              (
                NULL,
                NULL,
                diag_client_ptr->client,
                mode_pref,
                CM_PREF_TERM_PERMANENT, /* Store in NV */
                0,
                acq_order_pref,
                band_pref,
                CM_PRL_PREF_NO_CHANGE,
                roam_pref,
                hybr_pref,
                srv_domain_pref,
                nw_sel_mode_pref,
                &plmn_struct
             );
    }
    /*lint -restore */

    return rsp_ptr ;
}

/*===========================================================================
Function: cmdiag_cm_get_call_information_list
Description:

       This function returns call_id, call_state and call_type for all allocated
       call ids.
============================================================================*/
/*lint -esym(765, cmdiag_cm_get_call_information_list)
** Can't be static, to be called by DIAG via COMMAND DISPATCH TABLE
*/
PACKED void *cmdiag_cm_get_call_information_list
(
  PACKED void* req_pkt,
  uint16 pkt_len
)
/*lint -esym(818, req_pkt) diagpkt_user_table_entry_type prohibits that */
/*lint -esym(715,req_pkt)*/
/*lint -esym(715,pkt_len)*/
{
  uint8 loop = 0;

  DIAG_SUBSYS_CM_CM_GET_CALL_INFORMATION_LIST_rsp_type *rsp_ptr ;

  const int rsp_len = sizeof(DIAG_SUBSYS_CM_CM_GET_CALL_INFORMATION_LIST_rsp_type);


  cmlog_call_state_s_type* call_state_list_ptr = cmlog_get_call_state_list();

  /* Stops Lint complain DIAG_SUBSYS_CM_CM_GET_CALL_INFORMATION_LIST_req_type */
  if (((DIAG_SUBSYS_CM_CM_GET_CALL_INFORMATION_LIST_req_type *) req_pkt) == NULL)
  {
    CM_MSG_LOW_0("cmdiag_cm_get_call_information_list, req_pkt is NULL");
  }

  /*lint -save -e641*/
  /* Allocate the memory for response */
  rsp_ptr = (DIAG_SUBSYS_CM_CM_GET_CALL_INFORMATION_LIST_rsp_type *)
                diagpkt_subsys_alloc(DIAG_SUBSYS_CM,
                 CM_GET_CALL_INFORMATION_LIST, rsp_len );
  /*lint -restore */

  if (rsp_ptr != NULL)
  {
    rsp_ptr->status = TRUE;
  }
  else
  {
    return rsp_ptr;
  }

  /* Initialize number of calls to 0 */
  rsp_ptr->num_calls = 0;

  /* Fill in the call information */
  for (loop=0;loop<CMCALL_MAX_CALL_OBJ;++loop)
  {
    if (call_state_list_ptr[loop].call_id != CM_CALL_ID_INVALID)
    {

      rsp_ptr->num_calls = rsp_ptr->num_calls + 1;

      /* Update rsp_ptr */
      rsp_ptr->call_info[rsp_ptr->num_calls - 1].call_id            =
        (byte) call_state_list_ptr[loop].call_id;

      /* call type */
      rsp_ptr->call_info[rsp_ptr->num_calls - 1].call_type          =
        (byte) call_state_list_ptr[loop].call_type;

      /* call state */
      rsp_ptr->call_info[rsp_ptr->num_calls - 1].call_state         =
        (byte) call_state_list_ptr[loop].call_state;

      /* ps call state */
      rsp_ptr->call_info[rsp_ptr->num_calls - 1].is_ps_call_dormant =
        (byte) call_state_list_ptr[loop].is_ps_call_dormant;

    }
  }/* for () */

  #ifdef CM_GW_SUPPORTED
  cmlog_cm_send_call_information_log();
  #endif

  return rsp_ptr;

} /* cmdiag_cm_get_call_information_list () */
/*lint +esym(715,req_pkt)*/
/*lint +esym(715,pkt_len)*/
/*lint +esym(818, req_pkt) diagpkt_user_table_entry_type prohibits that */

#if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))

/*===========================================================================
Function: cmdiag_cm_call_orig
Description:

       This function processes the call manager call origination request
       packet from QXDM and sends the response back.

============================================================================*/
/*lint -esym(765, cmdiag_cm_call_orig)
** Can't be static, to be called by DIAG via COMMAND DISPATCH TABLE
*/
PACKED void *cmdiag_cm_call_orig (PACKED void* req_pkt, uint16 pkt_len)
/*lint -esym(715,pkt_len) */
{
  DIAG_SUBSYS_CM_CM_CDMA_CALL_ORIGINATION_req_type *req_ptr
          = (DIAG_SUBSYS_CM_CM_CDMA_CALL_ORIGINATION_req_type*)req_pkt;
  DIAG_SUBSYS_CM_CM_CDMA_CALL_ORIGINATION_rsp_type *rsp_ptr;

  cm_num_s_type               called_number;
  cm_num_s_type               calling_number;
  cm_orig_alpha_s_type        alpha;
  cm_cdma_orig_params_s_type  cdma_orig_params;

  cmlog_client_s_type* diag_client_ptr = cmlog_get_diag_client();

  /*-----------------------------------------------------------------------*/

  /*lint -save -e641*/
  /* Allocate the memory for this */
  rsp_ptr = (DIAG_SUBSYS_CM_CM_CDMA_CALL_ORIGINATION_rsp_type *) diagpkt_subsys_alloc(DIAG_SUBSYS_CM, CM_CDMA_CALL_ORIGINATION,
    sizeof(DIAG_SUBSYS_CM_CM_CDMA_CALL_ORIGINATION_rsp_type) );
  /*lint-restore */

  /* Ensure response packet allocation */
  if (!rsp_ptr)
  {
    return NULL;
  }

  /* Sanity check request packet */
  if ( !req_ptr )
  {
    rsp_ptr->status = FALSE;
  }
  else
  {
    /* Set the called number */
    cm_num_init( &called_number );
    if ( req_ptr->called_calling_num_count > 0 )
    {
      /*
      called_number.len = MIN( req_ptr->number[0].digits_count,
        CM_MAX_NUMBER_CHARS );
      memcpy( called_number.buf, (const byte *) req_ptr->number[0].digits,
        called_number.len );
      called_number.digit_mode = (cm_digit_mode_e_type)(req_ptr->number[0].digit_mode);

      */

      /* Sets the digit mode logic right */
      cm_num_fill(&called_number, (const byte *) req_ptr->number[0].digits,
                  MIN( req_ptr->number[0].digits_count, CM_MAX_NUMBER_CHARS ),
                  (cm_digit_mode_e_type)(req_ptr->number[0].digit_mode));

      called_number.number_type = req_ptr->number[0].number_type;
      called_number.number_plan = req_ptr->number[0].number_plan;
      called_number.pi = req_ptr->number[0].presentation_ind;
      called_number.si = req_ptr->number[0].screening_ind;
      called_number.subaddr.extension_bit = req_ptr->number[0].subaddr_extension;
      called_number.subaddr.subadd_type = req_ptr->number[0].subaddr_type;
      called_number.subaddr.odd_even_ind = req_ptr->number[0].subaddr_odd_even_ind;
      called_number.subaddr.num_char = memscpy( called_number.subaddr.chari, sizeof(called_number.subaddr.chari),
                                                (const byte *) req_ptr->number[0].subaddr, req_ptr->number[0].subaddr_count );
    }

    /* Set the calling number */
    cm_num_init( &calling_number );
    if ( req_ptr->called_calling_num_count > 1 )
    {
      calling_number.len = memscpy( calling_number.buf, sizeof(calling_number.buf),
                                    (const byte *) req_ptr->number[1].digits, req_ptr->number[1].digits_count );
      calling_number.digit_mode = (cm_digit_mode_e_type)(req_ptr->number[1].digit_mode);
      calling_number.number_type = req_ptr->number[1].number_type;
      calling_number.number_plan = req_ptr->number[1].number_plan;
      calling_number.pi = req_ptr->number[1].presentation_ind;
      calling_number.si = req_ptr->number[1].screening_ind;
      calling_number.subaddr.extension_bit = req_ptr->number[1].subaddr_extension;
      calling_number.subaddr.subadd_type = req_ptr->number[1].subaddr_type;
      calling_number.subaddr.odd_even_ind = req_ptr->number[1].subaddr_odd_even_ind;
      calling_number.subaddr.num_char = memscpy( calling_number.subaddr.chari, sizeof(calling_number.subaddr.chari),
                                                 (const byte *) req_ptr->number[1].subaddr, req_ptr->number[1].subaddr_count );
    }

    /* Set alpha buffer */
    alpha.len = MIN( req_ptr->alpha_digits_count, CMLOG_CM_MAX_ALPHA_DIGITS );
    alpha.len = memscpy( alpha.buf, sizeof(alpha.buf), (char*) req_ptr->alpha_digits, req_ptr->alpha_digits_count );

    /* Set the cdma specific orig parameters */
    cdma_orig_params.srv_opt = req_ptr->service_option;
    cdma_orig_params.activate_code = (cm_activate_code_e_type)(req_ptr->otasp_activate_code);
    cdma_orig_params.drs_bit = req_ptr->drs_bit;
    cdma_orig_params.sr_id_included = req_ptr->sr_id_included;
    cdma_orig_params.sr_id = req_ptr->sr_id;
    cdma_orig_params.qos_parms_incl = req_ptr->qos_params_included;
    if ( cdma_orig_params.qos_parms_incl )
    {
      cdma_orig_params.qos_parms_len = memscpy( cdma_orig_params.qos_parms, sizeof(cdma_orig_params.qos_parms),
                                               (char*) req_ptr->qos_params, req_ptr->qos_params_count );
    }
    cdma_orig_params.last_act_data_net = (sys_sys_mode_e_type)(req_ptr->hdr_last_act_data_net);

    /* Issue SYS_SEL_PREF & Fill the response packet */
    rsp_ptr->status =
             cm_mm_call_cmd_orig_exten3(NULL,
                                        NULL,
                                        diag_client_ptr->client,
                                        (cm_call_type_e_type)(req_ptr->call_type),
                                        (cm_srv_type_e_type)(req_ptr->srv_type),
                                        &calling_number,
                                        &called_number,
                                        &alpha,
                                        &cdma_orig_params,
                                        NULL,
                                        NULL,
                                        CM_CALL_ID_INVALID,
                                        CM_ORIG_CLIENT_CATEG_USER,
                                        &call_id );
    rsp_ptr->call_id = call_id;
  }

  return rsp_ptr;

} /* cmdiag_cm_call_orig() */
/*lint +esym(715,pkt_len) */

#endif /* (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)) */

/*===========================================================================
Function: cmdiag_cm_call_end
Description:

       This function processes the call manager call end request
       packet from QXDM and sends the response back.

============================================================================*/
/*lint -esym(765, cmdiag_cm_call_end)
** Can't be static, to be called by DIAG via COMMAND DISPATCH TABLE
*/
PACKED void *cmdiag_cm_call_end (PACKED void* req_pkt, uint16 pkt_len)
{
  DIAG_SUBSYS_CM_CM_CALL_END_req_type  *req_ptr =
    (DIAG_SUBSYS_CM_CM_CALL_END_req_type*) req_pkt;
  DIAG_SUBSYS_CM_CM_CALL_END_rsp_type  *rsp_ptr;

  uint8                 num_call_ids = 0;
  cm_end_params_s_type  *end_params = NULL;//
  int i;
  cmlog_client_s_type* diag_client_ptr = cmlog_get_diag_client();


  /*-----------------------------------------------------------------------*/

  /*lint -save -e641*/
  /* Allocate the memory for this */
  rsp_ptr = (DIAG_SUBSYS_CM_CM_CALL_END_rsp_type *) diagpkt_subsys_alloc(DIAG_SUBSYS_CM, CM_CALL_END,
    sizeof(DIAG_SUBSYS_CM_CM_CALL_END_rsp_type) );
  /*lint -restore */

  /* Ensure response packet allocation */
  if (!rsp_ptr)
  {
    return NULL;
  }

  /* Check for packet length */
  if (cmdiag_check_request_length (pkt_len, req_pkt, CM_CALL_END) == FALSE)
  {
    CM_ERR_0("Packet length check failed");
    return rsp_ptr;
  }

  /* Sanity check request packet */
  /* if (!req_ptr || pkt_len < sizeof(DIAG_SUBSYS_CM_CM_CALL_END_req_type) ) */
  /* Checking for packet size to conform to min length required*/
  if ( !req_ptr )
  {
    rsp_ptr->status = FALSE;
  }
  else
  {
    /* Get params from Request Packet */
    num_call_ids = MIN( req_ptr->call_id_count, CM_DIAG_CALL_ID_MAX );

    if ( num_call_ids > 0 )
    {
      end_params = (cm_end_params_s_type*)
                               cm_mem_malloc(num_call_ids*sizeof(cm_end_params_s_type));

      for ( i = 0; i < num_call_ids; i++ )
      {

        CM_MSG_MED_1("cmlog call end call id = %d", req_ptr->end_params[i].call_id);

        end_params[i].call_id = req_ptr->end_params[i].call_id;
        if ( req_ptr->end_params[i].call_mode_type == 0 )
        {
          end_params[i].info_type = CM_CALL_MODE_INFO_CDMA;
          end_params[i].end_params.cdma_end.end_reason_included =
            req_ptr->end_params[i].cdma_end_reason_included;
          end_params[i].end_params.cdma_end.end_reason =
            req_ptr->end_params[i].cdma_end_reason;
        }
        else if ( req_ptr->end_params[i].call_mode_type == 1 )
        {
          end_params[i].info_type = CM_CALL_MODE_INFO_GW_CS;
        }
        else /* It is PS */
        {
          end_params[i].info_type = CM_CALL_MODE_INFO_GW_PS;

        #ifdef CM_GW_SUPPORTED

           end_params[i].end_params.gw_ps_end.tear_down_ind.valid =
             req_ptr->end_params[i].gw_tear_down_ind_valid;

           end_params[i].end_params.gw_ps_end.tear_down_ind.ti_flag =
            req_ptr->end_params[i].gw_tear_down_ind_ti_flag;

         #endif

          end_params[i].end_params.gw_ps_end.cause =
            (sm_status_T) (req_ptr->end_params[i].gw_cause);
        }
      } /* for ( i = 0; i < num_call_ids; i++ ) */
    }

    /* Issue the command & Fill the response packet */
    /*lint -esym(772, end_params) if not initialized then num_call_ids is 0
    ** so we don't care what's in end_params anyway
    */
    rsp_ptr->status = cm_mm_call_cmd_end( NULL, NULL, diag_client_ptr->client,
                                          num_call_ids, end_params );
    /*lint +esym(772, end_params) */
    if (end_params != NULL){ cm_mem_free(end_params);}
  }


  return rsp_ptr ;

} /* cmdiag_cm_call_end() */

#ifdef CM_GW_SUPPORTED

/*===========================================================================
Function: cmdiag_cm_set_cug_pref
Description:
       Executes request to enable or disable closed user group,
        set group out access to Suppress or no Suppress and to set group Id.
============================================================================*/
/*lint -esym(765, cmdiag_cm_set_cug_pref)
** Can't be static, to be called by DIAG via COMMAND DISPATCH TABLE
*/
PACKED void *cmdiag_cm_set_cug_pref (PACKED void* req_pkt, uint16 pkt_len)
{
  DIAG_SUBSYS_CM_CM_SET_CLOSED_USER_GROUP_PREFERENCE_req_type  *req_ptr =
        (DIAG_SUBSYS_CM_CM_SET_CLOSED_USER_GROUP_PREFERENCE_req_type *) req_pkt;
  DIAG_SUBSYS_CM_CM_SET_CLOSED_USER_GROUP_PREFERENCE_rsp_type  *rsp_ptr;

   /*lint -save -e641*/
  /* Allocate the memory for this */
  rsp_ptr = (DIAG_SUBSYS_CM_CM_SET_CLOSED_USER_GROUP_PREFERENCE_rsp_type *) diagpkt_subsys_alloc(DIAG_SUBSYS_CM, CM_SET_CLOSED_USER_GROUP_PREFERENCE,
    sizeof(DIAG_SUBSYS_CM_CM_SET_CLOSED_USER_GROUP_PREFERENCE_rsp_type));
  /*lint -restore */

  /* Ensure response packet allocation */
  if (!rsp_ptr)
  {
    return NULL;
  }

  /* Check for packet length */
  if (cmdiag_check_request_length (pkt_len, req_pkt, CM_SET_CLOSED_USER_GROUP_PREFERENCE) == FALSE)
  {
    CM_ERR_0("Packet length check failed");
    return rsp_ptr;
  }

  if (!req_ptr)
  {
    rsp_ptr->status = FALSE;
  }
  else
  {
    /* 0xFF for a request field denotes no change to be made for that variable */
    if (req_ptr->use_cug != 0xFF)
    {
      if ((req_ptr->use_cug == 0) || (req_ptr->use_cug == 1))
      {
        use_cug = req_ptr->use_cug;
      }
      else
      {
        CM_MSG_HIGH_1("Invalid use_cug=%d, it can take only 1 or 0", req_ptr->use_cug);
        rsp_ptr->status = FALSE;
        return rsp_ptr;
      }
    }/* if (req_ptr->use_cug != 0xFF) */

    if (req_ptr->cug_suppress_oa != 0xFF)
    {
      if ((req_ptr->cug_suppress_oa == 0) || (req_ptr->cug_suppress_oa == 1))
      {
        cug_suppress_oa = req_ptr->cug_suppress_oa;
      }
      else
      {
        CM_MSG_HIGH_1("Invalid cug_suppress_oa=%d, can take only 1 or 0",
                      req_ptr->cug_suppress_oa);
        rsp_ptr->status = FALSE;
        return rsp_ptr;
      }
    }/* if (req_ptr->cug_suppress_oa != 0xFF) */

    if (req_ptr->cug_group_id != 0xFFFF)
    {
      if (req_ptr->cug_group_id <= 32767)
      {
        cug_group_id = req_ptr->cug_group_id;
      }
      else
      {
        CM_MSG_HIGH_1("Invalid cug_group_id=%d, it has to be a number from 1 to 32767",
                      req_ptr->cug_group_id);
        rsp_ptr->status = FALSE;
        return rsp_ptr;
      }
    }/* if (req_ptr->cug_group_id != 0xFFFF) */
  } /* if (!req_ptr) */


  /* Getting all the values for response packet */
  rsp_ptr->status = TRUE;
  rsp_ptr->use_cug = use_cug;
  rsp_ptr->cug_suppress_oa = cug_suppress_oa;
  rsp_ptr->cug_group_id = cug_group_id;

  return rsp_ptr;
}

/*===========================================================================
Function: cmdiag_cm_get_cug_pref
Description:
       Executes request to get closed user group setting,
       group out access and group Id.
============================================================================*/
/*lint -esym(765, cmdiag_cm_get_cug_pref)
** Can't be static, to be called by DIAG via COMMAND DISPATCH TABLE
*/
PACKED void *cmdiag_cm_get_cug_pref (PACKED void* req_pkt, uint16 pkt_len)
/*lint -esym(715,pkt_len) */
{
  DIAG_SUBSYS_CM_CM_GET_CLOSED_USER_GROUP_PREFERENCE_req_type  *req_ptr =
        (DIAG_SUBSYS_CM_CM_GET_CLOSED_USER_GROUP_PREFERENCE_req_type *) req_pkt;
  DIAG_SUBSYS_CM_CM_GET_CLOSED_USER_GROUP_PREFERENCE_rsp_type  *rsp_ptr;

   /*lint -save -e641*/
  /* Allocate the memory for this */
  rsp_ptr = (DIAG_SUBSYS_CM_CM_GET_CLOSED_USER_GROUP_PREFERENCE_rsp_type *) diagpkt_subsys_alloc(DIAG_SUBSYS_CM, CM_GET_CLOSED_USER_GROUP_PREFERENCE,
    sizeof(DIAG_SUBSYS_CM_CM_GET_CLOSED_USER_GROUP_PREFERENCE_rsp_type));
  /*lint -restore */

  /* Ensure response packet allocation */
  if (!rsp_ptr)
  {
    return NULL;
  }


  if (!req_ptr)
  {
    rsp_ptr->status = FALSE;
  }
  else
  {
    /* Getting all the values for response packet */
    rsp_ptr->use_cug = use_cug;
    rsp_ptr->cug_suppress_oa = cug_suppress_oa;
    rsp_ptr->cug_group_id = cug_group_id;

    rsp_ptr->status = TRUE;
  }

  return rsp_ptr;
}
/*lint +esym(715,pkt_len) */

/*=================================================================================
Function: cmlog_cm_sups_cmd_cb
Description:
           Called by Call manager to indicate status of SUPS command
==================================================================================*/
/*lint -esym(765, cmdiag_cm_sups_cmd_cb)
** Can't be static, it's a callback
*/
void cmdiag_cm_sups_cmd_cb (
  void*                   data_block_ptr,
  cm_sups_cmd_e_type      cmd,
  cm_sups_cmd_err_e_type  cmd_err
)
/*lint -esym(715, data_block_ptr, cmd)*/
/*lint -esym(818, data_block_ptr)*/
{
  if (cmd_err == CM_SUPS_CMD_ERR_NOERR)
  {
    return;
  }
  else
  {
    CM_ERR_2("CM SUPS command returned error cmd=%d, cmd_err=%d", cmd, cmd_err);
    return;
  }

}/* cmdiag_cm_sups_cmd_cb () */
/*lint +esym(715, data_block_ptr, cmd)*/
/*lint +esym(818, data_block_ptr)*/

/*=================================================================================
Function: cmdiag_cm_set_password
Description:
       Called for setting user given password in response to a get password request.
==================================================================================*/
/*lint -esym(765, cmdiag_cm_set_password)
** Can't be static, to be called by DIAG via COMMAND DISPATCH TABLE
*/
PACKED void *cmdiag_cm_set_password (PACKED void* req_pkt, uint16 pkt_len)
{
  DIAG_SUBSYS_CM_CM_SET_PASSWORD_req_type  *req_ptr =
        (DIAG_SUBSYS_CM_CM_SET_PASSWORD_req_type *) req_pkt;
  DIAG_SUBSYS_CM_CM_SET_PASSWORD_rsp_type  *rsp_ptr;
  cm_get_pwd_res_params_s_type       get_pwd_res;
  int i;
  cmlog_client_s_type* diag_client_ptr = cmlog_get_diag_client();

  /*lint -save -e641*/
  /* Allocate the memory for this */
  rsp_ptr = (DIAG_SUBSYS_CM_CM_SET_PASSWORD_rsp_type *) diagpkt_subsys_alloc(DIAG_SUBSYS_CM, CM_SET_PASSWORD,
    sizeof(DIAG_SUBSYS_CM_CM_SET_PASSWORD_rsp_type));
  /*lint -restore */

  /* Ensure response packet allocation */
  if (!rsp_ptr)
  {
    return NULL;
  }

  /* Check for packet length */
  if (cmdiag_check_request_length (pkt_len, req_pkt, CM_SET_PASSWORD) == FALSE)
  {
    CM_ERR_0("Packet length check failed");
    return rsp_ptr;
  }

  if (!req_ptr)
  {
    rsp_ptr->status = FALSE;
    return rsp_ptr;
  }
  else
  {

    memset( &get_pwd_res,
          CM_CALL_CMD_PARAM_DEFAULT_VALUE,
          sizeof( cm_get_pwd_res_params_s_type ));

    /* Invoke ID */
    get_pwd_res.invoke_id = req_ptr->invoke_id;

    /* Check if password digits are between '0' and '9' */
    for (i=0; i<MAX_DIAG_PWD_CHAR; i++) {
      if ((req_ptr->ss_password[i] < '0') || (req_ptr->ss_password[i] > '9'))
      {
        CM_ERR_1("Bad Password: %c", req_ptr->ss_password[i]);

        rsp_ptr->status = FALSE;
        return rsp_ptr;
      }
    }

    /*
    ** 506: constant value boolean
    ** 774: boolean always evaluates to TRUE
    */
    /*lint -save -e506 -e774*/

    /*
    ** MAX_DIAG_PWD_CHAR is the size allowed by diag for password
    ** field. Added a check to catch if CM decides to increase size
    ** of ss_password
    */
    if (MAX_DIAG_PWD_CHAR != sizeof (get_pwd_res.ss_pwd.ss_password))
    {
      CM_ERR_0("Password length mismatch between DIAG and CM");

      return rsp_ptr;
    }
    /*lint -restore */

    /*Copy password string*/
    memscpy( get_pwd_res.ss_pwd.ss_password,
             sizeof(get_pwd_res.ss_pwd.ss_password),
             (const byte *)req_ptr->ss_password,
             MAX_DIAG_PWD_CHAR);

    get_pwd_res.ss_pwd.present = TRUE;

    /* Copy invoke_id into response */
    rsp_ptr->invoke_id = req_ptr->invoke_id;

    /* send the command to CM command queue */
    rsp_ptr->status = cm_sups_cmd_get_password_res( cmdiag_cm_sups_cmd_cb,
                                NULL,
                                diag_client_ptr->client,
                                &get_pwd_res);
    return rsp_ptr;
  }

} /* cmdiag_cm_set_password () */

/*===========================================================================
Function: cmdiag_cm_send_ussd_reply
Description:
       called to send user USSD response or user notify response to network
============================================================================*/
/*lint -esym(765, cmdiag_cm_send_ussd_reply)
** Can't be static, to be called by DIAG via COMMAND DISPATCH TABLE
*/
PACKED void *cmdiag_cm_send_ussd_reply (PACKED void* req_pkt, uint16 pkt_len)
{
  DIAG_SUBSYS_CM_CM_SEND_USSD_REPLY_req_type  *req_ptr =
        (DIAG_SUBSYS_CM_CM_SEND_USSD_REPLY_req_type *) req_pkt;
  DIAG_SUBSYS_CM_CM_SEND_USSD_REPLY_rsp_type  *rsp_ptr;

  cm_uss_notify_res_params_s_type  uss_notify_res;/* Response for USSD based notify request */
  cm_uss_res_params_s_type  uss_res;/* Response for user input towards a ussd request*/
  byte num_bytes = 0;
  cmlog_client_s_type* diag_client_ptr = cmlog_get_diag_client();

  memset( &uss_res,
          CM_CALL_CMD_PARAM_DEFAULT_VALUE,
          sizeof( cm_uss_res_params_s_type ));

  memset( &uss_notify_res,
          CM_CALL_CMD_PARAM_DEFAULT_VALUE,
          sizeof( cm_uss_notify_res_params_s_type));

  /*lint -save -e641*/
  /* Allocate the memory for this */
  rsp_ptr = (DIAG_SUBSYS_CM_CM_SEND_USSD_REPLY_rsp_type *) diagpkt_subsys_alloc(DIAG_SUBSYS_CM, CM_SEND_USSD_REPLY,
    sizeof(DIAG_SUBSYS_CM_CM_SEND_USSD_REPLY_rsp_type));
  /*lint -restore */

  /* Ensure response packet allocation */
  if (!rsp_ptr)
  {
    return NULL;
  }

  /* Check for packet length */
  if (cmdiag_check_request_length (pkt_len, req_pkt, CM_SEND_USSD_REPLY) == FALSE)
  {
    CM_ERR_0("Packet length check failed");
    return rsp_ptr;
  }

  if (!req_ptr)
  {
    rsp_ptr->status = FALSE;
    return rsp_ptr;
  }
  else
  {
    /* Response For USSD notify indication */
    if (req_ptr->ussd_type == USS_NOTIFY_RESPONSE)
    {
      uss_notify_res.ss_error.present = FALSE;
      uss_notify_res.invoke_id = req_ptr->invoke_id;

      rsp_ptr->invoke_id = req_ptr->invoke_id;
      rsp_ptr->ussd_type = req_ptr->ussd_type;


      CM_MSG_HIGH_0("Sending USSD notify response");
      rsp_ptr->status = cm_sups_cmd_uss_notify_res( cmdiag_cm_sups_cmd_cb,
                              NULL,
                              diag_client_ptr->client,
                              &uss_notify_res);


    }
    /* Response for USSD indication */
    else if (req_ptr->ussd_type == USS_RESPONSE)
    {
      /* uss data */
      uss_res.uss_res_data.data_type = (cm_ss_conf_e_type) CM_USS_RES_SS_DATA;

      uss_res.uss_res_data.data.uss_data.present = TRUE;
      uss_res.uss_res_data.data.uss_data.uss_data_coding_scheme =
                                              USS_DEF_ALPHABET_LANG_UNSPEC;

      rsp_ptr->invoke_id = req_ptr->invoke_id;
      rsp_ptr->ussd_type = req_ptr->ussd_type;

      /* Check for USSD reply to not exceed MAX_DIAG_USS_CHAR */
      if (req_ptr->reply_data_size > MAX_DIAG_USS_CHAR)
      {
        CM_ERR_1("MAX number of USS charcters is %d", MAX_DIAG_USS_CHAR);
        req_ptr->reply_data_size = MAX_DIAG_USS_CHAR;
      }

      if (req_ptr->reply_data_size != 0)
      {
        /* convert the response from ascii to GSM def alphabet */
        num_bytes = cm_util_ascii_to_gsm_alphabet(
                                uss_res.uss_res_data.data.uss_data.ussData,
                                (byte *) req_ptr->reply_data,
                                req_ptr->reply_data_size );
      }
      else
      {
        CM_ERR_0("reply_data_size is 0");
      }

      /* size */
      uss_res.uss_res_data.data.uss_data.size = num_bytes;

      /* invocation id */
      uss_res.invoke_id = req_ptr->invoke_id;


      CM_MSG_HIGH_0("Sending USSD request response");
      /* send the command to the CM command queue */
      rsp_ptr->status = cm_sups_cmd_uss_res( cmdiag_cm_sups_cmd_cb,
                       NULL,
                       diag_client_ptr->client,
                       &uss_res );


    } /* if (req_ptr->ussd_type == 0) */
    else
    {
      rsp_ptr->status = FALSE;
    }

  } /* if (!req_ptr) */

  return rsp_ptr;

} /* cmdiag_cm_send_ussd_reply () */


/*===========================================================================
Function: cmdiag_cm_get_ccm_info
Description:

       This function processes the umts specific Current Call Meter (CCM)
       information request packet from QXDM and sends the response back.

============================================================================*/
/*lint -esym(765, cmdiag_cm_get_ccm_info)
** Can't be static, to be called by DIAG via COMMAND DISPATCH TABLE
*/
PACKED void *cmdiag_cm_get_ccm_info
(
  PACKED void* req_pkt,
  uint16 pkt_len
)
/*lint -esym(715,req_pkt,pkt_len)*/
/*lint -esym(818, req_pkt)*/
{
  DIAG_SUBSYS_CM_CM_GET_CCM_INFO_rsp_type *rsp_ptr ;

  const int rsp_len = sizeof(DIAG_SUBSYS_CM_CM_GET_CCM_INFO_rsp_type);

  /* Stops Lint complain DIAG_SUBSYS_CM_CM_GET_CCM_INFO_req_type */
  if (((DIAG_SUBSYS_CM_CM_GET_CCM_INFO_req_type *) req_pkt) == NULL)
  {
    CM_MSG_LOW_0 ("cmdiag_cm_get_ccm_info NULL");
  }

  /*lint -save -e641*/
  /* Allocate the memory for this */
  rsp_ptr = (DIAG_SUBSYS_CM_CM_GET_CCM_INFO_rsp_type *)diagpkt_subsys_alloc(
                                 DIAG_SUBSYS_CM,
                                 CM_GET_CCM_INFO,
                                 rsp_len );

  if (rsp_ptr != NULL)
  {
    /* Fill the response packet */
    /* Query Call Manager for CCM information */
    rsp_ptr->ccm                   = cmwaoc_get_ccm(CM_SS_MAIN);
  }
  /*lint -restore */

  return rsp_ptr ;
}/* cmdiag_cm_get_ccm_info() */
/*lint +esym(715,req_pkt,pkt_len)*/
/*lint +esym(818, req_pkt)*/

#if (defined(FEATURE_CM_MMGPS) || defined(FEATURE_CM_LCS))
/*===========================================================================
Function: cmdiag_cm_send_sups_lcs_molr
Description:

       Function processes location service request for mobile originated location
       registration.

============================================================================*/

/*lint -esym(765, cmdiag_cm_send_sups_lcs_molr)
** Can't be static, to be called by DIAG via COMMAND DISPATCH TABLE
*/
PACKED void *cmdiag_cm_send_sups_lcs_molr (PACKED void* req_pkt, uint16 pkt_len)
/*lint -esym(715,pkt_len)*/
{

  DIAG_SUBSYS_CM_CM_SEND_SUPS_LCS_MOLR_req_type *req_ptr =
    (DIAG_SUBSYS_CM_CM_SEND_SUPS_LCS_MOLR_req_type *)req_pkt;



  DIAG_SUBSYS_CM_CM_SEND_SUPS_LCS_MOLR_rsp_type *rsp_ptr;

  cm_lcs_molr_params_s_type                      sups_cmd_lcs_molr;


  const int rsp_len =
          sizeof(DIAG_SUBSYS_CM_CM_SEND_SUPS_LCS_MOLR_rsp_type);

  cmlog_client_s_type* diag_client_ptr = cmlog_get_diag_client();


  /*lint -save -e641*/
  /* Allocate the memory for this */
  rsp_ptr = (DIAG_SUBSYS_CM_CM_SEND_SUPS_LCS_MOLR_rsp_type *)
            diagpkt_subsys_alloc(DIAG_SUBSYS_CM, CM_SEND_SUPS_LCS_MOLR,
             rsp_len );
  /*lint -restore */

  /* Ensure response packet allocation */
  if (!rsp_ptr)
  {
    CM_MSG_HIGH_0("Memory alloc failure for cm_send_sups_lcs_molr");
    return NULL;
  }



  /* Sanity check */
  if (!req_ptr)
  {
    rsp_ptr->status = FALSE;
  }
  else
  {
    /* Copy ss_ref to rsp ptr */
    rsp_ptr->ss_ref                  =  req_ptr->ss_ref;

    /* clear lcs molr request */
    memset (&sups_cmd_lcs_molr, 0, sizeof(cm_lcs_molr_params_s_type));

    /* Copy fields into sups cmd */
    sups_cmd_lcs_molr.ss_ref         =   req_ptr->ss_ref;
    sups_cmd_lcs_molr.molr_type      =   (cm_molr_type_e_type) req_ptr->molr_type;

    /* Location method */
    sups_cmd_lcs_molr.location_method.present                       =
                                       req_ptr->location_method_present;

    sups_cmd_lcs_molr.location_method.data                          =
                        (location_method_e_type) req_ptr->location_method_data;



    /* Location qos */
    sups_cmd_lcs_molr.lcs_qos.present  =   req_ptr->lcs_qos_present;

    sups_cmd_lcs_molr.lcs_qos.horiz_accuracy.present                =
                                       req_ptr->loc_qos_hor_accur_present;

    sups_cmd_lcs_molr.lcs_qos.horiz_accuracy.data                   =
                                       req_ptr->lcs_qos_hor_accur_data;

    sups_cmd_lcs_molr.lcs_qos.vertical_coord_req.present            =
                                       req_ptr->lcs_qos_ver_cord_req_present;

    sups_cmd_lcs_molr.lcs_qos.vertical_accuracy.present             =
                                       req_ptr->lcs_qos_ver_accur_present;

    sups_cmd_lcs_molr.lcs_qos.vertical_accuracy.data                =
                                       req_ptr->lcs_qos_ver_accur_data;

    sups_cmd_lcs_molr.lcs_qos.response_time.present                 =
                                       req_ptr->lcs_qos_rsp_time_present;

    sups_cmd_lcs_molr.lcs_qos.response_time.response_time_category  =
         (response_time_category_e_type) req_ptr->lcs_qos_rsp_time_categ;


    /* External id */

    if (req_ptr->lcs_client_external_add_size > CM_LCS_CLIENT_EXT_ADD_DATA_MAX)
    {
      CM_ERR_1("external address size exceeds %d bytes",
             CM_LCS_CLIENT_EXT_ADD_DATA_MAX);
      rsp_ptr->status = FALSE;

      return rsp_ptr;
    }

    sups_cmd_lcs_molr.external_id.present                           =
                                       req_ptr->lcs_client_external_id_present;

    sups_cmd_lcs_molr.external_id.external_address.present          =
                                       req_ptr->lcs_client_external_add_present;

    /* Copy external add data from req_ptr to sups cmd */
    sups_cmd_lcs_molr.external_id.external_address.size = memscpy( sups_cmd_lcs_molr.external_id.external_address.data,
                                                                   sizeof(sups_cmd_lcs_molr.external_id.external_address.data),
                                                                   (const byte *)req_ptr->lcs_client_external_add_data,
                                                                   req_ptr->lcs_client_external_add_size );

    /* MLC number */
    if (req_ptr->mlc_number_size > CM_MLC_NUM_DATA_MAX)
    {
      CM_ERR_1("mlc number data exceeds %d bytes", CM_MLC_NUM_DATA_MAX);
      rsp_ptr->status = FALSE;

      return rsp_ptr;
    }


    sups_cmd_lcs_molr.mlc_number.present      =   req_ptr->mlc_number_present;

    /* Copy MLC number data */
    sups_cmd_lcs_molr.mlc_number.size = memscpy( sups_cmd_lcs_molr.mlc_number.data,
                                                 sizeof(sups_cmd_lcs_molr.mlc_number.data),
                                                 (const byte *) req_ptr->mlc_number_data,
                                                 req_ptr->mlc_number_size );


    /* GPS assistance data */
    if (req_ptr->gps_assis_data_size > CM_GPS_ASSIS_DATA_MAX)
    {
      CM_ERR_1("GPS assistance data exceeds %d bytes",CM_GPS_ASSIS_DATA_MAX);
      rsp_ptr->status = FALSE;

      return rsp_ptr;
    }

    /* GPS Assistance */
    sups_cmd_lcs_molr.gps_assistance_data.present =
                                       req_ptr->gps_assis_data_present;

    /* Copy GPS assistance data */
    sups_cmd_lcs_molr.gps_assistance_data.size = memscpy( sups_cmd_lcs_molr.gps_assistance_data.data,
                                                          sizeof(sups_cmd_lcs_molr.gps_assistance_data.data),
                                                          (const byte *) req_ptr->gps_assis_data,
                                                          req_ptr->gps_assis_data_size );


    /* Supported GAD shapes */
    sups_cmd_lcs_molr.supported_gad_shapes.present      =
                                       req_ptr->supp_gad_shapes_present;
    sups_cmd_lcs_molr.supported_gad_shapes.data         =
                                       req_ptr->supp_gad_shapes_data;

    rsp_ptr->status =
    cm_sups_cmd_lcs_molr( NULL,
                          NULL,
                          diag_client_ptr->client,
                          &sups_cmd_lcs_molr );

  }

  return rsp_ptr;

} /* cmdiag_cm_send_sups_lcs_molr () */
/*lint +esym(715,pkt_len)*/

#endif /* #ifdef FEATURE_CM_MMGPS || defined(FEATURE_CM_LCS) */

/*===========================================================================
Function: cmdiag_cm_send_sups_lcs_molr_completed
Description:

       Function processes location service request to indicate completion of
       Mobile originated location registration.

============================================================================*/
/*lint -esym(765, cmdiag_cm_send_sups_lcs_molr_completed)
** Can't be static, to be called by DIAG via COMMAND DISPATCH TABLE
*/
/*lint -esym(714, cmdiag_cm_send_sups_lcs_molr_completed) not referenced */


PACKED void *cmdiag_cm_send_sups_lcs_molr_completed (

  PACKED void* req_pkt,
    /* diag request to CM */

  uint16 pkt_len
    /* length of diag request in bytes */

)
/*lint -esym(715, pkt_len)*/
{
  DIAG_SUBSYS_CM_CM_SEND_SUPS_LCS_MOLR_COMPLETED_req_type *req_ptr =
    (DIAG_SUBSYS_CM_CM_SEND_SUPS_LCS_MOLR_COMPLETED_req_type *)req_pkt;

  DIAG_SUBSYS_CM_CM_SEND_SUPS_LCS_MOLR_COMPLETED_rsp_type *rsp_ptr = NULL;

  cm_lcs_molr_completed_params_s_type           sups_cmd_lcs_molr_completed;

  const int rsp_len =
          sizeof(DIAG_SUBSYS_CM_CM_SEND_SUPS_LCS_MOLR_COMPLETED_rsp_type);

  cmlog_client_s_type* diag_client_ptr = cmlog_get_diag_client();

  /*lint -save -e641*/
  /* Allocate the memory for this */
  rsp_ptr = (DIAG_SUBSYS_CM_CM_SEND_SUPS_LCS_MOLR_COMPLETED_rsp_type *)
            diagpkt_subsys_alloc(DIAG_SUBSYS_CM, CM_SEND_SUPS_LCS_MOLR_COMPLETED,
             rsp_len );
  /*lint -restore */

  /* Ensure response packet allocation */
  if (!rsp_ptr)
  {
    CM_MSG_HIGH_0("Memory alloc failure for cm_send_sups_lcs_molr_completed");
    return NULL;
  }


  /* clear lcs molr completed */
  memset (&sups_cmd_lcs_molr_completed, 0,
           sizeof(cm_lcs_molr_completed_params_s_type));

  if (req_ptr != NULL)
  {
    /* Copy ss_ref to rsp ptr */
    rsp_ptr->ss_ref                  =  req_ptr->ss_ref;

    sups_cmd_lcs_molr_completed.ss_ref  = req_ptr->ss_ref;

    rsp_ptr->status = cm_sups_cmd_lcs_molr_completed(
                           NULL,
                           NULL,
                           diag_client_ptr->client,
                           &sups_cmd_lcs_molr_completed);

  }
  else
  {
    rsp_ptr->status = FALSE;
  }

  return rsp_ptr;
} /* cmdiag_cm_send_sups_lcs_molr_completed () */
  /*lint +esym(715, pkt_len)*/

/*===========================================================================
Function: cmdiag_wcdma_call_origination
Description:

       This function processes the wcdma specific call orig request packet
       from QXDM and sends the response back.

============================================================================*/

/*lint -esym(765, cmdiag_wcdma_call_origination)
** Can't be static, to be called by DIAG via COMMAND DISPATCH TABLE
*/
PACKED void *cmdiag_wcdma_call_origination (PACKED void* req_pkt, uint16 pkt_len)
{
  DIAG_SUBSYS_CM_CM_WCDMA_CALL_ORIGINATION_req_type *req_ptr =
    (DIAG_SUBSYS_CM_CM_WCDMA_CALL_ORIGINATION_req_type *)req_pkt;
  DIAG_SUBSYS_CM_CM_WCDMA_CALL_ORIGINATION_rsp_type *rsp_ptr;

  cm_num_s_type                numbers_buf;
  uint8                        work_buf[CM_MAX_NUMBER_CHARS];
  uint8                        i;
  cm_gw_cs_orig_params_s_type  call_orig_params;
  cm_num_s_type                calling_num;
  cm_orig_alpha_s_type         alpha_buf;

  const int rsp_len = sizeof(DIAG_SUBSYS_CM_CM_WCDMA_CALL_ORIGINATION_rsp_type);

  /*lint -save -e641*/
  /* Allocate the memory for this */
  rsp_ptr = (DIAG_SUBSYS_CM_CM_WCDMA_CALL_ORIGINATION_rsp_type *)diagpkt_subsys_alloc(DIAG_SUBSYS_CM, CM_WCDMA_CALL_ORIGINATION,
                                 rsp_len );
  /*lint -restore */

  /* Check for packet length */
  if (cmdiag_check_request_length (pkt_len, req_pkt, CM_WCDMA_CALL_ORIGINATION) == FALSE)
  {
    CM_ERR_0("Packet length check failed");
    return rsp_ptr;
  }

  /* Ensure response packet allocation */
  if (!rsp_ptr)
  {

    CM_MSG_HIGH_0("wcdmadiag call orig memory alloc failed for response packet");
    return NULL;
  }

  /* Fill the memory returned, with 0s */
  rsp_ptr->status = 0;
  rsp_ptr->call_id = 0;

  /* Initialize the buffer to CM
  */
  memset(&call_orig_params,
         CM_CALL_CMD_PARAM_DEFAULT_VALUE,
         sizeof(call_orig_params));

  /* amr_rate is a value between 0 to 7 */
  if (req_ptr->amr_rate > 7)
  {
    return NULL;
  }

  if ( (req_ptr->num_digits > DIAG_MAX_WCDMA_DIGITS) ||
       (req_ptr->num_digits > CM_MAX_NUMBER_CHARS) )
  {
    CM_ERR_0("Dialed too many digits");
    return rsp_ptr;
  }

  /* Initialize work_buf
  */
  memset (work_buf, 0, ARR_SIZE(work_buf));

  for (i=0; i<req_ptr->num_digits; i++)
  {
    /* Check for dial digits (0 - 9, *, #) */
    if ( ((req_ptr->digits[i] < '0') || (req_ptr->digits[i] > '9')) &&
         (req_ptr->digits[i] != '*')  &&
         (req_ptr->digits[i] != '#'))
    {
        /* Was an invalid digit (not 0 - 9, *, #) */
        CM_ERR_1("Bad dial digit: %c", req_ptr->digits[i]);
        return rsp_ptr;
    }
    work_buf[i] = req_ptr->digits[i];
  }

  alpha_buf.len = memscpy( alpha_buf.buf, sizeof(alpha_buf.buf), (const byte *)"DIAG CALL", (uint8) strlen("DIAG CALL") );
  cm_num_init( &calling_num );

  cm_num_fill(&numbers_buf, work_buf, req_ptr->num_digits, CM_DIGIT_MODE_4BIT_DTMF);


  /* Fill in the CUG. */
  if (use_cug)
  {
    call_orig_params.forward_cug_info.present = TRUE;
    call_orig_params.forward_cug_info.cm_cug_index.present = TRUE;
    call_orig_params.forward_cug_info.cm_cug_index.msb =
      (cug_group_id & 0xff00) >> 8;
    call_orig_params.forward_cug_info.cm_cug_index.lsb =
      (cug_group_id & 0x00ff);
    call_orig_params.forward_cug_info.cm_suppress_pref_cug =
      cug_suppress_pref_cug;
    call_orig_params.forward_cug_info.cm_suppress_outgoing_access =
      cug_suppress_oa;
  }
  rsp_ptr->status = cm_mm_call_cmd_orig_exten3(
    NULL, /* client callback function */
    NULL, /* pointer to client callback data block */
    CM_CLIENT_ID_ANONYMOUS,
    CM_CALL_TYPE_VOICE,
    CM_SRV_TYPE_AUTOMATIC,
    &calling_num,
    &numbers_buf,
    &alpha_buf,        /* Alpha buffer for call  */
    NULL,              /* CDMA call orig params  */
    &call_orig_params, /* GW CS call orig params */
    NULL,              /* GW PS call orig params */
    CM_CALL_ID_INVALID,
    CM_ORIG_CLIENT_CATEG_USER,
    &wcdma_call_id
   );


  /* call_id is valid only when status is TRUE*/
  if (rsp_ptr->status == TRUE)
  {

    rsp_ptr->call_id = wcdma_call_id;

    CM_MSG_HIGH_1("wcdma call id = %d", wcdma_call_id);
  }/* if (rsp_ptr->status == TRUE) */

  return rsp_ptr ;

} /* cmdiag_wcdma_call_origination() */

/*===========================================================================
Function: cmdiag_gsm_call_origination
Description:

       This function processes the gsm specific call orig request packet
       from QXDM and sends the response back.

============================================================================*/

/*lint -esym(765, cmdiag_gsm_call_origination)
** Can't be static, to be called by DIAG via COMMAND DISPATCH TABLE
*/
PACKED void *cmdiag_gsm_call_origination (PACKED void* req_pkt, uint16 pkt_len)
{

  DIAG_SUBSYS_CM_CM_GSM_CALL_ORIGINATION_req_type *req_ptr =
    (DIAG_SUBSYS_CM_CM_GSM_CALL_ORIGINATION_req_type *)req_pkt;
  DIAG_SUBSYS_CM_CM_GSM_CALL_ORIGINATION_rsp_type *rsp_ptr;

  cm_num_s_type     numbers_buf;
  uint8       work_buf[CM_MAX_NUMBER_CHARS];
  uint8       i;
  cm_gw_cs_orig_params_s_type   call_orig_params;
  cm_num_s_type     calling_num;
  cm_orig_alpha_s_type    alpha_buf;

  const int rsp_len = sizeof(DIAG_SUBSYS_CM_CM_GSM_CALL_ORIGINATION_rsp_type);

  /*lint -save -e641*/
  /* Allocate the memory for this */
  rsp_ptr = (DIAG_SUBSYS_CM_CM_GSM_CALL_ORIGINATION_rsp_type *)diagpkt_subsys_alloc (DIAG_SUBSYS_CM, CM_GSM_CALL_ORIGINATION, rsp_len);
  /*lint -restore */

  /* Ensure response packet allocation */
  if (!rsp_ptr)
  {

    CM_MSG_HIGH_0("gsmdiag call orig memory alloc failed for response packet");
    return NULL;
  }

  /* Check for packet length */
  if (cmdiag_check_request_length (pkt_len, req_pkt, CM_GSM_CALL_ORIGINATION) == FALSE)
  {
    CM_ERR_0("Packet length check failed");
    return rsp_ptr;
  }

  /* Fill the memory returned, with 0s */
  rsp_ptr->status = 0;
  rsp_ptr->call_id = 0;

  /* Initialize the buffer to CM */
  memset(&call_orig_params,
        CM_CALL_CMD_PARAM_DEFAULT_VALUE,
        sizeof(call_orig_params));


  /* Sanity check */
  if (!req_ptr)
  {
    rsp_ptr->status = FALSE;
  }
  else
  {
    if ( (req_ptr->num_digits > CM_DIAG_MAX_GSM_DIGITS)
       || (req_ptr->num_digits > CM_MAX_NUMBER_CHARS) )
    {
      CM_ERR_0("Dialed too many digits");
      return rsp_ptr;
    }

    /* Initialize work_buf
    */
    memset (work_buf, 0, ARR_SIZE(work_buf));


    for (i=0; i<req_ptr->num_digits; i++)
    {
      /* Check for dial digits (0 - 9, *, #) */
      if ( ((req_ptr->digits[i] < '0') || (req_ptr->digits[i] > '9')) &&
           (req_ptr->digits[i] != '*')  &&
           (req_ptr->digits[i] != '#')) {
           /* Was an invalid digit (not 0 - 9, *, #) */
           CM_ERR_1("Bad dial digit: %c", req_ptr->digits[i]);
           return rsp_ptr;
      }
      work_buf[i] = req_ptr->digits[i];
    }

    alpha_buf.len = memscpy( alpha_buf.buf, sizeof(alpha_buf.buf), (const byte *)"DIAG CALL", strlen("DIAG CALL") );
    cm_num_init( &calling_num );
    cm_num_fill( &numbers_buf, work_buf, req_ptr->num_digits, CM_DIGIT_MODE_4BIT_DTMF);


     /* Fill the memory returned, with 0s */
    rsp_ptr->status = 0;
    rsp_ptr->call_id = 0;

    /* Fill in the CUG. */
    if (use_cug)
    {
      call_orig_params.forward_cug_info.present = TRUE;
      call_orig_params.forward_cug_info.cm_cug_index.present = TRUE;
      call_orig_params.forward_cug_info.cm_cug_index.msb =
        (cug_group_id & 0xff00) >> 8;
      call_orig_params.forward_cug_info.cm_cug_index.lsb =
        (cug_group_id & 0x00ff);
      call_orig_params.forward_cug_info.cm_suppress_pref_cug =
        cug_suppress_pref_cug;
      call_orig_params.forward_cug_info.cm_suppress_outgoing_access =
        cug_suppress_oa;
    }
    rsp_ptr->status = cm_mm_call_cmd_orig_exten3(
      NULL, /* client callback function */
      NULL, /* pointer to client callback data block */
      CM_CLIENT_ID_ANONYMOUS, /* client id */
      CM_CALL_TYPE_VOICE, /* call type */
      CM_SRV_TYPE_AUTOMATIC,  /* service type */
      &calling_num,  /* service option */
      &numbers_buf,
      &alpha_buf,        /* Alpha buffer for call  */
      NULL,              /* CDMA call orig params  */
      &call_orig_params, /* GW CS call orig params */
      NULL,              /* GW PS call orig params */
      CM_CALL_ID_INVALID,
      CM_ORIG_CLIENT_CATEG_USER,
      &gsm_call_id
      );

      /* call_id is valid only when status is TRUE*/
      if (rsp_ptr->status == TRUE)
      {

        rsp_ptr->call_id = gsm_call_id;
      }/* if (rsp_ptr->status == TRUE) */

  } /* if (!req_ptr) */

  return rsp_ptr ;


} /* cmdiag_gsm_call_origination() */

#endif /* FEATURE_GSM || FEATURE_WCDMA */


#if (defined (FEATURE_CALL_THROTTLE))
/*===========================================================================
Function: cmdiag_orig_thr_tbl_reset
Description:

       This function processes the Throttle table reset request packet
       from QXDM and sends the response back.

============================================================================*/

/*lint -esym(765, cmdiag_orig_thr_tbl_reset)
** Can't be static, to be called by DIAG via COMMAND DISPATCH TABLE
*/
PACKED void *cmdiag_orig_thr_tbl_reset (PACKED void* req_pkt, uint16 pkt_len)
{

  DIAG_SUBSYS_CM_CM_ORIG_THR_TBL_RESET_req_type *req_ptr =
                    (DIAG_SUBSYS_CM_CM_ORIG_THR_TBL_RESET_req_type *)req_pkt;
  DIAG_SUBSYS_CM_CM_ORIG_THR_TBL_RESET_rsp_type *rsp_ptr;

  const int rsp_len = sizeof(DIAG_SUBSYS_CM_CM_ORIG_THR_TBL_RESET_rsp_type);

  /*lint -save -e641*/
  /* Allocate the memory for this */
  rsp_ptr = (DIAG_SUBSYS_CM_CM_ORIG_THR_TBL_RESET_rsp_type *)
       diagpkt_subsys_alloc (DIAG_SUBSYS_CM, CM_ORIG_THR_TBL_RESET, rsp_len);
  /*lint -restore */

  SYS_ARG_NOT_USED( pkt_len );

  /* Ensure response packet allocation */
  if (!rsp_ptr)
  {

    CM_MSG_HIGH_0("orig thr tbl reset memory alloc failed for response packet");
    return NULL;
  }

  /* Fill the memory returned, with 0s */
  rsp_ptr->status = 0;

  /* Sanity check */
  if (!req_ptr)
  {
    rsp_ptr->status = FALSE;
  }
  else
  {

    rsp_ptr->status =  cm_ss_cmd_orig_thr_update_tbl(
                                            NULL,
                                            NULL,
                                            cmclient_get_client_id_for_client_type(CM_CLIENT_TYPE_DIAG),
                                            CM_SS_ORIG_THR_RESET_TBL,
                                            CM_SS_ORIG_THR_DS_MIP_FA_FAILURE,
                                            CM_CALL_TYPE_PS_DATA,
                                            SYS_SYS_MODE_NONE,
                                            NULL,
                                            SYS_BAND_BC0,
                                            0 );
  }
  return rsp_ptr ;

} /* cmdiag_orig_thr_tbl_reset */

#endif /* (defined(FEATURE_CALL_THROTTLE)) */


/*===========================================================================
Function: cmdiag_cm_system_selection2
Description:

       This function processes the call manager system selection request2
       packet from QXDM and sends the response back.

       Note: It expands the band preference from 32-bit to 64-bit. The rest
       fields are same as those of call manager system selection request.

============================================================================*/
/*lint -esym(765, cmdiag_cm_set_system_selection2)
** Can't be static, to be called by DIAG via COMMAND DISPATCH TABLE
*/
PACKED void *cmdiag_cm_set_system_selection2 (PACKED void* req_pkt, uint16 pkt_len)
{
    DIAG_SUBSYS_CM_CM_SYSTEM_SELECTION_PREFERENCE2_req_type *req_ptr
            = (DIAG_SUBSYS_CM_CM_SYSTEM_SELECTION_PREFERENCE2_req_type*)req_pkt;
    DIAG_SUBSYS_CM_CM_SYSTEM_SELECTION_PREFERENCE2_rsp_type *rsp_ptr;
    const int rsp_len = sizeof(DIAG_SUBSYS_CM_CM_SYSTEM_SELECTION_PREFERENCE2_rsp_type);

    /*lint -save -e641*/

    cm_network_sel_mode_pref_e_type  nw_sel_mode_pref =
                                          CM_NETWORK_SEL_MODE_PREF_NO_CHANGE;

    sys_plmn_id_s_type               plmn_struct;

    cm_mode_pref_e_type               mode_pref = CM_MODE_PREF_NO_CHANGE;
    cm_gw_acq_order_pref_e_type       acq_order_pref =
                                              CM_GW_ACQ_ORDER_PREF_NO_CHANGE;
    cm_band_pref_e_type               band_pref = CM_BAND_PREF_NO_CHANGE;
    cm_roam_pref_e_type               roam_pref = CM_ROAM_PREF_NO_CHANGE;

    cm_hybr_pref_e_type               hybr_pref = CM_HYBR_PREF_NO_CHANGE;
    cm_srv_domain_pref_e_type         srv_domain_pref =
                                                CM_SRV_DOMAIN_PREF_NO_CHANGE;

    cmlog_client_s_type* diag_client_ptr = cmlog_get_diag_client();

    /* Allocate the memory for this */
    rsp_ptr = (DIAG_SUBSYS_CM_CM_SYSTEM_SELECTION_PREFERENCE2_rsp_type *)
    diagpkt_subsys_alloc(DIAG_SUBSYS_CM,
                         CM_SYSTEM_SELECTION_PREFERENCE2,
                         rsp_len );

    /* Ensure response packet allocation */
    if (!rsp_ptr)
    {
        CM_ERR_0(" System reference2 mem alloc fail for response pkt ");
        return NULL;
    }

    /* Sanity check request packet */
    if (!cmdiag_check_request_length (
                                  pkt_len,
                                  req_pkt,
                                  CM_SYSTEM_SELECTION_PREFERENCE2))
    {
        rsp_ptr->status = FALSE;

        CM_ERR_0("System reference2 Packet length check failed");
        /* Return status now.*/
        return rsp_ptr;

    }

    /* Get params from Request Packet */
    /* Convert from little endian notation */
    plmn_struct.identity[0] = req_ptr->PLMN[2];
    plmn_struct.identity[1] = req_ptr->PLMN[1];
    plmn_struct.identity[2] = req_ptr->PLMN[0];


    /* Update the system prefernce*/
    if( req_ptr->network_sel_mode_pref < (unsigned int)CM_NETWORK_SEL_MODE_PREF_MAX )
    {
      nw_sel_mode_pref = (cm_network_sel_mode_pref_e_type) req_ptr->network_sel_mode_pref;
    }
    else
    {
      CM_ERR_1 ("Bad nw_sel_mode_pref %d, using no change",
                    req_ptr->network_sel_mode_pref);
    }

    if( req_ptr->mode_pref < (unsigned int)CM_MODE_PREF_MAX )
    {
      mode_pref = (cm_mode_pref_e_type) req_ptr->mode_pref;
    }
    else
    {
      CM_ERR_1 ("Bad mode_pref %d, using no change",req_ptr->mode_pref);
    }

    if( req_ptr->acq_order_pref < (unsigned int)CM_GW_ACQ_ORDER_PREF_MAX )
    {
      acq_order_pref = (cm_gw_acq_order_pref_e_type) req_ptr->acq_order_pref;
    }
    else
    {
      CM_ERR_1 ("Bad acq_order_pref %d, using no change",
                    req_ptr->acq_order_pref);
    }

    if( req_ptr->band_pref < CM_BAND_PREF_MAX )
    {
      band_pref = (cm_band_pref_e_type) req_ptr->band_pref;
    }
    else
    {
      CM_ERR_1 ("Bad band_pref %d, using no change",
                    req_ptr->band_pref);
    }

    if( req_ptr->roam_pref < (unsigned int)CM_ROAM_PREF_MAX )
    {
      roam_pref = (cm_roam_pref_e_type) req_ptr->roam_pref;
    }
    else
    {
      CM_ERR_1 ("Bad roam_pref %d, using no change",
                 req_ptr->roam_pref);
    }

    if( req_ptr->hybr_pref < (unsigned int)CM_HYBR_PREF_MAX )
    {
      hybr_pref = (cm_hybr_pref_e_type) req_ptr->hybr_pref;
    }
    else
    {
      CM_ERR_1 ("Bad hybr_pref %d, using no change",
                 req_ptr->hybr_pref);
    }

    if( req_ptr->srv_domain_pref < (unsigned int)CM_SRV_DOMAIN_PREF_MAX )
    {
      srv_domain_pref = (cm_srv_domain_pref_e_type) req_ptr->srv_domain_pref;
    }
    else
    {
      CM_ERR_1 ("Bad srv_domain_pref %d, using no change",
                    req_ptr->srv_domain_pref);
    }

    /* Issue SYS_SEL_PREF & Fill the response packet */
    rsp_ptr->status = cm_ph_cmd_sys_sel_pref
              (
                NULL,
                NULL,
                diag_client_ptr->client,
                mode_pref,
                CM_PREF_TERM_PERMANENT,
                0,
                acq_order_pref,
                band_pref,
                CM_PRL_PREF_NO_CHANGE,
                roam_pref,
                hybr_pref,
                srv_domain_pref,
                nw_sel_mode_pref,
                &plmn_struct,
        NULL
             );


    CM_MSG_HIGH_1( "Status %d", rsp_ptr->status);

    /* Return status.*/
    return rsp_ptr ;
}


/*===========================================================================
Function: cmdiag_cm_get_cell_service_ind
Description:

       This function processes the call manager cell service indication
       request from QXDM and sends the response back.
       Note : Currently supports hsdpa_hsupa and dtm

============================================================================*/
/*lint -esym(765, cmdiag_cm_get_cell_service_ind)
** Can't be static, to be called by DIAG via COMMAND DISPATCH TABLE
*/
PACKED void *cmdiag_cm_get_cell_service_ind(PACKED void* req_pkt, uint16 pkt_len)
/*lint -esym(818, req_pkt)*/
{

  DIAG_SUBSYS_CM_CM_GET_CELL_SERVICE_IND_rsp_type *rsp_ptr;
  const int rsp_len = sizeof(DIAG_SUBSYS_CM_CM_GET_CELL_SERVICE_IND_rsp_type);
  cmlog_client_s_type* diag_client_ptr = cmlog_get_diag_client();

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  rsp_ptr = (DIAG_SUBSYS_CM_CM_GET_CELL_SERVICE_IND_rsp_type *)
            diagpkt_subsys_alloc(DIAG_SUBSYS_CM,
                                 CM_GET_CELL_SERVICE_IND,
                                 rsp_len );

  /* Ensure response packet allocation */
  if (!rsp_ptr)
  {
      CM_ERR_0(" CELL Service IND mem alloc fail for response pkt ");
      return NULL;
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Sanity check request packet */
  if (!cmdiag_check_request_length (
                                pkt_len,
                                req_pkt,
                                CM_GET_CELL_SERVICE_IND))
  {

      CM_ERR_0("CELL SERVICE IND Packet length check failed");
      /* Return status now.*/
      return rsp_ptr;

  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Filling cell service indication */


  rsp_ptr->hsdpa_hsupa_support = (uint8)diag_client_ptr->cell_srv_ind.hs_ind;
  rsp_ptr->dtm_support = (uint8)diag_client_ptr->cell_srv_ind.dtm_supp;
  rsp_ptr->reserved1 = 0;
  rsp_ptr->reserved2 = 0;
  rsp_ptr->reserved3 = 0;


  return rsp_ptr;

}
/*lint +esym(818, req_pkt)*/

#if ((defined CM_GW_SUPPORTED) && defined (FEATURE_MULTIMEDIA_FALLBACK))
#error code not present
#endif  /* FEATURE_GSM || FEATURE_WCDMA && FEATURE_MULTIMEDIA_FALLBACK */


#ifdef FEATURE_MMODE_DUAL_SIM

/*===========================================================================
Function: cmdiag_cm_state_info_per_subs
Description:

       This function processes the umts specific call manager state
       information request packet from QXDM and sends the response back.

============================================================================*/
/*lint -esym(765, cmdiag_cm_state_info_per_subs)
** Can't be static, to be called by DIAG via COMMAND DISPATCH TABLE
*/
PACKED void *cmdiag_cm_state_info_per_subs
(
  PACKED void* req_pkt,

  uint16       pkt_len
)
/*lint -esym(818, req_pkt) diagpkt_user_table_entry_type prohibits that */
/*lint -esym(715,req_pkt)*/
/*lint -esym(715,pkt_len)*/
{
  DIAG_SUBSYS_CM_CM_STATE_INFO_PER_SUBS_rsp_type *rsp_ptr;
  DIAG_SUBSYS_CM_CM_STATE_INFO_PER_SUBS_req_type *req_ptr;

  const int rsp_len = sizeof(DIAG_SUBSYS_CM_CM_STATE_INFO_PER_SUBS_rsp_type);
  cmlog_client_s_type* diag_client_ptr = cmlog_get_diag_client();

  req_ptr = (DIAG_SUBSYS_CM_CM_STATE_INFO_PER_SUBS_req_type *)req_pkt;
  
  /*lint -save -e641*/
  /* Allocate the memory for this */
  rsp_ptr = (DIAG_SUBSYS_CM_CM_STATE_INFO_PER_SUBS_rsp_type *)diagpkt_subsys_alloc(DIAG_SUBSYS_CM, CM_STATE_INFO_PER_SUBS,
                                 rsp_len );

  if (req_ptr != NULL)
  {
  CM_MSG_HIGH_1("cmdiag_cm_state_info_per_subs as_id %d",\
    req_ptr->asubs_id );
  }
  if (rsp_ptr != NULL && req_ptr != NULL )
  {
    rsp_ptr->oprt_mode             = diag_client_ptr->oprt_mode;
    rsp_ptr->asubs_id              = req_ptr->asubs_id;
    #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
    if( req_ptr->asubs_id == SYS_MODEM_AS_ID_3 || (req_ptr->asubs_id == SYS_MODEM_AS_ID_2 && cmph_is_sxlte()))
    {
      /* Fill the response packet */
      /* Query Call Manager for State information */
      rsp_ptr->call_state            = cmcall_overall_call_state_per_subs(CM_SS_HYBR_3);
      rsp_ptr->system_mode           = diag_client_ptr->gw3_sys_mode;
      rsp_ptr->mode_pref             = diag_client_ptr->gw3_mode_pref;
      rsp_ptr->acq_order_pref        = diag_client_ptr->gw3_acq_order_pref;
      rsp_ptr->srv_domain_pref       = diag_client_ptr->gw3_srv_domain_pref;
      rsp_ptr->band_pref             = (int)diag_client_ptr->gw3_band_pref;
      rsp_ptr->roam_pref             = diag_client_ptr->gw3_roam_pref;
      rsp_ptr->hybr_pref             = diag_client_ptr->gw3_hybr_pref;
      rsp_ptr->network_sel_mode_pref = diag_client_ptr->gw3_network_sel_mode_pref;
      rsp_ptr->srv_status            = diag_client_ptr->gw3_srv_status;
    }
    else
    #endif /* FEATURE_MMODE_TRIPLE_SIM */
    if( req_ptr->asubs_id == SYS_MODEM_AS_ID_2 )
    {
      /* Fill the response packet */
      /* Query Call Manager for State information */
      rsp_ptr->call_state            = cmcall_overall_call_state_per_subs(CM_SS_HYBR_2);
      rsp_ptr->system_mode           = diag_client_ptr->gw_sys_mode;
      rsp_ptr->mode_pref             = diag_client_ptr->gw_mode_pref;
      rsp_ptr->acq_order_pref        = diag_client_ptr->gw_acq_order_pref;
      rsp_ptr->srv_domain_pref       = diag_client_ptr->gw_srv_domain_pref;
      rsp_ptr->band_pref             = (int)diag_client_ptr->gw_band_pref;
      rsp_ptr->roam_pref             = diag_client_ptr->gw_roam_pref;
      rsp_ptr->hybr_pref             = diag_client_ptr->gw_hybr_pref;
      rsp_ptr->network_sel_mode_pref = diag_client_ptr->gw_network_sel_mode_pref;
      rsp_ptr->srv_status            = diag_client_ptr->gw_srv_status;
    }
    else
    {
      /* Fill the response packet */
      /* Query Call Manager for State information */
      rsp_ptr->call_state            = cmcall_overall_call_state_per_subs(CM_SS_MAIN);
      rsp_ptr->system_mode           = diag_client_ptr->sys_mode;
      rsp_ptr->mode_pref             = diag_client_ptr->mode_pref;
      rsp_ptr->acq_order_pref        = diag_client_ptr->acq_order_pref;
      rsp_ptr->srv_domain_pref       = diag_client_ptr->srv_domain_pref;
      rsp_ptr->band_pref             = (int)diag_client_ptr->band_pref;
      rsp_ptr->roam_pref             = diag_client_ptr->roam_pref;
      rsp_ptr->hybr_pref             = diag_client_ptr->hybr_pref;
      rsp_ptr->network_sel_mode_pref = diag_client_ptr->network_sel_mode_pref;
      rsp_ptr->srv_status            = diag_client_ptr->srv_status;
    }
  }
  /*lint -restore */

  return rsp_ptr ;
}
/*lint +esym(715,req_pkt)*/
/*lint +esym(715,pkt_len)*/
/*lint +esym(818, req_pkt) */

/*===========================================================================
Function: cmdiag_cm_multisim_state_info
Description:

       This function forms the response for multi-SIM state info.

============================================================================*/
/*
** Can't be static, to be called by DIAG via COMMAND DISPATCH TABLE
*/
PACKED void *cmdiag_cm_multisim_state_info
(
  PACKED void* req_pkt,

  uint16       pkt_len
)
{
  DIAG_SUBSYS_CM_CM_MULTISIM_STATE_INFO_rsp_type *rsp_ptr;
  DIAG_SUBSYS_CM_CM_MULTISIM_STATE_INFO_req_type *req_ptr;
  const int rsp_len = sizeof(DIAG_SUBSYS_CM_CM_MULTISIM_STATE_INFO_rsp_type);
  cmph_s_type *ph_ptr = cmph_ptr();

  req_ptr = (DIAG_SUBSYS_CM_CM_MULTISIM_STATE_INFO_req_type *)req_pkt;
  rsp_ptr = (DIAG_SUBSYS_CM_CM_MULTISIM_STATE_INFO_rsp_type *)diagpkt_subsys_alloc(DIAG_SUBSYS_CM,
            CM_MULTISIM_STATE_INFO, rsp_len );

  if(rsp_ptr != NULL && req_ptr != NULL)
  {
    rsp_ptr->active_subs = ph_ptr->active_subs;
    rsp_ptr->standby_pref = (sys_modem_dual_standby_pref_e_type)ph_ptr->standby_pref;
    rsp_ptr->default_data_subs = (sys_modem_as_id_e_type)ph_ptr->default_data_subs;
    rsp_ptr->priority_subs = (sys_modem_as_id_e_type)ph_ptr->priority_subs;
  }

  return rsp_ptr ;
}


/*===========================================================================
Function: cmdiag_cm_get_plmn_list_per_subs
Description:

       This function processes the call manager plmn list request
       packet from QXDM and sends the response back.

============================================================================*/
/*lint -esym(765, cmdiag_cm_get_plmn_list_per_subs)
** Can't be static, to be called by DIAG via COMMAND DISPATCH TABLE
*/
PACKED void *cmdiag_cm_get_plmn_list_per_subs
(
  PACKED void   *req_pkt,

  uint16        pkt_len
)
/*lint -esym(818, req_pkt) diagpkt_user_table_entry_type prohibits that */
/*lint -esym(715,req_pkt) */
/*lint -esym(715,pkt_len) */
{
    DIAG_SUBSYS_CM_CM_PLMN_SELECTION_MODE_PER_SUBS_rsp_type *rsp_ptr ;

    DIAG_SUBSYS_CM_CM_PLMN_SELECTION_MODE_PER_SUBS_req_type *req_ptr ;

    cmlog_client_s_type* diag_client_ptr = cmlog_get_diag_client();

    /*lint -save -e641*/
    /* Allocate the memory for this */
    rsp_ptr = (DIAG_SUBSYS_CM_CM_PLMN_SELECTION_MODE_PER_SUBS_rsp_type *)diagpkt_subsys_alloc(DIAG_SUBSYS_CM, CM_PLMN_SELECTION_MODE_PER_SUBS,
                                   sizeof(DIAG_SUBSYS_CM_CM_PLMN_SELECTION_MODE_PER_SUBS_rsp_type) );
    /*lint -restore */

    req_ptr = (DIAG_SUBSYS_CM_CM_PLMN_SELECTION_MODE_PER_SUBS_req_type*) req_pkt;

    if (rsp_ptr != NULL && req_ptr != NULL)
    {   /* Fill the response packet */
        rsp_ptr->status = cm_ph_cmd_get_networks_per_subs(
            NULL,
            NULL,
            diag_client_ptr->client,
            CM_NETWORK_LIST_AVAILABLE,
            CM_MODE_PREF_GSM_WCDMA_ONLY,
            (sys_modem_as_id_e_type)req_ptr->asubs_id
            );
    }

    return rsp_ptr ;
}
/*lint +esym(715,req_pkt) */
/*lint +esym(715,pkt_len) */
/*lint +esym(818, req_pkt) diagpkt_user_table_entry_type prohibits that */

/*===========================================================================
Function: cmdiag_cm_get_call_information_list_per_subs
Description:

       This function returns call_id, call_state and call_type for all allocated
       call ids.
============================================================================*/
/*lint -esym(765, cmdiag_cm_get_call_information_list_per_subs)
** Can't be static, to be called by DIAG via COMMAND DISPATCH TABLE
*/
PACKED void *cmdiag_cm_get_call_information_list_per_subs
(
  PACKED void   *req_pkt,

  uint16        pkt_len
)
/*lint -esym(818, req_pkt) diagpkt_user_table_entry_type prohibits that */
/*lint -esym(715,req_pkt)*/
/*lint -esym(715,pkt_len)*/
{
  uint8 loop = 0;

  DIAG_SUBSYS_CM_CM_GET_CALL_INFORMATION_LIST_PER_SUBS_rsp_type *rsp_ptr ;

  DIAG_SUBSYS_CM_CM_GET_CALL_INFORMATION_LIST_PER_SUBS_req_type *req_ptr = NULL;

  const int rsp_len = sizeof(DIAG_SUBSYS_CM_CM_GET_CALL_INFORMATION_LIST_PER_SUBS_rsp_type);

  cmlog_call_state_s_type* call_state_list_ptr = cmlog_get_call_state_list();

  /* Stops Lint complain DIAG_SUBSYS_CM_CM_GET_CALL_INFORMATION_LIST_req_type */
  if (((DIAG_SUBSYS_CM_CM_GET_CALL_INFORMATION_LIST_PER_SUBS_req_type *) req_pkt) == NULL)
  {
    CM_MSG_LOW_0("cmdiag_cm_get_call_information_list, req_pkt is NULL");
  }
  else
  {
    req_ptr = (DIAG_SUBSYS_CM_CM_GET_CALL_INFORMATION_LIST_PER_SUBS_req_type *) req_pkt;
  }

  /*lint -save -e641*/
  /* Allocate the memory for response */
  rsp_ptr = (DIAG_SUBSYS_CM_CM_GET_CALL_INFORMATION_LIST_PER_SUBS_rsp_type *)
                diagpkt_subsys_alloc(DIAG_SUBSYS_CM,
                 CM_GET_CALL_INFORMATION_LIST_PER_SUBS, rsp_len );
  /*lint -restore */

  if (rsp_ptr != NULL && req_ptr != NULL)
  {
    rsp_ptr->status = TRUE;
  }
  else
  {
    return rsp_ptr;
  }

  /* Initialize number of calls to 0 */
  rsp_ptr->num_calls = 0;

  /* Fill in the call information */
  for (loop=0; loop < CMCALL_MAX_CALL_OBJ; ++loop)
  {
    if (call_state_list_ptr[loop].call_id != CM_CALL_ID_INVALID &&
        call_state_list_ptr[loop].asubs_id == req_ptr->asubs_id
       )
    {
      rsp_ptr->num_calls = rsp_ptr->num_calls + 1;

      /* Update rsp_ptr */
      rsp_ptr->call_info[rsp_ptr->num_calls - 1].call_id            =
        (byte) call_state_list_ptr[loop].call_id;

      /* call type */
      rsp_ptr->call_info[rsp_ptr->num_calls - 1].call_type          =
        (byte) call_state_list_ptr[loop].call_type;

      /* call state */
      rsp_ptr->call_info[rsp_ptr->num_calls - 1].call_state         =
        (byte) call_state_list_ptr[loop].call_state;

      /* ps call state */
      rsp_ptr->call_info[rsp_ptr->num_calls - 1].is_ps_call_dormant =
        (byte) call_state_list_ptr[loop].is_ps_call_dormant;
    }
  }/* for () */

  #ifdef CM_GW_SUPPORTED
  cmlog_cm_send_call_information_log();
  #endif

  return rsp_ptr;

} /* cmdiag_cm_get_call_information_list_per_subs () */
/*lint +esym(715,req_pkt)*/
/*lint +esym(715,pkt_len)*/
/*lint +esym(818, req_pkt) diagpkt_user_table_entry_type prohibits that */

#ifdef CM_GW_SUPPORTED

/*=================================================================================
Function: cmdiag_cm_set_password_per_subs
Description:
       Called for setting user given password in response to a get password request.
==================================================================================*/
/*lint -esym(765, cmdiag_cm_set_password_per_subs)
** Can't be static, to be called by DIAG via COMMAND DISPATCH TABLE
*/
PACKED void *cmdiag_cm_set_password_per_subs (

  PACKED void   *req_pkt,

  uint16        pkt_len

)
{
  DIAG_SUBSYS_CM_CM_SET_PASSWORD_PER_SUBS_req_type  *req_ptr =
        (DIAG_SUBSYS_CM_CM_SET_PASSWORD_PER_SUBS_req_type *) req_pkt;
  DIAG_SUBSYS_CM_CM_SET_PASSWORD_PER_SUBS_rsp_type  *rsp_ptr;
  cm_get_pwd_res_params_s_type                      get_pwd_res;

  int i;

  cmlog_client_s_type* diag_client_ptr = cmlog_get_diag_client();

  /*lint -save -e641*/
  /* Allocate the memory for this */
  rsp_ptr = (DIAG_SUBSYS_CM_CM_SET_PASSWORD_PER_SUBS_rsp_type *) diagpkt_subsys_alloc(DIAG_SUBSYS_CM, CM_SET_PASSWORD_PER_SUBS,
    sizeof(DIAG_SUBSYS_CM_CM_SET_PASSWORD_PER_SUBS_rsp_type));
  /*lint -restore */

  /* Ensure response packet allocation */
  if (!rsp_ptr)
  {
    return NULL;
  }

  /* Check for packet length */
  if (cmdiag_check_request_length (pkt_len, req_pkt, CM_SET_PASSWORD_PER_SUBS) == FALSE)
  {
    CM_ERR_0("Packet length check failed");
    return rsp_ptr;
  }

  if (!req_ptr)
  {
    rsp_ptr->status = FALSE;
    return rsp_ptr;
  }
  else
  {
    memset( &get_pwd_res, CM_CALL_CMD_PARAM_DEFAULT_VALUE,
            sizeof( cm_get_pwd_res_params_s_type ));

    /* Invoke ID */
    get_pwd_res.invoke_id = req_ptr->invoke_id;

    /* Check if password digits are between '0' and '9' */
    for (i=0; i<MAX_DIAG_PWD_CHAR; i++)
    {
      if ((req_ptr->ss_password[i] < '0') || (req_ptr->ss_password[i] > '9'))
      {
        CM_ERR_1("Bad Password: %c", req_ptr->ss_password[i]);

        rsp_ptr->status = FALSE;
        return rsp_ptr;
      }
    }

    /*
    ** 506: constant value boolean
    ** 774: boolean always evaluates to TRUE
    */
    /*lint -save -e506 -e774*/

    /*
    ** MAX_DIAG_PWD_CHAR is the size allowed by diag for password
    ** field. Added a check to catch if CM decides to increase size
    ** of ss_password
    */
    if (MAX_DIAG_PWD_CHAR != sizeof (get_pwd_res.ss_pwd.ss_password))
    {
      CM_ERR_0("Password length mismatch between DIAG and CM");

      return rsp_ptr;
    }
    /*lint -restore */

    /*Copy password string*/
    memscpy( get_pwd_res.ss_pwd.ss_password, sizeof(get_pwd_res.ss_pwd.ss_password),
             (const byte *)req_ptr->ss_password, MAX_DIAG_PWD_CHAR );

    get_pwd_res.ss_pwd.present = TRUE;

    /* Copy invoke_id into response */
    rsp_ptr->invoke_id = req_ptr->invoke_id;

    /* send the command to CM command queue */
    rsp_ptr->status = cm_sups_cmd_get_password_res_per_subs(
                                cmdiag_cm_sups_cmd_cb,
                                NULL,
                                diag_client_ptr->client,
                                &get_pwd_res,
                                (sys_modem_as_id_e_type)req_ptr->asubs_id
                                );
    return rsp_ptr;
  }

} /* cmdiag_cm_set_password_per_subs () */

/*===========================================================================
Function: cmdiag_cm_send_ussd_reply_per_subs
Description:
       called to send user USSD response or user notify response to network
============================================================================*/
/*lint -esym(765, cmdiag_cm_send_ussd_reply_per_subs)
** Can't be static, to be called by DIAG via COMMAND DISPATCH TABLE
*/
PACKED void *cmdiag_cm_send_ussd_reply_per_subs (

  PACKED void   *req_pkt,

  uint16        pkt_len

)
{
  DIAG_SUBSYS_CM_CM_SEND_USSD_REPLY_PER_SUBS_req_type  *req_ptr =
        (DIAG_SUBSYS_CM_CM_SEND_USSD_REPLY_PER_SUBS_req_type *) req_pkt;
  DIAG_SUBSYS_CM_CM_SEND_USSD_REPLY_PER_SUBS_rsp_type  *rsp_ptr;

  cm_uss_notify_res_params_s_type                       uss_notify_res;
    /* Response for USSD based notify request */
  cm_uss_res_params_s_type                              uss_res;
    /* Response for user input towards a ussd request*/
  byte num_bytes = 0;

  cmlog_client_s_type* diag_client_ptr = cmlog_get_diag_client();

  memset( &uss_res,
          CM_CALL_CMD_PARAM_DEFAULT_VALUE,
          sizeof( cm_uss_res_params_s_type ));

  memset( &uss_notify_res,
          CM_CALL_CMD_PARAM_DEFAULT_VALUE,
          sizeof( cm_uss_notify_res_params_s_type));

  /*lint -save -e641*/
  /* Allocate the memory for this */
  rsp_ptr = (DIAG_SUBSYS_CM_CM_SEND_USSD_REPLY_PER_SUBS_rsp_type *) diagpkt_subsys_alloc(DIAG_SUBSYS_CM, CM_SEND_USSD_REPLY_PER_SUBS,
    sizeof(DIAG_SUBSYS_CM_CM_SEND_USSD_REPLY_PER_SUBS_rsp_type));
  /*lint -restore */

  /* Ensure response packet allocation */
  if (!rsp_ptr)
  {
    return NULL;
  }

  /* Check for packet length */
  if (cmdiag_check_request_length (pkt_len, req_pkt, CM_SEND_USSD_REPLY_PER_SUBS) == FALSE)
  {
    CM_ERR_0("Packet length check failed");
    return rsp_ptr;
  }

  if (!req_ptr)
  {
    rsp_ptr->status = FALSE;
    return rsp_ptr;
  }
  else
  {
    /* Response For USSD notify indication */
    if (req_ptr->ussd_type == USS_NOTIFY_RESPONSE)
    {
      uss_notify_res.ss_error.present = FALSE;
      uss_notify_res.invoke_id = req_ptr->invoke_id;

      rsp_ptr->invoke_id = req_ptr->invoke_id;
      rsp_ptr->ussd_type = req_ptr->ussd_type;

      CM_MSG_HIGH_0("Sending USSD notify response");
      rsp_ptr->status = cm_sups_cmd_uss_notify_res_per_subs(
                              cmdiag_cm_sups_cmd_cb,
                              NULL,
                              diag_client_ptr->client,
                              &uss_notify_res,
                              (sys_modem_as_id_e_type)req_ptr->asubs_id);
    }
    /* Response for USSD indication */
    else if (req_ptr->ussd_type == USS_RESPONSE)
    {
      /* uss data */
      uss_res.uss_res_data.data_type = (cm_ss_conf_e_type) CM_USS_RES_SS_DATA;

      uss_res.uss_res_data.data.uss_data.present = TRUE;
      uss_res.uss_res_data.data.uss_data.uss_data_coding_scheme =
                                              USS_DEF_ALPHABET_LANG_UNSPEC;
      rsp_ptr->invoke_id = req_ptr->invoke_id;
      rsp_ptr->ussd_type = req_ptr->ussd_type;

      /* Check for USSD reply to not exceed MAX_DIAG_USS_CHAR */
      if (req_ptr->reply_data_size > MAX_DIAG_USS_CHAR)
      {
        CM_ERR_1("MAX number of USS charcters is %d", MAX_DIAG_USS_CHAR);

        req_ptr->reply_data_size = MAX_DIAG_USS_CHAR;
      }

      if (req_ptr->reply_data_size != 0)
      {
        /* convert the response from ascii to GSM def alphabet */
        num_bytes = cm_util_ascii_to_gsm_alphabet(
                                uss_res.uss_res_data.data.uss_data.ussData,
                                (byte *) req_ptr->reply_data,
                                req_ptr->reply_data_size );
      }
      else
      {
        CM_ERR_0("reply_data_size is 0");
      }

      /* size */
      uss_res.uss_res_data.data.uss_data.size = num_bytes;

      /* invocation id */
      uss_res.invoke_id = req_ptr->invoke_id;

      CM_MSG_HIGH_0("Sending USSD request response");

      /* send the command to the CM command queue */
      rsp_ptr->status = cm_sups_cmd_uss_res_per_subs(
                           cmdiag_cm_sups_cmd_cb,
                           NULL,
                           diag_client_ptr->client,
                           &uss_res,
                           (sys_modem_as_id_e_type)req_ptr->asubs_id
                           );

    } /* if (req_ptr->ussd_type == 0) */
    else
    {
      rsp_ptr->status = FALSE;
    }

  } /* if (!req_ptr) */

  return rsp_ptr;

} /* cmdiag_cm_send_ussd_reply_per_subs () */

/*===========================================================================
Function: cmdiag_cm_get_ccm_info_per_subs
Description:

       This function processes the umts specific Current Call Meter (CCM)
       information request packet from QXDM and sends the response back.

============================================================================*/
/*lint -esym(765, cmdiag_cm_get_ccm_info_per_subs)
** Can't be static, to be called by DIAG via COMMAND DISPATCH TABLE
*/
PACKED void *cmdiag_cm_get_ccm_info_per_subs
(
  PACKED void* req_pkt,

  uint16 pkt_len
)
/*lint -esym(715,req_pkt,pkt_len)*/
/*lint -esym(818, req_pkt)*/
{
  DIAG_SUBSYS_CM_CM_GET_CCM_INFO_PER_SUBS_rsp_type *rsp_ptr ;
  DIAG_SUBSYS_CM_CM_GET_CCM_INFO_PER_SUBS_req_type *req_ptr = NULL;

  const int rsp_len = sizeof(DIAG_SUBSYS_CM_CM_GET_CCM_INFO_PER_SUBS_rsp_type);

  /* Stops Lint complain DIAG_SUBSYS_CM_CM_GET_CCM_INFO_req_type */
  if (((DIAG_SUBSYS_CM_CM_GET_CCM_INFO_PER_SUBS_req_type *) req_pkt) == NULL)
  {
    CM_MSG_LOW_0 ("cmdiag_cm_get_ccm_info NULL");
  }
  else
  {
    req_ptr = (DIAG_SUBSYS_CM_CM_GET_CCM_INFO_PER_SUBS_req_type *) req_pkt;
  }

  /*lint -save -e641*/
  /* Allocate the memory for this */
  rsp_ptr = (DIAG_SUBSYS_CM_CM_GET_CCM_INFO_PER_SUBS_rsp_type *)diagpkt_subsys_alloc(
                                 DIAG_SUBSYS_CM,
                                 CM_GET_CCM_INFO_PER_SUBS,
                                 rsp_len );

  if (rsp_ptr != NULL && req_ptr != NULL)
  {
    cm_ss_e_type  ss;

    ss = cmph_map_subs_to_ss((sys_modem_as_id_e_type)req_ptr->asubs_id);

    /* Fill the response packet */
    if(ss == CM_SS_NONE)
    {
      rsp_ptr->ccm = 0;
    }
    else
    {
      /* Query Call Manager for CCM information */
      rsp_ptr->ccm   = cmwaoc_get_ccm(ss);
    }
  }
  /*lint -restore */

  return rsp_ptr ;
}/* cmdiag_cm_get_ccm_info_per_subs() */
/*lint +esym(715,req_pkt,pkt_len)*/
/*lint +esym(818, req_pkt)*/

#if (defined(FEATURE_CM_MMGPS) || defined(FEATURE_CM_LCS))
/*===========================================================================
Function: cmdiag_cm_send_sups_lcs_molr_per_subs
Description:

       Function processes location service request for mobile originated location
       registration.

============================================================================*/

/*lint -esym(765, cmdiag_cm_send_sups_lcs_molr_per_subs)
** Can't be static, to be called by DIAG via COMMAND DISPATCH TABLE
*/
PACKED void *cmdiag_cm_send_sups_lcs_molr_per_subs (

  PACKED void   *req_pkt,

  uint16        pkt_len
)
/*lint -esym(715,pkt_len)*/
{

  DIAG_SUBSYS_CM_CM_SEND_SUPS_LCS_MOLR_PER_SUBS_req_type *req_ptr =
    (DIAG_SUBSYS_CM_CM_SEND_SUPS_LCS_MOLR_PER_SUBS_req_type *)req_pkt;

  DIAG_SUBSYS_CM_CM_SEND_SUPS_LCS_MOLR_PER_SUBS_rsp_type *rsp_ptr;

  cm_lcs_molr_params_s_type                      sups_cmd_lcs_molr;

  const int rsp_len =
          sizeof(DIAG_SUBSYS_CM_CM_SEND_SUPS_LCS_MOLR_PER_SUBS_rsp_type);

  cmlog_client_s_type* diag_client_ptr = cmlog_get_diag_client();

  /*lint -save -e641*/
  /* Allocate the memory for this */
  rsp_ptr = (DIAG_SUBSYS_CM_CM_SEND_SUPS_LCS_MOLR_PER_SUBS_rsp_type *)
            diagpkt_subsys_alloc(DIAG_SUBSYS_CM, CM_SEND_SUPS_LCS_MOLR_PER_SUBS,
             rsp_len );
  /*lint -restore */

  /* Ensure response packet allocation */
  if (!rsp_ptr)
  {
    CM_MSG_HIGH_0("Memory alloc failure for cm_send_sups_lcs_molr");
    return NULL;
  }

  /* Sanity check */
  if (!req_ptr)
  {
    rsp_ptr->status = FALSE;
  }
  else
  {
    /* Copy ss_ref to rsp ptr */
    rsp_ptr->ss_ref                  =  req_ptr->ss_ref;

    /* clear lcs molr request */
    memset (&sups_cmd_lcs_molr, 0, sizeof(cm_lcs_molr_params_s_type));

    /* Copy fields into sups cmd */
    sups_cmd_lcs_molr.ss_ref         =   req_ptr->ss_ref;
    sups_cmd_lcs_molr.molr_type      =   (cm_molr_type_e_type) req_ptr->molr_type;

    /* Location method */
    sups_cmd_lcs_molr.location_method.present                       =
                                       req_ptr->location_method_present;

    sups_cmd_lcs_molr.location_method.data                          =
                        (location_method_e_type) req_ptr->location_method_data;

    /* Location qos */
    sups_cmd_lcs_molr.lcs_qos.present  =   req_ptr->lcs_qos_present;

    sups_cmd_lcs_molr.lcs_qos.horiz_accuracy.present                =
                                       req_ptr->loc_qos_hor_accur_present;

    sups_cmd_lcs_molr.lcs_qos.horiz_accuracy.data                   =
                                       req_ptr->lcs_qos_hor_accur_data;

    sups_cmd_lcs_molr.lcs_qos.vertical_coord_req.present            =
                                       req_ptr->lcs_qos_ver_cord_req_present;

    sups_cmd_lcs_molr.lcs_qos.vertical_accuracy.present             =
                                       req_ptr->lcs_qos_ver_accur_present;

    sups_cmd_lcs_molr.lcs_qos.vertical_accuracy.data                =
                                       req_ptr->lcs_qos_ver_accur_data;

    sups_cmd_lcs_molr.lcs_qos.response_time.present                 =
                                       req_ptr->lcs_qos_rsp_time_present;

    sups_cmd_lcs_molr.lcs_qos.response_time.response_time_category  =
         (response_time_category_e_type) req_ptr->lcs_qos_rsp_time_categ;

    /* External id */
    if (req_ptr->lcs_client_external_add_size > CM_LCS_CLIENT_EXT_ADD_DATA_MAX)
    {
      CM_ERR_1("external address size exceeds %d bytes",
              CM_LCS_CLIENT_EXT_ADD_DATA_MAX);
      rsp_ptr->status = FALSE;

      return rsp_ptr;
    }

    sups_cmd_lcs_molr.external_id.present                           =
                                       req_ptr->lcs_client_external_id_present;

    sups_cmd_lcs_molr.external_id.external_address.present          =
                                       req_ptr->lcs_client_external_add_present;

    /* Copy external add data from req_ptr to sups cmd */
    sups_cmd_lcs_molr.external_id.external_address.size = memscpy( sups_cmd_lcs_molr.external_id.external_address.data,
                                                                   sizeof(sups_cmd_lcs_molr.external_id.external_address.data),
                                                                   (const byte *)req_ptr->lcs_client_external_add_data,
                                                                   req_ptr->lcs_client_external_add_size );

    /* MLC number */
    if (req_ptr->mlc_number_size > CM_MLC_NUM_DATA_MAX)
    {
      CM_ERR_1("mlc number data exceeds %d bytes",CM_MLC_NUM_DATA_MAX);
      rsp_ptr->status = FALSE;

      return rsp_ptr;
    }

    sups_cmd_lcs_molr.mlc_number.present      =   req_ptr->mlc_number_present;

    /* Copy MLC number data */
    sups_cmd_lcs_molr.mlc_number.size = memscpy( sups_cmd_lcs_molr.mlc_number.data,
                                                 sizeof(sups_cmd_lcs_molr.mlc_number.data),
                                                 (const byte *) req_ptr->mlc_number_data,
                                                 req_ptr->mlc_number_size );

    /* GPS assistance data */
    if (req_ptr->gps_assis_data_size > CM_GPS_ASSIS_DATA_MAX)
    {
      CM_ERR_1("GPS assistance data exceeds %d bytes",CM_GPS_ASSIS_DATA_MAX);
      rsp_ptr->status = FALSE;

      return rsp_ptr;
    }

    /* GPS Assistance */
    sups_cmd_lcs_molr.gps_assistance_data.present =
                                       req_ptr->gps_assis_data_present;

    /* Copy GPS assistance data */
    sups_cmd_lcs_molr.gps_assistance_data.size = memscpy( sups_cmd_lcs_molr.gps_assistance_data.data,
                                                          sizeof(sups_cmd_lcs_molr.gps_assistance_data.data),
                                                          (const byte *) req_ptr->gps_assis_data,
                                                          req_ptr->gps_assis_data_size );

    /* Supported GAD shapes */
    sups_cmd_lcs_molr.supported_gad_shapes.present      =
                                       req_ptr->supp_gad_shapes_present;
    sups_cmd_lcs_molr.supported_gad_shapes.data         =
                                       req_ptr->supp_gad_shapes_data;

    rsp_ptr->status =
                        cm_sups_cmd_lcs_molr_per_subs( NULL,
                                              NULL,
                                              diag_client_ptr->client,
                                              &sups_cmd_lcs_molr,
                                              (sys_modem_as_id_e_type)req_ptr->asubs_id);

  } /* req_ptr */

  return rsp_ptr;

} /* cmdiag_cm_send_sups_lcs_molr_per_subs () */
/*lint +esym(715,pkt_len)*/

/*===========================================================================
Function: cmdiag_cm_send_sups_lcs_molr_completed_per_subs
Description:

       Function processes location service request to indicate completion of
       Mobile originated location registration.

============================================================================*/
/*lint -esym(765, cmdiag_cm_send_sups_lcs_molr_completed_per_subs)
** Can't be static, to be called by DIAG via COMMAND DISPATCH TABLE
*/
/*lint -esym(714, cmdiag_cm_send_sups_lcs_molr_completed) not referenced */
PACKED void *cmdiag_cm_send_sups_lcs_molr_completed_per_subs (

  PACKED void   *req_pkt,
    /* diag request to CM */

  uint16        pkt_len
    /* length of diag request in bytes */

)
/*lint -esym(715, pkt_len)*/
{
  DIAG_SUBSYS_CM_CM_SEND_SUPS_LCS_MOLR_COMPLETED_PER_SUBS_req_type *req_ptr =
    (DIAG_SUBSYS_CM_CM_SEND_SUPS_LCS_MOLR_COMPLETED_PER_SUBS_req_type *)req_pkt;

  DIAG_SUBSYS_CM_CM_SEND_SUPS_LCS_MOLR_COMPLETED_PER_SUBS_rsp_type *rsp_ptr = NULL;

  cm_lcs_molr_completed_params_s_type           sups_cmd_lcs_molr_completed;

  const int rsp_len =
          sizeof(DIAG_SUBSYS_CM_CM_SEND_SUPS_LCS_MOLR_COMPLETED_PER_SUBS_rsp_type);
  cmlog_client_s_type* diag_client_ptr = cmlog_get_diag_client();

  /*lint -save -e641*/
  /* Allocate the memory for this */
  rsp_ptr = (DIAG_SUBSYS_CM_CM_SEND_SUPS_LCS_MOLR_COMPLETED_PER_SUBS_rsp_type *)
            diagpkt_subsys_alloc(DIAG_SUBSYS_CM, CM_SEND_SUPS_LCS_MOLR_COMPLETED_PER_SUBS,
             rsp_len );
  /*lint -restore */

  /* Ensure response packet allocation */
  if (!rsp_ptr)
  {
    CM_MSG_HIGH_0("Memory alloc failure for cm_send_sups_lcs_molr_completed");
    return NULL;
  }

  /* clear lcs molr completed */
  memset (&sups_cmd_lcs_molr_completed, 0,
           sizeof(cm_lcs_molr_completed_params_s_type));

  if (req_ptr != NULL)
  {
    /* Copy ss_ref to rsp ptr */
    rsp_ptr->ss_ref                  =  req_ptr->ss_ref;

    sups_cmd_lcs_molr_completed.ss_ref  = req_ptr->ss_ref;

    rsp_ptr->status = cm_sups_cmd_lcs_molr_completed_per_subs(
                           NULL,
                           NULL,
                           diag_client_ptr->client,
                           &sups_cmd_lcs_molr_completed,
                           (sys_modem_as_id_e_type)req_ptr->asubs_id);
  }
  else
  {
    rsp_ptr->status = FALSE;
  }

  return rsp_ptr;
} /* cmdiag_cm_send_sups_lcs_molr_completed_per_subs () */
/*lint +esym(715, pkt_len)*/
#endif /* #ifdef FEATURE_CM_MMGPS || defined(FEATURE_CM_LCS) */


/*===========================================================================
Function: cmdiag_wcdma_call_origination_per_subs
Description:

       This function processes the wcdma specific call orig request packet
       from QXDM and sends the response back.

============================================================================*/

/*lint -esym(765, cmdiag_wcdma_call_origination_per_subs)
** Can't be static, to be called by DIAG via COMMAND DISPATCH TABLE
*/
PACKED void *cmdiag_wcdma_call_origination_per_subs (PACKED void* req_pkt, uint16 pkt_len)
{
  DIAG_SUBSYS_CM_CM_WCDMA_CALL_ORIGINATION_PER_SUBS_req_type *req_ptr =
    (DIAG_SUBSYS_CM_CM_WCDMA_CALL_ORIGINATION_PER_SUBS_req_type *)req_pkt;
  DIAG_SUBSYS_CM_CM_WCDMA_CALL_ORIGINATION_PER_SUBS_rsp_type *rsp_ptr;

  cm_num_s_type                numbers_buf;
  uint8                        work_buf[CM_MAX_NUMBER_CHARS];
  uint8                        i;
  cm_gw_cs_orig_params_s_type  call_orig_params;
  cm_num_s_type                calling_num;
  cm_orig_alpha_s_type         alpha_buf;

  const int rsp_len = sizeof(DIAG_SUBSYS_CM_CM_WCDMA_CALL_ORIGINATION_PER_SUBS_rsp_type);

  /*lint -save -e641*/
  /* Allocate the memory for this */
  rsp_ptr = (DIAG_SUBSYS_CM_CM_WCDMA_CALL_ORIGINATION_PER_SUBS_rsp_type *)diagpkt_subsys_alloc(DIAG_SUBSYS_CM, CM_WCDMA_CALL_ORIGINATION_PER_SUBS,
                                 rsp_len );
  /*lint -restore */

  /* Check for packet length */
  if (cmdiag_check_request_length (pkt_len, req_pkt, CM_WCDMA_CALL_ORIGINATION_PER_SUBS) == FALSE)
  {
    CM_ERR_0("Packet length check failed");
    return rsp_ptr;
  }

  /* Ensure response packet allocation */
  if (!rsp_ptr)
  {
    CM_MSG_HIGH_0("wcdmadiag call orig memory alloc failed for response packet");
    return NULL;
  }

  /* Fill the memory returned, with 0s */
  rsp_ptr->status = 0;
  rsp_ptr->call_id = 0;

  /* Initialize the buffer to CM
  */
  memset(&call_orig_params,
         CM_CALL_CMD_PARAM_DEFAULT_VALUE,
         sizeof(call_orig_params));

  /* amr_rate is a value between 0 to 7 */
  if (req_ptr->amr_rate > 7)
  {
    return NULL;
  }

  if ( (req_ptr->num_digits > DIAG_MAX_WCDMA_DIGITS) ||
       (req_ptr->num_digits > CM_MAX_NUMBER_CHARS) )
  {
    CM_ERR_0("Dialed too many digits");
    return rsp_ptr;
  }

  /* Initialize work_buf
  */
  memset (work_buf, 0, ARR_SIZE(work_buf));

  for (i=0; i<req_ptr->num_digits; i++)
  {
    /* Check for dial digits (0 - 9, *, #) */
    if ( ((req_ptr->digits[i] < '0') || (req_ptr->digits[i] > '9')) &&
         (req_ptr->digits[i] != '*')  &&
         (req_ptr->digits[i] != '#'))
    {
        /* Was an invalid digit (not 0 - 9, *, #) */
        CM_ERR_1("Bad dial digit: %c", req_ptr->digits[i]);
        return rsp_ptr;
    }
    work_buf[i] = req_ptr->digits[i];
  }

  alpha_buf.len = memscpy( alpha_buf.buf, sizeof(alpha_buf.buf), (const byte *)"DIAG CALL", (uint8) strlen("DIAG CALL") );
  cm_num_init( &calling_num );

  cm_num_fill(&numbers_buf, work_buf, req_ptr->num_digits, CM_DIGIT_MODE_4BIT_DTMF);


  /* Fill in the CUG. */
  if (use_cug)
  {
    call_orig_params.forward_cug_info.present = TRUE;
    call_orig_params.forward_cug_info.cm_cug_index.present = TRUE;
    call_orig_params.forward_cug_info.cm_cug_index.msb =
      (cug_group_id & 0xff00) >> 8;
    call_orig_params.forward_cug_info.cm_cug_index.lsb =
      (cug_group_id & 0x00ff);
    call_orig_params.forward_cug_info.cm_suppress_pref_cug =
      cug_suppress_pref_cug;
    call_orig_params.forward_cug_info.cm_suppress_outgoing_access =
      cug_suppress_oa;
  }

 rsp_ptr->status = cm_mm_call_cmd_orig_cc_per_subs(
    NULL, /* client callback function */
    NULL, /* pointer to client callback data block */
    CM_CLIENT_ID_ANONYMOUS,
    CM_CALL_TYPE_VOICE,
    CM_SRV_TYPE_AUTOMATIC,
    &calling_num,
    &numbers_buf,
    &alpha_buf,        /* Alpha buffer for call  */
    NULL,              /* CDMA call orig params  */
    &call_orig_params, /* GW CS call orig params */
    NULL,              /* GW PS call orig params */
    CM_CALL_ID_INVALID,
    CM_ORIG_CLIENT_CATEG_USER,
    (sys_modem_as_id_e_type)req_ptr->asubs_id,
    &wcdma_call_id
   );

  /* call_id is valid only when status is TRUE*/
  if (rsp_ptr->status == TRUE)
  {
    rsp_ptr->call_id = wcdma_call_id;
    CM_MSG_HIGH_1("wcdma call id = %d", wcdma_call_id);
  }/* if (rsp_ptr->status == TRUE) */

  return rsp_ptr ;

} /* cmdiag_wcdma_call_origination_per_subs_per_subs() */

/*===========================================================================
Function: cmdiag_gsm_call_origination_per_subs
Description:

       This function processes the gsm specific call orig request packet
       from QXDM and sends the response back.

============================================================================*/

/*lint -esym(765, cmdiag_gsm_call_origination_per_subs)
** Can't be static, to be called by DIAG via COMMAND DISPATCH TABLE
*/
PACKED void *cmdiag_gsm_call_origination_per_subs (

  PACKED void   *req_pkt,

  uint16        pkt_len
)
{

  DIAG_SUBSYS_CM_CM_GSM_CALL_ORIGINATION_PER_SUBS_req_type *req_ptr =
    (DIAG_SUBSYS_CM_CM_GSM_CALL_ORIGINATION_PER_SUBS_req_type *)req_pkt;
  DIAG_SUBSYS_CM_CM_GSM_CALL_ORIGINATION_PER_SUBS_rsp_type *rsp_ptr;

  cm_num_s_type                 numbers_buf;
  uint8                         work_buf[CM_MAX_NUMBER_CHARS];
  uint8                         i;
  cm_gw_cs_orig_params_s_type   call_orig_params;
  cm_num_s_type                 calling_num;
  cm_orig_alpha_s_type          alpha_buf;

  const int rsp_len = sizeof(DIAG_SUBSYS_CM_CM_GSM_CALL_ORIGINATION_PER_SUBS_rsp_type);

  /*lint -save -e641*/
  /* Allocate the memory for this */
  rsp_ptr = (DIAG_SUBSYS_CM_CM_GSM_CALL_ORIGINATION_PER_SUBS_rsp_type *)diagpkt_subsys_alloc (DIAG_SUBSYS_CM, CM_GSM_CALL_ORIGINATION_PER_SUBS, rsp_len);
  /*lint -restore */

  /* Ensure response packet allocation */
  if (!rsp_ptr)
  {
    CM_MSG_HIGH_0("gsmdiag call orig memory alloc failed for response packet");
    return NULL;
  }

  /* Check for packet length */
  if (cmdiag_check_request_length (pkt_len, req_pkt, CM_GSM_CALL_ORIGINATION_PER_SUBS) == FALSE)
  {
    CM_ERR_0("Packet length check failed");
    return rsp_ptr;
  }

  /* Fill the memory returned, with 0s */
  rsp_ptr->status = 0;
  rsp_ptr->call_id = 0;

  /* Initialize the buffer to CM */
  memset(&call_orig_params,
        CM_CALL_CMD_PARAM_DEFAULT_VALUE,
        sizeof(call_orig_params));

  /* Sanity check */
  if (!req_ptr)
  {
    rsp_ptr->status = FALSE;
  }
  else
  {
    if ( (req_ptr->num_digits > CM_DIAG_MAX_GSM_DIGITS)
       || (req_ptr->num_digits > CM_MAX_NUMBER_CHARS) )
    {
      CM_ERR_0("Dialed too many digits");
      return rsp_ptr;
    }

    /* Initialize work_buf
    */
    memset (work_buf, 0, ARR_SIZE(work_buf));

    for (i=0; i<req_ptr->num_digits; i++)
    {
      /* Check for dial digits (0 - 9, *, #) */
      if ( ((req_ptr->digits[i] < '0') || (req_ptr->digits[i] > '9')) &&
           (req_ptr->digits[i] != '*')  &&
           (req_ptr->digits[i] != '#'))
      {
        /* Was an invalid digit (not 0 - 9, *, #) */
        CM_ERR_1("Bad dial digit: %c", req_ptr->digits[i]);
        return rsp_ptr;
      }
      work_buf[i] = req_ptr->digits[i];
    }

    alpha_buf.len = memscpy( alpha_buf.buf, sizeof(alpha_buf.buf), (const byte *)"DIAG CALL", (uint8) strlen("DIAG CALL") );
    cm_num_init( &calling_num );
    cm_num_fill( &numbers_buf, work_buf, req_ptr->num_digits, CM_DIGIT_MODE_4BIT_DTMF);

    /* Fill the memory returned, with 0s */
    rsp_ptr->status = 0;
    rsp_ptr->call_id = 0;

    /* Fill in the CUG. */
    if (use_cug)
    {
      call_orig_params.forward_cug_info.present = TRUE;
      call_orig_params.forward_cug_info.cm_cug_index.present = TRUE;
      call_orig_params.forward_cug_info.cm_cug_index.msb =
        (cug_group_id & 0xff00) >> 8;
      call_orig_params.forward_cug_info.cm_cug_index.lsb =
        (cug_group_id & 0x00ff);
      call_orig_params.forward_cug_info.cm_suppress_pref_cug =
        cug_suppress_pref_cug;
      call_orig_params.forward_cug_info.cm_suppress_outgoing_access =
        cug_suppress_oa;
    }
    rsp_ptr->status = cm_mm_call_cmd_orig_cc_per_subs(
        NULL, /* client callback function */
        NULL, /* pointer to client callback data block */
        CM_CLIENT_ID_ANONYMOUS,
        CM_CALL_TYPE_VOICE,
        CM_SRV_TYPE_AUTOMATIC,
        &calling_num,
        &numbers_buf,
        &alpha_buf,        /* Alpha buffer for call  */
        NULL,              /* CDMA call orig params  */
        &call_orig_params, /* GW CS call orig params */
        NULL,              /* GW PS call orig params */
        CM_CALL_ID_INVALID,
        CM_ORIG_CLIENT_CATEG_USER,
        (sys_modem_as_id_e_type)req_ptr->asubs_id,
        &gsm_call_id
        );

      /* call_id is valid only when status is TRUE*/
      if (rsp_ptr->status == TRUE)
      {
        rsp_ptr->call_id = gsm_call_id;
      }/* if (rsp_ptr->status == TRUE) */

  } /* if (!req_ptr) */

  return rsp_ptr ;

} /* cmdiag_gsm_call_origination_per_subs() */

#endif /* FEATURE_GSM || FEATURE_WCDMA */

/*===========================================================================
Function: cmdiag_cm_system_selection_per_subs
Description:

       This function processes the call manager system selection request2
       packet from QXDM and sends the response back.

=============================================================================*/
/*lint -esym(765, cmdiag_cm_set_system_selection_per_subs)
** Can't be static, to be called by DIAG via COMMAND DISPATCH TABLE
*/
PACKED void *cmdiag_cm_set_system_selection_per_subs (

  PACKED void   *req_pkt,

  uint16        pkt_len

)
{
  DIAG_SUBSYS_CM_CM_SYSTEM_SELECTION_PREFERENCE_PER_SUBS_req_type *req_ptr
            = (DIAG_SUBSYS_CM_CM_SYSTEM_SELECTION_PREFERENCE_PER_SUBS_req_type*)req_pkt;
  DIAG_SUBSYS_CM_CM_SYSTEM_SELECTION_PREFERENCE_PER_SUBS_rsp_type *rsp_ptr;

  const int rsp_len = sizeof(DIAG_SUBSYS_CM_CM_SYSTEM_SELECTION_PREFERENCE_PER_SUBS_rsp_type);

  /*lint -save -e641*/
  cm_network_sel_mode_pref_e_type  nw_sel_mode_pref =
                                          CM_NETWORK_SEL_MODE_PREF_NO_CHANGE;

  sys_plmn_id_s_type               plmn_struct;

  cm_mode_pref_e_type               mode_pref = CM_MODE_PREF_NO_CHANGE;
  cm_gw_acq_order_pref_e_type       acq_order_pref =
                                              CM_GW_ACQ_ORDER_PREF_NO_CHANGE;
  cm_band_pref_e_type               band_pref = CM_BAND_PREF_NO_CHANGE;
  cm_roam_pref_e_type               roam_pref = CM_ROAM_PREF_NO_CHANGE;

  cm_hybr_pref_e_type               hybr_pref = CM_HYBR_PREF_NO_CHANGE;
  cm_srv_domain_pref_e_type         srv_domain_pref =
                                                CM_SRV_DOMAIN_PREF_NO_CHANGE;
  cmlog_client_s_type* diag_client_ptr = cmlog_get_diag_client();

  /* Allocate the memory for this */
  rsp_ptr = (DIAG_SUBSYS_CM_CM_SYSTEM_SELECTION_PREFERENCE_PER_SUBS_rsp_type *)
  diagpkt_subsys_alloc(DIAG_SUBSYS_CM,
                         CM_SYSTEM_SELECTION_PREFERENCE_PER_SUBS,
                         rsp_len );

  /* Ensure response packet allocation */
  if (!rsp_ptr)
  {
    CM_ERR_0(" System reference2 mem alloc fail for response pkt ");
    return NULL;
  }

  /* Sanity check request packet */
  if (!cmdiag_check_request_length (
                                  pkt_len,
                                  req_pkt,
                                  CM_SYSTEM_SELECTION_PREFERENCE_PER_SUBS))
  {
    rsp_ptr->status = FALSE;

    CM_ERR_0("System reference2 Packet length check failed");
    /* Return status now.*/
    return rsp_ptr;
  }

  /* Get params from Request Packet */
  /* Convert from little endian notation */
  plmn_struct.identity[0] = req_ptr->PLMN[2];
  plmn_struct.identity[1] = req_ptr->PLMN[1];
  plmn_struct.identity[2] = req_ptr->PLMN[0];

  /* Update the system prefernce*/
  if( req_ptr->network_sel_mode_pref < (unsigned int)CM_NETWORK_SEL_MODE_PREF_MAX )
  {
    nw_sel_mode_pref = (cm_network_sel_mode_pref_e_type) req_ptr->network_sel_mode_pref;
  }
  else
  {
    CM_ERR_1 ("Bad nw_sel_mode_pref %d, using no change",
                 req_ptr->network_sel_mode_pref);
  }

  if( req_ptr->mode_pref < (unsigned int)CM_MODE_PREF_MAX )
  {
    mode_pref = (cm_mode_pref_e_type) req_ptr->mode_pref;
  }
  else
  {
    CM_ERR_1 ("Bad mode_pref %d, using no change",
                 req_ptr->mode_pref);
  }

  if( req_ptr->acq_order_pref < (unsigned int)CM_GW_ACQ_ORDER_PREF_MAX )
  {
    acq_order_pref = (cm_gw_acq_order_pref_e_type) req_ptr->acq_order_pref;
  }
  else
  {
    CM_ERR_1 ("Bad acq_order_pref %d, using no change",
                 req_ptr->acq_order_pref);
  }

  if( req_ptr->band_pref < (unsigned int)CM_BAND_PREF_MAX )
  {
    band_pref = (cm_band_pref_e_type) req_ptr->band_pref;
  }
  else
  {
    CM_ERR_1 ("Bad band_pref %d, using no change",
                 req_ptr->band_pref);
  }

  if( req_ptr->roam_pref < (unsigned int)CM_ROAM_PREF_MAX )
  {
    roam_pref = (cm_roam_pref_e_type) req_ptr->roam_pref;
  }
  else
  {
    CM_ERR_1 ("Bad roam_pref %d, using no change",
                req_ptr->roam_pref);
  }

  if( req_ptr->hybr_pref < (unsigned int)CM_HYBR_PREF_MAX )
  {
    hybr_pref = (cm_hybr_pref_e_type) req_ptr->hybr_pref;
  }
  else
  {
    CM_ERR_1 ("Bad hybr_pref %d, using no change",
                 req_ptr->hybr_pref);
  }

  if( req_ptr->srv_domain_pref < (unsigned int)CM_SRV_DOMAIN_PREF_MAX )
  {
    srv_domain_pref = (cm_srv_domain_pref_e_type) req_ptr->srv_domain_pref;
  }
  else
  {
    CM_ERR_1 ("Bad srv_domain_pref %d, using no change",
                 req_ptr->srv_domain_pref);
  }

  /* Issue SYS_SEL_PREF & Fill the response packet */
  rsp_ptr->status = cm_ph_cmd_sys_sel_pref_per_subs
              (
                NULL,
                NULL,
                diag_client_ptr->client,
                (sys_modem_as_id_e_type)req_ptr->asubs_id,
                mode_pref,
                CM_PREF_TERM_PERMANENT,
                0,
                acq_order_pref,
                band_pref,
                CM_PRL_PREF_NO_CHANGE,
                roam_pref,
                hybr_pref,
                srv_domain_pref,
                nw_sel_mode_pref,
                &plmn_struct
             );

  CM_MSG_HIGH_1( "Status %d", rsp_ptr->status);

  /* Return status.*/
  return rsp_ptr ;
}

/*===========================================================================
Function: cmdiag_cm_set_system_selection2_per_subs
Description:

       This function processes the call manager system selection request2
       packet from QXDM and sends the response back.

       Note: It expands the band preference from 32-bit to 64-bit. The rest
       fields are same as those of call manager system selection request.

============================================================================*/
/*lint -esym(765, cmdiag_cm_set_system_selection2_per_subs)
** Can't be static, to be called by DIAG via COMMAND DISPATCH TABLE
*/
PACKED void *cmdiag_cm_set_system_selection2_per_subs (

  PACKED void   *req_pkt,

  uint16        pkt_len

)
{
  DIAG_SUBSYS_CM_CM_SYSTEM_SELECTION_PREFERENCE2_PER_SUBS_req_type *req_ptr
            = (DIAG_SUBSYS_CM_CM_SYSTEM_SELECTION_PREFERENCE2_PER_SUBS_req_type*)req_pkt;
  DIAG_SUBSYS_CM_CM_SYSTEM_SELECTION_PREFERENCE2_PER_SUBS_rsp_type *rsp_ptr;

  const int rsp_len = sizeof(DIAG_SUBSYS_CM_CM_SYSTEM_SELECTION_PREFERENCE2_PER_SUBS_rsp_type);
  cmlog_client_s_type* diag_client_ptr = cmlog_get_diag_client();

  /*lint -save -e641*/
  cm_network_sel_mode_pref_e_type  nw_sel_mode_pref =
                                          CM_NETWORK_SEL_MODE_PREF_NO_CHANGE;

  sys_plmn_id_s_type               plmn_struct;

  cm_mode_pref_e_type               mode_pref = CM_MODE_PREF_NO_CHANGE;
  cm_gw_acq_order_pref_e_type       acq_order_pref =
                                              CM_GW_ACQ_ORDER_PREF_NO_CHANGE;
  cm_band_pref_e_type               band_pref = CM_BAND_PREF_NO_CHANGE;
  cm_roam_pref_e_type               roam_pref = CM_ROAM_PREF_NO_CHANGE;

  cm_hybr_pref_e_type               hybr_pref = CM_HYBR_PREF_NO_CHANGE;
  cm_srv_domain_pref_e_type         srv_domain_pref =
                                                CM_SRV_DOMAIN_PREF_NO_CHANGE;

  /* Allocate the memory for this */
  rsp_ptr = (DIAG_SUBSYS_CM_CM_SYSTEM_SELECTION_PREFERENCE2_PER_SUBS_rsp_type *)
  diagpkt_subsys_alloc(DIAG_SUBSYS_CM,
                         CM_SYSTEM_SELECTION_PREFERENCE2_PER_SUBS,
                         rsp_len );

  /* Ensure response packet allocation */
  if (!rsp_ptr)
  {
    CM_ERR_0(" System preference2 per subs mem alloc fail for response pkt ");
    return NULL;
  }

  /* Sanity check request packet */
  if (!cmdiag_check_request_length (
                                  pkt_len,
                                  req_pkt,
                                  CM_SYSTEM_SELECTION_PREFERENCE2_PER_SUBS))
  {
    rsp_ptr->status = FALSE;

    CM_ERR_0("System Preference2 per subs Packet length check failed");
    /* Return status now.*/
    return rsp_ptr;
  }

  /* Get params from Request Packet */
  /* Convert from little endian notation */
  plmn_struct.identity[0] = req_ptr->PLMN[2];
  plmn_struct.identity[1] = req_ptr->PLMN[1];
  plmn_struct.identity[2] = req_ptr->PLMN[0];

  /* Update the system prefernce*/
  if( req_ptr->network_sel_mode_pref < (unsigned int)CM_NETWORK_SEL_MODE_PREF_MAX )
  {
    nw_sel_mode_pref = (cm_network_sel_mode_pref_e_type) req_ptr->network_sel_mode_pref;
  }
  else
  {
    CM_ERR_1 ("Bad nw_sel_mode_pref %d, using no change",
                 req_ptr->network_sel_mode_pref);
  }

  if( req_ptr->mode_pref < (unsigned int)CM_MODE_PREF_MAX )
  {
    mode_pref = (cm_mode_pref_e_type) req_ptr->mode_pref;
  }
  else
  {
    CM_ERR_1 ("Bad mode_pref %d, using no change",
                 req_ptr->mode_pref);
  }

  if( req_ptr->acq_order_pref < (unsigned int)CM_GW_ACQ_ORDER_PREF_MAX )
  {
    acq_order_pref = (cm_gw_acq_order_pref_e_type) req_ptr->acq_order_pref;
  }
  else
  {
    CM_ERR_1 ("Bad acq_order_pref %d, using no change",
                 req_ptr->acq_order_pref);
  }

  if( req_ptr->band_pref < CM_BAND_PREF_MAX )
  {
    band_pref = (cm_band_pref_e_type) req_ptr->band_pref;
  }
  else
  {
    CM_ERR_1 ("Bad band_pref %d, using no change",
                 req_ptr->band_pref);
  }

  if( req_ptr->roam_pref < (unsigned int)CM_ROAM_PREF_MAX )
  {
    roam_pref = (cm_roam_pref_e_type) req_ptr->roam_pref;
  }
  else
  {
    CM_ERR_1 ("Bad roam_pref %d, using no change",
                req_ptr->roam_pref);
  }

  if( req_ptr->hybr_pref < (unsigned int)CM_HYBR_PREF_MAX )
  {
    hybr_pref = (cm_hybr_pref_e_type) req_ptr->hybr_pref;
  }
  else
  {
    CM_ERR_1 ("Bad hybr_pref %d, using no change",
                 req_ptr->hybr_pref);
  }

  if( req_ptr->srv_domain_pref < (unsigned int)CM_SRV_DOMAIN_PREF_MAX )
  {
    srv_domain_pref = (cm_srv_domain_pref_e_type) req_ptr->srv_domain_pref;
  }
  else
  {
    CM_ERR_1 ("Bad srv_domain_pref %d, using no change",
                 req_ptr->srv_domain_pref);
  }

  /* Issue SYS_SEL_PREF & Fill the response packet */
  rsp_ptr->status = cm_ph_cmd_sys_sel_pref_per_subs
              (
                NULL,
                NULL,
                diag_client_ptr->client,
                (sys_modem_as_id_e_type)req_ptr->asubs_id,
                mode_pref,
                CM_PREF_TERM_PERMANENT,
                0,
                acq_order_pref,
                band_pref,
                CM_PRL_PREF_NO_CHANGE,
                roam_pref,
                hybr_pref,
                srv_domain_pref,
                nw_sel_mode_pref,
                &plmn_struct
             );

  CM_MSG_HIGH_1( "Status %d", rsp_ptr->status);

  /* Return status.*/
  return rsp_ptr ;
}


/*===========================================================================
Function: cmdiag_cm_get_cell_service_ind_per_subs
Description:

       This function processes the call manager cell service indication
       request from QXDM and sends the response back.
       Note : Currently supports hsdpa_hsupa and dtm

============================================================================*/
/*lint -esym(765, cmdiag_cm_get_cell_service_ind_per_subs)
** Can't be static, to be called by DIAG via COMMAND DISPATCH TABLE
*/
PACKED void *cmdiag_cm_get_cell_service_ind_per_subs(

  PACKED void   *req_pkt,

  uint16        pkt_len
)
/*lint -esym(818, req_pkt)*/
{

  DIAG_SUBSYS_CM_CM_GET_CELL_SERVICE_IND_PER_SUBS_rsp_type *rsp_ptr;
  DIAG_SUBSYS_CM_CM_GET_CELL_SERVICE_IND_PER_SUBS_req_type *req_ptr;

  const int rsp_len = sizeof(DIAG_SUBSYS_CM_CM_GET_CELL_SERVICE_IND_PER_SUBS_rsp_type);
  cmlog_client_s_type* diag_client_ptr = cmlog_get_diag_client();

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  rsp_ptr = (DIAG_SUBSYS_CM_CM_GET_CELL_SERVICE_IND_PER_SUBS_rsp_type *)
            diagpkt_subsys_alloc(DIAG_SUBSYS_CM,
                                 CM_GET_CELL_SERVICE_IND_PER_SUBS,
                                 rsp_len );

  req_ptr = (DIAG_SUBSYS_CM_CM_GET_CELL_SERVICE_IND_PER_SUBS_req_type *)req_pkt;

  /* Ensure response packet allocation */
  if (!rsp_ptr)
  {
    CM_ERR_0(" CELL Service IND mem alloc fail for response pkt ");
    return NULL;
  }

  if (!req_ptr)
  {
    CM_ERR_0(" REQ ptr is NULL ");
    return NULL;
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* Sanity check request packet */
  if (!cmdiag_check_request_length ( pkt_len, req_pkt, CM_GET_CELL_SERVICE_IND_PER_SUBS))
  {
    CM_ERR_0("CELL SERVICE IND Packet length check failed");
    /* Return status now.*/
    return rsp_ptr;
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
  if( req_ptr->asubs_id == SYS_MODEM_AS_ID_3  || (req_ptr->asubs_id == SYS_MODEM_AS_ID_2 && cmph_is_sxlte()) )
  {
    /* Filling cell service indication */
    rsp_ptr->hsdpa_hsupa_support = (uint8)diag_client_ptr->gw3_cell_srv_ind.hs_ind;
    rsp_ptr->dtm_support = (uint8)diag_client_ptr->gw3_cell_srv_ind.dtm_supp;
  }
  else 
  #endif
  if( req_ptr->asubs_id == SYS_MODEM_AS_ID_2 )
  {
    /* Filling cell service indication */
    rsp_ptr->hsdpa_hsupa_support = (uint8)diag_client_ptr->gw_cell_srv_ind.hs_ind;
    rsp_ptr->dtm_support = (uint8)diag_client_ptr->gw_cell_srv_ind.dtm_supp;
  }
  else
  {
    /* Filling cell service indication */
    rsp_ptr->hsdpa_hsupa_support = (uint8)diag_client_ptr->cell_srv_ind.hs_ind;
    rsp_ptr->dtm_support = (uint8)diag_client_ptr->cell_srv_ind.dtm_supp;
  }

  rsp_ptr->reserved1 = 0;
  rsp_ptr->reserved2 = 0;
  rsp_ptr->reserved3 = 0;

  return rsp_ptr;
}
/*lint +esym(818, req_pkt)*/

/*===========================================================================
Function: cmdiag_cm_dual_standby_pref
Description:

       This function processes the call manager plmn list request
       packet from QXDM and sends the response back.

============================================================================*/
/*lint -esym(765, cmdiag_cm_dual_standby_pref)
** Can't be static, to be called by DIAG via COMMAND DISPATCH TABLE
*/
PACKED void *cmdiag_cm_dual_standby_pref
(
  PACKED void* req_pkt, uint16 pkt_len
)
/*lint -esym(818, req_pkt) diagpkt_user_table_entry_type prohibits that */
/*lint -esym(715,req_pkt) */
/*lint -esym(715,pkt_len) */
{
  DIAG_SUBSYS_CM_CM_DUAL_STANDBY_PREF_rsp_type *rsp_ptr ;
  DIAG_SUBSYS_CM_CM_DUAL_STANDBY_PREF_req_type *req_ptr;
  cmlog_client_s_type* diag_client_ptr = cmlog_get_diag_client();

  /*lint -save -e641*/
  /* Allocate the memory for this */
  rsp_ptr = (DIAG_SUBSYS_CM_CM_DUAL_STANDBY_PREF_rsp_type *)diagpkt_subsys_alloc(DIAG_SUBSYS_CM, CM_DUAL_STANDBY_PREF,
                                 sizeof(DIAG_SUBSYS_CM_CM_DUAL_STANDBY_PREF_rsp_type) );
  req_ptr = (DIAG_SUBSYS_CM_CM_DUAL_STANDBY_PREF_req_type *)req_pkt;
  /*lint -restore */

  if (!req_ptr)
  {
    CM_ERR_0(" REQ ptr is NULL ");
    return NULL;
  }

  if (rsp_ptr != NULL)
  {   /* Fill the response packet */
    rsp_ptr->status = cm_ph_cmd_dual_standby_pref
		                    (
                            NULL,
                            NULL,
                            diag_client_ptr->client,
                            SYS_MODEM_DS_PREF_NO_CHANGE,
                            SYS_MODEM_AS_ID_NO_CHANGE,
                            (sys_modem_as_id_e_type)req_ptr->default_voice_subs,
                            (sys_modem_as_id_e_type)req_ptr->default_data_subs,
                            (sys_modem_as_id_e_type)req_ptr->priority_subs,
		             DDS_CAUSE_PERMANENT
                            );
  }

  return rsp_ptr ;
}
/*lint +esym(715,req_pkt) */
/*lint +esym(715,pkt_len) */
/*lint +esym(818, req_pkt) diagpkt_user_table_entry_type prohibits that */

#endif /* FEATURE_MMODE_DUAL_SIM */


/*===========================================================================
Function: cmdiag_cm_get_cdma_info
Description:

       This function processes the CDMA specific call manager state
       information request packet from QXDM and sends the response back.

============================================================================*/
/*lint -esym(765, cmdiag_cm_get_cdma_info)
** Can't be static, to be called by DIAG via COMMAND DISPATCH TABLE
*/
PACKED void *cmdiag_cm_get_cdma_info
(
  PACKED void* req_pkt,

  uint16       pkt_len
)
/*lint -esym(818, req_pkt) diagpkt_user_table_entry_type prohibits that */
/*lint -esym(715,req_pkt)*/
/*lint -esym(715,pkt_len)*/
{
  DIAG_SUBSYS_CM_CM_GET_CDMA_INFO_rsp_type *rsp_ptr ;

  const int rsp_len = sizeof(DIAG_SUBSYS_CM_CM_GET_CDMA_INFO_rsp_type);
  cmlog_client_s_type* diag_client_ptr = cmlog_get_diag_client();

  /*lint -save -e641*/
  /* Allocate the memory for this */
  rsp_ptr = (DIAG_SUBSYS_CM_CM_GET_CDMA_INFO_rsp_type *)diagpkt_subsys_alloc(DIAG_SUBSYS_CM, CM_GET_CDMA_INFO,
                                 rsp_len );

  if (rsp_ptr != NULL)
  {
    /* Fill the response packet */
    /* Query Call Manager for State information */
    rsp_ptr->active_channel    = diag_client_ptr->active_channel;
    rsp_ptr->band_class        = diag_client_ptr->band_class;
    rsp_ptr->packet_zone       = diag_client_ptr->packet_zone;
    rsp_ptr->roam_status       = diag_client_ptr->roam_status;
    rsp_ptr->latitude          = diag_client_ptr->latitude;
    rsp_ptr->longitude         = diag_client_ptr->longitude;
    rsp_ptr->mcc               = diag_client_ptr->mcc;

  }
  /*lint -restore */

  return rsp_ptr ;
}


/*lint +esym(715,req_pkt) */
/*lint +esym(715,pkt_len) */
/*lint +esym(818, req_pkt) diagpkt_user_table_entry_type prohibits that */


/*===========================================================================
Function: cmdiag_cm_set_system_selection_with_lte
Description:

       This function processes the call manager system selection request
       packet from QXDM and sends the response back.

============================================================================*/
/*lint -esym(765, cmdiag_cm_set_system_selection_with_lte)
** Can't be static, to be called by DIAG via COMMAND DISPATCH TABLE
*/
PACKED void *cmdiag_cm_set_system_selection_with_lte
(
  PACKED void* req_pkt,

  uint16       pkt_len
)
{
  DIAG_SUBSYS_CM_CM_SYSTEM_SELECTION_PREFERENCE_WITH_LTE_req_type *req_ptr;
  DIAG_SUBSYS_CM_CM_SYSTEM_SELECTION_PREFERENCE_WITH_LTE_rsp_type *rsp_ptr;

  /*lint -save -e641*/

  cm_network_sel_mode_pref_e_type   nw_sel_mode_pref;
  sys_plmn_id_s_type                plmn_struct;

  cm_mode_pref_e_type               mode_pref;
  cm_gw_acq_order_pref_e_type       acq_order_pref;
  cm_band_pref_e_type               band_pref;
  cm_band_pref_e_type               lte_band_pref;
  cm_roam_pref_e_type               roam_pref;
  cm_hybr_pref_e_type               hybr_pref;
  cm_srv_domain_pref_e_type         srv_domain_pref;
  cmlog_client_s_type* diag_client_ptr = cmlog_get_diag_client();

  /* Allocate the memory for this */
  rsp_ptr = (DIAG_SUBSYS_CM_CM_SYSTEM_SELECTION_PREFERENCE_WITH_LTE_rsp_type *)diagpkt_subsys_alloc(DIAG_SUBSYS_CM, CM_SYSTEM_SELECTION_PREFERENCE_WITH_LTE,
                                 sizeof(DIAG_SUBSYS_CM_CM_SYSTEM_SELECTION_PREFERENCE_WITH_LTE_rsp_type) );
  req_ptr = (DIAG_SUBSYS_CM_CM_SYSTEM_SELECTION_PREFERENCE_WITH_LTE_req_type*) req_pkt;

  /* Ensure response packet allocation */
  if (!rsp_ptr)
  {
    return NULL;
  }

  /* Check for packet length */
  if (cmdiag_check_request_length (pkt_len, req_pkt, CM_SYSTEM_SELECTION_PREFERENCE_WITH_LTE) == FALSE)
  {
    CM_ERR_0("Packet length check failed");
    return rsp_ptr;
  }

  /* Get params from Request Packet */
  /* Convert from little endian notation */
  plmn_struct.identity[0] = req_ptr->PLMN[2];
  plmn_struct.identity[1] = req_ptr->PLMN[1];
  plmn_struct.identity[2] = req_ptr->PLMN[0];

  /*lint -save -e650 -e737
  **            Loss of sign in promotion from int to unsigned int
  **            Constant out of range for operator '!='
  **            AST picked 0xFFFFFFFF to mean "no change"
  **            so we just disable those two here
  */
  if (((req_ptr->mode_pref >= CM_MODE_PREF_MAX) &&
        (req_ptr->mode_pref != 0xFFFFFFFF))                         ||

      ((req_ptr->acq_order_pref >= CM_GW_ACQ_ORDER_PREF_MAX) &&
       (req_ptr->acq_order_pref != 0xFFFFFFFF ))                    ||

      ((req_ptr->roam_pref >= CM_ROAM_PREF_MAX) &&
       (req_ptr->roam_pref != 0xFFFFFFFF))                          ||

      ((req_ptr->hybr_pref >= CM_HYBR_PREF_MAX) &&
       (req_ptr->hybr_pref != 0xFFFFFFFF))                          ||

      ((req_ptr->srv_domain_pref >= CM_SRV_DOMAIN_PREF_MAX) &&
       (req_ptr->srv_domain_pref != 0xFFFFFFFF))                    ||

      ((req_ptr->network_sel_mode_pref >= CM_NETWORK_SEL_MODE_PREF_MAX) &&
       (req_ptr->network_sel_mode_pref != 0xFF)))
  {
    CM_ERR_0("One or more request parameters had illegal values");
  }
  /*lint -restore */

  if( req_ptr->network_sel_mode_pref < CM_NETWORK_SEL_MODE_PREF_MAX )
  {
    nw_sel_mode_pref = (cm_network_sel_mode_pref_e_type) req_ptr->network_sel_mode_pref;
  }
  else
  {
    nw_sel_mode_pref = CM_NETWORK_SEL_MODE_PREF_NO_CHANGE;
  }

  if( req_ptr->mode_pref < CM_MODE_PREF_MAX )
  {
    mode_pref = (cm_mode_pref_e_type) req_ptr->mode_pref;
  }
  else
  {
    mode_pref = CM_MODE_PREF_NO_CHANGE;
  }

  if( req_ptr->acq_order_pref < CM_GW_ACQ_ORDER_PREF_MAX )
  {
    acq_order_pref = (cm_gw_acq_order_pref_e_type) req_ptr->acq_order_pref;
  }
  else
  {
    acq_order_pref = CM_GW_ACQ_ORDER_PREF_NO_CHANGE;
  }

  /*lint -save -e650 Constant '-2147483647' out of range for operator '<'
  ** Band pref went to 64 bits.
  ** Can't modify req packet so just suppress it and accept that we'll
  ** have garbage there
  */
  /* This CM ss pref dispatch command/resp uses 32bit band pref.
  ** The value of CM_BAND_PREF_NO_CHANGE is unchanged when
  ** band pref is increased from 32bit to 64bit.
  */
  if( req_ptr->band_pref < CM_BAND_PREF_NO_CHANGE )
  {
    band_pref = (cm_band_pref_e_type) req_ptr->band_pref;
  }
  else
  {
    band_pref = CM_BAND_PREF_NO_CHANGE;
  }
  if( req_ptr->lte_band_pref < CM_BAND_PREF_NO_CHANGE )
  {
    lte_band_pref = (cm_band_pref_e_type) req_ptr->lte_band_pref;
  }
  else
  {
    lte_band_pref = CM_BAND_PREF_NO_CHANGE;
  }
  /*lint -restore */

  if( req_ptr->roam_pref < CM_ROAM_PREF_MAX )
  {
    roam_pref = (cm_roam_pref_e_type) req_ptr->roam_pref;
  }
  else
  {
    roam_pref = CM_ROAM_PREF_NO_CHANGE;
  }

  if( req_ptr->hybr_pref < CM_HYBR_PREF_MAX )
  {
    hybr_pref = (cm_hybr_pref_e_type) req_ptr->hybr_pref;
  }
  else
  {
    hybr_pref = CM_HYBR_PREF_NO_CHANGE;
  }

  if( req_ptr->srv_domain_pref < CM_SRV_DOMAIN_PREF_MAX )
  {
    srv_domain_pref = (cm_srv_domain_pref_e_type) req_ptr->srv_domain_pref;
  }
  else
  {
    srv_domain_pref = CM_SRV_DOMAIN_PREF_NO_CHANGE;
  }

  /* Issue SYS_SEL_PREF & Fill the response packet */
  rsp_ptr->status = cm_ph_cmd_sys_sel_pref_with_lte_new
                    (
                      NULL,
                      NULL,
                      diag_client_ptr->client,
                      mode_pref,
                      CM_PREF_TERM_PERMANENT, /* Store in NV */
                      0,
                      acq_order_pref,
                      band_pref,
                      lte_band_pref,
                      CM_PRL_PREF_NO_CHANGE,
                      roam_pref,
                      hybr_pref,
                      srv_domain_pref,
                      nw_sel_mode_pref,
                      &plmn_struct
                    );
  /*lint -restore */

  return rsp_ptr ;
}


/*===========================================================================
Function: cmdiag_cm_call_answer
Description:

       This function processes the call manager call answer request
       packet from QXDM and sends the response back.

============================================================================*/
/*lint -esym(765, cmdiag_cm_call_answer)
** Can't be static, to be called by DIAG via COMMAND DISPATCH TABLE
*/
PACKED void *cmdiag_cm_call_answer
(
  PACKED void* req_pkt,

  uint16       pkt_len
)
/*lint -esym(818, req_pkt) diagpkt_user_table_entry_type prohibits that */
/*lint -esym(715,req_pkt)*/
/*lint -esym(715,pkt_len)*/
{
  DIAG_SUBSYS_CM_CM_CALL_ANSWER_req_type *req_ptr;
  DIAG_SUBSYS_CM_CM_CALL_ANSWER_rsp_type *rsp_ptr;
  cm_ans_params_s_type                   *ans_params 
                                         = (cm_ans_params_s_type *) 
                                           cm_mem_malloc(sizeof(cm_ans_params_s_type));
  cmlog_client_s_type* diag_client_ptr = cmlog_get_diag_client();

  /*lint -save -e641*/
  /* Allocate the memory for this */
  rsp_ptr = (DIAG_SUBSYS_CM_CM_CALL_ANSWER_rsp_type *) diagpkt_subsys_alloc(DIAG_SUBSYS_CM, CM_CALL_ANSWER,
    sizeof(DIAG_SUBSYS_CM_CM_CALL_ANSWER_rsp_type));
  req_ptr = (DIAG_SUBSYS_CM_CM_CALL_ANSWER_req_type*) req_pkt;

  CM_ASSERT(ans_params != NULL);
  /* Ensure response packet allocation */
  if (!rsp_ptr)
  {
    /* Free allocated memory to heap */
    cm_mem_free(ans_params);
    return NULL;
  }

  /* Check for packet length */
  if (cmdiag_check_request_length (pkt_len, req_pkt, CM_CALL_ANSWER) == FALSE)
  {
    CM_ERR_0("Packet length check failed");
    /* Free allocated memory to heap */
    cm_mem_free(ans_params);
    return rsp_ptr;
  }
  /*lint -restore */

  /* Check call mode information type */
  switch (cmcall_call_id_mode(req_ptr->call_id))
  {
    case CM_CALL_MODE_INFO_CDMA:
      ans_params->info_type                        = CM_CALL_MODE_INFO_CDMA;
      ans_params->ans_params.cdma_ans.call_type    = CM_CALL_TYPE_VOICE;

      rsp_ptr->status = cm_mm_call_cmd_answer( NULL, NULL, diag_client_ptr->client,
                                            req_ptr->call_id, ans_params );
      break;

    case CM_CALL_MODE_INFO_GW_CS:
      ans_params->info_type                      = CM_CALL_MODE_INFO_GW_CS;
      ans_params->ans_params.gw_cs_ans.accept    = TRUE;
      ans_params->ans_params.gw_cs_ans.call_type = CM_CALL_TYPE_VOICE;

      rsp_ptr->status = cm_mm_call_cmd_answer( NULL, NULL, diag_client_ptr->client,
                                            req_ptr->call_id, ans_params );
      break;

    default:
      rsp_ptr->status = FALSE;
      break;
  }
  /* Free allocated memory to heap */
  cm_mem_free(ans_params);
  return rsp_ptr ;
}


/*===========================================================================
Function: cmdiag_cm_gw_setup_response
Description:

       This function processes the call manager setup response request
       packet from QXDM and sends the response back.

============================================================================*/
/*lint -esym(765, cmdiag_cm_gw_setup_response)
** Can't be static, to be called by DIAG via COMMAND DISPATCH TABLE
*/
PACKED void *cmdiag_cm_gw_setup_response
(
  PACKED void* req_pkt,

  uint16       pkt_len
)
/*lint -esym(818, req_pkt) diagpkt_user_table_entry_type prohibits that */
/*lint -esym(715,req_pkt)*/
/*lint -esym(715,pkt_len)*/
{
  DIAG_SUBSYS_CM_CM_GW_CALL_SETUP_RESPONSE_req_type *req_ptr;
  DIAG_SUBSYS_CM_CM_GW_CALL_SETUP_RESPONSE_rsp_type *rsp_ptr;
  cm_setup_res_params_s_type                        setup_res;
  cmlog_client_s_type* diag_client_ptr = cmlog_get_diag_client();

  /*lint -save -e641*/
  /* Allocate the memory for this */
  rsp_ptr = (DIAG_SUBSYS_CM_CM_GW_CALL_SETUP_RESPONSE_rsp_type *) diagpkt_subsys_alloc(DIAG_SUBSYS_CM, CM_GW_CALL_SETUP_RESPONSE,
    sizeof(DIAG_SUBSYS_CM_CM_GW_CALL_SETUP_RESPONSE_rsp_type));
  req_ptr = (DIAG_SUBSYS_CM_CM_GW_CALL_SETUP_RESPONSE_req_type*) req_pkt;

  /* Ensure response packet allocation */
  if (!rsp_ptr)
  {
    return NULL;
  }

  /* Check for packet length */
  if (cmdiag_check_request_length (pkt_len, req_pkt, CM_GW_CALL_SETUP_RESPONSE) == FALSE)
  {
    CM_ERR_0("Packet length check failed");
    return rsp_ptr;
  }
  /*lint -restore */

  memset (&setup_res, 0, sizeof(cm_setup_res_params_s_type));
  setup_res.accept = TRUE;
  rsp_ptr->status = cm_mm_call_cmd_setup_res( NULL, NULL, diag_client_ptr->client,
                                              req_ptr->call_id, &setup_res );

  return rsp_ptr ;
}


/**************************************************************************
** CM SUBSYSTEM COMMAND DISPATCH TABLE.
Note:  Any new commands for CM need to be added here.
****************************************************************************/

static const diagpkt_user_table_entry_type cm_subsys_diag_tbl[] =
{
  {CM_STATE_INFO,                           CM_STATE_INFO,
                                            cmdiag_cm_state_info},
  {CM_PLMN_SELECTION_MODE,                  CM_PLMN_SELECTION_MODE,
                                            cmdiag_cm_get_plmn_list},
  #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
  {CM_CDMA_CALL_ORIGINATION,                CM_CDMA_CALL_ORIGINATION,
                                            cmdiag_cm_call_orig},
  #endif
  #ifdef CM_GW_SUPPORTED
  {CM_WCDMA_CALL_ORIGINATION,               CM_WCDMA_CALL_ORIGINATION,
                                            cmdiag_wcdma_call_origination},
  {CM_GSM_CALL_ORIGINATION,                 CM_GSM_CALL_ORIGINATION,
                                            cmdiag_gsm_call_origination},
  {CM_GET_CLOSED_USER_GROUP_PREFERENCE,     CM_GET_CLOSED_USER_GROUP_PREFERENCE,
                                            cmdiag_cm_get_cug_pref},
  {CM_SET_CLOSED_USER_GROUP_PREFERENCE,     CM_SET_CLOSED_USER_GROUP_PREFERENCE,
                                            cmdiag_cm_set_cug_pref},
  {CM_SEND_USSD_REPLY,                      CM_SEND_USSD_REPLY,
                                            cmdiag_cm_send_ussd_reply},
  {CM_SET_PASSWORD,                         CM_SET_PASSWORD,
                                            cmdiag_cm_set_password},
  {CM_GET_CCM_INFO,                         CM_GET_CCM_INFO,
                                            cmdiag_cm_get_ccm_info},
  #endif
  #if (defined(FEATURE_CM_MMGPS) || defined(FEATURE_CM_LCS))
  {CM_SEND_SUPS_LCS_MOLR,                   CM_SEND_SUPS_LCS_MOLR,
                                            cmdiag_cm_send_sups_lcs_molr},
  {CM_SEND_SUPS_LCS_MOLR_COMPLETED,         CM_SEND_SUPS_LCS_MOLR_COMPLETED,
                                            cmdiag_cm_send_sups_lcs_molr_completed},
  #endif
  {CM_CALL_END,                             CM_CALL_END,
                                            cmdiag_cm_call_end},
  {CM_GET_CALL_INFORMATION_LIST,            CM_GET_CALL_INFORMATION_LIST,
                                            cmdiag_cm_get_call_information_list},
  {CM_SYSTEM_SELECTION_PREFERENCE,          CM_SYSTEM_SELECTION_PREFERENCE,
                                            cmdiag_cm_set_system_selection},
  #if defined(FEATURE_CALL_THROTTLE)
  {CM_ORIG_THR_TBL_RESET,                   CM_ORIG_THR_TBL_RESET,
                                            cmdiag_orig_thr_tbl_reset},
  #endif
  {CM_SYSTEM_SELECTION_PREFERENCE2,         CM_SYSTEM_SELECTION_PREFERENCE2,
                                            cmdiag_cm_set_system_selection2},
  {CM_GET_CELL_SERVICE_IND,                 CM_GET_CELL_SERVICE_IND,
                                            cmdiag_cm_get_cell_service_ind},
  #if ((defined CM_GW_SUPPORTED) && defined (FEATURE_MULTIMEDIA_FALLBACK))
  #error code not present
#endif

  #ifdef FEATURE_MMODE_DUAL_SIM
  {CM_DUAL_STANDBY_PREF,                    CM_DUAL_STANDBY_PREF,
                                            cmdiag_cm_dual_standby_pref},
  #endif

  #ifdef FEATURE_ECALL_APP
   {CM_ECALL_CALL_ORIGINATION, CM_ECALL_CALL_ORIGINATION,   ecall_diag_call_origination},
   {CM_ECALL_CALL_END, CM_ECALL_CALL_END,   ecall_diag_call_end},
  #endif /* FEATURE_ECALL_APP */

  {CM_GET_CDMA_INFO,                        CM_GET_CDMA_INFO,
                                            cmdiag_cm_get_cdma_info},
  {CM_SYSTEM_SELECTION_PREFERENCE_WITH_LTE, CM_SYSTEM_SELECTION_PREFERENCE_WITH_LTE,
                                            cmdiag_cm_set_system_selection_with_lte},
  {CM_CALL_ANSWER,                          CM_CALL_ANSWER,
                                            cmdiag_cm_call_answer},
  {CM_GW_CALL_SETUP_RESPONSE,               CM_GW_CALL_SETUP_RESPONSE,
                                            cmdiag_cm_gw_setup_response},
  #ifdef FEATURE_MMODE_DUAL_SIM
  {CM_MULTISIM_STATE_INFO,                  CM_MULTISIM_STATE_INFO,
                                            cmdiag_cm_multisim_state_info}
  #endif
};


/* Dispatch table to register with the diagnostics packet service. */
static const diagpkt_user_table_entry_type cmdiag_tbl[] = {

  {DIAG_ORIG_F,       DIAG_ORIG_F,        cmdiag_orig_req},
  {DIAG_END_F,        DIAG_END_F,         cmdiag_end_req},
  {DIAG_CONTROL_F,    DIAG_CONTROL_F,     cmdiag_control},
  {DIAG_CNTRY_INFO_F, DIAG_CNTRY_INFO_F,  cmdiag_cntry_info_req},
  {DIAG_SUPS_REQ_F,   DIAG_SUPS_REQ_F,    cmdiag_sups_req},
  #ifdef FEATURE_CM_SS_MEAS
  #error code not present
#endif

};

#ifdef FEATURE_MMODE_DUAL_SIM
static const diagpkt_user_table_entry_type cm_subsys_diag_tbl_per_subs[] =
{
  {CM_STATE_INFO_PER_SUBS,                   CM_STATE_INFO_PER_SUBS,
                                             cmdiag_cm_state_info_per_subs},
  {CM_PLMN_SELECTION_MODE_PER_SUBS,          CM_PLMN_SELECTION_MODE_PER_SUBS,
                                             cmdiag_cm_get_plmn_list_per_subs},
  #ifdef CM_GW_SUPPORTED
  {CM_WCDMA_CALL_ORIGINATION_PER_SUBS,       CM_WCDMA_CALL_ORIGINATION_PER_SUBS,
                                             cmdiag_wcdma_call_origination_per_subs},
  {CM_GSM_CALL_ORIGINATION_PER_SUBS,         CM_GSM_CALL_ORIGINATION_PER_SUBS,
                                             cmdiag_gsm_call_origination_per_subs},
  {CM_SEND_USSD_REPLY_PER_SUBS,              CM_SEND_USSD_REPLY_PER_SUBS,
                                             cmdiag_cm_send_ussd_reply_per_subs},
  {CM_SET_PASSWORD_PER_SUBS,                 CM_SET_PASSWORD_PER_SUBS,
                                             cmdiag_cm_set_password_per_subs},
  {CM_GET_CCM_INFO_PER_SUBS,                 CM_GET_CCM_INFO_PER_SUBS,
                                             cmdiag_cm_get_ccm_info_per_subs},
  #endif
  #if (defined(FEATURE_CM_MMGPS) || defined(FEATURE_CM_LCS))
  {CM_SEND_SUPS_LCS_MOLR_PER_SUBS,           CM_SEND_SUPS_LCS_MOLR_PER_SUBS,
                                             cmdiag_cm_send_sups_lcs_molr_per_subs},
  {CM_SEND_SUPS_LCS_MOLR_COMPLETED_PER_SUBS, CM_SEND_SUPS_LCS_MOLR_COMPLETED_PER_SUBS,
                                             cmdiag_cm_send_sups_lcs_molr_completed_per_subs},
  #endif
  {CM_GET_CALL_INFORMATION_LIST_PER_SUBS,    CM_GET_CALL_INFORMATION_LIST_PER_SUBS,
                                             cmdiag_cm_get_call_information_list_per_subs},
  {CM_SYSTEM_SELECTION_PREFERENCE_PER_SUBS,  CM_SYSTEM_SELECTION_PREFERENCE_PER_SUBS,
                                             cmdiag_cm_set_system_selection_per_subs},
  {CM_GET_CELL_SERVICE_IND_PER_SUBS,         CM_GET_CELL_SERVICE_IND_PER_SUBS,
                                             cmdiag_cm_get_cell_service_ind_per_subs},
  {CM_SYSTEM_SELECTION_PREFERENCE2_PER_SUBS,  CM_SYSTEM_SELECTION_PREFERENCE2_PER_SUBS,
                                             cmdiag_cm_set_system_selection2_per_subs}
};
#endif /* FEATURE_MMODE_DUAL_SIM */

#ifdef __cplusplus

DIAGPKT_DISPATCH_AUTOREGISTER (DIAGPKT_NO_SUBSYS_ID, cmdiag_tbl);
DIAGPKT_DISPATCH_AUTOREGISTER (DIAG_SUBSYS_CM, cm_subsys_diag_tbl);
#ifdef FEATURE_MMODE_DUAL_SIM
DIAGPKT_DISPATCH_AUTOREGISTER (DIAG_SUBSYS_CM, cm_subsys_diag_tbl_per_subs);
#endif /*FEATURE_MMODE_DUAL_SIM */
#else

void cmdiag_init (void)
{
  /*
  ** 717: do ... while(0) used by DIAG so we supress it
  */
  /*lint -save -e717 */
  DIAGPKT_DISPATCH_TABLE_REGISTER (DIAGPKT_NO_SUBSYS_ID, cmdiag_tbl);
  DIAGPKT_DISPATCH_TABLE_REGISTER (DIAG_SUBSYS_CM, cm_subsys_diag_tbl);
  #ifdef FEATURE_MMODE_DUAL_SIM
  DIAGPKT_DISPATCH_TABLE_REGISTER (DIAG_SUBSYS_CM, cm_subsys_diag_tbl_per_subs);
  #endif /*FEATURE_MMODE_DUAL_SIM */
  TTLBOOTUP(CM_DIAG_REGISTERED);
  /*lint -restore */
}

#endif

#ifdef __cplusplus
}
#endif

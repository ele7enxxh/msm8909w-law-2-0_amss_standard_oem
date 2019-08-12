
/*===========================================================================

                  Main Control Event and Logging Functions

GENERAL DESCRIPTION
  This module contains utilities and functions related to logging of events
  and DIAG/DM packets inside MC

  Copyright (c) 2000 - 2014 Qualcomm Technologies, Inc.
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

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE
This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mclog.c_v   1.19   02 Oct 2002 22:52:30   louiel  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/src/mclog.c#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/02/16   ppr     QSH:Enable event diagnostic data to QSH
06/19/15   agh     Added Diag cmd to modify Ovhds Refresh time for testing
12/02/14   agh     Support for new customer specific voice service option
07/21/14   gga     FR 20298: SIM Swap without RESET if carrier settings have 
                   changed
06/09/14   agh     FR 17879: Added support for extended EARFCN
12/20/13   ppr     1X_CP related logging changes
10/03/13   pap     Mainlining FEATURE_SPECIFIC_CALL_END_STATS.
08/08/13   agh     Provided df_str size as argument to mccap_df_query to
                   handle buffer overflow in mccap_df_create_discovery_string
06/13/13   gga     Added NV support for FEATURE_SPECIFIC_CALL_END_STATS
02/20/13   ppr     Added 1xCSFB: 1x_to_lte_reselection start event
01/18/13   ppr     Changes for 1xCSFB:Event for UHDM/ECAM/Redirection call types
11/14/12   ppr     Added Page match event
07/02/12   jj      Added event upon EHOCM is sent on UL.
06/29/12   ppr     FEATURE_IS2000_REL_A_CC cleanup changes
06/21/12   ssh     NV based disabling of 1X to LTE reselections.
05/31/12   ssh     Added minimum length checks on Diag packet for DF items.
05/09/12   ppr     Feature Cleanup
03/13/12   jj      Added new CSFB events to indicate Time Trasfer ,
                   Handover comletion and Receipt of GCSNA UL/DL messages.
02/29/12   ssh     Added events for 1X to LTE reselection.
02/21/12   ppr     Feature Cleanup
02/15/12   srk     Feature Cleanup: Removed FEATURE_NO_13K_VOICE_SUPPORT and
                   FEATURE_SO_VOICE_SMV
02/04/12   ppr     Feature Cleanup
01/27/12   ppr     Feature Cleanup: Mainlind Always ON features
01/23/12   srk     Feature Cleanup
01/17/12   ppr     Feature Cleanup: Mainlined Always ON features
01/06/12   ppr     1X CP - Feature clean up: Mainline Always On features,
                   Remove Always Off Features
09/02/11   sk      Rebased the code to the NikeL modem package by mainlining
                   FEATURE_MODEM_1X_NIKEL.
05/02/11   ssh     Added Diag request support for getting SPM, ESPM, ITSPM.
04/22/11   jtm     CMI Phase 4 - Final SU API changes.
03/30/11   ag      Fix to use DB_CDMA_RSSI instead of DB_RSSO in DIAG status
                   commands response packets.
02/10/11   ag      Increased size of Markov statistics variables for eMSO to
                   32 bits from 16 bits to support long duration testing.
01/04/11   ag      Used #if !(defined) for FEATURE_NO_13K_VOICE_SUPPORT.
11/15/10   jtm     Initial SU API split for CP.
10/08/10   ag      Fixed KW errors.
06/15/10   ag      Added support for RDA testing.
05/26/10   ag      Replaced PACKED with PACK() to support QDSP6.
02/25/10   ag      Support to display SO74 frame statistics
01/25/10   ag      Reverted changes done for EMSO stats.
01/20/10   ag      Use ONEX_CVT_Q2N instead of QW_CVT_Q2N for MDM9K.
11/06/09   ag      1X Advanced - Enhanced Markov Service Option Support
10/01/09   jj      Included new diag header to accomodate diag's CMI changes.
09/09/09   adw     Removed unnecessary commented out _v.h includes.
08/02/09   jtm     Lint fixes.
06/23/09   ag      Support to display network parameters on UI
05/14/09   jj      CMI Phase-II update.
12/02/08   adw     Commented out _v.h includes that no longer exist inside 1x.
08/21/08   ag      Included cdma2kdiag.h as part of DIAG VU decoupling process
20/03/08   bb      Fixed klocwork errors
11/21/06   sb      Added support for VOIP-1X handoffs.
06/02/06   fc      Added IHO/AHO/APHO event report function.
05/01/06   an      Fixed Lint Errors
04/29/06   fc      Fixed lint error.
04/24/06   fc      Fixed Rel A featurization.
10/15/05   fc      Cleanup NCC featurization.
10/10/05   fh      Removed including of dec.h. changed dec_version to txc_dec_version
07/11/05   fh      Cleaned up header file including from MUX:
                     changed rxc.h to rxcmc.h
07/07/05   va      Merged the following from MSMSHARED:
07/05/05   sb       Added MEID support.
06/17/05   fc      Fixed the issue of MER counters being incremented
                   incorrectly if multiple GPM received in the assigned slot.
                   Added the suport for MER benchmark in non-slotted mode.
05/03/05   fc      Merged the following from MSMSHARED:
04/27/05   an       Added support for 4GV service option
03/23/05   fc      Merged the following from MSMSHARED:
03/21/05   an       Mainlined IS2000 features and implemented the new
                    Dynamic Feature Design.
02/25/05   fc      Merged the following from MSMSHARED:
02/17/05   sb       Added event to indicate a registration was suppressed.
09/27/04   va      Merged from REL B/C archive:
07/08/04   bkm      Re-organize support for MER counters.
08/23/04   az      Rel D Negative SCI - changes for dynamic feature
08/18/04   fc      Removed silk incl flags on overhead messages.
08/17/04   az      Diag command MC_DIAG_SUBSYS_SET_CUR_PREF_SCI for setting
                   current preferred sci
06/28/04   bkm     Merged the following from IS2000_REL_B_C:
  06/24/04   sb      Release C addendum changes for PLCM.
06/16/04   fc      Added DF support for Sync ID and Reconnect message.
06/08/04   bkm     Fixed typos in feature names within comments.
06/04/04   bkm     Lint cleanup.
05/28/04   bkm     Merged the following from IS2000_REL_B_C:
  03/15/04   yll     Added the slept flag in the SMS_WAKEUP event.
04/08/04   bkm     Channel mask updates.
01/05/04   bkm     Misc changes for DV F-PDCH and channel masks.
12/16/03   fc      Added support for EVENT_SEG_UPM_ADDR_MISMATCH.
12/02/03   bkm     Updated CH_IND/EXT_CH_IND event reporting.
11/06/03   bkm     Added diag command to retrieve Paging MER counters.
11/03/03   ph      fix r-dcch message logging.
10/24/03   ph      include mccccim.h to rid of compiler warning
10/23/03   va      Changed caii_ext_ch_ind3_pilot_rec_type to
                   mccdma_pilot_info_type
10/17/03   ph      added support for new DIAG interface to retrieve MC state.
10/03/03   bkm     Add match type to mclog_report_matched_msg().
10/03/03   bkm     Corrected setting of msg_id in mclog_report_event_msg_rxed()
                   to exclude 2-bit prot_disc and removed PREV check from
                   mclog_report_event_msg_txed().
09/11/03   yll     Added featurization for ACP include file.
09/09/03   yll     Dynamic Feature query directly from DF NV item.
08/11/03   bkm     Removed support for FEATURE_SPECIAL_MDR.
08/01/03   yll     Added support for Dynamic Feature.
07/23/03   bkm     Some more lint cleanup.
07/23/03   lh      Zero init static psist to make tools happy.
06/02/03   bkm     Lint cleanup.
04/17/03   bkm     Removed call to diag_nv_read to read the NAM in mclog_stat()
                   as the info in diagnv should be current.
12/02/02   yll     Added event logging for wakeup slot, matched msg, call
                   release, pref system reselection, etc.
10/02/02   yll     Linted the code.
10/02/02   az      Mainlined FEATURE_IS2000
09/30/02   sb      Mainlined FEATURE_SD20.
09/20/02   fc      Included mcsys.h only if FEATURE_SD20 is not defined.
08/22/02   va      Fixed the error when release substate can be reported as
                   WFA in events for REL_A off builds.
07/02/02   az      Added new voice service option FEATURE_SO_VOICE_SMV
06/13/02   yll     Changed the BCSMS feature name to be under Rel A CC.
04/09/02   yll     Fixed the logging mask for IS2000 RelA common channel.
03/27/02   sb      Added bcch_acquired event.
03/27/02   va      Fixed compiler issues for non-RELA build.
03/25/02   va      Added support for converting CC related events into CP state
                   change events for non-RELA builds.
03/11/02   va      Merged from MSM CP VU 119.
  02/20/02 az      Featurized 13K voice service option
02/26/02   fc      Added support for event common channel monitored.
02/22/02   fc      Obsolete event QPCH in use for Release A common channels.
02/13/02   sb      Merged the following from the common archive:
                   01/12/02   sst     Modified to use the new srch_* files that replaced srchdrv.c
                                      Includes a function name change:
                                      srch_get_dmod_id -> srch_util_get_dmod_id
                                      Also removed reference to enc_version() and replaced it
                                      with the function call srch_util_get_dmod_id()
02/13/02   va      Added support for CC related events
08/08/01   yll     Merged in the following from MSM archive:
                    Added ifdef to solve problem with non-IS2000 builds.
                    Added support for SO54, SO55.
                    Corrected some #ifdefs.
06/26/01   sb      Merged the following from common archive:
           06/11/01   lcc     Corrected compilation errors for non-IS2000 targets.
           06/06/01   lcc     Added BS P_REV, and F/R RCs for FCH, DCCH, and SCH
                      to the IS2000 status packet.
04/27/01   fc      Changed function prototype of
                   mclog_event_report_pch_acquired.
04/18/01   fc      Featurized mclog_report_event_ch_ind under FEATURE_IS2000.
                   Changed mclog_report_event_f_fdcch_acquired to report
                   F-FCH acquired event for non-IS2000 targets.
04/12/01   fc      Added/Moved events logging from mc.c and mccsup.c to here.
04/12/01   fc      Fixed compilation problem of mclog_get_is2k_status() with
                   SD20.
04/10/01   jq      Created the file to handle logging and diagpkt needs.
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "1x_variation.h"
#include "comdef.h"
#include "target.h"
#include "customer.h"
#include "modem_1x_defs.h"
#include "mccccl.h"
#include "mccdma.h"
#include "rxcmc_i.h"
#include "diagbuf.h"
#include "diagi.h"
#include "diagnv.h"
#include "cdma2kdiag.h"
#include "msg.h"
#include "m1x_diag.h"

#include "mar_i.h"
#include "bit.h"
#include "mccap_v.h"
#include "mccsch.h"
#include "mccsch_i.h"
#include "err.h"
#include "cai_v.h"
#include "event.h"
#include "mclog_v.h"
#include "mccdma_v.h"
#include "mccdma_i.h"
#include "mccccim.h"
#include "snm_v.h"
#include "snm_i.h"
#include "modem_1x_utils.h"

#ifdef FEATURE_1XCP_QSH_SUPPORT
#include "mccqsh_i.h"
#endif /* FEATURE_1XCP_QSH_SUPPORT */

/*===========================================================================

                     VARIABLES AND ROUTINES

===========================================================================*/
/*---------------------------------------------------------------------------
  CRC counts for paging and sync channels
---------------------------------------------------------------------------*/
static rxc_crc_cnt_type  rx_sc_crc_cnt={0,0};  /* catch rx's counts here */
static rxc_crc_cnt_type  sc_crc_cnt={0,0};
static rxc_crc_cnt_type  rx_pc_crc_cnt={0,0};  /* catch rx's counts here */
static rxc_crc_cnt_type  pc_crc_cnt={0,0};

/*--------------------------------------------------------------------------
  DB item data buffer for db reads
--------------------------------------------------------------------------*/
static db_items_value_type diag_db;

/*--------------------------------------------------------------------------
  Network Parameters values
--------------------------------------------------------------------------*/
static uint32 mclog_inter_freq_idle_ho_count = 0;
static uint32 mclog_hard_ho_count = 0;

/*--------------------------------------------------------------------------
   Holding info for events reporting
--------------------------------------------------------------------------*/

LOCAL struct
{

  boolean using_private_lcm;
  /* TRUE if the mobile is using the private long code mask; FALSE otherwise */

  cai_plcm_type public_long_code_mask_type;
  /* The type of public long code mask currently in use */
  mclog_event_op_mode_type curr_op_mode;
    /* Current operation mode */
  word curr_state;
   /* Current call processing state */
  boolean pagech_acq;
   /* Flag indicates whether paging channel is acquired for the first time
      after power up or operation mode change */

#ifndef FEATURE_IS2000_REL_A
  /* Needed for NON-RELA targets to report CP state change event
  correctly */
  mclog_event_cp_state_type tc_substate;
#endif

} mclog_event = {

  FALSE,

  CAI_PLCM_TYPE_ESN,
  MCLOG_OP_MODE_POWERUP, (word) CDMA_SYS_DET_S, FALSE

#ifndef FEATURE_IS2000_REL_A
        ,CDMA_MAX_S //Initialize to an invalid value to catch errors
  #endif /* !defined(FEATURE)IS2000_REL_A) */

};

/* Structure of information needed for MER counter support */
mclog_mer_type mclog_mer;

/*--------------------------------------------------------------------------
   Holding info for Diag Subsystem Dispatch command processing.
--------------------------------------------------------------------------*/

/* Dynamic Feature related command handlers. Note they are not under */
/* FEATURE_IS2000_DYNAMIC_FEATURE, because even if the feature is    */
/* disabled, we still may receive these commands from Tools          */

PACK (void *) mc_diag_df_items_query_handler (PACK (void *), uint16);
PACK (void *) mc_diag_df_item_verify_handler (PACK (void *), uint16);
PACK (void *) mc_diag_df_items_write_handler (PACK (void *), uint16);
PACK (void *) mc_diag_get_nw_params(PACK (void *), uint16);
#ifdef FEATURE_1XCP_FTD
PACK (void *) mc_diag_get_system_parameters(PACK (void *), uint16);
#endif /* FEATURE_1XCP_FTD */
#ifdef FEATURE_IS2000_1X_ADV
PACK (void *) mc_diag_enh_markov_stat(PACK (void *), uint16);
#endif

/* MC State Query handler */
PACK (void *) mclog_diag_state_query_handler (PACK (void *), uint16);

/* Paging MER Stats diag command handler */
PACK (void *) mclog_paging_mer_stat (PACK (void *) req_pkt, uint16 pkt_len);

/* set the current preferred slot cycle index */
PACK (void *) mclog_diag_set_curr_pref_sci (PACK (void *) req_pkt, uint16 pkt_len);

#ifdef FEATURE_MODEM_1X_MSO_RDA_TEST
PACK (void *) mc_diag_mar_rda_test_stat (PACK (void *) req_pkt, uint16 pkt_len);
#endif /* FEATURE_MODEM_1X_MSO_RDA_TEST */

#ifdef FEATURE_MODEM_CONFIG_REFRESH
/* Handler for powerup load command from diag */    
PACK (void *) mc_diag_reload_nv_items (PACK (void *) req_pkt, uint16 pkt_len);
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

/* Handler for cmd to modify overheads refresh time */    
PACK (void *) mclog_diag_set_ovhd_refresh_timer (PACK (void *) req_pkt, uint16 pkt_len);

/* 1X_CP Diag Subsystem Dispatch Command. The dispatch table. */
static const diagpkt_user_table_entry_type mc_diag_tbl[] =
{
  {MC_DIAG_SUBSYS_STATE_QUERY, MC_DIAG_SUBSYS_STATE_QUERY, mclog_diag_state_query_handler},
  {MC_DIAG_SUBSYS_PAGING_MER_STAT, MC_DIAG_SUBSYS_PAGING_MER_STAT, mclog_paging_mer_stat},
  {MC_DIAG_SUBSYS_SET_CUR_PREF_SCI, MC_DIAG_SUBSYS_SET_CUR_PREF_SCI, mclog_diag_set_curr_pref_sci},
  {MC_DIAG_SUBSYS_DF_ITEMS_QUERY, MC_DIAG_SUBSYS_DF_ITEMS_QUERY, mc_diag_df_items_query_handler},
  {MC_DIAG_SUBSYS_DF_ITEM_VERIFY, MC_DIAG_SUBSYS_DF_ITEM_VERIFY, mc_diag_df_item_verify_handler},
  {MC_DIAG_SUBSYS_DF_ITEMS_WRITE, MC_DIAG_SUBSYS_DF_ITEMS_WRITE, mc_diag_df_items_write_handler},
  {MC_DIAG_SUBSYS_GET_NW_PARAMS, MC_DIAG_SUBSYS_GET_NW_PARAMS, mc_diag_get_nw_params}
  #ifdef FEATURE_IS2000_1X_ADV
  ,{MC_DIAG_SUBSYS_ENH_MARKOV_STAT, MC_DIAG_SUBSYS_ENH_MARKOV_STAT, mc_diag_enh_markov_stat}
  #endif
  #ifdef FEATURE_MODEM_1X_MSO_RDA_TEST
  ,{MC_DIAG_SUBSYS_MAR_RDA_TEST_STAT, MC_DIAG_SUBSYS_MAR_RDA_TEST_STAT, mc_diag_mar_rda_test_stat}
  #endif
  #ifdef FEATURE_1XCP_FTD
  ,{MC_DIAG_SYS_PARAMS_QUERY, MC_DIAG_SYS_PARAMS_QUERY, mc_diag_get_system_parameters}
  #endif /* FEATURE_1XCP_FTD */
  #ifdef FEATURE_MODEM_CONFIG_REFRESH   
  ,{MC_DIAG_SUBSYS_RELOAD_NV_ITEMS, MC_DIAG_SUBSYS_RELOAD_NV_ITEMS, mc_diag_reload_nv_items}
  #endif /* FEATURE_MODEM_CONFIG_REFRESH */  
  ,{MC_DIAG_SUBSYS_SET_OVHD_REFRESH_TIME, MC_DIAG_SUBSYS_SET_OVHD_REFRESH_TIME, mclog_diag_set_ovhd_refresh_timer}
};

/*===========================================================================

FUNCTION MCLOG_GOOD_FRAMES formerly DIAGP_GOOD_FRAMES

DESCRIPTION
  Check for good frames
  This function is taken from diagp.c

DEPENDENCIES
  None.

RETURN VALUE
  0 if bad, 1 if good, -1 if indeterminate

SIDE EFFECTS
  None.

===========================================================================*/

int1 mclog_good_frames
(
  rxc_crc_cnt_type     *cur,    /* Current good CRCs */
  rxc_crc_cnt_type     *old     /* Old good CRCs */
)
{
  static word psist = 0;
  int1 ret_val;

  /*-----------------------------------------------------------------------
    If the good and bad counters have reset, then reset the old values.
  -----------------------------------------------------------------------*/

  if( cur->good_crcs < old->good_crcs || cur->bad_crcs < old->bad_crcs ) {
    /* we've gone back to zero... */
    old->good_crcs = 0;
    old->bad_crcs = 0;
  }

  if( cur->good_crcs > old->good_crcs ) {
    /* we've had some good frames */
    old->good_crcs = cur->good_crcs;
    old->bad_crcs = cur->bad_crcs;
    psist = 2;
    ret_val = 1;                  /* good frames */
  }

  else if( cur->bad_crcs > old->bad_crcs+4 ) {
    /* no good frames, more than 5 bad */
    ret_val = 0;                  /* bad frames */
  }

  else if( psist != 0 ) {
    psist--;
    ret_val = 1;                  /* still return good for a few */
  }

  else {
    ret_val = -1;                 /* don't know */
  }

  return (ret_val);

} /* mclog_good_frames */

/*===========================================================================

FUNCTION MCLOG_STAT

DESCRIPTION
  This procedure processes a Status request and formats a Status
  Response type.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

===========================================================================*/

PACK (void *) mclog_stat
(
  PACK (void *)req_pkt,            /* pointer to request packet  */
  word len                 /* length of request packet   */
)
{
  word              i;
  mcc_status_type entry; /* CDMA and Analog last entry reason */
  boolean           digital;
  diag_rsp_type *rsp_ptr;
  const int rsp_len = FSIZ(diag_rsp_type, stat);

  rsp_ptr = (diag_rsp_type *) diagbuf_pkt_alloc(rsp_len);

  /* if rsp_ptr is NULL return from this function */
  if( rsp_ptr == NULL)
  {
      M1X_MSG( DCP, LEGACY_HIGH,
        "Out of free DIAG Pkts");
      return NULL;
  }

  /*----------------------------------------------------------------------
    Fill in the packet with status information.
  -----------------------------------------------------------------------*/

  rsp_ptr->stat.cmd_code = DIAG_STATUS_F;

  rsp_ptr->stat.demod    = (byte)srch_util_get_dmod_id();
  rsp_ptr->stat.decode   = rxc_dec_version();
  rsp_ptr->stat.inter    = (byte)srch_util_get_dmod_id();
  rsp_ptr->stat.esn      = diagnv.esn;

  /* Get the current mode.  If RF mode is not Analog ( CDMA or PCN ) then
     declare digital,  otherwise declare analog
  */

  db_get ( DB_RF_MODE, &diag_db );

  if ( diag_db.rf_mode == DB_RF_ANALOG )
  {
    rsp_ptr->stat.rf_mode = 0;
  }
  else if ((diag_db.rf_mode == DB_RF_CDMA)    ||
           (diag_db.rf_mode == DB_RF_900MHZ))
  {
    /* --------------------------------------------------------
    ** For now, have the DM treat 900MHZ mode just like regular
    ** CDMA cellular mode ( there's really no difference except
    ** for the frequency range anyway)
    ** -------------------------------------------------------- */
    rsp_ptr->stat.rf_mode = 1;
  }
  else if ( diag_db.rf_mode == DB_RF_PCN )
  {
    rsp_ptr->stat.rf_mode = 2;
  }
  else if ( diag_db.rf_mode == DB_RF_SLEEP )
  {
    rsp_ptr->stat.rf_mode = 3;
  }
  else if ( diag_db.rf_mode == DB_RF_GPS )
  {
    rsp_ptr->stat.rf_mode = 4;
  }
  else if ( diag_db.rf_mode == DB_RF_HDR )
  {
    rsp_ptr->stat.rf_mode = 5;
  }
  else
  {
    M1X_MSG( DCP, LEGACY_MED,
      "Unexpected rf mode in database: %d",
      diag_db.rf_mode );
  }

  digital = ( rsp_ptr->stat.rf_mode == DB_RF_ANALOG ) ? FALSE : TRUE; /*lint !e641 */

  /* Now that we know mode,  get the last entry reason */
  mc_get_entry(&entry);
  if ( digital ) {
    rsp_ptr->stat.entry_reason = entry.cdma; /*lint !e641 */
  }

  /* MIN1 and MIN2 for the current NAM */
  for (i = 0; i<NV_MAX_MINS; i++ ) {
    rsp_ptr->stat.min1[i] = diagnv.min1[diagnv.current_nam][i];
    rsp_ptr->stat.min2[i] = diagnv.min2[diagnv.current_nam][i];
  }

  rsp_ptr->stat.orig_min = 0;

  /* Always provide rx state data, even if in analog */

  /* rxc_get_state returns an enum type which uses no negative values */
  switch( rsp_ptr->stat.cdma_rx_state = (word) rxc_get_state() )
  {
    case RXC_SC_STATE:
      rxc_get_sc_crc ( &rx_sc_crc_cnt );
      rsp_ptr->stat.cdma_good_frames =
            mclog_good_frames( &rx_sc_crc_cnt, &sc_crc_cnt );
      break;

    case RXC_PC_STATE:
      rxc_get_pc_crc ( &rx_pc_crc_cnt );
      rsp_ptr->stat.cdma_good_frames =
            mclog_good_frames( &rx_pc_crc_cnt, &pc_crc_cnt );
      break;

    default:
      rsp_ptr->stat.cdma_good_frames = -1;
  }

  rsp_ptr->stat.analog_corrected_frames = 0;
  rsp_ptr->stat.analog_bad_frames = 0;
  rsp_ptr->stat.analog_word_syncs = 0;

  db_get ( DB_CHANNEL, &diag_db );
  rsp_ptr->stat.curr_chan = diag_db.channel;

  db_get ( DB_CODE_CHANNEL, &diag_db );
  rsp_ptr->stat.cdma_code_chan = diag_db.code_channel;

  db_get (DB_PILOT_PN_OFF, &diag_db );
  rsp_ptr->stat.pilot_base = diag_db.pilot_pn_off;

  db_get ( DB_SID, &diag_db );
  rsp_ptr->stat.sid = diag_db.sid;

  db_get ( DB_NID, &diag_db );
  rsp_ptr->stat.nid = diag_db.nid;

  db_get ( DB_LOCAID, &diag_db );
  rsp_ptr->stat.locaid = diag_db.locaid;

  db_get ( DB_CDMA_RSSI, &diag_db );
  rsp_ptr->stat.rssi = diag_db.cdma_rssi;

  db_get ( DB_POWER, &diag_db );
  rsp_ptr->stat.power = diag_db.power;

  return (rsp_ptr);

} /* mclog_stat */  /*lint !e715 */

/*===========================================================================
FUNCTION MCLOG_MARKOV

DESCRIPTION
  This returns Markov mode statistics.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/

PACK (void *) mclog_markov
(
  PACK (void *)req_pkt,      /* pointer to request packet  */
  word   len                 /* length of request packet   */
    /*lint -esym(715,req_pkt,len) */

)
{
  word i;                        /* Loop index */
  word mode;                     /* rxc mode   */
  static mar_stat_type mar;      /* buffer for Markov statistics */
  diag_rsp_type   *rsp_ptr;
  const int rsp_len = FSIZ(diag_rsp_type, markov);

  rsp_ptr = (diag_rsp_type *) diagbuf_pkt_alloc(rsp_len);

  /* if rsp_ptr is NULL return from this function */
  if( rsp_ptr == NULL)
  {
      M1X_MSG( DCP, LEGACY_HIGH,
        "Out of free DIAG Pkts");
      return NULL;
  }

  /*----------------------------------------------------------------------
    Retreive bytes from mar and rx modules.  Use mar_get_stat to fill
    in the mar data buffer.
  -----------------------------------------------------------------------*/
  rsp_ptr->markov.cmd_code = DIAG_MARKOV_F;

  mode = rxc_get_mode();

  switch (mode)
  {
    case CAI_SO_NULL:
      rsp_ptr->markov.mode = DIAG_SO_NONE;
      break;

    case CAI_SO_VOICE_IS96A:
      rsp_ptr->markov.mode = DIAG_SO_VOICE_96A;
      break;

    case CAI_SO_LOOPBACK:
      rsp_ptr->markov.mode = DIAG_SO_LOOPBACK;
      break;

    case CAI_SO_MARKOV:
      rsp_ptr->markov.mode = DIAG_SO_MARKOV;
      break;

    case CAI_SO_VOICE_13K:
      rsp_ptr->markov.mode = DIAG_SO_VOICE13;
      break;

    case CAI_SO_MARKOV_13K:
      rsp_ptr->markov.mode = DIAG_SO_MARKOV_13K;
      break;

    case CAI_SO_MARKOV_SO54:
      rsp_ptr->markov.mode = DIAG_SO_MARKOV_SO54;
      break;

    case CAI_SO_LOOPBACK_SO55:
      rsp_ptr->markov.mode = DIAG_SO_LOOPBACK_SO55;
      break;

    case CAI_SO_RS1_MARKOV:
      rsp_ptr->markov.mode = DIAG_SO_RS1_MARKOV;
      break;

    case CAI_SO_RS2_MARKOV:
      rsp_ptr->markov.mode = DIAG_SO_RS2_MARKOV;
      break;

    case CAI_SO_LOOPBACK_13K:
      rsp_ptr->markov.mode = DIAG_SO_LOOPBACK_13K;
      break;

    case CAI_SO_VOICE_EVRC:
      rsp_ptr->markov.mode = DIAG_SO_EVRC;
      break;

    case CAI_SO_VOICE_4GV_NB:
      rsp_ptr->markov.mode = CAI_SO_VOICE_4GV_NB;
      break;
    case CAI_SO_VOICE_4GV_WB:
      rsp_ptr->markov.mode = CAI_SO_VOICE_4GV_WB;
      break;


    case CAI_SO_VOICE_13K_IS733:
      rsp_ptr->markov.mode = DIAG_SO_VOICE_13K_IS733;
      break;

    default:
      #ifdef FEATURE_1X_CUST_VOICE_SO
      /* This workaround has been added to handle NV dependent non-constant
       * SO while ensuring the existing switch construct is not disrupted */        
      if (mode == mcc_get_cust_so() && mode != CAI_SO_NULL)
      {
        rsp_ptr->markov.mode = DIAG_SO_CUST_ENC_VOICE;
        break;
      }        
      #endif /* FEATURE_1X_CUST_VOICE_SO */  
      switch(mode)
      {
        /* DS_CASE_DATA is a macro containing several cases.
           It is defined in ds.h. */
        DS_CASE_DATA
           /* all the data service options */
          rsp_ptr->markov.mode = DIAG_SO_DATA;
          break;

        default:
          rsp_ptr->markov.mode = DIAG_SO_NONE;
          break;
      }

      break;
  }

  mar_get_stat( &mar );

  rsp_ptr->markov.markov_rate = mar.m_rate; /*lint !e641 */

  /* rxc_get_state returns an enum type which uses no negative values */
  rsp_ptr->markov.rx_state = (word) rxc_get_state();
  rxc_get_pc_crc ( &rx_pc_crc_cnt );
  if( (rsp_ptr->markov.rx_state == RXC_PC_STATE ) && /*lint !e641 */
        mclog_good_frames( &rx_pc_crc_cnt, &pc_crc_cnt ) == 0 ) {
    rsp_ptr->markov.rx_state |= 0x8000;
  }

  /*------------------------------------------------------------------------
    Update frame information.
  -------------------------------------------------------------------------*/
  rsp_ptr->markov.total_frames = mar.total_frames;
  rsp_ptr->markov.bad_frames = mar.bad_frames;
  for( i=0; i< 4; i++ ) {
    rsp_ptr->markov.good_errs[i] = (word)(mar.good_errs[i]);
  }

  for( i=0; i< 10; i++ ) {
    rsp_ptr->markov.data[0][i] = (word)(mar.mar_test[0][i]);
    rsp_ptr->markov.data[1][i] = (word)(mar.mar_test[1][i]);
    rsp_ptr->markov.data[2][i] = (word)(mar.mar_test[2][i]);
    rsp_ptr->markov.data[3][i] = (word)(mar.mar_test[3][i]);
  }

  rsp_ptr->markov.bit_errs = mar.bit_errs;

  return (rsp_ptr);

} /* mclog_markov */

/*===========================================================================
FUNCTION MCLOG_MARKOV_RESET

DESCRIPTION
  This procedure resets the markov statistics.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/

PACK (void *) mclog_markov_reset
(
  PACK (void *)req_pkt,      /* pointer to request packet  */
  word len                   /* length of request packet   */
)
{
  diag_rsp_type   *rsp_ptr;
  const int rsp_len = FSIZ(diag_rsp_type, m_reset);

  /*-----------------------------------------------------------------------
    Reset the statistics and bark out a response packet.
  -----------------------------------------------------------------------*/
  rxc_markov_reset();

  rsp_ptr = (diag_rsp_type *) diagbuf_pkt_alloc(rsp_len);

  /* if rsp_ptr is NULL return from this function */
  if( rsp_ptr == NULL)
  {
      M1X_MSG( DCP, LEGACY_HIGH,
        "Out of free DIAG Pkts");
      return NULL;
  }

  rsp_ptr->m_reset.cmd_code = DIAG_MARKOV_RESET_F;

  return (rsp_ptr);

} /* mclog_markov_reset */  /*lint !e715 */

/*===========================================================================

FUNCTION mclog_get_is2k_status

  This function sends IS2000 related channel information to DM via diag service

DEPENDENCIES
  None

RETURN VALUE
  The next state.

SIDE EFFECTS
  None

===========================================================================*/

/*lint -esym(715, req_pkt) */
PACK (void *) mclog_get_is2k_status
(
  PACK (void *)req_pkt,      /* pointer to request packet  */
  word len                   /* length of request packet   */
)
{
  mcc_status_type entry; /* CDMA and Analog last entry reason */
  boolean           digital;
  byte fsch_rate;
  mclog_aset_rsch_type rsch;

  DIAG_IS2000_STATUS_F_rsp_type *rsp_ptr;
  const int rsp_len = sizeof(DIAG_IS2000_STATUS_F_rsp_type);

  rsp_ptr = (DIAG_IS2000_STATUS_F_rsp_type *) diagbuf_pkt_alloc(rsp_len);

  /* if rsp_ptr is NULL return from this function */
  if( rsp_ptr == NULL)
  {
      M1X_MSG( DCP, LEGACY_HIGH,
        "Out of free DIAG Pkts");
      return NULL;
  }

  diagpkt_set_cmd_code(rsp_ptr, DIAG_IS2000_STATUS_F);

  db_get ( DB_RF_MODE, &diag_db );
  rsp_ptr->rf_mode = diag_db.rf_mode; /*lint !e641 */
  rsp_ptr->band_class = mc_get_band_class();

  switch( rsp_ptr->cdma_rx_state = (word) rxc_get_state() )
  {
    case RXC_SC_STATE:
      rxc_get_sc_crc ( &rx_sc_crc_cnt );
      rsp_ptr->cdma_good_frames =
            mclog_good_frames( &rx_sc_crc_cnt, &sc_crc_cnt );
      break;

    case RXC_PC_STATE:
      rxc_get_pc_crc ( &rx_pc_crc_cnt );
      rsp_ptr->cdma_good_frames =
            mclog_good_frames( &rx_pc_crc_cnt, &pc_crc_cnt );
      break;

    default:
      rsp_ptr->cdma_good_frames = -1;
  }

  rsp_ptr->analog_corrected_frames = 0;
  rsp_ptr->analog_bad_frames = 0;
  rsp_ptr->analog_word_syncs = 0;

  digital = ( rsp_ptr->rf_mode == DB_RF_ANALOG ) ? FALSE : TRUE; /*lint !e641 */

  /* Now that we know mode,  get the last entry reason */

  mc_get_entry(&entry);

  if ( digital ) {
    rsp_ptr->entry_reason = entry.cdma; /*lint !e641 */
  }

  db_get ( DB_CHANNEL, &diag_db );
  rsp_ptr->curr_chan = diag_db.channel;

  db_get ( DB_SID, &diag_db );
  rsp_ptr->sid = diag_db.sid;

  db_get ( DB_NID, &diag_db );
  rsp_ptr->nid = diag_db.nid;

  db_get ( DB_LOCAID, &diag_db );
  rsp_ptr->locaid = diag_db.locaid;

  db_get ( DB_CDMA_RSSI, &diag_db );
  rsp_ptr->rssi = diag_db.cdma_rssi;

  db_get ( DB_POWER, &diag_db );
  rsp_ptr->power = diag_db.power;

  db_get ( DB_BS_P_REV, &diag_db );
  rsp_ptr->bs_p_rev = diag_db.bs_p_rev;

  // The following status is applicable only if on traffic channel
  if ( (cdma.curr_state & 0xff00) == CDMA_TC )
  {
    const caii_srv_cfg_type  *config = snm_get_current_config();

    rsp_ptr->num_pilots = mccccl_code_channel_list.get_aset_info
                    (rsp_ptr->aset, &fsch_rate, &rsch) ;
    rsp_ptr->fsch_rate = fsch_rate;
    rsp_ptr->rsch=rsch;

    if (mccap_item_is_supported(MCCAP_DF_CHS))
    {
      mccccl_code_channel_list.get_control_hold_info(&(rsp_ptr->control_hold));
    }
    else
    {
      rsp_ptr->control_hold.enable = FALSE;
    }

    // Now all the RCs
    // Note: FCH/DCCH are always on all pilots
    if ( rsp_ptr->aset[0].fch.enable )
    {
      rsp_ptr->fch.for_rc = config->for_fch_rc;
      rsp_ptr->fch.rev_rc = config->rev_fch_rc;
    }

    if ( rsp_ptr->aset[0].dcch.enable )
    {
      rsp_ptr->dcch.for_rc = config->for_dcch_rc;
      rsp_ptr->dcch.rev_rc = config->rev_dcch_rc;
    }

    if ( mccsch_sch_is_active( MCCSCH_FORWARD, 0 ) )
    {
      rsp_ptr->sch.for_rc = config->for_sch[0].sch_chn_cfg.sch_rc;
    }
    if ( rsp_ptr->rsch.active )
    {
      rsp_ptr->sch.rev_rc = config->rev_sch[0].sch_chn_cfg.sch_rc;
    }
  }

  return rsp_ptr;

} /* mclog_get_is2k_status */
/*lint +esym(715, req_pkt) */

/*===========================================================================

FUNCTION MCLOG_REPORT_EVENT_OP_MODE

DESCRIPTION
   Updates internal data and report events if necessary when operation mode
   changes.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mclog_report_event_op_mode
(
  mclog_event_op_mode_type new_op_mode
)
{
  /* Update internal data and report event if operation mode changes */
  if (new_op_mode != mclog_event.curr_op_mode)
  {
    mclog_event.curr_op_mode = new_op_mode;
    event_report_payload(EVENT_OP_MODE_CHANGE, sizeof(event_op_mode_change_type)
      , &new_op_mode);

    /* Initialize paging channel acquisition flag */
    mclog_report_event_pch_acquired(FALSE, 0, 0);

  }
} /* mclog_report_event_op_mode */

/*===========================================================================

REDFUNCTION MCLOG_REPORT_EVENT_MSG_RXED

DESCRIPTION
  Report event Message received.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mclog_report_event_msg_rxed
(
  cai_rx_channel_id_type channel_id, /* Channel ID */
  byte *msg_ptr /* Pointer to message received in external format */
)
{
  event_message_type event_msg_rxed;
  event_msg_rxed.channel = (uint8) channel_id;

  switch (channel_id)
  {
    case CAI_SYNCH:
    case CAI_PCH:
    case CAI_BCCH:
    case CAI_F_CCCH:
      /* Msg id is first 6 bits of message in external format */
      event_msg_rxed.msg_id = *msg_ptr & 0x3F;
      break;

    case CAI_F_FCH:
    case CAI_F_DCCH:
      /* Msg id is first 8 bits of message in external format */
      event_msg_rxed.msg_id = *msg_ptr;
      break;

   case CAI_F_CACH:
   default:
     M1X_MSG( DCP, LEGACY_ERROR,
       "Msg on unsupported channel?");
     event_msg_rxed.msg_id = 0xFF;  /* Unsupported channel */
     break;
  }
  event_report_payload(EVENT_MESSAGE_RECEIVED,
    sizeof(event_message_type), (void *) &event_msg_rxed);

} /* mclog_report_event_msg_rxed */

/*===========================================================================

REDFUNCTION MCLOG_REPORT_EVENT_MSG_TXED

DESCRIPTION
  Report event Message transmitted.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mclog_report_event_msg_txed
(
  cai_tx_channel_id_type channel_id, /* Channel ID */
  byte *msg_ptr /* Message transmitted in external format */
)
{
  event_message_type event_msg_txed;
  event_msg_txed.channel = (uint8) channel_id;

  switch (channel_id)
  {
    case CAI_ACH:
    case CAI_EACH:
      /* Msg id is the lsb 6 bits of the second msg byte in external format */
      event_msg_txed.msg_id = *(++msg_ptr) & 0x3F;
      break;

    case CAI_R_FCH:
    case CAI_R_DCCH:
      /* Msg id is the second byte of message in external format */
      event_msg_txed.msg_id = *(++msg_ptr);
      break;

    case CAI_R_CCCH:
    default:
      M1X_MSG( DCP, LEGACY_ERROR,
        "Msg on unsupported channel?");
      event_msg_txed.msg_id = 0xFF;  /* Unsupported channel */
      break;
  }

  event_report_payload(EVENT_MESSAGE_TRANSMITTED,
    sizeof(event_message_type), (void *) &event_msg_txed);

} /* mclog_report_event_msg_txed */

/*===========================================================================

REDFUNCTION MCLOG_REPORT_EVENT_TIMER

DESCRIPTION
  Report event Timers T1m, T5m, T20m, T21m, T30m, T72m, T40m, T41m, T42m,
  T50m, T51m, T52m, T53m, T55m, T57m, T64m, T65m or T68m expired.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mclog_report_event_timer
(
  cai_timer_id_type timer_id /* Timer ID */
)
{
  event_report_payload(EVENT_TIMER_EXPIRED, sizeof(event_timer_expired_type),
    &timer_id);
    
  #if defined(FEATURE_1XCP_QSH_SUPPORT) &&  \
      defined(FEATURE_QSH_EVENT_NOTIFY_TO_QSH)
  /* Log an QSH Event based on Timer_id expiry */
  switch (timer_id)
  {   
    case CAI_TIMER_T5M:
    {
      QSH_LOG(QSH_CLT_ONEXL3, QSH_CAT_EVENT, QSH_MSG_TYPE_HIGH,
              "QSH:T5M-5sec Fade Timer Expired");
      mc_send_qsh_event(ONEXL3_QSH_EVENT_L3_RLF);              
      break;
    } /* CAI_TIMER_T5M */
    
    case CAI_TIMER_T20M:
    {
      QSH_LOG(QSH_CLT_ONEXL3, QSH_CAT_EVENT, QSH_MSG_TYPE_HIGH,
              "QSH:T20M-15sec Pilot Acq Timer Expired");   
      mc_send_qsh_event(ONEXL3_QSH_EVENT_L3_OOS);                
      break;
    } /* CAI_TIMER_T20M */
    
    case CAI_TIMER_T21M:
    {
      QSH_LOG(QSH_CLT_ONEXL3, QSH_CAT_EVENT, QSH_MSG_TYPE_HIGH,
              "QSH:T21M-1sec Sync Acq Timer Expired");   
      mc_send_qsh_event(ONEXL3_QSH_EVENT_L3_OOS);      
      break;
    } /* CAI_TIMER_T21M */
    
    case CAI_TIMER_T30M:
    {
      QSH_LOG(QSH_CLT_ONEXL3, QSH_CAT_EVENT, QSH_MSG_TYPE_HIGH,
              "QSH:T30M-3sec Timer Expired, Lost Paging ch in Idle");   
      mc_send_qsh_event(ONEXL3_QSH_EVENT_L3_OOS);     
      break;
    } /* CAI_TIMER_T30M */
        
    case CAI_TIMER_T40M:
    {
      QSH_LOG(QSH_CLT_ONEXL3, QSH_CAT_EVENT, QSH_MSG_TYPE_HIGH,
              "QSH:T40M-3sec Timer Expired, Lost Paging ch in Access");   
      mc_send_qsh_event(ONEXL3_QSH_EVENT_L3_ACCESS_FAIL);     
      break;
    } /* CAI_TIMER_T40M */
    
    case CAI_TIMER_T41M:
    {
      QSH_LOG(QSH_CLT_ONEXL3, QSH_CAT_EVENT, QSH_MSG_TYPE_HIGH,
              "QSH:T41M-4sec Update Ovhd Timer Expired");   
      mc_send_qsh_event(ONEXL3_QSH_EVENT_L3_OOS);      
      break;
    } /* CAI_TIMER_T41M */
    
    case CAI_TIMER_T42M:
    {
      QSH_LOG(QSH_CLT_ONEXL3, QSH_CAT_EVENT, QSH_MSG_TYPE_HIGH,
              "QSH:T42M-12sec ECAM Timer Expired");   
      mc_send_qsh_event(ONEXL3_QSH_EVENT_L3_ACCESS_FAIL);
      break;
    } /* CAI_TIMER_T42M */
    
    case CAI_TIMER_T50M:
    {
      QSH_LOG(QSH_CLT_ONEXL3, QSH_CAT_EVENT, QSH_MSG_TYPE_HIGH,
              "QSH:T50M-1sec TCI Valid Frame Timer Expired");   
      mc_send_qsh_event(ONEXL3_QSH_EVENT_L3_RLF);    
      break;
    } /* CAI_TIMER_T50M */
    
    case CAI_TIMER_T51M:
    {
      QSH_LOG(QSH_CLT_ONEXL3, QSH_CAT_EVENT, QSH_MSG_TYPE_HIGH,
              "QSH:T51M-2sec TCI BS Ack Timer Expired");   
      mc_send_qsh_event(ONEXL3_QSH_EVENT_L3_RLF);     
      break;
    } /* CAI_TIMER_T51M */
    
    case CAI_TIMER_T52M:
    {
      QSH_LOG(QSH_CLT_ONEXL3, QSH_CAT_EVENT, QSH_MSG_TYPE_HIGH,
              "QSH:T52M-5sec WFO Substate Timer Expired");   
      mc_send_qsh_event(ONEXL3_QSH_EVENT_L3_RLF);    
      break;
    } /* CAI_TIMER_T52M */
    
    case CAI_TIMER_T53M:
    {
      QSH_LOG(QSH_CLT_ONEXL3, QSH_CAT_EVENT, QSH_MSG_TYPE_HIGH,
              "QSH:T53M-65sec WFA Substate Timer Expired");   
      mc_send_qsh_event(ONEXL3_QSH_EVENT_L3_RLF);    
      break;
    } /* CAI_TIMER_T53M */
    
    case CAI_TIMER_T55M:
    {
      QSH_LOG(QSH_CLT_ONEXL3, QSH_CAT_EVENT, QSH_MSG_TYPE_HIGH,
              "QSH:T55M-2sec Release State Timer Expired");   
      mc_send_qsh_event(ONEXL3_QSH_EVENT_L3_RLF);    
      break;
    } /* CAI_TIMER_T55M */
    
    case CAI_TIMER_T57M:
    {
      QSH_LOG(QSH_CLT_ONEXL3, QSH_CAT_EVENT, QSH_MSG_TYPE_HIGH,
              "QSH:T57M-20sec Power-up Reg Timer Expired");         
      break;
    } /* CAI_TIMER_T57M */
 
    case CAI_TIMER_T64M:
    {
      QSH_LOG(QSH_CLT_ONEXL3, QSH_CAT_EVENT, QSH_MSG_TYPE_HIGH,
              "QSH:T64M-10sec SSD Update Timer Expired");          
      break;
    } /* CAI_TIMER_T64M */
    
    case CAI_TIMER_T65M:
    {
      QSH_LOG(QSH_CLT_ONEXL3, QSH_CAT_EVENT, QSH_MSG_TYPE_HIGH,
              "QSH:T65M-5sec Wait for Service Connect Msg Timer Expired");   
      mc_send_qsh_event(ONEXL3_QSH_EVENT_L3_RLF);    
      break;
    } /* CAI_TIMER_T65M */
    
    case CAI_TIMER_T68M:
    {
      QSH_LOG(QSH_CLT_ONEXL3, QSH_CAT_EVENT, QSH_MSG_TYPE_HIGH,
       "QSH:T68M-5sec Wait for Service Req/Resp/Connect Msg Timer Expired");   
      mc_send_qsh_event(ONEXL3_QSH_EVENT_L3_RLF);     
      break;
    } /* CAI_TIMER_T68M */
    
    case CAI_TIMER_T72M:
    {
      QSH_LOG(QSH_CLT_ONEXL3, QSH_CAT_EVENT, QSH_MSG_TYPE_HIGH,
              "QSH:T72M-1sec Timer Expired");    
      break;
    } /* CAI_TIMER_T72M */
    
    default:
    {
      /* Do Nothing */
      break;
    } /* default */
  } /* end switch */
  #endif /* FEATURE_1XCP_QSH_SUPPORT && FEATURE_QSH_EVENT_NOTIFY_TO_QSH */
} /* mclog_report_event_timer */

/*===========================================================================

REDFUNCTION MCLOG_REPORT_EVENT_COUNTER

DESCRIPTION
  Report event Counters N1m, N14m, N15m, N2m, N3m, N5m, N11m threshold
  reached.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mclog_report_event_counter
(
  cai_counter_id_type counter_id /* Counter ID */
)
{
  event_report_payload(EVENT_COUNTER_THRESHOLD,
    sizeof(event_counter_threshold_type), &counter_id);
} /* mclog_report_event_counter */

/*===========================================================================

FUNCTION MCLOG_MAP_EVENT_CP_STATE

DESCRIPTION
  Convert CP state to event CP state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

mclog_event_cp_state_type mclog_map_event_cp_state
(
  word curr_state /* Current state */
)
{
  mclog_event_cp_state_type cp_state = CDMA_MAX_S;

  if (curr_state == CDMA_SYS_DET_S) /*lint !e641 */
  {
    cp_state = (mclog_event_cp_state_type) curr_state;
  }
  else
  {
    switch (MAIN_STATE(curr_state))
    {
      case CDMA_ENTER:
        cp_state = CDMA_SYS_DET_S;
        break;

      case CDMA_INIT:
        if (curr_state == CDMA_PIL_ACQ) /*lint !e641 */
        {
          cp_state = CDMA_PIL_ACQ_S;
        }
        else if ((curr_state == CDMA_SYNC_ACQ) || (curr_state == CDMA_SYNC_MSG)) /*lint !e641 */
        {
          cp_state = CDMA_SYNC_ACQ_S;
        }
        else
        {
          cp_state = CDMA_TIME_CHG_S;
        }
        break;

      case CDMA_IDLE:
        if (curr_state == CDMA_OVHD) /*lint !e641 */
        {
          cp_state = CDMA_UPDATE_OVH_S;
        }
        else
        {
          cp_state = CDMA_IDLE_S;
        }
        break;

      case CDMA_SYS_ACC:
        cp_state = (mclog_event_cp_state_type)
          (CDMA_UPDATE_OVH_S + curr_state - CDMA_SYS_ACC); /*lint !e641 */
        break;

      case CDMA_TC:
        #ifdef FEATURE_IS2000_REL_A
        cp_state = (mclog_event_cp_state_type) (CDMA_TCI_S + curr_state - /*lint !e641 */
                   CDMA_TC);
        #else
        if ( curr_state == CDMA_RELEASE)
        {
          cp_state = CDMA_RELEASE_S;
        }
        else
        {
          /* For WFO, WFA & CONV this function is not even called anymore */
          cp_state = (mclog_event_cp_state_type) (CDMA_TCI_S + curr_state -
                   CDMA_TC);
        }
        #endif /*FEATURE_IS2000_REL_A*/
        break;

      default:
        M1X_MSG( DCP, LEGACY_ERROR,
          "Unknown CP state");
        break;
    } /* end switch */
  } /* end if */

  return cp_state;

} /* end mclog_map_event_cp_state */

/*===========================================================================

FUNCTION MCLOG_REPORT_EVENT_CP_STATE

DESCRIPTION
  Report events Call processing state change, Slotted mode operation. and
  QPCH in use.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mclog_report_event_cp_state
(
  word new_state
)
{
  event_call_processing_state_change_type event_cp_state;
  boolean slotted_mode;

  boolean qpch_in_use;

  /* CP state changes */
  if (new_state != mclog_event.curr_state)
  {
    /* Event report: Call processing state change */
    event_cp_state.old_state =
      (uint8) mclog_map_event_cp_state(mclog_event.curr_state);
    event_cp_state.new_state = (uint8) mclog_map_event_cp_state(new_state);

    if (event_cp_state.old_state != event_cp_state.new_state)
    {
      #ifdef FEATURE_IS2000_REL_A
      event_report_payload(EVENT_HIGH_LEVEL_CALL_PROCESSING_STATE_CHANGE,
        sizeof(event_call_processing_state_change_type),
        (void *) &event_cp_state);
      #else
      if (new_state != CDMA_TRAFFIC)
      {
        if (mclog_event.curr_state == CDMA_TRAFFIC)
        {
          /* This is the high level CP state but we need the current CC
          state to know which state in TC MS was in */
          event_cp_state.old_state = mclog_event.tc_substate;
        }
        /* These are reported by Call control report event function,
           so skip it here */
        event_report_payload(EVENT_CALL_PROCESSING_STATE_CHANGE,
          sizeof(event_call_processing_state_change_type),
          (void *) &event_cp_state);
      }
      #endif

    }

    /* Enter slotted mode of operation */
    if (new_state == CDMA_SLOTTED) /*lint !e641 */
    {
      /* Events report: Slotted mode operation */
      slotted_mode = TRUE;
      event_report_payload(EVENT_SLOTTED_MODE_OPERATION,
        sizeof(event_slotted_mode_operation_type), &slotted_mode);

      /* Events report: QPCH in use */
      if (mccap_item_is_supported(MCCAP_DF_QPCH))
      {
        qpch_in_use = FALSE;
      }
      else if (!cur_bs_ptr->csp.sp.ext_sys_parameter ||
               !P_REV_IS_6_OR_GREATER)
      {
        qpch_in_use = FALSE;
      }
      else
      {
        qpch_in_use = cur_bs_ptr->csp.esp.qpch_supported;
      }

      event_report_payload(EVENT_QPCH_IN_USE,
        sizeof(event_qpch_in_use_type), &qpch_in_use);

    }
    /* Event report: Slotted mode operation exit */
    else if (mclog_event.curr_state == CDMA_SLOTTED) /*lint !e641 */
    {
      slotted_mode = FALSE;
      event_report_payload(EVENT_SLOTTED_MODE_OPERATION,
        sizeof(event_slotted_mode_operation_type), &slotted_mode);
    }
  }

  /* Records current CP state */
  mclog_event.curr_state = new_state;

} /* mclog_report_event_cp_state */

#ifdef FEATURE_IS2000_REL_A
/*===========================================================================

FUNCTION MCLOG_MAP_EVENT_CC_STATE

DESCRIPTION
  Convert CP state to event call control state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

mclog_event_cc_state_type mclog_map_event_cc_state
(
  cdma_cc_state_type curr_state /* Current state */
)
{
  mclog_event_cc_state_type cc_state;

   /* The payload for this event is 4 bits, make sure we are not exceeding the max */
  if (CDMA_CC_MAX_S > MAX_NUM_CALL_CONTROL_SUBSTATES)  /*lint !e506 !e774 !e641 */
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Too many Call Control Substates" );
  }

  /*lint -e641*/
  cc_state = (mclog_event_cc_state_type) (CDMA_CC_WFO_S + (int) curr_state - CDMA_CC);
  /*lint +e641*/

  return cc_state;

} /* end mclog_map_event_cc_state */
#endif //FEATURE_IS2000_REL_A

/*===========================================================================

FUNCTION MCLOG_REPORT_EVENT_CC_CREATE

DESCRIPTION
  Reports Call Control instantiated event for Release A and a
  corresponding CP state change event for NON-Release A

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

/*lint -esym(715,con_ref) */
void mclog_report_event_cc_create
(
 uint8 con_ref,
 cdma_cc_state_type new_state
)
{
  #ifdef FEATURE_IS2000_REL_A
  mclog_event_cc_instantiated_type cc_event = {0};
  #else
  event_call_processing_state_change_type event_cp_state;
  #endif

  #ifdef FEATURE_IS2000_REL_A
  cc_event.con_ref = (int)con_ref;
  /*lint -e641*/
  cc_event.init_substate = (uint8) (CDMA_CC_WFO_S + (int) new_state - CDMA_CC) ;
  /*lint +e641*/

  event_report_payload(EVENT_CALL_CONTROL_INSTANTIATED,
    sizeof(mclog_event_cc_instantiated_type), (void *) &cc_event);
  #else
  /* If we are here, it means the old state is TC Init */
  event_cp_state.old_state = CDMA_TCI_S;
  event_cp_state.new_state = CDMA_WFO_S+(new_state-CDMA_CC_WFO); //lint !e734

  event_report_payload(EVENT_CALL_PROCESSING_STATE_CHANGE,
    sizeof(event_call_processing_state_change_type), (void *) &event_cp_state);
  #endif

} /* mclog_report_event_cc_create */
/*lint +esym(715,con_ref) */

/*===========================================================================

FUNCTION MCLOG_REPORT_EVENT_CC_STATE

DESCRIPTION
  Report event Call Control State Change

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

/*lint -esym(715, con_ref) */
void mclog_report_event_cc_state
(
 cdma_cc_state_type old_state,
 cdma_cc_state_type new_state,
 uint8 con_ref
)
{
  #ifdef FEATURE_IS2000_REL_A
  event_call_control_state_change_type event_cc_state={0,0,0};
  #else
  event_call_processing_state_change_type cp_state;
  #endif

  if (new_state != old_state)
  {
    #ifdef FEATURE_IS2000_REL_A
    event_cc_state.con_ref = (int)con_ref;
    /* Event report: Call Control State Change */
    event_cc_state.old_substate = (uint8) mclog_map_event_cc_state(old_state);
    event_cc_state.new_substate = (uint8) mclog_map_event_cc_state(new_state);

    event_report_payload(EVENT_CALL_CONTROL_STATE_CHANGE,
             sizeof(event_call_control_state_change_type),
             (void *) &event_cc_state);
    #else
    /* For non Release A targets report corresponding Call processing
    state change event */
    if (new_state == CDMA_CC_RELEASE)
    {
      /* No need to send this event here, The high level call processing
      evemnt reporting will take care of this one */
      return;
    }
    cp_state.new_state = (mclog_event_cp_state_type) (CDMA_WFO_S + new_state -
                                            CDMA_CC);
    cp_state.old_state = (mclog_event_cp_state_type) (CDMA_WFO_S + old_state -
                                            CDMA_CC);

    event_report_payload(EVENT_CALL_PROCESSING_STATE_CHANGE,
      sizeof(event_call_processing_state_change_type), (void *) &cp_state);

    #endif
  }

  #ifndef FEATURE_IS2000_REL_A
  mclog_event.tc_substate = (mclog_event_cp_state_type) (CDMA_WFO_S + new_state -
                                            CDMA_CC);
  #endif

} /* mclog_report_event_cc_state */
/*lint +esym(715, con_ref) */

/*===========================================================================

REDFUNCTION MCLOG_REPORT_EVENT_PCH_ACQUIRED

DESCRIPTION
  Report event paging channel acquired for the first time after power up or
  operation mode change.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mclog_report_event_pch_acquired
(
  boolean pagech_acq,
    /* Flag indicates whether MS acquires the paging channel */
  byte pagech,
    /* Paging channel number of the acquired PCH */
  word pn_offset
    /* PN offset of the BS whose paging channel is acquired */
)
{
  event_pch_acquired_type event_pch_acquired;

  if (pagech_acq)
  {
    /* -------------------------------------------------------------------
    ** MS acquires the paging channel for the first time after power up or
    ** operation mode change.
    ** ------------------------------------------------------------------- */
    if (!mclog_event.pagech_acq)
    {
      /* Events report: Paging channel acquired */
      event_pch_acquired.pagech = pagech;
      event_pch_acquired.pn_offset = pn_offset;
      event_report_payload(EVENT_PCH_ACQUIRED, sizeof(event_pch_acquired_type),
        (void *) &event_pch_acquired);

      /* Record paging channel acquisition */
      mclog_event.pagech_acq = pagech_acq;
    }
  }
  else
  {
    mclog_event.pagech_acq = pagech_acq;
  }

} /* mclog_report_event_pch_acquired */

/*===========================================================================

FUNCTION MCLOG_REPORT_EVENT_PAGE_MATCH

DESCRIPTION
  Report page match event

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void mclog_report_event_page_match( word service_option)
{
  event_page_match_type event_page_match;

  if(!cdma.specific_call_end_stats_supported)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Feature call end stats not supported");
    return;
  }

  event_page_match.service_option = service_option;

  if ( CAI_IS_SO_VOICE(service_option) )
  {
    event_page_match.call_type = SO_VOICE;
  }
  else if( CAI_IS_SO_SMS(service_option) )
  {
    event_page_match.call_type = SO_SMS;
  }
  else if( CAI_IS_SO_DATA(service_option) )
  {
    event_page_match.call_type = SO_DATA;
  }
  else if( CAI_IS_SO_SIGNALING(service_option) )
  {
    event_page_match.call_type = SO_SIGNALING;
  }
  else
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Unknown call type for SO:%d",
      service_option );
    return;
  }

  M1X_MSG( DCP, LEGACY_MED,
    "Sending diag EVENT_PAGE_MATCH with SO:%d, call_Type: %d",
    event_page_match.service_option,
    event_page_match.call_type);

  event_report_payload(EVENT_PAGE_MATCH,
                       sizeof(event_page_match_type), (void *) &event_page_match);

} /* mclog_report_event_page_match() */

#ifdef FEATURE_1X_TO_LTE
/*===========================================================================

FUNCTION MCLOG_REPORT_EVENT_1X_TO_LTE_RESEL_START

DESCRIPTION
  Report event 1X to LTE reselection is started.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void mclog_report_event_1x_to_lte_resel_start
(
  lte_earfcn_t earfcn_id, /* Frequency of the LTE cell to be selected */
  uint16 cell_id   /* Cell id of the LTE cell to be selected */
)
{
  mclog_event_1x_to_lte_resel_start_type resel_start;

  if(IS_1X_TO_LTE_RESELECTION_ENABLED)
  {
    resel_start.earfcn_id = earfcn_id;
    resel_start.cell_id = cell_id;

    event_report_payload(EVENT_1X_TO_LTE_RESEL_START,
                         sizeof(mclog_event_1x_to_lte_resel_start_type),
                         (void*) &resel_start);
  }
  else
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "1X to LTE reselections are disabled.");
  }
} /* mclog_report_event_1x_to_lte_resel_start() */

/*===========================================================================

FUNCTION MCLOG_REPORT_EVENT_1X_TO_LTE_RESEL_DONE

DESCRIPTION
  Report event 1X to LTE reselection is completed.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mclog_report_event_1x_to_lte_resel_done
(
  mccidl_1x_to_lte_resel_status_type resel_status
)
{
  mclog_event_1x_to_lte_resel_status_type resel_result = ONEX_TO_LTE_RESEL_ABORTED;

  if(IS_1X_TO_LTE_RESELECTION_ENABLED)
  {
    switch (resel_status)
    {
      case MCC_1X_TO_LTE_RESEL_ABORTED:
        resel_result = ONEX_TO_LTE_RESEL_ABORTED;
        break;

      case MCC_1X_TO_LTE_RESEL_FAILED:
        resel_result = ONEX_TO_LTE_RESEL_FAILED;
        break;

      case MCC_1X_TO_LTE_RESEL_SUCCESS:
        resel_result = ONEX_TO_LTE_RESEL_SUCCESS;
        break;

      default:
        ERR_FATAL("Invalid resel status",resel_status,0,0);
        break;
    }

    event_report_payload(EVENT_1X_TO_LTE_RESEL_DONE,
                         sizeof(mclog_event_1x_to_lte_resel_status_type),
                         (void*) &resel_result);

  }
  else
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "1X to LTE reselections are disabled.");
  }
}
#endif /* FEATURE_1X_TO_LTE */
/*===========================================================================

FUNCTION MCLOG_REPORT_EVENT_CSCH_HO

DESCRIPTION
  Report event idle handoff, access probe handoff and access handoff.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mclog_report_event_csch_ho
(
  cdma_ho_type ho_state,
  boolean cheap_idle_ho,
  word new_pilot,
  word new_freq,
  byte new_band_class,
  int8 ecio_old_pilot,
  int8 ecio_new_pilot,
  cdma_ho_type ho_type
)
{
  event_id_enum_type event_id = EVENT_MAX_ID;

  mclog_event_idl_acc_handoff_type idl_acc_ho_event;
    /* Structure for event reporting */

  /* Prepare idle/access event packet for reporting - EcIo from srch is in
  ** 1/2 db units so divide by 2 and set the fractional portion (.0 or .5)
  ** based on the lsb.
  */
  idl_acc_ho_event.new_pilot = new_pilot;
  idl_acc_ho_event.ecio_old_pilot = (ecio_old_pilot / 2);
  idl_acc_ho_event.ecio_old_pilot_frac = ((ecio_old_pilot & 0x01) ? 5 : 0);
  idl_acc_ho_event.ecio_new_pilot = (ecio_new_pilot / 2);
  idl_acc_ho_event.ecio_new_pilot_frac = ((ecio_new_pilot & 0x01) ? 5 : 0);
  idl_acc_ho_event.new_freq = new_freq;
  idl_acc_ho_event.new_band_class = new_band_class;

  if (ho_state == CDMA_IDLE_HO)
  {
    /* Record that we need to generate Idle Handoff Event */
    event_id = EVENT_IDLE_HANDOFF;
  }
  else if (ho_state == CDMA_AEHO)
  {
    /* Record that we need to generate Access Entry Handoff Event */
    event_id = EVENT_ACCESS_ENTRY_HANDOFF;
  }
  else if (ho_state == CDMA_AHO)
  {
    /* Record that we need to generate Access Handoff Event */
    event_id = EVENT_ACCESS_HANDOFF;
  }
  else if (ho_state == CDMA_APHO)
  {
    /* Record that we need to generate Access Probe Handoff Event */
    event_id = EVENT_ACCESS_PROBE_HANDOFF;
  }

  /* Check if we previously determined that an event is needed */
  if (event_id != EVENT_MAX_ID)
  {
    if (event_id == EVENT_IDLE_HANDOFF)
    {
      switch (ho_type)
      {
        case CDMA_RAHO:
          idl_acc_ho_event.idle_ho_type = MCLOG_IDLE_HO_REACQ; /*lint !e641 */
          break;

        case CDMA_FAST_RAHO:
          if ( cheap_idle_ho )
          {
            idl_acc_ho_event.idle_ho_type = MCLOG_IDLE_HO_FAST_REACQ_CHEAP; /*lint !e641 */
          }
          else
          {
            idl_acc_ho_event.idle_ho_type = MCLOG_IDLE_HO_FAST_REACQ_EXPENSIVE; /*lint !e641 */
          }
          break;

        case CDMA_IDLE_HO:
          idl_acc_ho_event.idle_ho_type = MCLOG_IDLE_HO; /*lint !e641 */
          break;

        default:
          idl_acc_ho_event.idle_ho_type = 0xFF;
          M1X_MSG( DCP, LEGACY_ERROR,
            "Invalid Idle HO type for event");
          break;
      }

      event_report_payload(event_id, sizeof(mclog_event_idl_acc_handoff_type),
        (void*) &idl_acc_ho_event);
    }
    else
    {
      /* Subtract off the last byte of the structure - AHO/AEHO/APHO events
         do not need to include field for the type of idle HO. */
      event_report_payload(event_id,
        sizeof(mclog_event_idl_acc_handoff_type)-sizeof(byte),
        (void*) &idl_acc_ho_event);
    }
  }

} /* mclog_report_event_csch_ho */

/*===========================================================================

REDFUNCTION MCLOG_REPORT_EVENT_CH_IND

DESCRIPTION
  Report event channel indicator change.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mclog_report_event_ch_ind
(
  cai_chind_type     ota_ch_ind,      /* Channel indicator */
  cai_ext_chind_type  ota_ext_ch_ind  /* Extended Channel indicator */
)
{
  struct
  {
    uint8 ch_ind;
    uint8 ext_ch_ind;

  } chind_info;

  /* OTA values for CH_IND is 3 bits max and for EXT_CH_IND is 5 bits max
  ** Casts of enums to uint8 make lint happy */
  chind_info.ch_ind = (uint8) ota_ch_ind;
  chind_info.ext_ch_ind = (uint8) ota_ext_ch_ind;

  event_report_payload(EVENT_CH_IND_CHANGE, sizeof(chind_info), &chind_info);

} /* mclog_report_event_ch_ind */

/*===========================================================================

REDFUNCTION MCLOG_REPORT_EVENT_F_FDCCH_ACQUIRED

DESCRIPTION
  Report event F-FCH acquired or F-DCCH acquired or F-FCH plus F-DCCH
  acquired.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mclog_report_event_f_fdcch_acquired
(
  void
)
{
  mclog_fch_dcch_acq_type acquire =
                           {{0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
                            {0,0,0,0,0,0}};
  mccdma_pilot_info_type  active_pilots[CAI_N6M];
  int  i, num_pilots;
  mcc_fwd_chan_mask_type fwd_chan_mask;

  /* What forward channels are currently active? */
  fwd_chan_mask = mccccl_code_channel_list.get_fwd_chan_mask();

  /* Get the active pilots */
  num_pilots = mccccl_code_channel_list.get_pilots(active_pilots);

  /* Copy the pilot_pn and pwr_comb_ind */
  for (i = 0; i < num_pilots; i++)
  {
    acquire.pn[i] = active_pilots[i].pilot_pn;
    switch (i)
    {
      case 0:
        break;

      case 1:
        acquire.sr.sr2 = active_pilots[i].pwr_comb_ind;
        break;

      case 2:
        acquire.sr.sr3 = active_pilots[i].pwr_comb_ind;
        break;

      case 3:
        acquire.sr.sr4 = active_pilots[i].pwr_comb_ind;
        break;

      case 4:
        acquire.sr.sr5 = active_pilots[i].pwr_comb_ind;
        break;

      case 5:
        acquire.sr.sr6 = active_pilots[i].pwr_comb_ind;
        break;

      default:
        break;
    } /* switch */
  } /* for */

  if ((fwd_chan_mask & MCC_F_FCH) && (fwd_chan_mask & MCC_F_DCCH))  // FCH and DCCH
  {
      event_report(EVENT_FFCH_PLUS_DCCH_ACQUIRED);
  }
  else if (fwd_chan_mask & MCC_F_FCH)                                // FCH only
  {
      event_report_payload(EVENT_FFCH_ACQUIRED,
                           sizeof(mclog_fch_dcch_acq_type),
                           (void *)&acquire);
  } else if (fwd_chan_mask & MCC_F_DCCH)                             // DCCH only
  {
    event_report_payload(EVENT_FDCCH_ACQUIRED,
                         sizeof(mclog_fch_dcch_acq_type),
                         (void *)&acquire);
  }
} /* mclog_report_event_f_fdcch_acquired */

/*===========================================================================

REDFUNCTION MCLOG_REPORT_EVENT_COMMON_CHANNEL_MONITORED

DESCRIPTION
  Report event common channel monitored.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mclog_report_event_common_channel_monitored
(
  byte active_channel_mask
)
{
  event_common_channel_monitored_type event_common_channel_monitored;

  event_common_channel_monitored.qpch_active         =
  event_common_channel_monitored.pch_active          = FALSE;

  if (active_channel_mask == MCC_PCH)
  {
    event_common_channel_monitored.pch_active = TRUE;
    if ((mccap_item_is_supported(MCCAP_DF_QPCH) ||
         mccap_is_enhanced_operation_supported()) &&
        !SP_MSG_NOT_RECVD && cur_bs_ptr->csp.sp.ext_sys_parameter &&
        !ESP_MSG_NOT_RECVD && cur_bs_ptr->csp.esp.p_rev >= P_REV_IS2000 &&
        cur_bs_ptr->csp.esp.qpch_supported)
    {
      event_common_channel_monitored.qpch_active = TRUE;
    }
  }

  event_report_payload(EVENT_COMMON_CHANNEL_MONITORED,
    sizeof(event_common_channel_monitored_type),
    (void *) &event_common_channel_monitored);

} /* end mclog_report_event_common_channel_monitored */

/*===========================================================================

FUNCTION MCLOG_REPORT_WAKEUP_SLOT

DESCRIPTION
  Report event of the wakeup slot

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mclog_report_wakeup_slot
(
  byte      mask,
  boolean   slept
)
{
  mclog_event_wakeup_slot_v2_type wakeup;

  wakeup.slot_mask = mask;
  wakeup.slept     = slept;

  event_report_payload(EVENT_SMS_SLOT_WAKEUP_V2,
                       sizeof(mclog_event_wakeup_slot_v2_type),
                       (void *) &wakeup);

} /* mclog_report_wakeup_slot */

/*===========================================================================

FUNCTION MCLOG_REPORT_MATCHED_MSG

DESCRIPTION
  Report the csch mobile addressed message, after it passes address matching
  and duplicate detection.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mclog_report_matched_msg
(
  byte  msg_type,
  mclog_match_type  match_type
)
{
  mclog_event_cp_matched_msg_type match_event;

  match_event.msg_type = msg_type;
  match_event.match_type = (byte) match_type;

  event_report_payload(EVENT_CP_MATCHED_MSG,
                       sizeof(mclog_event_cp_matched_msg_type),
                       (void *) &match_event);

} /* mclog_report_matched_msg */

/*===========================================================================

FUNCTION MCLOG_REPORT_RELEASE

DESCRIPTION
  Report the call release with release reason

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mclog_report_release
(
  mclog_release_reason_type reason
)
{
  mclog_event_release_type rel_reason;

  rel_reason = (mclog_event_release_type)reason;

  event_report_payload(EVENT_RELEASE,
                       sizeof(mclog_event_release_type),
                        (void *) &rel_reason);

} /* mclog_report_release */

/*===========================================================================

FUNCTION MCLOG_REPORT_REGISTRATION_SUPPRESSED

DESCRIPTION
  Report that a Registration Message was suppressed.  The payload is the
  type of Registration that was suppressed.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mclog_report_registration_suppressed
(
 byte registration_suppressed_type
)
{

  mclog_event_registration_suppressed_type reg_type = registration_suppressed_type;

  event_report_payload(EVENT_REGISTRATION_SUPPRESSED,
                       sizeof(mclog_event_registration_suppressed_type),
                       (void *) &reg_type);

} /* mclog_report_registration_suppressed */

/*===========================================================================

FUNCTION MCLOG_REPORT_EVENT_LONG_CODE_MASK

DESCRIPTION
  Report a change in the long code mask.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mclog_report_event_long_code_mask
(
 boolean always_report
 /* Normally, we check the new value against the old value to see if we
    need to report, but in the case of ECAM, we always need to report */
)
{
  mclog_event_long_code_mask_type event_lcm;

  if (always_report
      || (cdma.long_code.public_lcm_type != mclog_event.public_long_code_mask_type)

      || (cdma.long_code.private_lcm_active != mclog_event.using_private_lcm)

     )
  {

    event_lcm.using_private_lcm = cdma.long_code.private_lcm_active;


    event_lcm.public_long_code_mask_type = cdma.long_code.public_lcm_type; /*lint !e641 */

    event_report_payload(EVENT_LONG_CODE_MASK_CHANGED,
                         sizeof(mclog_event_long_code_mask_type),
                         (void *) &event_lcm);


    mclog_event.using_private_lcm = cdma.long_code.private_lcm_active;


    mclog_event.public_long_code_mask_type = cdma.long_code.public_lcm_type;
  }

} /* mclog_report_event_long_code_mask */

/*===========================================================================

FUNCTION MCLOG_MER_CONFIG
  This function enables or disables MER counter update. If MER counter update
  is enabled, update real/telecoping SCI page slot counters accordingly.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mclog_mer_config
(
  boolean enable_mer_stat,
  qword slot_sys_time
)
{
  word real_slot, real_slot_pos, real_sci_mask;

  if (enable_mer_stat)
  {
    /* Mark MER statistical operation enabled for the slot */
    mclog_mer.mer_stat_enabled = TRUE;

    /* Record system time of the slot */
    qw_equ(mclog_mer.slot_sys_time, slot_sys_time);

    real_sci_mask = mcc_calculate_sci_mask(mccidl_get_slot_cycle_index_s());

    real_slot_pos = ((word)(qw_lo(slot_sys_time)) >> 2) & real_sci_mask;
    real_slot = (mcc_get_idle_pgslot() & real_sci_mask);

    /* Real SCI page slot */
    if (real_slot_pos == real_slot)
    {
      mclog_mer.real_sci_page_slot_ctr++;
      M1X_MSG( DCP, LEGACY_MED,
        "Incremented REAL_SCI_PAGE_SLOT to %d, slot: 0x%x",
        mclog_mer.real_sci_page_slot_ctr,
        qw_lo(mclog_mer.slot_sys_time));
    }
    /* Telescoping SCI page slot */
    else
    {
      mclog_mer.tele_sci_page_slot_ctr++;
      M1X_MSG( DCP, LEGACY_MED,
        "Incremented TELE_SCI_PAGE_SLOT to %d, slot: 0x%x",
        mclog_mer.tele_sci_page_slot_ctr,
        qw_lo(mclog_mer.slot_sys_time));
    }
  }
  else
  {
    /* Mark MER statistical operation disabled for the slot */
    mclog_mer.mer_stat_enabled = FALSE;
    qw_set(mclog_mer.slot_sys_time, 0, 0);

    M1X_MSG( DCP, LEGACY_LOW,
      "MER stat off, slot: 0x%x",
      qw_lo(mclog_mer.slot_sys_time));
  }

} /* mclog_mer_config */

/*===========================================================================

FUNCTION MCLOG_MER_INIT
  This function initializes the information fields for MER counter support.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mclog_mer_init
(
  void
)
{
  mclog_mer.page_rxed_real_sci_asgn_slot_ctr = 0;
  mclog_mer.page_rxed_real_sci_next_slot_ctr = 0;
  mclog_mer.page_rxed_tele_sci_asgn_slot_ctr = 0;
  mclog_mer.page_rxed_tele_sci_next_slot_ctr = 0;
  mclog_mer.real_sci_page_slot_ctr = 0;
  mclog_mer.tele_sci_page_slot_ctr = 0;
  mclog_mer_config(FALSE, 0);

} /* mclog_mer_init */

/*===========================================================================

FUNCTION MCLOG_MER_UPDATE_COUNTER
  This function updates MER counters.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mclog_mer_update_counter
(
  boolean in_tele, /* Indicates if it is telescoping sci or not */
  boolean in_slot, /* Indicates if it is in-slot or not */
  qword page_rxed_sys_time /* System time that Page rxed */
)
{
  qword time_diff = {0, 0};
  boolean page_rxed_in_asgn_slot = TRUE;

  /* Perform MER statistical operation if it is enabled for the slot */
  if (mclog_mer.mer_stat_enabled)
  {
    M1X_MSG( DCP, LEGACY_MED,
      "GPM rxed: 0x%x, slot: 0x%x",
      qw_lo(page_rxed_sys_time),
      qw_lo(mclog_mer.slot_sys_time));

    /* Page Msg received before slot boundary. */
    if (qw_cmp(page_rxed_sys_time, mclog_mer.slot_sys_time) == -1)
    {
      M1X_MSG( DCP, LEGACY_MED,
        "Ignore Page Msg rxed before slot boundary");
    }

    /* Page Msg received after slot boundary. */
    else
    {
      /* See if Page Msg is received in first 81ms of the slot. */
      qw_sub(time_diff, page_rxed_sys_time, mclog_mer.slot_sys_time);
      if (ONEX_CVT_Q2N(time_diff) > MCLOG_ONE_SLOT)
      {
        /* Page Msg received after first 81ms of the slot */
        page_rxed_in_asgn_slot = FALSE;
        M1X_MSG( DCP, LEGACY_MED,
          "TIME diff lo: 0x%x hi: 0x%x",
          qw_lo(time_diff),
          qw_hi(time_diff));
      }

      /* Real SCI */
      if (!in_tele)
      {
        /* In-slot and Page Msg received within first 81ms of the slot */
        if (in_slot && page_rxed_in_asgn_slot)
        {
          mclog_mer.page_rxed_real_sci_asgn_slot_ctr++;
          M1X_MSG( DCP, LEGACY_LOW,
            "Incremented RXED_REAL_SCI_ASGN_SLOT to %d",
            mclog_mer.page_rxed_real_sci_asgn_slot_ctr);
        }
        /* Next rather than in-slot or Page Msg received after first
           81ms of the slot */
        else
        {
          mclog_mer.page_rxed_real_sci_next_slot_ctr++;
          M1X_MSG( DCP, LEGACY_MED,
            "Incremented RXED_REAL_SCI_NEXT_SLOT to %d",
            mclog_mer.page_rxed_real_sci_next_slot_ctr);
        }
      }
      /* Telescoping SCI */
      else
      {
        /* In-slot and Page Msg recived within first 81ms of the slot */
        if (in_slot && page_rxed_in_asgn_slot)
        {
          mclog_mer.page_rxed_tele_sci_asgn_slot_ctr++;
          M1X_MSG( DCP, LEGACY_MED,
            "Incremented RXED_TELE_SCI_ASGN_SLOT to %d",
            mclog_mer.page_rxed_tele_sci_asgn_slot_ctr);
        }
        /* Next rather than in-slot or Page Msg received after first
           81ms of the slot */
        else
        {
          mclog_mer.page_rxed_tele_sci_next_slot_ctr++;
          M1X_MSG( DCP, LEGACY_MED,
            "Incremented RXED_TELE_SCI_NEXT_SLOT to %d",
            mclog_mer.page_rxed_tele_sci_next_slot_ctr);
        }
      }

      /* Mark MER statistical operation for the slot is done. */
      mclog_mer_config(FALSE, 0);
    }
  }

} /* mclog_mer_update_counter */

/*===========================================================================

FUNCTION MC_DIAG_INIT
  This function registers the subsystem dispatch command handler table with
  Diag and performs other mclog initialization.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mc_diag_init
(
  void
)
{
  DIAGPKT_DISPATCH_TABLE_REGISTER (DIAG_SUBSYS_CALLP1X, mc_diag_tbl); //lint !e717 !e641

  /* Initialize MER counter support */
  mclog_mer_init();

} /* mc_log_init */

/*===========================================================================

FUNCTION MC_DIAG_DF_ITEMS_QUERY_HANDLER
  This function handles the DF items query command from Diag.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/

PACK (void *) mc_diag_df_items_query_handler
(
  PACK (void *)req_ptr,
  uint16      pkt_len
)
{
  mc_diag_df_items_query_req_type    *dfq_req_ptr;                /* DF query request String */
  mc_diag_df_items_query_rsp_type    *dfq_rsp_ptr;                /* DF query response String */
  uint16                        rsp_len;

  rsp_len = sizeof (mc_diag_df_items_query_rsp_type);

  if (pkt_len != sizeof (mc_diag_df_items_query_req_type))
  {
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);
  }

  dfq_req_ptr = (mc_diag_df_items_query_req_type *) req_ptr;

  if (dfq_req_ptr == NULL)
  {
    return NULL;
  }

  dfq_rsp_ptr = (mc_diag_df_items_query_rsp_type *)
                  diagpkt_subsys_alloc (DIAG_SUBSYS_CALLP1X, /*lint !e641 */
                                        MC_DIAG_SUBSYS_DF_ITEMS_QUERY,
                                        rsp_len);

  if (dfq_rsp_ptr == NULL) {
    return NULL;
  }


  /* Discover the DF items supported by the mobile */
  dfq_rsp_ptr->status = mccap_df_query(dfq_rsp_ptr->str,sizeof(dfq_rsp_ptr->str));


  return dfq_rsp_ptr;

} /* mc_diag_df_items_query_handler */

/*===========================================================================

FUNCTION MC_DIAG_DF_ITEM_VERIFY_HANDLER
  This function handles the DF item verify command from DIAG

DEPENDENCIES
  None

RETURN VALUE
  List of DF items which change their value because of the verify command

SIDE EFFECTS
  None

===========================================================================*/

PACK (void *) mc_diag_df_item_verify_handler
(
  PACK (void *)req_ptr,
  uint16      pkt_len
)
{
  mc_diag_df_item_verify_rsp_type     *dfv_rsp_ptr;     /* DF verify response string */
  mc_diag_df_item_verify_req_type     *dfv_req_ptr;     /* DF verify request string */
  uint16                              rsp_len;

  rsp_len = sizeof (mc_diag_df_item_verify_rsp_type);

  if (pkt_len != sizeof (mc_diag_df_item_verify_req_type))
  {
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);
  }

  dfv_req_ptr = (mc_diag_df_item_verify_req_type *) req_ptr;

  if (dfv_req_ptr == NULL)
  {
    return NULL;
  }

  dfv_rsp_ptr = (mc_diag_df_item_verify_rsp_type *)
                  diagpkt_subsys_alloc (DIAG_SUBSYS_CALLP1X, /*lint !e641 */
                                        MC_DIAG_SUBSYS_DF_ITEM_VERIFY,
                                        rsp_len);

  if (dfv_rsp_ptr == NULL)
  {
    return NULL;
  }

  /* Verify the DF items */

  dfv_rsp_ptr->status = mccap_df_verify(dfv_req_ptr->str, dfv_rsp_ptr->str);


  return dfv_rsp_ptr;

} /* mc_diag_df_item_verify_handler */

/*===========================================================================

FUNCTION MC_DIAG_DF_ITEMS_WRITE_HANDLER
  This function handles the DF items write command from DIAG

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/

PACK (void *) mc_diag_df_items_write_handler
(
  PACK (void *)req_ptr,
  uint16      pkt_len
)
{
  mc_diag_df_items_write_rsp_type     *dfw_rsp_ptr;           /* DF write response string */
  mc_diag_df_items_write_req_type     *dfw_req_ptr;           /* DF write request string */
  uint16                              rsp_len;

  rsp_len = sizeof (mc_diag_df_items_write_rsp_type);

  M1X_MSG( DCP, LEGACY_MED,
    "pkt_len = %d",
    pkt_len);

  /* In case of a write command from QPST, the packet length is variable. But
   * it should exceed the size of the write request packet */
  if (pkt_len > sizeof (mc_diag_df_items_write_req_type))
  {
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);
  }

  dfw_req_ptr = (mc_diag_df_items_write_req_type *) req_ptr;

  if (dfw_req_ptr == NULL)
  {
    return NULL;
  }

  dfw_rsp_ptr = (mc_diag_df_items_write_rsp_type *)
                  diagpkt_subsys_alloc (DIAG_SUBSYS_CALLP1X, /*lint !e641 */
                                        MC_DIAG_SUBSYS_DF_ITEMS_WRITE,
                                        rsp_len);

  if (dfw_rsp_ptr == NULL)
  {
    return NULL;
  }


  /* Write the DF items to NV */
  dfw_rsp_ptr->status = mccap_df_write(dfw_req_ptr->str, pkt_len);


  return dfw_rsp_ptr;

} /* mc_diag_df_items_write_handler */

/*===========================================================================

FUNCTION mclog_diag_state_query_handler
  This function handles the MC state query command from Diag.

DEPENDENCIES
  None

RETURN VALUE
  Returns struct mclog_diag_state_query_rsp_type

SIDE EFFECTS
  None

===========================================================================*/

PACK (void *) mclog_diag_state_query_handler
(
  PACK (void *)req_ptr,
  uint16      pkt_len
)
{
  mclog_diag_state_query_rsp_type     *rsp_ptr;
  uint16                               rsp_len;

  if (pkt_len != sizeof (mclog_diag_state_query_req_type))
  {
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);
  }

  rsp_len = sizeof (mclog_diag_state_query_rsp_type);
  rsp_ptr = (mclog_diag_state_query_rsp_type *)
                  diagpkt_subsys_alloc (DIAG_SUBSYS_CALLP1X, /*lint !e641 */
                                        MC_DIAG_SUBSYS_STATE_QUERY,
                                        rsp_len);
  if (rsp_ptr == NULL)
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "Out of free DIAG Pkts");
    return NULL;
  }

  rsp_ptr->mc_state = (uint8) mc_state();
  rsp_ptr->event_count = (uint16) diag_get_event_cnt();
  rsp_ptr->num_cc=0;

  if (rsp_ptr->mc_state == DIAG_D_CONV_S)
  {
    /* We need to fill in CC info */
    mccccim_diag_get_state(rsp_ptr);
  }

  return rsp_ptr;

} /* mclog_diag_state_query_handler */

/*===========================================================================

FUNCTION mclog_diag_set_curr_pref_sci
  This function is used to set the current preferred slot cycle index

DEPENDENCIES
  None

RETURN VALUE
  Returns struct

SIDE EFFECTS
  None

===========================================================================*/

PACK (void *) mclog_diag_set_curr_pref_sci
(
  PACK (void *)req_ptr,
  uint16      pkt_len
)
{
  mc_diag_cur_pref_sci_rsp_type       *rsp_ptr;
  uint16                               rsp_len;

  if (pkt_len != sizeof (mc_diag_cur_pref_sci_req_type))
  {
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);
  }

  rsp_len = sizeof (mc_diag_cur_pref_sci_rsp_type);
  rsp_ptr = (mc_diag_cur_pref_sci_rsp_type *)
                  diagpkt_subsys_alloc (DIAG_SUBSYS_CALLP1X, /*lint !e641 */
                                        MC_DIAG_SUBSYS_SET_CUR_PREF_SCI,
                                        rsp_len);

  if (rsp_ptr == NULL)
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "Out of free DIAG Pkts");
    return NULL;
  }

  rsp_ptr->sci.curr_pref_sci =
            mcc_set_current_preferred_sci(
                   ((mc_diag_cur_pref_sci_req_type *) req_ptr)->sci.curr_pref_sci
                  );
  return rsp_ptr;

} /* mclog_diag_set_curr_pref_sci */

/*===========================================================================

FUNCTION mclog_diag_set_ovhd_refresh_timer

  This function is used to modify the ovhd fresh time i.e. the time after  
  which stored overheads are considered to be expired.
  This is purely needed for testing puposes particularly to test split  
  overheads feature.

DEPENDENCIES
  None

RETURN VALUE
  Returns struct

SIDE EFFECTS
  None

===========================================================================*/

PACK (void *) mclog_diag_set_ovhd_refresh_timer
(
  PACK (void *)req_ptr,
  uint16      pkt_len
)
{
  mc_diag_ovhd_refresh_time_rsp_type       *rsp_ptr;
  uint16                               rsp_len;

  if (pkt_len != sizeof (mc_diag_ovhd_refresh_time_req_type))
  {
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);
  }

  rsp_len = sizeof (mc_diag_ovhd_refresh_time_rsp_type);
  rsp_ptr = (mc_diag_ovhd_refresh_time_rsp_type *)
                  diagpkt_subsys_alloc (DIAG_SUBSYS_CALLP1X, /*lint !e641 */
                                        MC_DIAG_SUBSYS_SET_OVHD_REFRESH_TIME,
                                        rsp_len);

  if (rsp_ptr == NULL)
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "Out of free DIAG Pkts");
    return NULL;
  }

  M1X_MSG( DCP, LEGACY_HIGH,
    "Updating overhead refresh time to %d seconds via Diag",
    ((mc_diag_ovhd_refresh_time_req_type *) req_ptr)->refresh_time.ovhd_refresh_time );

  rsp_ptr->refresh_time.ovhd_refresh_time =
            mcc_set_ovhd_refresh_time(
                   ((mc_diag_ovhd_refresh_time_req_type *) req_ptr)->refresh_time.ovhd_refresh_time
                  );
  return rsp_ptr;

} /* mclog_diag_set_ovhd_refresh_timer */


/*===========================================================================

FUNCTION MC_DIAG_GET_NW_PARAMS
  This function handles the MC_DIAG_SUBSYS_GET_NW_PARAMS command from Diag.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to the response packet

SIDE EFFECTS
  None

===========================================================================*/

PACK (void *) mc_diag_get_nw_params
(
  PACK (void *)req_ptr,
  uint16      pkt_len
)
{
  mc_diag_get_nw_params_rsp_type       *rsp_ptr;
  uint16                               rsp_len;

  const caii_srv_cfg_type  *config = snm_get_current_config();

  if (pkt_len != sizeof (mc_diag_get_nw_params_req_type))
  {
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);
  }

  rsp_len = sizeof (mc_diag_get_nw_params_rsp_type);
  rsp_ptr = (mc_diag_get_nw_params_rsp_type *)
                  diagpkt_subsys_alloc (DIAG_SUBSYS_CALLP1X, /*lint !e641 */
                                        MC_DIAG_SUBSYS_GET_NW_PARAMS,
                                        rsp_len);

  if (rsp_ptr == NULL)
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "Out of free DIAG Pkts");
    return NULL;
  }

  rsp_ptr->params.inter_freq_idle_ho_count = mclog_inter_freq_idle_ho_count;
  rsp_ptr->params.hard_ho_count = mclog_hard_ho_count;

  if(!config->for_sch_cc_incl || (config->num_for_sch == 0))
  {
    M1X_MSG( DCP, LEGACY_MED,
      "Channel config for F-SCH0 is not available");
  }
  rsp_ptr->params.fwd_sch0_mux_option = config->for_sch[0].for_sch_mux;
  rsp_ptr->params.fwd_sch0_radio_config = config->for_sch[0].sch_chn_cfg.sch_rc;

  if(!config->rev_sch_cc_incl || (config->num_rev_sch == 0))
  {
    M1X_MSG( DCP, LEGACY_MED,
      "Channel config for R-SCH0 is not available");
  }
  rsp_ptr->params.rev_sch0_mux_option = config->rev_sch[0].rev_sch_mux;
  rsp_ptr->params.rev_sch0_radio_config = config->rev_sch[0].sch_chn_cfg.sch_rc;

  return rsp_ptr;

} /* mc_diag_get_nw_params */

/*===========================================================================

FUNCTION MCLOG_PAGING_MER_STAT
  This function handles the Paging MER Statistics command from Diag.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to the response packet

SIDE EFFECTS
  None

===========================================================================*/

PACK (void *)mclog_paging_mer_stat (PACK (void *) req_pkt, uint16 pkt_len)
{
  DIAG_SUBSYS_CALLP1X_MC_DIAG_SUBSYS_PAGING_MER_STAT_req_type *req_ptr;
  DIAG_SUBSYS_CALLP1X_MC_DIAG_SUBSYS_PAGING_MER_STAT_rsp_type *rsp_ptr;
  const int rsp_len = sizeof(DIAG_SUBSYS_CALLP1X_MC_DIAG_SUBSYS_PAGING_MER_STAT_rsp_type);

  req_ptr = (DIAG_SUBSYS_CALLP1X_MC_DIAG_SUBSYS_PAGING_MER_STAT_req_type *)req_pkt;

  if (pkt_len != sizeof (DIAG_SUBSYS_CALLP1X_MC_DIAG_SUBSYS_PAGING_MER_STAT_req_type))
  {
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);
  }

  /* Allocate the memory for response */
  rsp_ptr = (DIAG_SUBSYS_CALLP1X_MC_DIAG_SUBSYS_PAGING_MER_STAT_rsp_type *) diagpkt_subsys_alloc(DIAG_SUBSYS_CALLP1X, MC_DIAG_SUBSYS_PAGING_MER_STAT, /*lint !e641 */
                                 rsp_len );

  if (rsp_ptr != NULL)
  {
    if (req_ptr->counters_bitmask & MC_DIAG_PAG_RXED_REAL_SCI_ASGN_SLOT)
    {
      rsp_ptr->page_rxed_real_sci_asgn_slot_ctr = mclog_mer.page_rxed_real_sci_asgn_slot_ctr;
    }
    else
    {
      mclog_mer.page_rxed_real_sci_asgn_slot_ctr = 0;
      rsp_ptr->page_rxed_real_sci_asgn_slot_ctr = 0;
    }

    if (req_ptr->counters_bitmask & MC_DIAG_PAG_RXED_REAL_SCI_NEXT_SLOT)
    {
      rsp_ptr->page_rxed_real_sci_next_slot_ctr = mclog_mer.page_rxed_real_sci_next_slot_ctr;
    }
    else
    {
      rsp_ptr->page_rxed_real_sci_next_slot_ctr = 0;
      mclog_mer.page_rxed_real_sci_next_slot_ctr = 0;
    }

    if (req_ptr->counters_bitmask & MC_DIAG_PAG_RXED_TELE_SCI_ASGN_SLOT)
    {
      rsp_ptr->page_rxed_tele_sci_asgn_slot_ctr = mclog_mer.page_rxed_tele_sci_asgn_slot_ctr;
    }
    else
    {
      mclog_mer.page_rxed_tele_sci_asgn_slot_ctr = 0;
      rsp_ptr->page_rxed_tele_sci_asgn_slot_ctr = 0;
    }

    if (req_ptr->counters_bitmask & MC_DIAG_PAG_RXED_TELE_SCI_NEXT_SLOT)
    {
      rsp_ptr->page_rxed_tele_sci_next_slot_ctr = mclog_mer.page_rxed_tele_sci_next_slot_ctr;
    }
    else
    {
      rsp_ptr->page_rxed_tele_sci_next_slot_ctr = 0;
      mclog_mer.page_rxed_tele_sci_next_slot_ctr = 0;
    }

    if (req_ptr->counters_bitmask & MC_DIAG_REAL_SCI_PAGE_SLOT)
    {
      rsp_ptr->real_sci_page_slot_ctr = mclog_mer.real_sci_page_slot_ctr;
    }
    else
    {
      rsp_ptr->real_sci_page_slot_ctr = 0;
      mclog_mer.real_sci_page_slot_ctr = 0;
    }

    if (req_ptr->counters_bitmask & MC_DIAG_TELE_SCI_PAGE_SLOT)
    {
      rsp_ptr->tele_sci_page_slot_ctr = mclog_mer.tele_sci_page_slot_ctr;
    }
    else
    {
      rsp_ptr->tele_sci_page_slot_ctr = 0;
      mclog_mer.tele_sci_page_slot_ctr = 0;
    }
  }
  else
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Null pointer from diagpkt_subsys_alloc");
  }

  return rsp_ptr ;

} /* mclog_paging_mer_stat */

#ifdef FEATURE_MODEM_CONFIG_REFRESH 
/*===========================================================================

FUNCTION MCLOG_PAGING_MER_STAT
  This function handles the Reload NV Items command from Diag.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to the response packet

SIDE EFFECTS
  None

===========================================================================*/

PACK (void *)mc_diag_reload_nv_items
(
  PACK (void *)req_ptr,
  uint16      pkt_len
)
{
  mc_diag_reload_nv_items_rsp_type       *rsp_ptr;
  uint16                                  rsp_len;
  mc_msg_type                            *mc_cmd_ptr; 

  if (pkt_len != sizeof (mc_diag_reload_nv_items_req_type))
  {
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);
  }

  rsp_len = sizeof (mc_diag_reload_nv_items_rsp_type);
  rsp_ptr = (mc_diag_reload_nv_items_rsp_type *)
                  diagpkt_subsys_alloc (DIAG_SUBSYS_CALLP1X, /*lint !e641 */
                                        MC_DIAG_SUBSYS_RELOAD_NV_ITEMS,
                                        rsp_len);

  if (rsp_ptr == NULL)
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "Out of free DIAG Pkts");
    return NULL;
  }

  if (cdma.curr_state == CDMA_EXIT)
  {
    M1X_MSG( DCP, LEGACY_HIGH, "Reloading all NV items due to Diag cmd" );
    REX_ISR_LOCK(&mc_crit_sect);

    /* Send the NV refresh status to the MC */
    mc_cmd_ptr = (mc_msg_type*)mc_alloc_queue_buf( MC_CMD_BUF_Q );

    if(mc_cmd_ptr == NULL)
    {
      ERR_FATAL("Can't get MC Buffer",0,0,0);
    }
    else
    {
      mc_cmd_ptr->hdr.cmd = MC_NV_REFRESH_F;
      mc_cmd_ptr->nv_refresh.req_module = NV_REFRESH_DIAG;

      mc_cmd_ptr->hdr.cmd_hdr.task_ptr    = NULL;

      /* Initialize and link the command onto the MC command queue,
      * set a signal to the MC task and exit.
      */
      M1X_MSG( DCP, LEGACY_MED,
        "Send Cmd %d to MC",
        mc_cmd_ptr->hdr.cmd );
      mc_cmd(mc_cmd_ptr);
    }

    REX_ISR_UNLOCK(&mc_crit_sect);

    rsp_ptr->status = TRUE;
  }
  else
  {
    M1X_MSG( DCP, LEGACY_HIGH, 
      "Cannot process Diag cmd to reload NV items in state = %d", 
       cdma.curr_state );
    rsp_ptr->status = FALSE;
  }

  return rsp_ptr;
}/* mc_diag_reload_nv_items */
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

/*===========================================================================

FUNCTION MCLOG_INC_NW_PARAM
  This function increments of the value of the network paramater.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mclog_inc_nw_param(mc_diag_nw_param_type param_type)
{
  switch(param_type)
  {
    case INTER_FREQ_IDLE_HO_COUNT:
      if((++mclog_inter_freq_idle_ho_count) == 0)
      {
        M1X_MSG( DCP, LEGACY_MED,
          "mclog_inter_freq_idle_ho_count overflowed!");
      }
      break;

    case HARD_HO_COUNT:
      if((++mclog_hard_ho_count) == 0)
      {
        M1X_MSG( DCP, LEGACY_MED,
          "mclog_hard_ho_count overflowed!");
      }
      break;
    default:
      M1X_MSG( DCP, LEGACY_MED,
        "invalid parameter type %d",
        param_type);
  }
}

#ifdef FEATURE_IS2000_1X_ADV
/*===========================================================================

FUNCTION MC_DIAG_ENH_MARKOV_STAT
  This function handles the MC_DIAG_SUBSYS_ENH_MARKOV_STAT command from Diag.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to the response packet

SIDE EFFECTS
  None

===========================================================================*/

PACK (void *) mc_diag_enh_markov_stat
(
  PACK (void *)req_ptr,
  uint16      pkt_len
)
{
  mc_diag_enh_markov_stat_rsp_type       *rsp_ptr;
  uint16                                  rsp_len;

  word i;                        /* Loop index */
  word mode;                     /* rxc mode   */
  static mar_stat_type mar;      /* buffer for Markov statistics */

  if (pkt_len != sizeof (mc_diag_enh_markov_stat_req_type))
  {
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);
  }

  rsp_len = sizeof (mc_diag_enh_markov_stat_rsp_type);
  rsp_ptr = (mc_diag_enh_markov_stat_rsp_type *)
                  diagpkt_subsys_alloc (DIAG_SUBSYS_CALLP1X, /*lint !e641 */
                                        MC_DIAG_SUBSYS_ENH_MARKOV_STAT,
                                        rsp_len);

  if (rsp_ptr == NULL)
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "Out of free DIAG Pkts");
    return NULL;
  }

  /* Fill the stats */
  mode = rxc_get_mode();

  switch (mode)
  {
    case CAI_SO_NULL:
      rsp_ptr->stat.mode = DIAG_SO_NONE;
      break;

    case CAI_SO_VOICE_IS96A:
      rsp_ptr->stat.mode = DIAG_SO_VOICE_96A;
      break;

    case CAI_SO_LOOPBACK:
      rsp_ptr->stat.mode = DIAG_SO_LOOPBACK;
      break;

    case CAI_SO_MARKOV:
      rsp_ptr->stat.mode = DIAG_SO_MARKOV;
      break;

    case CAI_SO_VOICE_13K:
      rsp_ptr->stat.mode = DIAG_SO_VOICE13;
      break;

    case CAI_SO_MARKOV_13K:
      rsp_ptr->stat.mode = DIAG_SO_MARKOV_13K;
      break;

    case CAI_SO_MARKOV_SO54:
      rsp_ptr->stat.mode = DIAG_SO_MARKOV_SO54;
      break;

    case CAI_SO_MARKOV_SO74:
      rsp_ptr->stat.mode = DIAG_SO_MARKOV_SO74;
      break;

    case CAI_SO_LOOPBACK_SO55:
      rsp_ptr->stat.mode = DIAG_SO_LOOPBACK_SO55;
      break;

    case CAI_SO_RS1_MARKOV:
      rsp_ptr->stat.mode = DIAG_SO_RS1_MARKOV;
      break;

    case CAI_SO_RS2_MARKOV:
      rsp_ptr->stat.mode = DIAG_SO_RS2_MARKOV;
      break;

    case CAI_SO_LOOPBACK_13K:
      rsp_ptr->stat.mode = DIAG_SO_LOOPBACK_13K;
      break;

    case CAI_SO_VOICE_EVRC:
      rsp_ptr->stat.mode = DIAG_SO_EVRC;
      break;

    case CAI_SO_VOICE_4GV_NB:
      rsp_ptr->stat.mode = CAI_SO_VOICE_4GV_NB;
      break;
    case CAI_SO_VOICE_4GV_WB:
      rsp_ptr->stat.mode = CAI_SO_VOICE_4GV_WB;
      break;

    case CAI_SO_VOICE_13K_IS733:
      rsp_ptr->stat.mode = DIAG_SO_VOICE_13K_IS733;
      break;

    default:
      #ifdef FEATURE_1X_CUST_VOICE_SO
      /* This workaround has been added to handle NV dependent non-constant
       * SO while ensuring the existing switch construct is not disrupted */        
      if (mode == mcc_get_cust_so() && mode != CAI_SO_NULL)
      {
        rsp_ptr->stat.mode = DIAG_SO_CUST_ENC_VOICE;
        break;
      }       
      #endif /* FEATURE_1X_CUST_VOICE_SO */  
      switch(mode)
      {
        /* DS_CASE_DATA is a macro containing several cases.
           It is defined in ds.h. */
        DS_CASE_DATA
           /* all the data service options */
          rsp_ptr->stat.mode = DIAG_SO_DATA;
          break;

        default:
          rsp_ptr->stat.mode = DIAG_SO_NONE;
          break;
      }

      break;
  }

  mar_get_stat( &mar );

  rsp_ptr->stat.markov_rate = mar.m_rate; /*lint !e641 */

  /* rxc_get_state returns an enum type which uses no negative values */
  rsp_ptr->stat.rx_state = (word) rxc_get_state();
  rxc_get_pc_crc ( &rx_pc_crc_cnt );
  if( (rsp_ptr->stat.rx_state == RXC_PC_STATE ) && /*lint !e641 */
        mclog_good_frames( &rx_pc_crc_cnt, &pc_crc_cnt ) == 0 ) {
    rsp_ptr->stat.rx_state |= 0x8000;
  }

  /*------------------------------------------------------------------------
    Update frame information.
  -------------------------------------------------------------------------*/
  rsp_ptr->stat.total_frames = mar.total_frames;
  rsp_ptr->stat.bad_frames = mar.bad_frames;

  for( i=0; i<10; i++ ) {
    rsp_ptr->stat.data[0][i] = mar.mar_test[0][i];
    rsp_ptr->stat.data[1][i] = mar.mar_test[1][i];
    rsp_ptr->stat.data[2][i] = mar.mar_test[2][i];
    rsp_ptr->stat.data[3][i] = mar.mar_test[3][i];
    rsp_ptr->stat.data[4][i] = mar.mar_test[4][i];
  }

  rsp_ptr->stat.bit_errs = mar.bit_errs;

  for( i=0; i< 4; i++ ) {
    rsp_ptr->stat.good_errs[i] = mar.good_errs[i];
  }

  /* Statistics copied */

  return rsp_ptr;

} /* mc_diag_enh_markov_stat */
#endif

#ifdef FEATURE_MODEM_1X_MSO_RDA_TEST
/*===========================================================================

FUNCTION MC_DIAG_MAR_RDA_TEST_STAT
  This function handles the MC_DIAG_SUBSYS_MAR_RDA_TEST_STAT command from Diag.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to the response packet

SIDE EFFECTS
  None

===========================================================================*/

PACK (void *) mc_diag_mar_rda_test_stat
(
  PACK (void*) req_ptr,
  uint16      pkt_len
)
{
  mc_diag_mar_rda_test_stat_rsp_type      *rsp_ptr;
  uint16                                  rsp_len;

  word i;                        /* Loop index */
  word mode;                     /* rxc mode   */
  static mar_rda_test_stat_type stat;      /* buffer for Markov statistics */

  if (pkt_len != sizeof (mc_diag_mar_rda_test_stat_req_type))
  {
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);
  }

  rsp_len = sizeof (mc_diag_mar_rda_test_stat_rsp_type);
  rsp_ptr = (mc_diag_mar_rda_test_stat_rsp_type *)
                  diagpkt_subsys_alloc (DIAG_SUBSYS_CALLP1X, /*lint !e641 */
                                        MC_DIAG_SUBSYS_MAR_RDA_TEST_STAT,
                                        rsp_len);

  if (rsp_ptr == NULL)
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "Out of free DIAG Pkts");
    return NULL;
  }

  /* Fill the stats */
  mode = rxc_get_mode();

  switch (mode)
  {
    case CAI_SO_MARKOV:
      rsp_ptr->stat.mode = DIAG_SO_MARKOV;
      break;

    case CAI_SO_MARKOV_13K:
      rsp_ptr->stat.mode = DIAG_SO_MARKOV_13K;
      break;

    case CAI_SO_MARKOV_SO54:
      rsp_ptr->stat.mode = DIAG_SO_MARKOV_SO54;
      break;

    #ifdef FEATURE_IS2000_1X_ADV
    case CAI_SO_MARKOV_SO74:
      rsp_ptr->stat.mode = DIAG_SO_MARKOV_SO74;
      break;
    #endif

    case CAI_SO_RS1_MARKOV:
      rsp_ptr->stat.mode = DIAG_SO_RS1_MARKOV;
      break;

    case CAI_SO_RS2_MARKOV:
      rsp_ptr->stat.mode = DIAG_SO_RS2_MARKOV;
      break;

    default:
      rsp_ptr->stat.mode = DIAG_SO_NONE;
      break;
  }

  mar_get_rda_test_stat( &stat );

  rsp_ptr->stat.markov_rate = stat.m_rate; /*lint !e641 */

  /*------------------------------------------------------------------------
    Update frame information.
  -------------------------------------------------------------------------*/
  for( i=0; i<5; i++ )
  {
    rsp_ptr->stat.good_frames[i][0] = stat.mar_good_frames[i][0];
    rsp_ptr->stat.good_frames[i][1] = stat.mar_good_frames[i][1];
    rsp_ptr->stat.good_frames[i][2] = stat.mar_good_frames[i][2];
    rsp_ptr->stat.good_frames[i][3] = stat.mar_good_frames[i][3];
    rsp_ptr->stat.good_frames[i][4] = stat.mar_good_frames[i][4];
  }

  for( i=0; i<5; i++ )
  {
    rsp_ptr->stat.bad_frames[i][0] = stat.mar_bad_frames[i][0];
    rsp_ptr->stat.bad_frames[i][1] = stat.mar_bad_frames[i][1];
    rsp_ptr->stat.bad_frames[i][2] = stat.mar_bad_frames[i][2];
    rsp_ptr->stat.bad_frames[i][3] = stat.mar_bad_frames[i][3];
    rsp_ptr->stat.bad_frames[i][4] = stat.mar_bad_frames[i][4];
  }
  /* Statistics copied */

  return rsp_ptr;

} /* mc_diag_mar_rda_test_stat */
#endif /* FEATURE_MODEM_1X_MSO_RDA_TEST */

#ifdef FEATURE_1XCP_FTD

/*===========================================================================

FUNCTION MC_DIAG_GET_SYSTEM_PARAMETERS
  This function handles the MC_DIAG_SYS_PARAMS_QUERY command from Diag.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to the response packet

SIDE EFFECTS
  None

===========================================================================*/

PACK (void *) mc_diag_get_system_parameters
(
  PACK (void*) req_ptr,
  uint16      pkt_len
)
{
  mc_diag_get_system_params_rsp_type        *rsp_ptr;
  uint16                                  rsp_len;

  byte i;
  if (pkt_len != sizeof (mc_diag_get_system_params_req_type))
  {
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);
  }

  rsp_len = sizeof (mc_diag_get_system_params_rsp_type);
  rsp_ptr = (mc_diag_get_system_params_rsp_type *)
                  diagpkt_subsys_alloc (DIAG_SUBSYS_CALLP1X, /*lint !e641 */
                                        MC_DIAG_SYS_PARAMS_QUERY,
                                        rsp_len);

  if (rsp_ptr == NULL)
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "Out of free DIAG Pkts");
    return NULL;
  }

  rsp_ptr->version = 1;

  /* Fill in the SPM fields. */
  rsp_ptr->params.spm_included   = cur_bs_ptr->rx.sp_rx;
  if(rsp_ptr->params.spm_included)
  {
    rsp_ptr->params.spm.pilot_pn = cur_bs_ptr->csp.sp.pilot_pn;
    rsp_ptr->params.spm.config_msg_seq = cur_bs_ptr->csp.sp.config_msg_seq;
    rsp_ptr->params.spm.sid = cur_bs_ptr->csp.sp.sid;
    rsp_ptr->params.spm.nid = cur_bs_ptr->csp.sp.nid;
    rsp_ptr->params.spm.reg_zone = cur_bs_ptr->csp.sp.reg_zone;
    rsp_ptr->params.spm.total_zones = cur_bs_ptr->csp.sp.total_zones;
    rsp_ptr->params.spm.zone_timer = cur_bs_ptr->csp.sp.zone_timer;
    rsp_ptr->params.spm.mult_sids = cur_bs_ptr->csp.sp.mult_sids;
    rsp_ptr->params.spm.mult_nids = cur_bs_ptr->csp.sp.mult_nids;
    rsp_ptr->params.spm.base_id = cur_bs_ptr->csp.sp.base_id;
    rsp_ptr->params.spm.base_class = cur_bs_ptr->csp.sp.base_class;
    rsp_ptr->params.spm.page_chan = cur_bs_ptr->csp.sp.page_chan;
    rsp_ptr->params.spm.max_slot_cycle_index = cur_bs_ptr->csp.sp.max_slot_cycle_index;
    rsp_ptr->params.spm.home_reg = cur_bs_ptr->csp.sp.home_reg;
    rsp_ptr->params.spm.for_sid_reg = cur_bs_ptr->csp.sp.for_sid_reg;
    rsp_ptr->params.spm.for_nid_reg = cur_bs_ptr->csp.sp.for_nid_reg;
    rsp_ptr->params.spm.power_up_reg = cur_bs_ptr->csp.sp.power_up_reg;
    rsp_ptr->params.spm.power_down_reg = cur_bs_ptr->csp.sp.power_down_reg;
    rsp_ptr->params.spm.parameter_reg = cur_bs_ptr->csp.sp.parameter_reg;
    rsp_ptr->params.spm.reg_prd = cur_bs_ptr->csp.sp.reg_prd;
    rsp_ptr->params.spm.base_lat = cur_bs_ptr->csp.sp.base_lat;
    rsp_ptr->params.spm.base_long = cur_bs_ptr->csp.sp.base_long;
    rsp_ptr->params.spm.reg_dist = cur_bs_ptr->csp.sp.reg_dist;
    rsp_ptr->params.spm.srch_win_a = cur_bs_ptr->csp.sp.srch_win_a;
    rsp_ptr->params.spm.srch_win_n = cur_bs_ptr->csp.sp.srch_win_n;
    rsp_ptr->params.spm.srch_win_r = cur_bs_ptr->csp.sp.srch_win_r;
    rsp_ptr->params.spm.nghbr_max_age = cur_bs_ptr->csp.sp.nghbr_max_age;
    rsp_ptr->params.spm.pwr_rep_thresh = cur_bs_ptr->csp.sp.pwr_rep_thresh;
    rsp_ptr->params.spm.pwr_rep_frames = cur_bs_ptr->csp.sp.pwr_rep_frames;
    rsp_ptr->params.spm.pwr_thresh_enable = cur_bs_ptr->csp.sp.pwr_thresh_enable;
    rsp_ptr->params.spm.pwr_period_enable = cur_bs_ptr->csp.sp.pwr_period_enable;
    rsp_ptr->params.spm.pwr_rep_delay = cur_bs_ptr->csp.sp.pwr_rep_delay;
    rsp_ptr->params.spm.rescan = cur_bs_ptr->csp.sp.rescan;
    rsp_ptr->params.spm.t_add = cur_bs_ptr->csp.sp.t_add;
    rsp_ptr->params.spm.t_drop = cur_bs_ptr->csp.sp.t_drop;
    rsp_ptr->params.spm.t_comp = cur_bs_ptr->csp.sp.t_comp;
    rsp_ptr->params.spm.t_tdrop = cur_bs_ptr->csp.sp.t_tdrop;
    rsp_ptr->params.spm.ext_sys_parameter = cur_bs_ptr->csp.sp.ext_sys_parameter;
    rsp_ptr->params.spm.ext_nghbr_list = cur_bs_ptr->csp.sp.ext_nghbr_list;
    rsp_ptr->params.spm.gen_nghbr_list = cur_bs_ptr->csp.sp.gen_nghbr_list;
    rsp_ptr->params.spm.global_redirect = cur_bs_ptr->csp.sp.global_redirect;
    rsp_ptr->params.spm.is2000_incl = cur_bs_ptr->csp.sp.is2000_incl;
    rsp_ptr->params.spm.pri_nghbr_list = cur_bs_ptr->csp.sp.pri_nghbr_list;
    rsp_ptr->params.spm.user_zone_id = cur_bs_ptr->csp.sp.user_zone_id;
    rsp_ptr->params.spm.ext_global_redirect = cur_bs_ptr->csp.sp.ext_global_redirect;
    rsp_ptr->params.spm.ext_chan_list = cur_bs_ptr->csp.sp.ext_chan_list;
    rsp_ptr->params.spm.is2000_rel_c_incl = cur_bs_ptr->csp.sp.is2000_rel_c_incl;
    rsp_ptr->params.spm.t_tdrop_range_incl = cur_bs_ptr->csp.sp.t_tdrop_range_incl;
    rsp_ptr->params.spm.t_tdrop_range = cur_bs_ptr->csp.sp.t_tdrop_range;
  }

  /* Fill in the ESPM fields. */
  rsp_ptr->params.espm_included  = cur_bs_ptr->rx.esp_rx;
  if(rsp_ptr->params.espm_included)
  {
    rsp_ptr->params.espm.pilot_pn = cur_bs_ptr->csp.esp.pilot_pn;
    rsp_ptr->params.espm.config_msg_seq = cur_bs_ptr->csp.esp.config_msg_seq;
    rsp_ptr->params.espm.delete_for_tmsi = cur_bs_ptr->csp.esp.delete_for_tmsi;
    rsp_ptr->params.espm.pref_msid_type = cur_bs_ptr->csp.esp.pref_msid_type;
    rsp_ptr->params.espm.mcc = cur_bs_ptr->csp.esp.mcc;
    rsp_ptr->params.espm.imsi_11_12 = cur_bs_ptr->csp.esp.imsi_11_12;
    rsp_ptr->params.espm.tmsi_zone_len = cur_bs_ptr->csp.esp.tmsi_zone_len;
    qw_equ(rsp_ptr->params.espm.tmsi_zone, cur_bs_ptr->csp.esp.tmsi_zone);
    rsp_ptr->params.espm.bcast_index = cur_bs_ptr->csp.esp.bcast_index;
    rsp_ptr->params.espm.is95b_incl = cur_bs_ptr->csp.esp.is95b_incl;
    rsp_ptr->params.espm.imsi_t_supported = cur_bs_ptr->csp.esp.imsi_t_supported;
    rsp_ptr->params.espm.p_rev = cur_bs_ptr->csp.esp.p_rev;
    rsp_ptr->params.espm.min_p_rev = cur_bs_ptr->csp.esp.min_p_rev;
    rsp_ptr->params.espm.soft_slope = cur_bs_ptr->csp.esp.soft_slope;
    rsp_ptr->params.espm.add_intercept = cur_bs_ptr->csp.esp.add_intercept;
    rsp_ptr->params.espm.drop_intercept = cur_bs_ptr->csp.esp.drop_intercept;
    rsp_ptr->params.espm.packet_zone_id = cur_bs_ptr->csp.esp.packet_zone_id;
    rsp_ptr->params.espm.max_num_alt_so = cur_bs_ptr->csp.esp.max_num_alt_so;
    rsp_ptr->params.espm.reselect_included = cur_bs_ptr->csp.esp.reselect_included;
    rsp_ptr->params.espm.ec_thresh = cur_bs_ptr->csp.esp.ec_thresh;
    rsp_ptr->params.espm.ec_io_thresh = cur_bs_ptr->csp.esp.ec_io_thresh;
    rsp_ptr->params.espm.pilot_report = cur_bs_ptr->csp.esp.pilot_report;
    rsp_ptr->params.espm.nghbr_set_entry_info = cur_bs_ptr->csp.esp.nghbr_set_entry_info;
    rsp_ptr->params.espm.acc_ent_ho_order = cur_bs_ptr->csp.esp.acc_ent_ho_order;
    rsp_ptr->params.espm.nghbr_set_access_info = cur_bs_ptr->csp.esp.nghbr_set_access_info;
    rsp_ptr->params.espm.access_ho = cur_bs_ptr->csp.esp.access_ho;
    rsp_ptr->params.espm.access_ho_msg_rsp = cur_bs_ptr->csp.esp.access_ho_msg_rsp;
    rsp_ptr->params.espm.access_probe_ho = cur_bs_ptr->csp.esp.access_probe_ho;
    rsp_ptr->params.espm.acc_ho_list_upd = cur_bs_ptr->csp.esp.acc_ho_list_upd;
    rsp_ptr->params.espm.acc_probe_ho_other_msg = cur_bs_ptr->csp.esp.acc_probe_ho_other_msg;
    rsp_ptr->params.espm.max_num_probe_ho = cur_bs_ptr->csp.esp.max_num_probe_ho;
    rsp_ptr->params.espm.nghbr_set_size = cur_bs_ptr->csp.esp.nghbr_set_size;
    for(i=0; i < MAX_NGHBR_SET_SIZE; i++)
    {
      rsp_ptr->params.espm.access_entry_ho[i] = cur_bs_ptr->csp.esp.access_entry_ho[i];
      rsp_ptr->params.espm.access_ho_allowed[i] = cur_bs_ptr->csp.esp.access_ho_allowed[i];
    }
    rsp_ptr->params.espm.broadcast_gps_asst = cur_bs_ptr->csp.esp.broadcast_gps_asst;
    rsp_ptr->params.espm.qpch_supported = cur_bs_ptr->csp.esp.qpch_supported;
    rsp_ptr->params.espm.num_qpch = cur_bs_ptr->csp.esp.num_qpch;
    rsp_ptr->params.espm.qpch_rate = cur_bs_ptr->csp.esp.qpch_rate;
    rsp_ptr->params.espm.qpch_power_level_page = cur_bs_ptr->csp.esp.qpch_power_level_page;
    rsp_ptr->params.espm.qpch_cci_supported = cur_bs_ptr->csp.esp.qpch_cci_supported;
    rsp_ptr->params.espm.qpch_power_level_config = cur_bs_ptr->csp.esp.qpch_power_level_config;
    rsp_ptr->params.espm.sdb_supported = cur_bs_ptr->csp.esp.sdb_supported;
    rsp_ptr->params.espm.rlgain_traffic_pilot = cur_bs_ptr->csp.esp.rlgain_traffic_pilot;
    rsp_ptr->params.espm.rev_pwr_cntl_delay_incl = cur_bs_ptr->csp.esp.rev_pwr_cntl_delay_incl;
    rsp_ptr->params.espm.rev_pwr_cntl_delay = cur_bs_ptr->csp.esp.rev_pwr_cntl_delay;

    rsp_ptr->params.espm.is_is2000_rel_a_incl = FALSE;
#ifdef FEATURE_IS2000_REL_A
    if(rsp_ptr->params.espm.p_rev >= P_REV_IS2000_REL_A)
    {
      rsp_ptr->params.espm.is_is2000_rel_a_incl = TRUE;
      rsp_ptr->params.espm.auto_msg_supported = cur_bs_ptr->csp.esp.auto_msg_supported;
      rsp_ptr->params.espm.auto_msg_interval = cur_bs_ptr->csp.esp.auto_msg_interval;
      rsp_ptr->params.espm.mob_qos = cur_bs_ptr->csp.esp.mob_qos;
      rsp_ptr->params.espm.enc_supported = cur_bs_ptr->csp.esp.enc_supported;
      rsp_ptr->params.espm.sig_encrypt_sup = cur_bs_ptr->csp.esp.sig_encrypt_sup;
      rsp_ptr->params.espm.ui_encrypt_sup = cur_bs_ptr->csp.esp.ui_encrypt_sup;
      rsp_ptr->params.espm.use_sync_id = cur_bs_ptr->csp.esp.use_sync_id;
      rsp_ptr->params.espm.cs_supported = cur_bs_ptr->csp.esp.cs_supported;
      rsp_ptr->params.espm.bcch_supported = cur_bs_ptr->csp.esp.bcch_supported;
      rsp_ptr->params.espm.ms_init_pos_loc_sup_ind = cur_bs_ptr->csp.esp.ms_init_pos_loc_sup_ind;
      rsp_ptr->params.espm.pilot_info_req_supported = cur_bs_ptr->csp.esp.pilot_info_req_supported;
    }
#endif /* FEATURE_IS2000_REL_A */

    rsp_ptr->params.espm.is_is2000_rel_b_incl = FALSE;
#ifdef FEATURE_IS2000_REL_B
    if(rsp_ptr->params.espm.p_rev >= P_REV_IS2000_REL_B)
    {
      rsp_ptr->params.espm.is_is2000_rel_b_incl = TRUE;
      rsp_ptr->params.espm.qpch_bi_supported = cur_bs_ptr->csp.esp.qpch_bi_supported;
      rsp_ptr->params.espm.qpch_power_level_bcast = cur_bs_ptr->csp.esp.qpch_power_level_bcast;
      rsp_ptr->params.espm.band_class_info_req = cur_bs_ptr->csp.esp.band_class_info_req;
      rsp_ptr->params.espm.alt_band_class = cur_bs_ptr->csp.esp.alt_band_class;
      rsp_ptr->params.espm.cdma_off_time_rep_sup_ind = cur_bs_ptr->csp.esp.cdma_off_time_rep_sup_ind;
      rsp_ptr->params.espm.cdma_off_time_rep_threshold_unit = cur_bs_ptr->csp.esp.cdma_off_time_rep_threshold_unit;
      rsp_ptr->params.espm.cdma_off_time_rep_threshold = cur_bs_ptr->csp.esp.cdma_off_time_rep_threshold;
    }
#endif /* FEATURE_IS2000_REL_B */

   rsp_ptr->params.espm.is_is2000_rel_c_incl = FALSE;
#ifdef FEATURE_IS2000_REL_C
    if(rsp_ptr->params.espm.p_rev >= P_REV_IS2000_REL_C)
    {
      rsp_ptr->params.espm.is_is2000_rel_c_incl = TRUE;
      rsp_ptr->params.espm.chm_supported = cur_bs_ptr->csp.esp.chm_supported;
      rsp_ptr->params.espm.release_to_idle_ind = cur_bs_ptr->csp.esp.release_to_idle_ind;
      rsp_ptr->params.espm.reconnect_msg_ind = cur_bs_ptr->csp.esp.reconnect_msg_ind;
      rsp_ptr->params.espm.msg_integrity_sup = cur_bs_ptr->csp.esp.msg_integrity_sup;
      rsp_ptr->params.espm.sig_integrity_sup_incl = cur_bs_ptr->csp.esp.sig_integrity_sup_incl;
      rsp_ptr->params.espm.sig_integrity_sup = cur_bs_ptr->csp.esp.sig_integrity_sup;
      rsp_ptr->params.espm.for_pdch_supported = cur_bs_ptr->csp.esp.for_pdch_supported;
      rsp_ptr->params.espm.pdch_chm_supported = cur_bs_ptr->csp.esp.pdch_chm_supported;
      rsp_ptr->params.espm.pdch_parms_incl = cur_bs_ptr->csp.esp.pdch_parms_incl;
      rsp_ptr->params.espm.for_pdch_rlgain_incl = cur_bs_ptr->csp.esp.for_pdch_rlgain_incl;
      rsp_ptr->params.espm.rlgain_ackch_pilot = cur_bs_ptr->csp.esp.rlgain_ackch_pilot;
      rsp_ptr->params.espm.rlgain_cqich_pilot = cur_bs_ptr->csp.esp.rlgain_cqich_pilot;
      rsp_ptr->params.espm.num_soft_switching_frames = cur_bs_ptr->csp.esp.num_soft_switching_frames;
      rsp_ptr->params.espm.num_softer_switching_frames = cur_bs_ptr->csp.esp.num_softer_switching_frames;
      rsp_ptr->params.espm.num_soft_switching_slots = cur_bs_ptr->csp.esp.num_soft_switching_slots;
      rsp_ptr->params.espm.num_softer_switching_slots = cur_bs_ptr->csp.esp.num_softer_switching_slots;
      rsp_ptr->params.espm.pdch_soft_switching_delay = cur_bs_ptr->csp.esp.pdch_soft_switching_delay;
      rsp_ptr->params.espm.pdch_softer_switching_delay = cur_bs_ptr->csp.esp.pdch_softer_switching_delay;
      rsp_ptr->params.espm.walsh_table_id = cur_bs_ptr->csp.esp.walsh_table_id;
      rsp_ptr->params.espm.num_pdcch = cur_bs_ptr->csp.esp.num_pdcch;
      for(i=0; i<CAI_MAX_NUM_FOR_PDCCH; i++)
      {
        rsp_ptr->params.espm.for_pdcch_walsh[i] = cur_bs_ptr->csp.esp.for_pdcch_walsh[i];
      }
      rsp_ptr->params.espm.imsi_10_incl = cur_bs_ptr->csp.esp.imsi_10_incl;
      rsp_ptr->params.espm.imsi_10 = cur_bs_ptr->csp.esp.imsi_10;
    }
#endif /* FEATURE_IS2000_REL_C */
  }
  /* Fill in the ITSPM fields. */
  rsp_ptr->params.itspm_included = itspm_rxed;

  if(rsp_ptr->params.itspm_included)
  {
    rsp_ptr->params.itspm.sid = itspm.sid;
    rsp_ptr->params.itspm.nid = itspm.nid;
    rsp_ptr->params.itspm.srch_win_a = itspm.srch_win_a;
    rsp_ptr->params.itspm.srch_win_n = itspm.srch_win_n;
    rsp_ptr->params.itspm.srch_win_r = itspm.srch_win_r;
    rsp_ptr->params.itspm.t_add = itspm.t_add;
    rsp_ptr->params.itspm.t_drop = itspm.t_drop;
    rsp_ptr->params.itspm.t_comp = itspm.t_comp;
    rsp_ptr->params.itspm.t_tdrop = itspm.t_tdrop;
    rsp_ptr->params.itspm.nghbr_max_age = itspm.nghbr_max_age;
    rsp_ptr->params.itspm.p_rev = itspm.p_rev;
    rsp_ptr->params.itspm.soft_slope = itspm.soft_slope;
    rsp_ptr->params.itspm.add_intercept = itspm.add_intercept;
    rsp_ptr->params.itspm.drop_intercept = itspm.drop_intercept;
    rsp_ptr->params.itspm.packet_zone_id = itspm.packet_zone_id;
    rsp_ptr->params.itspm.extension = itspm.extension;
    rsp_ptr->params.itspm.t_mulchan = itspm.t_mulchan;
    rsp_ptr->params.itspm.begin_preamble = itspm.begin_preamble;
    rsp_ptr->params.itspm.resume_preamble = itspm.resume_preamble;
    rsp_ptr->params.itspm.t_slotted_incl = itspm.t_slotted_incl;
    rsp_ptr->params.itspm.t_slotted = itspm.t_slotted;

    rsp_ptr->params.itspm.is_is2000_rel_a_incl = FALSE;
#ifdef FEATURE_IS2000_REL_A
    if(rsp_ptr->params.itspm.p_rev >= P_REV_IS2000_REL_A)
    {
      rsp_ptr->params.itspm.is_is2000_rel_a_incl = TRUE;
      rsp_ptr->params.itspm.enc_supported = itspm.enc_supported;
      rsp_ptr->params.itspm.sig_encrypt_sup = itspm.sig_encrypt_sup;
      rsp_ptr->params.itspm.ui_encrypt_sup = itspm.ui_encrypt_sup;
      rsp_ptr->params.itspm.cs_supported = itspm.cs_supported;
    }
#endif /* FEATURE_IS2000_REL_A */

    rsp_ptr->params.itspm.is_is2000_rel_a_incl = FALSE;
#ifdef FEATURE_IS2000_REL_C
    if(rsp_ptr->params.itspm.p_rev >= P_REV_IS2000_REL_C)
    {
      rsp_ptr->params.itspm.is_is2000_rel_a_incl = TRUE;
      rsp_ptr->params.itspm.chm_supported = itspm.chm_supported;
      rsp_ptr->params.itspm.cdma_off_time_rep_sup_ind = itspm.cdma_off_time_rep_sup_ind;
      rsp_ptr->params.itspm.cdma_off_time_rep_threshold_unit = itspm.cdma_off_time_rep_threshold_unit;
      rsp_ptr->params.itspm.cdma_off_time_rep_threshold = itspm.cdma_off_time_rep_threshold;
      rsp_ptr->params.itspm.t_tdrop_range_incl = itspm.t_tdrop_range_incl;
      rsp_ptr->params.itspm.t_tdrop_range = itspm.t_tdrop_range;
      rsp_ptr->params.itspm.for_pdch_supported = itspm.for_pdch_supported;
      rsp_ptr->params.itspm.pdch_chm_supported = itspm.pdch_chm_supported;
      rsp_ptr->params.itspm.sdb_supported = itspm.sdb_supported;
      rsp_ptr->params.itspm.mob_qos = itspm.mob_qos;
      rsp_ptr->params.itspm.ms_init_pos_loc_sup_ind = itspm.ms_init_pos_loc_sup_ind;
    }
#endif /* FEATURE_IS2000_REL_C */
  }

  return rsp_ptr;
} /* mc_diag_mar_rda_test_stat */
#endif /* FEATURE_1XCP_FTD */

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X

/*===========================================================================

REDFUNCTION MCLOG_REPORT_EVENT_CSFB_TT_STATUS

DESCRIPTION
  Report event to indicate TT success/failure upon receiving UHDM/ECAM.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mclog_report_event_csfb_tt_status
(
  boolean tt_status,
  uint32  tt_ticks
)
{
  struct
  {
    boolean tt_status;
    uint32  tt_ticks;

  } tt_info;

  tt_info.tt_status = tt_status;
  tt_info.tt_ticks = tt_ticks;

  event_report_payload(EVENT_LTE_TO_1X_TT, sizeof(tt_info), &tt_info);
} /* mclog_report_event_csfb_tt_status */

/*===========================================================================

REDFUNCTION MCLOG_REPORT_EVENT_HO_STATUS

DESCRIPTION
  Report event to indicate 1xcsfb handover success. This is sent after receiving
  ack order for handover completion message or if there is a failure encountered.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mclog_report_event_csfb_ho_status
(
  boolean ho_status
)
{
  boolean ho_status_info;
  ho_status_info = ho_status;

  event_report_payload(EVENT_LTE_TO_1X_HO, sizeof(ho_status_info), &ho_status_info);
} /* mclog_report_event_csfb_ho_status */

/*===========================================================================

REDFUNCTION MCLOG_REPORT_EVENT_GCSNA_DL_MSG

DESCRIPTION
  Report GCSNA event every time a GCSNA msg is received.
  Payload indicatse the message_id (1=GCSNA1xServie, 2=L2ACK, etc) and cdma_msg_type.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mclog_report_event_gcsna_dl_msg
(
  byte gcsna_msg_id,
  byte cdma_msg_id
)
{
  struct
  {
    uint8 gcsna_msg_id;
    uint8 cdma_msg_id;

  } gcsna_dl_msg_info;

  gcsna_dl_msg_info. gcsna_msg_id = gcsna_msg_id ,
  gcsna_dl_msg_info. cdma_msg_id = cdma_msg_id ,

  event_report_payload(EVENT_LTE_TO_1X_DL_GCSNA_MSG, sizeof(gcsna_dl_msg_info), &gcsna_dl_msg_info);
} /* mclog_report_event_gcsna_dl_msg */

/*===========================================================================

REDFUNCTION MCLOG_REPORT_EVENT_GCSNA_UL_MSG

DESCRIPTION
  Report GCSNA event every time a GCSNA msg is sent.
  Payload indicatse the message_id (1=GCSNA1xServie, 2=L2ACK, etc) and cdma_msg_type.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mclog_report_event_gcsna_ul_msg
(
  byte gcsna_msg_id,
  byte cdma_msg_id
)
{
  struct
  {
    uint8 gcsna_msg_id;
    uint8 cdma_msg_id;

  } gcsna_ul_msg_info;

  gcsna_ul_msg_info. gcsna_msg_id = gcsna_msg_id ,
  gcsna_ul_msg_info. cdma_msg_id = cdma_msg_id ,

  event_report_payload(EVENT_LTE_TO_1X_UL_GCSNA_MSG, sizeof(gcsna_ul_msg_info), &gcsna_ul_msg_info);
} /* mclog_report_event_gcsna_ul_msg */

#endif /* FEATURE_MODEM_1X_IRAT_LTO1X */

/*===========================================================================

REDFUNCTION MCLOG_REPORT_EVENT_HOCM_SENT

DESCRIPTION
  Report event whenever (E)HOCM is sent on RL after Hard-Handoff.

DEPENDENCIES
None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mclog_report_event_hocm_sent(void)
{
  event_report(EVENT_HOCM_TRANSMITTED);
} /* mclog_report_event_ehocm_sent */

/*===========================================================================

FUNCTION MCLOG_REPORT_EVENT_CSFB_CALL_TYPE

DESCRIPTION
  Report 1x csfb call type event

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void mclog_report_event_csfb_call_type( csfb_call_info_type csfb_call_info)
{
  event_csfb_call_type event_csfb_call;

  event_csfb_call.csfb_call_info = csfb_call_info;

  M1X_MSG( DCP, LEGACY_MED,
    "CSFB DEBUG: Sending diag EVENT_CSFB_CALL_TYPE and csfb_call_info:%d",
    event_csfb_call.csfb_call_info);

  event_report_payload(EVENT_CSFB_CALL_TYPE,
                       sizeof(event_csfb_call_type),
                       (void*) &event_csfb_call);

} /* mclog_report_event_1x_csfb_call_type() */

/*lint +e818 +e655 */

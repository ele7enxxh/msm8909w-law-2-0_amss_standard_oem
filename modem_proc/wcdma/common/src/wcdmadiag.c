/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                Diagnostics Packet Processing Routines
                  WCDMA Target specific Routines

General Description
  Diagnostic packet processing routines that are specific to WCDMA
  target.

Copyright (c) 2001 - 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           Edit History

$PVCSPath:  L:/src/asw/MSM5200/common/vcs/diagpkt_wcdma.c_v   1.12   16 May 2002 09:49:12   gchhabra  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/common/src/wcdmadiag.c#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
09/09/15   vn      Addng QXDM support to attenuate WCDMA signal
02/12/15   ms      Query CM state to form the WCDMA polling cmd response
01/23/15   gv      Putting back the RACH stats code (incorrectly removed in code cleanup)
10/01/14   gv      Code cleanup. Removal of stale and unused code.  
09/01/14   kcm     Dime and Triton Featurization cleanup 
08/12/14   sd      Fixed DSDS Medium and Low compiler warnings in WCDMA
08/04/14   rs      Fixed compiler warnings
07/26/14   rkmk    Reduce image size by using right macro depending upon number of parameters
06/26/14   hk      DL Features Mainline
05/01/14   gsk     Changes for Version-2 of qxdm command to crash UE on unexpected search results
05/06/14   geg     Use safer versions of memcpy() and memmove()
11/19/13   vs      CQI override changes for 3C.
11/09/13   scm     Remove tmc.h include, as that file is empty now.
09/29/13   scm     Validate as_id before calling mm_ds_get_imsi().
09/29/13   scm     Use ghdi_ds_read_imei() in dual-SIM builds.
07/03/13   gsk     Adding Qxdm support to crash UE on unexpected search results
07/02/13   ks      Add ability to freeze cm measurements through qxdm
05/23/13   geg     WSW decoupling (except for component of QCHAT)
04/17/2013 jd      Changes to prevent multiple calls to wcdmadiag_init 
01/14/12   pr      Cleanup of code added as part of Dime Bringup.
10/24/12   scm     Fix unused var and duplicate define compiler warnings.
09/19/12   dp      Finished mainlining FEATURE_WCDMA_QICE_CONTROLLER
07/27/12   pr      Dime Bringup Changes
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
05/22/12   pr      Dime Bringup Changes
03/28/12   zr      Adding feature definitions for Dime
03/30/12   zr      Added support for FEATURE_WCDMA_DATA_FOR_UI_BAR_DISPLAY.
01/30/12   scm     Mainline FEATURE_L1_AUTO_BLER_MEAS.
                   Obsolete FEATURE_WCDMA_L1_MANUAL_CQI_UPD.
01/29/12   ms      Added QXDM command to set debug flag for DL datapath f3s
11/04/11   zr      Adding command to enable power profiling via QXDM
10/14/11   rl      Made changes for WCDMA Security Setting independent of NV Items.
04/27/11   scm     Call mm_get_imsi() instead of accessing mm_stored_imsi directly.
04/20/11   vsr     Added support to call RxD APIs through QXDM command 
04/16/11   sj      Added featurization in wcdmadiag_meas_req_handler for ECIO
04/14/11   ms      Changed #include filename from msmdsp.h to wl1mdspif.h
02/23/11   sb      Featurized changes to l1_rssi_struct under FEATURE_WCDMA_DIAG_QUERY_CELL_INFO
02/18/11   sb      Added code to support Network Meas parameters through QXDM command.
02/11/11   su      Added code to support setting UE HSDPA category 
                   through QXDM command.
11/03/10   grk     Added support to set RLC/MAC debug variables through QXDM command.
09/13/10   vb      Removed inclusion of mdsp header for Genesis (featurized)
08/23/10   scm     wl1_get_frame_number_for_diag() returns 0 if WCDMA not active.
05/10/10   rc      Corrected featurization for wcdma_cqi_params_handler().
05/03/10   rvs     Add params to call to
                   wl1_cme_proc_wplt_qice_oride_config_params_cmd().
04/27/10   rc      Corrected featurization for wcdma_cqi_params_handler. 
04/22/10   rvs     Fixed KW errors   
03/08/10   mr      Removed wl1dlcarrcfg.h to fix MOB errors.
03/08/10   mc/rgn  Added DIAG command for DC-HSDPA
01/13/10   scm     Move enchs_set_cqi_override_from_diag() from enchs.h to wenc.h.
12/16/09   rvs     Added the WCDMA_QICE_DEBUG_ENABLE_F handler.
10/07/09   rmsd    CMI changes. New API wl1_get_frame_number_for_diag() added.
09/24/09   sup     Corrected lint high errors
09/24/09   sup     Added check for comparing the last internal log code and the
                   last external log code
09/24/09   rvs     Modify QICE SW oride command params.
06/05/09   scm     Include enchs.h for enchs_set_cqi_override_from_diag().
05/13/09   scm     Merge in mods from 8650 branch.
03/25/09   rm      Fix for lint issue.
10/01/08   kps     Fix more Klocwork errors.
09/23/08   kps     Fix compiler warning.
07/10/08   kps     Fix Klocwork errors.
06/25/08   kps     Added support for diag commands WCDMA_DIAG_WRITE_FBIBER_THRESH_F 
                   and WCDMA_DIAG_READ_FBIBER_THRESH_F
03/17/08   ar      Added FEATURE_UL_CPU_BASED_FC and FEATURE_DL_CPU_BASED_FC
                   featurization in wcdmadiag_rlc_flow_control_config_handler()
02/08/08   gnk     Added QXDM support for R99 interface
11/30/07   mc      Added support for dumping TBset data.
10/08/07   mc      Added support for enabling MBMS debug mode
09/10/07   yh      Added support for WPLT cmds being sent through DIAG.
08/28/07   gnk     Added, renamed and modified some RACH statistic variables
08/20/07   kps     Expose dl_num_times_sfn_mismatch for diag
07/12/07   kps     Fix response to WCDMA_DIAG_WCDMA_CLEAR_STATS_F
07/03/07   kps     Don't reset DRX cycle count display on entering DRX mode.
05/25/07   kps     Add support to output RACH Transmission Preamble Count.
05/03/07   bd      Merged changes from 7200 and 7200A versions, so that a common file
                   can be used on all targets.
05/01/07   kps     Fix compilation problem with WPLT CRM builds.
04/24/07   kp      Added ability to output RRC counters,Namely Intra freq  cell reselection 
                   statistics, WCDMA to GSM cell reselection statistics, OOS related statistics
                   & RACH transmission related statistics.
04/23/07   nd      post the PSC SCAN STATE event immediately after receiving the 
                   diag cmd to START/STOP the scan. removed the l1m state check
				   while accepting the diag cmd to start/stop the scanner
04/20/07   kps     Display/reset reacq counters in wcdma stat functions.
                   "Paging stats" is now renamed to "wcdma stats".
04/12/07   kps     Add ability to output Paging counters.
03/28/07   scm     Featurize file with FEATURE_WCDMA to help non-WCDMA builds.
03/07/07   nd      changes for PSC scanner srch under FEATURE_WCDMA_L1_PSC_SCANNER
08/11/06   ttl     Support CPU based flow control.
03/20/06   mg      Support for Sample Server Logging request and response packets
07/25/05   mc      Corrected compiler #ifdef conditional switch.
07/20/05   mc      Added a new entry to the WCDMA dispatch table and a callback
                   function to update the CQI offset manually.
07/07/04   scm     Add debug ability to freeze and slam VCTCXO.
11/24/03   ao      Adjusted include files to make #define Feature_GSM_PLT visible
11/21/03   ka      Fixed compilation errors and warnings
11/04/03   ka      Removed wcdma call end based on global call_id. List of active calls
                   is searched and the first one is ended.
07/28/03   scm     Support for query of various DRX reacquisition statistics.
06/10/03   src     Updated hashed-included header names to use only those mDSP
                   headers that encapsulate recent firmware API changes.
04/22/03   gs      Added BLER measurement rate change command (Id 8)
02/21/03   prk     Modified parameter list to cm_mm_call_cmd_end().
09/17/02   gsc     Added support for command code 15 (wcdma_additional_status)
02-07-23   tjw     diagpkt_wcdma_status returns TMC_STATE_ONLINE for TMC_LPM
                   to keep QPST happy
05/08/02   prk     Modified wcdmadiag_call_orig and wcdmadiag_call_end
                   to be compatible with multi-mode CM.
04/16/02   abp     Included msm.h instead of msm5200reg.h
02/22/02   abp     Update WCDMA verno packet per Serial ICD revC.
02/12/02   yiz     Update call originate and call end packets according to new
                   CM interfaces to originate / end a call.
11/21/01   gsc     Only include cm.h if FEATURE_CM is defined
11/02/01   yiz     Added functions wcdmadiag_call_orig() and
                   wcdmadiag_call_end() to implement the two commands.
09/25/01   ABP     Modifications to wcdmadiag_verno() to get correct
                   version info
09/14/01   abp     Implemented wcdmadiag_status() Added it to the
                   dispatch table
05/21/01   cpe     Featurized the diagpkt_qxdm_al1if function
04/27/01   cpe     Created file.

===========================================================================*/
#include "wcdma_variation.h"
#include "comdef.h"
#include "customer.h"
#include "log_codes_wcdma_int.h"
#if !defined(FEATURE_GSM_PLT) && defined(FEATURE_WCDMA)

#ifdef __cplusplus
  extern "C" {
#endif

#include <string.h>
#include "wcdmadiag.h"
#include "diagcmd.h"
#include "msg.h"
#include "err.h"
#include "mobile.h"
#include "cm_v.h"

#ifdef FEATURE_CM
#include "cmutil.h"  /* prototype for cm_num_fill() */
#include "cm.h"
#endif

#ifndef FEATURE_WCDMA_PLT
#include "mm.h"
#endif

#ifdef FEATURE_WCDMA_TCXO_RESOURCE_SUPPORT
#include "l1pos.h"
#endif

#include "l1extif.h"
#include "ghdi_exp.h"

#ifdef FEATURE_WCDMA_SAMPLE_SERVER_DATA_LOGGING
#error code not present
#endif

#ifdef FEATURE_CPU_BASED_FLOW_CONTROL
#include "rlcfc.h"
#endif //FEATURE_CPU_BASED_FLOW_CONTROL

#ifdef FEATURE_WCDMA_HSUPA
#include "euledpch.h"
#endif

#include "ulcmd.h"

#ifdef FEATURE_MBMS
#error code not present
#endif

#ifdef FEATURE_WCDMA_TBSET_DEBUG_DUMP
#error code not present
#endif

#ifdef FEATURE_WCDMA_L1_PSC_SCANNER
#include "wsrchmobileview.h"
#endif

#include "mdspasync.h"

#if defined(FEATURE_WCDMA_MIMO) && defined(FEATURE_QDSP6)
#include "wenc.h"
#endif

#include "drx.h"
#include "dldec.h"
#include "wl1m.h"
#include "wsrchi.h"
#ifndef FEATURE_WCDMA_PLT
#include "rrccmd_v.h"
#include "rrcpg1.h"
#include "rrcdata_v.h"
#else
#error code not present
#endif /* FEATURE_WCDMA_PLT */

#include "wl1qicectrl.h"

#ifdef FEATURE_WCDMA_RX_DIVERSITY
#include "rxdiv.h"
#endif

#ifdef FEATURE_CMI
#include "wcdmamvsif.h"
#include "wcdmadiagif.h"
#endif
#include "seq_v.h"

#include "macrlcif.h"
#include "l1rrcif.h"
#include "drx.h"

/* TMC states defined in Serial Interface Control Document for W-CDMA */
const uint8 TMC_STATE_OFFLINE = 0;
const uint8 TMC_STATE_ONLINE  = 1;
const uint8 TMC_STATE_UNKNOWN = 99;

extern void drx_set_pwr_strobe(boolean enabled);

#ifdef FEATURE_CM
LOCAL static cm_call_id_type call_id = 0;
extern boolean cm_call_id_is_allocated( cm_call_id_type ); /* For wcdmadiag_call_end() */
#endif

#ifdef FEATURE_WCDMA_DC_HSDPA
extern boolean wl1_dl_carrcfg_setup_debug_dc_cfg(
  /** psc*/
  uint16 psc,
  /** freq*/
  uint16 freq,
  /** ops_type */
  uint8 ops_type);
#endif
#ifdef FEATURE_WCDMA_SECURITY_SETTINGS
#include "rrcdata_v.h"
#endif
/* -----------------------------------------------------------------------
** Local functions
** ----------------------------------------------------------------------- */

PACKED void *wcdmadiag_verno (PACKED void* req_pkt, uint16 pkt_len);

PACKED void *wcdma_diag_change_bler_rate(PACKED void* req_pkt, uint16 pkt_len);

PACKED void *wcdmadiag_reacq_slew_stats (PACKED void* req_pkt, uint16 pkt_len);

PACKED void *wcdmadiag_wcdma_stats (PACKED void* req_pkt, uint16 pkt_len);

PACKED void *wcdmadiag_clear_wcdma_stats (PACKED void* req_pkt, uint16 pkt_len);

PACKED void *wcdmadiag_reacq_srch_stats (PACKED void* req_pkt, uint16 pkt_len);

PACKED void *wcdmadiag_reacq_clear_stats (PACKED void* req_pkt, uint16 pkt_len);

PACKED void *wcdmadiag_set_trk_lo_pdm (PACKED void* req_pkt, uint16 pkt_len);

PACKED void *wcdmadiag_call_orig (PACKED void* req_pkt, uint16 pkt_len);

PACKED void *wcdmadiag_call_end (PACKED void* req_pkt, uint16 pkt_len);

PACKED void *wcdmadiag_status (PACKED void* req_pkt, uint16 pkt_len);

PACKED void *wcdmadiag_additional_status (PACKED void* req_pkt, uint16 pkt_len);

#ifdef FEATURE_MBMS
#error code not present
#endif

#ifdef FEATURE_WCDMA_TBSET_DEBUG_DUMP
#error code not present
#endif

#ifdef FEATURE_WCDMA_MIMO
PACKED void * wcdmadiag_wcdma_mimo_params_handler(
   PACKED void *req_ptr,
   uint16       pkt_len
);
#endif


#ifdef FEATURE_HSDPA
PACKED void * wcdmadiag_wcdma_cqi_params_handler(
   PACKED void *req_ptr,
   uint16       pkt_len
);
#endif

PACKED void * wcdmadiag_wcdma_fw_debug_cmd_handler(
   PACKED void *req_ptr,
   uint16       pkt_len
);

#ifdef FEATURE_CPU_BASED_FLOW_CONTROL
PACKED void * wcdmadiag_rlc_flow_control_config_handler(
   PACKED void *req_ptr,
   uint16       pkt_len
);
#endif

#ifdef FEATURE_WCDMA_HSUPA
PACKED void * wcdmadiag_manual_eul_ul_var_upd_handler(
   PACKED void *req_ptr,
   uint16       pkt_len
);
#endif

#ifdef FEATURE_WCDMA_DC_HSDPA
PACKED void * wcdmadiag_manual_setup_dc_test_cfg(
   PACKED void *req_ptr,
   uint16       pkt_len
);
#endif

PACKED void * wcdmadiag_qice_oride_table_params_handler(
   PACKED void *req_ptr,
   uint16       pkt_len
);

PACKED void * wcdmadiag_qice_oride_config_params_handler(
   PACKED void *req_ptr,
   uint16       pkt_len
);

PACKED void * wcdmadiag_qice_oride_fht_params_handler(
   PACKED void *req_ptr,
   uint16       pkt_len
);

PACKED void * wcdmadiag_qice_oride_fw_params_handler(
   PACKED void *req_ptr,
   uint16       pkt_len
);

PACKED void * wcdmadiag_qice_oride_log_params_handler(
   PACKED void *req_ptr,
   uint16       pkt_len
);

PACKED void * wcdmadiag_qice_reset_handler(
   PACKED void *req_ptr,
   uint16       pkt_len
);

PACKED void * wcdmadiag_cme_oride_params_handler(
   PACKED void *req_ptr,
   uint16       pkt_len
);

PACKED void * wcdmadiag_qice_debug_enable_handler(
   PACKED void *req_ptr,
   uint16       pkt_len
);


#ifdef FEATURE_WCDMA_RX_DIVERSITY
PACKED void * wcdmadiag_rxd_api_oride_handler(
   PACKED void *req_ptr,
   uint16       pkt_len
);

#endif

PACKED void * wcdmadiag_manual_ul_var_upd_handler(
   PACKED void *req_ptr,
   uint16       pkt_len
);

#ifdef FEATURE_WCDMA_SAMPLE_SERVER_DATA_LOGGING
#error code not present
#endif

#ifdef FEATURE_WCDMA_L1_PSC_SCANNER
PACKED void * wcdmadiag_l1_psc_scanner_config_handler(PACKED void* req_pkt, uint16 pkt_len);
#endif

PACKED void * wcdma_diag_write_fbiber_thresh_handler(PACKED void* req_pkt, uint16 pkt_len);

PACKED void * wcdma_diag_read_fbiber_thresh_handler(PACKED void* req_pkt, uint16 pkt_len);

/* Code added for Skip CM Measurements from QXDM */
PACKED void * wcdmadiag_skip_cm_meas_var_upd_handler(PACKED void *req_ptr, uint16 pkt_len);

void wcdmadiag_init (void);

uint32 wl1_get_frame_number_for_diag(void);

PACKED void * wcdmadiag_manual_setup_rlc_dbg_cfg(
   PACKED void *req_pkt,
   uint16       pkt_len
);

PACKED void * wcdmadiag_manual_setup_mac_dbg_cfg(
   PACKED void *req_pkt,
   uint16       pkt_len
);


#ifdef FEATURE_WCDMA_SET_HS_CAT
PACKED void * wcdmadiag_rrc_set_hs_cat(
   PACKED void *req_ptr,
   uint16       pkt_len
);
#endif

PACKED void * wcdmadiag_meas_req_handler(
   PACKED void *req_pkt,
   uint16       pkt_len
);
#ifdef FEATURE_WCDMA_SECURITY_SETTINGS
PACKED void * wcdmadiag_manual_security_set_integ(
   PACKED void *req_ptr,
   uint16       pkt_len
);

PACKED void * wcdmadiag_manual_security_set_ciph(
   PACKED void *req_ptr,
   uint16       pkt_len
);

PACKED void * wcdmadiag_manual_security_set_fksec(
   PACKED void *req_ptr,
   uint16       pkt_len
);
#endif

PACKED void* wcdmadiag_gpio_pwr_prof_handler(
  PACKED void* req_pkt,
  uint16       pkt_len
);


PACKED void * wcdmadiag_debug_crash_on_srch_done_handler(
   PACKED void* req_pkt,
   uint16       pkt_len
);


PACKED void * wcdmadiag_attenuate_signal_handler(
   PACKED void* req_pkt,
   uint16       pkt_len
);


#ifdef FEATURE_DUAL_SIM
#define IS_NOT_VALID_AS_ID(x) ((x < SYS_MODEM_AS_ID_1) || (x >= SYS_MODEM_AS_ID_1 + MAX_AS_IDS))
#endif

/* =======================================================================
**                            Function Definitions
** ======================================================================= */


/*===========================================================================
Function: wcdmadiag_verno
Description:

       This function processes the wcdma specific version request packet
       from QXDM and sends the response back.

============================================================================*/

PACKED void *wcdmadiag_verno (PACKED void* req_pkt, uint16 pkt_len)
{

  DIAG_SUBSYS_WCDMA_WCDMA_DIAG_VERNO_F_rsp_type *rsp_ptr;
  //uint32 val;

  const int rsp_len = sizeof(DIAG_SUBSYS_WCDMA_WCDMA_DIAG_VERNO_F_rsp_type);

  /* Allocate the memory for this */
  rsp_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_DIAG_VERNO_F_rsp_type *)diagpkt_subsys_alloc(DIAG_SUBSYS_WCDMA, WCDMA_DIAG_VERNO_F,
                                 rsp_len );
  if (!rsp_ptr)
  {
    ERR_FATAL("diagpkt_subsys_alloc(%d, %d, %d) returned NULL!", 
              DIAG_SUBSYS_WCDMA, WCDMA_DIAG_VERNO_F, rsp_len);
  }
  else
  {
    /* Fill the response packet */

    //rsp_ptr->class_mark = CLASS_MARK ;

    /* 1 - 2000 and c for 12 month */
    //rsp_ptr->g3gpp_rev = G3GPP_REV ;

    //rsp_ptr->g3gpp_rel = 4 ;

    rsp_ptr->vdsp_ver = VOC_DSP_VER;

    /* Not supported in new WFW API yet. */
    rsp_ptr->mdsp_ver_rom = 0;
    rsp_ptr->mdsp_ver_ram = 0;

    //rsp_ptr->drx_index = DRX_INDEX;

    /*  last 8 bits of HW_REVISION_NUMBER represent the MSM version  */

    //val = INP32 (HW_REVISION_NUMBER);
    //rsp_ptr->msm_ver_maj = (val & 0xff) ;
    //rsp_ptr->msm_ver_min = 0;

    /*  WCDMA model number is 90 */
    //rsp_ptr->model_num = mob_model ;
  }
  return rsp_ptr ;

}

/*===========================================================================
Function: wcdma_diag_change_bler_rate
Description:

       This function processes the wcdma specific L1 bler measure rate change.

============================================================================*/

/* Variable for automatically measure BLER.
   Value 0 means that timer is infinite */
extern uint16  dl_bler_meas_duration_ms;
/* set this variable to TRUE if the duration is required to be updated.
   The time will be updated in next 10 ms */
extern boolean dl_bler_meas_timer_update;

PACKED void *wcdma_diag_change_bler_rate(PACKED void* req_pkt, uint16 pkt_len)
{
  DIAG_SUBSYS_WCDMA_WCDMA_DIAG_BLER_RATE_CHANGE_F_req_type *req_ptr = ( DIAG_SUBSYS_WCDMA_WCDMA_DIAG_BLER_RATE_CHANGE_F_req_type *)req_pkt;
  DIAG_SUBSYS_WCDMA_WCDMA_DIAG_BLER_RATE_CHANGE_F_rsp_type *rsp_ptr;

  const int rsp_len = sizeof(DIAG_SUBSYS_WCDMA_WCDMA_DIAG_BLER_RATE_CHANGE_F_rsp_type);

  dl_bler_meas_timer_update = TRUE;

  dl_bler_meas_duration_ms = req_ptr->bler_meas_duration_ms;

  /* Allocate the memory for this */
  rsp_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_DIAG_BLER_RATE_CHANGE_F_rsp_type * )diagpkt_subsys_alloc(DIAG_SUBSYS_WCDMA, WCDMA_DIAG_BLER_RATE_CHANGE_F,
                                 rsp_len );

  return rsp_ptr;
}

/*===========================================================================
Function: wcdmadiag_reacq_slew_stats
Description:

  This function processes the WCDMA specific WCDMA_DIAG_REACQ_SLEW_STATS_F
  command, which requests the statistics on the various DRX reacquisition
  slew values recorded in WCDMA L1.

============================================================================*/
PACKED void *wcdmadiag_reacq_slew_stats (PACKED void* req_pkt, uint16 pkt_len)
{
  DIAG_SUBSYS_WCDMA_WCDMA_DIAG_REACQ_SLEW_STATS_F_rsp_type *rsp_ptr;

  const int rsp_len = sizeof(DIAG_SUBSYS_WCDMA_WCDMA_DIAG_REACQ_SLEW_STATS_F_rsp_type);

  /* Allocate the memory for this */
  rsp_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_DIAG_REACQ_SLEW_STATS_F_rsp_type *)diagpkt_subsys_alloc(DIAG_SUBSYS_WCDMA, WCDMA_DIAG_REACQ_SLEW_STATS_F,
                                 rsp_len );

  if (!rsp_ptr)
  {
    ERR_FATAL("diagpkt_subsys_alloc(%d, %d, %d) returned NULL!", 
              DIAG_SUBSYS_WCDMA, WCDMA_DIAG_REACQ_SLEW_STATS_F, rsp_len);
  }
  else
  {
    rsp_ptr->srchzz_reacq_slew_stats = srchzz_reacq_slew_stats;
  }

  return rsp_ptr ;

}

/*===========================================================================
Function: wcdmadiag_wcdma_stats
Description:

  This function processes the WCDMA specific WCDMA_DIAG_WCDMA_STATS_F
  command, which requests the statistics related to Paging recorded in WL1 and RRC.

  This function is called when "send_data 75 4 26 0" is issued from cmd 
  line interface from QXDM

  Where the 75 is DIAG_SUBSYS_CMD_F {Subssytem dispatcher (extended diag cmd)}
  The 4 is DIAG_SUBSYS_WCDMA
  and the 26 is for WCDMA_DIAG_WCDMA_STATS_F, which is the function we are using.
  
  Sample output would be:
  16:54:56.580 0x4B 04 1A 00 02 00 00 00 03 00 00 00 00 00 00 00 00 01 00 00 F7 04 00 00
                              ---------   ---------   ---------   ---------   --------- 

   rrcpg1_num_pages          = 0x00 00 00 02
   rrcpg1_num_pages_matched  = 0x00 00 00 03
   drx_pich_demod_error      = 0x00 00 00 00
   drx_cycle_len             = 0x00 00 01 00
   num_drx_cycles            = 0x00 00 04 F7

============================================================================*/
PACKED void *wcdmadiag_wcdma_stats (PACKED void* req_pkt, uint16 pkt_len)
{
  DIAG_SUBSYS_WCDMA_WCDMA_DIAG_WCDMA_STATS_F_rsp_type *rsp_ptr;

  const int rsp_len = sizeof(DIAG_SUBSYS_WCDMA_WCDMA_DIAG_WCDMA_STATS_F_rsp_type);

  /* Allocate the memory for this */
  rsp_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_DIAG_WCDMA_STATS_F_rsp_type *)
              diagpkt_subsys_alloc(DIAG_SUBSYS_WCDMA, WCDMA_DIAG_WCDMA_STATS_F,
                                   rsp_len );
  if (!rsp_ptr)
  {
    ERR_FATAL("diagpkt_subsys_alloc(%d, %d, %d) returned NULL!", 
              DIAG_SUBSYS_WCDMA, WCDMA_DIAG_WCDMA_STATS_F, rsp_len);
  }
  else
  {
    /* Reacq slew stats*/
    rsp_ptr->srchzz_reacq_slew_stats = srchzz_reacq_slew_stats;
  
    /* Reacq search stats*/
    rsp_ptr->srchzz_reacq_srch_stats = srchzz_reacq_srch_stats;
  
    /* Number of times SFN mismatch is detected */
    rsp_ptr->dl_num_times_sfn_mismatch = dl_num_times_sfn_mismatch;
  
    /* Paging stats*/
  
  #ifdef FEATURE_WCDMA_PLT
    #error code not present
#else
    rsp_ptr->page_stats.rrcpg1_num_pages = rrcpg1_get_num_pages() - reset_rrcpg1_num_pages;
    rsp_ptr->page_stats.rrcpg1_num_pages_matched = rrcpg1_get_num_pages_matched() -
                                                          reset_rrcpg1_num_pages_matched;
  #endif /* FEATURE_WCDMA_PLT */
  
    rsp_ptr->page_stats.drx_pich_demod_error = 
                                   drx_parms.num_zero_pisum + drx_parms.num_po_overruns;
    rsp_ptr->page_stats.drx_cycle_len = drx_parms.drx_cycle_len;
    rsp_ptr->page_stats.num_drx_cycles = drx_parms.total_drx_cycles;

  #ifdef FEATURE_WCDMA_PLT
    #error code not present
#else
    /* Various Intra freq cell reselection statistics */
    rsp_ptr->intra_freq_resel_stats.intra_freq_resel_success_cnt = rrc_debug_get_cel_resel_success_cnt();
    rsp_ptr->intra_freq_resel_stats.intra_freq_nbcch_setup_failure_cnt = rrc_debug_get_cel_resel_nbcch_set_up_fail_cnt();
    rsp_ptr->intra_freq_resel_stats.intra_freq_sib_wait_timeout_cnt = rrc_debug_get_cel_resel_sib_wait_timeout_cnt();
    rsp_ptr->intra_freq_resel_stats.intra_freq_transition_fail_cnt = rrc_debug_get_cel_resel_transition_fail_cnt();
    rsp_ptr->intra_freq_resel_stats.intra_freq_failed_other_causes_cnt = rrc_debug_get_cel_resel_failed_other_causes_cnt();
  
    /* Various WCDMA to GSM cell reselection statistics */
    rsp_ptr->wtog_resel_stats.wtog_resel_cnt = rrc_debug_get_wtog_cel_resel_cnt();
    rsp_ptr->wtog_resel_stats.wtog_resel_failure_cnt = rrc_debug_get_wtog_cel_resel_fail_cnt();
  
    /* Various OOS related statistics */
    rsp_ptr->oos_stats.oos_idle_cnt = rrc_debug_get_oos_idle_mode_cnt();
    rsp_ptr->oos_stats.oos_connected_cnt = rrc_debug_get_oos_connected_mode_cnt();
  
    /* Various RACH transmission related statistics,This is currently TBD. Its values are set to Zero for now*/
    rsp_ptr->rach_transmission_stats.succful_rrc_tran_retry_cnt = rrc_debug_get_succful_rrc_trans_retry_cnt();
    rsp_ptr->rach_transmission_stats.rrc_retry_fail_due_to_l2_ack_ind = rrc_debug_get_rrc_retry_fail_due_to_l2_ack_ind_cnt();
    rsp_ptr->rach_transmission_stats.rrc_retry_fail_due_to_others = rrc_debug_get_rrc_retry_fail_due_to_others_cnt();
  #endif /* FEATURE_WCDMA_PLT */
  
    /* RACH Transmission Preamble related statistics */
    WCDMA_MEMCPY((void *)(rsp_ptr->rach_transmission_preamble_stats.rach_ack_prmbl_cnt),
                 sizeof(rsp_ptr->rach_transmission_preamble_stats.rach_ack_prmbl_cnt),
                 (void *)rach_ack_prmbl_cnt,
                 sizeof(rach_ack_prmbl_cnt));
    
    rsp_ptr->rach_transmission_preamble_stats.rach_noack_cnt = rach_noack_cnt;
    rsp_ptr->rach_transmission_preamble_stats.rach_nak_prmbl_max_cnt = 
                                                      rach_nak_prmbl_max_cnt;
    rsp_ptr->rach_transmission_preamble_stats.rach_max_prmbl_cnt = 
                                                      rach_max_prmbl_cnt;
    rsp_ptr->rach_transmission_preamble_stats.rach_total_attempts =
                                                      rach_total_attempts;
    rsp_ptr->rach_transmission_preamble_stats.rach_prmbl_detected_max_pwr_dB =
                                                      rach_prmbl_detected_max_pwr_dB;
    rsp_ptr->rach_transmission_preamble_stats.rach_detected_last_prmbl_avg_pwr_dB =
                                                      rach_detected_last_prmbl_avg_pwr_dB;
    rsp_ptr->rach_transmission_preamble_stats.rach_noack_last_prmbl_avg_pwr_dB =
                                                      rach_noack_last_prmbl_avg_pwr_dB;
    rsp_ptr->rach_transmission_preamble_stats.rach_noack_prmbl_max_pwr_dB =
                                                     rach_noack_prmbl_max_pwr_dB;
  
  }
  return rsp_ptr ;

}

/*===========================================================================
Function: wcdmadiag_clear_wcdma_stats 
Description:

  This function processes the WCDMA specific WCDMA_DIAG_WCDMA_CLEAR_STATS_F
  command, which clears the statistics related to Paging recorded in WL1 and RRC.
  This is implemented by setting auxiliary variables to the current values, 
  and then subtracting these values from the actual values, and then returning them.

  This function is called when "send_data 75 4 27 0" is issued from cmd 
  line interface from QXDM

============================================================================*/
PACKED void *wcdmadiag_clear_wcdma_stats (PACKED void* req_pkt, uint16 pkt_len)
{
  DIAG_SUBSYS_WCDMA_WCDMA_DIAG_WCDMA_CLEAR_STATS_F_rsp_type *rsp_ptr;

  const int rsp_len = sizeof(DIAG_SUBSYS_WCDMA_WCDMA_DIAG_WCDMA_CLEAR_STATS_F_rsp_type);

  /* "Clear out" all the paging statistics, by setting reset counters,
  ** Because we dont want to reset the counters themselves. */
#ifndef FEATURE_WCDMA_PLT
  /* RRC doesn't exist in a WPLT build. */
  reset_rrcpg1_num_pages = rrcpg1_get_num_pages();
  reset_rrcpg1_num_pages_matched = rrcpg1_get_num_pages_matched();
#endif /* FEATURE_WCDMA_PLT */
  drx_parms.num_zero_pisum = 0;
  drx_parms.num_po_overruns = 0;
  drx_parms.total_drx_cycles = 0;

  /* Clear out the SFN mismatch counter */
  dl_num_times_sfn_mismatch = 0;

  /* Clear out all reacquisition statistics. */
  memset((void*)&srchzz_reacq_slew_stats, 0x00, sizeof(srchzz_reacq_slew_stats_type));
  memset((void*)&srchzz_reacq_srch_stats, 0x00, sizeof(srchzz_reacq_srch_stats_type));

#ifndef FEATURE_WCDMA_PLT
  /* Clear out all RRC statistics. */
  RRC_DEBUG_CLEAR_ALL_STATS();
#endif /* FEATURE_WCDMA_PLT */

  /* Clear out RACH Transmission Preamble related statistics */
  memset((void*)rach_ack_prmbl_cnt, 0x00, sizeof(rach_ack_prmbl_cnt));
  rach_noack_cnt = 0;
  rach_nak_prmbl_max_cnt =0;
  rach_max_prmbl_cnt =0;
  rach_total_attempts =0;
  rach_prmbl_detected_max_pwr_dB = RACH_STATS_INITIAL_TXAGC;
  rach_noack_prmbl_max_pwr_dB = RACH_STATS_INITIAL_TXAGC;
  rach_detected_last_prmbl_avg_pwr_dB = 0;
  rach_noack_last_prmbl_avg_pwr_dB = 0;
  rach_detected_last_prmbl_tot_pwr_dB = 0;
  rach_noack_last_prmbl_tot_pwr_dB = 0;
  rach_ack_cnt = 0;
  rach_nack_cnt = 0;
  
  /* Allocate the memory for this unused response item. */
  rsp_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_DIAG_WCDMA_CLEAR_STATS_F_rsp_type *)
              diagpkt_subsys_alloc(DIAG_SUBSYS_WCDMA, WCDMA_DIAG_WCDMA_CLEAR_STATS_F,
                                   rsp_len );

  return rsp_ptr ;
}

/*===========================================================================
Function: wcdmadiag_reacq_srch_stats
Description:

  This function processes the WCDMA specific WCDMA_DIAG_REACQ_SRCH_STATS_F
  command, which requests the statistics on the DRX reacquisition successes
  and failures, for both list and 1/2/3 searches.

============================================================================*/
PACKED void *wcdmadiag_reacq_srch_stats (PACKED void* req_pkt, uint16 pkt_len)
{
  DIAG_SUBSYS_WCDMA_WCDMA_DIAG_REACQ_SRCH_STATS_F_rsp_type *rsp_ptr;

  const int rsp_len = sizeof(DIAG_SUBSYS_WCDMA_WCDMA_DIAG_REACQ_SRCH_STATS_F_rsp_type);

  /* Allocate the memory for this */
  rsp_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_DIAG_REACQ_SRCH_STATS_F_rsp_type *)diagpkt_subsys_alloc(DIAG_SUBSYS_WCDMA, WCDMA_DIAG_REACQ_SRCH_STATS_F,
                                 rsp_len );

  if (!rsp_ptr)
  {
    ERR_FATAL("diagpkt_subsys_alloc(%d, %d, %d) returned NULL!", 
              DIAG_SUBSYS_WCDMA, WCDMA_DIAG_REACQ_SRCH_STATS_F, rsp_len);
  }
  else
  {
    rsp_ptr->srchzz_reacq_srch_stats = srchzz_reacq_srch_stats;
  }

  return rsp_ptr ;

}

/*===========================================================================
Function: wcdmadiag_reacq_clear_stats
Description:

  This function processes the WCDMA specific WCDMA_DIAG_REACQ_CLEAR_STATS_F
  command, which clears all the statistics on the DRX reacquisition successes
  and failures, for both list and 1/2/3 searches.

============================================================================*/
PACKED void *wcdmadiag_reacq_clear_stats (PACKED void* req_pkt, uint16 pkt_len)
{
  DIAG_SUBSYS_WCDMA_WCDMA_DIAG_REACQ_CLEAR_STATS_F_rsp_type *rsp_ptr;

  const int rsp_len = sizeof(DIAG_SUBSYS_WCDMA_WCDMA_DIAG_REACQ_CLEAR_STATS_F_rsp_type);

  /* Clear out all reacquisition statistics. */
  memset((void*)&srchzz_reacq_slew_stats, 0x00, sizeof(srchzz_reacq_slew_stats_type));
  memset((void*)&srchzz_reacq_srch_stats, 0x00, sizeof(srchzz_reacq_srch_stats_type));

  /* Allocate the memory for this unused response item. */
  rsp_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_DIAG_REACQ_CLEAR_STATS_F_rsp_type *)diagpkt_subsys_alloc(DIAG_SUBSYS_WCDMA, WCDMA_DIAG_REACQ_CLEAR_STATS_F,
                                 rsp_len );

  return rsp_ptr ;

}

/*===========================================================================
Function: wcdmadiag_set_trk_lo_pdm
Description:

  This function processes the WCDMA specific WCDMA_DIAG_REACQ_CLEAR_STATS_F
  command, which freezes and sets the trk_lo_adj PDM to a specific value.

============================================================================*/
PACKED void *wcdmadiag_set_trk_lo_pdm (PACKED void* req_pkt, uint16 pkt_len)
{
  DIAG_SUBSYS_WCDMA_WCDMA_DIAG_SET_TRK_LO_PDM_F_req_type *req_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_DIAG_SET_TRK_LO_PDM_F_req_type *)req_pkt;
  DIAG_SUBSYS_WCDMA_WCDMA_DIAG_SET_TRK_LO_PDM_F_rsp_type *rsp_ptr;

  const int rsp_len = sizeof(DIAG_SUBSYS_WCDMA_WCDMA_DIAG_SET_TRK_LO_PDM_F_rsp_type);

#ifdef FEATURE_WCDMA_TCXO_RESOURCE_SUPPORT
  /* Freeze trk_lo_adj PDM and set it to a specific value. */
  wl1_freeze_and_set_trk_lo_pdm(req_ptr->tcxo_pdm_val);
#endif

  /* Allocate the memory for this unused response item. */
  rsp_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_DIAG_SET_TRK_LO_PDM_F_rsp_type *)diagpkt_subsys_alloc(DIAG_SUBSYS_WCDMA, WCDMA_DIAG_SET_TRK_LO_PDM_F,
                                 rsp_len);
  return rsp_ptr ;
}

/*===========================================================================
Function: wcdmadiag_call_orig
Description:

       This function processes the wcdma specific call orig request packet
       from QXDM and sends the response back.

============================================================================*/

PACKED void *wcdmadiag_call_orig (PACKED void* req_pkt, uint16 pkt_len)
{
  DIAG_SUBSYS_WCDMA_WCDMA_DIAG_ORIG_F_rsp_type *rsp_ptr;

#ifdef FEATURE_CM
  DIAG_SUBSYS_WCDMA_WCDMA_DIAG_ORIG_F_req_type *req_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_DIAG_ORIG_F_req_type *)req_pkt;
  cm_num_s_type                numbers_buf;
  uint8                        work_buf[CM_MAX_NUMBER_CHARS];
  uint8                        i;
  cm_gw_cs_orig_params_s_type  call_orig_params;
  cm_num_s_type                calling_num;
  cm_orig_alpha_s_type         alpha_buf;
#endif /* FEATURE_CM */

  const int rsp_len = sizeof(DIAG_SUBSYS_WCDMA_WCDMA_DIAG_ORIG_F_rsp_type);

  /* Allocate the memory for this */
  rsp_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_DIAG_ORIG_F_rsp_type *)diagpkt_subsys_alloc(DIAG_SUBSYS_WCDMA, WCDMA_DIAG_ORIG_F,
                                 rsp_len );

#ifdef FEATURE_CM
  // Initialize the buffer to CM */
  memset(&call_orig_params,
         CM_CALL_CMD_PARAM_DEFAULT_VALUE,
         sizeof(call_orig_params));

  if ( (req_ptr->num_digits > WCDMA_DIAG_MAX_WCDMA_DIGITS)
       || (req_ptr->num_digits > CM_MAX_NUMBER_CHARS) )
  {
    WL1_MSG0(ERROR, "Dialed too many digits");
    return rsp_ptr;
  }

  // initialize work_buf
  memset (work_buf, 0, ARR_SIZE(work_buf));

  for (i=0; i<req_ptr->num_digits; i++) {
      /* Check for dial digits (0 - 9, *, #) */
      if ( ((req_ptr->digits[i] < '0') || (req_ptr->digits[i] > '9')) &&
           (req_ptr->digits[i] != '*')  &&
           (req_ptr->digits[i] != '#')) {
          /* Was an invalid digit (not 0 - 9, *, #) */
          WL1_MSG1(ERROR, "Bad dial digit: %c", req_ptr->digits[i]);
          return rsp_ptr;
      }
      work_buf[i] = req_ptr->digits[i];
  }

  alpha_buf.len = strlen("DIAG CALL");
  WCDMA_MEMCPY(alpha_buf.buf,
               sizeof(uint8) * CM_MAX_ALPHA_LEN_FOR_CALL_ORIG,
               (const byte *)"DIAG CALL",
               alpha_buf.len+1);

  cm_num_init( &calling_num );

  cm_num_fill(&numbers_buf, work_buf, req_ptr->num_digits, CM_DIGIT_MODE_4BIT_DTMF);


  (void) cm_mm_call_cmd_orig(
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
      &call_id
      );

#endif /* FEATURE_CM */

  return rsp_ptr ;

}

/*===========================================================================
Function: wcdmadiag_call_end
Description:

       This function processes the wcdma specific call end request packet
       from QXDM and sends the response back.

============================================================================*/

PACKED void *wcdmadiag_call_end (PACKED void* req_pkt, uint16 pkt_len)
{
  DIAG_SUBSYS_WCDMA_WCDMA_DIAG_END_F_rsp_type *rsp_ptr;


#ifdef FEATURE_CM
  cm_end_params_s_type end_params;
  uint8 num_call_ids = 0;
  int i;
#endif

  const int rsp_len = sizeof(DIAG_SUBSYS_WCDMA_WCDMA_DIAG_END_F_rsp_type);

  /* Allocate the memory for this */
  rsp_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_DIAG_END_F_rsp_type *)diagpkt_subsys_alloc(DIAG_SUBSYS_WCDMA, WCDMA_DIAG_END_F,
                                 rsp_len );

#ifdef FEATURE_CM
  memset( &end_params,
          CM_CALL_CMD_PARAM_DEFAULT_VALUE,
          sizeof ( cm_end_params_s_type));


  /* Among active calls, the first active call returned by cm_call_id_is_allocated() is ended */
  for (i = 0; i < CM_CALL_ID_MAX; i++)
  {
    if ( cm_call_id_is_allocated ((cm_call_id_type) i) )
    {

      num_call_ids++;
      end_params.call_id = (cm_call_id_type) i;
      end_params.info_type = CM_CALL_MODE_INFO_GW_CS;
      WL1_MSG1(HIGH , "WCMDADIAG::END_REQ: Adding call_id %d to the end list",
                      end_params.call_id);

      break;
    } /* if ( cm_call_id_is_allocated ((cm_call_id_type) i) ) */
  } /* for (i = 0; i < CM_CALL_ID_MAX; i++) */



  if (num_call_ids > 0)
  {
    (void) cm_mm_call_cmd_end(
      NULL, /* client callback function */
      NULL, /* pointer to client callback data block */
      CM_CLIENT_ID_ANONYMOUS,
      1,
      &end_params
      );
  }
#endif /* FEATURE_CM */
  return rsp_ptr ;

}

/*===========================================================================
Function: wcdmadiag_status
Description:

       This function processes the wcdma specific status request packet
       from QXDM and sends the response back.

============================================================================*/

PACKED void *wcdmadiag_status (PACKED void* req_pkt, uint16 pkt_len)
{
  DIAG_SUBSYS_WCDMA_WCDMA_STATUS_F_req_type *req_ptr;

  req_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_STATUS_F_req_type *) req_pkt;

  if (diagpkt_get_cmd_code( req_ptr ) == DIAG_SUBSYS_CMD_F)
  {
    /* This is the legacy WCDMA polling command */
    DIAG_SUBSYS_WCDMA_WCDMA_STATUS_F_rsp_type *rsp_ptr;
    const int rsp_len = sizeof(DIAG_SUBSYS_WCDMA_WCDMA_STATUS_F_rsp_type);
    sys_oprt_mode_e_type cm_state; /* cm state */
    uint8 tmc_state_return;

    /* Allocate the memory for this */
    rsp_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_STATUS_F_rsp_type *)diagpkt_subsys_alloc(DIAG_SUBSYS_WCDMA, WCDMA_STATUS_F,
                                   rsp_len);

    if (!rsp_ptr)
    {
      ERR_FATAL("diagpkt_subsys_alloc(%d, %d, %d) returned NULL!", 
                DIAG_SUBSYS_WCDMA, WCDMA_STATUS_F, rsp_len);
    }
    else
    {
      /* Fill the response packet */
      cm_state = cm_ph_get_oprt_mode();
  
      switch (cm_state)
      {
      case SYS_OPRT_MODE_OFFLINE:
      case SYS_OPRT_MODE_OFFLINE_AMPS:
      case SYS_OPRT_MODE_OFFLINE_CDMA:
          tmc_state_return = TMC_STATE_OFFLINE;
          break;
      case SYS_OPRT_MODE_LPM:
      case SYS_OPRT_MODE_ONLINE:
      case SYS_OPRT_MODE_NET_TEST_GW:
      case SYS_OPRT_MODE_PSEUDO_ONLINE:
          tmc_state_return = TMC_STATE_ONLINE;
          break;
  
      default:
          tmc_state_return = TMC_STATE_UNKNOWN;
          break;
      }
  
      rsp_ptr->tmc_state = tmc_state_return;
    }

    return rsp_ptr;
  }
  else
  {
  DIAG_SUBSYS_WCDMA_WCDMA_STATUS_rsp_type *rsp_ptr;

    /*  Allocate our packet */
  rsp_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_STATUS_rsp_type *) diagpkt_subsys_alloc_v2 (
    DIAG_SUBSYS_WCDMA,
    DIAG_WCDMA_MAX_F,
    sizeof (DIAG_SUBSYS_WCDMA_WCDMA_STATUS_rsp_type) );

    if(rsp_ptr == NULL)
    {
        WL1_MSG0(ERROR, "wcdmadiag_status: unable to alloc mem for rsp");
    }
    else
    {
      diagpkt_subsys_reset_delayed_rsp_id(rsp_ptr);
      rsp_ptr->status = (uint8)3;
      rsp_ptr->value = 0;

      /* Set the first (0) response count */
      diagpkt_subsys_set_rsp_cnt(rsp_ptr, 0);

      /* Set the status in the response to SUCCESS */
      diagpkt_subsys_set_status(rsp_ptr, 0);
    }

    return rsp_ptr ;
  }
}



/*===========================================================================
Function: wcdmadiag_additional_status
Description:

       This function processes the wcdma specific additional status request
       packet from QXDM and sends the response back.

============================================================================*/
PACKED void *wcdmadiag_additional_status (PACKED void* req_pkt, uint16 pkt_len)
{
  DIAG_SUBSYS_WCDMA_WCDMA_ADDITIONAL_STATUS_F_rsp_type *rsp_ptr ;

  const int rsp_len = sizeof(DIAG_SUBSYS_WCDMA_WCDMA_ADDITIONAL_STATUS_F_rsp_type);

#ifdef FEATURE_DUAL_SIM
  /* Retrieve Access Stratum ID in use by WCDMA RAT. */
  sys_modem_as_id_e_type  as_id = rrc_get_as_id();


  if (IS_NOT_VALID_AS_ID(as_id))
  {
    WL1_MSG1(ERROR, "Invalid AS_ID:%d", as_id);
  }
#endif

  /* Allocate the memory for this */
  rsp_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_ADDITIONAL_STATUS_F_rsp_type *)diagpkt_subsys_alloc(DIAG_SUBSYS_WCDMA, WCDMA_ADDITIONAL_STATUS_F,
                                 rsp_len );

  if (!rsp_ptr)
  {
    ERR_FATAL("diagpkt_subsys_alloc(%d, %d, %d) returned NULL!", 
              DIAG_SUBSYS_WCDMA, WCDMA_ADDITIONAL_STATUS_F, rsp_len);
  }
  else
  {
#ifdef FEATURE_WCDMA_PLT
    #error code not present
#else

    /* IMEI/IMSI retrieval depends on dual-SIM features. */
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_IMEI)
    if (IS_NOT_VALID_AS_ID(as_id))
    {
      memset( (byte*)rsp_ptr->imei, 0, sizeof(rsp_ptr->imei) );
    }
    else
    {
      (void)ghdi_ds_read_imei((byte *)rsp_ptr->imei, as_id);
    }
#else
    (void)ghdi_read_imei((byte *)rsp_ptr->imei);
#endif

#ifdef FEATURE_DUAL_SIM
    if (IS_NOT_VALID_AS_ID(as_id))
    {
      memset( (byte*)rsp_ptr->imsi, 0, sizeof(rsp_ptr->imsi) );
    }
    else
    {
      mm_ds_get_imsi((imsi_data_T*)&rsp_ptr->imsi[0], as_id);
    }
#else
    mm_get_imsi((imsi_data_T*)&rsp_ptr->imsi[0]);
#endif

#endif  /* #ifdef FEATURE_WCDMA_PLT */
  
    rsp_ptr->l1_state = l1m_get_state();
  }

  return rsp_ptr ;

}

#ifdef FEATURE_MBMS
#error code not present
#endif 

#ifdef FEATURE_WCDMA_TBSET_DEBUG_DUMP
#error code not present
#endif 

#ifdef FEATURE_WCDMA_MIMO
/*===========================================================================

FUNCTION wcdmadiag_wcdma_mimo_params_handler

DESCRIPTION
  This function handles the request to manually set MIMO params.

============================================================================*/
PACKED void * wcdmadiag_wcdma_mimo_params_handler(
   PACKED void *req_ptr,
   uint16       pkt_len
)
{
   DIAG_SUBSYS_WCDMA_WCDMA_MIMO_PARAMS_F_req_type *config_req_ptr;
   DIAG_SUBSYS_WCDMA_WCDMA_MIMO_PARAMS_F_rsp_type *config_rsp_ptr;

   config_req_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_MIMO_PARAMS_F_req_type *) req_ptr;

   /* A basic error checking on the received values */
   if (config_req_ptr->mimo_param_type > 5)
   {
      return diagpkt_err_rsp(
         DIAG_BAD_PARM_F,
         config_req_ptr,
         sizeof(DIAG_SUBSYS_WCDMA_WCDMA_MIMO_PARAMS_F_req_type)
         );
   }

   /* Prepare the response packet */
   config_rsp_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_MIMO_PARAMS_F_rsp_type *)
                         diagpkt_alloc(
                            diagpkt_get_cmd_code( req_ptr ),
                            sizeof(DIAG_SUBSYS_WCDMA_WCDMA_MIMO_PARAMS_F_rsp_type)
                         );

   if ( config_rsp_ptr == NULL )
   {
      /* Allocation failed, and diagpkt_err_rsp will fail as well.
      ** So just return NULL.
      */
      return NULL;
   }

   /* Copy the header from the request packet to the response packet */
   WCDMA_MEMCPY((void*)config_rsp_ptr,
                sizeof(DIAG_SUBSYS_WCDMA_WCDMA_MIMO_PARAMS_F_rsp_type),
                (void*)config_req_ptr,
                sizeof(diagpkt_subsys_header_type));
      
   /* Generate the response */
   config_rsp_ptr->status = 1;
   #ifdef FEATURE_QDSP6
   /* Call the HS function that will take the values from diag request packet
      and update the CQI offset variables */
   hs_set_mimo_user_params(
         config_req_ptr->mimo_param_type,
         config_req_ptr->mimo_param_val);
   #endif
   return (void *) config_rsp_ptr;
}
#endif

#ifdef FEATURE_HSDPA
/*===========================================================================

FUNCTION wcdmadiag_wcdma_cqi_params_handler

DESCRIPTION
  This function handles the request to manually set cqi params.

============================================================================*/
PACKED void * wcdmadiag_wcdma_cqi_params_handler(
   PACKED void *req_ptr,
   uint16       pkt_len
)
{
   DIAG_SUBSYS_WCDMA_WCDMA_CQI_OVERIDE_PARAMS_F_req_type *config_req_ptr;
   DIAG_SUBSYS_WCDMA_WCDMA_CQI_OVERIDE_PARAMS_F_rsp_type *config_rsp_ptr;

   config_req_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_CQI_OVERIDE_PARAMS_F_req_type *) req_ptr;

   /* A basic error checking on the received values */
   if (config_req_ptr->override > 1)
   {
      return diagpkt_err_rsp(
         DIAG_BAD_PARM_F,
         config_req_ptr,
         sizeof(DIAG_SUBSYS_WCDMA_WCDMA_CQI_OVERIDE_PARAMS_F_req_type)
         );
   }

   /* Prepare the response packet */
   config_rsp_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_CQI_OVERIDE_PARAMS_F_rsp_type *)
                         diagpkt_alloc(
                            diagpkt_get_cmd_code( req_ptr ),
                            sizeof(DIAG_SUBSYS_WCDMA_WCDMA_CQI_OVERIDE_PARAMS_F_rsp_type)
                         );

   if ( config_rsp_ptr == NULL )
   {
      /* Allocation failed, and diagpkt_err_rsp will fail as well.
      ** So just return NULL.
      */
      return NULL;
   }

   /* Copy the header from the request packet to the response packet */
   WCDMA_MEMCPY((void*)config_rsp_ptr,
                sizeof(DIAG_SUBSYS_WCDMA_WCDMA_CQI_OVERIDE_PARAMS_F_rsp_type),
                (void*)config_req_ptr,
                sizeof(diagpkt_subsys_header_type));
      
   /* Generate the response */
   config_rsp_ptr->status = 1;
  #ifdef FEATURE_QDSP6 
  enchs_set_cqi_override_from_diag(
    config_req_ptr->override,
    config_req_ptr->repetition_cycle_repeat_count,
    config_req_ptr->pattern_len,
    config_req_ptr->test_pattern);
  #endif
   return (void *) config_rsp_ptr;

} 
#endif

/*===========================================================================

FUNCTION wcdmadiag_wcdma_fw_debug_cmd_handler

DESCRIPTION
  This function handles the request to send debug cmd to WCDMA FW.

============================================================================*/
PACKED void * wcdmadiag_wcdma_fw_debug_cmd_handler(
   PACKED void *req_ptr,
   uint16       pkt_len
)
{
   DIAG_SUBSYS_WCDMA_WCDMA_FW_DEBUG_CMD_F_req_type *config_req_ptr;
   DIAG_SUBSYS_WCDMA_WCDMA_FW_DEBUG_CMD_F_rsp_type *config_rsp_ptr;

   config_req_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_FW_DEBUG_CMD_F_req_type *) req_ptr;

   /* Prepare the response packet */
   config_rsp_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_FW_DEBUG_CMD_F_rsp_type *)
                         diagpkt_alloc(
                            diagpkt_get_cmd_code( req_ptr ),
                            sizeof(DIAG_SUBSYS_WCDMA_WCDMA_FW_DEBUG_CMD_F_rsp_type)
                         );

   if ( config_rsp_ptr == NULL )
   {
      /* Allocation failed, and diagpkt_err_rsp will fail as well.
      ** So just return NULL.
      */
      return NULL;
   }

   /* Copy the header from the request packet to the response packet */
   WCDMA_MEMCPY((void*)config_rsp_ptr,
                sizeof(DIAG_SUBSYS_WCDMA_WCDMA_FW_DEBUG_CMD_F_rsp_type),
                (void*)config_req_ptr,
                sizeof(diagpkt_subsys_header_type));
      
   /* update debug cmd packet to WCDMA FW */
   mdspasync_update_wfw_debug_cmd(config_req_ptr->debug_pkt);

   /* Generate the response */
   config_rsp_ptr->status = 1;

   /* Copy the header from the request packet to the response packet */
   WCDMA_MEMCPY((void*)config_rsp_ptr->debug_pkt,
                sizeof(config_rsp_ptr->debug_pkt),
                (void*)config_req_ptr->debug_pkt,
                sizeof(config_req_ptr->debug_pkt));

   return (void *) config_rsp_ptr;
} 

#ifdef FEATURE_CPU_BASED_FLOW_CONTROL
/*===========================================================================

FUNCTION WCDMADIAG_RLC_FLOW_CONTROL_CONFIG_HANDLER

DESCRIPTION
  This function handles the request to RLC Flow Controller thresholds

============================================================================*/
PACKED void * wcdmadiag_rlc_flow_control_config_handler(
   PACKED void *req_ptr,
   uint16       pkt_len
)
{
   DIAG_SUBSYS_WCDMA_WCDMA_RLC_FLOW_CONTROL_CONFIG_F_req_type *config_req_ptr;
   DIAG_SUBSYS_WCDMA_WCDMA_RLC_FLOW_CONTROL_CONFIG_F_rsp_type *config_rsp_ptr;

   config_req_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_RLC_FLOW_CONTROL_CONFIG_F_req_type *) req_ptr;

   /* skip parameters validation */

   config_rsp_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_RLC_FLOW_CONTROL_CONFIG_F_rsp_type *)
                         diagpkt_alloc(
                            diagpkt_get_cmd_code( req_ptr ),
                            sizeof(DIAG_SUBSYS_WCDMA_WCDMA_RLC_FLOW_CONTROL_CONFIG_F_rsp_type)
                         );

   if ( config_rsp_ptr == NULL )
   {
      /* Allocation failed, and diagpkt_err_rsp will fail as well.
      ** So just return NULL.
      */
      return NULL;
   }

   WCDMA_MEMCPY((void*)config_rsp_ptr,
                sizeof(DIAG_SUBSYS_WCDMA_WCDMA_RLC_FLOW_CONTROL_CONFIG_F_rsp_type),
                (void*)config_req_ptr,
                sizeof(diagpkt_subsys_header_type));

   config_rsp_ptr->status = 1;

   #ifdef FEATURE_UL_CPU_BASED_FC
   rlc_ul_set_fc_config(config_req_ptr->ws_step_period,
                     config_req_ptr->ws_step_stage,
                     config_req_ptr->init_ws);
   #endif

   #ifdef FEATURE_DL_CPU_BASED_FC
   rlc_dl_set_fc_config(config_req_ptr->ws_step_period,
                     config_req_ptr->ws_step_stage,
                     config_req_ptr->init_ws);
   #endif
   return (void *) config_rsp_ptr;

} /* rex_diag_taskprofile_handler */
#endif /* FEATURE_CPU_BASED_FLOW_CONTROL */

#ifdef FEATURE_WCDMA_HSUPA
/*===========================================================================

FUNCTION  wcdmadiag_manual_eul_ul_var_upd_handler

DESCRIPTION
  This function handles request to manually update eul uplink module variables.
  Right now this handler is used to enable or disable eul pwr restriction.

============================================================================*/
PACKED void * wcdmadiag_manual_eul_ul_var_upd_handler(
   PACKED void *req_ptr,
   uint16       pkt_len
)
{
   DIAG_SUBSYS_WCDMA_WCDMA_DIAG_EUL_UL_VAR_UPD_F_req_type *upd_req_ptr;
   DIAG_SUBSYS_WCDMA_WCDMA_DIAG_EUL_UL_VAR_UPD_F_rsp_type *upd_rsp_ptr;

   upd_req_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_DIAG_EUL_UL_VAR_UPD_F_req_type *) req_ptr;

   /* Prepare the response packet */
   upd_rsp_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_DIAG_EUL_UL_VAR_UPD_F_rsp_type *)
                         diagpkt_alloc(
                            diagpkt_get_cmd_code( req_ptr ),
                            sizeof(DIAG_SUBSYS_WCDMA_WCDMA_DIAG_EUL_UL_VAR_UPD_F_rsp_type)
                         );

   if ( upd_rsp_ptr == NULL )
   {
      /* Allocation failed, and diagpkt_err_rsp will fail as well.
      ** So just return NULL.
      */
      return NULL;
   }

   /* Copy the header from the request packet to the response packet */
   WCDMA_MEMCPY((void*)upd_rsp_ptr,
                sizeof(DIAG_SUBSYS_WCDMA_WCDMA_DIAG_EUL_UL_VAR_UPD_F_rsp_type),
                (void*)upd_req_ptr,
                sizeof(diagpkt_subsys_header_type));
   
   /* Generate the response */
   upd_rsp_ptr->status = 1;

   /* Call the eul uplink function that will take the values from diag request packet
      and update the variable */
   eul_set_var_through_qxdm(
         upd_req_ptr->debug_enable_f3_message_type,upd_req_ptr->debug_var_val);

   return (void *) upd_rsp_ptr;

} 
#endif /*FEATURE_WCDMA_HSUPA*/

#ifdef FEATURE_WCDMA_DC_HSDPA
/*===========================================================================

FUNCTION  wcdmadiag_manual_setup_dc_test_cfg

DESCRIPTION
  This function takes 
  - Enable/Disable
  - PSC
  - FREQ
  as inputs and calls WL1 function that tunes RF as per the input argument.

============================================================================*/
PACKED void * wcdmadiag_manual_setup_dc_test_cfg(
   PACKED void *req_ptr,
   uint16       pkt_len
)
{
   DIAG_SUBSYS_WCDMA_WCDMA_DIAG_DC_HS_SETUP_req_type *upd_req_ptr;
   DIAG_SUBSYS_WCDMA_WCDMA_DIAG_DC_HS_SETUP_rsp_type *upd_rsp_ptr;

   upd_req_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_DIAG_DC_HS_SETUP_req_type *) req_ptr;

   /* Prepare the response packet */
   upd_rsp_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_DIAG_DC_HS_SETUP_rsp_type *)
                         diagpkt_alloc(
                            diagpkt_get_cmd_code( req_ptr ),
                            sizeof(DIAG_SUBSYS_WCDMA_WCDMA_DIAG_DC_HS_SETUP_rsp_type)
                         );

   if ( upd_rsp_ptr == NULL )
   {
      /* Allocation failed, and diagpkt_err_rsp will fail as well.
      ** So just return NULL.
      */
      return NULL;
   }

   /* Copy the header from the request packet to the response packet */
   WCDMA_MEMCPY((void*)upd_rsp_ptr,
                sizeof(DIAG_SUBSYS_WCDMA_WCDMA_DIAG_DC_HS_SETUP_rsp_type),
                (void*)upd_req_ptr,
                sizeof(diagpkt_subsys_header_type));
   
   /* Generate the response */

   if ( upd_req_ptr->enable > 1 )
   {
     WL1_MSG1(ERROR, "Incorrect op_type %d", upd_req_ptr->enable);
     upd_rsp_ptr->status = 0;
   }
   else
   {
     upd_rsp_ptr->status = 
       wl1_dl_carrcfg_setup_debug_dc_cfg(upd_req_ptr->dc_psc, upd_req_ptr->dc_freq, upd_req_ptr->enable);
   }

   return (void *) upd_rsp_ptr;
} 
#endif /*FEATURE_WCDMA_HSUPA*/

/*===========================================================================

FUNCTION  wcdmadiag_qice_oride_table_params_handler

DESCRIPTION
  This function handles request to override QICE table parameters.

============================================================================*/
PACKED void * wcdmadiag_qice_oride_table_params_handler(
   PACKED void *req_ptr,
   uint16       pkt_len
)
{
   DIAG_SUBSYS_WCDMA_WCDMA_QICE_ORIDE_TABLE_PARAMS_F_req_type * oride_req_ptr;
   DIAG_SUBSYS_WCDMA_WCDMA_QICE_ORIDE_TABLE_PARAMS_F_rsp_type * oride_rsp_ptr;

   oride_req_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_QICE_ORIDE_TABLE_PARAMS_F_req_type *) req_ptr;

   /* Prepare the response packet */
   oride_rsp_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_QICE_ORIDE_TABLE_PARAMS_F_rsp_type *)
     diagpkt_alloc(diagpkt_get_cmd_code(req_ptr),
                   sizeof(DIAG_SUBSYS_WCDMA_WCDMA_QICE_ORIDE_TABLE_PARAMS_F_rsp_type));

   if (oride_rsp_ptr == NULL)
   {
     /* Allocation failed, and diagpkt_err_rsp will fail as well.
      * So just return NULL.
      */
     return NULL;
   }
   
   /* Copy the header from the request packet to the response packet */
   WCDMA_MEMCPY((void *)oride_rsp_ptr,
                sizeof(DIAG_SUBSYS_WCDMA_WCDMA_QICE_ORIDE_TABLE_PARAMS_F_rsp_type),
                (void *)oride_req_ptr,
                sizeof(diagpkt_subsys_header_type));
   
   /* Generate the response */
   oride_rsp_ptr->status = wl1_cme_proc_wplt_qice_oride_table_params_cmd(oride_req_ptr->carrier,
                                                                         oride_req_ptr->tanh_table,
                                                                         oride_req_ptr->sqrt_table_1,
                                                                         oride_req_ptr->sqrt_table_2,
                                                                         oride_req_ptr->param_table);

   return (void *) oride_rsp_ptr;
}

/*===========================================================================

FUNCTION  wcdmadiag_qice_oride_config_params_handler

DESCRIPTION
  This function handles request to override QICE config parameters.

============================================================================*/
PACKED void * wcdmadiag_qice_oride_config_params_handler(
   PACKED void *req_ptr,
   uint16       pkt_len
)
{
   DIAG_SUBSYS_WCDMA_WCDMA_QICE_ORIDE_CONFIG_PARAMS_F_req_type * oride_req_ptr;
   DIAG_SUBSYS_WCDMA_WCDMA_QICE_ORIDE_CONFIG_PARAMS_F_rsp_type * oride_rsp_ptr;

   oride_req_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_QICE_ORIDE_CONFIG_PARAMS_F_req_type *) req_ptr;

   /* Prepare the response packet */
   oride_rsp_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_QICE_ORIDE_CONFIG_PARAMS_F_rsp_type *)
     diagpkt_alloc(diagpkt_get_cmd_code(req_ptr),
                   sizeof(DIAG_SUBSYS_WCDMA_WCDMA_QICE_ORIDE_CONFIG_PARAMS_F_rsp_type));

   if (oride_rsp_ptr == NULL)
   {
     /* Allocation failed, and diagpkt_err_rsp will fail as well.
      * So just return NULL.
      */
     return NULL;
   }
   
   /* Copy the header from the request packet to the response packet */
   WCDMA_MEMCPY((void *) oride_rsp_ptr,
                sizeof(DIAG_SUBSYS_WCDMA_WCDMA_QICE_ORIDE_CONFIG_PARAMS_F_rsp_type),
                (void *) oride_req_ptr,
                sizeof(diagpkt_subsys_header_type));
   
   /* Generate the response */
   oride_rsp_ptr->status = wl1_cme_proc_wplt_qice_oride_config_params_cmd(oride_req_ptr->carrier,
                                                                          oride_req_ptr->commit_margin_cx2,
                                                                          oride_req_ptr->sfc_energy_thresh,
                                                                          oride_req_ptr->sfc_hw_sum_alpha,
                                                                          oride_req_ptr->sfc_ratio_alpha,
                                                                          oride_req_ptr->sfc_ratio_thresh,
                                                                          oride_req_ptr->mmse_weight_diag,
                                                                          oride_req_ptr->mmse_weight_cross,
                                                                          oride_req_ptr->noise_inflation,
                                                                          oride_req_ptr->iterations,
                                                                          oride_req_ptr->bank_switch_delay,
                                                                          oride_req_ptr->sfc_bpg_per_dump,
                                                                          oride_req_ptr->sfc_dump_per_ratio,
                                                                          oride_req_ptr->sfc_ratio_per_classify,
                                                                          oride_req_ptr->sf16_active_subframes,
                                                                          oride_req_ptr->idle_windows);

   return (void *) oride_rsp_ptr;
}

/*===========================================================================

FUNCTION  wcdmadiag_qice_oride_fht_params_handler

DESCRIPTION
  This function handles request to override QICE FHT parameters.

============================================================================*/
PACKED void * wcdmadiag_qice_oride_fht_params_handler(
   PACKED void *req_ptr,
   uint16       pkt_len
)
{
   DIAG_SUBSYS_WCDMA_WCDMA_QICE_ORIDE_FHT_PARAMS_F_req_type * oride_req_ptr;
   DIAG_SUBSYS_WCDMA_WCDMA_QICE_ORIDE_FHT_PARAMS_F_rsp_type * oride_rsp_ptr;

   oride_req_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_QICE_ORIDE_FHT_PARAMS_F_req_type *) req_ptr;

   /* Prepare the response packet */
   oride_rsp_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_QICE_ORIDE_FHT_PARAMS_F_rsp_type *)
     diagpkt_alloc(diagpkt_get_cmd_code(req_ptr),
                   sizeof(DIAG_SUBSYS_WCDMA_WCDMA_QICE_ORIDE_FHT_PARAMS_F_rsp_type));

   if (oride_rsp_ptr == NULL)
   {
     /* Allocation failed, and diagpkt_err_rsp will fail as well.
      * So just return NULL.
      */
     return NULL;
   }
   
   /* Copy the header from the request packet to the response packet */
   WCDMA_MEMCPY((void *) oride_rsp_ptr,
                sizeof(DIAG_SUBSYS_WCDMA_WCDMA_QICE_ORIDE_FHT_PARAMS_F_rsp_type),
                (void *) oride_req_ptr,
                sizeof(diagpkt_subsys_header_type));
   
   /* Generate the response */
   oride_rsp_ptr->status = wl1_cme_proc_wplt_qice_oride_fht_params_cmd(oride_req_ptr->carrier,
                                                                       oride_req_ptr->psc,
                                                                       oride_req_ptr->fht256,
                                                                       oride_req_ptr->fht16);

   return (void *) oride_rsp_ptr;
}

/*===========================================================================

FUNCTION  wcdmadiag_qice_oride_fw_params_handler

DESCRIPTION
  This function handles request to override QICE FW parameters.

============================================================================*/
PACKED void * wcdmadiag_qice_oride_fw_params_handler(
   PACKED void *req_ptr,
   uint16       pkt_len
)
{
   DIAG_SUBSYS_WCDMA_WCDMA_QICE_ORIDE_FW_PARAMS_F_req_type * oride_req_ptr;
   DIAG_SUBSYS_WCDMA_WCDMA_QICE_ORIDE_FW_PARAMS_F_rsp_type * oride_rsp_ptr;

   oride_req_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_QICE_ORIDE_FW_PARAMS_F_req_type *) req_ptr;

   /* Prepare the response packet */
   oride_rsp_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_QICE_ORIDE_FW_PARAMS_F_rsp_type *)
     diagpkt_alloc(diagpkt_get_cmd_code(req_ptr),
                   sizeof(DIAG_SUBSYS_WCDMA_WCDMA_QICE_ORIDE_FW_PARAMS_F_rsp_type));

   if (oride_rsp_ptr == NULL)
   {
     /* Allocation failed, and diagpkt_err_rsp will fail as well.
      * So just return NULL.
      */
     return NULL;
   }
   
   /* Copy the header from the request packet to the response packet */
   WCDMA_MEMCPY((void *) oride_rsp_ptr,
                sizeof(DIAG_SUBSYS_WCDMA_WCDMA_QICE_ORIDE_FW_PARAMS_F_rsp_type),
                (void *) oride_req_ptr,
                sizeof(diagpkt_subsys_header_type));
      
   /* Generate the response */
   oride_rsp_ptr->status = wl1_cme_proc_wplt_qice_oride_fw_params_cmd(oride_req_ptr->carrier,
                                                                      oride_req_ptr->psc,
                                                                      oride_req_ptr->misc_sf16_overrides,
                                                                      oride_req_ptr->sf16_non_linear_override,
                                                                      oride_req_ptr->sf16_non_linear_select,
                                                                      oride_req_ptr->sf16_mode_override,
                                                                      oride_req_ptr->sf16_mode_select);

   return (void *) oride_rsp_ptr;
}

/*===========================================================================

FUNCTION  wcdmadiag_qice_oride_log_params_handler

DESCRIPTION
  This function handles request to override QICE logging parameters.

============================================================================*/
PACKED void * wcdmadiag_qice_oride_log_params_handler(
   PACKED void *req_ptr,
   uint16       pkt_len
)
{
   DIAG_SUBSYS_WCDMA_WCDMA_QICE_ORIDE_LOG_PARAMS_F_req_type * oride_req_ptr;
   DIAG_SUBSYS_WCDMA_WCDMA_QICE_ORIDE_LOG_PARAMS_F_rsp_type * oride_rsp_ptr;

   oride_req_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_QICE_ORIDE_LOG_PARAMS_F_req_type *) req_ptr;

   /* Prepare the response packet */
   oride_rsp_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_QICE_ORIDE_LOG_PARAMS_F_rsp_type *)
     diagpkt_alloc(diagpkt_get_cmd_code(req_ptr),
                   sizeof(DIAG_SUBSYS_WCDMA_WCDMA_QICE_ORIDE_LOG_PARAMS_F_rsp_type));

   if (oride_rsp_ptr == NULL)
   {
     /* Allocation failed, and diagpkt_err_rsp will fail as well.
      * So just return NULL.
      */
     return NULL;
   }
   
   /* Copy the header from the request packet to the response packet */
   WCDMA_MEMCPY((void *) oride_rsp_ptr,
                sizeof(DIAG_SUBSYS_WCDMA_WCDMA_QICE_ORIDE_LOG_PARAMS_F_rsp_type),
                (void *) oride_req_ptr,
                sizeof(diagpkt_subsys_header_type));
   
   /* Generate the response */
   oride_rsp_ptr->status = wl1_cme_proc_wplt_qice_oride_log_params_cmd(oride_req_ptr->carrier,
                                                                       oride_req_ptr->hw_dump_per_log,
                                                                       oride_req_ptr->primary_ovsf,
                                                                       oride_req_ptr->non_primary_ovsf);

   return (void *) oride_rsp_ptr;
}

/*===========================================================================

FUNCTION  wcdmadiag_qice_reset_handler

DESCRIPTION
  This function handles request to reset QICE.

============================================================================*/
PACKED void * wcdmadiag_qice_reset_handler(
   PACKED void *req_ptr,
   uint16       pkt_len
)
{
   DIAG_SUBSYS_WCDMA_WCDMA_QICE_RESET_F_req_type * reset_req_ptr;
   DIAG_SUBSYS_WCDMA_WCDMA_QICE_RESET_F_rsp_type * reset_rsp_ptr;

   reset_req_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_QICE_RESET_F_req_type *) req_ptr;

   /* Prepare the response packet */
   reset_rsp_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_QICE_RESET_F_rsp_type *)
     diagpkt_alloc(diagpkt_get_cmd_code(req_ptr),
                   sizeof(DIAG_SUBSYS_WCDMA_WCDMA_QICE_RESET_F_rsp_type));

   if (reset_rsp_ptr == NULL)
   {
     /* Allocation failed, and diagpkt_err_rsp will fail as well.
      * So just return NULL.
      */
     return NULL;
   }
   
   /* Copy the header from the request packet to the response packet */
   WCDMA_MEMCPY((void *) reset_rsp_ptr,
                sizeof(DIAG_SUBSYS_WCDMA_WCDMA_QICE_RESET_F_rsp_type),
                (void *) reset_req_ptr,
                sizeof(diagpkt_subsys_header_type));
   
   /* Generate the response */
   reset_rsp_ptr->status = wl1_cme_proc_wplt_qice_reset_cmd(reset_req_ptr->carrier,
                                                            reset_req_ptr->reset_bmsk);

   return (void *) reset_rsp_ptr;
}

/*===========================================================================

FUNCTION  wcdmadiag_cme_oride_params_handler

DESCRIPTION
  This function handles request to override CME SW parameters.

============================================================================*/
PACKED void * wcdmadiag_cme_oride_params_handler(
   PACKED void *req_ptr,
   uint16       pkt_len
)
{
   DIAG_SUBSYS_WCDMA_WCDMA_CME_ORIDE_PARAMS_F_req_type * oride_req_ptr;
   DIAG_SUBSYS_WCDMA_WCDMA_CME_ORIDE_PARAMS_F_rsp_type * oride_rsp_ptr;

   oride_req_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_CME_ORIDE_PARAMS_F_req_type *) req_ptr;

   /* Prepare the response packet */
   oride_rsp_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_CME_ORIDE_PARAMS_F_rsp_type *)
     diagpkt_alloc(diagpkt_get_cmd_code(req_ptr),
                   sizeof(DIAG_SUBSYS_WCDMA_WCDMA_CME_ORIDE_PARAMS_F_rsp_type));

   if (oride_rsp_ptr == NULL)
   {
     /* Allocation failed, and diagpkt_err_rsp will fail as well.
      * So just return NULL.
      */
     return NULL;
   }
   
   /* Copy the header from the request packet to the response packet */
   WCDMA_MEMCPY((void *) oride_rsp_ptr,
                sizeof(DIAG_SUBSYS_WCDMA_WCDMA_CME_ORIDE_PARAMS_F_rsp_type),
                (void *) oride_req_ptr,
                sizeof(diagpkt_subsys_header_type));
   
   /* Generate the response */
   oride_rsp_ptr->status = wl1_cme_proc_wplt_cme_oride_params_cmd(oride_req_ptr->carrier,
                                                                  oride_req_ptr->max_num_qset_cells,
                                                                  oride_req_ptr->qset_ecio_add_threshold,
                                                                  oride_req_ptr->qset_ecio_drop_threshold,
                                                                  oride_req_ptr->qset_drop_ctr_threshold,
                                                                  oride_req_ptr->qset_metric_threshold,
                                                                  oride_req_ptr->qset_primary_ctr_threshold,
                                                                  oride_req_ptr->pset_leak,
                                                                  oride_req_ptr->num_locked_fings_window_size,
                                                                  oride_req_ptr->scch_success_rate_window_size,
                                                                  oride_req_ptr->min_avg_num_locked_fings_no_rxd,
                                                                  oride_req_ptr->min_avg_num_locked_fings_rxd,
                                                                  oride_req_ptr->min_avg_scch_success_rate,
                                                                  oride_req_ptr->max_tx_agc_val,
                                                                  oride_req_ptr->disable_qice_timer_init_val,
                                                                  oride_req_ptr->enable_qice_timer_init_val);

   return (void *) oride_rsp_ptr;
}

/*===========================================================================

FUNCTION  wcdmadiag_qice_debug_enable_handler

DESCRIPTION
  This function handles request to override the QICE debug enable.

============================================================================*/
PACKED void * wcdmadiag_qice_debug_enable_handler(
   PACKED void *req_ptr,
   uint16       pkt_len
)
{
   DIAG_SUBSYS_WCDMA_WCDMA_QICE_DEBUG_ENABLE_F_req_type * oride_req_ptr;
   DIAG_SUBSYS_WCDMA_WCDMA_QICE_DEBUG_ENABLE_F_rsp_type * oride_rsp_ptr;

   oride_req_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_QICE_DEBUG_ENABLE_F_req_type *) req_ptr;

   /* Prepare the response packet */
   oride_rsp_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_QICE_DEBUG_ENABLE_F_rsp_type *)
     diagpkt_alloc(diagpkt_get_cmd_code(req_ptr),
                   sizeof(DIAG_SUBSYS_WCDMA_WCDMA_QICE_DEBUG_ENABLE_F_rsp_type));

   if (oride_rsp_ptr == NULL)
   {
     /* Allocation failed, and diagpkt_err_rsp will fail as well.
      * So just return NULL.
      */
     return NULL;
   }
   
   /* Copy the header from the request packet to the response packet */
   WCDMA_MEMCPY((void *) oride_rsp_ptr,
                sizeof(DIAG_SUBSYS_WCDMA_WCDMA_QICE_DEBUG_ENABLE_F_rsp_type),
                (void *) oride_req_ptr,
                sizeof(diagpkt_subsys_header_type));
   
   /* Generate the response */
   oride_rsp_ptr->status = wl1_cme_proc_wplt_qice_debug_enable_cmd(oride_req_ptr->carrier,
                                                                   oride_req_ptr->enable);

   return (void *) oride_rsp_ptr;
}

#ifdef FEATURE_WCDMA_RX_DIVERSITY
/*===========================================================================

FUNCTION  wcdmadiag_rxd_api_handler

DESCRIPTION
  This function handles request to call RxD APIs.

============================================================================*/
PACKED void * wcdmadiag_rxd_api_oride_handler(
   PACKED void *req_ptr,
   uint16       pkt_len
)
{
   DIAG_SUBSYS_WCDMA_WCDMA_RXD_API_ORIDE_F_req_type * oride_req_ptr;
   DIAG_SUBSYS_WCDMA_WCDMA_RXD_API_ORIDE_F_rsp_type * oride_rsp_ptr;

   oride_req_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_RXD_API_ORIDE_F_req_type *) req_ptr;

   /* Prepare the response packet */
   oride_rsp_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_RXD_API_ORIDE_F_rsp_type *)
     diagpkt_alloc(diagpkt_get_cmd_code(req_ptr),
                   sizeof(DIAG_SUBSYS_WCDMA_WCDMA_RXD_API_ORIDE_F_rsp_type));

   if (oride_rsp_ptr == NULL)
   {
     /* Allocation failed, and diagpkt_err_rsp will fail as well.
      * So just return NULL.
      */
     return NULL;
   }
   
   /* Copy the header from the request packet to the response packet */
   WCDMA_MEMCPY((void *) oride_rsp_ptr,
                sizeof(DIAG_SUBSYS_WCDMA_WCDMA_RXD_API_ORIDE_F_rsp_type),
                (void *) oride_req_ptr,
                sizeof(diagpkt_subsys_header_type));
   
   /* Generate the response */
   oride_rsp_ptr->status = wl1_rxd_api_oride_cmd(oride_req_ptr->api_identifier,
           oride_req_ptr->input_arg_1, oride_req_ptr->input_arg_2, oride_req_ptr->input_arg_3, oride_req_ptr->input_arg_4);

   return (void *) oride_rsp_ptr;
}
#endif

/*===========================================================================

FUNCTION  wcdmadiag_manual_ul_var_upd_handler

DESCRIPTION
  This function handles request to manually update eul uplink module variables.
  Right now this handler is used to enable or disable eul pwr restriction.

============================================================================*/
PACKED void * wcdmadiag_manual_ul_var_upd_handler(
   PACKED void *req_ptr,
   uint16       pkt_len
)
{
   DIAG_SUBSYS_WCDMA_WCDMA_DIAG_UL_VAR_UPD_F_req_type *upd_req_ptr;
   DIAG_SUBSYS_WCDMA_WCDMA_DIAG_UL_VAR_UPD_F_rsp_type *upd_rsp_ptr;

   upd_req_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_DIAG_UL_VAR_UPD_F_req_type *) req_ptr;

   /* Prepare the response packet */
   upd_rsp_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_DIAG_UL_VAR_UPD_F_rsp_type *)
                         diagpkt_alloc(
                            diagpkt_get_cmd_code( req_ptr ),
                            sizeof(DIAG_SUBSYS_WCDMA_WCDMA_DIAG_UL_VAR_UPD_F_rsp_type)
                         );

   if ( upd_rsp_ptr == NULL )
   {
      /* Allocation failed, and diagpkt_err_rsp will fail as well.
      ** So just return NULL.
      */
      return NULL;
   }

   /* Copy the header from the request packet to the response packet */
   WCDMA_MEMCPY((void*)upd_rsp_ptr,
                sizeof(DIAG_SUBSYS_WCDMA_WCDMA_DIAG_UL_VAR_UPD_F_rsp_type),
                (void*)upd_req_ptr,
                sizeof(diagpkt_subsys_header_type));
   
   /* Generate the response */
   upd_rsp_ptr->status = 1;

   /* Call the uplink function that will take the values from diag request packet
      and update the variable */
   ul_set_var_through_qxdm(
         upd_req_ptr->debug_enable_f3_message_type,upd_req_ptr->debug_var_val);

   return (void *) upd_rsp_ptr;

} 

/*===========================================================================

FUNCTION  wcdmadiag_skip_cm_meas_var_upd_handler

DESCRIPTION
 This function handles request to enable/diasble skip_cm_measurement variable

============================================================================*/
PACKED void * wcdmadiag_skip_cm_meas_var_upd_handler(
   PACKED void *req_ptr,
   uint16       pkt_len
)
{
   DIAG_SUBSYS_WCDMA_WCDMA_DIAG_SKIP_CM_MEAS_VAR_UPD_F_req_type *upd_req_ptr;
   DIAG_SUBSYS_WCDMA_WCDMA_DIAG_SKIP_CM_MEAS_VAR_UPD_F_rsp_type *upd_rsp_ptr;

   upd_req_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_DIAG_SKIP_CM_MEAS_VAR_UPD_F_req_type *) req_ptr;

   /* Prepare the response packet */
   upd_rsp_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_DIAG_SKIP_CM_MEAS_VAR_UPD_F_rsp_type *)
                         diagpkt_alloc(
                            diagpkt_get_cmd_code( req_ptr ),
                            sizeof(DIAG_SUBSYS_WCDMA_WCDMA_DIAG_SKIP_CM_MEAS_VAR_UPD_F_rsp_type)
                         );

   if ( upd_rsp_ptr == NULL )
   {
      /* Allocation failed, and diagpkt_err_rsp will fail as well.
      ** So just return NULL.
      */
      return NULL;
   }

   /* Copy the header from the request packet to the response packet */
   WCDMA_MEMCPY((void*)upd_rsp_ptr,
                sizeof(DIAG_SUBSYS_WCDMA_WCDMA_DIAG_SKIP_CM_MEAS_VAR_UPD_F_rsp_type),
                (void*)upd_req_ptr,
                sizeof(diagpkt_subsys_header_type));
   
   /* Generate the response */
   upd_rsp_ptr->status = 1;

   /* Call the L1CMMGR function that will take the values from diag request packet
      and update the variable skip_cm_measurements */
   l1_cm_set_skip_cm_meas_through_qxdm(upd_req_ptr->debug_var_val);

   return (void *) upd_rsp_ptr;
} 

/*===========================================================================
Function: wcdmadiag_sample_serv_config
Description:

       This function processes the wcdma sample server logging config request 
       packet from QXDM and sends the response back.

============================================================================*/
#ifdef FEATURE_WCDMA_SAMPLE_SERVER_DATA_LOGGING
#error code not present
#endif /* FEATURE_WCDMA_SAMPLE_SERVER_DATA_LOGGING */

#ifdef FEATURE_WCDMA_L1_PSC_SCANNER
/*===========================================================================
Function: wcdmadiag_l1_psc_scanner_config_handler
Description:

       This function processes the wcdma PSC scanner request passed through
       diag req/rsp commands. This is the interface through which Mobileview will
       request the PSC scanner search.

============================================================================*/

PACKED void * wcdmadiag_l1_psc_scanner_config_handler(PACKED void* req_pkt, uint16 pkt_len)
{
  DIAG_SUBSYS_WCDMA_WCDMA_L1_PSC_SCANNER_CONFIG_F_req_type *req_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_L1_PSC_SCANNER_CONFIG_F_req_type *)req_pkt;
  DIAG_SUBSYS_WCDMA_WCDMA_L1_PSC_SCANNER_CONFIG_F_rsp_type *rsp_ptr;

  const int rsp_len = sizeof(DIAG_SUBSYS_WCDMA_WCDMA_L1_PSC_SCANNER_CONFIG_F_rsp_type);
  uint8 scan_state;
  /* Allocate the memory for this */
  rsp_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_L1_PSC_SCANNER_CONFIG_F_rsp_type *)diagpkt_subsys_alloc(DIAG_SUBSYS_WCDMA, 
                                                                                             WCDMA_L1_PSC_SCANNER_CONFIG_F, 
                                                                                             rsp_len );
  if (!rsp_ptr)
  {
    ERR_FATAL("diagpkt_subsys_alloc(%d, %d, %d) returned NULL!", 
              DIAG_SUBSYS_WCDMA, WCDMA_L1_PSC_SCANNER_CONFIG_F, rsp_len);
  }
  else
  {
    /* accept command only if its PSC scanner ON/OFF */
    if((req_ptr->mview_cmd == SRCH_MVIEW_CMD_ON) || 
       (req_ptr->mview_cmd == SRCH_MVIEW_CMD_OFF)
      )
    {
      srch_mview_psc_scan_cmd = (req_ptr->mview_cmd == SRCH_MVIEW_CMD_ON) ? TRUE : FALSE;
      rsp_ptr->status = SRCH_MVIEW_STATUS_ACCEPTED;
      WL1_MSG1(LOW, "SCAN_DBG: received Cmd (0x%x)", req_ptr->mview_cmd);
      scan_state = (req_ptr->mview_cmd == SRCH_MVIEW_CMD_ON) ? 
                             PSC_SCAN_STARTED_ON_REQ : PSC_SCAN_STOPPED_ON_REQ;
  
      /* post a event EVENT_WCDMA_PSC_SCANNER_STATE with the commanded state*/
      srch_mview_post_scan_state_event(scan_state);
    }
    else
    {
      rsp_ptr->status = SRCH_MVIEW_STATUS_ERROR;
      WL1_MSG1(ERROR, "SCAN: Incorrect Cmd (0x%x)", req_ptr->mview_cmd);
    }
    /* send back the cmd in the response pkt */
    rsp_ptr->mview_cmd = req_ptr->mview_cmd;
  }

  return rsp_ptr;
}
#endif

/*===========================================================================
Function: wcdma_diag_write_fbiber_thresh_handler
Description:

       This function writes the given value into the mdsp variable fbiBER_Threshold.

============================================================================*/
PACKED void * wcdma_diag_write_fbiber_thresh_handler(PACKED void* req_pkt, uint16 pkt_len)
{
  DIAG_SUBSYS_WCDMA_WCDMA_DIAG_WRITE_FBIBER_THRESH_F_req_type *req_ptr = 
    (DIAG_SUBSYS_WCDMA_WCDMA_DIAG_WRITE_FBIBER_THRESH_F_req_type *) req_pkt;
  DIAG_SUBSYS_WCDMA_WCDMA_DIAG_WRITE_FBIBER_THRESH_F_rsp_type *rsp_ptr;

  const int rsp_len = sizeof(DIAG_SUBSYS_WCDMA_WCDMA_DIAG_WRITE_FBIBER_THRESH_F_rsp_type);

  rsp_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_DIAG_WRITE_FBIBER_THRESH_F_rsp_type *)
              diagpkt_subsys_alloc(DIAG_SUBSYS_WCDMA,
                                   WCDMA_DIAG_WRITE_FBIBER_THRESH_F,
                                   rsp_len);

  if (!rsp_ptr)
  {
    ERR_FATAL("diagpkt_subsys_alloc(%d, %d, %d) returned NULL!", 
              DIAG_SUBSYS_WCDMA, WCDMA_DIAG_WRITE_FBIBER_THRESH_F, rsp_len);
  }
  else 
  {
    rsp_ptr->status = wl1_write_fbiber_threshold(req_ptr->fbiBER_threshold);
  }
                                                                                          
  return rsp_ptr;
}


/*===========================================================================
Function: wcdma_diag_read_fbiber_thresh_handler
Description:

       This function returns the value read from the mdsp variable fbiBER_Threshold.

============================================================================*/
PACKED void * wcdma_diag_read_fbiber_thresh_handler(PACKED void* req_pkt, uint16 pkt_len)
{
  uint16   threshold;

  DIAG_SUBSYS_WCDMA_WCDMA_DIAG_READ_FBIBER_THRESH_F_rsp_type *rsp_ptr;

  const int rsp_len = sizeof(DIAG_SUBSYS_WCDMA_WCDMA_DIAG_READ_FBIBER_THRESH_F_rsp_type);

  rsp_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_DIAG_READ_FBIBER_THRESH_F_rsp_type *)
              diagpkt_subsys_alloc(DIAG_SUBSYS_WCDMA,
                                   WCDMA_DIAG_READ_FBIBER_THRESH_F,
                                   rsp_len);

  if (!rsp_ptr)
  {
    ERR_FATAL("diagpkt_subsys_alloc(%d, %d, %d) returned NULL!", 
              DIAG_SUBSYS_WCDMA, WCDMA_DIAG_READ_FBIBER_THRESH_F, rsp_len);
  }
  else
  {
    rsp_ptr->status = wl1_read_fbiber_threshold(&threshold);
    rsp_ptr->fbiBER_threshold = threshold;
  }
                                                                                          
  return rsp_ptr;
}
/*===========================================================================

FUNCTION  wcdmadiag_manual_setup_rlc_dbg_cfg

DESCRIPTION
  This function handles request to manually update RLC module debug variables.

============================================================================*/
PACKED void * wcdmadiag_manual_setup_rlc_dbg_cfg(
   PACKED void *req_pkt,
   uint16       pkt_len
)
{
   DIAG_SUBSYS_WCDMA_WCDMA_DIAG_RLC_DBG_F_req_type *req_ptr;
   DIAG_SUBSYS_WCDMA_WCDMA_DIAG_RLC_DBG_F_rsp_type *rsp_ptr;

   req_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_DIAG_RLC_DBG_F_req_type *) req_pkt;

   /* Prepare the response packet */
   rsp_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_DIAG_RLC_DBG_F_rsp_type *)
                         diagpkt_alloc(
                            diagpkt_get_cmd_code( req_pkt ),
                            sizeof(DIAG_SUBSYS_WCDMA_WCDMA_DIAG_RLC_DBG_F_rsp_type)
                         );

   if ( rsp_ptr == NULL )
   {
      /* Allocation failed, and diagpkt_err_rsp will fail as well.
      ** So just return NULL.
      */
      return NULL;
   }

   /* Copy the header from the request packet to the response packet */
   WCDMA_MEMCPY((void*)rsp_ptr,
                sizeof(DIAG_SUBSYS_WCDMA_WCDMA_DIAG_RLC_DBG_F_rsp_type),
                (void*)req_ptr,
                sizeof(diagpkt_subsys_header_type));
   
   /* Generate the response */
   rsp_ptr->status = 1;

   /* Call the RLC function that will take the values from diag request packet
      and update the variable */
   wcdma_rlc_set_dbg_var_through_qxdm(
         req_ptr->debug_var_type,req_ptr->debug_var_val);

   return (void *) rsp_ptr;

} 
/*===========================================================================

FUNCTION  wcdmadiag_manual_setup_mac_dbg_cfg

DESCRIPTION
  This function handles request to manually update MAC module debug variables.

============================================================================*/
PACKED void * wcdmadiag_manual_setup_mac_dbg_cfg(
   PACKED void *req_pkt,
   uint16       pkt_len
)
{

   DIAG_SUBSYS_WCDMA_WCDMA_DIAG_MAC_DBG_F_req_type *req_ptr;
   DIAG_SUBSYS_WCDMA_WCDMA_DIAG_MAC_DBG_F_rsp_type *rsp_ptr;

   req_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_DIAG_MAC_DBG_F_req_type *) req_pkt;

   /* Prepare the response packet */
   rsp_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_DIAG_MAC_DBG_F_rsp_type *)
                         diagpkt_alloc(
                            diagpkt_get_cmd_code( req_pkt ),
                            sizeof(DIAG_SUBSYS_WCDMA_WCDMA_DIAG_MAC_DBG_F_rsp_type)
                         );

   if ( rsp_ptr == NULL )
   {
      /* Allocation failed, and diagpkt_err_rsp will fail as well.
      ** So just return NULL.
      */
      return NULL;
   }

   /* Copy the header from the request packet to the response packet */
   WCDMA_MEMCPY((void*)rsp_ptr,
                sizeof(DIAG_SUBSYS_WCDMA_WCDMA_DIAG_MAC_DBG_F_rsp_type),
                (void*)req_ptr,
                sizeof(diagpkt_subsys_header_type));
   
   /* Generate the response */
   rsp_ptr->status = 1;

   /* Call the MAC function that will take the values from diag request packet
      and update the variable */
   wcdma_mac_set_dbg_var_through_qxdm(
         req_ptr->debug_var_type,req_ptr->debug_var_val);

   return (void *) rsp_ptr;

} 


#ifdef FEATURE_WCDMA_SET_HS_CAT
/*===========================================================================

FUNCTION  wcdmadiag_manual_setup_dc_test_cfg

DESCRIPTION
  This function takes 
  - hs_cat
  as inputs and calls RRC function that sets the HSDPA category as per the input argument.

============================================================================*/
PACKED void * wcdmadiag_rrc_set_hs_cat(
   PACKED void *req_ptr,
   uint16       pkt_len
)
{
   DIAG_SUBSYS_WCDMA_WCDMA_DIAG_RRC_HS_CAT_req_type *upd_req_ptr;
   DIAG_SUBSYS_WCDMA_WCDMA_DIAG_RRC_HS_CAT_rsp_type *upd_rsp_ptr;

   upd_req_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_DIAG_RRC_HS_CAT_req_type *) req_ptr;

   /* Prepare the response packet */
   upd_rsp_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_DIAG_RRC_HS_CAT_rsp_type *)
                         diagpkt_alloc(
                            diagpkt_get_cmd_code( req_ptr ),
                            sizeof(DIAG_SUBSYS_WCDMA_WCDMA_DIAG_RRC_HS_CAT_rsp_type)
                         );

   if ( upd_rsp_ptr == NULL )
   {
      /* Allocation failed, and diagpkt_err_rsp will fail as well.
      ** So just return NULL.
      */
      return NULL;
   }

   /* Copy the header from the request packet to the response packet */
   WCDMA_MEMCPY((void*)upd_rsp_ptr,
                sizeof(DIAG_SUBSYS_WCDMA_WCDMA_DIAG_RRC_HS_CAT_rsp_type),
                (void*)upd_req_ptr,
                sizeof(diagpkt_subsys_header_type));
   
   /* Generate the response */

     upd_rsp_ptr->status = 
       rrc_qxdm_set_hs_cat(upd_req_ptr->hs_cat);

   return (void *) upd_rsp_ptr;
} 
#endif /*FEATURE_WCDMA_HSUPA*/



/*===========================================================================

FUNCTION  wcdmadiag_meas_req_handler

DESCRIPTION
  This function handles the MEAS query command from QXDM.

============================================================================*/
PACKED void * wcdmadiag_meas_req_handler(
   PACKED void *req_pkt,
   uint16       pkt_len
)
{
   DIAG_SUBSYS_WCDMA_WCDMA_DIAG_QUERY_MEAS_F_req_type *req_ptr;
   DIAG_SUBSYS_WCDMA_WCDMA_DIAG_QUERY_MEAS_F_rsp_type *rsp_ptr;
#ifdef FEATURE_WCDMA_DATA_FOR_UI_BAR_DISPLAY
   #error code not present
#else
   l1_rssi_info_struct_type rssi_data;
#endif

   req_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_DIAG_QUERY_MEAS_F_req_type *) req_pkt;

   /* Prepare the response packet */
   rsp_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_DIAG_QUERY_MEAS_F_rsp_type *)
                         diagpkt_alloc(
                            diagpkt_get_cmd_code( req_pkt ),
                            sizeof(DIAG_SUBSYS_WCDMA_WCDMA_DIAG_QUERY_MEAS_F_rsp_type)
                         );

   if ( rsp_ptr == NULL )
   {
      /* Allocation failed, and diagpkt_err_rsp will fail as well.
      ** So just return NULL.
      */
      return NULL;
   }

   /* Copy the header from the request packet to the response packet */
   WCDMA_MEMCPY((void*)rsp_ptr,
                sizeof(DIAG_SUBSYS_WCDMA_WCDMA_DIAG_QUERY_MEAS_F_rsp_type),
                (void*)req_ptr,
                sizeof(diagpkt_subsys_header_type));

#ifdef FEATURE_WCDMA_DATA_FOR_UI_BAR_DISPLAY
   #error code not present
#else
   l1_get_rssi (&rssi_data);

   /* Fill the indivudal  response fields */
   rsp_ptr->valid = rssi_data.is_valid_rssi;
   rsp_ptr->version = 1;
   rsp_ptr->rscp =  -(rssi_data.rssi_value);
#ifdef FEATURE_UMTS_REPORT_ECIO
   rsp_ptr->ec_io = (uint8)rssi_data.ecio;
#endif
#ifdef FEATURE_WCDMA_DIAG_QUERY_CELL_INFO
   rsp_ptr->scr_code = rssi_data.scr_code;
   rsp_ptr->freq = rssi_data.freq;
#endif
#endif
   return (void*)rsp_ptr;
}

#ifdef FEATURE_WCDMA_SECURITY_SETTINGS
/*===========================================================================

FUNCTION  wcdmadiag_manual_security_set_integ

DESCRIPTION
  This function handles setting the integrity flag enable/disable.
  Run the command as below in the QXDM command prompt
  send_data 0x4b 0x04 0x32 0x00 0x01    to enable integrity
  send_data 0x4b 0x04 0x32 0x00 0x00    to disable integrity
============================================================================*/
PACKED void * wcdmadiag_manual_security_set_integ(
   PACKED void *req_ptr,
   uint16       pkt_len
)
{
   DIAG_SUBSYS_WCDMA_WCDMA_DIAG_INTEGRITY_ENABLE_F_req_type * int_req_ptr;
   DIAG_SUBSYS_WCDMA_WCDMA_DIAG_INTEGRITY_ENABLE_F_rsp_type * int_rsp_ptr;

   int_req_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_DIAG_INTEGRITY_ENABLE_F_req_type *) req_ptr;   

   /* Prepare the response packet */
   int_rsp_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_DIAG_INTEGRITY_ENABLE_F_rsp_type *)
     diagpkt_alloc(diagpkt_get_cmd_code(req_ptr),
                   sizeof(DIAG_SUBSYS_WCDMA_WCDMA_DIAG_INTEGRITY_ENABLE_F_rsp_type));

   if (int_rsp_ptr == NULL)
   {
     /* Allocation failed, and diagpkt_err_rsp will fail as well.
      * So just return NULL.
      */
     return NULL;
   }
   
   /* Copy the header from the request packet to the response packet */
   WCDMA_MEMCPY((void *) int_rsp_ptr,
                sizeof(DIAG_SUBSYS_WCDMA_WCDMA_DIAG_INTEGRITY_ENABLE_F_rsp_type),
                (void *) int_req_ptr,
                sizeof(diagpkt_subsys_header_type));
   
   /* Generate the response */
   int_rsp_ptr->status = rrc_qxdm_integrity_enable_diagcmd(int_req_ptr->enable);  
   
   return (void *) int_rsp_ptr;
}
/*===========================================================================

FUNCTION  wcdmadiag_manual_security_set_ciph

DESCRIPTION
  This function handles setting the ciphering flag enable/disable.
  Run the command as below in the QXDM command prompt
  send_data 0x4b 0x04 0x33 0x00 0x01    to enable ciphering
  send_data 0x4b 0x04 0x33 0x00 0x00    to disable ciphering

============================================================================*/
PACKED void * wcdmadiag_manual_security_set_ciph(
   PACKED void *req_ptr,
   uint16       pkt_len
)
{
  
  DIAG_SUBSYS_WCDMA_WCDMA_DIAG_CIPHERING_ENABLE_F_req_type * ciph_req_ptr;
  DIAG_SUBSYS_WCDMA_WCDMA_DIAG_CIPHERING_ENABLE_F_rsp_type * ciph_rsp_ptr;
  
  ciph_req_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_DIAG_CIPHERING_ENABLE_F_req_type *) req_ptr;
  
  /* Prepare the response packet */
  ciph_rsp_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_DIAG_CIPHERING_ENABLE_F_rsp_type *)
    diagpkt_alloc(diagpkt_get_cmd_code(req_ptr),
                  sizeof(DIAG_SUBSYS_WCDMA_WCDMA_DIAG_CIPHERING_ENABLE_F_rsp_type));
  
  if (ciph_rsp_ptr == NULL)
  {
    /* Allocation failed, and diagpkt_err_rsp will fail as well.
     * So just return NULL.
     */
    return NULL;
  }
  
  /* Copy the header from the request packet to the response packet */
  WCDMA_MEMCPY((void *) ciph_rsp_ptr,
               sizeof(DIAG_SUBSYS_WCDMA_WCDMA_DIAG_CIPHERING_ENABLE_F_rsp_type),
               (void *) ciph_req_ptr,
               sizeof(diagpkt_subsys_header_type));
  
  /* Generate the response */
  ciph_rsp_ptr->status = rrc_qxdm_ciphering_enable_diagcmd(ciph_req_ptr->enable);
  
  return (void *) ciph_rsp_ptr;
}

/*===========================================================================

FUNCTION  wcdmadiag_manual_security_set_fksec

DESCRIPTION
  This function handles setting the fake security flag enable/disable.
  Run the command as below in the QXDM command prompt
  send_data 0x4b 0x04 0x34 0x00 0x01    to enable fake security
  send_data 0x4b 0x04 0x34 0x00 0x00    to disable fake security

============================================================================*/

PACKED void * wcdmadiag_manual_security_set_fksec(
   PACKED void *req_ptr,
   uint16       pkt_len
)
{
  
  DIAG_SUBSYS_WCDMA_WCDMA_DIAG_FAKE_SECURITY_ENABLE_F_req_type * fk_req_ptr;
  DIAG_SUBSYS_WCDMA_WCDMA_DIAG_FAKE_SECURITY_ENABLE_F_rsp_type * fk_rsp_ptr;
  
  fk_req_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_DIAG_FAKE_SECURITY_ENABLE_F_req_type *) req_ptr;
  
  /* Prepare the response packet */
  fk_rsp_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_DIAG_FAKE_SECURITY_ENABLE_F_rsp_type *)
    diagpkt_alloc(diagpkt_get_cmd_code(req_ptr),
                  sizeof(DIAG_SUBSYS_WCDMA_WCDMA_DIAG_FAKE_SECURITY_ENABLE_F_rsp_type));
  
  if (fk_rsp_ptr == NULL)
  {
    /* Allocation failed, and diagpkt_err_rsp will fail as well.
     * So just return NULL.
     */
    return NULL;
  }
  
  /* Copy the header from the request packet to the response packet */
  WCDMA_MEMCPY((void *) fk_rsp_ptr,
               sizeof(DIAG_SUBSYS_WCDMA_WCDMA_DIAG_FAKE_SECURITY_ENABLE_F_rsp_type),
               (void *) fk_req_ptr,
               sizeof(diagpkt_subsys_header_type));
  
  /* Generate the response */
  fk_rsp_ptr->status = rrc_qxdm_fakesecurity_enable_diagcmd(fk_req_ptr->enable);
  
  return (void *) fk_rsp_ptr;
}

#endif

/*===========================================================================

FUNCTION  wcdmadiag_gpio_pwr_prof_handler

DESCRIPTION
  This function controls whether GPIO power profiling is enabled

============================================================================*/
PACKED void* wcdmadiag_gpio_pwr_prof_handler
(
  PACKED void* req_pkt,
  uint16       pkt_len
)
{
   DIAG_SUBSYS_WCDMA_WCDMA_GPIO_PWR_PROF_F_req_type *req_ptr;
   DIAG_SUBSYS_WCDMA_WCDMA_GPIO_PWR_PROF_F_rsp_type *rsp_ptr;

   req_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_GPIO_PWR_PROF_F_req_type*) req_pkt;

   /* Prepare the response packet */
   rsp_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_GPIO_PWR_PROF_F_rsp_type*)
              diagpkt_alloc
              (
                diagpkt_get_cmd_code(req_pkt),
                sizeof(DIAG_SUBSYS_WCDMA_WCDMA_GPIO_PWR_PROF_F_rsp_type)
              );

   if (!req_ptr || !rsp_ptr)
   {
      /* Allocation failed, and diagpkt_err_rsp will fail as well.
      ** So just return NULL.
      */
      return NULL;
   }

   /* Copy the header from the request packet to the response packet */
   WCDMA_MEMCPY((void*) rsp_ptr,
                sizeof(DIAG_SUBSYS_WCDMA_WCDMA_GPIO_PWR_PROF_F_rsp_type),
                (void*) req_ptr,
                sizeof(diagpkt_subsys_header_type));
   
   /* Generate a hard-coded response */
   rsp_ptr->status = 1;

   /* Call the DRX function that will update its internal state */
   drx_set_pwr_strobe(req_ptr->enabled);

   return (void*) rsp_ptr;
}

/*===========================================================================

FUNCTION  wcdmadiag_manual_setup_datapath_dbg_cfg

DESCRIPTION
  This function handles request to manually update datapath "datapath_f3_debug" variable.

============================================================================*/
PACKED void * wcdmadiag_manual_setup_datapath_dbg_cfg(
   PACKED void *req_pkt,
   uint16       pkt_len
)
{
  DIAG_SUBSYS_WCDMA_WCDMA_DIAG_DATAPATH_DBG_F_req_type *req_ptr;
  DIAG_SUBSYS_WCDMA_WCDMA_DIAG_DATAPATH_DBG_F_rsp_type *rsp_ptr;

  req_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_DIAG_DATAPATH_DBG_F_req_type *) req_pkt;

  /* Prepare the response packet */
  rsp_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_DIAG_DATAPATH_DBG_F_rsp_type *)
                        diagpkt_alloc(
                           diagpkt_get_cmd_code( req_pkt ),
                           sizeof(DIAG_SUBSYS_WCDMA_WCDMA_DIAG_DATAPATH_DBG_F_rsp_type)
                        );

  if ( rsp_ptr == NULL )
  {
    /* Allocation failed, and diagpkt_err_rsp will fail as well.
     ** So just return NULL.
     */
    return NULL;
  }

  /* Copy the header from the request packet to the response packet */
  WCDMA_MEMCPY((void*)rsp_ptr,
               sizeof(DIAG_SUBSYS_WCDMA_WCDMA_DIAG_DATAPATH_DBG_F_rsp_type),
               (void*)req_ptr,
               sizeof(diagpkt_subsys_header_type));
   
  /* Generate the response */
  rsp_ptr->status = 1;

  /* Call the RLC function that will take the values from diag request packet
     and update the variable */
   
  wcdma_datapath_set_datapath_f3_dbg_var_through_qxdm(
    req_ptr->debug_var_type,req_ptr->debug_var_val);

  return (void *) rsp_ptr;
} 

/*===========================================================================

FUNCTION  wcdmadiag_debug_crash_on_srch_done_handler

DESCRIPTION
  This function updates the info to crash UE upon search done.

============================================================================*/
PACKED void * wcdmadiag_debug_crash_on_srch_done_handler
(
	 PACKED void *req_pkt,
   uint16       pkt_len
)
{
	DIAG_SUBSYS_WCDMA_WCDMA_DEBUG_CRASH_ON_SRCH_DONE_F_req_type *req_ptr;
  DIAG_SUBSYS_WCDMA_WCDMA_DEBUG_CRASH_ON_SRCH_DONE_F_rsp_type *rsp_ptr;
  req_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_DEBUG_CRASH_ON_SRCH_DONE_F_req_type*) req_pkt;

  /* Prepare the response packet */
  rsp_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_DEBUG_CRASH_ON_SRCH_DONE_F_rsp_type*)
            diagpkt_alloc
            (
             diagpkt_get_cmd_code(req_pkt),
             sizeof(DIAG_SUBSYS_WCDMA_WCDMA_DEBUG_CRASH_ON_SRCH_DONE_F_rsp_type)
             );

  if (!req_ptr || !rsp_ptr)
  {
    /* Allocation failed, and diagpkt_err_rsp will fail as well.
    ** So just return NULL. 
    */
    return NULL;
	}

  /* Copy the header from the request packet to the response packet */
  WCDMA_MEMCPY((void*)rsp_ptr,
               sizeof(DIAG_SUBSYS_WCDMA_WCDMA_DEBUG_CRASH_ON_SRCH_DONE_F_rsp_type),
               (void*)req_ptr,
               sizeof(diagpkt_subsys_header_type));

  /* Populate the status response byte. */
  rsp_ptr->status = 1;

	/* Call the function to update the debug crash info */
  srch_enable_and_update_crash_info(req_ptr->l1m_state, req_ptr->bmask,
     req_ptr->carr_idx, req_ptr->type_of_search, req_ptr->thresh, req_ptr->freq, req_ptr->psc);

  return (void*) rsp_ptr;
} 

/*===========================================================================

FUNCTION  wcdmadiag_attenuate_signal_handler

DESCRIPTION This function helps in attenuating wcdma signal through qxdm
            This is the interface to receive command from QXDM

============================================================================*/
PACKED void * wcdmadiag_attenuate_signal_handler
(
	 PACKED void *req_pkt,
   uint16       pkt_len
)
{
	DIAG_SUBSYS_WCDMA_WCDMA_ATTENUATE_SIGNAL_F_req_type *req_ptr;
  DIAG_SUBSYS_WCDMA_WCDMA_ATTENUATE_SIGNAL_F_rsp_type *rsp_ptr;
  req_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_ATTENUATE_SIGNAL_F_req_type*) req_pkt;

  /* Prepare the response packet */
  rsp_ptr = (DIAG_SUBSYS_WCDMA_WCDMA_ATTENUATE_SIGNAL_F_rsp_type*)
            diagpkt_alloc
            (
             diagpkt_get_cmd_code(req_pkt),
             sizeof(DIAG_SUBSYS_WCDMA_WCDMA_ATTENUATE_SIGNAL_F_rsp_type)
             );

  if (!req_ptr || !rsp_ptr)
  {
    /* Allocation failed, and diagpkt_err_rsp will fail as well.
    ** So just return NULL. 
    */WL1_MSG2(ERROR, "wcdmadiag error req ptr %d rspptr %d",req_ptr,rsp_ptr);
    return NULL;
  }

  /* Copy the header from the request packet to the response packet */
  WCDMA_MEMCPY((void*)rsp_ptr,
               sizeof(DIAG_SUBSYS_WCDMA_WCDMA_ATTENUATE_SIGNAL_F_rsp_type),
               (void*)req_ptr,
               sizeof(diagpkt_subsys_header_type));

  /* Populate the status response byte. */
  rsp_ptr->status = 1;

	/* Call the function to update the debug crash info */
  srch_enable_and_update_attenuate_info(req_ptr->attenuate_value);

  return (void*) rsp_ptr;
} 

/**************************************************************************
** DISPATCH TABLE.
Note:  Any new commands for WCDMA need to be added here.
****************************************************************************/
static const diagpkt_user_table_entry_type wcdmadiag_tbl[] =
{
  {WCDMA_DIAG_VERNO_F, WCDMA_DIAG_VERNO_F, wcdmadiag_verno},
#ifdef FEATURE_AL1
  #error code not present
#endif
  {WCDMA_DIAG_BLER_RATE_CHANGE_F, WCDMA_DIAG_BLER_RATE_CHANGE_F, wcdma_diag_change_bler_rate},
  {WCDMA_DIAG_REACQ_SLEW_STATS_F, WCDMA_DIAG_REACQ_SLEW_STATS_F, wcdmadiag_reacq_slew_stats},
  {WCDMA_DIAG_REACQ_SRCH_STATS_F, WCDMA_DIAG_REACQ_SRCH_STATS_F, wcdmadiag_reacq_srch_stats},
  {WCDMA_DIAG_REACQ_CLEAR_STATS_F, WCDMA_DIAG_REACQ_CLEAR_STATS_F, wcdmadiag_reacq_clear_stats},
  {WCDMA_DIAG_SET_TRK_LO_PDM_F, WCDMA_DIAG_SET_TRK_LO_PDM_F, wcdmadiag_set_trk_lo_pdm},
  {WCDMA_DIAG_ORIG_F, WCDMA_DIAG_ORIG_F, wcdmadiag_call_orig},
  {WCDMA_DIAG_END_F, WCDMA_DIAG_END_F, wcdmadiag_call_end},
  {WCDMA_STATUS_F, WCDMA_STATUS_F, wcdmadiag_status},
  {WCDMA_ADDITIONAL_STATUS_F, WCDMA_ADDITIONAL_STATUS_F, wcdmadiag_additional_status}
#ifdef FEATURE_WCDMA_SAMPLE_SERVER_DATA_LOGGING
  #error code not present
#endif /* FEATURE_WCDMA_SAMPLE_SERVER_DATA_LOGGING */
#ifdef FEATURE_WCDMA_HSUPA
  ,{WCDMA_DIAG_EUL_UL_VAR_UPD_F,WCDMA_DIAG_EUL_UL_VAR_UPD_F,wcdmadiag_manual_eul_ul_var_upd_handler}
#endif
#ifdef FEATURE_CPU_BASED_FLOW_CONTROL
  ,{WCDMA_RLC_FLOW_CONTROL_CONFIG_F, WCDMA_RLC_FLOW_CONTROL_CONFIG_F, wcdmadiag_rlc_flow_control_config_handler}
#endif //FEATURE_CPU_BASED_FLOW_CONTROL
#ifdef FEATURE_WCDMA_L1_PSC_SCANNER
  ,{WCDMA_L1_PSC_SCANNER_CONFIG_F, WCDMA_L1_PSC_SCANNER_CONFIG_F, wcdmadiag_l1_psc_scanner_config_handler}
#endif //FEATURE_WCDMA_L1_PSC_SCANNER
  ,{WCDMA_DIAG_WCDMA_STATS_F, WCDMA_DIAG_WCDMA_STATS_F, wcdmadiag_wcdma_stats}
  ,{WCDMA_DIAG_WCDMA_CLEAR_STATS_F, WCDMA_DIAG_WCDMA_CLEAR_STATS_F, wcdmadiag_clear_wcdma_stats}
#ifdef FEATURE_WCDMA_PLT
  #error code not present
#endif //FEATURE_WCDMA_PLT
#ifdef FEATURE_MBMS
#error code not present
#endif
#ifdef FEATURE_WCDMA_TBSET_DEBUG_DUMP
#error code not present
#endif
  ,{WCDMA_DIAG_UL_VAR_UPD_F,WCDMA_DIAG_UL_VAR_UPD_F,wcdmadiag_manual_ul_var_upd_handler}
#ifdef FEATURE_WCDMA_MIMO
  ,{WCDMA_MIMO_PARAMS_F, WCDMA_MIMO_PARAMS_F, wcdmadiag_wcdma_mimo_params_handler}
#endif
#ifdef FEATURE_HSDPA
  ,{WCDMA_CQI_OVERIDE_PARAMS_F, WCDMA_CQI_OVERIDE_PARAMS_F, wcdmadiag_wcdma_cqi_params_handler}
#endif
  ,{WCDMA_QICE_ORIDE_TABLE_PARAMS_F, WCDMA_QICE_ORIDE_TABLE_PARAMS_F, wcdmadiag_qice_oride_table_params_handler}
  ,{WCDMA_QICE_ORIDE_CONFIG_PARAMS_F, WCDMA_QICE_ORIDE_CONFIG_PARAMS_F, wcdmadiag_qice_oride_config_params_handler}
  ,{WCDMA_QICE_ORIDE_FHT_PARAMS_F, WCDMA_QICE_ORIDE_FHT_PARAMS_F, wcdmadiag_qice_oride_fht_params_handler}
  ,{WCDMA_QICE_ORIDE_FW_PARAMS_F, WCDMA_QICE_ORIDE_FW_PARAMS_F, wcdmadiag_qice_oride_fw_params_handler}
  ,{WCDMA_QICE_ORIDE_LOG_PARAMS_F, WCDMA_QICE_ORIDE_LOG_PARAMS_F, wcdmadiag_qice_oride_log_params_handler}
  ,{WCDMA_QICE_RESET_F, WCDMA_QICE_RESET_F, wcdmadiag_qice_reset_handler}
  ,{WCDMA_CME_ORIDE_PARAMS_F, WCDMA_CME_ORIDE_PARAMS_F, wcdmadiag_cme_oride_params_handler}
  ,{WCDMA_QICE_DEBUG_ENABLE_F, WCDMA_QICE_DEBUG_ENABLE_F, wcdmadiag_qice_debug_enable_handler}
#ifdef FEATURE_WCDMA_RX_DIVERSITY
,{WCDMA_RXD_API_ORIDE_F, WCDMA_RXD_API_ORIDE_F, wcdmadiag_rxd_api_oride_handler}
#endif
  ,{WCDMA_FW_DEBUG_CMD_F, WCDMA_FW_DEBUG_CMD_F, wcdmadiag_wcdma_fw_debug_cmd_handler}
  ,{WCDMA_DIAG_WRITE_FBIBER_THRESH_F, WCDMA_DIAG_WRITE_FBIBER_THRESH_F, wcdma_diag_write_fbiber_thresh_handler}
  ,{WCDMA_DIAG_READ_FBIBER_THRESH_F, WCDMA_DIAG_READ_FBIBER_THRESH_F, wcdma_diag_read_fbiber_thresh_handler}
#ifdef FEATURE_WCDMA_DC_HSDPA
  ,{WCDMA_DIAG_DC_HS_SETUP, WCDMA_DIAG_DC_HS_SETUP, wcdmadiag_manual_setup_dc_test_cfg}
#endif
  ,{WCDMA_DIAG_RLC_DBG_F, WCDMA_DIAG_RLC_DBG_F, wcdmadiag_manual_setup_rlc_dbg_cfg}
  ,{WCDMA_DIAG_MAC_DBG_F, WCDMA_DIAG_MAC_DBG_F, wcdmadiag_manual_setup_mac_dbg_cfg}
#ifdef FEATURE_WCDMA_SET_HS_CAT
  ,{WCDMA_DIAG_RRC_HS_CAT,WCDMA_DIAG_RRC_HS_CAT, wcdmadiag_rrc_set_hs_cat}
#endif
  ,{WCDMA_DIAG_QUERY_MEAS_F, WCDMA_DIAG_QUERY_MEAS_F, wcdmadiag_meas_req_handler}
#ifdef FEATURE_WCDMA_SECURITY_SETTINGS
  ,{WCDMA_DIAG_INTEGRITY_ENABLE_F,WCDMA_DIAG_INTEGRITY_ENABLE_F,wcdmadiag_manual_security_set_integ}
  ,{WCDMA_DIAG_CIPHERING_ENABLE_F,WCDMA_DIAG_CIPHERING_ENABLE_F,wcdmadiag_manual_security_set_ciph}
  ,{WCDMA_DIAG_FAKE_SECURITY_ENABLE_F,WCDMA_DIAG_FAKE_SECURITY_ENABLE_F,wcdmadiag_manual_security_set_fksec}
#endif
  ,{WCDMA_GPIO_PWR_PROF_F, WCDMA_GPIO_PWR_PROF_F, wcdmadiag_gpio_pwr_prof_handler}
  ,{WCDMA_DIAG_DATAPATH_DBG_F, WCDMA_DIAG_DATAPATH_DBG_F, wcdmadiag_manual_setup_datapath_dbg_cfg}
  /* Code added for Skip CM Measurement Cmd from QXDM */
  ,{WCDMA_DIAG_SKIP_CM_MEAS_VAR_UPD_F, WCDMA_DIAG_SKIP_CM_MEAS_VAR_UPD_F, wcdmadiag_skip_cm_meas_var_upd_handler}
  ,{WCDMA_DEBUG_CRASH_ON_SRCH_DONE_F, WCDMA_DEBUG_CRASH_ON_SRCH_DONE_F, wcdmadiag_debug_crash_on_srch_done_handler}
  ,{WCDMA_ATTENUATE_SIGNAL_F,WCDMA_ATTENUATE_SIGNAL_F, wcdmadiag_attenuate_signal_handler}
};

#ifdef __cplusplus

  DIAGPKT_DISPATCH_AUTOREGISTER (DIAG_SUBSYS_WCDMA, wcdmadiag_tbl);

#else

void wcdmadiag_init (void)
{ 
 uint16 internal_last_code = LOG_WCDMA_INTERNAL_LAST_C ;
 static boolean wcdmadiag_initialized = FALSE;

 if(wcdmadiag_initialized)
 {
   WL1_MSG0(ERROR, "wcdmadiag already initialized");
   return;
 }
 if ( internal_last_code > LOG_WCDMA_LAST_C )
 {
   ERR_FATAL("Internal last log code %d is greater than the external last log code %d",LOG_WCDMA_INTERNAL_LAST_C,LOG_WCDMA_LAST_C,0);
 }

 DIAGPKT_DISPATCH_TABLE_REGISTER (DIAG_SUBSYS_WCDMA, wcdmadiag_tbl);
 wcdmadiag_initialized = TRUE;
}

#endif

#if defined __cplusplus
  }
#endif

/*===========================================================================
FUNCTION        WL1_GET_FRAME_NUMBER_FOR_DIAG

DESCRIPTION     This function returns the frame number for diag

PARAMETER       Void.

RETURN VALUE    uint32 value.
===========================================================================*/
uint32 wl1_get_frame_number_for_diag(void)
{

  uint32 frame_number;


  if ((l1m_state == L1M_STOPPED) || (l1m_state == L1M_DEEP_SLEEP) ||
      (l1m_state == L1M_DEACTIVE))
  {
    /* WCDMA is not active.  Frame number not valid. */
    frame_number = 0;
  }
  else
  {
    if ( l1m_is_connected() )
    {
      frame_number = seq_get_cfn();
    }
    else
    {
      frame_number = seq_get_sfn();
    }
  }

  return frame_number;
}

#endif  /* #if !defined(FEATURE_GSM_PLT) && defined(FEATURE_WCDMA) */

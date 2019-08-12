/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             C D M A    R E G I S T R A T I O N    S E R V I C E S

GENERAL DESCRIPTION
  This module performs CDMA registration operations.

EXTERNALIZED FUNCTIONS
  mccreg_init
    Perform registration variable initialization

  mccreg_reset_reg
    This function resets CDMA registration indicators

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Call mccreg_init first.

  Copyright (c) 1993 - 2016 Qualcomm Technologies, Inc.
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

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mccreg.c_v   1.13   01 Oct 2002 13:43:20   sbrelsfo  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/src/mccreg.c#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/26/16   ppr     Logging changes
07/17/15   ppr     Split_Ovhds:Chgs to move to update ovhd state if any reg  
                   is pending even though ovhds are not current
12/03/14   ppr     Revert "Don't re-start Power-up timer if its running"
11/11/14   ppr     Changes to handle n/w Err REDIR to HDR after MT CSFB ESR
08/27/14   ppr     Don't re-start Power-up timer if its already running
08/11/14   ppr     Added critical section protection to access 
                   mccreg_zone_ptr and mccreg_zone_q
07/21/14   gga     FR 20298: SIM Swap without RESET if carrier settings have 
                   changed
06/23/14   ppr     Graceful handling for NV Read/Write error in Card mode
05/06/14   gga     1X_CP F3 redistribution
02/10/14   ppr     Fix to initialize registration related parameters 
                   immediately after activated in CSFB mode.
11/14/13   gga     Fixing the compilation error.
08/20/13   dkb     Mainline FEATURE_PASSPORT_MODE
07/29/13   eye     Switch to secure random number generation api.
07/18/13   ssh     To trigger powerup reg in CSFB mode when reg wasn't acked
                   on Native 1X.
03/12/13   ppr     Changes to pass correct arg to mcc_nv_read()
                   in mccreg_powerdown()
03/11/13   ppr     PPP cleanup changes when moving from 1x to LTE
                   in Non-SV mode
01/24/13   ppr     New API - mccreg_powerup_mode_online() added for
                   PROT_OPRT_MODE_ONLINE case
01/10/13   ppr     Do NV# writes in mccreg_powerdown() only
                   if there is a change in NV value
12/28/12   gga     Klocwork Reviews
10/16/12   ppr     FEATURE_USERZONE cleanup
09/24/12   jj      Changes to support QCHAT on 4.x PLs.
06/29/12   ppr     FEATURE_IS2000_REL_A_CC cleanup changes
06/15/12   ppr     Feature Cleanup
05/24/12   jj      CW fix.
04/04/12   trc     Convert to m1x_time module
01/11/12   jj      Changes to support SCI modification by QCHAT.
02/16/12   srk     Replaced FEATURE_LTE_TO_1X by FEATURE_MODEM_1X_IRAT_LTO1X.
02/04/12   ppr     Feature Cleanup
01/17/12   ppr     Feature Cleanup: Mainlined Always ON features
01/05/12   ssh     Replacing the compile time JCDMA feature check to the
                   NV based check.
07/22/11   jj      CSFB: Support for slotted wakeup in CSFB mode.
04/18/11   ag      Support for power down and power up registration after
                   OTASP commit.
02/15/11   jj      Fixed compile and lint warnings.
02/14/11   ag      Used REX_ISR_LOCK and REX_ISR_UNLOCK for protecting shared
                   data from simultaneous access by multiple tasks/ISRs.
02/11/11   jj      CSFB: Added check to have timer reg being performed at
                   correct interval.
01/27/11   azh     Lint and KW fixes.
01/17/11   jj      CSFB: Made changes not to do power-up registration because
                   of blk_sys change in CSFB mode.
01/04/11   ag      Fixed compiler and lint warnings.
12/23/10   jj      CSFB: Updated checks to signal to MC in CSFB mode inorder
                   to perform Timer Based Registration.
12/15/10   jj      CSFB: Power down reg support.
12/15/10   ag      SU level CMI clean up.
12/13/10   jj      Indentation update.
12/13/10   ssh     Fixed a CSFB merging issue.
12/05/10   jj      CSFB: Merged CSFB Rel8/Rel9 changes.
11/15/10   jtm     Initial SU API split for CP.
09/15/10   azh     Fixed KlocWork warnings.
08/06/10   ag      Replaced clk APIs with time APIs.
07/12/10   jj      Replaced CP's QDSP6 feature with an umbrella feature.
04/15/10   ssh     Merged the GOBI specific changes under the FEATURE_GOBI.
04/14/10   jj      Use 5000ms instead of ms_interval in
                   mccreg_zone_timer_done function.
02/15/10   jj      Added support for ESS feature.
02/12/10   vks     Use 80ms instead of ms_interval in mccreg_timer_reg_timer_
                   done function.

08/02/09   jtm     Lint fixes.
05/03/09   jj      CMI Phase-II update.
03/23/09   jtm     Featurize _v.h includes from subsystems outside of 1x.
03/20/09   jtm     Added include for sd_v.h. Changed sdprl_map_chan_to_blksys()
                   to sd_map_chan_to_blksys(). This function was moved and
                   renamed.
03/16/09   jtm     Removed sdprl.h include. No longer needed by this file.
03/05/09   jj      Featurized Q6 migration.
03/04/09   adw     Added mccdma_v.h include.
12/10/08   jj      QDSP6 migration porting effort, as a part of which replaced
                   INTLOCK and INTFREE with REX_ENABLE_PREMPTION and
                   REX_DISABLE_PREMPTION.
12/14/07   an      Added Passport Mode
06/14/07   an      Added support for White Grey Black Card Operation
06/07/07   pg      Avoid registration attemp during silent redial under
                   FEATURE_JCDMA.
02/23/07   sb      Added support for operational mode feature.
08/01/06   fc      Merged the following from MSMSHARED:
08/01/06   an      Made klokwork fixes
07/26/06   fh      Fixed an issue so that the mobile does not attempt to register
                   when it is locked by Lock Order
06/01/06   an      Added the bypass registration indicator changes
05/30/06   fc      Fixed lint errors.
05/20/06   fc      Fixed lint errors.
04/26/06   an      Fixed lint errors
10/16/05   fc      Default last registered SCI properly.
06/03/05   fc      Merged the following from MSMSHARED:
06/03/05   fh       Fixed a problem that mistakingly deletes AMSS6500 deletes item
                    from ZONE LIST and SID_NID_LIST
05/03/05   fc      Merged the following from MSMSHARED:
05/04/05   fc       Fixed lint error.
04/26/05   fc       Added zone_timer to idle_reg_info.
04/27/05   az      Submitted changes for CR 62361. This also requires
                   corresponding changes in NV. Added an NV item,
                   last_registered_sci. Also added code for reading
                   and writing it.
04/25/05   fc      Merged the following from MSMSHARED:
04/20/04   fc       Changes for the update of reg_sid, reg_nid, reg_reg_zone,
                    auth and rand during AHO/APHO.
03/22/05   an       Mainlined FEATURE_ENHANCED_BAND_CLASS. Eliminated
                    FEATURE_CDMA_800 and FEATURE_CDMA_1900
04/12/05   az      Added a condition check (mccreg_vars.reg_sci != MCC_NULL_SCI)
                   when setting operational SCI.
03/28/05   fc      Sent NSET_F after PARM_F to avoid improper reset of SRCH
                   win parmeters.
12/08/04   va      Merge the following from REL_B_C archive:
10/29/04   pg       If 1X subscription is not available, use default values for
                    subscription items.
11/08/04   va      Merge following from REL_B_C archive:
09/20/04   fc       Added null pointer check for mccreg_zone_ptr in
                    mccreg_idle_check.
11/08/04   pg      Added debug msg for timer based registration procedure.
                   Correct some issues in the previous change.
10/27/04   pg      Do P_REV check for Rel D timer based registration procedure.
                   Combined some common cases for Rel D and Non Rel D code.
08/17/04   az      Rel D negative SCI changes
08/03/04   pg      Added support for Rel D timer based registration.
07/12/04   pg      If RUIM card is not available on a RUIM enable only build,
                   we do not do any registration.
05/19/04   sb      Made following merge official via integrate.
05/14/04   sb      Merged following from msmshared:
                   04/06/04   bkm     Remove featurization from 2/27/04 change
                   since it applies to non-JCDMA case as well.
                   03/11/04   yll     Added support for KDDI requirement to
                   ignore async data and async fax 1x pages during HDR connected
                   state.
04/06/04   bkm     Remove featurization from 2/27/04 change since it applies to
                   non-JCDMA case as well.
03/11/04   yll     Added support for KDDI requirement to ignore async data
                   and async fax 1x pages during HDR connected state.
03/05/04   bkm     Lint cleanup.

02/27/04   bkm     Added check so that if an emergency call is pending and
                   PSIST would deny access for registration request then
                   the registration is not attempted.
02/04/04   fc      Removed redundant code in mccreg_idle_check.
11/19/03   va      Added support for REL C register_in_idle
09/11/03   yll     Added the featurization for ACP include files.
08/20/03   yll     Changed FEATURE_HDR_HYBRID to FEATURE_HDR.
07/23/03   bkm     Mainlined FEATURE_ANTIREGISTRATION_BIAS.
07/01/03   az      Added FEATURE_ENHANCED_BAND_CLASS
10/31/02   az      Added support for HDR Data dedicated transmission mode
                   access prevention.
09/30/02   sb      Mainlined FEATURE_SD20.
09/06/02   va      Cleaned up lint errors.
08/01/02   fc      Added support for the count of the number of powerup
                   registration performed.
07/10/02   fc/lc   Corrected MS unnecessary staying awake for changes in APM
                   and EAPM.
03/09/02   lh      Merged from common archive.
  02/28/02   fc      Fixed incorrect variable reference in mccreg_idle_check.
02/22/02   fc      Added support for Release A common channels.
02/14/02   fc      Merged from 1.9 in MSM archive.
 01/25/02  sj      Added support for User Zone based registration.
 12/14/01  fc      Removed function mccreg_blksys_bc_init.
                   Recorded the band class and cellular serving system / PCS
                   block frequency of the base station that mobile last
                   registered to support power-up registration for band class,
                   cellular serving system or PCS frequency block change.
11/28/01   lh      Time keeping of timer based registration in terms of ms.
12/06/01   lh      Merged from MSM_CP.05.00.108.
  11/28/01   lh      Time keeping of timer based registration in terms of ms.
11/01/01   fc      Featurized the operation of powerup registration for freq
                   block, serv-sys or band class change under
                   FEATURE_BLKSYS_BC_CHG_POWERUP_REG.
06/26/01   fc      Merged the following from common archive:
           06/13/01   fc      Added mccreg_blk_bc_init to support powerup
                              registration for freq block, serv-sys or band
                              class switch.
03/27/01   kk      Merged mainline changes.
03/06/01   fc      Added support of events report: Timer expired.
01/10/01   va      Changes the ovhd_okay macros into function calls.
10/27/00   jq      Changed the update checking in mccreg_update_lists()
09/20/00   jq      Cleaned all lint errors.
06/19/00   yll     Added sanity checking in mccreg_update_lists().
03/27/00   ry      Merged the following from PP (MSM3100_CP.03.00.10):
           ks      Removed the ENHANCED_STANDBY_II feature defintion.
10/05/99   lh      Serving System update for CM is added.
06/22/99   ks      Added mccreg_get_bs_reg_info function which provides sid,
                   band_class, block_or_sys for a CDMA registered system.
05/10/99   kmp     Changed T_ACP to FEATURE_ACP
04/07/99   abh     Changes for the new Queue Interface.
02/01/99   br      RAM saving changes.

03/01/99   ejv     Added missing #endif and 2 more functions for
                   FEATURE_ANTIREGISTRATION_BIAS.
03/01/99   ejv     Merged mccreg_find_zone for FEATURE_ANTIREGISTRATION_BIAS.
11/04/98   ych     Modifed mccreg_clear_reg() and created mccreg_reset_reg to
                   set the REGISTERED's flag only when desired.
09/15/98   pms     Backed out _near pointers
09/04/98   pms     Neared the variables to save ROM space.
08/04/98   ych     Merged SSPR, E911 and Acquisition improvements.
05/21/98   abh     NV item SID_NID changes for SSPR-800.
10/13/97   jca     Timer-based registration counter no longer reset when "switching"
                   from alternate mode/system.  Powerup registration timer only set
                   when mobile is in powerup state, instead of every time we register
                   in AMPS.
05/13/96   jca     Updates for J-STD-008 registration procedures.
05/25/95   jca     Updates for IS-95A registration procedures.
08/02/94   jca     Added T_ACP conditional compiles to support ISS1 release.
04/25/94   gb      Parameterised all IS95 timeouts in cai_tmo.
12/07/93   jah     Fixed mccreg_powerup_timer_done() to deal with slotted mode.
06/15/93   gb      Fixed small eternal loop bug, in idle_ack system change.
03/15/93   jai     Created module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "1x_variation.h"
#include "target.h"
#include "comdef.h"
#include "modem_1x_defs.h"

#include "nv.h"
#include "err.h"
#include "msg.h"
#include "m1x_diag.h"
#include "ran.h"
#include "qw.h"

#include "mc_i.h"
#include "mccreg_v.h"
#include "mccreg_i.h"
#include "cai_v.h"
#include "queue.h"
#include "mccdma_v.h"
#include "mccdma_i.h"
#include "distreg.h"

#include "mclog_v.h"

#include "sd_v.h"
#include "qw.h"

#ifdef FEATURE_GOBI
#if defined(FEATURE_GOBI_VZW)
#include "otaspi_i.h"
#endif
#endif /* FEATURE_GOBI */

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X
#include "mcccsfbsup.h"
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X */

#ifdef FEATURE_QCHAT
#include "onex_qchatupk.h"
#include "data1x_qchatupk_opts.h"
#endif /*FEATURE_QCHAT*/

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/



#define MCCREG_MAX_ZONES CAI_N9M

mccreg_zone_type mccreg_zone_bufs[MCCREG_MAX_ZONES];

q_type mccreg_zone_free_q;
q_type mccreg_zone_q;

/* Pointer to item on ZONE_LISTs with a disabled timer */
mccreg_zone_type *mccreg_zone_ptr;

/* ----------------------------------------------------------------------
** The mobile station shall store a list of the systems/networks in which
** the mobile station has registered (SID_NID_LISTs).  The mobile station
** shall be capable of storing at least N10m entries in SID_NID_LISTs.
** ---------------------------------------------------------------------- */
#define MCCREG_MAX_SID_NID CAI_N10M

mccreg_sid_nid_type mccreg_sid_nid_bufs[MCCREG_MAX_SID_NID];

q_type mccreg_sid_nid_free_q;
q_type mccreg_sid_nid_q;

/* Pointer to item on SID_NID_LISTs with a disabled timer */
mccreg_sid_nid_type *mccreg_sid_nid_ptr;

/* ----------------------------------
** Registration Timers and Indicators
** ---------------------------------- */
timer_type mccreg_powerup_timer;
  /* Power-up/Initialization timer */
timer_type mccreg_timer_reg_timer;
  /* Timer-based registration timer */
timer_type mccreg_zone_timer;
  /* Timer for zone based registration */

#ifdef FEATURE_PPP_CLEANUP_NON_SV_MODE
/* PPP_CLEANUP_NON_SV_MODE:Deregister powerdown / powerup timer */
timer_type mccreg_deregister_timer;
#endif /* FEATURE_PPP_CLEANUP_NON_SV_MODE */

mccreg_vars_type mccreg_vars;

/* Buffers to store and retrieve information from NV */
nv_cmd_type mccreg_nv_buf;    /* Define command buffer */
nv_item_type mccreg_nv_data;   /* Define data buffer    */

/* Lookup table for timer based reg period formula */
dword const reg_count_table [57] = {        /* 57 = 85-29+1 */
152, 181, 215, 256, 304, 362, 431, 512,
609, 724, 861, 1024, 1218, 1448, 1722, 2048,
2435, 2896, 3444, 4096, 4871, 5793, 6889, 8192,
9742, 11585, 13777, 16384, 19484, 23170, 27554, 32768,
38968, 46341, 55109, 65536, 77936, 92682, 110218, 131072,
155872, 185364, 220436, 262144, 311744, 370728, 440872, 524288,
623487, 741455, 881744, 1048576, 1246974, 1482910, 1763488, 2097152,
2493948 };

/* Lookup table for zone based timer period formula in msecs */
int4 const timer_tab [8] = {
60000-5000, 120000-5000, 300000-5000, 600000-5000,
1200000-5000, 1800000-5000, 2700000-5000, 3600000-5000
};


qword mccreg_powerup_reg_count;
  /* Number of powerup registration performed */


#ifdef FEATURE_MODEM_1X_IRAT_LTO1X
/* This provides wildcard value which will be used in LTE mode
 * to prevent powerup registrations because of blk_sys change.
 * Since on LTE we dont have freq/channel we can not obtain
 * blk_sys and hence we dont want to perform power-up regs based on it
 * while being on LTE.
 */
#define MCC_BLK_SYS_WILDCARD 0xFFFF
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X */

/*===========================================================================

FUNCTION MCCREG_POWERUP_TIMER_DONE

DESCRIPTION
  This function is the callback procedure for when the powerup timer
  expires.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mccreg_powerup_timer_done
(
  int4 ms_interval /*lint -esym(715,ms_interval) */
)
{
  /* Set indicator that the powerup/initialization timer is no longer on */
  mccreg_vars.powerup_timer_on = FALSE;
  mccreg_vars.powering_up = FALSE;

  /* Event report: Timer T57m expired */
  mclog_report_event_timer(CAI_TIMER_T57M);

} /* mccreg_powerup_timer_done */

/*===========================================================================

FUNCTION MCCREG_CDMA_REGISTERED

DESCRIPTION
  This function compares the passed in SID and NID with the one in the
  SID_NID_LIST that has the timer disabled. If match found, that means
  the mobile is registered for the given SID/NID pair.

DEPENDENCIES
  This should be called based on the current Mode of the MS.

RETURN VALUE
  TRUE if registered on CDMA for the given SID and NID.
  FALSE otherwise.

SIDE EFFECTS


===========================================================================*/

boolean mccreg_cdma_registered
(
  word sid,
  word nid
)
{
  boolean registered = FALSE;

  /* Enter Critical Section */
  REX_ISR_LOCK(&mc_reg_crit_sect);
  
  if (mccreg_sid_nid_ptr != NULL)
  {
    if ((mccreg_sid_nid_ptr->sid == sid) &&
      ((mccreg_sid_nid_ptr->nid == nid) ||
       (mccreg_sid_nid_ptr->nid == CAI_HOME_NID)))
    {
      /* (SID,NID) pair is the one with the disabled timer */
      registered = TRUE;
    }
  }
  else
  {
    registered = FALSE;
  }

  /* Leave Critical Section */
  REX_ISR_UNLOCK(&mc_reg_crit_sect);
  
  return registered;

} /* mccreg_cdma_registered */

/*===========================================================================

FUNCTION MCCREG_GET_BS_REG_INFO

DESCRIPTION
  This function is called by other subsystems to receive the current Base
  Station information (SID,BAND_CLASS and BLOCK_OR_SYS type) where the
  Mobile has registered.

DEPENDENCIES
  This should be called based on the current Mode of the MS. This function
  returns "FALSE" if the Mobile is not registered,

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS


===========================================================================*/

boolean mccreg_get_bs_reg_info
(
  mccreg_bs_reg_info* info
)
{
  boolean status;
  word sid;
  word nid;
  db_items_value_type db_item;

  db_get (DB_NO_SVC,  &db_item);

  if (db_item.no_svc)
  {
    status = FALSE;  // No service Available, return False
  }
  else
  {
      // CDMA but not registered
      db_get( DB_SID, &db_item );
      sid = db_item.sid;
      db_get( DB_NID, &db_item );
      nid = db_item.nid;

      if( !mccreg_cdma_registered(sid, nid) )
      {
        /* Not registered */
        info->band_class = cdma.band_class;
        info->sid = MAXUINT16;      // DS depends on this setting
        status = TRUE;
      }
      else
      {
        /*-----------------------------------------------------------------
        ** The Mobile has a Service and is registered on a CDMA system
        ** Update the BS info for the passed in mcc_reg_bs_info argument.
        ** --------------------------------------------------------------*/
        info->sid = sid;
        info->band_class = cdma.band_class;
        info->block_or_sys = cdma.block_or_sys;
        status = TRUE;   // The info record is updated and TRUE is returned
      }

  }

  return status;

} /* mccreg_get_bs_reg_info */

/*===========================================================================

FUNCTION MCCREG_IS_REGISTERED

DESCRIPTION
  This function is called by other subsystems to find out if the
  Mobile has registered.

DEPENDENCIES
  This should be called based on the current Mode of the MS. This function
  returns "FALSE" if the Mobile is not registered,

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS


===========================================================================*/
boolean mccreg_is_registered
(
  void
)
{
  boolean status;
  word sid, nid;
  db_items_value_type db_item;

  db_get (DB_NO_SVC,  &db_item);

  if (db_item.no_svc)
  {
    status = FALSE;  // No service Available, return False
  }
  else
  {
    db_get(DB_DIGITAL, &db_item);

    db_get(DB_SID, &db_item);
    sid = db_item.sid;

    db_get(DB_NID, &db_item);
    nid = db_item.nid;

    status = mccreg_cdma_registered(sid, nid);
  }

  return status;

} /* mccreg_is_registered */

/*===========================================================================

FUNCTION MCCREG_ZONE_TIMER_DONE

DESCRIPTION
  This function is the callback procedure for when the timer used to maintain
  the sid nid list and the zone list expires.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mccreg_zone_timer_done
(
  int4 ms_interval
)
{
  mccreg_vars.timer_cnt += ms_interval;
} /* mccreg_zone_timer_done */

/*===========================================================================

FUNCTION MCCREG_TIMER_REG_TIMER_DONE

DESCRIPTION
  This function is the callback procedure for when the timer based
  registration timer expires.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mccreg_timer_reg_timer_done
(
  int4 ms_interval
)
{
  mccreg_vars.reg_count_ms += (dword) ms_interval;

} /* mccreg_timer_reg_timer_done */

/*===========================================================================

FUNCTION MCCREG_CHECK_TOTAL_ZONES

DESCRIPTION
  This function removes entries from ZONE_LISTs if there are more entries
  than allowed by the setting of TOTAL_ZONES.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mccreg_check_total_zones
(
  word total_zones
    /* Number of registration zones to be retained */
)
{
  word max_zones;
    /* Maximum registration zones allowed on zone list */
  mccreg_zone_type *zone_ptr;
    /* Pointer to entry from ZONE_LISTs */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Enter Critical Section */
  REX_ISR_LOCK(&mc_reg_crit_sect); 

  if ((total_zones == 0) || (mccreg_zone_ptr == NULL))
  {
    max_zones = total_zones;
  }
  else
  {
    max_zones = total_zones - 1;
  }

  /* Delete excess entries of zone list if required */
  while (q_cnt( &mccreg_zone_q ) > max_zones)
  {
    /* Delete excess entries - assume priority queue */
    zone_ptr = (mccreg_zone_type*) q_get( &mccreg_zone_q );
    q_put( &mccreg_zone_free_q,  &zone_ptr->link);
  }

  /* Also remove the zone list entry item with a disabled timer if required */
  if ((total_zones == 0) && (mccreg_zone_ptr != NULL))
  {
    q_put( &mccreg_zone_free_q, &mccreg_zone_ptr->link );
    mccreg_zone_ptr = NULL;
  }

  /* Leave Critical Section */
  REX_ISR_UNLOCK(&mc_reg_crit_sect);
} /* mccreg_check_total_zones */

/*===========================================================================

FUNCTION MCCREG_FIND_SID_NID

DESCRIPTION
  This function searches SID_NID_LISTs the input (SID,NID) pair.

DEPENDENCIES
  None

RETURN VALUE
  A pointer to the (SID,NID) pair on SID_NID_LISTs if it is found.
  NULL otherwise.

SIDE EFFECTS
  None

===========================================================================*/

mccreg_sid_nid_type *mccreg_find_sid_nid
(
  word sid,   /* SID to find */
  word nid    /* NID to find */
)
{
  mccreg_sid_nid_type *sid_ptr;
    /* Pointer to (SID,NID) nid pair on SID_NID_LISTs */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sid_ptr = NULL;

  /* Enter Critical Section */
  REX_ISR_LOCK(&mc_reg_crit_sect);
  
  if (mccreg_sid_nid_ptr != NULL)
  {
    if ((mccreg_sid_nid_ptr->sid == sid) &&
        ((mccreg_sid_nid_ptr->nid == nid) ||
         (mccreg_sid_nid_ptr->nid == CAI_HOME_NID)))
    {
      /* (SID,NID) pair is the one with the disabled timer */
      sid_ptr = mccreg_sid_nid_ptr;
    }
  }

  if (sid_ptr == NULL)
  {
    /* Haven't found pair yet,  check SID_NID list for current SID_NID */
    sid_ptr = (mccreg_sid_nid_type*) q_check( &mccreg_sid_nid_q );

    while (sid_ptr != NULL)
    {
      if ((sid_ptr->sid == sid) &&
          ((sid_ptr->nid == nid) ||
           (sid_ptr->nid == CAI_HOME_NID)))
      {
        /* sid_ptr will still be pointing to correct pair */
        break;
      }
      else
      {
        sid_ptr =
           (mccreg_sid_nid_type*) q_next( &mccreg_sid_nid_q, &sid_ptr->link );
      }
    }
  }
  /* Leave Critical Section */
  REX_ISR_UNLOCK(&mc_reg_crit_sect);

  /* At this point we have either looped through the whole list and the
     sid_ptr is null or we found the sid, nid pair and the sid_ptr is
     pointing to it */

  return (sid_ptr);

} /* mccreg_find_sid_nid */

/*===========================================================================

FUNCTION MCCREG_FIND_ZONE

DESCRIPTION
  This function searches ZONE_LISTs the input (ZONE, SID,NID).

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the input (ZONE, SID,NID) is found in ZONE_LISTs.
  FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/

boolean mccreg_find_zone
(
  word zone,  /* ZONE to find */
  word sid,   /* SID to find */
  word nid    /* NID to find */
)
{
  mccreg_zone_type *zone_ptr;
    /* Pointer used to loop through zone list */
  boolean found;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Enter Critical Section */
  REX_ISR_LOCK(&mc_reg_crit_sect);

  if ((mccreg_zone_ptr != NULL) &&
      (mccreg_zone_ptr->sid == sid) &&
      ((mccreg_zone_ptr->nid == nid) ||
       (mccreg_zone_ptr->nid == CAI_HOME_NID)) &&
      (mccreg_zone_ptr->reg_zone == zone))
  {
    found = TRUE;
  }
  else
  {
    found = FALSE;

    /* Check zone list for current ZONE, SID, and NID */
    zone_ptr = (mccreg_zone_type *)q_check( &mccreg_zone_q );

    while ((zone_ptr != NULL) && (!found))
    {
      if ((zone_ptr->sid == sid) &&
          ((zone_ptr->nid == nid) ||
           (zone_ptr->nid == CAI_HOME_NID)) &&
          (zone_ptr->reg_zone == zone))
      {
        found = TRUE;
      }
      else
      {
        zone_ptr = (mccreg_zone_type *)q_next( &mccreg_zone_q, &zone_ptr->link );
      }
    } /* end while ((zone_ptr != NULL) && (!found)) */
  }

  /* Leave Critical Section */
  REX_ISR_UNLOCK(&mc_reg_crit_sect);

  return(found);

} /* mccreg_find_zone */

/*===========================================================================

FUNCTION MCCREG_ZONE_REG_REQUIRED

DESCRIPTION
  This function checks if a zone-based registration is required
  for the input (ZONE, SID,NID).

DEPENDENCIES
  None

RETURN VALUE
  TRUE if a zone-based registration is required for the input (ZONE, SID,NID).
  FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/

boolean mccreg_zone_reg_required
(
  word zone,  /* ZONE to find */
  word sid,   /* SID to find */
  word nid    /* NID to find */
)
{
  return (!mccreg_vars.powerup_timer_on &&
          (mccreg_vars.reg_enabled) &&
          !mccreg_find_zone(zone, sid, nid));

} /* mccreg_zone_reg_required */

/*===========================================================================

FUNCTION MCCREG_PARAMETER_REG_REQUIRED

DESCRIPTION
  This function checks if a zone-based registration is required
  for the input (SID,NID).

DEPENDENCIES
  None

RETURN VALUE
  TRUE if a parameter-based registration is required for the input (SID,NID).
  FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/

boolean mccreg_parameter_reg_required
(
  word sid,   /* SID to find */
  word nid    /* NID to find */
)
{
  return (!mccreg_vars.powerup_timer_on &&
          (mccreg_find_sid_nid(sid, nid) == NULL));

} /* mccreg_parameter_reg_required */

/*===========================================================================

FUNCTION MCCREG_ADD_SID_NID

DESCRIPTION
  This function adds an entry to SID_NID_LISTs.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mccreg_add_sid_nid
(
  mccreg_sid_nid_type *sid_nid_ptr
    /* Pointer to item to be placed on the sid, nid list */
)
{
  mccreg_sid_nid_type *temp_sid_ptr;
    /* Pointer to (SID,NID) pair from SID_NID_LISTs */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Enter Critical Section - REX_ISR_LOCK(&mc_reg_crit_sect)
   * should have been called before calling this API 
   * so Critical Section protection is not required in this API */ 

  /* Figure out whether the item can be placed at the end of the list or
     whether it must be inserted in the middle */
  temp_sid_ptr = (mccreg_sid_nid_type*) q_check( &mccreg_sid_nid_q );

  while ((temp_sid_ptr != NULL) &&
         (temp_sid_ptr->cnt <= sid_nid_ptr->cnt))
  {
    temp_sid_ptr =
       (mccreg_sid_nid_type*) q_next( &mccreg_sid_nid_q, &temp_sid_ptr->link );
  }

  if (temp_sid_ptr == NULL)
  {
    /* Put new sid nid at end of list */
    q_put( &mccreg_sid_nid_q, &sid_nid_ptr->link );
  }
  else
  {
    /* insert the new item in order to maintain priority queue */
    #ifdef FEATURE_Q_NO_SELF_QPTR
    q_insert( &mccreg_sid_nid_q, &sid_nid_ptr->link, &temp_sid_ptr->link );
    #else
    q_insert( &sid_nid_ptr->link, &temp_sid_ptr->link );
    #endif /* FEATURE_Q_NO_SELF_QPTR */
  }

} /* mccreg_add_sid_nid */

/*===========================================================================

FUNCTION MCCREG_ADD_ZONE

DESCRIPTION
  This function adds an entry to the zone list maintaining the priority
  queue of zone timers.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  The zone list will change.

===========================================================================*/

void mccreg_add_zone
(
  mccreg_zone_type *zone_ptr
    /* pointer to zone type to be added */
)
{
  mccreg_zone_type *temp_zone_ptr;
    /* pointer to zone from zone list */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Enter Critical Section - REX_ISR_LOCK(&mc_reg_crit_sect)
   * should have been called before calling this API 
   * so Critical Section protection is not required in this API */
  
  /* Find whether we can place the item on the end of the priority list
     or if we have to insert it in the list */

  temp_zone_ptr = (mccreg_zone_type*) q_check( &mccreg_zone_q );

  while ((temp_zone_ptr != NULL ) && (temp_zone_ptr->cnt <= zone_ptr->cnt))
  {
    temp_zone_ptr =
       (mccreg_zone_type*) q_next( &mccreg_zone_q, &temp_zone_ptr->link );
  }

  if (temp_zone_ptr == NULL)
  {
    /* Put new zone list entry at end of list */
    q_put( &mccreg_zone_q, &zone_ptr->link );
  }
  else
  {
    /* Insert the new item in order to maintain priority queue */
    #ifdef FEATURE_Q_NO_SELF_QPTR
    q_insert( &mccreg_zone_q, &zone_ptr->link, &temp_zone_ptr->link );
    #else
    q_insert( &zone_ptr->link, &temp_zone_ptr->link );
    #endif /* FEATURE_Q_NO_SELF_QPTR */
  }

} /* mccreg_add_zone */

/*===========================================================================

FUNCTION MCCREG_DELETE_MULT_SIDS

DESCRIPTION
  If SID_NID_LISTs contains entries with different SIDs, this function
  deletes the excess entries according to the rules specified in
  IS-95A section 6.6.5.1.5.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mccreg_delete_mult_sids( void )
{
  word sid = 0;
    /* SID to check against for MULT_SIDS updates */
  mccreg_sid_nid_type *sid_nid_ptr, *temp_sid_nid_ptr;
    /* Pointers to items from the SID_NID_LIST queue */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Enter Critical Section */
  REX_ISR_LOCK(&mc_reg_crit_sect);
  
  if (mccreg_sid_nid_ptr != NULL)
  {
    /* ----------------------------------------------------------------
    ** If the SID/NID entry timer for any entry is disabled, the mobile
    ** station shall delete all entries not having the same SID as the
    ** entry whose timer is disabled.
    ** ---------------------------------------------------------------- */
    sid_nid_ptr = (mccreg_sid_nid_type*) q_check( &mccreg_sid_nid_q );

    while (sid_nid_ptr != NULL)
    {
      if (sid_nid_ptr->sid != mccreg_sid_nid_ptr->sid)
      {
        /* SID differs from entry whose timer is disabled - delete it */
        temp_sid_nid_ptr = sid_nid_ptr;
        sid_nid_ptr = (mccreg_sid_nid_type*) q_next( &mccreg_sid_nid_q,
                                                     &sid_nid_ptr->link );
        #ifdef FEATURE_Q_NO_SELF_QPTR
        q_delete( &mccreg_sid_nid_q, &temp_sid_nid_ptr->link );
        #else
        q_delete( &temp_sid_nid_ptr->link );
        #endif /* FEATURE_Q_NO_SELF_QPTR */

        q_put( &mccreg_sid_nid_free_q, &temp_sid_nid_ptr->link );
      }
      else
      {
        sid_nid_ptr = (mccreg_sid_nid_type*) q_next( &mccreg_sid_nid_q,
                                                     &sid_nid_ptr->link );
      }
    } /* end while (sid_nid_ptr != NULL) */
  } /* if (mccreg_sid_nid_ptr != NULL) */
  else
  {
    /* ----------------------------------------------------------------
    ** Otherwise, the mobile station shall delete all entries not having
    ** the same SID as the newest entry in SID_NID_LIST, as determined
    ** by the timer values.  (NOTE: No need to delete entries if only
    ** one entry in the SID_NID_LIST.)
    ** ---------------------------------------------------------------- */
    if (q_cnt( &mccreg_sid_nid_q ) > 1)
    {
      /* ----------------------------------------------------------
      ** First pick up the SID from the newest entry (which sits at
      ** the tail of the prioritized mccreg_sid_nid_q)
      ** ---------------------------------------------------------- */
      sid_nid_ptr = (mccreg_sid_nid_type*) q_check( &mccreg_sid_nid_q );

      while (sid_nid_ptr != NULL)
      {
        sid = sid_nid_ptr->sid;
        sid_nid_ptr = (mccreg_sid_nid_type*) q_next( &mccreg_sid_nid_q,
                                                     &sid_nid_ptr->link );
      }

      /* ----------------------------------------------------------
      ** Now delete all entries whose SID does not match the newest
      ** entry in the SID_NID_LIST
      ** ---------------------------------------------------------- */
      sid_nid_ptr = (mccreg_sid_nid_type*) q_check( &mccreg_sid_nid_q );

      while (sid_nid_ptr != NULL)
      {
        if (sid_nid_ptr->sid != sid)
        {
          /* SID differs from SID in newest entry - delete from list */
          temp_sid_nid_ptr = sid_nid_ptr;
          sid_nid_ptr = (mccreg_sid_nid_type*) q_next( &mccreg_sid_nid_q,
                                                       &sid_nid_ptr->link );
          #ifdef FEATURE_Q_NO_SELF_QPTR
          q_delete( &mccreg_sid_nid_q, &temp_sid_nid_ptr->link );
          #else
          q_delete( &temp_sid_nid_ptr->link );
          #endif /* FEATURE_Q_NO_SELF_QPTR */

          q_put( &mccreg_sid_nid_free_q, &temp_sid_nid_ptr->link );
        }
        else
        {
          sid_nid_ptr = (mccreg_sid_nid_type*) q_next( &mccreg_sid_nid_q,
                                                       &sid_nid_ptr->link );
        }
      } /* end while (sid_nid_ptr != NULL) */
    } /* if (q_cnt( &mccreg_sid_nid_q ) > 1) */
  } /* (mccreg_sid_nid_ptr == NULL) */
  
  /* Leave Critical Section */
  REX_ISR_UNLOCK(&mc_reg_crit_sect);

} /* end mccreg_delete_mult_sids() */

/*===========================================================================

FUNCTION MCCREG_DELETE_MULT_NIDS

DESCRIPTION
  If SID_NID_LISTs contains more than one entry for any SID, this function
  deletes the excess entries according to the rules specified in IS-95A
  section 6.6.5.1.5.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mccreg_delete_mult_nids( void )
{
  mccreg_sid_nid_type *sid_nid_ptr, *temp_sid_nid_ptr;
    /* Pointers to items from the SID_NID_LIST queue */
  word sid = 0;
    /* SID to check against for MULT_NIDS updates */
  word sid_nid_q_cnt;
    /* Used to count back from end of SID_NID_LIST */
  byte i,loop_cnt;
    /* Counter variables */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Enter Critical Section */
  REX_ISR_LOCK(&mc_reg_crit_sect);
  
  if (mccreg_sid_nid_ptr != NULL)
  {
    /* ----------------------------------------------------------------
    ** If the SID/NID entry timer for any entry is disabled, the mobile
    ** station shall delete all entries for that SID except the entry
    ** whose timer is disabled.
    ** ---------------------------------------------------------------- */
    sid_nid_ptr = (mccreg_sid_nid_type*) q_check( &mccreg_sid_nid_q );

    while (sid_nid_ptr != NULL)
    {
      if (sid_nid_ptr->sid == mccreg_sid_nid_ptr->sid)
      {
        /* SID is the same as entry whose timer is disabled - delete it */
        temp_sid_nid_ptr = sid_nid_ptr;
        sid_nid_ptr = (mccreg_sid_nid_type*) q_next( &mccreg_sid_nid_q,
                                                     &sid_nid_ptr->link );
        #ifdef FEATURE_Q_NO_SELF_QPTR
        q_delete( &mccreg_sid_nid_q, &temp_sid_nid_ptr->link );
        #else
        q_delete( &temp_sid_nid_ptr->link );
        #endif /* FEATURE_Q_NO_SELF_QPTR */

        q_put( &mccreg_sid_nid_free_q, &temp_sid_nid_ptr->link );
      }
      else
      {
        sid_nid_ptr = (mccreg_sid_nid_type*) q_next( &mccreg_sid_nid_q,
                                                     &sid_nid_ptr->link );
      }
    } /* end while (sid_nid_ptr != NULL) */
  } /* end if (mccreg_sid_nid_ptr != NULL) */

  /* ---------------------------------------------------------------
  ** For all other SIDs, the mobile station shall delete all entries
  ** for each SID except the newest entry, as determined by the
  ** timer values.
  ** --------------------------------------------------------------- */

  /* Intialize loop counters.  On the first pass through loop pick up
     SID from newest (last) entry in SID_NID_LIST.  For each additional
     pass pick up SID FROM preceeding entry in SID_NID_LIST */

  sid_nid_q_cnt = q_cnt( &mccreg_sid_nid_q ); /*lint !e734 loss of precisiion is okay */
  loop_cnt = 0;

  /* NOTE: No need to delete entries if only one entry in the list */
  while (sid_nid_q_cnt > 1)
  {
    /* --------------------------------------------
    ** Pick up SID from latest entries for each SID
    ** -------------------------------------------- */
    sid_nid_ptr = (mccreg_sid_nid_type*) q_check( &mccreg_sid_nid_q );

    for (i=0; i < sid_nid_q_cnt; i++)
    {
      if ( sid_nid_ptr == NULL )
      {
        ERR_FATAL(" Null sid_nid_ptr, cnt=%d", sid_nid_q_cnt, 0, 0 );
      }
      else
      {
        sid = sid_nid_ptr->sid;
        sid_nid_ptr = (mccreg_sid_nid_type*) q_next( &mccreg_sid_nid_q,
                                                   &sid_nid_ptr->link );
      }
    }

    /* --------------------------------------------------------------
    ** Now delete all entries having the same SID as the latest entry
    ** -------------------------------------------------------------- */
    sid_nid_ptr = (mccreg_sid_nid_type*) q_check( &mccreg_sid_nid_q );

    for (i=0; i < (sid_nid_q_cnt-1); i++)
    {
      if ( sid_nid_ptr == NULL )
      {
        ERR_FATAL(" Null sid_nid_ptr, cnt=%d", sid_nid_q_cnt, 0, 0 );
      }

      if ( (sid_nid_ptr != NULL) && (sid_nid_ptr->sid == sid))
      {
        /* SID matches newest entry - delete it */
        temp_sid_nid_ptr = sid_nid_ptr;
        sid_nid_ptr = (mccreg_sid_nid_type*) q_next( &mccreg_sid_nid_q,
                                                     &sid_nid_ptr->link );
        #ifdef FEATURE_Q_NO_SELF_QPTR
        q_delete( &mccreg_sid_nid_q, &temp_sid_nid_ptr->link );
        #else
        q_delete( &temp_sid_nid_ptr->link );
        #endif /* FEATURE_Q_NO_SELF_QPTR */

        q_put( &mccreg_sid_nid_free_q, &temp_sid_nid_ptr->link );
      }
      else
      {
        sid_nid_ptr = (mccreg_sid_nid_type*) q_next( &mccreg_sid_nid_q,
                                                     &sid_nid_ptr->link );
      }
    } /* end for */

    loop_cnt++;
    sid_nid_q_cnt = q_cnt( &mccreg_sid_nid_q ) - loop_cnt; /*lint !e734 loss of precisiion is okay */

  } /* end while (sid_nid_q_cnt > 1) */
  
  /* Leave Critical Section */
  REX_ISR_UNLOCK(&mc_reg_crit_sect);
  
} /* end mccreg_delete_mult_nids */

/*===========================================================================

FUNCTION MCCREG_RESET_REG_STATUS

DESCRIPTION
  This function resets the registration status to indicate that the mobile
  needs to register.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccreg_reset_reg_status( void )
{
  /* -----------------------------
  ** The mobile needs to register.
  ** ----------------------------- */
  mccreg_vars.reg_status = MCCREG_REG_NEEDED;

} /* mccreg_reset_reg_status */

/*===========================================================================

FUNCTION MCCREG_RESET_REG

DESCRIPTION
  This function resets CDMA registration indicators upon successful
  registration on an analog system as specified in IS-95A sections
  2.6.3.8 and 2.6.3.9 by calling mccreg_clear_reg and then updates
  the registration status.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccreg_reset_reg( void )
{
  /* First reset the CDMA registration indicators */
  mccreg_clear_reg();

  /* -----------------------------
  ** The mobile needs to register.
  ** ----------------------------- */
  mccreg_vars.reg_status = MCCREG_REG_NEEDED;

} /* mccreg_reset_reg */

/*===========================================================================

FUNCTION MCCREG_CLEAR_REG

DESCRIPTION
  This function resets CDMA registration indicators upon successful
  registration on an analog system as specified in IS-95A sections
  2.6.3.8 and 2.6.3.9.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccreg_clear_reg( void )
{
  mccreg_sid_nid_type *sid_ptr;
    /* Pointer to (SID,NID) nid pair on SID_NID_LISTs */
  mccreg_zone_type *zone_ptr;
    /* Pointer to entry from ZONE_LISTs */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Enter Critical Section */
  REX_ISR_LOCK(&mc_reg_crit_sect);
  
  /* -------------------------------------
  ** Delete all entries from SID_NID_LISTs
  ** ------------------------------------- */
  while ((sid_ptr = (mccreg_sid_nid_type*) q_get( &mccreg_sid_nid_q )) != NULL)
  {
    q_put( &mccreg_sid_nid_free_q, &sid_ptr->link );
  }

  /* Clear the SID_NID_LISTs item with a disabled timer if it exists */
  if (mccreg_sid_nid_ptr != NULL)
  {
    q_put( &mccreg_sid_nid_free_q, &mccreg_sid_nid_ptr->link );
    mccreg_sid_nid_ptr = NULL;
  }

  /* ----------------------------------
  ** Delete all entries from ZONE_LISTs
  ** ---------------------------------- */
  while ((zone_ptr = (mccreg_zone_type*) q_get( &mccreg_zone_q )) != NULL)
  {
    q_put( &mccreg_zone_free_q, &zone_ptr->link );
  }

  /* Clear the ZONE_LISTs item with a disabled timer if it exists */
  if (mccreg_zone_ptr != NULL)
  {
    q_put( &mccreg_zone_free_q, &mccreg_zone_ptr->link );
    mccreg_zone_ptr = NULL;
  }

  /* Leave Critical Section */
  REX_ISR_UNLOCK(&mc_reg_crit_sect);
  
  /* -------------------------------------------------
  ** Initialize last registered SCI to a default value
  ** -------------------------------------------------- */
  mcc_set_last_sci_reg ( MCC_DEFAULT_SCI );

} /* mccreg_clear_reg */

/*===========================================================================

FUNCTION MCCREG_INIT

DESCRIPTION
  Performs powerup initialization:
    - Initialize timers
    - Initialize queues

DEPENDENCIES
  This procedure should only be called once after powerup.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mccreg_init( void )
{
  word i;


  nv_stat_enum_type read_status;  /* NV read status */


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Enter Critical Section */
  REX_ISR_LOCK(&mc_reg_crit_sect);
  
  /* Initialize queues */
  (void) q_init( &mccreg_sid_nid_free_q );
  (void) q_init( &mccreg_sid_nid_q );
  (void) q_init( &mccreg_zone_free_q );
  (void) q_init( &mccreg_zone_q );

  /* Place buffers on mccreg_zone_free_q */
  for (i=0; i < MCCREG_MAX_ZONES; i++)
  {
    q_put( &mccreg_zone_free_q,
           q_link( &mccreg_zone_bufs[i], &mccreg_zone_bufs[i].link ) );
  }

  /* Place buffers on mccreg_sid_nid_free_q */
  for (i=0; i < MCCREG_MAX_SID_NID; i++)
  {
    q_put( &mccreg_sid_nid_free_q,
           q_link( &mccreg_sid_nid_bufs[i], &mccreg_sid_nid_bufs[i].link ) );
  }
  /* Leave Critical Section */
  REX_ISR_UNLOCK(&mc_reg_crit_sect);

  timer_def2( &mccreg_timer_reg_timer, NULL );
  timer_def2( &mccreg_zone_timer, NULL );
  timer_def2( &mccreg_powerup_timer, NULL );

#ifdef FEATURE_PPP_CLEANUP_NON_SV_MODE
  /* PPP_CLEANUP_NON_SV_MODE:Deregister powerdown / powerup timer */
  timer_def2( &mccreg_deregister_timer, NULL );
#endif /* FEATURE_PPP_CLEANUP_NON_SV_MODE */

  #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
  /* CSFB Retain MT Page Context Timer */
  timer_def2( &mcc_csfb_retain_mt_page_cxt_timer, NULL );
  #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */

  /* Retrieve the number of CDMA powerup registrations performed from NV */
  mccreg_nv_buf.item = NV_CDMA_POWERUP_REG_PERFORMED_I;
  mccreg_nv_buf.cmd  = NV_READ_F;
  mccreg_nv_buf.data_ptr = &mccreg_nv_data;

  read_status = mcc_nv_cmd( &mccreg_nv_buf );

  if (read_status == NV_DONE_S)
  {
    qw_equ(mccreg_powerup_reg_count, mccreg_nv_data.cdma_powerup_reg_performed);
  }


  #ifdef FEATURE_IS2000_REL_C
  mccreg_vars.register_in_idle = FALSE;
  #endif /* FEATURE_IS2000_REL_C */

} /* mccreg_init */

/*===========================================================================

FUNCTION MCCREG_POWERUP

DESCRIPTION
  This function performs power-up registration procedures as called out
  in IS-95A section 6.6.5.5.1.1.

DEPENDENCIES
  None

RETURN VALUE
  TRUE - If power-up registration procedures were successful
  FALSE - Otherwise

SIDE EFFECTS
  None

===========================================================================*/

boolean mccreg_powerup( void )
{
  nv_stat_enum_type read_status;  
    /* NV read status */
  mccreg_zone_type *zone_ptr;
    /* Pointer to entry from ZONE_LISTs */
  mccreg_sid_nid_type *sid_ptr;
    /* Pointer to (SID,NID) nid pair on SID_NID_LISTs */
  boolean read_ok = TRUE; 
    /* indicates if items were read from NV successfully */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* -------------------------------------------------
  ** Set indicator that the mobile is in powerup state
  ** ------------------------------------------------- */
  mccreg_vars.powering_up = TRUE;

  /* ---------------------------------------------------------------------
  ** Upon powerup, the mobile station shall perform the following actions:
  ** --------------------------------------------------------------------- */

  /* Enter Critical Section */
  REX_ISR_LOCK(&mc_reg_crit_sect);
  
  /* --------------------------------
  ** Delete all entries of ZONE_LISTs
  ** -------------------------------- */
  while ((zone_ptr = (mccreg_zone_type*) q_get( &mccreg_zone_q )) != NULL)
  {
    q_put( &mccreg_zone_free_q, &zone_ptr->link );
  }

  /* Clear the zone list item with a disabled timer if it exists */
  if (mccreg_zone_ptr != NULL)
  {
    q_put( &mccreg_zone_free_q, &mccreg_zone_ptr->link );
    mccreg_zone_ptr = NULL;
  }

  if (cdma.is_cdma_subs_avail)
  {
    /* ---------------------------------------------------------------
    ** If ZONE_LISTs-p contains an entry, copy the entry to ZONE_LISTs
    ** and disable the corresponding entry timer
    ** --------------------------------------------------------------- */
    /* Retrieve zone list from NV */
    mccreg_nv_buf.item = NV_ZONE_LIST_I;
    mccreg_nv_buf.cmd  = NV_READ_F;
    mccreg_nv_buf.data_ptr = &mccreg_nv_data;

    read_status = mcc_nv_cmd( &mccreg_nv_buf );

    if ((read_status != NV_DONE_S) && (read_status != NV_NOTACTIVE_S))
    {
      /* Check if def val for pwrup nv err feature is enabled */
      if(cdma.use_def_val_for_pwrup_nv_err == TRUE)
      {
        M1X_MSG( DCP, LEGACY_ERROR,
                 "Bad NV read, Item %d, Cmd %d, Status:%d",
                 mccreg_nv_buf.item, 
                 mccreg_nv_buf.cmd, 
                 read_status );
        read_ok = FALSE;
      }
      else
      {
        ERR_FATAL("Bad NV read, Item %d, Cmd %d, Status:%d",
                  mccreg_nv_buf.item, 
                  mccreg_nv_buf.cmd, 
                  read_status);
      } /* (cdma.use_def_val_for_pwrup_nv_err == FALSE) */
    }

    if ((read_status == NV_DONE_S) && (mccreg_nv_data.zone_list.sid != 0))
    {
      if ((mccreg_zone_ptr =
           (mccreg_zone_type*) q_get( &mccreg_zone_free_q )) != NULL)
      {
        mccreg_zone_ptr->reg_zone = mccreg_nv_data.zone_list.reg_zone;
        mccreg_zone_ptr->sid = mccreg_nv_data.zone_list.sid;
        mccreg_zone_ptr->nid = mccreg_nv_data.zone_list.nid;
        mccreg_zone_ptr->band_class = mccreg_nv_data.zone_list.band_class;
        mccreg_zone_ptr->block_or_sys = mccreg_nv_data.zone_list.block_or_sys;
      }
      else
      {
        ERR_FATAL( "zone_free_q error", 0, 0, 0 );
      }
    }
  } /* if (cdma.is_cdma_subs_avail) */

  /* -----------------------------------
  ** Delete all entries of SID_NID_LISTs
  ** ----------------------------------- */
  while ((sid_ptr = (mccreg_sid_nid_type*) q_get( &mccreg_sid_nid_q )) != NULL)
  {
    q_put( &mccreg_sid_nid_free_q, &sid_ptr->link );
  }

  /* Clear the SID_NID_LISTs item with a disabled timer if it exists */
  if (mccreg_sid_nid_ptr != NULL)
  {
    q_put( &mccreg_sid_nid_free_q, &mccreg_sid_nid_ptr->link );
    mccreg_sid_nid_ptr = NULL;
  }

  if (cdma.is_cdma_subs_avail)
  {
    /* ---------------------------------------------------------------------
    ** If SID_NID_LISTs-p contains an entry, copy the entry to SID_NID_LISTs
    ** and disable the corresponding entry timer
    ** --------------------------------------------------------------------- */
    mccreg_nv_buf.item = NV_SID_NID_LIST_I;
    mccreg_nv_buf.cmd  = NV_READ_F;
    mccreg_nv_buf.data_ptr = &mccreg_nv_data;

    read_status = mcc_nv_cmd( &mccreg_nv_buf );

    if ((read_status != NV_DONE_S) && (read_status != NV_NOTACTIVE_S))
    {
      /* Check if def val for pwrup nv err feature is enabled */
      if(cdma.use_def_val_for_pwrup_nv_err == TRUE)
      {
        M1X_MSG( DCP, LEGACY_ERROR,
                 "Bad NV read, Item %d, Cmd %d, Status:%d",
                 mccreg_nv_buf.item, 
                 mccreg_nv_buf.cmd, 
                 read_status );
        read_ok = FALSE;
      }
      else
      {
        ERR_FATAL("Bad NV read, Item %d, Cmd %d, Status:%d",
                  mccreg_nv_buf.item, 
                  mccreg_nv_buf.cmd, 
                  read_status);
      } /* (cdma.use_def_val_for_pwrup_nv_err == FALSE) */
    }

    if ((read_status == NV_DONE_S) && (mccreg_nv_data.sid_nid_list.sid != 0))
    {
      if ((mccreg_sid_nid_ptr =
           (mccreg_sid_nid_type*) q_get( &mccreg_sid_nid_free_q )) != NULL)
      {
        mccreg_sid_nid_ptr->sid = mccreg_nv_data.sid_nid_list.sid;
        mccreg_sid_nid_ptr->nid = mccreg_nv_data.sid_nid_list.nid;
        mccreg_sid_nid_ptr->band_class = mccreg_nv_data.sid_nid_list.band_class;
        mccreg_sid_nid_ptr->block_or_sys =
          mccreg_nv_data.sid_nid_list.block_or_sys;
      }
      else
      {
        ERR_FATAL( "sid_nid_free_q error", 0, 0, 0 );
      }
    }
  } /* if (cdma.is_cdma_subs_avail) */
  
  /* Leave Critical Section */
  REX_ISR_UNLOCK(&mc_reg_crit_sect);
  
  
  /* -----------------------------------------------
  ** Initialize last registered SCI to default value
  ** ----------------------------------------------- */
  mcc_set_last_sci_reg ( MCC_DEFAULT_SCI );
  
  if (cdma.is_cdma_subs_avail)
  {
    /* ---------------------------------------------------------------------
    ** Read the last_registered_sci value from NV
    ** --------------------------------------------------------------------- */
    mccreg_nv_buf.item = NV_LAST_REGISTERED_SCI_I;
    mccreg_nv_buf.cmd  = NV_READ_F;
    mccreg_nv_buf.data_ptr = &mccreg_nv_data;

    read_status = mcc_nv_cmd( &mccreg_nv_buf );

    if ((read_status != NV_DONE_S) && (read_status != NV_NOTACTIVE_S))
    {
      /* Check if def val for pwrup nv err feature is enabled */
      if(cdma.use_def_val_for_pwrup_nv_err == TRUE)
      {
        M1X_MSG( DCP, LEGACY_ERROR,
                 "Bad NV read, Item %d, Cmd %d, Status:%d",
                 mccreg_nv_buf.item, 
                 mccreg_nv_buf.cmd, 
                 read_status );
        read_ok = FALSE;
      }
      else
      {
        ERR_FATAL("Bad NV read, Item %d, Cmd %d, Status:%d",
                  mccreg_nv_buf.item, 
                  mccreg_nv_buf.cmd, 
                  read_status);
      } /* (cdma.use_def_val_for_pwrup_nv_err == FALSE) */
    }
    else
    {
      mcc_set_last_sci_reg ( mccreg_nv_data.last_registered_sci );
    }
  } /* if (cdma.is_cdma_subs_avail) */

  /* -----------------------------
  ** The mobile needs to register.
  ** ----------------------------- */
  mccreg_vars.reg_status = MCCREG_REG_NEEDED;

  /* -------------------------------------------------------------------
  ** Set timer-based registration enable status (COUNTER_ENABLEDs) to NO
  ** ------------------------------------------------------------------- */
  mccreg_vars.counter_enabled = FALSE;

  /* --------------------------------------------------------------
  ** Set autonomous registration enable status (REG_ENABLEDs) to NO
  ** -------------------------------------------------------------- */
  mccreg_vars.reg_enabled = FALSE;

  /* -------------------------------------------------------------
  ** Set BASE_LAT_REGs-p, BASE_LONG_REGs-p and REG_DIST_REGs-p
  ** to 0
  ** ------------------------------------------------------------- */
  mccreg_vars.base_lat_reg  = 0;
  mccreg_vars.base_long_reg = 0;
  mccreg_vars.reg_dist      = 0;

  if (cdma.is_cdma_subs_avail)
  {
    /* -------------------------------------------------------------
    ** Copy in BASE_LAT_REGs-p, BASE_LONG_REGs-p and REG_DIST_REGs-p
    ** from NV now
    ** ------------------------------------------------------------- */
    mccreg_nv_buf.item = NV_DIST_REG_I;
    mccreg_nv_buf.cmd  = NV_READ_F;
    mccreg_nv_buf.data_ptr = &mccreg_nv_data;

    read_status = mcc_nv_cmd( &mccreg_nv_buf );

    if ((read_status != NV_DONE_S) && (read_status != NV_NOTACTIVE_S))
    {
      /* Check if def val for pwrup nv err feature is enabled */
      if(cdma.use_def_val_for_pwrup_nv_err == TRUE)
      {
        M1X_MSG( DCP, LEGACY_ERROR,
                 "Bad NV read, Item %d, Cmd %d, Status:%d",
                 mccreg_nv_buf.item, 
                 mccreg_nv_buf.cmd, 
                 read_status );
        read_ok = FALSE;
      }
      else
      {
        ERR_FATAL("Bad NV read, Item %d, Cmd %d, Status:%d",
                  mccreg_nv_buf.item, 
                  mccreg_nv_buf.cmd, 
                  read_status);
      } /* (cdma.use_def_val_for_pwrup_nv_err == FALSE) */
    }

    if (read_status == NV_DONE_S)
    {
      mccreg_vars.base_lat_reg  = mccreg_nv_data.dist_reg.base_lat_reg;
      mccreg_vars.base_long_reg = mccreg_nv_data.dist_reg.base_long_reg;
      mccreg_vars.reg_dist      = mccreg_nv_data.dist_reg.reg_dist_reg;
    }
  } /* if (cdma.is_cdma_subs_avail) */

  /* Initialize other registration indicators */
  mccreg_vars.powerup_timer_on = FALSE;
  mccreg_vars.mob_term = FALSE;
  mccreg_vars.timer_cnt = 0;

  mccreg_vars.reg_sci = MCC_NULL_SCI;

  return read_ok;

} /* mccreg_powerup */

/*===========================================================================

FUNCTION MCCREG_POWERUP_MODE_ONLINE

DESCRIPTION
  This function performs power-up registration procedures as called out
  in IS-95A section 6.6.5.5.1.1.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mccreg_powerup_mode_online( void )
{

  /* -------------------------------------------------
  ** Set indicator that the mobile is in powerup state
  ** ------------------------------------------------- */
  mccreg_vars.powering_up = TRUE;

  /* -----------------------------
  ** The mobile needs to register.
  ** ----------------------------- */
  mccreg_vars.reg_status = MCCREG_REG_NEEDED;

  /* -------------------------------------------------------------------
  ** Set timer-based registration enable status (COUNTER_ENABLEDs) to NO
  ** ------------------------------------------------------------------- */
  mccreg_vars.counter_enabled = FALSE;

  /* --------------------------------------------------------------
  ** Set autonomous registration enable status (REG_ENABLEDs) to NO
  ** -------------------------------------------------------------- */
  mccreg_vars.reg_enabled = FALSE;

  /* Initialize other registration indicators */
  mccreg_vars.powerup_timer_on = FALSE;
  mccreg_vars.mob_term = FALSE;
  mccreg_vars.timer_cnt = 0;

  mccreg_vars.reg_sci = MCC_NULL_SCI;

} /* mccreg_powerup_mode_online */

/*===========================================================================

FUNCTION MCCREG_ACQ_INIT

DESCRIPTION
  This function performs registration variable initialization for a mode
  or serving system change as called out in IS-95A section 6.6.5.5.1.1.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Flags are initialized.

===========================================================================*/

void mccreg_acq_init( void )
{

    mccreg_vars.reg_enabled = FALSE;

}  /* mccreg_acq_init */

/*===========================================================================

FUNCTION MCCREG_IDLE_INIT

DESCRIPTION
  This function performs processing necessary when entering the Idle state
  from the Initialization state (see IS-95A section 6.6.5.5.1.3).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mccreg_idle_init( void )
{
  /* ----------------------------------------------------------------
  ** If the mobile needs to register, enable the powerup/initialization
  ** timer with an expiration time of T57m seconds.
  ** ---------------------------------------------------------------- */

  if (mcc_post_otasp_reg_pending && mccreg_vars.powering_up)
  {
    /* Reset registration status if post OTASP registration is pending
     * and powering up indication is ON. This indicates that power down
     * registration procedure has been done.
     */
    mccreg_vars.reg_status = MCCREG_REG_NEEDED;
    mcc_post_otasp_reg_pending = FALSE;
  }

  if ((mccreg_vars.reg_status == MCCREG_REG_NEEDED) && (mccreg_vars.powering_up))
  {
    timer_reg( &mccreg_powerup_timer, (timer_t2_cb_type)mccreg_powerup_timer_done,
               (timer_cb_data_type)NULL, (uint32) cai_tmo.t57m, 0L );
    mccreg_vars.powerup_timer_on = TRUE;
    M1X_MSG( DCP, LEGACY_MED,
      "Start powerup timer" );
  }

  /* Make sure that no pending distance registrations are done */
  mccreg_vars.dist_reg_reqd = FALSE;

  /* Register the zone timer */
  timer_reg( &mccreg_zone_timer, (timer_t2_cb_type)mccreg_zone_timer_done,
             (timer_cb_data_type)NULL, 5000L, 5000L );

} /* mccreg_idle_init */

/*===========================================================================

FUNCTION MCCREG_UPDATE_LISTS

DESCRIPTION
  This function updates and expires timers on the sid nid and zone lists.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mccreg_update_lists( void )
{
  mccreg_sid_nid_type *sid_ptr;
    /* Pointer used to loop through sid nid list */
  mccreg_zone_type *zone_ptr;
    /* Pointer used to loop through zone list */
  boolean next = FALSE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Enter Critical Section */
  REX_ISR_LOCK(&mc_reg_crit_sect);
  
  /* Loop through the zone list expiring */
  zone_ptr = (mccreg_zone_type*) q_check( &mccreg_zone_q );
  sid_ptr = (mccreg_sid_nid_type*) q_check( &mccreg_sid_nid_q );

  /* we need to update the time counts when either zone or sid list needs
   * update, since there's only one timer count mccreg_vars.timer_cnt */
  if (((zone_ptr != NULL) && (mccreg_vars.timer_cnt >= zone_ptr->cnt)) ||
      ((sid_ptr!= NULL) && (mccreg_vars.timer_cnt >= sid_ptr->cnt)))
  {
    /* This loop depends on a priority queue */
    next = FALSE;
    while (zone_ptr != NULL)
    {
      zone_ptr->cnt -= mccreg_vars.timer_cnt;

      if (zone_ptr->cnt <= 0)
      {
        if (next)
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "zone_q update error" );
        }
        else
        {
          zone_ptr = (mccreg_zone_type*) q_get( &mccreg_zone_q );
          q_put( &mccreg_zone_free_q, &zone_ptr->link);
          zone_ptr = (mccreg_zone_type*) q_check(&mccreg_zone_q );
        }
      }
      else
      {
        zone_ptr =
           (mccreg_zone_type*) q_next( &mccreg_zone_q, &zone_ptr->link );
        next = TRUE;  /* indicate we are looping through queue items */
      }
    }

    /* loop through the sid nid list updating counts */
    next = FALSE;
    while (sid_ptr != NULL)
    {
      sid_ptr->cnt -= mccreg_vars.timer_cnt;

      if (sid_ptr->cnt <= 0)
      {
        if (next)
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "sid nid queue update error" );
        }
        else
        {
          sid_ptr = (mccreg_sid_nid_type*) q_get( &mccreg_sid_nid_q );
          q_put( &mccreg_sid_nid_free_q, &sid_ptr->link );
          sid_ptr = (mccreg_sid_nid_type*) q_check(&mccreg_sid_nid_q );
        }
      }
      else
      {
        sid_ptr = (mccreg_sid_nid_type*) q_next( &mccreg_sid_nid_q,
                                                 &sid_ptr->link );
        next = TRUE;
      }
    }

    mccreg_vars.timer_cnt = 0;
  } /* End if() */
  
  /* Leave Critical Section */
  REX_ISR_UNLOCK(&mc_reg_crit_sect);
  
} /* mccreg_update_lists */

/*===========================================================================

FUNCTION MCCREG_IDLE_CHECK

DESCRIPTION
  This function performs idle registration procedures called out in
  IS-95A section 6.6.5.5.2.1.

DEPENDENCIES
  None.

RETURN VALUE
  If a registration is required, the type of registration required will be
  returned.  Otherwise the special indicator MCCREG_NO_REG_REQUIRED will
  be returned.

SIDE EFFECTS
  None.

===========================================================================*/

byte mccreg_idle_check( void )
{
  mccreg_zone_type *zone_ptr;
    /* Pointer used to loop through zone list */
  boolean found;
  qword reg_count_max;
  qword reg_count;
  nv_block_or_sys_enum_type block_or_sys = NV_BLOCK_OR_SYS_MAX;
    /* Cellular serv-sys (A or B) or PCS frequency block (A-F) of current BS */

  byte reg_prd;               /* Registration period */
  boolean power_up_reg;       /* Power-up registration indicator */
  word reg_distance;          /* Registration distance */
  boolean parameter_reg;      /* Parameter-change registration  */
  word sid;                   /* System identification */
  word nid;                   /* Network identification */
  word reg_zone;              /* Registration zone */
  byte total_zones;           /* Number of registration zones
                                 to be retained */
  /* variables to store reg related params */


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* This prevents all kinds of registration without updating the necessary
  data structures
  */

  /* If RUIM card is not available on a RUIM enable only build, we should
     not do any registration.
  */
  if (!cdma.is_cdma_subs_avail)
  {
    return MCCREG_NO_REG_REQUIRED;
  }
  #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
  if(cdma.entry_ptr->entry != MCC_CSFB)
  #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
  {
    /* If mobile is locked, it shall not perform any registration. */
    if ( sd_misc_is_cdma_locked() )
    {
      return MCCREG_NO_REG_REQUIRED;
    }
  } /* if(cdma.entry_ptr->entry != MCC_CSFB) */

  #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
  if(cdma.entry_ptr->entry != MCC_CSFB)
  #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
  {
    if(mcc_is_jcdma_supported() &&
       mcc_msg_is_blocked(CDMA_MSG_TYPE_REG))
    {
      return MCCREG_NO_REG_REQUIRED;
    }
  } /* if(cdma.entry_ptr->entry != MCC_CSFB) */

  #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
  if(cdma.entry_ptr->entry != MCC_CSFB)
  #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
  {
    #ifdef FEATURE_HDR
    #ifdef FEATURE_DDTM_CNTL
    /*
     * When MS is in a HDR call and user has indicated of no HDR interruption,
     * (the dedicated data transfer mode), we do NOT do registration.
    */
    if ((cdma.ddtm.status) &&
        (cdma.ddtm.ddtm_act_mask & SYS_DDTM_ACT_MASK_SUPPRESS_REG)
       )
    {
      return MCCREG_NO_REG_REQUIRED;
    }
    #endif /* FEATURE_DDTM_CNTL */
    #endif /* FEATURE_HDR */
  } /* if(cdma.entry_ptr->entry != MCC_CSFB) */

  /* --------------------------------------------------------------
  ** If network leaves various registration mechanisms enabled while
  ** sets PSIST(0-9) to 63, then access attempts for those registrations
  ** are denied and DMSS exits CDMA as a result.  If a user is attempting
  ** to do an emergency origination, by the time MC reports that Overhead
  ** is OK to SD and SD let's CM know that it is OK to orig/redial,
  ** MC may already attempt to register and will exit again.  Avoid this
  ** situation by checking with SD if an emergency origination is coming soon
  ** and if so, don't attempt to register if the PSIST settings would prevent
  ** the registration message from being transmitted.
  ** -------------------------------------------------------------- */
  #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
  if(cdma.entry_ptr->entry != MCC_CSFB)
  #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
  {
    if ( sd_misc_is_orig_mode(SD_SS_MAIN, SD_SS_ORIG_MODE_VAL_EMERG)
         ||
         (
         mcc_passport_mode_is_jcdma() &&
         sd_misc_is_orig_mode(SD_SS_MAIN, SD_SS_ORIG_MODE_VAL_ORIG)
         )
       )
    {
      access_result_type reg_access = NOT_ABORT;

      /* See if the registration would be denied based on PSIST settings.
      mccreg_idle_check() is only called for request type of registrations. */
      reg_access = mccsa_calc_psist(CAI_ACH, &(cur_bs_ptr->ap), CDMA_REG);   /*lint !e641 */

      if (reg_access == ABORT_PSIST)
      {
        M1X_MSG( DCP, LEGACY_HIGH,
          "Skip Reg to catch coming Emerg Orig/Silent Redial");
        return MCCREG_NO_REG_REQUIRED;
      }
    }
  } /*   if(cdma.entry_ptr->entry != MCC_CSFB) */
  /* --------------------------------------------------------------
  ** If the powerup/initialization timer has expired or is disabled
  ** the mobile station shall perform the following actions in the
  ** order given:
  ** -------------------------------------------------------------- */
  if (!mccreg_vars.powerup_timer_on)
  {
    /* -----------------------------------------------------------------
    ** 1. The timer-based registration timer (COUNTER_ENABLEDs) shall be
    **    enabled and the timer count (REG_COUNTs) shall be set to a
    **    pseudorandom number if the following conditions are met:
    **      a. COUNTER_ENABLEDs is equal to NO; and
    **      b. The stored configuration parameters are current 
    **         if Split Ovhds feature not supported; and
    **      c. REG_ENABLEDs is equal to YES; and
    **      d. REG_PRDs is not equal to zero.
    ** -------------------------------------------------------------- */
    #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
    if(cdma.entry_ptr->entry == MCC_CSFB)
    {
      reg_prd = mcc_csfb_sib8_msg.reg_params.registrationPeriod;
    }
    else
    #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
    {
      reg_prd = cur_bs_ptr->csp.sp.reg_prd;
    }
    if ( (!mccreg_vars.counter_enabled) && 
         #ifdef FEATURE_1X_SPLIT_OVERHEAD
         ( (cdma.split_ovhd_supported == TRUE) ||
           (mcc_is_ovhd_cfg_okay())
         ) &&
         #endif /* FEATURE_1X_SPLIT_OVERHEAD */
         (mccreg_vars.reg_enabled) && 
         (reg_prd != 0)
       )
    {
      /* Compute the timer expiration count (REG_COUNT_MAXs) */
      mccreg_vars.reg_count_max =
        reg_count_table[reg_prd - MIN_REG_PERIOD];

        /* Set REG_COUNTs to a pseudorandom value between 0 and
           REG_COUNT_MAXs - 1.  (Calculate reg_count with qwords
           since ran_dist only works for words) */
        qw_set( reg_count_max, 0L, mccreg_vars.reg_count_max - 1 );
        qw_mul( reg_count, reg_count_max, mc_sec_ran_next() );
        mccreg_vars.reg_count_ms = qw_hi( reg_count ) * 80;


      M1X_MSG( DCP, LEGACY_MED,
        "REG_COUNTs is set to %ul",
        mccreg_vars.reg_count_ms);
      M1X_MSG( DCP, LEGACY_MED,
        "REG_COUNT_MAXs is set to %ul",
        mccreg_vars.reg_count_max);
      /* Start timer-based registration */
      mccreg_vars.counter_enabled = TRUE;
      M1X_MSG( DCP, LEGACY_MED,
        "Start timer-based registration");
      timer_reg( &mccreg_timer_reg_timer, (timer_t2_cb_type)mccreg_timer_reg_timer_done,
                 (timer_cb_data_type)NULL, 80L, 80L );
    }

    /* ------------------------------------------------------------------
    ** 2. If any zone list entry timer has expired, the mobile station
    **    shall delete the corresponding entry from ZONE_LISTs.
    ** 3. If any SID/NID list entry timer has expired, the mobile station
    **    shall delete the corresponding entry from SID_NID_LISTs.
    ** ------------------------------------------------------------------ */
    mccreg_update_lists();

    /* ---------------------------------------------------------------
    ** 4.1 The mobile station shall perform powerup registration if all
    **     the following conditions are met:
    **       a. POWER_UP_REGs is equal to '1'; and
    **       b. The stored configuration parameters are current 
    **          if Split Ovhds feature not supported; and
    **       c. The mobile has not registered; and
    **       d. REG_ENABLEDs is equal to YES.
    ** --------------------------------------------------------------- */
    #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
    if(cdma.entry_ptr->entry == MCC_CSFB)
    {
      power_up_reg = mcc_csfb_sib8_msg.reg_params.powerUpReg;
    }
    else
    #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
    {
      power_up_reg = cur_bs_ptr->csp.sp.power_up_reg;
    } /* if(cdma.entry_ptr->entry == MCC_CSFB) */

    if ( 
         #ifdef FEATURE_1X_SPLIT_OVERHEAD
         ( (cdma.split_ovhd_supported == TRUE) ||
           (mcc_is_ovhd_cfg_okay())
         ) &&
         #endif /* FEATURE_1X_SPLIT_OVERHEAD */
         (power_up_reg) &&
         (mccreg_vars.reg_status == MCCREG_REG_NEEDED) && 
         (mccreg_vars.reg_enabled)
       )
    {
      return (CAI_POWERUP_REG);
    }

    /* ---------------------------------------------------------------
    ** 4.2 The mobile station shall perform powerup registration if all
    **     the following conditions are met:
    **      a. POWER_UP_REGs is equal to '1'; and
    **      b. The stored configuration parameters are current 
    **         if Split Ovhds feature not supported; and
    **      c. The mobile has registered, but the band class,
               frequency block or serving system is different from last
               regsitration; and
    **      d. REG_ENABLEDs is equal to YES.
    ** --------------------------------------------------------------- */
    /* Get the frequency block or serving system of the current BS */
    #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
    if(cdma.entry_ptr->entry != MCC_CSFB)
    #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
    {
      block_or_sys = (uint16)
         sd_map_chan_to_blksys(
           SD_MODE_CDMA,
           (sys_band_class_e_type) cdma.band_class,
           cdma.cdmach);
    }
    // func is returing enum (32 bit) and assigning to 16 bit field,
    // but values are low enough to fit without any problem.

    if ( 
         #ifdef FEATURE_1X_SPLIT_OVERHEAD
         ( (cdma.split_ovhd_supported == TRUE) ||
           (mcc_is_ovhd_cfg_okay())
         ) &&
         #endif /* FEATURE_1X_SPLIT_OVERHEAD */
         (power_up_reg) &&
         (mccreg_vars.reg_status == MCCREG_REG_COMPLETED) && 
         (mccreg_vars.reg_enabled) &&
         ( ( ( (cdma.band_class != mccreg_vars.band_class)
               #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
               || (csfb_cdma.registered_band_class != mccreg_vars.band_class)
               #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
             )
             #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
             /* This check is to prevent power-up registration 
              * when we are in CSFB mode/ 1X mode and the band_class 
              * we registered/using is Wild Class.
              */
             && ( (cdma.band_class != (uint16)MCC_BAND_CLS_WILDCARD) &&
                  (mccreg_vars.band_class) != (uint16)MCC_BAND_CLS_WILDCARD
                )
             #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
           )
           ||
           (
             #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
             /* This check is to prevent power-up registration 
              * when we are in CSFB mode.
              */
             (cdma.entry_ptr->entry != MCC_CSFB) &&
             /* This check is to prevent power-up registration 
              * when we move from LTE to 1X.
              */
             (mccreg_vars.block_or_sys != (uint16)MCC_BLK_SYS_WILDCARD) &&
             #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
             (block_or_sys != mccreg_vars.block_or_sys)
           )
         )

       )
    {
      return (CAI_POWERUP_REG);
    }

    /* ------------------------------------------------------------------
    ** 5. The mobile station shall perform parameter-change registration
    **    if all the following conditions are met:
    **      a. PARAMETER_REGs is equal to '1'; and
    **      b. The stored configuration parameters are current 
    **         if Split Ovhds feature not supported and
    **      c. There is no entry of SID_NID_LISTs whose SID and NID
    **         fields match the stored SIDs and NIDs or Current
    **         preferred SCI is different from previously registered one.
    ** ------------------------------------------------------------------ */
    #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
    if(cdma.entry_ptr->entry == MCC_CSFB)
    {
      parameter_reg = mcc_csfb_sib8_msg.reg_params.parameterReg;
      sid = mcc_csfb_sib8_msg.reg_params.sid;
      nid = mcc_csfb_sib8_msg.reg_params.nid;
    }
    else
    #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
    {
      parameter_reg = cur_bs_ptr->csp.sp.parameter_reg;
      sid = cur_bs_ptr->csp.sp.sid;
      nid = cur_bs_ptr->csp.sp.nid;
    } /* if(cdma.entry_ptr->entry == MCC_CSFB) */

    if ( 
         #ifdef FEATURE_1X_SPLIT_OVERHEAD
         ( (cdma.split_ovhd_supported == TRUE) ||
           (mcc_is_ovhd_cfg_okay())
         ) &&
         #endif /* FEATURE_1X_SPLIT_OVERHEAD */
         (parameter_reg) &&
         ( (mccreg_find_sid_nid(sid, nid)== NULL) ||
           (mcc_get_last_sci_reg() != mcc_convert_pref_sci_to_ota_sci())
         )
       )
    {
      return (CAI_PARAMETER_REG);
    }

    /* ---------------------------------------------------------------
    ** 6. The mobile station shall perform timer-based registration if
    **    all the following conditions are met:
    **      a. COUNTER_ENABLEDs is equal to YES; and
    **      b. The stored configuration parameters are current
    **         if Split Ovhds feature not supported; and
    **      c. REG_ENABLEDs is equal to YES.
    **      d. REG_COUNTs is greater than or equal to REG_COUNT_MAXs.
    ** --------------------------------------------------------------- */
    if ( (mccreg_vars.counter_enabled) &&
         #ifdef FEATURE_1X_SPLIT_OVERHEAD
         ( (cdma.split_ovhd_supported == TRUE) ||
           (mcc_is_ovhd_cfg_okay())
         ) &&
         #endif /* FEATURE_1X_SPLIT_OVERHEAD */
         (mccreg_vars.reg_enabled) &&
         (mccreg_vars.reg_count_ms >= (mccreg_vars.reg_count_max * 80))
       )
    {
      M1X_MSG( DCP, LEGACY_MED,
        "REG_COUNTs is greater than or equal to REG_COUNT_MAXs");
      return (CAI_TIMER_REG);
    }

    /* ------------------------------------------------------------------
    ** 7. The mobile station shall perform distance-based registration if
    **    all the following conditions are met:
    **      a. REG_DISTs is not equal to zero; and
    **      b. The stored configuration parameters are current 
    **         if Split Ovhds feature not supported; and
    **      c. REG_ENABLEDs is equal to YES.
    **      d. The current base station's distance from the base station
    **         in which the mobile last registered is greater than or
    **         equal to REG_DIST_REGs-p.
    ** ------------------------------------------------------------------ */
    #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
    if(cdma.entry_ptr->entry != MCC_CSFB)
    #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
    {
      reg_distance = cur_bs_ptr->csp.sp.reg_dist;
      if (
           #ifdef FEATURE_1X_SPLIT_OVERHEAD
           ( (cdma.split_ovhd_supported == TRUE) ||
             (mcc_is_ovhd_msg_okay())
           ) &&
           #endif /* FEATURE_1X_SPLIT_OVERHEAD */
           (reg_distance) &&
           (mccreg_vars.reg_enabled)
         )
      {
        /* Check for distance based registration calculations done while
           processing the System Parameters Message */
        if (mccreg_vars.dist_reg_reqd)
        {
          mccreg_vars.dist_reg_reqd = FALSE;
          return (CAI_DISTANCE_REG);
        }
      }
    } /* if(cdma.entry_ptr->entry != MCC_CSFB) */

    /* ------------------------------------------------------------------
    ** 8. The mobile station shall perform zone-based registration if all
    **    the following conditions are met:
    **      a. TOTAL_ZONESs is not equal to zero; and
    **      b. The stored configuration parameters are current 
    **         if Split Ovhds feature not supported; and
    **      c. REG_ENABLEDs is equal to YES.
    **      d. There is no entry of ZONE_LISTs whose SID, NID and
    **         REG_ZONE fields match the stored SIDs, NIDs and REG_ZONEs.
    ** ------------------------------------------------------------------ */
    #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
    if(cdma.entry_ptr->entry == MCC_CSFB)
    {
      total_zones = mcc_csfb_sib8_msg.reg_params.totalZone;
      reg_zone = mcc_csfb_sib8_msg.reg_params.registrationZone;
    }
    else
    #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
    {
      total_zones = cur_bs_ptr->csp.sp.total_zones;
      reg_zone = cur_bs_ptr->csp.sp.reg_zone;
    } /*  if(cdma.entry_ptr->entry == MCC_CSFB) */

    if ( 
         #ifdef FEATURE_1X_SPLIT_OVERHEAD
         ( (cdma.split_ovhd_supported == TRUE) ||
           (mcc_is_ovhd_cfg_okay())
         ) &&
         #endif /* FEATURE_1X_SPLIT_OVERHEAD */
         (total_zones!= 0) &&
         (mccreg_vars.reg_enabled)
       )
    {
      /* Enter Critical Section */
      REX_ISR_LOCK(&mc_reg_crit_sect);
      
      if ((mccreg_zone_ptr != NULL) &&
          (mccreg_zone_ptr->sid == sid) &&
          ((mccreg_zone_ptr->nid == nid) ||
           (mccreg_zone_ptr->nid == CAI_HOME_NID)) &&
          (mccreg_zone_ptr->reg_zone == reg_zone))
      {
        found = TRUE;
      }
      else
      {
        found = FALSE;

        /* Check zone list for current ZONE, SID, and NID */
        zone_ptr = (mccreg_zone_type*) q_check( &mccreg_zone_q );

        while ((zone_ptr != NULL) && (!found))
        {
          if ((zone_ptr->sid == sid) &&
              ((zone_ptr->nid == nid) ||
               (zone_ptr->nid == CAI_HOME_NID)) &&
              (zone_ptr->reg_zone == reg_zone))
          {
            found = TRUE;
          }
          else
          {
            zone_ptr = (mccreg_zone_type*) q_next( &mccreg_zone_q,
                                                   &zone_ptr->link );
          }
        } /* end while ((zone_ptr != NULL) && (!found)) */
      }

      /* Leave Critical Section */
      REX_ISR_UNLOCK(&mc_reg_crit_sect);
  
      if (!found)
      {
        return (CAI_ZONE_REG);
      }
    }

    #ifdef FEATURE_IS2000_REL_C
    /* ------------------------------------------------------------------
    ** 10. The mobile station shall perform encryption/message integrity
    **     re-sync required registration (see 2.6.5.1.11) if all the following
    **     conditions are met:
    **       a. REG_ENCRYPTSECURITY_RESYNC is equal to YES or
    **         REGISTER_IN_IDLEs is equal to '1'; and
    **      b. None of the above registrations have been performed since
    **        the last entering of the Mobile Station Idle State.
    ** ------------------------------------------------------------------ */
    #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
    if(cdma.entry_ptr->entry != MCC_CSFB)
    #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
    {
      if (mccreg_vars.register_in_idle)
      {
        /* cdma.register_in_idle will be reset after any successful registration */
        return ( CAI_ENC_RESYNC_REG );
      }
    } /* if(cdma.entry_ptr->entry != MCC_CSFB) */
    #endif /* FEATURE_IS2000_REL_C */

  }

  /* If we made it to this point then no registration is required */
  return (MCCREG_NO_REG_REQUIRED);

} /* mccreg_idle_check */

/*===========================================================================

FUNCTION MCCREG_SET_MOB_TERM

DESCRIPTION
  This function sets the call termination enabled indicator (MOB_TERMs)
  based on the input system parameters.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mccreg_set_mob_term
(
  word sid,
    /* System ID */
  word nid
    /* Network ID */
)
{
  word i;
    /* SID/NID pair index */
  boolean for_nid_roam;
    /* Indicator that mobile is a foreign NID roamer */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize variables */
  for_nid_roam = FALSE;

  /* Determine if the mobile station is a foreign NID roamer */

  for (i=0; i < NV_MAX_HOME_SID_NID; i++)

  {
    if (sid == cdma.pair[i].sid)
    {
      if ((nid == cdma.pair[i].nid) || (cdma.pair[i].nid == CAI_HOME_NID))
      {
        for_nid_roam = FALSE;
        break;
      }
      else
      {
        for_nid_roam = TRUE;
      }
    }
  } /* end for (i=0; i < NV_MAX_SID_NID; i++) */


  if (i < NV_MAX_HOME_SID_NID)

  {
    /* -----------------------------------------------------------------
    ** Must be a home (SID,NID) pair - MOB_TERMs is set based on whether
    ** mobile is configured to received mobile terminated calls when
    ** using a home (SID,NID) pair.
    ** ----------------------------------------------------------------- */
    mccreg_vars.mob_term = cdma.mob_term_home;
  }
  else if (for_nid_roam)
  {
    /* ----------------------------------------------------------------
    ** The mobile station is a foreign NID roamer - set MOB_TERMs based
    ** on whether the mobile is configured to receive mobile-terminated
    ** calls in a foreign NID
    ** ---------------------------------------------------------------- */
    mccreg_vars.mob_term = cdma.mob_term_for_nid;
  }
  else
  {
    /* ----------------------------------------------------------------
    ** The mobile station is a foreign SID roamer - set MOB_TERMs based
    ** on whether the mobile is configured to receive mobile-terminated
    ** calls in a foreign SID
    ** ---------------------------------------------------------------- */
    mccreg_vars.mob_term = cdma.mob_term_for_sid;
  }
} /* end mccreg_set_mob_term() */

/*===========================================================================

FUNCTION MCCREG_SET_REG_ENABLED

DESCRIPTION
  This function sets the registration enabled indicator based on
  the input system parameters.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mccreg_set_reg_enabled
(
  word sid,
    /* System ID */
  word nid,
    /* Network ID */
  boolean home_reg,
    /* Home registration enabled indicator */
  boolean for_nid_reg,
    /* Foreign NID roamer registration enabled indicator */
  boolean for_sid_reg
    /* Foreign SID roamer registration enabled indicator */
)
{
  word i;
    /* SID/NID pair index */
  boolean for_nid_roam;
    /* Indicator that mobile is a foreign NID roamer */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize variables */
  mccreg_vars.reg_enabled = FALSE;
  for_nid_roam = FALSE;

  /* Determine if the mobile station is a foreign NID roamer */

  for (i=0; i < NV_MAX_HOME_SID_NID; i++)

  {
    if (sid == cdma.pair[i].sid)
    {
      if ((nid == cdma.pair[i].nid) || (cdma.pair[i].nid == CAI_HOME_NID))
      {
        for_nid_roam = FALSE;
        break;
      }
      else
      {
        for_nid_roam = TRUE;
      }
    }
  } /* end for (i=0; i < NV_MAX_SID_NID; i++) */


  if (i < NV_MAX_HOME_SID_NID)

  {
    /* -----------------------------------------------------------------
    ** Must be a home (SID,NID) pair - MOB_TERMs is set based on whether
    ** mobile is configured to received mobile terminated calls when
    ** using a home (SID,NID) pair.
    ** ----------------------------------------------------------------- */
    mccreg_vars.mob_term = cdma.mob_term_home;

    /* ------------------------------------------------------------
    ** Set REG_ENABLEDs to YES if both HOME_REGs and MOB_TERM_HOMEp
    ** are equal to '1'
    ** ------------------------------------------------------------ */
    if ((home_reg) && (cdma.mob_term_home))
    {
      mccreg_vars.reg_enabled = TRUE;
    }
  }
  else if (for_nid_roam)
  {
    /* ----------------------------------------------------------------
    ** The mobile station is a foreign NID roamer - set MOB_TERMs based
    ** on whether the mobile is configured to receive mobile-terminated
    ** calls in a foreign NID
    ** ---------------------------------------------------------------- */
    mccreg_vars.mob_term = cdma.mob_term_for_nid;

    /* ------------------------------------------------------------------
    ** Set REG_ENABLEDs to YES if both FOR_NID_REGs and MOB_TERM_FOR_NIDp
    ** are equal to '1'
    ** ------------------------------------------------------------------ */
    if ((for_nid_reg) && (cdma.mob_term_for_nid))
    {
      mccreg_vars.reg_enabled = TRUE;
    }
  }
  else
  {
    /* ----------------------------------------------------------------
    ** The mobile station is a foreign SID roamer - set MOB_TERMs based
    ** on whether the mobile is configured to receive mobile-terminated
    ** calls in a foreign SID
    ** ---------------------------------------------------------------- */
    mccreg_vars.mob_term = cdma.mob_term_for_sid;

    /* ------------------------------------------------------------------
    ** Set REG_ENABLEDs to YES if both FOR_SID_REGs and MOB_TERM_FOR_SIDp
    ** are equal to '1'
    ** ------------------------------------------------------------------ */
    if ((for_sid_reg) && (cdma.mob_term_for_sid))
    {
      mccreg_vars.reg_enabled = TRUE;
    }
  }
} /* mccreg_set_reg_enabled */

/*===========================================================================

FUNCTION MCCREG_SYSPARM

DESCRIPTION
  This function processes the registration variables of a received System
  Parameters Message as specified in IS-95A section 6.6.5.5.2.2.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Stored registration variables are updated.

===========================================================================*/

/*lint -e818*/
void mccreg_sysparm
(
  caii_sysparm_type *msg_ptr
    /* Pointer to received System Parameter message */
)
{
  /* ---------------------------------------------------------------
  ** When the mobile station processes the System Parameters Message
  ** it shall perform the following actions:
  ** --------------------------------------------------------------- */

  /* -------------------------------------------------------------
  ** 1. If REG_PRDs is equal to zero, the mobile station shall set
  **    COUNTER_ENABLEDs to NO.
  ** ------------------------------------------------------------- */
  if (msg_ptr->reg_prd == 0)
  {
    /* Deregister the timer-based registration timer */
    (void) timer_clr( &mccreg_timer_reg_timer, T_NONE );
    mccreg_vars.counter_enabled = FALSE;
  }
  else
  {
    /* -----------------------------------------------------------------
    ** 2. If REG_PRDs is not equal to zero, the mobile station shall set
    **    REG_COUNT_MAXs as specified in 6.6.5.1.3.
    ** ----------------------------------------------------------------- */
    if ((msg_ptr->reg_prd > 28) && (msg_ptr->reg_prd < 86))
    {
      mccreg_vars.reg_count_max = reg_count_table [msg_ptr->reg_prd - 29];
    }
    else
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "SPM error, invalid reg period %d", msg_ptr->reg_prd );
    }
  }

  /* -------------------------------------------------------------
  ** 3. The mobile station shall update its roaming status and set
  **    REG_ENABLEDs as specified in 6.6.5.3.
  ** ------------------------------------------------------------- */
  mccreg_set_reg_enabled( msg_ptr->sid, msg_ptr->nid, msg_ptr->home_reg,
                          msg_ptr->for_nid_reg, msg_ptr->for_sid_reg );

  /* ----------------------------------------------------------------------
  ** 4. If ZONE_LISTs contains more than TOTAL_ZONESs entries, the mobile
  **    station shall delete the excess entries according to the rules
  **    specified in 6.6.5.1.5.  (When deleting zone list entries you have
  **    to keep in mind that there may be one with a disabled timer which
  **    is not on the mccreg_zone_q.)
  ** ---------------------------------------------------------------------- */
  mccreg_check_total_zones( msg_ptr->total_zones );

  /* ------------------------------------------------------------------
  ** 5. If MULT_SIDs is equal to '0' and SID_NID_LIST contains entries
  **    with different SIDs, delete the excess entries according to the
  **    rules specified in 6.6.5.1.5.
  ** ------------------------------------------------------------------ */
  if (msg_ptr->mult_sids == 0)
  {
    mccreg_delete_mult_sids();
  }

  /* --------------------------------------------------------------------
  ** 6. If MULT_NIDs is equal to '0' and SID_NID_LIST contains more than
  **    one entry for any SID, delete the excess entries according to the
  **    rules specified in 6.6.5.1.5.
  ** -------------------------------------------------------------------- */
  if (msg_ptr->mult_nids == 0)
  {
    mccreg_delete_mult_nids();
  }

  /* ------------------------------------------------------
  ** Calculate if a distance-based registration is required
  ** ------------------------------------------------------ */
  if ((msg_ptr->reg_dist != 0) && (mccreg_vars.reg_enabled))
  {
    if (reg_dist( (long) mccreg_vars.base_lat_reg,
                  (long) msg_ptr->base_lat,
                  (long) mccreg_vars.base_long_reg,
                  (long) msg_ptr->base_long,
                  mccreg_vars.reg_dist ))
    {
      mccreg_vars.dist_reg_reqd = TRUE;
    }
  }

} /* end mccreg_sysparm() */
/*lint +e818*/

/*===========================================================================

FUNCTION MCCREG_POWERDOWN

DESCRIPTION
  This function performs the powerdown updates to the zone list, the
  sid nid list and the distance registration variables in NV.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  The NV sid nid list, zone list and distance registration variables
  are updated.

===========================================================================*/

void mccreg_powerdown( void )
{
  nv_stat_enum_type    nv_read_status; /* for status from NV read */

  /* -------------------------------------------------------------------
  ** If an entry of ZONE_LISTs does not have an active timer, copy that
  ** entry to ZONE_LISTs-p; otherwise, delete any entry in ZONE_LISTs-p.
  ** ------------------------------------------------------------------- */
  mccreg_nv_buf.item = NV_ZONE_LIST_I;
  mccreg_nv_buf.data_ptr = &mccreg_nv_data;

  /* Enter Critical Section */
  REX_ISR_LOCK(&mc_reg_crit_sect);
  
  if (mccreg_zone_ptr != NULL)
  {
    mccreg_nv_buf.cmd = NV_READ_F;
    if( (nv_read_status = mcc_nv_cmd( &mccreg_nv_buf ) ) == NV_DONE_S )
    {
      if( (mccreg_nv_data.zone_list.sid != mccreg_zone_ptr->sid) ||
          (mccreg_nv_data.zone_list.nid != mccreg_zone_ptr->nid) ||
          (mccreg_nv_data.zone_list.reg_zone != mccreg_zone_ptr->reg_zone) ||
          (mccreg_nv_data.zone_list.band_class != mccreg_zone_ptr->band_class) ||
          (mccreg_nv_data.zone_list.block_or_sys != mccreg_zone_ptr->block_or_sys)
        )
      {
        /* There is change in NV_ZONE_LIST_I value so write this NV */
        mccreg_nv_data.zone_list.sid = mccreg_zone_ptr->sid;
        mccreg_nv_data.zone_list.nid = mccreg_zone_ptr->nid;
        mccreg_nv_data.zone_list.reg_zone = mccreg_zone_ptr->reg_zone;
        mccreg_nv_data.zone_list.band_class = mccreg_zone_ptr->band_class;
        mccreg_nv_data.zone_list.block_or_sys = mccreg_zone_ptr->block_or_sys;

        mccreg_nv_buf.cmd = NV_WRITE_F;
        (void) mcc_nv_cmd( &mccreg_nv_buf );
      }
    }
    else
    {
      /* NV_ZONE_LIST_I read failed may be because of NV_NOTACTIVE_S
       * so write NV */
      mccreg_nv_data.zone_list.sid = mccreg_zone_ptr->sid;
      mccreg_nv_data.zone_list.nid = mccreg_zone_ptr->nid;
      mccreg_nv_data.zone_list.reg_zone = mccreg_zone_ptr->reg_zone;
      mccreg_nv_data.zone_list.band_class = mccreg_zone_ptr->band_class;
      mccreg_nv_data.zone_list.block_or_sys = mccreg_zone_ptr->block_or_sys;

      M1X_MSG( DCP, LEGACY_ERROR,
        "Write NV_ZONE_LIST_I, as read failed with nv_read_status:%d",
        nv_read_status);
      mccreg_nv_buf.cmd = NV_WRITE_F;
      (void) mcc_nv_cmd( &mccreg_nv_buf );
    }
  } /* End if (mccreg_zone_ptr != NULL) */
  else
  {
    mccreg_nv_buf.cmd = NV_WRITE_F;
    /* Clear out old values */
    mccreg_nv_data.zone_list.sid = 0;
    mccreg_nv_data.zone_list.nid = 0;
    mccreg_nv_data.zone_list.reg_zone = 0;
    mccreg_nv_data.zone_list.band_class = 0;
    mccreg_nv_data.zone_list.block_or_sys = NV_PCS_BLOCK_A;

    (void) mcc_nv_cmd( &mccreg_nv_buf );
  }


  /* ----------------------------------------------------------------
  ** If an entry of SID_NID_LISTs does not have an active timer, copy
  ** that entry to SID_NID_LISTs-p; otherwise, delete any entry in
  ** SID_NID_LISTs-p.
  ** ---------------------------------------------------------------- */
  mccreg_nv_buf.item = NV_SID_NID_LIST_I;

  if (mccreg_sid_nid_ptr != NULL)
  {
    mccreg_nv_buf.cmd = NV_READ_F;
    if( (nv_read_status = mcc_nv_cmd( &mccreg_nv_buf ) ) == NV_DONE_S )
    {
      if( (mccreg_nv_data.sid_nid_list.sid != mccreg_sid_nid_ptr->sid) ||
          (mccreg_nv_data.sid_nid_list.nid != mccreg_sid_nid_ptr->nid) ||
          (mccreg_nv_data.sid_nid_list.band_class != mccreg_sid_nid_ptr->band_class) ||
          (mccreg_nv_data.sid_nid_list.block_or_sys != mccreg_sid_nid_ptr->block_or_sys)
        )
      {
        /* There is change in NV_SID_NID_LIST_I value so write this NV */
        mccreg_nv_data.sid_nid_list.sid = mccreg_sid_nid_ptr->sid;
        mccreg_nv_data.sid_nid_list.nid = mccreg_sid_nid_ptr->nid;
        mccreg_nv_data.sid_nid_list.band_class = mccreg_sid_nid_ptr->band_class;
        mccreg_nv_data.sid_nid_list.block_or_sys = mccreg_sid_nid_ptr->block_or_sys;

        mccreg_nv_buf.cmd = NV_WRITE_F;
        (void) mcc_nv_cmd( &mccreg_nv_buf );
      }
    }
    else
    {
      /* NV_SID_NID_LIST_I read failed may be because of NV_NOTACTIVE_S
       * so write NV */
      mccreg_nv_data.sid_nid_list.sid = mccreg_sid_nid_ptr->sid;
      mccreg_nv_data.sid_nid_list.nid = mccreg_sid_nid_ptr->nid;
      mccreg_nv_data.sid_nid_list.band_class = mccreg_sid_nid_ptr->band_class;
      mccreg_nv_data.sid_nid_list.block_or_sys =
      mccreg_sid_nid_ptr->block_or_sys;

      M1X_MSG( DCP, LEGACY_ERROR,
        "Write NV_SID_NID_LIST_I, as read failed with nv_read_status:%d",
        nv_read_status);
      mccreg_nv_buf.cmd = NV_WRITE_F;
      (void) mcc_nv_cmd( &mccreg_nv_buf );
    }
  } /* End if (mccreg_sid_nid_ptr != NULL) */
  else
  {
    mccreg_nv_buf.cmd = NV_WRITE_F;
    /* Clear out old values */
    mccreg_nv_data.sid_nid_list.sid = 0;
    mccreg_nv_data.sid_nid_list.nid = 0;
    mccreg_nv_data.sid_nid_list.band_class = 0;
    mccreg_nv_data.sid_nid_list.block_or_sys = NV_PCS_BLOCK_A;

    (void) mcc_nv_cmd( &mccreg_nv_buf );
  }

  /* ----------------------------------------------------------------
  ** If an entry of SID_NID_LISTs does not have an active timer, copy
  ** the last registered SCI on that system; otherwise, initialize
  ** last registered SCI to default sci.
  ** ---------------------------------------------------------------- */

  mccreg_nv_buf.item = NV_LAST_REGISTERED_SCI_I;
  if (mccreg_sid_nid_ptr != NULL)
  {
    mccreg_nv_buf.cmd = NV_READ_F;
    if( (nv_read_status = mcc_nv_cmd( &mccreg_nv_buf ) ) == NV_DONE_S )
    {
      if( mccreg_nv_data.last_registered_sci != mcc_get_last_sci_reg() )
      {
        /* There is change in NV_LAST_REGISTERED_SCI_I value so write this NV */
        mccreg_nv_data.last_registered_sci = mcc_get_last_sci_reg();

        mccreg_nv_buf.cmd = NV_WRITE_F;
        (void) mcc_nv_cmd( &mccreg_nv_buf );
      }
    }
    else
    {
      /* NV_LAST_REGISTERED_SCI_I read failed may be because of NV_NOTACTIVE_S
       * so write NV */
      mccreg_nv_data.last_registered_sci = mcc_get_last_sci_reg();

      M1X_MSG( DCP, LEGACY_ERROR,
        "Write NV_LAST_REGISTERED_SCI_I, as read failed with nv_read_status:%d",
        nv_read_status);
      mccreg_nv_buf.cmd = NV_WRITE_F;
      (void) mcc_nv_cmd( &mccreg_nv_buf );
    }
  } /* End if (mccreg_sid_nid_ptr != NULL) */
  else
  {
    mccreg_nv_buf.cmd = NV_WRITE_F;
    mccreg_nv_data.last_registered_sci = MCC_DEFAULT_SCI;

    (void) mcc_nv_cmd( &mccreg_nv_buf );
  }

  /* Leave Critical Section */
  REX_ISR_UNLOCK(&mc_reg_crit_sect);

  /* Store out base_lat, base_long and reg_dist now */
  mccreg_nv_buf.item = NV_DIST_REG_I;
  mccreg_nv_buf.cmd = NV_READ_F;
  if( (nv_read_status = mcc_nv_cmd( &mccreg_nv_buf ) ) == NV_DONE_S )
  {
    if( (mccreg_nv_data.dist_reg.base_lat_reg != mccreg_vars.base_lat_reg) ||
        (mccreg_nv_data.dist_reg.base_long_reg != mccreg_vars.base_long_reg) ||
        (mccreg_nv_data.dist_reg.reg_dist_reg != mccreg_vars.reg_dist)
      )
      {
        /* There is change in NV_DIST_REG_I value so write this NV */
        mccreg_nv_data.dist_reg.base_lat_reg = mccreg_vars.base_lat_reg;
        mccreg_nv_data.dist_reg.base_long_reg = mccreg_vars.base_long_reg;
        mccreg_nv_data.dist_reg.reg_dist_reg = mccreg_vars.reg_dist;

        mccreg_nv_buf.cmd = NV_WRITE_F;
        (void) mcc_nv_cmd( &mccreg_nv_buf );
      }
  }
  else
  {
    /* NV_DIST_REG_I read failed may be because of NV_NOTACTIVE_S
     * so write NV */
    mccreg_nv_data.dist_reg.base_lat_reg = mccreg_vars.base_lat_reg;
    mccreg_nv_data.dist_reg.base_long_reg = mccreg_vars.base_long_reg;
    mccreg_nv_data.dist_reg.reg_dist_reg = mccreg_vars.reg_dist;

    M1X_MSG( DCP, LEGACY_ERROR,
      "Write NV_DIST_REG_I, as read failed with nv_read_status:%d",
      nv_read_status);
    mccreg_nv_buf.cmd = NV_WRITE_F;
    (void) mcc_nv_cmd( &mccreg_nv_buf );
  }

  /* Store the number of CDMA powerup registrations performed in NV */
  mccreg_nv_buf.item = NV_CDMA_POWERUP_REG_PERFORMED_I;
  mccreg_nv_buf.cmd = NV_READ_F;
  if( (nv_read_status = mcc_nv_cmd( &mccreg_nv_buf ) ) == NV_DONE_S )
  {
    if( qw_cmp(mccreg_nv_data.cdma_powerup_reg_performed, mccreg_powerup_reg_count) != 0 )
    {
      /* There is change in NV_CDMA_POWERUP_REG_PERFORMED_I value so write this NV */
      qw_equ(mccreg_nv_data.cdma_powerup_reg_performed, mccreg_powerup_reg_count);

      mccreg_nv_buf.cmd = NV_WRITE_F;
      (void) mcc_nv_cmd( &mccreg_nv_buf );
    }
  }
  else
  {
    /* NV_CDMA_POWERUP_REG_PERFORMED_I read failed may be because of NV_NOTACTIVE_S
     * so write NV */
    qw_equ(mccreg_nv_data.cdma_powerup_reg_performed, mccreg_powerup_reg_count);

    M1X_MSG( DCP, LEGACY_ERROR,
      "Write NV_CDMA_POWERUP_REG_PERFORMED_I, as read failed with nv_read_status:%d",
      nv_read_status);
    mccreg_nv_buf.cmd = NV_WRITE_F;
    (void) mcc_nv_cmd( &mccreg_nv_buf );
  }

} /* mccreg_powerdown */

/*===========================================================================

FUNCTION MCCREG_IDLE_POWERDOWN

DESCRIPTION
  This function performs Idle Channel powerdown registration procedures
  as specified in IS-95A section 6.6.5.5.2.4.

DEPENDENCIES
  None.

RETURN VALUE
  CAI_POWERDOWN_REG if the mobile is required to register.
  MCCREG_NO_REG_REQUIRED if the mobile is not required to register.

SIDE EFFECTS
  Stored registration variables are updated.

===========================================================================*/

byte mccreg_idle_powerdown( void )
{
  byte reg_type = MCCREG_NO_REG_REQUIRED;
    /* Assume powerdown registration is not required */
  caii_sysparm_type *sys_ptr;
    /* Pointer to most recent System Parameter Message */
  boolean pwr_dwn_reg = FALSE;
    /* Power-down registration indicator */
  word sid = 0;
    /* System identification */
  word nid = 0;
    /* Network identification */


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(mcc_is_jcdma_supported() &&
     mcc_msg_is_blocked(CDMA_MSG_TYPE_REG))
  {
    return MCCREG_NO_REG_REQUIRED;
  }

#if (defined(FEATURE_DDTM_CNTL) && defined(FEATURE_HDR))
  /* If ddtm registration suppression is on, the mobile should not send any
     registration messages.  If L2 ack suppression is on, the mobile will
     not recognize L2 acks, and it will get stuck in System Access; so the
     mobile should not initiate registration in that case either. */
  if (cdma.ddtm.status &&
      ((cdma.ddtm.ddtm_act_mask & SYS_DDTM_ACT_MASK_SUPPRESS_L2ACK)
       || (cdma.ddtm.ddtm_act_mask & SYS_DDTM_ACT_MASK_SUPPRESS_REG))
     )
  {
    return(MCCREG_NO_REG_REQUIRED);
  }
#endif

  /* ---------------------------------------------------------------------
  ** The mobile sttaion shall perform powerdown registration, as specified
  ** in 6.6.5.1.2, if all the following conditions are met:
  **   REG_ENABLEDs equals YES; and
  **   POWER_DOWN_REGs equals '1'; and
  **   There is an entry of SID_NID_LISTs for which the SID and NID
  **   fields are equal to SIDs and NIDs; and
  **   The powerup/initialization timer is disabled or has expired.
  ** --------------------------------------------------------------------- */
  #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
  if(cdma.entry_ptr->entry == MCC_CSFB)
  {
    pwr_dwn_reg = mcc_csfb_sib8_msg.reg_params.powerDownReg;
    sid = mcc_csfb_sib8_msg.reg_params.sid;
    nid = mcc_csfb_sib8_msg.reg_params.nid;
  }
  else

  #endif
  {
    if ((sys_ptr = (caii_sysparm_type*) mcc_get_msg( CAI_SYS_PARM_MSG )) != NULL)
    {
      pwr_dwn_reg = sys_ptr->power_down_reg;
      sid = sys_ptr->sid;
      nid = sys_ptr->nid;
    }
  }

  if ((mccreg_vars.reg_enabled)
      && ((pwr_dwn_reg)
#if ( defined(FEATURE_GOBI) && defined(FEATURE_GOBI_VZW))
         || otasp_is_successful_commit()
    /* This was VZW requirement to perform power down after successful data
    ** commit. And also, MS should send the power down reg regardless of
    ** power_down_reg field in SPM  */
#endif
         )
        && (!mccreg_vars.powerup_timer_on))
  {
    if (mccreg_find_sid_nid(sid,nid) != NULL)
    {
      reg_type = CAI_POWERDOWN_REG;
    }
  }
  return (reg_type);

} /* mccreg_idle_powerdown */

/*===========================================================================

FUNCTION MCCREG_UPDATE

DESCRIPTION
  This function performs the SID_NID_LIST and ZONE_LIST updates described
  in IS-95A section 6.6.4.4.3.1 (after the mobile station receives an
  acknowledgement for a Registration Message, Origination Message, or
  Page Response Message sent on the Access Channel) and section 6.6.5.5.4.3.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  SID_NID_LISTs and ZONE_LISTs will change.

===========================================================================*/

void mccreg_update
(
  word sid,        /* SID for new entry */
  word nid,        /* NID for new entry */
  word reg_zone,   /* REG_ZONEs for new entry */
  word zone_timer, /* Zone timer for new entry */
  word total_zones /* New number of total zones */
)
{
  mccreg_zone_type *zone_ptr;
    /* Pointer to zone from zone list */
  mccreg_sid_nid_type *sid_ptr;
    /* Pointer to (SID,NID) pair from SID_NID_LISTs */
  boolean found;
    /* Indicator of whether (SID,NID) pair or registration zone is found
       on appropriate list */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* ----------------------------------------------------------------------
  ** Add REG_ZONEs, SIDs and NIDs to ZONE_LISTs if not already in the list.
  **
  ** Disable the zone list entry timer for the entry of ZONE_LISTs
  ** containing REG_ZONEs, SIDs and NIDs.  For any other entry of
  ** ZONE_LISTs whose entry timer is not active, enable the entry timer
  ** with the duration specified by ZONE_TIMERs.
  ** ---------------------------------------------------------------------- */

  /* Search ZONE_LISTs for the current zone */
  found = FALSE;

  /* Enter Critical Section */
  REX_ISR_LOCK(&mc_reg_crit_sect);
  
  if (mccreg_zone_ptr != NULL)
  {
    if ((mccreg_zone_ptr->sid == sid) &&
        ((mccreg_zone_ptr->nid == nid) ||
         (mccreg_zone_ptr->nid == CAI_HOME_NID)) &&
        (mccreg_zone_ptr->reg_zone == reg_zone))
    {
      /* Zone is already on ZONE_LISTs with a disabled timer */
      found = TRUE;
    }
  }

  if (!found)
  {
    /* Search for current zone in list of zones with active timers */
    zone_ptr = (mccreg_zone_type*) q_check( &mccreg_zone_q );

    while ((!found) && (zone_ptr != NULL))
    {
      if ((zone_ptr->sid == sid) &&
          ((zone_ptr->nid == nid) ||
           (zone_ptr->nid == CAI_HOME_NID)) &&
          (zone_ptr->reg_zone == reg_zone))
      {
        /* Current zone found in zone list */
        found = TRUE;

        /* Pull this item off mccreg_zone_q because we are disabling
           its timer */

        #ifdef FEATURE_Q_NO_SELF_QPTR
        q_delete(&mccreg_zone_q, &zone_ptr->link );
        #else
        q_delete( &zone_ptr->link );
        #endif /* FEATURE_Q_NO_SELF_QPTR */

        /* If there is an entry with a disabled timer, enable its timer
           (put it on mccreg_zone_q) */
        if (mccreg_zone_ptr != NULL)
        {
          /* Enable counter in msec units */
          mccreg_zone_ptr->cnt =
            timer_tab[zone_timer] + mccreg_vars.timer_cnt;

          mccreg_add_zone( mccreg_zone_ptr );
        }

        /* Disable timer of entry with previously enabled timer */
        mccreg_zone_ptr = zone_ptr;
      }
      else
      {
        zone_ptr = (mccreg_zone_type*) q_next( &mccreg_zone_q,
                                               &zone_ptr->link );
      }
    } /* end while ((!found) && (zone_ptr != NULL)) */

    if (!found)
    {
      /* Current zone is not on ZONE_LISTs  */

      /* If there is an entry with a disabled timer, enable its timer
         (put it on mccreg_zone_q) */
      if (mccreg_zone_ptr != NULL)
      {
        /* Enable counter in msec units */
        mccreg_zone_ptr->cnt =
          timer_tab[zone_timer] + mccreg_vars.timer_cnt;

        mccreg_add_zone( mccreg_zone_ptr );
      }

      /* Add new zone list item as entry with disabled timer */
      if ((mccreg_zone_ptr =
           (mccreg_zone_type*) q_get( &mccreg_zone_free_q )) == NULL)
      {
        /* Pull the oldest item off the zone list among those zone
           list entries having active entry timers */
        if ((mccreg_zone_ptr =
             (mccreg_zone_type*) q_get( &mccreg_zone_q )) == NULL)
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "mccreg_zone_q error" );
        }
      }

      if (mccreg_zone_ptr != NULL)
      {
        mccreg_zone_ptr->sid = sid;
        mccreg_zone_ptr->nid = nid;
        mccreg_zone_ptr->reg_zone = reg_zone;
        mccreg_zone_ptr->block_or_sys = cdma.block_or_sys;
        mccreg_zone_ptr->band_class = cdma.band_class;
      }
    }
  }
  /* Leave Critical Section */
  REX_ISR_UNLOCK(&mc_reg_crit_sect);

  /* ---------------------------------------------------------------------
  ** If ZONE_LISTs contains more entries than TOTAL_ZONESs entries, delete
  ** the excess entries according to the rules specified in 6.6.5.1.
  ** Critical Section prot added inside "API - mccreg_check_total_zones()"
  ** --------------------------------------------------------------------- */
  mccreg_check_total_zones( total_zones );

  /* ---------------------------------------------------------------------
  ** Add SIDs and NIDs to SID_NID_LISTs if not already in the list.
  **
  ** Disable the SID/NID list entry timer for the entry of SID_NID_LISTs
  ** containing SIDs and NIDs.  For any other entry of SID_NID_LISTs whose
  ** entry timer is not active, enable the entry timer with the duration
  ** specified in 6.6.5.1.5.
  ** --------------------------------------------------------------------- */

  sid_ptr = mccreg_find_sid_nid( sid, nid );
  
  /* Enter Critical Section */
  REX_ISR_LOCK(&mc_reg_crit_sect);
  
  /* Search SID_NID_LISTs for the current (SID,NID) pair */
  if (sid_ptr != NULL)
  {
    if (sid_ptr != mccreg_sid_nid_ptr)
    {
      /* (SID,NID) is on the list but does not have a disabled timer */

      #ifdef FEATURE_Q_NO_SELF_QPTR
      q_delete(&mccreg_sid_nid_q, &sid_ptr->link );
      #else
      q_delete( &sid_ptr->link );
      #endif /* FEATURE_Q_NO_SELF_QPTR */

      /* If there is an entry with a disabled timer, enable its timer
         (put it on mccreg_sid_nid_q) */
      if (mccreg_sid_nid_ptr != NULL)
      {
        mccreg_sid_nid_ptr->cnt =
          timer_tab[zone_timer] + mccreg_vars.timer_cnt;

        mccreg_add_sid_nid( mccreg_sid_nid_ptr );
      }

      /* Disable timer for current (SID,NID) pair */
      mccreg_sid_nid_ptr = sid_ptr;
    } /* if (sid_ptr != mccreg_sid_nid_ptr) */
  }
  else
  {
    /* Item is not on SID_NID_LISTs  */
    if (mccreg_sid_nid_ptr != NULL)
    {
      /* Enable timer for item with previously disabled timer */
      mccreg_sid_nid_ptr->cnt =
        timer_tab [zone_timer] + mccreg_vars.timer_cnt;

      mccreg_add_sid_nid( mccreg_sid_nid_ptr );
    }

    if ((mccreg_sid_nid_ptr =
         (mccreg_sid_nid_type*) q_get( &mccreg_sid_nid_free_q )) == NULL)
    {
      /* -----------------------------------------------------------------
      ** Pull the oldest item off SID_NID_LISTs among those entries having
      ** active SID/NID list entry timers
      ** ----------------------------------------------------------------- */
      if ((mccreg_sid_nid_ptr =
           (mccreg_sid_nid_type*) q_get( &mccreg_sid_nid_q )) == NULL)
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "mccreg_sid_nid_q error" );
      }
    }

    if (mccreg_sid_nid_ptr != NULL)
    {
      mccreg_sid_nid_ptr->sid = sid;
      mccreg_sid_nid_ptr->nid = nid;
      mccreg_sid_nid_ptr->block_or_sys = cdma.block_or_sys;
      mccreg_sid_nid_ptr->band_class   = cdma.band_class;
    }
  }
  
  /* Leave Critical Section */
  REX_ISR_UNLOCK(&mc_reg_crit_sect);
  
} /* end mccreg_update() */

/*===========================================================================

FUNCTION MCCREG_SET_REG_SCI

DESCRIPTION
  This sets the reg_sci in mccreg_vars structure.
  It is set during explicit and implicit registration.

  This set value is later used to appropriately update the operational SCI when
  L2 ack for the registration is received.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mccreg_set_reg_sci
(
  int8 sci
)
{
  mccreg_vars.reg_sci = sci;

} /* mccreg_set_reg_sci */

/*===========================================================================

FUNCTION MCCREG_SUCCESS

DESCRIPTION
  This function performs registration actions required after the mobile
  station receives an acknowledgement for a Registration Message,
  Origination Message, or Page Response Message sent on the Access
  Channel as described in IS-95A section 6.6.5.5.3.1.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Stored registration variables are updated.

===========================================================================*/

void mccreg_success( void )
{
  caii_sysparm_type *sys_ptr;
    /* Pointer to most recently received System Parameter message */
  mccreg_zone_type *zone_ptr, *temp_zone_ptr;
    /* Pointers to zone from zone list */
  mccreg_sid_nid_type *sid_ptr, *temp_sid_ptr;
    /* Pointers to (SID,NID) pair from SID/NID list */
  uint8 total_zones;
  boolean mult_sids,mult_nids;
    /* Total number of zones/sid/nid as indicated in overheads */
  int8 new_sci_s = 0;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* ----------------------------------------
  ** Disable the powerup/initialization timer
  ** ---------------------------------------- */
  if (mccreg_vars.powerup_timer_on)
  {
    (void) timer_clr( &mccreg_powerup_timer, T_NONE );
    mccreg_vars.powerup_timer_on = FALSE;
    mccreg_vars.powering_up = FALSE;
  }

#ifdef FEATURE_QCHAT
  if(qchatupk_data_q1x_enabled())
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "QChat_1X: Checking 1X CB for any queued SCI.");
    qchatupk_sci_1x_cb();
  }
#endif /* FEATURE_QCHAT */
  /* ----------------------
  ** Set REG_COUNTs to zero
  ** ---------------------- */
  mccreg_vars.reg_count_ms = 0;
  M1X_MSG( DCP, LEGACY_MED,
    "REG_COUNTs is set to 0");

  /* -----------------------------------------------------
  ** Indicate that the mobile has successfully registered.
  ** ----------------------------------------------------- */
  mccreg_vars.reg_status = MCCREG_REG_COMPLETED;

#ifdef FEATURE_PPP_CLEANUP_NON_SV_MODE
  /* ----------PPP_CLEANUP_NON_SV_MODE-----------------------------
   ** Check if this is successful deregister powerdown registration
   ** If yes, then send the deregister cmd status rpt to CM
   **------------------------------------------------------------ */
  if(cdma.deregister_state == MCC_DEREGISTER_RSP_PENDING)
  {
    /* Send the deregister cmd status to CM */
    mcc_send_deregister_status(CM_DEREG_SUCCESS);
  } /* if(cdma.deregister_state == MCC_DEREGISTER_RSP_PENDING) */
#endif /* FEATURE_PPP_CLEANUP_NON_SV_MODE */

  sys_ptr = (caii_sysparm_type*) mcc_get_msg(CAI_SYS_PARM_MSG);

  #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
  if(cdma.entry_ptr->entry != MCC_CSFB)
  #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
  {
    /* Get a pointer to the current System Parameter Message */
    if( sys_ptr == NULL)
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "System Parameter msg error" );
      return;
    }
  } /*  if(cdma.entry_ptr->entry != MCC_CSFB) */

  /* Enter Critical Section */
  REX_ISR_LOCK(&mc_reg_crit_sect);
  if (mccreg_zone_ptr != NULL)
  {
    /* -----------------------------------------------------------------
    ** IS95A: Delete from ZONE_LISTs all entries that have a different
    ** serving system than SERVSYSs.
    ** J-STD-008: Delete all entries from ZONE_LISTs belonging to a
    ** different PCS frequency block than the frequency block associated
    ** with SIDs.  Delete all entries from ZONE_LISTs belonging to a
    ** different band class than the band class associated with SIDs.
    ** ----------------------------------------------------------------- */

    if ( ( ( (mccreg_zone_ptr->block_or_sys != cdma.block_or_sys)
             #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
             /* This check will prevent deleting of entry from queue having a
              * wild-class blk_or_sys. Thus we will retain context from CSFB
              * mode w.r.t.blk_or_sys. Note that in LTE mode we will not consider
              * blk_or_sys altogether for cleaning lists.
              */
             && ( (cdma.block_or_sys != MCC_BLK_SYS_WILDCARD) &&
                  (mccreg_zone_ptr->block_or_sys != MCC_BLK_SYS_WILDCARD)
                )
             #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
           )
           ||
           ( (mccreg_zone_ptr->band_class != cdma.band_class)
             #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
             /* This check will prevent deleting of entry from queue having a
              * wild-class band_class. Thus we will retain context from CSFB
              * mode w.r.t.wild card band class.
              */
             && ( (mccreg_zone_ptr->band_class != MCC_BAND_CLS_WILDCARD) &&
                  (cdma.band_class != MCC_BAND_CLS_WILDCARD)
                )
             #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
           )
         )
       )
    {
      q_put( &mccreg_zone_free_q, &mccreg_zone_ptr->link );
      mccreg_zone_ptr = NULL;
    }
  }

  /* -----------------------------------------------------------------
  ** Delete all ZONE_LISTs entries with active timers which are from a
  ** different serving system, frequency block or band class than SIDs
  ** ------------------------------------------------------------------ */
  zone_ptr = (mccreg_zone_type*) q_check( &mccreg_zone_q );

  while (zone_ptr != NULL)
  {
    if ( ( ( (zone_ptr->block_or_sys != cdma.block_or_sys)
             #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
             /* This check will prevent deleting of entry from queue having a
              * wild-class blk_or_sys. Thus we will retain context from CSFB
              * mode w.r.t.blk_or_sys. Note that in LTE mode we will not consider
              * blk_or_sys altogether for cleaning lists.
              */
             && ( (cdma.block_or_sys != MCC_BLK_SYS_WILDCARD) &&
                  (zone_ptr->block_or_sys != MCC_BLK_SYS_WILDCARD)
                )
             #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
           )
           ||
           ( (zone_ptr->band_class != cdma.band_class)
             #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
             /* This check will prevent deleting of entry from queue having a
              * wild-class blk_or_sys. Thus we will retain context from CSFB
              * mode w.r.t.blk_or_sys. Note that in LTE mode we will not consider
              * blk_or_sys altogether for cleaning lists.
              */
             && ( (cdma.band_class != MCC_BAND_CLS_WILDCARD) &&
                  (zone_ptr->band_class != MCC_BAND_CLS_WILDCARD)
                )
             #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
           )
         )
       )
    {
      /* Zone is from a different serving system - delete it */
      temp_zone_ptr = zone_ptr;
      zone_ptr = (mccreg_zone_type*) q_next( &mccreg_zone_q, &zone_ptr->link );

      #ifdef FEATURE_Q_NO_SELF_QPTR
      q_delete( &mccreg_zone_q, &temp_zone_ptr->link );
      #else
      q_delete( &temp_zone_ptr->link );
      #endif /* FEATURE_Q_NO_SELF_QPTR */

      q_put( &mccreg_zone_free_q, &temp_zone_ptr->link );
    }
    else
    {
      zone_ptr = (mccreg_zone_type*) q_next( &mccreg_zone_q,
                                             &zone_ptr->link );
    }
  }

  /* ------------------------------------------------------------------
  ** IS95A: Delete from SID_NID_LISTs all entries that have a different
  ** serving system than SERVSYSs.
  ** J-STD-008: Delete all entries from SID_NID_LISTs belonging to a
  ** different PCS frequency block than the frequency block associated
  ** with SIDs.  Delete all entries from SID_NID_LISTs belonging to a
  ** different band class than the band class associated with SIDs.
  ** ------------------------------------------------------------------ */
  if (mccreg_sid_nid_ptr != NULL)
  {
    if ( ( ( (mccreg_sid_nid_ptr->block_or_sys != cdma.block_or_sys)
             #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
             /* This check will prevent deleting of entry from queue having a
              * wild-class blk_or_sys. Thus we will retain context from CSFB
              * mode w.r.t.blk_or_sys. Note that in LTE mode we will not consider
              * blk_or_sys altogether for cleaning lists.
              */
             && ( (cdma.block_or_sys != MCC_BLK_SYS_WILDCARD) &&
                  (mccreg_sid_nid_ptr->block_or_sys != MCC_BLK_SYS_WILDCARD)
                )
             #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
           )
           ||
           ( (mccreg_sid_nid_ptr->band_class != cdma.band_class)
             #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
             /* This check will prevent deleting of entry from queue having a
              * wild-class blk_or_sys. Thus we will retain context from CSFB
              * mode w.r.t.blk_or_sys. Note that in LTE mode we will not consider
              * blk_or_sys altogether for cleaning lists.
              */
             && ( (cdma.band_class != MCC_BAND_CLS_WILDCARD) &&
                  (mccreg_sid_nid_ptr->band_class != MCC_BAND_CLS_WILDCARD)
                )
             #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
           )
         )
        )
    {
      q_put( &mccreg_sid_nid_free_q, &mccreg_sid_nid_ptr->link );
      mccreg_sid_nid_ptr = NULL;
    }
  }

  /* ------------------------------------------------------------------
  ** Delete all SID_NID_LISTs entries with active timers which are from
  ** a different serving system, freq block or band class than SIDs
  ** ------------------------------------------------------------------ */
  sid_ptr = (mccreg_sid_nid_type*) q_check( &mccreg_sid_nid_q );

  while (sid_ptr != NULL)
  {
    if ( ( ( (sid_ptr->block_or_sys != cdma.block_or_sys)
             #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
             /* This check will prevent deleting of entry from queue having a
              * wild-class blk_or_sys. Thus we will retain context from CSFB
              * mode w.r.t.blk_or_sys. Note that in LTE mode we will not consider
              * blk_or_sys altogether for cleaning lists.
              */
             && ( (cdma.block_or_sys != MCC_BLK_SYS_WILDCARD) &&
                  (sid_ptr->block_or_sys != MCC_BLK_SYS_WILDCARD)
                )
             #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
           )
           ||
           ( (sid_ptr->band_class != cdma.band_class)
             #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
             /* This check will prevent deleting of entry from queue having a
              * wild-class blk_or_sys. Thus we will retain context from CSFB
              * mode w.r.t.blk_or_sys. Note that in LTE mode we will not consider
              * blk_or_sys altogether for cleaning lists.
              */
             && ( (cdma.band_class != MCC_BAND_CLS_WILDCARD) &&
                  (sid_ptr->band_class != MCC_BAND_CLS_WILDCARD)
                )
             #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
           )
         )
        )
    {
      /* SID is from different serving system - delete it */
      temp_sid_ptr = sid_ptr;
      sid_ptr = (mccreg_sid_nid_type*) q_next( &mccreg_sid_nid_q, &sid_ptr->link );

      #ifdef FEATURE_Q_NO_SELF_QPTR
      q_delete( &mccreg_sid_nid_q, &temp_sid_ptr->link );
      #else
      q_delete( &temp_sid_ptr->link );
      #endif /* FEATURE_Q_NO_SELF_QPTR */

      q_put( &mccreg_sid_nid_free_q, &temp_sid_ptr->link );
    }
    else
    {
      sid_ptr = (mccreg_sid_nid_type*) q_next( &mccreg_sid_nid_q,
                                               &sid_ptr->link );
    }
  }
  /* Leave Critical Section */
  REX_ISR_UNLOCK(&mc_reg_crit_sect);
  
  #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
  if(cdma.entry_ptr->entry == MCC_CSFB)
  {
    total_zones =  mcc_csfb_sib8_msg.reg_params.totalZone;
    mult_sids  = mcc_csfb_sib8_msg.reg_params.multipleSID;
    mult_nids = mcc_csfb_sib8_msg.reg_params.multipleNID;
  }
  else
  #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
  {
    total_zones = sys_ptr->total_zones;
    mult_sids  = sys_ptr->mult_sids;
    mult_nids = sys_ptr->mult_nids;
  }

  /* -----------------------------------------------------------------
  ** Update ZONE_LISTs and SID_NID_LISTs with REG_ZONEs, SIDs and NIDs
  ** ----------------------------------------------------------------- */
  mccreg_update(mccreg_vars.idle_reg_info.sid, mccreg_vars.idle_reg_info.nid,
                mccreg_vars.idle_reg_info.reg_zone,
                mccreg_vars.idle_reg_info.zone_timer,
                total_zones);

  /* ---------------------------------------------------------------
  ** If MULT_SIDs is equal to '0' and SID_NID_LIST contains entries
  ** with different SIDs, delete the excess entries according to the
  ** rules specified in 6.6.1.5.
  ** --------------------------------------------------------------- */
  if (mult_sids == 0)
  {
    mccreg_delete_mult_sids();
  }

  /* -----------------------------------------------------------------
  ** If MULT_NIDs is equal to '0' and SID_NID_LIST contains more than
  ** one entry for any SID, delete the excess entries according to the
  ** rules specified in 6.6.5.1.5.
  ** ----------------------------------------------------------------- */
  if (mult_nids == 0)
  {
    mccreg_delete_mult_nids();
  }

  /* -----------------------------------------------------------------
  ** Set the stored location of last registration (BASE_LAT_REGs-p
  ** and BASE_LONG_REGs-p) to the current base station's location
  ** (BASE_LATs and BASE_LONGs).  Set the stored registration distance
  ** (REG_DIST_REGs-p) to the current base station's registration
  ** distance (REG_DISTs).
  ** ----------------------------------------------------------------- */
  #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
  /* We can choose to ignore updating these values in CSFB mode
  ** since these are used only for Dist Reg and we dont do Dist Reg in
  ** CSFB mode.
  */
  if(cdma.entry_ptr->entry != MCC_CSFB)
  #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
  {
  mccreg_vars.base_lat_reg = sys_ptr->base_lat;
  mccreg_vars.base_long_reg = sys_ptr->base_long;
  mccreg_vars.reg_dist = sys_ptr->reg_dist;
  mccreg_vars.dist_reg_reqd = FALSE;
  } /* if(cdma.entry_ptr->entry != MCC_CSFB) */

  /* ----------------------------------------------------------------
  ** Set the stored band class and block-or-sys of last registration.
  ** ---------------------------------------------------------------- */
  mccreg_vars.band_class = cdma.band_class;
  #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
  csfb_cdma.registered_band_class = mccreg_vars.band_class;

  /* We can choose to ignore updating blk_sys in CSFB mode
  ** since these are used only for power-up reg, and in CSFB mode, we
  ** only do it based on bad class change. Also we dont have a cdma channel
  ** in CSFB mode.
  */
  if(cdma.entry_ptr->entry == MCC_CSFB)
  {
    mccreg_vars.block_or_sys = (uint16)MCC_BLK_SYS_WILDCARD;
  }
  else
  #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
  {
    mccreg_vars.block_or_sys = (uint16)
        sd_map_chan_to_blksys(SD_MODE_CDMA,
                              (sys_band_class_e_type) cdma.band_class,
                              cdma.cdmach);
    // func is returing enum (32 bit) and assigning to 16 bit field,
    // but values are low enough to fit without any problem.
  } /* if(cdma.entry_ptr->entry != MCC_CSFB) */

  #ifdef FEATURE_IS2000_REL_C
  /* Reset this variable so that we do not register again */
  mccreg_vars.register_in_idle = FALSE;
  #endif /* FEATURE_IS2000_REL_C */

  if ( ( mccreg_vars.reg_sci != MCC_NULL_SCI ) &&
       ( mcc_get_last_sci_reg() != mccreg_vars.reg_sci)
     )
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "Reg success: last reg sci %d, new reg sci %d",
      mcc_get_last_sci_reg(),
      mccreg_vars.reg_sci);

    /* Update the last registered sci */
    mcc_set_last_sci_reg ( mccreg_vars.reg_sci );


#ifdef FEATURE_QCHAT
    /* Successfully registered, change the current SCI if it is
     * different from what app requested for
     */
    if(cdma.curr_sci_pref != cdma.use_sci_pref)
    {
       M1X_MSG( DCP, LEGACY_HIGH,
         "Changed Current SCI to %d",
         cdma.use_sci_pref);
       cdma.curr_sci_pref = cdma.use_sci_pref;
    }
#endif /* FEATURE_QCHAT */

    /* Recalculate the operational sci */
    if ( !mcc_set_operational_sci() )
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "Operational SCIs was not updated after registration ack");
    }

    new_sci_s = mccidl_get_slot_cycle_index_s();

    M1X_MSG( DCP, LEGACY_HIGH,
      "Operational SCIs after registration ack is %d",
      new_sci_s);

    /* Recalculate the SCI mask */
    mccidl_set_sci_mask( mcc_calculate_sci_mask( new_sci_s ) );
    #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
    if(cdma.entry_ptr->entry != MCC_CSFB)
    #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
    {
      /* Update search's parameters
         This will cause SRCH to start using new SCI
      */
      mccidl_send_srch_parms();

      /* To avoid default WIN_N overrides pilot specific WIN_N, update Searcher's
         neighbor set also. */
      mccidl_send_srch_idle_nset();
    } /* if(cdma.entry_ptr->entry != MCC_CSFB) */
  }
} /* mccreg_success */

/*===========================================================================

FUNCTION MCCREG_IDLE_ACK_OR_FAIL

DESCRIPTION
  This function performs registration actions required after an
  acknowledgement is received to any message other than a Registration,
  Origination or Page Response or after a failed access attempt.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Stored registration variables are updated.

===========================================================================*/

void mccreg_idle_ack_or_fail( void )
{
  mccreg_zone_type *zone_ptr, *temp_zone_ptr;
    /* Pointer to an item on ZONE_LISTs */
  mccreg_sid_nid_type *sid_ptr, *temp_sid_ptr;
    /* Pointer to an item on SID_NID_LISTs */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_QCHAT
if(qchatupk_data_q1x_enabled())
  {
     M1X_MSG( DCP, LEGACY_HIGH,
       "QChat_1X: Checking 1X CB for queued SCI.");
     qchatupk_sci_1x_cb();
  }
#endif /* FEATURE_QCHAT */

  /* Enter Critical Section */
  REX_ISR_LOCK(&mc_reg_crit_sect);
  
  if (mccreg_zone_ptr != NULL)
  {
    /* -----------------------------------------------------------------
    ** IS95A: Delete from ZONE_LISTs all entries that have a different
    ** serving system than SERVSYSs.
    ** J-STD-008: Delete all entries from ZONE_LISTs belonging to a
    ** different PCS frequency block than the frequency block associated
    ** with SIDs.  Delete all entries from ZONE_LISTs belonging to a
    ** different band class than the band class associated with SIDs.
    ** ----------------------------------------------------------------- */
    if ( ( ( (mccreg_zone_ptr->block_or_sys != cdma.block_or_sys)
             #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
             /* This check will prevent deleting of entry from queue having a
              * wild-class blk_or_sys. Thus we will retain context from CSFB
              * mode w.r.t.blk_or_sys. Note that in LTE mode we will not consider
              * blk_or_sys altogether for cleaning lists.
              */
             && ( (cdma.block_or_sys != MCC_BLK_SYS_WILDCARD) &&
                  (mccreg_zone_ptr->block_or_sys != MCC_BLK_SYS_WILDCARD)
                )
             #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
           )
           ||
           ( (mccreg_zone_ptr->band_class != cdma.band_class)
              #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
              /* This check will prevent deleting of entry from queue having a
               * wild-class blk_or_sys. Thus we will retain context from CSFB
               * mode w.r.t.blk_or_sys. Note that in LTE mode we will not consider
               * blk_or_sys altogether for cleaning lists.
               */
             && ( (cdma.band_class != MCC_BAND_CLS_WILDCARD) &&
                  (mccreg_zone_ptr->band_class != MCC_BAND_CLS_WILDCARD)
                )
             #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
           )
         )
       )
    {
      q_put( &mccreg_zone_free_q, &mccreg_zone_ptr->link );
      mccreg_zone_ptr = NULL;
    }
  }

  /* -----------------------------------------------------------------
  ** Delete all ZONE_LISTs entries with active timers which are from a
  ** different serving system, frequency block or band class than SIDs
  ** ------------------------------------------------------------------ */
  zone_ptr = (mccreg_zone_type*) q_check( &mccreg_zone_q );

  while (zone_ptr != NULL)
  {
    if ( ( ( (zone_ptr->block_or_sys != cdma.block_or_sys)
             #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
             /* This check will prevent deleting of entry from queue having a
              * wild-class blk_or_sys. Thus we will retain context from CSFB
              * mode w.r.t.blk_or_sys. Note that in LTE mode we will not consider
              * blk_or_sys altogether for cleaning lists.
              */
             && ( (cdma.block_or_sys != MCC_BLK_SYS_WILDCARD) &&
                  (zone_ptr->block_or_sys != MCC_BLK_SYS_WILDCARD)
                )
             #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
           )
           ||
           ( (zone_ptr->band_class != cdma.band_class)
             #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
             /* This check will prevent deleting of entry from queue having a
              * wild-class blk_or_sys. Thus we will retain context from CSFB
              * mode w.r.t.blk_or_sys. Note that in LTE mode we will not consider
              * blk_or_sys altogether for cleaning lists.
              */
             && ( (cdma.band_class != MCC_BAND_CLS_WILDCARD) &&
                  (zone_ptr->band_class != MCC_BAND_CLS_WILDCARD)
                )
             #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
           )
         )
       )
    {
      /* Zone is from a different serving system - delete it */
      temp_zone_ptr = zone_ptr;
      zone_ptr = (mccreg_zone_type*) q_next( &mccreg_zone_q, &zone_ptr->link );

      #ifdef FEATURE_Q_NO_SELF_QPTR
      q_delete(&mccreg_zone_q, &temp_zone_ptr->link );
      #else
      q_delete( &temp_zone_ptr->link );
      #endif /* FEATURE_Q_NO_SELF_QPTR */

      q_put( &mccreg_zone_free_q, &temp_zone_ptr->link );
    }
    else
    {
      zone_ptr = (mccreg_zone_type*) q_next( &mccreg_zone_q,
                                             &zone_ptr->link );
    }
  }

  /* -----------------------------------------------------------------
  ** For any entry of ZONE_LISTs not matching REG_ZONEs, SIDs and NIDs
  ** and not having an active entry timer, enable the timer with the
  ** duration specified by ZONE_TIMERs.
  ** ----------------------------------------------------------------- */
  if (mccreg_zone_ptr != NULL)
  {
    if ((mccreg_zone_ptr->sid != mccreg_vars.idle_reg_info.sid) ||
        ((mccreg_zone_ptr->nid != mccreg_vars.idle_reg_info.nid) &&
         (mccreg_zone_ptr->nid != CAI_HOME_NID)) ||
        (mccreg_zone_ptr->reg_zone != mccreg_vars.idle_reg_info.reg_zone))
    {
      /* SID, NID and REG_ZONE don't match current zone - enable timer */
      mccreg_zone_ptr->cnt =
        timer_tab[mccreg_vars.idle_reg_info.zone_timer] + mccreg_vars.timer_cnt;

      mccreg_add_zone( mccreg_zone_ptr );

      /* There is no longer an entry with a disabled timer */
      mccreg_zone_ptr = NULL;
    }
  } /* end if (mccreg_zone_ptr != NULL) */

  /* ------------------------------------------------------------------
  ** IS95A: Delete from SID_NID_LISTs all entries that have a different
  ** serving system than SERVSYSs.
  ** J-STD-008: Delete all entries from SID_NID_LISTs belonging to a
  ** different PCS frequency block than the frequency block associated
  ** with SIDs.  Delete all entries from SID_NID_LISTs belonging to a
  ** different band class than the band class associated with SIDs.
  ** ------------------------------------------------------------------ */
  if (mccreg_sid_nid_ptr != NULL)
  {
    if ( ( ( ( mccreg_sid_nid_ptr->block_or_sys != cdma.block_or_sys)
             #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
             /* This check will prevent deleting of entry from queue having a
              * wild-class blk_or_sys. Thus we will retain context from CSFB
              * mode w.r.t.blk_or_sys. Note that in LTE mode we will not consider
              * blk_or_sys altogether for cleaning lists.
              */
             && ( (cdma.block_or_sys != MCC_BLK_SYS_WILDCARD) &&
                  (mccreg_sid_nid_ptr->block_or_sys != MCC_BLK_SYS_WILDCARD)
                )
            #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
           )
           ||
           ( (mccreg_sid_nid_ptr->band_class != cdma.band_class)
             #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
             /* This check will prevent deleting of entry from queue having a
              * wild-class blk_or_sys. Thus we will retain context from CSFB
              * mode w.r.t.blk_or_sys. Note that in LTE mode we will not consider
              * blk_or_sys altogether for cleaning lists.
              */
             && ( (cdma.band_class != MCC_BAND_CLS_WILDCARD) &&
                  (mccreg_sid_nid_ptr->band_class != MCC_BAND_CLS_WILDCARD)
                )
             #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
           )
         )
       )
    {
      q_put( &mccreg_sid_nid_free_q, &mccreg_sid_nid_ptr->link );
      mccreg_sid_nid_ptr = NULL;
    }
  }

  /* ------------------------------------------------------------------
  ** Delete all SID_NID_LISTs entries with active timers which are from
  ** a different serving system, freq block or band class than SIDs
  ** ------------------------------------------------------------------ */
  sid_ptr = (mccreg_sid_nid_type*) q_check( &mccreg_sid_nid_q );

  while (sid_ptr != NULL)
  {
    if ( ( ( (sid_ptr->block_or_sys != cdma.block_or_sys)
             #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
             /* This check will prevent deleting of entry from queue having a
              * wild-class blk_or_sys. Thus we will retain context from CSFB
              * mode w.r.t.blk_or_sys. Note that in LTE mode we will not consider
              * blk_or_sys altogether for cleaning lists.
              */
             && ( (cdma.block_or_sys != MCC_BLK_SYS_WILDCARD) &&
                  (sid_ptr->block_or_sys != MCC_BLK_SYS_WILDCARD)
                )
             #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
           )
           ||
           ( (sid_ptr->band_class != cdma.band_class)
             #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
             /* This check will prevent deleting of entry from queue having a
              * wild-class blk_or_sys. Thus we will retain context from CSFB
              * mode w.r.t.blk_or_sys. Note that in LTE mode we will not consider
              * blk_or_sys altogether for cleaning lists.
              */
             && ( (cdma.band_class != MCC_BAND_CLS_WILDCARD) &&
                  (sid_ptr->band_class != MCC_BAND_CLS_WILDCARD)
                )
             #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
           )
         )
       )
    {
      /* SID is from different serving system - delete it */
      temp_sid_ptr = sid_ptr;
      sid_ptr = (mccreg_sid_nid_type*) q_next( &mccreg_sid_nid_q,
                                               &sid_ptr->link );
      #ifdef FEATURE_Q_NO_SELF_QPTR
      q_delete( &mccreg_sid_nid_q, &temp_sid_ptr->link );
      #else
      q_delete( &temp_sid_ptr->link );
      #endif /* FEATURE_Q_NO_SELF_QPTR */

      q_put( &mccreg_sid_nid_free_q, &temp_sid_ptr->link );
    }
    else
    {
      sid_ptr = (mccreg_sid_nid_type*) q_next( &mccreg_sid_nid_q,
                                               &sid_ptr->link );
    }
  }

  /* -----------------------------------------------------------------
  ** For any entry of SID_NID_LISTs not matching SIDs and NIDs and not
  ** having an active entry timer, enable the entry timer with the
  ** duration specified by ZONE_TIMERs.
  ** ----------------------------------------------------------------- */
  if (mccreg_sid_nid_ptr != NULL)
  {
    if ((mccreg_sid_nid_ptr->sid != mccreg_vars.idle_reg_info.sid) ||
        ((mccreg_sid_nid_ptr->nid != mccreg_vars.idle_reg_info.nid) &&
         (mccreg_sid_nid_ptr->nid != CAI_HOME_NID)))
    {
      /* Enable timer */
      mccreg_sid_nid_ptr->cnt =
        timer_tab[mccreg_vars.idle_reg_info.zone_timer] + mccreg_vars.timer_cnt;

      mccreg_add_sid_nid( mccreg_sid_nid_ptr );

      /* There is no longer an entry with a disabled timer */
      mccreg_sid_nid_ptr = NULL;
    }
  } /* end if (mccreg_sid_nid_ptr != NULL) */

  /* Leave Critical Section */
  REX_ISR_UNLOCK(&mc_reg_crit_sect);
  
} /* mccreg_idle_ack_or_fail */

/*===========================================================================

FUNCTION MCCREG_TC_INIT

DESCRIPTION
  This function performs registration procedures required for entering
  the Traffic Channel Initialization Substate of the Mobile Station Control
  on the Traffic Channel State as specified in IS-95A section 6.6.5.5.4.1.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Timer based registration is disabled.

===========================================================================*/

void mccreg_tc_init( void )
{
  /* ---------------------------------------------------------------------
  ** Upon entering the Traffic Channel Initialization Substate, the mobile
  ** station shall set COUNTER_ENABLEDs to NO
  ** --------------------------------------------------------------------- */
  mccreg_vars.counter_enabled = FALSE;

  (void) timer_clr( &mccreg_timer_reg_timer, T_NONE );

} /* mccreg_tc_init */

/*===========================================================================

FUNCTION MCCREG_MS_REG

DESCRIPTION
  This function performs registration procedures required after receiving
  a Mobile Station Registered Message as specified in IS-95A section
  6.6.5.5.4.3.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Registration variables are updated.

===========================================================================*/

/*lint -e818*/
void mccreg_ms_reg
(
  caii_registered_type *msg_ptr
    /* Pointer to received Mobile Station Registered Message */
)
{
  /* -----------------------------------------------
  ** The mobile shall perform the following actions:
  ** ----------------------------------------------- */

  /* -----------------------------------------------------------------
  ** Update ZONE_LISTs and SID_NID_LISTs with REG_ZONEs, SIDs and NIDs
  ** ----------------------------------------------------------------- */
  mccreg_update( msg_ptr->sid, msg_ptr->nid, msg_ptr->reg_zone,
                 msg_ptr->zone_timer, msg_ptr->total_zones );


  /* ---------------------------------------------------------------
  ** If MULT_SIDs is equal to '0' and SID_NID_LIST contains entries
  ** with different SIDs, delete the excess entries according to the
  ** rules specified in 6.6.1.5.
  ** --------------------------------------------------------------- */
  if (msg_ptr->mult_sids == 0)
  {
    mccreg_delete_mult_sids();
  }

  /* -----------------------------------------------------------------
  ** If MULT_NIDs is equal to '0' and SID_NID_LIST contains more than
  ** one entry for any SID, delete the excess entries according to the
  ** rules specified in 6.6.5.1.5.
  ** ----------------------------------------------------------------- */
  if (msg_ptr->mult_nids == 0)
  {
    mccreg_delete_mult_nids();
  }

  /* -----------------------------------------------------------------
  ** Set the stored location of last registration (BASE_LAT_REGs-p
  ** and BASE_LONG_REGs-p) to the current base station's location
  ** (BASE_LATs and BASE_LONGs).  Set the stored registration distance
  ** (REG_DIST_REGs-p) to the current base station's registration
  ** distance (REG_DISTs).
  ** ----------------------------------------------------------------- */
  mccreg_vars.base_lat_reg = msg_ptr->base_lat;
  mccreg_vars.base_long_reg = msg_ptr->base_long;
  mccreg_vars.reg_dist = msg_ptr->reg_dist;
  mccreg_vars.dist_reg_reqd = FALSE;

  /* ----------------------------------------------------------------
  ** Set the stored band class and block-or-sys of last registration.
  ** ---------------------------------------------------------------- */
  mccreg_vars.band_class = cdma.band_class;
  mccreg_vars.block_or_sys = (uint16)
          sd_map_chan_to_blksys(SD_MODE_CDMA,
                                (sys_band_class_e_type) cdma.band_class,
                                cdma.cdmach);
    // func is returing enum (32 bit) and assigning to 16 bit field,
    // but values are low enough to fit without any problem.

  /* -------------------------------------
  ** Set MOB_TERMs as specified in 6.6.5.3
  ** ------------------------------------- */
  mccreg_set_mob_term( msg_ptr->sid, msg_ptr->nid );

} /* end mccreg_ms_reg() */
/*lint +e818*/

/*===========================================================================

FUNCTION MCCREG_GET_MOB_TERM

DESCRIPTION
  This function retrieves the call termination enabled indicator.

DEPENDENCIES
  None.

RETURN VALUE
  Call termination enabled indicator.

SIDE EFFECTS
  None.

===========================================================================*/

boolean mccreg_get_mob_term( void )
{
  return (mccreg_vars.mob_term);

} /* mccreg_get_mob_term */

/*===========================================================================

FUNCTION MCCREG_SET_IDLE_REG_INFO

DESCRIPTION
  This function stores Idle state BS info to be used in registration
  success/failure procedure.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/

/*lint -e818*/
void mccreg_set_idle_reg_info
(
  caii_sysparm_type *msg_ptr
    /* Pointer to received System Parameter message */
)
{
  M1X_MSG( DCP, LEGACY_MED,
    "Update REG_SID = %d, REG_NID = %d and REG_REG_ZONE = %d from ovhd",
    msg_ptr->sid, msg_ptr->nid, msg_ptr->reg_zone);
  mccreg_vars.idle_reg_info.sid = msg_ptr->sid;
  mccreg_vars.idle_reg_info.nid = msg_ptr->nid;
  mccreg_vars.idle_reg_info.reg_zone = msg_ptr->reg_zone;
  mccreg_vars.idle_reg_info.zone_timer = msg_ptr->zone_timer;

} /* mccreg_set_idle_reg_info */
/*lint +e818*/


/*===========================================================================

FUNCTION MCCREG_GET_POWERUP_REG_COUNT

DESCRIPTION
  This function retrieves the count for the number of powerup registrations
  performed.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mccreg_get_powerup_reg_count
(
  qword count
)
{
  qw_equ(count, mccreg_powerup_reg_count);
  M1X_MSG( DCP, LEGACY_MED,
    "Powerup Reg count : HI %d LO %d",
    qw_hi(mccreg_powerup_reg_count),
    qw_lo(mccreg_powerup_reg_count));

} /* mccreg_get_powerup_reg_count */

/*===========================================================================

FUNCTION MCCREG_RESET_POWERUP_REG_COUNT

DESCRIPTION
  This function resets the count for the number of powerup registrations
  performed.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mccreg_reset_powerup_reg_count(void)
{
  /* ----------------------------------------------------------------
  ** Both MC and UI can update the count. Protect critical section.
  ** ---------------------------------------------------------------- */
  REX_ISR_LOCK(&mc_crit_sect);

  qw_set(mccreg_powerup_reg_count, 0, 0);

  REX_ISR_UNLOCK(&mc_crit_sect);

  M1X_MSG( DCP, LEGACY_MED,
    "Reset Powerup Reg count : HI %d LO %d",
    qw_hi(mccreg_powerup_reg_count),
    qw_lo(mccreg_powerup_reg_count));

} /* mccreg_reset_powerup_reg_count */

/*===========================================================================

FUNCTION MCCREG_INCREMENT_POWERUP_REG_COUNT

DESCRIPTION
  This function increments the count for the number of powerup registrations
  performed by 1.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mccreg_increment_powerup_reg_count(void)
{
  /* ----------------------------------------------------------------
  ** Both MC and UI can update the count. Protect criterical section.
  ** ---------------------------------------------------------------- */
  REX_ISR_LOCK(&mc_crit_sect);

  qw_inc(mccreg_powerup_reg_count, 1);

  REX_ISR_UNLOCK(&mc_crit_sect);

  M1X_MSG( DCP, LEGACY_MED,
    "INC Powerup Reg count : HI %d LO %d",
    qw_hi(mccreg_powerup_reg_count),
    qw_lo(mccreg_powerup_reg_count));

} /* mccreg_increment_powerup_reg_count */


#ifdef FEATURE_IS2000_REL_C
/*===========================================================================

FUNCTION MCCREG_SET_REGISTER_IN_IDLE

DESCRIPTION
  This function sets the register in idle flag which is used for
  enc/msg integrity resync required registration as commanded by BS on TC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Setting this may cause MS to perform "enc/msgint resync required" reg
  in the common channel.

===========================================================================*/

void mccreg_set_register_in_idle ( boolean register_in_idle)
{
  mccreg_vars.register_in_idle = register_in_idle;

} /* mccreg_set_register_in_idle */
#endif /* FEATURE_IS2000_REL_C */

/*===========================================================================

FUNCTION MCCREG_IS_REG

DESCRIPTION
  This function check if MS is currently registered with a CDMA network.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - MS is currtenly registered with a CDMA network.
  FALSE - MS is not currently registered with a CDMA network.

SIDE EFFECTS
  None.

===========================================================================*/
boolean mccreg_is_reg( void )
{
  if( mccreg_vars.reg_status == MCCREG_REG_COMPLETED )
  {
    return TRUE;
  }
  return FALSE;
} /* mccreg_is_reg */

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X
/*===========================================================================

FUNCTION MCCREG_CSFB_REG_INIT

DESCRIPTION
  This procedure performs init operations before entering into CSFB sub-states.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void mccreg_csfb_reg_init(void)
{
  /* Initialize Registration Powerup Related Variables */
  mccreg_vars.powerup_timer_on = FALSE;
  mccreg_vars.powering_up = FALSE;

  /* Register the zone timer */
  timer_reg( &mccreg_zone_timer, (timer_t2_cb_type)mccreg_zone_timer_done,
             (timer_cb_data_type)NULL, 5000L, 5000L );
  /* Initialize Registration related parameters 
   * (Timer based registration REG_COUNT = Random Value, etc) */
  (void) mccreg_idle_check();

} /* void mccreg_csfb_reg_init(void) */
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X */

#ifdef FEATURE_MODEM_CONFIG_REFRESH
/*===========================================================================

FUNCTION MCCREG_READ_NV_POWERUP_REG_PERFORMED

DESCRIPTION
  Reads the NV item NV_CDMA_POWERUP_REG_PERFORMED_I. 
  This procedure should only be called during MCFG refresh.

DEPENDENCIES
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mccreg_read_nv_powerup_reg_performed( void )
{
  nv_stat_enum_type read_status;  /* NV read status */
  
  /* Retrieve the number of CDMA powerup registrations performed from NV */
  mccreg_nv_buf.item = NV_CDMA_POWERUP_REG_PERFORMED_I;
  mccreg_nv_buf.cmd  = NV_READ_F;
  mccreg_nv_buf.data_ptr = &mccreg_nv_data;

  read_status = mcc_nv_cmd( &mccreg_nv_buf );

  /* If the NV read is failing, then just ignore the NV */
  if (read_status == NV_DONE_S)
  {
    qw_equ(mccreg_powerup_reg_count, mccreg_nv_data.cdma_powerup_reg_performed);
  }
} /* mccreg_read_nv_powerup_reg_performed */
#endif /* FEATURE_MODEM_CONFIG_REFRESH */
#ifndef MMOC_V_H
#define MMOC_V_H
/**===========================================================================
@FILE_DOC

                    M U L T I M O D E   C O N T R O L L E R

                             H E A D E R   F I L E

DESCRIPTION
  This header file contains all the definitions necessary for Call Manager
  (CM) and the protocols ( MC, HDR and REG) to interface with the
  Multi-MOde Controller (MMoC) task.

Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/

/* <EJECT> */
/**===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  L:/src/asw/MSM_MM/APPS/vcs/mmoc.h_v   1.3   10 Jun 2002 11:35:48   sjaikuma  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/mmoc_v.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/28/13   jvo     Cleanup remaining Fusion pieces
07/21/13   skk     FR2921: Network initiated Authentication Procedure Support
                   during emergency call When SIM is perso Locked
07/19/13   jvo     Remove all MDM/MSM Fusion features
09/27/12   vm      Removing DIME featurization and cleanup
09/20/12   vm      Added ignore_protocol_activate in mmoc_cmd_suspend_ss_s_type
05/01/12   gm      Add offline notify send from MMOC
03/24/12   gm      DIME integration support - fix to compilation errors
11/30/11   jh      Added feature for dynamic rat acq order change
01/17/12   aj/cl   SC SVLTE SO 33 changes
01/10/12   gm      Mainlining HDR API change integration
12/15/11   sg      Add TDS band pref to mmoc_cmd_get_networks_gw().
09/14/11   vk      New MMOC transaction "MMOC_TRANS_DEACT_1XCSFB_CMD" related changes.
07/22/11   xs      MMOC SC_SVLTE 1.0 merging to Mainline
05/14/11   rk      Adding user mode preference for mmoc_cmd_pref_sys_chgd4
05/12/11   mp      BPLMN - LTE Only capable is not able to perform Manual Search
04/12/11   ay      TD-SCDMA changes
03/09/11   cl      Add support for SVLTE2 MMSS arbitration
02/28/11   aj      Add wait for activation_cnf state in DO to LTE reselection
02/14/11   rm      LTE RAT-B changes
01/04/11   rm      CMI-4: Moved #def to internal files
12/23/10   sv      Integrate DSDS feature
12/10/10   rk      MO/MT 1XCSFB call successful scenarios support
12/03/10   gm      Fix to compilation warning- added void
11/06/10   aj      DO -> LTE reselection code
10/01/10   ak      Merged the Fusion code from Dev branch.
07/08/10   sv      Added msm_sys_chgd API command
02/11/10   rn      LTE-1x redirection/reselection
01/28/10   mp      EF_EPRL Implementation
12/11/09   rn      LTE-eHRPD Support
12/11/09   fj      Added mmoc_cmd_subscription_chgd3 and mmoc_cmd_pref_sys_chgd3.
10/21/09   mh      Added code for GW/LTE Support
07/13/09   np      Added code for LTE support
07/06/09   rm      Moving mmoc_task from mmoc_v.h to mmoc.h
07/30/08   rm      CMI defeaturisation
09/04/08   sv      Added WLAN_WPS API
04/28/08   fj      Lint cleanup, CR 144389
04/17/07   rp      Added RPC metacomments for WLAN.
04/16/07   ic      Moved inclusion of AEE* headers to mmoci.h
03/19/07   ka      Replacing banned apis
01/31/07   ka      Adding new state in MMOC to wait for TMC to ack operating
                   mode change.
11/15/06   sk      Added hybrid GW support.
08/29/06   sk      Added WLAN Adhoc support.
04/03/06   ka      Changing copyright.
03/02/05   ka      Added prot_subsc_chag param to mmoc_cmd_subscription_chgd
08/20/04   sj      Added support for prl_pref.
02/27/03   SJ      Added support sanity_timeout err_fatal in respective
                   protocols.
02/13/03   SJ      Added mmoc_cmd_wakeup_from_pwr_save API.
01/16/03   SJ      Added define to indicate if MMOC uses DB.
09/03/02   SJ      Unfeaturized GSM support in MMoC.
03/28/02   SJ      Initial release.

===========================================================================*/


#include "customer.h"  /* Customer configuration file */
#include "comdef.h"    /* Definition for basic types and macros */
#include "prot.h"
#include "prot_v.h"      /* Common types associated with MMoC <-> protocol
                       ** communciation.
                       */
#include "sd_v.h"
#include "mmgsdilib.h"
#include "mmgsdilib_common.h"
#include "mmgsdisessionlib.h"

#include "mmoc.h"
#include "mmoc_msgr.h"

/*===========================================================================
=============================================================================
=============================================================================
============================== Common interface types =======================
=============================================================================
=============================================================================
===========================================================================*/
/** Macro for creating a bitmask by shifting '1' left by a specified
    number indicated by 'val'. Useful for defining bitmask enum values.
*/
#define BM( val ) ( 1<< ((int)(val)) )

/** @defgroup CIT Common Interface Types
** @{
*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/



/* Enumeration for  type of command queued in the MMoC's command queue.
*/
typedef enum {

  MMOC_CMD_SUBSCRIPTION_CHGD,          /* Subscription changed command, could
                                       ** because of NAM selection change,
                                       ** GSM/WCDMA subscription availability
                                       ** change or CDMA/AMPS/HDR subscription
                                       ** availability change.
                                       */

  MMOC_CMD_PROT_GEN_CMD,               /* Generic command bound for protocol,
                                       ** could be
                                       ** pref sys changed command or
                                       ** reselection timeout command or
                                       ** get networks command for GW or
                                       ** terminate get networks command for
                                       ** GW.
                                       */

  MMOC_CMD_OPRT_MODE_CHGD,             /* Operating mode changed command type.
                                       */


  MMOC_CMD_WAKEUP_FROM_PWR_SAVE,       /* Power save wakeup command.
                                       */

  MMOC_CMD_DUAL_STANDBY_CHGD,          /* Dual standby preference changed */


  MMOC_CMD_PROT_REDIR_IND,             /* Redirection command from redirected
                                       ** protocol
                                       */

  MMOC_CMD_PROT_HO_IND,                /* Hand off command from Hand overed
                                       ** protocol
                                       */

  MMOC_CMD_MMGSDI_INFO_IND,            /* MMGSDI card/session information
                                       ** command from MMGSDI card
                                       */

  MMOC_CMD_DEACT_1XCSFB_PROT,          /* Deactivate 1XCSFB protocol */

  MMOC_CMD_SUSPEND_SS,                 /* Suspend status of SS has changed. */

  MMOC_CMD_DEACT_FROM_DORMANT,         /* Command to deactivate a protocol
                                       ** from dormant state
                                       */
  MMOC_CMD_MCFG_REFRESH,              /* Command to handle MCFG refresh
                                                                  */                                   

  MMOC_CMD_MAX                         /* For internal use. */

} mmoc_cmd_e_type;


/** Enumeration of subscription change type.
*/
typedef enum {

  MMOC_SUBSC_CHG_SUBSC_AVAIL,          /**< Subscription changed due to change
                                       ** in CDMA or GSM/WCDMA subscription
                                       ** availability.
                                       */

  MMOC_SUBSC_CHG_NAM_SEL,              /**< Subscription changed due to NAM
                                       ** selection change.
                                       */

  MMOC_SUBSC_CHG_MODE_CHNG,            /**< Subscription changed due to MODE
                                       ** PREF change.
                                       */

  MMOC_SUBSC_CHG_MAX                   /**< For internal use */

} mmoc_subsc_chg_e_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

typedef enum
{
  MMOC_MODE_NONE    =   SYS_SYS_MODE_NONE,
  MMOC_MODE_INACT   =   SYS_SYS_MODE_NO_SRV,
  MMOC_MODE_AMPS    =   SYS_SYS_MODE_AMPS,
  MMOC_MODE_CDMA    =   SYS_SYS_MODE_CDMA,
  MMOC_MODE_HDR     =   SYS_SYS_MODE_HDR,
  MMOC_MODE_GPS     =   SYS_SYS_MODE_GPS,
  MMOC_MODE_GSM     =   SYS_SYS_MODE_GSM,
  MMOC_MODE_WCDMA   =   SYS_SYS_MODE_WCDMA,
  MMOC_MODE_GW      =   SYS_SYS_MODE_GW,
  MMOC_MODE_LTE     =   SYS_SYS_MODE_LTE,
  MMOC_MODE_GWL     =   SYS_SYS_MODE_GWL,
  MMOC_MODE_HYBR_GW =   11,
  MMOC_MODE_HYBR_3_GW =   12,
  MMOC_MODE_MAX     =   13
} mmoc_mode_e_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/



typedef struct {

  sys_modem_as_id_e_type          as_id;
    /* subscription-id of this subscription */

  uint16                          nv_context;
    /* NV Context used for this subscription */

  mmgsdi_session_type_enum_type   session_type;
    /* Session type used for this subscription */

  boolean                         is_subs_avail;
    /* Avail/Not Avail */

  boolean                         is_perso_locked;
    /* Perso locked or not */

  boolean                         is_subsc_chg;
    /* Has Subscription changed */

  sd_ss_e_type                    ss;
    /* SS on which this subscription is to be provisioned */

  sd_ss_orig_mode_e_type          orig_mode;
  sd_ss_mode_pref_e_type          mode_pref;
  /* 2G and 3G band preference, excluding TDSCDMA */
  sd_ss_band_pref_e_type          band_pref;
  sd_ss_roam_pref_e_type          roam_pref;
  sd_ss_hybr_pref_e_type          hybr_pref;
  /* LTE band preference */
  sys_lte_band_mask_e_type        lte_band_pref;
  /* TDSCDMA band preference */
  sd_ss_band_pref_e_type          tds_band_pref;
  /* Preferences for the subscription */

  uint32          subs_capability;
  /* Technology specific preferences of the subscription */
  union
  {
    /* GW preferences for GW/Hybr GW subscription */
    struct
    {
      sd_ss_acq_order_pref_e_type  acq_order_pref;
      sd_ss_srv_domain_pref_e_type srv_domain_pref;
      sys_manual_sys_info_s_type   manual_sys_info;
    } gw;


    /* CDMA preferences for CDMA subscription */
    struct
    {
      sd_ss_prl_pref_e_type        prl_pref;
    } cdma;

  } prm;
   sys_ue_mode_e_type curr_ue_mode;

   boolean                        is_ue_mode_substate_srlte;
   boolean                        only_subs_cap_change;
} mmoc_subs_data_s_type;

typedef enum{

  MMOC_PREF_SYS_ADDL_ACTION_NONE = 0,
     /**< For internal use */

  MMOC_PREF_SYS_ADDL_ACTION_PS_DETACH = BM(0),
       /* Send PS detach request prior to handling the pref update cmd */

  MMOC_PREF_SYS_ADDL_ACTION_UE_MODE_SWITCH = BM(1),
       /* Send PS detach request prior to handling the pref update cmd */

  MMOC_PREF_SYS_ADDL_ACTION_RESUME = BM(2),
       /* Resume stack which was suspened due to Em call */

  MMOC_PREF_SYS_ADDL_ACTION_MAX
       /**< For internal use */


}mmoc_pref_sys_addl_action_e_type;

/**===========================================================================
=============================================================================
=============================================================================
============================== CM -> MMOC commands ==========================
=============================================================================
=============================================================================
===========================================================================*/

/** @defgroup CMC CM->MMOC commands
** @{
*/

/* The following section defines the API when the clients ( example CM )
** have to communicate to MMoC using the command queue.
*/


/**===========================================================================

@FUNCTION mmoc_cmd_subscription_chgd3

@DESCRIPTION
  This function should be called when the subscription changed because of NAM
  change or GSM/WCDMA subscription availability change or CDMA/AMPS/HDR
  subscription availability change. This function will queue the subscription
  changed command to the MMoC task.

  lte_band_pref is supported in this funciton.

@DEPENDENCIES
  None.

@RETURN VALUE
  None.

@SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           mmoc_cmd_subscription_chgd3
(
        mmoc_subsc_chg_e_type          chg_type,
             /**< Subscription change type.
             */

        byte                           nam,
            /**< Current NAM selection (NAM-1, NAM-2, etc.).
            */

        sd_ss_orig_mode_e_type         orig_mode,
            /**< The origination mode (normal, OTASP, emergency, other, etc.)
            ** that is associated with the selected NAM.
            */

        sd_ss_mode_pref_e_type         mode_pref,
           /**< The mode preference that is associated with the selected NAM.
           */

        sd_ss_band_pref_e_type         band_pref,
           /**< Band preference that is associated with the selected NAM.
           */

        sys_lte_band_mask_e_type       lte_band_pref,
           /* LTE band preference that is associated with the selected NAM.
           */

        sd_ss_band_pref_e_type         tds_band_pref,
           /* TD-SCDMA band preference that is associated with the selected NAM.
           */
        sd_ss_prl_pref_e_type          prl_pref,
           /**< PRL preference that is associated with the selected NAM.
           */

        sd_ss_roam_pref_e_type         roam_pref,
           /**< Roam preference that is associated with the selected NAM.
           */

        sd_ss_hybr_pref_e_type         hybr_pref,
           /**< New hybrid preference (none, CDMA/HDR).
           */

        const sys_manual_sys_info_s_type    *manual_sys_info_ptr,
          /**< Pointer to struct that defines the manual system.
           */

        sd_ss_srv_domain_pref_e_type   srv_domain_pref,
           /**< Service domain preference that is associated with the selected
           ** NAM.
           ** Applies only for GSM/WCDMA modes.
           */

        sd_ss_acq_order_pref_e_type    acq_order_pref,
           /**< Preference for the order of acquisition ( WCDMA before GSM,
           ** GSM before WCDMA etc).
           */

        boolean                        is_gwl_subsc_avail,
           /**< Current GSM/WCDMA/LTE subscription availability status.
           ** true = subscription available,
           ** false = subscription not available.
           */

        boolean                        is_cdma_subsc_avail,
           /**< Current CDMA/AMPS/HDR subscription availability status.
           ** true = subscription available,
           ** false = subscription not available.
           */


        prot_subsc_chg_e_type          prot_subsc_chg
           /**< Enumeration of protocols that have a change in
           ** subscription available status
           */
);

/**===========================================================================

@FUNCTION mmoc_cmd_oprt_mode_chgd

@DESCRIPTION
  This function will be used to command the MMoC to do operating mode change;
  the new operating mode is specified in the oprt_mode parameter. This
  function will queue the operating mode changed command to the MMoC task.

@DEPENDENCIES
  None.

@RETURN VALUE
  None.

@SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           mmoc_cmd_oprt_mode_chgd
(

        sys_oprt_mode_e_type           oprt_mode
            /**< The operating mode to be switched to.
            */

);

/**===========================================================================

@FUNCTION mmoc_cmd_pref_sys_chgd6

@DESCRIPTION
  This function is used to command the MMoC to do Preferred system selection
  change. This function will queue the preferred system selection changed
  command to the MMoC Task.

  lte_band_pref and CSG are supported in this function.

@DEPENDENCIES
  None.

@RETURN VALUE
  None.

@SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           mmoc_cmd_pref_sys_chgd6
(

        sd_ss_e_type                   ss,
            /**< Indicates the SS-Instance ( SS_MAIN or SS_HDR) this
            ** pref_sys_chgd command is intended for.
            */


        sd_ss_pref_reas_e_type         pref_reas,
            /**< Reason for changing the SS-Preference.
            */

        sd_ss_orig_mode_e_type         orig_mode,
            /**< The origination mode (normal, OTASP, emergency, other, etc.)
            ** that is associated with the selected NAM.
            */

        sd_ss_mode_pref_e_type         mode_pref,
           /**< The mode preference that is associated with the selected NAM.
           */

        sd_ss_band_pref_e_type         band_pref,
           /**< Band preference that is associated with the selected NAM.
           */

        sys_lte_band_mask_e_type       lte_band_pref,
           /* LTE band preference that is associated with the selected NAM.
           */

        sd_ss_band_pref_e_type         tds_band_pref,
           /* TDS band preference that is associated with the selected NAM.
           */

        sd_ss_prl_pref_e_type          prl_pref,
           /**< PRL preference that is associated with the selected NAM.
           */

        sd_ss_roam_pref_e_type         roam_pref,
           /**< Roam preference that is associated with the selected NAM.
           */

        sd_ss_hybr_pref_e_type         hybr_pref,
           /**< New hybrid preference (none, CDMA/HDR).
           */

        sd_band_e_type                 otasp_band,
           /**< The band-class that is associated with OTASP origination mode.
           ** Note that this parameter is ignored if orig_mode != OTASP.
           */

        sd_blksys_e_type               otasp_blksys,
          /**< The PCS frequency block/Cellular system that is associated
          ** with OTASP origination mode. Note that this parameter is ignored
          ** if orig_mode != OTASP.
          */

        sd_ss_avoid_sys_e_type         avoid_type,
          /**< Type of the system to be avoided, valid only
          ** if pref_reason = SD_SS_PREF_REAS_AVOID_SYS.
          */

        dword                          avoid_time,
          /**< Time in seconds for which the system is to be avoided, valid
          ** only if pref_reason = SD_SS_PREF_REAS_AVOID_SYS.
          */

        const sys_manual_sys_info_s_type    *manual_sys_info_ptr,
          /**< Pointer to struct that defines the manual system.
          */

        sd_ss_srv_domain_pref_e_type   srv_domain_pref,
           /**< Service domain preference that is associated with the selected
           ** NAM.
           ** Applies only for GSM/WCDMA modes.
           */

        sd_ss_acq_order_pref_e_type    acq_order_pref,
           /**< Preference for the order of acquisition ( WCDMA before GSM,
           ** GSM before WCDMA etc).
           */

        sd_ss_pref_update_reason_e_type  pref_update_reas,
           /**< Preference change reason
           */

        sys_addtl_action_s_type          *addl_action,
        /*
        To indicate sub action to be performed by MMOC during pref sys chgd cmd
        */

        sd_ss_mode_pref_e_type            user_mode_pref,
        /**< The user/phone mode preference.
        */

        const sd_rat_acq_order_s_type     *rat_acq_order_ptr,
        /**< Rat acquisition order including LTE.
        */

        uint16                            req_id,
        /**< System Selection Preferences request id.
        */

        sd_ss_pref_camp_mode_e_type  camp_only_pref,

        sys_csg_id_type                    csg_id,
         /**< CSG identifier */

        sys_radio_access_tech_e_type       csg_rat,
        /**< RAT specified for CSG */

        sys_voice_domain_pref_e_type       voice_domain_pref
        /**< Voice domain pref */
);


/**===========================================================================

@FUNCTION mmoc_cmd_get_networks_gw

@DESCRIPTION
  The function will be used to command the MMoC to get the networks on
  GSM/WCDMA/TD-SCDMA/LTE technologies. This function will queue the Get networks
  command to the MMoC task.

@DEPENDENCIES
  None.

@RETURN VALUE
  None.

@SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           mmoc_cmd_get_networks_gw
(

        sd_ss_mode_pref_e_type         mode_pref,
           /**< Mode preference that is associated with the Get networks
           ** command.
           */


        sd_ss_band_pref_e_type         band_pref,
          /**< Legacy systems band preference that is associated with the
          ** Get networks command.
          */

        sys_lte_band_mask_e_type       lte_band_pref,
          /**< LTE band preference that is associated with the Get networks
          ** command.
          */

        sd_ss_band_pref_e_type         tds_band_pref,
          /**< TD-SCDMA band preference that is associated with the Get networks
          ** command.
          */

        sd_ss_e_type                   ss,
          /**< System Selection */

        sd_network_list_type_e_type    list_type
          /**< Network List Type */
);


/*===========================================================================

FUNCTION mmoc_cmd_deactivate_1xcsfb_prot

DESCRIPTION
  The function will be used to deactivate 1xCSFB through MMoC


DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void                           mmoc_cmd_deactivate_1xcsfb_prot
(
  prot_deact_e_type deact_reason
);


/**===========================================================================

@FUNCTION mmoc_cmd_term_get_networks_gw

@DESCRIPTION
  This function will be used to inform the MMoC that the user has terminated
  the Get networks request on GSM/WCDMA networks. This function will queue
  the Terminate get networks command to the MMoC task.

@DEPENDENCIES
  None.

@RETURN VALUE
  None.

@SIDE EFFECTS
  None.
===========================================================================*/
extern  void                          mmoc_cmd_term_get_networks_gw
(

                sd_ss_e_type                      ss
);



/**===========================================================================

@FUNCTION mmoc_cmd_suspend_ss

@DESCRIPTION
  This function will be used to command the MMoC to suspend or resume
  operation on a given system selection instance.

@DEPENDENCIES
  None.

@RETURN VALUE
  None.

@SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           mmoc_cmd_suspend_ss
(

        sd_ss_e_type                   ss,
          /**< System Selection instance that needs to be suspended
          */

        boolean                        is_suspend,
          /**< whether to suspend or resume
          */
        boolean                        ignore_protocol_activate
      /* Indicates whether to ignore protocol activation
      */

);


/**===========================================================================

@FUNCTION mmoc_cmd_suspend_ss_new

@DESCRIPTION
  This function will be used to command the MMoC to suspend or resume
  operation on a given system selection instance when emergency call is dialed on other instance.

@DEPENDENCIES
  None.

@RETURN VALUE
  None.

@SIDE EFFECTS
  None.
===========================================================================*/
extern  void                          mmoc_cmd_suspend_ss_new
(

  sd_ss_e_type                   ss,
    /* System Selection instance that needs to be suspended
    */

  boolean                        is_suspend,
    /* whether to suspend or resume
    */
  boolean                        ignore_protocol_activate,
    /* Whether to activate protocol or not during resume
    */
  prot_deact_e_type              susp_reason
  /* Reason for suspension
  */
);


/**===========================================================================

@FUNCTION mmoc_cmd_wakeup_from_pwr_save

@DESCRIPTION
  This function will be used to command the MMoC to wakeup the phone from
  power save.

@DEPENDENCIES
  None.

@RETURN VALUE
  None.

@SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           mmoc_cmd_wakeup_from_pwr_save
(

        sd_ss_e_type                   ss
          /**< Wakeup the 'ss' System Selection instance from power save.
          */

);


/*===========================================================================

FUNCTION mmoc_cmd_prot_resel_ind

DESCRIPTION
   This function will be used by protocol (LTE RRC) to inform MMoC that it
   wants to start reselection process and activate itself

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           mmoc_cmd_prot_resel_ind(

   sd_ss_e_type                 ss
  /* Stack which needs to be activated
  */

);


/*===========================================================================

FUNCTION mmoc_cmd_prot_gw_sim_state_update_ind

DESCRIPTION
   This function will be used by cmregprx to inform MMoC that it needs
   to update the sim state on T3245 timer expiry indication from NAS.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
=============================================================================*/
extern void                           mmoc_cmd_prot_gw_sim_state_update_ind
(
   prot_cmd_gw_sim_state_e_type         sim_state,
   /* New sim state reported from REG
   */

   sd_ss_e_type                 ss
  /* Stack which needs to be activated
  */
);
/*===========================================================================

FUNCTION mmoc_cmd_prot_local_deact_ind

DESCRIPTION
   This function will be used to locally deactivate the stack.
   This is done by invoke Generic Command, and from there, send new indication
   to SD.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           mmoc_cmd_prot_local_deact_ind(

   sd_ss_e_type                 ss
  /* Stack which needs to be deactivated
  */

);

/*===========================================================================

FUNCTION mmoc_cmd_prot_local_deact_ind

DESCRIPTION
   This function will be used to locally activate the stack.
   This is done by invoke Generic Command, and from there, send new indication
   to SD.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           mmoc_cmd_prot_local_act_ind(

   sd_ss_e_type                 ss
  /* Stack which needs to be activated
  */

);

/*===========================================================================

FUNCTION mmoc_cmd_prot_redir_ind

DESCRIPTION
   This function will be used by protocol to inform MMoC that it wants to
   start redirection process.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           mmoc_cmd_prot_redir_ind
(

        prot_act_e_type                actd_reason,
           /* Reason that the protocol was activated.
           */

        prot_state_e_type              prot_state
           /* Protocol which sent the autonomous activation for redirection.
           */

);

/*===========================================================================

FUNCTION mmoc_cmd_prot_handover_ind

DESCRIPTION
   This indication is sent when TCH is acquired and HOCM is acked by n/w
   along with this, CP passes SID/NID/BAND/CHAN/MCC ,
   SID/NID received from SIB8 which we have validated / from UHDM if they are received.
   Band/Chan received from UHDM
   MCC if its included in C2K Params

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/


extern  void                           mmoc_cmd_prot_handover_ind
(

        prot_act_e_type                actd_reason, //  PROT_ACT_LTE_HANDOVER
           /* Reason that the protocol was activated.
           */

        prot_state_e_type              prot_state, // PROT_STATE_ONLINE_CDMA
           /* Protocol which sent the autonomous activation for Handover.
           */
        sd_ho_sys_param_s_type  sys_param
           /* handovered system params */
);


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_cmd_subscription_chgd_new

DESCRIPTION
  This function should be called when the subscription changed because of NAM
  change or GSM/WCDMA subscription availability change or CDMA/AMPS/HDR
  subscription availability change. This function will queue the subscription
  changed command to the MMoC task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern  void                                 mmoc_cmd_subscription_chgd_new
(
        mmoc_subsc_chg_e_type                chg_type,
             /* Subscription change type.
             */
        prot_subsc_chg_e_type                prot_subsc_chg,

        boolean                              hybr_gw_subs_chg,

        boolean                                   hybr3_gw_subs_chg,

        byte                                 nam,

        const mmoc_subs_data_s_type          *cdma_subs_ptr,
        /* CDMA subscription info */

        const mmoc_subs_data_s_type          *gw_subs_ptr,
           /* GW subscription info */

        const mmoc_subs_data_s_type          *gw_hybr_subs_ptr,
           /* GW HYBR subscription info */

        const mmoc_subs_data_s_type          *gw3_hybr_subs_ptr ,
           /* GW HYBR subscription info */
        
        uint8                                active_ss,

        sys_modem_device_mode_e_type          device_mode
);

/*===========================================================================

FUNCTION mmoc_cmd_dual_standby_chgd

DESCRIPTION
  This function will be used to command the MMoC to change dual standby
  preferences

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           mmoc_cmd_dual_standby_chgd
(
       sys_modem_dual_standby_pref_e_type   ds_pref,
       uint8                              active_ss,
       sys_modem_device_mode_e_type    device_mode
);


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_rpt_ph_stat_chgd_cnf2

DESCRIPTION
  This function should be used by the protocols to acknowledge the MMoC that
  the "phone status changed (due to either NAM change, Subscription
  availability change(GSM/WCDMA or CDMA/AMPS/HDR) or operating mode change
  request. This function will queue the phone status changed confirmation
  report to the MMoC task.

  Note! This should be called only after completing the necessary
  housekeeping actions based on the request.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           mmoc_rpt_ph_stat_chgd_cnf2
(

        prot_trans_type                trans_id,
          /* Protocols set this same as the trans_id received in the phone
          ** status changed ack. trans_id cannot be MMOC_TRANS_AUTONOMOUS.
          */

        mmoc_mode_e_type                 prot
          /* Protocol responding to the Phone status changed command.
          */


);

/**===========================================================================

@FUNCTION mmoc_rpt_ph_stat_chgd_cnf

@DESCRIPTION
  This function should be used by the protocols to acknowledge the MMoC that
  the "phone status changed (due to either NAM change, Subscription
  availability change(GSM/WCDMA or CDMA/AMPS/HDR) or operating mode change
  request. This function will queue the phone status changed confirmation
  report to the MMoC task.

  Note! This should be called only after completing the necessary
  housekeeping actions based on the request.

@DEPENDENCIES
  None.

@RETURN VALUE
  None.

@SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           mmoc_rpt_ph_stat_chgd_cnf
(

        prot_trans_type                trans_id,
          /**< Protocols set this same as the trans_id received in the phone
          ** status changed ack. trans_id cannot be MMOC_TRANS_AUTONOMOUS.
          */

          sd_mode_e_type                 prot,
      /**< Protocol responding to the Phone status changed command. */

          mmoc_ph_stat_e_type        req_status
      /**< status of the command - success or error code. */          

);
/*~ FUNCTION mmoc_rpt_ph_stat_chgd_cnf */

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_rpt_ds_stat_chgd_cnf

DESCRIPTION
  This function should be used by the protocols to acknowledge the MMoC that
  the "dual standby status changed.
  This function will queue the dual standby status changed confirmation
  report to the MMoC task.

  Note! This should be called only after completing the necessary
  housekeeping actions based on the request.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           mmoc_rpt_ds_stat_chgd_cnf
(

        prot_trans_type                trans_id
          /* Protocols set this same as the trans_id received in the phone
          ** status changed ack. trans_id cannot be MMOC_TRANS_AUTONOMOUS.
          */
);

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_rpt_ps_detach_cnf

DESCRIPTION
  This function should be used by the protocols to acknowledge the MMoC that
  the "PS detach is done".
  This function will queue the ps detach confirmation
  report to the MMoC task.

  Note! This should be called only after completing the necessary
  housekeeping actions based on the request.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void                           mmoc_rpt_ps_detach_cnf
(

        prot_trans_type                trans_id
          /* Protocols set this same as the trans_id received in the phone
          ** status changed ack. trans_id cannot be MMOC_TRANS_AUTONOMOUS.
          */
);

/*===========================================================================

FUNCTION mmoc_rpt_ue_mode_switch_cnf

DESCRIPTION
  This function should be used by the protocols to acknowledge the MMoC that
  the "PS detach is done".
  This function will queue the ps detach confirmation
  report to the MMoC task.

  Note! This should be called only after completing the necessary
  housekeeping actions based on the request.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void                           mmoc_rpt_ue_mode_switch_cnf
(
        sd_ss_e_type                   ss,

        prot_trans_type                trans_id,
          /* Protocols set this same as the trans_id received in the phone
          ** status changed ack. trans_id cannot be MMOC_TRANS_AUTONOMOUS.
          */
        boolean                            is_prot_deactiavted
        /*whether prot was deactivated as part of this  ue_mode_switch */
);

/*===========================================================================

FUNCTION mmoc_1X_rpt_ue_mode_switch_cnf

DESCRIPTION
  This function should be used by the protocols to acknowledge MMoC that,
  ue mode change processing has been completed. 
  This function will queue the 1X ue mode change cnf
  report to the MMoC task.

  Note! This should be called only after completing the necessary
  housekeeping actions based on the request.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void                           mmoc_1x_rpt_ue_mode_switch_cnf
(
  prot_trans_type                trans_id,
          /* Protocols set this same as the trans_id received in the ue mode
          **  changed ack. trans_id cannot be MMOC_TRANS_AUTONOMOUS.
          */
  sys_modem_as_id_e_type         asub_id

);

/*===========================================================================

FUNCTION mmoc_HDR_rpt_ue_mode_switch_cnf

DESCRIPTION
  This function should be used by the protocols to acknowledge MMoC that,
  ue mode change processing has been completed. 
  This function will queue the HDR ue mode change cnf
  report to the MMoC task.

  Note! This should be called only after completing the necessary
  housekeeping actions based on the request.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void                           mmoc_hdr_rpt_ue_mode_switch_cnf
(
  prot_trans_type                trans_id,
          /* Protocols set this same as the trans_id received in the ue mode
          ** changed ack. trans_id cannot be MMOC_TRANS_AUTONOMOUS.
          */
  sys_modem_as_id_e_type         asub_id

);


/** @}
**
*/

/**===========================================================================
=============================================================================
=============================================================================
============================== Protocols -> MMOC reports ====================
=============================================================================
=============================================================================
===========================================================================*/

/** @defgroup PMR Protocols->MMOC Reports
*  @{
*/

/* The following section defines the API when the clients ( example
** protocols ) have to communicate to MMoC using the report queue.
*/


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_rpt_activation_cnf

DESCRIPTION
  This function should be used by the protocols to acknowledge that Activation
  has been processed.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           mmoc_rpt_activation_cnf
(

        prot_state_e_type                prot_state,
          /* Protocol that sent activation cnf
          */

        sd_ss_e_type                     ss
          /* Stack on which activation is occuring
          */
);


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_rpt_ds_stat_chgd_cnf

DESCRIPTION
  This function should be used by the protocols to acknowledge the MMoC that
  the "dual standby status changed.
  This function will queue the dual standby status changed confirmation
  report to the MMoC task.

  Note! This should be called only after completing the necessary
  housekeeping actions based on the request.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           mmoc_rpt_ds_stat_chgd_cnf
(

        prot_trans_type                trans_id
          /* Protocols set this same as the trans_id received in the phone
          ** status changed ack. trans_id cannot be MMOC_TRANS_AUTONOMOUS.
          */
);

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_rpt_subs_cap_chgd_cnf

DESCRIPTION
  This function should be used by the protocols to acknowledge the MMoC that
  the "dual standby status changed.
  This function will queue the dual standby status changed confirmation
  report to the MMoC task.

  Note! This should be called only after completing the necessary
  housekeeping actions based on the request.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           mmoc_rpt_subs_cap_chgd_cnf
(

        prot_trans_type                trans_id
          /* Protocols set this same as the trans_id received in the phone
          ** status changed ack. trans_id cannot be MMOC_TRANS_AUTONOMOUS.
          */
);

/*===========================================================================

FUNCTION mmoc_rpt_prot_auto_deactd_ind_ss

DESCRIPTION
   This function will be used to inform the MMoC that the protocol has been
  deactivated autonomously. This function will queue the deactivated
  indication report to the MMoC task.

  Note! This function should not called in response to protocol deactivate
  request from MMoC.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           mmoc_rpt_prot_auto_deactd_ind_ss
(

        prot_deact_e_type              deactd_reason,
           /* Reason that the protocol was deactivated.
           */

        prot_state_e_type              prot_state,
           /* Protocol which sent the autonomous deactivation.
           */

        sd_ss_e_type                 ss

);

/**===========================================================================

@FUNCTION mmoc_rpt_csfb_prot_deactd_cnf

@DESCRIPTION
  This function will be used to confirm the MMoC that the csfb protocol has
  been deactivated. This function will queue the protocol deactivated report
  to the MMoC task.

  Note! This function should be called only in response to protocol
  deactivate request from MMoC.

@DEPENDENCIES
  None.

@RETURN VALUE
  None.

@SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           mmoc_rpt_1xcsfb_prot_deactd_cnf
(

        prot_trans_type                trans_id
           /**< Protocols set this same as the trans_id received in their
           ** Deactivate request.
           */

);
/** @}
**
*/
/*===========================================================================

FUNCTION mmoc_rpt_irat_hold_user_act_cnf

DESCRIPTION
  This function will be used to confirm that CM has marked to pend any user
  command till MMOC send UNHOLD_USER_ACT.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void                           mmoc_rpt_irat_hold_user_act_cnf
(
       boolean                        irat_ret_status
);

#endif /*MMOC_V_H*/

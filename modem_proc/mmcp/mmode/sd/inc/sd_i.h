#ifndef SD_I_H
#define SD_I_H
/*===========================================================================

               S Y S T E M   D E T E R M I N A T I O N

               I N T E R N A L   H E A D E R   F I L E

DESCRIPTION
  This header file contains definitions used internally by the System
  Determination, as well as, providing internal interface to the SD.C file.

  This header file should NOT be included by any source file outside the
  System Determination module.

  Copyright (c) 2013 by Qualcomm Technologies, Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary.

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/sd/inc/sd_i.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/01/13   jvo     Added MMode initialization interdependency check support
08/28/13   jvo     Cleanup remaining Fusion pieces
02/11/13   xs      SD send acq fail to CM during HDR silent redial
02/03/13   cl      FR1975: Dynamic Switch SVLTE <-> CSFB
07/18/12   jh      Added realign_lte flag to support LTEAvailFile feature
02/23/12   ak      SD Free floating changes
08/25/11   xs      Added EPS network feature support definition (PS voice indication)
07/24/11   aj      SC SVLTE 1.0 merge to mainline
07/18/11   ak      Disrupt MSM GW scan/service during MDM traffic.
06/16/11   ak      MDM to notify MSM LTE activity status for MMSS arbitration
05/18/11   cl      Added support for GW throttle filtering based on regions.
04/28/11   sv      Add geo system index to the si info
04/12/11   cl      Propagate SD stacks's states to CM
03/25/11   gm      LTE (Lmtd) to DO resel support
03/15/11   cl      Remove is_active indication from service indication
03/09/11   cl      Add support for SVLTE2 MMSS arbitration
02/22/11   rk      Adding  lte cs capability in API and passing it in srv ind
01/24/11   rm      Initial release.

===========================================================================*/

#include "mmcp_variation.h"
#include "comdef.h"
#include "sd_v.h"
#include "prl_v.h"

/* *****************************
** Forward variable declarations
** *****************************/

#ifdef SD_DEBUG
#error code not present
#endif

/** Type for a HDR system id
**
** Note! The order of fields in this structure is important in order to
** optimize the structure size (i.e. eliminate compiler padding) as this
** structure is used in the System Record Table (sdsr_tbl). Add new fields
** only at the end and try to order fields smartly to minimize compiler
** padding.
*/
typedef struct {
  /**< IS-856 sector ID - Identifies HDR systems
  */
  #ifdef FEATURE_HDR
  byte  sector_id[16];
  #else
  byte  sector_id[1];
  #endif

  /**< The subnet mask length of the Sector ID.
  */
  byte  subnet_mask_len;

} sd_hdr_sys_id_s_type;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/** Type for system ID.
*/
typedef union {

  struct {
    word sid;       /**< IS-95 SID */
    word nid;       /**< IS-95 NID */

    word true_sid;  /**< The "true" SID when sid/nid above store the
                    ** MCC/IMSII_11_12 */

    word true_nid;  /**< The "true" NID when sid/nid above store the
                    ** MCC/IMSII_11_12 */
    word mcc;       /**< MCC - Mobile County Code */
    byte imsi_11_12;/**< IMSI_11_12 */

  } is95;           /**< IS-95 system ID - Identifies CDMA, and AMPS systems */

  /**< HDR system id */
  sd_hdr_sys_id_s_type is856;

                    /**< PLMN that identifies a GSM/WCDMA systems. */
  sd_plmn_s_type  plmn;

  /**< A temporary solution to indicate both acquired system mode and plmn for
  ** GW systems.The plmn field in gw union exactly matches the plmn in plmn
  ** union.
  */
  struct {
    sd_plmn_s_type plmn;
    sd_mode_e_type mode;
  } gw;

} sd_sid_u_type;

typedef union sd_sid_band_u
{
  sd_ss_band_pref_e_type                              chgwt_band;
  /** Used with SYS_BAND_MASK_* band masks */

  sd_sys_rec_lte_band_pref_e_type                     lte_band;
  /** Used with SYS_BAND_MASK_LTE_* band masks */

} sd_sid_band_u_type;

/** Type for system ID except WLAN SSID.
*/
typedef union {

  struct {
    word sid;       /**< IS-95 SID */
    word nid;       /**< IS-95 NID */

    word true_sid;  /**< The "true" SID when sid/nid above store the
                    ** MCC/IMSII_11_12 */

    word true_nid;  /**< The "true" NID when sid/nid above store the
                    ** MCC/IMSII_11_12 */
    word mcc;       /**< MCC - Mobile County Code */
    byte imsi_11_12;/**< IMSI_11_12 */

  } is95;           /**< IS-95 system ID - Identifies CDMA, and AMPS systems */

  /**< HDR system id */
  sd_hdr_sys_id_s_type is856;

                    /**< PLMN that identifies a GSM/WCDMA systems. */
  sd_plmn_s_type  plmn;

  /**< indicate both acquired system mode , band and plmn for
  ** 3GPP systems.The plmn field in gw union exactly matches the plmn in plmn
  ** union.
  */
  struct {
    sd_plmn_s_type      plmn;
    sd_mode_e_type      mode;
    sd_sid_band_u_type          band;
  } gw;


} sd_sid2_u_type;


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/** Type for system ID, only SID and PLMN id.
*/
typedef struct {

  union
  {

    struct
    {
      word sid;       /**< IS-95 SID */
      word nid;       /**< IS-95 NID */
    } is95;           /**< IS-95 system ID - Identifies CDMA, and AMPS systems */

    struct
    {
      /**< PLMN Id that identifies a GSM/WCDMA systems.
      */
      sd_plmn_s_type        plmn;

      /**< Defines the relative priority among 3gpp systems
      ** This info is obtained from the 3gpp Band support table
      */
      byte                  num_rats;
      sys_sys_mode_e_type   gwl_pri_lst[SYS_MAX_NUM_3GPP_RATS];

    }gwl;


      /**< HDR system id */
  sd_hdr_sys_id_s_type is856;

  } prm;

  /**< Network selection mode type.
  */
  sd_net_select_mode_e_type net_select_mode;

  /* Subscription id / stack id for GW-G Dual Standby */
  sd_ss_e_type   gw_ss;


} sd_sid_plmn_s_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/** Type for Service Indication (SI). Provides information about the service
** that is associated with the acquired/candidate system.
*/
typedef struct {

  /** Indicates the Service status ( Limited, Full service etc.,).
  **
  ** NOTE! When the value of this field is NO_SRV the value of all the fields
  ** that follows this field is undefined.
  */
  sys_srv_status_e_type                srv_status;

  /** Indicate whether system determination is still attempting to acquire
  ** a more preferred service.
  */
  boolean                              is_pref_srv_acq;

  /** Indicate SD service stable on a system and not scanning at
  **  the moment.
  */
  boolean                              is_stable_in_svc;

  /** Indicate whether this service indication is due to reselecting HYBR HDR
  ** over MAIN (LMTD) service in HYBR BSR state
  */
  boolean                              is_hybr_bsr_acq;

  /** Indicate whether redirection or handoff indication is currently ON.
  */
  boolean                              is_redir_or_ho;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /** The mode of the acquired/candidate system.
  */
  sd_mode_e_type                       mode;

  /** Mode specific information - based on mode.
  */
  sd_si_mode_info_u_type               mode_info;

  /** The band class of the acquired/candidate system.
  */
  sd_band_e_type                       band;

  /** The CDMA Channel/AMPS A/B system of the acquired/candidate system.
  */
  sd_chan_type                         chan;

  /** The System ID of the acquired/candidate system.
  */
  sd_sid_u_type                        sid;

  /** The roaming indication of the acquired/candidate system.
  */
  sd_roam_e_type                       roam;

  /** Serving system's service capability.
  */
  sys_srv_capability_e_type            sys_srv_cap;

  /** PRL designation
  */
  byte                                 prl_desig;

  /** Is system prl match?
  */
  boolean                              is_sys_prl_match;

  /** Is system forbidden?
  */
  boolean                              is_sys_forbidden;

  /** Registered service domain.
  */
  sys_srv_domain_e_type                srv_domain;

  /** PRL association fields - Indicate whether this system is tag, PN and/or
  ** data associated.
  */
  boolean                              is_colloc;
  boolean                              is_pn_assn;
  boolean                              is_data_assn;

  /** System index of the first system in the current Geo.
  */
  uint16                               geo_sys_idx;

  /** User zone information associated with the acquired system.
  */
  sd_si_uz_info_s_type                 curr_uz_info;

  /** This flag indicates whether the HDR instance needs to be re-aligned,
  ** Would typically occur whenever the SID/NID, band or is_colloc field
  ** changes in SD's main instance.
  */
  boolean                              realign_hdr;

  /** This flag indicates whether the LTE instance needs to be re-aligned,
   ** this will occer when alt_bsr flag is on and sid/nid/bid is changed
   */
  boolean                              realign_lte;

  /** System usable status indicator.
  */
  sd_sys_usable_e_type                 usable_status;

  /** Broadcast/multicast data service availability
  ** This fields specify whether lower layer
  ** supports broadcast/multicast as per
  ** High Rate Broadcast-Multicast Packet data Air
  ** Interface Specification C.S0054 and
  ** BCMCS Framework Draft Document 0.1.3 XP0019
  */
  sys_bcmcs_srv_status_e_type          bcmcs_srv_status;

  /** The uptime the current service was acquired
  */
  dword                                srv_acq_time;

  /** Srv lost time
  */
  dword                                srv_lost_time;

  sys_simultaneous_mode_e_type       simultaneous_mode;

  /** The active HDR protocol revision
  */
  sys_active_prot_e_type               hdr_active_prot;

  /** The active HDR personality
  */
  sys_personality_e_type               hdr_personality;

  /* Indicates the CDMA/HDR time info
  */
  sys_time_info_s_type                 time_info;

  /* Indicates the CS domain service status ( Limited, Full service etc.)
  */
  sys_srv_status_e_type                cs_srv_status;

  /* Default roaming indicator from PRL
  */
  sys_roam_status_e_type               def_roam_ind;

  /* SIB8 status
  */
  sd_sib8_status                       sib8_available;

  /* When CDMA/HDR service is acquired, this flag will indicate if there are
  ** more preferred GWL systems in the pref_lst
  ** When LTE service is acquired, this flag will indicates if there are more
  ** CDMA systems in the more pref_lst
  */
  prl_gwl_preferred_e_type             is_more_pref_gwl_avail;

  /* Is acquition throttling enabled. This flag only populated to notify CM.
  */
  boolean                              is_hdr_activity;

  /** Indicate CS registation status on lte network >
  */
  sys_lte_cs_capability_e_type         lte_cs_capability;

  /* Is the stack active. By default set to TRUE.
  */
  boolean                              is_active;

  /* Indicate if IMS voice is supported */
  boolean                              lte_ims_voice_avail;

  /* Indicate if IMS emergency is supported */
  sys_ims_lte_emerg_supp_e_type        lte_ims_emerg_avail;
  
  sys_extend_srv_info_e_type           extend_srv_info;
  /*  Extended service info */
  /* Version number added for Free floating support.
  ** Version number is incremented when si_info is successfully updated.
  **
  ** Before updating the si_info the version numbers must match.
  ** Mismatch indicates some other process updated si_info and version which
  ** is currently being updated is stale.
  */
  uint32                               version_num;

  /* System selection preference related request ID.
  ** This Id was supplied by client in CM.
  */
  uint16                               sys_sel_pref_req_id;

  /* The service domain reported to CM client during SGLTE CAMPED_ONLY mode.
  */
  sys_reg_domain_e_type                 reg_domain;

  /* HDR/cdma MCC list drived from PLMN in eqprl
   */
  sys_plmn_mcc_list                     plmn_mcc_list;

  /* CSG Information
  */
  sys_csg_info_s_type                  csg_info;

  /* This applies only to CDMA and HDR */
  boolean                              is_custom_home;

  /* Flag to indicate whther LTE has been scanned atleast once
  ** after mode switch */
  boolean                              is_lte_scanned;

} sd_si_info_s_type;


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/** Type for Service Indicators callback @FUNCTION.
**
** The client (typically this is CM) that wants to be notified of service
** indicators changes, needs to register a @FUNCTION of this type with the
** SD Service Indicators component.
*/
typedef void (sd_si_info_f_type) (

    sd_ss_e_type              ss,
        /**< System selection - MAIN or HDR.
        */

    const sd_si_info_s_type   *si_info_ptr
        /**< A const pointer to a structure containing the current values of
        ** the service indicators
        */
);

/*Enumeration for the type of operators*/
typedef enum sd_operator_e {
/** @cond  */
  SD_OPERATOR_NULL = 0,   /* not custom design */
/** @endcond */

  SD_OPERATOR_CT,
  /**< custom design for China Telecom. */  
  
  SD_OPERATOR_CU,
  /**< custom design for China Unicom. */  

  SD_OPERATOR_CMCC,
   /**< custom design for China Mobile Corp. */

/** @cond  */
  SD_OPERATOR_MAX
/** @endcond */
} sd_operator_e_type;

/* <EJECT> */
/*===========================================================================

@FUNCTION sd_si_reg

@DESCRIPTION
  Register callback functions with the Service Indicators component.

  SD calls on the Service Indicators callback function whenever one or more
  of the service indicators changes their value.

  SD calls on the ok_to_orig callback function whenever the ok_to_orig status
  changes from FALSE to TRUE while the origination mode is other than none.

  SD calls on the emerg_cb_mode callback function whenever the emerg_cb_mode
  status changes from FALSE to TRUE.

  NOTE! A new call to this function overrides all the previously registered
  callback functions.

@DEPENDENCIES
  None.

@RETURN VALUE
  None.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  void                          sd_si_reg(

        sd_si_info_f_type             *si_info_cb_func,
            /**< Pointer to a Service Indicators callback function. SD calls on
            ** this function whenever one or more of the service indicators
            ** changes their value.
            **
            ** The argument to this function is a const pointer to a
            ** structure containing the current value of the service
            ** indicators */

        sd_si_acq_fail_f_type         *acq_fail_cb_func,
            /**< Pointer to an Acquisition Fail callback function. SD calls on
            ** this function whenever acquisition fails (i.e., SD have
            ** attempted full acquisitions on all systems with no success).
            */

        sd_si_ok_to_orig_f_type       *ok_to_orig_cb_func,
            /**< Pointer to an Ok To Orig callback function. SD calls on this
            ** function whenever the Ok To Orig status changes from FALSE to
            ** TRUE while the origination mode is other than none */

        sd_si_emerg_cb_mode_f_type    *emerg_cb_mode_cb_func,
            /**< Pointer to an Emergency Callback Mode callback function. SD
            ** calls on this function whenever the Emergency Callback Mode
            ** status changes from FALSE to TRUE */

        sd_si_autonam_change_f_type   *autonam_change_cb_func,
            /**< Pointer to an auto-NAM callback function. SD calls on this
            ** function whenever auto-NAM change is appropriate */

        sd_si_gw_net_list_f_type      *gw_net_list_cb_func,
            /**< Pointer to the GW network list call function. SD calls on
            ** this function once it receives the results of the get network
            ** list on GSM/WCDMA systems.
            */

        sd_si_sim_state_f_type        *sim_state_cb_func,
           /**< Pointer to the sim state call back function. Sd calls on this
           ** function if the SIM state was changed.
           */

        sd_si_reg_done_f_type         *reg_done_cb_func,
           /**< Pointer to the reg. done call back function. SD calls this when
           ** a reg. is done successfully.
           */

        sd_si_data_suspend_f_type     *data_suspend_cb_func,
           /**< Pointer to the data suspend call back function. SD calls this when
           ** a REG reports change in data suspend flag.
           */

        sd_si_srv_lost_f_type         *srv_lost_cb_func,
           /**< Pointer to the service lost indication call back. SD calls on
           ** this function on a service lost change
           */

        sd_si_prl_load_complete_f_type *si_prl_load_complete_cb_func,
           /**< Pointer to the prl init complete. Sd calls on this
           ** function once the prl is loaded.
           */

        sd_si_ss_stack_status_f_type   *ss_stack_status_cb_func,
           /**< Pointer to the stack status callback. SD calls on this
           ** function when the stack status is changed.
           */	

        sd_si_generic_data_f_type      *sd_generic_cb_func,
           /**< Pointer to generic callback function. SD Calls to update
           ** CM with generic data depending on th type.
           */

        sd_si_full_srv_lost_f_type      *full_srv_lost_cb_func,
           /**< Pointer to full service lost callback function. SD Calls on this function 
                  once full service is lost
           */

        sd_si_cdma_bsr_stat_chgd_f_type *cdma_bsr_stat_chgd_cb_func,
        /**< If SD is doing more pref acquisition from the acquired 1x system */

		sd_si_cs_emerg_scan_fail_f_type    *cs_emerg_scan_fail_cb_func
		/**<SD report for sending cs scan fail during e911*/

);

/* ^L<EJECT> */
/**===========================================================================

@FUNCTION sd_si_info_ptr_get

@DESCRIPTION
  This function is used to fetch the current SI info into the buffer provided.

@DEPENDENCIES
  None.

@RETURN VALUE
  sd_si_info_s_type* - pointer to current SI information.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  void                          sd_si_info_ptr_get(

        sd_ss_e_type                  ss,

        sd_si_info_s_type*            info

);

/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_hybr_2_act_get

DESCRIPTION
  Return the SS-Action that was most recently returned to the SS-Client
  other than CONTINUE.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

  At least one sd_ss_ind_xxx() SS-Indication function must have already been
  called.

RETURN VALUE
  The SS-Action that was most recently returned to the SS-Client.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_hybr_2_act_get(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
);

/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_hybr_3_act_get

DESCRIPTION
  Return the SS-Action that was most recently returned to the SS-Client
  other than CONTINUE.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

  At least one sd_ss_ind_xxx() SS-Indication function must have already been
  called.

RETURN VALUE
  The SS-Action that was most recently returned to the SS-Client.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_hybr_3_act_get(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
);

/* <EJECT> */
/*===========================================================================

FUNCTION prl_extract_sys_rec

DESCRIPTION
  Extract PRL system record from a specified memory location.

DEPENDENCIES
  None.

RETURN VALUE
  Offset (in bits) to next system record.

SIDE EFFECTS
  None.

===========================================================================*/
extern  dword                     prl_extract_sys_rec(

        byte                      *buf_ptr,
            /* Pointer to buffer containing PRL from which to extract the
            ** system record.
            */

        dword                      offset,
            /* Offset (in bits) to the beginning of system record which to
            ** extract.
            */

        prli_sys_rec_type         *sys_rec,
            /* Pointer to buffer where to deposit the extracted system
            ** record.
            */

        prl_sspr_p_rev_e_type    sspr_p_rev
            /* PRL SSPR_P_REV to indicate PRL or Extended PRL.
            */
);

/* <EJECT> */
/*===========================================================================

FUNCTION prl_sys_rec_get_sid

DESCRIPTION
  Extract the SID from a PRL system record.

DEPENDENCIES
  None.

RETURN VALUE
  Void.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      prl_sys_rec_get_sid(

        const prli_sys_rec_type   *sys_rec_ptr,
            /* Pointer to a system record from which to get the SID.
            */

        sd_sid2_u_type            *sid_ptr
            /* Pointer to buffer where to extract the SID.
            */
);

#endif /* SD_I_H */



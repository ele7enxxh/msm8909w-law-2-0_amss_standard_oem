/*===========================================================================
          ONLY INTERNAL FUNCTIONS TO sd.c and
          TO BE INCLUDED ONLY BY sd.c and sdcmd.c

GENERAL DESCRIPTION

  SD 3.0 marks the move from SD (System Determination) as a library called
  from the various components that use it to a task. This change stems from
  the more highly multi-threaded environment in which MMODE now runs -
  multiple simultaneous access to the SD database jeapordizes the integrity
  of that database. Attempts were made to handle this with critical sections,
  but there is just too much going on inside SD to safely take that approach.

  Instead, SD is now its own task, and protects access to its database by
  dispatching commands for each call of a frontend API. However, command
  dispatch is an inherently asynchronous process. Changing SD's APIs so that
  they were asynchronous was not an option, as this would have required major
  rewrites of all SD clients. Instead, the APIs exported by SD still behave
  as if they are synchronous, but instead of each API running to completion
  and then returning when it is called, the APIs now queue a command and
  then block the calling thread on a signal associated with that command
  until the SD task has executed the command. Only when the command has
  completed execution does the SD task set its associated signal, which
  unblocks the thread that called the API, allowing it to return to the
  caller.

  As a result of these changes, adding a frontend API to SD is now done
  differently than in the past - you can no longer just write a function and
  export the API. Instead, the actual API function needs to post a command to
  the SD task and then block on a signal, waiting for command completion.

Things to watch out for:

  Never call a frontend API from within the function that executes that API!!!

  These functions are run on the SD task thread; calling a frontend API from
  such a function will create a command, queue it to the SD command queue,
  and then block the SD task thread waiting for the command execution to
  complete. The SD task thread is now deadlocked, because it can't run to
  execute that newly queued command. SD will ERR_FATAL if a command is queued
  from the SD task thread, so if there is a unit test that exercises the code
  path you will find out when you run SUTE.

http://qwiki.qualcomm.com/qct-multimode/SD_3.0


  Copyright (c) 2000 - 2013 by Qualcomm Technologies, Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary.


             EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/28/13   jvo     Cleanup remaining Fusion pieces
07/19/13   jvo     Remove all MDM/MSM Fusion features
06/21/13   dk      added revision history comments section,
                   general description for the file, and copyright statement

============================================================================*/ 

#ifndef SD_INTERNAL_H
#define SD_INTERNAL_H

#include "sd.h"
#include "sdtask.h"
#include "sdsr.h"
#include "sdss.h"
#include "sd_v.h"
#include "sdi.h"
#include "sdprl.h"
#include "sddbg.h"
#include "sdlog.h"
#include "cm_gw.h"
#include "sdssscr.h"  /* External interface to sdssscr.c */

#include "sdnv.h"     /* External interface to sdnv.c */
#include "sddbg.h"    /* SD debug macros */
#include "sdlog.h"    /* SD20 logging */
#include "prl.h"      /* External interface to prl.c */
#include "prl_v.h"
#include "sdssscr.h"  /* External interface to sdssscr.c */
#include "sys_v.h"
#include "cai_v.h"
#include "puzl_v.h"

#include "time_svc.h" /* Interface to clock services */
#include "nv.h"       /* Interface to NV services */

#include "mmcp_variation.h"
#include "customer.h" /* Customer configuration file */

#ifndef FEATURE_CUSTMMODE_H
#error  Need to pull in custmmode.h!!!
#endif /* FEATURE_CUSTMMODE_H */

#include "comdef.h"   /* Definition for basic types and macros */
#include "msg.h"      /* Message output services */
#include "err.h"      /* Error and error fatal services */
#include "event.h"    /* Event reporting services */
#include "sys_v.h"
#include "sys.h"



/*=============================================================================*/
/* typedefs moved from sd.c*/
/*=============================================================================*/

typedef struct {
  SD_CMD_HDR;
  sd_ss_pref_reas_e_type              pref_reason;
  sd_ss_orig_mode_e_type              orig_mode;
  sd_ss_mode_pref_e_type              mode_pref;
  sd_ss_band_pref_e_type              band_pref;
  sys_lte_band_mask_e_type            band_pref_lte;
  sd_ss_band_pref_e_type              band_pref_tds;
  sd_ss_prl_pref_e_type               prl_pref;
  sd_ss_roam_pref_e_type              roam_pref;
  sd_ss_hybr_pref_e_type              hybr_pref;
  sd_band_e_type                      otasp_band;
  sd_blksys_e_type                    otasp_blksys;
  sd_ss_avoid_sys_e_type              avoid_type;
  dword                               avoid_time;
  const sys_manual_sys_info_s_type    *manual_sys_info_ptr;
  sd_ss_srv_domain_pref_e_type        domain_pref;
  sd_ss_acq_order_pref_e_type         acq_order_pref;
  sd_ss_pref_update_reason_e_type     pref_update_reason;
  sd_ss_mode_pref_e_type              user_mode_pref;
  const sd_rat_acq_order_s_type       *rat_acq_order_ptr;
  uint16                              req_id;
  sd_ss_pref_camp_mode_e_type         camp_mode;
  sys_csg_id_type                     csg_id;
  sys_radio_access_tech_e_type        csg_rat;
  sys_voice_domain_pref_e_type        voice_domain_pref;
} sd_ss_ind_user_ss_pref_cmd;


typedef struct {
  SD_CMD_HDR;
  int                           pos;
  sd_ss_mode_pref_e_type        *rtrn_mode_ptr;
  sd_ss_band_pref_e_type        *rtrn_band_ptr;
  sd_chan_type                  *rtrn_chan_ptr;
  sys_rat_pri_list_info_s_type  *rat_pri_list_ptr;
} sd_ss_meas_list_cmd;

/* Type for MCFG refresh cmd */
typedef struct
{
  SD_CMD_HDR;

  sys_modem_as_id_e_type     as_id;
  /* Store the as_id for refresh request  */

  uint32                     reference_id;
  /* Store the reference id sent by MCFG   */

} sd_ss_mcfg_refresh_cmd;

/* Type for holding the state variables that are associated with the system
** selection front-end.
*/
typedef struct {

  /* SS-Action buffer - records the SS-Action that was most recently returned
  ** to the SS-Client */
  sd_ss_act_s_type        act;
  sd_ss_act_s_type        prev_act;
  sd_ss_act_s_type        true_curr_act;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* SS-State indicator - indicates the current SS-State.
  */
  sdss_state_e_type       ss_state;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Acquisition list, position and mode - indicates the list and position
  ** most recently attempted acquisition, as well as, the acquisition mode.
  */
  sdsr_e_type             acq_list;
  int                     acq_pos;
  sdss_acq_mode_e_type    acq_mode;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Measurement ID - indicate the ID of the most recent channel strength
  ** measurement request. This ID is being incremented every time a channel
  ** strength measurement is submitted to Search. It is being consulted upon
  ** receipt of the measurement report to determine the validity of the
  ** report.
  */
  word                    meas_id;

  /* Measurement is done - indicate whether the most recent measurement
  ** request (i.e., the one that is identified by meas_id) is done being
  ** serviced by Search.
  */
  boolean                 meas_is_done;

  /* Measurement list is different - indicate whether the most recent
  ** requested measurement list is different than the previous requested
  ** measurement list.
  */
  boolean                 meas_list_is_diff;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Indicates if the get networks request was serviced or not.
  */
  boolean                 is_get_net_done;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Pointer to an SS-Timer function.
  **
  ** SD calls on this function to set/disable the SS-Timer. Upon SS-Timer
  ** expiration the SS-Client should call on the sd_ss_ind_misc_timer()
  ** SS-Indication function.
  **
  ** The argument to this function is the time in ms with which to set the
  ** SS-Timer. An input of 0 should disable the SS-Timer.
  */
  void                    (*timer_func_ptr) (int4  ms);

  /* SS-Timer - indicates the uptime in seconds when the SS-Timer is set to
  ** expire, at which time the SS-Client is responsible for calling on the
  ** sd_ss_ind_misc_timer() SS-Indication function.
  */
  dword                   ss_timer;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ///* Bad point update uptime - indicates the uptime in seconds when the bad
  //** point count was last updated.
  //*/
  //dword                   bad_point_update_uptime;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Return cause - indicate the RETURN_CAUSE value shown in IS-2000-5 Table
  ** 2.7.1.3.2.1-2 corresponding to the service redirection failure
  ** condition */
  byte                    return_cause;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Optimized GPS SS-Preference change flag - idicate whether optimized GPS
  ** SS-preference change was performed */
  boolean                 gps_is_opti_ss_pref;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Critical section lock for SS component, to detect SS-Indications from
  ** multiple tasks being processed simultaneously */
  boolean                 is_ss_locked;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Flag to start SD instance
  ** Reset: power up and after SD instance has been started.
  ** Set : when nam is changed or when hybrid preference is
  ** toggled from none->on */
  boolean                 is_start_instance;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Indicate the HDR activity bit mask */
  sd_ss_hdr_act_mask_e_type   hdr_act_mask;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Indicate the CDMA activity bit mask. MC can send two types of masks
  ** 1. IN_TRAFFIC: This MASK is set when 1x is in Voice, data or SMS 
  **                traffic.
  ** 2. IN_ACCESS:  This MASK is set when 1x is in access state 
  **  
  ** 3. The MASK is initialized to NONE at Power-Up
  */ 
  sd_ss_cdma_act_mask_e_type  cdma_act_mask;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      Registered PLMN. As of now, this global parameter is being used for CSG (Femto) related code changes only.
      This is populated from cmregprx when plmn_success or failed function is called in SD.
  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  sys_plmn_id_s_type rplmn;

} sd_ss_s_type;

/* Type for holding the update on stack searches
*/


/* Type for holding the information that is associated with the SD Service
** Indicator component */
typedef struct {

  /* Service Indicators Information buffer - Holds the information of the
  ** various service indicators */
  sd_si_info_s_type             si_info;

  /* Service update criteria - Indicate the minimum criteria for updating
  ** the service indicators */
  sd_si_update_e_type           si_update_criteria;

  /* Pointer to associated system selection item */
  sd_ss_s_type                  *ss_ptr;

  /* Associated service indicator system  */
  sdsr_e_type                   si_sys;

  /* Flag to indicate the data suspend state reported from REG */
  boolean                       data_suspend;

  /* Pointer to a Service Indicators callback function. SD calls on
  ** this function whenever one or more of the service indicators
  ** changes their value.
  **
  ** The argument to this function is a const pointer to a
  ** structure containing the current value of the service
  ** indicators.
  */
  sd_si_info_f_type             *si_info_cb_func;

  /* Pointer to an acq_fail callback function. SD calls on this function
  ** whenever acquisition fails (i.e., SD have attempted full acquisitions on
  ** all systems with no success),
  */
  sd_si_acq_fail_f_type         *acq_fail_cb_func;

  /* Pointer to an ok_to_orig callback function. SD calls on this
  ** function whenever ok_to_orig status changes from FALSE to
  ** TRUE while the origination mode is other than none.
  */
  sd_si_ok_to_orig_f_type       *ok_to_orig_cb_func;

  /* Pointer to a emergency callback mode function. SD calls on this function
  ** whenever the emergency callback mode status changes from FALSE to
  ** TRUE */
  sd_si_emerg_cb_mode_f_type    *emerg_cb_mode_cb_func;

  /* Pointer to an auto-NAM change function. SD calls on this function
  ** whenever auto-NAM change is appropriate */
  sd_si_autonam_change_f_type   *autonam_change_cb_func;

  /* Pointer to the GW network list call back function. SD calls on
  ** this function once it receives the results of the get network
  ** list on GSM/WCDMA/LTE systems */
  sd_si_gw_net_list_f_type      *gw_net_list_cb_func;

  /* Pointer to the SIM state change call back function. SD calls this
  ** function when there is a change in SIM state */
  sd_si_sim_state_f_type        *sim_state_cb_func;

  /* Pointer to the registration completed call back function. SD calls
  ** this when registration( including implicit) is done successfully */
  sd_si_reg_done_f_type         *reg_done_cb_func;

  /* Pointer to the data suspend flag call back function. SD calls
  ** this when REG reports a change in data suspend flag */
  sd_si_data_suspend_f_type     *data_suspend_cb_func;

  /**< Pointer to the service lost call function. SD calls on
  ** this function once the service lost status is changed */
  sd_si_srv_lost_f_type         *srv_lost_cb_func;

  /**< Pointer to the prl-init callback function. SD calls on
  ** this function once the prl is loaded */
  sd_si_prl_load_complete_f_type *si_prl_load_complete_cb_func;

  /**< Pointer to the stack status callback function. SD calls on
  ** this functions when its stack stack status is changed */
  sd_si_ss_stack_status_f_type  *ss_stack_status_cb_func;

  /**< Pointer to generic callback function. SD calls on this function
  ** to provide information to CM. It's generic data and interpreted
  ** based on the type of the data.
  */
  sd_si_generic_data_f_type     *sd_generic_cb_func;

  /**< Pointer to the service lost call function. SD calls on
  ** this function once the service lost status is changed */
  sd_si_full_srv_lost_f_type         *full_srv_lost_cb_func;

  /**< Pointer to the BSR status changed function. SD calls on
  ** this function once the BSR status has changed */
  sd_si_cdma_bsr_stat_chgd_f_type   *cdma_bsr_stat_chgd_cb_func;

  /**<SD report for sending cs scan fail during e911*/
  sd_si_cs_emerg_scan_fail_f_type    *cs_emerg_scan_fail_cb_func;

} sd_si_s_type;


/* Macros to lock the System selection component and unlock it */
#define SD_SS_LOCK( ss_ptr )                                               \
                          sd_rex_enter_crit_sect();                        \
                          if ( ss_ptr->is_ss_locked)                       \
                          {                                                \
                            SD_ERR_FATAL_0("SS already locked");   \
                          }                                                \
                          else                                             \
                          {                                                \
                            ss_ptr->is_ss_locked = TRUE;                   \
                          }                                                \
                          sd_rex_leave_crit_sect();
#define SD_SS_FREE( ss_ptr )  ( ss_ptr->is_ss_locked = FALSE )             \


typedef struct
{
  SD_CMD_HDR;

  int   pos;
  int2  meas_val;
} sd_ss_meas_list_rprt_cmd;


typedef struct
{
  SD_CMD_HDR;

  boolean           is_cdma_unlock;

} sd_ss_ind_user_pwr_up_cmd;


#if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)
typedef struct
{
  SD_CMD_HDR;

  sd_ss_mode_pref_e_type      mode_pref;
  sd_ss_band_pref_e_type      band_pref;
  sys_lte_band_mask_e_type    band_pref_lte;
  sd_ss_band_pref_e_type      band_pref_tds;
  sd_network_list_type_e_type list_type;
} sd_ss_ind_user_get_net_gw_ss_cmd;
#endif


#if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)

typedef struct
{
  SD_CMD_HDR;

  word                      sid;
  word                      nid;
  boolean                   is_from_sync;
  byte                      ltm_off;
  sys_daylt_savings_e_type  daylt_savings;
  byte                      leap_secs;
} sd_ss_ind_cdma_acq_schm_cmd;


typedef struct
{
  SD_CMD_HDR;

  sd_band_e_type            band;
  sd_chan_type              chan;
  word                      sid;
  word                      nid;
  boolean                   is_pending_gsrdm;
  word                      mcc;
  byte                      imsi_11_12;
  sys_srch_win_type         srch_win_n;
  sys_base_lat_type         base_lat;
  sys_base_long_type        base_long;
  sys_base_id_type          base_id;
  sd_ss_ovhd_status_e_type  ovhd_status;
  sys_reg_status_e_type     reg_status;
  byte                      reg_prd;
  uint16                    height;
  uint8                     loc_unc_h;
  uint8                     loc_unc_v;
} sd_ss_ind_cdma_opr_oh_info_cmd;


typedef struct
{
  SD_CMD_HDR;

  sd_ss_sys_lost_e_type sys_lost;
} sd_ss_ind_cdma_opr_sys_lost_cmd;


typedef struct
{
  SD_CMD_HDR;

  boolean                   is_rif;
  sd_ss_redir_e_type        redir_type;
  const sd_redir_rec_s_type *rec_list_ptr;
  int                       num_rec;
} sd_ss_ind_cdma_opr_redir_cmd;


#if defined(FEATURE_LTE_TO_1X)
typedef struct
{
  SD_CMD_HDR;

  sd_ss_ho_e_type               ho_type;
  const sd_ho_sys_param_s_type  *ho_sys_param;
} sd_ss_ind_cdma_opr_ho_lte_cmd;
#endif /* defined(FEATURE_LTE_TO_1X) */


typedef struct
{
  SD_CMD_HDR;

  const sd_cdma_sib8_nbr_cell_info_list_s *sib8_nbr_cell_info_ptr;
} sd_ss_ind_update_cdma_sib8_nbrs_cmd;


typedef struct
{
  SD_CMD_HDR;

  sd_band_e_type  band;
  sd_chan_type    chan;
} sd_ss_ind_cdma_opr_new_sys_cmd;


typedef struct
{
  SD_CMD_HDR;

  const sd_sys_s_type *sys_ptr;
  int                 sys_num;
} sd_ss_ind_cdma_opr_sys_resel_cmd;


typedef struct
{
  SD_CMD_HDR;

  sd_ss_acc_reas_e_type acc_reas;
  sd_ss_acc_term_e_type acc_term;
} sd_ss_ind_cdma_opr_acc_term_cmd;


typedef struct
{
  SD_CMD_HDR;

  sd_ss_ho_e_type ho_type;
  sd_band_e_type  band;
  sd_chan_type    chan;
} sd_ss_ind_cdma_opr_cam_cdma_cmd;


typedef struct
{
  SD_CMD_HDR;

  sd_ss_ho_e_type     ho_type;
  sd_cell_sys_e_type  cell_sys;
  word                voice_chan;
  word                sid;
} sd_ss_ind_cdma_opr_cam_amps_cmd;


typedef struct
{
  SD_CMD_HDR;

  sd_band_e_type  band;
  word            chan;
} sd_ss_ind_cdma_opr_hdm_cdma_cmd;


typedef struct
{
  SD_CMD_HDR;

  word  sid;
  word  nid;
} sd_ss_ind_cdma_opr_itspm_cmd;


#ifdef FEATURE_MMODE_LTE_RESEL
typedef struct
{
  SD_CMD_HDR;

  boolean is_resel_allowed;

} sd_ss_ind_misc_irat_to_gwl_cmd;
#endif /* FEATURE_MMODE_LTE_RESEL */


typedef struct
{
  SD_CMD_HDR;

  sd_ss_redir_e_type        redir_type;
  const sd_redir_rec_s_type *rec_list_ptr;
  int                       num_rec;
} sd_ss_ind_user_cdma_irat_lte_cmd;

typedef struct
{
  SD_CMD_HDR;

  sd_ss_cdma_act_mask_e_type cdma_act_mask;
} sd_ss_ind_cdma_opr_activity_cmd;


#endif  /* defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900) */


typedef struct
{
  SD_CMD_HDR;

} sd_ss_ind_misc_service_transfer_cmd;


typedef struct
{
  SD_CMD_HDR;

  word    meas_id;
  boolean resel_is_allowed;
} sd_ss_ind_misc_meas_rprt_cmd;


typedef struct
{
  SD_CMD_HDR;

  sys_sim_state_e_type   sim_state;
} sd_ss_ind_misc_gw_sim_state_update_cmd;


#ifdef FEATURE_HDR
typedef struct
{
  SD_CMD_HDR;

  sd_ss_sys_get_f_type      *sys_get_func;
  void                      *data_ptr;
  sd_ss_redir_e_type        redir_type;
} sd_ss_ind_hdr_user_redir_lte_cmd;


typedef struct
{
  SD_CMD_HDR;

  const byte              *sid_ptr;
  byte                    subnet_mask_len;
  const sys_channel_type  *chan_ptr;
  word                    num_chan;
  word                    mcc;
} sd_ss_ind_hdr_acq_sid_cmd;


typedef struct
{
  SD_CMD_HDR;

  sd_band_e_type          band;
  sd_chan_type            chan;
  const byte              *sid_ptr;
  byte                    subnet_mask_len;
  boolean                 is_ok_to_orig;
  const sys_channel_type  *chan_ptr;
  word                    num_chan;
  sys_active_prot_e_type  hdr_active_prot;
  sys_personality_e_type  hdr_personality;
  int16                   local_time_offset;
  uint8                   leap_secs;
  uint16                  hdr_mcc;
  int32                   latitude;		
  int32                   longitude;	
  uint8                   system_type;
} sd_ss_ind_hdr_opr_oh_info_cmd;


typedef struct
{
  SD_CMD_HDR;

  sd_sys_usable_e_type  usable_status;
} sd_ss_ind_hdr_opr_sys_usable_cmd;


typedef struct
{
  SD_CMD_HDR;

  sd_ss_sys_get_f_type  *sys_get_func;
  void                  *data_ptr;
  sd_ss_redir_e_type    redir_type;
} sd_ss_ind_hdr_opr_redir_cmd;


typedef struct
{
  SD_CMD_HDR;

  sd_ss_sys_lost_e_type sys_lost;
} sd_ss_ind_hdr_opr_sys_lost_cmd;


typedef struct
{
  SD_CMD_HDR;

  sd_ss_acc_term_e_type acc_term;
} sd_ss_ind_hdr_opr_acc_term_cmd;


typedef struct
{
  SD_CMD_HDR;

  sd_ss_ses_close_e_type  ses_close;
} sd_ss_ind_hdr_opr_ses_close_cmd;


typedef struct
{
  SD_CMD_HDR;

  sd_ss_bad_sys_e_type  reas;
} sd_ss_ind_hdr_opr_bad_sys_cmd;


typedef struct
{
  SD_CMD_HDR;

  sd_ss_hdr_act_mask_e_type hdr_act_mask;
} sd_ss_ind_hdr_opr_activity_cmd;

#endif /* FEATURE_HDR */


typedef struct
{
  SD_CMD_HDR;

  boolean resel_is_allowed;
} sd_ss_ind_misc_timer_cmd;


#if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) || defined(FEATURE_SD_LTE)
typedef struct
{
  SD_CMD_HDR;

  sys_sim_state_e_type          sim_state;
  boolean                       data_suspend;
  sd_ss_gwl_scan_status_e_type  scan_status;
  sys_acq_status_e_type         acq_status;
  sys_lte_cs_capability_e_type  lte_cs_capability;
  uint16                        req_id;
  sys_extend_srv_info_e_type    extend_srv_info;
  const sys_csg_info_s_type*   csg_info_ptr;
  sys_plmn_id_s_type*        registered_plmn;
} sd_ss_ind_gw_acq_plmn_failed_cmd;


typedef struct
{
  SD_CMD_HDR;

  sys_sim_state_e_type        sim_state;
  boolean                     data_suspend;
  uint32                      suitable_search_time;
  sys_extend_srv_info_e_type  extend_srv_info;
  sys_acq_status_e_type       acq_status;
  sys_plmn_id_s_type*         registered_plmn;
} sd_ss_ind_gw_opr_srv_lost_cmd;


typedef struct
{
  SD_CMD_HDR;

  sys_sim_state_e_type      sim_state;
  boolean                   data_suspend;
  uint32                    connected_time;
  sys_plmn_id_s_type        *registered_plmn;
  sys_acq_status_e_type       acq_status;
} sd_ss_ind_lte_opr_srv_lost_cmd;


typedef struct
{
  SD_CMD_HDR;

  sys_plmn_id_s_type            plmn;
  sd_si_gwl_mode_info_s_type    lac_tac_rac;
  sd_band_u_type                band_pref;
  sys_chan_type                 chan;
  sys_srv_capability_e_type     plmn_srv_cap;
  boolean                       is_plmn_forbidden;
  sys_sim_state_e_type          sim_state;
  sys_srv_status_e_type         service_status;
  sys_roam_status_e_type        roaming_ind;
  sys_srv_domain_e_type         srv_domain;
  sd_mode_e_type                mode;
  boolean                       data_suspend;
  sys_srv_status_e_type         cs_service_status;
  sys_lte_cs_capability_e_type  lte_cs_capability;
  sd_sib8_status                sib8_available;
  boolean                       lte_ims_voice_avail;
  boolean                       lte_ims_emerg_avail;
  uint16                        req_id;
  sys_extend_srv_info_e_type    extend_srv_info;
  sys_reg_domain_e_type         reg_domain;
  const sys_csg_info_s_type*    csg_info_ptr;
  sys_plmn_id_s_type*           registered_plmn;
} sd_ss_ind_gw_srv_info_cmd;


typedef struct
{
  SD_CMD_HDR;

  sd_mode_e_type      mode;
  sd_band_u_type      band_pref;
  sys_plmn_id_s_type  plmn;
  sys_lac_type        lac_id;
} sd_ss_ind_gw_acq_plmn_camped_cmd;


#endif /* defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) || defined(FEATURE_SD_LTE) */


#if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)
typedef struct
{
  SD_CMD_HDR;

  sys_plmn_id_s_type            *rplmn_ptr;
  sys_found_plmn_list_u_type *plmn_list_ptr;
  sys_plmn_list_status_e_type   plmn_list_status;
} sd_ss_ind_misc_gw_net_list_rpt_ss_cmd;
#endif /* defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) */


#ifdef FEATURE_HDR
typedef struct
{
  SD_CMD_HDR;

  sys_bcmcs_srv_status_e_type bcmcs_srv_status;
} sd_ss_ind_misc_hdr_bcmcs_srv_status_cmd;
#endif /* FEATURE_HDR */


typedef struct
{
  SD_CMD_HDR;

  sd_acq_mode_select_e_type is_acq_sys_needed;
} sd_misc_get_curr_sys_mode_cmd;


typedef struct
{
  SD_CMD_HDR;

  sd_mode_e_type                  mode;
  sys_avoided_channel_record_type *avoid_list_ptr;
  byte                            *num_sys;
  byte                            max_entry;
} sd_misc_sys_avoid_list_cmd;


#if defined (FEATURE_LTE) && defined(FEATURE_LTE_TO_1X)
typedef struct
{
  SD_CMD_HDR;

  word  sid;
  word  nid;
} sd_misc_allowed_sid_nid_cmd;
#endif /* defined (FEATURE_LTE) && defined(FEATURE_LTE_TO_1X) */


typedef struct
{
  SD_CMD_HDR;

  sd_nam_e_type                     curr_nam;
  sd_ss_orig_mode_e_type            orig_mode;
  sd_ss_mode_pref_e_type            mode_pref;
  sd_ss_band_pref_e_type            band_pref;
  sys_lte_band_mask_e_type          lte_band_pref;
  sd_ss_band_pref_e_type            tds_band_pref;
  sd_ss_prl_pref_e_type             prl_pref;
  sd_ss_roam_pref_e_type            roam_pref;
  sd_ss_hybr_pref_e_type            hybr_pref;
  const sys_manual_sys_info_s_type  *manual_sys_info_ptr;
  sd_ss_srv_domain_pref_e_type      domain_pref;
  sd_ss_acq_order_pref_e_type       acq_order_pref;
  boolean                           is_gwl_subsc_avail;
  boolean                           is_1x_subsc_avail;
  boolean                           is_gw2_subsc_avail;
  boolean                           is_gw3_subsc_avail;
} sd_nam_sel_cmd;


typedef struct
{
  SD_CMD_HDR;

  sys_plmn_id_s_type    plmn_id;
  sd_mode_e_type        mode;
} sd_is_plmn_rat_allowed_cmd;


typedef struct
{
  SD_CMD_HDR;

  sd_ss_prot_deact_adtnl_info_e_type adtnl_info;
} sd_ss_ind_user_prot_deactivate_cmd;

typedef struct
{
  SD_CMD_HDR;

  sd_splmn_info_list_s *splmn_info_ptr;
} sd_ss_ind_update_splmn_list_cmd;


/*=============================================================================*/
/* static functions moved from sd.c */
/*=============================================================================*/

sd_ss_s_type*             sd_ss_ptr(
        sd_ss_e_type              ss
        /* System selection item */
);


sd_si_s_type*             sd_si_ptr(
        sd_ss_e_type              ss
        /* System selection - MAIN or HDR */
);


void                      sd_si_ok_to_orig(

        sd_ss_e_type              ss,
            /* System selection - MAIN or HDR */

        boolean                   data_suspend
            /* Data suspend status  */
);


void                       sd_update_manual_sys(

        sdsr_e_type                         lst,
            /* List to be updated */

        const sys_manual_sys_info_s_type    *man_sys_info_ptr
            /* Pointer to the manual system */

);


sd_ss_act_e_type          sd_ss_event_proc(

        sd_ss_e_type              ss,
            /* The ss to use for the event */

        sdss_evt_e_type           event,
            /* SS-Event to process */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action */
);


void                          sd_ss_read_acq_prms(

        sd_ss_e_type                  ss,
            /* The SS for which the acq is to be updated */

        sdsr_e_type                   *list,
            /* List from which acquisition is attempted */
        int                           *pos,
            /* List position of system that is attempted acquisition */

        sdss_acq_mode_e_type          *acq_mode
            /* The acquisition mode that is attempted */
);


EXTERN  sd_ss_act_e_type          sd_ss_act_get_true_curr_act_ss(

        sd_ss_e_type              ss,

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action */
);


EXTERN sdss_state_e_type          sd_get_ss_state(
       sd_ss_e_type               ss
);


EXTERN void                       sd_set_ss_state(
       sd_ss_e_type               ss,
       sdss_state_e_type          ss_state
);



void                      sd_get_band_mode_mmode(

        sd_ss_mode_pref_e_type    *mode_ptr,
            /* Pointer to the mode preference that needs to be filtered */

        sd_ss_band_pref_e_type    *band_ptr,
            /* 3GPP band preference that needs to be filtered */

        sys_lte_band_mask_e_type  *band_ptr_lte,
            /* 4G LTE band preference that needs to be filtered */

        sd_ss_band_pref_e_type    *band_ptr_tds,
            /* TD-SCDMA band preference that needs to be filtered */

        sd_ss_mode_pref_e_type    sys_mode_pref,
            /* Enumeration of system mode perference */

        sd_ss_band_pref_e_type    sys_band_pref,
        /* 3GPP Enumeration of system selection band and sub-band preferences */

        sys_lte_band_mask_e_type  sys_band_pref_lte,
        /* 4G LTE Enumeration of system selection band and sub-band preferences */

        sd_ss_band_pref_e_type    sys_band_pref_tds
        /* TD-SCDMA Enumeration of system selection band and sub-band preferences */
);


EXTERN  void          sd_ss_act_set_true_curr_act_ss(

        sd_ss_e_type              ss,

        const sd_ss_act_s_type   *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action */
);


EXTERN  sd_ss_act_e_type          sd_ss_hybr_1_act_get_true_curr_act(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action */
);


EXTERN  sd_ss_act_e_type          sd_ss_hybr_2_act_get_true_curr_act(

        sd_ss_act_s_type          *rtrn_act_ptr ,
            /* Pointer to a buffer where to place returned SS-Action */
        sd_ss_e_type                ss
);



EXTERN void                           sd_si_info_ptr_set(

       sd_ss_e_type                   ss,

       const sd_si_info_s_type*       info
);


EXTERN void                           sd_invoke_info_callback(

       sd_ss_e_type                   ss
          /* System selection stack */
);


sdlog_ss_act_e_type       sd_log_map_ss_act(
        sd_ss_act_e_type          ss_act
          /* SD SS-Action to be mapped */
);


sd_log_state_e_type       sd_log_state_get(

        sd_ss_e_type              ss
            /* System selection - MAIN or HDR */
);


boolean          sd_ss_act_is_equal(

        const sd_ss_act_s_type    *act_a_ptr,
            /* Pointer to an SS-Action to be compared */

        const sd_ss_act_s_type    *act_b_ptr,
            /* Pointer to an SS-Action to be compared */

        boolean                   meas_list_is_diff,
            /* Indicate whether the most recent list of systems to be
            ** measured is different than the previous list of systems to be
            ** measured */

        sd_ss_e_type              ss
);


void                         sd_si_srv_offline(

        sdss_opr_mode_e_type          opr_mode
            /* Offline mode (CDMA, AMPS, HDR, etc) */
);


#if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)
void                      sd_si_update_sim_state(

        sys_sim_state_e_type      sim_state,
           /* New SIM state reported from REG.
           */

        sd_ss_e_type                ss
           /* ss to update
           */
);
void                      sd_si_update_data_suspend_state(

        sd_ss_e_type              ss,
           /* The ss whose data suspend is to be updated */

        boolean                   data_suspend,
           /* New data suspend state reported from REG */

        sys_srv_status_e_type     srv_status
);

#endif /* defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) */


#if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) || defined(FEATURE_SD_LTE)

EXTERN boolean                    sd_misc_gwl_is_sys_lost_map_acq_fail(
      sd_ss_e_type               ss
);


sd_ss_act_e_type                  sd_misc_gwl_map_sys_lost_to_acq_fail
(
        sd_ss_e_type              ss,
            /* SD Stack on which to process */

        sys_sim_state_e_type      sim_state,
            /* SIM state */

        boolean                   data_suspend,
            /* Data suspend flag from REG */

        sd_ss_act_s_type          *rtrn_act_ptr,
            /* Pointer to a buffer where to place returned SS-Action */

		sys_acq_status_e_type     acq_status
		    /* acq_status returned in srv_lost report  */
);

#endif /* defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) || defined(FEATURE_SD_LTE) */


boolean                   sd_si_is_srv_on(
        sd_ss_e_type              ss
        /* System selection - MAIN or HDR */
);


void                          sd_ss_idle_sys_update(

        sd_ss_e_type                  ss,
            /* The ss for which the idle system is to be updated */

        sdsr_e_type                   sys,
            /* System per which to update the idle system.
            ** Ignored if set to SDSR_MAX */

        sdsr_e_type                   geo_list,
            /* List per which to update the GEO list  */

        sdsr_e_type                   pref_list
            /* List per which to update the PREF list */
);


void                          sd_ss_srv_sys_update(

        sd_ss_e_type                  ss,
            /* The ss for which the srv_sys is to be updated */

        sdsr_e_type                   sys,
            /* System per which to update the serving system
            ** Ignored if set to SDSR_MAX  */

        sdsr_e_type                   geo_list,
            /* List per which to update the serving system's GEO */

        sdsr_e_type                   pref_list,
            /* List per which to update the serving system's more preferred
            ** list */

        sdsr_e_type                   colloc_list
            /* List per which to update the serving system's collocated list */
);


void                      sd_si_update_if_srv_is_off(

        sd_ss_e_type              ss,
            /* System selection - MAIN or HDR
            */

        sd_si_update_e_type       update_criteria,
            /* Update criteria.
            */

        sys_srv_status_e_type     srv_status,
            /* Current service status ( LIMITED, FULL service etc ).
            */

        sys_srv_domain_e_type     srv_domain,
            /* Service domain.
            */


        sdsr_ref_type             sr_ref,
            /* System record reference according to which to update the
            ** system indicators.
            */

        const sd_mode_e_type      *mode_ptr,
           /* Pointer to mode - ignored if NULL.
           */

        const sd_band_e_type      *band_ptr,
            /* Pointer to band - ignored if NULL.
            */

        const sd_chan_type        *chan_ptr,
             /* Pointer to channel - ignored if NULL.
             */

        sdsr_e_type               colloc_list,
            /* collocated list.
            */

        const sys_active_prot_e_type *hdr_active_prot_ptr,
            /* HDR active protocol type.
            */

        const sys_personality_e_type    *hdr_personality_ptr,
            /* The HDR active personality
            */

        const sdss_sys_addn_info_s_type *curr_sys_info,
            /* Association tag of the currently acquired
            ** system, if any.
            */

        sys_srv_status_e_type     cs_srv_status,
            /* Current CS service status.
            */

        sd_sib8_status                   sib8_available,
        /** <sib8 status> */

        sys_extend_srv_info_e_type        extend_srv_info,
            /* Extended service info
            */

        sys_reg_domain_e_type           reg_domain,
            /* The REG domain value is used in CAMPED_ONLY mode by the
            ** CM client to determine service availability.
            */


        sd_si_mode_info_u_type    *mode_info_ptr
          /* Mode specific information
          */     
);


void    sd_si_update(
           /*lint -esym(550, is_uz_check) */ /* para not accessed */
      sd_ss_e_type                    ss,
          /* System selection - MAIN or HDR */

      sd_si_update_e_type             update_criteria,
          /* Update criteria */

      sys_srv_status_e_type           srv_status,
          /* Current service status ( LIMITED, FULL service etc ) */

      sys_srv_domain_e_type           srv_domain,
          /* Registered service domain */

      sdsr_ref_type                   sr_ref,
          /* System record reference according to which to update the
          ** system indicators */

      boolean                         is_uz_check,
          /* Flag to indicate if User zone selected
          ** check needs to be done */

      const sd_mode_e_type            *mode_ptr,
         /* Pointer to mode - ignored if NULL */

      const sd_band_e_type            *band_ptr,
          /* Pointer to band - ignored if NULL */

      const sd_chan_type              *chan_ptr,
           /* Pointer to channel - ignored if NULL */

      const sd_si_mode_info_u_type    *mode_info_ptr,
          /* Pointer to mode info */

      sdsr_e_type                     colloc_lst,
          /* The collocated list */

      const sys_active_prot_e_type    *hdr_active_prot_ptr,
          /* The HDR active protocol revision */

      const sys_personality_e_type    *hdr_personality_ptr,
          /* The HDR active personality */

      const sdss_sys_addn_info_s_type  *curr_sys_info,
          /* Association tag of the currently acquired
          ** system, if any */

      sys_srv_status_e_type           cs_srv_status,
          /* Current CS service status */

      sys_lte_cs_capability_e_type    lte_cs_capability,
          /* LTE CS capability */

      sd_sib8_status                  sib8_available,
          /*sib8 status */

      boolean                         lte_ims_voice_avail,
          /* If IMS voice supported */

      sys_ims_lte_emerg_supp_e_type   lte_ims_emerg_avail,
          /* If IMS emergency call is supported over LTE */

      uint16                            req_id,
        /* Request Id to be populated back to client in case required for SS events */

      sys_extend_srv_info_e_type   extend_srv_info,
          /* Extended service info */

      sys_reg_domain_e_type           reg_domain,
        /* The REG domain value is used in CAMPED_ONLY mode by the
        ** CM client to determine service availability */

       /*lint -esym(715, csg_info_ptr) */ /* param not referenced */
      const sys_csg_info_s_type*      csg_info_ptr,
        /* CSG Information */

      /* Indicates if new srv needs to be informed to CM*/
      boolean                         is_silent

);

sys_sys_mode_e_type       sd_misc_map_sd_mode_to_sys_mode(

        sd_mode_e_type            sd_mode
        /* Sd mode
        */
);


EXTERN  sd_ss_act_e_type          sd_ss_act_get_curr_act_ss(
        sd_ss_e_type              ss,
        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action */
);


EXTERN  sd_ss_act_e_type          sd_ss_act_get_true_curr_act(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
);


#if (defined (FEATURE_HDR_HYBRID))
void                          sd_ss_hdr_acq_update_post(

        sdsr_e_type                   list,
            /* List from which acquisition was attempted.
            */
        int                           pos,
            /* List position of system that was attempted acquisition.
            */

        sdss_acq_mode_e_type          acq_mode
            /* The acquisition mode that was attempted.
            */
);
#endif /* FEATURE_HDR_HYBRID */


#ifdef FEATURE_GPSONE_MSBASED
boolean                   sd_ss_pref_opti_gps(

        sd_ss_e_type              ss,
            /* Pointer to system selection.
            */

        sd_ss_mode_pref_e_type    mode_pref,
            /* New mode preference.
            */

        sd_ss_orig_mode_e_type    orig_mode,
            /* New origination mode.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
);
#endif /* FEATURE_GPSONE_MSBASED */


#if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)

void                      sd_si_update_cdma_time(

        sd_ss_e_type              ss,
        /**< System selection - MAIN */

        uint8                     ltm_offset,
        /**< Ltm offset on acquired CDMA system */

        sys_daylt_savings_e_type  daylt_savings,
        /**< Daylight savings on acquired CDMA system */

        byte                      leap_secs
        /**< Leap seconds */
);


void                      sd_ss_redir_rec_store(

        sdsr_e_type               sdsr_list,
            /* Pointed to the sdsr list to store the redirection record.
            */

        const sd_redir_rec_s_type *redir_rec_ptr,
            /* Pointer to a list of redirection records to which the MS is
            ** being redirected.
            **
            ** The list contains one or more redirection record. Each
            ** redirection record specifies the redirection system that is
            ** either CDMA or AMPS system.
            */

        int                       num_rec
            /* number of records in the redirection record list.
            */
);


int                       sd_ss_redir_rec_get_num_chan(

        const sd_redir_rec_s_type *redir_rec_ptr,
            /* Pointer to a list of redirection records to which the MS is
            ** being redirected.
            **
            ** The list contains one or more redirection record. Each
            ** redirection record specifies the redirection system that is
            ** either CDMA or AMPS system.
            */

        int                       num_rec
            /* number of records in the redirection record list.
            */
);


#ifdef FEATURE_ACP
void                      sd_ss_redir_rec_amps_store(

        sdsr_e_type               sdsr_list,
            /* Pointed to the sdsr list to store the redirection record.
            */

        const sd_redir_rec_amps_s_type  *redir_rec_amps_ptr,
            /* Pointer to a analog redirection record to which the MS is
            ** being redirected.
            */

        boolean                   is_delete_tmsi
            /* Indicate if MS is required to delete the TMSI assigned.
            */
);
#endif/* FEATURE_ACP */


void                      sd_ss_redir_rec_cdma_store(

        sdsr_e_type               sdsr_list,
            /* Pointed to the sdsr list to store the redirection record.
            */

        const sd_redir_rec_cdma_s_type  *redir_rec_cdma_ptr,
            /* Pointer to a analog redirection record to which the MS is
            ** being redirected.
            */
        boolean                   is_delete_tmsi
            /* Indicate if MS is required to delete the TMSI assigned.
            */

);


#if defined(FEATURE_LTE_TO_1X)
void                      sd_ss_ho_sys_param_store(

        sdsr_e_type               sdsr_list,
            /* Pointed to the sdsr list to store the redirection record.
            */

        const sd_ho_sys_param_s_type *ho_system_pram_ptr
            /* Pointer to a analog redirection record to which the MS is
            ** being redirected.
            */
);
#endif /* FEATURE_LTE_TO_1X */

#endif /* defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900) */


#ifdef FEATURE_HDR

void                        sd_ss_chan_list_update(

        sdsr_e_type                 chan_list,
           /* The output list to store the channel list.
           */

        sd_mode_e_type              mode,
           /* The system mode of the chan list.
           */

        const sys_channel_type      *chan_ptr,
           /* Pointer to HDR hashing channel records.
           */

        word                        num_chan,
           /* The number of HDR hashing channel records specified in
           ** chan_ptr
           */

        sdsr_e_type                 curr_sys,
           /* Current system information used if channel ptr is null.
           */

        const sd_sid2_u_type        *sid_ptr,
             /* pointer to ID.
             */

        sdsr_e_type                 list,
           /* Store the channels exists in the list if provided.
           */

        sdsr_e_type                 acq_sys,
           /* Order channel list according to the acq system if provided.
           */

        sdsr_e_type                 srv_sys
           /* Order channel list according to the srv system if provided.
           */
);


void                          sd_ss_hdr_srv_sys_update(

        sdsr_e_type                   sys,
            /* System per which to update the HDR serving system.
            ** Ignored if set to SDSR_MAX.
            */

        sdsr_e_type                   geo_list,
            /* List per which to update the HDR serving system's GEO.
            */

        sdsr_e_type                   pref_list,
            /* List per which to update the HDR serving system's more
            ** preferred list.
            */

        sdsr_e_type                   colloc_list
            /* List per which to update the serving system's collocated list.
            */
);


void                      sd_ss_idle_cand_sys_update(
        sdsr_e_type               cand_sys,
            /* candidate system to be updated.
            */

        sd_ss_e_type              ss,
            /* System selection enum type
            */

        sdsr_e_type               chan_list,
            /* Input channel list
            */

        sdsr_e_type               acq_sys
            /* Input acquisition system
            */

);


void                          sd_ss_hdr_idle_sys_update(

        sdsr_e_type                   sys,
            /* System per which to update the idle system.
            ** Ignored if set to SDSR_MAX.
            */

        sdsr_e_type                   geo_list,
            /* List per which to update the GEO list.
            */

        sdsr_e_type                   pref_list
            /* List per which to update the PREF list.
            */
);


void                      sd_si_update_hdr_time(

        sd_ss_e_type              ss,
        /**< System selection - MAIN or HDR. */

        int16                     ltm_offset,
        /**< Ltm offset on acquired HDR system */

        uint8                     leap_secs
        /**< Leap seconds */
);


void                      sd_si_sys_usable(

        sd_ss_e_type              ss,
            /* System selection - MAIN or HDR.
            */

        sd_sys_usable_e_type      usable_status
            /* new usable status of the system.
            */
);


void                             sd_si_update_bcmcs_status(

        sd_ss_e_type                     ss,
           /* SD instance.
           */

        sys_bcmcs_srv_status_e_type      bcmcs_srv_status
           /* New BCMCS service status.
           */
);

#endif /* defined(FEATURE_HDR) */


EXTERN  sd_ss_act_e_type          sd_ss_hdr_act_get_true_curr_act(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
);

void                      sd_si_update_hdr_activity(

        boolean                   is_hdr_activity
        /* New acquisition throttling status. */

);

#if defined (FEATURE_SD_LTE)
void                      sd_si_update_lte_cs_capability(

     sd_ss_e_type                ss,

     sys_lte_cs_capability_e_type lte_cs_capability
           /* New LTE CS capability
           */

 );
#endif /* FEATURE_SD_LTE */


#ifdef FEATURE_EOOS_UMTS
void sd_display_eoos_params (const sd_ss_act_acq_gwl_s_type*  act_ptr);
#endif


EXTERN void                           sd_si_info_ptr_get(

       sd_ss_e_type                   ss,

       sd_si_info_s_type*             info
);


EXTERN  void                          sd_si_reg(

        sd_si_info_f_type             *si_info_cb_func,
            /* Pointer to a Service Indicators callback function. SD calls on
            ** this function whenever one or more of the service indicators
            ** changes their value.
            **
            ** The argument to this function is a const pointer to a
            ** structure containing the current value of the service
            ** indicators */

        sd_si_acq_fail_f_type         *acq_fail_cb_func,
            /* Pointer to an Acquisition Fail callback function. SD calls on
            ** this function whenever acquisition fails (i.e., SD have
            ** attempted full acquisitions on all systems with no success).
            */

        sd_si_ok_to_orig_f_type       *ok_to_orig_cb_func,
            /* Pointer to an Ok To Orig callback function. SD calls on this
            ** function whenever the Ok To Orig status changes from FALSE to
            ** TRUE while the origination mode is other than none */

        sd_si_emerg_cb_mode_f_type    *emerg_cb_mode_cb_func,
            /* Pointer to an Emergency Callback Mode callback function. SD
            ** calls on this function whenever the Emergency Callback Mode
            ** status changes from FALSE to TRUE */

        sd_si_autonam_change_f_type   *autonam_change_cb_func,
            /* Pointer to an auto-NAM callback function. SD calls on this
            ** function whenever auto-NAM change is appropriate */


        sd_si_gw_net_list_f_type      *gw_net_list_cb_func,
            /* Pointer to the GW network list call function. SD calls on
            ** this function once it receives the results of the get network
            ** list on GSM/WCDMA/LTE systems.
            */

        sd_si_sim_state_f_type        *sim_state_cb_func,
           /* Pointer to the sim state call back function. Sd calls on this
           ** function if the SIM state was changed.
           */
        sd_si_reg_done_f_type         *reg_done_cb_func,
           /* Pointer to the reg. done call back function. SD calls this when
           ** a reg. is done successfully.
           */
        sd_si_data_suspend_f_type     *data_suspend_cb_func,
           /* Pointer to the data suspend call back function. SD calls this when
           ** a REG reports change in data suspend flag.
           */

        sd_si_srv_lost_f_type         *srv_lost_cb_func,
           /**< Pointer to the service lost callback. Sd calls on this
           ** function once the service is lost
           */

        sd_si_prl_load_complete_f_type *si_prl_load_complete_cb_func,
           /**< Pointer to the prl load complete. Sd calls on this
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
           /**< Pointer to the BSR status changed function. SD calls on
            ** this function once the BSR status has changed */
			
		sd_si_cs_emerg_scan_fail_f_type    *cs_emerg_scan_fail_cb_func
		   /**<SD report for sending cs scan fail during e911*/
);

EXTERN  void                      sd_si_srv_off(

        sd_ss_e_type              ss,
            /* System selection - MAIN or HDR.
            */
        boolean                   is_not_true_srv_loss
        /* Indicates whether it is internal or actual service loss */
);


EXTERN void                      sd_si_resel_hybr_bsr_state_update(

        sd_ss_e_type              ss,
            /* System selection - MAIN or HDR */

        boolean                   is_hybr_bsr
            /* If the new resel state is HYBR_BSR */
);


EXTERN  void                      sd_si_srv_off_until(

        sd_ss_e_type              ss,
            /* System selection - MAIN or HDR.
            */

        sd_si_update_e_type       update_criteria
            /* Service update criteria - indicate the minimum criteria for
            ** updating the service indicators.
            */
);


EXTERN  void                      sd_si_srv_pwr_save(

        sd_ss_e_type              ss
            /* System selection - MAIN or HDR.
            */
);


EXTERN  void                      sd_si_acq_fail(

        sd_ss_e_type              ss
            /* System selection - MAIN or HDR.
            */
);

EXTERN  void                      sd_si_hybr_bsr_to_hdr(
	        sd_ss_e_type              ss,
        sd_hybr_bsr_to_hdr_s_type        hybr_bsr_to_hdr_status);
        /* TRUE indicates start and FALSE indicates END */

EXTERN  void                      sd_si_kick_hybr2(
        sd_ss_e_type              ss
);


EXTERN  void                      sd_si_emerg_cb_mode(

        sd_emerg_cb_mode_e_type          emerg_cb_mode
            /* Callback mode - enter or exit
            */
);


EXTERN  void                      sd_si_srv_lost_cb(

        sd_ss_e_type              ss
            /* System selection - MAIN or HDR.
            */
);


EXTERN  void                      sd_si_full_srv_lost_cb(
  sd_ss_e_type              ss
      /* System selection - MAIN or HDR.
      */
);


EXTERN  void                      sd_si_autonam_change(

        sd_nam_e_type             nam
        /* The NAM that is proposed by the auto-NAM functionality.
        */
);


EXTERN  boolean          sd_ss_meas_is_done(

        sd_ss_e_type     ss
          /* System selection - MAIN or HDR.
          */
);


EXTERN  boolean          sd_ss_is_get_net_done(

        sd_ss_e_type     ss
          /* System selection - MAIN or HDR.
          */
);


EXTERN  void             sd_ss_update_get_net_done(

        sd_ss_e_type     ss,
          /* System selection - MAIN or HDR */
        boolean          flag

);


EXTERN  dword                      sd_ss_get_timer(

        sd_ss_e_type              ss
            /* System selection - MAIN or HDR.
            */
);


EXTERN  sd_ss_act_e_type          sd_ss_hybr_2_act_get(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
);


EXTERN  sd_ss_act_e_type          sd_ss_hybr_3_act_get(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
);




#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
EXTERN  sd_ss_band_pref_e_type    sd_filter_out_overlap_band(

        sd_ss_band_pref_e_type    band_pref
       /* Enumeration of system selection band and sub-band preferences.*/
);


#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif

EXTERN  void sd_decode_3gpp2_mcc(
	word *decoded_mcc, 
	const word encoded_mcc
);


EXTERN  void sd_decode_3gpp2_mnc(
	word *decoded_mnc, 
	const byte encoded_mnc
);


EXTERN  void                      sd_si_emerg_cb_mode(

        sd_emerg_cb_mode_e_type          emerg_cb_mode
            /* Callback mode - enter or exit
            */
);


EXTERN  sys_sys_mode_e_type            sd_misc_get_curr_acq_sys_mode_ss
(
  sd_ss_e_type ss
);


/*=============================================================================*/
/*=============================================================================*/
/* internal functions moved from sd.c which begin with _                       */
/*=============================================================================*/
/*=============================================================================*/


sd_ss_act_e_type
  sdcmd_user_ss_pref7 ( sd_ss_ind_user_ss_pref_cmd *cmd);


boolean
  _sd_ss_meas_list_get3 ( sd_ss_meas_list_cmd *cmd);


boolean
  _sd_ss_hybr_2_meas_list_get2 ( sd_ss_meas_list_cmd *cmd);


boolean
  _sd_ss_meas_list_rprt ( sd_ss_meas_list_rprt_cmd  *cmd);


boolean
  _sd_ss_hybr_2_meas_list_rprt ( sd_ss_meas_list_rprt_cmd  *cmd);


sd_ss_act_e_type
  sdcmd_user_pwr_up ( sd_ss_ind_user_pwr_up_cmd *cmd);


  sd_ss_act_e_type  sdcmd_hybr_2_user_pwr_up ( sd_ss_ind_user_pwr_up_cmd *cmd);

#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
  sd_ss_act_e_type  sdcmd_hybr_3_user_pwr_up ( sd_ss_ind_user_pwr_up_cmd *cmd);
#endif /* defined FEATURE_MMODE_TRIPLE_SIM */

sd_ss_act_e_type
  sdcmd_user_offline_cdma( sd_cmd_t  *cmd);


  sd_ss_act_e_type          sdcmd_hybr_2_user_prot_deactivate(
  sd_ss_ind_user_prot_deactivate_cmd  *cmd
);

#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
  sd_ss_act_e_type          sdcmd_hybr_3_user_prot_deactivate(
  sd_ss_ind_user_prot_deactivate_cmd  *cmd
);
#endif /* defined(FEATURE_MMODE_TRIPLE_SIM) */


sd_ss_act_e_type
  sdcmd_user_prot_deactivate ( sd_ss_ind_user_prot_deactivate_cmd  *cmd);


  sd_ss_act_e_type    sdcmd_hybr_2_user_pwr_save_exit(sd_cmd_t *cmd);

#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
  sd_ss_act_e_type    sdcmd_hybr_3_user_pwr_save_exit(sd_cmd_t *cmd);
#endif /*defined(FEATURE_MMODE_TRIPLE_SIM) */


sd_ss_act_e_type          sdcmd_user_pwr_save_exit(
  sd_cmd_t  *cmd
);


  sd_ss_act_e_type sdcmd_hybr_2_user_ss_pref5(sd_ss_ind_user_ss_pref_cmd *cmd);

#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
  sd_ss_act_e_type sdcmd_hybr_3_user_ss_pref(sd_ss_ind_user_ss_pref_cmd *cmd);
#endif /* defined(FEATURE_MMODE_TRIPLE_SIM) */

#if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)
sd_ss_act_e_type
  sdcmd_user_get_net_gw_ss ( sd_ss_ind_user_get_net_gw_ss_cmd  *cmd);
#endif /*defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)*/


#if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)


  sd_ss_act_e_type          sdcmd_cdma_acq_failed_pilot(
        sd_cmd_t                *cmd
            /* Pointer to a buffer where to place returned SS-Action */
  );

  sd_ss_act_e_type          sdcmd_cdma_acq_failed_sync(
        sd_ss_act_s_type          *rtrn_act_ptr
  );

  sd_ss_act_e_type          sdcmd_cdma_acq_failed_sync2(
        sd_cmd_t  *cmd
  );

  sd_ss_act_e_type          sdcmd_cdma_acq_prot_mis(
        sd_cmd_t                  *cmd
            /* Pointer to a buffer where to place returned SS-Action */
  );


  sd_ss_act_e_type          sdcmd_cdma_opr_prot_mis(
        sd_cmd_t  *cmd
  );

  sd_ss_act_e_type          sdcmd_cdma_acq_schm(
        sd_ss_ind_cdma_acq_schm_cmd *cmd
  );


  sd_ss_act_e_type          sdcmd_cdma_acq_ho_fail(
        sd_cmd_t                  *cmd
            /* Pointer to a buffer where to place returned SS-Action */
  );


  sd_ss_act_e_type sdcmd_cdma_acq_redir_abort(
        sd_cmd_t        *cmd
           /**< Pointer to a buffer where to place returned SS-Action */
  );


  sd_ss_act_e_type          sdcmd_cdma_opr_oh_info(
        sd_ss_ind_cdma_opr_oh_info_cmd  *cmd
  );


  sd_ss_act_e_type          sdcmd_cdma_opr_rescan(
        sd_cmd_t  *cmd
  );


  sd_ss_act_e_type          sdcmd_cdma_opr_reg_rej(
        sd_cmd_t  *cmd
  );


  sd_ss_act_e_type          sdcmd_cdma_opr_sys_lost2(
        sd_ss_ind_cdma_opr_sys_lost_cmd *cmd
  );

  sd_ss_act_e_type          sdcmd_cdma_operator_capability_changed(
        sd_ss_ind_cdma_opr_sys_lost_cmd *cmd
  );


  sd_ss_act_e_type          sdcmd_cdma_opr_redir(
        sd_ss_ind_cdma_opr_redir_cmd  *cmd
  );


#if defined(FEATURE_LTE_TO_1X)
  sd_ss_act_e_type          sdcmd_cdma_opr_ho_lte(
        sd_ss_ind_cdma_opr_ho_lte_cmd *cmd
  );
#endif /* defined(FEATURE_LTE_TO_1X) */


  sd_ss_act_e_type          sdcmd_cdma_opr_acpt_rej(
        sd_cmd_t  *cmd
  );

  void       sdcmd_update_cdma_sib8_nbrs(
        sd_ss_ind_update_cdma_sib8_nbrs_cmd *cmd
  );


  sd_ss_act_e_type          sdcmd_cdma_opr_new_sys(
        sd_ss_ind_cdma_opr_new_sys_cmd  *cmd
  );


  sd_ss_act_e_type          sdcmd_cdma_opr_sys_resel(
        sd_ss_ind_cdma_opr_sys_resel_cmd  *cmd
  );


  sd_ss_act_e_type          sdcmd_cdma_opr_lock(
        sd_cmd_t  *cmd
  );

  sd_ss_act_e_type          sdcmd_cdma_opr_unlock(
        sd_cmd_t  *cmd
  );


  sd_ss_act_e_type          sdcmd_cdma_opr_acc_term(
        sd_ss_ind_cdma_opr_acc_term_cmd *cmd
  );


  sd_ss_act_e_type          sdcmd_cdma_opr_cam_err(
        sd_cmd_t  *cmd
  );


  sd_ss_act_e_type          sdcmd_cdma_opr_cam_cdma(
        sd_ss_ind_cdma_opr_cam_cdma_cmd *cmd
  );


  sd_ss_act_e_type          sdcmd_cdma_opr_cam_amps(
        sd_ss_ind_cdma_opr_cam_amps_cmd *cmd
  );


  sd_ss_act_e_type          sdcmd_cdma_opr_hdm_cdma(
        sd_ss_ind_cdma_opr_hdm_cdma_cmd *cmd
  );


  sd_ss_act_e_type          sdcmd_cdma_opr_itspm(
        sd_ss_ind_cdma_opr_itspm_cmd  *cmd
  );


  sd_ss_act_e_type          sdcmd_cdma_opr_release(
  sd_cmd_t  *cmd
  );


#ifdef FEATURE_MMODE_LTE_RESEL
  sd_ss_act_e_type          sdcmd_misc_irat_to_gwl(
        sd_ss_ind_misc_irat_to_gwl_cmd  *cmd
  );
#endif /* FEATURE_MMODE_LTE_RESEL */

  sd_ss_act_e_type          sdcmd_user_cdma_irat_lte(
        sd_ss_ind_user_cdma_irat_lte_cmd  *cmd
  );

  sd_ss_act_e_type          sdcmd_cdma_opr_start_activity(
        sd_ss_ind_cdma_opr_activity_cmd *cmd
  );


  sd_ss_act_e_type          sdcmd_cdma_opr_stop_activity(
        sd_ss_ind_cdma_opr_activity_cmd  *cmd
  );


#endif /* defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900) */


sd_ss_act_e_type          sdcmd_misc_stack_local_deactivate(
      sd_ss_ind_misc_service_transfer_cmd  *cmd
);

sd_ss_act_e_type          sdcmd_misc_stack_local_activate(
      sd_ss_ind_misc_service_transfer_cmd  *cmd
);


#ifdef FEATURE_HDR

  sd_ss_act_e_type          sdcmd_hdr_user_ss_pref3(
  sd_ss_ind_user_ss_pref_cmd  *cmd
);

sd_ss_act_e_type          sd_ss_hdr_event_proc(


        sdss_evt_e_type           event,
            /* SS-Event to process.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
);
#endif /* FEATURE_HDR */


#ifdef FEATURE_HDR
  sd_ss_act_e_type          sdcmd_hdr_user_prot_deactivate(
        sd_ss_ind_user_prot_deactivate_cmd  *cmd
);
#endif /* FEATURE_HDR */


sd_ss_act_e_type          sdcmd_misc_meas_rprt(
  sd_ss_ind_misc_meas_rprt_cmd  *cmd
);


#ifdef FEATURE_HDR
  sd_ss_act_e_type sdcmd_hdr_user_redir_lte(
       sd_ss_ind_hdr_user_redir_lte_cmd  *cmd
);

  sd_ss_act_e_type          sdcmd_hybr_1_user_pwr_save_exit(
        sd_cmd_t  *cmd
);

  sd_ss_act_e_type          sdcmd_hdr_misc_meas_rprt(
        sd_ss_ind_misc_meas_rprt_cmd  *cmd
);


  sd_ss_act_e_type          sdcmd_hdr_acq_failed_pilot(
        sd_cmd_t  *cmd
);


  sd_ss_act_e_type          sdcmd_hdr_acq_failed_sync(
        sd_cmd_t  *cmd
);


  sd_ss_act_e_type          sdcmd_hdr_acq_prot_mis(
        sd_cmd_t  *cmd
);


  sd_ss_act_e_type          sdcmd_hdr_opr_prot_mis(
        sd_cmd_t  *cmd
);

  sd_ss_act_e_type          sdcmd_hdr_acq_sid2(
        sd_ss_ind_hdr_acq_sid_cmd *cmd
);

  sd_ss_act_e_type          sdcmd_hdr_acq_sid3(
        sd_ss_ind_hdr_acq_sid_cmd *cmd
);


  sd_ss_act_e_type sdcmd_hdr_acq_redir_abort(
        sd_cmd_t  *cmd
);


  sd_ss_act_e_type          sdcmd_hdr_opr_oh_info(
        sd_ss_ind_hdr_opr_oh_info_cmd *cmd
);


  sd_ss_act_e_type          sdcmd_hdr_opr_sys_usable(
        sd_ss_ind_hdr_opr_sys_usable_cmd  *cmd
);


  sd_ss_act_e_type          sdcmd_hdr_opr_redir(
        sd_ss_ind_hdr_opr_redir_cmd *cmd
);


  sd_ss_act_e_type          sdcmd_hdr_opr_sys_lost2(
        sd_ss_ind_hdr_opr_sys_lost_cmd  *cmd
);


  sd_ss_act_e_type          sdcmd_hdr_opr_sys_lost3(
        sd_ss_ind_hdr_opr_sys_lost_cmd  *cmd
);


  sd_ss_act_e_type          sdcmd_hdr_opr_resel_ok(
        sd_cmd_t  *cmd
);


  sd_ss_act_e_type          sdcmd_hdr_opr_acc_term(
        sd_ss_ind_hdr_opr_acc_term_cmd  *cmd
);


  sd_ss_act_e_type          sdcmd_hdr_opr_ses_abort(
        sd_cmd_t  *cmd
);


  sd_ss_act_e_type          sdcmd_hdr_opr_ses_nego_timeout(
        sd_cmd_t  *cmd
);


  sd_ss_act_e_type          sdcmd_hdr_opr_ses_opened(
        sd_cmd_t  *cmd
);


  sd_ss_act_e_type          sdcmd_hdr_opr_ses_close(
        sd_ss_ind_hdr_opr_ses_close_cmd *cmd
);


  sd_ss_act_e_type          sdcmd_hdr_opr_bad_sys(
        sd_ss_ind_hdr_opr_bad_sys_cmd *cmd
);


  sd_ss_act_e_type          sdcmd_hdr_opr_start_activity(
        sd_ss_ind_hdr_opr_activity_cmd *cmd
);


  sd_ss_act_e_type          sdcmd_hdr_opr_stop_activity(
        sd_ss_ind_hdr_opr_activity_cmd  *cmd
);
#endif /* FEATURE_HDR */


  sd_ss_act_e_type          sdcmd_misc_timer(
        sd_ss_ind_misc_timer_cmd  *cmd
);


  sd_ss_act_e_type          sdcmd_hybr_2_misc_timer(
        sd_ss_ind_misc_timer_cmd *cmd
);

#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
  sd_ss_act_e_type          sdcmd_hybr_3_misc_timer(
        sd_ss_ind_misc_timer_cmd *cmd
);
#endif /* defined(FEATURE_MMODE_TRIPLE_SIM) */


sd_ss_act_e_type          sdcmd_hybr_1_misc_timer(
      sd_ss_ind_misc_timer_cmd *cmd
);


#if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) || defined(FEATURE_SD_LTE)


sd_ss_act_e_type          sdcmd_gw_acq_plmn_failed_ss_2 (
  sd_ss_ind_gw_acq_plmn_failed_cmd  *cmd
);


void _sd_ss_reset_rlf_conn_mode_status
(
  sd_cmd_t  *cmd
);

sd_ss_act_e_type          sdcmd_gw_opr_srv_lost_ss_3(
        sd_ss_ind_gw_opr_srv_lost_cmd *cmd
);


#if defined ( FEATURE_WRLF_SYSTEM_SEL)

  sd_ss_act_e_type          sdcmd_gw_opr_srv_lost3(
        sd_ss_ind_gw_opr_srv_lost_cmd *cmd
);

sd_ss_act_e_type          sdcmd_wcdma_suitable_search_end(
        sd_cmd_t  *cmd
);


  sd_ss_act_e_type          sdcmd_wcdma_conn_mode_end(
        sd_cmd_t  *cmd
);
#endif /* FEATURE_WRLF_SYSTEM_SEL */


sd_ss_act_e_type          sdcmd_lte_opr_srv_lost_1(
        sd_ss_ind_lte_opr_srv_lost_cmd  *cmd
);


sd_ss_act_e_type          sdcmd_gw_opr_srv_info_ss_1(
        sd_ss_ind_gw_srv_info_cmd *cmd
);


sd_ss_act_e_type          sdcmd_gw_opr_acc_done_ss(
        sd_cmd_t *cmd
);


sd_ss_act_e_type    sdcmd_gw_ps_detach (
        sd_cmd_t  *cmd
);


sd_ss_act_e_type          sdcmd_gw_acq_plmn_success_ss_2 (
      sd_ss_ind_gw_srv_info_cmd *cmd
);


sd_ss_act_e_type          sdcmd_gw_acq_plmn_camped_ss (
  sd_ss_ind_gw_acq_plmn_camped_cmd  *cmd
);


sd_ss_act_e_type          sdcmd_lte_suitable_search_end(
  sd_cmd_t  *cmd
);

void    sdcmd_update_sys_sel_pref_req_id (
    sd_ss_e_type               ss,
    /* Stack id
      */
    uint16                     req_id
    /* Request Id
      */
);


#endif /* defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) || defined(FEATURE_SD_LTE) */




  sd_ss_act_e_type          sdcmd_hybr_2_misc_meas_rprt(
        sd_ss_ind_misc_meas_rprt_cmd  *cmd
);


  sd_ss_act_e_type          sdcmd_hybr_2_misc_pwr_save_enter(
        sd_cmd_t  *cmd
);

#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
  sd_ss_act_e_type          sdcmd_hybr_3_misc_meas_rprt(
        sd_ss_ind_misc_meas_rprt_cmd  *cmd
);

  sd_ss_act_e_type          sdcmd_hybr_3_misc_pwr_save_enter(
        sd_cmd_t  *cmd
);
#endif /* defined(FEATURE_MMODE_TRIPLE_SIM) */


sd_ss_act_e_type          sdcmd_misc_gw_sim_state_update(
      sd_ss_ind_misc_gw_sim_state_update_cmd  *cmd
);


sd_ss_act_e_type          sdcmd_misc_orig_success(
      sd_cmd_t  *cmd
);


sd_ss_act_e_type          sdcmd_misc_pwr_save_enter(
      sd_cmd_t  *cmd
);


sd_ss_act_e_type          sdcmd_hybr_1_misc_pwr_save_enter(
      sd_cmd_t  *cmd
);


sd_ss_act_e_type          sdcmd_misc_cnf(
      sd_cmd_t  *cmd
);


#if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)
  sd_ss_act_e_type              sdcmd_misc_gw_net_list_rpt_ss (
        sd_ss_ind_misc_gw_net_list_rpt_ss_cmd *cmd
);
#endif /* defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) */


#ifdef FEATURE_HDR
  sd_ss_act_e_type                sdcmd_misc_hdr_bcmcs_srv_status(
        sd_ss_ind_misc_hdr_bcmcs_srv_status_cmd *cmd
);
#endif /* FEATURE_HDR */


void   sdcmd_misc_mru_write ( sd_cmd_t  *cmd );


sys_sys_mode_e_type       sdcmd_misc_get_curr_sys_mode(
      sd_misc_get_curr_sys_mode_cmd *cmd
);


void                       sdcmd_misc_sys_avoid_list(
    sd_misc_sys_avoid_list_cmd *cmd
);


#if defined (FEATURE_LTE) && defined(FEATURE_LTE_TO_1X)
boolean        sdcmd_misc_allowed_sid_nid (
    sd_misc_allowed_sid_nid_cmd *cmd
);
#endif /* defined (FEATURE_LTE) && defined(FEATURE_LTE_TO_1X) */


boolean                     sdcmd_nam_sel4(
      sd_nam_sel_cmd  *cmd
);

boolean                     sdcmd_hybr_per_ss_nam_sel(
      sd_nam_sel_cmd *cmd
);


boolean                     sdcmd_is_plmn_rat_allowed (
      sd_is_plmn_rat_allowed_cmd  *cmd
);


sd_ss_act_e_type        sdcmd_user_nam_sel(
  sd_cmd_t  *cmd
);


sd_ss_act_e_type          sdcmd_user_pwr_down(
  sd_cmd_t  *cmd
);


sd_ss_act_e_type          sdcmd_user_term_get_net_gw_ss(
  sd_cmd_t  *cmd
);


EXTERN  sd_ss_act_e_type          sdcmd_cdma_opr_ndss_off(
        sd_cmd_t  *cmd
);


sd_ss_act_e_type          sdcmd_misc_cnf(
  sd_cmd_t  *cmd
);


boolean                 sdcmd_meas_list_get3(
  sd_ss_meas_list_cmd  *cmd
);


boolean                   sdcmd_hybr_2_meas_list_get2(
  sd_ss_meas_list_cmd *cmd
);

boolean                   sdcmd_hybr_3_meas_list_get(
  sd_ss_meas_list_cmd *cmd
);

boolean                   sdcmd_meas_list_rprt(
  sd_ss_meas_list_rprt_cmd  *cmd
);


boolean                   sdcmd_hybr_2_meas_list_rprt(
  sd_ss_meas_list_rprt_cmd  *cmd
);


boolean                   sdcmd_hybr_3_meas_list_rprt(
  sd_ss_meas_list_rprt_cmd  *cmd
);


#if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) || defined(FEATURE_SD_LTE)

void sdcmd_reset_rlf_conn_mode_status
(
  sd_cmd_t  *cmd
);
#endif /* defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) || defined(FEATURE_SD_LTE) */


void   sdcmd_misc_mru_write ( sd_cmd_t  *cmd );


sys_sys_mode_e_type       sdcmd_misc_get_curr_sys_mode(
      sd_misc_get_curr_sys_mode_cmd *cmd
);


/*===========================================================================

FUNCTION sdcmd_cdma_bsr_in_progress_cb

DESCRIPTION
  Notify the registered client (i.e., CM) that BSR status has changed

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void sdcmd_cdma_bsr_in_progress_cb(

       boolean bsr_status
);

/*===========================================================================

FUNCTION sdcmd_update_cdma_splmn_list

DESCRIPTION
  CDMA updates the splmn list to SD.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void sdcmd_update_cdma_splmn_list( sd_ss_ind_update_splmn_list_cmd  *cmd );
/*===========================================================================

FUNCTION sdcmd_update_hdr_splmn_list

DESCRIPTION
  HDR updates the splmn list to SD.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void sdcmd_update_hdr_splmn_list( sd_ss_ind_update_splmn_list_cmd  *cmd );

/*===========================================================================

FUNCTION sdcmd_mcfg_refresh

DESCRIPTION
  SD refreshes NV/EFS items based on MCFG update

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void          sdcmd_mcfg_refresh(sd_ss_mcfg_refresh_cmd  *cmd );

#endif /*SD_INTERNAL_H*/

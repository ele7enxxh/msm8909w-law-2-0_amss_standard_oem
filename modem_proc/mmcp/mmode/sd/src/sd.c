/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               S Y S T E M   D E T E R M I N A T I O N

                          F R O N T   E N D

GENERAL DESCRIPTION
  This file makes up the Front-end of System Determination.

  System Determination (SD) is a task that provides the following type of
  services to its clients:

    - System Selection

    - Service Indicators

    - Miscellaneous

  System Selection:
    SD provides the System Selection Client (SS-Client) with system selection
    service upon receipt of system selection indications (SS-Indications)
    that are being received from the SS-Client. The SS-Indications are
    received in the form of function calls. The return value from these
    function calls instruct the SS-Client of the next System Selection action
    (SS-Action) to be taken, as follows:

      - Continue:               Continue with current mode, band and/or CDMA
                                channel/AMPS system.

      - Acquire CDMA:           Attempt acquiring a CDMA system.

      - Acquire AMPS:           Attempt acquiring an AMPS system.

      - Acquire HDR:            Attempt acquiring an HDR system.

      - Dedicated Measurement:  Request channels strength measurement from
                                the searcher in dedicated mode.

      - Background Measurement: Request channels strength measurement from
                                the searcher in background mode.

      - Enter Power-save:       Enter Power-Save (deep sleep) mode of
                                operation.

      - Accept:                 Accept BS proposed mode, band and/or channel.

      - Reject:                 Reject BS proposed mode, band and/or channel.

      - Release:                Release current call.


  Service Indicators:

    SD provide an interface for a single client to register a callback
    function to be notified of the following service indicators status.

      - Service On/Off Indicator

      - Mode Indicator

      - Band Indicator

      - Roaming Indicator

    SD notifies the registered client (by calling on the client's provided
    callback function) of a new service indicators status whenever one or
    more of the service indicators change their value.


  Miscellaneous:

    SD provides an interface for clients to poll the following miscellaneous
    information:

      - PRL version:            Returns the version number of the PRL that is
                                associated with the currently selected NAM.

      - Preference validation:  Inform the caller as to whether a specified
                                system selection preference (like analog-
                                only mode) is supported by the PRL that is
                                associated with the currently selected NAM.
                                For example, analog-only mode preference is
                                only supported by a PRL that contains analog
                                acquisition records.

      - Capability validation:  Inform the caller as to whether the target
                                supports a specified combination of mode and
                                band.


  The following diagram shows a 2nd level decomposition of system
  determination. The arrows in this diagram represent function calls.


               2nd Level System Determination Decomposition
               --------------------------------------------

  +----------------------------------------------------------------------+
  |                              SD Clients                              |
  |  +--------------------+ +--------------------+ +-------------------+ |
  |  | Service Indicators | |  System Selection  | |   Misc. Service   | |
  |  | Client (SI-Client) | | Client (SS-Client) | | Client (MS-Client)| |
  |  +--------------------+ ---------------------+ +-------------------+ |
  +---------|--/|\-------------|--------------------------------|--------+
            |   |              |                                |
            |   |              |                                |
  +=========|===|==============|================================|========+
  |        \|/  |             \|/ SD Front-end                 \|/       |
  |   +-------------+  +-------------------------------+  +-----------+  |
  |   |             |  |                               |  |           |  |
  |   |   Service   |  |       System Selection        |  |   Misc.   |  |
  |   |  Indicators |  |           Front-end           |  |  Service  |  |
  |   |  (sd_si_xxx)|  |          (sd_ss_xxx)          |  |(sd_ms_xxx)|  |
  |   |             |  |                               |  |           |  |
  |   +-------------+  +-------------------------------+  +-----------+  |
  |           |            |                      |                      |
  +===========|============|======================|======================+
              |            |                      |
              |            |                      |
  +-----------|------------|----------------------|----------------------+
  |           |            |       SD Core        |                      |
  |          \|/          \|/                    \|/                     |
  |   +--------------------------+        +--------------------------+   |
  |   |                          |        |                          |   |
  |   |                          |        |                          |   |
  |   |                          |        |          Database        |   |
  |   |   System Selection Core  |------->|         (sdsr_xxx)       |   |
  |   |        (sdss_xxx)        |        |         (sdprl_xxx)      |   |
  |   |                          |        |                          |   |
  |   |                          |        |                          |   |
  |   |                          |        |                          |   |
  |   +--------------------------+        +--------------------------+   |
  |                                                                      |
  +----------------------------------------------------------------------+

  System Selection Front-end:
    Encapsulates the functionality that is associated with SS-Indications
    interfacing. The SS-Front-end is responsible for updating the System-
    Record with the information that is being received in the SS-Indications,
    as well as, mapping the SS-Indications into SS-Events that can be
    processed by the SS-Core.

  Database:
    Encapsulates all the knowledge that SD has of systems. This knowledge
    comes from two major sources; programmable knowledge (mostly in the form
    of a PRL and a SID/NID list that are programmed into NV) and accumulated
    knowledge that is collected during phone operation.

  System Selection Core:
    Encapsulates all the functionality that is associated with processing the
    SS-Events that are received from the SS-Front-end and for instructing the
    SS-Front-end of the next SS-Action to perform (e.g. acquire a CDMA
    system). The SS-Core operation is mostly controlled by SS-Scripts that
    are associated with the SS-Events that are being received from the
    SS-Front-end.

  Service Indicators:
    Encapsulate all the functionality that is associated with service
    indicators service.

  Miscellaneous Service:
    Encapsulate all the functionality that is associated with SD
    miscellaneous services.


  The following diagram shows the event flow between the SS-Client,
  SS-Front-end, SS-Core and the System Record that takes place when the
  SS-Client calls on an SS-Indication function.


                        SS-Indication Event Flow
                        ------------------------

  +-----------+       +------------+        +-----------+         +---------+
  |           |       |            |        |           |         |         |
  | SS-Client |       |SS-Front-end|        |  SS-Core  |         | Database|
  |           |       |            |        |           |         |         |
  +-----------+       +------------+        +-----------+         +---------+
       |                     |                    |                    |
       |     SS-Indication   |                    |                    |
      (1)-------------------->                    |                    |
       |   sd_ss_ind_xxx()   |                    |                    |
       |                     |             Update Database             |
       |                    (2)---------------------------------------->
       |                     |               sdsr_xxx()                |
       |                     |               sdprl_xxx()               |
       |                     |                    |                    |
       |                     |  Process SS-Event  |                    |
       |                    (3)------------------->                    |
       |                     |     sdss_xxx()     |                    |
       |                     |                    |                    |
       |                     |                   (4)                   |
       |                     |                    |                    |
       |                     |                    |Retrieve System Info|
       |                     |                   (5)------------------->
       |                     |                    |     sdsr_xxx()     |
       |                     |                    |     sdprl_xxx()    |
       |                     |                    |                    |
       |                     |                    < - - - - - - - - - -|
       |                     |                    |                    |
       |                     | Returned SS-Action |                    |
       |                     <- - - - - - - - - -(6)                   |
       |                     |    SDSS_IACT_XXX   |                    |
       | Returned SS-Action  |                    |                    |
       <- - - - - - - - - - (7)                   |                    |
       |   SD_SS_ACT_XXX     |                    |                    |
       |                     |                    |                    |


       Function Call:         ----------->
       Function Call Return:  <- - - - - -


  1. The SS-Client calls on an SS-Indication function.

  2. The SS-Front-end updates the Database with the information that comes
     with the SS-Indication, as necessary.

  3. The SS-Front-end maps the SD-Indication into an SS-Event and calls on
     the SS-Core to process the SS-Event.

  4. The SS-Core consults its script table as to whether an SS-Script is
     associated with the SS-Event it received from the SS-Front-end. If so,
     the associated script becomes the active SS-Script.

  5. The SS-Core processes the SS-Event in the context of the active
     SS-Script and the information that is stored in the Database.

  6. The SS-Core returns an SS-Action to the SS-Front-end, instructing it of
     the next action to take (i.e., continue, measurement-request, acquire,
     etc.).

  7. Based on the returned SS-Action from the SS-Core, the SS-Front-end
     instructs the SS-Client of the next SS-Action to take (i.e., continue,
     measurement-request, acquire, etc.).


EXTERNALIZED FUNCTIONS

  General:
    sd_init_before_app_task_start
    sd_init
    sd_term

  System Selection - General:
    sd_ss_act_get
    sd_ss_hdr_act_get
    sd_ss_log_last_action
    sd_ss_meas_list_get3
    sd_ss_meas_list_get2
    sd_ss_meas_list_get
    sd_ss_meas_list_rprt

  System Selection Indications - User:
    sd_ss_ind_user_pwr_up
    sd_ss_ind_user_offline_cdma
    sd_ss_ind_user_offline_amps
    sd_ss_ind_user_pwr_save_exit
    sd_ss_ind_user_ss_pref
    sd_ss_ind_user_ss_pref2
    sd_ss_ind_user_nam_sel
    sd_ss_ind_user_pwr_down
    sd_ss_ind_user_get_net_gw
    sd_ss_ind_user_term_get_net_gw

  System Selection Indications - CDMA Acquisition:
    sd_ss_ind_cdma_acq_failed_pilot
    sd_ss_ind_cdma_acq_failed_sync
    sd_ss_ind_cdma_acq_failed_unlock_rf
    sd_ss_ind_cdma_acq_prot_mis
    sd_ss_ind_cdma_acq_schm

  System Selection Indications - CDMA Operation:
    sd_ss_ind_cdma_opr_oh_info
    sd_ss_ind_cdma_opr_oh_info2
    sd_ss_ind_cdma_opr_oh_info3
    sd_ss_ind_cdma_opr_rescan
    sd_ss_ind_cdma_opr_reg_rej
    sd_ss_ind_cdma_opr_sys_lost
    sd_ss_ind_cdma_opr_sys_lost2
    sd_ss_ind_cdma_opr_redir_cdma
    sd_ss_ind_cdma_opr_redir_amps
    sd_ss_ind_cdma_opr_redir
    sd_ss_ind_cdma_opr_acpt_rej
    sd_ss_ind_cdma_opr_ndss_off
    sd_ss_ind_cdma_opr_new_sys
    sd_ss_ind_cdma_opr_sys_resel
    sd_ss_ind_cdma_opr_lock
    sd_ss_ind_cdma_opr_unlock
    sd_ss_ind_cdma_opr_acc_term
    sd_ss_ind_cdma_opr_cam_err
    sd_ss_ind_cdma_opr_cam_cdma
    sd_ss_ind_cdma_opr_cam_amps
    sd_ss_ind_cdma_opr_hdm_cdma
    sd_ss_ind_cdma_opr_hdm_amps
    sd_ss_ind_cdma_opr_itspm
    sd_ss_ind_cdma_opr_release

  System Selection Indications - AMPS Acquisition:
    sd_ss_ind_amps_acq_failed_cch
    sd_ss_ind_amps_acq_cch_sid

  System Selection Indications - AMPS Operation:
    sd_ss_ind_amps_opr_failed_pch
    sd_ss_ind_amps_opr_pch_sid
    sd_ss_ind_amps_opr_acc_sid
    sd_ss_ind_amps_opr_rescan
    sd_ss_ind_amps_opr_sys_lost
    sd_ss_ind_amps_opr_cdma_avail
    sd_ss_ind_amps_opr_acc_term
    sd_ss_ind_amps_opr_cam
    sd_ss_ind_amps_opr_bs_in_ovld

  System Selection Indications - HDR User:
    sd_ss_ind_hdr_user_ss_pref
    sd_ss_ind_hdr_user_ss_pref2

  System Selection Indications - HDR Miscellaneous:
    sd_ss_ind_hdr_misc_timer
    sd_ss_ind_hdr_misc_meas_rprt
    sd_ss_ind_hdr_misc_pwr_save_enter

  System Selection Indications - HDR Acquisition:
    sd_ss_ind_hdr_acq_failed_pilot
    sd_ss_ind_hdr_acq_failed_sync
    sd_ss_ind_hdr_acq_prot_mis
    sd_ss_ind_hdr_acq_sid
    sd_ss_ind_hdr_acq_sid2

  System Selection Indications - HDR Operation:
    sd_ss_ind_hdr_opr_oh_info
    sd_ss_ind_hdr_opr_oh_info2
    sd_ss_ind_hdr_opr_oh_info3
    sd_ss_ind_hdr_opr_redir
    sd_ss_ind_hdr_opr_sys_lost
    sd_ss_ind_hdr_opr_sys_lost2
    sd_ss_ind_hdr_opr_sys_lost3
    sd_ss_ind_hdr_opr_resel_ok
    sd_ss_ind_hdr_opr_acc_term
    sd_ss_ind_hdr_opr_ses_abort
    sd_ss_ind_hdr_opr_ses_close
    sd_ss_ind_hdr_opr_ses_nego_timeout
    sd_ss_ind_hdr_opr_ses_opened
    sd_ss_ind_hdr_opr_conv
    sd_ss_ind_hdr_opr_release
    sd_ss_ind_hdr_opr_bad_sys
    sd_ss_ind_hdr_opr_start_activity
    sd_ss_ind_hdr_opr_stop_activity
    sd_ss_ind_hdr_opr_sys_usable

  System Selection Indications - GSM/WCDMA/LTE Acquisition:
    sd_ss_ind_gw_acq_plmn_failed
    sd_ss_ind_gw_acq_plmn_success

  System Selection Indications - GSM/WCDMA/LTE Operation:
    sd_ss_ind_gw_opr_srv_lost
    sd_ss_ind_gw_opr_srv_info
    sd_ss_ind_gw_opr_acc_done

  System Selection Indications - Miscellaneous:
    sd_ss_ind_misc_timer
    sd_ss_ind_misc_meas_rprt
    sd_ss_ind_misc_orig_success
    sd_ss_ind_misc_pwr_save_enter
    sd_ss_ind_misc_cnf
    sd_ss_ind_misc_gw_net_list_rpt
    sd_ss_ind_misc_hdr_bcmcs_srv_status

  System Indicators:
    sd_si_reg
    sd_si_info_ptr_get
    sd_si_uz_info_ptr_get

  Miscellaneous:
    sd_misc_prl_ver_get
    sd_misc_is_target_supp_mode_band
    sd_misc_is_target_supp_mode_band_chan
    sd_misc_is_supp_pref
    sd_misc_is_cdma_locked
    sd_misc_get_cdma_request_mode
    sd_misc_get_cdma_return_cause
    sd_misc_get_curr_acq_sys_mode
    sd_misc_is_mode_pref
    sd_misc_remove_mode_pref
    sd_misc_add_mode_pref
    sd_misc_is_hybr_opr
    sd_misc_get_supp_band_pref
    sd_misc_sys_id_print
    sd_misc_is_sys_same
    sd_misc_is_del_tmsi
    sd_misc_get_eoos_param
    sd_misc_is_orig_mode
    sd_misc_oprt_mode_online


REGIONAL FUNCTIONS

  System Selection:
    sd_ss_meas_is_done

  System Indicators:
    sd_si_srv_off
    sd_si_emerg_cb_mode
    sd_si_autonam_change


INITIALIZATION AND SEQUENCING REQUIREMENTS

  Power-up:
    The initialization function sd_init_before_task_start must be called at
    power-up before any other SD function is called and before SD clients try
    to contact SD.

    After all tasks have started the SD initialization function sd_init() must
    be called to perform SD initialization that needs to be done after tasks
    have started.

    sd_init() returns TRUE if SD initialization is successful, in which case
    MS initialization should continue normally. sd_init() returns FALSE if SD
    initialization is not successful, in which case the MS is expected to go
    offline.

  Online:
    Upon receiving online command from CM, the SS-Client should call on
    sd_nam_sel() function. sd_nam_sel() returns TRUE if NAM selection is
    successful, in which case the SS-Client should call on
    sd_ss_ind_user_pwr_up() and follow the returned SS-Action. sd_nam_sel()
    returns FALSE if NAM selection is not successful, in which case the MS is
    expected to go offline.

  Offline:
    Upon a decision to go offline, the SS-Client should call on
    sd_ss_ind_user_offline_xxx().

  LPM:
    Upon entering LPM, the SS-Client should call
    on sd_ss_ind_user_pwr_down().

  Power-save:
    Upon entering Power-save (i.e., deep sleep), the SS-Client should call
    on sd_ss_ind_misc_pwr_save_enter().

  Power-off:
    Upon power-off, the SS-Client should call on sd_ss_ind_user_pwr_down()
    followed by sd_term(). After calling on sd_term(), no other public SD
    function shall be called prior to calling sd_init_before_task_start().

  Copyright (c) 2000 - 2013 by Qualcomm Technologies, Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/sd/src/sd.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/16/13   jvo     Replaced flag is_sd_crit_sec_init flag with call to
                   sdtask_is_init_complete()
11/01/13   jvo     Added MMode initialization interdependency check support
08/28/13   jvo     Cleanup remaining Fusion pieces
07/19/13   jvo     Remove all MDM/MSM Fusion features
07/15/13   ss      FR1860 - T/G on DSDS
06/21/13   gm      Selectively clear SDSR lists/systems and engine stack
06/19/13   gm      Send pref to SD-ALL stacks before ONLINE/PWRUP ind
06/18/13   dk      added is_sd_crit_sec_init to check mutex init, needed for when SD is not up
06/04/13   xs      E911 ready ind with ac_bar info considered
04/28/13   xs      LTE avail and tele bsr for non-SVLTE
03/27/13   dk      CR 463315 - sd_ss_ind_hdr_acq_sid2() API now queues command
                   to sd task queue using function pointer to sdcmd_hdr_acq_sid2
03/18/13   gm      Start hybrid operation flag should be enabled on ONLINE
03/14/13   gm      Don't ignore SS type for sd_ss_ind_gw_opr_acc_done_ss
02/21/13   cl      Re-initialize SD core during power-down event
02/11/13   xs      SD send acq fail to CM during HDR silent redial
02/03/13   cl      FR1975: Dynamic Switch SVLTE <-> CSFB
01/04/13   mg      Handle DO/1x to LTE reselection failure
12/14/12   qf      Reset sd curr action to continue upon receiving detach cnf
                   from cmregprx
12/03/12   xs      Added IRAT debug msg
10/24/12   qf      Make sure sd does not bail out at front end for starting
                   hybrid hdr.
10/10/12   qf      Make sure sd sends ok_to_orig for hdr data call if the
                   optimized ho LTE->ehrpd on going and not disturb the redir.
10/09/12   sr      Support  to derive MCC of an EvDo system from eqprl.
10/05/12   cl      Avoid scan LTE-only records on MAIN during CSFB call
09/28/12   jh      Reset max_acc_prob counter upon HDR traffic start
09/26/12   qf      Trigger service lost acquisition upon lte->do redirection
                   failure/abort/overhead supervision timeout
09/21/12   xs      Extend location based BSR to main
09/21/12   vm      Fix for going no serv before pwr save
09/20/12   qf      not update band_to_avoid for successful limited gwl acquisition
09/18/12   qf      FR2502 - 3GPP scan in MMSS operation
09/18/12   qf      Suspend duplicate band scan in immediate BSR after srv lost
09/17/12   skk     Do not send internal service lost or limited service during uptime.
09/15/12   xs      LTE telescopic BSR in SVLTE support
09/14/12   ak      Move Hybrid preference back to individual stacks.
09/12/12   aj/mg   Update service state in CM to NO SRV when LTE gets deactivated by MMOC
08/17/12   gm      Mainline EFS feature - Remove FEATURE_EFS
07/23/12   xs      Added support of location based BSR
07/18/12   jh      Added realign_lte flag to support LTEAvailFile feature
07/18/12   jh      Fix LTEAvailFile not maintained across the pwr cycle/LPM
07/17/12   jh      Added LTE Available File BSR support
05/29/12   jh      Repopulate PREF_LST upon LTE suitable search end
05/24/12   ak      Use the correct stack for processing the event.
05/16/12   ak      Validate the stack value returned by sdss_get_ss_formode() API
05/15/12   jh      LTE loss in idle to do lte scan once and go PWR_SAVE.
05/15/12   gm      Fix removing GWL bitmask along with removing LTE mode.
05/14/12   qf      FR1921-Eliminating 3GPP duplicate band scan
05/14/12   ak      Do not reset OOS after camping on Limited regional service.
04/26/12   jh      Handle LTE RLF in acq state for lte_conn mode processing.
04/12/12   vk      FIX to report proper geo_sys_idx.
04/10/12   aj      remove LTE from RAT pri list if sim state is ps invalid
04/04/12   sg      Added support for new mode_pref SD_SS_MODE_PREF_TDS_LTE
03/30/12   vk      FIX to report power save sttus during WRLF
03/27/12   cl      SD notify acq fail so CM can realign HDR when LTE pref no change
03/27/12   cl      Avoid sending duplicate service indication for same HDR srv
03/26/12   cl      Update eHRPD zone indicates if HDR service is eHRPD
03/23/12   gm      DIME integration fix: TMC deprecation and others.
03/15/12   ak      Periodically save MRU to NV\EFS
03/14/12   cl      GW/L limited reselection to LTE/DO enhancement
03/12/12   aj      prevent simultaneous access of HDR lists on 2 stacks
03/09/12   ak      SD Free floating cleanup
03/01/12   aj      update SRV SYS per currently acquired 3gpp system
02/28/12   aj      order RAT pri list per rat_acq_order for manual plmn srch
02/28/12   aj      update hybr 2 opti ss pref change functionality
02/28/12   aj      Fix DO/1x->LTE BSR in case where there is 1x/DO loss and reacq
02/23/12   ak      SD Free floating changes
02/21/12   vk      FIX to send NO_SRV during ONLINE to LPM transition
11/30/11   jh      Added feature for dynamic rat acq order change
02/15/12   sg      In case of service from 3GPP stack, SD need not to update
                   SDSR_ACQ_COLLOC_LST/ SDSR_SRV_COLLOC_LST
10/26/11   aj      update sim state on both MAIN and HYBR2 for ind on either ss
01/27/12   aj      set PRL status to inactive when sd_init() is invoked
01/20/12   sg      Adding TD-SCDMA RAT into default PRL
01/12/12   sg      Made changes for TGWL configuration.
01/03/12   ab      Added new function sdss_update_plmn_status()
12/26/11   rk      Fix to update RAT priority while doing BPLMN search
12/22/11   gm      New power save reason for Hybr BSR reselection.
12/15/11   sg      Add TDS band pref to SD get_networks related functions.
11/16/11   vk      Added API to return supported band preferance mask.
11/15/11   ar      Avoiding sd_si_update when received camped ind from NAS
                   on current PLMN when in full service
11/10/11   vk      FIX to clear SIB-8 NBR LIST when zero neighbors are reported.
10/25/11   vk      Changes related to Manual Search rejection in RLF.
10/21/11   aj      update HYBR2 stack to ONLINE state when oprt mode is ONLINE
10/21/11   rj      Added the feature to clear Avoid System list
10/18/11   rk      Reset the LTE CS capability upon oprt mode change to LPM
10/18/11   rj      CM shouldn't update clients when srv_domain is CAMPED
10/14/11   vk      Search on SIB8 neighbors if re-direction is failed on
                   redirected frequencies.
09/30/11   vk      Added API to check whether the requested CDMA system is supported by
                   current UE and target configuration .
09/12/11   aj      Clear IDL SYS on getting BSR ind on HYBR2 stack
09/03/11   rk      Fix to update GWL manaul record during LPM to ONLINE
08/27/11   aj      Clear LTE srv status during LTE-DO redir
08/29/11   sg      Add new API  sd_ss_gw_get_sim_state()
08/26/11   vk      changes for LTE connected mode Reset procedure.
08/25/11   xs      Added EPS network feature support definition (PS voice indication)
08/17/11   aj      update SD apis to support DO to LTE resel when LTE is on HYBR2
08/15/11   aj      support IRAT from LTE to DO when LTE is on HYBR2
08/05/11   xs      VOLTE phase 1 support
08/04/11   xs      Replace FEATURE_MMODE_NIKEL with FEATURE_MMODE_SC_SVLTE
07/25/11   aj      allow update of LTE in service req only if call orig is on same stack
07/24/11   aj      SC SVLTE 1.0 merge to mainline
07/21/11   rk      Changing the order of data suspend indication and service
                   indication from SD to CM in case of RLF
07/18/11   ak      Disrupt MSM GW scan/service during MDM traffic.
07/11/11   gm      Allow stack-sync-up with GW restore to SS-core processing
07/05/11   rk      Adding check for filter LTE in CS only domain pref back
                   again
06/29/11   rk      Remove check for filter LTE in CS only domain pref
06/23/11   ak      Fix UT for FEATURE_MMODE_MSM_GW_FULL_SERVICE_ONLY.
06/23/11   ak      Allow MSM to camp on GW Ltd when no 1X subscription.
06/16/11   ak      MSM to skip GW scans when MDM in LTE/DO activity.
06/14/11   ak      Fixed sd_misc_get_supp_band_pref API to return correct DO
                   bands when HDR on Hybrid stack.
06/14/11   cl      Fix issues with remove/restore GW during MMSS Arbitration
06/12/11   aj      Add utility API sd_misc_is_lte_full_srv_allowed()
06/06/11   cl      Added support for GW throttle filtering based on LTE srv
05/26/11   rk      Correcting the condition to pass lte_cs_capability to CM
05/25/11   cl      Remove/Restore GW for MMSS arbitration optimization
05/19/11   cl      Update sim_state correctly per subscriptions.
05/18/11   cl      Added support for GW throttle filtering based on regions.
05/18/11   rk      Fix for PS detach (in idle mode) in GWL configuration
                   when current domain preference is CS+PS
05/16/11   gm      HYBR_BSR should not be activated in LTE_ONLY mode
05/15/11   rk      Fix for PS detach when CS call is present and updating RAT
                   priority list based on phone mode preferences
05/13/11   skp     Merging DSDS Changes and Lint Fixes.
05/13/11   rk      Process suitable search end indication as USER_SS_PREF
                   event in case of LTE RLF
05/12/11   mp      BPLMN - LTE Only capable is not able to perform Manual Search
05/10/11   gm      Maintain separate copy of IDL sys for CDMA
04/28/11   sv      Read geo system index and add it to si info
04/28/11   sv      Added sd_ss_ind_cdma_opr_oh_info5 to pass cdma registration
                   period to CM
04/18/11   rk      For PPCSFB only no need of recovering for RLF.
                   Treat is as regular OOS
04/18/11   cl      Update SIM state correctly in HYBR stack for SVLTE2
04/14/11   skp     Updating PLMN when back from no srv area to service area
04/13/11   ak      Added support for scanning skipped GW systems on MSM.
04/12/11   cl      Propagate SD stacks's states to CM
04/12/11   cl      Map GW-Limited service to FAIL so we can perform
                   GW-Ltd -> DO/LTE BSR
04/07/11   ak      Fix issues in HDR-->LTE BSR on Fusion MDM.
04/07/11   sv      Added cdma_unlock param to user_pwr_up API
03/30/11   rk      Fix to disable LTE if sim state is PS invalid
03/29/11   rk      Fix for not doing PPCSFB after receiving AttachAccept with
                   SMSOnly set
03/25/11   gm      LTE (Lmtd) to DO resel support
03/15/11   cl      Remove is_active indication from service indication
03/09/11   cl      Add support for SVLTE2 MMSS arbitration
03/02/11   rk      Fixing issues for coming back to LTE after CSFB call end.
02/22/11   rk      Adding  lte cs capability in API and passing it in srv ind
02/17/11   cl      Add support FEATURE_MMODE_REMOVE_1X for FUSION-MDM
02/16/11   gm      Extend acquisition while in ECBM & sys loss
02/02/11   rm      Adding LTE RAT-B changes
02/11/11   ak      MDM to notify MSM of DO activity for acq throttling
02/10/11   am      Adding QTF support
01/28/11   fj      Remove FEATURE_SD_GWL.
01/28/11   rk      Fix for reacquire LTE after MO 1XCSFB call release
01/19/11   ak      Added support for periodic GWL scans in DO only coverage.
01/19/11   gm      LTE band pref are included in filtering mode-pref
01/19/11   rk      Fix for handling orig mode CSFB_EMERG_ORIG correctly
01/18/11   cl      Fix HDR acqusition issue
01/04/11   rm      CMI-4: Removing #ifdef's, puzl.h changes
12/23/10   sv      Integrate DSDS changes
12/16/10   ay      Handling of back to back LTE RLFs
12/10/10   aj      support to scan all 3gpp bands during 911 call
12/22/10   rm      Fixing lint errors
12/10/10   rk      Changes for e1XCSFB call
12/09/10   aj      Add utility API sd_misc_is_gwl_full_srv_allowed
12/03/10   aj      Update service indication if moving from lmtd to full srv
11/24/10   xs      Fusion featurization clean up
11/03/10   aj      DO -> LTE reselection code
11/02/10   gm      Fix to KW errors.
10/27/10   aj      set realign_hdr if colloc status has changed in cdma srv
10/25/10   aj      BSR after IRAT from LTE->DO should be triggered by TMR_CDMA_RESEL
10/21/10   gm      Changed default values for MCC & IMSI_11_12, SD to update
                   its clients even if only MCC/IMSI_11_12 values change.
10/18/10   rk      Support for 3GPP CSFB call, OOS scenarios service aqusition
                   for normal CS/Emergency call
10/18/10   cl      Clear the hybrid flag under LTE to HRPD redirection
10/12/10   sv      check realign_hdr flag before skipping srv indication
10/11/10   sg      Add a check for handling acquired system added in MRU,
                   in Maual Mode such that UE should send Auto req after
                   it's mode pref changed to Auto from Manual.
10/05/10   rk      Merging 1XCSFB pre-registration code
10/04/10   aj      additional checks for lte do redir
10/01/10   ak      Merged the Fusion code from Dev branch.
09/24/10   sv      Set the correct pwr_save reason
09/24/10   xs      Add check to only print HDR rev in HDR mode
09/22/10   aj      add support to inform SD of HDR deactivation by mmoc
09/15/10   gm      Fix unreachable code for FEATURE_WLAN.
09/08/10   xs      Fixed issue of Service is OFF occurs despite Registration
                   succeeds due to SD_SS_ACC_TERM_ACC_SUCCESS.
09/03/10   aj      support to stay on any LTE system if UE is in conn state
08/31/10   rn      Use Stack as per hybrid pref
08/23/10   ay      Minor fix in EOOS function
08/18/10   rm      In pwr save, inform srv_domain as NO_SRV to CM
08/16/10   aj      update band filtering logic in update_rat_pri_list api
08/06/10   aj      Replaced SD_ERR with SD_MSG_ERROR
08/05/10   rk      Added support to update SD with GW session info
08/04/10   ay      Mod for LTE RLF
08/04/10   rk      Doing pure limited service aquisition when sim state is not
                   valid for current service domain preference.
08/03/10   aj      Add LTE support in sdprl_is_supp_mode_band_pref
07/30/10   aj      EFS MRU update for gwl rat pri list
07/28/10   mh      Fixed KW error
07/27/10   fj      Revert RLF indication change CL#1355784.
07/22/10   xs      Added new API sd_misc_get_curr_sys_mode
07/20/10   aj      temporary limited srv Rat priority list update
07/16/10   aj      Update srv sys before updating acquired mode in
                   sd_ss_ind_gw_opr_srv_info2()
07/14/10   rk      Fixing compilation issue
07/08/10   sv      Add sd_ss_ind_cdma_sys_on_msm Fusion API
07/10/10   np      Swap Data Suspend and Service Info event
07/02/10   fj      For RLF indication, bypass data_suspend update in LTE connected mode.
06/28/10   aj      RAT priority list updates for BSR 2.0
06/27/10   ay      Added EOOS support
06/24/10   rn      Add RAT priority list support in MEAS_BACK
06/24/10   rk      Changing the order of data suspend ind and service  ind.
06/23/10   rk      Reverting the change done to wakeup from deep sleep on
                   connected mode end indication.
06/21/10   rk      Redesign of WRLF feature.
06/18/10   aj      Add support to pass HDR subnet mask length to CM
06/18/10   aj      seperate band_cap and bst_band_cap in service request
06/16/10   fj      Added LTE support in sd_ss_get_supp_mode_pref().
06/15/10   aj      Adding support for passing HDR personality info
06/09/10   rm      In manual mode with full service, SD should store the PLMN
                   given by NAS.
06/02/10   xs      Fixed issue of DO channel avoidance time when MAP
05/26/10   aj      add support for IRAT pilot measurement
05/20/10   fj      Update CM when RAT changes for GWL limited service.
05/19/10   fj      Featurize caii.h and caii_v.h under 1x feature.
05/19/10   gm      Avoidance/ESS logging
05/18/10   rn      Added rat_pri_list_info in get_net_gw
05/07/10   fj      Add lte_band_pref in sd_misc_is_target_supp_mode_band().
05/11/10   sg      Updated function sd_ss_ind_gw_opr_srv_info2(), to compute
                   ref for "SDSR_SRV_SYS" before calling sd_si_update().
05/05/10   rn      LTE-1x IRAT - add support in ACQ state, reject if invalid sys
04/26/10   aj      LTE code fixes for BSR operation
04/05/10   fj      Remove unaccessed code in sd_ss_event_proc().
04/05/10   aj      add sdss_get_rat_pri_sys_idx()
04/02/10   aj      update 3gpp2 info in rat priority list
04/02/10   aj      replace ue_capability with rat priority in missed out places
04/01/10   aj      fix issue with ue capability in hybrid operation
03/31/10   cl      Added support event to indicate PRL initialization completed
03/25/10   aj      ERR_FATAL => error msg , when NAS returns incorrect band
03/25/10   ay      Fixed the lte end of connected mode
03/24/10   fj      Added support for reporting LAC/TAC/RAC to cmss.
03/22/10   aj      Lint fixes
03/20/10   rn      Replace UE_CAPABILITY with rat_priority_list_info
03/19/10   aj      MMSS support
03/20/10   ay      Compiler warning fixes
03/19/10   am      Fixing compilation warning
03/17/10   jqi     Lint fixes.
03/11/10   ay      Fixed the argument handling in lte_suitable_search_end()
02/16/10   rn      Added SD EFS support.
02/08/10   jqi     ESS support.
01/11/10   fj      Added support for GWL.
01/14/10   ay      Added support for LTE in Connected Mode
13/01/10   rn      removed unsupported modes from UE_CAPABILITY in ACQ_GWL
01/08/10   rn      LTE-eHRPD redirection support
01/06/10   ay      Removed UOoS function
01/04/10   ay      Made the sd_ss_ind_gw_opr_srv_lost2() static
12/21/09   ak      Removing UOOS
01/04/10   rk      Service indication to CM on limited service and SD next
                   action is another service aquisition request.
12/22/09   ay      Removed a redundant function sd_nam_sel_lte()
12/10/09   fj      Added sd_misc_get_mode_band_capability_with_lte() and
                   sd_misc_is_supp_pref_lte().
12/09/09   ay      Added sd_misc_get_band_mode_mmode(), renamed some functions
11/05/09   fj      Set the default value of acq_sys_ptr->sys.mode based on
                   acq_sys_ptr->sys.mode.
11/17/09   rn      Add UE_CAPABILITY in ACQ_GWL
11/07/09   aj      Add hybrid version of TEMP_LST and TEMP2_LST
11/09/09   sg      Add non-demand paging macro to function that are not to be
                   demand-paged.
09/21/09   cl      Blocked emergency call back mode on the JCDMA build.
10/21/09   sv      Moving from clk API to time API
10/16/09   ay      Added support for lte_band_pref, sys ind update
10/16/09   ay      Added GWL support and related code due to LTE having a dedicated band
10/13/09   sg      Update MRU with manual system - customer specific requirement.
10/10/09   sg      Fix to avoid adding duplicate GW entries in system record table,
                   on service aquisition in manual mode.
10/01/09   fj      Moved prldiag_init() from sd_init() to
                   sd_init_before_app_task_start().
09/30/09   sv      KW Fixes
09/29/09   fj      Fixed Lint errors due to LTE merge.
07/30/09   fj      Removed FEATURE_LTE_PENDING for 2G/3G UT environment.
07/10/09   np      Adding LTE Code
07/20/09   sv      Reload HDR (GEO,PREF,COLLOC) lists before re-aligning HDR
07/13/09   aj      Ignore WLAN user_ss_pref with no change and pref_reason user
                   when there is WLAN service
07/07/09   mh      Check Hybrid HDR operation state when realigning hdr
06/29/09   rk      When service status is limited reginal and for pref reason
                   multimode scan or orig_resume just update pref and continue
06/10/09   sv      Updated preference change optimization check to ignore
                   any stack sync up preference changes when main stack
                   is in pwr save
05/27/09   aj      Adding support for updating clients with cdma local time
05/15/09   mh      Update MCC/IMSI_11_12, regardless of is_mcc_sys_rec
04/26/09   aj      Initialize is_wps_survey when issuing get networks to WLAN
03/31/09   mh      Added support for prl match indicator in ss info
03/26/09   mh      Update hdr_cand_idle_sys per hdr_cq_sys for hdr ovhd
03/20/09   rm      Renaming : sdprl_map_chan_to_blksys -> sd_map_chan_to_blksys
                   sdss_map_band_to_band_pref -> sd_map_band_to_band_pref
03/12/09   rm      Including caii_v.h
03/09/09   rm      Adding wrapper function - sd_ss_get_supp_mode_pref
04/03/09   aj      Added support for emergency callback exit on sys lost
03/03/09   fj      Added prldiag_init() in sd_init().
02/25/09   sv      Removing inclusion of sys_wlan_v.h
02/23/09   rm      Including sys_v.h and sys_wlan_v.h
02/13/09   mh      Added support for default roaming indicator in ss info
02/10/09   sn      Constant GW UOOS scan time across band groups.
02/06/09   aj      WLAN subscription decoupling
02/05/09   rk      Treating limited service ind on forbidden PLMN in
                   manual mode as aquisition failure
11/28/08   rm      In UOOS report Limited service to NAS if sim state is
                   CS_PS_Invalid. Reverting part of the fix for CR-124247
11/05/08   sv      Updated stack syncup to clear the stack sync-up progress flag
                   when the current pref_reason is not stack sync-up
10/30/08   rm      Removing Lint warnings
10/29/08   fj      Move FEATURE_WLAN into APIs for CMI effort in WM6 build
10/23/08   ak      Fixing the no firmware scenario
09/29/08   rk      Featurizing band filtering logic.
09/25/08   sv      Memory Reduction changes
09/18/08   rm      Adding acq_order_pref in sd_ss_act_get_net_gw_s_type.
09/15/08   pk      Added support for WLAN UOoS
09/15/08   rm      Get band and mode pref based on sdss_pref.true_mode_pref,
                   when GW service is unavailable.
09/04/08   sv      Added WPS API's for start and abort
                   Added Callback function for WPS result
08/20/08   st      Klockwork fixes
07/30/08   sv      Filter Stack Sync-up event when HDR call is being originated
07/24/08   sn      UOOS timer handling for both P2 and PWR_UP_ENH enabled.
07/13/08   sv      Optimise the use of List sorting.
06/11/08   fj      Added sd_misc_get_mode_band_capability
06/13/08   sn      Set UOOS WCDMA scan time as per ratio even for GSM only
                   mode and vice versa.
06/12/08   aj      Klocwork fixes
05/28/08   ak      Updated sd_ss_chan_list_update API interface to ensure
                   channel list update.
04/30/08   cl      Adding sd_misc_sys_avoid_list to list all the avoided
                   systems. HDR/SCHR use this info to update their scan list
                   when doing OFS.
04/28/08   ak      Added sd_ss_ind_cdma_opr_prot_mis & ssscr_hdr_opr_prot_mis
04/24/08   rm      Removing feature flag FEATURE_CM_NAS_SERVICE_REQ_TYPE
04/07/08   cl      Update Function's Description sd_misc_is_fts_mode()
03/19/08   cl      Fixing UOOS Time to support Auto Mode Builds.
03/17/08   vs      Added is_assn_tag_changed, is_geo_count_changed and
                   is_geo_changed flag's to check if HDR needs to be
                   re-aligned
03/14/08   rm      CM-NAS interface for new service req type
03/22/08   ic      Undo of "Replace custmmode.h with modem_mmode_cust.h and
                   FEATURE_MMODE_CUSTMMODE_H with FEATURE_MODEM_MMODE_CUST_H"
03/18/08   ic      Replace custmmode.h with custmmode.h
                   Replace FEATURE_MMODE_CUSTMMODE_H with FEATURE_CUSTMMODE_H
03/08/08   ic      Use FEATURE_MMODE_CUSTMMODE_H instead of FEATURE_MM_CUSTMM_H
02/19/08   cl      Optimize the execution time of SD API sd_misc_is_fts_mode
02/21/08   cl      Added SD API for sd_misc_is_fts_mode to determine if the
                   phone is in FULL_TIME_SHDR Mode or not.
02/20/08   vs      Made changes to clear SDSR_PREV_COLLOC_LST
                   and reset realign-hdr when camping on GW system
01/17/08   pk/vs   Added support for non-Hybrid UOOS

01/22/08   ak      Update MRU table in JCDMA config if GSM enabled.
01/22/08   rm      Reverted the changes done for- support for SIM Invalid
                   and UOOS
01/04/08   ka      New API for informing Daylight savings and LTM Offset as
                   part of schm.
09/17/07   pk      Added support for non-Hybrid UOOS
09/11/07   pk      Added support for FEATURE_DISABLE_DEEP_SLEEP
09/06/07   cl      KDDI request not to restart the TMR_HOLD_RESEL when we exit
                   session related traffic.
08/30/07   cl      Added CM critical section support. Also replaced INTLOCK /
                   INTFREE with rex_enter_crit_sect() / rex_leave_crit_sect()
08/29/07   sk      Fixed error in MOST_PREF classification of a system to
                   ignore systems that shall not be acquired due to user pref
                   or target capability.
08/21/07   ic      Added checks for custmm.h being used
07/24/07   cl      Mainline FEATURE_INT_ROAMING and FEATURE_2ND_PRL
06/15/07   rp      Adding support for rem_uoos_sleep_time and rem_uoos_awake_time
06/15/07   sk      Zeroing out unused fields.
06/11/07   sk      Adding support for passing netlist system record index
                   for WLAN acquisition and measurement.
06/07/07   sk/cl   Limited orig start filtering in MAIN instance to
                   non-powersave states if the prefs didn't change.
06/06/07   pk      UOOS enhancements...
05/25/07   pk      Added support for SIM Invalid and UOOS cont...
05/14/07   pk      Enabling UMTS UOOS ENH and P2 features simultaneously
05/03/07   jqi     DO not shorten the reselection timer after call release
                   if the expected SID/NID of redirected system is wildcard.
05/02/07   jqi     DO not turn off the redirection afer IHO if the expected
                   SID/NID of the redirected system is WILDCARD.
                   Expanded 1x system reselection timer value from 16-bit to
                   32-bit.
04/23/07   jqi     Added Filtering support for stopping HDR activity.
                   Added support for sd_misc_is_orig_mode.
04/17/07   rp      Adding check for NULL pointer
04/15/07   rp      Updated the  net_sel_mode field in srv_sys
03/28/07   sk      Added new GW UOOS wakeup time for GCF fix.
02/26/07   pk      Added sdss_get_ss_for_mode for all featurizations
02/15/07   jqi     Added support for sd_ss_ind_cdma_opr_oh_info3
01/04/07   pk      Added support for VoIP -> 1X Handoffs
12/01/06   pk      Added support for Automatic mode for CDMA + GSM UOOS
                   builds
12/03/06   ka      Correcting sd_ss_meas_list_get2() to return all GW
                   bands required for measurement.
11/29/06   ka      Limited service resel timer expiry results in sending
                   MEAS_BACK action instead of full service req. Satisfies
                   GCF 6.1.2.6.
11/17/06   jqi     Added support for HDR active protocol revision information.
11/15/06   pk      Added support for HYBR_GW
11/02/06   pk      Added support for SD_LOG_STATE_PWR_SAVE
09/17/06   sk      Store the newly passed band info for WLAN manual system.
09/11/06   rp      Added code to copy wlan_tech_pref from sr_ptr to act_ptr.
09/08/06   sk      Added new APIs to support WLAN measurement.
09/05/06   pk      FEATURE_UMTS_UNIFORM_OOS_HANDLING, added support to
                   query UOOS timers
08/29/06   sk      Added WLAN Adhoc support.
08/25/06   pk      Added support for sd_ss_ind_gw_acq_plmn_failed2
08/24/06   pk      Added support for IF construct and GCF test fixes
08/24/06   pk      Added support for sdss_nv_init
08/23/06   jqi     Added support for system reselection during BCMCS flow.
08/16/06   pk      Added support for GW UOOS
08/14/06   jqi     Added the system lost reason for 1x system exit events.
08/07/06   pk      Added support for 1X UOOS
07/14/06   pk      Modified the computation of is_colloc_changed flag when 1X
                   acquires service
07/13/06   jqi     Added the system lost reason for HDR system lost event.
07/07/06   sk      Added support for logging GW system into MRU when moving
                   from limited system to system with service.
06/14/06   jqi     Do not declare the redirection is invalid if there is zero
                   channel in the redirection recs.
06/09/06   jqi     Modified the comments.
05/23/06   jqi     Always forced HDR usable flag as usable for JCDMA build.
                   Fixed strip errors.
05/22/06   jqi     Made use of the chan list for HDR oh info.
05/12/06   sk      Fixed bug in WLAN API using/modifying non-WLAN lists.
05/09/06   jqi     Fixed the compilation errors caused by EJECT.
05/08/06   sk      Added support for expected SID for HDR.
05/04/06   jqi     Lint fixes.
04/09/06   jqi     Increased Register Failure Counter to 6 in over reach.
                   Made "1x Reselection Threshold" as a configurable NV item.
                   Mainline FEATURE_SILENT_REDIAL_20.
                   Connection Deny reason(network busy), avoid the channel
                   60s.
                   No TCA and NoRTC ACK, 3 failures avoid 60s or encountering
                   ACQ_FAIL, whichever come first.
                   Scan all channels during overreach wake up.
                   Do not cancel avoidance when entering power save mode.
                   Added null pointer check in sdsr_list_is_fade.
                   Mainline FEATURE_SILENT_REDIAL_20
04/06/06   sk      BCMCS can be turned off implicitly only if CDMA is
                   acquired. All other cases of implicit turning off of BCMCS
                   removed.
04/01/06   ka      Adding plmn list status to network list cnf.
03/31/05   pk      Modified sd_si_ok_to_orig() interface. Added data_suspend
                   flag as one more parameter
                   Modified sd_ss_ind_gw_opr_srv_info() and
                   sd_ss_ind_gw_acq_plmn_success to update the data
                   suspend flag as the last step of processing. The behavior
                   was changed to fix CR 59670.
03/27/06   jqi     Lint fixes.
03/14/06   jqi     Counting num of fades per HDR IDLE system.
03/9/06    jqi     DO system reselection optimization(Sys resl at DO MO call
                   end.)
                   Multichannel avoidance.
                   Reset the DO failure counter when Subnet ID is changed.
                   Do not change sd state if only srv domain is changed.
03/06/06   jqi     Added comments for bad system.
02/08/06   pk      Replaced SD_ERR with SD_MSG_ERROR
01/12/06   pk      Added sd_misc_is_sys_same
12/22/05   jqi     Merged background PLMN search support.
12/07/05   ka      Adding sd_ss_ind_gw_data_suspended() to inform CM about
                   data suspend.
12/02/05   jqi     Lint fixes for lint ver8.
11/23/05   jqi     Mainline FEATURE_HICPS20 and FEATURE_HICPS30 under
                   FEATURE_HDR.
11/08/05   jqi     Added support for 1x acquisition throttling for BCMCS
                   activity.
11/07/05   jqi     Used hybr pref from main instance.
11/02/05   sk      Fixed all Lint and compiler warnings.
10/18/05   pk      Added sd_misc_get_common_mode_pref
10/05/05   sk      Added support for configuring SD values from NV.
09/16/05   jqi     Fixed the errors for CDMA_WLAN configuration.
09/10/05   pk      Added srv_acq_time to sd_si_info_s_type
09/01/05   jqi     Fixed the race condition where SD return continue for the
                   system lost event.
                   Ignore the release event when SD is in acquisition state
                   and return continue.
8/15/05   pk       Modified the use of realign HDR flag while reporting
                   service to CM. If the collocated lists didn't change the
                   re align flag is not set.
07/21/05   jqi     Added 1xEV-DO system reselection on hybrid operation.
07/18/05   dk      DeCouple WLAN dependency from HDR_HYBRID.
07/11/05   jqi     Mainlined FEATURE_BCMCS.
07/11/05   jqi     Added avoidance timer for access failures and fades.
07/08/05   sk      Added 2 new APIs:
                     - sd_ss_ind_hdr_opr_ses_nego_timeout
                     - sd_ss_ind_hdr_opr_ses_opened
                   Added SDSS_EVT_HDR_OPR_SES_NEGO_TIMEOUT to
                   sd_ss_return_cause_update().
                   Reset event counters on SID change.
06/26/05   ic      Lint and compiler warnings cleanup
06/17/05   sk      Removed previous changes.
06/17/05   sk      Updated sd_ss_ind_hdr_opr_ses_abort() to call
                   sdss_acc_update()
06/15/05   jqi     Return SD action continue for SD_SS_ACC_TERM_NORMAL for
                   HDR operation.
06/06/05   jqi     Added debug message for HDR access term report.
06/06/05   pk      Modified sd_ss_ind_hdr_user_ss_pref(),
                   modified the check for service status for Powersave, only
                   for redial normal.
                   Lint Clean up.
05/25/05   dk      Initialize WLAN SS state to ACQ during sd_nam_sel.
05/20/05   ka      Adding support for Limited Manual network sel type
                   to possibly camp with full service on Manually selected
                   PLMN.
05/13/05   ka      API to avoid a bad WLAN system
05/04/05   jqi     Fixed act and true act out of sync issue on GPS
                   activities.
                   Set redirect system with acq system during redirection
                   process
                   Provided the API to return supported band preference
                   according to target capability and user mode/band pref.
                   Added support to process HDR bad system event while in
                   acquisition state.
04/29/05   pk      Added sd_misc_sys_id_print() to print the sys_id
03/28/05   RC      Rebuilding COLLOC_LST upon ITSPM - CR59458
03/11/05   pk      Modified sd_ss_ind_hdr_user_ss_pref(),
                   if SS-Preferences are not changed, we return
                   SD_SS_ACT_CONTINUE for REDIAL_NORM.
02/24/05   pk      Fixed some lint errors
01/30/05   sj      Added support for WLAN.
01/10/05   dk      Updated sd_si_update() to report service change during
                   LAC changes when in GW mode.
01/07/05   sj      Fixed issue with setting reselect flag during auto->auto.
01/06/04   jqi     Lint fixes.
01/06/05   sj      Set srv_status to NO_SRV even if it is currently PWR_SAVE.
12/04/04   jqi     Power save mode optimization for GSM/UMTS targets.
11/24/04   sj      Added HDR acc_term in event report.
11/16/04   sj      Avoid system will place sid/nid lock.
10/21/04   RC      More enhancements related to FEATURE_INT_ROAMING
11/02/04   jqi     Added API to validate the mode preference and band class
                   against the target capability
10/26/04   jqi     Read subscription items only when the subscription is
                   available.
10/25/04   sj      Added bad_sys SD SS HDR indication.
10/21/04   RC      Update the SID of the serving system per the reported SID
                   in sd_ss_ind_hdr_opr_oh_info().
10/08/04   sj      Fixed issue with lists mapping in hybrid mode when hdr timer
                   expires after subscription available.
10/04/04   jqi     Valid band and mode againt both target and PRL for task
                   initialization and system pref change command.
09/15/04   sj      Fixed issue of SS-HDR returning ACQ_HDR if hybr_pref
                   is turned off before the instance is kicked.
09/08/04   jqi     Added ps dormant optimization.
09/08/04   sj      Update subscription status on SS-HDR also.
09/08/04   sj      Do not map hdr timer expiry to SS-Main.
08/26/04   jqi     Lint fix.
08/20/04   sj      Added support for Auto A/B.
08/16/04   sj      For user_ss_pref2(), if SS is in pwr_save, don't optimise
                   on pref_update().
08/09/04   sj      Updated call to sdprl_nam_sel().
08/06/04   sj      Changed sd_ss_*_act_get() to return the action other than
                   CONTINUE.
08/03/04   sj      Converts user_nam_sel to user_pwr_up if not already in
                   online.
08/02/04   sj      Backed out Featurize baseid/lat/long/srch_win_n in ss_info.
07/30/04   jqi     Featureize baseid/lat/long/srch_win_n in ss_info.
                   Qualband optimization - eliminate scheduling.
07/26/04   sj      Changes for camped_ind support.
07/23/04   sj      Added support for baseid/lat/long/srch_win_n in ss_info.
07/15/04   sj      Updated orig_start handling for orig_start_ps &
                   orig_start_cs.
07/14/04   sj      Changed calls to update_srv_status to include srv_domain.
07/09/04   sj      Do not filter the first pref_sys changed due to ORIG_START.
                   Simplified filtering supported mode/band preferences.
06/30/04   jqi     Quad-band enhancement for get network requst.
                   More quad-band enhancement for manual mode.
06/07/04   jqi     More quad-band enhancement for automatic mode.
05/26/04   jqi     Revised support for CM_CAMPED_IND received from REG that
                   happens when phone is camped without completing a location
                   updates.
05/25/04   jqi     Fixed mode preference reset issue after power up.
05/19/04   jqi     Added quad-band enhancement to stay longer within a band
                   group prior to trying to acquire a new one when system is
                   lost.
           ka      Added support for CM_CAMPED_IND received from REG that happens
                   when phone is camped without completing a Loctaion Update.
05/17/04   sj      Updated support for BCMCS.
04/21/04   jqi     Completed two SS-action comparison for ACT GW.
                   Added band and mode message for gw acq sucess and srv info.
04/19/04   jqi     Lint fix - for error level : error.
04/13/04   sj      Added new SD indications for HDR conversation and release.
                   Reset acquisition throttle counters upon getting overhead
                   on 1x.
04/09/04   jqi     Filter unsupported bands prior to filter overlapped bands.
03/18/04   jqi     Introduced the true act type in sd_ss_s_type.
                   Kept band and mode consistent before passing to the GW
                   stack.
                   lint fix.
03/12/04   sj      Added debug messages for user_ss_pref2().
           jqi     Use service domain form service indicator for call orig.
03/10/04   jqi     Propagated service domain for call origination.
02/18/04   JQ      Complied band pref with MS Radio Access capability before
                   passing down to the stack.
02/16/04   SJ      Removed casting in the call to get_acc_prm().
02/06/04   SJ      If origination mode is MANUAL, and the script is requesting
                   limited service, set net_sel_type = LIMITED, to avoid
                   the UMTS/GSM stack to acquire full service on non Manual
                   PLMNs.
02/04/04   SJ      Added flag in acq_gw to indicate if orig is pending.
02/02/03   RC      Added a call to sd_si_ok_to_orig( SD_SS_MAIN ) in
                   sd_ss_ind_misc_orig_success().
02/01/04   SJ      Fixed issue wherein hybrid lists are not switched
                   correctly after OTASP.
01/27/04   SJ      Fixed issue with current acq. system when there is no SIM.
01/23/04   JQ      Passed down most recent acquired band group to GW stack
                   during early power scan.
01/22/04   JQ      Changed SDSR_GW_LTD_SYS and SDSR_USER_SYS to lists with
                   appropriate grouping of GW bands.
12/12/03   RC      Added support for FEATURE_INT_ROAMING
12/08/03   SJ      Fixed issue with lists switch upon transition from
                   hybrid -> non-hybrid when 1x was not acquired in hybrid
                   mode.
11/17/03   SJ      Updates for HDR usability and new hdr power save.
11/14/03   JQ      Fixes for IS683D support.
11/12/03   JQ      Use manual sel mode specified with undef plmn for power up
                   manual orig mode.
11/05/03   JQ      Do not error fatal while validating PRL.
                   More MRU3 updates.
10/31/03   RC      Added GSM/WCDMA support for fall-back to limited service.
10/31/03   JQ      Added support for latest IS-683D format changes.
10/24/03   RC      Changed SDSR_GW_LST to contain a single system of mode
                   SD_MODE_GW.
10/14/03   SJ      Added function sd_misc_add_mode_pref.
10/09/03   jqi     Adjusted band preference enum.
09/28/03   SJ      Added functions to determine if we are in hybrid opr,
                   sd_misc_is_hybr_opr().
09/05/03   SJ      Added functions sd_misc_is_mode_pref &
                   sd_misc_remove_mode_pref.
08/26/03   SJ      Added mode to ok_to_orig cb.
                   Added pwr save reason in act_pwr_save SS Action.
08/20/03   SJ      Added new SS-Indication sd_ss_ind_misc_cnf.
08/18/03   RC      Fixed issues with IS-683D implementation.
07/18/03   JQ      Added Extended PRL support
                   - D PRL
                   - Auto detection.
07/17/03   SJ      Added SI call back to inform clients registration was
                   done succesfully.
07/14/03   SJ      In sd_misc_get_curr_acq_sys_mode() use the mode_pref
                   if SD is not attempting acqusitions.
06/24/03   RC      Call sdsr_component_init() in sd_init().
06/23/03   RC      Enhanced HDR wildcard subnet-ID behavior.
06/10/03   RC      Added counter to track CDMA available indications.
06/09/03   RC      Consult subnet_mask_len in sd_ss_ind_hdr_opr_oh_info()
06/05/03   RC      Modify sd_ss_idle_sys_update() to update the idle system
                   when doing channel assignment (CAM) to CDMA paging.
05/23/03   RC      Fixed ss_timer setup to not cause too frequent expiration.
05/22/03   RC      In sd_ss_ind_cdma_opr_oh_info() handling band change same
                   as a SID/NID change.
04/28/03   RC      Added support for event logging.
04/23/03   SJ      Added early power scan support for GSM/WCDMA
04/04/03   SJ      Acquire limited service if SIM was invalidated.
03/04/03   SJ      Added support for limited automatic mode using API
                   sd_misc_get_curr_acq_sys_mode.
02/26/03   SJ      Added support for Band Class 8 & 9.
02/24/03   SJ      Turn service indicators off when pwr_down indication is
                   called.
02/06/03   SJ      Adjust mode_pref in acq_gw action according to phone
                   capabilities.
02/05/03   SJ      Fixed the issues with manual WCDMA plmn selection.
01/22/03   SJ      Fixed the issue with GSM/WCDMA service indicator.
01/16/03   SJ      Featurized dependency on DB & CAI for non 1x targets.
                   Featurized SS-Indications for CDMA.
12/09/02   RC      Added support for FEATURE_JCDMA.
12/03/02   SJ      Fixed merge issue with hybrid operation.
11/07/02   RC      Added support for new hybrid-HDR power-save algorithm.
10/15/02   RC      Copy PRL association fields over in OH_INFO, CAM and HDM.
09/03/02   RC      Removed optimization in sd_ss_ind_hdr_user_ss_pref() to
                   return CONTINUE if no SS-Preference is changed.
07/31/02   RC      Optimized GPS SS-Preference related changes.
07/19/02   RC      In sd_ss_ind_hdr_opr_oh_info() call on
                   sd_ss_hdr_event_proc() if doing CDMA+HDR hybrid operation.
05/17/02   RC      Added support for IS-683C, PN and data association fields.
03/08/02   hxw     Added GPS mode support for MSBased gpsOne.
02/08/02   mpa     Added support for logging.
02/06/02   RC      Eliminated compiler warnings.
01/25/02   sj      Added support for Tiered Services - User Zone feature.
01/23/02   RC      Added support for Rel. 3.0 CDMA-HDR hybrid operation.
12/12/01   RC      Added support for CDMA-HDR hybrid operation.
12/11/01   sj      Added "sd_si_roam_update" to set the roam indication on
                   serving system.
12/10/01   RC      Ignore HDR directed redirection if it is reported before
                   SD decided to stay on the system.
10/30/01   RC      Added support for IS-683C collcated systems.
10/03/01   RC      Treat sd_ss_ind_hdr_opr_ses_abort() as sync acquisition
                   failure if it received during HDR acquisition.
09/07/01   RC      Added support for T53 and JCDMA features.
07/20/01   RC      Added support for IS-683C.
04/12/01   RC      Changes to support HDR subnet ID.
03/27/01   RC      Changes to support tri-mode targets.
03/09/01   RC      Changes per 11th week of I&T.
02/07/01   RC      Changes per 9th week of I&T.
02/01/01   RC      Changes per 8th week of I&T.
01/12/01   RC      Changes per 7th week of I&T.
01/12/01   RC      Changes per 6th week of I&T.
01/03/01   RC      Changes per 4th week of I&T.
12/14/00   RC      Changes per 3rd week of I&T.
12/04/00   RC      Changes per 2nd week of I&T.
11/27/00   RC      Added a channel strength measurement done indicator.
11/17/00   RC      Initial release.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "sdcmd.h"


#include "mmcp_variation.h"
#include "customer.h" /* Customer configuration file */
#ifndef FEATURE_CUSTMMODE_H
#error  Need to pull in custmmode.h!!!
#endif

#include "sdtask.h"
#include "sd.h"       /* External interface to sd.c */
#include "sd_v.h"
#include "sdi.h"      /* Internal interface to sd.c */
#include "sdss.h"     /* External interface to sdss.c */
#include "sdsr.h"     /* External interface to sdsr.c (System Record)*/
#include "sdprl.h"    /* External interface to sdprl.c */
#ifdef FEATURE_MMSS_3_1
#include "sdmmss.h"   /* External interface to sdmmss.c */
#endif
#include "sdnv.h"     /* External interface to sdnv.c */
#include "sddbg.h"    /* SD debug macros */
#include "sdlog.h"    /* SD20 logging */
#include "prl.h"      /* External interface to prl.c */
#include "prl_v.h"
#include "sdssscr.h"  /* External interface to sdssscr.c */
#include "sys_v.h"

#include "comdef.h"   /* Definition for basic types and macros */
#include "msg.h"      /* Message output services */
#include "err.h"      /* Error and error fatal services */
#include "event.h"    /* Event reporting services */

#include "time_svc.h"      /* Interface to clock services */
#include "nv.h"       /* Interface to NV services */
#include "puzl_v.h"
#include "modem_mem.h"
#if( defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
#include "caii.h"     /* Internal IS-95 protocol data declaration */
#include "caii_v.h"
#endif

#ifdef SD_USES_DB_SERVICES
#include "db.h"       /* Interface to database services */
#endif

#include "prldiag.h"


#include "sdefs.h"    /* SD EFS interface */

#include <string.h>
#include "sys_eplmn_list.h"
#include "cm_v.h"


#ifdef FEATURE_MMODE_QTF
#error code not present
#endif /* FEATURE_MMODE_QTF */

#ifdef FEATURE_MMODE_QTF
#error code not present
#endif
#include <stringl/stringl.h>

/*lint -esym(528, sd_si_update_if_srv_is_off) */ /* func not referenced */
/*lint -esym(528, sd_ss_ind_gw_opr_srv_lost2) */ /* func not referenced */
/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/


/* This set of features must be defined for SD 2.0
*/

#if( defined(FEATURE_HDR_HYBRID) && !defined(FEATURE_HDR) )
#error "FEATURE_HDR must be defined for HDR Hybrid."
#endif

/*===========================================================================
=============================================================================
=============================================================================
=========================== FORWARD DECLARATIONS ============================
=============================================================================
=============================================================================
===========================================================================*/



//EXTERN sd_ss_s_type*             sd_ss_ptr(
//
 //       sd_ss_e_type              ss
  //          /* System selection item.
   //         */
//);


#ifdef FEATURE_EOOS_UMTS
static void sd_display_eoos_params (const sd_ss_act_acq_gwl_s_type* acq_gwl);
#endif

/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
============================ SERVICE INDICATORS =============================
=============================================================================
=============================================================================
===========================================================================*/



/* <EJECT> */

/*===========================================================================

FUNCTION sd_si_init

DESCRIPTION
  Initialize SD Service Indicators component.

  NOTE! This function must be called before any other sd_si_xxx() function is
  called. This function shall not be called a second time prior to calling on
  sd_term().

DEPENDENCIES
  This function must be called before any other sd_si_xxx() function is ever
  called. This function shall not be called a second time prior to calling on
  sd_term().

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                      sd_si_init(

        sd_ss_e_type              ss,
            /* System selection - MAIN or HDR.
            */

        sd_ss_s_type              *ss_ptr,
            /* Pointer to associated system selection item.
            */

        sdsr_e_type               si_sys
            /* Associated service indicator system.
            */
)
{
  sd_si_s_type          *si_ptr       = sd_si_ptr( ss );
  sd_si_info_s_type     *si_info_ptr  = &si_ptr->si_info;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );
  SD_ASSERT( ss_ptr != NULL );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the system selection item pointer.
  */
  si_ptr->ss_ptr = ss_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the service indicators system.
  */
  si_ptr->si_sys = si_sys;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize data suspend flag to FALSE.
  */
  si_ptr->data_suspend = FALSE;

  /* Initialize callback functions to NULL.
  */
  si_ptr->si_info_cb_func              = NULL;
  si_ptr->acq_fail_cb_func             = NULL;
  si_ptr->ok_to_orig_cb_func           = NULL;
  si_ptr->emerg_cb_mode_cb_func        = NULL;
  si_ptr->autonam_change_cb_func       = NULL;
  si_ptr->gw_net_list_cb_func          = NULL;
  si_ptr->sim_state_cb_func            = NULL;
  si_ptr->reg_done_cb_func             = NULL;
  si_ptr->data_suspend_cb_func         = NULL;
  si_ptr->si_prl_load_complete_cb_func = NULL;
  si_ptr->ss_stack_status_cb_func      = NULL;
  si_ptr->cdma_bsr_stat_chgd_cb_func   = NULL;
  si_ptr->cs_emerg_scan_fail_cb_func   = NULL;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize sib8 available to  NONE.
  */
  si_info_ptr->sib8_available      = SD_SIB8_NONE;

  si_info_ptr->lte_cs_capability   = SYS_LTE_CS_CAPABILITY_NONE;

  si_info_ptr->lte_ims_voice_avail = FALSE;

  /* initialize whether network supports emergency call over LTE
  */
  si_info_ptr->lte_ims_emerg_avail = SYS_IMS_LTE_EMERG_SUPP_UNKNOWN;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the mode to INACT on powerup.
  */
  si_info_ptr->mode = SD_MODE_INACT;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the usable status to usable by default.
  */
  si_info_ptr->usable_status = SD_SYS_USABLE_OK;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the service uptime to default
  */
  si_info_ptr->srv_acq_time = 0;

  /* Initialize the service lost uptime to default
  */
  si_info_ptr->srv_lost_time = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the HDR active prot revision
  */
  si_info_ptr->hdr_active_prot = SYS_ACTIVE_PROT_NONE;

  /* Initialize the HDR active personality
  */
  si_info_ptr->hdr_personality = SYS_PERSONALITY_NONE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Initialize the sys_mode in time info. if sys_mode has this value
  ** it means that no time info is available
  */
  si_info_ptr->time_info.sys_mode = SYS_SYS_MODE_NO_SRV;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the prl match flag
  */
  si_info_ptr->is_sys_prl_match = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the status of acquisition throttling.
  */
  si_info_ptr->is_hdr_activity = FALSE;

  /* Initialize stack status to inactive.
  */
  si_info_ptr->is_active = FALSE;

  /* Initialize is_gw_more_pref to NONE
  */
  si_info_ptr->is_more_pref_gwl_avail = PRL_GWL_PREFERRED_PREF_NONE;

  si_info_ptr->reg_domain = SYS_REG_DOMAIN_NOT_APPLICABLE;

  si_info_ptr->is_custom_home = FALSE;
  
  si_info_ptr->version_num = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize service to OFF. The service update condition is successful
  ** acquisition.
  */
  sd_si_srv_off_until( ss, SD_SI_UPDATE_ACQ );

  /* Initialize hdr_mcc_count and list */
  memset(&si_info_ptr->plmn_mcc_list, 0, sizeof(sys_plmn_mcc_list));

} /* sd_si_init */








#if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)

/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_gw_get_sim_state

DESCRIPTION
  Return sim_state of main stack to the SS-Client.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

  At least one sd_ss_ind_xxx() SS-Indication function must have already been
  called.

RETURN VALUE
  The sim_state that was most recently returned to the SS-Client.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sys_sim_state_e_type          sd_ss_gw_get_sim_state(
               sd_ss_e_type        ss
)
{

  return sdss_get_sim_state(ss);

} /* sd_ss_gw_get_sim_state */

#endif /* defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) */










/*===========================================================================
=============================================================================
=============================================================================
============================== SYSTEM SELECTION =============================
=============================================================================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION sd_ss_init

DESCRIPTION
  Initialize the SD System Selection component.

  NOTE! This function must be called before any other sd_ss_xxx() or
  sdss_xxx() function is ever called. This function shall not be called a
  second time prior to calling on sd_term().

  This function also registers an SS-Timer callback function with SS. SS
  calls on the provided callback function to set/disable the SS-Timer. Upon
  SS-Timer expiration the SS-Client should call on the sd_ss_ind_misc_timer()
  SS-Indication function.

  This function set the System Selection State (SS-State) to acquisition.

DEPENDENCIES
  This function must be called before any other sd_ss_xxx() or sdss_xxx()
  function is ever called. This function shall not be called a second time
  prior to calling on sd_term().

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                      sd_ss_init(

        sd_ss_s_type              *ss_ptr,
            /* Pointer to a system selection item.
            */

        void                      (*timer_func_ptr) (int4  ms)
            /* Pointer to an SS-Timer callback function.
            **
            ** SS calls on this function to set/disable the SS-Timer. Upon
            ** SS-Timer expiration the SS-Client should call on the
            ** sd_ss_ind_misc_timer() SS-Indication function.
            **
            ** The argument to this function is the time in ms with which
            ** to set the SS-Timer. An input of 0 should disable the
            ** SS-Timer */
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( ss_ptr == NULL )
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the system selection structure.
  */
  ss_ptr->act.act                   = SD_SS_ACT_MAX;
  ss_ptr->prev_act.act              = SD_SS_ACT_MAX;
  ss_ptr->true_curr_act.act         = SD_SS_ACT_MAX;
  ss_ptr->ss_state                  = SDSS_STATE_ACQ;

  ss_ptr->acq_list                  = SDSR_MAX;
  ss_ptr->acq_pos                   = SDSR_POS_NULL;
  ss_ptr->acq_mode                  = SDSS_ACQ_MODE_MAX;

  ss_ptr->meas_id                   = 0;
  ss_ptr->meas_is_done              = FALSE;
  ss_ptr->is_get_net_done           = TRUE;
  ss_ptr->meas_list_is_diff         = TRUE;
  if( timer_func_ptr != NULL )
  {
    ss_ptr->timer_func_ptr            = timer_func_ptr;
  }
  ss_ptr->ss_timer                  = 0;
  //ss_ptr->bad_point_update_uptime   = 0;
  #if ((defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)) && \
       !defined(FEATURE_MMODE_REMOVE_1X))
  ss_ptr->return_cause              = CAI_NORMAL_ACCESS_RETURN;
  #else
  ss_ptr->return_cause              = 0xff;
  #endif
  ss_ptr->gps_is_opti_ss_pref       = FALSE;
  ss_ptr->is_ss_locked              = FALSE;
  ss_ptr->is_start_instance         = FALSE;

  ss_ptr->hdr_act_mask              = SD_SS_HDR_ACT_MASK_NONE;

  ss_ptr->cdma_act_mask             = SD_SS_CDMA_ACT_MASK_NONE;

  sys_plmn_undefine_plmn_id(&ss_ptr->rplmn);

} /* sd_ss_init */


/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_reset

DESCRIPTION
  Reset the system selection front end and core

===========================================================================*/
static void sd_ss_reset( void )
{

  sd_ss_init( sd_ss_ptr(SD_SS_MAIN), NULL );

  #if defined FEATURE_HDR_HYBRID
  sd_ss_init( sd_ss_ptr(SD_SS_HYBR_HDR), NULL );
  #endif

  #if defined(FEATURE_MMODE_DUAL_SIM) || defined FEATURE_MMODE_SC_SVLTE ||\
      defined(FEATURE_MMODE_SC_SGLTE)
  sd_ss_init(sd_ss_ptr(SD_SS_HYBR_2), NULL);
  #endif

  #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
  sd_ss_init(sd_ss_ptr(SD_SS_HYBR_3), NULL);
  #endif
}

/*=============================================================================
  sd_ss_generic_cmd
    This is a function to execute a generic SD command, i.e. an SD command that
    takes no arguments.  All that is necessary in this case is to pass the
    pointer to the function to execute to carry out the command.
=============================================================================*/
static sd_ss_act_e_type sd_ss_generic_cmd(
   execute_fn       fn,             /* function to execute to carry out command */
   sd_ss_act_s_type *rtrn_act_ptr   /* pointer to storage for return action */
   )
{
  sd_ss_act_e_type  act;
  sd_cmd_t          *cmd;

  cmd = sdtask_sd_cmd_new(sizeof(sd_cmd_t), fn, rtrn_act_ptr, NULL);
  act = sdtask_queue_put_cmd_wait(cmd);
  ref_cnt_obj_release(cmd);

  return act;
}

/*=============================================================================
  sd_ss_generic_ss_cmd
    Similar to sd_ss_generic_cmd, but for a specific stack, which is passed.
=============================================================================*/
static sd_ss_act_e_type sd_ss_generic_ss_cmd(
   execute_fn       fn,             /* function to execute to carry out command */
   sd_ss_act_s_type *rtrn_act_ptr,  /* pointer to storage for return action */
   sd_ss_e_type     ss
   )
{
  sd_ss_act_e_type  act;
  sd_cmd_t          *cmd;

  cmd = sdtask_sd_cmd_new(sizeof(sd_cmd_t), fn, rtrn_act_ptr, NULL);
  cmd->ss = ss;
  act = sdtask_queue_put_cmd_wait(cmd);
  ref_cnt_obj_release(cmd);

  return act;
}


/*===========================================================================

FUNCTION sd_ss_act_get_per_ss

DESCRIPTION
  Wrapper function to return the latest SS-Action other than CONTINUE for
  the stack passed in as the argument.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

  At least one sd_ss_ind_xxx() SS-Indication function must have already been
  called.

RETURN VALUE
  The SS-Action on the stack passed in that was most recently returned to 
  the SS-Client.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN sd_ss_act_e_type          sd_ss_act_get_per_ss(

       sd_ss_act_s_type          *rtrn_act_ptr,
            /* Pointer to a buffer where to place returned SS-Action.
            */
       sd_ss_e_type              ss 
            /* the stack to get the last SS-Action from
            */
)
{
  sd_ss_act_e_type act = SD_SS_ACT_MAX;

  switch(ss)
  {
    case SD_SS_MAIN:
      act = sd_ss_act_get(rtrn_act_ptr);
      break;
    case SD_SS_HYBR_1:
      act = sd_ss_hybr_1_act_get(rtrn_act_ptr);
      break;
    case SD_SS_HYBR_2:
      act = sd_ss_hybr_2_act_get(rtrn_act_ptr);
      break;
    case SD_SS_HYBR_3:
      act = sd_ss_hybr_3_act_get(rtrn_act_ptr);
      break;
    default:
      SD_MSG_HIGH_1("invalid ss: %d", ss);
      break;
  }

  return act;
}

/*===========================================================================

FUNCTION sd_ss_act_get

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
EXTERN  sd_ss_act_e_type          sd_ss_act_get(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  #ifdef FEATURE_HYBR_3GPP
  return  sd_ss_hybr_2_act_get(rtrn_act_ptr);
  #elif defined(CFG_HYBR_GW_3)
  return  sd_ss_hybr_3_act_get(rtrn_act_ptr);
  #else

  sd_ss_act_e_type act = SD_SS_ACT_MAX;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If caller supplied a buffer, copy the most recent SS-Action into
  ** into the supplied buffer.
  */
  SD_CRIT_SECT_ENTER();
  act = sd_ss_ptr(SD_SS_MAIN)->true_curr_act.act;
  if( rtrn_act_ptr != NULL )
  {
    *rtrn_act_ptr = sd_ss_ptr(SD_SS_MAIN)->true_curr_act;
  }
  SD_CRIT_SECT_EXIT();
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

  #endif

} /* sd_ss_act_get */


/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_set_hdr_deact_by_hybr2_bsr

DESCRIPTION
  Set HDR_DEACT_BY_HYBR2_BSR flag to TRUE when mmoc received deact hdr due to GWL acq on
  hybr2 stack
  Flag will be reset when HDR service is lost due to acq fail pilot or acq fail sync.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  NONE

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void          sd_ss_set_hdr_deact_by_hybr2_bsr(
void
)
{

  if (sd_si_is_srv_on(SD_SS_HYBR_1) &&
     sdss_is_flag_set(SD_SS_HYBR_2, SDSS_FLAG_HYBR2_BSR))
  {
    sdss_set_flag(SD_SS_HYBR_2,SDSS_FLAG_HDR_DEACT_BY_HYBR2_BSR, TRUE);
  }
  
  return;
}



/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_hdr_act_get

DESCRIPTION
  Return to the HDR client the true SS-Action that was most recently returned to
  the SS-Client.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

  At least one sd_ss_ind_xxx() SS-Indication function must have already been
  called.

RETURN VALUE
  The SS-Action that was most recently returned to the SS-Client.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sd_ss_act_e_type          sd_ss_hdr_act_get(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type act = SD_SS_ACT_MAX;

  SYS_ARG_NOT_USED(act);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifndef FEATURE_HDR_HYBRID

  return sd_ss_act_get( rtrn_act_ptr );

  #else /* FEATURE_HDR_HYBRID */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* If currently not doing CDMA+HDR hybrid operation, call on
  ** sd_ss_act_get().
  */
  if( !sd_misc_is_hybr_opr() )
  {
    return sd_ss_act_get( rtrn_act_ptr );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Find last true act on HYBR HDR stack and return to caller
  */

  SD_CRIT_SECT_ENTER();
  act = sd_ss_ptr(SD_SS_HYBR_HDR)->true_curr_act.act;
  /* If caller supplied a buffer, copy the most recent SS-Action into
  ** into the supplied buffer.
  */
  if( rtrn_act_ptr != NULL )
  {
    *rtrn_act_ptr = sd_ss_ptr(SD_SS_HYBR_HDR)->true_curr_act;
  }
  SD_CRIT_SECT_EXIT();
  return act;

  #endif /* FEATURE_HDR_HYBRID */

} /* sd_ss_hdr_act_get */



/*===========================================================================

FUNCTION sd_ss_gw_act_get

DESCRIPTION
  Return to the client the true SS-Action that was most recently returned to
  the SS-Client on MAIN stack

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

  At least one sd_ss_ind_xxx() SS-Indication function must have already been
  called.

RETURN VALUE
  The SS-Action that was most recently returned to the SS-Client.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sd_ss_act_e_type          sd_ss_gw_act_get(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{


  return sd_ss_act_get( rtrn_act_ptr );

} /* sd_ss_hdr_act_get */


/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_gw_act_get_true_curr_act

DESCRIPTION
  Return to the HYBR_1 client the SS-Action that was most recently returned to
  the SS-Client.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

  At least one sd_ss_ind_xxx() SS-Indication function must have already been
  called.

RETURN VALUE
  The SS-Action that was most recently returned to the SS-Client.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sd_ss_act_e_type          sd_ss_gw_act_get_true_curr_act(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{

  return sd_ss_act_get_true_curr_act( rtrn_act_ptr );

} /* sd_ss_hybr_1_act_get_true_curr_act */


/*===========================================================================

FUNCTION sd_ss_hybr_1_act_get

DESCRIPTION
  Return to the HDR client the true SS-Action that was most recently returned to
  the SS-Client.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

  At least one sd_ss_ind_xxx() SS-Indication function must have already been
  called.

RETURN VALUE
  The SS-Action that was most recently returned to the SS-Client.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sd_ss_act_e_type          sd_ss_hybr_1_act_get(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{

  return sd_ss_hdr_act_get( rtrn_act_ptr );

} /* sd_ss_hdr_act_get */





#ifdef FEATURE_SD20_LOGGING
/*===========================================================================

FUNCTION sd_ss_log_last_action

DESCRIPTION
  Logs information about System Determination state and variables

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                      sd_ss_log_last_action(

        sd_ss_e_type              ss
            /* System selection - MAIN or HDR.
            */
)
{
  sd_si_s_type          *si_ptr     = sd_si_ptr( ss );
  sd_ss_s_type          *ss_ptr     = sd_ss_ptr( ss );
  sd_ss_act_s_type      ss_act;

  log_code_type         log_code    = LOG_SD20_LAST_ACTION_C;
    /* Code to be logged.
    */

  unsigned int          log_pkt_len = sizeof( LOG_SD20_LAST_ACTION_C_type );
    /* Length of packet to be logged.
    */

  LOG_SD20_LAST_ACTION_C_type *log_ptr  = NULL;
    /* log pointer to the log record type.
    */

  sdss_state_e_type     ss_state    = SDSS_STATE_MAX;
  sd_si_info_s_type    *si_info_ptr = &si_ptr->si_info;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );

  (void)sd_ss_act_get_true_curr_act_ss( ss, &ss_act);
  ss_state = sd_get_ss_state(ss);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* If logging for HDR, use appropriate log code.
  */
  if( ss == SD_SS_HYBR_HDR || ss == SD_SS_HYBR_1 )
  {
    log_code = LOG_SD20_LAST_ACTION_HYBRID_C;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we don't log an HDR UATI, we can log a 12 byte smaller packet).
  */
  if( si_info_ptr->mode != SD_MODE_HDR )
  {
    log_pkt_len -= 12;
  }

  /* Allocate a log packet.
  */
  log_ptr = (LOG_SD20_LAST_ACTION_C_type *) log_alloc_ex( log_code,
                                                          log_pkt_len );
  /* If allocation failed, just return.
  */
  if ( log_ptr == NULL )
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy SS-Action related information into the log packet.
  */
  log_ptr->act = ss_act.act;

  switch( ss_act.act )
  {
    case SD_SS_ACT_ACQ_CDMA:
      log_ptr->acq_band                 = ss_act.prm.acq_cdma.band;
      log_ptr->acq_channel              = ss_act.prm.acq_cdma.chan;
      log_ptr->act_info.acq_cdma.blksys = ss_act.prm.acq_cdma.blksys;
      log_ptr->act_info.acq_cdma.is_new_mode_or_blksys =
        ss_act.prm.acq_cdma.is_new_mode_or_blksys;
      break;

    case SD_SS_ACT_ACQ_AMPS:
      log_ptr->acq_band         = ss_act.prm.acq_amps.band;
      log_ptr->acq_channel      = ss_act.prm.acq_amps.sys;
      log_ptr->act_info.acq_amps.is_amps_2nd_cch =
        ss_act.prm.acq_amps.is_amps_2nd_cch;
      log_ptr->act_info.acq_amps.max_redirect_delay =
        ss_act.prm.acq_amps.max_redirect_delay;
      break;

    case SD_SS_ACT_ACQ_HDR:
      log_ptr->acq_band         = ss_act.prm.acq_hdr.band;
      log_ptr->acq_channel      = ss_act.prm.acq_hdr.chan;
      break;

    case SD_SS_ACT_MEAS_DED:
      log_ptr->act_info.ded_meas.meas_type = ss_act.prm.meas.type;
      log_ptr->act_info.ded_meas.meas_id   = ss_act.prm.meas.id;
      break;

    default:
      break;

  } /* switch */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy SS-State related information into the log packet.
  */
  if( ss_state == SDSS_STATE_ACQ )
  {
    switch (sdss_eng_stack_get_state( ss ))
    {
      case SDSS_ACQ_STT_START:
        log_ptr->sd_state = SD_LOG_STATE_ACQUISITION;
        break;

      case SDSS_ACQ_STT_MORE_PREF:
        log_ptr->sd_state = SD_LOG_STATE_ALT_ACQUISITION;
        break;

      case SDSS_ACQ_STT_REACQ:
        log_ptr->sd_state = SD_LOG_STATE_REACQUISITION;
        break;

      default:
        log_ptr->sd_state = SD_LOG_STATE_ACQUISITION;
        break;
    }
  }
  else if( ss_state == SDSS_STATE_PWR_SAVE )
  {
    log_ptr->sd_state = SD_LOG_STATE_PWR_SAVE;
  }
  else
  {
    log_ptr->sd_state = SD_LOG_STATE_OPERATION;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy SI-State related information into the log packet.
  */
  log_ptr->srv_is_on = sys_srv_status_is_srv(si_info_ptr->srv_status );
  log_ptr->sd_state_flags.is_redir =
         !sdss_is_redir(ss,SDSS_REDIR_VAL_NONE);
  log_ptr->sd_state_flags.is_ho = !sdss_is_handoff(SDSS_HO_VAL_NONE);
  log_ptr->sd_state_flags.is_reselection = (ss_ptr->ss_timer != 0);
  log_ptr->mode    = si_info_ptr->mode;
  log_ptr->band    = si_info_ptr->band;
  log_ptr->roam    = si_info_ptr->roam;
  log_ptr->channel = si_info_ptr->chan;

  if (si_info_ptr->mode == SD_MODE_HDR )
  {
    memscpy( (void*)(log_ptr->system_id.sector_id),
             sizeof(log_ptr->system_id.sector_id),
             (void*)(si_info_ptr->sid.is856.sector_id), 16 );
  }
  else
  {
    log_ptr->system_id.sid_nid.sid = si_info_ptr->sid.is95.sid;
    log_ptr->system_id.sid_nid.nid = si_info_ptr->sid.is95.nid;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Submit the log to DIAG.
  */
  log_commit( log_ptr );

} /* sd_ss_log_last_action */
#endif /* FEATURE_SD20_LOGGING */


/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_meas_list_get3

DESCRIPTION
  Extract
  mode, band pref and channel of each one of the systems that
  are on the measurement list (i.e., systems for which the channel strength
  measurement is requested)
  and
  UE RAT priority list.


DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  TRUE if list position <= position of last element on measurement list.
  FALSE if list position is > position of last element on measurement list.

SIDE EFFECTS
  None.

===========================================================================*/

EXTERN  boolean                   sd_ss_meas_list_get3(

        int                       pos,
            /* List position for which to return the mode band and channel.
            */

        sd_ss_mode_pref_e_type    *rtrn_mode_ptr,
            /* Pointer to a buffer where to copy the system's mode.
            */

        sd_ss_band_pref_e_type    *rtrn_band_ptr,
            /* Pointer to a buffer where to copy the system's band.
            */

        sd_chan_type              *rtrn_chan_ptr,
            /* Pointer to a buffer where to copy the system's chan.
            */

        sys_rat_pri_list_info_s_type *rat_pri_list_ptr
            /* Pointer to a buffer where to copy UE's RAT priority list.
            */
)
{
  sd_ss_act_e_type    act;
  sd_ss_meas_list_cmd *cmd;

  cmd = (sd_ss_meas_list_cmd *) sdtask_sd_cmd_new(
                                            sizeof(sd_ss_meas_list_cmd),
                                            (execute_fn) sdcmd_meas_list_get3,
                                            NULL,
                                            NULL
                                            );

  cmd->pos = pos;
  cmd->rtrn_mode_ptr = rtrn_mode_ptr;
  cmd->rtrn_band_ptr = rtrn_band_ptr;
  cmd->rtrn_chan_ptr = rtrn_chan_ptr;
  cmd->rat_pri_list_ptr = rat_pri_list_ptr;

  act = sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd);
  ref_cnt_obj_release(cmd);

  return act;
}


/* <EJECT> */


EXTERN  boolean                   sd_ss_hybr_2_meas_list_get2(

        int                       pos,
            /* List position for which to return the mode band and channel.
            */

        sd_ss_mode_pref_e_type    *rtrn_mode_ptr,
            /* Pointer to a buffer where to copy the system's mode.
            */

        sd_ss_band_pref_e_type    *rtrn_band_ptr,
            /* Pointer to a buffer where to copy the system's band.
            */

        sd_chan_type              *rtrn_chan_ptr,
            /* Pointer to a buffer where to copy the system's chan.
            */

        sys_rat_pri_list_info_s_type *rat_pri_list_ptr
            /* Pointer to a buffer where to copy UE's RAT priority list.
            */

)
{
  sd_ss_act_e_type    act;
  sd_ss_meas_list_cmd *cmd;

  cmd = (sd_ss_meas_list_cmd *) sdtask_sd_cmd_new(
                                            sizeof(sd_ss_meas_list_cmd),
                                            (execute_fn) sdcmd_hybr_2_meas_list_get2,
                                            NULL,
                                            NULL
                                            );

  cmd->pos = pos;
  cmd->rtrn_mode_ptr = rtrn_mode_ptr;
  cmd->rtrn_band_ptr = rtrn_band_ptr;
  cmd->rtrn_chan_ptr = rtrn_chan_ptr;
  cmd->rat_pri_list_ptr = rat_pri_list_ptr;

  act = sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd);
  ref_cnt_obj_release(cmd);

  return act;
}

/* <EJECT> */


EXTERN  boolean                   sd_ss_hybr_3_meas_list_get(

        int                       pos,
            /* List position for which to return the mode band and channel.
            */

        sd_ss_mode_pref_e_type    *rtrn_mode_ptr,
            /* Pointer to a buffer where to copy the system's mode.
            */

        sd_ss_band_pref_e_type    *rtrn_band_ptr,
            /* Pointer to a buffer where to copy the system's band.
            */

        sd_chan_type              *rtrn_chan_ptr,
            /* Pointer to a buffer where to copy the system's chan.
            */

        sys_rat_pri_list_info_s_type *rat_pri_list_ptr
            /* Pointer to a buffer where to copy UE's RAT priority list.
            */

)
{
  sd_ss_act_e_type    act;
  sd_ss_meas_list_cmd *cmd;

  cmd = (sd_ss_meas_list_cmd *) sdtask_sd_cmd_new(
                                            sizeof(sd_ss_meas_list_cmd),
                                            (execute_fn) sdcmd_hybr_3_meas_list_get,
                                            NULL,
                                            NULL
                                            );

  cmd->pos = pos;
  cmd->rtrn_mode_ptr = rtrn_mode_ptr;
  cmd->rtrn_band_ptr = rtrn_band_ptr;
  cmd->rtrn_chan_ptr = rtrn_chan_ptr;
  cmd->rat_pri_list_ptr = rat_pri_list_ptr;

  act = sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd);
  ref_cnt_obj_release(cmd);

  return act;
}

/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_meas_list_get

DESCRIPTION
  Extract the mode, band and channel of each one of the systems that are on
  the measurement list (i.e., systems for which the channel strength
  measurement is requested).

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  TRUE if list position <= position of last element on measurement list.
  FALSE if list position is > position of last element on measurement list.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  boolean                   sd_ss_meas_list_get(

        int                       pos,
            /* List position for which to return the mode band and channel.
            */

        sd_mode_e_type            *rtrn_mode_ptr,
            /* Pointer to a buffer where to copy the system's mode.
            */

        sd_band_e_type            *rtrn_band_ptr,
            /* Pointer to a buffer where to copy the system's band.
            */

        sd_chan_type              *rtrn_chan_ptr
            /* Pointer to a buffer where to copy the system's chan.
            */
)
{

  sd_ss_mode_pref_e_type mode_pref = SD_SS_MODE_PREF_NONE;
    /* mode_pref from sd_ss_meas_list_get2()
    */

  sd_ss_band_pref_e_type band_pref = SD_SS_BAND_PREF_NONE;
    /* band_pref from sd_ss_meas_list_get2()
    */
  sys_rat_pri_list_info_s_type rat_pri_list;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  SD_ASSERT( rtrn_mode_ptr != NULL );
  SD_ASSERT( rtrn_band_ptr != NULL );
  SD_ASSERT( rtrn_chan_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Calling new api for getting meas list
  */
  if (!sd_ss_meas_list_get3 ( pos,
                              &mode_pref,
                              &band_pref,
                              rtrn_chan_ptr,
                              &rat_pri_list
                            )
     )

  {
    SD_MSG_MED_1 ("sd_ss_meas_list_get3() FAILED pos %d", pos);

    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Convert mode and band_pref */

  *rtrn_mode_ptr = sdss_map_mode_pref_to_mode (mode_pref);

  *rtrn_band_ptr = sdss_map_band_pref_to_band (band_pref);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return TRUE;

} /* sd_ss_meas_list_get */

/*===========================================================================

FUNCTION sd_ss_meas_list_rprt

DESCRIPTION
  Report channel strength measurement for each one of the systems that are
  on the measurement list (i.e., systems for which the channel strength
  measurement is requested).

  Note that reported measurement values are in units of -1dB for Rx-power and
  -0.5dB for Pilot-strength.

  Note that a reported value of 255 implies that no measurement was obtained.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  TRUE if reported measurement ID matches the requested measurement ID and
  list position <= position of last system on the measurement list. FALSE
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/






EXTERN  boolean                   sd_ss_meas_list_rprt(

        int                       pos,
            /* List position for which measurement is reported.
            */

        int2                      meas_val
            /* Reported channel strength measurement value in units
            ** of -1dB for Rx-power and -0.5dB for Pilot strength. 255
            ** implies that no measurement was obtained.
            */
)
{
  sd_ss_act_e_type          act;
  sd_ss_meas_list_rprt_cmd  *cmd;

  cmd = (sd_ss_meas_list_rprt_cmd *) sdtask_sd_cmd_new(
                                            sizeof(sd_ss_meas_list_rprt_cmd),
                                            (execute_fn) sdcmd_meas_list_rprt,
                                            NULL,
                                            NULL
                                            );

  cmd->pos = pos;
  cmd->meas_val = meas_val;

  act = sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd);
  ref_cnt_obj_release(cmd);

  return act;
}


EXTERN  boolean                   sd_ss_hybr_2_meas_list_rprt(

        int                       pos,
            /* List position for which measurement is reported.
            */

        int2                      meas_val
            /* Reported channel strength measurement value in units
            ** of -1dB for Rx-power and -0.5dB for Pilot strength. 255
            ** implies that no measurement was obtained.
            */
)
{
  sd_ss_act_e_type          act;
  sd_ss_meas_list_rprt_cmd  *cmd;

  cmd = (sd_ss_meas_list_rprt_cmd *) sdtask_sd_cmd_new(
                                            sizeof(sd_ss_meas_list_rprt_cmd),
                                            (execute_fn) sdcmd_hybr_2_meas_list_rprt,
                                            NULL,
                                            NULL
                                            );

  cmd->pos = pos;
  cmd->meas_val = meas_val;

  act = sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd);
  ref_cnt_obj_release(cmd);

  return act;
}


EXTERN  boolean                   sd_ss_hybr_3_meas_list_rprt(

        int                       pos,
            /* List position for which measurement is reported.
            */

        int2                      meas_val
            /* Reported channel strength measurement value in units
            ** of -1dB for Rx-power and -0.5dB for Pilot strength. 255
            ** implies that no measurement was obtained.
            */
)
{
  sd_ss_act_e_type          act;
  sd_ss_meas_list_rprt_cmd  *cmd;

  cmd = (sd_ss_meas_list_rprt_cmd *) sdtask_sd_cmd_new(
                                            sizeof(sd_ss_meas_list_rprt_cmd),
                                            (execute_fn) sdcmd_hybr_3_meas_list_rprt,
                                            NULL,
                                            NULL
                                            );

  cmd->pos = pos;
  cmd->meas_val = meas_val;

  act = sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd);
  ref_cnt_obj_release(cmd);

  return act;
}






/* <EJECT> */
/*===========================================================================

FUNCTION sd_get_band_mode

DESCRIPTION
  Filter out the band and mode not supported by the target capability.
  Filter out the band and mode that are not in part of system preference.
  Filter out any overlapped bands.
  Bands that do not have corresponding modes.
  Modes that do not have corresponding bands.

  MODE
DEPENDENCIES
  None

RETURN VALUE
  Return the filtered mode and band preference as described.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                      sd_get_band_mode(

        sd_ss_mode_pref_e_type    *mode_ptr,
            /* Pointer to the mode preference that needs to be filtered.
            */

        sd_ss_band_pref_e_type    *band_ptr,
            /* Pointer to the band preference that needs to be filtered.
            */

        sd_ss_mode_pref_e_type    sys_mode_pref,
            /* Enumeration of system mode perference.
            */

        sd_ss_band_pref_e_type    sys_band_pref
            /* Enumeration of system selection band and sub-band preferences.
            */
)
{
  sd_ss_mode_pref_e_type          mode_pref;
  sd_ss_band_pref_e_type          band_pref;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( mode_ptr !=NULL );
  SD_ASSERT( band_ptr !=NULL );

  mode_pref = *mode_ptr;
  band_pref = *band_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Filter out modes that are not part of the mode preference.
  */
  mode_pref = SD_GET_COMMON_MODE( mode_pref, sys_mode_pref );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Filter out bands that are not in the band preference.
  */
  band_pref = SD_GET_COMMON_BAND( band_pref, sys_band_pref );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Filter out:
  ** 1. Bands that do not have corresponding modes.
  ** 2. Modes that do not have corresponding bands.
  */
  mode_pref = sdss_get_supp_mode_pref(mode_pref, band_pref);
  band_pref = sdss_get_supp_mode_band_pref( mode_pref, band_pref );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Filter out overlapping bands.
  */
  band_pref = sd_filter_out_overlap_band( band_pref );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Copy the filtered mode and band back into the input parameters.
  */
  *mode_ptr = mode_pref;
  *band_ptr = band_pref;

} /* sd_get_gw_band_mode */









/* <EJECT> */
/*===========================================================================
=============================================================================
============================ USER SS-INDICATIONS ============================
=============================================================================
===========================================================================*/

/* SS-Indications that are a direct result of a user action, such as system
** preference change or NAM selection change. These indications are usually
** mode independent; or in other words, they can be received in more than one
** of the SS-Client operating modes (e.g. CDMA, AMPS or PWR_SAVE).
*/





/*===========================================================================

FUNCTION sd_ss_ind_user_pwr_up

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the user commands the
  MS to power-up (from OFF or from LPM).

  Updates local SD data as necessary and call on the SS-Core to process the
  SS-Event that is associated with this SS-Indication.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GWL  -  Acquire a GSM/WCDMA/LTE/TD-SCDMA system as specified
                          by the returned "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sd_ss_act_e_type          sd_ss_ind_user_pwr_up(
        /*lint -esym(715,is_cdma_unlock) *//**< param not referenced in CFG_HYBR_GW */
        boolean                   is_cdma_unlock,
            /* Indicate if CDMA unlock needs to be performed
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_ind_user_pwr_up_cmd *cmd;
  sd_ss_act_e_type          act;

  cmd = (sd_ss_ind_user_pwr_up_cmd *) sdtask_sd_cmd_new(
                                        sizeof(sd_ss_ind_user_pwr_up_cmd),
                                        (execute_fn) sdcmd_user_pwr_up,
                                        rtrn_act_ptr,
                                        NULL
                                        );

  cmd->is_cdma_unlock = is_cdma_unlock;

  act = sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd);
  ref_cnt_obj_release(cmd);

  return act;
}


/*===========================================================================

FUNCTION sd_ss_ind_hybr_2_user_pwr_up

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the user commands the
  MS to power-up (from OFF or from LPM).

  Updates local SD data as necessary and call on the SS-Core to process the
  SS-Event that is associated with this SS-Indication.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GW  -  Attempt acquiring an GSM/WCDMA system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sd_ss_act_e_type          sd_ss_ind_hybr_2_user_pwr_up(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_ind_user_pwr_up_cmd *cmd;
  sd_ss_act_e_type          act;

  cmd = (sd_ss_ind_user_pwr_up_cmd *) sdtask_sd_cmd_new(
                                        sizeof(sd_ss_ind_user_pwr_up_cmd),
                                        (execute_fn) sdcmd_hybr_2_user_pwr_up,
                                        rtrn_act_ptr,
                                        NULL
                                        );
  cmd->is_cdma_unlock = FALSE;

  act = sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd);
  ref_cnt_obj_release(cmd);

  return act;
}

#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
/*===========================================================================

FUNCTION sd_ss_ind_hybr_3_user_pwr_up

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the user commands the
  MS to power-up (from OFF or from LPM).

  Updates local SD data as necessary and call on the SS-Core to process the
  SS-Event that is associated with this SS-Indication.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GW  -  Attempt acquiring an GSM/WCDMA system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sd_ss_act_e_type          sd_ss_ind_hybr_3_user_pwr_up(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_ind_user_pwr_up_cmd *cmd;
  sd_ss_act_e_type          act;

  cmd = (sd_ss_ind_user_pwr_up_cmd *) sdtask_sd_cmd_new(
                                        sizeof(sd_ss_ind_user_pwr_up_cmd),
                                        (execute_fn) sdcmd_hybr_3_user_pwr_up,
                                        rtrn_act_ptr,
                                        NULL
                                        );
  cmd->is_cdma_unlock = FALSE;

  act = sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd);
  ref_cnt_obj_release(cmd);

  return act;
}

#endif //defined FEATURE_MMODE_DUAL_SIM || defined FEATURE_MMODE_SC_SVLTE


EXTERN  sd_ss_act_e_type          sd_ss_ind_user_offline_cdma(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  return sd_ss_generic_cmd(sdcmd_user_offline_cdma, rtrn_act_ptr);
}


/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_user_offline_amps

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the user commands the
  MS to enter offline AMPS mode of operation.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with AMPS offline operation.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sd_ss_act_e_type          sd_ss_ind_user_offline_amps(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type    act;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Call on the SS-Core to update its operating mode.
  */
  sdss_opr_mode_update( SD_SS_MAIN,SDSS_OPR_MODE_OFFLINE_AMPS );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Inform the System Indication component that the MS is going offline-
  ** AMPS.
  */
  sd_si_srv_offline( SDSS_OPR_MODE_OFFLINE_AMPS );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Save PRL-Component related information to NV.
  */
  sdprl_save();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_USER_OFFLINE_AMPS, rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act == SD_SS_ACT_CONTINUE );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_user_offline_amps */


EXTERN  sd_ss_act_e_type          sd_ss_ind_hybr_2_user_prot_deactivate(

        sd_ss_act_s_type          *rtrn_act_ptr,
            /**< Pointer to a buffer where to place returned SS-Action.
            */
        sd_ss_prot_deact_adtnl_info_e_type adtnl_info
            /**< Additional information that caller can provide for special handling.
            */
)
{
  sd_ss_ind_user_prot_deactivate_cmd  *cmd;
  sd_ss_act_e_type            act;

  cmd = (sd_ss_ind_user_prot_deactivate_cmd *)
         sdtask_sd_cmd_new( sizeof(sd_ss_ind_user_prot_deactivate_cmd),
                            (execute_fn) sdcmd_hybr_2_user_prot_deactivate,
                            rtrn_act_ptr,
                            NULL );

  cmd->adtnl_info = adtnl_info;

  act = sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd);
  ref_cnt_obj_release(cmd);

  return act;
}

#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)

EXTERN  sd_ss_act_e_type          sd_ss_ind_hybr_3_user_prot_deactivate(

        sd_ss_act_s_type          *rtrn_act_ptr,
            /**< Pointer to a buffer where to place returned SS-Action.
            */
        sd_ss_prot_deact_adtnl_info_e_type adtnl_info
            /**< Additional information that caller can provide for special handling.
            */
)
{
  sd_ss_ind_user_prot_deactivate_cmd  *cmd;
  sd_ss_act_e_type            act;

  cmd = (sd_ss_ind_user_prot_deactivate_cmd *)
         sdtask_sd_cmd_new( sizeof(sd_ss_ind_user_prot_deactivate_cmd),
                            (execute_fn) sdcmd_hybr_3_user_prot_deactivate,
                            rtrn_act_ptr,
                            NULL );

  cmd->adtnl_info = adtnl_info;

  act = sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd);
  ref_cnt_obj_release(cmd);

  return act;
}
#endif


EXTERN  sd_ss_act_e_type          sd_ss_ind_user_prot_deactivate(

        sd_ss_act_s_type          *rtrn_act_ptr,
            /**< Pointer to a buffer where to place returned SS-Action.
            */
        sd_ss_prot_deact_adtnl_info_e_type adtnl_info
            /**< Additional information that caller can provide for special handling.
            */
)
{
  sd_ss_ind_user_prot_deactivate_cmd  *cmd;
  sd_ss_act_e_type            act;

  cmd = (sd_ss_ind_user_prot_deactivate_cmd *)
        sdtask_sd_cmd_new( sizeof(sd_ss_ind_user_prot_deactivate_cmd),
                           (execute_fn) sdcmd_user_prot_deactivate,
                           rtrn_act_ptr,
                           NULL );

  cmd->adtnl_info = adtnl_info;

  act = sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd);
  ref_cnt_obj_release(cmd);

  return act;
}


/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_user_pwr_save_exit

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the user commands the
  MS to exit power save mode of operation.

  Note that the UI is the one to determine when the user wants the phone to
  exit power save mode of operation. Typically, however, the UI interprets
  any key press (while MS in power-save) as indication that the user wants
  the MS to exit power-save mode and to start searching for systems actively.

  Note that the actual SD behavior when getting this indication from the
  SS-Client is controlled by the SS-Script for this indication. This script,
  however, is expected to be fairly similar to the power-up script.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GWL  -  Acquire a GSM/WCDMA/LTE/TD-SCDMA system as specified
                          by the returned "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Go offline. SD instructs SS-Client to go right back
                          into power-save if the current SS-Preference
                          forbids any PRL system from being attempted
                          acquisition.

SIDE EFFECTS
  None.

===========================================================================*/



EXTERN  sd_ss_act_e_type          sd_ss_ind_user_pwr_save_exit(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  return sd_ss_generic_cmd(sdcmd_user_pwr_save_exit, rtrn_act_ptr);
}



/*===========================================================================

FUNCTION sd_ss_ind_user_ss_pref7

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the user changes the
  System Selection Preference (SS-Preference).

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GWL  -  Acquire a GSM/WCDMA/LTE/TD-SCDMA system as specified
                          by the returned "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/







/*===========================================================================

FUNCTION sd_ss_ind_user_ss_pref7

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the user changes the
  System Selection Preference (SS-Preference).

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GWL  -  Acquire a GSM/WCDMA/LTE/TD-SCDMA system as specified
                          by the returned "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sd_ss_act_e_type                    sd_ss_ind_user_ss_pref7(

        sd_ss_pref_reas_e_type              pref_reason,
            /* Reason for changing the SS-Preference.
            */

        sd_ss_orig_mode_e_type              orig_mode,
            /* New origination mode (normal, OTASP, emergency, other, etc.).
            */

        sd_ss_mode_pref_e_type              mode_pref,
            /* New mode preference (AMPS, CDMA, any, etc.).
            */

        sd_ss_band_pref_e_type              band_pref,
            /* New band preference (BC0, BC1, any, etc.)
            */
        sys_lte_band_mask_e_type            band_pref_lte,
            /* LTE band preference (BC0, BC1, any, etc.)
            */

        sd_ss_band_pref_e_type              band_pref_tds,
            /* TDS band preference
            */

        sd_ss_prl_pref_e_type               prl_pref,
            /* New prl system preference (Available BC0A, available BC0B, etc.)
            */

        sd_ss_roam_pref_e_type              roam_pref,
            /* New roam preference (any, home, affiliated, etc.)
            */

        sd_ss_hybr_pref_e_type              hybr_pref,
            /* New hybrid preference (none, CDMA/HDR).
            */

        sd_band_e_type                      otasp_band,
            /* The band-class that is associated with OTASP origination mode.
            ** Note that this parameter is ignored if orig_mode != OTASP.
            */

        sd_blksys_e_type                    otasp_blksys,
            /* The PCS frequency block/Cellular system that is associated
            ** with OTASP origination mode. Note that this parameter is
            ** ignored if orig_mode != OTASP.
            */

        sd_ss_avoid_sys_e_type              avoid_type,
            /* Type of the system to be avoided, valid only if
            ** pref_reason = SD_SS_PREF_REAS_AVOID_SYS.
            */

        dword                               avoid_time,
            /* Time in seconds for which the system is to be avoided,
            ** valid only if pref_reason = SD_SS_PREF_REAS_AVOID_SYS.
            */

        const sys_manual_sys_info_s_type    *manual_sys_info_ptr,
          /* Pointer to struct that defines the manual system.
           */

        sd_ss_srv_domain_pref_e_type        domain_pref,
           /* Service domain preference that is associated with the selected
           ** NAM.
           ** Applies only for GSM/WCDMA/LTE modes.
           */

        sd_ss_acq_order_pref_e_type         acq_order_pref,
           /* Preference for the order of acquisition ( WCDMA before GSM,
           ** GSM before WCDMA etc).
           */

        sd_ss_pref_update_reason_e_type     pref_update_reason,
            /**Prefernce change reason
            */
        sd_ss_mode_pref_e_type              user_mode_pref,
            /**< The user/phone mode preference
            */

        const sd_rat_acq_order_s_type       *rat_acq_order_ptr,
           /**< Rat acquisition order including LTE.
           */

        uint16                               req_id,
          /* Request id
          */

        sd_ss_pref_camp_mode_e_type          camp_mode,
        /**< CAMPED_ONLY mode. Whether or not UE will register with network.
        ** Specific for SGLTE.
        */

       sys_csg_id_type                      csg_id,
          /* CSG identifier
          */

       sys_radio_access_tech_e_type        csg_rat,
          /* RAT specified for CSG
           */

        sys_voice_domain_pref_e_type          voice_domain_pref,
          /* Voice domain pref
          */

        sd_ss_act_s_type                    *rtrn_act_ptr
           /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  execute_fn                  fn;
  sd_ss_ind_user_ss_pref_cmd  *cmd;
  sd_ss_act_e_type            act;

  #ifdef FEATURE_HYBR_3GPP
  fn = (execute_fn) sdcmd_hybr_2_user_ss_pref5;
  #elif defined(CFG_HYBR_GW_3)
  fn = (execute_fn) sdcmd_hybr_3_user_ss_pref;
  #else
  fn = (execute_fn) sdcmd_user_ss_pref7;
  #endif

  cmd = (sd_ss_ind_user_ss_pref_cmd *) sdtask_sd_cmd_new(
                                          sizeof(sd_ss_ind_user_ss_pref_cmd),
                                          fn,
                                          rtrn_act_ptr,
                                          NULL
                                          );

  cmd->pref_reason = pref_reason;
  cmd->orig_mode = orig_mode;
  cmd->mode_pref = mode_pref;
  cmd->band_pref = band_pref;
  cmd->band_pref_lte = band_pref_lte;
  cmd->band_pref_tds = band_pref_tds;
  cmd->prl_pref = prl_pref;
  cmd->roam_pref = roam_pref;
  cmd->hybr_pref = hybr_pref;
  cmd->otasp_band = otasp_band;
  cmd->otasp_blksys = otasp_blksys;
  cmd->avoid_type = avoid_type;
  cmd->avoid_time = avoid_time;
  cmd->manual_sys_info_ptr = manual_sys_info_ptr;
  cmd->domain_pref = domain_pref;
  cmd->acq_order_pref = acq_order_pref;
  cmd->pref_update_reason = pref_update_reason;
  cmd->user_mode_pref = user_mode_pref;
  cmd->rat_acq_order_ptr = rat_acq_order_ptr;
  cmd->req_id = req_id;
  cmd->camp_mode = camp_mode;
  cmd->csg_id = csg_id;
  cmd->csg_rat=csg_rat;
  cmd->voice_domain_pref = voice_domain_pref;

  act = sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd);
  ref_cnt_obj_release(cmd);

#ifdef FEATURE_AVOID_DUP_BAND_SCAN
  if( !sdss_is_orig_mode0(orig_mode,SD_SS_ORIG_MODE_VAL_EMERG_ORIG))
  {
    /* when non emergency orig mode, prefs are forced, that time emerg dup band info can be cleared */
    sdss_reset_emerg_band_to_avoid(SD_SS_MAIN);
  }
#endif /*FEATURE_AVOID_DUP_BAND_SCAN */


  return act;
}





EXTERN  sd_ss_act_e_type          sd_ss_ind_user_nam_sel(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  return sd_ss_generic_cmd(sdcmd_user_nam_sel, rtrn_act_ptr);
}




EXTERN  sd_ss_act_e_type          sd_ss_ind_user_pwr_down(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  return sd_ss_generic_cmd(sdcmd_user_pwr_down, rtrn_act_ptr);
}


/*===========================================================================

FUNCTION sd_ss_ind_hybr_2_user_ss_pref5

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the user changes the
  System Selection Preference (SS-Preference).

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GW  -  Attempt acquiring an GSM/WCDMA system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sd_ss_act_e_type                    sd_ss_ind_hybr_2_user_ss_pref5(

        sd_ss_pref_reas_e_type              pref_reason,
            /* Reason for changing the SS-Preference.
            */

        sd_ss_orig_mode_e_type              orig_mode,
            /* New origination mode (normal, OTASP, emergency, other, etc.).
            */

        sd_ss_mode_pref_e_type    mode_pref,
            /* New mode preference (AMPS, CDMA, any, etc.).
           */

        sd_ss_band_pref_e_type    band_pref,
            /* New band preference (BC0, BC1, any, etc.)
            */

        sys_lte_band_mask_e_type            lte_band_pref,
            /* New LTE band preference
            */

        sd_ss_band_pref_e_type              tds_band_pref,
            /* New TDS band preference
            */

        sd_ss_prl_pref_e_type               prl_pref,
            /* New prl system preference (Available BC0A, available BC0B, etc.)
            */

        sd_ss_roam_pref_e_type              roam_pref,
            /* New roam preference (any, home, affiliated, etc.)
            */

        sd_ss_hybr_pref_e_type              hybr_pref,
            /* New hybrid preference (none, CDMA/HDR).
            */

        sd_band_e_type                      otasp_band,
            /* The band-class that is associated with OTASP origination mode.
            ** Note that this parameter is ignored if orig_mode != OTASP.
            */

        sd_blksys_e_type                    otasp_blksys,
            /* The PCS frequency block/Cellular system that is associated
            ** with OTASP origination mode. Note that this parameter is
            ** ignored if orig_mode != OTASP.
            */

        sd_ss_avoid_sys_e_type              avoid_type,
            /* Type of the system to be avoided, valid only if
            ** pref_reason = SD_SS_PREF_REAS_AVOID_SYS.
            */

        dword                               avoid_time,
            /* Time in seconds for which the system is to be avoided,
            ** valid only if pref_reason = SD_SS_PREF_REAS_AVOID_SYS.
           */

        const sys_manual_sys_info_s_type    *manual_sys_info_ptr,
          /* Pointer to struct that defines the manual system.
            */

        sd_ss_srv_domain_pref_e_type        domain_pref,
           /* Service domain preference that is associated with the selected
           ** NAM.
           ** Applies only for GSM/WCDMA modes.
            */

        sd_ss_acq_order_pref_e_type         acq_order_pref,
           /* Preference for the order of acquisition ( WCDMA before GSM,
           ** GSM before WCDMA etc).
           */

        sd_ss_pref_update_reason_e_type     pref_update_reason,
           /**Prefernce change reason
           */

        const sd_rat_acq_order_s_type       *rat_acq_order_ptr,
           /**< Rat acquisition order including LTE.
           */

        sd_ss_mode_pref_e_type              user_mode_pref,
            /**< The user/phone mode preference
            */

        uint16                              req_id,
          /* Request Id
          */

        sys_csg_id_type                     csg_id,
            /* CSG Id
            */

        sys_radio_access_tech_e_type        csg_rat,
           /* RAT specified for CSG
           */

        sys_voice_domain_pref_e_type         voice_domain_pref,
           /* Voice domain pref
           */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_ind_user_ss_pref_cmd  *cmd;
  sd_ss_act_e_type            act;

  cmd = (sd_ss_ind_user_ss_pref_cmd *) sdtask_sd_cmd_new(
                                          sizeof(sd_ss_ind_user_ss_pref_cmd),
                                          (execute_fn) sdcmd_hybr_2_user_ss_pref5,
                                          rtrn_act_ptr,
                                          NULL
                                          );

  cmd->pref_reason = pref_reason;
  cmd->orig_mode = orig_mode;
  cmd->mode_pref = mode_pref;
  cmd->band_pref = band_pref;
  cmd->band_pref_lte = lte_band_pref;
  cmd->band_pref_tds = tds_band_pref;
  cmd->prl_pref = prl_pref;
  cmd->roam_pref = roam_pref;
  cmd->hybr_pref = hybr_pref;
  cmd->otasp_band = otasp_band;
  cmd->otasp_blksys = otasp_blksys;
  cmd->avoid_type = avoid_type;
  cmd->avoid_time = avoid_time;
  cmd->manual_sys_info_ptr = manual_sys_info_ptr;
  cmd->domain_pref = domain_pref;
  cmd->acq_order_pref = acq_order_pref;
  cmd->pref_update_reason = pref_update_reason;
  cmd->user_mode_pref = user_mode_pref;
  cmd->rat_acq_order_ptr = rat_acq_order_ptr;
  cmd->req_id = req_id;
  cmd->csg_id = csg_id;
  cmd->csg_rat = csg_rat;
  cmd->voice_domain_pref = voice_domain_pref;

  act = sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd);
  ref_cnt_obj_release(cmd);

  #ifdef FEATURE_AVOID_DUP_BAND_SCAN
  if( !sdss_is_orig_mode0(orig_mode,SD_SS_ORIG_MODE_VAL_EMERG_ORIG))
  {
    /* when non emergency orig mode, prefs are forced, that time emerg dup band info can be cleared */
    sdss_reset_emerg_band_to_avoid(SD_SS_HYBR_2);
  }
  #endif /*FEATURE_AVOID_DUP_BAND_SCAN */

  return act;

}
#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
/*===========================================================================

FUNCTION sd_ss_ind_hybr_3_user_ss_pref

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the user changes the
  System Selection Preference (SS-Preference).

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GW  -  Attempt acquiring an GSM/WCDMA system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sd_ss_act_e_type                    sd_ss_ind_hybr_3_user_ss_pref(

        sd_ss_pref_reas_e_type              pref_reason,
            /* Reason for changing the SS-Preference.
            */

        sd_ss_orig_mode_e_type              orig_mode,
            /* New origination mode (normal, OTASP, emergency, other, etc.).
            */

        sd_ss_mode_pref_e_type    mode_pref,
            /* New mode preference (AMPS, CDMA, any, etc.).
           */

        sd_ss_band_pref_e_type    band_pref,
            /* New band preference (BC0, BC1, any, etc.)
            */

        sys_lte_band_mask_e_type              lte_band_pref,
            /* New LTE band preference
            */

        sd_ss_band_pref_e_type              tds_band_pref,
            /* New TDS band preference
            */

        sd_ss_prl_pref_e_type               prl_pref,
            /* New prl system preference (Available BC0A, available BC0B, etc.)
            */

        sd_ss_roam_pref_e_type              roam_pref,
            /* New roam preference (any, home, affiliated, etc.)
            */

        sd_ss_hybr_pref_e_type              hybr_pref,
            /* New hybrid preference (none, CDMA/HDR).
            */

        sd_band_e_type                      otasp_band,
            /* The band-class that is associated with OTASP origination mode.
            ** Note that this parameter is ignored if orig_mode != OTASP.
            */

        sd_blksys_e_type                    otasp_blksys,
            /* The PCS frequency block/Cellular system that is associated
            ** with OTASP origination mode. Note that this parameter is
            ** ignored if orig_mode != OTASP.
            */

        sd_ss_avoid_sys_e_type              avoid_type,
            /* Type of the system to be avoided, valid only if
            ** pref_reason = SD_SS_PREF_REAS_AVOID_SYS.
            */

        dword                               avoid_time,
            /* Time in seconds for which the system is to be avoided,
            ** valid only if pref_reason = SD_SS_PREF_REAS_AVOID_SYS.
           */

        const sys_manual_sys_info_s_type    *manual_sys_info_ptr,
          /* Pointer to struct that defines the manual system.
            */

        sd_ss_srv_domain_pref_e_type        domain_pref,
           /* Service domain preference that is associated with the selected
           ** NAM.
           ** Applies only for GSM/WCDMA modes.
            */

        sd_ss_acq_order_pref_e_type         acq_order_pref,
           /* Preference for the order of acquisition ( WCDMA before GSM,
           ** GSM before WCDMA etc).
           */

        sd_ss_pref_update_reason_e_type     pref_update_reason,
           /**Prefernce change reason
           */

        const sd_rat_acq_order_s_type       *rat_acq_order_ptr,
           /**< Rat acquisition order including LTE.
           */

        sd_ss_mode_pref_e_type              user_mode_pref,
            /**< The user/phone mode preference
            */

        uint16                              req_id,
          /* Request Id
          */

        sys_csg_id_type                     csg_id,
            /* CSG Id
            */

        sys_radio_access_tech_e_type        csg_rat,
           /* RAT specified for CSG
           */

        sys_voice_domain_pref_e_type         voice_domain_pref,
           /* Voice domain pref
           */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_ind_user_ss_pref_cmd  *cmd;
  sd_ss_act_e_type            act;

  cmd = (sd_ss_ind_user_ss_pref_cmd *) sdtask_sd_cmd_new(
                                          sizeof(sd_ss_ind_user_ss_pref_cmd),
                                          (execute_fn) sdcmd_hybr_3_user_ss_pref,
                                          rtrn_act_ptr,
                                          NULL
                                          );

  cmd->pref_reason = pref_reason;
  cmd->orig_mode = orig_mode;
  cmd->mode_pref = mode_pref;
  cmd->band_pref = band_pref;
  cmd->band_pref_lte = lte_band_pref;
  cmd->band_pref_tds = tds_band_pref;
  cmd->prl_pref = prl_pref;
  cmd->roam_pref = roam_pref;
  cmd->hybr_pref = hybr_pref;
  cmd->otasp_band = otasp_band;
  cmd->otasp_blksys = otasp_blksys;
  cmd->avoid_type = avoid_type;
  cmd->avoid_time = avoid_time;
  cmd->manual_sys_info_ptr = manual_sys_info_ptr;
  cmd->domain_pref = domain_pref;
  cmd->acq_order_pref = acq_order_pref;
  cmd->pref_update_reason = pref_update_reason;
  cmd->user_mode_pref = user_mode_pref;
  cmd->rat_acq_order_ptr = rat_acq_order_ptr;
  cmd->req_id = req_id;
  cmd->csg_id = csg_id;
  cmd->csg_rat = csg_rat;
  cmd->voice_domain_pref = voice_domain_pref;

  act = sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd);
  ref_cnt_obj_release(cmd);

 #ifdef FEATURE_AVOID_DUP_BAND_SCAN
  if( !sdss_is_orig_mode0(orig_mode,SD_SS_ORIG_MODE_VAL_EMERG_ORIG))
  {
    /* when non emergency orig mode, prefs are forced, that time emerg dup band info can be cleared */
    sdss_reset_emerg_band_to_avoid(SD_SS_HYBR_3);
  }
  #endif /* FEATURE_AVOID_DUP_BAND_SCAN */

  return act;

}

#endif
#if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)
/* ^L<EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_user_get_net_gw_ss

DESCRIPTION
  Inform SD that the user commanded to get the available networks on GSM
  WCDMA, and/or TD-SCDMA

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_GET_NET_GW - Get the available GSM, WCDMA and/or
                           TD-SCDMA systems.

    SD_SS_ACT_CONTINUE   - Continue with current mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/





EXTERN  sd_ss_act_e_type          sd_ss_ind_user_get_net_gw_ss(

        sd_ss_mode_pref_e_type    mode_pref,
           /* Mode preference to use while obtaining the available
           ** GSM/WCDMA/LTE/TD-SCDMA networks.
           */

        sd_ss_band_pref_e_type    band_pref,
           /* Band preference to use while obtaining the available GSM/WCDMA/LTE
           ** networks.
           */

        sys_lte_band_mask_e_type    band_pref_lte,
            /* LTE band preference (BC0, BC1, any, etc.)
            */

        sd_ss_band_pref_e_type    band_pref_tds,
            /* Band preference to use while obtaining available
            ** TD-SCDMA networks.
            */

        sd_ss_e_type              ss,

        sd_network_list_type_e_type list_type,


        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type                  act;
  sd_ss_ind_user_get_net_gw_ss_cmd  *cmd;

  cmd = (sd_ss_ind_user_get_net_gw_ss_cmd *) sdtask_sd_cmd_new(
                                              sizeof(sd_ss_ind_user_get_net_gw_ss_cmd),
                                              (execute_fn) sdcmd_user_get_net_gw_ss,
                                              rtrn_act_ptr,
                                              NULL
                                              );

  cmd->ss = ss;
  cmd->mode_pref = mode_pref;
  cmd->band_pref = band_pref;
  cmd->band_pref_lte = band_pref_lte;
  cmd->band_pref_tds = band_pref_tds;
  cmd->list_type     = list_type;

  act = sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd);
  ref_cnt_obj_release(cmd);

  return act;
}



EXTERN  sd_ss_act_e_type          sd_ss_ind_user_term_get_net_gw_ss(
        sd_ss_e_type              ss,

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  return sd_ss_generic_ss_cmd(sdcmd_user_term_get_net_gw_ss, rtrn_act_ptr, ss);
}


/* ^L<EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_user_term_get_net_gw

DESCRIPTION
  Inform SD that the user commanded to terminate the previous get networks
  command.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_GW  -   Attempt acquiring an GSM/WCDMA system as specified by
                          the "GW acquisition" parameters


SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sd_ss_act_e_type          sd_ss_ind_user_term_get_net_gw(


        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
    #ifdef FEATURE_HYBR_3GPP
    return sd_ss_ind_user_term_get_net_gw_ss(SD_SS_HYBR_2,
                     rtrn_act_ptr);
    #elif defined(CFG_HYBR_GW_3)
    return sd_ss_ind_user_term_get_net_gw_ss(SD_SS_HYBR_3,
                     rtrn_act_ptr);
    #else
    return sd_ss_ind_user_term_get_net_gw_ss(SD_SS_MAIN,
                     rtrn_act_ptr);
    #endif

} /* sd_ss_ind_user_term_get_net_gw() */

#endif /* defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) */


#if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)
/* <EJECT> */
/*===========================================================================
=============================================================================
====================== CDMA SPECIFIC HELPER FUNCTIONS =======================
=============================================================================
===========================================================================*/



/*===========================================================================
=============================================================================
====================== CDMA ACQUISITION SS-INDICATIONS ======================
=============================================================================
===========================================================================*/

/* Indications that are specific to CDMA pilot and sync channel acquisition,
** such as pilot acquisition failure or protocol mismatch. These indications
** can be received from the SS-Client during CDMA pilot and sync channel
** acquisition only; that is, before the SS-Client enters the CDMA idle
** state.
*/



EXTERN  sd_ss_act_e_type          sd_ss_ind_cdma_acq_failed_pilot(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  return sd_ss_generic_cmd(sdcmd_cdma_acq_failed_pilot, rtrn_act_ptr);
} /* sd_ss_ind_cdma_acq_failed_pilot */





/*===========================================================================

FUNCTION sd_ss_ind_cdma_acq_failed_sync

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when CDMA acquisition
  fails due to failure to successfully receive the sync channel message.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GWL  -  Acquire a GSM/WCDMA/LTE/TD-SCDMA system as specified by
                          the returned "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/

EXTERN  sd_ss_act_e_type          sd_ss_ind_cdma_acq_failed_sync(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  return sd_ss_generic_cmd(sdcmd_cdma_acq_failed_sync2, rtrn_act_ptr);
} /* sd_ss_ind_cdma_acq_failed_pilot */





/*===========================================================================

FUNCTION sd_ss_ind_cdma_acq_failed_unlock_rf

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when CDMA acquisition
  fails due to unlock RF resources.

  This indication is called before CDMA protocol acquired the sync channel
  messages.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GWL  -  Acquire a GSM/WCDMA/LTE/TD-SCDMA system as specified
                          by the returned "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/
/*-----------------------------------------------------------------------------
  Note that there is no need to issue a command for this - the only thing
  this does is call sd_ss_act_get(), which uses a critical section to
  protect access.
-----------------------------------------------------------------------------*/
EXTERN  sd_ss_act_e_type          sd_ss_ind_cdma_acq_failed_unlock_rf(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type    act;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Return the last true action.
  */
  act = sd_ss_act_get( rtrn_act_ptr );

  SD_MSG_HIGH_1( "cdma acq fail unlock rf. Get true action= %d ", act);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GWL   |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_PWR_SAVE) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

}



/*===========================================================================

FUNCTION sd_ss_ind_cdma_acq_prot_mis

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when CDMA acquisition
  fails due to protocol mismatch (i.e., MOB_P_REV < MIN_P_REV or MS does not
  support the Paging Channel data rate).

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GWL  -  Acquire a GSM/WCDMA/LTE/TD-SCDMA system as specified
                          by the returned "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/

EXTERN  sd_ss_act_e_type          sd_ss_ind_cdma_acq_prot_mis(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  return sd_ss_generic_cmd(sdcmd_cdma_acq_prot_mis, rtrn_act_ptr);
} /* sd_ss_ind_cdma_acq_prot_mis */



/*===========================================================================

FUNCTION sd_ss_ind_cdma_opr_prot_mis

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when CDMA idle handoff
  fails due to protocol mismatch (i.e., MOB_P_REV < MIN_P_REV or MS does not
  support the Paging Channel data rate).

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GWL   - Acquire a GSM/WCDMA/LTE/TD-SCDMA system as specified
                          by the returned "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/


EXTERN  sd_ss_act_e_type          sd_ss_ind_cdma_opr_prot_mis(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  return sd_ss_generic_cmd(sdcmd_cdma_opr_prot_mis, rtrn_act_ptr);
}


/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_cdma_acq_schm2

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when successfully receives
  the CDMA Sync Channel Message (SCHM) and the protocol revision is
  compatible.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

  The search algorithm should find the strongest energy peak around the PN
  circle (rather than the first strong enough peak).

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GWL   -  Acquire a GSM/WCDMA/LTE system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/




/*-----------------------------------------------------------------------------
  This function is what is executed when sd_ss_ind_cdma_acq_schm2 is called.
  Rather than being command based, though, it takes arguments just like
  sd_ss_ind_cdma_acq_schm2 because it is called from sd_ss_ind_cdma_opr_oh_info.
  The command-based function is sdcmd_cdma_acq_schm, which calls this
  using the arguments in the command.
-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
  Just calls sdcmd_cdma_acq_schm2 using the arguments from the command.
-----------------------------------------------------------------------------*/


/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_cdma_acq_schm4

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when successfully receives
  the CDMA Sync Channel Message (SCHM) and the protocol revision is
  compatible. Leap seconds was added to the interface between SD and CP as
  compared to sd_ss_ind_cdma_acq_schm3

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GWL  -   Attempt acquiring an GSM/WCDMA/LTE system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/
sd_ss_act_e_type          sd_ss_ind_cdma_acq_schm4 (

        word                      sid,
            /* SID from SCHM.
            */

        word                      nid,
            /* NID from SCHM.
            */

        byte                      ltm_off,
            /* LTM Offset
            */

        sys_daylt_savings_e_type  daylt_savings,
            /* Daylight savings ind
            */

        byte                      leap_secs,
            /* Leap seconds
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{

  sd_ss_act_e_type            act;
  sd_ss_ind_cdma_acq_schm_cmd *cmd;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd = (sd_ss_ind_cdma_acq_schm_cmd *) sdtask_sd_cmd_new(
                                          sizeof(sd_ss_ind_cdma_acq_schm_cmd),
                                          (execute_fn) sdcmd_cdma_acq_schm,
                                          rtrn_act_ptr,
                                          NULL
                                          );

  cmd->sid = sid;
  cmd->nid = nid;
  cmd->is_from_sync = TRUE;
  cmd->ltm_off = ltm_off;
  cmd->daylt_savings = daylt_savings;
  cmd->leap_secs = leap_secs;

  act = sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd);
  ref_cnt_obj_release(cmd);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

}

/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_cdma_acq_ho_fail

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the handoff from
  HDR fails

DEPENDENCIES
  sd_init() must have already been called to initialize SD.


RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/


EXTERN  sd_ss_act_e_type          sd_ss_ind_cdma_acq_ho_fail(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  return sd_ss_generic_cmd(sdcmd_cdma_acq_ho_fail, rtrn_act_ptr);
} /* sd_ss_ind_cdma_acq_ho_fail */


/**==========================================================================

@FUNCTION sd_ss_ind_cdma_acq_redir_abort

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when CDMA
  aborts redirection while in LTE to 1x redirection process.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_PWR_SAVE -  Exit out of the operation.

@SIDE EFFECTS
  None.

===========================================================================*/


EXTERN sd_ss_act_e_type sd_ss_ind_cdma_acq_redir_abort(

        sd_ss_act_s_type          *rtrn_act_ptr
           /**< Pointer to a buffer where to place returned SS-Action.
           */

)
{
  return sd_ss_generic_cmd(sdcmd_cdma_acq_redir_abort, rtrn_act_ptr);
}



#endif /* defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900) */



#if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)
/* <EJECT> */
/*===========================================================================
=============================================================================
======================= CDMA OPERATION SS-INDICATIONS =======================
=============================================================================
===========================================================================*/

/* Indications that are specific to CDMA operation, such as CDMA Redirection
** or a CDMA new system. These indications can be received from the SS-Client
** during CDMA operation only; that is, after the SS-Client enters the CDMA
** idle state.
*/

/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_cdma_opr_oh_info6

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the CDMA overhead
  information changes from not current to current. This happen under the
  following conditions:

    1. The SS-Client acquires a new paging channel (due to initial
       acquisition or idle handoff) and receives the last required overhead
       message on that channel.

    2. The SS-Client updates the overhead information of the acquired system.
       Note that this happen when the base station changes CONFIG_MSG_SEQ and
       the SS-Client receives the last required overhead message with the new
       CONFIG_MSG_SEQ.

    3. The SS-Client updates the overhead information of the aquired system,
       when the registration status of the acquired system is changed.

  In addition to the functionality in sd_ss_ind_cdma_opr_oh_info5(), this
  API allows for CDMA base station parameters to be passed to SD


  NOTE! This indication should not be called when the SS-Client is just
  passing through on the way to a re-hash.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GWL   -  Attempt acquiring a GSM/WCDMA/LTE system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sd_ss_act_e_type          sd_ss_ind_cdma_opr_oh_info6
(

        sd_band_e_type            band,
            /* Serving system's band class.
            */

        sd_chan_type              chan,
            /* Serving system's CDMA channel.
            */

        word                      sid,
            /* Serving system's SID.
            */

        word                      nid,
            /* Serving system's NID.
            */

        boolean                   is_pending_gsrdm,
            /* Indicate whether a GSRDM is pending.
            */

        word                      mcc,
            /* Serving system's MCC.
            */
        byte                      imsi_11_12,
            /* Serving system's IMISI_11_12.
            */

        sys_srch_win_type         srch_win_n,
            /* Neighbor set window search size.
            */

        sys_base_lat_type         base_lat,
            /* Base station latitude.
            */

        sys_base_long_type        base_long,
            /* Base station latitude.
            */

        sys_base_id_type          base_id,
            /* Base station Id.
            */

        sd_ss_ovhd_status_e_type  ovhd_status,
           /* Indicate if the overhead message is complete.
           */

        sys_reg_status_e_type     reg_status,
            /* Registration status of the acquired system.
            */

        byte                      reg_prd,
            /* Registration period
            */

        uint16                    height,
           /* Base station shall set this field to the height, above the WGS-84  
           ** reference ellipsoid, in units of 1 meter, in the range from -500m  
           ** to 15,882m, where the binary value of the field conveys the height 
           ** plus 500m. The base station shall set this field to all ones 
           ** if its height is not known.
           */				

        uint8                     loc_unc_h,
           /* Base station shall set this field to the circular horizontal 
           ** location uncertainty. The value of this field indicates the radius 
           ** of a circular 95% 40 confidence coverage area.
           */

        uint8                     loc_unc_v,
           /* Base station set this field to the vertical location uncertainty. 
           ** The value of this field indicates the one-sigma uncertainty of HEIGHT
           ** within the corresponding horizontal coverage area. The base station 
           ** shall set this field to all ones if its height is not known.
           */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
	sd_ss_act_e_type				act;
	sd_ss_ind_cdma_opr_oh_info_cmd	*cmd;
	
        cmd = (sd_ss_ind_cdma_opr_oh_info_cmd *) sdtask_sd_cmd_new(
                                              sizeof(sd_ss_ind_cdma_opr_oh_info_cmd),
                                              (execute_fn) sdcmd_cdma_opr_oh_info,
                                              rtrn_act_ptr,
                                              NULL
                                              );	

	cmd->band = band;
	cmd->chan = chan;
	cmd->sid = sid;
	cmd->nid = nid;
	cmd->is_pending_gsrdm = is_pending_gsrdm;
	cmd->mcc = mcc;
	cmd->imsi_11_12 = imsi_11_12;
	cmd->srch_win_n = srch_win_n;
	cmd->base_lat = base_lat;
	cmd->base_long = base_long;
	cmd->base_id = base_id;
	cmd->ovhd_status = ovhd_status;
	cmd->reg_status = reg_status;
	cmd->reg_prd = reg_prd;
	cmd->height = height;
	cmd->loc_unc_h = loc_unc_h;
	cmd->loc_unc_v = loc_unc_v;

	act = sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd);
	ref_cnt_obj_release(cmd);
	
	return act;

}

/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_cdma_opr_oh_info5

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the CDMA overhead
  information changes from not current to current. This happen under the
  following conditions:

    1. The SS-Client acquires a new paging channel (due to initial
       acquisition or idle handoff) and receives the last required overhead
       message on that channel.

    2. The SS-Client updates the overhead information of the acquired system.
       Note that this happen when the base station changes CONFIG_MSG_SEQ and
       the SS-Client receives the last required overhead message with the new
       CONFIG_MSG_SEQ.

    3. The SS-Client updates the overhead information of the aquired system,
       when the registration status of the acquired system is changed.

  Adds CDMA registration period parameter, that is sent by 1x-cp

  NOTE! This indication should not be called when the SS-Client is just
  passing through on the way to a re-hash.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GWL   -  Attempt acquiring a GSM/WCDMA/LTE system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/

EXTERN  sd_ss_act_e_type          sd_ss_ind_cdma_opr_oh_info5(

        sd_band_e_type            band,
            /* Serving system's band class.
            */

        sd_chan_type              chan,
            /* Serving system's CDMA channel.
            */

        word                      sid,
            /* Serving system's SID.
            */

        word                      nid,
            /* Serving system's NID.
            */

        boolean                   is_pending_gsrdm,
            /* Indicate whether a GSRDM is pending.
            */

        word                      mcc,
            /* Serving system's MCC.
            */
        byte                      imsi_11_12,
            /* Serving system's IMISI_11_12.
            */

        sys_srch_win_type         srch_win_n,
            /* Neighbor set window search size.
            */

        sys_base_lat_type         base_lat,
            /* Base station latitude.
            */

        sys_base_long_type        base_long,
            /* Base station latitude.
            */

        sys_base_id_type          base_id,
            /* Base station Id.
            */

        sd_ss_ovhd_status_e_type  ovhd_status,
           /* Indicate if the overhead message is complete.
           */

        sys_reg_status_e_type     reg_status,
            /* Registration status of the acquired system.
            */

        byte                      reg_prd,
            /* Registration period
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{

  return sd_ss_ind_cdma_opr_oh_info6(band,
                                     chan,
                                     sid,
                                     nid,
                                     is_pending_gsrdm,
                                     mcc,
                                     imsi_11_12,
                                     srch_win_n,
                                     base_lat,
                                     base_long,
                                     base_id,
                                     ovhd_status,
                                     reg_status,
                                     reg_prd,
                                     0, /*  height */
                                     0, /* horizontal location uncertainty */
                                     0, /* vertical location uncertainty */
                                     rtrn_act_ptr);

}

/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_cdma_opr_oh_info

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the CDMA overhead
  information changes from not current to current. This happen under the
  following conditions:

    1. The SS-Client acquires a new paging channel (due to initial
       acquisition or idle handoff) and receives the last required overhead
       message on that channel.

    2. The SS-Client updates the overhead information of the acquired system.
       Note that this happen when the base station changes CONFIG_MSG_SEQ and
       the SS-Client receives the last required overhead message with the new
       CONFIG_MSG_SEQ.

  NOTE! This indication should not be called when the SS-Client is just
  passing through on the way to a re-hash.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GWL   -  Attempt acquiring a GSM/WCDMA/LTE system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sd_ss_act_e_type          sd_ss_ind_cdma_opr_oh_info(

        sd_band_e_type            band,
            /* Serving system's band class.
            */

        sd_chan_type              chan,
            /* Serving system's CDMA channel.
            */

        word                      sid,
            /* Serving system's SID.
            */

        word                      nid,
            /* Serving system's NID.
            */

        boolean                   is_pending_gsrdm,
            /* Indicate whether a GSRDM is pending.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  return sd_ss_ind_cdma_opr_oh_info5( band,
                                      chan,
                                      sid,
                                      nid,
                                      is_pending_gsrdm,
                                      SD_MCC_NONE,
                                      SD_IMSI_11_12_NONE,
                                      0,    /* srch_win_n */
                                      0,    /* base_lat   */
                                      0,    /* base_long  */
                                      0,    /* base_id    */
                                      SD_SS_OVHD_STATUS_COMPLETE,
                                      SYS_REG_STATUS_UNKNOWN,
                                      0,    /* reg_prd    */
                                      rtrn_act_ptr );
}




/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_cdma_opr_rescan

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when MS gets a CDMA SPM
  with a RESCAN field that is set to 1.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GWL   -  Acquire a GSM/WCDMA/LTE system as specified by the returned
                          "GSM/WCDMA acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/



EXTERN  sd_ss_act_e_type          sd_ss_ind_cdma_opr_rescan(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  return sd_ss_generic_cmd(sdcmd_cdma_opr_rescan, rtrn_act_ptr);
}


/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_cdma_opr_reg_rej

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when CDMA registration is
  rejected.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GWL   -  Acquire a GSM/WCDMA/LTE system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/

EXTERN  sd_ss_act_e_type          sd_ss_ind_cdma_opr_reg_rej(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  return sd_ss_generic_cmd(sdcmd_cdma_opr_reg_rej, rtrn_act_ptr);
}

/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_cdma_opr_sys_lost2

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when an acquired CDMA
  system is lost and the system lost reason is provided.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GWL   -  Acquire a GSM/WCDMA/LTE system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/




EXTERN  sd_ss_act_e_type          sd_ss_ind_cdma_opr_sys_lost2(

        sd_ss_sys_lost_e_type     sys_lost,
            /* System lost reason
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type                act;
  sd_ss_ind_cdma_opr_sys_lost_cmd *cmd;

  cmd = (sd_ss_ind_cdma_opr_sys_lost_cmd *) sdtask_sd_cmd_new(
                                              sizeof(sd_ss_ind_cdma_opr_sys_lost_cmd),
                                              (execute_fn) sdcmd_cdma_opr_sys_lost2,
                                              rtrn_act_ptr,
                                              NULL
                                              );

  cmd->sys_lost = sys_lost;

  act = sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd);
  ref_cnt_obj_release(cmd);

  return act;
}

/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_cdma_opr_redir_cdma

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the mobile receives a
  CDMA redirection message that redirects MS to CDMA systems.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACCEPT   -  Accept BS proposed redirection.

    SD_SS_ACT_REJECT   -  Reject BS proposed redirection.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sd_ss_act_e_type          sd_ss_ind_cdma_opr_redir_cdma(

        boolean                   is_rif,
            /* RETURN_IF_FAILED. Indicates whether the MS is required to
            ** return to the system from which it is being redirected upon
            ** failure to obtain service using the redirection criteria
            ** specified by this function call.
            */

        sd_ss_redir_e_type        redir_type,
            /* Redirection type - GSRDM, SRDM, NDSS.
            */

        sd_band_e_type            band_class,
            /* Band class. Indicate the CDMA band class, as specified in
            ** TSB58-A, for which the MS is being directed.
            */

        word                      expected_sid,
            /* Expected SID. If the MS is being redirected to a specific
            ** system, indicate the SID of that system; otherwise, should be
            ** set to SD_WILDCARD_SID.
            */

        word                      expected_nid,
            /* Expected NID. If the MS is being redirected to a specific
            ** network, indicate the NID of that network; otherwise, should
            ** be set to SD_WILDCARD_NID.
            */

        const word                *chans_ptr,
            /* Pointer to array of CDMA channels to which the MS is being
            ** redirected.
            */

        int                       num_chans,
            /* Number of CDMA channels in the array that is pointed to by
            ** chans_ptr.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_redir_rec_s_type       rec_list;
  int                       i;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( redir_type == SD_SS_REDIR_NONE       ||
             redir_type == SD_SS_REDIR_CDMA_SRDM  ||
             redir_type == SD_SS_REDIR_CDMA_GSRDM ||
             redir_type == SD_SS_REDIR_CDMA_NDSS  );

  SD_ASSERT_ENUM_IS_INRANGE( band_class, SD_BAND_MAX );
  SD_ASSERT( INRANGE(num_chans, 0, SD_MAX_NUM_REDIR_CHAN) );
  SD_ASSERT( chans_ptr != NULL || num_chans == 0 );
  if( !(chans_ptr != NULL || num_chans == 0))
  {
    sys_err_fatal_null_ptr_exception();
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Need to confirm with ping
  ** Wrap the follwoing into a funtion.
  */
  rec_list.is_delete_tmsi = FALSE;

  rec_list.record_type = SD_REDIR_REC_CDMA;
  rec_list.rec.cdma_rec.band_class = band_class;
  rec_list.rec.cdma_rec.num_chans = num_chans;

  num_chans = MIN( num_chans, SD_MAX_NUM_REDIR_CHAN );

  for( i=0; i<num_chans; i++ )
  {
    SD_ASSERT( chans_ptr != NULL );
    rec_list.rec.cdma_rec.chan[i] = *chans_ptr;
    chans_ptr++;
  }

  rec_list.rec.cdma_rec.expected_sid = expected_sid;
  rec_list.rec.cdma_rec.expected_nid = expected_nid;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return( sd_ss_ind_cdma_opr_redir( is_rif,
                                    redir_type,
                                    &rec_list,
                                    1,
                                    rtrn_act_ptr ));

} /* sd_ss_ind_cdma_opr_redir_cdma */



/*===========================================================================

FUNCTION sd_misc_is_all_sys_perm_map

DESCRIPTION
  Used by clients to know if the systems being passed in list are all
  permanent mapped/avoided.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  boolean:

    TRUE   -  all the systems passed in rec_list_ptr are not permanent mapped.

    FALSE  -  all the systems passed in rec_list_ptr are permanent mapped.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  boolean          sd_misc_is_all_sys_perm_map(

        const sd_redir_rec_s_type *rec_list_ptr,
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
)
{
  boolean    ret_val = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* check is there is need to store and replace scratch list here */
  sdsr_list_clr( SDSR_SCRATCH_LST );

  sd_ss_redir_rec_store( SDSR_SCRATCH_LST,
                         rec_list_ptr,
                         num_rec);


  ret_val = sdss_is_all_sys_perm_avoid_in_lst(SDSR_SCRATCH_LST);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sdsr_list_clr( SDSR_SCRATCH_LST ); /* clear the list */

  return ret_val;
}/* sd_misc_is_all_sys_perm_map */

/*===========================================================================

FUNCTION sd_ss_get_redir_rec_store

DESCRIPTION
  Step through the redirection message and store the redirection record that
  contains valide band/channl in the redirection list.

DEPENDENCIES

RETURN VALUE
 None

SIDE EFFECTS
  None.

===========================================================================*/

EXTERN  sd_ss_act_e_type          sd_ss_ind_cdma_opr_redir(

        boolean                   is_rif,
            /* RETURN_IF_FAILED. Indicates whether the MS is required to
            ** return to the system from which it is being redirected upon
            ** failure to obtain service using the redirection criteria
            ** specified by this function call.
            */

        sd_ss_redir_e_type        redir_type,
            /* Redirection type - GSRDM, SRDM, NDSS.
            */

        const sd_redir_rec_s_type *rec_list_ptr,
            /* Pointer to a list of redirection records to which the MS is
            ** being redirected.
            **
            ** The list contains one or more redirection record. Each
            ** redirection record specifies the redirection system that is
            ** either CDMA or AMPS system.
            */

        int                       num_rec,
            /* number of records in the redirection record list.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
    sd_ss_act_e_type              act;
    sd_ss_ind_cdma_opr_redir_cmd  *cmd;

    cmd = (sd_ss_ind_cdma_opr_redir_cmd *) sdtask_sd_cmd_new(
                                                sizeof(sd_ss_ind_cdma_opr_redir_cmd),
                                                (execute_fn) sdcmd_cdma_opr_redir,
                                                rtrn_act_ptr,
                                                NULL
                                                );

    cmd->is_rif = is_rif;
    cmd->redir_type = redir_type;
    cmd->rec_list_ptr = rec_list_ptr;
    cmd->num_rec = num_rec;

    act = sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd);
    ref_cnt_obj_release(cmd);

    return act;
}


#if defined(FEATURE_LTE_TO_1X)
/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_cdma_opr_ho_lte

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the mobile receives a
  HO message  to CDMA system from LTE.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:s
  SD_SS_ACT_ACQ_CDMA - aquire the CDMA system

SIDE EFFECTS
  None.

===========================================================================*/






EXTERN  sd_ss_act_e_type          sd_ss_ind_cdma_opr_ho_lte(


        sd_ss_ho_e_type           ho_type,
          /* HO type
          */

        const sd_ho_sys_param_s_type *ho_sys_param,
            /* Pointer to a list of redirection records to which the MS is
            ** being redirected.
            **
            ** The list contains one or more redirection record. Each
            ** redirection record specifies the redirection system that is
            ** either CDMA or AMPS system.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type              act;
  sd_ss_ind_cdma_opr_ho_lte_cmd *cmd;

  cmd = (sd_ss_ind_cdma_opr_ho_lte_cmd *) sdtask_sd_cmd_new(
                                              sizeof(sd_ss_ind_cdma_opr_ho_lte_cmd),
                                              (execute_fn) sdcmd_cdma_opr_ho_lte,
                                              rtrn_act_ptr,
                                              NULL
                                              );

  cmd->ho_type = ho_type;
  cmd->ho_sys_param = ho_sys_param;

  act = sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd);
  ref_cnt_obj_release(cmd);

  return act;
}

#endif
/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_cdma_opr_acpt_rej

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the SS-Clients is
  done with accepting/rejecting a CDMA redirection.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GWL   -  Acquire a GSM/WCDMA/LTE system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/


EXTERN  sd_ss_act_e_type          sd_ss_ind_cdma_opr_acpt_rej(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  return sd_ss_generic_cmd(sdcmd_cdma_opr_acpt_rej, rtrn_act_ptr);
}


/**===========================================================================

@FUNCTION sd_ss_ind_update_cdma_sib8_nbrs

@DESCRIPTION
Updates the SIB-8 neighbor to system record table.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

  At least one sd_ss_ind_xxx() SS-Indication function must have already been
  called.

@RETURN VALUE
None

@SIDE EFFECTS
  None.

===========================================================================*/


EXTERN void       sd_ss_ind_update_cdma_sib8_nbrs(

       const      sd_cdma_sib8_nbr_cell_info_list_s *sib8_nbr_cell_info_ptr
)
{
  sd_ss_ind_update_cdma_sib8_nbrs_cmd *cmd;

  cmd = (sd_ss_ind_update_cdma_sib8_nbrs_cmd *) sdtask_sd_cmd_new(
                                              sizeof(sd_ss_ind_update_cdma_sib8_nbrs_cmd),
                                              (execute_fn) sdcmd_update_cdma_sib8_nbrs,
                                              NULL,
                                              NULL
                                              );

  cmd->sib8_nbr_cell_info_ptr = sib8_nbr_cell_info_ptr;

  (void) sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd);
  ref_cnt_obj_release(cmd);
}



EXTERN  sd_ss_act_e_type          sd_ss_ind_cdma_opr_ndss_off(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  return sd_ss_generic_cmd(sdcmd_cdma_opr_ndss_off, rtrn_act_ptr);
}


/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_cdma_opr_new_sys

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when performing a CDMA
  idle-handoff to a BS with unknown configuration.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GWL   -  Acquire a GSM/WCDMA/LTE system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/



EXTERN  sd_ss_act_e_type          sd_ss_ind_cdma_opr_new_sys(

        sd_band_e_type            band,
            /* The band class of the new system.
            */

        sd_chan_type              chan,
            /* The CDMA channel of the new system.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type                act;
  sd_ss_ind_cdma_opr_new_sys_cmd  *cmd;

  cmd = (sd_ss_ind_cdma_opr_new_sys_cmd *) sdtask_sd_cmd_new(
                                              sizeof(sd_ss_ind_cdma_opr_new_sys_cmd),
                                              (execute_fn) sdcmd_cdma_opr_new_sys,
                                              rtrn_act_ptr,
                                              NULL
                                              );

  cmd->band = band;
  cmd->chan = chan;

  act = sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd);
  ref_cnt_obj_release(cmd);

  return act;
}






EXTERN  sd_ss_act_e_type          sd_ss_ind_cdma_opr_sys_resel(

        const sd_sys_s_type       *sys_ptr,
            /* Pointer to array of systems from which to re-select.
            */

        int                       sys_num,
            /* Number of systems in the array that is pointed to by sys_ptr.
            */


        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type                  act;
  sd_ss_ind_cdma_opr_sys_resel_cmd  *cmd;

  cmd = (sd_ss_ind_cdma_opr_sys_resel_cmd *) sdtask_sd_cmd_new(
                                              sizeof(sd_ss_ind_cdma_opr_sys_resel_cmd),
                                              (execute_fn) sdcmd_cdma_opr_sys_resel,
                                              rtrn_act_ptr,
                                              NULL
                                              );

  cmd->sys_ptr = sys_ptr;
  cmd->sys_num = sys_num;

  act = sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd);
  ref_cnt_obj_release(cmd);

  return act;
}


EXTERN  sd_ss_act_e_type          sd_ss_ind_cdma_opr_lock(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  return sd_ss_generic_cmd(sdcmd_cdma_opr_lock, rtrn_act_ptr);
}


/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_cdma_opr_unlock

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when MS receives a CDMA
  unlock order.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GWL   -  Acquire a GSM/WCDMA/LTE system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/


EXTERN  sd_ss_act_e_type          sd_ss_ind_cdma_opr_unlock(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  return sd_ss_generic_cmd(sdcmd_cdma_opr_unlock, rtrn_act_ptr);
}

/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_cdma_opr_acc_term

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when CDMA access attempt
  terminates due to access denied, system lost, access state timeout, max
  access probes, reorder, intercept, BS release, MS release or normal access
  completion.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GWL   -  Acquire a GSM/WCDMA/LTE system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/






EXTERN  sd_ss_act_e_type          sd_ss_ind_cdma_opr_acc_term(

        sd_ss_acc_reas_e_type     acc_reas,
            /* Access termination reason.
            */

        sd_ss_acc_term_e_type     acc_term,
            /* Access termination reason.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type                act;
  sd_ss_ind_cdma_opr_acc_term_cmd *cmd;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd = (sd_ss_ind_cdma_opr_acc_term_cmd *) sdtask_sd_cmd_new(
                                              sizeof(sd_ss_ind_cdma_opr_acc_term_cmd),
                                              (execute_fn) sdcmd_cdma_opr_acc_term,
                                              rtrn_act_ptr,
                                              NULL
                                              );

  cmd->acc_reas = acc_reas;
  cmd->acc_term = acc_term;

  act = sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd);
  ref_cnt_obj_release(cmd);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

}


/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_cdma_opr_cam_err

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when MS receives a CDMA
  channel assignment that is not supported by the MS.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GWL   -  Acquire a GSM/WCDMA/LTE system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/


EXTERN  sd_ss_act_e_type          sd_ss_ind_cdma_opr_cam_err(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */

)
{
  return sd_ss_generic_cmd(sdcmd_cdma_opr_cam_err, rtrn_act_ptr);
}


/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_cdma_opr_cam_cdma

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when MS receives a CDMA
  channel assignment to a CDMA paging/traffic channel.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with BS proposed channel assignment.

    SD_SS_ACT_REJECT   -  Reject BS proposed channel assignment.

SIDE EFFECTS
  None.

===========================================================================*/







EXTERN  sd_ss_act_e_type          sd_ss_ind_cdma_opr_cam_cdma(

        sd_ss_ho_e_type           ho_type,
            /* The handoff type the MS needs to perform as part of this
            ** channel assignment:
            **
            **   SD_SS_HO_CDMA_CAM_TO_CDMA_PAG
            **   SD_SS_HO_CDMA_CAM_TO_CDMA_TRF
            */

        sd_band_e_type            band,
            /* Assigned CDMA band class.
            */

        sd_chan_type              chan,
            /* Assigned CDMA traffic channel number.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */

)
{
  sd_ss_act_e_type                act;
  sd_ss_ind_cdma_opr_cam_cdma_cmd *cmd;

  cmd = (sd_ss_ind_cdma_opr_cam_cdma_cmd *) sdtask_sd_cmd_new(
                                              sizeof(sd_ss_ind_cdma_opr_cam_cdma_cmd),
                                              (execute_fn) sdcmd_cdma_opr_cam_cdma,
                                              rtrn_act_ptr,
                                              NULL
                                              );

  cmd->ho_type = ho_type;
  cmd->band = band;
  cmd->chan = chan;

  act = sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd);
  ref_cnt_obj_release(cmd);

  return act;
}


/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_cdma_opr_cam_amps

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when MS receives a CDMA
  channel assignment to an AMPS system/voice channel.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with BS proposed channel assignment.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_REJECT   -  Reject BS proposed channel assignment.

SIDE EFFECTS
  None.

===========================================================================*/







EXTERN  sd_ss_act_e_type          sd_ss_ind_cdma_opr_cam_amps(

        sd_ss_ho_e_type           ho_type,
            /* The handoff type the MS needs to perform as part of this
            ** channel assignment:
            **
            **   SD_SS_HO_CDMA_CAM_TO_AMPS_SYS
            **   SD_SS_HO_CDMA_CAM_TO_AMPS_VOICE
            */

        sd_cell_sys_e_type        cell_sys,
            /* The assigned cellular system (A/B). Note that this parameter
            ** is ignored when the MS is being assigned an AMPS voice
            ** channel.
            */

        word                      voice_chan,
            /* The assigned AMPS voice channel number. Note that this
            ** parameter is ignored when the MS is being assigned an AMPS
            ** system.
            */

        word                      sid,
            /* SID from channel assignment. If no SID is supplied with
            ** channel assignment, must be set to SD_WILDCARD_SID.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type                act;
  sd_ss_ind_cdma_opr_cam_amps_cmd *cmd;

  cmd = (sd_ss_ind_cdma_opr_cam_amps_cmd *) sdtask_sd_cmd_new(
                                              sizeof(sd_ss_ind_cdma_opr_cam_amps_cmd),
                                              (execute_fn) sdcmd_cdma_opr_cam_amps,
                                              rtrn_act_ptr,
                                              NULL
                                              );

  cmd->ho_type = ho_type;
  cmd->cell_sys = cell_sys;
  cmd->voice_chan = voice_chan;
  cmd->sid = sid;

  act = sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd);
  ref_cnt_obj_release(cmd);

  return act;
}


/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_cdma_opr_hdm_cdma

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when MS receives a CDMA
  handoff direction to a CDMA traffic channel.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with BS proposed handoff direction.

    SD_SS_ACT_REJECT   -  Reject BS proposed handoff direction.

SIDE EFFECTS
  None.

===========================================================================*/





EXTERN  sd_ss_act_e_type          sd_ss_ind_cdma_opr_hdm_cdma(

        sd_band_e_type            band,
            /* The assigned CDMA band class.
            */

        word                      chan,
            /* The Assigned CDMA traffic channel number.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */

)
{
  sd_ss_act_e_type                act;
  sd_ss_ind_cdma_opr_hdm_cdma_cmd *cmd;

  cmd = (sd_ss_ind_cdma_opr_hdm_cdma_cmd *) sdtask_sd_cmd_new(
                                              sizeof(sd_ss_ind_cdma_opr_hdm_cdma_cmd),
                                              (execute_fn) sdcmd_cdma_opr_hdm_cdma,
                                              rtrn_act_ptr,
                                              NULL
                                              );

  cmd->band = band;
  cmd->chan = chan;

  act = sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd);
  ref_cnt_obj_release(cmd);

  return act;
}

/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_cdma_opr_itspm

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when MS receives a CDMA In
  Traffic System Parameters Message or Mobile Station Registered Message.
  with new SID/NID information.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_RELEASE  -  Release call.

SIDE EFFECTS
  None.

===========================================================================*/






EXTERN  sd_ss_act_e_type          sd_ss_ind_cdma_opr_itspm(

        word                      sid,
            /* SID from ITSPM.
            */

        word                      nid,
            /* NID from ITSPM.
            */
        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */

)
{
  sd_ss_act_e_type              act;
  sd_ss_ind_cdma_opr_itspm_cmd  *cmd;

  cmd = (sd_ss_ind_cdma_opr_itspm_cmd *) sdtask_sd_cmd_new(
                                              sizeof(sd_ss_ind_cdma_opr_itspm_cmd),
                                              (execute_fn) sdcmd_cdma_opr_itspm,
                                              rtrn_act_ptr,
                                              NULL
                                              );

  cmd->sid = sid;
  cmd->nid = nid;

  act = sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd);
  ref_cnt_obj_release(cmd);

  return act;
}


/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_cdma_opr_release

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when CDMA call is
  released.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GWL   -  Acquire a GSM/WCDMA/LTE system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/

EXTERN  sd_ss_act_e_type          sd_ss_ind_cdma_opr_release(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */

)
{
  return sd_ss_generic_cmd(sdcmd_cdma_opr_release, rtrn_act_ptr);
}

/* <EJECT> */
#ifdef FEATURE_MMODE_LTE_RESEL
/*===========================================================================

FUNCTION sd_ss_ind_misc_irat_to_gwl

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when DO CP/ 1x CP decides
  to redirect/reselect to LTE

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE   -  Reject redirection/reselection. Continue
                            on current system.

    SD_SS_ACT_ACQ_GWL    -  Accept  redirection/reselection.

SIDE EFFECTS
  None.

===========================================================================*/

EXTERN  sd_ss_act_e_type          sd_ss_ind_misc_irat_to_gwl(

        sd_ss_e_type              ss,
            /* SS on which to activate GWL
            */

        boolean                   is_resel_allowed,
            /* flag to indicate to SD whether reselection is allowed
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type                act;
  sd_ss_ind_misc_irat_to_gwl_cmd  *cmd;

  cmd = (sd_ss_ind_misc_irat_to_gwl_cmd *) sdtask_sd_cmd_new(
                                              sizeof(sd_ss_ind_misc_irat_to_gwl_cmd),
                                              (execute_fn) sdcmd_misc_irat_to_gwl,
                                              rtrn_act_ptr,
                                              NULL
                                              );

  cmd->ss = ss;
  cmd->is_resel_allowed = is_resel_allowed;

  act = sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd);
  ref_cnt_obj_release(cmd);

  return act;
}
#endif /* FEATURE_MMODE_LTE_RESEL */


/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_user_cdma_irat_lte

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the mobile receives a
  redirection message containing one or more redirection recoreds that
  redirects MS to CDMA systems.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACCEPT   -  Accept BS proposed redirection.

    SD_SS_ACT_REJECT   -  Reject BS proposed redirection.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sd_ss_act_e_type          sd_ss_ind_user_cdma_irat_lte(

        sd_ss_redir_e_type        redir_type,
            /* Redirection type - LTE.
            */

        const sd_redir_rec_s_type *rec_list_ptr,
            /* Pointer to a list of redirection records to which the MS is
            ** being redirected.
            **
            ** The list contains one or more redirection record. Each
            ** redirection record specifies the redirection system that is
            ** either CDMA or AMPS system.
            */

        int                       num_rec,
            /* number of records in the redirection record list.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type                  act;
  sd_ss_ind_user_cdma_irat_lte_cmd  *cmd;

  cmd = (sd_ss_ind_user_cdma_irat_lte_cmd *) sdtask_sd_cmd_new(
                                              sizeof(sd_ss_ind_user_cdma_irat_lte_cmd),
                                              (execute_fn) sdcmd_user_cdma_irat_lte,
                                              rtrn_act_ptr,
                                              NULL
                                              );

  cmd->redir_type = redir_type;
  cmd->rec_list_ptr = rec_list_ptr;
  cmd->num_rec = num_rec;

  act = sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd);
  ref_cnt_obj_release(cmd);

  return act;
}

#endif  /* defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900) */

/*===========================================================================

FUNCTION sd_ss_ind_misc_stack_local_activate

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when local activated indication is received.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:
    SD_SS_ACT_CONTINUE

SIDE EFFECTS
  None.

===========================================================================*/

EXTERN  sd_ss_act_e_type          sd_ss_ind_misc_stack_local_activate(

        sd_ss_e_type              ss,
            /* SS on which to activate GWL
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type act = SD_SS_ACT_CONTINUE;
  sd_ss_ind_misc_service_transfer_cmd  *cmd;

  if(!sdss_is_sglte())
  {
    return act;
  }
  
  cmd = (sd_ss_ind_misc_service_transfer_cmd *) sdtask_sd_cmd_new(
                                              sizeof(sd_ss_ind_misc_service_transfer_cmd),
                                              (execute_fn) sdcmd_misc_stack_local_activate,
                                              rtrn_act_ptr,
                                              NULL
                                              );

  cmd->ss = ss;

  act = sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd);
  ref_cnt_obj_release(cmd);

  return act;
}




/*===========================================================================

FUNCTION sd_ss_ind_misc_stack_local_deactivate

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when a certain stack is
  locally deactivated by protocol.
  For SGLTE, This happens in scenarios:
    a) PS Domain Transfer due to PS service lost (LTE/TD) on Main
    b) IRAT from L -> G.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:
    SD_SS_ACT_CONTINUE   -  Take no action. Hybr-2 is deactiated.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sd_ss_act_e_type          sd_ss_ind_misc_stack_local_deactivate(

        sd_ss_e_type              ss,
            /* SS on which to deactivate GWL
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type                      act;
  sd_ss_ind_misc_service_transfer_cmd  *cmd;

  if(!sdss_is_sglte())
  {
    return SD_SS_ACT_CONTINUE;
  }
  
  cmd = (sd_ss_ind_misc_service_transfer_cmd *) sdtask_sd_cmd_new(
                                              sizeof(sd_ss_ind_misc_service_transfer_cmd),
                                              (execute_fn) sdcmd_misc_stack_local_deactivate,
                                              rtrn_act_ptr,
                                              NULL
                                              );

  cmd->ss = ss;

  act = sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd);
  ref_cnt_obj_release(cmd);

  return act;
}

#if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) || \
    defined(FEATURE_SD_LTE)
/*===========================================================================

FUNCTION sd_ss_ind_misc_gw_sim_state_update

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when REG updates the sim
  state on T3245 timer expiry - for PS_INVALID, CS_INVALID case

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE   -  Continue on current system

    SD_SS_ACT_ACQ_GWL    -  Attempt acquiring an GSM/WCDMA system as specified by
                            the "GWL acquisition" parameters

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN sd_ss_act_e_type          sd_ss_ind_misc_gw_sim_state_update(

      sys_sim_state_e_type       sim_state,
      /* New sim state reported from REG.
      */

      sd_ss_e_type              ss,
      /* SS on which to activate GWL
      */

      sd_ss_act_s_type          *rtrn_act_ptr
      /* Pointer to a buffer where to place returned SS-Action.
      */
)
{

  sd_ss_act_e_type                      act;
  sd_ss_ind_misc_gw_sim_state_update_cmd    *cmd;

  cmd = (sd_ss_ind_misc_gw_sim_state_update_cmd *) sdtask_sd_cmd_new(
                                              sizeof(sd_ss_ind_misc_gw_sim_state_update_cmd),
                                              (execute_fn) sdcmd_misc_gw_sim_state_update,
                                              rtrn_act_ptr,
                                              NULL
                                              );

  cmd->ss = ss;
  cmd->sim_state = sim_state;
  act = sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd);
  ref_cnt_obj_release(cmd);

  return act;
}
#endif

#ifdef FEATURE_ACP

/* <EJECT> */
/*===========================================================================
=============================================================================
====================== AMPS ACQUISITION SS-INDICATIONS ======================
=============================================================================
===========================================================================*/


/* Indications that are specific to AMPS acquisition, such as AMPS dedicated
** control channel acquisition failure. These indications can be received
** from SS-Client during AMPS acquisition processing only; that is, before
** the SS-Client enters the AMPS idle state.
*/


/*===========================================================================

FUNCTION sd_ss_ind_amps_acq_failed_cch

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when an AMPS acquisition
  attempt fails due to a failure to acquire any of the two strongest control
  channels.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GWL   -  Acquire a GSM/WCDMA/LTE system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sd_ss_act_e_type          sd_ss_ind_amps_acq_failed_cch(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type    act;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_AMPS_ACQ_FAILED_CCH, rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GWL  |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_PWR_SAVE) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_amps_acq_failed_cch */




/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_amps_acq_cch_sid

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when MS receives the SID
  from the AMPS Control Channel OHM train during AMPS initialization.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GWL   -  Acquire a GSM/WCDMA system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sd_ss_act_e_type          sd_ss_ind_amps_acq_cch_sid(

        word                      sid,
            /* SID from SCHM.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type    act;

  /* Point at acquired system.
  */
  sdsr_ref_type       acq_sys_ref   = sdsr_list_get_ref( SDSR_ACQ_SYS, 0 );
  sdsr_s_type         *acq_sys_ptr  = sdsr_list_get_ptr( SDSR_ACQ_SYS, 0 );

  /* Point at candidate system.
  */
  sdsr_s_type         *cand_sys_ptr = sdsr_list_get_ptr( SDSR_CAND_SYS, 0 );

  /* Remember whether the service indicator is ON.
  */
  boolean             is_srv_on = sd_si_is_srv_on( SD_SS_MAIN );

  sdss_sys_addn_info_s_type curr_sys_info;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( acq_sys_ref != SDSR_REF_NULL );
  SD_ASSERT( acq_sys_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the additional sys info structure */
  sdss_addn_sys_info_init(&curr_sys_info);

  /* Update the SID/NID of the acquired system per the CCH SID.
  */
  acq_sys_ptr->sid.is95.sid = sid;
  acq_sys_ptr->sid.is95.nid = SD_WILDCARD_NID;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get the PRL designation, GEO list and more preferred list of the
  ** acquired system.
  */
  (void) sdprl_get_info(
                  SD_SS_MAIN,          /* MAIN system selection */
                  SDSR_ACQ_SYS,        /* Get PRL info for this list */
                  0,                   /* and list position */
                  SDSR_ACQ_GEO_LST,    /* Put GEO systems into this list */
                  SDSR_ACQ_PREF_LST,   /* Put PREF systems into this list */
                  SDSR_ACQ_COLLOC_LST, /* Put collocated sys in this list */
                  SDPRL_EXTRACT_ALL ); /* Get info regardless of PRL match */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Insert the acquired system at the beginning of the SDSR_ACQED_LST and
  ** remove any duplicates.
  */
  sdsr_list_insert_ref( SDSR_ACQED_LST, 0, acq_sys_ref, TRUE );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Do auto-NAM processing.
  */
  sdprl_autonam_proc( acq_sys_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the acquired system does not conflict with the current SS-Preference
  ** and it is designated as an available in the PRL, append the acquired
  ** system to the AVAIL_LST list.
  */
  if( sdss_sr_is_pref_conf(SD_SS_MAIN, acq_sys_ptr, SDSS_PREF_CONF_NONE) &&
      sdss_sr_is_prl(acq_sys_ptr, SDSS_PRL_VAL_AVAIL) )
  {
    sdsr_list_app( SDSR_AVAIL_LST, SDSR_ACQ_SYS );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_AMPS_ACQ_CCH_SID, rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                    SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GWL   |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_PWR_SAVE) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If SS-Action is CONTINUE (i.e., we have selected a new serving system),
  ** do the following system record updates.
  */
  if( act == SD_SS_ACT_CONTINUE )
  {
    /* Update the expected SID/NID of the acquired system per the acquired
    ** SID/NID.
    */
    acq_sys_ptr->sid_expected.prm.is95.sid = sid;
    acq_sys_ptr->sid_expected.prm.is95.nid = SD_WILDCARD_NID;

    /* Update the system indicators per the acquired system.
    */
    sd_si_update_if_srv_is_off( SD_SS_MAIN,
                                SD_SI_UPDATE_ACQ,
                                SYS_SRV_STATUS_SRV,
                                SYS_SRV_DOMAIN_CS_ONLY,
                                acq_sys_ref,
                                NULL,
                                NULL,
                                NULL,
                                SDSR_ACQ_COLLOC_LST,
                                NULL,
                                NULL,
                                NULL,
                                SYS_SRV_STATUS_SRV,
                                SD_SIB8_NO_CHANGE,
                                SYS_EXTEND_SRV_INFO_NONE,
                                SYS_REG_DOMAIN_NOT_APPLICABLE,
                                NULL);

    /* Update the candidate system per the acquired system.
    */
    sdsr_list_put_ref( SDSR_CAND_SYS, acq_sys_ref );

    /* Update the serving system per the acquired system.
    */
    sd_ss_srv_sys_update( SD_SS_MAIN,
                          SDSR_ACQ_SYS,
                          SDSR_ACQ_GEO_LST,
                          SDSR_ACQ_PREF_LST,
                          SDSR_ACQ_COLLOC_LST );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Else (we have not yet selected a serving system), if the acquired system
  ** is allowed by the PRL and complies with the current SS-Preference do the
  ** following updates:
  */
  else if(sdss_sr_is_prl(acq_sys_ptr, SDSS_PRL_VAL_ALLOWED) &&
          sdss_sr_is_pref_conf(SD_SS_MAIN, acq_sys_ptr, SDSS_PREF_CONF_NONE))
  {
    /* If the candidate system is NULL or the acquired system is more
    ** preferred than the candidate system, update the candidate system per
    ** the acquired system and reorder the available list according to the
    ** new candidate system.
    */
    if( cand_sys_ptr == NULL ||
        sdprl_is_more_pref(SD_SS_MAIN, acq_sys_ptr, cand_sys_ptr) )
    {
      sdsr_list_put_ref( SDSR_CAND_SYS, acq_sys_ref );
      sdsr_list_order( SDSR_AVAIL_LST, SDSR_CAND_SYS );
    }

    /* If before processing the SS-Event the service indicator was OFF,
    ** update the system indicators per the acquired system.
    */
    if( ! is_srv_on )
    {
      /* Get the additional system info of the acquired system.
      */
      sdprl_get_addn_sys_record_info(acq_sys_ptr,
                                     &(curr_sys_info.assn_tag),
                                     &(curr_sys_info.geo_offset),
                                     &(curr_sys_info.geo_count));

      sd_si_update( SD_SS_MAIN,
                    SD_SI_UPDATE_ACQ,
                    SYS_SRV_STATUS_SRV,
                    SYS_SRV_DOMAIN_CS_ONLY,
                    acq_sys_ref,
                    FALSE,
                    NULL,
                    NULL,
                    NULL,
                    NULL,
                    SDSR_ACQ_COLLOC_LST,
                    NULL,
                    NULL,
                    &curr_sys_info,
                    SYS_SRV_STATUS_SRV,
                    SYS_LTE_CS_CAPABILITY_NONE,
                    SD_SIB8_NO_CHANGE,
                    FALSE,
                    SYS_IMS_LTE_EMERG_SUPP_UNKNOWN,
                    0, /* Request Id not needed */
                    SYS_EXTEND_SRV_INFO_NONE,
                    SYS_REG_DOMAIN_NOT_APPLICABLE,
                    NULL,
                    FALSE);

      /* Update the additional system info in sd state with that of
      ** the acquired system.
      */
      sdss_set_prev_sys_info(SD_SS_MAIN,
                             curr_sys_info.assn_tag,
                             curr_sys_info.geo_offset,
                             curr_sys_info.geo_count);
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_amps_acq_cch_sid */




/* <EJECT> */
/*===========================================================================
=============================================================================
======================= AMPS OPERATION SS-INDICATIONS =======================
=============================================================================
===========================================================================*/

/* Indications that are specific to AMPS operation, such as AMPS rescan or
** AMPS BS Overload. These indications can be received from the SS-Client
** during AMPS operation only; that is, after the SS-Client enters the AMPS
** idle state.
*/


/*===========================================================================

FUNCTION sd_ss_ind_amps_opr_failed_pch

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when MS fails to acquire
  any of the two strongest AMPS paging channels (during initialization or
  operation).

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GWL   -  Acquire a GSM/WCDMA/LTE system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sd_ss_act_e_type          sd_ss_ind_amps_opr_failed_pch(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type    act;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_AMPS_OPR_FAILED_PCH, rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GWL   |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_PWR_SAVE) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_amps_opr_failed_pch */




/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_amps_opr_pch_sid

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when MS receives the SID
  from the AMPS Paging Channel OHM train (during initialization or
  operation).

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GWL   -  Acquire a GSM/WCDMA/LTE system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sd_ss_act_e_type          sd_ss_ind_amps_opr_pch_sid(

        word                      sid,
            /* SID from AMPS paging channel.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type    act;
  boolean             sid_is_changed;
  sdss_state_e_type   prev_ss_state;

  /* Point at serving system.
  */
  sdsr_s_type         *srv_sys_ptr  = sdsr_list_get_ptr( SDSR_SRV_SYS, 0 );
  sdsr_ref_type       srv_sys_ref   = sdsr_list_get_ref( SDSR_SRV_SYS, 0 );
  sd_si_s_type        *si_ptr       = sd_si_ptr(SD_SS_MAIN);
  sdss_state_e_type   ss_state      = sd_get_ss_state(SD_SS_MAIN);

  sdss_sys_addn_info_s_type curr_sys_info;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( srv_sys_ptr != NULL );
  SD_ASSERT( srv_sys_ref != SDSR_REF_NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the additional sys info structure */
  sdss_addn_sys_info_init(&curr_sys_info);

  /* Find out whether serving system's SID has changed.
  */
  if( srv_sys_ptr->sid.is95.sid != sid )
  {
    sid_is_changed = TRUE;
  }
  else
  {
    sid_is_changed = FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If serving system's SID has changed, update the serving system per the
  ** new SID.
  */
  if( sid_is_changed )
  {
    srv_sys_ptr->sid.is95.sid = sid;
    srv_sys_ptr->sid.is95.nid = SD_WILDCARD_NID;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Get the PRL designation, GEO list and more preferred list of the
    ** new serving system.
    */
    (void) sdprl_get_info(
                  SD_SS_MAIN,          /* MAIN system selection */
                  SDSR_SRV_SYS,        /* Get PRL info for this list */
                  0,                   /* and list position */
                  SDSR_SRV_GEO_LST,    /* Put GEO systems into this list */
                  SDSR_SRV_PREF_LST,   /* Put PREF systems into this list */
                  SDSR_SRV_COLLOC_LST, /* Put collocated sys in this list */
                  SDPRL_EXTRACT_ALL ); /* Get info regardless of PRL match */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Do auto-NAM processing.
    */
    sdprl_autonam_proc( srv_sys_ptr );

  } /* if( SID is changed ) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Remember the current SS-State.
  */
  prev_ss_state = ss_state;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication as
  ** follows:
  **
  **   1. If SS-State is not yet operation or SID/NID has changed, process
  **      SDSS_EVT_CDMA_OPR_OH_SID.
  **   2. Else, process SDSS_EVT_MISC_FILLER.
  */
  if( ss_state != SDSS_STATE_OPR ||
      sid_is_changed )
  {
    act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_AMPS_OPR_PCH_SID, rtrn_act_ptr );
  }
  else
  {
    act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_MISC_FILLER, rtrn_act_ptr );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                    SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GWL   |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_PWR_SAVE) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If SS-Action is CONTINUE, do the following updates.
  */
  if( act == SD_SS_ACT_CONTINUE )
  {
    /* Update the expected SID/NID of the serving system per the Overhead
    ** Info's SID/NID.
    */
    srv_sys_ptr->sid_expected.prm.is95.sid = sid;
    srv_sys_ptr->sid_expected.prm.is95.nid = SD_WILDCARD_NID;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* If previous SS-State was not operation, or SID is changed, update the
    ** idle system, its GEO and its PREF lists per the serving system.
    */
    if( prev_ss_state != SDSS_STATE_OPR ||
        sid_is_changed )
    {
      sd_ss_idle_sys_update( SD_SS_MAIN,
                             SDSR_SRV_SYS,
                             SDSR_SRV_GEO_LST,
                             SDSR_SRV_PREF_LST );
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Get the additional system info of the acquired system.
    */
    sdprl_get_addn_sys_record_info(srv_sys_ptr,
                                   &(curr_sys_info.assn_tag),
                                   &(curr_sys_info.geo_offset),
                                   &(curr_sys_info.geo_count));

    /* Update the system indicators per the new serving system.
    */
    sd_si_update( SD_SS_MAIN,
                  SD_SI_UPDATE_OH,
                  SYS_SRV_STATUS_SRV,
                  SYS_SRV_DOMAIN_CS_ONLY,
                  srv_sys_ref,
                  TRUE,
                  NULL,
                  NULL,
                  NULL,
                  NULL,
                  SDSR_SRV_COLLOC_LST,
                  NULL,
                  NULL,
                  &curr_sys_info,
                  SYS_SRV_STATUS_SRV,
                  SYS_LTE_CS_CAPABILITY_NONE,
                  SD_SIB8_NO_CHANGE,
                  FALSE,
                  SYS_IMS_LTE_EMERG_SUPP_UNKNOWN,
                  0, /* Request Id not needed */
                  SYS_EXTEND_SRV_INFO_NONE,
                  SYS_REG_DOMAIN_NOT_APPLICABLE,
                  NULL,
                  FALSE);

    /* Update the additional system info in sd state with that of
    ** the acquired system.
    */
     sdss_set_prev_sys_info(SD_SS_MAIN,
                            curr_sys_info.assn_tag,
                            curr_sys_info.geo_offset,
                            curr_sys_info.geo_count);

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Inform the registered client (i.e., CM) that is OK to originate as
    ** necessary.
    */
    sd_si_ok_to_orig( SD_SS_MAIN, si_ptr->data_suspend );


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Reset any throttle counters if running.
    ** necessary.
    */
    sdss_throttle_reset_cntrs( SD_SS_MAIN );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_amps_opr_pch_sid */




/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_amps_opr_acc_sid

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when MS receives SID from
  AMPS access channels.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  SID is acceptable.

    SD_SS_ACT_REJECT   -  SID is not acceptable.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sd_ss_act_e_type          sd_ss_ind_amps_opr_acc_sid(

        word                      sid,
            /* SID from AMPS access channel.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type    act;

  /* Point at serving system.
  */
  sdsr_s_type         *srv_sys_ptr  = sdsr_list_get_ptr( SDSR_SRV_SYS, 0 );
  sdsr_ref_type       srv_sys_ref   = sdsr_list_get_ref( SDSR_SRV_SYS, 0 );

  sdss_sys_addn_info_s_type curr_sys_info;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( srv_sys_ptr != NULL );
  SD_ASSERT( srv_sys_ref != SDSR_REF_NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the additional sys info structure */
  sdss_addn_sys_info_init(&curr_sys_info);

  /* If serving system's SID has not changed, instruct SS-Client to CONTINUE.
  */
  if( srv_sys_ptr->sid.is95.sid == sid )
  {
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* If SS-Client supplied a buffer for the return SS-Action, set the
    ** returned SS-Action in this buffer to CONTINUE.
    */
    if( rtrn_act_ptr != NULL )
    {
      rtrn_act_ptr->act = SD_SS_ACT_CONTINUE;
    }

    return SD_SS_ACT_CONTINUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here the serving system's SID has changed, so process this
  ** SS-Indication.
  */

  /* Remember the old SID - it needs to be restored if new SID is not
  ** acceptable.
  */
  srv_sys_ptr->sid_expected.prm.is95.sid = srv_sys_ptr->sid.is95.sid;

  /* Update the system record of the serving system per the new SID.
  */
  srv_sys_ptr->sid.is95.sid  = sid;
  srv_sys_ptr->sid.is95.nid  = SD_WILDCARD_NID;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get the PRL designation, GEO list and more preferred list of the new
  ** serving system.
  */
  (void) sdprl_get_info(
                  SD_SS_MAIN,         /* MAIN system selection */
                  SDSR_SRV_SYS,       /* Get PRL info for this list */
                  0,                  /* and list position */
                  SDSR_SRV_GEO_LST,   /* Put GEO systems into this list */
                  SDSR_SRV_PREF_LST,  /* Put PREF systems into this list */
                  SDSR_MAX,            /* Do not extract collocated sys */
                  SDPRL_EXTRACT_ALL );/* Get info regardless of PRL match */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Do auto-NAM processing.
  */
  sdprl_autonam_proc( srv_sys_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_AMPS_OPR_ACC_SID, rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                    SD_SS_ACT_REJECT) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If SS-Action is CONTINUE, do the following updates.
  */
  if( act == SD_SS_ACT_CONTINUE )
  {
    /* Update the expected SID of the serving system per the new SID.
    */
    srv_sys_ptr->sid_expected.prm.is95.sid = sid;
    srv_sys_ptr->sid_expected.prm.is95.nid = SD_WILDCARD_NID;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Update the idle system per the new serving system's information.
    */
    sd_ss_idle_sys_update( SD_SS_MAIN,
                           SDSR_SRV_SYS,
                           SDSR_SRV_GEO_LST,
                           SDSR_SRV_PREF_LST );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Get the additional system info of the acquired system.
    */
    sdprl_get_addn_sys_record_info(srv_sys_ptr,
                                   &(curr_sys_info.assn_tag),
                                   &(curr_sys_info.geo_offset),
                                   &(curr_sys_info.geo_count));

    /* Update the system indicators per the new serving system.
    */
    sd_si_update( SD_SS_MAIN,
                  SD_SI_UPDATE_OH,
                  SYS_SRV_STATUS_SRV,
                  SYS_SRV_DOMAIN_CS_ONLY,
                  srv_sys_ref,
                  FALSE,
                  NULL,
                  NULL,
                  NULL,
                  NULL,
                  SDSR_MAX,
                  NULL,
                  NULL,
                  &curr_sys_info,
                  SYS_SRV_STATUS_SRV,
                  SYS_LTE_CS_CAPABILITY_NONE,
                  SD_SIB8_NO_CHANGE,
                  FALSE,
                  SYS_IMS_LTE_EMERG_SUPP_UNKNOWN,
                  0, /* Request Id not needed */
                  SYS_EXTEND_SRV_INFO_NONE,
                  SYS_REG_DOMAIN_NOT_APPLICABLE,
                  NULL,
                  FALSE);

    /* Update the additional system info in sd state with that of
    ** the acquired system.
    */
      sdss_set_prev_sys_info(SD_SS_MAIN,
                             curr_sys_info.assn_tag,
                             curr_sys_info.geo_offset,
                             curr_sys_info.geo_count);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Else (SID is not acceptable), restore the SID, PRL designation, GEO list
  ** and more preferred list of the previous serving system's SID.
  */
  else
  {
    srv_sys_ptr->sid.is95.sid = srv_sys_ptr->sid_expected.prm.is95.sid;

    (void) sdprl_get_info(
                  SD_SS_MAIN,         /* MAIN system selection */
                  SDSR_SRV_SYS,       /* Get PRL info for this list */
                  0,                  /* and list position */
                  SDSR_SRV_GEO_LST,   /* Put GEO systems into this list */
                  SDSR_SRV_PREF_LST,  /* Put PREF systems into this list */
                  SDSR_MAX,            /* Do not extract collocated sys */
                  SDPRL_EXTRACT_ALL );/* Get info regardless of PRL match */
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_amps_opr_acc_sid */




/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_amps_opr_rescan

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when MS receives an AMPS
  Rescan Message.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GWL   -  Acquire a GSM/WCDMA/LTE system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sd_ss_act_e_type          sd_ss_ind_amps_opr_rescan(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type    act;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_AMPS_OPR_RESCAN, rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                    SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GWL   |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_PWR_SAVE ) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_amps_opr_rescan */




/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_amps_opr_sys_lost

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the acquired AMPS
  system is lost.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GWL   -  Acquire a GSM/WCDMA/LTE system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.


SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sd_ss_act_e_type          sd_ss_ind_amps_opr_sys_lost(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type    act;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_AMPS_OPR_SYS_LOST, rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GWL   |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_PWR_SAVE) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_amps_opr_sys_lost */




/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_amps_opr_cdma_avail

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when MS receives CDMA
  Available GAM (while operating in AMPS mode).

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sd_ss_act_e_type          sd_ss_ind_amps_opr_cdma_avail(

        word                      cdma_chan,
            /* CDMA channel to acquire.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type    act;
  sdsr_ref_type       cdma_avail_sys_ref;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Clear the SDSR_CDMA_AVAIL_SYS.
  */
  sdsr_list_clr( SDSR_CDMA_AVAIL_SYS );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the CDMA available system is supported by the current target, store
  ** the system in the system record table and set the SDSR_CDMA_AVAIL_SYS to
  ** reference the stored record.
  */
  if( sdss_is_supp_sys(SD_MODE_CDMA, sd_map_band_to_band_pref(SD_BAND_CELL), cdma_chan) )
  {
    cdma_avail_sys_ref = sdsr_tbl_store2( SD_MODE_CDMA,
                                          sd_map_band_to_band_pref(SD_BAND_CELL),
                                          cdma_chan );

    sdsr_list_put_ref( SDSR_CDMA_AVAIL_SYS, cdma_avail_sys_ref );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_AMPS_OPR_CDMA_AVAIL, rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                    SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_PWR_SAVE ) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_amps_opr_cdma_avail */




/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_amps_opr_acc_term

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when AMPS access attempt
  terminates (i.e. the SS-Client has entered the AMPS serving system
  determination task).

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GWL   -  Acquire a GSM/WCDMA/LTE system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sd_ss_act_e_type          sd_ss_ind_amps_opr_acc_term(

        sd_ss_acc_reas_e_type     acc_reas,
            /* Access termination reason.
            */

        sd_ss_acc_term_e_type     acc_term,
            /* Access termination reason.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type    act;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( acc_reas, SD_SS_ACC_REAS_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( acc_term, SD_SS_ACC_TERM_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the SS-Core with the access and termination reasons.
  */
  sdss_acc_update( SD_SS_MAIN, acc_reas, acc_term );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If access is successful, update the service indicators due to implicit
  ** registration.
  */
  if( acc_term == SD_SS_ACC_TERM_NORMAL )
  {
    sd_si_update( SD_SS_MAIN,
                  SD_SI_UPDATE_REG,
                  SYS_SRV_STATUS_SRV,
                  SYS_SRV_DOMAIN_CS_ONLY,
                  sdsr_list_get_ref(SDSR_SRV_SYS, 0),
                  FALSE,
                  NULL,
                  NULL,
                  NULL,
                  NULL,
                  SDSR_MAX,
                  NULL,
                  NULL,
                  NULL,
                  SYS_SRV_STATUS_SRV,
                  SYS_LTE_CS_CAPABILITY_NONE,
                  SD_SIB8_NO_CHANGE,
                  FALSE,
                  SYS_IMS_LTE_EMERG_SUPP_UNKNOWN,
                  0, /* Request Id not needed */
                  SYS_EXTEND_SRV_INFO_NONE,
                  SYS_REG_DOMAIN_NOT_APPLICABLE,
                  NULL,
                  FALSE);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_AMPS_OPR_ACC_TERM, rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                    SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GWL   |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_PWR_SAVE ) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_amps_opr_acc_term */




/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_amps_opr_cam

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when MS receives a AMPS
  voice channel assignment.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sd_ss_act_e_type          sd_ss_ind_amps_opr_cam(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type    act;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the service indicators due to implicit registration.
  */
  sd_si_update( SD_SS_MAIN,
                SD_SI_UPDATE_REG,
                SYS_SRV_STATUS_SRV,
                SYS_SRV_DOMAIN_CS_ONLY,
                sdsr_list_get_ref(SDSR_SRV_SYS, 0),
                FALSE,
                NULL,
                NULL,
                NULL,
                NULL,
                SDSR_MAX,
                NULL,
                NULL,
                NULL,
                SYS_SRV_STATUS_SRV,
                SYS_LTE_CS_CAPABILITY_NONE,
                SD_SIB8_NO_CHANGE,
                FALSE,
                SYS_IMS_LTE_EMERG_SUPP_UNKNOWN,
                0, /* Request Id not needed */
                SYS_EXTEND_SRV_INFO_NONE,
                SYS_REG_DOMAIN_NOT_APPLICABLE,
                NULL,
                FALSE);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the SS-Core with the handoff type.
  */
  sdss_ho_update( SD_SS_MAIN, SD_SS_HO_AMPS_CAM_TO_AMPS_VOICE );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_AMPS_OPR_CAM, rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act ==  SD_SS_ACT_CONTINUE );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_amps_opr_cam */




/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_amps_opr_overload

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when MS receives an AMPS
  overload control message.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GWL   -  Acquire a GSM/WCDMA/LTE system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sd_ss_act_e_type          sd_ss_ind_amps_opr_overload(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type    act;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_AMPS_OPR_OVERLOAD, rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                    SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GWL   |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_PWR_SAVE ) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_amps_opr_overload */

#endif /* FEATURE_ACP */




#ifdef FEATURE_HDR

/* <EJECT> */
/*===========================================================================
=============================================================================
============================ HDR USER INDICATIONS ===========================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION sd_ss_ind_hdr_user_ss_pref3

DESCRIPTION
  Instructs the HDR SS-Client of the SS-Action to take when the user changes
  the System Selection Preference (SS-Preference) and/or origination mode.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_ACQ_GWL   -  Acquire a GSM/WCDMA/LTE system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/


EXTERN  sd_ss_act_e_type          sd_ss_ind_hdr_user_ss_pref3(

        sd_ss_pref_reas_e_type    pref_reason,
            /* Reason for changing the SS-Preference.
            */

        sd_ss_orig_mode_e_type    orig_mode,
            /* New origination mode (normal, OTASP, emergency, other, etc.).
            */

        sd_ss_mode_pref_e_type    mode_pref,
            /* New mode preference (AMPS, CDMA, any, etc.).
            */

        sd_ss_band_pref_e_type    band_pref,
            /* New band preference (BC0, BC1, any, etc.)
            */

        sd_ss_prl_pref_e_type     prl_pref,
            /* New prl system preference (Available BC0A, available BC0B, etc.)
            */

        sd_ss_roam_pref_e_type    roam_pref,
            /* New roam preference (any, home, affiliated, etc.)
            */

        sd_ss_hybr_pref_e_type    hybr_pref,
            /* New hybrid preference (none, CDMA/HDR).
            */

        sd_band_e_type            otasp_band,
            /* The band-class that is associated with OTASP origination mode.
            ** Note that this parameter is ignored if orig_mode != OTASP.
            */

        sd_blksys_e_type          otasp_blksys,
            /* The PCS frequency block/Cellular system that is associated
            ** with OTASP origination mode. Note that this parameter is
            ** ignored if orig_mode != OTASP.
            */

        const sys_manual_sys_info_s_type
                                  *manual_sys_info_ptr,
          /* Pointer to struct that defines the manual system.
           */

        sd_ss_srv_domain_pref_e_type   domain_pref,
           /* Service domain preference that is associated with the selected
           ** NAM.
           ** Applies only for GSM/WCDMA/LTE modes.
           */

        sd_ss_acq_order_pref_e_type    acq_order_pref,
           /* Preference for the order of acquisition ( WCDMA before GSM,
           ** GSM before WCDMA etc).
           */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_ind_user_ss_pref_cmd  *cmd;
  sd_ss_act_e_type            act;

  cmd = (sd_ss_ind_user_ss_pref_cmd *) sdtask_sd_cmd_new(
                                          sizeof(sd_ss_ind_user_ss_pref_cmd),
                                          (execute_fn) sdcmd_hdr_user_ss_pref3,
                                          rtrn_act_ptr,
                                          NULL
                                          );

 
  cmd->pref_reason = pref_reason;
  cmd->orig_mode = orig_mode;
  cmd->mode_pref = mode_pref;
  cmd->band_pref = band_pref;
  cmd->band_pref_lte = SYS_LTE_BAND_MASK_CONST_NONE;
  cmd->band_pref_tds = SD_SS_BAND_PREF_NONE;
  cmd->prl_pref = prl_pref;
  cmd->roam_pref = roam_pref;
  cmd->hybr_pref = hybr_pref;
  cmd->otasp_band = otasp_band;
  cmd->otasp_blksys = otasp_blksys;
  cmd->avoid_type = SD_SS_AVOID_SYS_IDLE_IF_UZ;
  cmd->avoid_time = 0;
  cmd->manual_sys_info_ptr = manual_sys_info_ptr;
  cmd->domain_pref = domain_pref;
  cmd->acq_order_pref = acq_order_pref;
  cmd->pref_update_reason = SD_SS_PREF_UPDATE_REASON_NONE;
  cmd->user_mode_pref = mode_pref;
  cmd->rat_acq_order_ptr = NULL;
  cmd->req_id = 0;
  cmd->csg_id = SYS_CSG_ID_INVALID;
  cmd->camp_mode = SD_SS_PREF_CAMP_MODE_OFF;

  act = sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd);
  ref_cnt_obj_release(cmd);

  return act;
}

#endif

/* <EJECT> */

/*===========================================================================

FUNCTION sd_ss_ind_hybr_1_user_ss_pref3

DESCRIPTION
  Instructs the HDR SS-Client of the SS-Action to take when the user changes
  the System Selection Preference (SS-Preference) and/or origination mode.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_ACQ_GWL   -  Acquire a GSM/WCDMA/LTE system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sd_ss_act_e_type          sd_ss_ind_hybr_1_user_ss_pref3(

        sd_ss_pref_reas_e_type    pref_reason,
            /* Reason for changing the SS-Preference.
            */

        sd_ss_orig_mode_e_type    orig_mode,
            /* New origination mode (normal, OTASP, emergency, other, etc.).
            */

        sd_ss_mode_pref_e_type    mode_pref,
            /* New mode preference (AMPS, CDMA, any, etc.).
            */

        sd_ss_band_pref_e_type    band_pref,
            /* New band preference (BC0, BC1, any, etc.)
            */

        sd_ss_prl_pref_e_type     prl_pref,
            /* New prl system preference (Available BC0A, available BC0B, etc.)
            */

        sd_ss_roam_pref_e_type    roam_pref,
            /* New roam preference (any, home, affiliated, etc.)
            */

        sd_ss_hybr_pref_e_type    hybr_pref,
            /* New hybrid preference (none, CDMA/HDR).
            */

        sd_band_e_type            otasp_band,
            /* The band-class that is associated with OTASP origination mode.
            ** Note that this parameter is ignored if orig_mode != OTASP.
            */

        sd_blksys_e_type          otasp_blksys,
            /* The PCS frequency block/Cellular system that is associated
            ** with OTASP origination mode. Note that this parameter is
            ** ignored if orig_mode != OTASP.
            */

        const sys_manual_sys_info_s_type
                                  *manual_sys_info_ptr,
          /* Pointer to struct that defines the manual system.
           */

        sd_ss_srv_domain_pref_e_type   domain_pref,
           /* Service domain preference that is associated with the selected
           ** NAM.
           ** Applies only for GSM/WCDMA/LTE modes.
           */

        sd_ss_acq_order_pref_e_type    acq_order_pref,
           /* Preference for the order of acquisition ( WCDMA before GSM,
           ** GSM before WCDMA etc).
           */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_HDR
  /* Call the HDR API
  */
  return ( sd_ss_ind_hdr_user_ss_pref3(pref_reason,
                                  orig_mode,
                                  mode_pref,
                                  band_pref,
                                  prl_pref,
                                  roam_pref,
                                  hybr_pref,
                                  otasp_band,
                                  otasp_blksys,
                                  manual_sys_info_ptr,
                                  domain_pref,
                                  acq_order_pref,
                                  rtrn_act_ptr ));
  #else
  return (sd_ss_ind_user_ss_pref7( pref_reason,
                                   orig_mode,
                                   mode_pref,
                                   band_pref,
                                   SYS_LTE_BAND_MASK_CONST_NONE,
                                   SD_SS_BAND_PREF_NONE,
                                   prl_pref,
                                   roam_pref,
                                   hybr_pref,
                                   otasp_band,
                                   otasp_blksys,
                                   SD_SS_AVOID_SYS_IDLE_IF_UZ, /* Don't care */
                                   0,                          /* Don't care */
                                   manual_sys_info_ptr,
                                   domain_pref,
                                   acq_order_pref,
                                   SD_SS_PREF_UPDATE_REASON_NONE,
                                   mode_pref,
                                   NULL,
                                   0,
                                   SD_SS_PREF_CAMP_MODE_NO_CHANGE,
                                   SYS_CSG_ID_INVALID,
                                   SYS_RAT_NONE,
                                   SYS_VOICE_DOMAIN_PREF_NONE,
                                   rtrn_act_ptr
                                 ));
  #endif

} /* sd_ss_ind_hybr_1_user_ss_pref3 */


#ifdef FEATURE_HDR
/*===========================================================================

FUNCTION sd_ss_ind_hdr_user_prot_deactivate

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the user informs that
  protocol must be deactivated.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with CDMA offline operation.

SIDE EFFECTS
  None.

===========================================================================*/


EXTERN  sd_ss_act_e_type          sd_ss_ind_hdr_user_prot_deactivate(


        sd_ss_act_s_type          *rtrn_act_ptr,
            /**< Pointer to a buffer where to place returned SS-Action.
            */
        sd_ss_prot_deact_adtnl_info_e_type adtnl_info
            /**< Additional information that caller can provide for special handling.
            */
)
{
  sd_ss_ind_user_prot_deactivate_cmd  *cmd;
  sd_ss_act_e_type            act;

  cmd = (sd_ss_ind_user_prot_deactivate_cmd *)
        sdtask_sd_cmd_new( sizeof(sd_ss_ind_user_prot_deactivate_cmd),
                           (execute_fn) sdcmd_hdr_user_prot_deactivate,
                           rtrn_act_ptr,
                           NULL );

  cmd->adtnl_info = adtnl_info;

  act = sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd);
  ref_cnt_obj_release(cmd);

  return act;
}


/* <EJECT> */
/**==========================================================================

@FUNCTION sd_ss_ind_hdr_user_redir_lte

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when MS receives an HDR
  redirection message while in progress of LTE to eHPRD redirection.


@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

@SIDE EFFECTS
  None.

===========================================================================*/





EXTERN  sd_ss_act_e_type sd_ss_ind_hdr_user_redir_lte(

        sd_ss_sys_get_f_type      *sys_get_func,
           /**< Pointer to a system extraction function.
           */

        void                      *data_ptr,
           /**< Pointer to SS-Client callback data block.
           */

        sd_ss_redir_e_type        redir_type,
           /**< Redirection type - from LTE.
           */

        sd_ss_act_s_type          *rtrn_act_ptr
           /**< Pointer to a buffer where to place returned SS-Action.
           */

)
{
  sd_ss_act_e_type                  act;
  sd_ss_ind_hdr_user_redir_lte_cmd  *cmd;

  cmd = (sd_ss_ind_hdr_user_redir_lte_cmd *) sdtask_sd_cmd_new(
                                              sizeof(sd_ss_ind_hdr_user_redir_lte_cmd),
                                              (execute_fn) sdcmd_hdr_user_redir_lte,
                                              rtrn_act_ptr,
                                              NULL
                                              );

  cmd->sys_get_func = sys_get_func;
  cmd->data_ptr = data_ptr;
  cmd->redir_type = redir_type;

  act = sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd);
  ref_cnt_obj_release(cmd);

  return act;
}


#endif

#if (defined(FEATURE_HDR) )
/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_hdr_user_pwr_save_exit

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the user commands the
  MS to exit power save mode of operation for HYBR HDR instance.

  Note that the actual SD behavior when getting this indication from the
  SS-Client is controlled by the SS-Script for this indication. This script,
  however, is expected to be fairly similar to the power-up script.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_HDR -  Acquire a HDR system as specified by the returned
                         "HDR acquisition" parameters.


    SD_SS_ACT_PWR_SAVE -  Go offline. SD instructs SS-Client to go right back
                          into power-save if the current SS-Preference
                          forbids any PRL system from being attempted
                          acquisition.

SIDE EFFECTS
  None.

===========================================================================*/


EXTERN  sd_ss_act_e_type          sd_ss_ind_hybr_1_user_pwr_save_exit(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  return sd_ss_generic_cmd(sdcmd_hybr_1_user_pwr_save_exit, rtrn_act_ptr);
}


#endif
/*===========================================================================
=============================================================================
============================ HDR HELPER FUNCTIONS ===========================
=============================================================================
===========================================================================*/


EXTERN  sd_ss_act_e_type          sd_ss_ind_hybr_2_user_pwr_save_exit(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  return sd_ss_generic_cmd(sdcmd_hybr_2_user_pwr_save_exit, rtrn_act_ptr);
}

#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
EXTERN  sd_ss_act_e_type          sd_ss_ind_hybr_3_user_pwr_save_exit(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  return sd_ss_generic_cmd(sdcmd_hybr_3_user_pwr_save_exit, rtrn_act_ptr);
}
#endif

/* <EJECT> */
/*===========================================================================
=============================================================================
============================ HDR MISC INDICATIONS ===========================
=============================================================================
===========================================================================*/




#ifdef FEATURE_HDR
/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_hdr_misc_meas_rprt

DESCRIPTION
  Instructs the HDR SS-Client of the SS-Action to take when the channel
  strength measurement that was requested by SD is completed and reported
  back to SD.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

  If reselection is allowed (per the resel_is_allowed parameter):

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Acquire an HDR system as specified by the returned
                          "HDR acquisition" parameters.

    SD_SS_ACT_ACQ_GWL   -  Acquire a GSM/WCDMA/LTE system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

  If reselection is not allowed (per the resel_is_allowed parameter):

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.


SIDE EFFECTS
  None.

===========================================================================*/

EXTERN  sd_ss_act_e_type          sd_ss_ind_hdr_misc_meas_rprt(

        word                      meas_id,
            /* The measurement ID. This IS should match the ID that was
            ** provided to the SS-Client with the measurement request (i.e.,
            ** SD_SS_ACT_MEAS_BACK or SD_SS_ACT_MEAS_DED) SS-Action.
            */

        boolean                   resel_is_allowed,
            /* Indicate whether reselection is currently allowed.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type              act;
  sd_ss_ind_misc_meas_rprt_cmd  *cmd;

  cmd = (sd_ss_ind_misc_meas_rprt_cmd *) sdtask_sd_cmd_new(
                                              sizeof(sd_ss_ind_misc_meas_rprt_cmd),
                                              (execute_fn) sdcmd_hdr_misc_meas_rprt,
                                              rtrn_act_ptr,
                                              NULL
                                              );

  cmd->meas_id = meas_id;
  cmd->resel_is_allowed = resel_is_allowed;

  act = sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd);
  ref_cnt_obj_release(cmd);

  return act;
}



/* <EJECT> */
/*===========================================================================
=============================================================================
======================= HDR ACQUISITION SS-INDICATIONS ======================
=============================================================================
===========================================================================*/

/* Indications that are specific to HDR pilot and sync channel acquisition,
** such as HDR pilot acquisition failure or HDR protocol mismatch. These
** indications can be received from the SS-Client during HDR pilot and sync
** channel acquisition only; that is, before the SS-Client enters the HDR
** idle state.
*/


/*===========================================================================

FUNCTION sd_ss_ind_hdr_acq_failed_pilot

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when HDR acquisition fails
  due to failure to acquire the pilot channel.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Acquire an HDR system as specified by the returned
                          "HDR acquisition" parameters.

    SD_SS_ACT_ACQ_GWL   -  Acquire a GSM/WCDMA/LTE system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/


EXTERN  sd_ss_act_e_type          sd_ss_ind_hdr_acq_failed_pilot(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  /* Reset HDR deact by Hybr2 BSR flag to FALSE 
  */
  sdss_set_flag(SD_SS_HYBR_2,SDSS_FLAG_HDR_DEACT_BY_HYBR2_BSR, FALSE);


  return sd_ss_generic_cmd(sdcmd_hdr_acq_failed_pilot, rtrn_act_ptr);
}


/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_hdr_acq_failed_sync

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when HDR acquisition fails
  due to failure to successfully receive the sync channel message.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Acquire an HDR system as specified by the returned
                          "HDR acquisition" parameters.

    SD_SS_ACT_ACQ_GWL   -  Acquire a GSM/WCDMA/LTE system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/


EXTERN  sd_ss_act_e_type          sd_ss_ind_hdr_acq_failed_sync(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  /* Reset HDR deact by Hybr2 BSR flag to FALSE 
  */
  sdss_set_flag(SD_SS_HYBR_2,SDSS_FLAG_HDR_DEACT_BY_HYBR2_BSR, FALSE);


  return sd_ss_generic_cmd(sdcmd_hdr_acq_failed_sync, rtrn_act_ptr);
}


/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_hdr_acq_prot_mis

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when HDR acquisition fails
  due to protocol mismatch (i.e., the access terminal's revision number is
  not in the range defined by the MinimumRevision and MaximumRevision
  (inclusive) fields of the sync message).

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Acquire an HDR system as specified by the returned
                          "HDR acquisition" parameters.

    SD_SS_ACT_ACQ_GWL   -  Acquire a GSM/WCDMA/LTE system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/

EXTERN  sd_ss_act_e_type          sd_ss_ind_hdr_acq_prot_mis(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  return sd_ss_generic_cmd(sdcmd_hdr_acq_prot_mis, rtrn_act_ptr);
}


/*===========================================================================

FUNCTION sd_ss_ind_hdr_opr_prot_mis

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when HDR idle handoff
  fails due to protocol mismatch (i.e., MOB_P_REV < MIN_P_REV or MS does not
  support the Paging Channel data rate).

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GWL   -  Acquire a GSM/WCDMA/LTE system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/

EXTERN  sd_ss_act_e_type          sd_ss_ind_hdr_opr_prot_mis(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  return sd_ss_generic_cmd(sdcmd_hdr_opr_prot_mis, rtrn_act_ptr);
}

/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_hdr_acq_sid2

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the HDR system ID
  (i.e., AccessNetworkID) is obtained for the first time since the system is
  acquired.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Acquire an HDR system as specified by the returned
                          "HDR acquisition" parameters.

    SD_SS_ACT_ACQ_GWL   -  Acquire a GSM/WCDMA/LTE system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/
  EXTERN  sd_ss_act_e_type          sd_ss_ind_hdr_acq_sid2(

          const byte                sid_ptr[16],
              /* Pointer to array of 16 byte Sector ID.
              */

          byte                      subnet_mask_len,
              /* The subnet mask length that is associated with the Sector ID.
              *//*lint -esym(715,subnet_mask_len) *//*not referenced.*/

          const sys_channel_type    *chan_ptr,
              /* Pointer to HDR hashing channel records.
              */

          word                      num_chan,
              /* The number of HDR hashing channel records specified in
              ** chan_ptr
              */

          sd_ss_act_s_type          *rtrn_act_ptr
              /* Pointer to a buffer where to place returned SS-Action.
              */
  )
  {
    sd_ss_act_e_type  act;
  sd_ss_ind_hdr_acq_sid_cmd *cmd;

  cmd = (sd_ss_ind_hdr_acq_sid_cmd *) sdtask_sd_cmd_new(
                                              sizeof(sd_ss_ind_hdr_acq_sid_cmd),
                                              (execute_fn) sdcmd_hdr_acq_sid2,
                                              rtrn_act_ptr,
                                              NULL
                                              );

  cmd->sid_ptr = sid_ptr;
  cmd->subnet_mask_len = subnet_mask_len;
  cmd->chan_ptr = chan_ptr;
  cmd->num_chan = num_chan;

  act = sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd);
  ref_cnt_obj_release(cmd);

    return act;
    }




/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_hdr_acq_sid3

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the HDR system ID
  (i.e., AccessNetworkID) is obtained for the first time since the system is
  acquired.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Acquire an HDR system as specified by the returned
                          "HDR acquisition" parameters.

    SD_SS_ACT_ACQ_GWL   -  Acquire a GSM/WCDMA/LTE system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/





EXTERN  sd_ss_act_e_type          sd_ss_ind_hdr_acq_sid3(

        const byte                sid_ptr[16],
            /* Pointer to array of 16 byte Sector ID.
            */

        byte                      subnet_mask_len,
            /* The subnet mask length that is associated with the Sector ID.
            *//*lint -esym(715,subnet_mask_len) *//*not referenced.*/

        const sys_channel_type    *chan_ptr,
            /* Pointer to HDR hashing channel records.
            */

        word                      num_chan,
            /* The number of HDR hashing channel records specified in
            ** chan_ptr
            */

        word                      mcc,
            /* Serving system's MCC.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type          act;
  sd_ss_ind_hdr_acq_sid_cmd *cmd;

  cmd = (sd_ss_ind_hdr_acq_sid_cmd *) sdtask_sd_cmd_new(
                                              sizeof(sd_ss_ind_hdr_acq_sid_cmd),
                                              (execute_fn) sdcmd_hdr_acq_sid3,
                                              rtrn_act_ptr,
                                              NULL
                                              );

  cmd->sid_ptr = sid_ptr;
  cmd->subnet_mask_len = subnet_mask_len;
  cmd->chan_ptr = chan_ptr;
  cmd->num_chan = num_chan;
  cmd->mcc = mcc;

  act = sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd);
  ref_cnt_obj_release(cmd);

  return act;
}

/* <EJECT> */
/**==========================================================================

@FUNCTION sd_ss_ind_hdr_acq_redir_abort

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when HDR
  aborts redirection while in LTE to eHPRD redirection process.
  Following is the only sequence of abort indication that is allowed:
  1. DO-CP indicates to SD of Redirection from LTE
  2. SD gives action ACQ_HDR
  3. DO-CP indicates redirection abort
  4. SD clears its context and gives as action PWR_SAVE

  Step 2 can be single or multiple attempts due to failure reported on earlier systems

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_PWR_SAVE -  Exit out of the operation.

@SIDE EFFECTS
  None.

===========================================================================*/


EXTERN  sd_ss_act_e_type sd_ss_ind_hdr_acq_redir_abort(

        sd_ss_act_s_type          *rtrn_act_ptr
           /**< Pointer to a buffer where to place returned SS-Action.
           */

)
{
  return sd_ss_generic_cmd(sdcmd_hdr_acq_redir_abort, rtrn_act_ptr);
}


/* <EJECT> */
/*===========================================================================
=============================================================================
======================== HDR OPERATION SS-INDICATIONS =======================
=============================================================================
===========================================================================*/

/* Indications that are specific to HDR operation, such as HDR Redirection
** or a HDR session closed. These indications can be received from the
** SS-Client during HDR operation only; that is, after the SS-Client enters
** the HDR idle state.
*/

/*lint  -save*/
/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_hdr_opr_oh_info4

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the HDR overhead
  information changes from not current to current. This happen under the
  following conditions:

    1. The SS-Client acquires a new paging channel (due to initial
       acquisition or idle handoff).

    2. The SS-Client updates the overhead information of the acquired system.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Acquire an HDR system as specified by the returned
                          "HDR acquisition" parameters.

    SD_SS_ACT_ACQ_GW   -  Acquire a GSM/WCDMA system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
None.

===========================================================================*/
EXTERN  sd_ss_act_e_type          sd_ss_ind_hdr_opr_oh_info4(
 /*lint  -esym(715,chan_ptr) -esym(715, num_chan)
 ** Symbol 'chan_ptr' 'num_chan' not referenced
 */

        sd_band_e_type            band,
            /* Serving system's band class.
            */

        sd_chan_type              chan,
            /* Serving system's HDR channel.
            */

        const byte                sid_ptr[16],
            /* Pointer to array of 16 byte Sector ID.
            */

        byte                      subnet_mask_len,
            /* The subnet mask length that is associated with the Sector ID.
            */

        boolean                   is_ok_to_orig,
            /* Indicate whether the HDR protocol is currently able to process
            ** originations.
            */

        const sys_channel_type    *chan_ptr,
            /* Pointer to HDR hashing channel records.
            */

        word                      num_chan,
            /* The number of HDR hashing channel records specified in
            ** chan_ptr
            */

        sys_active_prot_e_type    hdr_active_prot,
            /* The active HDR protocol revision.
            */

        int16                     local_time_offset,
            /* Local time Offset from system time
            */

        uint8                     leap_secs,
            /* Leap seconds occurred since start of system time
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return( sd_ss_ind_hdr_opr_oh_info5( band,
                                      chan,
                                      sid_ptr,
                                      subnet_mask_len,
                                      is_ok_to_orig,
                                      chan_ptr,
                                      num_chan,
                                      hdr_active_prot,
                                      SYS_PERSONALITY_NONE,
                                      local_time_offset,
                                      leap_secs,
                                      rtrn_act_ptr ));

}/* sd_ss_ind_hdr_opr_oh_info4 */

/**===========================================================================

@FUNCTION sd_ss_ind_hdr_opr_oh_info6

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the HDR overhead
  information changes from not current to current. This happen under the
  following conditions:

    1. The SS-Client acquires a new paging channel (due to initial
       acquisition or idle handoff).

    2. The SS-Client updates the overhead information of the acquired system.

  In addition to the functionality in sd_ss_ind_hdr_opr_oh_info5() , this
  API allows for HDR mcc, system type, latitude, longitude to be passed to SD

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR -   Acquire an HDR system as specified by the returned
                          "HDR acquisition" parameters.

    SD_SS_ACT_ACQ_GW  -   Attempt acquiring an GSM/WCDMA system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_hdr_opr_oh_info6(


        sd_band_e_type            band,
            /**< Serving system's band class.
            */

        sd_chan_type              chan,
            /**< Serving system's HDR channel.
            */

        const byte                sid_ptr[16],
            /**< Pointer to array of 16 byte Sector ID.
            */

        byte                      subnet_mask_len,
            /**< The subnet mask length that is associated with the Sector ID.
            */

        boolean                   is_ok_to_orig,
            /**< Indicate whether the HDR protocol is currently able to process
            ** originations.
            */

        const sys_channel_type    *chan_ptr,
            /**< Pointer to HDR hashing channel records.
            */

        word                      num_chan,
            /**< The number of HDR hashing channel records specified in
            ** chan_ptr
            */

        sys_active_prot_e_type    hdr_active_prot,
            /**< The active HDR protocol revision.
            */

        sys_personality_e_type    hdr_personality,
            /**< The active HDR personality attributes
            */

        int16                     local_time_offset,
            /**< Local time Offset from system time
            */

        uint8                     leap_secs,
            /**< Leap seconds occurred since start of system time
            */

        uint16                     mcc, 
           /**< Serving system country code
           */

        int32                      latitude,			
           /**<  Base station latitude
           */

        int32                      longitude,	
           /**< Base station longitude	
           */

        uint8                      system_type,
           /**< HDR channel system type - Type of spec the hdr channel is compliant to
           */

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
)
{
	sd_ss_act_e_type			  act;
	sd_ss_ind_hdr_opr_oh_info_cmd *cmd;
	
        cmd = (sd_ss_ind_hdr_opr_oh_info_cmd *) sdtask_sd_cmd_new(
                                              sizeof(sd_ss_ind_hdr_opr_oh_info_cmd),
                                              (execute_fn) sdcmd_hdr_opr_oh_info,
                                              rtrn_act_ptr,
                                              NULL
                                              );	
	cmd->band = band;
	cmd->chan = chan;
	cmd->sid_ptr = sid_ptr;
	cmd->subnet_mask_len = subnet_mask_len;
	cmd->is_ok_to_orig = is_ok_to_orig;
	cmd->chan_ptr = chan_ptr;
	cmd->num_chan = num_chan;
	cmd->hdr_active_prot = hdr_active_prot;
	cmd->hdr_personality = hdr_personality;
	cmd->local_time_offset = local_time_offset;
	cmd->leap_secs = leap_secs;
        cmd->hdr_mcc = mcc;
	cmd->latitude = latitude;
	cmd->longitude = longitude;
	cmd->system_type = system_type;


	act = sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd);
	ref_cnt_obj_release(cmd);
	
	return act;


}

/*lint  -save*/
/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_hdr_opr_oh_info5

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the HDR overhead
  information changes from not current to current. This happen under the
  following conditions:

    1. The SS-Client acquires a new paging channel (due to initial
       acquisition or idle handoff).

    2. The SS-Client updates the overhead information of the acquired system.

 hdr personality was added as parameter when moving from
 sd_ss_ind_hdr_opr_oh_info4 to sd_ss_ind_hdr_opr_oh_info5

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Acquire an HDR system as specified by the returned
                          "HDR acquisition" parameters.

    SD_SS_ACT_ACQ_GWL   -  Acquire a GSM/WCDMA/LTE system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
None.

===========================================================================*/






EXTERN  sd_ss_act_e_type          sd_ss_ind_hdr_opr_oh_info5(

        sd_band_e_type            band,
            /* Serving system's band class.
            */

        sd_chan_type              chan,
            /* Serving system's HDR channel.
            */

        const byte                sid_ptr[16],
            /* Pointer to array of 16 byte Sector ID.
            */

        byte                      subnet_mask_len,
            /* The subnet mask length that is associated with the Sector ID.
            */

        boolean                   is_ok_to_orig,
            /* Indicate whether the HDR protocol is currently able to process
            ** originations.
            */

        const sys_channel_type    *chan_ptr,
            /* Pointer to HDR hashing channel records.
            */

        word                      num_chan,
            /* The number of HDR hashing channel records specified in
            ** chan_ptr
            */

        sys_active_prot_e_type    hdr_active_prot,
            /* The active HDR protocol revision.
            */

        sys_personality_e_type    hdr_personality,
            /* The active HDR protocol revision.
            */

        int16                     local_time_offset,
            /* Local time Offset from system time
            */

        uint8                     leap_secs,
            /* Leap seconds occurred since start of system time
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{

  return sd_ss_ind_hdr_opr_oh_info6( band,
                                     chan,
                                     sid_ptr,
                                     subnet_mask_len,
                                     is_ok_to_orig,
                                     chan_ptr,
                                     num_chan,
                                     hdr_active_prot,
                                     hdr_personality,
                                     local_time_offset,
                                     leap_secs,
                                     0, /* mcc */
                                     0, /* Latitude */
                                     0, /* Longitude */
                                     0, /* system type */
                                     rtrn_act_ptr );

}


/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_hdr_opr_is_usable

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the HDR system
  becomes unusable for originations but still good for idling.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR -   Acquire an HDR system as specified by the returned
                          "HDR acquisition" parameters.

    SD_SS_ACT_ACQ_GWL  -   Attempt acquiring an GSM/WCDMA/LTE system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/







EXTERN  sd_ss_act_e_type          sd_ss_ind_hdr_opr_sys_usable(

        sd_sys_usable_e_type      usable_status,
            /* System usable status.
            */

        sd_ss_act_s_type    *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type                  act;
  sd_ss_ind_hdr_opr_sys_usable_cmd  *cmd;

  cmd = (sd_ss_ind_hdr_opr_sys_usable_cmd *) sdtask_sd_cmd_new(
                                              sizeof(sd_ss_ind_hdr_opr_sys_usable_cmd),
                                              (execute_fn) sdcmd_hdr_opr_sys_usable,
                                              rtrn_act_ptr,
                                              NULL
                                              );

  cmd->usable_status = usable_status;

  act = sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd);
  ref_cnt_obj_release(cmd);

  return act;
}

/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_hdr_opr_redir

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when MS receives an HDR
  redirection message.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Acquire an HDR system as specified by the returned
                          "HDR acquisition" parameters.

    SD_SS_ACT_ACQ_GWL   -  Acquire a GSM/WCDMA/LTE system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_MEAS_BACK-  Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/






EXTERN  sd_ss_act_e_type          sd_ss_ind_hdr_opr_redir(

        sd_ss_sys_get_f_type      *sys_get_func,
            /* Pointer to a system extraction function.
            */

        void                      *data_ptr,
            /* Pointer to SS-Client callback data block.
            */

        sd_ss_redir_e_type        redir_type,
            /* Redirection type - QC, Directed.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type            act;
  sd_ss_ind_hdr_opr_redir_cmd *cmd;

  cmd = (sd_ss_ind_hdr_opr_redir_cmd *) sdtask_sd_cmd_new(
                                              sizeof(sd_ss_ind_hdr_opr_redir_cmd),
                                              (execute_fn) sdcmd_hdr_opr_redir,
                                              rtrn_act_ptr,
                                              NULL
                                              );

  cmd->sys_get_func = sys_get_func;
  cmd->data_ptr = data_ptr;
  cmd->redir_type = redir_type;

  act = sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd);
  ref_cnt_obj_release(cmd);

  return act;
}



/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_hdr_opr_sys_lost

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the acquired HDR
  system is lost.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_GWL   -  Acquire a GSM/WCDMA/LTE system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Acquire an HDR system as specified by the returned
                          "HDR acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sd_ss_act_e_type          sd_ss_ind_hdr_opr_sys_lost(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{

  return( sd_ss_ind_hdr_opr_sys_lost2( SD_SS_SYS_LOST_NONE,
                                       rtrn_act_ptr ) );

}




/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_hdr_opr_sys_lost2

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the acquired HDR
  system is lost.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_GWL   -  Acquire a GSM/WCDMA/LTE system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Acquire an HDR system as specified by the returned
                          "HDR acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/






EXTERN  sd_ss_act_e_type          sd_ss_ind_hdr_opr_sys_lost2(

        sd_ss_sys_lost_e_type     sys_lost,
            /* System lost reason
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type                act;
  sd_ss_ind_hdr_opr_sys_lost_cmd *cmd;

  cmd = (sd_ss_ind_hdr_opr_sys_lost_cmd *) sdtask_sd_cmd_new(
                                              sizeof(sd_ss_ind_hdr_opr_sys_lost_cmd),
                                              (execute_fn) sdcmd_hdr_opr_sys_lost2,
                                              rtrn_act_ptr,
                                              NULL
                                              );

  cmd->sys_lost = sys_lost;

  act = sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd);
  ref_cnt_obj_release(cmd);

  return act;
}



/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_hdr_opr_sys_lost3

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the acquired HDR
  system is lost.

  This API is called by MMOC whenever HDR is deactivated because of GWL
  acquisition on main stack. It will report NO service to CM on HDR
  and trigger srv lost processing on HDR stack.


DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_GWL   -  Acquire a GSM/WCDMA/LTE system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Acquire an HDR system as specified by the returned
                          "HDR acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/

EXTERN  sd_ss_act_e_type          sd_ss_ind_hdr_opr_sys_lost3(

        sd_ss_sys_lost_e_type     sys_lost,
            /* System lost reason
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type                act;
  sd_ss_ind_hdr_opr_sys_lost_cmd *cmd;

  cmd = (sd_ss_ind_hdr_opr_sys_lost_cmd *) sdtask_sd_cmd_new(
                                              sizeof(sd_ss_ind_hdr_opr_sys_lost_cmd),
                                              (execute_fn) sdcmd_hdr_opr_sys_lost3,
                                              rtrn_act_ptr,
                                              NULL
                                              );

  cmd->sys_lost = sys_lost;

  act = sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd);
  ref_cnt_obj_release(cmd);

  return act;
}



/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_hdr_opr_resel_ok

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when HDR protocol enters
  a state that allows reselection to take place.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GWL   -  Acquire a GSM/WCDMA/LTE system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/

EXTERN  sd_ss_act_e_type          sd_ss_ind_hdr_opr_resel_ok(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  return sd_ss_generic_cmd(sdcmd_hdr_opr_resel_ok, rtrn_act_ptr);
}


/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_hdr_opr_acc_term

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when HDR access attempt
  terminates due to access denied, system lost, access state timeout, max
  access probes, reorder, intercept, BS release, MS release or normal access
  completion.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.
                          (Not valid for SD_SS_ACC_TERM_MAX_ACC).

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GWL   -  Acquire a GSM/WCDMA/LTE system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.
                          (Not valid for SD_SS_ACC_TERM_MAX_ACC).

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/






EXTERN  sd_ss_act_e_type          sd_ss_ind_hdr_opr_acc_term(

        sd_ss_acc_term_e_type     acc_term,
            /* Access termination reason.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type                act;
  sd_ss_ind_hdr_opr_acc_term_cmd  *cmd;

  cmd = (sd_ss_ind_hdr_opr_acc_term_cmd *) sdtask_sd_cmd_new(
                                              sizeof(sd_ss_ind_hdr_opr_acc_term_cmd),
                                              (execute_fn) sdcmd_hdr_opr_acc_term,
                                              rtrn_act_ptr,
                                              NULL
                                              );

  cmd->acc_term = acc_term;

  act = sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd);
  ref_cnt_obj_release(cmd);

  return act;
}


/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_hdr_opr_ses_abort

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when MS does not receive
  an HDR OpenResponse message within T-SBPANResponse seconds after sending
  the OpenRequest message.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Acquire an HDR system as specified by the returned
                          "HDR acquisition" parameters.

    SD_SS_ACT_ACQ_GWL   -  Acquire a GSM/WCDMA/LTE system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/


EXTERN  sd_ss_act_e_type          sd_ss_ind_hdr_opr_ses_abort(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  return sd_ss_generic_cmd(sdcmd_hdr_opr_ses_abort, rtrn_act_ptr);
}


/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_hdr_opr_ses_nego_timeout

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when MS does not receive
  an HDR OpenResponse message within T-SBPANResponse seconds after sending
  the OpenRequest message.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Acquire an HDR system as specified by the returned
                          "HDR acquisition" parameters.

    SD_SS_ACT_ACQ_GWL   -  Acquire a GSM/WCDMA/LTE system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/


EXTERN  sd_ss_act_e_type          sd_ss_ind_hdr_opr_ses_nego_timeout(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  return sd_ss_generic_cmd(sdcmd_hdr_opr_ses_nego_timeout, rtrn_act_ptr);
}


/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_hdr_opr_ses_opened

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when a session is opened
  successfully.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/




EXTERN  sd_ss_act_e_type          sd_ss_ind_hdr_opr_ses_opened(

  sd_ss_act_s_type          *rtrn_act_ptr
      /* Pointer to a buffer where to place returned SS-Action.
      */
)
{
  return sd_ss_generic_cmd(sdcmd_hdr_opr_ses_opened, rtrn_act_ptr);
}


/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_hdr_opr_ses_close

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when HDR session is
  closed.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Acquire an HDR system as specified by the returned
                          "HDR acquisition" parameters.

    SD_SS_ACT_ACQ_GWL   -  Acquire a GSM/WCDMA/LTE system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/






EXTERN  sd_ss_act_e_type          sd_ss_ind_hdr_opr_ses_close(

        sd_ss_ses_close_e_type    ses_close,
            /* Session close reason.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type                act;
  sd_ss_ind_hdr_opr_ses_close_cmd *cmd;

  cmd = (sd_ss_ind_hdr_opr_ses_close_cmd *) sdtask_sd_cmd_new(
                                              sizeof(sd_ss_ind_hdr_opr_ses_close_cmd),
                                              (execute_fn) sdcmd_hdr_opr_ses_close,
                                              rtrn_act_ptr,
                                              NULL
                                              );

  cmd->ses_close = ses_close;

  act = sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd);
  ref_cnt_obj_release(cmd);

  return act;
}



/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_hdr_opr_conv

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when HDR has entered
  traffic.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sd_ss_act_e_type          sd_ss_ind_hdr_opr_conv(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  return sd_ss_ind_hdr_opr_start_activity( rtrn_act_ptr,
                                           SD_SS_HDR_ACT_MASK_TRAFFIC);

} /* sd_ss_ind_hdr_opr_conv */

/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_hdr_opr_release

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when HDR has released
  traffic channels.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sd_ss_act_e_type          sd_ss_ind_hdr_opr_release(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  return sd_ss_ind_hdr_opr_stop_activity( rtrn_act_ptr,
                                           SD_SS_HDR_ACT_MASK_TRAFFIC);
} /* sd_ss_ind_hdr_opr_release */


/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_hdr_opr_bad_sys

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when HDR system is bad due
  to the following reasons.

  1.AP message with invalid max capsule length i.e. < 2
  2.SP message with hash channel on unsupported band
  3. TCA to unsupported band
  4.TCA with no pilots



DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/


EXTERN  sd_ss_act_e_type          sd_ss_ind_hdr_opr_bad_sys(

        sd_ss_bad_sys_e_type      reas,
            /* Bad system due to 'reas'.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type              act;
  sd_ss_ind_hdr_opr_bad_sys_cmd *cmd;

  cmd = (sd_ss_ind_hdr_opr_bad_sys_cmd *) sdtask_sd_cmd_new(
                                              sizeof(sd_ss_ind_hdr_opr_bad_sys_cmd),
                                              (execute_fn) sdcmd_hdr_opr_bad_sys,
                                              rtrn_act_ptr,
                                              NULL
                                              );

  cmd->reas = reas;

  act = sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd);
  ref_cnt_obj_release(cmd);

  return act;
}


/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_hdr_opr_start_activity

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when HDR starts a active
  (i.e. start BCMCS, or enter traffic channel.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/



EXTERN  sd_ss_act_e_type          sd_ss_ind_hdr_opr_start_activity(

        sd_ss_act_s_type          *rtrn_act_ptr,
            /* Pointer to a buffer where to place returned SS-Action.
            */
        sd_ss_hdr_act_mask_e_type hdr_act_mask
            /* Indicate the HDR activity to be started.
            */
)
{
  sd_ss_act_e_type                act;
  sd_ss_ind_hdr_opr_activity_cmd  *cmd;

  cmd = (sd_ss_ind_hdr_opr_activity_cmd *) sdtask_sd_cmd_new(
                                              sizeof(sd_ss_ind_hdr_opr_activity_cmd),
                                              (execute_fn) sdcmd_hdr_opr_start_activity,
                                              rtrn_act_ptr,
                                              NULL
                                              );

  cmd->hdr_act_mask = hdr_act_mask;

  act = sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd);
  ref_cnt_obj_release(cmd);

  return act;
}


/*===========================================================================

FUNCTION sd_efs_is_loc_bsr_enabled

DESCRIPTION
  API to query if location based BSR feature is enabled

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  TRUE if SDSSSCR_EFS_ITEM_LOC_BASE_BSR is enabled
  FALSE otherwise

SIDE EFFECTS
  None.

===========================================================================*/

boolean sd_efs_is_loc_bsr_enabled(void)
{
  return sdssscr_efs_item_get(SDSSSCR_EFS_ITEM_LOC_BASE_BSR,
                             sdss_map_ss_to_as_id(SD_SS_MAIN)) > 0;
}


/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_hdr_opr_stop_activity

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when HDR stops a active
  (i.e. stop BCMCS, or exit traffic channel.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/

EXTERN  sd_ss_act_e_type          sd_ss_ind_hdr_opr_stop_activity(

        sd_ss_act_s_type          *rtrn_act_ptr,
            /* Pointer to a buffer where to place returned SS-Action.
            */
        sd_ss_hdr_act_mask_e_type hdr_act_mask
            /* Indicate the HDR activity to be stopped.
            */
)
{
  sd_ss_act_e_type                act;
  sd_ss_ind_hdr_opr_activity_cmd  *cmd;

  cmd = (sd_ss_ind_hdr_opr_activity_cmd *) sdtask_sd_cmd_new(
                                              sizeof(sd_ss_ind_hdr_opr_activity_cmd),
                                              (execute_fn) sdcmd_hdr_opr_stop_activity,
                                              rtrn_act_ptr,
                                              NULL
                                              );

  cmd->hdr_act_mask = hdr_act_mask;

  act = sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd);
  ref_cnt_obj_release(cmd);

  return act;
}

#endif /* FEATURE_HDR */

/*===========================================================================

FUNCTION sd_ss_ind_hybr_1_misc_timer

DESCRIPTION
  Instructs the HDR client of the SS-Action to take when the system selection
  timer expires.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

  If reselection is allowed (per the resel_is_allowed parameter):

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Acquire an HDR system as specified by the returned
                          "HDR acquisition" parameters.

    SD_SS_ACT_ACQ_GWL   -  Acquire a GSM/WCDMA/LTE system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

  If reselection is not allowed (per the resel_is_allowed parameter):

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.


SIDE EFFECTS
  None.

===========================================================================*/







EXTERN  sd_ss_act_e_type          sd_ss_ind_hybr_1_misc_timer(

        boolean                   resel_is_allowed,
            /* Indicate whether reselection is currently allowed.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type          act;
  sd_ss_ind_misc_timer_cmd  *cmd;

  cmd = (sd_ss_ind_misc_timer_cmd *) sdtask_sd_cmd_new(
                                            sizeof(sd_ss_ind_misc_timer_cmd),
                                            (execute_fn) sdcmd_hybr_1_misc_timer,
                                            rtrn_act_ptr,
                                            NULL
                                            );

  cmd->resel_is_allowed = resel_is_allowed;

  act = sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd);
  ref_cnt_obj_release(cmd);

  return act;
}


#if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) || defined(FEATURE_SD_LTE)
/*===========================================================================

FUNCTION sd_si_compare_plmn

DESCRIPTION
Compare PLMNs and return true if they are same.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

/* ^L<EJECT> */
/*===========================================================================
=============================================================================
======================== GSM/WCDMA/LTE SS-INDICATIONS ===========================
=============================================================================
===========================================================================*/

/* Indications that are specific to GSM/WCDMA/LTE operation. These indications
** can be received from the SS-Client during GSM/WCDMA/LTE operation only; that
** is, after the SS-Client enters the GSM/WCDMA/LTE active state.
*/

/*===========================================================================

FUNCTION sd_ss_ind_gw_acq_plmn_failed_ss_2

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when PLMN acquisition
  fails.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_GWL   -  Acquire a GSM/WCDMA/LTE system as specified by the
                          returned "GW acquisition" parameters.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/


EXTERN  sd_ss_act_e_type          sd_ss_ind_gw_acq_plmn_failed_ss_2
(

        sys_sim_state_e_type        sim_state,
            /* SIM state
            */

        boolean                     data_suspend,
            /* Data suspend flag from REG.
            */

        sd_ss_gwl_scan_status_e_type scan_status,
            /* Scan status, did the scan complete or it was aborted due to
            ** timer expiry
            */

        sys_acq_status_e_type        acq_status,
            /* indicates if actually acquisition was performed */

        sd_ss_e_type                  ss,
            /* Stack id
            */

        sys_lte_cs_capability_e_type lte_cs_capability,
            /* LTE CS capability
            */

        uint16                       req_id,
            /* Request Id
            */

        sys_extend_srv_info_e_type   extend_srv_info,

        const sys_csg_info_s_type*   csg_info_ptr,
            /* CSG information
            */

        sys_plmn_id_s_type*        registered_plmn,
           /* Registered PLMN.
           */

        sd_ss_act_s_type           *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */

)
{
  sd_ss_act_e_type                  act;
  sd_ss_ind_gw_acq_plmn_failed_cmd  *cmd;
  sys_sim_state_e_type               lte_rpm_sim_state = SYS_SIM_STATE_NONE;

  cmd = (sd_ss_ind_gw_acq_plmn_failed_cmd *) sdtask_sd_cmd_new(
                                              sizeof(sd_ss_ind_gw_acq_plmn_failed_cmd),
                                              (execute_fn) sdcmd_gw_acq_plmn_failed_ss_2,
                                              rtrn_act_ptr,
                                              NULL
                                              );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* LTE RPM might adjust the SIM state to CS_PS_INVALID or PS_INVALID
  ** if event timer is still running and EMM generic timer has reach maximum
  */
  lte_rpm_sim_state = cmrpm_lte_update_sim_state();
  if ( (lte_rpm_sim_state == SYS_SIM_STATE_CS_PS_INVALID) ||
       (lte_rpm_sim_state == SYS_SIM_STATE_PS_INVALID) )
  {
    sim_state = lte_rpm_sim_state;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd->sim_state = sim_state;
  cmd->data_suspend = data_suspend;
  cmd->scan_status = scan_status;
  cmd->acq_status = acq_status;
  cmd->ss = ss;
  cmd->lte_cs_capability = lte_cs_capability;
  cmd->req_id = req_id;
  cmd->extend_srv_info = extend_srv_info;
  cmd->csg_info_ptr = csg_info_ptr;
  cmd->registered_plmn = registered_plmn;

  act = sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd);
  ref_cnt_obj_release(cmd);

  return act;
}

/**===========================================================================

@FUNCTION sd_ss_ind_gw_acq_plmn_success_ss_2

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when PLMN acquisition
  is successful.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_GW   -  Acquire a GSM/WCDMA system as specified by the
                          returned "GW acquisition" parameters.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

@SIDE EFFECTS
  None.

===========================================================================*/

EXTERN  sd_ss_act_e_type          sd_ss_ind_gw_acq_plmn_success_ss_2
(           /*lint -esym(715, chan) */ /* param not referenced */
        sys_plmn_id_s_type        plmn,
           /* PLMN.
           */

        sd_si_gwl_mode_info_s_type lac_tac_rac,
           /* LAC/TAC/RAC
           */

        sd_band_u_type        band_pref,
           /* Pointer to acquired band.
           */

        sys_chan_type             chan,
           /* Pointer to acquire channel.
           */

        sys_srv_capability_e_type plmn_srv_cap,
            /* PLMN service capability.
            */

        boolean                   is_plmn_forbidden,
            /* Is PLMN forbidden?
            */

        sys_sim_state_e_type      sim_state,
            /* SIM state
            */

        sys_srv_status_e_type     service_status,
            /* Service status.
            */

        sys_roam_status_e_type    roaming_ind,
            /* Roaming status.
            */

        sys_srv_domain_e_type     srv_domain,
            /* Service domain.
            */

        sd_mode_e_type            mode,
            /* Mode of the acquired system.
            */

        boolean                   data_suspend,
            /* Data suspend flag from REG.
            */

        sys_srv_status_e_type     cs_service_status,
            /* CS Service status.
            */

        sd_ss_e_type                ss,
            /* Stack id
            */

        sys_lte_cs_capability_e_type lte_cs_capability,
            /* LTE CS capability
            */

        sd_sib8_status            sib8_available,
            /** <sib8 status> */

        boolean                   lte_ims_voice_avail,
            /* If IMS voice supported */

        boolean                   lte_ims_emerg_avail,
            /* If IMS emerg supported */

        uint16                    req_id,
            /* Request Id */

        sys_extend_srv_info_e_type   extend_srv_info,
            /* Extended service info
              */

        sys_reg_domain_e_type     reg_domain,
            /* The REG domain value is used in SGLTE CAMPED_ONLY mode by the
            ** CM client to determine service availability.
            */

        const sys_csg_info_s_type *csg_info_ptr,
            /* CSG information
            */

        sys_plmn_id_s_type*        registered_plmn,
           /* Registered PLMN.
           */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type          act;
  sd_ss_ind_gw_srv_info_cmd *cmd;

  cmd = (sd_ss_ind_gw_srv_info_cmd *) sdtask_sd_cmd_new(
                                              sizeof(sd_ss_ind_gw_srv_info_cmd),
                                              (execute_fn) sdcmd_gw_acq_plmn_success_ss_2,
                                              rtrn_act_ptr,
                                              NULL
                                              );

  cmd->plmn = plmn;
  cmd->lac_tac_rac = lac_tac_rac;
  cmd->band_pref = band_pref;
  cmd->plmn_srv_cap = plmn_srv_cap;
  cmd->chan = chan;
  cmd->is_plmn_forbidden = is_plmn_forbidden;
  cmd->sim_state = sim_state;
  cmd->service_status = service_status;
  cmd->roaming_ind = roaming_ind;
  cmd->srv_domain = srv_domain;
  cmd->mode = mode;
  cmd->data_suspend = data_suspend;
  cmd->cs_service_status = cs_service_status;
  cmd->ss = ss;
  cmd->lte_cs_capability = lte_cs_capability;
  cmd->sib8_available = sib8_available;
  cmd->lte_ims_voice_avail = lte_ims_voice_avail;
  cmd->lte_ims_emerg_avail = lte_ims_emerg_avail;
  cmd->req_id = req_id;
  cmd->extend_srv_info = extend_srv_info;
  cmd->reg_domain = reg_domain;
  cmd->csg_info_ptr = csg_info_ptr;
  cmd->registered_plmn = registered_plmn;

  act = sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd);
  ref_cnt_obj_release(cmd);

  return act;
}

/* ^L<EJECT> */
/*===========================================================================


FUNCTION sd_ss_ind_gw_acq_plmn_camped_ss_1


DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when camped on a cell but
  not yet registered.


DEPENDENCIES
  sd_init() must have already been called to initialize SD.


RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:


    SD_SS_ACT_CONTINUE -  Continue


SIDE EFFECTS
  None.


===========================================================================*/
EXTERN  sd_ss_act_e_type          sd_ss_ind_gw_acq_plmn_camped_ss_1
(

        sd_mode_e_type            mode,
           /* Mode of the acquired system.
           */

        sd_band_u_type            band_pref,
           /* Pointer to acquired band.
           */

       sd_ss_e_type               ss,
           /* Stack id
           */

        sd_ss_act_s_type          *rtrn_act_ptr,
            /* Pointer to a buffer where to place returned SS-Action.
            */
        sys_plmn_id_s_type         plmn,
            /* PLMN ID
            */

        sys_lac_type               lac_id
            /* Location Area Code (LAC) ID
            */
)
{
  sd_ss_act_e_type                  act;
  sd_ss_ind_gw_acq_plmn_camped_cmd  *cmd;

  cmd = (sd_ss_ind_gw_acq_plmn_camped_cmd *) sdtask_sd_cmd_new(
                                              sizeof(sd_ss_ind_gw_acq_plmn_camped_cmd),
                                              (execute_fn) sdcmd_gw_acq_plmn_camped_ss,
                                              rtrn_act_ptr,
                                              NULL
                                              );

  cmd->mode = mode;
  cmd->band_pref = band_pref;
  cmd->ss = ss;
  cmd->plmn = plmn;
  cmd->lac_id = lac_id;

  act = sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd);
  ref_cnt_obj_release(cmd);

  return act;
}


/*===========================================================================

@FUNCTION sd_ss_ind_lte_suitable_search_end

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the protocol stack
  indicates that the LTE Connected mode be ended.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GWL  -  Acquire a GSM/WCDMA/LTE/TD-SCDMA system as specified
                          by the returned "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

@SIDE EFFECTS
  None.

===========================================================================*/


EXTERN  sd_ss_act_e_type          sd_ss_ind_lte_suitable_search_end(

        sd_ss_e_type              ss,
            /* SS on which the indication was recieved
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
)
{
  return sd_ss_generic_ss_cmd(sdcmd_lte_suitable_search_end, rtrn_act_ptr, ss);
}



void sd_ss_reset_rlf_conn_mode_status
(
     sd_ss_e_type ss
)
{
  (void) sd_ss_generic_ss_cmd((execute_fn) sdcmd_reset_rlf_conn_mode_status, NULL, ss);
}

/*===========================================================================
FUNCTION sd_ss_update_emc_attached

DESCRIPTION
Update SD about emergency attach status

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  None.

@SIDE EFFECTS
  None.

===========================================================================*/
void sd_ss_update_emc_attached(

       boolean   is_emc_attached
)
{
  sdss_set_emerg_attached_state(is_emc_attached);
}
/*===========================================================================

FUNCTION sd_ss_update_emerg_bar_status

DESCRIPTION
Update SD about emergency barring status

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  None.

@SIDE EFFECTS
  None.

===========================================================================*/
void sd_ss_update_emerg_bar_status
(
    sys_lte_emerg_access_status_e_type   emerg_bar_status
)
{
  sdss_set_emerg_bar_status(emerg_bar_status);
}
/*===========================================================================

FUNCTION sd_ss_set_lte_connected_mode

DESCRIPTION
   Updates the settings related to the LTE Connected mode

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN void    sd_ss_set_lte_connected_mode
(
       dword              connected_time,
            /* T311 : time out passed in from the network to indicate
               the duration of the connected mode. Note that value
               of 0 indicates that the UE is no longer in LTE connected
               state.
            */

        sd_ss_e_type       ss
)
{
  sdss_set_lte_connected_mode(ss , connected_time);
}


/*===========================================================================

FUNCTION sd_ss_set_wcdma_connected_mode

DESCRIPTION
   Updates the settings related to the WCDMA Connected mode

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN void             sd_ss_set_wcdma_connected_mode(

        dword   connected_time,
            /* T311 : time out passed in from the network to indicate
               the duration of the connected mode. Note that value
               of 0 indicates that the UE is no longer in LTE connected
               state.
            */

        sd_ss_e_type ss
)
{
  #if defined ( FEATURE_WRLF_SYSTEM_SEL)
  sdss_set_wcdma_connected_mode(connected_time, ss);
  #endif
}

/*===========================================================================

FUNCTION sd_ss_ind_gw_opr_srv_lost_ss

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when GW service
  is lost in WCDMA connected mode.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_GWL   -  Acquire a GSM/WCDMA/LTE/TD-SCDMA system as specified
                           by the returned "GW acquisition" parameters.

SIDE EFFECTS
  None.
===========================================================================*/

sd_ss_act_e_type          sd_ss_ind_gw_opr_srv_lost_ss
(

        sys_sim_state_e_type      sim_state,
            /* SIM state
            */

        boolean                   data_suspend,
            /* Data suspend flag from REG.
            */

        sd_ss_act_s_type          *rtrn_act_ptr,
            /* Pointer to a buffer where to place returned SS-Action.
            */

        uint32                    suitable_search_time,
        /* Timer in milliseconds that determines how long UE should
        ** so suitable search
        */

        sd_ss_e_type              ss
)
{
  return sd_ss_ind_gw_opr_srv_lost_ss_3(sim_state, data_suspend, rtrn_act_ptr,
                suitable_search_time, ss, SYS_ACQ_STATUS_SUCCESS, NULL);

}



/*===========================================================================

FUNCTION sd_ss_ind_gw_opr_srv_lost_ss_2

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when GW service
  is lost in WCDMA connected mode.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

  SD_SS_ACT_ACQ_GWL   -  Acquire a GSM/WCDMA/LTE/TD-SCDMA system as specified
                         by the returned "GW acquisition" parameters.

SIDE EFFECTS
  None.

===========================================================================*/
sd_ss_act_e_type          sd_ss_ind_gw_opr_srv_lost_ss_2
(

        sys_sim_state_e_type      sim_state,
            /* SIM state
            */

        boolean                   data_suspend,
            /* Data suspend flag from REG.
            */

        sd_ss_act_s_type          *rtrn_act_ptr,
            /* Pointer to a buffer where to place returned SS-Action.
            */

        uint32                    suitable_search_time,
        /* Timer in milliseconds that determines how long UE should
        ** so suitable search
        */

        sd_ss_e_type              ss,

        sys_acq_status_e_type     acq_status
)
{
  return sd_ss_ind_gw_opr_srv_lost_ss_3(sim_state, data_suspend, rtrn_act_ptr,
                                        suitable_search_time, ss, acq_status,
                                        NULL);
}

/*===========================================================================

FUNCTION sd_ss_ind_gw_opr_srv_lost_ss_3

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when GW service
  is lost in WCDMA connected mode.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

  SD_SS_ACT_ACQ_GWL   -  Acquire a GSM/WCDMA/LTE/TD-SCDMA system as specified
                         by the returned "GW acquisition" parameters.

SIDE EFFECTS
  None.

===========================================================================*/
sd_ss_act_e_type          sd_ss_ind_gw_opr_srv_lost_ss_3
(

        sys_sim_state_e_type      sim_state,
            /* SIM state
            */

        boolean                   data_suspend,
            /* Data suspend flag from REG.
            */

        sd_ss_act_s_type          *rtrn_act_ptr,
            /* Pointer to a buffer where to place returned SS-Action.
            */

        uint32                    suitable_search_time,
        /* Timer in milliseconds that determines how long UE should
        ** so suitable search
        */

        sd_ss_e_type              ss,

        sys_acq_status_e_type     acq_status,

        sys_plmn_id_s_type*       registered_plmn
           /* Registered PLMN.
           */
)
{
  sd_ss_act_e_type              act;
  sd_ss_ind_gw_opr_srv_lost_cmd *cmd;

  cmd = (sd_ss_ind_gw_opr_srv_lost_cmd *) sdtask_sd_cmd_new(
                                              sizeof(sd_ss_ind_gw_opr_srv_lost_cmd),
                                              (execute_fn) sdcmd_gw_opr_srv_lost_ss_3,
                                              rtrn_act_ptr,
                                              NULL
                                              );

  cmd->ss = ss;
  cmd->sim_state = sim_state;
  cmd->data_suspend = data_suspend;
  cmd->suitable_search_time = suitable_search_time;
  cmd->acq_status = acq_status;
  cmd->registered_plmn = registered_plmn;

  act = sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd);
  ref_cnt_obj_release(cmd);

  return act;
}


#if defined ( FEATURE_WRLF_SYSTEM_SEL)

/*===========================================================================

@FUNCTION sd_ss_ind_wcdma_suitable_search_end

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the protocol stack
  indicates that the WCDMA Connected mode be ended.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GWL  -  Acquire a GSM/WCDMA/LTE/TD-SCDMA system as specified
                          by the returned "GW acquisition" parameters.


    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

@SIDE EFFECTS
  None.

===========================================================================*/

EXTERN  sd_ss_act_e_type          sd_ss_ind_wcdma_suitable_search_end(
        sd_ss_e_type              ss,


        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
)
{
  return sd_ss_generic_ss_cmd(sdcmd_wcdma_suitable_search_end, rtrn_act_ptr, ss);
}

/*===========================================================================

@FUNCTION sd_ss_ind_wcdma_conn_mode_end

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the protocol stack
  indicates that the WCDMA Connected mode be ended.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GWL  -  Acquire a GSM/WCDMA/LTE/TD-SCDMA system as specified
                          by the returned "GW acquisition" parameters.


    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

@SIDE EFFECTS
  None.

===========================================================================*/

EXTERN  sd_ss_act_e_type          sd_ss_ind_wcdma_conn_mode_end(

        sd_ss_e_type              ss,

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
)
{
  return sd_ss_generic_ss_cmd(sdcmd_wcdma_conn_mode_end, rtrn_act_ptr, ss);
}

#endif /* FEATURE_WRLF_SYSTEM_SEL */

/*===========================================================================

FUNCTION sd_ss_ind_lte_opr_srv_lost_1

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when LTE service is lost
  due to RLF (LTE connected Mode).

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_GWL   -  Acquire a GSM/WCDMA/LTE/TD-SCDMA system as specified
                           by the returned "GW acquisition" parameters.


SIDE EFFECTS
  None.

===========================================================================*/



EXTERN  sd_ss_act_e_type          sd_ss_ind_lte_opr_srv_lost_1
(

        sys_sim_state_e_type      sim_state,
            /* SIM state
            */

        boolean                   data_suspend,
            /* Data suspend flag from REG.
            */

        sd_ss_act_s_type          *rtrn_act_ptr,
            /* Pointer to a buffer where to place returned SS-Action.
            */
        uint32                    connected_time,
          /* Timer in milliseconds that determines how long the system
             remains in LTE connected mode
          */

        sys_acq_status_e_type    acq_status,
        /* ACQ STATUS reported */

        sd_ss_e_type              ss,
            /* SS on which the indication was recieved
            */

        sys_plmn_id_s_type        *registered_plmn
           /* Registered PLMN */
)
{
  sd_ss_act_e_type                act;
  sd_ss_ind_lte_opr_srv_lost_cmd  *cmd;

  cmd = (sd_ss_ind_lte_opr_srv_lost_cmd *) sdtask_sd_cmd_new(
                                              sizeof(sd_ss_ind_lte_opr_srv_lost_cmd),
                                              (execute_fn) sdcmd_lte_opr_srv_lost_1,
                                              rtrn_act_ptr,
                                              NULL
                                              );

  cmd->ss = ss;
  cmd->sim_state = sim_state;
  cmd->data_suspend = data_suspend;
  cmd->connected_time = connected_time;
  cmd->registered_plmn = registered_plmn;
  cmd->acq_status = acq_status;

  act = sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd);
  ref_cnt_obj_release(cmd);

  return act;
}

/* ^L<EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_gw_opr_srv_lost

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when GW service is lost.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_GWL   -  Acquire a GSM/WCDMA/LTE/TD-SCDMA system as specified
                           by the returned "GW acquisition" parameters.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sd_ss_act_e_type          sd_ss_ind_gw_opr_srv_lost
(

        sys_sim_state_e_type      sim_state,
            /* SIM state
            */

        boolean                   data_suspend,
            /* Data suspend flag from REG.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{

  return sd_ss_ind_gw_opr_srv_lost_ss(
    sim_state,
    data_suspend,
    rtrn_act_ptr,
    0,
    #ifdef FEATURE_HYBR_3GPP
    SD_SS_HYBR_2
    #elif defined(CFG_HYBR_GW_3)
    SD_SS_HYBR_3
    #else
    SD_SS_MAIN
    #endif

    );

} /*  sd_ss_ind_gw_opr_srv_lost() */


/*===========================================================================

FUNCTION sdcmd_gw_opr_srv_info_ss_1

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when GW overhead
  information is changed.

  This function is with CSG support.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue

    SD_SS_ACT_ACQ_GWL   -  Acquire a GSM/WCDMA/LTE/TD-SCDMA system as specified
                           by the returned "GW acquisition" parameters.


SIDE EFFECTS
  None.

===========================================================================*/



/*===========================================================================

FUNCTION sd_ss_ind_gw_opr_srv_info_ss

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when GW overhead
  information is changed.

  This function is with CSG support.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue

    SD_SS_ACT_ACQ_GWL   -  Acquire a GSM/WCDMA/LTE/TD-SCDMA system as specified
                           by the returned "GW acquisition" parameters.


SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sd_ss_act_e_type          sd_ss_ind_gw_opr_srv_info_ss_1
(          /*lint -esym(715, chan) */ /* param not referenced */
        sys_plmn_id_s_type        plmn,
           /* PLMN.
           */

        sd_si_gwl_mode_info_s_type lac_tac_rac,
           /* LAC/TAC/RAC
           */

        sd_band_u_type        band_pref,
           /* Pointer to acquired band.
           */

        sys_chan_type             chan,
           /* Pointer to acquire channel.
           */

        sys_srv_capability_e_type plmn_srv_cap,
            /* PLMN service capability.
            */

        boolean                   is_plmn_forbidden,
           /* Is PLMN forbidden?
           */

        sys_sim_state_e_type      sim_state,
            /* SIM state
            */

        sys_srv_status_e_type     service_status,
           /* Service status.
           */

        sys_roam_status_e_type    roaming_ind,
           /* Roaming status.
           */

        sys_srv_domain_e_type     srv_domain,
           /* Registered service domain.
           */

        sd_mode_e_type            mode,
           /* Mode of the acquired system.
           */

        boolean                   data_suspend,
            /* Data suspend flag from REG.
            */

        sys_srv_status_e_type     cs_service_status,
           /* CS Service status.
           */

        sd_ss_e_type                ss,
           /* Stack id */

        sys_lte_cs_capability_e_type lte_cs_capability,
           /* LTE CS capability
           */
        sd_sib8_status             sib8_available,
           /** <sib8 status> */

        boolean                    lte_ims_voice_avail,
            /* If IMS voice supported */

        boolean                    lte_ims_emerg_avail,
            /* If IMS emerg supported */

        sys_extend_srv_info_e_type   extend_srv_info,
              /* Extended service info
              */

        sys_reg_domain_e_type     reg_domain,


        const sys_csg_info_s_type* csg_info_ptr,
            /* CSG Information */

        sys_plmn_id_s_type*        registered_plmn,
           /* Registered PLMN.
           */

        sd_ss_act_s_type*          rtrn_act_ptr
           /* Pointer to a buffer where to place returned SS-Action.
           */
)
/*lint -esym(715, sib8_available) */ /* param not referenced */
{
  sd_ss_act_e_type          act;
  sd_ss_ind_gw_srv_info_cmd *cmd;

  cmd = (sd_ss_ind_gw_srv_info_cmd *) sdtask_sd_cmd_new(
                                              sizeof(sd_ss_ind_gw_srv_info_cmd),
                                              (execute_fn) sdcmd_gw_opr_srv_info_ss_1,
                                              rtrn_act_ptr,
                                              NULL
                                              );

  cmd->plmn = plmn;
  cmd->lac_tac_rac = lac_tac_rac;
  cmd->band_pref = band_pref;
  cmd->plmn_srv_cap = plmn_srv_cap;
  cmd->chan = chan;
  cmd->is_plmn_forbidden = is_plmn_forbidden;
  cmd->sim_state = sim_state;
  cmd->service_status = service_status;
  cmd->roaming_ind = roaming_ind;
  cmd->srv_domain = srv_domain;
  cmd->mode = mode;
  cmd->data_suspend = data_suspend;
  cmd->cs_service_status = cs_service_status;
  cmd->ss = ss;
  cmd->lte_cs_capability = lte_cs_capability;
  cmd->sib8_available = sib8_available;
  cmd->lte_ims_voice_avail = lte_ims_voice_avail;
  cmd->lte_ims_emerg_avail = lte_ims_emerg_avail;
  cmd->req_id = 0;
  cmd->extend_srv_info = extend_srv_info;
  cmd->reg_domain = reg_domain;
  cmd->csg_info_ptr = csg_info_ptr;
  cmd->registered_plmn = registered_plmn;

  act = sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd);
  ref_cnt_obj_release(cmd);

  return act;
}/* sd_ss_ind_gw_opr_srv_info_ss() */

/* ^L<EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_gw_opr_acc_done

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the GW access
  is completed.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_GWL   -  Acquire a GSM/WCDMA/LTE/TD-SCDMA system as specified
                           by the returned "GW acquisition" parameters.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Acquire an HDR system as specified by the returned
                          "HDR acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/





EXTERN  sd_ss_act_e_type          sd_ss_ind_gw_opr_acc_done_ss(

        sd_ss_e_type                 ss,
        /* Stack ID */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  return sd_ss_generic_ss_cmd(sdcmd_gw_opr_acc_done_ss, rtrn_act_ptr, ss);
}

/**===========================================================================

@FUNCTION sd_ss_ind_gw_ps_detach

@DESCRIPTION
  Informs SD of PS detach

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
   None

@SIDE EFFECTS
  None.

===========================================================================*/


EXTERN  sd_ss_act_e_type    sd_ss_ind_gw_ps_detach (
        sd_ss_e_type ss,
            /* SS for which detach is successful */

        sd_ss_act_s_type          *rtrn_act_ptr
           /* Pointer to a buffer where to place returned SS-Action.
           */
)
{
  return sd_ss_generic_ss_cmd(sdcmd_gw_ps_detach, rtrn_act_ptr, ss);
}

/**===========================================================================

@FUNCTION sd_ss_ind_misc_update_req_id

@DESCRIPTION
CMREGPRX updates SD about the sys_sel_pref_req_id in cmregprx stack info
            to update the clients about serving status

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
None

@SIDE EFFECTS
  None.

===========================================================================*/
void sd_ss_ind_misc_update_req_id
(
    sd_ss_e_type               ss,
    /* Stack id
      */
    uint16                     req_id
    /* Request Id
      */
)
{
    sdcmd_update_sys_sel_pref_req_id(ss, req_id);
}


#endif /* #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) */


/* <EJECT> */
/*===========================================================================
=============================================================================
======================== MISCELLANEOUS SS-INDICATIONS =======================
=============================================================================
===========================================================================*/

/* Indications that are not USER, CDMA or AMPS related. These indications are
** usually mode independent; or in other words, they can be received in more
** than one of the CP modes (e.g. CDMA, AMPS or PWR_SAVE).
*/

/*===========================================================================

FUNCTION sd_ss_ind_misc_srv_lost

DESCRIPTION
  Indicates to SD that service is lost on a given stack. This will set the
  service state to SYS_SRV_STATUS_NO_SRV_INTERNAL if the current service is
  not NO_SRV or NO_SRV_INTERNAL or PWR_SAVE in SD (for the given SS) and
  propagate the service loss info to CM (or other registered clients)

  There will be no call to SD scripts and no returned action

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void          sd_ss_ind_misc_srv_lost
(

        sd_ss_e_type              ss
            /* stack on which indication comes
            */
)
{
  sd_si_info_s_type     *si_info_ptr = &sd_si_ptr(ss)->si_info;

  if(sys_srv_status_is_srv(si_info_ptr->srv_status))
  {
    // Set the service status to SYS_SRV_STATUS_NO_SRV_INTERNAL for the corresponding stack
    sd_si_srv_off (ss, TRUE);
  }

} /*  sd_ss_ind_misc_srv_lost() */


/*===========================================================================

FUNCTION sd_ss_ind_misc_timer

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the system selection
  timer expires.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

  If reselection is allowed (per the resel_is_allowed parameter):

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Acquire an HDR system as specified by the returned
                          "HDR acquisition" parameters.

    SD_SS_ACT_ACQ_GWL   -  Acquire a GSM/WCDMA/LTE/TD-SCDMA system as specified
                           by the returned "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

  If reselection is not allowed (per the resel_is_allowed parameter):

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.


SIDE EFFECTS
  None.

===========================================================================*/

EXTERN  sd_ss_act_e_type          sd_ss_ind_misc_timer(

        boolean                   resel_is_allowed,
            /* Indicate whether reselection is currently allowed.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type          act;
  sd_ss_ind_misc_timer_cmd  *cmd;

  cmd = (sd_ss_ind_misc_timer_cmd *) sdtask_sd_cmd_new(
                                            sizeof(sd_ss_ind_misc_timer_cmd),
                                            (execute_fn) sdcmd_misc_timer,
                                            rtrn_act_ptr,
                                            NULL
                                            );

  cmd->resel_is_allowed = resel_is_allowed;

  act = sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd);
  ref_cnt_obj_release(cmd);

  return act;
}


/*===========================================================================

FUNCTION sd_ss_ind_hybr_2_misc_timer

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the system selection
  timer expires.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

  If reselection is allowed (per the resel_is_allowed parameter):

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Acquire an HDR system as specified by the returned
                          "HDR acquisition" parameters.

    SD_SS_ACT_ACQ_GW   -  Acquire a GSM/WCDMA system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

  If reselection is not allowed (per the resel_is_allowed parameter):

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.


SIDE EFFECTS
  None.

===========================================================================*/

EXTERN  sd_ss_act_e_type          sd_ss_ind_hybr_2_misc_timer(

        boolean                   resel_is_allowed,
            /**< Indicate whether reselection is currently allowed.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type          act;
  sd_ss_ind_misc_timer_cmd  *cmd;

  cmd = (sd_ss_ind_misc_timer_cmd *) sdtask_sd_cmd_new(
                                            sizeof(sd_ss_ind_misc_timer_cmd),
                                            (execute_fn) sdcmd_hybr_2_misc_timer,
                                            rtrn_act_ptr,
                                            NULL
                                            );

  cmd->resel_is_allowed = resel_is_allowed;

  act = sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd);
  ref_cnt_obj_release(cmd);

  return act;
}


/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_hybr_2_misc_meas_rprt

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the channel
  strength measurement that was requested by SD is completed and reported
  back to SD.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

  If reselection is allowed (per the resel_is_allowed parameter):

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Acquire an HDR system as specified by the returned
                          "HDR acquisition" parameters.

    SD_SS_ACT_ACQ_GW   -  Acquire a GSM/WCDMA system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

  If reselection is not allowed (per the resel_is_allowed parameter):

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.


SIDE EFFECTS
  None.

===========================================================================*/

EXTERN  sd_ss_act_e_type          sd_ss_ind_hybr_2_misc_meas_rprt(

        word                      meas_id,
            /* The measurement ID. This IS should match the ID that was
            ** provided to the SS-Client with the measurement request (i.e.,
            ** SD_SS_ACT_MEAS_BACK or SD_SS_ACT_MEAS_DED) SS-Action.
            */

        boolean                   resel_is_allowed,
            /* Indicate whether reselection is currently allowed.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type              act;
  sd_ss_ind_misc_meas_rprt_cmd  *cmd;

  cmd = (sd_ss_ind_misc_meas_rprt_cmd *) sdtask_sd_cmd_new(
                                            sizeof(sd_ss_ind_misc_meas_rprt_cmd),
                                            (execute_fn) sdcmd_hybr_2_misc_meas_rprt,
                                            rtrn_act_ptr,
                                            NULL
                                            );

  cmd->meas_id = meas_id;
  cmd->resel_is_allowed = resel_is_allowed;;

  act = sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd);
  ref_cnt_obj_release(cmd);

  return act;
}


/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_hybr_2_misc_pwr_save_enter

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the MS enters power
  save mode of operation.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Acquire an HDR system as specified by the returned
                          "HDR acquisition" parameters.

    SD_SS_ACT_ACQ_GW   -  Acquire a GSM/WCDMA system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

SIDE EFFECTS
  None.

===========================================================================*/


EXTERN  sd_ss_act_e_type          sd_ss_ind_hybr_2_misc_pwr_save_enter(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  return sd_ss_generic_cmd(sdcmd_hybr_2_misc_pwr_save_enter, rtrn_act_ptr);
}

#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
/*===========================================================================

FUNCTION sd_ss_ind_hybr_3_misc_timer

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the system selection
  timer expires.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

  If reselection is allowed (per the resel_is_allowed parameter):

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Acquire an HDR system as specified by the returned
                          "HDR acquisition" parameters.

    SD_SS_ACT_ACQ_GW   -  Acquire a GSM/WCDMA system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

  If reselection is not allowed (per the resel_is_allowed parameter):

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.


SIDE EFFECTS
  None.

===========================================================================*/

EXTERN  sd_ss_act_e_type          sd_ss_ind_hybr_3_misc_timer(

        boolean                   resel_is_allowed,
            /**< Indicate whether reselection is currently allowed.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type          act;
  sd_ss_ind_misc_timer_cmd  *cmd;

  cmd = (sd_ss_ind_misc_timer_cmd *) sdtask_sd_cmd_new(
                                            sizeof(sd_ss_ind_misc_timer_cmd),
                                            (execute_fn) sdcmd_hybr_3_misc_timer,
                                            rtrn_act_ptr,
                                            NULL
                                            );

  cmd->resel_is_allowed = resel_is_allowed;

  act = sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd);
  ref_cnt_obj_release(cmd);

  return act;
}


/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_hybr_3_misc_meas_rprt

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the channel
  strength measurement that was requested by SD is completed and reported
  back to SD.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

  If reselection is allowed (per the resel_is_allowed parameter):

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Acquire an HDR system as specified by the returned
                          "HDR acquisition" parameters.

    SD_SS_ACT_ACQ_GW   -  Acquire a GSM/WCDMA system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

  If reselection is not allowed (per the resel_is_allowed parameter):

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.


SIDE EFFECTS
  None.

===========================================================================*/

EXTERN  sd_ss_act_e_type          sd_ss_ind_hybr_3_misc_meas_rprt(

        word                      meas_id,
            /* The measurement ID. This IS should match the ID that was
            ** provided to the SS-Client with the measurement request (i.e.,
            ** SD_SS_ACT_MEAS_BACK or SD_SS_ACT_MEAS_DED) SS-Action.
            */

        boolean                   resel_is_allowed,
            /* Indicate whether reselection is currently allowed.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type              act;
  sd_ss_ind_misc_meas_rprt_cmd  *cmd;

  cmd = (sd_ss_ind_misc_meas_rprt_cmd *) sdtask_sd_cmd_new(
                                            sizeof(sd_ss_ind_misc_meas_rprt_cmd),
                                            (execute_fn) sdcmd_hybr_3_misc_meas_rprt,
                                            rtrn_act_ptr,
                                            NULL
                                            );

  cmd->meas_id = meas_id;
  cmd->resel_is_allowed = resel_is_allowed;;

  act = sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd);
  ref_cnt_obj_release(cmd);

  return act;
}


/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_hybr_3_misc_pwr_save_enter

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the MS enters power
  save mode of operation.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Acquire an HDR system as specified by the returned
                          "HDR acquisition" parameters.

    SD_SS_ACT_ACQ_GW   -  Acquire a GSM/WCDMA system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

SIDE EFFECTS
  None.

===========================================================================*/


EXTERN  sd_ss_act_e_type          sd_ss_ind_hybr_3_misc_pwr_save_enter(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  return sd_ss_generic_cmd(sdcmd_hybr_3_misc_pwr_save_enter, rtrn_act_ptr);
}

#endif

/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_misc_meas_rprt

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the channel
  strength measurement that was requested by SD is completed and reported
  back to SD.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

  If reselection is allowed (per the resel_is_allowed parameter):

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Acquire an HDR system as specified by the returned
                          "HDR acquisition" parameters.

    SD_SS_ACT_ACQ_GWL   - Acquire a GSM/WCDMA/LTE/TD-SCDMA system as specified
                          by the returned "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

  If reselection is not allowed (per the resel_is_allowed parameter):

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.


SIDE EFFECTS
  None.

===========================================================================*/


EXTERN  sd_ss_act_e_type          sd_ss_ind_misc_meas_rprt(

        word                      meas_id,
            /* The measurement ID. This IS should match the ID that was
            ** provided to the SS-Client with the measurement request (i.e.,
            ** SD_SS_ACT_MEAS_BACK or SD_SS_ACT_MEAS_DED) SS-Action.
            */

        boolean                   resel_is_allowed,
            /* Indicate whether reselection is currently allowed.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type              act;
  sd_ss_ind_misc_meas_rprt_cmd  *cmd;

  cmd = (sd_ss_ind_misc_meas_rprt_cmd *) sdtask_sd_cmd_new(
                                            sizeof(sd_ss_ind_misc_meas_rprt_cmd),
                                            (execute_fn) sdcmd_misc_meas_rprt,
                                            rtrn_act_ptr,
                                            NULL
                                            );

  cmd->meas_id = meas_id;
  cmd->resel_is_allowed = resel_is_allowed;;

  act = sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd);
  ref_cnt_obj_release(cmd);

  return act;
}


/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_misc_orig_success

DESCRIPTION
  Inform SD of origination success.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/

EXTERN  sd_ss_act_e_type          sd_ss_ind_misc_orig_success(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  return sd_ss_generic_cmd(sdcmd_misc_orig_success, rtrn_act_ptr);
}


/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_misc_pwr_save_enter

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the MS enters power
  save mode of operation.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Acquire an HDR system as specified by the returned
                          "HDR acquisition" parameters.

    SD_SS_ACT_ACQ_GWL   - Acquire a GSM/WCDMA/LTE/TD-SCDMA system as specified
                          by the returned "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

SIDE EFFECTS
  None.

===========================================================================*/

EXTERN  sd_ss_act_e_type          sd_ss_ind_misc_pwr_save_enter(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  return sd_ss_generic_cmd(sdcmd_misc_pwr_save_enter, rtrn_act_ptr);
}


/*===========================================================================

FUNCTION sd_ss_ind_hybr_1_misc_pwr_save_enter

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the MS enters power
  save mode of operation.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Acquire an HDR system as specified by the returned
                          "HDR acquisition" parameters.

    SD_SS_ACT_ACQ_GWL   - Acquire a GSM/WCDMA/LTE/TD-SCDMA system as specified
                          by the returned "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

SIDE EFFECTS
  None.

===========================================================================*/

EXTERN  sd_ss_act_e_type          sd_ss_ind_hybr_1_misc_pwr_save_enter(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  return sd_ss_generic_cmd(sdcmd_hybr_1_misc_pwr_save_enter, rtrn_act_ptr);
}




EXTERN  sd_ss_act_e_type          sd_ss_ind_misc_cnf(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  return sd_ss_generic_cmd(sdcmd_misc_cnf, rtrn_act_ptr);
}


#if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)

/*===========================================================================

FUNCTION sd_ss_ind_misc_gw_net_list_rpt

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the Lower layers
  report the results of previous Get Network list request.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR -   Acquire an HDR system as specified by the returned
                          "HDR acquisition" parameters.

    SD_SS_ACT_ACQ_GWL   - Acquire a GSM/WCDMA/LTE/TD-SCDMA system as specified
                          by the returned "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sd_ss_act_e_type              sd_ss_ind_misc_gw_net_list_rpt
(

        sys_plmn_id_s_type            *rplmn_ptr,
            /* Pointer to a structure containing the last registered PLMN ID.
            */

        sys_found_plmn_list_u_type  *plmn_list_ptr,
            /* Pointer to a structure containing the list of available PLMNs.
            */

        sd_ss_act_s_type              *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{

  return sd_ss_ind_misc_gw_net_list_rpt2( rplmn_ptr,
                                         plmn_list_ptr,
                                         SYS_PLMN_LIST_SUCCESS,
                                         rtrn_act_ptr);
}/*sd_ss_ind_misc_gw_net_list_rpt*/


/* ^L<EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_misc_gw_net_list_rpt2

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the Lower layers
  report the results of previous Get Network list request.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR -   Acquire an HDR system as specified by the returned
                          "HDR acquisition" parameters.

    SD_SS_ACT_ACQ_GWL -   Acquire a GSM/WCDMA/LTE/TD-SCDMA system as specified
                          by the returned "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sd_ss_act_e_type              sd_ss_ind_misc_gw_net_list_rpt2
(

        sys_plmn_id_s_type            *rplmn_ptr,
            /* Pointer to a structure containing the last registered PLMN ID.
            */

        sys_found_plmn_list_u_type *plmn_list_ptr,
            /* Pointer to a structure containing the list of available PLMNs.
            */

        sys_plmn_list_status_e_type   plmn_list_status,
            /* The PLMN list status indication.
            */

        sd_ss_act_s_type              *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
    return sd_ss_ind_misc_gw_net_list_rpt_ss( rplmn_ptr,
                                         plmn_list_ptr,
                                         plmn_list_status,
                                         #ifdef FEATURE_HYBR_3GPP
                                         SD_SS_HYBR_2,
                                         #elif defined(CFG_HYBR_GW_3)
                                         SD_SS_HYBR_3,
                                         #else
                                         SD_SS_MAIN,
                                         #endif
                                         rtrn_act_ptr);
}

/* ^L<EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_misc_gw_net_list_rpt2

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the Lower layers
  report the results of previous Get Network list request.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR -   Acquire an HDR system as specified by the returned
                          "HDR acquisition" parameters.

    SD_SS_ACT_ACQ_GWL -   Acquire a GSM/WCDMA/LTE/TD-SCDMA system as specified
                          by the returned "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/




EXTERN  sd_ss_act_e_type              sd_ss_ind_misc_gw_net_list_rpt_ss
(

        sys_plmn_id_s_type            *rplmn_ptr,
            /* Pointer to a structure containing the last registered PLMN ID.
            */

        sys_found_plmn_list_u_type  *plmn_list_ptr,
            /* Pointer to a structure containing the list of available PLMNs.
            */

        sys_plmn_list_status_e_type   plmn_list_status,
            /* The PLMN list status indication.
            */
        sd_ss_e_type                      ss,

        sd_ss_act_s_type              *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  sd_ss_act_e_type                      act;
  sd_ss_ind_misc_gw_net_list_rpt_ss_cmd *cmd;

  cmd = (sd_ss_ind_misc_gw_net_list_rpt_ss_cmd *) sdtask_sd_cmd_new(
                                            sizeof(sd_ss_ind_misc_gw_net_list_rpt_ss_cmd),
                                            (execute_fn) sdcmd_misc_gw_net_list_rpt_ss,
                                            rtrn_act_ptr,
                                            NULL
                                            );

  cmd->ss = ss;
  cmd->rplmn_ptr = rplmn_ptr;
  cmd->plmn_list_ptr = plmn_list_ptr;
  cmd->plmn_list_status = plmn_list_status;

  act = sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd);
  ref_cnt_obj_release(cmd);

  return act;
}
/* ^L<EJECT> */
/*===========================================================================


FUNCTION sd_ss_ind_csg_nw_sel_mode_update_ss


DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when csg network sel mode update is
  received from NAS.


DEPENDENCIES
  sd_init() must have already been called to initialize SD.


RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:


    SD_SS_ACT_CONTINUE -  Continue


SIDE EFFECTS
  None.


===========================================================================*/


EXTERN  sd_ss_act_e_type          sd_ss_ind_csg_nw_sel_mode_update_ss
(

        /* Network selection mode 
             */
        sys_network_selection_mode_e_type        nw_sel_mode,
        
          /* Corresponding SD SS to be used for this stack
          */
          sd_ss_e_type                          ss


)
{
   sd_ss_act_s_type        ss_act;

  (void)sd_ss_act_get_true_curr_act_ss(ss,&ss_act);
   ss_act.prm.acq_gwl.net_sel_type = nw_sel_mode;
   sd_ss_act_set_true_curr_act_ss(ss,&ss_act);

  return SD_SS_ACT_CONTINUE;
}


/* ^L<EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_gw_data_suspended_ss

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the Lower layers
  report the results of previous Get Network list request.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SD_SS_ACT_CONTINUE

SIDE EFFECTS
  None.

===========================================================================*/
// TODO: Look into whether this needs task protectionn
EXTERN  sd_ss_act_e_type    sd_ss_ind_gw_data_suspended_ss (sd_ss_e_type ss)
{

  sd_si_info_s_type         *si_info_ptr = &sd_si_ptr(ss)->si_info;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)
  /* Update the data suspend state.
  */
  sd_si_update_data_suspend_state (ss, TRUE,  si_info_ptr->srv_status);
#endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return SD_SS_ACT_CONTINUE;

} /*  sd_ss_ind_gw_data_suspended () */

#endif /* SD_GSM_SUPPORTED || SD_WCDMA_SUPPORTED */


#ifdef FEATURE_HDR
/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_misc_hdr_bcmcs_srv_status

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when HDR has bcmcs srv
  has changed.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/



EXTERN  sd_ss_act_e_type                sd_ss_ind_misc_hdr_bcmcs_srv_status
(


        sys_bcmcs_srv_status_e_type     bcmcs_srv_status,
           /* New BCMCS service status.
           */

        sd_ss_act_s_type                *rtrn_act_ptr
           /* Pointer to a buffer where to place returned SS-Action.
           */
)
{
  sd_ss_act_e_type                        act;
  sd_ss_ind_misc_hdr_bcmcs_srv_status_cmd *cmd;

  cmd = (sd_ss_ind_misc_hdr_bcmcs_srv_status_cmd *) sdtask_sd_cmd_new(
                                            sizeof(sd_ss_ind_misc_hdr_bcmcs_srv_status_cmd),
                                            (execute_fn) sdcmd_misc_hdr_bcmcs_srv_status,
                                            rtrn_act_ptr,
                                            NULL
                                            );

  cmd->bcmcs_srv_status = bcmcs_srv_status;

  act = sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd);
  ref_cnt_obj_release(cmd);

  return act;
}


#endif /* FEATURE_HDR */


/*===========================================================================
=============================================================================
=============================================================================
========================== MISCELLANEOUS SERVICES ===========================
=============================================================================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION sd_misc_prl_ver_get

DESCRIPTION
  Returns the version number of the PRL that is stored in NV and associated
  with the currently selected NAM.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  Version number of the PRL that is stored in NV and associated with the
  currently selected NAM. PRL_DEFAULT_VER if no PRL is stored in NV or PRL
  has no version number associated with it.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  word                      sd_misc_prl_ver_get( void )
{
  return prl_hdr_nv_get_ptr()->pr_list_id;
}




/* <EJECT> */
/*===========================================================================

FUNCTION sd_misc_is_target_supp_mode_band

  Inform the caller to whether a specified combination of mode and band
  preference is supported by the current target.

  For example, SD_SS_MODE_PREF_AMPS is only supported by a target that is
  AMPS capable.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  TRUE if the specified combination of mode and band preference is supported
  by the current target. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  boolean                   sd_misc_is_target_supp_mode_band(

        sd_ss_mode_pref_e_type    mode_pref,
            /* Mode preference to be validated.
            */

        sd_ss_band_pref_e_type    band_pref,
            /* Band preference to be validated.
            */

        sys_lte_band_mask_e_type  lte_band_pref,
            /* LTE band preference to be validated.
            */

        sd_ss_band_pref_e_type    tds_band_pref
            /* TD-SCDMA band preference to be validated.
            */

)
{
  /* If the specified mode and band preference combination is NOT supported
  ** by the current target, return FALSE.
  */
  if( (! sdss_is_supp_mode_band_pref(mode_pref, band_pref))
     && (! sdss_is_supp_mode_band_pref_tds(mode_pref, tds_band_pref) )
      #ifdef FEATURE_SD_LTE
      &&
      (! sdss_is_supp_mode_band_pref_lte(mode_pref,lte_band_pref))
      #endif
  )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifndef FEATURE_SD_LTE
  lte_band_pref = SYS_LTE_BAND_MASK_CONST_NONE;
  #endif

  /* If we got here, the specified mode and band preference combination is
  ** supported - return TRUE.
  */
  return TRUE;

} /* sd_misc_is_target_supp_mode_band */



/* <EJECT> */
/*===========================================================================

FUNCTION sd_misc_get_mode_band_capability

  Inform the called the modes and bands preference supported by the target.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                   sd_misc_get_mode_band_capability(

        sd_ss_mode_pref_e_type    *mode_pref_ptr,
            /* Mode preferences to be filled in.
            */

        sd_ss_band_pref_e_type    *band_pref_ptr,
            /* Band preferences to be filled in.
            */

        sys_lte_band_mask_e_type  *lte_band_pref_ptr,
            /* LTE band preferences to be filled in.
            */

        sd_ss_band_pref_e_type    *tds_band_pref_ptr
            /* TD-SCDMA band preferences to be filled in.
            */

)
{
  sdss_get_mode_band_capability_mmode(mode_pref_ptr,
                                      band_pref_ptr,
                                      lte_band_pref_ptr,
                                      tds_band_pref_ptr);

} /* sd_misc_get_mode_band_capability */



/* <EJECT> */
/*===========================================================================

FUNCTION sd_misc_is_target_supp_mode_band_class

  Inform the caller to whether a specified combination of mode and band
  is supported by the current target. (2G/3G mode and band)

  For example, SD_SS_MODE_PREF_AMPS is only supported by a target that is
  AMPS capable.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  TRUE if the specified combination of mode and band is supported by the
  current target. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  boolean                   sd_misc_is_target_supp_mode_band_class(

        sd_ss_mode_pref_e_type    mode_pref,
            /* Mode preference to be validated.
            */

        sd_band_e_type            band
            /* Band to be validated. (2G/3G band)
            */
)
{
  return ( sd_misc_is_target_supp_mode_band(
                                       mode_pref,
                                       sd_map_band_to_band_pref(band),
                                       SYS_LTE_BAND_MASK_CONST_NONE,
                                       SD_SS_BAND_PREF_NONE) );
}



#if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)
/*===========================================================================

FUNCTION sd_misc_update_card_imsi


DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
 

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                   sd_misc_update_card_imsi(

	const  byte    *card_imsi_val,

    uint32   data_size

     
)
{

    sdss_update_card_imsi(card_imsi_val,data_size);


  
}
#endif
/* <EJECT> */
/*===========================================================================

FUNCTION sd_misc_is_target_supp_mode_band_chan

  Inform the caller to whether a specified combination of mode , band class
  and channel number is supported by the current target.
  Only used for 3gpp2 systems where channel number is of relevance.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  TRUE if the specified combination of mode, band class and channel number is
  supported by the current target. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  boolean  sd_misc_is_target_supp_mode_band_chan(

        sd_mode_e_type            mode,
            /* Mode to be validated.
            */

        sd_band_e_type            band,
            /* 2G 3G Bands to be validated.
            */

        sys_channel_num_type      chan
            /* Channel number to be validated.
            */
)
{
  return sdss_is_supp_sys(mode,sd_map_band_to_band_pref(band),chan);
}


/* <EJECT> */
/*===========================================================================

FUNCTION sd_misc_oprt_mode_online

  Updates the oprt mode for a given stack to ONLINE

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN void sd_misc_oprt_mode_online(

        sd_ss_e_type             ss
            /* ss to be updated
            */
)
{
  if(ss == SD_SS_HYBR_1)
  {
    /* This is needed to HDR_USER_HYBR_START for first HYBR_HDR kick */
    SD_CRIT_SECT_ENTER();
    sd_ss_ptr(ss)->is_start_instance = TRUE;
    SD_CRIT_SECT_EXIT();
  }

  /* Call on the SS-Core to update its operating mode.
  */
  sdss_opr_mode_update( ss,SDSS_OPR_MODE_ONLINE );
}

/*===========================================================================

FUNCTION sd_misc_oprt_mode_pwr_down

  Updates the oprt mode for a given stack to LPM/OFFLINE

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN void sd_misc_oprt_mode_pwr_down(

        sd_ss_e_type             ss
            /* ss to be updated
            */
)
{
  /* Call on the SS-Core to update its operating mode.
  */
  sdss_opr_mode_update( ss, SDSS_OPR_MODE_PWR_DOWN );
}

/* <EJECT> */
/*===========================================================================

FUNCTION sd_misc_is_supp_mode_band_chan

  Inform the caller to whether a specified combination of mode , band class
  and channel number is supported by the current UE configuration and Target configuration.
  Only used for 3gpp2 systems where channel number is of relevance.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  TRUE if the specified combination of mode, band class and channel number is
  supported by the current target. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  boolean  sd_misc_is_supp_mode_band_chan(

        sd_mode_e_type            mode,
            /* Mode to be validated.
            */

        sd_band_e_type            band,
            /* 2G 3G Bands to be validated.
            */

        sys_channel_num_type      chan
            /* Channel number to be validated.
            */
)
{

  /* Check whether the band supplied by the ss-client is upported by UE or not
   ** followed by target support.
   */
  
  if (sdss_is_ue_supp_band_pref (sd_map_band_to_band_pref(band)))
  {
    /*Check whether target support's the supplied mode,band,chan combination */
    return sdss_is_supp_sys(mode,sd_map_band_to_band_pref(band),chan);
  }

  return FALSE;
}

/*===========================================================================

FUNCTION sd_misc_get_supp_band_pref_lte

Returns band preferance bitmask, which is an intersection of UE & Target's
band preferance.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
Returns band preferance bitmask, which is a an intersection of UE & Target's
band preferance.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sys_lte_band_mask_e_type  sd_misc_get_supp_band_pref_lte()
{
  sd_ss_e_type ss = sdss_get_ss_for_mode( SD_MODE_LTE );

  SD_MSG_HIGH_1("sd_misc_get_supp_band_pref_lte(): LTE ss = %d",ss);

  if( ss == SD_SS_MAX )
  {
    return SYS_LTE_BAND_MASK_CONST_NONE;
  }

  return sdss_get_curr_lte_band_pref( ss );
}

/* <EJECT> */
/*===========================================================================

FUNCTION sd_misc_is_supp_pref

  Inform the caller to whether a specified combination of mode and band
  preference is supported by the current target, as well as, the PRL that is
  associated with the currently selected NAM.

  For example, SD_SS_MODE_PREF_AMPS is only supported by a target that is
  AMPS capable and a PRL that contains AMPS acquisition records.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.
  sd_nam_sel() must already been called to load the PRL.

RETURN VALUE
  TRUE if the specified combination of mode and band preference is supported
  by the current target, as well as, the PRL that is associated with the
  currently selected NAM. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  boolean                   sd_misc_is_supp_pref(

        sd_ss_mode_pref_e_type    mode_pref,
            /* Mode preference to be validated.
            */

        sd_ss_band_pref_e_type    band_pref
            /* Band preference to be validated.
            */
)
{
  boolean  target_support = TRUE, prl_support = TRUE;
  
  /* If the specified mode and band preference combination is NOT supported
  ** by the current target, return FALSE.
  */
  if( ! sdss_is_supp_mode_band_pref(mode_pref, band_pref) )
  {
    target_support = FALSE; 
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the specified mode and band preference combination is NOT supported
  ** by the currently loaded PRL, return FALSE.
  */
  else if( ! sdprl_is_supp_mode_band_pref(mode_pref,
                                     band_pref,
                                     SYS_LTE_BAND_MASK_CONST_ANY,
                                     SD_SS_BAND_PREF_TDS_ANY) )
  {
    prl_support = FALSE; 
  }

  if(!(target_support && prl_support))
  {
    SD_MSG_HIGH_5("is_supp_pref:Return as mode=0x%x band=0x%08x 0x%08x target_support:%d, prl_support=%d",
                   mode_pref,
                   QWORD_HIGH(band_pref),
                 QWORD_LOW(band_pref),
                 target_support,
                 prl_support);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the specified mode and band preference combination is
  ** supported - return TRUE.
  */
  return (target_support && prl_support);

} /* sd_misc_is_supp_pref */




/* <EJECT> */
/*===========================================================================

FUNCTION sd_misc_is_supp_pref_lte

  Inform the caller to whether a specified combination of mode and band
  preference is supported by the current target.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.
  sd_nam_sel() must already been called to load the PRL.

RETURN VALUE
  TRUE if the specified combination of mode and band preference is supported
  by the current target. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  boolean                   sd_misc_is_supp_pref_lte(

        sd_ss_mode_pref_e_type    mode_pref,
            /* Mode preference to be validated.
            */

        sys_lte_band_mask_e_type  lte_band_pref
            /* LTE band preference to be validated.
            */
)
{
  /* If the specified mode and band preference combination is NOT supported
  ** by the current target, return FALSE.
  */
  if( ! sdss_is_supp_mode_band_pref_lte(mode_pref, lte_band_pref) )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the specified mode and band preference combination is
  ** supported - return TRUE.
  */
  return TRUE;

} /* sd_misc_is_supp_pref_lte */

/* <EJECT> */
/*===========================================================================

FUNCTION sd_misc_is_supp_pref_tds

  Inform the caller to whether a specified combination of mode and band
  preference is supported by the current target.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.
  sd_nam_sel() must already been called to load the PRL.

RETURN VALUE
  TRUE if the specified combination of mode and band preference is supported
  by the current target. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  boolean                   sd_misc_is_supp_pref_tds(

        sd_ss_mode_pref_e_type    mode_pref,
            /* Mode preference to be validated.
            */

        sd_ss_band_pref_e_type    tds_band_pref
            /* TD-SCDMA band preference to be validated.
            */
)
{
  /* Returns true if the specified mode and band preference combination is
  ** supported by the current target, otherwise returns FALSE.
  */
  return sdss_is_supp_mode_band_pref_tds(mode_pref, tds_band_pref);

} /* sd_misc_is_supp_pref_tds */


/* <EJECT> */
/*===========================================================================

FUNCTION sd_misc_is_cdma_locked

DESCRIPTION
  Return the value of the CDMA locked indicator.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the CDMA locked indicator is set to TRUE. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  boolean                   sd_misc_is_cdma_locked( void )
{
  sd_ss_e_type                    ss = SD_SS_MAIN;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return sdss_is_cdma_locked(ss);

} /* sd_misc_is_cdma_locked */


/* <EJECT> */
/*===========================================================================

FUNCTION sd_misc_is_cdma_pseudo_online

  Inform the caller if CDMA is in pseudo online state

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  TRUE if CDMA was activated with pseudo online. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  boolean                   sd_misc_is_cdma_pseudo_online( void )
{
  sd_ss_e_type                    ss = SD_SS_MAIN;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return( sdss_get_pref_reason(ss) == SD_SS_PREF_REAS_PSEUDO_ONLINE );
}



/* <EJECT> */
/*===========================================================================

FUNCTION sd_misc_get_cdma_request_mode

DESCRIPTION
  Get the CDMA request_mode to be used in CDMA origination and page-response
  messages.

  The CDMA request mode is determined according to the current system
  selection preferences.

DEPENDENCIES
  None.

RETURN VALUE
  CDMA request_mode as defined in ANSI J-STD-008 Table 2.7.1.3.2.4-1 and
  cai.h.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  byte                      sd_misc_get_cdma_request_mode( void )
{
  return sdss_get_cdma_request_mode();
}

/* <EJECT> */
/*===========================================================================

FUNCTION sd_misc_get_cdma_return_cause

DESCRIPTION
  Get the CDMA return_cause to be used in CDMA registration and origination
  messages.

DEPENDENCIES
  None.

RETURN VALUE
  CDMA return_cause as defined in IS-2000-5 Table 2.7.1.3.2.1-2 and cai.h.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  byte                      sd_misc_get_cdma_return_cause( void )
{
  return sd_ss_ptr(SD_SS_MAIN)->return_cause;
}





/*===========================================================================

@FUNCTION sd_misc_mru_write

@DESCRIPTION
   Updates the MRU in the local RAM to NV\EFS if the MRU contents have been
   updated.

@DEPENDENCIES
  None.

@RETURN VALUE
  None.

@SIDE EFFECTS
  None.

===========================================================================*/


EXTERN  void                           sd_misc_mru_write( void )
{
  (void) sd_ss_generic_cmd((execute_fn) sdcmd_misc_mru_write, NULL);
}


/* ^L<EJECT> */
/*===========================================================================

FUNCTION sd_misc_get_curr_acq_sys_mode

DESCRIPTION
   Returns the mode of system over which service is currently provided. If
   no service is currently provided, return the mode of the system that
   is being attempted acquisition.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sys_sys_mode_e_type            sd_misc_get_curr_acq_sys_mode
(
  void
)
{
  return sd_misc_get_curr_acq_sys_mode_ss(SD_SS_MAIN);
}


/*===========================================================================

FUNCTION sd_misc_get_curr_sys_mode

DESCRIPTION
   If is_acq_sys_needed is SD_WITH_ACQ_MODE:
   1. If Main in service, return acquired mode.
   2. Else if Main is not in power save,  return the mode under acquiring
   3.  Else if Main is in Power save, Hybrid stack is in srv, return the acquired mode on hybrid
   4.  Else if Hybrid stack is not power save, return the mode under acquiring
   5.  Else return NO_SRV

   else if is_acq_sys_needed is SD_WITHOUT_ACQ_MODE:
   1. If Main in service, return acquired mode.
   2.  otherwise return NO_SRV

DEPENDENCIES
  None

RETURN VALUE
  Return the corresponding system mode.

SIDE EFFECTS
  None.

===========================================================================*/


EXTERN  sys_sys_mode_e_type       sd_misc_get_curr_sys_mode(

        sd_acq_mode_select_e_type is_acq_sys_needed
        /* Indicate whether ACQ sys mode needs to be returned when Main is not in service
        */
)
{
  sd_ss_act_e_type              act;
  sd_misc_get_curr_sys_mode_cmd *cmd;

  cmd = (sd_misc_get_curr_sys_mode_cmd *) sdtask_sd_cmd_new(
                                              sizeof(sd_misc_get_curr_sys_mode_cmd),
                                              (execute_fn) sdcmd_misc_get_curr_sys_mode,
                                              NULL,
                                              NULL
                                              );

  cmd->is_acq_sys_needed = is_acq_sys_needed;

  act = sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd);
  ref_cnt_obj_release(cmd);

  return act;
}

/*===========================================================================

FUNCTION sd_misc_get_rat_priority_list

DESCRIPTION
  Obtain the rat priority list used in SD.
  This API is used by 1xcp / DO cp for reselection to LTE.
  Provide the rat priority list that contains LTE.

  If LTE is on HYBR2 stack, return the RAT priority list on HYBR2
  Else return the RAT priority list on MAIN

DEPENDENCIES
  None

RETURN VALUE
  Return the current rat priority list that contains LTE.
  If neither MAIN/HYBR2 has LTE, return the  rat priority list on MAIN

SIDE EFFECTS
  None.

===========================================================================*/
sys_rat_pri_list_info_s_type   sd_misc_get_rat_priority_list( )
{
  sys_rat_pri_list_info_s_type    ss_rat_pri_list_info;

  if(sdss_is_1x_sxlte())
  {
    sdss_get_rat_pri_list_info( SD_SS_HYBR_2, &ss_rat_pri_list_info);

    if( sdss_find_rat_pri_sys_idx(&ss_rat_pri_list_info, SD_MODE_LTE) !=
                                                ss_rat_pri_list_info.num_items )
    {
      return ss_rat_pri_list_info;
    }
  }

  sdss_get_rat_pri_list_info( SD_SS_MAIN, &ss_rat_pri_list_info);
  return ss_rat_pri_list_info;
}

/*===========================================================================

FUNCTION sd_misc_get_main_rat_priority_list

DESCRIPTION
  Obtain the rat priority list from main stack used in SD

DEPENDENCIES
  None

RETURN VALUE
  Return the current rat priority list

SIDE EFFECTS
  None.

===========================================================================*/
sys_rat_pri_list_info_s_type  sd_misc_get_main_rat_priority_list(  )
{
  sys_rat_pri_list_info_s_type    ss_rat_pri_list_info;

  SD_MSG_HIGH_0("Get main rat priority list");

  sdss_get_rat_pri_list_info( SD_SS_MAIN, &ss_rat_pri_list_info);
  return ss_rat_pri_list_info;
}

/*===========================================================================

FUNCTION sd_misc_get_acq_sys_mode

DESCRIPTION
  Get the acq sys mode based on input ss

DEPENDENCIES
  None

RETURN VALUE
  Return the corresponding system mode.

SIDE EFFECTS
  None.

===========================================================================*/
// TODO: Verify that this does not need to be executed as a command.
EXTERN  sys_sys_mode_e_type       sd_misc_get_acq_sys_mode(

        sd_ss_e_type              ss
           /* the ss to use
           */
)
{
  sd_si_info_s_type   *si_info_ptr = &sd_si_ptr(ss)->si_info;
  sdsr_s_type        *sr_ptr;
  sd_mode_e_type      acq_sys_mode = SD_MODE_INACT;

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );

  if(sys_srv_status_is_srv(si_info_ptr->srv_status))
  {
    acq_sys_mode = si_info_ptr->mode;
  }
  else if(si_info_ptr->srv_status == SYS_SRV_STATUS_NO_SRV)
  {

    sr_ptr = sdsr_list_get_ptr( sdss_sr_list_map2(ss,SDSR_ACQ_SYS), 0 );
    if ( sr_ptr != NULL )
    {
      acq_sys_mode = sr_ptr->sys.mode;

    }
  }
  else if(si_info_ptr->srv_status == SYS_SRV_STATUS_PWR_SAVE)
  {
    acq_sys_mode = SD_MODE_INACT;
  }

  return sd_misc_map_sd_mode_to_sys_mode(acq_sys_mode);

}

/*===========================================================================

FUNCTION sd_misc_is_sys_same

DESCRIPTION

Checks if both the sys are same

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if sys_2 is equal to sys_1
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  boolean                   sd_misc_is_sys_same(

        sd_sys_s_type    sys_1,
            /* Specified sys_1
            */

        sd_sys_s_type    sys_2
            /* Specified sys_2
            */
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/




  if( sys_1.mode == sys_2.mode &&
      sys_1.band == sys_2.band &&
      sys_1.chan == sys_2.chan )
  {
    return TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return FALSE;

}

/* ^L<EJECT> */
/*===========================================================================

FUNCTION sd_misc_is_mode_pref

DESCRIPTION
  Check whether a specified mode_pref2 complies with a specified mode_pref1.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if mode_pref2 complies with mode_pref1
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  boolean                   sd_misc_is_mode_pref(

        sd_ss_mode_pref_e_type    mode_pref1,
            /* Specified mode_Pref1
            */

        sd_ss_mode_pref_e_type    mode_pref2
            /* Mode preference against which to check the compliance.
            */
)
{

  SD_ASSERT_ENUM_IS_INRANGE( mode_pref1, SD_SS_MODE_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( mode_pref2, SD_SS_MODE_PREF_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( (int)mode_pref1 & (int)mode_pref2 )
  {
    return TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return FALSE;

} /* sd_misc_is_mode_pref */


/* ^L<EJECT> */
/*===========================================================================

FUNCTION sd_misc_remove_mode_pref

DESCRIPTION
  Remove mode_pref2 from mode_pref1.

DEPENDENCIES
  None.

RETURN VALUE
  Resultant mode_pref.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sd_ss_mode_pref_e_type   sd_misc_remove_mode_pref(

        sd_ss_mode_pref_e_type    mode_pref1,
            /* Specified mode_Pref1.
            */

        sd_ss_mode_pref_e_type    mode_pref2
            /* Specified mode_pref2.
            */
)
{
  word bitmask_to_remove = (word) mode_pref2;

  SD_ASSERT_ENUM_IS_INRANGE( mode_pref1, SD_SS_MODE_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( mode_pref2, SD_SS_MODE_PREF_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* To remove LTE, should also remove GWL. Needed for:
  ** SD_SS_MODE_PREF_ANY_BUT_HDR - LTE = SD_SS_MODE_PREF_ANY_BUT_HDR_LTE */
  if(mode_pref2 == SD_SS_MODE_PREF_LTE)
  {
    bitmask_to_remove |= BM( SD_MODE_GWL );
  }

  return ( SD_MODE_DELETE(mode_pref1, bitmask_to_remove) );

} /* sd_misc_remove_mode_pref() */


/* ^L<EJECT> */
/*===========================================================================

FUNCTION sd_misc_add_mode_pref

DESCRIPTION
  Add mode_pref2 with mode_pref1.

DEPENDENCIES
  None.

RETURN VALUE
  Resultant mode_pref.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sd_ss_mode_pref_e_type   sd_misc_add_mode_pref(

        sd_ss_mode_pref_e_type    mode_pref1,
            /* Specified mode_Pref1.
            */

        sd_ss_mode_pref_e_type    mode_pref2
            /* Specified mode_pref2.
            */
)
{
  SD_ASSERT_ENUM_IS_INRANGE( mode_pref1, SD_SS_MODE_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( mode_pref2, SD_SS_MODE_PREF_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return ( SD_MODE_ADD( mode_pref1, mode_pref2) );

} /* sd_misc_add_mode_pref */


/* ^L<EJECT> */
#if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) || defined( FEATURE_SD_LTE )
/*===========================================================================

FUNCTION sd_misc_update_acq_status

DESCRIPTION
Set the acq_status in SD core.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void   sd_misc_update_acq_status(

        sd_ss_e_type              ss,
        /* SS on which acq_status to update
        */

        sys_acq_status_e_type       acq_status
        /*acq_status
        */

)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
   sdss_update_acq_status(ss, acq_status);

} /* sd_misc_add_mode_pref */
#endif


/* <EJECT> */
/*===========================================================================

FUNCTION sd_misc_get_common_mode_pref

DESCRIPTION
  Gets the common mode preference from mode_pref1 and mode_pref2.

DEPENDENCIES
  None.

RETURN VALUE
  Resultant mode_pref.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sd_ss_mode_pref_e_type   sd_misc_get_common_mode_pref(

        sd_ss_mode_pref_e_type    mode_pref1,
             /* Specified mode_Pref1.
             */

        sd_ss_mode_pref_e_type    mode_pref2
             /* Specified mode_pref2.
             */
)
{
  SD_ASSERT_ENUM_IS_INRANGE( mode_pref1, SD_SS_MODE_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( mode_pref2, SD_SS_MODE_PREF_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return ( SD_GET_COMMON_MODE( mode_pref1, mode_pref2) );

} /* sd_misc_get_common_mode_pref */
/*===========================================================================

FUNCTION sd_ss_get_operator_name

DESCRIPTION
  Get Operator Name for QRD Features

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN          int sd_ss_get_operator_name(
                sys_modem_as_id_e_type asubs_id
)
{
   return (int)sdss_get_operator_name(asubs_id);
}

/*===========================================================================

FUNCTION sd_misc_mru_update

DESCRIPTION
Invoke sdprl_update to write the sytem record in to MRU buffer.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN boolean   sd_misc_mru_update(

        sd_mmode_mru_table_entry_u_type *mru_table ,
        /* MRU object whihc has the system record of any system mode needs to be write in to MRU buffer .
             */

        uint8 index ,
        /* Need to be update the indexth record of MRU buffer with system record  .
             */

        sys_sys_mode_e_type       mode,
        /*System Mode
             */

        sd_mru_update_e_type  mru_update_e_type
        /*Type of enum which tells either update the MRU buffer or clear the MRU buffer */

)
{

      return (sdprl_update(mru_table,index,mode,mru_update_e_type));


}

/*===========================================================================

FUNCTION sd_misc_read_mru

DESCRIPTION
 Invoke  sdprl_read_mru with index & mru_table object to read the indexth record from the MRU buffer and copied in to insance.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

EXTERN void   sd_misc_read_mru(

        sd_mmode_mru_table_entry_u_type *mru_table,
        /* MRU object whihc needs to fill with appropriate indexth record .
             */

        uint8                    index
        /* index will give that  which record needs to be read from the MRU buffer .
            */
)
{

       sdprl_read_mru(mru_table,index);
     return;

}




/* ^L<EJECT> */
/*===========================================================================

FUNCTION sd_misc_is_hybr_opr

DESCRIPTION
  Function to query if we are in hybrid operation, based on SS-HDR instance.

DEPENDENCIES
  None.

RETURN VALUE
  boolean
    TRUE - hybrid operation on,
    FALSE - otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  boolean                  sd_misc_is_hybr_opr(

        void
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #if defined FEATURE_HDR_HYBRID
  return !sdss_is_hybr_pref( SD_SS_MAIN, SD_SS_HYBR_PREF_NONE );
  #else
  return FALSE;
  #endif

} /* sd_misc_is_hybr_opr() */


/* ^L<EJECT> */
/*===========================================================================

FUNCTION sd_misc_is_handoff

DESCRIPTION
  Function to query if the AT is in Handoff.

DEPENDENCIES
  None.

RETURN VALUE
  boolean
    TRUE - hybrid operation on,
    FALSE - otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  boolean                  sd_misc_is_handoff(

        sd_ss_e_type             ss,

        sd_ss_ho_e_type          ho_type
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  SD_ASSERT_ENUM_IS_INRANGE(ss, SD_SS_MAX);
  return ( sdss_misc_is_handoff( ss, ho_type));

} /* sd_misc_is_hybr_opr() */

/*===========================================================================

FUNCTION sd_misc_is_opt_ho_lte_do

DESCRIPTION
  Function to query if optimized ho LTE->ehrpd is in progress.

DEPENDENCIES
  None.

RETURN VALUE
  boolean
    TRUE - if opt_ho_lte_do is in progress
    FALSE - otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  boolean                  sd_misc_is_opt_ho_lte_do(void)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return ( sdss_is_opt_ho_lte_do());

} /* sd_misc_is_hybr_opr() */

/*===========================================================================

FUNCTION sd_misc_get_ss_sys_lost

DESCRIPTION
  Function to get the sys_lost value , internally used by CM
  Only

DEPENDENCIES
  None.

RETURN VALUE
 ss for the mode
 SD_SS_MAX if invalid call

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sd_ss_sys_lost_e_type sd_misc_get_ss_sys_lost(
	sd_ss_e_type			  ss

       )
{
  return sdss_get_ss_sys_lost( ss );
}

/*===========================================================================

FUNCTION sd_misc_get_ss_for_mode

DESCRIPTION
  Function to get the SS for the mode, to be used internally by CM and MMOC
  Only

DEPENDENCIES
  None.

RETURN VALUE
 ss for the mode
 SD_SS_MAX if invalid call

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sd_ss_e_type             sd_misc_get_ss_for_mode(

        sys_sys_mode_e_type      sys_mode
       /*lint -esym(715, sys_mode) *//* param not referenced*/)
{
  return sdss_get_ss_for_mode( sys_mode );
}
/*lint +esym(715, sys_mode) */

/*===========================================================================

FUNCTION sd_get_scan_scope_rule

DESCRIPTION
  Get scan scope rule to check ACQ DB Features enabled

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern          sys_scan_scope_rule_e_type sd_get_scan_scope_rule(void)
{

  return (sys_scan_scope_rule_e_type)sdss_get_scan_scope_rule();
}

/* <EJECT> */
/*===========================================================================

FUNCTION sd_misc_filter_out_overlap_band

DESCRIPTION
  Get band preference complies to MS radio access capability.Clear PGSM band
  pref if EGSM band pref is present. Clear both EGSM and PGSM band pref if
  RGSM band pref is present.

  SEMANTIC RULE : Among the three Access Type Technologies GSM 900-P,
  GSM 900-E and GSM 900-R only one shall be  present.

  Refer to 3GPP TS 05.05 Frequency in MHz
             Mobile Tx     Base Station Tx
  PGSM       890-915       935-960
  EGSM       880-915       925-960
  RGSM       876-915       921-960

DEPENDENCIES
  None

RETURN VALUE
  band preference enum type

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sd_ss_band_pref_e_type    sd_misc_filter_out_overlap_band(

        sd_ss_band_pref_e_type    band_pref
            /* Enumeration of system selection band and sub-band preferences.*/
)
{
  return (sd_filter_out_overlap_band(band_pref));
} /* sd_misc_filter_out_overlap_band */


/*===========================================================================

DESCRIPTION
  Return the avoidance list updated flag 

DEPENDENCIES
  None.

RETURN VALUE
  boolean  is_avoid_lst_updated

SIDE EFFECTS
  None.

*===========================================================================*/

EXTERN  boolean     sd_ss_get_avoid_list_updated_flag( void )
{
  return ( sdss_get_avoid_list_updated_flag());
}

/*===========================================================================

DESCRIPTION
  SETS  the avoidance list updated flag 

DEPENDENCIES
  None.

RETURN VALUE
  NONE

SIDE EFFECTS
  None.

*===========================================================================*/

EXTERN  void     sd_ss_set_avoid_list_updated_flag( boolean is_avoid_list_updated)
{

    sdss_set_avoid_list_updated_flag(is_avoid_list_updated);
}


/* <EJECT> */
/*===========================================================================

FUNCTION sd_misc_get_band_mode

DESCRIPTION
  Filter out the band and mode not supported by the target capability.
  Filter out the band and mode that are not in part of system preference.
  Filter out any overlapped bands.
  Bands that do not have corresponding modes.
  Modes that do not have corresponding bands.

  MODE
DEPENDENCIES
  None

RETURN VALUE
  Return the filtered mode and band preference as described.

SIDE EFFECTS
  None.

===========================================================================*/
// TODO: Verify that this does not need to be executed as a command.
EXTERN  void                      sd_misc_get_band_mode(

        sd_ss_mode_pref_e_type    *mode_ptr,
            /* Pointer to the mode preference that needs to be filtered.
            */

        sd_ss_band_pref_e_type    *band_ptr,
            /* Pointer to the band preference that needs to be filtered.
            */

        sd_ss_mode_pref_e_type    sys_mode_pref,
            /* Enumeration of system mode perference.
            */

        sd_ss_band_pref_e_type    sys_band_pref
            /* Enumeration of system selection band and sub-band preferences.
            */

)
{
  sd_get_band_mode(mode_ptr, band_ptr, sys_mode_pref, sys_band_pref);
} /* sd_misc_get_gw_band_mode */
/*===========================================================================

FUNCTION sd_misc_get_band_mode_mmode

DESCRIPTION
  Filter out the band and mode not supported by the target capability.
  Filter out the band and mode that are not in part of system preference.
  Filter out any overlapped bands.
  Bands that do not have corresponding modes.
  Modes that do not have corresponding bands.

  MODE
DEPENDENCIES
  None

RETURN VALUE
  Return the filtered mode and band preference as described.

SIDE EFFECTS
  None.

===========================================================================*/
// TODO: Verify that this does not need to be executed as a command.
EXTERN  void                      sd_misc_get_band_mode_mmode(

        sd_ss_mode_pref_e_type    *mode_ptr,
            /* Pointer to the mode preference that needs to be filtered.
            */

        sd_ss_band_pref_e_type    *band_ptr,
            /* Pointer to the non-LTE band preference that needs to be filtered.
            */

        sys_lte_band_mask_e_type  *band_ptr_lte,
            /* Pointer to the LTE band preference that needs to be filtered.
            */

        sd_ss_band_pref_e_type    *band_ptr_tds,
            /* Pointer to the TD-SCDMA band preference that needs to be filtered.
            */

        sd_ss_mode_pref_e_type    sys_mode_pref,
            /* Enumeration of system mode perference.
            */

        sd_ss_band_pref_e_type    sys_band_pref,
            /* Enumeration of non-LTE system selection band and sub-band preferences.
            */

        sys_lte_band_mask_e_type  sys_band_pref_lte,
            /* Enumeration of LTE system selection band and sub-band preferences.
            */

        sd_ss_band_pref_e_type    sys_band_pref_tds
            /* Enumeration of TD-SCDMA system selection band and sub-band preferences.
            */

)
{
  sd_get_band_mode_mmode(mode_ptr, band_ptr, band_ptr_lte, band_ptr_tds, sys_mode_pref, sys_band_pref,
      sys_band_pref_lte, sys_band_pref_tds);
}

/* ^L<EJECT> */
/*===========================================================================

FUNCTION sd_misc_get_mode_band_per_ss

DESCRIPTION
  Return mode_pref and band_pref value for passed ss.

DEPENDENCIES
  None

RETURN VALUE
  Return the filtered mode and band preference as described.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                      sd_misc_get_mode_band_per_ss(

		sd_ss_e_type				  ss,
        /* System selection stack
	       */

        sd_ss_mode_pref_e_type    *mode_ptr,
            /* Pointer to the mode preference that needs to be filtered.
            */

        sd_ss_band_pref_e_type    *band_ptr,
            /* Pointer to the non-LTE band preference that needs to be filtered.
            */

        sys_lte_band_mask_e_type  *band_ptr_lte,
            /* Pointer to the LTE band preference that needs to be filtered.
            */

        sd_ss_band_pref_e_type    *band_ptr_tds
            /* Pointer to the TD-SCDMA band preference that needs to be filtered.
            */
)
{
  sdss_get_mode_band_pref(ss, mode_ptr, band_ptr, band_ptr_lte, band_ptr_tds );
}

/* ^L<EJECT> */
/*===========================================================================

FUNCTION sd_misc_attach_complete_ind

DESCRIPTION
  Informs ATTACH COMPLETE IND to  SD. Currently SD doesn't ahve any specific dunctionality on this report.
  SD will route this report back to CM to searialise the SERVCIE_CNF & ATTACH_COMPLET_IND reports.

DEPENDENCIES
  None

RETURN VALUE
  Return the filtered mode and band preference as described.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                      sd_misc_attach_complete_ind(

        sd_ss_e_type                  ss
        /* System selection stack
           */
)
{
  sd_si_s_type        *si_ptr     = sd_si_ptr(ss);
  sd_rpt_generic_s_type  attach_complete_rpt;
  
  if(si_ptr == NULL)
  {
    return;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  attach_complete_rpt.type = SD_GENERIC_CB_ATTACH_COMPLETE_IND;
  attach_complete_rpt.data.attach_complete_rpt.ss = ss;

  /* reset ext_srv_info if it is pending on attach complete ack */
  sdss_reset_attach_pending(ss);

  if(si_ptr->sd_generic_cb_func != NULL)
  {
    /* Call to the sd_generic callback */
    SD_MSG_HIGH_0( "ATTACH/TAU COMPLETE_IND reporting to cm");
    si_ptr->sd_generic_cb_func(&attach_complete_rpt);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION sd_misc_get_supp_band_pref

DESCRIPTION

  Return the band preference that is based on the target capability and user
  preference.

  MODE
DEPENDENCIES
  None

RETURN VALUE
  Return the filtered band preference as described.

SIDE EFFECTS
  None.

===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
EXTERN  sd_ss_band_pref_e_type    sd_misc_get_supp_band_pref(void)
{
  sd_ss_band_pref_e_type     band_pref = SD_SS_BAND_PREF_NONE;
      /* The band preference that needs to be filtered.
      */

  sd_ss_mode_pref_e_type     mode_pref = SD_SS_MODE_PREF_NONE;

  sd_ss_mode_pref_e_type     ss_mode_pref_hybr = SD_SS_MODE_PREF_NONE;
  sd_ss_band_pref_e_type     ss_band_pref_main = SD_SS_BAND_PREF_NONE;

  if (sdtask_is_init_complete())
  {
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Fetch the preferences from main instance.
  */
  sdss_get_mode_band_pref( SD_SS_MAIN,
                           &mode_pref,
                           &ss_band_pref_main,
                           NULL,
                           NULL );

  if( !sdss_is_hybr_pref( SD_SS_MAIN, SD_SS_HYBR_PREF_NONE ))
  {
    sdss_get_mode_band_pref( SD_SS_HYBR_1,
                             &ss_mode_pref_hybr,
                             NULL,
                             NULL,
                             NULL );

    mode_pref = SD_MODE_ADD( mode_pref, ss_mode_pref_hybr );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Filter out bands that do not have corresponding modes and
  ** that are not supported by target.
  */
  band_pref = sdss_get_supp_mode_band_pref( mode_pref, ss_band_pref_main );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Filter out overlapping bands.
  */
  band_pref = sd_filter_out_overlap_band( band_pref );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  }/* else return SD_SS_BAND_PREF_NONE */
  else {
    SD_MSG_HIGH_0("SD critical section not init, SD task not up");
  }

  return band_pref;

} /* sd_misc_get_supp_band_pref */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif

/*===========================================================================

FUNCTION sd_misc_is_gwl_full_srv_allowed

DESCRIPTION
  Return a boolean indication of whether GWL full service acquisition is
  allowed or not

DEPENDENCIES
  None

RETURN VALUE
  TRUE: If the UE allows full service acquisition on GWL
  FALSE: IIf the UE does not allow full service acquisition on GWL

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  boolean    sd_misc_is_gwl_full_srv_allowed(void)
{
  sdss_subsc_e_type      subsc        = sdss_get_subsc_type(SD_SS_MAIN);
  sd_ss_orig_mode_e_type ss_orig_mode = sdss_get_orig_mode(SD_SS_MAIN);
  boolean                sim_domain_valid = sdss_gw_sim_domain_valid (SD_SS_MAIN);



  /* If there is no GWL subscription
  ** return false
  */
  if( !SD_IS_BIT_CONTAIN(subsc,SDSS_SUBSC_GW)
     || !sim_domain_valid
     || ss_orig_mode == SD_SS_ORIG_MODE_EMERG_ORIG 
     || ss_orig_mode == SD_SS_ORIG_MODE_PPCSFB_EMERG_ORIG
     ||ss_orig_mode == SD_SS_ORIG_MODE_EMERG_CB
    )
  {
    SD_MSG_HIGH_3("gwl_full_srv_allowed: FALSE: subsc:%d, sim_domain_valid:%d, \
               ss_orig_mode = %d",subsc,sim_domain_valid,ss_orig_mode);
    return FALSE;
  }
  /* If there are no issues, return TRUE
  */
  return TRUE;

} //sd_misc_is_gwl_full_srv_allowed


/*===========================================================================

FUNCTION sd_misc_is_lte_full_srv_allowed

DESCRIPTION
  Return a boolean indication of whether LTE full service acquisition is
  allowed or not

DEPENDENCIES
  None

RETURN VALUE
  TRUE: If the UE allows full service acquisition on LTE
  FALSE: If the UE does not allow full service acquisition on LTE

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  boolean    sd_misc_is_lte_full_srv_allowed(void)
{

  #ifdef FEATURE_SD_LTE
  sdss_subsc_e_type      subsc        = sdss_get_subsc_type(SD_SS_MAIN);
  sd_ss_orig_mode_e_type ss_orig_mode = sdss_get_orig_mode(SD_SS_MAIN);
  sd_ss_orig_mode_e_type ss2_orig_mode = sdss_get_orig_mode(SD_SS_HYBR_2);
  sys_sim_state_e_type   ss_sim_state = sdss_get_sim_state(SD_SS_MAIN);
  sys_rat_pri_list_info_s_type    ss_main_rat_pri_list_info;
  sys_rat_pri_list_info_s_type    ss_hybr2_rat_pri_list_info;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If there is no GWL subscription
  ** return false
  */
  if( !SD_IS_BIT_CONTAIN(subsc,SDSS_SUBSC_GW)||
      ss_sim_state == SYS_SIM_STATE_PS_INVALID    ||
      ss_sim_state == SYS_SIM_STATE_CS_PS_INVALID ||
      ss_sim_state == SYS_SIM_STATE_NOT_AVAILABLE ||
      ss_orig_mode == SD_SS_ORIG_MODE_EMERG_ORIG        ||
      ss_orig_mode == SD_SS_ORIG_MODE_PPCSFB_EMERG_ORIG ||
      ss_orig_mode == SD_SS_ORIG_MODE_EMERG_CB
    )
  {
    SD_MSG_HIGH_3("subsc:%d, sim_state:%d, ss_orig_mode:%d",subsc,
                           ss_sim_state,ss_orig_mode);
    return FALSE;
  }
  if( sdss_is_1x_sxlte() 
      &&
      (ss2_orig_mode == SD_SS_ORIG_MODE_EMERG_ORIG        ||
       ss2_orig_mode == SD_SS_ORIG_MODE_PPCSFB_EMERG_ORIG ||
       ss2_orig_mode == SD_SS_ORIG_MODE_EMERG_CB)
    )
  {
    SD_MSG_HIGH_0("Emerg call in progress on ss 2");
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If Rat priority list (mode/band/target cap) does not contain LTE , return FALSE
  */
  sdss_get_rat_pri_list_info( SD_SS_MAIN, &ss_main_rat_pri_list_info);
  
  // First check on MAIN stack if it has LTE. If not, then enter if condition.
  if( sdss_get_rat_pri_sys_idx(SD_SS_MAIN,SD_MODE_LTE) ==
                                        ss_main_rat_pri_list_info.num_items)
  {
  // For SXLTE, chek for HYBR2 as well i.e. if it has LTE
  if(sdss_is_1x_sxlte())
  {
    sdss_get_rat_pri_list_info( SD_SS_HYBR_2, &ss_hybr2_rat_pri_list_info);
      if( sdss_get_rat_pri_sys_idx(SD_SS_HYBR_2,SD_MODE_LTE) !=
                                        ss_hybr2_rat_pri_list_info.num_items )
    {
        /*SXLTE on and hybr2 RAT list has LTE*/
        return TRUE;
    }
  }
    else // It means that HYBR2 doesn't have LTE
  {
      /* Assigning MAX*/
      ss_hybr2_rat_pri_list_info.num_items = SYS_MAX_ACQ_PREF_RAT_LIST_NUM;
    }
    
    SD_MSG_HIGH_2("LTE not in rat_pri_list, main:%d, hybr2:%d",
                               ss_main_rat_pri_list_info.num_items,
                               ss_hybr2_rat_pri_list_info.num_items);
                               
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If there are no issues, return TRUE
  */
  return TRUE;

  #else

  return FALSE;

  #endif

} //sd_misc_is_lte_full_srv_allowed


/* <EJECT> */
/*===========================================================================

FUNCTION sd_misc_get_subs_supp_band_pref

DESCRIPTION

  Return the band preference that is based on the band pref of
  selected subscription id and user mode preference.

  MODE
DEPENDENCIES
  None

RETURN VALUE
  Return the filtered band preference as described.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sd_ss_band_pref_e_type    sd_misc_get_subs_supp_band_pref(

        sys_modem_as_id_e_type    as_id
)
{
  sd_ss_band_pref_e_type     band_pref         = SD_SS_BAND_PREF_NONE;
      /* The band preference that needs to be filtered.
      */

  sd_ss_e_type               ss                = SD_SS_MAIN;
  sd_ss_mode_pref_e_type     ss_mode_pref      = SD_SS_MODE_PREF_NONE;
  sd_ss_band_pref_e_type     ss_band_pref      = SD_SS_BAND_PREF_NONE;
  sd_ss_band_pref_e_type     ss_tds_band_pref  = SD_SS_BAND_PREF_NONE;

      /* Enumeration of system perference.
      */

  if (sdtask_is_init_complete())
  {
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Fetch the preferences from main instance.
    */
    if(as_id == SYS_MODEM_AS_ID_2)
    {
      if (sdss_is_sxlte())
      {
        ss = SD_SS_HYBR_3;
      }
      else
      {
        ss = SD_SS_HYBR_2;
      }
    }
  #if defined(FEATURE_MMODE_TRIPLE_SIM)
  else if(as_id == SYS_MODEM_AS_ID_3)
  {
    ss = SD_SS_HYBR_3;
  }
  #endif

    sdss_get_mode_band_pref( ss,
                             &ss_mode_pref,
                             &ss_band_pref,
                             NULL,
                             &ss_tds_band_pref );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Filter out bands that do not have corresponding modes and
    ** that are not supported by target.
  ** The Function sdss_get_supp_mode_band_pref_all is a wrapper function
  ** and returns the supp band prefs for TDS and LTE modes in addtion
  ** to the modes supported by sdss_get_supp_mode_band_pref
    */
    band_pref = sdss_get_supp_mode_band_pref( ss_mode_pref,
                                              ss_band_pref );

    //tds_band_pref = sdss_get_supp_band_pref_tds( ss_tds_band_pref );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Filter out overlapping bands.
    */
    band_pref = sd_filter_out_overlap_band( band_pref );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  } /* else return SD_SS_BAND_PREF_NONE */
  else
  {
    SD_MSG_HIGH_0("SD critical section not init, SD task not up");
  }

  return band_pref;

} /* sd_misc_get_supp_band_pref */


/* <EJECT> */
/*===========================================================================

FUNCTION sd_misc_sys_id_print

DESCRIPTION
  This function prints the sys_id

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
EXTERN  void                      sd_misc_sys_id_print(

  sys_sys_id_s_type     sys_id
      /* Sys Id
      */

)
{


  switch (sys_id.id_type)
    {
      case SYS_SYS_ID_TYPE_UMTS      :
        SD_MSG_MED_4("Lac: 0x%x.  Plmn:Identity [0] 0x%x [1] 0x%x [2] 0x%x", sys_id.id.plmn_lac.lac,
            sys_id.id.plmn_lac.plmn.identity[0],
            sys_id.id.plmn_lac.plmn.identity[1],
            sys_id.id.plmn_lac.plmn.identity[2]);

        break;

      case SYS_SYS_ID_TYPE_IS95      :
        SD_MSG_MED_2("Sid: %d Nid: %d", sys_id.id.is95.sid,
                                       sys_id.id.is95.nid);

        break;

      case SYS_SYS_ID_TYPE_IS856     :
        sdsr_hdr_sid_print( sys_id.id.is856);
        break;

       case SYS_SYS_ID_TYPE_UNDEFINED :
        break;

       default                        :
         break;
     } /* switch id_type */
  return;

} /* sd_misc_sys_id_print */




/*===========================================================================

FUNCTION sd_misc_is_del_tmsi

DESCRIPTION
  This function indicates that whether tmsi should be delete or not.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
EXTERN  boolean                      sd_misc_is_del_tmsi(void)
{


  sdsr_e_type                   list; /* reference list */
  int                           pos;  /* reference list position */
  sdss_acq_mode_e_type          ss_acq_mode;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* The list and position most recently attempted acquisition
  */
  sd_ss_read_acq_prms( SD_SS_MAIN,
                       &list,
                       &pos,
                       &ss_acq_mode );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return( sdsr_is_del_tmsi(list, pos) );

}/* sd_misc_is_del_tmsi */

#ifdef FEATURE_EOOS_UMTS
/*===========================================================================

FUNCTION sd_misc_get_eoos_param

DESCRIPTION
  Get the eoos scan parameters

DEPENDENCIES
  sd_init should be called before this function call

RETURN VALUE
  Returns the EOOS parameters

SIDE EFFECTS

===========================================================================*/
// TODO: Verify that this does not need to be executed as a command.
void                      sd_misc_get_eoos_param(

        dword                     *wcdma_scan_time,
            /* Pointer to return wcdma scan time in milliseconds
            */

        dword                     *gsm_scan_time,
            /* Pointer to return gsm scan time in milliseconds
            */

        dword                     *sleep_time
            /* Pointer to return sleep time in seconds
            */
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  dword    temp_wcdma_scan_time = 0;
  dword    temp_gsm_scan_time   = 0;
  dword    temp_sleep_time      = 0;
  boolean  is_gprs_anite_gcf    = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  SD_ASSERT( wcdma_scan_time != NULL );
  SD_ASSERT( gsm_scan_time != NULL );
  SD_ASSERT( sleep_time != NULL );

  sdss_compute_gwl_eoos_params(SD_SS_MAIN, &temp_wcdma_scan_time,
                       &temp_gsm_scan_time, NULL, &temp_sleep_time);

  /* Check if it's GCF testing */
  is_gprs_anite_gcf = sdss_is_gprs_anite_gcf(SD_SS_MAIN);

  *wcdma_scan_time = (is_gprs_anite_gcf ?
                        SD_GCF_EOOS_WCDMA_SCAN_TIME :
                        temp_wcdma_scan_time);

  *gsm_scan_time = (is_gprs_anite_gcf ?
                      SD_GCF_EOOS_GSM_SCAN_TIME :
                      temp_gsm_scan_time);


  #ifdef FEATURE_DISABLE_DEEP_SLEEP
  *sleep_time = 0;
  #else
  *sleep_time = ( is_gprs_anite_gcf ?
                     0 :
                     temp_sleep_time);
  #endif
  /*lint +e774 */
  return;
}
#endif

/* <EJECT> */
/*===========================================================================

FUNCTION sd_misc_is_orig_mode

DESCRIPTION
  Check whether a specified origination mode value includes the current
  origination mode.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the specified origination mode value includes the current
  origination mode. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  boolean                       sd_misc_is_orig_mode(

        sd_ss_e_type                  ss,
            /* System selection - MAIN or HDR.
            */

        sd_ss_orig_mode_val_e_type    orig_mode_val
            /* Origination mode value.
            */
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( orig_mode_val, SD_SS_ORIG_MODE_VAL_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return( sdss_is_orig_mode( ss, orig_mode_val ) );

} /* sdss_is_orig_mode */


/* <EJECT> */
/*===========================================================================

FUNCTION sd_misc_sys_avoid_list

DESCRIPTION
  Return a list of avoiding system (channels) for a mode preference

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to the avoided list and the number of avoided systems.

SIDE EFFECTS
  None.

===========================================================================*/



EXTERN  void                       sd_misc_sys_avoid_list(

        sd_mode_e_type               mode,
            /* Origination mode value.
            */

        sys_avoided_channel_record_type     *avoid_list_ptr,
            /* Pointer to avoided channels
            */

        byte                        *num_sys,

        byte                         max_entry
)
{
  sd_misc_sys_avoid_list_cmd  *cmd;

  cmd = (sd_misc_sys_avoid_list_cmd *) sdtask_sd_cmd_new(
                                              sizeof(sd_misc_sys_avoid_list_cmd),
                                              (execute_fn) sdcmd_misc_sys_avoid_list,
                                              NULL,
                                              NULL
                                              );

  cmd->mode = mode;
  cmd->avoid_list_ptr = avoid_list_ptr;
  cmd->num_sys = num_sys;
  cmd->max_entry = max_entry;

  (void) sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd);
  ref_cnt_obj_release(cmd);
}
 

#ifdef FEATURE_MMSS_3_1
/* <EJECT> */
/*===========================================================================

FUNCTION sd_misc_update_filedb

DESCRIPTION
  MMOC calls this API to update SD module with the files it has
  read for MMSS operation

DEPENDENCIES


RETURN VALUE


SIDE EFFECTS
  None.

===========================================================================*/
EXTERN void         sd_misc_update_filedb(

       mmgsdi_file_enum_type       file_name,
       /* The name of the file being updated
       */

       const byte                 *data_buf,
       /* The contents of the file
       */

       uint32                      data_size
       /* The size of the file
       */

)
{
  sdmmss_update_filedb(file_name, data_buf, data_size);
}

#endif // FEATURE_MMSS_3_1

 #if defined (FEATURE_LTE) && defined(FEATURE_LTE_TO_1X)
/*===========================================================================

FUNCTION sd_misc_allowed_sid_nid

DESCRIPTION
  Verify if the SID-NID is a negative system or unavailable in PRL

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if sid/nid is allowed, i.e. it is NOT a negative system as per PRL and it is available in PRL when pref_only flag is set
  FALSE if sid/nid is not allowed, i.e. it is either a negative system as per PRL or it is unavailable in PRL when pref_only is set

SIDE EFFECTS
  None.

===========================================================================*/



EXTERN  boolean        sd_misc_allowed_sid_nid (

        word                      sid,
            /* SID
            */

        word                      nid
            /* NID
            */

)
  {
  sd_ss_act_e_type            act;
  sd_misc_allowed_sid_nid_cmd *cmd;

  cmd = (sd_misc_allowed_sid_nid_cmd *) sdtask_sd_cmd_new(
                                              sizeof(sd_misc_allowed_sid_nid_cmd),
                                              (execute_fn) sdcmd_misc_allowed_sid_nid,
                                              NULL,
                                              NULL
                                              );

  cmd->sid = sid;
  cmd->nid = nid;

  act = sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd);
  ref_cnt_obj_release(cmd);

  return act;
}

#endif /* (FEATURE_LTE) && (FEATURE_LTE_TO_1X) */

/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_misc_update_session_info

DESCRIPTION
  MMOC calls this API to update SD module with the MMGSDI
  session information

DEPENDENCIES


RETURN VALUE


SIDE EFFECTS
  None.

===========================================================================*/


EXTERN void sd_ss_misc_update_session_info (

       mmgsdi_app_enum_type                  app_type,
       /* MMGSDI app type */

       sd_ss_e_type ss

)

{
  sdss_misc_update_session_info ( ss, app_type);
}


/*===========================================================================

FUNCTION sd_misc_get_act_net_sel

DESCRIPTION
  Return the  network_selection_mode type for the the most recently
  returned true_curr SS Action to the SS-Client.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.


RETURN VALUE
  sys_network_selection_mode_e_type - current Action's network selection mode
  If the act is not GW or GWL, it will return NONE

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN sys_network_selection_mode_e_type sd_misc_get_act_net_sel(
    sd_ss_e_type ss
)
{
  sd_ss_s_type                      *ss_ptr = NULL;
  sd_ss_act_e_type                   act;
  sys_network_selection_mode_e_type  net_sel_mode = SYS_NETWORK_SELECTION_MODE_NONE;

  SD_CRIT_SECT_ENTER();

  ss_ptr = sd_ss_ptr(ss);
  act = ss_ptr->true_curr_act.act;

  if(act == SD_SS_ACT_ACQ_GW || act == SD_SS_ACT_ACQ_GWL)
  {
    net_sel_mode =
      ss_ptr->true_curr_act.prm.acq_gwl.net_sel_type;
  }

  SD_CRIT_SECT_EXIT();
  SD_MSG_HIGH_2("last act = %d, net_sel_mode = %d",act, net_sel_mode);
  return net_sel_mode;
}

/* <EJECT> */
/*===========================================================================

FUNCTION sd_misc_get_prev_act_net_sel

DESCRIPTION
  Return the  network_selection_mode type for the previous action
  returned prev-act SS Action to the SS-Client.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.


RETURN VALUE
  sys_network_selection_mode_e_type - current Action's network selection mode
  If the act is not GW or GWL, it will return NONE

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN sys_network_selection_mode_e_type sd_misc_get_prev_act_net_sel(
    sd_ss_e_type ss
)
{
  sd_ss_s_type                      *ss_ptr = NULL;
  sd_ss_act_e_type                   act;
  sys_network_selection_mode_e_type  net_sel_mode = SYS_NETWORK_SELECTION_MODE_NONE;

  SD_CRIT_SECT_ENTER();

  ss_ptr = sd_ss_ptr(ss);
  act = ss_ptr->prev_act.act;

  if(act == SD_SS_ACT_ACQ_GW || act == SD_SS_ACT_ACQ_GWL)
  {
    net_sel_mode =
      ss_ptr->prev_act.prm.acq_gwl.net_sel_type;
  }

  SD_CRIT_SECT_EXIT();

  return net_sel_mode;
}

/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
================================= GENERAL ===================================
=============================================================================
=============================================================================
===========================================================================*/

/*===========================================================================

FUNCTION sdtask_crit_sect_ptr

DESCRIPTION

  Return pointer to SD critical section.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
rex_crit_sect_type* sd_crit_sect_ptr(void)
{
  static rex_crit_sect_type crit_sect;
    /* The one and only SD critical section */

  return &crit_sect;

}

/*===========================================================================

FUNCTION sdtask_init_crit_sect

DESCRIPTION
  Initialize SD critical section.
  This function must be called before any other part of SD is initialized

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void sd_init_crit_sect(void)
{
  rex_init_crit_sect(sd_crit_sect_ptr());
}

/*===========================================================================

FUNCTION sd_rex_enter_crit_sect

DESCRIPTION
  Enter SD critical section.
  This function must be called before any other part of SD is initialized

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void sd_rex_enter_crit_sect(void)
{

  rex_enter_crit_sect(sd_crit_sect_ptr());
}

/*===========================================================================

FUNCTION sd_rex_leave_crit_sect

DESCRIPTION
  Leave SD critical section.
  This function must be called before any other part of SD is initialized

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void sd_rex_leave_crit_sect(void)
{

  rex_leave_crit_sect(sd_crit_sect_ptr());
}

/*===========================================================================

FUNCTION sd_init_before_app_task_start

DESCRIPTION
  Perform SD initialization that needs to be done before application tasks
  have started and NV task has been started.

  NOTE! Any initialization that must be done before SD clients tries to
  contact SD, must be done in this function, as opposed to sd_init().

  NOTE! Any initialization that result in SD contacting some other tasks
  must be done in sd_init(), as opposed to this function.

DEPENDENCIES
  This function must be called before any other SD function is ever called.
  This function shall not be called a second time prior to calling on
  sd_term().

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                      sd_init_before_app_task_start( void )
{

  #ifdef SD_DEBUG
  #error code not present
#endif

  /* Initialize critical sections of SD */
  sd_init_crit_sect();
  sdsr_init_mutex();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Initialize EFS.
  */
  sdefs_init();
  sdssscr_efs_items_init();
  sdss_lbsr_mcc_items_init();
  sdss_1xcsfb_ecbm_init();
  sdss_1xcsfb_call_end_opt_init();
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the system record component.
  */
  sdsr_component_init();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the main service indicators component.
  */
  sd_si_init( SD_SS_MAIN, sd_ss_ptr(SD_SS_MAIN), SDSR_SI_SYS );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #if defined FEATURE_HDR_HYBRID
  /* Initialize the SS_HDR service indicators component.
  */
  sd_si_init( SD_SS_HYBR_HDR, sd_ss_ptr(SD_SS_HYBR_HDR), SDSR_SI_SYS_ );
  #endif /* FEATURE_HDR_HYBRID */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #if defined(FEATURE_MMODE_DUAL_SIM) || defined FEATURE_MMODE_SC_SVLTE || defined FEATURE_MMODE_SC_SGLTE
  /* Initialize the HYBR_2 service indicators component.
  */
  sd_si_init( SD_SS_HYBR_2, sd_ss_ptr(SD_SS_HYBR_2),
                                           sdss_sr_list_map2(SD_SS_HYBR_2, SDSR_SI_SYS) );
  #endif

  #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
  /* Initialize the HYBR_3 service indicators component.
  */
  sd_si_init( SD_SS_HYBR_3, sd_ss_ptr(SD_SS_HYBR_3),
                                           sdss_sr_list_map2(SD_SS_HYBR_3, SDSR_SI_SYS) );
  #endif
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the system selection core.
  */
  sdss_component_init();

  sdss_init_pwr_up(SD_SS_MAIN);
  sdss_init_pwr_up(SD_SS_HYBR_1);
  sdss_init_pwr_up(SD_SS_HYBR_2);
  sdss_init_pwr_up(SD_SS_HYBR_3);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the SDPRL before task start.
  */
  sdprl_component_init();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the PRL DIAG.
  */
  prldiag_init();

  #ifdef FEATURE_MMODE_SC_SVLTE
  /* Initialize LTEAvailFile/LTENotAvailFile */
  sdss_lte_avail_file_init();
  #endif

  #ifdef FEATURE_MMODE_DYNAMIC_SV_OPERATION_SWITCH
  /* Initialized dynamic switch */
  sdss_set_sv_dynamic_switch_enabled(FALSE);
  #endif

#if !defined(WIN32)
  sdtask_init_before_start();
#endif

  #ifdef SD_DEBUG
  #error code not present
#endif
} /* sd_init_before_app_task_start */




/* <EJECT> */
/*===========================================================================

FUNCTION sd_init

DESCRIPTION
  Perform SD initialization that needs to be done after tasks have started.

  NOTE! Any initialization that must be done before SD clients tries to
  contact SD, must be done in sd_init_before_task_start(), as opposed to
  this function.

  NOTE! Any initialization that result in SD contacting some other tasks,
  such as NV, must be done in this function, as opposed to
  sd_init_before_task_start()

  This function registers an SS-Timer callback function with SD. SD calls on
  the provided callback function to set/disable the SS-Timer. Upon SS-Timer
  expiration the SS-Client should call on the sd_ss_ind_misc_timer()
  SS-Indication function.

DEPENDENCIES
  This function must be called after sd_init_before_task_start() and
  before any other SD function is ever called. This function shall not be
  called a second time prior to calling on sd_term().

RETURN VALUE
  TRUE if SD initialization is successful (i.e. SD NV items are successfully
  read). FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  boolean                   sd_init(
            /*lint -esym(715, hdr_timer_func_ptr) *//* param not referenced*/
            /*lint -esym(715, wlan_timer_func_ptr) *//* param not referenced*/
        rex_sigs_type             nv_sig,
            /* Rex signal to be used by SD for waiting on NV to completes a
            ** read/write operation.
            */

        rex_sigs_type             dog_sig,
            /* Rex signal to be used by SD for watchdog reporting while SD
            ** waits on NV to complete a read/write operation.
            */

        void                      (*dog_rpt_func_ptr) (void),
            /* Pointer to a watchdog reporting function to be called by SD
            ** when the watchdog sig is set while SD waits on NV to complete
            ** a read/write operation.
            */

        void                      (*timer_func_ptr) (int4  ms),
            /* Pointer to an SS-Timer callback function.
            **
            ** SD calls on this function to set/disable the SS-Timer. Upon
            ** SS-Timer expiration the SS-Client should call on the
            ** sd_ss_ind_misc_timer() SS-Indication function.
            **
            ** The argument to this function is the time in ms with which
            ** to set the SS-Timer. An input of 0 should disable the
            ** SS-Timer */


        void                      (*hdr_timer_func_ptr) (int4  ms),
            /* Pointer to an HDR SS-Timer callback function for CDMA/HDR
            ** hybrid operation (equivalent to timer_func_ptr).
            **
            ** If target is not capable of CDMA/HDR hybrid operation, this
            ** parameter should be set to NULL */

        void                    (*hybr_2_timer_func_ptr)(int4 ms)
            /* Pointer to an SS-HYBR_2 SS-Timer callback function for
            ** hybrid operation (equivalent to timer_func_ptr).
            **
            ** If target is not capable of dual standby operation, this
            ** parameter can be set to NULL
            */
)
/*lint -esym(715,hybr_2_timer_func_ptr) */
{
	return sd_init2(nv_sig,
            dog_sig,
            dog_rpt_func_ptr,
            timer_func_ptr,
            hdr_timer_func_ptr,
            hybr_2_timer_func_ptr,
            NULL);

} /* sd_init */
/*lint +esym(715, hdr_timer_func_ptr) */    /* param not referenced*/
/*lint +esym(715, wlan_timer_func_ptr) */   /* param not referenced*/
/*lint +esym(715, hybr_2_timer_func_ptr) */ /* param not referenced*/


/* <EJECT> */
/*===========================================================================

FUNCTION sd_init2

DESCRIPTION
  Perform SD initialization that needs to be done after tasks have started.

  NOTE! Any initialization that must be done before SD clients tries to
  contact SD, must be done in sd_init_before_task_start(), as opposed to
  this function.

  NOTE! Any initialization that result in SD contacting some other tasks,
  such as NV, must be done in this function, as opposed to
  sd_init_before_task_start()

  This function registers an SS-Timer callback function with SD. SD calls on
  the provided callback function to set/disable the SS-Timer. Upon SS-Timer
  expiration the SS-Client should call on the sd_ss_ind_misc_timer()
  SS-Indication function.

DEPENDENCIES
  This function must be called after sd_init_before_task_start() and
  before any other SD function is ever called. This function shall not be
  called a second time prior to calling on sd_term().

RETURN VALUE
  TRUE if SD initialization is successful (i.e. SD NV items are successfully
  read). FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  boolean                   sd_init2(
            /*lint -esym(715, hdr_timer_func_ptr) *//* param not referenced*/
            /*lint -esym(715, wlan_timer_func_ptr) *//* param not referenced*/
        rex_sigs_type             nv_sig,
            /* Rex signal to be used by SD for waiting on NV to completes a
            ** read/write operation.
            */

        rex_sigs_type             dog_sig,
            /* Rex signal to be used by SD for watchdog reporting while SD
            ** waits on NV to complete a read/write operation.
            */

        void                      (*dog_rpt_func_ptr) (void),
            /* Pointer to a watchdog reporting function to be called by SD
            ** when the watchdog sig is set while SD waits on NV to complete
            ** a read/write operation.
            */

        void                      (*timer_func_ptr) (int4  ms),
            /* Pointer to an SS-Timer callback function.
            **
            ** SD calls on this function to set/disable the SS-Timer. Upon
            ** SS-Timer expiration the SS-Client should call on the
            ** sd_ss_ind_misc_timer() SS-Indication function.
            **
            ** The argument to this function is the time in ms with which
            ** to set the SS-Timer. An input of 0 should disable the
            ** SS-Timer */


        void                      (*hdr_timer_func_ptr) (int4  ms),
            /* Pointer to an HDR SS-Timer callback function for CDMA/HDR
            ** hybrid operation (equivalent to timer_func_ptr).
            **
            ** If target is not capable of CDMA/HDR hybrid operation, this
            ** parameter should be set to NULL */

        void                    (*hybr_2_timer_func_ptr)(int4 ms),
            /* Pointer to an SS-HYBR_2 SS-Timer callback function for
            ** hybrid operation (equivalent to timer_func_ptr).
            **
            ** If target is not capable of dual standby operation, this
            ** parameter can be set to NULL
            */

       void                    (*hybr_3_timer_func_ptr)(int4 ms)
           /* Pointer to an SS-HYBR_2 SS-Timer callback function for
           ** hybrid operation (equivalent to timer_func_ptr).
           **
           ** If target is not capable of dual standby operation, this
           ** parameter can be set to NULL
           */

)
/*lint -esym(715,hybr_2_timer_func_ptr) */
{
  SD_ASSERT( nv_sig != 0 );
  SD_ASSERT( dog_sig != 0 );
  SD_ASSERT( dog_rpt_func_ptr != NULL );
  SD_ASSERT( timer_func_ptr != NULL );

  

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* clear the sdprl active flag before the lists are cleared in
  ** sdsr_component_init()
  */
  sdprl_component_init();
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the system record component. We need to initialize here,
  ** as well, because when switching in and out of LPM/FTM
  ** sd_init_before_task_start() is not being called.
  */
  sdsr_component_init();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the system selection core.
  */
  sdss_component_init();

  /* Initialize the SD NV component.
  */
  sdnv_init( nv_sig, dog_sig, dog_rpt_func_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the system selection front end and core.
  */
  sd_ss_init( sd_ss_ptr(SD_SS_MAIN), timer_func_ptr );

  #if defined FEATURE_HDR_HYBRID
  SD_ASSERT( hdr_timer_func_ptr != NULL );
  sd_ss_init( sd_ss_ptr(SD_SS_HYBR_HDR), hdr_timer_func_ptr );
  #endif

  #if defined(FEATURE_MMODE_DUAL_SIM) || defined FEATURE_MMODE_SC_SVLTE || defined FEATURE_MMODE_SC_SGLTE
  if(hybr_2_timer_func_ptr != NULL)
  {
    sd_ss_init(sd_ss_ptr(SD_SS_HYBR_2), hybr_2_timer_func_ptr);
  }
  #endif

  #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
  if(hybr_3_timer_func_ptr != NULL)
  {
    sd_ss_init(sd_ss_ptr(SD_SS_HYBR_3), hybr_3_timer_func_ptr);
  }
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the MMSS component.
  */
  #ifdef FEATURE_MMSS_3_1
  sdmmss_init();
  #endif
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the PRL component.
  */
  if( ! sdprl_init() )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Read the configurable SD items from the NV.
  */
  sdssscr_nv_items_init();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the sdss NV items
  */
  sdss_nv_init( SD_SS_MAIN );

  #if defined (FEATURE_HDR_HYBRID)
  sdss_nv_init( SD_SS_HYBR_HDR );
  #endif

  #if defined(FEATURE_MMODE_DUAL_SIM) || defined FEATURE_MMODE_SC_SVLTE || defined FEATURE_MMODE_SC_SGLTE
  sdss_nv_init(SD_SS_HYBR_2);
  #endif

  #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)

  sdss_nv_init(SD_SS_HYBR_3);
  #endif

  #ifdef FEATURE_MMODE_SC_SVLTE
  /* Initialize LTEAvailFile/LTENotAvailFile */
  sdss_lte_avail_file_init();
  #endif

  /* Register for MCFG refresh callback
  */
  #ifdef FEATURE_MODEM_CONFIG_REFRESH
  sdtask_mcfg_refresh_register();
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, SD initialization is successful - return TRUE.
  */
  return TRUE;

} /* sd_init */
/*lint +esym(715, hdr_timer_func_ptr) */    /* param not referenced*/
/*lint +esym(715, wlan_timer_func_ptr) */   /* param not referenced*/
/*lint +esym(715, hybr_2_timer_func_ptr) */ /* param not referenced*/


/*===========================================================================

FUNCTION sd_nam_sel2

DESCRIPTION
  Inform SD of a new NAM selection, as well as, the selected NAM's
  SS-Preferences.

  NOTE! This function only returns after the PRL of the selected NAM is
  read from NV.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  TRUE if NAM change is successful (i.e. the PRL of the selected NAM is
  successfully extracted from NV). FALSE otherwise.

SIDE EFFECTS
  Note that MMOC uses sd_nam_sel3(). This function is kept for legacy reasons.

===========================================================================*/




/*===========================================================================

FUNCTION sd_nam_sel3

DESCRIPTION
  Inform SD of a new NAM selection, as well as, the selected NAM's
  SS-Preferences. Supports LTE.

  NOTE! This function only returns after the PRL of the selected NAM is
  read from NV.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  TRUE if NAM change is successful (i.e. the PRL of the selected NAM is
  successfully extracted from NV). FALSE otherwise.

SIDE EFFECTS
  None.

============================================================================*/

EXTERN  boolean                          sd_nam_sel3(

        sd_nam_e_type                     curr_nam,
            /* Current NAM selection (NAM-1, NAM-2, etc.).
            */

        sd_ss_orig_mode_e_type             orig_mode,
            /* The origination mode (normal, OTASP, emergency, other, etc.)
            ** that is associated with the selected NAM.
            */

        sd_ss_mode_pref_e_type             mode_pref,
            /* The mode preference that is associated with the selected NAM.
            */

        sd_ss_band_pref_e_type              band_pref,
            /* The 2G/3G band preference that is associated with the selected NAM.
            */

        sd_ss_band_pref_e_type              lte_band_pref,
            /* The LTE band preference that is associated with the selected NAM.
            */

        sd_ss_band_pref_e_type              tds_band_pref,
            /* The TD-SCDMA band preference that is associated with the selected NAM.
            */

        sd_ss_prl_pref_e_type               prl_pref,
            /* New prl system preference (Available BC0A, available BC0B, etc.)
            */

        sd_ss_roam_pref_e_type              roam_pref,
            /* The band preference that is associated with the selected NAM.
            */

        sd_ss_hybr_pref_e_type              hybr_pref,
            /* New hybrid preference (none, CDMA/HDR).
            */

        const sys_manual_sys_info_s_type    *manual_sys_info_ptr,
          /* Pointer to struct that defines the manual system.
           */

        sd_ss_srv_domain_pref_e_type        domain_pref,
           /* Service domain preference that is associated with the selected
           ** NAM.
           ** Applies only for GSM/WCDMA/LTE modes.
           */

        sd_ss_acq_order_pref_e_type         acq_order_pref,
           /* Preference for the order of acquisition ( WCDMA before GSM,
           ** GSM before WCDMA etc).
           */

        boolean                             is_gwl_subsc_avail,
           /* Indicates if subscription is available for providing service on
           ** GSM/UMTS network.
           */

        boolean                             is_1x_subsc_avail,
           /* Indicates if subscription is available for providing service on
           ** CDMA/AMPS/HDR network.
           */

        boolean                             is_gw2_subsc_avail
           /**< Indicates if subscription is available for providing service on
           ** GSM/UMTS network.
           */

)
{
  return sd_nam_sel4(curr_nam,
                     orig_mode,
                     mode_pref,
                     band_pref,
                     sys_map_band_mask_to_lte_band_mask(lte_band_pref),
                     tds_band_pref,
                     prl_pref,
                     roam_pref,
                     hybr_pref,
                     manual_sys_info_ptr,
                     domain_pref,
                     acq_order_pref,
                     is_gwl_subsc_avail,
                     is_1x_subsc_avail,
                     is_gw2_subsc_avail,
                     FALSE
                                              );


}

/*===========================================================================

FUNCTION sd_nam_sel4

DESCRIPTION
  Inform SD of a new NAM selection, as well as, the selected NAM's
  SS-Preferences. Supports LTE.

  NOTE! This function only returns after the PRL of the selected NAM is
  read from NV.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  TRUE if NAM change is successful (i.e. the PRL of the selected NAM is
  successfully extracted from NV). FALSE otherwise.

SIDE EFFECTS
  None.

============================================================================*/

EXTERN  boolean                          sd_nam_sel4(

        sd_nam_e_type                     curr_nam,
            /* Current NAM selection (NAM-1, NAM-2, etc.).
            */

        sd_ss_orig_mode_e_type             orig_mode,
            /* The origination mode (normal, OTASP, emergency, other, etc.)
            ** that is associated with the selected NAM.
            */

        sd_ss_mode_pref_e_type             mode_pref,
            /* The mode preference that is associated with the selected NAM.
            */

        sd_ss_band_pref_e_type              band_pref,
            /* The 2G/3G band preference that is associated with the selected NAM.
            */

        sys_lte_band_mask_e_type            lte_band_pref,
            /* The LTE band preference that is associated with the selected NAM.
            */

        sd_ss_band_pref_e_type              tds_band_pref,
            /* The TD-SCDMA band preference that is associated with the selected NAM.
            */

        sd_ss_prl_pref_e_type               prl_pref,
            /* New prl system preference (Available BC0A, available BC0B, etc.)
            */

        sd_ss_roam_pref_e_type              roam_pref,
            /* The band preference that is associated with the selected NAM.
            */

        sd_ss_hybr_pref_e_type              hybr_pref,
            /* New hybrid preference (none, CDMA/HDR).
            */

        const sys_manual_sys_info_s_type    *manual_sys_info_ptr,
          /* Pointer to struct that defines the manual system.
           */

        sd_ss_srv_domain_pref_e_type        domain_pref,
           /* Service domain preference that is associated with the selected
           ** NAM.
           ** Applies only for GSM/WCDMA/LTE modes.
           */

        sd_ss_acq_order_pref_e_type         acq_order_pref,
           /* Preference for the order of acquisition ( WCDMA before GSM,
           ** GSM before WCDMA etc).
           */

        boolean                             is_gwl_subsc_avail,
           /* Indicates if subscription is available for providing service on
           ** GSM/UMTS network.
           */

        boolean                             is_1x_subsc_avail,
           /* Indicates if subscription is available for providing service on
           ** CDMA/AMPS/HDR network.
           */

        boolean                             is_gw2_subsc_avail,
           /**< Indicates if subscription is available for providing service on
           ** GSM/UMTS network.
           */

        boolean                             is_gw3_subsc_avail
           /**< Indicates if subscription is available for providing service on
           ** GSM/UMTS network.
           */

)
{
  sd_ss_act_e_type  act;
  sd_nam_sel_cmd    *cmd;

  cmd = (sd_nam_sel_cmd *) sdtask_sd_cmd_new(
                                              sizeof(sd_nam_sel_cmd),
                                              (execute_fn) sdcmd_nam_sel4,
                                              NULL,
                                              NULL
                                              );

  cmd->curr_nam = curr_nam;
  /* Manual mode is supported only for 3GPP RATs (T,G,W,L) 
  *  update orig_mode if mode pref does not contain 3GPP RATs 
  */
  cmd->orig_mode = sdss_compute_orig_mode(orig_mode, \
    sdss_get_supp_mode_pref_mmode(mode_pref, band_pref, lte_band_pref, tds_band_pref), \
    &is_1x_subsc_avail);
  
  cmd->mode_pref = mode_pref;
  cmd->band_pref = band_pref;
  cmd->lte_band_pref = lte_band_pref;
  cmd->tds_band_pref = tds_band_pref;
  cmd->prl_pref = prl_pref;
  cmd->roam_pref = roam_pref;
  cmd->hybr_pref = hybr_pref;
  cmd->manual_sys_info_ptr = manual_sys_info_ptr;
  cmd->domain_pref = domain_pref;
  cmd->acq_order_pref = acq_order_pref;
  cmd->is_gwl_subsc_avail = is_gwl_subsc_avail;
  cmd->is_1x_subsc_avail = is_1x_subsc_avail;
  cmd->is_gw2_subsc_avail = is_gw2_subsc_avail;
  cmd->is_gw3_subsc_avail = is_gw3_subsc_avail;

  act = sdcmd_nam_sel4(cmd);
  /* act = sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd); */
  ref_cnt_obj_release(cmd);

  return act;
}


/*===========================================================================

FUNCTION sd_hybr_2_nam_sel

DESCRIPTION
  Inform SD of a new NAM selection, as well as, the selected NAM's
  SS-Preferences.

  NOTE! This function only returns after the PRL of the selected NAM is
  read from NV.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  TRUE if NAM change is successful (i.e. the PRL of the selected NAM is
  successfully extracted from NV). FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/


EXTERN  boolean                          sd_hybr_2_nam_sel(

        sd_nam_e_type                     curr_nam,
            /* Current NAM selection (NAM-1, NAM-2, etc.).
            */

        sd_ss_orig_mode_e_type             orig_mode,
            /* The origination mode (normal, OTASP, emergency, other, etc.)
            ** that is associated with the selected NAM.
            */

        sd_ss_mode_pref_e_type             mode_pref,
            /* The mode preference that is associated with the selected NAM.
            */

        sd_ss_band_pref_e_type              band_pref,
            /* The band preference that is associated with the selected NAM.
            */


        sd_ss_band_pref_e_type              lte_band_pref,
            /* The LTE band preference that is associated with the selected NAM.
            */

        sd_ss_band_pref_e_type              tds_band_pref,
            /* The LTE band preference that is associated with the selected NAM.
            */

        sd_ss_prl_pref_e_type               prl_pref,
            /* New prl system preference (Available BC0A, available BC0B, etc.)
            */

        sd_ss_roam_pref_e_type              roam_pref,
            /* The band preference that is associated with the selected NAM.
            */

        sd_ss_hybr_pref_e_type              hybr_pref,
            /* New hybrid preference (none, CDMA/HDR).
            */

        const sys_manual_sys_info_s_type    *manual_sys_info_ptr,
          /* Pointer to struct that defines the manual system.
           */

        sd_ss_srv_domain_pref_e_type        domain_pref,
           /* Service domain preference that is associated with the selected
           ** NAM.
           ** Applies only for GSM/WCDMA modes.
           */

        sd_ss_acq_order_pref_e_type         acq_order_pref,
           /* Preference for the order of acquisition ( WCDMA before GSM,
           ** GSM before WCDMA etc).
           */

        boolean                             is_gw_subsc_avail,
           /* Indicates if subscription is available for providing service on
           ** GSM/UMTS network.
           */

        boolean                             is_1x_subsc_avail,
           /**< Indicates if subscription is available for providing service on
           ** CDMA/AMPS/HDR network.
           */

        boolean                             is_gw2_subsc_avail
           /**< Indicates if subscription is available for providing service on
           ** GSM/UMTS network.
           */
)
{
  return sd_hybr_per_ss_nam_sel(curr_nam,
    orig_mode,
    mode_pref,
    band_pref,
    sys_map_band_mask_to_lte_band_mask(lte_band_pref),
    tds_band_pref,
    prl_pref,
    roam_pref,
    hybr_pref,
    manual_sys_info_ptr,
    domain_pref,
    acq_order_pref,
    is_gw_subsc_avail,
    is_1x_subsc_avail,
    is_gw2_subsc_avail,
    0,
    SD_SS_HYBR_2
    );
}

/*===========================================================================

FUNCTION sd_hybr_per_ss_nam_sel

DESCRIPTION
  Inform SD of a new NAM selection, as well as, the selected NAM's
  SS-Preferences.

  NOTE! This function only returns after the PRL of the selected NAM is
  read from NV.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  TRUE if NAM change is successful (i.e. the PRL of the selected NAM is
  successfully extracted from NV). FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/


EXTERN  boolean                          sd_hybr_per_ss_nam_sel(

        sd_nam_e_type                     curr_nam,
            /* Current NAM selection (NAM-1, NAM-2, etc.).
            */

        sd_ss_orig_mode_e_type             orig_mode,
            /* The origination mode (normal, OTASP, emergency, other, etc.)
            ** that is associated with the selected NAM.
            */

        sd_ss_mode_pref_e_type             mode_pref,
            /* The mode preference that is associated with the selected NAM.
            */

        sd_ss_band_pref_e_type              band_pref,
            /* The band preference that is associated with the selected NAM.
            */


        sys_lte_band_mask_e_type            lte_band_pref,
            /* The LTE band preference that is associated with the selected NAM.
            */

        sd_ss_band_pref_e_type              tds_band_pref,
            /* The LTE band preference that is associated with the selected NAM.
            */


        sd_ss_prl_pref_e_type               prl_pref,
            /* New prl system preference (Available BC0A, available BC0B, etc.)
            */

        sd_ss_roam_pref_e_type              roam_pref,
            /* The band preference that is associated with the selected NAM.
            */

        sd_ss_hybr_pref_e_type              hybr_pref,
            /* New hybrid preference (none, CDMA/HDR).
            */

        const sys_manual_sys_info_s_type    *manual_sys_info_ptr,
          /* Pointer to struct that defines the manual system.
           */

        sd_ss_srv_domain_pref_e_type        domain_pref,
           /* Service domain preference that is associated with the selected
           ** NAM.
           ** Applies only for GSM/WCDMA modes.
           */

        sd_ss_acq_order_pref_e_type         acq_order_pref,
           /* Preference for the order of acquisition ( WCDMA before GSM,
           ** GSM before WCDMA etc).
           */

        boolean                             is_gw_subsc_avail,
           /* Indicates if subscription is available for providing service on
           ** GSM/UMTS network.
           */

        boolean                             is_1x_subsc_avail,
           /**< Indicates if subscription is available for providing service on
           ** CDMA/AMPS/HDR network.
           */

        boolean                             is_gw2_subsc_avail,
           /**< Indicates if subscription is available for providing service on
           ** GSM/UMTS network.
           */

        boolean                             is_gw3_subsc_avail,
          /**< Indicates if subscription is available for providing service on
              ** GSM/UMTS network.
              */

        sd_ss_e_type                      ss
        /** SS for which nam sel is called */

)
{

  boolean         ret_val;
  sd_nam_sel_cmd  *cmd;

  /* ======================================================== */

  cmd = (sd_nam_sel_cmd *) sdtask_sd_cmd_new(
                                              sizeof(sd_nam_sel_cmd),
                                                (execute_fn) sdcmd_hybr_per_ss_nam_sel,
                                              NULL,
                                              NULL
                                              );
  /* ======================================================== */

  cmd->ss = ss;
  cmd->curr_nam = curr_nam;
  cmd->orig_mode = orig_mode;
  cmd->mode_pref = mode_pref;
  cmd->band_pref = band_pref;
  cmd->lte_band_pref = lte_band_pref;
  cmd->tds_band_pref = tds_band_pref;
  cmd->prl_pref = prl_pref;
  cmd->roam_pref = roam_pref;
  cmd->hybr_pref = hybr_pref;
  cmd->manual_sys_info_ptr = manual_sys_info_ptr;
  cmd->domain_pref = domain_pref;
  cmd->acq_order_pref = acq_order_pref;
  cmd->is_gwl_subsc_avail = is_gw_subsc_avail;
  cmd->is_1x_subsc_avail = is_1x_subsc_avail;
  cmd->is_gw2_subsc_avail = is_gw2_subsc_avail;
  #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
  cmd->is_gw3_subsc_avail = is_gw3_subsc_avail;
  #endif

  ret_val = (boolean) sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd);
  ref_cnt_obj_release(cmd);

  return ret_val;
}



/* <EJECT> */
/**===========================================================================

FUNCTION sd_term

DESCRIPTION
  Terminate SD.

  NOTE! This function must be called when powering-off the phone. After
  calling this function no other public SD function shall be called prior to
  calling sd_init().

DEPENDENCIES
  This function must be called when powering-off the phone. After calling
  this function no other public SD function shall be called prior to
  calling sd_init().

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                      sd_term( void )
{
  /* SD front end */
  sd_ss_reset();

  /* Reset sdss component acq info */
  sdss_component_reset();

  /* Reset SDSR related acq info */
  sdsr_reset_acq_info();

}

/*===========================================================================

FUNCTION sd_ss_get_supp_mode_pref

DESCRIPTION
  Wrapper function for sdss_get_supp_mode_pref.
  Used to call from outside of SD.

DEPENDENCIES
  None.

RETURN VALUE
  Return a mode preference that corresponds to all the supported modes
  preferences out of the input mode preference.

SIDE EFFECTS
  None.

===========================================================================*/
sd_ss_mode_pref_e_type    sd_ss_get_supp_mode_pref(

        sd_ss_mode_pref_e_type    mode_pref,
            /* Mode preference from which to extract supported modes.
            */

        sd_ss_band_pref_e_type    band_pref,
            /* band preference from which to extract supported modes.
            */

        sys_lte_band_mask_e_type  lte_band_pref,
            /* LTE band preference from which to extract supported modes.
            */

        sd_ss_band_pref_e_type    tds_band_pref
            /* TD-SCDMA band preference from which to extract supported modes.
            */

)
{
  return sdss_get_supp_mode_pref_mmode (mode_pref, band_pref, lte_band_pref, tds_band_pref);
}


/*===========================================================================

FUNCTION sd_is_plmn_rat_allowed

DESCRIPTION
  API for NAS to query if current PLMN is allowed to camp. If TRUE NAS can proceed with attach. Otherwise
  NAS won't attach and return NO_SRV to CM.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/


EXTERN boolean sd_is_plmn_rat_allowed (
       sys_plmn_id_s_type    plmn_id,
         /* PLMN id
         */

       sd_mode_e_type        mode,
         /* Mode of the system under query
         */

       sd_ss_e_type          ss
)
{
  sd_ss_act_e_type            act;
  sd_is_plmn_rat_allowed_cmd  *cmd;

  cmd = (sd_is_plmn_rat_allowed_cmd *) sdtask_sd_cmd_new(
                                              sizeof(sd_is_plmn_rat_allowed_cmd),
                                              (execute_fn) sdcmd_is_plmn_rat_allowed,
                                              NULL,
                                              NULL
                                              );

  cmd->plmn_id = plmn_id;
  cmd->mode = mode;
  cmd->ss = ss;

  act = sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd);
  ref_cnt_obj_release(cmd);

  return act;
}


boolean sd_is_3gpp_mode_pref(
  sd_ss_mode_pref_e_type  mode_pref
  )
{
#define ALL_3GPP (SD_SS_MODE_PREF_TDS_GSM_WCDMA_LTE)

  return    SD_MODE_CONTAIN(mode_pref, ALL_3GPP)
         && !SD_MODE_CONTAIN(mode_pref, ~ALL_3GPP);
}

/**===========================================================================

@FUNCTION sd_misc_is_true_power_save

@DESCRIPTION
   This function will check and tell whether SD is in powersave or not.

@DEPENDENCIES
  None.

@RETURN VALUE
  TRUE if SD is in power save that needs to be informed to protocals otherwise FALSE. .

@SIDE EFFECTS
  None.

===========================================================================*/

boolean  sd_misc_is_true_power_save(sd_ss_e_type ss)
{
  boolean ret = FALSE;
  sdss_state_e_type ss_state = sd_get_ss_state(ss);
  sd_ss_act_s_type        ss_act;

  //Get last true action
  (void)sd_ss_act_get_curr_act_ss(ss, &ss_act);

  SD_MSG_HIGH_4("ss_state:%d act:%d pwr_save_reas:%d ss:%d", ss_state,
                 ss_act.act, ss_act.prm.pwr_save.reas,ss);

  if (ss_act.act == SD_SS_ACT_PWR_SAVE)
  {
    switch(ss_act.prm.pwr_save.reas)
    {
      case SD_SS_PWR_SAVE_REAS_NO_SRV:
      case SD_SS_PWR_SAVE_REAS_NO_SYS:
      case SD_SS_PWR_SAVE_REAS_PWR_DOWN:
      case SD_SS_PWR_SAVE_REAS_HYBR_BSR:
        ret = TRUE;
        break;

      case SD_SS_PWR_SAVE_REAS_MAX:
      case SD_SS_PWR_SAVE_REAS_PROT_DEACT:
      default:
        ret = FALSE;
        break;
    }
  }

  return ret;
}
/**===========================================================================

@FUNCTION sd_ss_ind_cdma_update_splmn_list

@DESCRIPTION
1x updates the splmn list to SD.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

  CDMA OH info must have already been sent by 1x

@RETURN VALUE
None

@SIDE EFFECTS
  None.

===========================================================================*/
EXTERN void sd_ss_ind_cdma_update_splmn_list
(
       sd_splmn_info_list_s *splmn_info_ptr
)
{
       sd_ss_ind_update_splmn_list_cmd *cmd;

       cmd = (sd_ss_ind_update_splmn_list_cmd *) sdtask_sd_cmd_new(
                                                    sizeof(sd_ss_ind_update_splmn_list_cmd),
                                                    (execute_fn) sdcmd_update_cdma_splmn_list,
                                                    NULL,
                                                    NULL
                                                    );

       cmd->splmn_info_ptr = splmn_info_ptr;
	
       (void) sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd);
        ref_cnt_obj_release(cmd);

}

#if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)
/**===========================================================================

@FUNCTION sd_ss_ind_cdma_opr_start_activity

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when CDMA starts a active
  (i.e. enter traffic channel. )

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

@SIDE EFFECTS
  None.

===========================================================================*/
sd_ss_act_e_type          sd_ss_ind_cdma_opr_start_activity(

        sd_ss_act_s_type          *rtrn_act_ptr,
            /**< Pointer to a buffer where to place returned SS-Action.
            */
        sd_ss_cdma_act_mask_e_type cdma_act_mask
            /**< Indicate the CDMA activity to be started.
            */
)
{
  sd_ss_act_e_type                 act;
  sd_ss_ind_cdma_opr_activity_cmd *cmd;

  cmd = (sd_ss_ind_cdma_opr_activity_cmd *) sdtask_sd_cmd_new(
                                               sizeof(sd_ss_ind_cdma_opr_activity_cmd),
                                               (execute_fn) sdcmd_cdma_opr_start_activity,
                                               rtrn_act_ptr,
                                               NULL
                                               );

  cmd->cdma_act_mask = cdma_act_mask;

  act = sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd);
  ref_cnt_obj_release(cmd);

  return act;
}

/**===========================================================================

@FUNCTION sd_ss_ind_cdma_opr_stop_activity

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when CDMA stops a active
  (i.e. exit traffic channel )

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

@SIDE EFFECTS
  None.

===========================================================================*/
sd_ss_act_e_type          sd_ss_ind_cdma_opr_stop_activity(

        sd_ss_act_s_type          *rtrn_act_ptr,
            /**< Pointer to a buffer where to place returned SS-Action.
            */
        sd_ss_cdma_act_mask_e_type cdma_act_mask
            /**< Indicate the  HDR activity to be stopped.
            */
)
{
  sd_ss_act_e_type                 act;
  sd_ss_ind_cdma_opr_activity_cmd *cmd;

  cmd = (sd_ss_ind_cdma_opr_activity_cmd *) sdtask_sd_cmd_new(
                                               sizeof(sd_ss_ind_cdma_opr_activity_cmd),
                                               (execute_fn) sdcmd_cdma_opr_stop_activity,
                                               rtrn_act_ptr,
                                               NULL
                                               );
  SD_MSG_HIGH_1( "STOP Activity, Mask from 1x = 0x%x", cdma_act_mask );
  cmd->cdma_act_mask = cdma_act_mask;

  act = sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd);
  ref_cnt_obj_release(cmd);

  return act;
}
#endif

/**===========================================================================

@FUNCTION sd_ss_ind_hdr_update_splmn_list

@DESCRIPTION
HDR updates the splmn list to SD.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

  HDR OH info must have already been sent by DO.

@RETURN VALUE
None

@SIDE EFFECTS
  None.

===========================================================================*/
EXTERN void sd_ss_ind_hdr_update_splmn_list
(
       sd_splmn_info_list_s *splmn_info_ptr
)
{
       sd_ss_ind_update_splmn_list_cmd *cmd;
	
       cmd = (sd_ss_ind_update_splmn_list_cmd *) sdtask_sd_cmd_new(
                                                    sizeof(sd_ss_ind_update_splmn_list_cmd),
                                                    (execute_fn) sdcmd_update_hdr_splmn_list,
                                                    NULL,
                                                    NULL
                                                    );
	
       cmd->splmn_info_ptr = splmn_info_ptr;
	
       (void) sdtask_queue_put_cmd_wait((sd_cmd_t *) cmd);
        ref_cnt_obj_release(cmd);

}

/**===========================================================================

@FUNCTION sd_misc_get_ss_for_user_mode_pref

@DESCRIPTION
  Function to get the SS for the user mode pref, to be used internally by CM during 
  IMS registration update

@DEPENDENCIES
  None.

@RETURN VALUE
 ss for the mode pref
 SD_SS_MAX if invalid 

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_e_type             sd_misc_get_ss_for_user_mode_pref(

        sys_sys_mode_e_type      sys_mode
)
{
  return sdss_get_ss_for_user_mode_pref( sys_mode );
}
/*===========================================================================

FUNCTION sd_ss_update_is_special_volte_e911_calls

DESCRIPTION
Update SD about specialization of emergency call

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  None.

@SIDE EFFECTS
  None.

===========================================================================*/
extern void sd_ss_update_is_special_volte_e911_calls 
(
    sys_specialization_t   special_volte_e911_calls
)
{
  sdss_set_is_special_volte_e911_calls(special_volte_e911_calls);  
}

/*===========================================================================

FUNCTION sd_ss_update_call_mode

DESCRIPTION
Update SD about current call mode
whether it is normal or Volte Only.


DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  None.

@SIDE EFFECTS
  None.

===========================================================================*/
extern void sd_ss_update_call_mode 
(
    sd_ss_e_type                ss,
    sys_call_mode_t   call_mode    
)
{
  if(sdss_is_1x_sxlte() && (ss!= SD_SS_HYBR_3))
  {
    sdss_set_call_mode(SD_SS_MAIN, call_mode);
	sdss_set_call_mode(SD_SS_HYBR_2, call_mode);
  }
  else
  {
    sdss_set_call_mode(ss, call_mode);
  }
  
}

/* ^L<EJECT> */
/*===========================================================================

FUNCTION sd_misc_is_mmss_operation

DESCRIPTION
  Function to query if MMSS operation is TRUE on a ss

DEPENDENCIES
  None.

RETURN VALUE
  boolean
    TRUE - mmss operation on,
    FALSE - otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  boolean                  sd_misc_is_mmss_operation(

        sd_ss_e_type             ss
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  SD_ASSERT_ENUM_IS_INRANGE(ss, SD_SS_MAX);
  return ( sdss_is_mmss_operation( ss ));

} /* sd_misc_is_mmss_operation() */

/*===========================================================================

FUNCTION sd_misc_get_hdr_more_pref_plmn_list

DESCRIPTION
  Get a list of more preferred PLMNs as compared to the acquired HDR system per the PRL and/or other MMSS files
  Currently called only by HDR CP and used for reselection purposes.
  HDRCP should invoke this only after checking if LTE is supported using sd_misc_is_lte_full_srv_allowed()
  HDRCP should invoke this only after it has recieved permission from SD to continue on the current
  HDR system.
  the PLMN list may contain forbidden PLMNs
  The PLMN list can return PLMNs associated with more pref LTE and/or GW systems

DEPENDENCIES
  None

RETURN VALUE
  plmn list - pointer to memory allocated by HDR of type sys_plmn_list_s_type.
  SD will copy the PLMN list to this location
  If no pointer was passed, the function will return FALSE. else TRUE.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN boolean   sd_misc_get_hdr_more_pref_plmn_list( 

   sys_plmn_list_s_type *                  plmn_list 
     /* list of PLMNs more preferred than HDR */

   )
{
  sd_ss_e_type ss = sdss_get_ss_for_mode( SD_MODE_HDR);

  if (plmn_list == NULL) 
  {
    SD_MSG_LOW_0("null pointer passed");
    return FALSE;
  }

  /* Check if UE  is online*/
  if(sdss_get_opr_mode( SD_SS_MAIN ) != SDSS_OPR_MODE_ONLINE)
  {
    SD_MSG_LOW_0(" wrong state");
    return FALSE;
  }

  /* Check if HDR is on MAIN or HYBR1*/
  if (ss != SD_SS_MAIN && ss != SD_SS_HYBR_HDR) 
  {
    SD_MSG_HIGH_1("Invalid ss %d", ss );
    return FALSE;
  }

  /* Ensure HDR is in opr state . IF not , return FALSE*/
  if (!( sdss_get_ss_state(ss) == SDSS_STATE_OPR &&    
	 sd_si_ptr(ss)->si_info.srv_status == SYS_SRV_STATUS_SRV &&
         sd_si_ptr(ss)->si_info.mode  == SD_MODE_HDR
	) 
      ) 
  {
    SD_MSG_HIGH_3("Invalid state %d %d %d", ss, sdss_get_ss_state(ss),sd_si_ptr(ss)->si_info.srv_status );
    return FALSE;
  }


  /* Copy the list stored in SD core */ 
  {
    uint8  i = 0;
    sys_plmn_list_s_type * plmn_lst = sdss_get_more_pref_plmn_list(ss);
    SD_MSG_LOW_1("PLMN list cnt %d", plmn_lst->length);
    for (i = 0; i< plmn_lst->length; i++) 
    {
      SD_MSG_LOW_3("PLMN %x %x %x",plmn_lst->plmn[i].identity[0],
                                     plmn_lst->plmn[i].identity[1],
                                     plmn_lst->plmn[i].identity[2]);
    }


    /*=============    CRITICAL SECTION ENTER    ===============*/
    /* in case HDR oh is trying to update the plmn list on a diff thread. very remote
       possibility of this happening
    */
    sd_rex_enter_crit_sect();

    memscpy(plmn_list, 
            sizeof(sys_plmn_list_s_type),
            plmn_lst, 
            sizeof(sys_plmn_list_s_type));

    sd_rex_leave_crit_sect();
    /*=============    CRITICAL SECTION LEAVE    ===============*/

  }

  return TRUE;

}



/*===========================================================================

FUNCTION sd_ss_ind_update_special_volte_pref

DESCRIPTION
Update SD about current VOLTE Cs to PS redial flag


DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  None.

@SIDE EFFECTS
  None.

===========================================================================*/
extern void sd_ss_ind_update_special_volte_pref 
(
    sd_ss_e_type                ss,
    boolean                     is_special_volte_pref
)
{
  sdss_set_flag(ss,SDSS_FLAG_SPECIAL_VOLTE_REDIAL, is_special_volte_pref);
}

/*===========================================================================

FUNCTION sd_ss_ind_volte_call_state

DESCRIPTION
Update SD about current volte call status


DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  None.

@SIDE EFFECTS
  None.

===========================================================================*/
extern void sd_ss_ind_volte_call_state 
(
    sd_ss_e_type                ss,
    boolean   call_state    
)
{
  
  sdss_set_volte_call_state(ss, call_state);
  
}


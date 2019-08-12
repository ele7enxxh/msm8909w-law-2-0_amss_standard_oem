#ifndef CMREGPRX_H
#define CMREGPRX_H
/*===========================================================================

          R E G P R O X Y    H E A D E R    F I L E

DESCRIPTION
  This file contains all the definitions necessary for the Multi-Mode
  Controller ( MMoC ) and CM/REG to interface with the RegProxy module.

Copyright (c) 2002 - 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR FILE

   This section contains comments describing changes made to this file.
   Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/inc/cmregprx.h#1 $

when       who     what, where, why
--------   ---    -------------------------------------------------------
01/24/14   rm      Adding debug buffers 
11/14/13   ss      Added function to reset last srv request buffer to prevent
                   sending duplicate STOP MODE Req to NAS
07/21/13   skk     FR2921: Network initiated Authentication Procedure Support
                   during emergency call When SIM is perso Locked
07/24/11   aj     SC SVLTE 1.0 merge to mainline
12/23/10   sv     Integrate DSDS feature
11/08/10   aj     Moving cmi.h to cmregprx.c
11/05/10   aj     DO->LTE reselection code
12/22/09   fj     Modified APIs from GW to GWL
09/17/08   ks      When OOS in CELL_FACH, UE needs to follow UOOS scanning
                   pattern to re-acquire service
07/03/06   ic     Cleaned up leftovers from mainlining FEATURE_MULTIMODE_ARCH
04/01/06   ka     Adding sys to reg mapping function for net_list_cnf status.
11/30/05   ka     Adding api declaration cmregprx_send_hplmn_srch_req().
11/14/05   ic     Lint80 cleanup
11/08/05   sk     Separated CM reports and commands.
05/20/05   ic     Mainlined FEATURE_SD20
03/03/05   ka     Adding support to let mmoc know of protocol that has a
                  subs change when calling mmoc_cmd_subscription_chg
10/21/04   sj     Changed throttling mechanism for CM_SERVICE_CNF.
                  Removed handling of unused sanity timer mechanism.
02/06/04   ws     Initial jaguar/mainline merge.
08/20/03   SJ     Added srv cnf hold period, if srv cnf comes back within
                  1 second.
01/21/03   ws     Updated copyright information for 2003
04/22/02   sj     Initial release.

===========================================================================*/

#include "mmcp_variation.h"
#include "target.h"
#include "customer.h"
#include "rex.h"
#include "queue.h"
#include "cmd.h"
#include "sd.h"
#include "sd_v.h"
#include "prot.h"
#include "prot_v.h"
#include "cmwll.h"


/**--------------------------------------------------------------------------
** Macros
** --------------------------------------------------------------------------
*/

#if defined (FEATURE_MMODE_TRIPLE_SIM) || defined (FEATURE_MMODE_SXLTE_G)
  #ifdef FEATURE_MMODE_DUAL_SIM
    #define CMREGPRX_MMOC_MODE_PREF(ss, mmoc_mode)\
    if ((ss) == SD_SS_HYBR_3)\
    {\
      mmoc_mode = MMOC_MODE_HYBR_3_GW;\
    }\
    else\
    { \
      if((ss) == SD_SS_HYBR_2)\
      {\
        mmoc_mode = MMOC_MODE_HYBR_GW;\
      }\
      else\
      {\
        mmoc_mode = MMOC_MODE_GWL;\
      }\
    }
  #else /* FEATURE_GW_G_DUAL_STANDBY */
    /* This part might not required, because DUAL SIM flag will always be defined for TRIPLE SIM, Please check */
    #define CMREGPRX_MMOC_MODE_PREF(ss, mmoc_mode)\
    if ((ss) == SD_SS_HYBR_3)\
    {\
      mmoc_mode = MMOC_MODE_HYBR_3_GW;\
    }\
    else\
    { \
      mmoc_mode = MMOC_MODE_GWL;\
    }
  #endif
#else
  #define CMREGPRX_MMOC_MODE_PREF(ss, mmoc_mode)\
  if ((ss) == SD_SS_HYBR_3)\
  {\
    mmoc_mode = MMOC_MODE_HYBR_3_GW;\
  }\
  else\
  { \
    if((ss) == SD_SS_HYBR_2)\
    {\
      mmoc_mode = MMOC_MODE_HYBR_GW;\
    }\
    else\
    {\
      mmoc_mode = MMOC_MODE_GWL;\
    }\
  }
#endif /* FEATURE_MMODE_TRIPLE_SIM */



/*===========================================================================
=============================================================================
=============================================================================
============================== MMoC -> REGPROXY commands ====================
=============================================================================
=============================================================================
===========================================================================*/

/*===========================================================================

FUNCTION cmregprx_cmd_gwl_activate_protocol

DESCRIPTION
  This function should be called when the protocol related to GSM/WCDMA/LTE
  needs to be activated.

DEPENDENCIES
  cmregprx_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           cmregprx_cmd_gwl_activate_protocol
(
        prot_act_e_type                act_reason,
           /* Reason for activating the protocol.
           */

        sd_ss_e_type                 ss
          /* System selection type
          */

);

/*===========================================================================

FUNCTION cmregprx_cmd_gwl_activate_protocol_1

DESCRIPTION
  This function should be called when the protocol related to GSM/WCDMA/LTE
  needs to be activated.

DEPENDENCIES
  cmregprx_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           cmregprx_cmd_gwl_activate_protocol_1
(
        prot_act_e_type                act_reason,
           /* Reason for activating the protocol.
           */

        sd_ss_e_type                 ss,
          /* System selection type
          */

        uint16                          req_id
          /* Request Id
          */

);


/*===========================================================================

FUNCTION cmregprx_cmd_gwl_deactivate_protocol

DESCRIPTION
  This function should be called when the protocol related to GSM/WCDMA/LTE
  needs to be deactivated.

DEPENDENCIES
  cmregprx_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           cmregprx_cmd_gwl_deactivate_protocol
(
        prot_deact_e_type              deact_reason,
           /* Reason for deactivating the protocol.
           */

        prot_trans_type                trans_id,
           /* Trans_id for the deactivate protocol request.
           */

        sd_ss_e_type                   ss
           /* System Selection type
           */
);


/*===========================================================================

FUNCTION cmregprx_cmd_gwl_generic_cmd

DESCRIPTION
  This function should be called when the generic command needs to be
  processed by the RegProxy. The generic command could be because of Pref.
  system selection change, reselection time out, Get GSM/WCDMA/LTE networks
  list or Terminate Get GSM/WCDMA networks list.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           cmregprx_cmd_gwl_generic_cmd
(
        const prot_gen_cmd_s_type            *gen_cmd_ptr
           /* Pointer to the generic protocol command.
           */
);


/*===========================================================================

FUNCTION cmregprx_cmd_gwl_ph_stat_chgd

DESCRIPTION
  This function should be called to inform the RegProxy of the phone status
  changed. The phone status change could be because of operating mode change
  or subscription change.

DEPENDENCIES
  cmregprx_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           cmregprx_cmd_gwl_ph_stat_chgd
(

        prot_ph_stat_chg_e_type        chg_type,
           /* Type of the phone status change.
           */

        cm_subs_avail_e_type           is_gw_subs_avail,
           /* Current GSM/WCDMA subscription available flag.
           */

        prot_oprt_mode_e_type          oprt_mode,
           /* Current operating mode of the phone.
           */

        prot_trans_type                trans_id,
           /* Transaction id for the phone status change command.
           */

        prot_subsc_chg_e_type          prot_subsc_chg,
           /* Enumeration of protocols that have a change in
           ** subscription available status
           */
	    sys_ue_mode_e_type			   curr_ue_mode,
           /*  Current UE mode shared to capture UE mode change
           */   
      boolean        is_ue_mode_substate_srlte
           
);

/*===========================================================================

FUNCTION cmregprx_cmd_gwl_ph_stat_chgd_per_subs

DESCRIPTION
  This function should be called to inform the RegProxy of the phone status
  changed. The phone status change could be because of operating mode change
  or subscription change.

DEPENDENCIES
  cmregprx_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void                           cmregprx_cmd_gwl_ph_stat_chgd_per_subs
(

        prot_ph_stat_chg_e_type        chg_type,
           /* Type of the phone status change.
           */

        cm_subs_avail_e_type           is_gwl_subs_avail,
           /* Current GSM/WCDMA subscription available flag.
           */

        prot_oprt_mode_e_type          oprt_mode,
           /* Current operating mode of the phone.
           */

        prot_trans_type                trans_id,
           /* Transaction id for the phone status change command.
           */

        prot_subsc_chg_e_type          prot_subsc_chg,
         /* Enumeration of protocols that have a change in
         ** subscription available status
         */

        sd_ss_e_type                   ss,
          /* stack that is associated with this subscription
          */

        mmgsdi_session_type_enum_type  session_type,
          /* MMGSDI session type corresponding to this subscription
          */

        sys_modem_as_id_e_type         as_id,
          /* Active subscription-id assigned to this subscription
          */

        sd_ss_mode_pref_e_type         mode_pref,
          /* mode preference of this subscription
          */

        uint16                         nv_context,
          /* NV Context associated with this subscription
          */
          
        sys_ue_mode_e_type	           curr_ue_mode,
          /*  Current UE mode shared to capture UE mode change
           */
        boolean        is_ue_mode_substate_srlte
          /*  Current substate
           */
);

#ifdef FEATURE_MMODE_DUAL_SIM
/*===========================================================================

FUNCTION cmregprx_cmd_gw_ds_stat_chgd

DESCRIPTION
  This function should be called to inform the RegProxy of the dual standby status
  changed.

DEPENDENCIES
  cmregprx_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

extern void                          cmregprx_cmd_gw_ds_stat_chgd
(

  sys_modem_dual_standby_pref_e_type      ds_pref,
  /* Dual Standby preference
  */

  prot_trans_type          trans_id,
  /* Transaction id for the dual standby cmd
  */
  uint8                    active_ss,

  sys_modem_device_mode_e_type   device_mode

);

/*===========================================================================

FUNCTION cmregprx_cmd_gw_subs_cap_chgd

DESCRIPTION
  This function should be called to inform the RegProxy of the gw subs cap status
  changed. 

DEPENDENCIES
  cmregprx_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

extern void                          cmregprx_cmd_gw_subs_cap_chgd
(

  const  uint32  *subs_capability,
  
  prot_trans_type          trans_id
  /* Transaction id for the gw subs cap chgd cmd
  */
);

#endif

/*===========================================================================

FUNCTION cmregprx_cmd_ps_detach

DESCRIPTION
  This function should be called to inform the RegProxy of the ps detach req to be sent to NAS.

DEPENDENCIES
  cmregprx_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

extern void                            cmregprx_cmd_ps_detach
(
      sys_ps_detach_e_type           type,
      /* Type of detach requested
      */

      prot_trans_type                trans_id,
      /* Transaction id for the detach cmd
      */

      sd_ss_e_type                   ss
      /* SS for which this is requested
      */

);

/*===========================================================================

FUNCTION cmregprx_cmd_ps_detach

DESCRIPTION
  This function should be called to inform the RegProxy of the ps detach req to be sent to NAS.

DEPENDENCIES
  cmregprx_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

extern void                            cmregprx_cmd_ue_mode_switch
(

       sd_ss_e_type                    ss,

       /* Transaction id for the detach cmd */
       prot_trans_type                 trans_id,

       /* target ue_mode we're switching to */
       sys_ue_mode_e_type              ue_mode,

       boolean                         is_ue_mode_substate_srlte

);

/*===========================================================================

FUNCTION cmregprx_cmd_proc

DESCRIPTION
  This function should be called when the REX signal corresponding to
  RegProxy's command queue is set. This function will be responsible for
  dequeuing and getting the command processed.

DEPENDENCIES
  cmregprx_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           cmregprx_cmd_proc
(
        void

);

/*===========================================================================
=============================================================================
=============================================================================
============================== MSG ROUTER reports ======================
=============================================================================
=============================================================================
===========================================================================*/


#ifdef FEATURE_MMODE_LTE_RESEL
/*===========================================================================

FUNCTION cmregprx_msgr_rpt_proc

DESCRIPTION
  This function should be called to process the reports coming via MSG
  ROUTER. eg: LTE RRC ACT IND.

DEPENDENCIES
  cmregprx_init() should have been called previously.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
extern  void                        cmregprx_msgr_rpt_proc
(
  void         * rpt_ptr
    /* Pointer to MSGR report */
);

#endif //FEATURE_MMODE_LTE_RESEL

/*===========================================================================
=============================================================================
=============================================================================
============================== REG -> REGPROXY reports ======================
=============================================================================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION cmregprx_rpt_proc

DESCRIPTION
  This function should be called to process the reports from REG like
  cm_service_cnf, cm_service_ind , cm_network_list_cnf etc. This function
  would be responsible for processing the REG report by calling the
  appropriate state handlers.


DEPENDENCIES
  cmregprx_init() should have been called previously.

RETURN VALUE
  TRUE  - Report was processed.
  FALSE - Report was not processed.

SIDE EFFECTS
  None.
===========================================================================*/
extern  boolean                        cmregprx_rpt_proc
(
  const cm_hdr_type   *rpt_ptr
    /* Pointer to LL report */

);


/*===========================================================================
=============================================================================
=============================================================================
============================== Timer handler ================================
=============================================================================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION cmregprx_timer_proc

DESCRIPTION
  This function should be called to process the CM's timer tick.


DEPENDENCIES
  cmregprx_init() should have been called previously.

RETURN VALUE
     void

SIDE EFFECTS
  None.
===========================================================================*/
extern  void                        cmregprx_timer_proc
(
        void
);



/*===========================================================================
=============================================================================
=============================================================================
============================== CM -> REGPROXY functional interface ==========
=============================================================================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION cmregprx_proc_gw_resel_not_ok

DESCRIPTION
  This function should be called to inform the RegProxy that the GSM/WCDMA
  protocol is busy and RegProxy should not perform any reselection.

  Note! This function should only be called from the same REX task context as
  RegProxy's.

DEPENDENCIES
  cmregprx_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           cmregprx_proc_gw_resel_not_ok
(
        sd_ss_e_type ss
);


/*===========================================================================

FUNCTION cmregprx_proc_gw_resel_ok

DESCRIPTION
  This function should be called to inform the RegProxy that the GSM/WCDMA
  protocol has completed the system access and RegProxy can perform
  reselection if needed.

  Note! This function should only be called from the same REX task context as
  RegProxy's.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           cmregprx_proc_gw_resel_ok
(
        sd_ss_e_type ss
);


#ifdef MMODE_ADDITIONAL_DEBUG_INFO
/*===========================================================================

FUNCTION cmregprx_dbg_buf_init

DESCRIPTION
   This function should be called to initialize the Cmregproxy debug buffer values

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           cmregprx_dbg_buf_init
(
       void
);
#endif /* MMODE_ADDITIONAL_DEBUG_INFO */


/*===========================================================================

FUNCTION cmregprx_init

DESCRIPTION
  This function should be called to initialize the CM RegProxy.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           cmregprx_init
(
        sys_overall_feature_t         feature_mode,
        /* Which feature_mode is modem operating on */
        boolean                       is_power_on
        /* Is it power on */

);

extern  void         cmregprx_q_init
(
	void
);

extern void cmregprx_sig_init
(
	rex_tcb_type				   *tcb_ptr,
		/* Pointer to the task control block for the task in which
		** RegProxy runs.
		*/
	
	 rex_sigs_type					  cmd_q_sig
		/* Signal to be set when a command is posted in the RegProxy's
		** command queue.
		*/
);

/*===========================================================================

FUNCTION cmregprx_sanity_timeout

DESCRIPTION
  This function should be called when the RegProxy's sanity timer expires or
  in other words, when the RegProxy's sanity timer REX signal is set.

  Note! This function should only be called from the same REX task context as
  RegProxy's.

DEPENDENCIES
  cmregprx_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           cmregprx_sanity_timeout
(
        void
);


/*===========================================================================

FUNCTION cmregprx_send_hplmn_srch_req

DESCRIPTION
  Function is called to inform REG of user initiated HPLMN search request.
  As a result of this request REG initiates a PLMN search. IF HPLMN is found
  in the return result of this search REG attemps to register on the HPLMN.

  Note! This function should only be called from the same REX task context as
  RegProxy's.

DEPENDENCIES
  cmregprx_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern  void               cmregprx_send_hplmn_srch_req(

        sd_ss_e_type       ss
);

/*===========================================================================

FUNCTION cmregprx_send_ue_mode_ind_3gpp2

DESCRIPTION
  This function sends the UE mode Indication to 3GPP2 protocols

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void cmregprx_send_ue_mode_ind_3gpp2(

      sys_ue_mode_e_type   ue_mode,
      /* UE mode is the mode which UE is operating on. This is not Operation Mode
         ** (ONLINE, OFFLINE, LPM).
         */

      boolean              is_ue_mode_substate_srlte,
      /* valid only if UE mode is 1XSRLTE_CSFB_PREFERRED/1XSRLTE_ONLY.
          ** It indicates if UE is in CSFB mode or SRLTE mode of operation
          */

      sys_modem_as_id_e_type  as_id
);

/*===========================================================================

FUNCTION cmregprx_send_exit_from_pwr_save_ind

DESCRIPTION
  This function sends the cm_exit_from_pwr_save  to REG.
  CMREGPRXY substate will not change with this ind.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void                            cmregprx_send_exit_from_pwr_save_ind
(
        sd_ss_e_type  ss
);

/*===========================================================================

FUNCTION cmregprx_send_t3346_timer_ind

DESCRIPTION
  This function processes T3346 timer info from LL and sends the  
  timer info to clients via the message router interface.


DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void                            cmregprx_send_t3346_timer_ind
(
  sys_nas_t3346_status_e_type         t3346_notification,
  /* the status of timer T3346 */

  sys_radio_access_tech_e_type        active_rat,
  /* the RAT for which timer T3346 status is sent */

  sys_modem_as_id_e_type              subs_id
  /* active subscription */
);

/*===========================================================================

FUNCTION cmregprx_no_service_event_needed

DESCRIPTION
  This function will return GWL protocol is active or non-active on a particular SS

DEPENDENCIES
  None.

RETURN VALUE
  cmregprx_info_s_type* - Pointer to Regproxy's info.
  Note! Cannot be NULL

SIDE EFFECTS
  None.
===========================================================================*/
 boolean           cmregprx_no_service_event_needed
(

      sd_ss_e_type   ss

);

/*===========================================================================

FUNCTION cmregprx_map_sd_acq_order_to_sys_mode_pref

DESCRIPTION
  This function maps the SD mode pref type sd_ss_mode_pref_e_type to
  sys_mode_pref_e_type based on the acq_order_pref.

DEPENDENCIES
  None.

RETURN VALUE
  SYS_MODE_PREF_GSM_RADIO_ACCESS_PREFERRED  - acquire GSM mode systems first
                                              before WCDMA mode systems.
  SYS_MODE_PREF_UMTS_RADIO_ACCESS_PREFERRED - acquire UMTS mode systems first
                                              before GSM mode systems.
  SYS_MODE_PREF_GSM_RADIO_ACCESS_ONLY       - only GSM mode systems.
  SYS_MODE_PREF_UMTS_RADIO_ACCESS_ONLY      - only WCDMA mode systems.


SIDE EFFECTS
  None.
===========================================================================*/
sys_mode_pref_e_type       cmregprx_map_sd_acq_order_to_sys_mode_pref
(

       sd_ss_mode_pref_e_type          gw_mode_pref,
          /* SD GSM/WCDMA mode pref.
          */

       sd_ss_acq_order_pref_e_type     acq_order_pref,
          /* SD acquisition order preference.
          */

       sys_simultaneous_mode_e_type    simultaneous_mode
          /* SD simultaneous mode
          */
);

/*===========================================================================

FUNCTION cmregprx_map_sd_srv_req_type_to_reg_type

DESCRIPTION
  This function maps the SD mode pref type sd_ss_srv_req_type_e_type to
  reg_cm_service_req_e_type.

DEPENDENCIES
  None.

RETURN VALUE
  reg_cm_service_req_e_type

SIDE EFFECTS
  None.
===========================================================================*/
/*lint -esym(715,type)*/
/*lint -save -e533 */
reg_cm_service_req_e_type     cmregprx_map_sd_srv_req_type_to_reg_type
(
      sd_ss_srv_req_type_e_type  type
);

/*===========================================================================

FUNCTION cmregprx_map_sd_srv_domain_pref_to_sys

DESCRIPTION
  This function maps the SD service domain pref type
  sd_ss_srv_domain_pref_e_type to sys_mode_pref_e_type.

DEPENDENCIES
  None.

RETURN VALUE
  SYS_SRV_DOMAIN_CS_ONLY - Acquire circuit switched service.
  SYS_SRV_DOMAIN_PS_ONLY - Acquire packet switched service.
  SYS_SRV_DOMAIN_CS_PS   - Acquire both circuit & packet switched
                           service.



SIDE EFFECTS
  None.
===========================================================================*/
sys_srv_domain_e_type           cmregprx_map_sd_srv_domain_pref_to_sys
(

       sd_ss_srv_domain_pref_e_type    srv_domain_pref
          /* SD service domain preference.
          */

);


/*===========================================================================

FUNCTION cmregprx_map_list_cnf_from_reg_to_sys

DESCRIPTION
  Function maps List abort status from REG to sys.

DEPENDENCIES
  None.

RETURN VALUE
  SYS_PLMN_LIST_SUCCESS   - PLMN search success.
  SYS_PLMN_LIST_AS_ABORT  - Access startum aborted PLMN search.



SIDE EFFECTS
  None.
===========================================================================*/
extern sys_plmn_list_status_e_type    cmregprx_map_list_cnf_from_reg_to_sys
(

       reg_cm_network_list_status_e_type  net_list_status
          /* SD service domain preference.
          */

);


/*===========================================================================

FUNCTION cmregprx_map_list_cnf_from_sys_to_reg

DESCRIPTION
  This function maps the SD service domain pref type
  sd_ss_srv_domain_pref_e_type to sys_mode_pref_e_type.

DEPENDENCIES
  None.

RETURN VALUE




SIDE EFFECTS
  None.
===========================================================================*/
extern reg_cm_network_list_status_e_type  cmregprx_map_list_cnf_from_sys_to_reg
(

       sys_plmn_list_status_e_type     net_list_status
          /* SD service domain preference.
          */
);

/*===========================================================================

FUNCTION cmregprx_print_state_variables

DESCRIPTION
  This function should be invoked by the MMOC when sanity timer expired

DEPENDENCIES

RETURN VALUE


SIDE EFFECTS
  None.
===========================================================================*/
extern  void                        cmregprx_print_state_variables
(
void
);

/*===========================================================================

FUNCTION cmregprx_is_wait_srv_cnf

DESCRIPTION
  Tells if cmregprx is waiting for service confirmation.

DEPENDENCIES
  None.

RETURN VALUE
  



SIDE EFFECTS
  None.
===========================================================================*/
extern boolean  cmregprx_is_wait_srv_cnf(void);


#ifdef MMODE_ADDITIONAL_DEBUG_INFO

/*===========================================================================

FUNCTION cmregprx_dbg_buf_print_info

DESCRIPTION
  This function should be invoked by the MMOC when sanity timer expired to 
  print value of cmregprx_debug_buffer

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
extern  void     cmregprx_dbg_buf_print_info
(
void
);


#endif /* MMODE_ADDITIONAL_DEBUG_INFO */

/*===========================================================================
FUNCTION cmregprx_reset_last_service_req_buffer

DESCRIPTION
  This function reset the last srv req sent buffer for the given stack. This
  is necessary because when oprating in SGLTE Single Stack mode, if DEACT
  ind comes from NAS, CM does not send STOP_MODE_REQ on HYBR_2, and CMREGPRX
  is not Reset it can cause REGPRX to not send STOP_MODE_REQ subsequently

DEPENDENCIES
  None.

RETURN VALUE
  void.

SIDE EFFECTS
  None.
===========================================================================*/
extern void                            cmregprx_reset_last_service_req_buffer
(
   sd_ss_e_type          ss
);

/*===========================================================================
FUNCTION cmregprx_check_if_sim_state_update_pending

DESCRIPTION
  This function checks if the SIM_STATE_UPDATE_IND report is buffered in cmregprx 
  and if there is no higher priority activity in the priority queue and if yes, 
  process the report and update MMOC.

DEPENDENCIES
  None.

RETURN VALUE
  void.

SIDE EFFECTS
  None.
===========================================================================*/
extern void   cmregprx_check_if_sim_state_update_pending
( 
      sd_ss_e_type ss,
  
      boolean top_ptr_changed
);

/*===========================================================================

FUNCTION cmregprx_is_acq_db_in_progress

DESCRIPTION
  This function checks if Acq DB Scan is in progress during RLF

DEPENDENCIES
  None.

RETURN VALUE
  void.

SIDE EFFECTS
  None.
===========================================================================*/
boolean cmregprx_is_acq_db_in_progress
(
   sd_ss_e_type          ss
);


/*===========================================================================

FUNCTION cmregprx_get_hplmn

DESCRIPTION
  Get the EHPLMN list from NAS and store the first entry in the list in cmregprx. 
  Called when receiving SIM_AVAILABLE_CNF because the information read from 
  NAS is valid only after SIM becomes available.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmregprx_get_hplmn(sys_modem_as_id_e_type as_id);

#endif /* CMREGPRX_H */

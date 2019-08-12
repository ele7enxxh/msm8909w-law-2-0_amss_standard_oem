/*===========================================================================
            D S   S Y S T E M   C H A N G E   H A N D L E R         

GENERAL DESCRIPTION
  This module maintains the knowledge of the current system the DS C2K Mode 
  handler is present on.  All the modules that are interested in this 
  information should refer to the state of this module. 
  
  This module triggers the data_bearer_tech indication, maintains if the 
  C2K mode handler is on a DO system, or on a 1x-RTT system. If on a DO 
  system will also have the knowledge if it is connected to an EPC core 
  or a non-EPC core.

  
 
INITIALIZATION AND SEQUENCING REQUIREMENTS
   

 Copyright (c) 2009-2015 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_sys_chg_hdlr.c#1 $
  $DateTime: 2016/12/13 08:00:03 $ 
  $Author: mplcsds1 $
  
when       who        what, where, why
--------   ---        -------------------------------------------------------
05/04/15   sc         Send 1x acq notification to DSD while moving 
                      SYS_CHG_SM to NULL state only if it is delayed.
03/24/15   sc         Reset the sys chg info after all PDNs are down 
                      for eHRPD to 1x transition.      
06/30/14   niv        Added wrapper functions & API change for 
                      NV_REFRESH 
06/02/14   sc         Fixed array out of bounds in ds707_s101_transition_state 
11/18/11   vpk        HDR SU API cleanup
10/13/10   op         Migrated to MSG 2.0 macros
10/07/10   ttv        Added messages which explain the reason for asserting.
08/30/10   ls         Fixed the featurization issue
04/16/10   ls         Fix the featurization issues
03/26/10   ls         Change Silent Redial Timer in milliseconds
03/15/10   ls         Silent Redial Feature
01/04/10   ls         Remove the check from ds707_sec_pkt_mgr_is_ehrpd_capable
07/06/09   spn        Initial Revision.


===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "customer.h"
 
#if defined(T_WINNT)
#error code not present
#endif /* WINNT */

#include "ds707_sys_chg_hdlr.h"
#include "ds707_sec_pkt_mgr_util.h"
#include "data_msg.h"
#include "ds707_pkt_mgr.h"
#include "ds707_nv_util.h"
#include "ps_iface_defs.h"
#include "hdrcom_api.h"
#include "ds707_cback.h"
#include "nv_items.h"
#include "dstaski.h"
#include "ds_dsd_ext_i.h"
#include "ds3gdsdif.h"
#include "ds3gcfgmgr.h"
#include "ds707_pkt_mgri.h"
 
#ifdef FEATURE_EHRPD
  #include "dsutil.h"
  #include "nv.h"
  #include "ds3gmgr.h"
  #include "ds3gmshif.h"
  #include "ds3gsubsmgr.h"
#endif /* FEATURE_EHRPD */
 
#ifdef FEATURE_DATA_OPTHO
  #include "ds707_s101.h"
#endif /* FEATURE_DATA_OPTHO */
 
/*===========================================================================
                          VARIABLES
===========================================================================*/
typedef struct
{
  boolean hdr_sess_open;
  boolean stream_is_assigned;
  hdrcom_hai_app_subtype_enum_type  packet_app;
  uint32  phys_layer_subtype;
  ds707_sys_info_enum_type system_info; 
} ds707_sys_chg_hdlr_type;

typedef struct
{
  ds707_silent_redial_state_type sr_state;/*current SR state*/ 
  boolean                        call_over_ehrpd_only;/*enable disable ehrpd 
                                                        only origination*/
  uint8                          max_failure_count;/*max fail count in 
                                                     given time*/
  uint8                          failure_count;    /*fail count in 
                                                     given time*/
  uint8                          max_failure_total_count;/*max total fail count*/
  uint8                          failure_total_count;/*total fail count*/
  uint8                          new_call_attempt_max_timer_val; /*in minutes*/
  rex_timer_type                 new_call_attempt_timer; /*time between two 
                                                           call attempts*/
} ds707_silent_redial_info_type;

static ds707_sys_chg_hdlr_type ds_sys_chg_hdlr;
static ds707_silent_redial_info_type ds_silent_redial_info;
 
/*-------------------------------------------------------------------------
  Flag to indicate if DSD is waiting on MH's go-ahead to report
  pref sys chg to external clients
-------------------------------------------------------------------------*/
static boolean ds707_notify_dsd_after_pref_sys_update;

#ifdef FEATURE_EHRPD
static boolean ds707_cleanup_nonepc_data_session;

/*-------------------------------------------------------------------------
  Structure to store Sys Chg State Machine related details
-------------------------------------------------------------------------*/
typedef struct
{
  ds707_sys_chg_sm_e sys_chg_sm_state;      /* Sys Chg SM state */          
}ds707_sys_chg_sm_s;

static ds707_sys_chg_sm_s ds707_sys_chg_sm; /* Sys Chg State Machine */

/*---------------------------------------------------------------------------
  Used mainly for msg printing
  These need to be in sync with the ds707_sys_chg_sm_e enum
---------------------------------------------------------------------------*/
static char *sys_chg_sm_names[DS707_SYS_CHG_SM_MAX] =
{
  "NULL",
  "EPC_TO_NONEPC",
  "NONEPC_TO_EPC",
  "WAIT_FOR_DORM",
  "1X_SP_CALL_IN_PROG",
  "1X_SP_CALL_CLEANUP",
  "1X_DEREG_IN_PROG"
};

#if 0 /* Unused */
static ds_cmd_type *ds707_get_pdn_conn_ind_cmd_ptr = NULL;  /* Cache to store the 
                                                        get_pdn_conn_ind cmd */
#endif

#endif /* FEATURE_EHRPD */

/*===========================================================================
                            MACROS
===========================================================================*/
 
 
/*===========================================================================
                       INTERNAL FUNCTION DEFINITIONS
===========================================================================*/
#define SILENT_REDIAL_MAX_FAILURE_COUNT         3 
#define SILENT_REDIAL_MAX_FAILURE_TOTAL_COUNT   6
#define SILENT_REDIAL_DEF_TIME_IN_MIN           5
#define SILENT_REDIAL_TIME_IN_MS                60*1000 /* in milisecond */

#ifdef FEATURE_EHRPD
#define CLEANUP_NONEPC_CALL_EFS_PATH "/nv/item_files/data/3gpp2/NonEPC_data_session_cleanup"
#endif /* FEATURE_EHRPD */

/*===========================================================================
                       FORWARD DECLARATION
===========================================================================*/
#ifdef FEATURE_EHRPD
static void ds707_sys_chg_event_cback
(
  ds707_cback_event_type               event,
  const ds707_cback_event_info_type   *event_info_ptr
);
static void ds707_sys_chg_phys_link_up_ind(void);
static void ds707_sys_chg_phys_link_down_ind(void);
static void ds707_sys_chg_phys_link_null_ind(void);
static void ds707_sys_chg_cache_get_pdn_conn_ind_cmd(ds_cmd_type *cmd_ptr);
static void ds707_sys_chg_release_get_pdn_conn_ind_cmd_cache(void);

/* Commenting out below function since it's not being currently. It'll be
   enabled when there is a need to replay get_pdn_conn_ind cmd on 1X PPP
   Cleanup Failure */
/*static void ds707_sys_chg_replay_get_pdn_conn_ind_cmd(void);*/

#endif /* FEATURE_EHRPD */

/*=========================================================================== 
 
FUNCTION       DS707_SYS_CHG_HDLR_SILENT_REDIAL_INFO_INIT
 
DESCRIPTION    Initializes the module.
   
DEPENDENCIES   None.
 
RETURN VALUE   None.
 
SIDE EFFECTS   None.
===========================================================================*/ 
void ds707_sys_chg_hdlr_silent_redial_info_init(void)
{
  nv_stat_enum_type    nv_status;          /* status from NV call          */
  nv_item_type         nv_item;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*Read from NV*/
  DATA_IS707_MSG0(MSG_LEGACY_MED,
                  "ds707_sys_chg_hdlr_silent_redial_info_init");

  nv_status = ds3gcfgmgr_read_legacy_nv_ex( NV_DATA_CALL_OVER_EHRPD_ONLY_I,&nv_item, ds707_curr_subs_id() );
  
  /*-------------------------------------------------------------------------
    if NV read succeeds, load the values from NV,
    else load the default values
  -------------------------------------------------------------------------*/
  if (nv_status == NV_DONE_S)
  {
    ds_silent_redial_info.call_over_ehrpd_only = 
      nv_item.data_call_over_ehrpd_only.ehrpd_only_flag;
    if (ds_silent_redial_info.call_over_ehrpd_only == TRUE)
    {
      ds_silent_redial_info.sr_state = DS707_EHSR_STATE; 
    }
    else
    {
      ds_silent_redial_info.sr_state = DS707_AUSR_STATE;
    }
    ds_silent_redial_info.max_failure_count = 
      nv_item.data_call_over_ehrpd_only.max_failure_count;
    ds_silent_redial_info.max_failure_total_count = 
      nv_item.data_call_over_ehrpd_only.max_failure_total_count;
    ds_silent_redial_info.new_call_attempt_max_timer_val = 
      nv_item.data_call_over_ehrpd_only.new_attempt_max_timer;
  }
  else
  {
    //Siddharth: initialize sr_state correctly
    ds_silent_redial_info.sr_state = DS707_EHSR_STATE;
    ds_silent_redial_info.call_over_ehrpd_only = TRUE;
    ds_silent_redial_info.max_failure_count    = SILENT_REDIAL_MAX_FAILURE_COUNT;
    ds_silent_redial_info.max_failure_total_count = SILENT_REDIAL_MAX_FAILURE_TOTAL_COUNT;
    ds_silent_redial_info.new_call_attempt_max_timer_val = SILENT_REDIAL_DEF_TIME_IN_MIN;
  }

  ds_silent_redial_info.failure_count = 0;
  ds_silent_redial_info.failure_total_count = 0;
  
  if( !ds707_nv_util_is_refresh_in_progress(DS3GSUBSMGR_SUBS_ID_1))
  {
    rex_def_timer_ex(&(ds_silent_redial_info.new_call_attempt_timer),
                     (rex_timer_cb_type)ds707_sys_chg_new_call_attempt_timer_cb,
                     (uint32)NULL);
  }
}

/*===========================================================================
 
FUNCTION       DS707_SYS_CHG_NEW_CALL_ATTEMPT_TIMER_EXPIRED
 
DESCRIPTION    This funciton is called to process new call attempt timer 
               expiration. It resets the failure_count.  
   
DEPENDENCIES   None.
 
RETURN VALUE   None.
 
SIDE EFFECTS   None.
===========================================================================*/ 
void ds707_sys_chg_new_call_attempt_timer_expired(void)
{


  ds_silent_redial_info.failure_count = 0;
  DATA_IS707_MSG3(MSG_LEGACY_MED,
                      "processing new call attempt timer expiration "
                      "ds_silent_redial_info:sr_state %x,failure_count %x,failure_total_count %x",
                      ds_silent_redial_info.sr_state,
                      ds_silent_redial_info.failure_count,
                      ds_silent_redial_info.failure_total_count);

}
/*=========================================================================== 
 
FUNCTION       DS707_SYS_CHG_HDLR_INIT
 
DESCRIPTION    Initializes the module.
   
DEPENDENCIES   None.
 
RETURN VALUE   None.
 
SIDE EFFECTS   None.
===========================================================================*/ 
void ds707_sys_chg_hdlr_init(void)
{ 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 

  ds_sys_chg_hdlr.hdr_sess_open = FALSE; 
  ds_sys_chg_hdlr.stream_is_assigned = FALSE; 
  ds_sys_chg_hdlr.system_info  = DS707_SYS_INFO_NO_SYSTEM;
  ds707_notify_dsd_after_pref_sys_update = FALSE;

#ifdef FEATURE_EHRPD
  ds707_nv_util_sys_chg_hdlr_silent_redial_info_init();
#endif /* FEATURE_EHRPD */

#ifdef FEATURE_HDR
  ds707_sys_chg_hdlr_hdr_session_changed();
#endif /* FEATURE_HDR */

  DATA_IS707_MSG3( MSG_LEGACY_MED, 
                   "ds707_sys_chg_hdlr_init: phys_layer_subtype %d, packet_app %d, on_epc %d", 
                   ds_sys_chg_hdlr.phys_layer_subtype, 
                   ds_sys_chg_hdlr.packet_app,
                   ds_sys_chg_hdlr.system_info );
  
#ifdef FEATURE_EHRPD

  ds707_nv_util_read_nonepc_call_cleanup_efs();
  /*-----------------------------------------------------------------------
    Init Sys Chg State Machine
  -----------------------------------------------------------------------*/
  ds707_sys_chg_sm.sys_chg_sm_state = DS707_SYS_CHG_SM_NULL;

  /*-----------------------------------------------------------------------
    Register for Phys link events
  -----------------------------------------------------------------------*/
  if (ds707_cback_register( DS707_CBACK_PHYS_LINK_DOWN_EV, 
                            ds707_sys_chg_event_cback) == FALSE)
  {
    ASSERT(FALSE);
  }

  if (ds707_cback_register(DS707_CBACK_PHYS_LINK_UP_EV,
                           ds707_sys_chg_event_cback) == FALSE)
  {
    ASSERT(FALSE);
  }

#ifdef FEATURE_DATA_OPTHO
  if (ds707_cback_register(DS707_CBACK_TUN_PHYS_LINK_DOWN_EV,
                           ds707_sys_chg_event_cback) == FALSE)
  {
    ASSERT(FALSE);
  }
#endif /* FEATURE_DATA_OPTHO */

#endif /* FEATURE_EHRPD */

} /* ds707_sys_chg_hdlr_init() */ 
#ifdef FEATURE_HDR
/*=========================================================================== 
 
FUNCTION       DS707_SYS_CHG_HDLR_HDR_SESSION_CHANGED
 
DESCRIPTION    To be called to notify that the HDR session has changed. 
               Indication for this module to update the information related
               to HDR session
   
DEPENDENCIES   None.
 
RETURN VALUE   None.
 
SIDE EFFECTS   None.
===========================================================================*/ 
void ds707_sys_chg_hdlr_hdr_session_changed(void) 
{
  ds707_cback_event_info_type   dummy_event_info;
  uint32                        rat_mask = PS_IFACE_CDMA_NULL_BEARER;
  uint32                        so_mask = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_sys_chg_hdlr.stream_is_assigned = FALSE; 

  if ( hdrcp_smp_session_is_open(HDRHAI_PACKET_APPLICATION_CONTROLLER)
                                                               == TRUE &&
       hdrcp_stream_stream_is_assigned(HDRHAI_APP_PACKET_APP_ANY) == TRUE
     )
  {
    ds_sys_chg_hdlr.hdr_sess_open = TRUE;

#ifdef FEATURE_DATA_OPTHO
    /* Notify S101 about Session changed here since there might
       be changes happening to PPP/VSNCP due to sys change
       logic (EPC<->Non-EPC) implemented below */
    ds707_s101_hdr_session_chg_ev(TRUE);
#endif /* FEATURE_DATA_OPTHO */

    /* Init the packet_app to HDRHAI_APP_STREAM_NOT_USED */
    ds_sys_chg_hdlr.packet_app = HDRHAI_APP_STREAM_NOT_USED;
    ds_sys_chg_hdlr.stream_is_assigned = FALSE;

    /* Cache the Packet App Type */
    if (hdrcp_stream_stream_is_assigned(HDRHAI_DEF_PACKET_APP_SN))
    {
      ds_sys_chg_hdlr.packet_app = HDRHAI_DEF_PACKET_APP_SN;
      ds_sys_chg_hdlr.stream_is_assigned = TRUE; 
    }
    #ifdef FEATURE_HDR_QOS
    else if (hdrcp_stream_stream_is_assigned(HDRHAI_MULTIFLOW_PACKET_APP_SN))
    {
      ds_sys_chg_hdlr.packet_app = HDRHAI_MULTIFLOW_PACKET_APP_SN;
      ds_sys_chg_hdlr.stream_is_assigned = TRUE; 
    }
    #ifdef FEATURE_HDR_EMPA
    else if (hdrcp_stream_stream_is_assigned(
                                   HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN))
    {
      ds_sys_chg_hdlr.packet_app = HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN;
      ds_sys_chg_hdlr.stream_is_assigned = TRUE; 
    }
    #endif /* FEATURE_HDR_EMPA */
    else if (hdrcp_stream_stream_is_assigned(
                                   HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN))
    {
      ds_sys_chg_hdlr.packet_app = HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN;
      ds_sys_chg_hdlr.stream_is_assigned = TRUE; 
    }    
    #endif /* FEATURE_HDR_QOS */


    /* Cache the HDR Physical Layer Subtype, which determins the 
        Rel-Type ( Rev-0, Rev-A ) Type of the HDR Session */
    ds_sys_chg_hdlr.phys_layer_subtype = 
                hdrcp_scp_get_current_subtype(HDRHAI_PHYSICAL_LAYER_PROTOCOL);
    DATA_IS707_MSG2( MSG_LEGACY_MED, 
                     "ds707_sys_chg_hdlr: phys_layer_subtype %d, packet_app %d",
                     ds_sys_chg_hdlr.phys_layer_subtype,
                     ds_sys_chg_hdlr.packet_app );
    
    if ( ds_sys_chg_hdlr.system_info == DS707_SYS_INFO_NO_SYSTEM )
    {
      DATA_IS707_MSG0( MSG_LEGACY_MED, 
                       "ds707_sys_chg_hdlr: no current packet session, no handoff required on session changed" );
    }
    else if ((ds707_pkt_mgr_get_current_sys_mode() == SYS_SYS_MODE_HDR) 
#ifdef FEATURE_DATA_OPTHO
             || (ds707_s101_query_sm_state() == DS707_S101_SM_TUNNEL_CALL_ALLOWED)
#endif /* #ifdef FEATURE_DATA_OPTHO */
            )
    {
      if ( ds707_pkt_is_ehrpd_mode_of_operation() == TRUE )
      {
        if ( ds_sys_chg_hdlr.system_info == DS707_SYS_INFO_NON_EPC_SYSTEM )
        {
          ds707_sys_chg_hdlr_chg_sys_info(DS707_SYS_INFO_EPC_SYSTEM);
          DATA_IS707_MSG0( MSG_LEGACY_MED, 
                       "ds707_sys_chg_hdlr: posting event DS707_CBACK_HANDOFF_NONEPC_TO_EPC" );
          /* Notify that we moved from Non-EPC to EPC System */
          ds707_cback_post_event( DS707_CBACK_HANDOFF_NONEPC_TO_EPC,
                                  &dummy_event_info);
        }  
        else
        {
          DATA_IS707_MSG0( MSG_LEGACY_MED, 
                           "ds707_sys_chg_hdlr: EPC to EPC" );
        }  
      } /* ehrpd mode */
      else if ( ds_sys_chg_hdlr.system_info == DS707_SYS_INFO_EPC_SYSTEM )
      {
        DATA_IS707_MSG0( MSG_LEGACY_MED, 
                         "sys_chg_hdlr: personality switch / ehrpd - hrpd "
                         "posting event CBACK_HANDOFF_EPC_TO_NONEPC" );
        /* Personality switch / ehrpd - hrpd*/
        ds707_sys_chg_hdlr_chg_sys_info(DS707_SYS_INFO_NON_EPC_SYSTEM);
  
        /* Notify that we moved from Non-EPC to EPC System */
        ds707_cback_post_event( DS707_CBACK_HANDOFF_EPC_TO_NONEPC, 
                                &dummy_event_info);
      }  
      else
      {
        DATA_IS707_MSG0( MSG_LEGACY_MED, 
                           "ds707_sys_chg_hdlr: Non EPC to Non EPC" );
      }
    } /* there is current packet session and sys mode HDR*/
    else if (ds707_pkt_mgr_get_current_sys_mode() == SYS_SYS_MODE_CDMA)
    {
      /* UE has a data session on HRPD and switches from HRPD->1X->eHRPD
         rapidly. If the switch to eHRPD completes before HRPD->1X IDM chg 
         completes, the data session remains on HRPD (i.e CDSN = HDR)
         and doesn't transfer to 1X. The below check is to cover that 
         scenario and trigger nonepc_to_epc event if system moves from 
         HRPD to eHRPD and current sys is still 1X.
         Note: We cannot club this with the above nonepc->epc logic 'cos
         ehrpd_mode_of_operation will return false since curr sys is 1x*/

      if ((ds707_pkt_get_current_data_sess_network() == SYS_SYS_MODE_HDR) &&
          (ds707_sec_pkt_mgr_get_ehrpd_mode() == MODE_EHRPD) &&
          (ds_sys_chg_hdlr.packet_app == HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN ||
           ds_sys_chg_hdlr.packet_app == HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN)
         )
      {
        DATA_IS707_MSG0( MSG_LEGACY_MED, 
                       "ds707_sys_chg_hdlr: Curr Sys Mode: CDMA, CDSN: HDR"
                       "HDR session: eHRPD. Check if NonEPC->EPC event needs"
                       "to be triggered" );
        if ( ds_sys_chg_hdlr.system_info == DS707_SYS_INFO_NON_EPC_SYSTEM )
        {
          ds707_sys_chg_hdlr_chg_sys_info(DS707_SYS_INFO_EPC_SYSTEM);
          DATA_IS707_MSG0( MSG_LEGACY_MED, 
                       "ds707_sys_chg_hdlr: posting event DS707_CBACK_HANDOFF_NONEPC_TO_EPC" );
          /* Notify that we moved from Non-EPC to EPC System */
          ds707_cback_post_event( DS707_CBACK_HANDOFF_NONEPC_TO_EPC,
                                  &dummy_event_info);
        }
      } /* CDSN == HDR and in eHRPD mode */
    } /* Curr Sys == CDMA */
  } /* session open */
  else
  {
    DATA_IS707_MSG0( MSG_LEGACY_MED, 
                     "ds707_sys_chg_hdlr: session close - no updates to sys data" );
    ds_sys_chg_hdlr.hdr_sess_open = FALSE;
#ifdef FEATURE_DATA_OPTHO
    ds707_s101_hdr_session_chg_ev(FALSE);
#endif /* FEATURE_DATA_OPTHO */
  }

  /*---------------------------------------------------------------------
  Notify DSD Module
  ---------------------------------------------------------------------*/
  if(ds_sys_chg_hdlr.hdr_sess_open == TRUE)
  {
    /* Fill in RAT Mask based on phys_layer_subtype */
    switch (ds_sys_chg_hdlr.phys_layer_subtype)
    {
      case HDRCP_SCP_DEFAULT_PHYS_LAYER:
        rat_mask = PS_IFACE_CDMA_EVDO_REV0;
        break;

      case HDRCP_SCP_SUB2_PHYS_LAYER:
        rat_mask = PS_IFACE_CDMA_EVDO_REVA;
        break;

      case HDRCP_SCP_SUB3_PHYS_LAYER:
        rat_mask = PS_IFACE_CDMA_EVDO_REVB;
        break;

      default:
        DATA_IS707_MSG1( MSG_LEGACY_HIGH, 
                         "Invalid phys_layer_subtype %d",
                         ds_sys_chg_hdlr.phys_layer_subtype );
        break;
    }

    /* Fill in SO Mask based on packet_app */
    switch(ds_sys_chg_hdlr.packet_app)
    {
      case  HDRHAI_DEF_PACKET_APP_SN:
        so_mask = PS_IFACE_CDMA_EVDO_DPA;
        break;
      case HDRHAI_MULTIFLOW_PACKET_APP_SN:
        so_mask  = PS_IFACE_CDMA_EVDO_MFPA;
        break;
      case HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN:
        if(ds707_sec_pkt_mgr_get_ehrpd_mode() == MODE_EHRPD)
        {
          so_mask = PS_IFACE_CDMA_EVDO_EMPA_EHRPD;
        }
        else
        {
          so_mask = PS_IFACE_CDMA_EVDO_EMPA;
        }
        break;
      case HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN:
        if(ds707_sec_pkt_mgr_get_ehrpd_mode() == MODE_EHRPD)
        {
          so_mask = PS_IFACE_CDMA_EVDO_MMPA_EHRPD;
        }
        else
        {
          so_mask = PS_IFACE_CDMA_EVDO_MMPA;
        }
        break;
      default:
        DATA_IS707_MSG1( MSG_LEGACY_HIGH, "Invalid PKT APP %d",
                         ds_sys_chg_hdlr.packet_app );
        break;
    }
    if(ds_sys_chg_hdlr.stream_is_assigned == TRUE)
    {
      DATA_IS707_MSG2( MSG_LEGACY_HIGH,
                       "Session Open notified, SO: %d RAT: %d",
                       so_mask, rat_mask);

      ds3gdsdif_notify_dsd_hdr_session_event(
         SYS_MODEM_AS_ID_1,
         DS_DSD_CLIENT_RADIO_HDRSESSIONOPENED_EV,
         rat_mask,
         so_mask);
    }
    else
    {
      DATA_IS707_MSG0(MSG_LEGACY_HIGH,
                      "Stream not assigned: Session Close notified");
      ds3gdsdif_notify_dsd_hdr_session_event(
         SYS_MODEM_AS_ID_1,
         DS_DSD_CLIENT_RADIO_HDRSESSIONCLOSED_EV,
         rat_mask,
         so_mask);
    }
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH,"Session Close notified");
    ds3gdsdif_notify_dsd_hdr_session_event(
       SYS_MODEM_AS_ID_1,
       DS_DSD_CLIENT_RADIO_HDRSESSIONCLOSED_EV,
       rat_mask,
       so_mask);
  }
} /* ds707_sys_chg_hdlr_hdr_session_changed() */

/*=========================================================================== 
 
FUNCTION       DS707_SYS_CHG_HDLR_GET_ACTIVE_HDR_PROTOCOL
 
DESCRIPTION    
   
DEPENDENCIES   None.
 
RETURN VALUE   None.
 
SIDE EFFECTS   None.
===========================================================================*/ 
sys_active_prot_e_type ds707_sys_chg_hdlr_get_active_hdr_protocol(void) 
{
  sys_active_prot_e_type ret_val = SYS_ACTIVE_PROT_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( ds_sys_chg_hdlr.hdr_sess_open == TRUE )
  {
    if ( ds_sys_chg_hdlr.phys_layer_subtype == HDRCP_SCP_DEFAULT_PHYS_LAYER )
    {
      ret_val = SYS_ACTIVE_PROT_HDR_REL0;
    }
    #ifdef FEATURE_HDR_QOS
    else if ( ds_sys_chg_hdlr.phys_layer_subtype == HDRCP_SCP_SUB2_PHYS_LAYER )
    {
      ret_val = SYS_ACTIVE_PROT_HDR_RELA;
    }
    #ifdef FEATURE_HDR_REVB 
    else if ( ds_sys_chg_hdlr.phys_layer_subtype == HDRCP_SCP_SUB3_PHYS_LAYER )
    {
      ret_val = SYS_ACTIVE_PROT_HDR_RELB;
    }
    #endif /* FEATURE_HDR_REVB */
    #endif /* FEATURE_HDR_QOS */
  }  

  DATA_IS707_MSG1( MSG_LEGACY_MED, 
                   "ds707_sys_chg_hdlr: active_hdr_protocol %d",
                   ret_val );
  return ret_val;

} /* ds707_sys_chg_hdlr_get_active_hdr_protocol() */
#endif /* FEATURE_HDR */
/*=========================================================================== 
 
FUNCTION       DS707_SYS_CHG_HDLR_SYS_MODE_UPDATED
 
DESCRIPTION    
   
DEPENDENCIES   None.
 
RETURN VALUE   boolean - True to notify the caller that the data call is 
               cleaned up. 
 
SIDE EFFECTS   None.
===========================================================================*/ 
boolean ds707_sys_chg_hdlr_sys_mode_updated(sys_sys_mode_e_type new_sys_mode) 
{
  ds707_cback_event_info_type   dummy_event_info;
  boolean ret_val = FALSE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  dummy_event_info.new_network = new_sys_mode;
  DATA_IS707_MSG1( MSG_LEGACY_MED, 
                   "ds707_sys_chg_hdlr: _sys_mode_updated - new sys mode %d", 
                   dummy_event_info.new_network );
  /*------------------------------------------------------------------------- 
    Check if sys chg to DO and HDR session is open. IF session not open
    then don't make any decision regarding handoff. When the session is
    negotiated again, another trigger is given through _session_chagned.
    That time the correct decision regarding handoff is taken.
  -------------------------------------------------------------------------*/
#ifdef FEATURE_HDR
  if ((new_sys_mode == SYS_SYS_MODE_HDR) &&
      (hdrcp_smp_session_is_open(HDRHAI_PACKET_APPLICATION_CONTROLLER) == FALSE ||
       hdrcp_stream_stream_is_assigned(HDRHAI_APP_PACKET_APP_ANY) == FALSE))
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                    "sys chg to HDR but HDR session not open.Ignore sys chg");
    return ret_val;
  }
#endif /* FEATURE_HDR */
#ifdef FEATURE_EHRPD 
  if ( ds_sys_chg_hdlr.system_info == DS707_SYS_INFO_EPC_SYSTEM )
  {
    if ( new_sys_mode == SYS_SYS_MODE_CDMA )
    {
      ds_sys_chg_hdlr.system_info = DS707_SYS_INFO_NON_EPC_SYSTEM;

      /* Call to notify that handover from epc to non-epc */
      DATA_IS707_MSG0( MSG_LEGACY_MED, 
                       "ds707_sys_chg_hdlr: _sys_mode_updated - posting event DS707_CBACK_HANDOFF_EPC_TO_NONEPC" );
      ds707_cback_post_event( DS707_CBACK_HANDOFF_EPC_TO_NONEPC,
                              &dummy_event_info);
      ret_val = TRUE;
    }
    else if ( new_sys_mode == SYS_SYS_MODE_HDR )
    {
      if ( ds707_sec_pkt_mgr_get_ehrpd_mode() == MODE_NON_EHRPD )
      {
        ds_sys_chg_hdlr.system_info = DS707_SYS_INFO_NON_EPC_SYSTEM;

        /* Call to notify that handover from epc to non-epc */
        DATA_IS707_MSG0( MSG_LEGACY_MED, 
                       "ds707_sys_chg_hdlr: _sys_mode_updated - posting event DS707_CBACK_HANDOFF_EPC_TO_NONEPC" );
        ds707_cback_post_event( DS707_CBACK_HANDOFF_EPC_TO_NONEPC,
                                &dummy_event_info);
        ret_val = TRUE;
      }
      else
      {
        /* We were already on EPC system and moved to a new EPC system,
           no need to do any thing */
        DATA_IS707_MSG0( MSG_LEGACY_MED, 
                         "ds707_sys_chg_hdlr: _sys_mode_updated - on EPC, moved to new EPC" );
      }
    }
  } /* Were on EPC system */
  else if ( ds_sys_chg_hdlr.system_info == DS707_SYS_INFO_NON_EPC_SYSTEM )
  {
    if ( new_sys_mode == SYS_SYS_MODE_CDMA )
    {
      /* We were already on non-EPC system and moved to a new non-EPC system,
         no need to do any thing */
      DATA_IS707_MSG0( MSG_LEGACY_MED, 
                       "ds707_sys_chg_hdlr: _sys_mode_updated - on non-EPC, moved to new non-EPC" );
    }
    else if ( new_sys_mode == SYS_SYS_MODE_HDR )
    {
      if ( ds707_sec_pkt_mgr_get_ehrpd_mode() == MODE_EHRPD )
      {
        ds_sys_chg_hdlr.system_info = DS707_SYS_INFO_EPC_SYSTEM;
        DATA_IS707_MSG0( MSG_LEGACY_MED, 
                       "ds707_sys_chg_hdlr: _sys_mode_updated - DS707_CBACK_HANDOFF_NONEPC_TO_EPC" );
        /* Call to notify that handover from non-epc to epc */
        ds707_cback_post_event( DS707_CBACK_HANDOFF_NONEPC_TO_EPC,
                                &dummy_event_info);
        ret_val = TRUE;
      }
      else
      {
        /* We were already on non-EPC system and moved to a new non-EPC system,
           no need to do any thing */
        DATA_IS707_MSG0( MSG_LEGACY_MED, 
                       "ds707_sys_chg_hdlr: _sys_mode_updated - on non-EPC, moved to new non-EPC" );
      }
    }
  } /* Were on non-epc-system */
#endif /* FEATURE_EHRPD */
  return ret_val;
} /* ds707_sys_chg_hdlr_sys_mode_updated() */

/*=========================================================================== 
 
FUNCTION       DS707_SYS_CHG_HDLR_IS_CURRENT_SYSTEM_EPC
 
DESCRIPTION    

DEPENDENCIES   None.
 
RETURN VALUE   None.
 
SIDE EFFECTS   None.
===========================================================================*/ 
ds707_sys_info_enum_type ds707_sys_chg_hdlr_get_current_system(void)
{
  return (ds_sys_chg_hdlr.system_info);
} /* ds707_sys_chg_hdlr_get_current_system */

/*=========================================================================== 
 
FUNCTION       DS707_SYS_CHG_HDLR_IS_SESS_OPEN
 
DESCRIPTION    

DEPENDENCIES   None.
 
RETURN VALUE   None.
 
SIDE EFFECTS   None.
===========================================================================*/ 
boolean ds707_sys_chg_hdlr_is_sess_open(void)
{
  return (ds_sys_chg_hdlr.hdr_sess_open);
} /* ds707_sys_chg_hdlr_get_current_system */

/*=========================================================================== 
 
FUNCTION       DS707_SYS_CHG_HDLR_CHG_SYS_INFO
 
DESCRIPTION    
   
DEPENDENCIES   None.
 
RETURN VALUE   None.
 
SIDE EFFECTS   None.
===========================================================================*/ 
void ds707_sys_chg_hdlr_chg_sys_info(
  ds707_sys_info_enum_type sys_info
)
{
  DATA_IS707_MSG1( MSG_LEGACY_MED, 
                   "ds707_sys_chg_hdlr: sys info changed to %d", sys_info );
  ds_sys_chg_hdlr.system_info = sys_info;
} /* ds707_sys_chg_hdlr_chg_sys_info */

/*=========================================================================== 
 
FUNCTION       DS707_SYS_CHG_SR_TRANSITION_STATE
 
DESCRIPTION    This function processes silent redial state change to the 
               new state. 
   
DEPENDENCIES   None.
 
RETURN VALUE   None.
 
SIDE EFFECTS   None.
===========================================================================*/ 
void ds707_sys_chg_sr_transition_state
(
  ds707_silent_redial_state_type new_state
)
{
  DATA_IS707_MSG2(MSG_LEGACY_MED,
                  "ds707_sys_chg_sr_transition_state from %x to %x ",
                  ds_silent_redial_info.sr_state,
                  new_state);
  ds_silent_redial_info.sr_state = new_state;

  /*-------------------------------------------------------------------------
    Executing state based functionality.
  -------------------------------------------------------------------------*/
  switch( ds_silent_redial_info.sr_state )
  {
    case DS707_EHSR_STATE:

      ds_silent_redial_info.failure_count = 0;
      ds_silent_redial_info.failure_total_count = 0;

      DATA_IS707_MSG0(MSG_LEGACY_MED,
                      "resetting failure_count and failure_total_count");
      break;

    case DS707_AUSR_STATE:
      DATA_IS707_MSG0(MSG_LEGACY_MED,
                      "transitioning to DS707_AUSR_STATE, "
                      "Send get_ss_info to obtain throttle info "
                      "from CM (if any)");

      /* As part of the ss info processing, if either 1X or HDR is throttled,
         DS posts DS_707_THROTTLE_INFO_UPDATE_CMD by which DSD is notified
         about the throttled system. This will let DSD recompute the
         preferred system if needed */
      if( FALSE == ds707_get_ss_info() )
      {
        DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Could not post SS command info get");
      }
      break;

    default:
      /*---------------------------------------------------------------------
        Sanity Check
      ---------------------------------------------------------------------*/
      DATA_ERR_FATAL("Invalid Silent Redial new state value.");
  }/* end switch */
}/* ds707_sys_chg_sr_transition_state() */
/*=========================================================================== 
 
FUNCTION       DS707_SYS_CHG_SR_PROCESS_CALL_END
 
DESCRIPTION    This function is called when DS recieves call end from CM and 
               phys link is in COMING_UP or RESUMING state i.e. call end
               happend while call origination. 
   
DEPENDENCIES   None.
 
RETURN VALUE   None.
 
SIDE EFFECTS   None.
===========================================================================*/ 
void ds707_sys_chg_sr_process_call_end
(
  void
)
{
  rex_timer_cnt_type max_timer_val = 0;

  DATA_IS707_MSG1(MSG_LEGACY_MED,
                  "ds707_sys_chg_sr_process_call_end in sr_state: %x",
                  ds_silent_redial_info.sr_state);

  if ( ds_silent_redial_info.sr_state == DS707_EHSR_STATE )
  {
    ds_silent_redial_info.failure_count++;
    ds_silent_redial_info.failure_total_count++;
    DATA_IS707_MSG2(MSG_LEGACY_MED,
                    "ds_silent_redial_info:failure_count %x,failure_total_count %x",
                    ds_silent_redial_info.failure_count,
                    ds_silent_redial_info.failure_total_count);

    if ( ds_silent_redial_info.failure_count == ds_silent_redial_info.max_failure_count ||
         ds_silent_redial_info.failure_total_count == ds_silent_redial_info.max_failure_total_count )
    {
      ds707_sys_chg_sr_transition_state(DS707_AUSR_STATE);
    }
    else
    {
      max_timer_val = ds_silent_redial_info.new_call_attempt_max_timer_val * SILENT_REDIAL_TIME_IN_MS;
      DATA_IS707_MSG1(MSG_LEGACY_MED,
                      "starting new call attempt timer for %x",
                      max_timer_val );
      (void)rex_set_timer(&(ds_silent_redial_info.new_call_attempt_timer),
                          max_timer_val);
    }
  }
  else if ( ds_silent_redial_info.sr_state == DS707_AUSR_STATE )
  {
    DATA_IS707_MSG0(MSG_LEGACY_MED,
                    "call end received in DS707_AUSR_STATE, do nothing");
  }
}/* ds707_sys_chg_sr_process_call_end() */

/*=========================================================================== 
 
FUNCTION       DS707_SYS_CHG_SR_PROCESS_CALL_CONNECT
 
DESCRIPTION    This function is called when DS recieves phys link up event.
               If the silent redial state is EHSR and the call is connected
               on eHRPD then it resets the counters for failure_count and
               total_failure_count
   
DEPENDENCIES   None.
 
RETURN VALUE   None.
 
SIDE EFFECTS   None.
===========================================================================*/ 
void ds707_sys_chg_sr_process_call_connect
(
  void
)
{
  DATA_IS707_MSG1(MSG_LEGACY_MED,
                  "ds707_sys_chg_sr_process_call_connect in sr_state: %x",
                  ds_silent_redial_info.sr_state);

  if ( ds_silent_redial_info.sr_state == DS707_EHSR_STATE )
  {
    if ( ds707_pkt_is_ehrpd_mode_of_operation() == TRUE )
    {

      ds_silent_redial_info.failure_count = 0;
      ds_silent_redial_info.failure_total_count = 0;
      DATA_IS707_MSG0(MSG_LEGACY_MED,
                    "resetting failure_count and failure_total_count ");
    }
    else
    {
      /*----------------------------------------------------------------------- 
        sr_state: DS707_EHSR_STATE
        mode of operation: HRPD/1X
      -----------------------------------------------------------------------*/
      DATA_IS707_MSG0(MSG_LEGACY_MED,
                    "1X/hrpd call connect received in DS707_EHSR_STATE, transition to DS707_AUSR_STATE");
      ds707_sys_chg_sr_transition_state(DS707_AUSR_STATE);
    }
  }
  else if ( ds_silent_redial_info.sr_state == DS707_AUSR_STATE )
  {
    if ( ds707_pkt_is_ehrpd_mode_of_operation() && ds_silent_redial_info.call_over_ehrpd_only == TRUE )
    {
      DATA_IS707_MSG0(MSG_LEGACY_MED,
                    "ehrpd call connect received in DS707_AUSR_STATE, transition to DS707_EHSR_STATE");
      ds707_sys_chg_sr_transition_state(DS707_EHSR_STATE);
    }
    else
    {
      DATA_IS707_MSG0(MSG_LEGACY_MED,
                    "call connected received in DS707_AUSR_STATE, do nothing");
    }
  }
}/* ds707_sys_chg_sr_process_call_connect() */

/*=========================================================================== 
 
FUNCTION       DS707_SYS_CHG_SR_PROCESS_CALL_ORIG
 
DESCRIPTION    This function is called when DS originates a call to CM. It 
               performs some additional processing like stopping the new
               call attempt timer before originating the CM call. 
   
DEPENDENCIES   None.
 
RETURN VALUE   BOOLEAN.
 
SIDE EFFECTS   None.
===========================================================================*/ 
void ds707_sys_chg_sr_process_call_orig
(
  void
)
{
  DATA_IS707_MSG1(MSG_LEGACY_MED,
                  "ds707_sys_chg_sr_process_call_orig in sr_state: %x",
                  ds_silent_redial_info.sr_state);

 (void) rex_clr_timer(&(ds_silent_redial_info.new_call_attempt_timer));
}/* ds707_sys_chg_sr_process_call_orig() */

/*=========================================================================== 
 
FUNCTION       DS707_SYS_CHG_SR_PROCESS_EPC_HANDOFF
 
DESCRIPTION    This function is called on EPC to non-EPC or non-EPC to EPC 
               handoff. It transitions sr_state accordingly. 
   
DEPENDENCIES   None.
 
RETURN VALUE   BOOLEAN.
 
SIDE EFFECTS   None.
===========================================================================*/
void ds707_sys_chg_sr_process_epc_handoff
(
  void
)
{
  ds707_sys_info_enum_type sys_info;

  sys_info = ds707_sys_chg_hdlr_get_current_system();
  DATA_IS707_MSG2(MSG_LEGACY_MED,
                  "ds707_sys_chg_sr_process_epc_handoff in sr_state: %x and sys_info: %x",
                  ds_silent_redial_info.sr_state,
                  sys_info);

  
  if ( ( sys_info == DS707_SYS_INFO_EPC_SYSTEM ) &&
       ( ds_silent_redial_info.sr_state == DS707_AUSR_STATE ) )
  {
    if ( ds_silent_redial_info.call_over_ehrpd_only == TRUE  )
    {
      ds707_sys_chg_sr_transition_state(DS707_EHSR_STATE);
    }
  }
  else if ( ( sys_info == DS707_SYS_INFO_NON_EPC_SYSTEM ) &&
            ( ds_silent_redial_info.sr_state == DS707_EHSR_STATE ) )
  {
    ds707_sys_chg_sr_transition_state(DS707_AUSR_STATE);
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_MED,
                  "ds707_sys_chg_sr_process_epc_handoff: no processing needed");
  }
}/* ds707_sys_chg_sr_process_epc_handoff() */

/*=========================================================================== 
 
FUNCTION       DS707_SYS_CHG_IS_IN_EHSR_STATE
 
DESCRIPTION    This is an utility function that returns TRUE if ds is in 
               EHSR state currently. 
   
DEPENDENCIES   None.
 
RETURN VALUE   BOOLEAN.
 
SIDE EFFECTS   None.
===========================================================================*/ 
boolean ds707_sys_chg_is_in_ehsr_state
(
  void
)
{
  DATA_IS707_MSG1(MSG_LEGACY_MED,
                  "ds_silent_redial_info:sr_state %x", 
                  ds_silent_redial_info.sr_state);
  if ( ds_silent_redial_info.sr_state == DS707_EHSR_STATE )
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
} /* ds707_sys_chg_is_in_ehsr_state() */

/*===========================================================================
FUNCTION       DS707_SYS_CHG_NEW_CALL_ATTEMPT_TIMER_CB

DESCRIPTION    Function is called when the Timer for a new call atttempt 
               is expired.
               
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds707_sys_chg_new_call_attempt_timer_cb
(
  void * user_data/*call back data unused */
)
{
  ds_cmd_type   *cmd_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/   
  DATA_IS707_MSG0(MSG_LEGACY_MED, "CB: NEW CALL ATTEMPT TIMER expired");
  
  /*----------------------------------------------------------------------
  This is a callback function called when the timer expires. Puts command
  in ds task command queue.
  ----------------------------------------------------------------------*/
  if( (cmd_ptr = ds_allocate_cmd_buf(DS_CMD_PAYLOAD_SIZE_ZERO)) == NULL )
  {
    /*------------------------------------------------------------------
    This condition can not be recovered
    -------------------------------------------------------------------*/
    ASSERT(0);     
    return;
  }
  
  cmd_ptr->hdr.cmd_id = DS_CMD_707_NEW_CALL_ATTEMPT_TIMER;
  ds_put_cmd(cmd_ptr);
  return;
} /* ds707_sys_chg_new_call_attempt_timer_cb() */

/*===========================================================================
FUNCTION      DS707_SYS_CHG_PROCESS_CMD

DESCRIPTION   

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  
===========================================================================*/
void ds707_sys_chg_process_cmd
(
  ds_cmd_type   *ds_cmd_ptr
)
{
  ds3g_cm_ac_state_info_type  *ac_state_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(ds_cmd_ptr != NULL);

  switch (ds_cmd_ptr->hdr.cmd_id)
  {
    
    case DS_CMD_707_NEW_CALL_ATTEMPT_TIMER:
      ds707_sys_chg_new_call_attempt_timer_expired();
      break;

    case DS_CMD_CM_AC_STATE:
      ASSERT(ds_cmd_ptr->cmd_payload_ptr != NULL);
      ac_state_info_ptr
        = (ds3g_cm_ac_state_info_type*)ds_cmd_ptr->cmd_payload_ptr;

      ds707_sys_chg_process_cleanup_ppp_state(ac_state_info_ptr->state,
                                              ac_state_info_ptr->end_reason);
      break;

    default:
      DATA_IS707_MSG1(MSG_LEGACY_ERROR, "Unknown cmd to ds707_sys_chg %d",
                      ds_cmd_ptr->hdr.cmd_id);
      break;
  }
} /* ds707_sys_chg_process_cmd() */

#ifdef FEATURE_EHRPD
/*===========================================================================
FUNCTION      DS707_SYS_CHG_SR_PROCESS_LPM_MODE

DESCRIPTION  Will reset Silent redial state machine and clean 
             up the failure count and total failure count. 

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_sys_chg_sr_process_lpm_mode
(
  void
)
{
  DATA_IS707_MSG0(MSG_LEGACY_MED,
                    "In ds707_sys_chg_sr_process_lpm_mode");

  /*Clear the new call attempt timer if it is started*/
  (void) rex_clr_timer(&(ds_silent_redial_info.new_call_attempt_timer));
  
  /*------------------------------------------------------------------------- 
    Reset the failure count and total failure count.
    ------------------------------------------------------------------------*/
  ds_silent_redial_info.failure_count = 0;
  ds_silent_redial_info.failure_total_count = 0;

  /*-------------------------------------------------------------------------- 
    Transition to EHSR state if call over ehrpd only, else transition to
    AUSR state.
    -------------------------------------------------------------------------*/

  if (ds_silent_redial_info.call_over_ehrpd_only == TRUE)
  {
    ds707_sys_chg_sr_transition_state(DS707_EHSR_STATE);
  }
  else
  {
    ds707_sys_chg_sr_transition_state(DS707_AUSR_STATE);
  }
  
}/* ds707_sys_chg_sr_process_lpm_mode() */

/*===========================================================================
FUNCTION      DS707_SYS_CHG_SM_TRANSITION_STATE

DESCRIPTION   Deals with Sys Chg SM state transitions

DEPENDENCIES  None

RETURN VALUE  Sys Chg SM State

SIDE EFFECTS  None
===========================================================================*/
static void ds707_sys_chg_sm_transition_state
(
  ds707_sys_chg_sm_e new_state
)
{
  ds707_sys_chg_sm_e old_state = ds707_sys_chg_sm.sys_chg_sm_state;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_IS707_MSG2(MSG_LEGACY_HIGH, 
                  "DS707_SYS_CHG_SM: Transitioning from: %d state to: %d state",
                  old_state,
                  new_state);

  if (old_state == new_state)
  {
    if ( new_state == DS707_SYS_CHG_SM_NULL )
    {
      ds707_phys_link_enable_flow(DS_FLOW_707_LOCK_MASK);

      if (ds707_sys_chg_is_dsd_notification_pending())
      {
        ds707_sys_chg_send_dsd_notification();
      }
    }
    return;
  }

  ds707_sys_chg_sm.sys_chg_sm_state = new_state;

  switch (new_state)
  {
     case DS707_SYS_CHG_SM_NULL:
     {
       ds707_phys_link_enable_flow(DS_FLOW_707_LOCK_MASK);
       if (ds707_sys_chg_is_dsd_notification_pending())
       {
         ds707_sys_chg_send_dsd_notification();
       }

       /*-------------------------------------------------------------------- 
         This is just for fail-safe purpose. The cache should have been
         already cleaned up even before the sys sm transitions to NULL state
       --------------------------------------------------------------------*/
       ds707_sys_chg_release_get_pdn_conn_ind_cmd_cache();
       break;
     }
     case DS707_SYS_CHG_SM_EPC_TO_NONEPC:
     case DS707_SYS_CHG_SM_NONEPC_TO_EPC:
     {
       DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                      "DS707_SYS_CHG_SM: Disable Phys link flow - Mask: "
                      "DS_FLOW_707_LOCK_MASK");
       ds707_phys_link_disable_flow(DS_FLOW_707_LOCK_MASK);
       break;
     }
     case DS707_SYS_CHG_SM_WAIT_FOR_DORM:
     case DS707_SYS_CHG_SM_1X_SP_CALL_IN_PROG:
     case DS707_SYS_CHG_SM_1X_SP_CALL_CLEANUP:
     case DS707_SYS_CHG_SM_1X_DEREG_IN_PROG:
       break;

     default:
       DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                       "DS707_SYS_CHG_SM: Invalid Transition");
       break;
        
  } /* switch (new_state) */

} /* ds707_sys_chg_sm_transition_state */

/*===========================================================================
FUNCTION      DS707_QUERY_SYS_CHG_SM_STATE

DESCRIPTION   Returns the Sys Chg SM state

DEPENDENCIES  None

RETURN VALUE  Sys Chg SM State

SIDE EFFECTS  None
===========================================================================*/
ds707_sys_chg_sm_e ds707_query_sys_chg_sm_state
(
  void
)
{
  DATA_IS707_MSG_SPRINTF_1(MSG_LEGACY_MED, "DS707_SYS_CHG_SM: In %s state", 
                sys_chg_sm_names[ds707_sys_chg_sm.sys_chg_sm_state]);
  return ds707_sys_chg_sm.sys_chg_sm_state;
}/* ds707_query_sys_chg_sm_state */

/*===========================================================================
FUNCTION      DS707_IS_CLEANUP_NONEPC_CALL_NV_SET

DESCRIPTION   Returns the value of ds707_cleanup_nonepc_data_session

DEPENDENCIES  None

RETURN VALUE  TRUE  - If NV is set 
              FALSE - Otherwise

SIDE EFFECTS  None
===========================================================================*/
static boolean ds707_is_cleanup_nonepc_call_nv_set
(
  void
)
{
  return ds707_cleanup_nonepc_data_session;
} /* ds707_is_cleanup_nonepc_call_nv_set */

/*===========================================================================
FUNCTION      DS707_SYS_CHG_EVENT_CBACK

DESCRIPTION   This cback is triggered whenever phys link changes

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_sys_chg_event_cback
(
  ds707_cback_event_type               event,
  const ds707_cback_event_info_type   *event_info_ptr
)
{
  if (NULL == event_info_ptr)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                    "ds707_sys_chg: Rcvd phys link event with "
                    "NULL event_info_ptr - ASSERT");  
    ASSERT(FALSE);
    return;
  }

  DATA_IS707_MSG2(MSG_LEGACY_MED, "ds707_sys_chg: Rcvd phys link event: %d, "
                                  "going dormant: %d",
                                  event, event_info_ptr->going_dormant);

  switch(event)
  {
    case DS707_CBACK_PHYS_LINK_DOWN_EV:
    {
      if (event_info_ptr->going_dormant)
      {
        ds707_sys_chg_phys_link_down_ind();
      }
      else
      {
        ds707_sys_chg_phys_link_null_ind();
      }
      
      break;
    }
    case DS707_CBACK_PHYS_LINK_UP_EV:
    {
      ds707_sys_chg_phys_link_up_ind();
      break;
    }

#ifdef FEATURE_DATA_OPTHO
    /*------------------------------------------------------------------------- 
      Required for optho scenario where UE moved from LTE->1X with
      preregistration. In that case, 3GPP2 MH will clean up the prereg with
      EPC_TO_NONEPC event and sys chg sm will be in EPC_TO_NONEPC state. Sys
      chg SM will be waiting for phys link gone event to transition back to
      NULL and since this is a tunnel call, the phys link gone event will be
      posted as a TUN_PHYS_LINK_DOWN_EV. We only act on tunnel phys link down
      event with going_dormanct == FALSE and there is no need to act for
      tunnel phys link down with going_dormant == TRUE (dormancy) event
    ------------------------------------------------------------------------*/
    case DS707_CBACK_TUN_PHYS_LINK_DOWN_EV:
    {   
      if (event_info_ptr->going_dormant == FALSE)
      {
        ds707_sys_chg_phys_link_null_ind();
      }
      break;
    }
#endif /* FEATURE_DATA_OPTHO */

    default:
      break;
  }
} /* ds707_sys_chg_event_cback */

/*===========================================================================
FUNCTION      DS707_SYS_CHG_IS_NONEPC_SP_CALL_CLEANUP_NEEDED

DESCRIPTION   Checks if any special call cleanup procedure needs to be done and 
              kick starts the cleanup procedure 

DEPENDENCIES  None

RETURN VALUE  TRUE  - If cleanup needed 
              FALSE - Otherwise 

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_sys_chg_is_nonepc_sp_call_cleanup_needed
(
  ds707_sys_chg_nonepc_cleanup_trigger_e cleanup_trigger,
  ds_cmd_type *cmd_ptr
)
{
  boolean               ret_val             = FALSE;
  boolean               hybr_gw_operational = FALSE;
  ps_phys_link_type     *phys_link_ptr      = NULL;
  phys_link_state_type  phys_link_state     = PHYS_LINK_STATE_INVALID;
  sys_sys_mode_e_type   ldsn                = SYS_SYS_MODE_NO_SRV;
  sys_sys_mode_e_type   cdsn                = SYS_SYS_MODE_NO_SRV;
  ds707_sys_chg_sm_e    sys_chg_sm_state    = DS707_SYS_CHG_SM_NULL;
  int16                 phys_link_err_num;
  int                   phys_link_ret_val   = -1;
  ds707_cback_event_info_type     event_info = {0};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!ds707_is_cleanup_nonepc_call_nv_set())
  {
    DATA_IS707_MSG0(MSG_LEGACY_MED, 
                    "ds707_sys_chg: cleanup_nonepc_call_nv not set. No special "
                    "cleanup needed");
    return FALSE;
  }

  /*------------------------------------------------------------------------- 
    Get GW stack operational state
  ------------------------------------------------------------------------*/
  hybr_gw_operational = ds3g_is_lte_hybrid_stack_active(SYS_MODEM_AS_ID_1);

  /*------------------------------------------------------------------------- 
    Get the phys link state
  ------------------------------------------------------------------------*/
  phys_link_ptr = ds707_pkt_get_pri_phys_link_ptr();
  if (NULL == phys_link_ptr)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                        "ds707_sys_chg: Pri Physlink ptr is NULL. No Sp Cleanup");
    return FALSE;
  }

  phys_link_state = PS_PHYS_LINK_GET_STATE(phys_link_ptr);

  ldsn = ds707_pkt_get_last_data_sess_network();
  cdsn = ds707_pkt_get_current_data_sess_network();

  DATA_IS707_MSG5(MSG_LEGACY_HIGH, 
                  "ds707_sys_chg: Cleanup Trigger: %d, hybr_gw_operational: %d, "
                  "Phys_link_state: %d, LDSN: %d, CDSN: %d",
                  cleanup_trigger, hybr_gw_operational, phys_link_state, ldsn, cdsn);

  if ( PHYS_LINK_NULL       == phys_link_state || 
       PHYS_LINK_GOING_NULL == phys_link_state ||
       PHYS_LINK_COMING_UP  == phys_link_state
     )
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                  "ds707_sys_chg: Phys link state:NULL/GOING_NULL/COMING_UP "
                  "indicates no PPP session is up. No Special cleanup needed");
    return FALSE;
  }

  if (ldsn != SYS_SYS_MODE_CDMA && cdsn != SYS_SYS_MODE_CDMA)
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                    "ds707_sys_chg: Data Session not up on CDMA. "
                    "No Special cleanup needed");
    return FALSE;
  }

  sys_chg_sm_state = ds707_query_sys_chg_sm_state();

  switch (cleanup_trigger)
  {
    case DS707_SYS_CHG_LTE_ATTACH_INIT:
    {
      if (NULL == cmd_ptr)
      {
        DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                        "ds707_sys_chg: cmd_ptr is NULL. "
                        "No Special Cleanup on LTE attach init");
        return FALSE;
      }

      if (TRUE == hybr_gw_operational)
      {
        DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                        "ds707_sys_chg: hybr_gw_operational is TRUE. "
                        "No Special Cleanup on LTE attach init");
        return FALSE;
      }

      /*--------------------------------------------------------------- 
        It's possible that DS might already be in cleanup procedure and
        NAS sends one more get_pdn_conn_ind. In that case, just update
        the cmd cache and don't proceed with the attach process
      ---------------------------------------------------------------*/
      if (DS707_SYS_CHG_SM_1X_DEREG_IN_PROG == sys_chg_sm_state)
      {
        DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                        "ds707_sys_chg: Already in 1X_DEREG_IN_PROG state. "
                        "Update get_pdn_conn_ind cmd cache and return true");
        ds707_sys_chg_cache_get_pdn_conn_ind_cmd(cmd_ptr);
        return TRUE;
      }
      else if (DS707_SYS_CHG_SM_NULL != sys_chg_sm_state)
      {
        DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                        "ds707_sys_chg: Incorrect sys_chg_sm_state. "
                        "No Special Cleanup on LTE attach init");
        return FALSE;
      }
      

      /*--------------------------------------------------------------- 
        Send request to MMODE to start the PPP cleanup procedure
        (1X CP sends power down Dereg to notify NW that UE cleaned
        up the data session locally)
      ---------------------------------------------------------------*/
      if (!ds3g_msh_cleanup_nonepc_call())
      {
        DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                        "ds707_sys_chg: Cleanup request to CM Failed!");
        return FALSE;
      }

      /*--------------------------------------------------------------- 
        Cache the get_pdn_conn_ind command. If the cleanup procedure
        is rejected even before NAS is deactivated, DS has to continue
        with the LTE attach procedure. For that, cache the cmd here and
        replay the cmd if the cleanup aborts with an err code that
        indicates MMODE didn't even deactivate NAS
      ---------------------------------------------------------------*/
      ds707_sys_chg_cache_get_pdn_conn_ind_cmd(cmd_ptr);

      /*--------------------------------------------------------------- 
        Transition sys_chg_sm to 1X_DEREG_IN_PROG State
      ---------------------------------------------------------------*/
      ds707_sys_chg_sm_transition_state(DS707_SYS_CHG_SM_1X_DEREG_IN_PROG);

      /*--------------------------------------------------------------- 
        post abort to tear down the 1X PPP session
      ---------------------------------------------------------------*/
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                        "ds707_sys_chg: Abort PPP session on LTE attach init");
      event_info.curr_network = cdsn;
      event_info.tear_down_info.validity_mask |= 
                             PS_CLIENT_DATA_INFO_CALL_END_REASON_MASK;
      event_info.tear_down_info.data_info.call_end_reason = 
                                    PS_NET_DOWN_REASON_REDIR_OR_HANDOFF;
      ds707_cback_post_event(DS707_CBACK_ABORT_SESSION_EV, &event_info);

      ret_val = TRUE;

      break;
    }
    case DS707_SYS_CHG_NONEPC_TO_EPC:
    {
      /*--------------------------------------------------------------- 
        For NONSV case (where L is on main stack), we should have
        already aborted the ongoing session on LTE attach init.
        This should not have happened.
        Anyways return false here so the NONEPC session is cleaned up
      ---------------------------------------------------------------*/
      if ((FALSE == hybr_gw_operational) && 
          (SYS_SYS_MODE_LTE == ds707_pkt_mgr_get_current_sys_mode())
         )
      {
        DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                        "ds707_sys_chg: Non-SV Mode and current sys is LTE."
                        "No Special Cleanup on NONEPC_TO_EPC ev");
        return FALSE;
      }

      /*--------------------------------------------------------------- 
        Already sys chg SM is in the proces of special cleanup. Let it
        continue further. Return TRUE here
      ---------------------------------------------------------------*/
      if ( DS707_SYS_CHG_SM_1X_SP_CALL_IN_PROG == sys_chg_sm_state ||
           DS707_SYS_CHG_SM_1X_SP_CALL_CLEANUP == sys_chg_sm_state ||
           DS707_SYS_CHG_SM_WAIT_FOR_DORM      == sys_chg_sm_state
         )
      {
        DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                        "ds707_sys_chg: Sys chg SM already in some sp cleanup "
                        "state. Don't proceed with NONEPC_TO_EPC processing");
        return TRUE;
      }

      /*--------------------------------------------------------------- 
        sys chg SM should be in nonepc_to_epc state to continue further
        with sp cleanup. If not, return FALSE
      ---------------------------------------------------------------*/
      if (DS707_SYS_CHG_SM_NONEPC_TO_EPC != sys_chg_sm_state)
      {
        DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                        "ds707_sys_chg: Sys chg SM not in nonepc_to_epc state. "
                        "No Special Cleanup on NONEPC_TO_EPC ev");
        return FALSE;
      }

      /*--------------------------------------------------------------- 
        If Phys link is currently up on CDMA, no need of special
        cleanup. When DS cleans up the data session, it will send
        ORDQ=2 to nw when it brings down the traffic channel
      ---------------------------------------------------------------*/
      if (PHYS_LINK_UP == phys_link_state && SYS_SYS_MODE_CDMA == cdsn)
      {
        DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                        "ds707_sys_chg: Phys Link up on CDMA. "
                        "No Special Cleanup on NONEPC_TO_EPC ev");
        return FALSE;
      }

      /*---------------------------------------------------------------------- 
        If Phys Link is in going down state and the current data session is
        in 1X, transition sys chg sm to wait_for_dorm state
      -----------------------------------------------------------------------*/
      if (PHYS_LINK_GOING_DOWN == phys_link_state && SYS_SYS_MODE_CDMA == cdsn)
      {
        DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                       "ds707_sys_chg: Phys link in going down state "
                       "Special Cleanup needed on NONEPC_TO_EPC ev. "
                       "Transition to wait_for_dorm");
        ds707_sys_chg_sm_transition_state(DS707_SYS_CHG_SM_WAIT_FOR_DORM);
        return TRUE;
      }

      /*--------------------------------------------------------------------- 
        1. If Phys link is currently up on HDR but Last Data Session
           indicates CDMA, this is a 1X->eHRPD scenario.
       
           e.g: UE has 1X data session and phys link is dormant. On reorig
           Phys link connects on HDR and the session happens to be eHRPD.
           The CDSN is changed to HDR on call connect and then DSD issues
           sys chg to eHRPD. In this case, the data session is still on 1X
           and requires special cleanup
         
           To do the special cleanup, we need to bring down the HDR call
           and then issue the special call to 1X. Hence we first put the
           phys link to dormant.
        2. If Phys link is resuming from dormancy and the current data
           session is 1X, we need to first put it to dormancy and then
           trigger 1X special call
           e.g: UE has 1X data session and phys link is dormant. On reorig
           the call is yet to connect, but pref sys changed to a EPC system
           triggering this nonepc_to_epc event
      ----------------------------------------------------------------------*/
      if ((PHYS_LINK_UP == phys_link_state && SYS_SYS_MODE_HDR == cdsn
          && SYS_SYS_MODE_CDMA == ldsn) ||
          (PHYS_LINK_RESUMING == phys_link_state && SYS_SYS_MODE_CDMA == cdsn)
         )
      {
        phys_link_ret_val = ps_phys_link_down_cmd( phys_link_ptr,
                                                   &phys_link_err_num, NULL );

        if ((phys_link_ret_val < 0) && (phys_link_err_num != DS_EWOULDBLOCK))
        {
          DATA_IS707_MSG1(MSG_LEGACY_ERROR, 
                          "ds707_sys_chg: Cant tear down phys link - error %d. "
                          "No Special Cleanup on NONEPC_TO_EPC ev",
                          phys_link_err_num);
          return FALSE;
        }
        else if (phys_link_ret_val != 0)
        {
          /*--------------------------------------------------------------- 
            Phys Link Tear down in progress. Transition sys chg sm to
            wait_for_dorm state
          ---------------------------------------------------------------*/
          DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                         "ds707_sys_chg: Special Cleanup needed on "
                         "NONEPC_TO_EPC ev. Transition to wait_for_dorm");
          ds707_sys_chg_sm_transition_state(DS707_SYS_CHG_SM_WAIT_FOR_DORM);
          return TRUE;
        }
        /* Note: phys_link_ret_val == 0 means phys link is already down.
           Continue with special call bringup */
      }
  
      /*---------------------------------------------------------------------
        If we have come this far, it means we need to bring up the special
        service 1X call
      ---------------------------------------------------------------------*/
      phys_link_ret_val = ps_phys_link_up_cmd( phys_link_ptr, 
                                               &phys_link_err_num, NULL);
      if (phys_link_ret_val == 0)
      {
        DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                        "ds707_sys_chg: Phys Link already up. "
                        "No Special Cleanup on NONEPC_TO_EPC ev");
        return FALSE;
      }
      else if ((phys_link_ret_val < 0) && (phys_link_err_num != DS_EWOULDBLOCK))
      {
         DATA_IS707_MSG1(MSG_LEGACY_ERROR, 
                         "ds707_sys_chg: phys link bringup failed - error %d. "
                         "No Special Cleanup on NONEPC_TO_EPC ev",
                         phys_link_err_num);
         return FALSE;
      }
      else
      {
        /*--------------------------------------------------------------- 
          Phys Link bringup in progress. Transition sys chg sm to
          1X_Sp_call_in_prog state
        ---------------------------------------------------------------*/
        DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                       "ds707_sys_chg: Special Cleanup needed on NONEPC_TO_EPC "
                       "ev. Transition to 1X_Sp_Call_In_Prog state");
        ds707_sys_chg_sm_transition_state(DS707_SYS_CHG_SM_1X_SP_CALL_IN_PROG);
        return TRUE;
      }

      break;
    }
    default:
      break;
  }

  return ret_val;
} /* ds707_sys_chg_is_nonepc_sp_call_cleanup_needed */

/*===========================================================================
FUNCTION      DS707_SYS_CHG_EPC_TO_NONEPC_IND

DESCRIPTION   Triggered when EPC_TO_NONEPC event is posted 

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_sys_chg_epc_to_nonepc_ind
(
  void
)
{
  ds707_sys_chg_sm_e    sys_chg_sm_state = DS707_SYS_CHG_SM_NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_IS707_MSG0(MSG_LEGACY_MED, 
                    "ds707_sys_chg: EPC_TO_NONEPC IND");

  sys_chg_sm_state = ds707_query_sys_chg_sm_state();

  switch (sys_chg_sm_state)
  {
    case DS707_SYS_CHG_SM_NULL:
    {
      ds707_sys_chg_sm_transition_state(DS707_SYS_CHG_SM_EPC_TO_NONEPC);
      break;
    }
    default:
      DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
             "ds707_sys_chg: EPC_TO_NONEPC IND - sys chg sm not in NULL state");
      break;
  }
} /* ds707_sys_chg_epc_to_nonepc_ind */

/*===========================================================================
FUNCTION      DS707_SYS_CHG_NONEPC_TO_EPC_IND

DESCRIPTION   Triggered when NONEPC_TO_EPC event is posted 

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_sys_chg_nonepc_to_epc_ind
(
  void
)
{
  ds707_sys_chg_sm_e    sys_chg_sm_state = DS707_SYS_CHG_SM_NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_IS707_MSG0(MSG_LEGACY_MED, 
                    "ds707_sys_chg: NONEPC_TO_EPC IND");

  sys_chg_sm_state = ds707_query_sys_chg_sm_state();

  switch (sys_chg_sm_state)
  {
    case DS707_SYS_CHG_SM_NULL:
    {
      ds707_sys_chg_sm_transition_state(DS707_SYS_CHG_SM_NONEPC_TO_EPC);
      break;
    }
    default:
      DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
             "ds707_sys_chg: NONEPC_TO_EPC IND - sys chg sm not in NULL state");
      break;
  }
} /* ds707_sys_chg_nonepc_to_epc_ind */

/*===========================================================================
FUNCTION      DS707_SYS_CHG_ALL_PDN_DOWN_IND

DESCRIPTION   Triggered when all PDNs are cleaned up 

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_sys_chg_all_pdn_down_ind
(
  void
)
{
  ds707_sys_chg_sm_e    sys_chg_sm_state = DS707_SYS_CHG_SM_NULL;
  ps_phys_link_type     *phys_link_ptr   = NULL;
  phys_link_state_type  phys_link_state  = PHYS_LINK_STATE_INVALID;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  sys_chg_sm_state = ds707_query_sys_chg_sm_state();

  switch (sys_chg_sm_state)
  {
    case DS707_SYS_CHG_SM_NONEPC_TO_EPC:
    case DS707_SYS_CHG_SM_WAIT_FOR_DORM:
    case DS707_SYS_CHG_SM_1X_SP_CALL_IN_PROG:
    case DS707_SYS_CHG_SM_1X_SP_CALL_CLEANUP:
    {
      DATA_IS707_MSG0(MSG_LEGACY_MED, 
                    "ds707_sys_chg: All PDN Down IND when Sys chg SM in "
                    "nonepc_to_epc/special cleanup state. Move to NULL");
      ds707_sys_chg_sm_transition_state(DS707_SYS_CHG_SM_NULL);

      /*--------------------------------------------------------------- 
        The below fix is needed because in NONEPC_TO_EPC cleanup,
        calls were initiated in 1x/HRPD, device manager is invalid and
        cannot move sys info to NO_SYSTEM. After that if there is no
        active data call on eHRPD and move back to 1x/HRPD, sys info
        is stuck in EPC and all subsequent call request in 1x/HRPD
        will fail, until a successful call in eHRPD.
      ---------------------------------------------------------------*/
      ds707_sys_chg_hdlr_chg_sys_info(DS707_SYS_INFO_NO_SYSTEM);
      break; 
    }

    case DS707_SYS_CHG_SM_EPC_TO_NONEPC:
#ifdef FEATURE_DATA_WLAN_MAPCON
    case DS707_SYS_CHG_SM_NULL: 
    /* Earlier sys_chg_sm moved to EPC_NONEPC,
       but with iWLAN, we dont move it, Hence take all the 
       transistions to NULL STATE  that occur previously
       It is okay to transition from NULL to NULL 
       as we just enable the phys link which was disabled earlier 
     */
#endif /* FEATURE_DATA_WLAN_MAPCON */
    {
      /*--------------------------------------------------------------- 
        If all PDNs are down and phys link is null, then transition from
        EPC_TO_NONEPC->NULL. This should not happen since dev mgr needs
        to cleanup after all PDNs go down and only then phys link goes
        NULL. This is just for fail-safe purpose
      ---------------------------------------------------------------*/
      phys_link_state = PS_PHYS_LINK_GET_STATE(phys_link_ptr);

      if (PHYS_LINK_NULL == phys_link_state)
      {
        DATA_IS707_MSG0(MSG_LEGACY_MED, 
                    "ds707_sys_chg: All PDN Down IND, Phys link already in "
                    "NULL state. Transition from EPC_TO_NONEPC->NULL");
        ds707_sys_chg_sm_transition_state(DS707_SYS_CHG_SM_NULL);
      }
      break;
    }
 
    default:
      break;
  }
} /* ds707_sys_chg_all_pdn_down_ind */

/*===========================================================================
FUNCTION      DS707_SYS_CHG_IFACE_RECONFIG_IND

DESCRIPTION   Triggered when EPC issues iface reconfig ioctl due to SRAT 
              REcovery 

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_sys_chg_iface_reconfig_ind
(
  void
)
{
  ds707_sys_chg_sm_e    sys_chg_sm_state = DS707_SYS_CHG_SM_NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  sys_chg_sm_state = ds707_query_sys_chg_sm_state();

  switch (sys_chg_sm_state)
  {
    case DS707_SYS_CHG_SM_EPC_TO_NONEPC:
#ifdef FEATURE_DATA_WLAN_MAPCON
    case DS707_SYS_CHG_SM_NULL:
#endif /* FEATURE_DATA_WLAN_MAPCON  */
    {
      /*-------------------------------------------------------------------- 
         This is to handle a MAPCON type scenario where the UE transitions
         from eHRPD Only -> 1X+WLAN -> eHRPD Only rapdily. During eHRPD Only
         -> 1X+WLAN, MH will wait for EPC to clean up/Handoff (to wlan) the
         calls that are up on eHRPD. Sys Chg Hdlr SM will remain in
         EPC_TO_NONEPC until all PDNs are cleaned up (either due to HO
         success/failure). However, in this scenario UE transitions back to
         eHRPD before the HO to WLAN is attempted and EPC will trigger iface
         reconfig to recover the PDNs on eHRPD. To handle this case, the
         Sys Chg Hdlr SM should move back to NULL state and allow the iface
         reconfig (NCP resync) to continue    
      --------------------------------------------------------------------*/ 
      DATA_IS707_MSG0(MSG_LEGACY_MED, 
                    "ds707_sys_chg: Iface Reconfig IND. Transition from "
                    "EPC_TO_NONEPC->NULL");
      ds707_sys_chg_hdlr_chg_sys_info(DS707_SYS_INFO_EPC_SYSTEM);
      ds707_sys_chg_sm_transition_state(DS707_SYS_CHG_SM_NULL);
      break;
    }

    default: 
      break;
  }
} /* ds707_sys_chg_iface_reconfig_ind */

/*===========================================================================
FUNCTION      DS707_SYS_CHG_PHYS_LINK_UP_IND

DESCRIPTION   Triggered when Phys link Up Ind is recvd

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_sys_chg_phys_link_up_ind
(
  void
)
{
  ds707_sys_chg_sm_e    sys_chg_sm_state = DS707_SYS_CHG_SM_NULL;
  ds707_cback_event_info_type     event_info;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  memset(&event_info,0,sizeof(event_info));
  sys_chg_sm_state = ds707_query_sys_chg_sm_state();

  switch (sys_chg_sm_state)
  {
    case DS707_SYS_CHG_SM_1X_SP_CALL_IN_PROG:
    {


      /*--------------------------------------------------------------- 
        post abort to tear down the 1X PPP session
      ---------------------------------------------------------------*/
 
      event_info.curr_network = ds707_pkt_get_current_data_sess_network();
       DATA_IS707_MSG0(MSG_LEGACY_MED, 
                    "ds707_sys_chg: Phys Link UP IND" 
                     "Abort PPP session on 1X_SP_CALL_IN_PROG state "
                     " set call end reason to" 
                     " PS_NET_DOWN_REASON_REDIR_OR_HANDOFF");

      event_info.tear_down_info.validity_mask |= 
                                   PS_CLIENT_DATA_INFO_CALL_END_REASON_MASK;
      event_info.tear_down_info.data_info.call_end_reason = 
                                    PS_NET_DOWN_REASON_REDIR_OR_HANDOFF;
      ds707_cback_post_event(DS707_CBACK_ABORT_SESSION_EV, &event_info);

      ds707_sys_chg_sm_transition_state(DS707_SYS_CHG_SM_1X_SP_CALL_CLEANUP);
      
      break;
    }
    default:
     break;
  }

} /* ds707_sys_chg_phys_link_up_ind */

/*===========================================================================
FUNCTION      DS707_SYS_CHG_PHYS_LINK_DOWN_IND

DESCRIPTION   Triggered when Phys link Down Ind is recvd

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_sys_chg_phys_link_down_ind
(
  void
)
{
  ds707_sys_chg_sm_e    sys_chg_sm_state = DS707_SYS_CHG_SM_NULL;
  ps_phys_link_type     *phys_link_ptr   = NULL;
  int16                 phys_link_err_num;
  int                   phys_link_ret_val = -1;
  ds707_cback_event_info_type     event_info;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset(&event_info,0,sizeof(event_info));
  sys_chg_sm_state = ds707_query_sys_chg_sm_state();

  switch (sys_chg_sm_state)
  {
    case DS707_SYS_CHG_SM_1X_SP_CALL_IN_PROG:
    {
      /*---------------------------------------------------------------------
        We were waiting to receive UP ind in this state, but CM
        couldn't connect the call.
        Post abort to tear down the 1X PPP session
      ----------------------------------------------------------------------*/


      event_info.curr_network = ds707_pkt_get_current_data_sess_network();
       DATA_IS707_MSG0(MSG_LEGACY_MED, 
               "ds707_sys_chg: Phys Link Down IND in 1X_SP_CALL_IN_PROG state" 
               "-  Abort PPP session and "
               "set call end reason to PS_NET_DOWN_REASON_REDIR_OR_HANDOFF");
      event_info.tear_down_info.validity_mask |= 
                                  PS_CLIENT_DATA_INFO_CALL_END_REASON_MASK;
      event_info.tear_down_info.data_info.call_end_reason = 
                                    PS_NET_DOWN_REASON_REDIR_OR_HANDOFF;
      ds707_cback_post_event(DS707_CBACK_ABORT_SESSION_EV, &event_info);

      ds707_sys_chg_sm_transition_state(DS707_SYS_CHG_SM_1X_SP_CALL_CLEANUP);
      
      break;
    }

    case DS707_SYS_CHG_SM_WAIT_FOR_DORM:
    {
      /*--------------------------------------------------------------------- 
        We are waiting to receive Down(dorm) ind in this state and we got it
        Start special call bring up
      ----------------------------------------------------------------------*/
      phys_link_ptr = ds707_pkt_get_pri_phys_link_ptr();
      phys_link_ret_val = ps_phys_link_up_cmd( phys_link_ptr, 
                                               &phys_link_err_num, NULL);
      if (phys_link_ret_val == 0)
      {
        DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                        "ds707_sys_chg: Attempting special call bring up but "
                        "Phys Link already up - Assert! ");
        /* This is not possible since we just got phys link down ind */
        ASSERT(FALSE);
      }
      else if ((phys_link_ret_val < 0) && (phys_link_err_num != DS_EWOULDBLOCK))
      {

        event_info.curr_network = ds707_pkt_get_current_data_sess_network();
        DATA_IS707_MSG1(MSG_LEGACY_ERROR, 
                 "ds707_sys_chg: Special call bringup failed - "
                 "error %d. Abort PPP sessio"
                 "- set call end reason to PS_NET_DOWN_REASON_REDIR_OR_HANDOFF",
                 phys_link_err_num
                 );     
	event_info.tear_down_info.validity_mask |= 
                               PS_CLIENT_DATA_INFO_CALL_END_REASON_MASK;
        event_info.tear_down_info.data_info.call_end_reason = 
                                      PS_NET_DOWN_REASON_REDIR_OR_HANDOFF;
        ds707_cback_post_event(DS707_CBACK_ABORT_SESSION_EV, &event_info);

        ds707_sys_chg_sm_transition_state(DS707_SYS_CHG_SM_1X_SP_CALL_CLEANUP);
      }
      else
      {
        /*--------------------------------------------------------------- 
          Phys Link bringup in progress. Transition sys chg sm to
          1X_Sp_call_in_prog state
        ---------------------------------------------------------------*/
        DATA_IS707_MSG0(MSG_LEGACY_MED, 
                     "ds707_sys_chg: Special call bring up in prog. Transition "
                     "from Wait_for_dorm->1X_Sp_Call_In_Prog state");
        ds707_sys_chg_sm_transition_state(DS707_SYS_CHG_SM_1X_SP_CALL_IN_PROG);
      }
      break;
    }

    default:
     break;
  }

} /* ds707_sys_chg_phys_link_down_ind */

/*===========================================================================
FUNCTION      DS707_SYS_CHG_PHYS_LINK_NULL_IND

DESCRIPTION   Triggered when Phys link Null Ind is recvd

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_sys_chg_phys_link_null_ind
(
  void
)
{
  ds707_sys_chg_sm_e    sys_chg_sm_state = DS707_SYS_CHG_SM_NULL;
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sys_chg_sm_state = ds707_query_sys_chg_sm_state();

  switch (sys_chg_sm_state)
  {
    case DS707_SYS_CHG_SM_EPC_TO_NONEPC:
#ifdef FEATURE_DATA_WLAN_MAPCON
    case DS707_SYS_CHG_SM_NULL:
#endif /* FEATURE_DATA_WLAN_MAPCON */
    {
      DATA_IS707_MSG0(MSG_LEGACY_MED, 
                    "ds707_sys_chg: Phys NULL Ind when Sys chg SM in "
                    "epc_to_nonepc state. Move to NULL");
      ds707_sys_chg_sm_transition_state(DS707_SYS_CHG_SM_NULL);
      break;
    }


    case DS707_SYS_CHG_SM_NONEPC_TO_EPC:
    {
      if (ds707_pkt_mgr_all_ifaces_down())
      {
        DATA_IS707_MSG0(MSG_LEGACY_MED, 
                   "ds707_sys_chg: Phys NULL Ind recvd and all ifaces are down."
                   "Transition Sys chg SM from nonepc_to_epc->Null");
        ds707_sys_chg_sm_transition_state(DS707_SYS_CHG_SM_NULL);
      }
      break;
    }

    case DS707_SYS_CHG_SM_1X_SP_CALL_IN_PROG:
    {
      /*--------------------------------------------------------------------- 
        We are waiting to get phys link up in this state. However Phys link
        went NULL. Move to 1x_sp_call_cleanup state. When the ifaces go down
        we'll recv all pdn down ind and move the state to NULL
      ---------------------------------------------------------------------*/
      DATA_IS707_MSG0(MSG_LEGACY_MED, 
                    "ds707_sys_chg: Phys NULL Ind when Sys chg SM in "
                    "1x_sp_call_in_prog state. Move to 1x_sp_call_cleanup");
      ds707_sys_chg_sm_transition_state(DS707_SYS_CHG_SM_1X_SP_CALL_CLEANUP);
      break;
    }

    default:
     break;
  }

} /* ds707_sys_chg_phys_link_null_ind */

/*===========================================================================
FUNCTION      DS707_SYS_CHG_CACHE_GET_PDN_CONN_IND_CMD

DESCRIPTION   Caches the get_pdn_conn_ind_cmd to use it later if the cleanup 
              is rejected by MMODE and NAS is not deactivated 

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_sys_chg_cache_get_pdn_conn_ind_cmd(ds_cmd_type *cmd_ptr)
{
#if 0 /* Unused */
  if (NULL == cmd_ptr)
  {
    DATA_ERR_FATAL("ds707_sys_chg: get_pdn_conn_ind_cmd is NULL. ASSERT");
    return;
  }

  if (NULL == ds707_get_pdn_conn_ind_cmd_ptr)
  {
    if( (ds707_get_pdn_conn_ind_cmd_ptr = ds_get_cmd_buf()) == NULL )
    {
      DATA_ERR_FATAL("ds707_sys_chg: can't get cmd buf from DS task. ASSERT");
      return;
    }
  }

  memset(ds707_get_pdn_conn_ind_cmd_ptr, 0, sizeof(ds_cmd_type));

  /*--------------------------------------------------------------------- 
    Store the contents of the get_pdn_conn_ind cmd into the global cmd
    cache we maintain here. This cache will be released when we
    get the CM_AC_STATE_END CB from CM.
  ---------------------------------------------------------------------*/
  *ds707_get_pdn_conn_ind_cmd_ptr = *cmd_ptr;

  DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                  "ds707_sys_chg: Cached Get PDN Conn Ind Cmd");

#endif
} /* ds707_sys_chg_cache_get_pdn_conn_ind_cmd */

/*===========================================================================
FUNCTION      DS707_SYS_CHG_RELEASE_GET_PDN_CONN_IND_CMD_CACHE

DESCRIPTION   Clears the cmd cache stored 

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_sys_chg_release_get_pdn_conn_ind_cmd_cache(void)
{
#if 0 /* Unused */
  if (NULL != ds707_get_pdn_conn_ind_cmd_ptr)
  {
    ds_release_cmd_buf(&ds707_get_pdn_conn_ind_cmd_ptr);
    ds707_get_pdn_conn_ind_cmd_ptr = NULL;

    DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                    "ds707_sys_chg: Released Get PDN Conn Ind Cmd cache");
  }
#endif
} /* ds707_sys_chg_release_get_pdn_conn_ind_cmd_cache */

#if 0
/* Commenting out below function since it's not being used currently. It'll be
   enabled when there is a need to replay get_pdn_conn_ind cmd on 1X PPP
   Cleanup Failure */
/*===========================================================================
FUNCTION      DS707_SYS_CHG_REPLAY_GET_PDN_CONN_IND_CMD

DESCRIPTION   Replays the get_pdn_conn_ind_cmd to ds task 

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_sys_chg_replay_get_pdn_conn_ind_cmd(void)
{
  ds_cmd_type   *cmd_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == ds707_get_pdn_conn_ind_cmd_ptr)
  {
    DATA_ERR_FATAL("ds707_sys_chg: ds707_get_pdn_conn_ind_cmd_ptr is NULL when attempting to replay. ASSERT");
    return;
  }

  /*----------------------------------------------------------------------
  Get a command buf
  ----------------------------------------------------------------------*/
  if( (cmd_ptr = ds_get_cmd_buf()) == NULL )
  {
    /*------------------------------------------------------------------
      This condition can not be recovered
    -------------------------------------------------------------------*/
    DATA_ERR_FATAL("ds707_sys_chg: can't get cmd buf from DS task");
    return;
  }

  /*--------------------------------------------------------------------- 
    Store the contents of the get_pdn_conn_ind cmd cache into the cmd_buf
  ---------------------------------------------------------------------*/
  *cmd_ptr = *ds707_get_pdn_conn_ind_cmd_ptr;

  /*--------------------------------------------------------------------- 
    Replay the get_pdn_conn_ind cmd
  ---------------------------------------------------------------------*/
  ds_put_cmd(cmd_ptr);

  /*--------------------------------------------------------------------- 
    Release the global get_pdn_conn_ind cmd cache
  ---------------------------------------------------------------------*/
  ds707_sys_chg_release_get_pdn_conn_ind_cmd_cache();

} /* ds707_sys_chg_replay_get_pdn_conn_ind_cmd */
#endif 
#endif /*FEATURE_EHRPD*/

/*===========================================================================
FUNCTION      DS707_SYS_CHG_SET_NOTIFY_DSD

DESCRIPTION   Sets the Notify_DSD flag. Set when DSD reports pref sys chg and 
              MH needs to do some processing/cleanup before DSD can report the
              change to external clients

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_sys_chg_set_notify_dsd
(
  void
)
{
  DATA_IS707_MSG0(MSG_LEGACY_MED,
                  "ds707_sys_chg_sm: Setting Notify DSD flag");
  ds707_notify_dsd_after_pref_sys_update = TRUE;
} /* ds707_sys_chg_set_notify_dsd */

/*===========================================================================
FUNCTION      DS707_SYS_CHG_CLEAR_NOTIFY_DSD

DESCRIPTION   Clears the Notify_DSD flag

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_sys_chg_clear_notify_dsd
(
  void
)
{
  DATA_IS707_MSG0(MSG_LEGACY_MED,
                  "ds707_sys_chg_sm: Clearing Notify DSD flag");
  ds707_notify_dsd_after_pref_sys_update = FALSE;
} /* ds707_sys_chg_clear_notify_dsd */

/*===========================================================================
FUNCTION      DS707_SYS_CHG_IS_DSD_NOTIFICATION_PENDING

DESCRIPTION   returns Notify_DSD flag value

DEPENDENCIES  None

RETURN VALUE  TRUE - If notification is pending 
              FALSE - Otherwise 

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_sys_chg_is_dsd_notification_pending
(
  void
)
{
  DATA_IS707_MSG1(MSG_LEGACY_MED,
                  "ds707_sys_chg_sm: Notify DSD: %d", 
                  ds707_notify_dsd_after_pref_sys_update);
  return ds707_notify_dsd_after_pref_sys_update;
} /* ds707_sys_chg_is_dsd_notification_pending */

/*===========================================================================
FUNCTION      DS707_SYS_CHG_SEND_DSD_NOTIFICATION

DESCRIPTION   Sends notification to DSD confirmation MH has completed 
              pref sys ind processing

DEPENDENCIES  None

RETURN VALUE  None 

SIDE EFFECTS  None
===========================================================================*/
void ds707_sys_chg_send_dsd_notification
(
  void
)
{
  sys_sys_mode_e_type current_sys;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  current_sys = ds707_pkt_mgr_get_current_sys_mode();

  if (ds707_notify_dsd_after_pref_sys_update)
  {
    DATA_IS707_MSG0(MSG_LEGACY_MED,
                    "ds707_sys_chg_sm: Notifying DSD and clearing "
                    "notify_dsd flag");
    /* DSD will ignore this notification if the pending radio doesn't match
       with the radio sent here*/
    //NOTE ruishi check with 3GPP2 on how to pass sub id
    ds_dsd_ext_pref_system_ind_completed(current_sys, 
                        ds3gsubsmgr_subs_id_cm_to_ds3g(ds3g_get_ps_subs_id()));
    ds707_notify_dsd_after_pref_sys_update = FALSE;
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_MED,
                  "ds707_sys_chg_sm: Notify DSD flag not set. Not Sending "
                  "notification to DSD");
  }

} /* ds707_sys_chg_is_dsd_notification_pending */


/*===========================================================================
FUNCTION      DS707_SYS_CHG_PROCESS_CLEANUP_PPP_STATE

DESCRIPTION   Called when CM notifies the current state of 
              CM_AC_CMD_CLEAR_1XPPP cmd processing

DEPENDENCIES  None

RETURN VALUE  None 

SIDE EFFECTS  None
===========================================================================*/
void ds707_sys_chg_process_cleanup_ppp_state
(
  cm_ac_state_e_type      state,        /* CLEAR_1XPPP command state*/
  cm_ac_end_reason_e_type end_reason    /* CLEAR_1XPPP state end reason */
)
{
  DATA_IS707_MSG2(MSG_LEGACY_MED,
                 "ds707_sys_chg_sm: PPP Cleanup State:%d , end_reason: %d ",
                 state, end_reason);

  switch (state)
  {
    case CM_AC_STATE_START:
    {
      /*--------------------------------------------------------------------- 
        Currently there is no action to take on Cleanup start
      ---------------------------------------------------------------------*/
       DATA_IS707_MSG0(MSG_LEGACY_MED,
                  "ds707_sys_chg_sm: PPP Cleanup started");
       break;
    }

    case CM_AC_STATE_END:
    {
      /*--------------------------------------------------------------------- 
        Immaterial of the end reason (success/fail), we transition the sys
        chg SM to null state if it was in 1X_DEREG_IN_PROG
      ---------------------------------------------------------------------*/
#ifdef FEATURE_EHRPD
      if (DS707_SYS_CHG_SM_1X_DEREG_IN_PROG == ds707_query_sys_chg_sm_state())
      {
        DATA_IS707_MSG0(MSG_LEGACY_MED,
                  "ds707_sys_chg_sm: PPP Cleanup end rcvd in 1X_DEREG_IN_PROG "
                  "state. Transition to NULL");

        /* Currently there is no end_reason that MMODE provides where it
           rejects the cleanup req and doesn't deactivate the NAS stack -
           Revisit when MMODE adds a valid end_reason */
#if 0
        if (CM_AC_END_REASON_FAIL_INTERNAL == end_reason)
        {
          DATA_IS707_MSG1(MSG_LEGACY_MED,
                          "ds707_sys_chg_sm: Recvd end_reason: %d, "
                          "replay get_pdn_conn_ind cmd", end_reason);
          ds707_sys_chg_replay_get_pdn_conn_ind_cmd(); 
        }
        else
#endif
        {
          /*------------------------------------------------------------------- 
            Release the global get_pdn_conn_ind cmd cache
          -------------------------------------------------------------------*/
          ds707_sys_chg_release_get_pdn_conn_ind_cmd_cache();
        }

        ds707_sys_chg_sm_transition_state(DS707_SYS_CHG_SM_NULL);
      }
      else
      {
        DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                  "ds707_sys_chg_sm: PPP Cleanup end rcvd when "
                  "not in 1X_DEREG_IN_PROG state - Ignore");
      }
#endif /* FEATURE_EHRPD */

      break;
    }

    default:
      DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                  "ds707_sys_chg_sm: UnKnown PPP Cleanup state");
      break;
  }
} /* ds707_sys_chg_process_cleanup_ppp_state */

/*=========================================================================== 
 
FUNCTION       DS707_SYS_CHG_HDLR_READ_NONEPC_CALL_CLEANUP
 
DESCRIPTION    This function is used to read the EFS items for 
               NONEPC call cleanup 
   
DEPENDENCIES   None.
 
RETURN VALUE   None.
 
SIDE EFFECTS   None.
===========================================================================*/ 

void ds707_sys_chg_hdlr_read_nonepc_call_cleanup()
{
  boolean cleanup_nonepc_data_session  = FALSE;
  nv_stat_enum_type    read_status     = NV_DONE_S;
  /*-----------------------------------------------------------------------
    Read EFS NV for NONEPC Call Cleanup
  -----------------------------------------------------------------------*/
  read_status = ds3gcfgmgr_read_efs_nv_ex(CLEANUP_NONEPC_CALL_EFS_PATH, 
                               &cleanup_nonepc_data_session, 
                               sizeof(boolean), ds707_curr_subs_id());
  if(read_status != NV_DONE_S)
  {
    DATA_IS707_MSG0( MSG_LEGACY_MED, 
              "ds707_sys_chg_hdlr_init: cleanup_nonepc_data_session EFS read "
              "failed.Setting default to FALSE" );
    ds707_cleanup_nonepc_data_session = FALSE;
  }
  else
  {
    ds707_cleanup_nonepc_data_session = cleanup_nonepc_data_session;
  }

  DATA_IS707_MSG1( MSG_LEGACY_MED, 
              "ds707_sys_chg_hdlr_init: cleanup_nonepc_data_session NV = %d",
               ds707_cleanup_nonepc_data_session);

  return;
}

/*===========================================================================
FUNCTION      DS707_SYS_CHG_SR_PROCESS_NV_REFRESH_IND

DESCRIPTION  Will reset Silent redial state machine and clean 
             up the failure count and total failure count. 

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_sys_chg_sr_process_nv_refresh_ind
(
  void
)
{
  DATA_IS707_MSG0(MSG_LEGACY_MED,
                    "In ds707_sys_chg_sr_process_nv_refresh_ind");

  /*Clear the new call attempt timer if it is started*/
  (void) rex_clr_timer(&(ds_silent_redial_info.new_call_attempt_timer));

  /*------------------------------------------------------------------------- 
    Reset the failure count and total failure count.
    ------------------------------------------------------------------------*/
  ds_silent_redial_info.failure_count = 0;
  ds_silent_redial_info.failure_total_count = 0;
}

#ifdef TEST_FRAMEWORK
#error code not present
#endif

/*===========================================================================

                      D S 7 0 7 _ P K T _ M G R _ H D R. C
                      
GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2004-2013 by Qualcomm Technologies Incorporated.  All Rights Reserved.  

===========================================================================*/

/*===========================================================================
                           EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_pkt_mgr.c_v   1.45   25 Feb 2003 14:25:02   ajithp  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_pkt_mgr_hdr.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/25/13    sc     Added changes to not bringup the phys link on IDM change
                   if special cleanup is in progress
06/03/13    sd     ATCoP decoupling changes
11/18/11    vpk    HDR SU API cleanup
03/15/11    ls     Pass correct old bearer tech info when IDM change happens
03/15/11    mg     Global variable cleanup
10/13/10    op     Migrated to MSG 2.0 macros
04/16/10    ls     Fix the featurization issues
09/29/09    sn     Fixed issue of wrong bearer tech info being informed to APP.
02/15/09    ls     Initialize epzid in ds3g_pkt_call_info_type
07/28/08    mga    Added new event in ds707_pkti_idm_changed_cmd_hdlr and 
                   ds707_hdr_ln_update_status_cmd_hdlr
07/25/08    psng   Fixed issue in handing over to Evdo when in 1x RTT 
                   dormancy.
04/30/08    psng   Fix Extra Data Origination issue after Handoff from 
                   EVDO to 1x
04/02/08    hs     Made changes in places where CDSN was updated directly.
05/25/07  squ/mga  Added new functions ds707_ln_update_status_cmd, 
                   ds707_hdr_ln_update_status_cmd_hdlr 
                   Added case DS_707_HDR_LN_STATUS_CMD in 
                   ds707_pkt_hdr_process_cmd
                   Included cmxll.h file 
02/09/07    gr/sq  Changes to notify app about BEARER_TECHNOLOGY upon IDM
                   change notification for all cases (dormant/active states)
05/18/06    gr     Fix to send DRS=0 orig for IDM change after PHYS_LINK
                   goes down (if IDM Change was received before CALL_END)
11/11/05    vrk/gr Avoiding ppp resync on failed ALN on MIP
10/17/05    as/kvd  Added bearer technology reporting to PS on IDM change
09/10/05    gr     Merged changes to lock out GPS in the middle of a PPP 
                   resync during a handup from 1x to DO
04/26/05    gr     Added inc_call flag to the ds3g_pkt_call_info_type 
12/02/04    gr     Fixed handling of ALN command for scenarios where
                   the packet data session is not really dormant
11/30/04    kvd    Added some new functions for CAM module.
07/26/04    ifk    Added protocol to ppp_resync()
04/24/04    ak     Lint fixes.
04/11/04    gr     Set the value of force_cdma_only to FALSE for all
                   dormancy re-origination scenarios
03/22/04    ak     Call okay_to_orig_from_dorm() when ALN or IDM change.
01/05/04    ak     First revision.  Copied IOCTL over to this file.

===========================================================================*/


/*===========================================================================
                          INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "ds707_sec_pkt_mgr_util.h"
#include "data_msg.h"

#ifdef FEATURE_DATA_IS707
#include "ds707_pkt_mgr_hdlr.h"
  #ifdef FEATURE_HDR


    #define FEATURE_DSM_WM_CB

    #include "ds707_cback.h"
    #include "ds707_drs.h"
    #include "ds707_pkt_mgr.h"
    #include "ds707_pkt_mgri.h"
    #include "dsat707extctab.h"
    #include "ds707_event_defs.h"

    #include "ds3gmgrint.h"

    #ifdef FEATURE_HDR
      #include "hdrrlp.h"
      #include "hdrpacdsif.h"
      #include "hdrpac.h"
      #include "hdrcom_api.h"
      #include "hdrds.h"
    #endif /* FEATURE_HDR */

      #include "hdrpacdsif.h"
    
#include "ds707_extif.h"
#include "data_err_suppr.h" 
#ifdef FEATURE_EHRPD
#include "ds707_sys_chg_hdlr.h"
#endif
#ifdef FEATURE_EPC_HANDOFF
#include "ds_epc_hdlr.h"
#endif
/*===========================================================================
                           FORWARD DECLARATIONS
===========================================================================*/
/*===========================================================================
                            VARIABLES
===========================================================================*/

    #ifdef FEATURE_DS_CAM
/*---------------------------------------------------------------------------
 Global variables used by CAM module.
---------------------------------------------------------------------------*/
LOCAL boolean             ds707_pkti_1x_colocated_with_evdo=FALSE;
    #endif /* FEATURE_DS_CAM */

/*---------------------------------------------------------------------------
 This flag is set to TRUE if the IDM re-orig failed because the PHYS_LINK 
 is still not cleaned up when an IDM change has been received from Call Mgr

 The new network passed in by the IDM changed handler is also saved to be
 used during the re-orig triggered by phys link down event
---------------------------------------------------------------------------*/
LOCAL boolean ds707_pkti_idm_orig_after_phys_link_down = FALSE;
LOCAL sys_sys_mode_e_type ds707_pkti_saved_idm_for_reorig = SYS_SYS_MODE_NO_SRV;

/*===========================================================================
                       INTERNAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
FUNCTION      DS707_PKTI_IDM_CHANGED_CMD_HDLR

DESCRIPTION   Called when IDM changes and we need to send an orig with DRS=0
              (or DRS=1  if ppp_resync needed on handoff)

              NOTE: This function is only applicable for HDR targets. It does
              nothing for 1X only targets.

DEPENDENCIES  runs in DS TASK context.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL void ds707_pkti_idm_changed_cmd_hdlr
(
  sys_sys_mode_e_type new_network
)
{
  ps_phys_link_type        *phys_link_ptr;
  ds3g_pkt_call_info_type  pkt_info;
  int16                     err_num;
  int                       ret_val;
  boolean                  *ppp_resync_ptr;
  ps_iface_ioctl_bearer_tech_changed_type  bearer_tech_change;
#ifdef FEATURE_DATA_WLAN_MAPCON
  ds_sys_system_status_info_type bearer_tech_ex;
#endif
  ds707_cback_event_info_type   event_info;

  ds707_pkt_idm_change_event_type   ds707_pkt_idm_change_event;
            
  boolean call_clean_up = FALSE;   /* to support ehrpd - 1x/hrpd handoff */
            
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DATA_IS707_MSG1(MSG_LEGACY_HIGH, "IDM Change %d ", new_network);

  phys_link_ptr = ds707_pkt_get_pri_phys_link_ptr();

  /*-------------------------------------------------------------------
    We have only one packet instance
    TODO: #define 0 --> DEF PKT INSTANCE
  -------------------------------------------------------------------*/
  event_info.pkt_inst    = 0;
  event_info.new_network = new_network;
  event_info.curr_network = ds707_pkt_get_last_data_sess_network();

  ds707_cback_post_event(DS707_CBACK_NEW_NET_EV,&event_info);

  /*-------------------------------------------------------------------
    Save the new network information to be used in the IDM re-orig 
    handler in case the origination attempt fails because the 
    PHYS LINK is not in DOWN state yet (dormancy)
  -------------------------------------------------------------------*/
  ds707_pkti_saved_idm_for_reorig = new_network;

  if ((ds707_pkt_get_last_data_sess_network() == SYS_SYS_MODE_NO_SRV) ||
      (new_network == SYS_SYS_MODE_NO_SRV))
  {
    if (new_network == SYS_SYS_MODE_NO_SRV) 
    {
      /* Phone has lost signal
       */
      ds707_set_has_phone_acquired_signal(FALSE);
    }
    else
    {
      bearer_tech_change.old_bearer_tech.current_network 
                = PS_IFACE_NETWORK_CDMA;
      bearer_tech_change.old_bearer_tech.data.cdma_type.rat_mask 
                = PS_IFACE_CDMA_NULL_BEARER;

      bearer_tech_change.new_bearer_tech.current_network 
                = PS_IFACE_NETWORK_CDMA;
#ifdef FEATURE_HDR
      if (new_network == SYS_SYS_MODE_HDR)
      {
        /* Phone has acquired signal
         */
        ds707_set_has_phone_acquired_signal(TRUE);
        ds707_pkt_mgr_set_current_sys_mode(new_network);
      }
      else
#endif /* FEATURE_HDR*/
      if (new_network == SYS_SYS_MODE_CDMA) 
      {
        /* Phone has acquired signal
         */
        ds707_set_has_phone_acquired_signal(TRUE);
        ds707_pkt_mgr_set_current_sys_mode(new_network);
      }
      else
      {
        DATA_IS707_MSG0( MSG_LEGACY_ERROR,
                         "new_network is neither 1x nor HDR");
        return;
      }
    }
    
    ds707_pkt_bearer_tech_changed_notification();

    /*-------------------------------------------------------------------
    If the current data session network is SYS_SYS_MODE_NO_SRV, or 
    new network is SYS_SYS_MODE_NO_SRV then, return. 
    We have delayed notifying to DSD since we posted IDM. So send the 
    notification now 
    -------------------------------------------------------------------*/
    if (ds707_sys_chg_is_dsd_notification_pending())
    {
      ds707_sys_chg_send_dsd_notification();
    }
    return;
  }
/*lint -save -e774 suppressing error 774*/
#ifdef FEATURE_EHRPD
  call_clean_up = ds707_sys_chg_hdlr_sys_mode_updated(new_network);
#endif

  /*-----------------------------------------------------------------------
    ds707_sys_chg_hdlr_sys_mode_updated() decides whether cleanup is
    necessary(EPC<->NONEPC). If cleanup is not required, notify DSD
    to go ahead with pref sys notification to external clients
  -----------------------------------------------------------------------*/
  if (!call_clean_up && ds707_sys_chg_is_dsd_notification_pending())
  {
    ds707_sys_chg_send_dsd_notification();
  }

  if ( (ds707_pkt_is_dormant(phys_link_ptr) == TRUE) &&
       (call_clean_up == FALSE) )
  {
    /*-----------------------------------------------------------------------
      This struct is passed onto the iface.  IDM has no data to send except
      if ppp_resync needed  on handoff to 1X
    -----------------------------------------------------------------------*/
    pkt_info.drs_bit = ds707_drs_get_drs_bit_on_idm_change(new_network);
    ppp_resync_ptr   = ds707_get_ppp_resync_flag_ptr();
    *ppp_resync_ptr  = pkt_info.drs_bit;

    pkt_info.use_hdown_timer = FALSE;
    pkt_info.new_network     = new_network;
    /*-------------------------------------------------------------------
      For Non TE Based origination force_cdma_only should always be FALSE
    -------------------------------------------------------------------*/
    pkt_info.force_cdma_only = FALSE; 
    /*-------------------------------------------------------------------
      Set the inc_call flag to FALSE for all Origination
    -------------------------------------------------------------------*/
    pkt_info.inc_call        = FALSE; 

    /*-------------------------------------------------------------------
     Set the epzid flag to FALSE for all Origination attempts
    -------------------------------------------------------------------*/
    pkt_info.epzid           = FALSE;
    ret_val = ps_phys_link_up_cmd(phys_link_ptr, 
                                  &err_num,
                                  &pkt_info);

    if ((ret_val < 0) && (err_num != DS_EWOULDBLOCK))
    {
      DATA_IS707_MSG1(MSG_LEGACY_ERROR, "ps phys_link reorig failed %d",
                      err_num);
    }
  }
  else if((PS_PHYS_LINK_GET_STATE(phys_link_ptr) == PHYS_LINK_GOING_DOWN) &&
          (call_clean_up != TRUE))
  {
    /*-------------------------------------------------------------------
      ds707_pkti_idm_orig_after_phys_link_down flag needs to be set to 
      TRUE only if special cleanup is not in progress as special cleanup
      would bring up the phys link after moving to 
      DS707_SYS_CHG_SM_WAIT_FOR_DORM state
    -------------------------------------------------------------------*/
    ds707_pkti_idm_orig_after_phys_link_down = TRUE;
    DATA_IS707_MSG1( MSG_LEGACY_HIGH,
                     "No IDM reorig. not dormant phys %d",
                     PS_PHYS_LINK_GET_STATE(phys_link_ptr) );
  }
/*lint -restore Restore lint error 774*/
  /*-------------------------------------------------------------------
    Report current bearer technology via ps_iface_generic_ind.
  -------------------------------------------------------------------*/
  memset( &bearer_tech_change, 
          0, 
          sizeof(ps_iface_ioctl_bearer_tech_changed_type));

#ifdef FEATURE_DATA_WLAN_MAPCON
  memset(&bearer_tech_ex, 0, sizeof(ds_sys_system_status_info_type));
#endif

  bearer_tech_change.old_bearer_tech.current_network 
                                                = PS_IFACE_NETWORK_CDMA;
  bearer_tech_change.new_bearer_tech.current_network 
                                                = PS_IFACE_NETWORK_CDMA;

  DATA_IS707_MSG1(MSG_LEGACY_MED, "Last data session network is %d", 
                  ds707_pkt_get_last_data_sess_network());
  if( ds707_pkt_get_last_data_sess_network() == SYS_SYS_MODE_CDMA )
  {
    bearer_tech_change.old_bearer_tech.data.cdma_type.rat_mask
                                                    = PS_IFACE_CDMA_1X;
  }
#ifdef FEATURE_HDR
  else if( ds707_pkt_get_last_data_sess_network() == SYS_SYS_MODE_HDR )
  {
    /*-------------------------------------------------------------------
      For DO system we lookup the current RTC_MAC protocols subtype
    -------------------------------------------------------------------*/
    if ((hdrcp_scp_get_current_subtype
         (HDRHAI_RTC_MAC_PROTOCOL)== HDRCP_SCP_SUB3_RMAC)
#ifdef FEATURE_HDR_REVB
         ||
         (hdrcp_scp_get_current_subtype
          (HDRHAI_RTC_MAC_PROTOCOL)== HDRCP_SCP_MC_RMAC)
#endif /* FEATURE_HDR_REVB */
        )
    {
      bearer_tech_change.old_bearer_tech.data.cdma_type.rat_mask
                                              = PS_IFACE_CDMA_EVDO_REVA;
    }
    else
    {
      bearer_tech_change.old_bearer_tech.data.cdma_type.rat_mask
                                              = PS_IFACE_CDMA_EVDO_REV0;      
    }
  }
#endif /* FEATURE_HDR */
  else
  {
    DATA_IS707_MSG0( MSG_LEGACY_MED,
                     "Last data session network is neither 1x nor HDR" );
  }

#ifdef FEATURE_HDR
  if( new_network == SYS_SYS_MODE_HDR )
  {
    DATA_IS707_MSG0(MSG_LEGACY_MED, "IDM changed to HDR");
    ds707_pkt_mgr_set_current_sys_mode(new_network);
    (void) ds707_pkt_mgr_get_bearer_tech_info(&(bearer_tech_change.new_bearer_tech));
#ifdef FEATURE_DATA_WLAN_MAPCON
    (void) ds707_pkt_mgr_get_bearer_tech_info_ex(&(bearer_tech_ex));
#endif
  }
  else 
#endif /* FEATURE_HDR */
  if( new_network == SYS_SYS_MODE_CDMA )
  {
    DATA_IS707_MSG0(MSG_LEGACY_MED, "IDM changed to CDMA");
    ds707_pkt_mgr_set_current_sys_mode(new_network);
    (void) ds707_pkt_mgr_get_bearer_tech_info(&(bearer_tech_change.new_bearer_tech));
#ifdef FEATURE_DATA_WLAN_MAPCON
    (void) ds707_pkt_mgr_get_bearer_tech_info_ex(&(bearer_tech_ex));
#endif
  }
  else
  {
    DATA_IS707_MSG2( MSG_LEGACY_ERROR,
                     "Invalid system on IDM_CHANGE from %d to %d",
                     ds707_pkt_get_last_data_sess_network(),
                     new_network);
    return;
  }

  ds707_pkt_iface_generic_ind( IFACE_BEARER_TECH_CHANGED_EV,
                               (void *)&bearer_tech_change);
  ds707_set_last_bearer_tech(bearer_tech_change.new_bearer_tech);

#ifdef FEATURE_DATA_WLAN_MAPCON
  ds707_pkt_iface_generic_ind( IFACE_BEARER_TECH_EX_CHANGED_EV,
                               (void *)&bearer_tech_ex);
#endif

#ifdef FEATURE_EPC_HANDOFF
  ds_epc_ehrpd_system(bearer_tech_change.new_bearer_tech.data.cdma_type.so_mask,
                      bearer_tech_change.new_bearer_tech.data.cdma_type.rat_mask);
#endif

/*lint -save -e774 suppressing error 774*/
  if ( call_clean_up == FALSE )
  {
    ds707_pkt_idm_change_event.last_data_session_network = 
                                              ds707_pkt_get_last_data_sess_network();
    ds707_pkt_idm_change_event.new_data_session_network = new_network; 
    event_report_payload ( EVENT_DS707_PKT_IDM_CHANGE,
                         (sizeof(ds707_pkt_idm_change_event_type)),
                         (void*)(&ds707_pkt_idm_change_event) );
  }
/*lint -restore Restore lint error 774*/
} /* ds707_pkti_idm_changed_cmd_hdlr() */

/*===========================================================================
FUNCTION       DS707_PKT_POST_IDM_CHANGE_CMD

DESCRIPTION    If we move a dormant data session from 1X to HDR or from HDR
               to 1X, send an orig with DRS=0 to notify the base station.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None
===========================================================================*/
void ds707_pkt_post_idm_change_cmd
(
  sys_sys_mode_e_type  new_system                /* New System             */
)
{
  ds_cmd_type          *cmd_ptr;                 /* DS Task msg            */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_ptr = ds_allocate_cmd_buf(sizeof(sys_sys_mode_e_type));
  if((NULL == cmd_ptr) || (NULL == cmd_ptr->cmd_payload_ptr))
  {
    ASSERT(0);
    return;
  }

  cmd_ptr->hdr.cmd_id = DS_CMD_707_PKT_IDM_CHANGE;

  *((sys_sys_mode_e_type *)cmd_ptr->cmd_payload_ptr) = new_system;

  ds_put_cmd( cmd_ptr );
} /* ds707_pkt_post_idm_change_cmd() */


#ifdef FEATURE_DS_LN_UPDATE
/*===========================================================================
FUNCTION       ds707_ln_update_status_cmd

DESCRIPTION    Post LN_update status command to DS task

DEPENDENCIES   

RETURN VALUE   None

SIDE EFFECTS   
===========================================================================*/
void ds707_ln_update_status_cmd 
(
  sys_hdr_location_notification_status_e_type  ln_status
)
{
  ds_cmd_type *cmd_ptr; 

  cmd_ptr = ds_allocate_cmd_buf(
                        sizeof(sys_hdr_location_notification_status_e_type));
  if((NULL == cmd_ptr) || (NULL == cmd_ptr->cmd_payload_ptr))
  {
    ASSERT(0);
    return;
  }
   
  else
  {
    cmd_ptr->hdr.cmd_id = DS_CMD_707_HDR_LN_STATUS;
    *((sys_hdr_location_notification_status_e_type * )
                                (cmd_ptr->cmd_payload_ptr)) = ln_status;
    ds_put_cmd(cmd_ptr);
  }
}

/*ds707_ln_update_status_cmd*/
#endif /*FEATURE_DS_LN_UPDATE*/


/*===========================================================================
FUNCTION       DS707_PKT_IDM_REORIG_AFTER_PHYS_LINK_DOWN

DESCRIPTION    If DRS=0 orig as a result of IDM change failed to go out, 
               this function attempts a re-origination after the PHYS_LINK
         goes down (dormancy)

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None
===========================================================================*/
void ds707_pkt_idm_reorig_after_phys_link_down
(
  void
)
{
  ps_phys_link_type        *phys_link_ptr;
  ds3g_pkt_call_info_type  pkt_info;
  int16                     err_num;
  int                       ret_val;
  boolean                  *ppp_resync_ptr;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  phys_link_ptr = ds707_pkt_get_pri_phys_link_ptr();
  if ((ds707_pkti_idm_orig_after_phys_link_down == TRUE) && 
      (ds707_pkt_is_dormant(phys_link_ptr) == TRUE) )      
  {
    DATA_IS707_MSG0( MSG_LEGACY_HIGH,
                     "IDM Change reorig after phys link down");
    /*-----------------------------------------------------------------------
      This struct is passed onto the iface.  IDM has no data to send except
      if ppp_resync needed  on handoff to 1X
    -----------------------------------------------------------------------*/
    pkt_info.drs_bit = ds707_drs_get_drs_bit_on_idm_change(
                                            ds707_pkti_saved_idm_for_reorig);
    ppp_resync_ptr   = ds707_get_ppp_resync_flag_ptr();
    *ppp_resync_ptr  = pkt_info.drs_bit;

    pkt_info.use_hdown_timer = FALSE;
    pkt_info.new_network     = ds707_pkti_saved_idm_for_reorig;
    /*-------------------------------------------------------------------
      For Non TE Based origination force_cdma_only should always be FALSE
    -------------------------------------------------------------------*/
    pkt_info.force_cdma_only = FALSE; 
    /*-------------------------------------------------------------------
      Set the inc_call flag to FALSE for all Origination
    -------------------------------------------------------------------*/
    pkt_info.inc_call        = FALSE; 

    /*-------------------------------------------------------------------
     Set the epzid flag to FALSE for all Origination attempts
    -------------------------------------------------------------------*/
    pkt_info.epzid           = FALSE;

    ret_val = ps_phys_link_up_cmd(phys_link_ptr, &err_num, &pkt_info);

    if( (ret_val < 0) &&
        (err_num != DS_EWOULDBLOCK))
    {
      DATA_IS707_MSG1(MSG_LEGACY_ERROR, "ps phys_link reorig failed %d",
                      err_num);
    }
    else
    {
      ds707_pkti_idm_orig_after_phys_link_down = FALSE;
      ds707_pkti_saved_idm_for_reorig = SYS_SYS_MODE_NO_SRV;
    }
  }
}
    #ifdef FEATURE_DS_CAM

/*===========================================================================
FUNCTION       DS707_PKT_POST_COLOC_INFO_CHANGED_CMD

DESCRIPTION    Posts new colocation info along with the INFO_CHANGE_CMD
               to DS. DS  updated its local copy of colocation info on
               receiving this command.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None
===========================================================================*/

void ds707_pkt_post_coloc_info_changed_cmd
(
  boolean evdo_colocated
)
{
  ds_cmd_type      *cmd_ptr;                     /* DS Task msg            */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_IS707_MSG2(MSG_LEGACY_MED, 
                  " Coloc info changed  old coloc info: %d new coloc info: %d",
                  ds707_pkti_1x_colocated_with_evdo, evdo_colocated);

  cmd_ptr = ds_allocate_cmd_buf(sizeof(boolean));
  if((NULL == cmd_ptr) || (NULL == cmd_ptr->cmd_payload_ptr))
  {
    ASSERT(0);
    return;
  }
  else
  {
    cmd_ptr->hdr.cmd_id = DS_CMD_707_COLOC_INFO_CHANGED;
    *((boolean *)(cmd_ptr->cmd_payload_ptr)) = evdo_colocated;
    ds_put_cmd( cmd_ptr );
  }

} /* ds707_pkt_post_coloc_info_changed_cmd() */

/*===========================================================================
FUNCTION      DS707_PKTI_COLOC_INFO_CHANGED_CMD_HDLR

DESCRIPTION   Process sys_info change event posted by CM.
              DS needs this information to see if we have evdo colocated.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/

void ds707_pkti_coloc_info_changed_cmd_hdlr
(
boolean   evdo_colocated                /* Holds the colocation info    */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Store colocation info in local variable, for later use
  -------------------------------------------------------------------------*/
  ds707_pkti_1x_colocated_with_evdo = evdo_colocated;
} /* ds707_pkti_coloc_info_changed_cmd_hdlr() */

/*===========================================================================
FUNCTION      DS707_PKT_EVDO_COLOCATED

DESCRIPTION   Process sys_info change event posted by CM.
              DS needs this information to see if we have evdo colocated.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_pkt_evdo_colocated
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return(ds707_pkti_1x_colocated_with_evdo);
}/*  ds707_pkt_evdo_colocated */

    #endif /* FEATURE_DS_CAM */

/*===========================================================================
FUNCTION       DS707_ALN_CMD_HDLR

DESCRIPTION    Processed INITIATE_ALN_CMD from HDR MC. If a MIP call is up,
               calls ppp_resync, else initiates orig with DRS 1 if we need
               to do a PPP resync.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None
===========================================================================*/
void ds707_aln_cmd_hdlr
(
  void
)
{
#ifdef FEATURE_HDR
  ps_phys_link_type        *phys_link_ptr;
  ds3g_pkt_call_info_type  pkt_info;
  int16                     err_num;
  int                       ret_val;
  boolean                  *ppp_resync_ptr;
  uint32                    cta_val = 0;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  phys_link_ptr = ds707_pkt_get_pri_phys_link_ptr();
  (void) ds707_pkt_mgr_get_1x_profile_val(DS707_CTA_VAL,(void *)&cta_val,
                                          DS707_NON_ARRAY);

  /*-------------------------------------------------------------------------
    If PKT is dormant, check if it is ok to Orig from Dormancy. Otherwise
    continue processing the ALN Command
  -------------------------------------------------------------------------*/

  if (ds707_pkt_is_dormant(phys_link_ptr))
  {
    if (ds707_pkti_ok_orig_from_dorm(phys_link_ptr,
                                     DS707_PKT_DORM_REASON_NETWORK) == FALSE)
    {
      DATA_IS707_MSG0( MSG_LEGACY_HIGH,
                       "No ALN reorig. since phys link say not OK");
      return;
    }
  }
  else
  {
    DATA_IS707_MSG0( MSG_LEGACY_MED,
                     "No ALN reorig but continue with PPP resync");
  }

  /*-------------------------------------------------------------------------
    If we are already active in a MIP call, do a PPP resync.
  -------------------------------------------------------------------------*/
  if ( ds707_get_pkt_data_sess_state() == DS707_PKT_DATA_SESS_HDR_ACTIVE)
  {
    if (ds707_drs_is_in_mip_call())
    {
      DATA_IS707_MSG0( MSG_LEGACY_MED,
                       "PPP resync on ALN cmd in active state");

      /*---------------------------------------------------------------------
        Acquire the ppp resync lock from DSSNET before proceeding with
        the Resync procedure. If the lock is acquired just after GPS checked
        the status to do a fix, it would atleast prevent GPS from doing
        another back-to-back fix after the current fix
      ---------------------------------------------------------------------*/
      ds707_pkt_acquire_ppp_resync_lock();

      if (ppp_resync( PPP_UM_SN_DEV,
                      PPP_PROTOCOL_LCP,
                      NULL) < 0)
      {
        DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Failed to resync PPP on ALN");
      }
    }
    return;
  }

  /*-------------------------------------------------------------------------
    If we are in a MIP call, bring up a call and do a PPP resync when the
    call gets connected.

    ds707_drs_get_drs_bit_on_ALN will also set the flag indicating that a 
    PPP resync is required when we get a call connected.
  -------------------------------------------------------------------------*/
  pkt_info.drs_bit = ds707_drs_get_drs_bit_on_ALN();
  if (pkt_info.drs_bit)
  {
    ppp_resync_ptr   = ds707_get_ppp_resync_flag_ptr();
    *ppp_resync_ptr  = pkt_info.drs_bit;

    pkt_info.use_hdown_timer = FALSE;
    /*-------------------------------------------------------------------
      For Non TE Based origination force_cdma_only should always be FALSE
    -------------------------------------------------------------------*/
    pkt_info.force_cdma_only = FALSE; 
    /*-------------------------------------------------------------------
      Set the inc_call flag to FALSE for all Origination
    -------------------------------------------------------------------*/
    pkt_info.inc_call        = FALSE; 

    /*-----------------------------------------------------------------------
      Restore the current Data session network to be the same as last data
      session network since the last origination was just a Fake origination
      to trigger ALN. This was never sent to the Network. The CDSN and LADN
      are both SYS_SYS_CDMA right after the following assignment 
    -----------------------------------------------------------------------*/
    ds707_pkt_set_current_data_sess_network(ds707_pkt_get_last_data_sess_network());

    /*-------------------------------------------------------------------
      Set the epzid flag to FALSE for all Origination attempts
     -------------------------------------------------------------------*/
    pkt_info.epzid           = FALSE;

    ds707_extif_inform_hdr_sess_status();

    /*-----------------------------------------------------------------------
      Save the original CTA timer value and temporarily set the CTA timer 
      value to 1 second to drop the traffic channel right away after a 
      failed ALN handup to DO from 1x. 
    -----------------------------------------------------------------------*/
    ds707_pkt_mgr_set_failed_aln_cta_restore_val(cta_val);

    /*-----------------------------------------------------------------------
      Note that new_network is retained as ds707_current_data_sess_network
      here since we are sending an orig with DRS=1, so we don't need to
      update ds707_current_data_sess_network when we send the orig. We will
      update it when we get a call connected.
    -----------------------------------------------------------------------*/
    pkt_info.new_network     = ds707_pkt_get_current_data_sess_network();

    if ((PS_PHYS_LINK_GET_STATE(phys_link_ptr) == PHYS_LINK_COMING_UP) ||
	      (PS_PHYS_LINK_GET_STATE(phys_link_ptr) == PHYS_LINK_RESUMING))
    {
      DATA_IS707_MSG0(MSG_LEGACY_HIGH,
            "Can't bring up TC for ALN. Setting aln_cmd_pending FLAG to TRUE");
      ds707_set_aln_cmd_pending(TRUE);
    }
    else
    {
      ret_val = ps_phys_link_up_cmd( phys_link_ptr,  
                                     &err_num,
                                     &pkt_info);	

      if ( (ret_val < 0) &&
           (err_num != DS_EWOULDBLOCK))
      {
        DATA_IS707_MSG1(MSG_LEGACY_ERROR, "ps phys link reorig failed %d",
                        err_num);
      }
    }
  }

#endif /* FEATURE_HDR */
} /* ds707_aln_cmd_hdlr() */

/*===========================================================================
FUNCTION       DS707_PKT_POST_ALN_CMD

DESCRIPTION    Post the INITIATE_ALN command from HDR protocols to DS

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None
===========================================================================*/
void ds707_pkt_post_aln_cmd
(
  void
)
{
  ds_cmd_type      *cmd_ptr;                     /* DS Task msg            */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ((cmd_ptr = ds_allocate_cmd_buf(DS_CMD_PAYLOAD_SIZE_ZERO)) == NULL)
  {
    ASSERT(0);
    return;
  }

  cmd_ptr->hdr.cmd_id = DS_CMD_707_INITIATE_ALN;
  ds_put_cmd(cmd_ptr);
} /* ds707_pkt_post_aln_cmd() */

#ifdef FEATURE_DS_LN_UPDATE
/*===========================================================================
FUNCTION       ds707_hdr_ln_update_status_cmd_hdlr

DESCRIPTION    Process the HDR LN Status command from CM 

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   
===========================================================================*/

void ds707_hdr_ln_update_status_cmd_hdlr
(
  sys_hdr_location_notification_status_e_type ln_status
)
{
  ps_phys_link_type *phys_link_ptr;
  ds707_pkt_ln_update_event_type ds707_pkt_ln_update_event;

  phys_link_ptr  = ds707_pkt_get_pri_phys_link_ptr();
  if (ln_status == SYS_HDR_LOCATION_NOTIFICATION_STATUS_SUCCESSFUL)
  {
    /*---------------------------------------------------------
    Update LDSN on getting an LN update only if in Dormant mode.
    If in traffic, the LDSN will be updated when phys_link goes down.
    ---------------------------------------------------------*/
    if (ds707_pkt_is_dormant(phys_link_ptr) == TRUE)
    {
      ds707_pkt_set_last_data_sess_network(SYS_SYS_MODE_HDR);
    }
    ds707_pkt_mgr_update_cdsn(SYS_SYS_MODE_HDR);
    ds707_extif_inform_hdr_sess_status();
    DATA_IS707_MSG1( MSG_LEGACY_HIGH,
                     "Recvd HDR LN Success. Updated CDSN = %d",
                     ds707_pkt_get_current_data_sess_network());
  }
  else if(ln_status == SYS_HDR_LOCATION_NOTIFICATION_STATUS_FAILED)
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, "HDR LN failed");
  }
  else
  {
    DATA_IS707_MSG1( MSG_LEGACY_HIGH, 
                     "Recvd HDR LN update with invalid parameter ln_status - %d",
                     ln_status);
  }
  DATA_IS707_MSG1( MSG_LEGACY_HIGH, "LDSN = %d",
                   ds707_pkt_get_last_data_sess_network());
  
  ds707_pkt_ln_update_event.ln_status = ln_status;
  ds707_pkt_ln_update_event.curr_data_session_network = 
                                            ds707_pkt_get_current_data_sess_network();
  ds707_pkt_ln_update_event.last_data_session_network = 
                                               ds707_pkt_get_last_data_sess_network();
  event_report_payload( EVENT_DS707_PKT_LN_UPDATE,
                        (sizeof(ds707_pkt_ln_update_event_type)),
                        (void*)(&ds707_pkt_ln_update_event) );
}
/*ds707_hdr_ln_update_status_cmd_hdlr*/
#endif /*FEATURE_DS_LN_UPDATE*/

/*===========================================================================
                       EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_PKT_HDR_PROCESS_CMD

DESCRIPTION   Called by 3G DSMGR, to handle commands specific to this
              mode handler.

              There is no featurization of commands here.  Functionality
              should be FEATURE'ized elsewhere, so that these commands are
              generated appopriately.  For example, the SID module only
              generates its command if the SID FEATURE's is defined.

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  Can result in various actions, such as call bring up/tear
              down, etc.
===========================================================================*/
void ds707_pkt_hdr_process_cmd
(
  ds_cmd_type   *ds_cmd_ptr
)
{
  sys_sys_mode_e_type  new_sys_mode;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DATA_MDM_ARG_NOT_CONST(ds_cmd_ptr);

  if(NULL == ds_cmd_ptr)
  {
    DATA_ERR_FATAL("Invalid ds_cmd_ptr passed");
    return;
  }

  if (((DS_CMD_707_PKT_IDM_CHANGE == ds_cmd_ptr->hdr.cmd_id) ||
       (DS_CMD_707_HDR_LN_STATUS == ds_cmd_ptr->hdr.cmd_id) ||
       (DS_CMD_707_COLOC_INFO_CHANGED == ds_cmd_ptr->hdr.cmd_id)) && 
      (NULL == ds_cmd_ptr->cmd_payload_ptr))
  {
    DATA_ERR_FATAL("cmd_payload_ptr is NULL");
    return;
  }

  switch (ds_cmd_ptr->hdr.cmd_id)
  {
    case DS_CMD_707_PKT_IDM_CHANGE:
      new_sys_mode = *((sys_sys_mode_e_type *)(ds_cmd_ptr->cmd_payload_ptr));
      ds707_pkti_idm_changed_cmd_hdlr(new_sys_mode);
      break;

    case DS_CMD_707_INITIATE_ALN:
      ds707_aln_cmd_hdlr();
      break;

#ifdef FEATURE_DS_LN_UPDATE
    case DS_CMD_707_HDR_LN_STATUS:
      ds707_hdr_ln_update_status_cmd_hdlr(
        *((sys_hdr_location_notification_status_e_type * ) 
          (ds_cmd_ptr->cmd_payload_ptr)));
      break;
#endif /*FEATURE_DS_LN_UPDATE*/

#ifdef FEATURE_DS_CAM
    case DS_CMD_707_COLOC_INFO_CHANGED:
      DATA_IS707_MSG0( MSG_LEGACY_HIGH, "Processing Coloc info change");
      ds707_pkti_coloc_info_changed_cmd_hdlr(
                                  *((boolean *)(ds_cmd_ptr->cmd_payload_ptr)));
      break;
#endif //FEATURE_DS_CAM

    default:
      DATA_IS707_MSG1(MSG_LEGACY_ERROR, "Unknown cmd to ds707_pkt %d",
                      ds_cmd_ptr->hdr.cmd_id);
      break;
  }
} /* ds707_pkt_hdr_process_cmd() */

  #endif /* FEATURE_HDR */
#endif /* FEATURE_DATA_IS707 */


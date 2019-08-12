/*===========================================================================
                       D S U M T S _ R M S M _ I P . C

DESCRIPTION
  The Data Services UMTS RM State Machine for pdp-ip calls source file. 
  This state machine manages the Um and Rm interfaces for pdp-ip type 
  pkt data calls initiated from laptop or connected terminals .
  
  
EXTERNALIZED FUNCTIONS

Copyright (c) 2002-2015 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/rmsm/src/dsumts_rmsm_ip.c#1 $ 
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/08/12    vs     Added support for 3GPP IPv6 over PPP
12/26/11    ss     Fixed race condition due to calling PPP function directly 
                   in ipcp resp timer cb.
05/09/11    asn    Support for new event for active Ifaces 
06/20/11    ttv    Merged silent redial support changes for UMTS tethered calls.
04/01/11    ttv    Support for Q6 free floating.
03/23/11    op     Data SU API cleanup
03/14/11    ss     Fixed a crash during DUN->embedded arbitration.
02/24/11    ttv    Fixed compilation warnings.
02/02/11    ss     Fixed a crash when DUN call is originated and disconnected 
                   immediately.
02/02/11    ss     Fixed crash on call_end when a RMNet/tethered and socket 
                   call shared an iface and RMNet/tethered call was ended 
                   before the socket call.
01/04/11    ss     Fixed the issue of UM IFACE not being cleaned up when
                   DTR drop is posted before IFACE bring up.
12/23/10    asn    Fix issue with DUN call disconnect after arbitration
10/27/10    ttv    Added feature wrapping for rr_dtm_get_max_LAPDm_octets
                   fuction.
09/21/10    ss     Fixed memory leak of PPP event handle.
07/29/10    sa     Fixed DUN data rate reporting based on device capability.
07/16/10    sa     Adding support to configure PPP Auth protocol.
07/09/10    asn    Support for Arbitration
07/12/10    sch    Configuring HDLC mode HDLC_A2_AUTONOMOUS in ppp config.
06/28/10    vk     Removed sending of fake DNS addresses in CONFIG_NAK
10/14/09    ss     CMI AU level cleanup.
09/21/09    ml     Retrieve PDP profile from registry (not iface tbl)
09/18/09    ss     DataCommon CMI SU level impacts.
07/04/09    pp     hton/ntoh* macros replaced with ps_hton/ntoh*.
04/08/08    vrk    Lint Fixes.
12/06/07    vrk    Changed extended CONNECT - default data rate to 7.2Mbps
11/15/07    asn    Added support for new feature MT-PDP terminating in TE
11/11/07    vrk    Added support for returning extended result codes 
                   with CONNECT.
09/12/07    scb    Replaced the ppp_update_ipcp_config() api with 
                   ppp_update_proto_config() API.
06/07/07    rr     Added DSUMTS_ENTER_CRIT_SECTION in dsumts_rmsm_ip_transit
                   ion_state and dsumts_rmsm_ip_process_um_iface_up_ev  .
04/12/07    rr     Added missing DSUMTS_BRANCH_LEAVE_CRIT_SECTION in
                   dsumts_rmsm_ip_rm_ipcp_cfg_req_cb. 
02/11/07    vk     Fixed RVCT 2.1 compiler warnings.
02/02/07    rr     Fixed Lint warnings.
02/08/07    rr     Added a fix to ignore IPCP config req packet if we have 
                   already injected config NAK packet.
09/09/06    rr     Added NV Item support for NBNS.
07/17/06    rr     Replaced using Large DSM items with small ones for IPCP 
                   response.
06/07/06    aw     Replaced TASKLOCK with rex_critical sections for L4  and
                   removed unnecessary TASKLOCKS.
05/01/06    rsl    Lint error fixes.
12/02/05    rr     Modified code to support RMNET calls with Legacy PPP calls.
07/28/05    rc     Removed calls to ps_flow_go_null_ind() and
                   ps_flow_activate_ind().
05/12/05    rr     Fixed Lint errors/warnings.
01/27/05    ssh    Changed include file name dsbyte.h to ps_byte.h
12/28/04    rr     Changed debug messages to correctly print IP addresses
11/21/04    rc     Added code to register and deregister phys link callbacks
                   for phys link flow enable and disable events. 
11/01/04    rr     Added WINS Support
10/15/04    ifk    Snoop callback now takes a meta info ptr reference
10/14/04    ar     Ensure SIO port allocated for external data call.
09/22/04    rr     Replaced ip4a with uint32
09/20/04    vp     Replaced the inclusion of psglobal.h with ps_in.h.
07/15/04    ifk    Replaced ppp_reg_event() with new PPP event architecture
06/04/04    rc     Fixed Lint errors/warnings.
05/17/04    rc     Fixed a bug where the Um interface was not getting
                   released if the user pressed cancel right after
                   hitting dial.
01/31/04    usb    No need to disable Um iface ACL, routing takes care of 
                   exclusive use of iface in laptop call.
01/27/04    vsk    M-PDP support 
01/05/03    rc     Enable flow by clearing all masks while transitioning to
                   DSUMTS_RMSM_IP_NULL_STATE state in order to accept next
                   call.
11/14/03    vsk    use the new PPP_AUTH_WAS_CHAP macro
11/03/03    vsk    use new ppp_get_auth_info() fn signature 
08/15/03    aku    Access graceful_dormant_close variable via ps_iface macro
08/13/03    vsk    Generate Spurious IPCP naks to keep ppp state machine going
07/14/03    rc     Set the Phys Link State appropriately on the SIO iface.
04/22/03    vsk    Code review cleanup
04/18/03    vsk    Created module
===========================================================================*/



/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "memory.h"
#include "err.h"
#include "msg.h"

#include "dstask.h"
#include "ps_iface.h"
#include "ds3gsiolib.h"
#include "ds_flow_control.h"
#include "dstaski.h"
#include "ps_iface.h"
#include "ps_in.h"
#include "ps_ppp_netmodel.h" 
#include "ps_ppp.h"
#include "ps_ppp_snoop.h"
#include "ps_ppp_fsm.h"
#include "ps_ppp_ipcp.h"
#include "ps_meta_info.h"
#include "ps_byte.h"

#ifdef FEATURE_GSM_DTM
#include "rr_ds_if.h"
#endif /* FEATURE_GSM_DTM */

#include "dsumtspdpreg.h"
#include "ds_3gppi_utils.h"

#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS)|| defined (FEATURE_DATA_LTE)
#include "dsumts_rmsm.h"
#include "dsumts_rmsmi.h"
#include "dsumts_rmsm_ip.h"
#include "dsumtspsqos.h"

#ifdef FEATURE_DATA_TE_MT_PDP
#include "dsatetsictab.h"
#include "dsumtspsmthdlr.h"
#endif /*FEATURE_DATA_TE_MT_PDP*/

#include "ps_ip_netconfig.h"
#include "ds_3gpp_rmsm_autoconfig_sm.h"

/*===========================================================================
                            External Declarations
===========================================================================*/
extern int32 ps_policy_mgr_get_priority
(
  int64  app_identifier,
  int32  profile_id
);

/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                            INTERNAL DEFINITIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/


/*===========================================================================
                            Forward Declarations
===========================================================================*/

/*---------------------------------------------------------------------------
  DSUMTS_RMSM IP state management function
---------------------------------------------------------------------------*/

LOCAL void dsumts_rmsm_ip_transition_state
(
  dsumts_rmsmi_sm_info_type    *sm_ptr,
  dsumts_rmsm_ipi_state_type   new_state
);

/*---------------------------------------------------------------------------
  Function to deregister Um events
---------------------------------------------------------------------------*/
LOCAL void dsumts_rmsm_ip_dereg_um_events
(
  dsumts_rmsmi_sm_info_type       *sm_ptr
);


/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                        EXTERNAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/


/*===========================================================================
  FUNCTION DSUMTS_RMSM_IP_INIT ()

  DESCRIPTION
    This function initializes the RmSm IP State Machine.

  PARAMETERS
    sm_ptr: Pointer to the RmSm IP State Machine

  RETURN VALUE
    TRUE,  if success
    FALSE, if failure

  DEPENDENCIES
    none  

  SIDE EFFECTS
    None
===========================================================================*/
boolean dsumts_rmsm_ip_init
(
  dsumts_rmsmi_sm_info_type* sm_ptr
)
{
  if (sm_ptr == NULL) 
  {
    DATA_ERR_FATAL("Sm Info ptr is NULL !");
    return FALSE;
  }
  
  sm_ptr->pdp_specific_info.pdp_ip.state = DSUMTS_RMSM_IP_NULL_STATE;
  
  return ds_3gpp_rmsm_autoconfig_sm_init(sm_ptr);

} /*dsumts_rmsm_ip_init*/


/*===========================================================================
  FUNCTION DSUMTS_RMSM_IP_PROCESS_CMD()

  DESCRIPTION
    This function receives a cmd posted to DSUMTS_RMSM (for pdp-ip calls)
    in DS task and calls an appropriate function to handle it.

  PARAMETERS
    inst_ptr: Pointer to the Rm Dev Instance and SM Instance
    cmd id:   Command to be processed

  RETURN VALUE
    none

  DEPENDENCIES
    none  
    
  SIDE EFFECTS
    None
===========================================================================*/
void dsumts_rmsm_ip_process_cmd
(
  dsumts_rmsm_instance_type* inst_ptr,
  ds_cmd_enum_type cmd
)
{
  dsumts_rmsmi_sm_info_type* sm_ptr = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (inst_ptr == NULL)
  {
    DATA_ERR_FATAL("inst ptr is null");
    /*lint -e527*/
    return;
    /*lint +e527*/
  }

  
  sm_ptr = dsumts_rmsmi_get_sm_info_ptr(inst_ptr);

  if (sm_ptr == NULL)
  {
    DATA_ERR_FATAL("Sm ptr is NULL");
    return;
  }

  switch(cmd)
  { 
    case DS_CMD_3GPP_RMSM_UM_IFACE_DOWN_ARB:
      dsumts_rmsm_ip_process_um_iface_down_arb_ev(sm_ptr);
      break;

    case DS_CMD_3GPP_RMSM_ADDR_CFG_COMPLETE:
      dsumts_rmsm_ip_process_addr_cfg_complete_ev(sm_ptr);
      break;

    case DS_CMD_3GPP_RMSM_ADDR_CFG_END:
      dsumts_rmsm_ip_process_addr_cfg_end_ev(sm_ptr);
      break;

    case DS_CMD_3GPP_RMSM_ADDR_CFG_FAIL:
      dsumts_rmsm_ip_process_addr_cfg_failure_ev(sm_ptr);
      break;

    case DS_CMD_3GPP_RMSM_ADDR_CFG_CHANGED:
      dsumts_rmsm_ip_process_addr_cfg_changed_ev(sm_ptr);
      break;      

    default:
      DS_3GPP_MSG3_ERROR("Invalid UMTS RMSM sm instance: %d, "
                         "rm dev instance: %d, cmd %d, ignoring", 
                          inst_ptr->sm_instance, inst_ptr->rm_dev_instance,
                          cmd);
      ASSERT(0);
  }
} /*dsumts_rmsm_ip_process_cmd*/

/*===========================================================================
  FUNCTION DSUMTS_RMSM_IP_PROCESS_BRING_UP_UM_IFACE_EV

  DESCRIPTION
    This function processes UMTS_RMSM_RM_BRING_UP_UM_IFACE_EV event. 

  PARAMETERS
    sm_ptr: Pointer to the RmSm IP State Machine

  RETURN VALUE
    None

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
void dsumts_rmsm_ip_process_bring_up_um_iface_ev
(
  dsumts_rmsmi_sm_info_type *sm_ptr
)
{

  dsumts_rmsmi_info_type* rmsmi_info_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (sm_ptr == NULL) 
  {
    DATA_ERR_FATAL("sm_ptr = NULL");
    /*lint -e527*/
    return;
    /*lint +e527*/
  }

  rmsmi_info_ptr = (dsumts_rmsmi_info_type*)sm_ptr->vobj_data_ptr;


  if (rmsmi_info_ptr == NULL) 
  {
    DATA_ERR_FATAL("rmsmi_info_ptr = NULL");
    /*lint -e527*/
    return;
    /*lint +e527*/
  }
  
  DS_3GPP_MSG2_HIGH( "Recd RM_BRING_UP_UM_IFACE_EV, sm instance:%d state:%d", 
           sm_ptr->instance,sm_ptr->pdp_specific_info.pdp_ip.state);

  switch( sm_ptr->pdp_specific_info.pdp_ip.state )
  {
    case DSUMTS_RMSM_IP_NULL_STATE:

        /*-----------------------------------------------------------------
          Transition the state to waiting for Um up state which would 
          bring up the um packet call
        -----------------------------------------------------------------*/
        dsumts_rmsm_ip_transition_state( sm_ptr,
                                DSUMTS_RMSM_IP_WAITING_FOR_UM_UP_STATE );
      break;

    case DSUMTS_RMSM_IP_WAITING_FOR_UM_UP_STATE:
    case DSUMTS_RMSM_IP_WAITING_FOR_UM_ARB:
    /*---------------------------------------------------------------------
      We have requested Um iface be bought up already. The new packet would
      have been stored in the callback & old one freed. Just ignore this
      request as we have already kick started bring up the Um interface
      ..fall through
    ---------------------------------------------------------------------*/      
      DS_3GPP_MSG0_HIGH("IPCP confg req ignored. waiting for Um to be up");
      /* Fall Through */
    //lint -fallthrough

    case DSUMTS_RMSM_IP_WAITING_FOR_AUTOCONFIG_COMPLETE_STATE:
    case DSUMTS_RMSM_IP_WAITING_FOR_RM_PPP_UP_STATE:
    case DSUMTS_RMSM_IP_UM_RM_UP_STATE:
    case DSUMTS_RMSM_IP_WAITING_FOR_UM_DOWN_STATE:

      /*---------------------------------------------------------------------
        Ignore event in all other states.
      ---------------------------------------------------------------------*/
      DS_3GPP_MSG2_HIGH( "Ignoring RM_BRING_UP_UM_IFACE_EV Sm instance :%d state:%d",
                sm_ptr->instance,sm_ptr->pdp_specific_info.pdp_ip.state);
      break;

    default:

      /*---------------------------------------------------------------------
        Invalid state
      ---------------------------------------------------------------------*/
      DS_3GPP_MSG1_ERROR( "Invalid RMSM state %d",
           sm_ptr->pdp_specific_info.pdp_ip.state);
      ASSERT( 0 );
      break;

  } /* switch( sm_ptr->pdp_specific_info.pdp_ip.state ) */

} /*dsumts_rmsm_ip_process_bring_up_um_iface_ev() */


/*===========================================================================
  FUNCTION DSUMTS_RMSM_IP_PROCESS_ADDR_CFG_COMPLETE_EV()

  DESCRIPTION
    This function processes ADDR_CFG_COMPLETE_EV event. 
    
  PARAMETERS
    sm_ptr: Pointer to the RmSm IP State Machine

  RETURN VALUE
    None

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
void dsumts_rmsm_ip_process_addr_cfg_complete_ev
(
  dsumts_rmsmi_sm_info_type *sm_ptr
)
{
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (sm_ptr == NULL) 
  {
    DATA_ERR_FATAL("sm_ptr = NULL");
    /*lint -e527*/
    return;
    /*lint +e527*/
  }

  DS_3GPP_MSG2_HIGH( "Recd ADDR_CFG_COMPLETE_EV, sm instance:%d state:%d", 
           sm_ptr->instance,sm_ptr->pdp_specific_info.pdp_ip.state);

  switch( sm_ptr->pdp_specific_info.pdp_ip.state )
  {
    case DSUMTS_RMSM_IP_WAITING_FOR_AUTOCONFIG_COMPLETE_STATE:
    case DSUMTS_RMSM_IP_RM_RECONFIGURING_STATE:
    case DSUMTS_RMSM_IP_WAITING_FOR_RM_PPP_UP_STATE:
      
      /*---------------------------------------------------------------------
        Setup IP fwd link bridging between SIO and UM 
        interface
      ---------------------------------------------------------------------*/
      ps_iface_set_bridge( sm_ptr->um_iface_ptr, 
                           &sm_ptr->sio_iface );
      dsumts_rmsm_ip_transition_state( sm_ptr,DSUMTS_RMSM_IP_UM_RM_UP_STATE );
      break;


    case DSUMTS_RMSM_IP_WAITING_FOR_UM_UP_STATE:
    case DSUMTS_RMSM_IP_UM_RM_UP_STATE:
    case DSUMTS_RMSM_IP_NULL_STATE:
    case DSUMTS_RMSM_IP_WAITING_FOR_UM_DOWN_STATE:
      /*---------------------------------------------------------------------
        Ignore event in all other states.
      ---------------------------------------------------------------------*/
      DS_3GPP_MSG2_HIGH( "Ignoring RM_IFACE_UP_EV Sm Instance :%d state:%d",
                sm_ptr->instance,sm_ptr->pdp_specific_info.pdp_ip.state);
      break;

    default:

      /*---------------------------------------------------------------------
        Invalid state
      ---------------------------------------------------------------------*/
      DS_3GPP_MSG1_ERROR( "Invalid RMSM state %d",
           sm_ptr->pdp_specific_info.pdp_ip.state);
      ASSERT( 0 );
      break;

  } /* switch( sm_ptr->pdp_specific_info.pdp_ip.state ) */

} /* dsumts_rmsm_ip_process_addr_cfg_complete_ev */



/*===========================================================================
  FUNCTION DSUMTS_RMSM_IP_PROCESS_ADDR_CFG_END_EV()

  DESCRIPTION
    This function processes ADDR_CFG_END_EV event. 
    
  PARAMETERS
    sm_ptr: Pointer to the RmSm IP State Machine

  RETURN VALUE
    None

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
void dsumts_rmsm_ip_process_addr_cfg_end_ev
(
  dsumts_rmsmi_sm_info_type *sm_ptr
)
{
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (sm_ptr == NULL) 
  {
    DATA_ERR_FATAL("sm_ptr = NULL");
    /*lint -e527 */
    return;
    /*lint +e527 */
  }

  DS_3GPP_MSG2_HIGH( "Recd ADDR_CFG_END_EV, sm instance:%d state:%d", 
           sm_ptr->instance,sm_ptr->pdp_specific_info.pdp_ip.state);

  switch( sm_ptr->pdp_specific_info.pdp_ip.state )
      {

    case DSUMTS_RMSM_IP_WAITING_FOR_AUTOCONFIG_COMPLETE_STATE:
    case DSUMTS_RMSM_IP_UM_UP_WAITING_FOR_AUTOCONFIG_END_STATE:
    /*---------------------------------------------------------------------
      Um interface should be shut down. Clean UP UM iface
    ---------------------------------------------------------------------*/      
      dsumts_rmsm_ip_transition_state( sm_ptr, 
                                DSUMTS_RMSM_IP_WAITING_FOR_UM_DOWN_STATE );
      break;

    case DSUMTS_RMSM_IP_UM_DOWN_WAITING_FOR_AUTOCONFIG_END_STATE:
      dsumts_rmsm_ip_transition_state(sm_ptr, DSUMTS_RMSM_IP_NULL_STATE);
      break;

    case DSUMTS_RMSM_IP_NULL_STATE:
    case DSUMTS_RMSM_IP_WAITING_FOR_UM_UP_STATE:
    case DSUMTS_RMSM_IP_WAITING_FOR_UM_DOWN_STATE:
    case DSUMTS_RMSM_IP_WAITING_FOR_UM_ARB:
    case DSUMTS_RMSM_IP_UM_RM_UP_STATE:
   

      /*---------------------------------------------------------------------
        Ignore event in all other states.
      ---------------------------------------------------------------------*/
      DS_3GPP_MSG2_HIGH( "Ignoring ADDR_CFG_END_EV Sm Instance :%d state:%d",
                sm_ptr->instance,sm_ptr->pdp_specific_info.pdp_ip.state);
      break;

    default:

      /*---------------------------------------------------------------------
        Invalid state
      ---------------------------------------------------------------------*/
      DS_3GPP_MSG1_ERROR( "Invalid RMSM state %d",
           sm_ptr->pdp_specific_info.pdp_ip.state);
      ASSERT( 0 );
      break;

  } /* switch( sm_ptr->pdp_specific_info.pdp_ip.state ) */
} /* dsumts_rmsm_ip_process_addr_cfg_end_ev */

/*===========================================================================
  FUNCTION DSUMTS_RMSM_IP_PROCESS_UM_IFACE_UP_EV()

  DESCRIPTION
    This function processes UM_IFACE_UP_EV event. 
    
  PARAMETERS
    sm_ptr: Pointer to the RmSm IP State Machine

  RETURN VALUE
    None

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
void dsumts_rmsm_ip_process_um_iface_up_ev
(
  dsumts_rmsmi_sm_info_type *sm_ptr
)
{
  dsumts_rmsmi_info_type *rmsmi_info_ptr;
  ds_3gpp_rmsm_ip_common_info_type *info_ptr = NULL;
  dsumts_rmsm_instance_type inst;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (sm_ptr == NULL) 
  {
    DATA_ERR_FATAL("sm_ptr = NULL");
    /*lint -e527 */
    return;
    /*lint +e527 */
  }

  rmsmi_info_ptr = (dsumts_rmsmi_info_type*)sm_ptr->vobj_data_ptr;

  if (rmsmi_info_ptr == NULL) 
  {
    DATA_ERR_FATAL("rmsmi_info_ptr = NULL");
    /*lint -e527 */
    return;
    /*lint +e527 */
  }
  inst.rm_dev_instance = rmsmi_info_ptr->instance;
  inst.sm_instance = sm_ptr->instance;

  info_ptr = rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr;
  ASSERT(info_ptr != NULL);

  DS_3GPP_MSG2_HIGH( "Recd UM_IFACE_UP_EV, sm type: %d state:%d", 
          sm_ptr->instance,sm_ptr->pdp_specific_info.pdp_ip.state);

  switch( sm_ptr->pdp_specific_info.pdp_ip.state )
  {
    case DSUMTS_RMSM_IP_WAITING_FOR_UM_UP_STATE:
    case DSUMTS_RMSM_IP_WAITING_FOR_UM_ARB:

      if (sm_ptr->arb_close_in_progress == TRUE)
      {
        DS_3GPP_MSG0_HIGH("Ignoring Iface UP event because arb "
                          "close in progress");
        return;
      }

      /*---------------------------------------------------------------------
        Um iface is up. Get PPP parameters from Um iface
      ---------------------------------------------------------------------*/
      ps_iface_routeable_ind( sm_ptr->um_iface_ptr );
      (void) ps_iface_configuring_ind(&sm_ptr->sio_iface);

      // Set the UpLink bridge from SIO Iface to Um iface
      ps_iface_set_bridge( &sm_ptr->sio_iface, sm_ptr->um_iface_ptr );

      if (sm_ptr->instance == DSUMTS_RMSM_V4_SM_INSTANCE)
      {
        if( -1 == ps_iface_set_addr_family(&sm_ptr->sio_iface,
                                            IFACE_IPV4_ADDR_FAMILY))
        {
          DS_3GPP_MSG1_ERROR("Error while setting the Address Family to V4,"
                             " Iface ptr: %x", &sm_ptr->sio_iface);
          dsumts_rmsm_ip_transition_state
          ( 
            sm_ptr, 
            DSUMTS_RMSM_IP_WAITING_FOR_UM_DOWN_STATE 
          );
                                        
          break;
        }

        else 
        { 
      
          if (info_ptr->configure_ppp_f_ptr == NULL)
          {
            DATA_ERR_FATAL("Configure PPP Function is NULL");
            return;
          }
      
          if (info_ptr->configure_ppp_f_ptr ((void*)rmsmi_info_ptr, 
                                              PPP_PROTOCOL_IPCP) == FALSE)
          {
            DS_3GPP_MSG0_ERROR("PPP Configuration failed");
      
            dsumts_rmsm_ip_transition_state
            ( 
              sm_ptr, 
              DSUMTS_RMSM_IP_WAITING_FOR_UM_DOWN_STATE 
            );
          }
          else
          {
            DS_3GPP_MSG0_MED("PPP Configuration successful");
            dsumts_rmsm_ip_transition_state
            ( 
              sm_ptr,
              DSUMTS_RMSM_IP_WAITING_FOR_RM_PPP_UP_STATE 
            );
          } 

        } 
      }

      else if (sm_ptr->instance == DSUMTS_RMSM_V6_SM_INSTANCE)
      {
        PS_IFACE_SET_IS_BCAST_IFACE( &sm_ptr->sio_iface, TRUE );
        sm_ptr->sio_iface.group_mask = (uint16)RM_GROUP;
        ds_3gpp_rmsm_autoconfig_sm_init_config (sm_ptr);
        ds_3gpp_rmsm_autoconfig_sm_post_event (&inst, 
                                              AUTOCONFIG_SM_START_ADDR_CFG_EV);
        dsumts_rmsm_ip_transition_state
        ( 
          sm_ptr,
          DSUMTS_RMSM_IP_WAITING_FOR_RM_PPP_UP_STATE 
        );
      }

      else
      {
        DS_3GPP_MSG1_ERROR("Invalid PDP Type %d", 
                           DSUMTS_RMSMI_GET_PDP_TYPE(rmsmi_info_ptr));
        ASSERT(0);
      }
       
      break;


    case DSUMTS_RMSM_IP_WAITING_FOR_AUTOCONFIG_COMPLETE_STATE:
    case DSUMTS_RMSM_IP_WAITING_FOR_RM_PPP_UP_STATE:
    case DSUMTS_RMSM_IP_UM_RM_UP_STATE:
    case DSUMTS_RMSM_IP_WAITING_FOR_UM_DOWN_STATE:
    case DSUMTS_RMSM_IP_NULL_STATE:
      /*---------------------------------------------------------------------
        Ignore event in all other states.
      ---------------------------------------------------------------------*/
      DS_3GPP_MSG2_HIGH( "Ignoring UM_IFACE_UP_EV Sm type :%d state:%d",
                sm_ptr->instance,sm_ptr->pdp_specific_info.pdp_ip.state);
      return;

    default:

      /*---------------------------------------------------------------------
        Invalid state
      ---------------------------------------------------------------------*/
      DS_3GPP_MSG1_ERROR( "Invalid RMSM state %d",
           sm_ptr->pdp_specific_info.pdp_ip.state);
      ASSERT( 0 );
      break;

  } /* switch( sm_ptr->pdp_specific_info.pdp_ip.st*/

  return;
} /* dsumts_rmsm_ip_process_um_iface_up_ev() */

/*===========================================================================
  FUNCTION DSUMTS_RMSM_IP_PROCESS_UM_IFACE_DOWN_EV()

  DESCRIPTION
    This function processes UM_IFACE_DOWN_EV event. 
    
  PARAMETERS
    sm_ptr: Pointer to the RmSm IP State Machine

  RETURN VALUE
    None

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
void dsumts_rmsm_ip_process_um_iface_down_ev
(
  dsumts_rmsmi_sm_info_type *sm_ptr
)
{
  dsumts_rmsmi_info_type* rmsmi_info_ptr = NULL;
  dsumts_rmsm_instance_type inst;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (sm_ptr == NULL) 
  {
    DATA_ERR_FATAL("sm_ptr = NULL");
    /*lint -e527 */
    return;
    /*lint +e527 */
  }

  rmsmi_info_ptr = (dsumts_rmsmi_info_type*)sm_ptr->vobj_data_ptr;

  if (rmsmi_info_ptr == NULL) 
  {
    DATA_ERR_FATAL("sm_ptr = NULL");
    /*lint -e527 */
    return;
    /*lint +e527 */
  }
        
  inst.rm_dev_instance = rmsmi_info_ptr->instance;
  inst.sm_instance = sm_ptr->instance;

  DS_3GPP_MSG2_HIGH( "Recd UM_IFACE_DOWN_EV, sm instance: %d state:%d", 
          sm_ptr->instance,sm_ptr->pdp_specific_info.pdp_ip.state);
  switch( sm_ptr->pdp_specific_info.pdp_ip.state )
  {
    case DSUMTS_RMSM_IP_WAITING_FOR_UM_DOWN_STATE:
    case DSUMTS_RMSM_IP_WAITING_FOR_UM_UP_STATE:
    case DSUMTS_RMSM_IP_WAITING_FOR_RM_PPP_UP_STATE:
    case DSUMTS_RMSM_IP_WAITING_FOR_UM_ARB:
        
      dsumts_rmsm_ip_transition_state( sm_ptr, DSUMTS_RMSM_IP_NULL_STATE );
        
        break;
      
    case DSUMTS_RMSM_IP_UM_RM_UP_STATE:
    case DSUMTS_RMSM_IP_RM_RECONFIGURING_STATE:
    case DSUMTS_RMSM_IP_WAITING_FOR_AUTOCONFIG_COMPLETE_STATE:
      if (sm_ptr->instance == DSUMTS_RMSM_V6_SM_INSTANCE)
      {
        ds_3gpp_rmsm_autoconfig_sm_post_event(&inst, 
                                             AUTOCONFIG_SM_STOP_ADDR_CFG_EV);
        dsumts_rmsm_ip_transition_state
        (
          sm_ptr,
          DSUMTS_RMSM_IP_UM_DOWN_WAITING_FOR_AUTOCONFIG_END_STATE
        );
      }

      else if (sm_ptr->instance == DSUMTS_RMSM_V4_SM_INSTANCE)
      {
        dsumts_rmsm_ip_transition_state( sm_ptr,
                                         DSUMTS_RMSM_IP_NULL_STATE );
      }

      else
      {
        DATA_ERR_FATAL("PDP type is neither IPv4 nor IPv6");
      }
 
      break;

    case DSUMTS_RMSM_IP_NULL_STATE:
      /*---------------------------------------------------------------------
        Ignore event in all other states.
      ---------------------------------------------------------------------*/
      DS_3GPP_MSG2_HIGH( "Ignoring UM_IFACE_DOWN_EV Sm type :%d state:%d",
                sm_ptr->instance,sm_ptr->pdp_specific_info.pdp_ip.state);
      break;

    default:

      /*---------------------------------------------------------------------
        Invalid state
      ---------------------------------------------------------------------*/
      DS_3GPP_MSG1_ERROR( "Invalid RMSM state %d",
           sm_ptr->pdp_specific_info.pdp_ip.state);
      ASSERT( 0 );
      break;

  } /* switch( sm_ptr->pdp_specific_info.pdp_ip.st */

} /* dsumts_rmsm_ip_process_um_iface_down_ev() */

/*===========================================================================
  FUNCTION DSUMTS_RMSM_IP_PROCESS_UM_IFACE_DOWN_ARB_EV()

  DESCRIPTION
    This function processes UMTS_RMSM_UM_IFACE_DOWN_ARB_EV event. 
    
  PARAMETERS
    sm_ptr: Pointer to the RmSm IP State Machine

  RETURN VALUE
    None

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
void dsumts_rmsm_ip_process_um_iface_down_arb_ev
(
  dsumts_rmsmi_sm_info_type *sm_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (sm_ptr == NULL) 
  {
    DATA_ERR_FATAL("sm_ptr = NULL");
    /*lint -e527 */
    return;
    /*lint +e527 */
  }

 DS_3GPP_MSG2_HIGH( "Recd UM_IFACE_DOWN_ARB_EV, sm instance: %d state:%d", 
          sm_ptr->instance,sm_ptr->pdp_specific_info.pdp_ip.state);

 switch( sm_ptr->pdp_specific_info.pdp_ip.state )
  {
    case DSUMTS_RMSM_IP_WAITING_FOR_UM_UP_STATE:
   case DSUMTS_RMSM_IP_WAITING_FOR_UM_ARB:
      /*---------------------------------------------------------------------
        Arbitration caused Iface teardown, transition to WAITNG_FOR_UM_ARB
      ---------------------------------------------------------------------*/
      DS_3GPP_MSG2_HIGH( "In arb, WAITING_FOR_UM_UP_STATE, "
                         "transition to WAITING_FOR_UM_ARB",
                         sm_ptr->instance,
                         sm_ptr->pdp_specific_info.pdp_ip.state);
      dsumts_rmsm_ip_transition_state( sm_ptr, 
                                        DSUMTS_RMSM_IP_WAITING_FOR_UM_ARB );
      break;

    case DSUMTS_RMSM_IP_WAITING_FOR_RM_PPP_UP_STATE:
    case DSUMTS_RMSM_IP_WAITING_FOR_AUTOCONFIG_COMPLETE_STATE:
    case DSUMTS_RMSM_IP_UM_RM_UP_STATE:
    case DSUMTS_RMSM_IP_WAITING_FOR_UM_DOWN_STATE:
    case DSUMTS_RMSM_IP_NULL_STATE:
      /*---------------------------------------------------------------------
        Ignore event in all other states.
      ---------------------------------------------------------------------*/
      DS_3GPP_MSG2_HIGH( "Ignoring UM_IFACE_DOWN_ARB_EV Sm type :%d state:%d",
                sm_ptr->instance,sm_ptr->pdp_specific_info.pdp_ip.state);
      break;

    default:

      /*---------------------------------------------------------------------
        Invalid state
      ---------------------------------------------------------------------*/
      DS_3GPP_MSG1_ERROR( "Invalid RMSM state %d",
                           sm_ptr->pdp_specific_info.pdp_ip.state);
      ASSERT( 0 );
      break;

  } /* switch( sm_ptr->pdp_specific_info.pdp_ip.state */

} /* dsumts_rmsm_ip_process_um_iface_down_arb_ev() */

/*===========================================================================
  FUNCTION DSUMTS_RMSM_IP_PROCESS_RM_PPP_UP_EV()
 
 DESCRIPTION
    This function processes RM_PPP_UP_EV event. 
 
 PARAMETERS
   	sm_ptr: Pointer to the RmSm IP State Machine
 
 RETURN VALUE
 None
 
 DEPENDENCIES
 none
 
 SIDE EFFECTS
 None
 ===========================================================================*/
void dsumts_rmsm_ip_process_rm_ppp_up_ev
(
  dsumts_rmsmi_sm_info_type *sm_ptr
)
{

  dsumts_rmsmi_info_type* rmsmi_info_ptr;
  DS_3GPP_MSG0_HIGH("Received Rm PPP Up EV");


  if (sm_ptr == NULL) 
  {
    DATA_ERR_FATAL("sm_ptr = NULL");
    /*lint -e527 */
    return;
    /*lint +e527 */
  }
  
  rmsmi_info_ptr = (dsumts_rmsmi_info_type*)sm_ptr->vobj_data_ptr;
  
  if (rmsmi_info_ptr == NULL) 
  {
    DATA_ERR_FATAL("rmsmi_info_ptr = NULL");
    /*lint -e527*/
    return;
    /*lint +e527*/
  }
      
  DS_3GPP_MSG3_HIGH( "Processing Rm PPP UP EV, sm instance:%d state:%d", 
           sm_ptr->instance,sm_ptr->pdp_specific_info.pdp_ip.state, 0);

  switch( sm_ptr->pdp_specific_info.pdp_ip.state )
  {
    case DSUMTS_RMSM_IP_WAITING_FOR_RM_PPP_UP_STATE:
    {
      if (sm_ptr->instance == DSUMTS_RMSM_V6_SM_INSTANCE)
      {
        dsumts_rmsm_ip_transition_state( sm_ptr,
                DSUMTS_RMSM_IP_WAITING_FOR_AUTOCONFIG_COMPLETE_STATE);
      }
      
      else if (sm_ptr->instance == DSUMTS_RMSM_V4_SM_INSTANCE)
      {
      /*---------------------------------------------------------------------
        Setup IP fwd link bridging between SIO and UM 
        interface
       ---------------------------------------------------------------------*/
        ps_iface_set_bridge( sm_ptr->um_iface_ptr, 
                             &sm_ptr->sio_iface );
        dsumts_rmsm_ip_transition_state( sm_ptr,DSUMTS_RMSM_IP_UM_RM_UP_STATE );
      }

      else
      {
         DS_3GPP_MSG1_ERROR("Rm PPP UP EV received for wrong PDP Type: %d",
                            DSUMTS_RMSMI_GET_PDP_TYPE(rmsmi_info_ptr));
      }

      break;
    }
      
    case DSUMTS_RMSM_IP_UM_RM_UP_STATE:
    {
       DS_3GPP_MSG0_HIGH("Ignoring ppp_up_ev:"
                        "Already in DSUMTS_RMSM_IP_UM_RM_UP_STATE ");
       break;
    }
      
    default:
     /*---------------------------------------------------------------------
        rmsm_ip sm state for v6 call is not reset after previous successfuly
        call connect and disconnect as per protocol design and they may post
        up event for v6. Just ignore rm ppp up if we are not expecting
      ---------------------------------------------------------------------*/
     DS_3GPP_MSG1_ERROR("PPP Configured EV received in wrong state %d", 
                        sm_ptr->pdp_specific_info.pdp_ip.state);
  } /*switch( sm_ptr->pdp_specific_info.pdp_ip.state )*/
      
} /*dsumts_rmsm_ip_process_rm_ppp_up_ev*/


/*===========================================================================
  FUNCTION DSUMTS_RMSM_IP_PROCESS_RM_PPP_DOWN_EV()

  DESCRIPTION
    This function processes RM_PPP_DOWN_EV event. 
    
  PARAMETERS
    sm_ptr: Pointer to the RmSm IP State Machine

  RETURN VALUE
    None

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
void dsumts_rmsm_ip_process_rm_ppp_down_ev
(
  dsumts_rmsmi_sm_info_type *sm_ptr
)
{
  dsumts_rmsmi_info_type* rmsmi_info_ptr = NULL;
  dsumts_rmsm_instance_type inst;
  /*---------------------------------------------------------*/
  DS_3GPP_MSG0_HIGH("Received Rm PPP down EV");

  if (sm_ptr == NULL) 
  {
    DATA_ERR_FATAL("sm_ptr = NULL");
    /*lint -e527*/
    return;
    /*lint +e527*/
  }
      
  rmsmi_info_ptr = (dsumts_rmsmi_info_type*)sm_ptr->vobj_data_ptr;
  
  if (rmsmi_info_ptr == NULL) 
  {
    DATA_ERR_FATAL("rmsmi_info_ptr = NULL");
    /*lint -e527*/
    return;
    /*lint +e527*/
  }

  inst.rm_dev_instance = rmsmi_info_ptr->instance;
  inst.sm_instance = sm_ptr->instance;

  DS_3GPP_MSG3_HIGH( "Processing Rm PPP down EV, sm instance:%d state:%d", 
           sm_ptr->instance,sm_ptr->pdp_specific_info.pdp_ip.state, 0);

  switch( sm_ptr->pdp_specific_info.pdp_ip.state )
  {
    case DSUMTS_RMSM_IP_WAITING_FOR_AUTOCONFIG_COMPLETE_STATE:
    case DSUMTS_RMSM_IP_UM_RM_UP_STATE:
    case DSUMTS_RMSM_IP_RM_RECONFIGURING_STATE:
    case DSUMTS_RMSM_IP_WAITING_FOR_RM_PPP_UP_STATE:
    {
      if (sm_ptr->instance == DSUMTS_RMSM_V6_SM_INSTANCE)
      {
        ds_3gpp_rmsm_autoconfig_sm_post_event(&inst, 
                                              AUTOCONFIG_SM_STOP_ADDR_CFG_EV);
        dsumts_rmsm_ip_transition_state
        (
          sm_ptr,
          DSUMTS_RMSM_IP_UM_UP_WAITING_FOR_AUTOCONFIG_END_STATE
        );
      }

      else if (sm_ptr->instance == DSUMTS_RMSM_V4_SM_INSTANCE)
      {
        dsumts_rmsm_ip_transition_state
        (
          sm_ptr,
          DSUMTS_RMSM_IP_WAITING_FOR_UM_DOWN_STATE
        );
      }

      else
      {
         DS_3GPP_MSG1_ERROR("Rm PPP DOWN EV received for wrong PDP Type: %d",
                            DSUMTS_RMSMI_GET_PDP_TYPE(rmsmi_info_ptr));
         ASSERT(0);
      }

      break;
    }

    case DSUMTS_RMSM_IP_WAITING_FOR_UM_UP_STATE:
    case DSUMTS_RMSM_IP_WAITING_FOR_UM_ARB:
      dsumts_rmsm_ip_transition_state
      (
        sm_ptr,
        DSUMTS_RMSM_IP_WAITING_FOR_UM_DOWN_STATE
      );
      break;

    case DSUMTS_RMSM_IP_UM_UP_WAITING_FOR_AUTOCONFIG_END_STATE:
    case DSUMTS_RMSM_IP_UM_DOWN_WAITING_FOR_AUTOCONFIG_END_STATE:
    case DSUMTS_RMSM_IP_WAITING_FOR_UM_DOWN_STATE:
    case DSUMTS_RMSM_IP_NULL_STATE:
      DS_3GPP_MSG3_HIGH("Rm PPP Down EV received in state %d, ignoring", 
                         sm_ptr->pdp_specific_info.pdp_ip.state,0,0);
      break;

    default:
     DS_3GPP_MSG1_ERROR("PPP Configured EV received in wrong state %d", 
                        sm_ptr->pdp_specific_info.pdp_ip.state);
     ASSERT(0);
  } /* switch( sm_ptr->pdp_specific_info.pdp_ip.state )*/
} /*dsumts_rmsm_ip_process_rm_ppp_down_ev*/

/*===========================================================================
  FUNCTION DSUMTS_RMSM_IP_PROCESS_ADDR_CFG_FAILURE_EV()

  DESCRIPTION
    This function processes ADDR_CFG_FAILURE_EV event. 

  PARAMETERS
    sm_ptr: Pointer to the RmSm IP State Machine

  RETURN VALUE
    None

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
void dsumts_rmsm_ip_process_addr_cfg_failure_ev
(
  dsumts_rmsmi_sm_info_type *sm_ptr
)
{
  dsumts_rmsmi_info_type* rmsmi_info_ptr;
  dsumts_rmsm_instance_type inst;

  /*------------------------------------------------------------*/

  DS_3GPP_MSG0_HIGH("Address Configuration failure EV");

  if (sm_ptr == NULL) 
  {
    DATA_ERR_FATAL("sm_ptr = NULL");
    /*lint -e527*/
    return;
    /*lint +e527*/
  }

  rmsmi_info_ptr = (dsumts_rmsmi_info_type*)sm_ptr->vobj_data_ptr;

  if (rmsmi_info_ptr == NULL) 
      {
    DATA_ERR_FATAL("rmsmi_info_ptr = NULL");
    /*lint -e527*/
    return;
    /*lint +e527*/
      }

  inst.rm_dev_instance = rmsmi_info_ptr->instance;
  inst.sm_instance = sm_ptr->instance;

  DS_3GPP_MSG3_HIGH( "Processing ADDR_CFG_FAILURE_EV, sm instance:%d state:%d", 
           sm_ptr->instance,sm_ptr->pdp_specific_info.pdp_ip.state, 0);

  if (sm_ptr->instance == DSUMTS_RMSM_V6_SM_INSTANCE)
  {
    switch( sm_ptr->pdp_specific_info.pdp_ip.state )
    {
      case DSUMTS_RMSM_IP_WAITING_FOR_RM_PPP_UP_STATE:
      case DSUMTS_RMSM_IP_RM_RECONFIGURING_STATE:

        ds_3gpp_rmsm_autoconfig_sm_post_event ( &inst, 
                                                AUTOCONFIG_SM_STOP_ADDR_CFG_EV );

        dsumts_rmsm_ip_transition_state
        (
          sm_ptr,
          DSUMTS_RMSM_IP_WAITING_FOR_UM_DOWN_STATE
        );

    break;

    default:
       DS_3GPP_MSG1_ERROR("Address Configuration Failure EV received in wrong "
                          "state %d", sm_ptr->pdp_specific_info.pdp_ip.state);
       ASSERT(0);
    }
  }
  else
  {
    DS_3GPP_MSG1_ERROR("Addr Cfg Failure EV received for wrong PDP Type: %d",
                        DSUMTS_RMSMI_GET_PDP_TYPE(rmsmi_info_ptr));
  }

} /*dsumts_rmsm_ip_process_addr_cfg_failure_ev*/


/*===========================================================================
  FUNCTION DSUMTS_RMSM_IP_PROCESS_ADDR_CFG_CHANGED_EV()

  DESCRIPTION
    This function processes ADDR_CFG_CHANGED_EV event. 

  PARAMETERS
    sm_ptr: Pointer to the RmSm IP State Machine

  RETURN VALUE
    None

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
 void dsumts_rmsm_ip_process_addr_cfg_changed_ev
(
  dsumts_rmsmi_sm_info_type *sm_ptr
)
{
  dsumts_rmsmi_info_type* rmsmi_info_ptr;

  DS_3GPP_MSG0_HIGH("Address Configuration changed");

  if (sm_ptr == NULL) 
  {
    DATA_ERR_FATAL("sm_ptr = NULL");
    /*lint -e527*/
    return;
    /*lint +e527*/
  }

  rmsmi_info_ptr = (dsumts_rmsmi_info_type*)sm_ptr->vobj_data_ptr;

  if (rmsmi_info_ptr == NULL) 
  {
    DATA_ERR_FATAL("rmsmi_info_ptr = NULL");
    /*lint -e527*/
    return;
    /*lint +e527*/
  }

  DS_3GPP_MSG3_HIGH("Processing Address Configuration changed EV, sm instance:%d state:%d", 
                     sm_ptr->instance, sm_ptr->pdp_specific_info.pdp_ip.state ,0);


  if (sm_ptr->instance == DSUMTS_RMSM_V6_SM_INSTANCE)
  {
    switch( sm_ptr->pdp_specific_info.pdp_ip.state )
    {
      case DSUMTS_RMSM_IP_UM_RM_UP_STATE:
        dsumts_rmsm_ip_transition_state
        (
          sm_ptr,
          DSUMTS_RMSM_IP_RM_RECONFIGURING_STATE
        );

        break;

      default:
       DS_3GPP_MSG1_ERROR("Address Configuration Changed EV received in wrong "
                          "state %d", sm_ptr->pdp_specific_info.pdp_ip.state);
       ASSERT(0);
    }
  }
  else
  {
    DS_3GPP_MSG1_ERROR("Addr Cfg Changed EV received for wrong PDP Type: %d",
                        DSUMTS_RMSMI_GET_PDP_TYPE(rmsmi_info_ptr));
  }
  
} /*dsumts_rmsm_ip_process_addr_cfg_changed_ev*/
    

/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                          LOCAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/
  
/*===========================================================================
  FUNCTION DSUMTS_RMSM_IP_TRANSITION_STATE()

  DESCRIPTION
    This function effects the transition to the state that is passed in.

  PARAMETERS
    sm_ptr: Pointer to the RmSm IP State Machine
    new_state: the state to transition to

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void dsumts_rmsm_ip_transition_state
(
  dsumts_rmsmi_sm_info_type       *sm_ptr,
  dsumts_rmsm_ipi_state_type  new_state
)
{
  /*lint -e655*/
  int16  ps_iface_errno = 0;

  int32  app_priority   = PS_POLICY_MGR_PRIORITY_INVALID;
  int32  retval         = 0;

  dsumts_rmsmi_info_type* rmsmi_info_ptr;
  dsumts_rmsm_instance_type inst;
  ds_3gpp_rmsm_ip_common_info_type *info_ptr = NULL;
  ps_iface_down_mh_event_info_type   down_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  
  memset(&down_info, 0, sizeof(ps_iface_down_mh_event_info_type));

  down_info.down_reason = PS_NET_DOWN_REASON_CLIENT_END;

  down_info.bearer_tech.technology = DS_SYS_NETWORK_3GPP;
  down_info.bearer_tech.so_mask = DS_SYS_SO_EX_UNSPECIFIED;
  down_info.bearer_tech.rat_value = DS_SYS_RAT_UNKNOWN;

  if (sm_ptr == NULL) 
  {
    DATA_ERR_FATAL("sm_ptr = NULL");
    /*lint -e527 */
    return;
    /*lint +e527 */
  }

  rmsmi_info_ptr = (dsumts_rmsmi_info_type*)sm_ptr->vobj_data_ptr;

  if (rmsmi_info_ptr == NULL) 
  {
    DATA_ERR_FATAL("sm_ptr = NULL");
    /*lint -e527 */
    return;
    /*lint +e527 */
  }

  inst.rm_dev_instance = rmsmi_info_ptr->instance;
  inst.sm_instance = sm_ptr->instance;

  info_ptr = rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr;
  ASSERT(info_ptr != NULL);

  DS_3GPP_MSG3_HIGH( "DSUMTS_RMSM:%lx state change from %d to %d", 
          (uint32)sm_ptr,sm_ptr->pdp_specific_info.pdp_ip.state, new_state );
      

  sm_ptr->pdp_specific_info.pdp_ip.state = new_state;
      
  /*-------------------------------------------------------------------------
    Perform action associated with entering into new state.
  -------------------------------------------------------------------------*/
  switch( new_state )
  {
    case DSUMTS_RMSM_IP_NULL_STATE:
      {
      dsumts_rmsmi_dereg_um_events(sm_ptr);

      /*---------------------------------------------------------------------
        Restore SIO and Um interfaces to correct state
      ---------------------------------------------------------------------*/

      PS_IFACE_SET_IS_BCAST_IFACE( &sm_ptr->sio_iface, FALSE );
      
      ps_iface_set_bridge( &sm_ptr->sio_iface, NULL );
      if ( PS_IFACE_IS_VALID(sm_ptr->um_iface_ptr) )
      {
        ps_iface_set_bridge( sm_ptr->um_iface_ptr, NULL );
      }

      sm_ptr->um_iface_ptr = NULL;
      sm_ptr->um_phys_lnk_ptr = NULL;
      
      ps_iface_phys_link_down_ind(&sm_ptr->sio_iface);
      ps_iface_down_ind( &sm_ptr->sio_iface ,&down_info);

      break;

    } /* DSUMTS_RMSM_IP_NULL_STATE */

    case DSUMTS_RMSM_IP_WAITING_FOR_UM_UP_STATE:
    {
    
      /*---------------------------------------------------------------------
        Grab a Um interface : If a UM iface is allocated, it is automatically
        locked (acl) and net model state machine initied. 
        If the iface allocation fails then RMSM moves to NULL_STATE        
      ---------------------------------------------------------------------*/
      sm_ptr->um_iface_ptr = dsumts_rmsmi_get_um_iface_ptr(sm_ptr);
      if ( sm_ptr->um_iface_ptr == NULL )
      {
        /*-----------------------------------------------------------------
         No interface available, transition RMSM to NULL state
        -----------------------------------------------------------------*/
        DS_3GPP_MSG0_HIGH("um_iface is not available: Shutdown call");
        dsumts_rmsm_post_event(&inst, UMTS_RMSM_UM_IFACE_DOWN_EV);
        break;
      }   

     /*---------------------------------------------------------------------
        Bring up the interface. If there is an error, bring iface to down 
        state which will post appropriate event to bring down the laptop 
        call.
      ---------------------------------------------------------------------*/
      DS_3GPP_MSG2_HIGH( "route lookup ret Um Iface [0x%x:%x], "
                         "try to bring-up by policy", 
                         sm_ptr->um_iface_ptr->name, 
                         sm_ptr->um_iface_ptr->instance);

      retval = dsumts_rmsmi_bring_up_um_iface_by_policy( sm_ptr, 
                                                        &sm_ptr->um_iface_ptr, 
                                                        &ps_iface_errno );

      if ( retval == 0 )
      {
        /* UP event handler will be called when we reg below */
        DS_3GPP_MSG3_HIGH( "bring up ret Um Iface [0x%x:%x] in state[%d], post UP", 
                           sm_ptr->um_iface_ptr->name, 
                           sm_ptr->um_iface_ptr->instance, 
                           PS_IFACEI_GET_STATE(sm_ptr->um_iface_ptr));

        /* post IFACE UP event internally */
        dsumts_rmsm_post_event( &inst, UMTS_RMSM_UM_IFACE_UP_EV );
      }

      else if ( ( retval == -1 ) && ( ps_iface_errno == DS_EWOULDBLOCK ) )
      {
        /*-------------------------------------------------------------------
          Bringup in progress.
        -------------------------------------------------------------------*/
        DS_3GPP_MSG1_HIGH("Um Iface 0x%x bringup in progress",
                           sm_ptr->um_iface_ptr);
      }

      else if ( ( retval == -1 ) && ( ps_iface_errno == DS_ENETCLOSEINPROGRESS ) )
      {
        /*-------------------------------------------------------------------
          Arbitration resulted in Iface teardown in progress. Register new
          set of Iface event handlers for arbitration.
        -------------------------------------------------------------------*/
        DS_3GPP_MSG1_HIGH("Um Iface 0x%x arb close in progress, set flag", 
                           sm_ptr->um_iface_ptr);
        sm_ptr->arb_close_in_progress = TRUE;
      }

      else /* unhandled */
      {

        DS_3GPP_MSG2_ERROR( "Um Iface bringup failed! retval [%d], "
                            "err [%d], BAIL-OUT",  retval, ps_iface_errno);
        /*-------------------------------------------------------------------
          Um iface could not be brought up, post the appropriate event so
          that the call on Rm can be cleaned up
        -------------------------------------------------------------------*/
        dsumts_rmsm_post_event( &inst, UMTS_RMSM_UM_IFACE_DOWN_EV );
        break;
      }

      PS_IFACE_SET_GRACEFUL_CLOSE(sm_ptr->um_iface_ptr,TRUE);

      if ( !dsumts_rmsmi_reg_um_events( sm_ptr ) )
      {
        /*-------------------------------------------------------------------
          Um Iface event registration failed.
        -------------------------------------------------------------------*/
        DS_3GPP_MSG0_ERROR( "Um Iface event reg failed");
        dsumts_rmsm_post_event( &inst, UMTS_RMSM_UM_IFACE_DOWN_EV );
        break;
      } 

      break;

    }/* DSUMTS_RMSM_IP_WAITING_FOR_UM_UP_STATE */

    case DSUMTS_RMSM_IP_WAITING_FOR_UM_ARB:
    {
      /*--------------------------------------------------------------------
          Deregister all the Um interface, flow, and phys link event callbacks
      ---------------------------------------------------------------------*/
      /* this routine tries to dereg physlink events too, confirm it's ok */
      DS_3GPP_MSG0_HIGH( "RmSM IP Arb, entered "
                         "DSUMTS_RMSM_IP_WAITING_FOR_UM_ARB");

      dsumts_rmsmi_dereg_um_events( sm_ptr );

      /*---------------------------------------------------------------------
        Attempt to bring up Iface by policy
      ---------------------------------------------------------------------*/
      if ( dsumts_rmsmi_bring_up_um_iface_by_policy( sm_ptr, 
                                                    &sm_ptr->um_iface_ptr, 
                                                    &ps_iface_errno ) == 0 )
      {
        DS_3GPP_MSG1_HIGH( "RmSM IP Arb: Um Iface 0x%x is already up, "
                           "unexpected, post UP ev", 
                  sm_ptr->um_iface_ptr);

        sm_ptr->arb_close_in_progress = FALSE;

        dsumts_rmsm_post_event( &inst, UMTS_RMSM_UM_IFACE_UP_EV );
      }
      else if( ps_iface_errno == DS_EWOULDBLOCK )
      {
        /*-------------------------------------------------------------------
            Bringup in progress.
        -------------------------------------------------------------------*/
        DS_3GPP_MSG1_HIGH( "RmSM IP Arb: Um Iface 0x%x bringup in progress", 
                  sm_ptr->um_iface_ptr);
        sm_ptr->arb_close_in_progress = FALSE;
      }

      else if( ps_iface_errno == DS_ENETCLOSEINPROGRESS )
      {
        /*-------------------------------------------------------------------
            Close in progress.
        -------------------------------------------------------------------*/
        DS_3GPP_MSG1_HIGH( "RmSM IP Arb: Um Iface 0x%x close in progress", 
                  sm_ptr->um_iface_ptr);
        sm_ptr->arb_close_in_progress = TRUE;
      }

      else
      {
        DS_3GPP_MSG2_ERROR( "RmSM IP Arb: Um Iface 0x%x, "
                            "unhandled errno [%d], BAIL-OUT", 
                   sm_ptr->um_iface_ptr, ps_iface_errno);
        sm_ptr->arb_close_in_progress = FALSE;
        dsumts_rmsm_post_event( &inst, UMTS_RMSM_UM_IFACE_DOWN_EV );
        break;
      }

      /*-------------------------------------------------------------------
        Register all the Um interface, flow, and phys link event callbacks
      -------------------------------------------------------------------*/
      if ( !dsumts_rmsmi_reg_um_events( sm_ptr ) )
      {
        /*-------------------------------------------------------------------
          Um Iface event registration failed.
        -------------------------------------------------------------------*/
        DS_3GPP_MSG0_ERROR( "Um Iface event reg failed, "
                            "post UMTS_RMSM_UM_IFACE_DOWN_EV");
        dsumts_rmsm_post_event( &inst, UMTS_RMSM_UM_IFACE_DOWN_EV );
        break;
      }

      break;
    } /* DSUMTS_RMSM_IP_WAITING_FOR_UM_ARB */

    case DSUMTS_RMSM_IP_UM_RM_UP_STATE:
      ps_iface_up_ind( &sm_ptr->sio_iface );
      ps_iface_phys_link_up_ind(&sm_ptr->sio_iface);
      break;

    case DSUMTS_RMSM_IP_WAITING_FOR_UM_DOWN_STATE:

      /*---------------------------------------------------------------------
        Get priority of TE RmNET application
      ---------------------------------------------------------------------*/
      app_priority = ps_policy_mgr_get_priority( APP_ID_TE_DUN, 
                                                 PS_POLICY_MGR_ANY_PROFILE );

      /*---------------------------------------------------------------------
        Since laptop has terminated the call, we need to cleanup Um before
        completely terminating the call.  Tear it down now.
        If tear down cmd fails, Um is already be down.
      ---------------------------------------------------------------------*/
      if ( ps_iface_tear_down_cmd_ex( sm_ptr->um_iface_ptr,
                                      app_priority,
                                     &ps_iface_errno,
                                      NULL ) == 0 )
      {  
        /*-------------------------------------------------------------------
          If the UM packet iface is already down, we should cleanup the
          call on Rm since we won't get any other notification from UM
          controller
        -------------------------------------------------------------------*/
        DS_3GPP_MSG0_HIGH("Um iface already down.");
        dsumts_rmsm_post_event(&inst, UMTS_RMSM_UM_IFACE_DOWN_EV);
      }
      DS_3GPP_MSG2_HIGH( "PS Iface teardown prio [%d], errno [%d]", 
                app_priority, ps_iface_errno);

        break;
  
    case DSUMTS_RMSM_IP_WAITING_FOR_RM_PPP_UP_STATE:
    case DSUMTS_RMSM_IP_WAITING_FOR_AUTOCONFIG_COMPLETE_STATE:
    case DSUMTS_RMSM_IP_UM_UP_WAITING_FOR_AUTOCONFIG_END_STATE:
    case DSUMTS_RMSM_IP_UM_DOWN_WAITING_FOR_AUTOCONFIG_END_STATE:
    case DSUMTS_RMSM_IP_RM_RECONFIGURING_STATE:
      break;

      default:
        DS_3GPP_MSG1_ERROR("Invalid state %d - transistion", new_state);
    ASSERT(0);
        break;

  } /* switch(new state) */
  /*lint +e655*/
} /* dsumts_rmsm_ip_transition_state() */

//Description
dsumts_rmsm_ipi_state_type dsumts_rmsm_ip_get_state
(
  dsumts_rmsmi_sm_info_type *sm_ptr
)
{
  if (sm_ptr == NULL)
  {
    DATA_ERR_FATAL("Invalid sm ptr passed");
  }

  return sm_ptr->pdp_specific_info.pdp_ip.state;
}

#endif /* FEATURE_DATA_WCDMA_PS || FEATURE_GSM_GPRS || FEATURE_DATA_LTE*/

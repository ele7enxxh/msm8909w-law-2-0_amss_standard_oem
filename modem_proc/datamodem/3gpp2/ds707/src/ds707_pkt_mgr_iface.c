/*===========================================================================

                  D S 7 0 7 _ P K T _ M G R _ I F A C E . C
                  
GENERAL DESCRIPTION
  This file is the controller for 1X packet calls and the PS_IFACE.
  
EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2002 - 2010 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                           EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_pkt_mgr.c_v   1.45   25 Feb 2003 14:25:02   ajithp  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_pkt_mgr_iface.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
10/13/10    op     Migrated to MSG 2.0 macros
22/10/09    ca     Fixed Lint Errors.
03/20/09    spn    changed ds707_pkt_state to ds707_pdn_state
07/07/08    ms     Klocwork Fixes
07/02/07    sc     Added support for Application Profiles and Dormant2 (Iface
                   Lingering) state in ds707_pkt_mgr_iface_init().
10/13/05    az     Support for IPv6
08/30/05    ks     Added a paramter to PS_FLOW_SET_CB_F_PTR macro.
08/08/05   sk/sy   Merged semi-dynamic QOS changes from sandbox.
08/05/05   sk/vs   Merged the HDR Semi-Dynamic QOS changes from sandbox.
07/26/05    vas    Added suport for rgistering 707 flow callback functions
08/02/04    ak     Added function for getting ipv6 iface.
07/30/04    kvd    Removed pkti_iface_up/pkti_iface_down and corr. cmd hdlrs.
06/11/04    vp     Changes due to changes in ip_addr_type being changes to
                   ps_ip_addr_type.
04/24/04    ak     Lint fixes.
04/11/04    gr     Set the value of force_cdma_only to FALSE for all
                   dormancy re-origination scenarios
03/24/04    ak     On dorm_status, check for IFACE_DOWN and DISABLED.
03/22/04    ak     Added function to see if it's okay to orig from dorm.
03/19/04    ak     Removed ifdef around get_iface_ptr; added extern for
                   get_ipv4_iface_ptr().
03/18/04    ak     F3 msgs have iface references, since there are now 2.
02/19/04    ak     Updated for IPV4 & IPV6 iface's.
02/19/04    ak     fixed commands to pass iface ptr.
11/04/04    rsl    Added code under FEATURE_HDR to get correct data in 
                   get_data_count_hdlr and get_call_info_hdlr.
10/30/03    vr     Inform hdrpac if there is a handdown from HDR to 1X under
                   FEATURE_HDR_PPP_RESYNC_ON_HANDDOWN
10/19/03    vas    End Key causes PPP abort if 
                   FEATURE_HDR_SESSION_TIED_TO_PPP is defined
02/20/02    ak     First version of file.
===========================================================================*/


/*===========================================================================
                          INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707
  #define FEATURE_DSM_WM_CB

  #include "amssassert.h"
  #include "ds707_pkt_mgr_ioctl.h"
  #include "ds707_pkt_mgri.h"
  #include "dstask.h"
  #include "dstaski.h"
  #include "msg.h"
  #include "data_msg.h"
  #include "ps_iface.h"
  #include "ps_ip_addr.h"
  #include "ps_route.h"
  #include "ds707_pkt_mgr_flow.h"
  #include "ds707_data_session_profile.h"
  #include "ds707_pdn_context.h"


/*===========================================================================
                        FORWARD DECLARATIONS
===========================================================================*/

/*===========================================================================
                              TYPEDEFS
===========================================================================*/

/*===========================================================================
                              VARIABLES
===========================================================================*/

/*===========================================================================
                       INTERNAL MACRO DEFINITIONS
===========================================================================*/
/*-------------------------------------------------------------------
  Connection release delay timer in ms
-------------------------------------------------------------------*/
#define CONNECTION_CLOSE_DELAY_TIME_MSEC 1000

/*===========================================================================
                       EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
FUNCTION      DS707_PKT_GET_IFACE_PTR_FROM_PS_FLOW

DESCRIPTION   Given a flow pointer, returns the pointer to the
              corresponding iface.

DEPENDENCIES  None.

RETURN VALUE  pointer to the IFACE ptr - whether v4 or v6.

SIDE EFFECTS  None.
===========================================================================*/
ps_iface_type * ds707_pkt_get_iface_ptr_from_ps_flow(
                                       ps_flow_type * ps_flow
                                       )
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if(ps_flow == NULL)
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, "NULL PS FLOW");
    return NULL;
  }
  return PS_FLOW_GET_IFACE(ps_flow);
} /* ds707_pkt_get_iface_ptr_from_ps_flow() */

/*===========================================================================
FUNCTION      DS707_PKT_SET_IFACE_PTR

DESCRIPTION   Creates a mapping between PS_FLOW and PS_IFACE. Using a flow 
              ptr, one can access the corresponding iface through
              ds707_pkt_get_iface_ptr_from_ps_flow()

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pkt_set_iface_ptr(
                            ps_flow_type    * ps_flow,
                            ps_iface_type   * iface_ptr
                            )
{
  ds707_flow_type* ds_flow_ptr = NULL;
 
  ds_flow_ptr = ds707_pkt_mgr_get_ds_flow(ps_flow);
  if(ds_flow_ptr != NULL)
  {
   ds_flow_ptr->ps_iface_ptr = iface_ptr;
  }
  else
  {
   DATA_IS707_MSG1( MSG_LEGACY_HIGH,
                    "ds_flow_ ptr is NULL. Unable to set iface ptr 0x%x",
                    iface_ptr);
  }
  return;
}

/*===========================================================================
FUNCTION      DS707_PKT_IFACE_ENABLE_IND

DESCRIPTION   Called when user wants to enable the 707 pkt ifaces.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pkt_iface_enable_ind(void)
{
  uint8 i;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  for (i = 0; i < DS707_MAX_IFACES; i++)
  {
    ps_iface_enable_ind(ds707_pdncntx_get_iface_ptr_from_iface_tbl_index(i));
  }
} /* ds707_pkt_iface_enable_ind() */

/*===========================================================================
FUNCTION      DS707_PKT_IFACE_DISABLE_IND

DESCRIPTION   Called when user wants to disable the 707 pkt ifaces.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pkt_iface_disable_ind(void)
{
  uint8 i;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  for (i = 0; i < DS707_MAX_IFACES; i++)
  {
    ps_iface_disabled_ind(ds707_pdncntx_get_iface_ptr_from_iface_tbl_index(i));
  }
} /* ds707_pkt_iface_disable_ind() */

/*===========================================================================
FUNCTION      DS707_PKT_MGR_IFACE_INIT

DESCRIPTION   Called once at mobile power-up.  Registers the appropriate
              iface unctionality with PS IFACE.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None
===========================================================================*/
void ds707_pkt_mgr_iface_init
(
ps_iface_type             *iface_ptr,       /* ptr to 1x iface          */
acl_type                  *my_acl_ptr,      /* ptr to acl               */
ps_phys_link_type         *phys_link_ptr,   /* ptr to phys link(s)      */
uint8                      num_phys_links   /* num phys links passed in */
)
{
  ps_flow_type *default_flow_ptr = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /*-------------------------------------------------------------------------
    Create the PS_IFACE.
  -------------------------------------------------------------------------*/
  if (ps_iface_create(
                     iface_ptr,
                     CDMA_SN_IFACE,
                     my_acl_ptr,
                     NULL,
                     phys_link_ptr,
                     num_phys_links
                     ) < 0)
  {
    ERR_FATAL("Could not create ps iface",0,0,0);
  }
  /*-------------------------------------------------------------------------
    Perform phys link initialization to set to decoupled mode.
  -------------------------------------------------------------------------*/
  /*lint -save -e774 Reason for suppressing error 774*/
  /*lint -save -e641 Reason for suppressing error 641*/
  /*lint -e506 suppressing constant boolean value evaluation*/
  PS_PHYS_LINK_SET_CAPABILITY(phys_link_ptr, 
                              PS_PHYS_LINK_CAPABILITY_FLOW_DECOUPLED);
  /*lint -restore*/
  /*lint -restore Restore lint error 641*/
  /*lint -restore Restore lint error 774*/

  /*-------------------------------------------------------------------------
    These bring up/tear down the traffic channel.  However, in the case of
    the tear down, it also cleans up the PPP state (i.e., no dormancy). The
    linger_cmd_f_ptr causes the PPP to goto LINGERING state.
  -------------------------------------------------------------------------*/
  iface_ptr->bring_up_cmd_f_ptr   = NULL;
  iface_ptr->tear_down_cmd_f_ptr  = NULL;
  iface_ptr->linger_cmd_f_ptr     = NULL;

  /*-------------------------------------------------------------------------
    This is for doing ioctls specific to 1x pkt interface.
  -------------------------------------------------------------------------*/
  iface_ptr->iface_ioctl_f_ptr    = ds707_pkt_ioctl;


  /*-------------------------------------------------------------------------
    Set the function pointers to call for the default PS Flow
  -------------------------------------------------------------------------*/
  default_flow_ptr = PS_IFACE_GET_DEFAULT_FLOW(iface_ptr);
  PS_FLOW_SET_CB_F_PTR( default_flow_ptr,
                        NULL,
                        NULL,
                        NULL,
                        NULL,
                        ds707_pkt_ps_flow_ioctl );
#ifdef FEATURE_HDR_QOS
  /*-------------------------------------------------------------------------
    Set up the callback functions for the default flow for this Iface
  -------------------------------------------------------------------------*/
  ds707_pkt_flow_register_callback_func(  
                                     PS_IFACE_GET_DEFAULT_FLOW(iface_ptr) );
#endif

  /*-------------------------------------------------------------------------
    ENABLE THE PS IFACE
  -------------------------------------------------------------------------*/
  ps_iface_enable_ind(iface_ptr);

  /*-----------------------------------------------------------------------
    Enable the Linger capability on this IFACE.
  -----------------------------------------------------------------------*/
  if (ps_iface_enable_iface_linger(iface_ptr) < 0)
  {
    DATA_IS707_MSG1( MSG_LEGACY_ERROR,
                     "No Linger Timer associated with iface: 0x%p",
                     iface_ptr);
  }
	 
  /*-----------------------------------------------------------------------
    Assign EPC group to the IFACE if feature EHRPD is defined
  -----------------------------------------------------------------------*/
#ifdef FEATURE_EHRPD
  /*lint -save -e641*/
  iface_ptr->group_mask = IFACE_EPC_GROUP;
  /*lint -restore*/
#endif

} /* ds707_pkt_mgr_iface_init() */

/*===========================================================================
FUNCTION       DS707_PKT_GET_CONN_DELAY_TIMER_FROM_IFACE_PTR

DESCRIPTION    Given the IFACE returns pointer to connection release delay timer
               
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
rex_timer_type * ds707_pkt_get_conn_delay_timer_from_iface_ptr( 
  ps_iface_type  *iface_ptr
)
{
  ds707_iface_table_type*  ds707_iface_tbl_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/ 
  ASSERT(iface_ptr != NULL);

  ds707_iface_tbl_ptr = 
    ds707_pdncntx_get_iface_tbl_entry_from_iface(iface_ptr);
  
  if ( ds707_iface_tbl_ptr == NULL )
  {
    DATA_IS707_MSG0(MSG_LEGACY_MED, "Cannot get Iface Tbl entry"); 
    return(NULL);
  }
  return (ds707_pdncntx_get_conn_delay_timer_ptr(ds707_iface_tbl_ptr));

} /* ds707_pkt_get_conn_delay_timer_from_iface_ptr() */


/*===========================================================================
FUNCTION       DS707_PKT_MGR_IFACE_CONN_DELAY_TIMER_CBACK

DESCRIPTION    Function registered when the Timer for a Connection release 
               delay is expired.
               
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds707_pkt_mgr_iface_conn_delay_timer_cback(
  unsigned long instance_ptr
)
{
  ds_cmd_type   *cmd_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/   
  DATA_IS707_MSG0(MSG_LEGACY_MED, "CB: CONN DELAY TIMER CB Processing");
  
  /*----------------------------------------------------------------------
  This is a callback function called when the timer expires. Puts command
  in ds task command queue.
  ----------------------------------------------------------------------*/
  cmd_ptr = ds_allocate_cmd_buf(DS_CMD_PAYLOAD_SIZE_ZERO);
  if(NULL == cmd_ptr)
  {
    ASSERT(0);
    return;
  }
  
  cmd_ptr->hdr.cmd_id = DS_CMD_707_PKT_DELAY_CONNECTION_CLOSE_TIMER;
  cmd_ptr->cmd_payload_ptr = (void *)instance_ptr; /* ds707_iface_tbl_ptr */
  ds_put_cmd(cmd_ptr);
  return;
} /* ds707_pkt_mgr_iface_conn_delay_timer_cback() */

/*===========================================================================
FUNCTION      DS707_PKT_MGR_IFACE_PROCESS_CMD

DESCRIPTION   

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  
===========================================================================*/
void ds707_pkt_mgr_iface_process_cmd(
  ds_cmd_type   *ds_cmd_ptr
)
{
  ds707_iface_table_type *iface_tbl_ptr  = NULL;
  ps_iface_type          *iface_ptr      = NULL;
  dssnet4_sm_cb_type     *dssnet4_sm_ptr = NULL;
  dssnet6_sm_cb_type     *dssnet6_sm_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(ds_cmd_ptr != NULL);
  ASSERT(ds_cmd_ptr->cmd_payload_ptr != NULL);

  switch (ds_cmd_ptr->hdr.cmd_id)
  {
    /*-------------------------------------------------------------------
     Connection release delay timer. Depending on if iface is IPv4 or
     IPv6 iface, call appropriate function to process the timer expiry event
    -------------------------------------------------------------------*/
  case DS_CMD_707_PKT_DELAY_CONNECTION_CLOSE_TIMER:
    iface_tbl_ptr = (ds707_iface_table_type *) ds_cmd_ptr->cmd_payload_ptr;
    if( !DS707_IFACE_TBL_IS_VALID(iface_tbl_ptr) )
    {
      DATA_IS707_MSG0(MSG_LEGACY_ERROR, "iface_tbl_ptr invalid, return" );
      return;
    }
    iface_ptr = ds707_pdncntx_get_iface_ptr_from_iface_tbl(iface_tbl_ptr);

    if(ps_iface_addr_family_is_v4(iface_ptr))
    {
      dssnet4_sm_ptr = ds707_pdncntx_get_dssnet4_sm_ptr(iface_tbl_ptr);
      ds707_iface4_hdlr_process_conn_delay_timer_ev(dssnet4_sm_ptr);
    }

#ifdef FEATURE_DATA_PS_IPV6
    if(ps_iface_addr_family_is_v6(iface_ptr))
    {
      dssnet6_sm_ptr = ds707_pdncntx_get_dssnet6_sm_ptr(iface_tbl_ptr);
      dsgen_iface6_hdlr_process_conn_delay_timer_ev(dssnet6_sm_ptr);
    }
#endif
    break;

    default:
      DATA_IS707_MSG1( MSG_LEGACY_ERROR, 
                       "Unknown cmd to ds707_pkt %d", 
                       ds_cmd_ptr->hdr.cmd_id );
      break;
  }
} /* ds707_pkt_mgr_iface_process_cmd() */

/*===========================================================================
FUNCTION       DS707_PKT_MGR_IFACE_START_CONN_DELAY_TIMER

DESCRIPTION    Starts the Connection Release Delay Timer for the Specified 
               Duration.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pkt_mgr_iface_start_conn_delay_timer( 
  ps_iface_type *iface_ptr
)
{
  rex_timer_type  *conn_delay_timer = NULL; 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/ 
  ASSERT(iface_ptr != NULL);
  conn_delay_timer = ds707_pkt_get_conn_delay_timer_from_iface_ptr(iface_ptr);
  ASSERT(conn_delay_timer != NULL);
  
  if(rex_get_timer(conn_delay_timer) == 0)
  {
    DATA_IS707_MSG1(MSG_LEGACY_HIGH, 
                    "STARTED CONN DELAY TIMER for:%d ms",
                    CONNECTION_CLOSE_DELAY_TIME_MSEC);
    /*lint -e534 return value not required*/
    (void)rex_set_timer( conn_delay_timer, 
                         CONNECTION_CLOSE_DELAY_TIME_MSEC );
    /*lint -restore*/
  }
} /* ds707_pkt_mgr_iface_start_conn_delay_timer() */

#endif /* FEATURE_DATA_IS707 */


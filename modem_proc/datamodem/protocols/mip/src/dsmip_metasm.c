/*===========================================================================

                         D S M I P _ M E T A S M . C

DESCRIPTION

 The Data Services Mobile IP Meta State Machine source file.

EXTERNALIZED FUNCTIONS
   mip_meta_sm_init()
     Initialize the Meta State Machine.
   mip_meta_sm_post_event()
     Post an event to the Meta State Machine.
   mip_meta_sm_open()
     Check if MIP client is running
   mip_meta_sm_set_in_flow()
     Enable or disable data flow from socket apps and tethered device

Copyright (c) 2000-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: O:/src/asw/COMMON/vcs/dsmip_metasm.c_v   1.51   12 Feb 2003 19:38:24   akuzhiyi  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/mip/src/dsmip_metasm.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
10/08/13    vl     Optimization of F3 messages
06/27/13    cx     Replace memcpy and memmove with memscpy and memsmove
06/03/13    sd     ATCoP decoupling changes
12/28/11    sn     Added fix to store MN AAA SPI value in MA Info cache to be 
                   retrieved during MA Info stats.
05/20/11    op     Migrated to MSG 2.0 macros
03/23/11    op     Data SU API cleanup
03/15/11    mg     Global variable cleanup
02/24/11    ash    Cleaning up global variables for Q6 free floating effort.
01/10/11    ss     Cleaning up of Globals for thread safety in Q6 Free 
                   Floating environment.
10/07/10    ttv    Added messages which explain the reason for asserting.
05/13/10    ls     Update policy for all the sockets
10/15/09    vs     SU Level Api Defeaturization
09/04/09    ss     CMI SU level modifications.
11/24/08    ss     Off target Lint fix
05/23/07    vp     MIP CCoA related changes.
05/10/07    ac     Lint fix
12/07/06    as     Added support for BRING DOWN MIP event used for MIP dereg
07/22/05    kvd    report MIP failure event via mIP callbacks,instead of 
                   directly posting to dssnet4
10/18/04    vp     Removed htonl for ip address before call to 
                   ps_iface_set_addr().
06/11/04    vp     Changes due to changes in PS data structures. Removed
                   inclusion of ps_global.h
06/29/04    kvd    Fixed some indentation, use mip_reset_config(), instead of
                   resetting config valid flag directly.
06/22/04    kvd    Clear MIP session valid flag on transition to MSMI_CLOSED.
06/22/04    kvd    Do a deselect on MIP ICMP sockets to hold reading Router
                   advs until MIP metasm is UP.
04/16/04    jd     Use ps_iface_ioctl directly instead of dss_iface_ioctl.
03/20/04    ak     Compiler fixes for IPV6/June Dev merge (has new dssnets).
02/02/04    jd     Add iface_id to post event.
                   Init MIP command handlers (in place of signals).
01/20/04    jd     Removed dependencies on ps_mip_compat.h
01/20/04    jd     Mainlined Multimode code (multimode only now)
12/26/03    usb    Removed dependency on mip_meta_sm_set_in_flow() for 
                   MM_DATA builds. Added new ps_ifac state CONFIGURING.
11/13/03    jd     Clear cached MIP iface ID at end of call after cleanup is
                   complete.
10/31/03    usb    Change non sys socket flow only if able to get an iface id
10/28/03    sv     changed code to use mipio_get_iface_id instead of the
                   macro.
10/09/03    jd     Start snooping ICMP at start of MIP call
09/23/03    jd     Mainlined Irfan's changes (!FEATURE_DATA_CURRENT_PPP_CB)
08/28/03    ss     Updated to use dss_iface_id_type
08/18/03    jd     Featurized Irfan's changes (FEATURE_DATA_CURRENT_PPP_CB)
08/13/03    ifk    rx_iface_ptr moved to IPCP fsm in ps_iface_set_addr().
05/18/03    aku    Call mipio_ppp_close on transition to closed state to
                   ensure MIP ppp instance is closed for SimpleIP fallback.
05/14/03    jd     For multimode, call mipio_ppp_open instead of pppopen
                   directly to make sure 1x SN iface is forced
05/05/03    jd     Hard code UM_SN_IFACE for ioctls() in 
                   mip_meta_sm_set_in_flow() for proper MIP call termination
05/05/03    usb    Removed access to RMSM private info block for MM_DATA.
04/18/03    ak     Featurized new RMSM interface usage.
04/11/03    ak     Updated to use new RMSM interfaces.
02/12/03    aku    Added support for flow controlling non system sockets on a 
                   per interface basis.
12/29/02    jd     Moved mip_perf_init() into mip_meta_sm_init()
11/25/02    usb    Calling pppopen for MM also.
11/19/02    jd     for MM, instead of pppopen, set output iface ptr in acb
11/16/02    jd     Remove pppopen call by MIP during sockets call for MM
11/14/02    jd     Removed netmodel_mode from msmi_info (unused)
                   In MM builds, call ps_iface_set_addr instead of dsiface_;
                   call ds3g_siol_set_inbound flow instead of ds_set_in...;
                   changed API to post events to dssnet
10/09/02    jay    Changed #define MAX_NAI_LENGTH to MAX_NAI_LEN
08/19/02    jay    Clear the DMU state at the end of the call.
07/18/02    jay    Initialize DMU
07/11/02    jd     Removed featurization under FEATURE_DATA_PS since affected
                   code is under FEATURE_DS_MOBILE_IP and this is never 
                   turned on for non-IS-2000 targets.
06/03/02    jay    Renamed ds_fill_mip_atcop_val to ds_atcop_fill_mip_val
05/10/02    jd     moved to dsmip.c:
                   msmi_get_nv_info            -> mipi_get_nv_info
                   mip_meta_sm_reg_ses_info_cb -> mip_reg_ses_info_cb
                   mip_meta_sm_reg_pwd_info_cb -> mip_reg_pwd_info_cb
05/10/02    jd     moved session info array, ui session & password data
                   strutures, mip_meta_sm_config_session to dsmip.c in order
                   to seperate configuration functionality from metasm and
                   solve build dependency issues
04/17/02    jd     ds_fill_mip_vals now calls ps_get_nv_item - fixes bug we
                   were in dsi_wait (waiting on DS signal) instead of psi_
04/17/02    jd     Use global ps_nv_item when retrieving global NV item
03/21/02   jd/sjy  Check to see if all MIP sessions are registered before
                   setting Um interface to pseudonet mode.
02/20/02    sjy    Mobile IP performance now featurized under 
                   FEATURE_DS_MOBILE_IP_PERF
02/12/02    dwp    Cleanup header inclusion(s) for non DS builds.
02/11/02    jay    Erase passwd info entered by the user through callback
                   at the end of the call.
02/07/02    sjy    Added support for Mobile IP call performance metrics.
02/07/02    vsk    Fixed  header inclusion so that it builds for all targets
                   more code wrapped in FEATURE_DATA_PS
01/24/02    jay    Copy profile info from NV into atcop_val at the end of a
                   M.IP call.
01/23/02    jd     Cleaned up some comments.
01/17/02    jay    Erase the shadow nv at the end of MIP call.
                   Use shadow NV in the session block.
01/02/02    aku    Added calls to MIPIO_OPEN_IFACE() and 
                   mipio_pppclose()
11/27/01    mvl    Made sure that default values are used for NV items that
                   are supposed to have defaults when the item is not set.
11/26/01    mvl    Changed the calculation of max reg time such that it
                   allows for an extra RRQ attempt.  Some clean up.
11/21/01    aku    dssnet_sm_post_event now takes an additional acb_ptr arg
11/21/01    ss     Added code for reading new NV item: ds_mip_rrq_if_tfrk
10/24/01    js     Added definition for dsmip_info which includes all MIP
                   related information 
10/09/01    js     Added externalized function mip_meta_sm_config_session(), 
                   so that sessions configuration can be performed prior to 
                   the traffic channel is established. Modified 
                   msmi_get_nv_info() to handle the case where NV retrieval 
                   fails.  
09/17/01    js     Added header comment block for externalized functions.  
09/16/01    jd     Added accessor functions to set aforementioned callbacks
09/16/01    jd     Added registerable UI callbacks for NAI and password 
09/16/01    jd     Removed duplicate line of code that saves nai_length 
09/11/01    mvl    Changed some variable names for clarity.  Modified
                   algorithm so that the unset HA address is 255.255.255.255
                   - also added code to set primary address to 0 in NV if it
                   is unset.
08/24/01    mvl    Changed the calculation of the base retry interval so that
                   it starts at 1s.  Removed the assert() in post event if
                   not initialized - now just return.
08/23/01    aku    Clearing ma_info struct on ppp resync to ensure that M. IP 
                   reregisters
08/15/01    sjy    Renamed RMSMI_UM_RM_UP_STATE to new state name
                   DS707_RMSM_UM_RM_UP_MIP_STATE
07/31/01    mvl    Changed to way the set_in_flow() function works with
                   respect to setting the routing on the Um interface.
07/30/01    na     regsm, solsm & ma_info are initialized on transitions to
                   OPEN state.
07/29/01    jd     Added accessor function to see if mip is active
07/27/01    aku    Notify DSSNET state machine on transtioning to 
                   CLOSED_STATE
07/07/01    mvl    Initializing the PDSN DB
06/26/01    mvl    Check that the shared secrets actually contain information
                   otherwise the session info block is invalid.
06/21/01    mvl    added the function to flow control sockets
06/20/01    mvl    Added check such that the netmodel mode is never set to
                   NONE when restoring the old mode.
06/19/01    jd     Added assignment of MN-AAA spi to the session info block 
06/12/01    mvl    Completely rewrote to support latest design - which
                   includes updates to the NV read function.
05/23/01    sy     Added function msmi_get_nv_info() for retrieving NV items
01/15/01    mvl    Created module
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "data_msg.h"
#include "dsmip_api.h"

#ifdef FEATURE_DS_MOBILE_IP

#include "amssassert.h"
#include "ds707_iface4_hdlr.h"
#include "dssocket.h"
#include "dsmip_metasm.h"
#include "ps_mip_task.h"
#include "dsmip_v.h"
#include "dsmip_cfg.h"
#include "nv.h"
#include "dsmip_solsm.h"
#include "dsmip_io.h"
#include "dsmip_regsm.h"
#include "dsmip_pdsn_db.h" 

#ifdef FEATURE_DS_MOBILE_IP_PERF
#include "dsmip_perf.h"
#endif /* FEATURE_DS_MOBILE_IP_PERF */

#ifdef FEATURE_DS_MOBILE_IP_DMU
#include "dsmip_dmu.h"
#endif /* FEATURE_DS_MOBILE_IP_DMU */

#include "dsat707mip.h"
#include "ps_utils.h"
#include "ps_ip_addr.h"
#include "ps_iface.h"
#include "ds707_pkt_mgr.h"
#include "ds707_pkt_mgri.h"
#include "ps_system_heap.h"

/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                            INTERNAL DEFINITIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

/*---------------------------------------------------------------------------
  Declaration of the states of the meta state machine.
---------------------------------------------------------------------------*/
typedef enum
{
  MSMI_MIN_STATE = -1,
  MSMI_CLOSED_STATE,       /* The null state - waiting for a trigger event */
  MSMI_OPENING_STATE,      /* Trying for MIP - phys_link_up                */
  MSMI_OPEN_STATE,         /* Solicitation sent                            */
  MSMI_MAX_STATE
} msmi_state_type;

/*---------------------------------------------------------------------------
  Type for meta state machine's information block
---------------------------------------------------------------------------*/
typedef struct
{
  msmi_state_type state;
  boolean inited;
} msmi_info_type;

#ifndef T_ARM
/*---------------------------------------------------------------------------
  Unit test state and event strings.
  These need to be kept in sync with the enums which they describe!
---------------------------------------------------------------------------*/
static char *msmi_state_names[MSMI_MAX_STATE] =
{
  "Closed State",
  "Opening State",
  "Open State"
};

static char *msmi_event_names[MSM_MAX_EV] =
{
  "Configure Interface",
  "Exit MIP",
  "M.IP RRQ failed"
};
#endif /* ! T_ARM */



/*===========================================================================

                               INTERNAL DATA

===========================================================================*/

/*---------------------------------------------------------------------------
  The control block of the meta state machine.
---------------------------------------------------------------------------*/
LOCAL msmi_info_type msmi_info;



/*===========================================================================

                           FORWARD DECLARATIONS

===========================================================================*/
LOCAL void msmi_process_cmd
(
  ps_cmd_enum_type  cmd,                           /* PS command type      */
  void *            data_ptr                       /* Pointer to user data */
);

LOCAL void msmi_transition_state
(
  msmi_state_type new_state  
);


/*===========================================================================

                          EXTERNALIZED FUNCTIONS

===========================================================================*/

/*===========================================================================
FUNCTION MIP_META_SM_POST_EVENT()

DESCRIPTION
  This function posts an event to the Agent solicitation state machine.

PARAMETERS
  event: The event that is being posted to the state machine.

RETURN VALUE
  None

DEPENDENCIES
  mip_meta_sm_init() has to have been called first.

SIDE EFFECTS
  None
===========================================================================*/
void mip_meta_sm_post_event
(
  mip_meta_sm_event_type  event,
  ps_iface_type *         iface_ptr
)
{
  ps_cmd_enum_type cmd_type;                                /* ps cmd type */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifndef T_ARM
  DATA_MIP_MSG1(MSG_LEGACY_MED, "'%s' event posted to Meta SM.\n", msmi_event_names[event]);
#else
  DATA_MIP_MSG3(MSG_LEGACY_MED, "MIP metasm event %d on iface 0x%x (state %d)", 
           event, iface_ptr, msmi_info.state);
#endif

  /*-------------------------------------------------------------------------
    If the state machine is not initialized then return immediately
  -------------------------------------------------------------------------*/
  if(msmi_info.inited != TRUE)
  {
    DATA_MIP_MSG0(MSG_LEGACY_ERROR, "Meta SM not initialized!");
    return;
  }

  /*---------------------------------------------------------------------
    Sanity Check
  ---------------------------------------------------------------------*/
  if (event >= MSM_MAX_EV)
  {
    /*lint -save -e506, -e774 */
    DATA_ERR_FATAL("Unknown METASM event");
    /*lint -restore */
  }

  /*-------------------------------------------------------------------------
    FUTURE:  Validate iface ID argument - call an ioctl() for fun
  -------------------------------------------------------------------------*/
  /*-------------------------------------------------------------------------
    Based on the event type, set the appropriate PS cmd
  -------------------------------------------------------------------------*/
  cmd_type = (ps_cmd_enum_type)((int)MIP_META_SM_MIN_CMD + (int)event);

  /*-------------------------------------------------------------------------
    Enqueue cmd with PS task if it has been set to a valid value
  -------------------------------------------------------------------------*/
  ps_send_cmd(cmd_type, (void *) iface_ptr);

} /* mip_meta_sm_post_event() */



/*===========================================================================
FUNCTION MIP_META_SM_IS_INITIALIZED

DESCRIPTION
  Accessor function to see if MIP subsystem is initialized.

PARAMETERS
  None

RETURN VALUE
  TRUE  - yes, the MIP subsystem is initialized.
  FALSE - no, the MIP subsystem is not initialized.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean mip_meta_sm_is_initialized
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Return initialized boolean from meta state machine internal info struct
  -------------------------------------------------------------------------*/
  return (msmi_info.inited);
} /* mip_meta_sm_is_initialized() */



/*===========================================================================
FUNCTION MIP_META_SM_OPEN()

DESCRIPTION
  Accessor function to see if Mobile IP is active or coming up.

PARAMETERS
  None

RETURN VALUE
  TRUE - mip is active or coming up
  FALSE - not active or coming up

DEPENDENCIES
  mip_meta_sm_init() has to have been called first.

SIDE EFFECTS
  None
===========================================================================*/
boolean mip_meta_sm_open
(
  void
)
{
  return (msmi_info.state == MSMI_OPEN_STATE ? TRUE : FALSE);
} /* mip_meta_sm_open() */

#ifdef FEATURE_DS_MOBILE_IP_DMU

/*===========================================================================
FUNCTION MIP_DMU_IN_PROGRESS()

DESCRIPTION
  This function  returns TRUE if MDU is in progress right now on the default
  MIP session. Used by atcop to not to update the keys on profile change,
  while DMU is in progress.

PARAMETERS
  None

RETURN VALUE
  TRUE  DMU in progress
  FALSE DMU not in progress

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean mip_dmu_in_progress
(
  void
)
{
  mip_session_info_type* mip_ses_info_array;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  mip_ses_info_array = mip_get_ses_info_array();
  /*-------------------------------------------------------------------------
    Hard coded to check MIP session #0 for now.
    Change if multiple MIP sessions are supported.
  -------------------------------------------------------------------------*/
  if( (&mip_ses_info_array[0])->dmu )
  {
    return( TRUE );
  }
  else
  {
    return( FALSE );
  }
} /* mip_dmu_in_progress() */

#endif /* FEATURE_DS_MOBILE_IP_DMU */


/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                          LOCAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

/*===========================================================================
FUNCTION MSMI_PROCESS_CMD()

DESCRIPTION
  This function processes a command resulting from a post_event() call

PARAMETERS
  cmd:       input command
  data_ptr:  ignored

RETURN VALUE
  None

DEPENDENCIES
  mip_meta_sm_init() has to have been called first.

SIDE EFFECTS
  None
===========================================================================*/
LOCAL void msmi_process_cmd
(
  ps_cmd_enum_type  cmd,                           /* PS command type      */
  void *            param                          /* associated interface */
)
{
  mip_session_info_type *   session;
  ps_ip_addr_type           ip_addr;
  int                       loop;
  ps_iface_type *           iface_ptr;
  dss_net_policy_info_type  net_policy;
  uint32  optlen;                 /* length of above option value          */
  sint15  l_errno;                /* errno returned from dss_socket() call */
  sint15 sock_desc;
  sint15  sock_err;               /* return value of dss_setsockopt       */
  uint8 i = 0;
  mip_session_info_type* mip_ses_info_array;
#ifdef FEATURE_DS_MOBILE_IP_PERF
  mip_perf_ts_info_type *mip_perf_info_ptr = NULL;
#endif
  ds707_data_session_profile_info_type *profile_info_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*------------------------------------------------------------------------ 
    Get MIP session Information Array.
  ------------------------------------------------------------------------*/
  mip_ses_info_array = mip_get_ses_info_array();

/*lint -save -e641 suppressing error 641*/
  DATA_MIP_MSG2(MSG_LEGACY_MED, "processing cmd %d in state %d", cmd, msmi_info.state);

  /*-------------------------------------------------------------------------
    Resolve session by interface
  -------------------------------------------------------------------------*/
  session = INDEX_2_MIP_SES_PTR(0);

  /*-------------------------------------------------------------------------
    Take action based on event received and which state we are currently in
  -------------------------------------------------------------------------*/
  switch(cmd)
  {
    /*-------------------------------------------------------------------------
                              MSM_PHYS_IFACE_UP_EV

        Physical link up. wiating for MIP PPP to come up - transition to the
        OPENING state if transitioning from the closed state initialize IO.
    -------------------------------------------------------------------------*/
    case PS_MIP_PHYS_IFACE_UP_CMD:
      switch(msmi_info.state)
      {
        case MSMI_CLOSED_STATE:
          msmi_transition_state(MSMI_OPENING_STATE);
          break;
        case MSMI_OPENING_STATE:
        case MSMI_OPEN_STATE:
          DATA_MIP_MSG1(MSG_LEGACY_ERROR, "phys_iface_up rcvd in %d state ",msmi_info.state);
          break;
        default:
          DATA_MIP_MSG1(MSG_LEGACY_ERROR, "Invalid MSM state %d",
                                msmi_info.state);	
          /*lint -save -e506, -e774 */
          ASSERT(0);
          /*lint -restore */
      }
      break;

      /*-------------------------------------------------------------------------
                                 MSM_CFG_IFACE_EV
 
          PPP up with MIP supported - transition to the OPEN state if
          transitioning from the closed state initialize IO.
      -------------------------------------------------------------------------*/
    case PS_MIP_CFG_IFACE_CMD:
      switch(msmi_info.state)
      {
        case MSMI_CLOSED_STATE:
          DATA_MIP_MSG0(MSG_LEGACY_ERROR, " msm_cfg_iface_ev received in closed state ");
          break;
        case MSMI_OPENING_STATE:
          /*---------------------------------------------------------------------
            Validate the interface ID
          ---------------------------------------------------------------------*/
          iface_ptr = (ps_iface_type *) param;

          if (iface_ptr == NULL)
          {
            DATA_MIP_MSG1(MSG_LEGACY_ERROR, "Invalid interface (%x)!", iface_ptr);
            // FUTURE: Fail the call here, clear associated interface
          }

          DATA_MIP_MSG1(MSG_LEGACY_HIGH,  "Mobile IP client starting on iface %x...",
                 iface_ptr);

#ifdef FEATURE_DS_MOBILE_IP_PERF
          /*---------------------------------------------------------------------
            M.IP call performance - start of initial M.IP registration
          ---------------------------------------------------------------------*/
          mip_perf_info_ptr = mip_get_perf_info_ptr(PERF_MIP_INIT_REG_DELAY);
          mip_perf_delay(mip_perf_info_ptr,
                         PERF_MIP_INIT_REG_DELAY,
                         PERF_START_TS); 
#endif /* FEATURE_DS_MOBILE_IP_PERF */

          /*---------------------------------------------------------------------
            Determine the specific ps_iface_ptr based on the interface name and
            instance passed in.
          ---------------------------------------------------------------------*/
          /*---------------------------------------------------------------------
            Set the UM interface (mobile's) source IP address to the value 
            stored in session info block.
          ---------------------------------------------------------------------*/
          ip_addr.type    = IPV4_ADDR;
          ip_addr.addr.v4.ps_s_addr = session->home_addr;
          DATA_MIP_MSG1(MSG_LEGACY_MED, "Set IP addr to %x", ip_addr.addr.v4.ps_s_addr);
          (void) ps_iface_set_addr( iface_ptr, &ip_addr );

          /*---------------------------------------------------------------------
            Fix the interface ID used by mobile IP to the CDMA SN interface
          ---------------------------------------------------------------------*/
          session->iface_ptr = iface_ptr;
#ifdef FEATURE_DATA_PS_MIP_CCOA
          if(dshdr_get_mobile_ip_uses_ccoa())
          {
            mipio_set_iface_ptr( PS_IFACE_GET_ASSOC_IFACE(iface_ptr) );
            session->curr_ccoa = ps_iface_get_v4_addr(
                                        PS_IFACE_GET_ASSOC_IFACE(iface_ptr));
          }
          else
          {
            /* FA mode operation */
            mipio_set_iface_ptr( iface_ptr );
          }
#else
          mipio_set_iface_ptr( iface_ptr );
#endif /* FEATURE_DATA_PS_MIP_CCOA */
          
          /*---------------------------------------------------------------------
            Update the sockets policy with the requesting profile id from iface.
            If MIP profile is selected for call bring up, and since socket is 
            open with default profile, this will fail in receiving udp packets
            on that socket
          ---------------------------------------------------------------------*/
          if ( iface_ptr != NULL )
          {         
            mipio_get_mip_policy( &net_policy );
            net_policy.cdma.data_session_profile_id = 
              ds707_data_session_get_iface_requesting_profile(iface_ptr,
                                                              &profile_info_ptr);
            DATA_MIP_MSG1(MSG_LEGACY_MED, 
                          "Changing the net policy for sockets with profile id %d",
                          net_policy.cdma.data_session_profile_id);

            /* the following policy has to be added */
            net_policy.iface.kind    = DSS_IFACE_ID;
            net_policy.iface.info.id = PS_IFACE_GET_ID(iface_ptr);

            DATA_MIP_MSG2(MSG_LEGACY_HIGH, 
                          "Changing net policy iface id 0x%x and iface kind %d",
                          net_policy.iface.info.id,
                          net_policy.iface.kind );

            optlen = sizeof(dss_net_policy_info_type);

            for ( i = 0; i < MIPIO_MAX_SOCKS; i++ )
            {
              sock_desc = mipioi_get_sock_descriptor_from_array_index(i);
              DATA_MIP_MSG2(MSG_LEGACY_HIGH, 
                            "Changing net policy for socket %d, socket_desc 0x%x  ",
                            i, 
                            sock_desc );
              if ( sock_desc != -1 )
              {
                sock_err = dss_setsockopt(sock_desc, 
                                          DSS_SOL_SOCKET, 
                                          (int) DSS_SO_NETPOLICY, 
                                          &net_policy,
                                          &optlen,
                                          &l_errno );
                if (sock_err == DSS_ERROR)
                {
                   DATA_MIP_MSG2(MSG_LEGACY_ERROR, 
                                 "Couldn't set socket option (errno=%d) for MIP socket status %d!",
                                 l_errno,
                                 sock_err);
                }
              }
            }
          }

          /*---------------------------------------------------------------------
            Start snooping for Agent Advertisements
          ---------------------------------------------------------------------*/
          if (!mipio_start_icmp_snooping())
          {
            DATA_MIP_MSG0(MSG_LEGACY_ERROR, "Reg ICMP filters for MIP failed");
            // FUTURE: Fail the call here, clear associated interface
          }
          msmi_transition_state(MSMI_OPEN_STATE);
          break;

        case MSMI_OPEN_STATE:
          DATA_MIP_MSG0(MSG_LEGACY_MED, "PPP resync. Initiate M. IP re-reg");
          msmi_transition_state(MSMI_OPEN_STATE);
          break;

        default:
          DATA_MIP_MSG1(MSG_LEGACY_ERROR, "Invalid MSM state %d",
                        msmi_info.state);
          /*lint -save -e506, -e774 */
          ASSERT(0);
          /*lint -restore */
      } /* switch(state) */
      break;

    /*-------------------------------------------------------------------------
                            MSM_EXIT_MIP_EV

      Either the interface went down, or it doesn't support Mobile IP -
      transition to the CLOSED state if OPEN otherwise ignore either event.
    -------------------------------------------------------------------------*/
    case PS_MIP_EXIT_CMD:
      switch(msmi_info.state)
      {
        case MSMI_CLOSED_STATE:
          DATA_MIP_MSG1(MSG_LEGACY_MED, "MIP exit cmd (%d) in Closed state!\n", cmd);
          break;
        case MSMI_OPENING_STATE:
        case MSMI_OPEN_STATE:
          msmi_transition_state(MSMI_CLOSED_STATE);
          break;
        default:
          DATA_MIP_MSG1(MSG_LEGACY_ERROR, "MIP exit cmd rcvd in %d state ",
                        msmi_info.state);
          /*lint -save -e506, -e774 */
          ASSERT(0);
          /*lint -restore */
      } /* switch(state) */
      break;

  /*-------------------------------------------------------------------------
                            MSM_INIT_RRQ_FAILED_EV
                            MSM_RE_RRQ_FAILED_EV

    check if there are any active M.IP registrations - if there are do
    nothing - if not do what needs to be done to Exit M.IP Mode and
    transition to the Closed state.  If already in in the closed state do
    nothing.
  -------------------------------------------------------------------------*/
  case PS_MIP_REG_FAILURE_CMD:
    switch(msmi_info.state)
    {
      case MSMI_CLOSED_STATE:
        break;

      case MSMI_OPENING_STATE:
        /* fall through and close MIP meta SM */

      case MSMI_OPEN_STATE:
        for(loop = 0; loop < MIP_MAX_SESSIONS; loop += 1)
        {
          /*-------------------------------------------------------------------
            check if any session is active do nothing
          -------------------------------------------------------------------*/
          if(mip_ses_info_array[loop].reg_lifetime > 0)
          {
            DATA_MIP_MSG1(MSG_LEGACY_MED, "M.IP Ses %d active, ignoring event\n", loop);
            return;
          }
        }

        /*---------------------------------------------------------------------
          Reset the interface - if this was a Re_RRQ we don't want to force the
          link down (i.e. the second argument to mip_reset())
          Init fail_reason to RRQ_TIMEOUT only if not initialized earlier.
        ---------------------------------------------------------------------*/
        DATA_MIP_MSG0(MSG_LEGACY_MED, "No active sessions, shutting down\n");
        if ( session->mip_fail_reason == MIP_FAIL_REASON_NONE )
        {
          session->mip_fail_reason = MIP_FAIL_REASON_RRQ_TIMEOUT;
        }
        msmi_transition_state(MSMI_CLOSED_STATE);
        break;

      default:
        DATA_MIP_MSG1(MSG_LEGACY_ERROR, "MIP reg failed cmd rcvd in %d state ",
                      msmi_info.state);
        /*lint -save -e506, -e774 */
        ASSERT(0);
        /*lint -restore */
    } /* switch(state) */
    break;

    /*-------------------------------------------------------------------------
                              MSM_MIP_BRING_DOWN_CMD

      Physical link up. wiating for MIP PPP to come up - transition to the
      OPENING state if transitioning from the closed state initialize IO.
    -------------------------------------------------------------------------*/
  case PS_MIP_BRING_DOWN_CMD:
    switch(msmi_info.state)
    {
      case MSMI_OPEN_STATE:
        mip_reg_sm_post_event_all(RSMI_DEREG_MIP_EV);
        break;
      case MSMI_OPENING_STATE:
      case MSMI_CLOSED_STATE:
        DATA_MIP_MSG1(MSG_LEGACY_ERROR, "mip bring down cmd rcvd in %d state ",msmi_info.state);
        break;
      default:
        DATA_MIP_MSG1(MSG_LEGACY_ERROR, "mip bring down cmd rcvd in %d state ",
                      msmi_info.state); 
        /*lint -save -e506, -e774 */
        ASSERT(0);
        /*lint -restore */
    }
    break;

  default:
    DATA_MIP_MSG1(MSG_LEGACY_ERROR, "Invalid msmi command %d ",cmd);
    /*lint -save -e506, -e774 */
    ASSERT(0);
    /*lint -restore */
  } /* switch(cmd) */
/*lint -restore Restore lint error 641*/
} /* msmi_process_cmd() */



/*===========================================================================
FUNCTION MSMI_TRANSITION_STATE()

DESCRIPTION
  This function effectuates the transition to the state that is passed in

PARAMETERS
  new_state: the state to transition to.

RETURN VALUE
  None

DEPENDENCIES
  mip_meta_sm_init() has to have been called first.

SIDE EFFECTS
  None
===========================================================================*/
LOCAL void msmi_transition_state
(
  msmi_state_type new_state  
)
{
  mip_session_info_type   *session;
  nv_item_type            *ps_nv_item_ptr;
  mip_ma_info_type        *ma_info_ptr;
  mip_pwd_info_type       *mip_pwd_info_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Allocate temporary memory for the NV item
  -------------------------------------------------------------------------*/
  ps_nv_item_ptr = (nv_item_type *)
    ps_system_heap_mem_alloc(sizeof(nv_item_type));
  if( ps_nv_item_ptr == NULL )
  {
    DATA_ERR_FATAL("Mem alloc from system heap failed.");
    return;
  }

#ifndef T_ARM
  DATA_MIP_MSG2(MSG_LEGACY_MED, "Meta SM from '%s' to '%s' state\n",
          msmi_state_names[msmi_info.state],
          msmi_state_names[new_state]);
#else
  DATA_MIP_MSG2(MSG_LEGACY_MED, "Meta SM from %d to %d state\n", msmi_info.state, new_state);
#endif

  session = INDEX_2_MIP_SES_PTR(0);

  /*-------------------------------------------------------------------------
    Get ma_info_ptr
  -------------------------------------------------------------------------*/
  ma_info_ptr = mip_get_ma_info_ptr();

  /*-------------------------------------------------------------------------
    Get ma_pwd_info_ptr
  -------------------------------------------------------------------------*/
  mip_pwd_info_ptr = mip_get_pwd_info_ptr();

  /*-------------------------------------------------------------------------
    Perform action associated before transitioning into new state.
  -------------------------------------------------------------------------*/
  switch(new_state)
  {
    /*-----------------------------------------------------------------------
                                 MSMI_CLOSED
    
      Nothing to do in this case
    -----------------------------------------------------------------------*/
    case MSMI_CLOSED_STATE:

      /*-----------------------------------------------------------------------
        Starting Mobile IP call.
      -----------------------------------------------------------------------*/
      if (msmi_info.state != MSMI_CLOSED_STATE)
      {
        DATA_MIP_MSG0(MSG_LEGACY_HIGH, "Mobile IP stopping...");
      }
      /*---------------------------------------------------------------------
        Reset mip_session valid flag. In future if there are mutliple MIP
        sessions, this needs to be done for each MIP session.
      ---------------------------------------------------------------------*/
      mip_reset_config();
      break;

    case MSMI_OPENING_STATE:

      /*-----------------------------------------------------------------------
        Waiting for MIP call to come up. We dont want to read any data pkts(eg.
        router advs) until MIP call is UP; deselect on the MIP icmp socket.
        Clear the ICMP RX
      -----------------------------------------------------------------------*/
      mipio_stop_read_from_rx_icmp_sock();
      break;

    /*-----------------------------------------------------------------------
                                 MSMI_OPEN_STATE

      If the current state is MSMI_OPEN_STATE, this indicates a PPP resync. 
      Reset the MA info so that M. IP re-registers.
    -----------------------------------------------------------------------*/
    case MSMI_OPEN_STATE:

      /*-----------------------------------------------------------------------
        Reset the FA info.
      -----------------------------------------------------------------------*/
      memset(ma_info_ptr, 0, sizeof(mip_ma_info_type));
      
      /*-----------------------------------------------------------------------
        Store MN AAA SPI in MA Info cache.
      -----------------------------------------------------------------------*/
      memscpy(ma_info_ptr->mn_AAA_SPI,
              sizeof(ma_info_ptr->mn_AAA_SPI), 
              &(session->mn_aaa_spi), 
              sizeof(ma_info_ptr->mn_AAA_SPI));

      /*-----------------------------------------------------------------------
        PPP is established, kick the solicitation state machine to get 
        the ball rolling.
      -----------------------------------------------------------------------*/
      mip_sol_sm_post_event(SSM_SOL_TRIGGER_EV);
      mipio_resume_read_from_rx_icmp_sock();
      break;

    default:
      DATA_MIP_MSG1(MSG_LEGACY_ERROR, "Invalid msmi new state %d ",new_state);      
      ASSERT(0);
  } /* switch( new state ) */

  msmi_info.state = new_state;

  /*-------------------------------------------------------------------------
    Perform action associated with entering into new state.
  -------------------------------------------------------------------------*/
  switch(new_state)
  {
    /*-----------------------------------------------------------------------
                                 MSMI_CLOSED

      Entering into the Closed state:
      - reset the FA info
      - post the Exit MIP event to all Sol and Reg state machines
      - Reset the IP address to 0
      - clear write event notification from MIP write sockets
      - tell dssnet that MIP is closing
      - erase shadow nv
      - reload NV values into MIP AT command settings
      - erase the pwd info that is passed by the user through the
        callback function
      - stop snooping for AAMs
      - clear the associated interface
    -----------------------------------------------------------------------*/
    case MSMI_CLOSED_STATE:
      memset(ma_info_ptr, 0, sizeof(mip_ma_info_type));
      mip_sol_sm_post_event(SSM_EXIT_MIP_EV);
      mip_reg_sm_post_event_all(RSM_EXIT_MIP_EV);
      mipio_stop_read_from_rx_icmp_sock();
      mipio_clear_write_event_notification();

      /*-----------------------------------------------------------------------
        Notify DSSNET State machine of M. IP failure.
      -----------------------------------------------------------------------*/
      mip_call_event_callback(MIP_FAILURE_EV);

      /*-----------------------------------------------------------------------
        Erase the shadow nv items when the call has ended and copy the nv
        items into atcop_val for display.
      -----------------------------------------------------------------------*/
      dsmip_erase_shadow_nv();
      dsmip_fill_mip_vals(ps_get_nv_item, ps_nv_item_ptr);
    
#ifdef FEATURE_DS_MOBILE_IP_DMU
      /*-----------------------------------------------------------------------
        Clear the DMU flag
      -----------------------------------------------------------------------*/
      session->dmu = FALSE;
#endif /* FEATURE_DS_MOBILE_IP_DMU */

      /*-----------------------------------------------------------------------
        Erase the passwd info that the user has passed though the callback
        function when the call has ended.
      -----------------------------------------------------------------------*/
      memset(mip_pwd_info_ptr,0,sizeof(mip_pwd_info_type));

      /*-----------------------------------------------------------------------
        Stop snooping for Agent Advertisements
      -----------------------------------------------------------------------*/
      mipio_stop_icmp_snooping();

      /*-----------------------------------------------------------------------
        Done MIP cleanup, reset interface ID
        Note this must be the last action in this state transition.
      -----------------------------------------------------------------------*/
      session->iface_ptr = NULL;
      mipio_set_iface_ptr (NULL);

      break;
    case MSMI_OPENING_STATE:
      /* Nothign to do */
      break;

    /*-----------------------------------------------------------------------
                                  MSMI_OPEN

      Entering into the Open state:
        make sure that our provisioning information is valid, otherwise post
        event that will cause things to fail.
    -----------------------------------------------------------------------*/
    case MSMI_OPEN_STATE:
      /*-----------------------------------------------------------------------
        Check to see if the session info was retrieved successfully from NV,
        If not, post failure to self. 
      -----------------------------------------------------------------------*/
      if (session->valid != TRUE)
      {
        DATA_MIP_MSG0(MSG_LEGACY_MED, "Session info block invalid - closing!\n");
        mip_meta_sm_post_event(MSM_INIT_RRQ_FAILED_EV, NULL);
      }
      break;

    default:
      DATA_MIP_MSG1(MSG_LEGACY_ERROR, "Invalid msmi new state %d ",new_state);      
      ASSERT(0);
  } /* switch( new state ) */

  PS_SYSTEM_HEAP_MEM_FREE(ps_nv_item_ptr);
} /* msmi_transition_state() */

#endif /* FEATURE_DS_MOBILE_IP */



/*===========================================================================
                               SU Level API
===========================================================================*/


/*===========================================================================
FUNCTION MIP_META_SM_INIT()

DESCRIPTION
  This function initializes the meta state machine.

PARAMETERS
  None

RETURN VALUE
  0 on success
 -1 on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int mip_meta_sm_init
(
  void
)
{
#ifdef FEATURE_DS_MOBILE_IP
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Initialize Mobile IP i/o module
  -------------------------------------------------------------------------*/
  if(mipio_init() == FALSE)
  {
    DATA_MIP_MSG0(MSG_LEGACY_ERROR, "M.IP initialization failed!");
    return -1;
  }

  /*-------------------------------------------------------------------------
    Initialize the solicitation state machine
  -------------------------------------------------------------------------*/
  if(mip_sol_sm_init() < 0)
  {
    /*-----------------------------------------------------------------------
      Timer allocation probably failed
    -----------------------------------------------------------------------*/
    DATA_MIP_MSG0(MSG_LEGACY_ERROR, "M.IP SolSM init failed!");
    return -1;
  }

  /*-------------------------------------------------------------------------
    Initialize the registration state machine
  -------------------------------------------------------------------------*/
  if(mip_reg_sm_init() < 0)
  {
    /*-----------------------------------------------------------------------
      Timer allocation probably failed
    -----------------------------------------------------------------------*/
    DATA_MIP_MSG0(MSG_LEGACY_ERROR, "M.IP RegSM init failed!");
    return -1;
  }

  /*-------------------------------------------------------------------------
    Initialize the PDSN database
  -------------------------------------------------------------------------*/
  pdsn_db_init();

#ifdef FEATURE_DS_MOBILE_IP_DMU
  /*-------------------------------------------------------------------------
    Initialize DMU
  -------------------------------------------------------------------------*/
  dmu_init();
#endif /* FEATURE_DS_MOBILE_IP_DMU */

#ifdef FEATURE_DS_MOBILE_IP_PERF
  /*-------------------------------------------------------------------------
    Initialize MIP performance
  -------------------------------------------------------------------------*/
  mip_perf_init();
#endif /* FEATURE_DS_MOBILE_IP_PERF */

  /*-------------------------------------------------------------------------
    register the handlers for the various MIP commands that must be handled
    in the task context.
  -------------------------------------------------------------------------*/
  (void) ps_set_cmd_handler(MIP_PHYS_IFACE_UP_CMD, msmi_process_cmd);
  (void) ps_set_cmd_handler(MIP_CFG_IFACE_CMD,     msmi_process_cmd);
  (void) ps_set_cmd_handler(MIP_EXIT_CMD,          msmi_process_cmd);
  (void) ps_set_cmd_handler(MIP_REG_FAILURE_CMD,   msmi_process_cmd);
  (void) ps_set_cmd_handler(MIP_BRING_DOWN_CMD,    msmi_process_cmd);

  /*-------------------------------------------------------------------------
    Set the initial state to its initial value
  -------------------------------------------------------------------------*/
  msmi_info.state  = MSMI_CLOSED_STATE;
  msmi_info.inited = TRUE;

  return 0;

#else
  DATA_MIP_MSG0(MSG_LEGACY_ERROR, "mip_meta_sm_init() not supported");
  return -1;
#endif                                      

} /* mip_meta_sm_init() */

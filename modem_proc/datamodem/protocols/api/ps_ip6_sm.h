#ifndef PS_IP6_SM
#define PS_IP6_SM
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            P S _ I P 6 _ S M . H

DESCRIPTION
  Header file defining the API for the IPv6 state machine.  This state
  machine is responsible for soliciting for Router Advertisements.  When a
  valid RA is received it will be in the Net Up state - which means that the
  IP interface it is associated with is in the up state.

  It provides a mechanism for a client to register for either UP or down
  indications - this will inform the client (or owner) that it has entered
  either of these states.

DEPENDENCIES
  The module MUST execute in the PS context.

EXTERNALIZED FUNCTIONS
  ip6_sm_create()
    Creates an instance of the state machine and associates it with a given
    PS Iface instance.
  ip6_sm_post_event()
    Used to post an event to the state machine - may cause state
    transitions.
    NOTE:  If this is called OUTSIDE of the PS context it will send a command
    to PS, otherwise it will execute in-line.
  ip6_sm_ind_cback_reg()
    Used to register for the UP/DOWN indications.
  ip6_sm_is_ipv6_priv_ext_enabled()
    Access method for ipv6_priv_ext_enabled flag
  ip6_sm_get_priv_ext_lifetimes()
    Access method for priv_ext_lifetimes structure

Copyright (c) 2003-2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/api/ps_ip6_sm.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/09/15    cx     Fix incorrect event order in IPv6 EVENTS
10/13/14    cx     Added support for config update event
06/27/14    ssh    Added ip6_sm_update_config()
04/09/14    ash    Added Prefix Delegation support.
02/20/14    ssh    Install filter for ICMPv6 packets directed to primary IID
10/03/11    ash    Added IPv6 SM passive mode support
03/21/11    op     Data SU API cleanup
09/21/10    ash    Increased IP6_SM_DEFAULT_SOL_INTERVAL from 2 sec to 4 sec
                   and decreased IP6_SM_DEFAULT_MAX_RESOL_ATTEMPTS from 5 to 3
08/11/10    ash    Increased IP6_SM_DEFAULT_SOL_INTERVAL to 4000
05/07/09    pp     CMI Phase-4: SU Level API Effort.
11/02/06    mct    Added support for IPv6 Privacy Extensions.
02/22/06    msr    Using single critical section
02/06/06    mct    Updated for L4 tasklock/crit sections.
03/21/05    ssh    Changes pursuant to the new typedef ps_timer_handle_type
08/12/04    mvl    Changed some RA resol fields for clarity.
07/12/04    mvl    Updates to the state machine to reflect design changes:
                   specifically the addition of a new state which allows the
                   differentiation of resolicitation for RAs and Prefixes.
06/03/04    mct    Added config struct for ip6 state machine. Updated
                   __ip6_sm_cb_s to take new rs config values
04/05/04    sv     Featurized the file.
03/02/04    rc     Added PS IFACE pointer to __ip6_sm_cb_s.
01/08/04    mvl    Some include cleanup.
12/22/03    mvl    Created module
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "pstimer.h"
#include "ps_iface_defs.h"
#include "ps_iface_ipfltr.h"
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
                            FORWARD DECLARATIONS
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
struct __ip6_sm_cb_s; /* the state machine data structure                */

/*===========================================================================

                         EXTERNAL DATA DECLARATIONS

===========================================================================*/
#define IP6_SM_DEFAULT_MAX_SOL_ATTEMPTS          3
#define IP6_SM_DEFAULT_MAX_RESOL_ATTEMPTS        3
#define IP6_SM_DEFAULT_INIT_SOL_DELAY          500
#define IP6_SM_DEFAULT_SOL_INTERVAL           4000
#define IP6_SM_DEFAULT_RESOL_INTERVAL         4000
#define IP6_SM_DEFAULT_PRE_RA_EXP_RESOL_TIME     0

/*---------------------------------------------------------------------------
  Default Privacy Extensions lifetimes:
  - Preferred IID lifetime: 1 day. (in seconds)
  - Valid IID lifetime:     1 week. (in seconds)
---------------------------------------------------------------------------*/
#define DEFAULT_IPV6_PRIV_IID_PREF_LIFETIME  86400
#define DEFAULT_IPV6_PRIV_IID_VALID_LIFETIME 604800

/*---------------------------------------------------------------------------
TYPEDEF IP6_SM_TYPE

DESCRIPTION
  This is the type of IPv6 State Machine
---------------------------------------------------------------------------*/
typedef struct __ip6_sm_cb_s ip6_sm_type;

/*---------------------------------------------------------------------------
TYPEDEF IP6_SM_CONFIG

DESCRIPTION
  This is the type of IPv6 State Machine Configuration Structure
---------------------------------------------------------------------------*/
typedef struct
{
  int   init_sol_delay;             /* The initial solicitation delay (ms) */
  int   sol_interval;               /* The solicitation interval (ms)      */
  int   resol_interval;             /* The resolicitation interval (ms)    */
  int   max_sol_attempts;           /* # of initial solicitation attempts  */
  int   max_resol_attempts;         /* # of resolicitation attempts        */
  int   pre_ra_exp_resol_time;      /* Pre-RA expiration resol time (ms)   */
} ip6_sm_config_type;

typedef struct
{
  uint32 pref_lifetime_timer;
  uint32 valid_lifetime_timer;
} priv_ext_lifetimes_type;

/*---------------------------------------------------------------------------
TYPEDEF IP6_SM_EVENT_TYPE

DESCRIPTION
  This is for IPv6 SM events.
  The possible events are:
  - Start: posted from the controller (e.g. DSSNet6)
  - RS_Timer: posted internally from the RS timer OR by PS Iface when
      preferred lifetime for a prefix has expired.
  - RA_Timer: posted internally by the RA timer
  - Valid_RA: posted from PS Iface whenever a valid RA is received (this is
      done from the ps_iface_apply_v6_prefix() call.
  - Line_Changed: posted from PS Iface when the phys link state changes.
  - Stop: posted either by the controller, or PS Iface when the lifetime for
      all prefixes for that interface have expired.
---------------------------------------------------------------------------*/
typedef enum
{
  IP6_SM_MIN_EV      = 0,
  IP6_SM_START_EV    = 0,
  IP6_SM_RS_TIMER_EV = 1,
  IP6_SM_RA_TIMER_EV = 2,
  IP6_SM_VALID_RA_EV = 3,
  IP6_SM_LINK_CHANGED_EV = 4,
  IP6_SM_STOP_EV     = 5,
  IP6_SM_ABORT_EV    = 6,
  IP6_SM_UPDATE_CONFIG_EV = 7,
#ifdef FEATURE_DATA_PS_DHCPV6
  IP6_SM_DHCP6_PROVISIONED_EV = 8,
  IP6_SM_DHCP6_FAILED_EV   = 9,
  IP6_SM_DHCP6_STOPPED_EV  = 10,
  IP6_SM_DHCP6_ABORTED_EV  = 11,
#endif /* FEATURE_DATA_PS_DHCPV6 */
  IP6_SM_MAX_EV
} ip6_sm_event_type;

/*---------------------------------------------------------------------------
TYPEDEF IP6_SM_INDICATION_TYPE

DESCRIPTION
  This is the type of indications that clients can register for from the SM.
---------------------------------------------------------------------------*/
typedef enum
{ 
  IP6_SM_MIN_IND     = 0,
  IP6_SM_DOWN_IND    = 0,
  IP6_SM_UP_IND      = 1,
  IP6_SM_ABORTED_IND = 2,
  IP6_SM_MAX_IND
} ip6_sm_indidcation_type;

/*---------------------------------------------------------------------------
TYPEDEF IP6_SM_CBACK_TYPE

DESCRIPTION
  This is the type for the callbacks which can be registered for indications

PARAMETERS
  instance: the instance that caused the indication.
  ind: the indication that occurred (will only be posted on an edge - e.g. UP
       will only happen when not already up).
  user_data: data that was given at registration time.

RETURN VALUE
  None
---------------------------------------------------------------------------*/
typedef void (*ip6_sm_cback_type)
(
  struct __ip6_sm_cb_s *instance,
  ip6_sm_indidcation_type ind,
  void *user_data
);

/*===========================================================================
FUNCTION IP6_SM_CREATE()

DESCRIPTION
  This function creates an instance of the IPv6 state machine using memory
  passed in by the caller.  It will initialize the associated ps_iface to
  refer to it so that events (such as adding a prefix or lifetime expiry) can
  be posted to the SM.

PARAMETERS
  instance: pointer to the memory to be used to allocate the state machine.
  iface_ptr:  pointer to the associated PS Iface instance
  init_sol_delay: initial delay for solicitations (ms)
  sol_interval:   interval between solicitations in Soliciting state (ms)
  re_sol_interval: interval between solicitations in RE-Soliciting state (ms)

RETURN VALUE
  0 on succuss
 -1 on failure

DEPENDENCIES
  ip6_sm_init() needs to have been called first.

SIDE EFFECTS
  will set the ip6_sm_ptr value in the associated PS Iface pointer to it.
===========================================================================*/
int ip6_sm_create
(
  ip6_sm_type         *instance,
  ps_iface_type       *iface_ptr,
  ip6_sm_config_type  *sm_config
);

/*===========================================================================
FUNCTION IP6_SM_UPDATE_CONFIG()

DESCRIPTION
  This function updates IPv6 configuration for an IPv6 instance that has
  already been created but not triggered yet.

PARAMETERS
  instance: pointer to the memory for the allocated IPv6 state machine
  iface_ptr:  pointer to the associated PS Iface instance
  sm_config: state machine configuration parameters

RETURN VALUE
  0 on succuss
 -1 on failure

DEPENDENCIES
  ip6_sm_create() needs to have been called first. No other IPv6 API
  should have been called after that.

SIDE EFFECTS
  None
===========================================================================*/
int ip6_sm_update_config
(
  ip6_sm_type         *instance,
  ps_iface_type       *iface_ptr,
  ip6_sm_config_type  *sm_config
);

/*===========================================================================
FUNCTION IP6_SM_SET_PASSIVE()

DESCRIPTION
  This function mark the IPv6 State machine to start in PASSIVE state.
  In Passive start IPv6 state machine wait for valid RA indication
  to start as opposed to active start where state machine send RS first.
 
PARAMETERS
  instance: pointer to the memory to be used to allocate the state machine.

RETURN VALUE
  -1 on error
   0 on success

DEPENDENCIES
  ip6_sm_init() needs to have been called first.

SIDE EFFECTS
  if set IPv6 state machine will not solicit unless valid RA indication is rcvd.
===========================================================================*/
int ip6_sm_set_passive
(
  ip6_sm_type *instance
);

#ifdef FEATURE_DATA_PS_DHCPV6
/*===========================================================================
FUNCTION IP6_SM_SET_PREFIX_DELEGATION_MODE()

DESCRIPTION
  This function mark the IPv6 prefix delegation mode to enable/diable.
  If enabled, IPv6 state machine will trigger DHCPv6 client to obtain
  delegated prefix from the network.
 
PARAMETERS
  instance: pointer to the memory to be used to allocate the state machine.
  mode    : TRUE indicates enabled, FALSE indicates disabled.

RETURN VALUE
  -1 on error
   0 on success

DEPENDENCIES
  ip6_sm_init() needs to have been called first.

SIDE EFFECTS
  None
===========================================================================*/
int ip6_sm_set_prefix_delegation_mode
(
  ip6_sm_type *instance,
  boolean enable
);
#endif /* FEATURE_DATA_PS_DHCPV6 */

/*===========================================================================
FUNCTION IP6_SM_POST_EVENT()

DESCRIPTION
  This function is used to post an event to the state machine passed in.
  NOTE: if the event is posted outside PS context a command will be sent to
  PS to process that event, otherwise this will happen in line.

PARAMETERS
  instance: the instance to which the event is being posted.
  event: the event being posted
  event_data_ptr: the user data when post an event to IPv6 sm

RETURN VALUE
  0 on success
 -1 on failure

DEPENDENCIES
  ip6_sm_create() needs to have been called first.

SIDE EFFECTS
  None
===========================================================================*/
int ip6_sm_post_event
(
  ip6_sm_type      *instance,
  ip6_sm_event_type event,
  void             *event_data_ptr
);


/*===========================================================================
FUNCTION IP6_SM_IND_CBACK_REG()

DESCRIPTION
  This function is used to register for indications from the SM.

PARAMETERS
  instance: the instance of the SM to register with.
  cback: the callback being registered.
  user_data: data that is passed back in the callback.

RETURN VALUE
  0: on success
 -1: on failure

DEPENDENCIES
  ip6_sm_create() needs to have been called first.

SIDE EFFECTS
  None
===========================================================================*/
int ip6_sm_ind_cback_reg
(
  ip6_sm_type       *instance,
  ip6_sm_cback_type  cback,
  void              *user_data
);

/*===========================================================================
FUNCTION       IP6_SM_IS_PRIV_EXT_ENABLED

DESCRIPTION    Access method for ipv6_priv_ext_enabled flag

PARAMETERS
  None

RETURN VALUE   
  ipv6_priv_ext_enabled

DEPENDENCIES   
  None

SIDE EFFECTS   
  None
===========================================================================*/
boolean ip6_sm_is_priv_ext_enabled(void);

/*===========================================================================
FUNCTION       IP6_SM_GET_PRIV_EXT_LIFETIMES

DESCRIPTION    Access method for priv_ext_lifetimes structure

PARAMETERS
  None

RETURN VALUE   
  Pointer to priv_ext_lifetimes structure

DEPENDENCIES   
  None

SIDE EFFECTS   
  None
===========================================================================*/
priv_ext_lifetimes_type *ip6_sm_get_priv_ext_lifetimes(void);

/*===========================================================================
FUNCTION IP6_GET_REMAINING_ROUTER_LIFETIME()

DESCRIPTION
  This function return the remaining router resolicitation lifetime value.
 
PARAMETERS
  instance: pointer to the memory to be used to allocate the state machine.

RETURN VALUE
  -1 on error
   router lifetime in seconds.

DEPENDENCIES
  ip6_sm_init() needs to have been called first.

SIDE EFFECTS
  None
===========================================================================*/
int32 ip6_sm_get_remaining_router_lifetime
(
  ip6_sm_type *instance
);

/*===========================================================================
  FUNCTION ip6_sm_dereg_fltr()

  DESCRIPTION
    This function deregisters all IPCMPv6 filters on the Um and Rm.

  RETURN VALUE
    0 success
   -1 failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void ip6_sm_dereg_fltr
(
  void
);

/*===========================================================================
FUNCTION IP6_SM_DESTROY()

DESCRIPTION
  This function will free the recouses used in the passed in IPv6 state 
  machine, it iwll not free the state machine itself.
PARAMETERS
  instance: the state machine need to destroy

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ip6_sm_destroy
(
  ip6_sm_type   *instance
);

/*===========================================================================
FUNCTION IP6_SM_DEINIT()

DESCRIPTION
  This function will free the allocated resouces in init function

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ip6_sm_deinit
(
  void
);


/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                           PRIVATE DATA STRUCTURES

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/*---------------------------------------------------------------------------
TYPEDEF IP6_SMI_STATE_TYPE

DESCRIPTION
  This is the definition of the IPv6 SM states.
---------------------------------------------------------------------------*/
typedef enum
{
  IP6_DOWN_STATE     = 0,
  IP6_SOL_WAIT_STATE = 1,
  IP6_SOL_STATE      = 2,
  IP6_UP_STATE       = 3,
  IP6_RESOL_FOR_RA_STATE     = 4,
  IP6_RESOL_FOR_PREFIX_STATE = 5,
#ifdef FEATURE_DATA_PS_DHCPV6
  IP6_CLOSING_STATE = 6,
#endif /* FEATURE_DATA_PS_DHCPV6 */
  IP6_MAX_STATE
} ip6_smi_state_type;


/*---------------------------------------------------------------------------
STRUCT __IP6_SM_CB_S

DESCRIPTION
  This is the type of IPv6 State Machine
---------------------------------------------------------------------------*/
struct __ip6_sm_cb_s
{
  ip6_smi_state_type          state;
  ps_iface_type              *ps_iface_ptr;
  ps_phys_link_type          *ps_phys_link_ptr;
  void                       *ip6_sm_phys_link_flow_event_buf;
  ps_timer_handle_type        rs_timer;
  ps_timer_handle_type        ra_timer;
  ps_iface_ipfltr_handle_type ip6_fltr;
  ps_iface_ipfltr_handle_type icmp6_dst_fltr;
#ifdef FEATURE_DATA_PS_DHCPV6
  void                        *dhcp6_client_handle;
  boolean                      prefix_delegation_enabled;
#endif /* FEATURE_DATA_PS_DHCPV6 */
  int tries;
  /*---------------------------------------------------------------------
     Passive mode: If set, no RS will be sent until the state machine 
     switches to active mode.
     Mode switch to active happens when state machine receives 
     valid RA indication with positive router lifetime.
   ---------------------------------------------------------------------*/
  boolean passive_start;

  struct
  {
    int init_sol_delay;
    int sol_interval;
    int max_sol_attempts;
    int init_resol_interval;
    int resol_interval;
    int max_resol_attempts;
    uint32 max_resol_time;        /* the maximum time re-sol can take      */
    uint32 pre_ra_exp_resol_time; /* configured time to resolicit for an RA*/
    uint32 remaining_ra_lifetime; /* time btw re-sol and lifetime expiring */
    uint32 ra_resol_time;         /* when to solicit for an RA             */
  } params;

  struct
  {
    ip6_sm_cback_type cback;
    void             *data;
  } ind;
  ip6_sm_type *this_sm_ptr;
};
#endif /* PS_IP6_SM */

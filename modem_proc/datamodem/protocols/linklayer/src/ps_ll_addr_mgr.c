/*===========================================================================

                  P S _ L L _ A D D R _ M G R . C


DESCRIPTION
This file contains the implementation of PS Link Local Address manager.

This entity is responsible for dynamic configuration of IPv4 link local
addresses per RFC 3927

This is particularly useful on a ilnk where connectivity is restricted to
`nodes' on the same link. e.g. in Ad Hoc networks

It is important to note that the 169.254/16 addresses registered with IANA
for this purpose are not routable addresses.

NOTE: Use of this module for IP address configuration may cause some
      applications to fail!!


EXTERNALIZED FUNCTIONS

  ll_addr_mgr_init()
    Called to initialize the link local address mgr (at powerup)

  ll_addr_mgr_start()
    Called to start the link local address manager

  ll_addr_mgr_stop()
    Called to stop the link local address manager and thereby release the
    address acquired

  ll_addr_mgr_claim()
    Called to acquire a link local address


Copyright (c) 2006-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/linklayer/src/ps_ll_addr_mgr.c#1 $
  $Author: mplcsds1 $

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/12/13    sd     Cryptographic vulnerability - replacing ran_next APIs.
09/21/12    ss     Replacing LOG_MSG_FATAL_ERROR with LOG_MSG_ERROR and 
                   evaluating ASSERT requirement.
02/17/11    sa     Fixed compiler warnings.
12/23/10    ss     Cleaning up of Globals for thread safety in Q6 Free 
                   Floating environment.
09/10/10    jee    Fixed KW warnings
09/24/09    ss     KW warning fixes.
07/23/06    tbh    Created module
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"   /* Customer Specific Features                      */


#include "ps_ll_addr_mgr.h"
#include "ps_iface.h"
#include "ps_svc.h"
#include "ps_arp.h"
#include "ps_in.h"
#include "ps_crit_sect.h"
#include "ps_utils.h"
#include "pstimer.h"
#include "ps_mem.h"
#include "fs_public.h"
#include "ran.h"
#include "qw.h"          /* Because time_type being a qword    */
#include "time_svc.h"    /* For getting the sys time           */
#include "ds_Utils_DebugMsg.h"


/*===========================================================================

                          LOCAL DATA DECLARATIONS

===========================================================================*/
/*---------------------------------------------------------------------------
  Tuning the number of ps ll address manager buffers needed by this module
---------------------------------------------------------------------------*/
#define PS_LL_ADDR_MGR_BUF_SIZE  ((sizeof(ll_addr_mgr_start_param_type) +3) & ~3)

#define PS_LL_ADDR_MGR_BUF_NUM        5
#define PS_LL_ADDR_MGR_BUF_HIGH_WM    4
#define PS_LL_ADDR_MGR_BUF_LOW_WM     1

/*----------------------------------------------------------------------------
  Allocate memory to hold ps_ll_addr_mgr along with ps_mem header
----------------------------------------------------------------------------*/
static int ps_ll_addr_mgr_buf_mem[PS_MEM_GET_TOT_SIZE(PS_LL_ADDR_MGR_BUF_NUM,
                                                      PS_LL_ADDR_MGR_BUF_SIZE)];

#ifdef FEATURE_DATA_PS_MEM_DEBUG
/*----------------------------------------------------------------------------
  Array of pointers used to facilitate easy debugging.  The first one points
  to the ps_mem header and the latter ponts to ps_ll_addr_mgr_buf
----------------------------------------------------------------------------*/
static ps_mem_buf_hdr_type           * ps_ll_addr_mgr_buf_hdr[PS_LL_ADDR_MGR_BUF_NUM];
static ll_addr_mgr_start_param_type  * ps_ll_addr_mgr_buf_ptr[PS_LL_ADDR_MGR_BUF_NUM];
#endif /* FEATURE_DATA_PS_MEM_DEBUG */

/*---------------------------------------------------------------------------
  Defining the critical section macros based on the target(6550 or other target)
--------------------------------------------------------------------------*/
#ifdef PS_ENTER_CRIT_SECTION
#define LL_ADDR_MGR_ENTER_CRIT_SECTION()                                    \
 PS_ENTER_CRIT_SECTION(&global_ps_crit_section)
#else
#define LL_ADDR_MGR_ENTER_CRIT_SECTION()                                    \
 TASKLOCK()
#endif

#ifdef PS_BRANCH_LEAVE_CRIT_SECTION
#define LL_ADDR_MGR_BRANCH_LEAVE_CRIT_SECTION()                             \
 PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section)
#else
#define LL_ADDR_MGR_BRANCH_LEAVE_CRIT_SECTION()                             \
 { TASKFREE()
#endif

#ifdef PS_LEAVE_CRIT_SECTION
#define LL_ADDR_MGR_LEAVE_CRIT_SECTION()                                    \
 PS_LEAVE_CRIT_SECTION(&global_ps_crit_section)
#else
#define LL_ADDR_MGR_LEAVE_CRIT_SECTION()                                    \
 TASKFREE()
#endif

/*---------------------------------------------------------------------------
  Macro to put the FSM is a snpecific state
--------------------------------------------------------------------------*/
#define LL_ADDR_MGR_FSM_STATE_TRANSITION(handle,state_next)                  \
        do                                                                   \
        {                                                                    \
          LOG_MSG_INFO2_2("Link local address manager FSM  State %d ==>  State %d",  \
            handle->state, state_next) ;                    \
            handle->state = state_next;                         \
        } while ( 0 )

#define LL_ASSERT(_statement_ )                              \
        do                                                     \
        {                                                      \
          LOG_MSG_ERROR_0( "Assertion " #_statement_ " failed"); \
        } while ( 0 )

#define LL_ADDR_MGR_IP_MSB                 0xA9FE   /* first 2 bytes of the link
                                                       local ip address, i.e.
                                                       169.254 */
#define LL_ADDR_MGR_IP_LSB_MAX             0xFEFF   /* Max value of the least
                                                       significant bytes of link
                                                       local ip address */
#define LL_ADDR_MGR_BIT_MASK               0x0000FFFF
#define LL_ADDR_MGR_SUBNET_MASK            0x0000FFFF
#define LL_ADDR_MGR_CONFIG_IP_BUFFER_SIZE  4
#define LL_ADDR_MGR_IP_ADDR_LEN            2
#define LL_ADDR_MGR_TIMER_LEN              2
#define LL_ADDR_MGR_MAX_CONFLICT           10
#define LL_ADDR_MGR_PROBE_NUM              3
#define LL_ADDR_MGR_ANNOUNCE_NUM           2
#define LL_ADDR_MGR_PROBE_WAIT             1000
#define LL_ADDR_MGR_PROBE_MIN              1000
#define LL_ADDR_MGR_PROBE_MAX              2000
#define LL_ADDR_MGR_PROBE_ANNOUNCE_WAIT    2000
#define LL_ADDR_MGR_RATE_LIMIT_INTERVAL    60000
#define LL_ADDR_MGR_DEFEND_INTERVAL        10000
#define LL_ADDR_MGR_CB_INSTANCE_MAX        10
#define LL_ADDR_MGR_MAC_ADDR_LOWER_32_BIT_MASK      0xFFFFFFFF
#define LL_ADDR_MGR_MAC_ADDR_DEFAULT       0x001122334455ULL
#define LL_ADDR_MGR_ACQUISITION_RETRIES_DEFAULT 2
/*===========================================================================

                          PUBLIC DATA DECLARATIONS

===========================================================================*/
/*------------------------------------------------------------------------------
TYPEDEF ps_ll_addr_mgr_sm_state_type

DESCRIPTION
  Type and static variable defining the state of the link local address manager.
------------------------------------------------------------------------------*/
typedef enum
{
  LL_ADDR_MGR_SM_INIT_S         = 0,
  LL_ADDR_MGR_SM_PROBING_S      = 1,
  LL_ADDR_MGR_SM_CLAIMED_S      = 2,
  LL_ADDR_MGR_SM_DEFENDING_S    = 3
} ll_addr_mgr_sm_state_type;

/*---------------------------------------------------------------------------
TYPEDEF PS_LL_ADDR_MGR_SM_EVENT_TYPE

DESCRIPTION
  Type and static variable defining events supported by the link local address
  manager state mechine.
---------------------------------------------------------------------------*/
typedef enum
{
  LL_ADDR_MGR_TIMER_EV             = 0,
  LL_ADDR_MGR_CONFLICT_EV          = 1,
  LL_ADDR_MGR_CLAIM_EV             = 2,
  LL_ADDR_MGR_PROBE_FAIL_EV        = 3,
  LL_ADDR_MGR_SM_MAX_EV
} ll_addr_mgr_sm_event_type;

/*---------------------------------------------------------------------------
TYPEDEF LL_ADDR_MGR_IP_PARAMS_TYPE

DESCRIPTION
  Structure with the IP related configuration parameters used for Link Local
  address assignment to iface
---------------------------------------------------------------------------*/
typedef struct
{
  uint32         pri_dns_addr;    /* Primary DNS server address             */

  uint32         sec_dns_addr;    /* Secondary DNS server address           */

  uint32         subnet_mask;     /* The relevant subnet mask               */

  uint32         default_gateway; /* Default gateway                        */

} ll_addr_mgr_ip_params_type;

/*---------------------------------------------------------------------------
STRUCT PS_LL_ADDR_MGR_SM_CB_S

DESCRIPTION
  Link Local address manager control block.
---------------------------------------------------------------------------*/
typedef struct
{
  boolean                      initialized; /* to check if an instance in use */
  ll_addr_mgr_sm_state_type    state;
  ps_timer_handle_type         timer_hndl;
  ps_iface_type               *iface_ptr;     /* iface pointer of client    */
  int                          retry_cnt; /* count for retries to acquire an IP address */
  uint32                       ip_address;
  uint8                        num_conflicts;
  ll_addr_mgr_ind_cback_type   ind_cback;     /* cback for notifying MC */
  boolean                      start_lock; /* Use to lock a START Request   */
  boolean                      stop_lock;  /* Use to lock a STOP Request    */
  ll_addr_mgr_ip_params_type   ip_params;
  uint32                       instance;
  ll_addr_mgr_start_param_type start_params;
}ll_addr_mgr_sm_cb_type;

/*---------------------------------------------------------------------------
Array of Link Local address manager cb instances
---------------------------------------------------------------------------*/
static ll_addr_mgr_sm_cb_type ll_addr_mgr_sm_cb[LL_ADDR_MGR_CB_INSTANCE_MAX];

/*---------------------------------------------------------------------------
 variable for storing Link Local address in a file
---------------------------------------------------------------------------*/
static const char    *filename ="/etc/ll_addr/link_local_ip_addr";


/*===========================================================================

                          FORWARD DECLARATIONS

===========================================================================*/
STATIC void ll_addr_mgr_sm_process_timer_ev
(
  ll_addr_mgr_sm_cb_type *
);

STATIC void ll_addr_mgr_sm_process_conflict_ev
(
  ll_addr_mgr_sm_cb_type *
);

STATIC void ll_addr_mgr_sm_process_claim_ev
(
  ps_cmd_enum_type,
  void *
);

STATIC void ll_addr_mgr_sm_process_probe_fail_ev
(
  ll_addr_mgr_sm_cb_type *
);

void ll_addr_mgr_conflict_cback
(
  void *
);

void ll_addr_mgr_timer_cback
(
  void *
);

void ll_addr_mgr_arp_probe_cback
(
  void*,
  arp_probe_status
);

void lli_addr_mgr_start
(
  ps_cmd_enum_type,
  void *
);

void lli_addr_mgr_stop
(
  ps_cmd_enum_type,
  void *
);

int ll_addr_mgr_ps_iface_set_ip_params
(
  ll_addr_mgr_sm_cb_type *,
  uint32
);

/*===========================================================================

                        PUBLIC FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION LL_ADDR_MGR_INIT()

DESCRIPTION
  This function is responsible for (power-up) initialization of the Link
  Local address manager

DEPENDENCIES
  Must be called in PS context.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ll_addr_mgr_init
(
  void
)
{
  uint32 instance = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* ------------------------------------------------------------------------
    Initialize pool
  -------------------------------------------------------------------------*/
  if (ps_mem_pool_init(PS_MEM_LL_ADDR_MGR_BUFFER_TYPE,
                       ps_ll_addr_mgr_buf_mem,
                       PS_LL_ADDR_MGR_BUF_SIZE,
                       PS_LL_ADDR_MGR_BUF_NUM,
                       PS_LL_ADDR_MGR_BUF_HIGH_WM,
                       PS_LL_ADDR_MGR_BUF_LOW_WM,
                       NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                       (int *) ps_ll_addr_mgr_buf_hdr,
                       (int *) ps_ll_addr_mgr_buf_ptr
#else
                       NULL,
                       NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                      ) == -1)
  {
    DATA_ERR_FATAL("Can't init the module");
    return;
  }

  for(instance = 0; instance < LL_ADDR_MGR_CB_INSTANCE_MAX; instance++)
  {
    memset(&ll_addr_mgr_sm_cb[instance], 0, sizeof(ll_addr_mgr_sm_cb_type));
    ll_addr_mgr_sm_cb[instance].start_lock = FALSE;
    ll_addr_mgr_sm_cb[instance].stop_lock  = FALSE;
  }

  (void) ps_set_cmd_handler( PS_LL_ADDR_MGR_START_CMD, lli_addr_mgr_start );
  (void) ps_set_cmd_handler( PS_LL_ADDR_MGR_STOP_CMD, lli_addr_mgr_stop );
  (void) ps_set_cmd_handler( PS_LL_ADDR_MGR_CLAIM_CMD,
                             ll_addr_mgr_sm_process_claim_ev );

} /* ll_addr_mgr_init */


/*===========================================================================
FUNCTION LL_ADDR_MGR_START()

DESCRIPTION
  This is called to start the LL address mgr.

  start() can only be followed by a stop() or any number of calls to claim().

DEPENDENCIES
  This MUST be called in mode controller context

RETURN VALUE
  handle for the link local address manager (to be used upon subsequent
  calls into the LL addr mgr)

SIDE EFFECTS
  None.
===========================================================================*/
void * ll_addr_mgr_start
(
  ps_iface_type                 *iface_ptr,     /* iface pointer of client */
  ll_addr_mgr_params_type       *params_ptr,    /* related parameters ptr  */
  ll_addr_mgr_ind_cback_type     ind_cback,     /* cback for notification  */
  void                          *user_data_ptr  /* user data for caller    */
)
{
  ll_addr_mgr_start_param_type *start;
  ll_addr_mgr_sm_cb_type       *ll_addr_mgr_sm_cb_ptr = NULL;
  uint32                        instance = 0;
  uint32                        seed;
  uint32                        mac_addr_lsb;
  qword                         time;  /* time is a qword */
  ll_addr_mgr_params_type       default_params;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( NULL == iface_ptr)
  {
    DATA_ERR_FATAL("ll_addr_mgr_start: iface_ptr is not valid");
    return NULL;
  }

  if( NULL == params_ptr)
  {
    LOG_MSG_ERROR_0("NULL parameters passed. Using default.");
    default_params.acquisition_retries = 
                   LL_ADDR_MGR_ACQUISITION_RETRIES_DEFAULT;
    default_params.mac_addr = LL_ADDR_MGR_MAC_ADDR_DEFAULT;
    params_ptr = &default_params;
  }

  if( NULL == ind_cback)
  {
    LOG_MSG_ERROR_0("No wlan_addr_clnt notification callback registered");
  }

  for(instance = 0; instance < LL_ADDR_MGR_CB_INSTANCE_MAX; instance++)
  {
    LL_ADDR_MGR_ENTER_CRIT_SECTION();
    if(FALSE == ll_addr_mgr_sm_cb[instance].initialized)
    {
      ll_addr_mgr_sm_cb[instance].initialized = TRUE;
      LL_ADDR_MGR_BRANCH_LEAVE_CRIT_SECTION();
      break;
    }
    LL_ADDR_MGR_LEAVE_CRIT_SECTION();
  }
  if(instance >= LL_ADDR_MGR_CB_INSTANCE_MAX)
  {
    LOG_MSG_ERROR_0("No Link Local control block instance available");
    ll_addr_mgr_sm_cb_ptr = NULL;
  }
  else
  {
    ll_addr_mgr_sm_cb_ptr = &ll_addr_mgr_sm_cb[instance];
    LL_ADDR_MGR_ENTER_CRIT_SECTION();
    /*-------------------------------------------------------------------------
      Check if this is not a duplicate start
    -------------------------------------------------------------------------*/
    if ( TRUE == ll_addr_mgr_sm_cb_ptr->start_lock)
    {
      LOG_MSG_ERROR_0("ll_addr_mgr already started");
      LL_ADDR_MGR_BRANCH_LEAVE_CRIT_SECTION();
      return NULL;
    }

    ll_addr_mgr_sm_cb_ptr->start_lock = TRUE;
    ll_addr_mgr_sm_cb_ptr->instance = instance;

    LL_ADDR_MGR_LEAVE_CRIT_SECTION();
  }

  /*-------------------------------------------------------------------------
    Get a buffer from PS MEM and populate it
  -------------------------------------------------------------------------*/
  start = (ll_addr_mgr_start_param_type*)
                      ps_mem_get_buf(PS_MEM_LL_ADDR_MGR_BUFFER_TYPE);

  if ( NULL == start )
  {
    LOG_MSG_ERROR_0("Could not get buffer to store start parameters");
    LL_ASSERT(0);
    return NULL;
  }

  /*-------------------------------------------------------------------------
  Calculating seed by XORing lower 32 bits of the mac address & the lower 32
  bits of current system time
  -------------------------------------------------------------------------*/
  /*-------------------------------------------------------------------------
    Retrieve the current system time in millisecondes!
  -------------------------------------------------------------------------*/
  memset(time, 0, sizeof(time));
  time_get_ms(time);
  /*-------------------------------------------------------------------------
   Get the lower 32 bits of time represented in the qword & XOR with lower 32
     bits of the mac address passed by wlan addr clnt
  -------------------------------------------------------------------------*/
  mac_addr_lsb = (uint32)(LL_ADDR_MGR_MAC_ADDR_LOWER_32_BIT_MASK & params_ptr->mac_addr);
  seed = mac_addr_lsb ^ qw_lo(time);
  ran_seed(seed);

  start->iface_ptr = iface_ptr;
  start->ind_cback = ind_cback;
  start->params = *params_ptr;
  start->user_data_ptr = user_data_ptr;
  start->ll_addr_mgr_sm_cb_instance = instance;

  ps_send_cmd( PS_LL_ADDR_MGR_START_CMD, start );

  return ((void *)instance);
} /* ll_addr_mgr_start */


/*===========================================================================
FUNCTION LL_ADDR_MGR_STOP()

DESCRIPTION
  This is called to stop the link local address manager: command will be
  serialized through PS task

DEPENDENCIES
  None.

RETURN VALUE
   0 : stop was taken into account
  -1 : serious error -> stop could not even be taken into account

SIDE EFFECTS
  None.
===========================================================================*/
int ll_addr_mgr_stop
(
  void *ll_addr_mgr_handle
)
{
  ll_addr_mgr_sm_cb_type       *ll_addr_mgr_sm_cb_ptr;
  uint32                        instance;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  instance = (uint32)ll_addr_mgr_handle;

  if(instance >= LL_ADDR_MGR_CB_INSTANCE_MAX)
  {
    DATA_ERR_FATAL("ll_addr_mgr_stop called with invalid handle");
    return -1;
  }

  ll_addr_mgr_sm_cb_ptr = &ll_addr_mgr_sm_cb[instance];

  LL_ADDR_MGR_ENTER_CRIT_SECTION();

  if ( TRUE == ll_addr_mgr_sm_cb_ptr->stop_lock )
  {
    LOG_MSG_INFO2_0("ll_addr_mgr stop is pending\n");
    LL_ADDR_MGR_BRANCH_LEAVE_CRIT_SECTION();
    return -1;
  }

  if (TRUE != ll_addr_mgr_sm_cb_ptr->start_lock)
  {
    LOG_MSG_INFO2_0("ll_addr_mgr is not started\n");
    LL_ADDR_MGR_BRANCH_LEAVE_CRIT_SECTION();
    return -1;
  }

  ll_addr_mgr_sm_cb_ptr->stop_lock = TRUE;

  LL_ADDR_MGR_LEAVE_CRIT_SECTION();

  ps_send_cmd( PS_LL_ADDR_MGR_STOP_CMD, ll_addr_mgr_handle );

  return 0;
} /* ll_addr_mgr_stop */


/*===========================================================================
FUNCTION LL_ADDR_MGR_CLAIM()

DESCRIPTION
  This is called to claim an IP address on the network

  It triggers the behavior per RFC 3927 of detecting conflicts and obtaining
  a unique link local address

DEPENDENCIES
  This MUST be called in mode controller context

RETURN VALUE
   0 : claim was taken into account
  -1 : serious error -> claim could not even be taken into account

SIDE EFFECTS
  None.
===========================================================================*/
int ll_addr_mgr_claim
(
  void *ll_addr_mgr_handle
)
{
  uint32                        instance;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  instance = (uint32)ll_addr_mgr_handle;

  if(instance >= LL_ADDR_MGR_CB_INSTANCE_MAX)
  {
    DATA_ERR_FATAL("ll_addr_mgr_claim called with invalid handle");
    return -1;
  }

  ps_send_cmd( PS_LL_ADDR_MGR_CLAIM_CMD, ll_addr_mgr_handle );

  return 0;
} /* ll_addr_mgr_claim */


/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                             INTERNAL FUNCTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
FUNCTION LLI_ADDR_MGR_START()

DESCRIPTION
  This is called to process the start cmd in the ps context.


DEPENDENCIES
  This MUST be called in ps context

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void lli_addr_mgr_start
(
  ps_cmd_enum_type cmd,
  void * user_data_ptr
)
{
  ll_addr_mgr_start_param_type  *ll_addr_mgr_start_param_ptr;
  ps_timer_handle_type           timer_hndl;
  int                            return_code;
  ll_addr_mgr_sm_cb_type        *ll_addr_mgr_sm_cb_ptr;
  uint32                         instance;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (cmd != PS_LL_ADDR_MGR_START_CMD)
  {
    LOG_MSG_ERROR_1("PS_LL_ADDR_MGR_START_CMD handler triggered by wrong cmd (%d)",
              cmd);
    ASSERT(0);
    return;
  }

  if( NULL == user_data_ptr)
  {
    LOG_MSG_ERROR_1("No parameters passed for processing the cmd (%d)",
              cmd);
    LL_ASSERT(0);
    return;
  }

  ll_addr_mgr_start_param_ptr = (ll_addr_mgr_start_param_type *)user_data_ptr;

  instance = ll_addr_mgr_start_param_ptr->ll_addr_mgr_sm_cb_instance;

  if(instance >= LL_ADDR_MGR_CB_INSTANCE_MAX)
  {
    DATA_ERR_FATAL("ll_addr_mgr_stop called with invalid handle");
    return;
  }

  ll_addr_mgr_sm_cb_ptr = &ll_addr_mgr_sm_cb[instance];

  ll_addr_mgr_sm_cb_ptr->iface_ptr = ll_addr_mgr_start_param_ptr->iface_ptr;
  ll_addr_mgr_sm_cb_ptr->ind_cback = ll_addr_mgr_start_param_ptr->ind_cback;
  ll_addr_mgr_sm_cb_ptr->retry_cnt =
    ll_addr_mgr_start_param_ptr->params.acquisition_retries;
  ll_addr_mgr_sm_cb_ptr->ip_params.pri_dns_addr = 0;
  ll_addr_mgr_sm_cb_ptr->ip_params.sec_dns_addr = 0;
  ll_addr_mgr_sm_cb_ptr->ip_params.default_gateway = 0;
  ll_addr_mgr_sm_cb_ptr->ip_params.subnet_mask = LL_ADDR_MGR_SUBNET_MASK;
  ll_addr_mgr_sm_cb_ptr->start_params = *ll_addr_mgr_start_param_ptr;

  return_code = arp_conflict_cback_reg(ll_addr_mgr_sm_cb_ptr->iface_ptr,
                                       ll_addr_mgr_conflict_cback,
                                       ll_addr_mgr_sm_cb_ptr);
  if( -1 == return_code)
  {
    LOG_MSG_ERROR_0("Error registering conflict callback");
    LL_ASSERT(0);
    return;
  }
  /*---------------------------------------------------------------------
        Allocate a new PS Timer
  ---------------------------------------------------------------------*/
  timer_hndl = ps_timer_alloc(ll_addr_mgr_timer_cback, (void *)instance);
  if( PS_TIMER_INVALID_HANDLE == timer_hndl)
  {
    DATA_ERR_FATAL("Timer allocation failed");
    return;
  }

  ll_addr_mgr_sm_cb_ptr->timer_hndl = timer_hndl;

  return_code = ll_addr_mgr_ps_iface_set_ip_params(ll_addr_mgr_sm_cb_ptr, 0);

  if( -1 == return_code)
  {
    LOG_MSG_ERROR_0("Failed to set the IP address on the iface");
    LL_ASSERT(0);
    return;
  }

  LL_ADDR_MGR_FSM_STATE_TRANSITION(ll_addr_mgr_sm_cb_ptr,
                                   LL_ADDR_MGR_SM_INIT_S);
  PS_MEM_FREE(user_data_ptr);

  LOG_MSG_INFO2_0("Link local address manager started");

} /* lli_addr_mgr_start */

/*===========================================================================
FUNCTION LLI_ADDR_MGR_STOP()

DESCRIPTION
  This is called to process the stop cmd in the ps context.


DEPENDENCIES
  This MUST be called in ps context

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void lli_addr_mgr_stop
(
  ps_cmd_enum_type cmd,
  void * user_data_ptr
)
{
  ll_addr_mgr_sm_cb_type        *ll_addr_mgr_sm_cb_ptr;
  int                            return_code;
  uint32                         instance;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (cmd != PS_LL_ADDR_MGR_STOP_CMD)
  {
    LOG_MSG_ERROR_1("PS_LL_ADDR_MGR_STOP_CMD handler triggered by wrong cmd (%d)",
              cmd);
    ASSERT(0);
    return;
  }

  instance = (uint32)user_data_ptr;

  if(instance >= LL_ADDR_MGR_CB_INSTANCE_MAX)
  {
    DATA_ERR_FATAL("lli_addr_mgr_stop called with invalid handle");
    return;
  }

  ll_addr_mgr_sm_cb_ptr = &ll_addr_mgr_sm_cb[instance];

  return_code = arp_conflict_cback_reg(ll_addr_mgr_sm_cb_ptr->iface_ptr,
                                       NULL,
                                       NULL);

  if( -1 == return_code)
  {
    LOG_MSG_ERROR_0("Error registering conflict callback");
    LL_ASSERT(0);
    return;
  }

  (void) ps_timer_free( ll_addr_mgr_sm_cb_ptr->timer_hndl);

  ll_addr_mgr_sm_cb_ptr->num_conflicts = 0;

  return_code = ll_addr_mgr_ps_iface_set_ip_params(ll_addr_mgr_sm_cb_ptr, 0);

  if( -1 == return_code)
  {
    LOG_MSG_ERROR_0("Failed to set the IP address on the iface");
    LL_ASSERT(0);
    return;
  }


  LL_ADDR_MGR_ENTER_CRIT_SECTION();
  ll_addr_mgr_sm_cb_ptr->start_lock  = FALSE;
  ll_addr_mgr_sm_cb_ptr->stop_lock   = FALSE;
  ll_addr_mgr_sm_cb_ptr->initialized = FALSE;
  LL_ADDR_MGR_LEAVE_CRIT_SECTION();
  LOG_MSG_INFO2_1("Link local address manager stopped in state %d",
          ll_addr_mgr_sm_cb_ptr->state);

  LL_ADDR_MGR_FSM_STATE_TRANSITION(ll_addr_mgr_sm_cb_ptr,
                                   LL_ADDR_MGR_SM_INIT_S);

} /* lli_addr_mgr_stop */


/*===========================================================================
FUNCTION LL_ADDR_MGR_POST_EV()

DESCRIPTION
   This function is used to post an event to the WLAN state machine in PS task
   context.

PARAMETERS
  Wlan state machine event type
  WLAN event info which is event specific

RETURN VALUE
  -1 in case of failure
  0 in case of success

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static int ll_addr_mgr_post_ev
(
  ll_addr_mgr_sm_event_type event,
  ll_addr_mgr_sm_cb_type  *evt_info
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(event > LL_ADDR_MGR_SM_MAX_EV)
  {
    LL_ASSERT(0);
    return -1;
  }

  LOG_MSG_INFO2_2("Posting event %d to Link local address manager in state %d.",
          event, evt_info->state);

  /*-------------------------------------------------------------------------
    Based on the event type, set the appropriate PS cmd
  -------------------------------------------------------------------------*/
  switch(event)
  {

    case LL_ADDR_MGR_TIMER_EV:
      ll_addr_mgr_sm_process_timer_ev(evt_info);
      break;

    case LL_ADDR_MGR_CONFLICT_EV:
      ll_addr_mgr_sm_process_conflict_ev(evt_info);
      break;

    case LL_ADDR_MGR_CLAIM_EV:
      ps_send_cmd( PS_LL_ADDR_MGR_CLAIM_CMD, evt_info );
      break;

    case LL_ADDR_MGR_PROBE_FAIL_EV:
      ll_addr_mgr_sm_process_probe_fail_ev(evt_info);
      break;

    default:
      /*---------------------------------------------------------------------
        Sanity Check
      ---------------------------------------------------------------------*/
      LOG_MSG_ERROR_1("Link local address manager got unknown event %d", event);
      LL_ASSERT(0);
      return -1;
  } /* switch(event) */

  return 0;

} /* ll_addr_mgr_post_ev() */

/*===========================================================================
FUNCTION LL_ADDR_MGR_SM_PROCESS_TIMER_EV()

DESCRIPTION
  This function processes the TIMER_EV.

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
STATIC void ll_addr_mgr_sm_process_timer_ev
(
  ll_addr_mgr_sm_cb_type *ll_addr_mgr_sm_cb_ptr
)
{
  arp_probe_ex_params_type       probe_params;
  void                          *user_data = NULL;
  int                            return_code;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( NULL == ll_addr_mgr_sm_cb_ptr)
  {
    DATA_ERR_FATAL("NULL ll_addr_mgr_sm_cb_ptr passed for processing PS_LL_ADDR_MGR_TIMER_EV event");
    return;
  }

  LOG_MSG_INFO2_1("Link local address manager processing PS_LL_ADDR_MGR_TIMER_EV \
          in state %d", ll_addr_mgr_sm_cb_ptr->state);

  switch (ll_addr_mgr_sm_cb_ptr->state)
  {
    case LL_ADDR_MGR_SM_INIT_S:
      probe_params.probe_type = ARP_BROADCAST_PROBE;
      probe_params.ip_addr = ll_addr_mgr_sm_cb_ptr->ip_address;
      probe_params.num_retries = LL_ADDR_MGR_PROBE_NUM;
      probe_params.not_confident = TRUE;
      probe_params.randomize_retry_intvl = TRUE;
      probe_params.retry_intvl = LL_ADDR_MGR_PROBE_MIN;
      probe_params.max_random_incr =
        LL_ADDR_MGR_PROBE_MAX - LL_ADDR_MGR_PROBE_MIN;
      probe_params.response_wait_time = LL_ADDR_MGR_PROBE_ANNOUNCE_WAIT;
      user_data = (void *)ll_addr_mgr_sm_cb_ptr->instance;
      return_code = arp_probe_ex(ll_addr_mgr_sm_cb_ptr->iface_ptr,
                                 &probe_params,
                                 ll_addr_mgr_arp_probe_cback,
                                 user_data);
      if( -1 == return_code)
      {
        LOG_MSG_ERROR_0("Probe request failed");
        LL_ASSERT(0);
        return;
      }

      LL_ADDR_MGR_FSM_STATE_TRANSITION(ll_addr_mgr_sm_cb_ptr,
                                       LL_ADDR_MGR_SM_PROBING_S);
      break;
    case LL_ADDR_MGR_SM_DEFENDING_S:
      LL_ADDR_MGR_FSM_STATE_TRANSITION(ll_addr_mgr_sm_cb_ptr,
                                       LL_ADDR_MGR_SM_CLAIMED_S);
      break;

    case LL_ADDR_MGR_SM_PROBING_S:
    case LL_ADDR_MGR_SM_CLAIMED_S:
      LOG_MSG_ERROR_1("Unexpected LL_ADDR_MGR_TIMER_EV in state %d",
                ll_addr_mgr_sm_cb_ptr->state);
      LL_ASSERT(0);
      break;

    default:
      LOG_MSG_ERROR_1("Link local address manager in unknown %d state",
                ll_addr_mgr_sm_cb_ptr->state);
      LL_ASSERT(0);
  }

  return;

} /* ll_addr_mgr_sm_process_timer_ev() */

/*===========================================================================
FUNCTION LL_ADDR_MGR_SM_PROCESS_CONFLICT_EV()

DESCRIPTION
  This function processes the CONFLICT_EV.

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
STATIC void ll_addr_mgr_sm_process_conflict_ev
(
  ll_addr_mgr_sm_cb_type *ll_addr_mgr_sm_cb_ptr
)
{
  int64                          time;
  arp_probe_ex_params_type       probe_params;
  void                          *user_data = NULL;
  int                            temp_val;
  int                            temp_val1;
  int                            temp_val2;
  int                            lsb_ip_address;
  int                            return_code;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if( NULL == ll_addr_mgr_sm_cb_ptr)
  {
    LOG_MSG_ERROR_0("NULL ll_addr_mgr_sm_cb_ptr passed for processing \
              PS_LL_ADDR_MGR_CONFLICT_EV event");
    ASSERT(0);
    return;
  }

  LOG_MSG_INFO2_1("Link local address manager processing PS_LL_ADDR_MGR_CONFLICT_EV \
          in state %d", ll_addr_mgr_sm_cb_ptr->state);

  switch (ll_addr_mgr_sm_cb_ptr->state)
  {
    case LL_ADDR_MGR_SM_CLAIMED_S:
      probe_params.probe_type = ARP_BROADCAST_PROBE;
      probe_params.ip_addr = ll_addr_mgr_sm_cb_ptr->ip_address;
      probe_params.num_retries = 1;
      probe_params.not_confident = FALSE;
      probe_params.randomize_retry_intvl = FALSE;
      probe_params.retry_intvl = LL_ADDR_MGR_PROBE_MIN;
      probe_params.max_random_incr = 0;
      probe_params.response_wait_time = 0;
      user_data = (void *)ll_addr_mgr_sm_cb_ptr->instance;
      return_code = arp_probe_ex(ll_addr_mgr_sm_cb_ptr->iface_ptr,
                                 &probe_params,
                                 ll_addr_mgr_arp_probe_cback,
                                 user_data);
      if( -1 == return_code)
      {
        LOG_MSG_ERROR_0("Probe request failed");
        LL_ASSERT(0);
        return;
      }

      return_code = (int)ps_timer_start(ll_addr_mgr_sm_cb_ptr->timer_hndl,
                                   LL_ADDR_MGR_DEFEND_INTERVAL);
      if( 0 == return_code)
      {
        LOG_MSG_ERROR_0("Failed to start the timer");
        LL_ASSERT(0);
        return;
      }

      (*ll_addr_mgr_sm_cb_ptr->ind_cback)(LL_ADDR_MGR_IP_CHANGED_IND,
                                          ll_addr_mgr_sm_cb_ptr->start_params.user_data_ptr);
      LL_ADDR_MGR_FSM_STATE_TRANSITION(ll_addr_mgr_sm_cb_ptr,
                                       LL_ADDR_MGR_SM_DEFENDING_S);
      break;

    case LL_ADDR_MGR_SM_PROBING_S:

      ll_addr_mgr_sm_cb_ptr->retry_cnt--;
      if(ll_addr_mgr_sm_cb_ptr->retry_cnt == 0)
      {
        (*ll_addr_mgr_sm_cb_ptr->ind_cback)(LL_ADDR_MGR_IP_UNAVAIL_IND,
                                            ll_addr_mgr_sm_cb_ptr->start_params.user_data_ptr);
        return;
      }

      lsb_ip_address = (int) ps_utils_generate_16bit_rand_num_by_range(0,
                                                  LL_ADDR_MGR_IP_LSB_MAX);
      /*-----------------------------------------------------------------------
      As per the RFC 3927 the link local address (IPv4) should be in the range
      169.254.1.0 to 169.254.254.255
      ------------------------------------------------------------------------*/
      temp_val  = 169;
      temp_val1 = 254;
      temp_val2 = 1;
      ll_addr_mgr_sm_cb_ptr->ip_address =
        temp_val | (temp_val1 << 8) |(temp_val2 << 16) | (lsb_ip_address << 16);

      time = (int64) ps_utils_generate_16bit_rand_num_by_range(0,
                                          LL_ADDR_MGR_PROBE_WAIT);

      ll_addr_mgr_sm_cb_ptr->num_conflicts++;
      if(ll_addr_mgr_sm_cb_ptr->num_conflicts > LL_ADDR_MGR_MAX_CONFLICT)
      {
        time += LL_ADDR_MGR_RATE_LIMIT_INTERVAL;
      }

      return_code = (int)ps_timer_start(ll_addr_mgr_sm_cb_ptr->timer_hndl, time);
      if( 0 == return_code)
      {
        LOG_MSG_ERROR_0("Failed to start the timer");
        LL_ASSERT(0);
        return;
      }

      LL_ADDR_MGR_FSM_STATE_TRANSITION(ll_addr_mgr_sm_cb_ptr,
                                       LL_ADDR_MGR_SM_INIT_S);
      break;
    case LL_ADDR_MGR_SM_DEFENDING_S:

      return_code = ll_addr_mgr_ps_iface_set_ip_params(ll_addr_mgr_sm_cb_ptr,0);

      if( -1 == return_code)
      {
        LOG_MSG_ERROR_0("Failed to set the IP address on the iface");
        LL_ASSERT(0);
        return;
      }

      time = (int64) ps_utils_generate_16bit_rand_num_by_range(0,
                                          LL_ADDR_MGR_PROBE_WAIT);
      return_code = ps_timer_cancel(ll_addr_mgr_sm_cb_ptr->timer_hndl);
      if( 0 == return_code)
      {
        LOG_MSG_ERROR_0("Failed to cancel the timer");
        LL_ASSERT(0);
        return;
      }

      return_code = (int)ps_timer_start(ll_addr_mgr_sm_cb_ptr->timer_hndl, time);
      if( 0 == return_code)
      {
        LOG_MSG_ERROR_0("Failed to start the timer");
        LL_ASSERT(0);
        return;
      }

      ll_addr_mgr_sm_cb_ptr->retry_cnt =
        ll_addr_mgr_sm_cb_ptr->start_params.params.acquisition_retries;

      LL_ADDR_MGR_FSM_STATE_TRANSITION(ll_addr_mgr_sm_cb_ptr,
                                       LL_ADDR_MGR_SM_INIT_S);
      break;
    case LL_ADDR_MGR_SM_INIT_S:
      LOG_MSG_ERROR_1("Unexpected LL_ADDR_MGR_CONFLICT_EV in state %d",
                ll_addr_mgr_sm_cb_ptr->state);
      LL_ASSERT(0);
      break;

    default:
      LOG_MSG_ERROR_1("Link local address manager in unknown %d state",
                ll_addr_mgr_sm_cb_ptr->state);
      LL_ASSERT(0);
  }

  return;

} /* ll_addr_mgr_sm_process_conflict_ev */

/*===========================================================================
FUNCTION LL_ADDR_MGR_SM_PROCESS_CLAIM_EV()

DESCRIPTION
  This function processes the CLAIM_EV.

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
STATIC void ll_addr_mgr_sm_process_claim_ev
(
  ps_cmd_enum_type cmd,
  void * user_data_ptr
)
{
  ll_addr_mgr_sm_cb_type        *ll_addr_mgr_sm_cb_ptr;
  int                            fd;
  int                            bytes_read;
  void                          *data_ptr = NULL;
  uint32                         new_ip_addr;
  int64                          time;
  int                            temp_val;
  int                            temp_val1;
  int                            temp_val2;
  int                            lsb_ip_address;
  int                            return_code;
  uint32                         instance;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (cmd != PS_LL_ADDR_MGR_CLAIM_CMD)
  {
    LOG_MSG_ERROR_1("PS_LL_ADDR_MGR_CLAIM_CMD handler triggered by wrong cmd (%d)",
              cmd);
    ASSERT(0);
    return;
  }

  instance = (uint32)user_data_ptr;

  if(instance >= LL_ADDR_MGR_CB_INSTANCE_MAX)
  {
    DATA_ERR_FATAL("ll_addr_mgr_sm_process_claim_ev called with invalid handle");
    return;
  }

  ll_addr_mgr_sm_cb_ptr = &ll_addr_mgr_sm_cb[instance];

  LOG_MSG_INFO2_1("Link local address manager processing LL_ADDR_MGR_CLAIM_EV \
          in state %d", ll_addr_mgr_sm_cb_ptr->state);


  return_code = ll_addr_mgr_ps_iface_set_ip_params(ll_addr_mgr_sm_cb_ptr, 0);

  if( -1 == return_code)
  {
    LOG_MSG_ERROR_0("Failed to set the IP address on the iface");
    LL_ASSERT(0);
    return;
  }

  LOG_MSG_INFO2_1( "Opening ip configuration file: %s", filename);

  /*-------------------------------------------------------------------------
  Synchronous call to open the file
  -------------------------------------------------------------------------*/
  fd = mcfg_fopen( filename, 
                 O_RDONLY,
                   S_IREAD, 
                   MCFG_FS_TYPE_EFS,
                   MCFG_FS_SUBID_NONE );

  if( fd < 0 )
  {
    /*-----------------------------------------------------------------------
    Nothing further to do with file - abort read
    -----------------------------------------------------------------------*/
    LOG_MSG_ERROR_1( "Error opening file: %s", filename);

  }
  else
  {
  /*-------------------------------------------------------------------------
  Proceed to read the data synchronously from file
  -------------------------------------------------------------------------*/

    bytes_read = mcfg_fread( fd,
                           data_ptr,
                             LL_ADDR_MGR_CONFIG_IP_BUFFER_SIZE,
                             MCFG_FS_TYPE_EFS);

    if( bytes_read < 0 )
    {
      LOG_MSG_ERROR_1( "Error reading file: %s", filename);
    }

    /* Close the file                                                        */
    if( mcfg_fclose( fd, MCFG_FS_TYPE_EFS ) < 0 )
    {
      LOG_MSG_ERROR_1( "Error closing file: %s", filename);
    }
  }

  /*-----------------------------------------------------------------------
  As per the RFC 3927 the link local address (IPv4) should be in the range
  169.254.1.0 to 169.254.254.255
  ------------------------------------------------------------------------*/
  temp_val  = 169;
  temp_val1 = 254;
  temp_val2 = 1;

  if((NULL != data_ptr) &&
     ((((uint32)(*(int *)data_ptr) >> 16)& LL_ADDR_MGR_BIT_MASK) ==
                                                        LL_ADDR_MGR_IP_MSB))
  {
    new_ip_addr = (uint32)*(int *)data_ptr;
  }
  else
  {
    lsb_ip_address = (int) ps_utils_generate_16bit_rand_num_by_range(0,
                                                LL_ADDR_MGR_IP_LSB_MAX);

    new_ip_addr =
      temp_val | (temp_val1 << 8) |(temp_val2 << 16) | (lsb_ip_address << 16);
  }

  ll_addr_mgr_sm_cb_ptr->ip_address = new_ip_addr;

  time = (int64) ps_utils_generate_16bit_rand_num_by_range(0,
                                      LL_ADDR_MGR_PROBE_WAIT);

  return_code = (int)ps_timer_start(ll_addr_mgr_sm_cb_ptr->timer_hndl, time);

  if( 0 == return_code)
  {
    LOG_MSG_ERROR_0("Failed to start the timer");
    LL_ASSERT(0);
    return;
  }

  LL_ADDR_MGR_FSM_STATE_TRANSITION(ll_addr_mgr_sm_cb_ptr,
                                   LL_ADDR_MGR_SM_INIT_S);

  return;

} /* ll_addr_mgr_sm_process_claim_ev */

/*===========================================================================
FUNCTION LL_ADDR_MGR_SM_PROCESS_PROBE_FAIL_EV()

DESCRIPTION
  This function processes the PROBE_FAIL_EV.

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
STATIC void ll_addr_mgr_sm_process_probe_fail_ev
(
  ll_addr_mgr_sm_cb_type *ll_addr_mgr_sm_cb_ptr
)
{
  int                            fd;
  int                            bytes_written;
  arp_probe_ex_params_type       probe_params;
  void                          *user_data = NULL;
  int                            return_code;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( NULL == ll_addr_mgr_sm_cb_ptr)
  {
    DATA_ERR_FATAL("NULL ll_addr_mgr_sm_cb_ptr passed for processing PS_LL_ADDR_MGR_PROBE_FAIL_EV event");
    return;
  }

  LOG_MSG_INFO2_1("Link local address manager processing PS_LL_ADDR_MGR_PROBE_FAIL_EV \
          in state %d", ll_addr_mgr_sm_cb_ptr->state);

  switch (ll_addr_mgr_sm_cb_ptr->state)
  {
    case LL_ADDR_MGR_SM_PROBING_S:

      return_code =
        ll_addr_mgr_ps_iface_set_ip_params(ll_addr_mgr_sm_cb_ptr,
                                      ll_addr_mgr_sm_cb_ptr->ip_address);
      if( -1 == return_code)
      {
        LOG_MSG_ERROR_0("Failed to set the IP address on the iface");
        LL_ASSERT(0);
        return;
      }

      LOG_MSG_INFO2_1( "Opening ip configuration file: %s", filename);

      /*------------------------------------------------------------------------
      Synchronous call to open the file
      ------------------------------------------------------------------------*/
      fd = mcfg_fopen( filename,
                     O_RDONLY,
                       S_IREAD|S_IWRITE,
                       MCFG_FS_TYPE_EFS,
                       MCFG_FS_SUBID_NONE);

      if( fd < 0 )
      {
        /*----------------------------------------------------------------------
        Nothing further to do with file - abort write
        ----------------------------------------------------------------------*/
        LOG_MSG_INFO2_1( "Error opening file: %s", filename);

      }
      else
      {
      /*------------------------------------------------------------------------
      Proceed to write the data to file - synchronous
      ------------------------------------------------------------------------*/
        bytes_written = mcfg_fwrite( fd,
                                   &ll_addr_mgr_sm_cb_ptr->ip_address,
                                     LL_ADDR_MGR_CONFIG_IP_BUFFER_SIZE,
                                     MCFG_FS_TYPE_EFS);

        if( bytes_written < 0 )
        {
          LOG_MSG_ERROR_1( "Error writing to file: %s", filename);
        }

      /* Close the file                                                       */
        if( mcfg_fclose( fd, MCFG_FS_TYPE_EFS ) < 0 )
        {
          LOG_MSG_ERROR_1( "Error closing file: %s", filename);
        }
      }

      (*ll_addr_mgr_sm_cb_ptr->ind_cback)(LL_ADDR_MGR_IP_CLAIMED_IND,
                                          ll_addr_mgr_sm_cb_ptr->start_params.user_data_ptr);
      probe_params.probe_type = ARP_BROADCAST_PROBE;
      probe_params.ip_addr = ll_addr_mgr_sm_cb_ptr->ip_address;
      probe_params.num_retries = LL_ADDR_MGR_ANNOUNCE_NUM;
      probe_params.not_confident = FALSE;
      probe_params.randomize_retry_intvl = FALSE;
      probe_params.retry_intvl = LL_ADDR_MGR_PROBE_MIN;
      probe_params.max_random_incr = 0;
      probe_params.response_wait_time = 0;
      user_data = (void *)ll_addr_mgr_sm_cb_ptr->instance;
      return_code = arp_probe_ex(ll_addr_mgr_sm_cb_ptr->iface_ptr,
                                 &probe_params,
                                 ll_addr_mgr_arp_probe_cback,
                                 user_data);
      if( -1 == return_code)
      {
        LOG_MSG_ERROR_0("Probe announce failed");
        LL_ASSERT(0);
        return;
      }

      ll_addr_mgr_sm_cb_ptr->num_conflicts = 0;
      return_code = ps_timer_cancel(ll_addr_mgr_sm_cb_ptr->timer_hndl);
      if( 0 == return_code)
      {
        LOG_MSG_ERROR_0("Failed to cancel the timer");
        LL_ASSERT(0);
        return;
      }

      LL_ADDR_MGR_FSM_STATE_TRANSITION(ll_addr_mgr_sm_cb_ptr,
                                       LL_ADDR_MGR_SM_CLAIMED_S);
      break;
    case LL_ADDR_MGR_SM_CLAIMED_S:
    case LL_ADDR_MGR_SM_DEFENDING_S:
      LOG_MSG_INFO2_1("Ignore the LL_ADDR_MGR_PROBE_FAIL_EV in state %d",
              ll_addr_mgr_sm_cb_ptr->state);
      break;
    case LL_ADDR_MGR_SM_INIT_S:
      LOG_MSG_ERROR_1("Unexpected LL_ADDR_MGR_PROBE_FAIL_EV in state %d",
                ll_addr_mgr_sm_cb_ptr->state);
      LL_ASSERT(0);
      break;

    default:
      LOG_MSG_ERROR_1("Link local address manager in unknown %d state",
                ll_addr_mgr_sm_cb_ptr->state);
      LL_ASSERT(0);
  }

  return;

} /* ll_addr_mgr_sm_process_probe_fail_ev */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                             CALLBACK FUNCTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
FUNCTION LL_ADDR_MGR_ARP_PROBE_CBACK()

DESCRIPTION
  This callback is registered with ARP for ARP probing outcome notification
  (ARP_PROBE_FAILURE, ARP_PROBE_SUCCESS).
  Depending on the outcome probe_fail event or conflict event is posted.

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ll_addr_mgr_arp_probe_cback
(
  void              *user_data,
  arp_probe_status   status
)
{
  ll_addr_mgr_sm_cb_type        *ll_addr_mgr_sm_cb_ptr;
  int                            return_code;
  uint32                         instance;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  instance = (uint32)user_data;

  if(instance >= LL_ADDR_MGR_CB_INSTANCE_MAX)
  {
    DATA_ERR_FATAL("ll_addr_mgr_arp_probe_cback called with invalid handle");
    return;
  }

  ll_addr_mgr_sm_cb_ptr = &ll_addr_mgr_sm_cb[instance];

  LL_ADDR_MGR_ENTER_CRIT_SECTION();
  if(FALSE == ll_addr_mgr_sm_cb_ptr->initialized)
  {
    LOG_MSG_ERROR_0("Wrong handle passed back in the callback");
    LL_ADDR_MGR_BRANCH_LEAVE_CRIT_SECTION();
    LL_ASSERT(0);
    return;
  }
  LL_ADDR_MGR_LEAVE_CRIT_SECTION();


  if(ARP_PROBE_FAILURE == status)
  {
    return_code = ll_addr_mgr_post_ev(
      LL_ADDR_MGR_PROBE_FAIL_EV, ll_addr_mgr_sm_cb_ptr);
    if ( -1 == return_code )
    {
      LOG_MSG_ERROR_1("Failed to post event %d", LL_ADDR_MGR_PROBE_FAIL_EV);
      LL_ASSERT(0);
      return;
    }
  }
  else if(ARP_PROBE_SUCCESS == status)
  {
    return_code = ll_addr_mgr_post_ev(
      LL_ADDR_MGR_CONFLICT_EV, ll_addr_mgr_sm_cb_ptr);
    if ( -1 == return_code )
    {
      LOG_MSG_ERROR_1("Failed to post event %d", LL_ADDR_MGR_CONFLICT_EV);
      LL_ASSERT(0);
      return;
    }

  }
} /* ll_addr_mgr_arp_probe_cback */

/*===========================================================================
FUNCTION LL_ADDR_MGR_CONFLICT_CBACK()

DESCRIPTION
  This callback is registered with ARP to notify of an IP conflict at any point
  any time.

  Posts a conflict event.

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ll_addr_mgr_conflict_cback
(
  void *user_data
)
{
  ll_addr_mgr_sm_cb_type *ll_addr_mgr_sm_cb_ptr;
  int                     return_code;
  uint32                  instance;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  instance = ((ll_addr_mgr_sm_cb_type *)user_data)->instance;

  if(instance >= LL_ADDR_MGR_CB_INSTANCE_MAX)
  {
    DATA_ERR_FATAL("ll_addr_mgr_conflict_cback called with invalid handle");
    return;
  }

  ll_addr_mgr_sm_cb_ptr = &ll_addr_mgr_sm_cb[instance];

  LL_ADDR_MGR_ENTER_CRIT_SECTION();
  if(FALSE == ll_addr_mgr_sm_cb_ptr->initialized)
  {
    LOG_MSG_ERROR_0("Wrong handle passed back in the callback");
    LL_ADDR_MGR_BRANCH_LEAVE_CRIT_SECTION();
    LL_ASSERT(0);
    return;
  }
  LL_ADDR_MGR_LEAVE_CRIT_SECTION();

  return_code = ll_addr_mgr_post_ev(
    LL_ADDR_MGR_CONFLICT_EV, ll_addr_mgr_sm_cb_ptr);

  if ( -1 == return_code )
  {
    LOG_MSG_ERROR_1("Failed to post event %d", LL_ADDR_MGR_CONFLICT_EV);
    ASSERT(0);
    return;
  }

} /* ll_addr_mgr_conflict_cback */

/*===========================================================================
FUNCTION LL_ADDR_MGR_TIMER_CBACK()

DESCRIPTION
  This callback is registered with PS Timer.
  When a timer expires, posts a timer event.

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ll_addr_mgr_timer_cback
(
  void *user_data
)
{
  ll_addr_mgr_sm_cb_type *ll_addr_mgr_sm_cb_ptr;
  int                     return_code;
  uint32                  instance;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  instance = (uint32)user_data;

  if(instance >= LL_ADDR_MGR_CB_INSTANCE_MAX)
  {
    DATA_ERR_FATAL("ll_addr_mgr_timer_cback called with invalid handle");
    return;
  }

  ll_addr_mgr_sm_cb_ptr = &ll_addr_mgr_sm_cb[instance];

  LL_ADDR_MGR_ENTER_CRIT_SECTION();
  if(FALSE == ll_addr_mgr_sm_cb_ptr->initialized)
  {
    LOG_MSG_ERROR_0("Wrong handle passed back in the callback");
    LL_ADDR_MGR_BRANCH_LEAVE_CRIT_SECTION();
    LL_ASSERT(0);
    return;
  }
  LL_ADDR_MGR_LEAVE_CRIT_SECTION();

  return_code = ll_addr_mgr_post_ev(LL_ADDR_MGR_TIMER_EV,ll_addr_mgr_sm_cb_ptr);
  if ( -1 == return_code )
  {
    LOG_MSG_ERROR_1("Failed to post event %d", LL_ADDR_MGR_TIMER_EV);
    ASSERT(0);
    return;
  }

} /* ll_addr_mgr_timer_cback */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                             UTILITY FUNCTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
FUNCTION LL_ADDR_MGR_PS_IFACE_SET_IP_PARAMS()

DESCRIPTION
  This function sets the IPv4 type ip address on ps_iface

PARAMETERS
  None

RETURN VALUE
  0 if sucessful
  -1 if fails


DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ll_addr_mgr_ps_iface_set_ip_params
(
  ll_addr_mgr_sm_cb_type *ll_addr_mgr_sm_cb_ptr,
  uint32  ip_address
)
{
  ps_ip_addr_type                ip_addr;
  int                            return_code;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ip_addr.type    = IPV4_ADDR;
  ip_addr.addr.v4.ps_s_addr = ip_address;

  return_code = ps_iface_set_addr(ll_addr_mgr_sm_cb_ptr->iface_ptr, &ip_addr);

  if(0 == return_code)
  {
    ll_addr_mgr_sm_cb_ptr->iface_ptr->v4_net_info.gateway.ps_s_addr =
      ll_addr_mgr_sm_cb_ptr->ip_params.default_gateway;
    ll_addr_mgr_sm_cb_ptr->iface_ptr->v4_net_info.primary_dns.ps_s_addr =
      ll_addr_mgr_sm_cb_ptr->ip_params.pri_dns_addr;
    ll_addr_mgr_sm_cb_ptr->iface_ptr->v4_net_info.secondary_dns.ps_s_addr =
      ll_addr_mgr_sm_cb_ptr->ip_params.sec_dns_addr;
    ll_addr_mgr_sm_cb_ptr->iface_ptr->v4_net_info.net_mask.ps_s_addr =
      ll_addr_mgr_sm_cb_ptr->ip_params.subnet_mask;
  }

  return return_code;
} /* ll_addr_mgr_ps_iface_set_ip_params */


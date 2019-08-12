/*===========================================================================
                         P S _ P P P _ N C P . C 

DESCRIPTION
  The PPP Network Control Protocol (NCP) Interface
  Contains support for the following network control protocols
  - 3GPP2 Vendor-Specific Network Configuration Protocol (VSNCP)

  Copyright (c) 2009-2014 QUALCOMM Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/linklayer/src/ps_ppp_ncp.c#1 $
  $Author: mplcsds1 $ 
  $DateTime: 2016/12/13 08:00:03 $

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
10/09/14    vl     Stack size optimization changes
05/13/14    cx     Add support for MH query APN info
09/20/13    cx     Add support for BCM in PCO over eHRPD
11/27/12    ash    Serialize VSNCP access in pppi_outgoing (in PS_RM Task) 
                   with VSNCP cleanup in PS task.
11/08/12    ash    Post VSNCP when PPP Device is not configured.
03/20/12    ash    API cleanup to enforce correct call order 
02/08/12    ash    Added NCPI_CB_IS_INITED Macro to check initilization status 
                   of VSNCP control block.
01/17/12    ash    Removed Assert from ncpi_start to avoid 
                   ncpi_start/pppi_abort race condition.
11/29/11    ash    Removed assert from ncpi_abort
09/14/11    op     Klocwork warning fix
05/16/11    ssh    If device not ready, post LCP down in ncpi_start()
03/23/11    op     Data SU API cleanup
02/17/11    op     Compiler warning fix
01/24/11    ash    Moved PPP_RESYNC_EV event to PS task in ncpi_resync, 
                   to avoid race condition.
01/21/11    ash    Initiate VSNCP only if PPP is in Network phase.
12/27/10    rp     RAM optimization changes.
12/05/10    ssh    Added ppp_ncp_abort(). Refactored ncpi_resync().
11/18/10    ssh    Added ppp_ncp_resync()
08/19/10    mga    Added support for sending VSNCP term for last VSNCP
4/19/2010   ash    Added changes to support Dual IP Feature.
08/10/09    mga    Fix in ncpi_stop
07/20/09    jdev   Fixed lint errors   
06/18/09    mga    Added ncpi_resync_all_ncps, ncpi_tear_down_all_ncps.
                   Renamed as ncpi_abort_all_ncps.
03/12/09    ssh    Code review comments for 3GPP2-VSNCP
01/16/09    ssh    Created module

===========================================================================*/
/*===========================================================================

                                INCLUDE FILES

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */
#ifdef FEATURE_DATA_PS_EHRPD
#include "ps_ppp_ncp.h"
#include "ps_ppp_ncpi.h"
#include "ps_ppp_vsncp_3gpp2.h"
#include "ps_mem.h"
#include "ps_ppp.h"
#include "ps_pppi.h"
#include "ps_pppi_fsm.h"
#include "ps_pppi_ipcp.h"
#include "ps_iface.h"
#ifdef FEATURE_DATA_PS_IPV6
#include "ps_ppp_ipv6cp.h"
#endif /* FEATURE_DATA_PS_IPV6 */
#include "ds_Utils_DebugMsg.h"
#include <stringl/stringl.h>
#include "ps_system_heap.h"
/*===========================================================================

                          MACRO DEFINITIONS

===========================================================================*/
/*---------------------------------------------------------------------------
  PS memory size for NCP Control Blocks
---------------------------------------------------------------------------*/
#define PS_PPP_NCP_CB_SIZE  ((sizeof(ncpi_cb_type) + 3) & ~3)

/*===========================================================================

                              DATA DECLARATIONS

===========================================================================*/
/*---------------------------------------------------------------------------
  Allocate memory to hold NCP CBs along with ps_mem header
---------------------------------------------------------------------------*/
static int ps_ppp_ncp_cb_mem[PS_MEM_GET_TOT_SIZE_OPT
                             ( PS_PPP_NCP_CB_NUM,
                               PS_PPP_NCP_CB_SIZE 
                             )];

#ifdef FEATURE_DATA_PS_MEM_DEBUG
/*---------------------------------------------------------------------------
  Array of pointers used to facilitate easy debugging.  The first one points
  to the ps_mem header and the second points to actual buf
---------------------------------------------------------------------------*/
static ps_mem_buf_hdr_type * ps_ppp_ncp_hdr[PS_PPP_NCP_CB_NUM];
static ncpi_cb_type * ps_ppp_ncp_ptr[PS_PPP_NCP_CB_NUM];
#endif /* FEATURE_DATA_PS_MEM_DEBUG */

/*===========================================================================

                          PPP-INTERNAL FUNCTIONS
                       (exported via ps_ppp_ncpi.h)

===========================================================================*/  
/*===========================================================================
FUNCTION NCPI_POWERUP_INIT()

DESCRIPTION
  This function is used to do power-up initialization for NCPs. This
  includes initializing PS mem blocks for the NCP control blocks, and 
  calling any NCP-specific powerup routines.

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ncpi_powerup_init
(
  void
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Initialize memory for all NCP control blocks
  -------------------------------------------------------------------------*/
  if( PS_MEM_POOL_INIT_OPT( PS_MEM_PPP_NCP_CB_TYPE,
                            ps_ppp_ncp_cb_mem,
                            PS_PPP_NCP_CB_SIZE,
                            PS_PPP_NCP_CB_NUM,
                            PS_PPP_NCP_CB_HIGH_WM,
                            PS_PPP_NCP_CB_LOW_WM,
                            NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                            (int *) ps_ppp_ncp_hdr,
                            (int *) ps_ppp_ncp_ptr
#else
                            NULL,
                            NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                            ) == -1)
  {
    DATA_ERR_FATAL("PPP NCP: Instance mem init error");
    return;
  }
  
  /*-------------------------------------------------------------------------
    Power-up initialization for 3GPP2-VSNCP
  -------------------------------------------------------------------------*/
  vsncp_3gpp2_powerup_init();
} /* ncpi_powerup_init() */

/*===========================================================================
FUNCTION NCPI_START()

DESCRIPTION
  This function is used to process the PPP_NCP_START_CMD. LCP will be 
  started first if it has not already been started. Else, only the specified
  NCP instance will be started, unless it has already been started.

PARAMETERS
  (IN) handle: Handle to the control block of the NCP 

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ncpi_start 
(
  ppp_ncp_handle_type handle
)
{
  ncpi_cb_type      * ncp_cb_ptr     = NULL;
  ppp_dev_opts_type * dev_config_ptr = NULL;
  ppp_fsm_type      * ncp_fsm_ptr    = NULL;
  ppp_type          * ppp_cb_ptr;
  ppp_type          * ppp_cb_array = NULL;
  static uint16       session_handle = 1;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Validate the handle passed in
  -------------------------------------------------------------------------*/
  ncp_cb_ptr = (ncpi_cb_type *)handle;
  if( NCPI_CB_IS_VALID( ncp_cb_ptr ) ==  FALSE )
  {
    LOG_MSG_ERROR_0("Invalid NCP control block");
    return;
  }

  LOG_MSG_INFO2_1("ncpi_start - handle %p", handle);
  
  ppp_cb_array = fsm_get_ppp_cb_array();  
  ppp_cb_ptr = &(ppp_cb_array[ncp_cb_ptr->device]);
  ppp_cb_ptr->session_handle = session_handle++;
  
  /*-------------------------------------------------------------------------
    PPP is in Termination Phase, post VSNCP DOWN Event with fail reason as
    PPP_FAIL_REASON_CLOSE_IN_PROGRESS.
    Note: Before posting any event on NCP FSM, VSNCP first has to be
    initialized.
  -------------------------------------------------------------------------*/
  if( ppp_cb_ptr->phase == pppTERMINATE )
  {
    ncp_fsm_ptr = &(ncp_cb_ptr->cb_u.vsncp_3gpp2.fsm);
    if (0 != vsncp_3gpp2_init( ncp_cb_ptr ))
    {
      LOG_MSG_ERROR_0("vsncp_3gpp2_init() returned error");
      return;
    }

    ncp_fsm_ptr->fail_reason = PPP_FAIL_REASON_CLOSE_IN_PROGRESS;

    pppi_call_event_callback( ncp_fsm_ptr, 
                              PPP_PROTOCOL_VSNCP, 
                              PPP_DOWN_EV );

    return;
  }

  /*-------------------------------------------------------------------------
    Initialize the PPP device if needed. To do this, device options 
    previously cached via ppp_device_configure() are applied on top of the 
    default options.
  -------------------------------------------------------------------------*/
  if( ppp_cb_ptr->phase == pppDEAD )
  {
    dev_config_ptr = (ppp_dev_opts_type*)ps_system_heap_mem_alloc(sizeof(ppp_dev_opts_type));    
    if(NULL == dev_config_ptr)
    {
      LOG_MSG_ERROR_0("ncpi_start: Failed to allocate dev_config");  
      return;
    }
    
    ppp_get_default_opts( dev_config_ptr );
    if( pppi_apply_cached_opts( ncp_cb_ptr->device, dev_config_ptr ) != 0 )
    {
      /*-------------------------------------------------------------------------
        Initialize NCP, required for posting an event to Mode Handler.
      -------------------------------------------------------------------------*/
      if (0 != vsncp_3gpp2_init( ncp_cb_ptr ))
      {
        PS_SYSTEM_HEAP_MEM_FREE(dev_config_ptr);
        DATA_ERR_FATAL("vsncp_3gpp2_init() returned error");
        return;
      }

      LOG_MSG_ERROR_0("Device not configured! Posting VSNCP down!");
      pppi_call_event_callback( &(ncp_cb_ptr->cb_u.vsncp_3gpp2.fsm), 
                                PPP_PROTOCOL_VSNCP, 
                                PPP_DOWN_EV );

      /*-------------------------------------------------------------------------
        In usual case NCP CB cleanup happens in pppi_abort after PPP goes down,
        but in this case because the device is not configured cleanup NCP CB. 
      -------------------------------------------------------------------------*/
      LOG_MSG_INFO2_2( "Aborting NCP 0x%x, handle %p",
                       ncp_cb_ptr->ncp, ncp_cb_ptr );
      ncp_cb_ptr->abort_f_ptr( ncp_cb_ptr );

      LOG_MSG_ERROR_0("Device not configured! Posting LCP down!");
      pppi_call_event_callback( &ppp_cb_ptr->fsm[LCP], 
                                PPP_PROTOCOL_LCP, 
                                PPP_DOWN_EV );
      PS_SYSTEM_HEAP_MEM_FREE(dev_config_ptr);
      
      return;
    }
    pppi_dev_init( ppp_cb_ptr, dev_config_ptr );
    PS_SYSTEM_HEAP_MEM_FREE(dev_config_ptr);
    
  }

  /*-------------------------------------------------------------------------
    Administratively open the LCP FSM
  -------------------------------------------------------------------------*/
  fsm_open( ppp_cb_ptr->fsm + LCP );
  
  /*-------------------------------------------------------------------------
    If needed, initialize the NCP being brought up and administratively open
    it's FSM
  -------------------------------------------------------------------------*/
  switch( ncp_cb_ptr->ncp )
  {
    case PPP_NCP_VSNCP_3GPP2:
      ncp_fsm_ptr = &(ncp_cb_ptr->cb_u.vsncp_3gpp2.fsm);
      if( ncp_fsm_ptr->state == fsmINITIAL )
      {
        if (0 != vsncp_3gpp2_init( ncp_cb_ptr ))
        {
          LOG_MSG_ERROR_0("vsncp_3gpp2_init() returned error");
          return;
        }
        fsm_open( ncp_fsm_ptr );
      }
      break;

    case PPP_NCP_IPCP:
    case PPP_NCP_IPV6CP:
    default:
      /* unexpected! */
      LOG_MSG_ERROR_1("Unsupported NCP 0x%x", ncp_cb_ptr->ncp);
      ASSERT(0);
      return;
  } /* switch( ncp ) */
  
  /*-------------------------------------------------------------------------
    If LCP has not been started yet, start it.
  -------------------------------------------------------------------------*/
  if( ppp_cb_ptr->fsm[LCP].state == fsmSTARTING )
  {
    pppi_dev_start( ppp_cb_ptr );
  }
   
    /*-------------------------------------------------------------------------
    If PPP is already in Network phase,
    start the NCP if it needs to be started.
    -------------------------------------------------------------------------*/
  if( ppp_cb_ptr->phase == pppNETWORK )
  {
    switch( ncp_cb_ptr->ncp )
    {
      case PPP_NCP_VSNCP_3GPP2:
        if( ncp_fsm_ptr->state == fsmSTARTING )
        { 
          vsncp_3gpp2_start( ncp_cb_ptr );
        } 
        break;
  
      case PPP_NCP_IPCP:
      case PPP_NCP_IPV6CP:
      default:
        /* unexpected! */
        LOG_MSG_ERROR_1("Unsupported NCP 0x%x", ncp_cb_ptr->ncp);
        ASSERT(0);
        break;
    } /* switch( ncp ) */
  } /* PPP in Network phase */
} /* ncpi_start() */

/*===========================================================================
FUNCTION NCPI_STOP()

DESCRIPTION
  This function is used to process the PPP_NCP_STOP_CMD

PARAMETERS
  (IN) handle: Handle to the control block of the NCP 

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ncpi_stop 
(
  ppp_ncp_handle_type handle
)
{
  ppp_fsm_type * fsm_ptr = NULL;
  ncpi_cb_type * ncp_cb_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /*-------------------------------------------------------------------------
    Validate the handle passed in
  -------------------------------------------------------------------------*/
  ncp_cb_ptr = (ncpi_cb_type *)handle;
  if( NCPI_CB_IS_INITED( ncp_cb_ptr ) ==  FALSE )
  {
    LOG_MSG_ERROR_0("Invalid NCP control block in ncpi_stop");
    return;
  }

  LOG_MSG_INFO2_1("ncpi_stop - handle %p", handle);

  switch( ncp_cb_ptr->ncp )
  {
    case PPP_NCP_VSNCP_3GPP2:
      fsm_ptr = &(ncp_cb_ptr->cb_u.vsncp_3gpp2.fsm);
      break;

    case PPP_NCP_IPCP:
    case PPP_NCP_IPV6CP:
    default:
      /* unexpected! */
      LOG_MSG_ERROR_1("Unsupported NCP 0x%x", ncp_cb_ptr->ncp);
      ASSERT(0);
      return;
  } /* switch( ncp ) */

  fsm_close( fsm_ptr );
  if( fsm_ptr->state != fsmCLOSING )
  {
    fsm_ptr->mode = PPP_CP_MODE_INVALID;
  }

} /* ncpi_stop() */

/*===========================================================================
FUNCTION NCPI_RESYNC()

DESCRIPTION
  This function is used to process the PPP_NCP_RESYNC_CMD

PARAMETERS
  (IN) handle: Handle to the control block of the NCP 

RETURN VALUE
  None

DEPENDENCIES
  ppp_ncp_start should have been called prior to this

SIDE EFFECTS
  None
===========================================================================*/
void ncpi_resync 
(
  ppp_ncp_handle_type handle
)
{
  ppp_fsm_type * fsm_ptr = NULL;
  ncpi_cb_type * ncp_cb_ptr = NULL;
  ppp_type * ppp_cb_ptr = NULL;
  ppp_type * ppp_cb_array = NULL;
  ppp_protocol_e_type protocol;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /*-------------------------------------------------------------------------
    Validate the handle passed in
  -------------------------------------------------------------------------*/
  ncp_cb_ptr = (ncpi_cb_type *)handle;
  if( NCPI_CB_IS_INITED( ncp_cb_ptr ) ==  FALSE )
  {
    LOG_MSG_ERROR_0("Invalid NCP control block in ncpi_resync");
    return;
  }

  LOG_MSG_INFO2_1("ncpi_resync - handle %p", handle);
  /*-------------------------------------------------------------------------
    Figure out FSM for NCP
  -------------------------------------------------------------------------*/
  switch( ncp_cb_ptr->ncp )
  {
    case PPP_NCP_VSNCP_3GPP2: 
      fsm_ptr = &(ncp_cb_ptr->cb_u.vsncp_3gpp2.fsm);

      pppi_call_event_callback( fsm_ptr, 
                                PPP_PROTOCOL_VSNCP, 
                                PPP_RESYNC_EV );

      protocol = PPP_PROTOCOL_VSNCP;
      break;

    case PPP_NCP_IPCP:
    case PPP_NCP_IPV6CP:
    default:
      /* unexpected! */
      LOG_MSG_ERROR_1( "Unsupported NCP 0x%x", ncp_cb_ptr->ncp);
      return;
  } /* switch( ncp ) */

  /*-------------------------------------------------------------------------
    Process NCP resyncs only if there is a valid session_handle, and
    LCP is open
  -------------------------------------------------------------------------*/
  ppp_cb_array = fsm_get_ppp_cb_array();
  ppp_cb_ptr = ppp_cb_array + ncp_cb_ptr->device;
  if( ( ppp_cb_ptr->session_handle == 0 ) ||
      ( ppp_cb_ptr->fsm[LCP].state != fsmOPENED ) )
  {
    LOG_MSG_ERROR_2("Cannot resync NCP, session handle %d, LCP state 0x%x",
                    ppp_cb_ptr->session_handle, ppp_cb_ptr->fsm[LCP].state);
    pppi_call_event_callback( fsm_ptr, 
                              protocol, 
                              PPP_DOWN_EV );
    return;
  }
  
  /*-------------------------------------------------------------------------
    Resync the NCP if it has been started
  -------------------------------------------------------------------------*/
  if( ( fsm_ptr != NULL ) && ( fsm_ptr->state != fsmINITIAL ) )
  {
    fsm_init( fsm_ptr ); /* go to INITIAL  */
    fsm_open( fsm_ptr ); /* go to STARTING */
    fsm_up( fsm_ptr );   /* send C-Req out */
    pppi_report_event_to_diag( EVENT_PPP,
                               fsm_ptr->device,
                               protocol,
                               PS_PPP_EVENT_UNKNOWN,
                               PS_PPP_EVENT_RESYNC );
  }
  else
  {
    LOG_MSG_ERROR_0("NCP hasn't been started, cannot resync!");
    pppi_call_event_callback( fsm_ptr, 
                              protocol, 
                              PPP_DOWN_EV );
    return;
  }
} /* ncpi_resync() */

/*===========================================================================
FUNCTION NCPI_ABORT()

DESCRIPTION
  This function is used to process the PPP_NCP_ABORT_CMD

PARAMETERS
  (IN) handle: Handle to the control block of the NCP 

RETURN VALUE
  None

DEPENDENCIES
  ppp_ncp_start should have been called prior to this

SIDE EFFECTS
  None
===========================================================================*/
void ncpi_abort
(
  ppp_ncp_handle_type handle
)
{
  ncpi_cb_type * ncp_cb_ptr = NULL;
  ppp_dev_enum_type   device;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /*-------------------------------------------------------------------------
    Validate the handle passed in
  -------------------------------------------------------------------------*/
  ncp_cb_ptr = (ncpi_cb_type *)handle;
  if( NCPI_CB_IS_VALID( ncp_cb_ptr ) ==  FALSE )
  {
    LOG_MSG_ERROR_0("Invalid NCP control block in ncpi_abort");
    return;
  }

  /*-------------------------------------------------------------------------
    Get PPP device
  -------------------------------------------------------------------------*/
  device = ncp_cb_ptr->device;
  if( device >= PPP_MAX_DEV )
  {
    LOG_MSG_ERROR_0("Invalid NCP Device ");
    return;
  }
  /*-------------------------------------------------------------------------
    Serialize VSNCP control block access between PS_RM_task & PS_task.
  -------------------------------------------------------------------------*/
  PS_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(device) );

  LOG_MSG_INFO2_1("ncpi_abort - handle %p", handle);

  /*-------------------------------------------------------------------------
    Abort the NCP
  -------------------------------------------------------------------------*/
  if( ncp_cb_ptr->abort_f_ptr != NULL )
  {
    LOG_MSG_INFO2_2( "Aborting NCP 0x%x, handle %p",
                     ncp_cb_ptr->ncp, ncp_cb_ptr );
    ncp_cb_ptr->abort_f_ptr( ncp_cb_ptr );
  }
  else
  {
     DATA_ERR_FATAL("NULL abort_f_ptr!");
  }

  PS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
} /* ncpi_abort() */

/*===========================================================================
FUNCTION NCPI_NETWORK_START()

DESCRIPTION
  This function will start the PPP network-layer protocol phase
  (it starts all the active network control protocols)

PARAMETERS
  (IN) device: PPP device

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ncpi_network_start
( 
  ppp_dev_enum_type device
)
{
  uint8 index = 0;
  ppp_type * ppp_cb_ptr = NULL;
  ppp_type * ppp_cb_array = NULL;
  ncpi_cb_type * ncp_cb_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( device >= PPP_MAX_DEV )
  {
    LOG_MSG_ERROR_1("Invalid device 0x%x passed", device);
    return;
  }

  LOG_MSG_INFO2_1("PPP device %d entering network phase", device);
  ppp_cb_array = fsm_get_ppp_cb_array();
  ppp_cb_ptr = &ppp_cb_array[device];

  /*-------------------------------------------------------------------------
    Start all the active NCPs
  -------------------------------------------------------------------------*/
  for(index = 0; index < PS_PPP_NCP_CB_NUM; index++ )
  {
    ncp_cb_ptr =
      (ncpi_cb_type *)ps_mem_index_to_buf( index,
                                           PS_MEM_PPP_NCP_CB_TYPE );
    if( ( NCPI_CB_IS_INITED( ncp_cb_ptr ) == TRUE )  &&
        ( ncp_cb_ptr->device == device ) )
    {
      switch( ncp_cb_ptr->ncp )
      {
        case PPP_NCP_VSNCP_3GPP2:
          /*-----------------------------------------------------------------
            fsm mode is always ACTIVE for 3GPP2-VSNCP, so just start it
          -----------------------------------------------------------------*/
          LOG_MSG_INFO2_2( "Starting 3GPP2-VSNCP - dev %d, handle %p",
                           device, ncp_cb_ptr );
          vsncp_3gpp2_start( ncp_cb_ptr );
          break;

        case PPP_NCP_IPCP:
        case PPP_NCP_IPV6CP:
        default:
          LOG_MSG_ERROR_1("Unsupported protocol 0x%x", ncp_cb_ptr->ncp);
          break;
      } /* ncp */
    } 
  } /* for all NCP CBs */

  /*-------------------------------------------------------------------------
    Future: Bringup of IPXCP should also happen via the mechanism above
  -------------------------------------------------------------------------*/
  if( ppp_cb_ptr->fsm[IPCP].mode == PPP_CP_MODE_ACTIVE )
  {
    LOG_MSG_INFO2_1( "Starting IPCP on dev %d", device );
    ipcp_start( ppp_cb_ptr );
  }
#ifdef FEATURE_DATA_PS_IPV6
  if( ppp_cb_ptr->fsm[IPV6CP].mode == PPP_CP_MODE_ACTIVE )
  {
    LOG_MSG_INFO2_1( "Starting IPv6CP on dev %d", device );
    ipv6cp_start( ppp_cb_ptr );
  }
#endif /* FEATURE_DATA_PS_IPV6 */
} /* ncpi_network_start */

/*===========================================================================
FUNCTION NCPI_ABORT_ALL_NCPS()

DESCRIPTION
  Aborts all NCPs

PARAMETERS
  (IN) device: PPP device

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ncpi_abort_all_ncps
( 
  ppp_dev_enum_type device
)
{
  uint8 index;
  ncpi_cb_type * ncp_cb_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Start all the active NCPs
  -------------------------------------------------------------------------*/
  for(index = 0; index < PS_PPP_NCP_CB_NUM; index++ )
  {
    ncp_cb_ptr =
      (ncpi_cb_type *)ps_mem_index_to_buf( index,
                                           PS_MEM_PPP_NCP_CB_TYPE );
    if( ( NCPI_CB_IS_INITED( ncp_cb_ptr ) == TRUE )  &&
        ( ncp_cb_ptr->device == device ) )
    {
      if( ncp_cb_ptr->abort_f_ptr != NULL )
      {
        LOG_MSG_INFO2_2( "Aborting NCP 0x%x, handle %p",
                         ncp_cb_ptr->ncp, ncp_cb_ptr );
        ncp_cb_ptr->abort_f_ptr( ncp_cb_ptr );
      }
    } 
  } /* for all NCP CBs */
}

/*===========================================================================

                         PPP-EXTERNAL FUNCTIONS
                       (exported via ps_ppp_ncp.h)

===========================================================================*/
/*===========================================================================
FUNCTION PPP_NCP_ALLOC()

DESCRIPTION
  This function is used to allocate an NCP instance. A handle to the 
  allocated instance is returned to the caller.

PARAMETERS
  (IN) device: PPP device
  (IN) ncp: The network control protocol 
  (IN) ncp_config_ptr: Pointer to the NCP configuration

RETURN VALUE
  ppp_ncp_handle_type: Handle to the allocated NCP instance on success
  NULL on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
ppp_ncp_handle_type ppp_ncp_alloc
(
  ppp_dev_enum_type           device,
  ppp_ncp_e_type              ncp,
  ppp_ncp_config_union_type * ncp_config_ptr   
)
{
  ppp_ncp_handle_type ncp_handle = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /*-------------------------------------------------------------------------
    Validate arguments
  -------------------------------------------------------------------------*/
  if( device >= PPP_MAX_DEV   || 
      ncp >= PPP_NCP_NAME_MAX || 
      ncp_config_ptr == NULL )
  {
    DATA_ERR_FATAL("Invalid params to ppp_ncp_alloc");
    return NULL;
  }

  /*-------------------------------------------------------------------------
    Allocate an NCP instance
  -------------------------------------------------------------------------*/
  switch( ncp )
  {
    case PPP_NCP_VSNCP_3GPP2:
      ncp_handle = vsncp_3gpp2_alloc( device, 
                                      &(ncp_config_ptr->vsncp_3gpp2) );
      if( ncp_handle == NULL )
      {
        LOG_MSG_ERROR_0("Unable to allocate an 3GPP2-VSNCP instance");
      }
      break;

    case PPP_NCP_IPCP:
    case PPP_NCP_IPV6CP:
    default:
      /* unexpected! */
      LOG_MSG_ERROR_1("Unsupported NCP 0x%x", ncp);
      ASSERT(0);
      break;
  } /* switch( ncp ) */

  LOG_MSG_INFO2_3( "ppp_ncp_alloc(dev %d, ncp 0x%x) -> new instance %p",
                   device, ncp, ncp_handle );
  return ncp_handle;
} /* ppp_ncp_alloc() */

/*===========================================================================
FUNCTION PPP_NCP_START()

DESCRIPTION
  Starts the specified NCP instance

PARAMETERS
  (IN) handle: The NCP handle allocated via ppp_ncp_alloc

RETURN VALUE
  0: Success
 -1: Failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ppp_ncp_start 
(
  ppp_ncp_handle_type handle
)
{
  uint8 index = 0;
  ncpi_cb_type * ncp_cb_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( handle == NULL )
  {
    LOG_MSG_ERROR_0("NULL handle passed");
    return -1;
  }

  LOG_MSG_INFO2_1("ppp_ncp_start - handle %p", handle);
  /*-------------------------------------------------------------------------
    Check if a matching control block exists
  -------------------------------------------------------------------------*/
  for(index = 0; index < PS_PPP_NCP_CB_NUM; index++ )
  {
    ncp_cb_ptr =
      (ncpi_cb_type *)ps_mem_index_to_buf( index,
                                           PS_MEM_PPP_NCP_CB_TYPE );
    if( ( NCPI_CB_IS_VALID( ncp_cb_ptr ) == TRUE )  && 
        ( ncp_cb_ptr == (ncpi_cb_type *)handle ) ) 
    {
      LOG_MSG_INFO2_1("Matching NCP handle found: %p", ncp_cb_ptr);
      /*---------------------------------------------------------------------
        Found a match, send command to PS
      ---------------------------------------------------------------------*/
      PPP_SEND_CMD( PPP_NCP_START_CMD, (void *)handle );

      /*---------------------------------------------------------------------
        Flag to indicate NCP start is initiated for this control block.
      ---------------------------------------------------------------------*/
      ncp_cb_ptr->is_ncp_started = TRUE;

      return 0;
    }
  } /* for */

  LOG_MSG_ERROR_1("No match found for handle 0x%x", handle);
  return -1;
} /* ppp_ncp_start() */

/*===========================================================================
FUNCTION PPP_NCP_STOP()

DESCRIPTION
  Stops the specified NCP instance. If this is the only NCP instance active,
  PPP will be terminated.

PARAMETERS
  (IN) handle: The NCP handle allocated via ppp_ncp_alloc

RETURN VALUE
  0: Success
 -1: Failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ppp_ncp_stop 
(
  ppp_ncp_handle_type handle
)
{
  uint8 index = 0;
  ncpi_cb_type * ncp_cb_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( handle == NULL )
  {
    LOG_MSG_ERROR_0("NULL handle passed");
    return -1;
  }

  LOG_MSG_INFO2_1("ppp_ncp_stop - handle %p", handle);
  /*-------------------------------------------------------------------------
    Check if a matching control block exists
  -------------------------------------------------------------------------*/
  for(index = 0; index < PS_PPP_NCP_CB_NUM; index++ )
  {
    ncp_cb_ptr =
      (ncpi_cb_type *)ps_mem_index_to_buf( index,
                                           PS_MEM_PPP_NCP_CB_TYPE );
    if( ( NCPI_CB_IS_VALID( ncp_cb_ptr ) == TRUE )  && 
        ( ncp_cb_ptr == (ncpi_cb_type *)handle ) ) 
    {
      LOG_MSG_INFO2_1("Matching NCP handle found: %p", ncp_cb_ptr);

      /*---------------------------------------------------------------------
        Make sure ppp_ncp_start was called
      ---------------------------------------------------------------------*/
      if (FALSE == ncp_cb_ptr->is_ncp_started)
      {
        LOG_MSG_INFO2_1("ppp_ncp_stop called before ppp_ncp_start! handle = %p",
                        ncp_cb_ptr);
        return -1;
      }
      /*---------------------------------------------------------------------
        Found a match, send command to PS
      ---------------------------------------------------------------------*/
      PPP_SEND_CMD( PPP_NCP_STOP_CMD, (void *)handle );
      return 0;
    }
  } /* for */

  LOG_MSG_ERROR_1("No match found for handle %p", handle);
  return -1;
} /* ppp_ncp_stop */

/*===========================================================================
FUNCTION PPP_NCP_RESYNC()

DESCRIPTION
  Resyncs the specified NCP instance. 

PARAMETERS
  (IN) handle: The NCP handle allocated via ppp_ncp_alloc

RETURN VALUE
  0: Success
 -1: Failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ppp_ncp_resync 
(
  ppp_ncp_handle_type handle
)
{
  uint8 index = 0;
  ncpi_cb_type * ncp_cb_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( handle == NULL )
  {
    LOG_MSG_ERROR_0("NULL handle passed");
    return -1;
  }

  LOG_MSG_INFO2_1("ppp_ncp_resync - handle %p", handle);
  /*-------------------------------------------------------------------------
    Check if a matching control block exists
  -------------------------------------------------------------------------*/
  for(index = 0; index < PS_PPP_NCP_CB_NUM; index++ )
  {
    ncp_cb_ptr =
      (ncpi_cb_type *)ps_mem_index_to_buf( index,
                                           PS_MEM_PPP_NCP_CB_TYPE );
    if( ( NCPI_CB_IS_VALID( ncp_cb_ptr ) == TRUE )  && 
        ( ncp_cb_ptr == (ncpi_cb_type *)handle ) ) 
    {
      LOG_MSG_INFO2_1("Matching NCP handle found: %p", ncp_cb_ptr);

      /*---------------------------------------------------------------------
        Make sure ppp_ncp_start was called
      ---------------------------------------------------------------------*/
      if (FALSE == ncp_cb_ptr->is_ncp_started)
      {
        LOG_MSG_INFO2_1("ppp_ncp_resync called before ppp_ncp_start! handle = %p",
                        ncp_cb_ptr);
        return -1;
      }

      /*---------------------------------------------------------------------
        Found a Match, send NCP Resync command to PS
      ---------------------------------------------------------------------*/
      switch( ncp_cb_ptr->ncp )
      {
        case PPP_NCP_VSNCP_3GPP2:
          PPP_SEND_CMD( PPP_NCP_RESYNC_CMD, (void *)handle );
          break;
        
        case PPP_NCP_IPCP:
        case PPP_NCP_IPV6CP:
        default:
          /* unexpected! */
          LOG_MSG_ERROR_1("Unsupported NCP 0x%x", ncp_cb_ptr->ncp);
          return -1;
      } /* switch( ncp ) */

      return 0;
    }
  } /* for */

  LOG_MSG_ERROR_1("No match found for handle %p", handle);
  return -1;
} /* ppp_ncp_resync */

/*===========================================================================
FUNCTION PPP_NCP_ABORT()

DESCRIPTION
  Aborts the specified NCP instance. 

PARAMETERS
  (IN) handle: The NCP handle allocated via ppp_ncp_alloc

RETURN VALUE
  0: Success
 -1: Failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ppp_ncp_abort
(
  ppp_ncp_handle_type handle
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( !NCPI_CB_IS_VALID( (ncpi_cb_type *)handle ) )
  {
    LOG_MSG_ERROR_1("Invalid handle passed to ppp_ncp_abort %p", handle);
    return -1;
  }

  LOG_MSG_INFO2_1("ppp_ncp_abort - handle %p", handle);
  /*-------------------------------------------------------------------------
    Send NCP Abort command to PS
  -------------------------------------------------------------------------*/
  PPP_SEND_CMD( PPP_NCP_ABORT_CMD, (void *)handle );
  return 0;
} /* ppp_ncp_abort */

/*===========================================================================
FUNCTION PPP_NCP_GET_NEGOTIATED_PARAM

DESCRIPTION
  Get the negotiated parameter from NCP 

PARAMETERS
  (IN) handle: The NCP handle allocated via ppp_ncp_alloc
  (IN) param_name: the parameter name caller asks for
  (OUT) param_data: the value of parameter
  (IN) param_buf_size: the size of param buffer

RETURN VALUE
  0: Success
 -1: Failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ppp_ncp_get_negotiated_param
(
   ppp_ncp_handle_type handle,
   ppp_ncp_negotiated_param_e_type param_name,
   void * data_buf,
   uint8  data_buf_size
) 
{
  
  uint8 index = 0;
  ncpi_cb_type * ncp_cb_ptr = NULL;
  vsncp_3gpp2i_type * vsncp_cb_ptr = NULL;
  uint8 apn_len = 0;
  ppp_ncp_negotiated_apn_type * apn_info_ptr;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( handle == NULL ||
      param_name < PPP_NCP_NEGOTIATED_MIN ||
      param_name >= PPP_NCP_NEGOTIATED_MAX ||
      data_buf == NULL ||
      data_buf_size <= 0)
  {
    LOG_MSG_ERROR_0("Invalid parameters!");
    return -1;
  }

  LOG_MSG_INFO2_2("param: %d is queried, handle:%p", param_name, handle);

  memset(data_buf, 0x00,data_buf_size);
  
  /*-------------------------------------------------------------------------
    Check if a matching control block exists
  -------------------------------------------------------------------------*/
  for(index = 0; index < PS_PPP_NCP_CB_NUM; index++ )
  {
    ncp_cb_ptr =
      (ncpi_cb_type *)ps_mem_index_to_buf( index,
                                           PS_MEM_PPP_NCP_CB_TYPE );
    if( ( NCPI_CB_IS_INITED( ncp_cb_ptr ) == TRUE )  && 
        ( ncp_cb_ptr == (ncpi_cb_type *)handle ) ) 
    {
      LOG_MSG_INFO2_1("Matching NCP handle found: %p", ncp_cb_ptr);
      
      vsncp_cb_ptr = &(ncp_cb_ptr->cb_u.vsncp_3gpp2);

      /*---------------------------------------------------------------------
         Make sure the fsm is valid and opened
         ---------------------------------------------------------------------*/
      if( VSNCP_3GPP2I_FSM_IS_VALID( &(vsncp_cb_ptr->fsm) ) &&
          vsncp_cb_ptr->fsm.state == fsmOPENED)
      {
        switch(param_name)
        {
          case PPP_NCP_VSNCP_3GPP2_NEGOTIATED_BCM:
            if(vsncp_cb_ptr->config.option_vals.pco.bcm_is_supported == FALSE)
            {
              LOG_MSG_ERROR_0("BCM is not supported when it is queried");
              return -1;
            }
              
            memscpy(data_buf,
                    data_buf_size,
                    &(ncp_cb_ptr->cb_u.vsncp_3gpp2.pco_vals.selected_bcm),
                    sizeof(uint8));
            break;
          
          case PPP_NCP_VSNCP_3GPP2_NEGOTIATED_APN:
            
            if(data_buf_size <(sizeof(ppp_ncp_negotiated_apn_type)))
            {
              LOG_MSG_ERROR_0("Data buffer size is too short!");
              return -1;
            }

            apn_len = ncp_cb_ptr->cb_u.vsncp_3gpp2.local_vals.apn_len;
            if( apn_len > 0 && apn_len <= VSNCP_3GPP2_APN_MAX_VAL_LEN)
            {
              apn_info_ptr = (ppp_ncp_negotiated_apn_type*)data_buf;
              apn_info_ptr->apn_len = apn_len;
              apn_info_ptr->apn_is_default = 
                ncp_cb_ptr->cb_u.vsncp_3gpp2.local_vals.apn_is_default; 
              memscpy(apn_info_ptr->apn_name,
                      sizeof(apn_info_ptr->apn_name),
                      ncp_cb_ptr->cb_u.vsncp_3gpp2.local_vals.apn,
                      apn_len);
            }
            break;            
          
          default:
            LOG_MSG_ERROR_1("Unsupported param %d", param_name);
            return -1;
            break;       
        }
      }
      else
      {
        LOG_MSG_ERROR_1("param %d is queried before fsm is opened",
                        param_name);
        return -1;
      } 
      return 0;
    }
  } 

  LOG_MSG_ERROR_1("No match found for handle %p", handle);
  return -1;
}/* ppp_ncp_get_negotiated_param */

/*===========================================================================
FUNCTION NCPI_RESYNC_ALL_NCPS

DESCRIPTION
  Posts the resync event to all the active NCPs.

PARAMETERS
  (IN) device    - PPP device
  (IN) event_ptr - PPP event info for the resync event
  
RETURN VALUE
  None
  
DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ncpi_resync_all_ncps
(
  ppp_dev_enum_type   device,
  ppp_event_info_s_type * event_ptr  
)
{

  ppp_event_mask_e_type  event_mask;
  ncpi_cb_type *ncp_handle_ptr = NULL;
  uint8 index = 0;
  ppp_type * ppp_cb_ptr = NULL;
  ppp_type * ppp_cb_array = NULL;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Verify arguments
  -------------------------------------------------------------------------*/
  if( ( PPP_MAX_DEV <= device )  || ( NULL == event_ptr ) )
  {
    LOG_MSG_ERROR_1( "Invalid args passed. device: %d", device );
    ASSERT( 0 );
    return;
  }

  event_mask = (ppp_event_mask_e_type)(1 << (uint8)PPP_RESYNC_EV);
  ppp_cb_array = fsm_get_ppp_cb_array();
  ppp_cb_ptr = &ppp_cb_array[device];
  LOG_MSG_INFO2_1( "ncpi_all_resync device %d ", device );

  /*---------------------------------------------------------------------------
    Post the resync event to the active NCPs. Loop into ps_mem to find 
    the corresponding NCP control block.
  ---------------------------------------------------------------------------*/
  for( index = 0; index < PS_PPP_NCP_CB_NUM; index++ )
  {
    ncp_handle_ptr = 
      (ncpi_cb_type *)( ps_mem_index_to_buf( index,
                                             PS_MEM_PPP_NCP_CB_TYPE ) );
    if( ( NCPI_CB_IS_INITED( ncp_handle_ptr ) == TRUE ) &&
        ( ncp_handle_ptr->device == device ) )
    {
      switch(ncp_handle_ptr->ncp)
      {
        case PPP_NCP_VSNCP_3GPP2:
          /*-----------------------------------------------------------------
            Call the callback on the VSNCP instance  if it is registered for 
            the event and is not in fsmINITIAL state.
          -----------------------------------------------------------------*/
          if( ( 0 != ( (uint8)event_mask & (uint8)event_ptr->event_mask[VSNCP] ) ) &&
              ( fsmINITIAL != ncp_handle_ptr->cb_u.vsncp_3gpp2.fsm.state ) )
          {
            if( event_ptr->cback[VSNCP] != NULL )
            {
              LOG_MSG_INFO1_1(" Calling cback for NCP instance 0x%x, PPP_RESYNC_EV ",
                              ncp_handle_ptr);
              event_ptr->cback[VSNCP]( event_ptr->device,
                                       PPP_PROTOCOL_VSNCP,
                                       PPP_RESYNC_EV,
                                       event_ptr->user_data[VSNCP],
                                       ppp_cb_ptr->session_handle,
                                       PPP_FAIL_REASON_NONE );

              /*--------------------------------------------------------------
                Post the VSNCP diag event for resync
              --------------------------------------------------------------*/
              NCPI_REPORT_EVENT_TO_DIAG( ncp_handle_ptr, PS_PPP_EVENT_RESYNC );
            } /* if(event_ptr->cback[VSNCP] != NULL)*/
          }
          break;
    
        case PPP_NCP_IPCP:
        case PPP_NCP_IPV6CP:
        default:
          LOG_MSG_ERROR_1("Unsupported NCP 0x%x", ncp_handle_ptr->ncp);
          ASSERT(0);
          break;
    

      } /*switch( ncp )*/
    }/*if ( NCPI_CB_IS_INITED )*/
  }/* for( PS_PPP_NCP_CB_NUM ) */  
}/* ncpi_resync_all_ncps */

/*===========================================================================
FUNCTION NCPI_TEAR_DOWN_ALL_NCPS

DESCRIPTION
  Brings down all the active NCPs. 

PARAMETERS
 (IN) device    - PPP device

RETURN VALUE
  None
  
DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ncpi_tear_down_all_ncps
(
  ppp_dev_enum_type   device
)
{
  ncpi_cb_type *ncp_handle_ptr = NULL;
  uint8 index = 0;
  ppp_fsm_type *ncp_fsm_ptr;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Verify arguments
  -------------------------------------------------------------------------*/
  if( PPP_MAX_DEV <= device )
  {
    LOG_MSG_ERROR_1( "Invalid args passed. device: %d", device );
    ASSERT( 0 );
    return;
  }

  LOG_MSG_INFO2_1( "ncpi_all_bring_down device : %d", device );

  /*---------------------------------------------------------------------------
    Loop into ps_mem to find the corresponding NCP control block.
    Call the corresponding down function of each ncp.
  ---------------------------------------------------------------------------*/
  for( index = 0; index < PS_PPP_NCP_CB_NUM; index++ )
  {
    ncp_handle_ptr = 
      (ncpi_cb_type *)( ps_mem_index_to_buf( index,
                                             PS_MEM_PPP_NCP_CB_TYPE ) );
    if( ( NCPI_CB_IS_INITED( ncp_handle_ptr ) == TRUE ) &&
        ( ncp_handle_ptr->device == device ) )
    {
      switch (ncp_handle_ptr->ncp)
      {
        case PPP_NCP_VSNCP_3GPP2:
          ncp_fsm_ptr = &ncp_handle_ptr->cb_u.vsncp_3gpp2.fsm;
          if( ( ncp_fsm_ptr->pdc != NULL ) &&
              ( ncp_fsm_ptr->pdc->down != NULL ) )
          {
            ncp_fsm_ptr->pdc->down( ncp_fsm_ptr );
          }
          break;

        case PPP_NCP_IPCP:
        case PPP_NCP_IPV6CP:
        default:
          /* unexpected! */
          LOG_MSG_ERROR_1("Unsupported NCP 0x%x", ncp_handle_ptr->ncp);
          ASSERT(0);
          break;
      }/* switch (ncp) */      
    }/* if (NCPI_CB_IS_INITED) */
  }/* for(PS_PPP_NCP_CB_NUM) */    
}/* ncpi_tear_down_all_ncps */

#endif /* FEATURE_DATA_PS_EHRPD */

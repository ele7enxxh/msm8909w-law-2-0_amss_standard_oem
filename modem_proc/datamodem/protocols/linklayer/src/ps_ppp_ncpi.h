#ifndef PS_PPP_NCPI_H
#define PS_PPP_NCPI_H
/*===========================================================================
                         P S _ P P P _ N C P I. H

DESCRIPTION
  These are all the PPP NCP definitions internal to PPP.

  Copyright (c) 2008 - 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/linklayer/src/ps_ppp_ncpi.h#1 $
  $Author: mplcsds1 $
  $DateTime: 2016/12/13 08:00:03 $

 when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
05/13/14    cx     Add support for MH query APN info
06/07/13    cx     Add MTU Size in PCO Over eHRPD
03/20/12    ash    API cleanup to enforce correct call order
02/08/12    ash    Added NCPI_CB_IS_INITED Macro to check initilization status
                   of VSNCP control block.
12/07/11   vpk     Add support for 5 PDNs
===========================================================================*/
/*===========================================================================

                                INCLUDE FILES

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */
#ifdef FEATURE_DATA_PS_EHRPD
#include "ps_ppp_defs.h"
#include "ps_ppp_fsm.h"
#include "ps_ppp.h"
#include "ps_in.h"
#include "ps_ppp_vsncp_3gpp2.h"
#include "ps_ppp_ncp.h"
#include "ps_mem.h"
#include "list.h"
#ifdef FEATURE_STA_PPP
#include "ps_pppi_fsm.h"
#endif /*FEATURE_STA_PPP*/
#include "ds_Utils_DebugMsg.h"
#include "ps_sys.h"

/*===========================================================================

                            MACRO DEFINITIONS

===========================================================================*/
/*---------------------------------------------------------------------------
  Default constants
---------------------------------------------------------------------------*/
#define VSNCP_REQ_TRY       20
#define VSNCP_TERM_TRY      3
#define VSNCP_ACK_TIMEOUT   1000
#define VSNCP_TERM_TIMEOUT  1000

/*---------------------------------------------------------------------------
  PS Mem Buffer counts
---------------------------------------------------------------------------*/
#ifndef FEATURE_DATA_PS_LOW_MEM_CHIPSET
/* Supports max of 8 NCP instances */
#define PS_PPP_NCP_CB_NUM     PS_SYS_MAX_AVAIL_PDNS
#define PS_PPP_NCP_CB_HIGH_WM PS_SYS_MAX_AVAIL_PDNS
#define PS_PPP_NCP_CB_LOW_WM  1

#else

#define PS_PPP_NCP_CB_NUM     2
#define PS_PPP_NCP_CB_HIGH_WM 2
#define PS_PPP_NCP_CB_LOW_WM  1
#endif /* FEATURE_DATA_PS_LOW_MEM_CHIPSET */

/*---------------------------------------------------------------------------
  3GPP2 OUI
---------------------------------------------------------------------------*/
#define PPP_VS_3GPP2_OUI  0xCF0002

/*---------------------------------------------------------------------------
  Supported 3GPP2-VSNCP option types
---------------------------------------------------------------------------*/
#define VSNCP_3GPP2I_PDN_ID            1
#define VSNCP_3GPP2I_APN               2
#define VSNCP_3GPP2I_PDN_TYPE          3
#define VSNCP_3GPP2I_PDN_ADDR          4
#define VSNCP_3GPP2I_PCO               5
#define VSNCP_3GPP2I_ERROR_CODE        6
#define VSNCP_3GPP2I_ATTACH_TYPE       7
#define VSNCP_3GPP2I_DEFAULT_ROUTER    8
#define VSNCP_3GPP2I_ADDR_ALLOC_CAUSE  9
#define VSNCP_3GPP2I_DEFAULT_APN_IND   17
#define VSNCP_3GPP2I_RECONNECT_IND     18
#define VSNCP_3GPP2I_OPTION_LIMIT      18

/*---------------------------------------------------------------------------
  MCCMNC length in operator reserved PCO containter content
---------------------------------------------------------------------------*/
#define VSNCP_3GPP2I_OP_PCO_MCCMNC_LEN 3

/*---------------------------------------------------------------------------
  Maximum length of a operator reserved PCO content:
  253 is the maximum length of protocol configuration options, with 2 bytes
  header and 3 bytes for PCO ID and container content length.
---------------------------------------------------------------------------*/
#define VSNCP_3GPP2I_MAX_OP_PCO_CONTENT_LEN 248
/*===========================================================================

                            DATA DECLARATIONS

===========================================================================*/
/*---------------------------------------------------------------------------
  Values for remote options (options expected from the peer)
---------------------------------------------------------------------------*/
typedef struct vsncp_3gpp2i_remote_opts_val_s
{
  uint8  pdn_id;
} vsncp_3gpp2i_remote_opts_val_type;

typedef struct vsncp_3gpp2i_pco_v4_addr
{
  uint32 addr;
  struct vsncp_3gpp2i_pco_v4_addr * next;
} vsncp_3gpp2i_pco_v4_addr;

typedef struct vsncp_3gpp2i_pco_v6_addr
{
  struct ps_in6_addr addr;
  struct vsncp_3gpp2i_pco_v6_addr * next;
} vsncp_3gpp2i_pco_v6_addr;

typedef struct vsncp_3gpp2i_op_pco_s
{
  uint16 container_id;
  uint8 mcc[PPP_MCC_LEN];
  uint8 mcc_len;
  uint8 mnc[PPP_MNC_LEN];
  uint8 mnc_len;
  uint8 app_specific_info[VSNCP_3GPP2I_MAX_OP_PCO_CONTENT_LEN - VSNCP_3GPP2I_OP_PCO_MCCMNC_LEN];
  uint8 app_specific_info_len;
} vsncp_3gpp2i_op_pco_type;
/*---------------------------------------------------------------------------
  PCO values
---------------------------------------------------------------------------*/
typedef struct vsncp_3gpp2i_pco_opts_val_s
{
  struct ps_in_addr dns_v4_primary;
  struct ps_in_addr dns_v4_secondary;
  vsncp_3gpp2i_pco_v6_addr * dns_v6_head_ptr;
  vsncp_3gpp2i_pco_v4_addr * p_cscf_v4_head_ptr;
  vsncp_3gpp2i_pco_v6_addr * p_cscf_v6_head_ptr;
  uint8 pdn_level_auth_active_id;
  /*---------------------------------------------------------------------------
    Place holder when this container is recvd from the network
  ---------------------------------------------------------------------------*/
  uint8 selected_bcm;
  vsncp_3gpp2i_op_pco_type op_pco_content;
  uint16 mtu;
} vsncp_3gpp2i_pco_opts_val_type;

/*---------------------------------------------------------------------------
  3GPP2 VSNCP control block
---------------------------------------------------------------------------*/
typedef struct vsncp_3gpp2i_cb_s
{
  ppp_vsncp_3gpp2_config_type       config;      /* configuration          */
  ppp_fsm_type                      fsm;         /* finite state machine   */
  vsncp_3gpp2_local_opts_val_type   local_vals;  /* working values         */
  vsncp_3gpp2i_remote_opts_val_type remote_vals; /* working values         */
  vsncp_3gpp2i_pco_opts_val_type    pco_vals;
} vsncp_3gpp2i_type;

/*---------------------------------------------------------------------------
  Type for NCP control blocks
---------------------------------------------------------------------------*/
typedef struct ncpi_cb_s
{
  ppp_dev_enum_type device;
  ppp_ncp_e_type    ncp;

  union
  {
    vsncp_3gpp2i_type  vsncp_3gpp2;
    int                ipcp;   /* dummy */
    int                ipv6cp; /* dummy */
  } cb_u;

  void (*abort_f_ptr)
  (
    struct ncpi_cb_s * ncp_cb_ptr
  );

  struct ncpi_cb_s * this_cb_ptr;
  boolean            is_ncp_started;
} ncpi_cb_type;

/*===========================================================================

                            INLINE FUNCTIONS

===========================================================================*/
/*===========================================================================
INLINE FUNCTION NCPI_CB_IS_VALID()

DESCRIPTION
  This function checks the validity of NCP control block. PPP uses this MACRO
  for APIs where it expects the control block to be valid (allocated) but not
  initialized (started).

PARAMETERS
  (IN) instance_ptr: Pointer to the NCP instance ptr

RETURN VALUE
  TRUE  if the passed instance is valid
  FALSE otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define NCPI_CB_IS_VALID ncpi_cb_is_valid
INLINE boolean ncpi_cb_is_valid
(
  ncpi_cb_type * instance_ptr
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( ( instance_ptr == NULL ) ||
      ( ps_mem_is_valid( instance_ptr,
                         PS_MEM_PPP_NCP_CB_TYPE ) == FALSE ) )
  {
    return FALSE;
  }

  if( ( instance_ptr->this_cb_ptr != instance_ptr ) ||
      ( instance_ptr->device >= PPP_MAX_DEV ) ||
      ( instance_ptr->ncp >= PPP_NCP_NAME_MAX ) )
  {
    return FALSE;
  }

  return TRUE;
} /* ncpi_cb_is_valid() */

/*===========================================================================
INLINE FUNCTION NCPI_CB_IS_INITED()

DESCRIPTION
  This function checks to see that the NCP instance pointer passed in is
  initialized. PPP uses this macro for APIs where it expects the instance
  to be started. Mostly used is PS task context.

PARAMETERS
  (IN) instance_ptr: Pointer to the NCP instance ptr

RETURN VALUE
  TRUE  if the passed instance is valid
  FALSE otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define NCPI_CB_IS_INITED ncpi_cb_is_inited
INLINE boolean ncpi_cb_is_inited
(
  ncpi_cb_type * instance_ptr
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( ( instance_ptr == NULL ) ||
      ( ps_mem_is_valid( instance_ptr,
                         PS_MEM_PPP_NCP_CB_TYPE ) == FALSE ) )
  {
    return FALSE;
  }

  if( ( instance_ptr->this_cb_ptr != instance_ptr ) ||
      ( instance_ptr->device >= PPP_MAX_DEV ) ||
      ( instance_ptr->ncp >= PPP_NCP_NAME_MAX ) ||
      ( instance_ptr->cb_u.vsncp_3gpp2.fsm.pdv != instance_ptr ) )
  {
    return FALSE;
  }

  return TRUE;
} /* ncpi_cb_is_inited() */
/*===========================================================================
INLINE FUNCTION VSNCP_3GPP2I_FSM_IS_VALID()

DESCRIPTION
  This function checks to see that the FSM pointed to by the passed in
  fsm pointer is a 3GPP2-VSNCP FSM. fsm_ptr's pdv points to the control
  block for the NCP and is examined for being correctly set up for
  3GPP2-VSNCP

PARAMETERS
  (IN) fsm_ptr: Pointer to the FSM structure

RETURN VALUE
  TRUE  if the passed FSM is a 3GPP2-VSNCP FSM
  FALSE otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define VSNCP_3GPP2I_FSM_IS_VALID vsncp_3gpp2i_fsm_is_valid
INLINE boolean vsncp_3gpp2i_fsm_is_valid
(
  ppp_fsm_type * fsm_ptr
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( ( fsm_ptr == NULL ) ||
      ( fsm_ptr->pdc == NULL ) ||
      ( fsm_ptr->pdc->protocol != PPP_VSNCP_PROTOCOL ) ||
      ( (ncpi_cb_type *)fsm_ptr->pdv == NULL ) ||
      ( ( (ncpi_cb_type *)fsm_ptr->pdv)->ncp != PPP_NCP_VSNCP_3GPP2 ) )
  {
    return FALSE;
  }

  return TRUE;
} /* vsncp_3gpp2i_fsm_is_valid() */

/*===========================================================================
INLINE FUNCTION VSNCP_3GPP2I_ADD_PDN_ID_OPT()

DESCRIPTION
  This function is used to add the PDN-ID option onto the passed DSM item
  if the passed FSM is a 3GPP2-VSNCP FSM

PARAMETERS
  (IN) fsm_ptr    : Pointer to the FSM structure
  (IN) pkt_ptr_ptr: Pointer to pointer to the DSM item

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define VSNCP_3GPP2I_ADD_PDN_ID_OPT vsncp_3gpp2i_add_pdn_id_opt
INLINE void vsncp_3gpp2i_add_pdn_id_opt
(
  ppp_fsm_type   * fsm_ptr,
  dsm_item_type ** pkt_ptr_ptr
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*------------------------------------------------------------------------
    Validate arguments. The fsm_ptr must be valid. Only the outer
    DSM ptr need be non-NULL. The inner pointer can be NULL - this might
    be the first option added to the packet (which is created bottom-up)
  ------------------------------------------------------------------------*/
  if( ( VSNCP_3GPP2I_FSM_IS_VALID( fsm_ptr ) == TRUE ) &&
      ( pkt_ptr_ptr != NULL ) )
  {
    (*fsm_ptr->pdc->option)( fsm_ptr,
                             fsm_ptr->local.work_pdv,
                             pkt_ptr_ptr,
                             VSNCP_3GPP2I_PDN_ID );
  }
} /* vsncp_3gpp2i_add_pdn_id_opt() */

/*===========================================================================
INLINE FUNCTION VSNCP_3GPP2I_ADD_OUI()

DESCRIPTION
  This function is used to pushdown the 3GPP2-OUI (0xcf0002) onto the
  passed DSM item if the passed FSM is a 3GPP2-VSNCP FSM

PARAMETERS
  (IN) fsm_ptr    : Pointer to the FSM structure
  (IN) pkt_ptr_ptr: Pointer to pointer to the DSM item

RETURN VALUE
   0: Success
  -1: Failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define VSNCP_3GPP2I_ADD_OUI vsncp_3gpp2i_add_oui
INLINE int vsncp_3gpp2i_add_oui
(
  ppp_fsm_type   * fsm_ptr,
  dsm_item_type ** pkt_ptr_ptr
)
{
  uint8 buf[PPP_OUI_LEN];
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*------------------------------------------------------------------------
    Validate arguments. The fsm_ptr must be valid. Both outer and inner
    DSM ptrs must be valid - OUI is added on top of VSNCP packet 'Data'
    contents (i.e. the options), so the inner pointer MUST be non-NULL.
  ------------------------------------------------------------------------*/
  if( ( VSNCP_3GPP2I_FSM_IS_VALID( fsm_ptr ) == TRUE ) &&
      ( pkt_ptr_ptr != NULL ) &&
      ( *pkt_ptr_ptr != NULL ) )
  {
    buf[0] = 0xcf;
    buf[1] = 0x00;
    buf[2] = 0x02;

    if( dsm_pushdown_packed( pkt_ptr_ptr,
                             buf,
                             PPP_OUI_LEN,
                             DSM_DS_SMALL_ITEM_POOL ) != PPP_OUI_LEN )
    {
      dsm_free_packet( pkt_ptr_ptr );
      return -1;
    }
  }

  return 0;
} /* vsncp_3gpp2i_add_oui() */


/*===========================================================================
INLINE FUNCTION NCPI_REPORT_EVENT_TO_DIAG

DESCRIPTION
  This function posts an NCP event to Diag.

PARAMETERS
  (IN)ncp_handle_ptr : Pointer to the NCP control block
  (IN)ppp_event_state : Event state to be reported

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
#define NCPI_REPORT_EVENT_TO_DIAG ncpi_report_event_to_diag
INLINE void ncpi_report_event_to_diag
(
  ncpi_cb_type *ncp_handle_ptr,
  ppp_event_state_type event_state
)
{
  ppp_event_vsncp_payload_type ppp_event_vsncp;

  if( NULL == ncp_handle_ptr )
  {
    LOG_MSG_ERROR_0("Invalid parameter to ncpi_report_event_to_diag");
    return;
  }
  switch( ncp_handle_ptr->ncp )
  {
    case PPP_NCP_VSNCP_3GPP2:
      ppp_event_vsncp.pdn_id = ncp_handle_ptr->cb_u.vsncp_3gpp2.
        config.option_vals.pdn_id;
      ppp_event_vsncp.pdn_type = ncp_handle_ptr->cb_u.vsncp_3gpp2.
        config.option_vals.req_addr_type;
      ppp_event_vsncp.ppp_event_device = ncp_handle_ptr->device;
      ppp_event_vsncp.ppp_event_state  = event_state;
      event_report_payload( EVENT_VSNCP,
                            sizeof(ppp_event_vsncp),
                            &ppp_event_vsncp );
      break;

    case PPP_NCP_IPCP:
    case PPP_NCP_IPV6CP:
    default:
      /* unexpected! */
      LOG_MSG_ERROR_1("Unsupported NCP 0x%x", ncp_handle_ptr->ncp);
      ASSERT(0);
      break;
  } /* switch( ncp ) */
}

/*===========================================================================
INLINE FUNCTION VSNCP_3GPP2_CALL_ACTIVITY_CBACK()

DESCRIPTION
  This function is used to call the PDN activity callback (should be called
  whenever data activity occurs for a PDN (i.e. a VSNCP or VSNP packet is
  seen)

PARAMETERS
  (IN) fsm_ptr    : Pointer to the FSM structure

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define VSNCP_3GPP2_CALL_ACTIVITY_CBACK vsncp_3gpp2_call_activity_cback
INLINE void vsncp_3gpp2_call_activity_cback
(
  ppp_fsm_type   * fsm_ptr
)
{
  vsncp_3gpp2i_type * vsncp_cb_ptr;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*------------------------------------------------------------------------
    Validate arguments. The fsm_ptr must be valid.
  ------------------------------------------------------------------------*/
  vsncp_cb_ptr = &(((ncpi_cb_type*)fsm_ptr->pdv)->cb_u.vsncp_3gpp2);
  if( vsncp_cb_ptr->config.activity_cb.func != NULL )
  {
    vsncp_cb_ptr->config.activity_cb.func
      ( vsncp_cb_ptr->config.activity_cb.data );
  }
} /* vsncp_3gpp2_call_activity_cback() */
/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================

                     GENERAL FUNCTIONS (Apply to all NCPs)

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
);

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
);

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
);

/*===========================================================================
FUNCTION NCPI_RESYNC()

DESCRIPTION
  This function is used to process the PPP_NCP_RESYNC_CMD

PARAMETERS
  (IN) handle: Handle to the control block of the NCP

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ncpi_resync
(
  ppp_ncp_handle_type handle
);

/*===========================================================================
FUNCTION NCPI_ABORT()

DESCRIPTION
  This function is used to process the PPP_NCP_ABORT_CMD

PARAMETERS
  (IN) handle: Handle to the control block of the NCP

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ncpi_abort
(
  ppp_ncp_handle_type handle
);

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
);

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
);

/*===========================================================================

                          IPCP SPECFIC FUNCTIONS

===========================================================================*/
/*===========================================================================

                          IPV6CP SPECFIC FUNCTIONS

===========================================================================*/
/*===========================================================================

                         VSNCP-3GPP2 SPECFIC FUNCTIONS

===========================================================================*/
/*===========================================================================
FUNCTION VSNCP_3GPP2_SET_CONSTANT()

DESCRIPTION
  This function is used to cache the NV settings for general VSNCP params
  (retries etc.) so they don't need to be read from NV every time an
  NCP needs to be set up. The global structure vsncp_3gpp2_nv_info is used
  to hold this information.

PARAMETERS
  (IN) constant: The VSNCP NV parameter to be updated
  (IN) value: Value to be written to the constant

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void vsncp_3gpp2_set_constant
(
  ppp_init_consts_enum_type constant,
  uint32                    value
);

/*===========================================================================
FUNCTION VSNCP_3GPP2_GET_CONSTANT()

DESCRIPTION
  This funtion is used to return the general VSNCP related constant values,
  like number of retries, timeout values, etc.

PARAMETERS
  (IN) constant: The VSNCP NV parameter to be updated
  (out) value: Value to be written to the constant

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void vsncp_3gpp2_get_constant
(
  ppp_init_consts_enum_type   constant,
  uint32                    * value
);

#ifdef FEATURE_STA_PPP
/*===========================================================================
FUNCTION VSNCP_3GPP2_GET_FSM_CONSTANT()

DESCRIPTION
  This funtion is used to return the VSNCP instance related fsm constant
  values, like fsm state, number of retries, timeout values, etc.

PARAMETERS
  (IN) device: PPP device on which the VSNCP connection is established.
  (IN) constant: The VSNCP NV parameter whose value is to be returned.
  (IN) pdn id: PDN ID associated with the VSNCP instance.
  (OUT) value: Storage for the constant.

RETURN VALUE
   0: success
  -1: Failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int vsncp_3gpp2_get_fsm_constant
(
  ppp_dev_enum_type           device,
  ppp_fsm_type_enum_type      fsm_enum,
  uint8                       pdn_id
);
#endif /*FEATURE_STA_PPP*/

/*===========================================================================
FUNCTION VSNCP_3GPP2_POWERUP_INIT()

DESCRIPTION
  This function is used to do power-up initialization for the 3GPP2-VSNCP
  module.

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void vsncp_3gpp2_powerup_init( void );

/*===========================================================================
FUNCTION VSNCP_3GPP2_ALLOC()

DESCRIPTION
  This function is used to create a 3GPP2-VSNCP instance and return a
  handle to it.

PARAMETERS
  (IN) device: PPP device
  (IN) vsncp_config_ptr: Pointer to the 3gpp2 vsncp configuration

RETURN VALUE
  ppp_ncp_handle_type: Handle to the allocated NCP instance on success
  NULL on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
ppp_ncp_handle_type vsncp_3gpp2_alloc
(
  ppp_dev_enum_type             device,
  ppp_vsncp_3gpp2_config_type * vsncp_config_ptr
);

/*===========================================================================
FUNCTION VSNCP_3GPP2_INIT()

DESCRIPTION
  This function is used to initialize the 3GPP2-VSNCP control block

PARAMETERS
  (IN) ncp_cb_ptr

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int vsncp_3gpp2_init
(
  ncpi_cb_type * ncp_cb_ptr
);

/*===========================================================================
FUNCTION VSNCP_3GPP2_START()

DESCRIPTION
  This function is used to start the 3GPP2-VSNCP instance.

PARAMETERS
  (IN) ncp_cb_ptr: Pointer to the NCP control block

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void vsncp_3gpp2_start
(
  ncpi_cb_type * ncp_cb_ptr
);

/*===========================================================================
FUNCTION VSNCP_3GPP2_PROT_REJ

DESCRIPTION
  This function is used to handle an incoming LCP protocol-reject for
  VSNCP protocol. pkt_ptr points to the rejected VSNCP packet
    0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |     Code      |  Identifier   |            Length             |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                    OUI                        |    Data ...
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

PARAMETERS
  (IN) device: PPP device on which the packet was received
  (IN) pkt_ptr: Pointer to the DSM item containing the rejected packet

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void vsncp_3gpp2_prot_rej
(
  ppp_dev_enum_type   device,
  dsm_item_type     * pkt_ptr
);

/*===========================================================================
FUNCTION VSNCP_3GPP2_RECEIVE()

DESCRIPTION
  This function is used to process a VSNCP packet
    0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |     Code      |  Identifier   |            Length             |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                    OUI                        |    Data ...
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

PARAMETERS
  (IN) device: PPP device on which the packet was received
  (IN) pkt_ptr: Pointer to the DSM item containing the rejected packet

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void vsncp_3gpp2_receive
(
  ppp_dev_enum_type   device,
  dsm_item_type     * pkt_ptr
);

/*===========================================================================
FUNCTION VSNCP_3GPP2_REQUEST()

DESCRIPTION
  This function processes the VSNCP C-Req packet from the HSGW.
  If all the options contained in the C-Req are acceptable (either expected
  or unrecognized), this should lead to sending a C-Ack back to the HSGW.
  If the C-Req contains some unacceptable options (e.g. an empty IP address,
  the C-Req will be dropped and VSNCP termination initiated.)

PARAMETERS
  (IN) fsm_ptr        : Pointer to the FSM structure
  (IN) config_hdr_ptr : Pointer to the config header for the C-Req
  (IN) pkt_ptr        : Pointer to the DSM item - data_ptr points to the
                        first option within the C-Req

DEPENDENCIES
  None

RETURN VALUE
  0: Success (a C-Ack message was sent)
 -1: Failure (an error occured, no C-Ack message was sent)

SIDE EFFECTS
  None
===========================================================================*/
int vsncp_3gpp2_request
(
  ppp_fsm_type      * fsm_ptr,
  struct config_hdr * config_hdr_ptr,
  dsm_item_type     * pkt_ptr
);

/*===========================================================================
FUNCTION VSNCP_3GPP2_ACK()

DESCRIPTION
  This function processes the VSNCP C-Ack packet from the HSGW.
  If all the options contained in the C-Ack are acceptable (either expected
  or unrecognized), this should lead to updating the local working values
  for the options (recall that a VSNCP C-Ack contains assigned values for the
  options previously requested via the C-Req.

PARAMETERS
  (IN) fsm_ptr        : Pointer to the FSM structure
  (IN) config_hdr_ptr : Pointer to the config header for the C-Ack
  (IN) pkt_ptr        : Pointer to the DSM item - data_ptr points to the
                        first option within the C-Ack

DEPENDENCIES
  None

RETURN VALUE
  0: Success (the C-Ack was succesfully processed)
 -1: Failure (an error occured during C-Ack processing)

SIDE EFFECTS
  None
===========================================================================*/
int vsncp_3gpp2_ack
(
  ppp_fsm_type      *fsm_ptr,
  struct config_hdr * config_hdr_ptr,
  dsm_item_type     * pkt_ptr
);

/*===========================================================================
FUNCTION VSNCP_3GPP2_TERM_REQ()

DESCRIPTION
  This function processes the VSNCP Term-Req packet from the HSGW. Term-Req
  may contain options other than PDN-ID. Currently supported options in
  Term-Req are:
  VSNCP_3GPP2I_RECONNECT_IND - to indicate whether or not the UE is allowed
                               to reconnect to the APN associated with the
                               PDN connection being terminated.
PARAMETERS
  (IN) fsm_ptr        : Pointer to the FSM structure
  (IN) config_hdr_ptr : Pointer to the config header for the C-Ack
  (IN) pkt_ptr        : Pointer to the DSM item - data_ptr points to the
                        first option within the C-Ack

DEPENDENCIES
  None

RETURN VALUE
  0: Success (the Term-Req was succesfully processed)
 -1: Failure (an error occured during Term-Req processing)

SIDE EFFECTS
  None
===========================================================================*/
int vsncp_3gpp2_term_req
(
  ppp_fsm_type      * fsm_ptr,
  struct config_hdr * config_hdr_ptr,
  dsm_item_type     **pkt_ptr_ptr
);

/*===========================================================================
FUNCTION VSNCP_3GPP2_REJECT

DESCRIPTION
  This function processes the VSNCP Config-Rej packet from the HSGW.

PARAMETERS
  (IN) fsm_ptr        : Pointer to the FSM structure
  (IN) config_hdr_ptr : Pointer to the config header for the Config-Rej
  (IN) pkt_ptr        : Pointer to the DSM item - data_ptr points to the
                        first option within the Config-Rej

DEPENDENCIES
  None

RETURN VALUE
  0: Success (the Config-Rej was succesfully processed)
 -1: Failure (an error occured during Config-Rej processing)

SIDE EFFECTS
  None
===========================================================================*/
int vsncp_3gpp2_reject
(
  ppp_fsm_type      *fsm_ptr,
  struct config_hdr * config_hdr_ptr,
  dsm_item_type     * pkt_ptr
);

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
);

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
);


#endif /* FEATURE_DATA_PS_EHRPD */
#endif /* PS_PPP_NCPI_H */

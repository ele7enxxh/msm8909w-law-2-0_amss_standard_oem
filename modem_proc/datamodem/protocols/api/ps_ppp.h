#ifndef PS_PPP_H
#define PS_PPP_H
/*===========================================================================
                               P S _ P P P . H

DESCRIPTION
  Header file for the PPP protocol suite Interfacing functions.

Copyright (c) 2002-2011 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_ppp.h_v   1.9   15 Nov 2002 23:35:24   jeffd  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/api/ps_ppp.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
12/22/11    scb    Added support for IPv6 over PPP-DUN calls
11/28/11    jz     ppp control packet on dos(FEATURE_DATA_SEND_PPP_CTL_ON_DOS) 
10/17/11    ssh    eHRPD Always On support
09/14/11    ash    Cancel PPP setup timer after AUTH success
06/06/11    ash    Added support for PPP Partial Context
03/23/11    op     Data SU API cleanup
08/31/09    mga    Merged from eHRPD branch
05/07/09    pp     CMI Phase-4: SU Level API Effort.
===========================================================================*/


/*===========================================================================

                                INCLUDE FILES

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "dsm.h"
#include "ps_ppp_defs.h"
#include "ps_ppp_sig.h"
#include "ps_svc.h"
#include "ps_ppp_events.h"
#include "ps_iface_defs.h"
#include "msg.h"
#include "ps_tx_meta_info.h"
#include "ps_ppp_ncp.h"
#include "ps_ppp_fsm.h"

/*===========================================================================

                          MACRO DEFINITIONS

===========================================================================*/

/*===========================================================================

                              DATA DECLARATIONS

===========================================================================*/
#define JCDMA_MAX_PPP_SETUP_TIME 40 /* JCDMA 40s guard time as per
                                       80-V5266-1 rev B sec 4.8            */
#define MAX_PPP_SETUP_TIME 30       /*  30s PPP guard time */

/*---------------------------------------------------------------------------
  Flag set in Tx pkt meta info to indicate this is an LCP Echo-Request
---------------------------------------------------------------------------*/
#define MSG_LCP_ECHO_REQ  0x10000000   

/*---------------------------------------------------------------------------
  struct ppp_dev_opts_type
  Structure for setting/getting the negotiated options on a particular
  interface.  This structure should be made a part of the ppp_cb - and
  remove all of the duplicated fields.

  Internal types are declared here ahead of the main structure.
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
  All of the LCP information
---------------------------------------------------------------------------*/
typedef struct ppp_lcp_info_s
{
  lcp_value_type  want_vals[FSM_SIDE_MAX];
  lcp_value_type  work_vals[FSM_SIDE_MAX];
  negotiate_t     want_mask[FSM_SIDE_MAX];
  negotiate_t     will_mask[FSM_SIDE_MAX];
  cp_mode_type    mode;
} ppp_lcp_info_s_type;

/*---------------------------------------------------------------------------
  All of the IPCP information
---------------------------------------------------------------------------*/
typedef struct ppp_ipcp_info_s
{
  ipcp_value_type want_vals[FSM_SIDE_MAX];
  ipcp_value_type work_vals[FSM_SIDE_MAX];
  negotiate_t     want_mask[FSM_SIDE_MAX];
  negotiate_t     will_mask[FSM_SIDE_MAX];
  cp_mode_type    mode;
  ps_iface_type  *rx_iface_ptr;  /* interface to associate with            */
} ppp_ipcp_info_s_type;

/*---------------------------------------------------------------------------
  All of the IPV6CP information
---------------------------------------------------------------------------*/
typedef struct ppp_ipv6cp_info_s
{
  ipv6cp_value_type want_vals[FSM_SIDE_MAX];
  ipv6cp_value_type work_vals[FSM_SIDE_MAX];
  negotiate_t       want_mask[FSM_SIDE_MAX];
  negotiate_t       will_mask[FSM_SIDE_MAX];
  cp_mode_type      mode;
  ps_iface_type    *rx_iface_ptr; /* interface to associate with           */
} ppp_ipv6cp_info_s_type;


/*---------------------------------------------------------------------------
  Event allocation information
---------------------------------------------------------------------------*/
typedef struct ppp_event_alloc_info_s
{  
  ppp_event_cback_f_ptr_type   lcp_cb_f;
  void                       * lcp_user_data;  
  ppp_event_cback_f_ptr_type   auth_cb_f;
  void                       * auth_user_data;  
  ppp_ncp_e_type               ncp;
  ppp_event_cback_f_ptr_type   ncp_cb_f;
  void                       * ncp_user_data;    /* vsncp handle for vsncp */
  ppp_ncp_handle_type          ncp_handle_ptr;
} ppp_event_alloc_info_type;


typedef struct
{
  ppp_lcp_info_s_type    lcp_info;    /* LCP information                   */
  ppp_ipcp_info_s_type   ipcp_info;   /* IPCP information                  */
  ppp_ipv6cp_info_s_type ipv6cp_info; /* IPV6CP information                */
  ppp_auth_info_type     auth_info;   /* Auth info                         */

  /*-------------------------------------------------------------------------
    PPP RX and TX information
  -------------------------------------------------------------------------*/
  tx_f_ptr_type     tx_f_ptr;       /* function user to tx PPP packets     */
  rx_f_ptr_type     rx_f_ptr;       /* Function to receive next data item  */
  void             *tx_user_data;   /* User data passed to tx_f_ptr        */
  void             *rx_user_data;   /* User data passed to rx_f_ptr        */

  ppp_sig_type      rx_signal;      /* the RX signal to use                */
  ppp_sig_handler_type rx_sig_f_ptr;/* PPP data received signal handler    */
  ppp_dev_enum_type bridge_dev;     /* device to bridge to (netmodel)      */
  boolean           serves_addrs;   /* does this serve IP addresses?       */
  boolean           auths_peer;     /* does this authenticate peer?        */

  /*-------------------------------------------------------------------------
    HDLC handling, including the unframing mode, and pointers to functions
    used to forward packets in various states of HDLC unframing.  The
    function that forwards unframed packets could be used by the network
    model callbacks - rather than forwarding them directly.
  -------------------------------------------------------------------------*/
  hdlc_mode_enum_type hdlc_mode;        /* HDLC framing mode               */

  /*-------------------------------------------------------------------------
    total setup time - seconds
  -------------------------------------------------------------------------*/
  uint8 setup_timeout;

  /*-------------------------------------------------------------------------
    Enable wakeup in passive mode on receiving any PPP packet.
  -------------------------------------------------------------------------*/
  boolean passive_wakeup_on_data;

  /*-------------------------------------------------------------------------
    PPP failure graceful close flag set by the MH which indicates whether a
    TERM REQ is to be sent in case PPP fails to establish or not.
  -------------------------------------------------------------------------*/
  boolean ppp_failure_graceful_close;

  /*-------------------------------------------------------------------------
    The set stablilize accm delay flag which indicates whether a DISCARD REQ
    is to be sent or not.
  -------------------------------------------------------------------------*/
  boolean set_stabilize_accm_delay;

  /*-------------------------------------------------------------------------
    Flag to indicate whether the DNS options (primary and secondary) 
    must be negotiated on a resync.
  -------------------------------------------------------------------------*/
  boolean resync_dns_opts;

  /*-------------------------------------------------------------------------
    This indicates the phase in which the setup timer be stopped.
  -------------------------------------------------------------------------*/
  ppp_link_phase_type    setup_final_phase;

  /*-------------------------------------------------------------------------
    Flag to indicate if no authentication is allowed.
  -------------------------------------------------------------------------*/
  boolean allow_no_auth;
#ifdef FEATURE_DATA_SEND_PPP_CTL_ON_DOS
  /*-------------------------------------------------------------------------
    Flag to indicate if ppp control packet is sent over DOS
  -------------------------------------------------------------------------*/
  boolean dos_is_enabled;
#endif /*FEATURE_DATA_SEND_PPP_CTL_ON_DOS*/
} ppp_dev_opts_type;



/*---------------------------------------------------------------------------
TYPEDEF PPP_DEV_CONFIG_TYPE

DESCRIPTION
  This structure is used to pass configuration for the PPP device.
  lcp_mode: Whether LCP should be brought up in active or passive mode.
            Other values (invalid, discard, disabled) are not valid for this.

  auth_role: Does this device do authentication ? If yes, is it the 
             authenticatee or authenticator ?
  auth_proto_mask: Which authentication protocols are supported ?
  auth_info: Authentication credentials

  tx_f_ptr: Function used to transmit PPP packets
  tx_user_data: User data passed to tx_f_ptr
  rx_signal: the RX signal to us
  rx_sig_f_ptr: PPP data received signal handler

  setup_timeout: Time to wait until first NCP bringup (else giveup)
  
  hdlc_mode: HDLC processing mode (software/hardware etc.)
  
  bridge_dev: Bridge Device
---------------------------------------------------------------------------*/
typedef struct ppp_dev_config_s
{
  cp_mode_type           lcp_mode; 
  hdlc_mode_enum_type    hdlc_mode; 
  ppp_auth_role_e_type   auth_role; 
  ppp_auth_proto_e_type  auth_proto_mask; 
  ppp_auth_info_type     auth_info;   
  
  uint8                  setup_timeout;

  tx_f_ptr_type          tx_f_ptr;       
  void                 * tx_user_data;   
  ppp_sig_type           rx_signal;      
  ppp_sig_handler_type   rx_sig_f_ptr;
    
  ppp_dev_enum_type      bridge_dev;

  /*-------------------------------------------------------------------------
    This indicates the phase in which the setup timer be stopped.
  -------------------------------------------------------------------------*/
  ppp_link_phase_type    setup_final_phase;

  /*-------------------------------------------------------------------------
    Flag to indicate if no authentication is allowed.
  -------------------------------------------------------------------------*/
  boolean allow_no_auth;
#ifdef FEATURE_DATA_SEND_PPP_CTL_ON_DOS
  /*-------------------------------------------------------------------------
    Flag to indicate if ppp control packet is sent over DOS
  -------------------------------------------------------------------------*/
  boolean dos_is_enabled;
#endif /*FEATURE_DATA_SEND_PPP_CTL_ON_DOS*/
} ppp_dev_config_type;


/*---------------------------------------------------------------------------
TYPEDEF PPP_INIT_CONSTS_ENUM_TYPE

DESCRIPTION
  Enum defining a subset of PPP constants that can be configured by the 
  client.
---------------------------------------------------------------------------*/
typedef enum
{
  PPP_MIN_INIT_CONSTS        = 0,   /* the lower bound value         */
  LCP_ACK_TIMEOUT            = PPP_MIN_INIT_CONSTS,/* LCP ACK timeout*/
  LCP_REQUEST_TRY                = 1,/* LCP Request Retry count      */
  IPCP_ACK_TIMEOUT           = 2,/* IPCP acknowledgement timeout     */
  IPCP_REQUEST_TRY               = 3,/* IPCP Request Retry count     */
  AUTH_TIMEOUT               = 4,/* LCP acknowledgement timeout      */
  AUTH_RETRY                 = 5,/* LCP Request Retry count          */
  IPV6CP_ACK_TIMEOUT         = 6,/* IPV6CP acknowledgement timeout   */
  IPV6CP_REQUEST_TRY             = 7,/* IPV6CP Request Retry count   */
  DEFAULT_LCP_ACK_TIMEOUT    = 8,/* Default LCP ACK timeout          */
  DEFAULT_LCP_REQUEST_TRY    = 9,/* Default LCP Request Retry count  */
  DEFAULT_IPCP_ACK_TIMEOUT   = 10,/* Default IPCP acknowledgement timeout */
  DEFAULT_IPCP_REQUEST_TRY   = 11,/* Default IPCP Request Retry count*/
  DEFAULT_AUTH_TIMEOUT       = 12, /* Default LCP acknowledgement timeout */
  DEFAULT_AUTH_RETRY         = 13,/* Default LCP Request Retry count */
  DEFAULT_IPV6CP_ACK_TIMEOUT = 14,/* Def. IPV6CP acknowledgement timeout  */
  DEFAULT_IPV6CP_REQUEST_TRY = 15,/* Def. IPV6CP Request Retry count */

  VSNCP_3GPP2_REQ_TRY           = 16,  /* vsncp-3gpp2, same for all dev    */
  VSNCP_3GPP2_TERM_TRY          = 17,  /* vsncp-3gpp2, same for all dev    */
  VSNCP_3GPP2_VSNCP_ACK_TIMEOUT = 18,  /* vsncp-3gpp2, same for all dev    */
  VSNCP_3GPP2_TERM_TIMEOUT      = 19,  /* vsncp-3gpp2, same for all dev    */
  
  PPP_MAX_INIT_CONSTS            /* max value - array sizing         */
} ppp_init_consts_enum_type;

#define PPP_HDR_LEN  4  /* Max bytes for PPP/HDLC envelope header          */
#define PPP_PADDING  8  /* Max bytes for PPP/HDLC padding                  */

/*===========================================================================
FUNCTION CALL_EVENT_CALLBACK_F_TYPE()

DESCRIPTION
  Function pointer for the function handler in the FSM to handle specific
  events that have been triggered because of FSM activity.

PARAMETERS
  device:           PPP device pkt arrived on
  protocol:         Protocol assc. with the packet
  event:            Event associated with cback function
  prev_state:       Previous state of the control block event
  state:            Current state of the control block event

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  Probably.
===========================================================================*/
typedef void (*call_event_callback_f_type)
(
  struct fsm_s         *fsm_ptr,          /* fsm_ptr                     */  
  uint16                protocol,         /* protocol of packet      */
  ppp_event_e_type      event,            /* event associated with cback */
  ppp_event_state_type  prev_state,       /* previous state of cb */
  ppp_event_state_type  state             /* current state of cb */
);


/*===========================================================================
FUNCTION OUTGOING_PKT_HDLR_F_TYPE()

DESCRIPTION
  Function pointer for the function handler in the FSM to send outgoing
  packets from the fsm_send function to the respective outgoing entry 
  points i.e pppi_outgoing in this case.

PARAMETERS
  device:           PPP device pkt arrived on
  protocol:         Protocol assc. with the packet
  incoming_pkt_ptr: Address of ptr to a dsm_item_type
  meta_info_ptr:    Pointer to meta_info if present

RETURN VALUE
  0: Indicates success
 -1: Indicates failure
  
DEPENDENCIES
  None

SIDE EFFECTS
  Probably.
===========================================================================*/
typedef int (*outgoing_pkt_hdlr_f_type)
(
  ppp_dev_enum_type     device,           /* device to be sent on    */
  uint16                protocol,         /* protocol of packet      */
  dsm_item_type       **incoming_pkt_ptr, /* ptr to PPP packet chain */
  ps_tx_meta_info_type    *meta_info_ptr     /* ptr to meta info        */
);

/*===========================================================================
FUNCTION VENDOR_EXT_HDLR_F_TYPE()

DESCRIPTION
  Function pointer for the function handler in the FSM to process vendor
  specific packets.

PARAMETERS
  device:           PPP device pkt arrived on
  protocol:         Protocol assc. with the packet
  hdr:              Pointer to config hdr
  pkt_ptr:          Pointer to a dsm_item_type

RETURN VALUE
  None
  
DEPENDENCIES
  None

SIDE EFFECTS
  Probably.
===========================================================================*/

typedef void (*vendor_ext_hdlr_f_type)
(
  ppp_dev_enum_type     device,       /* (in) device on which pkt arrived  */
  uint16                protocol,     /* (in) PPP protocol field of pkt    */
  ppp_config_hdr_type * hdr,          /* (in) ptr to config hdr            */
  dsm_item_type       * pkt_ptr       /* (in) ptr to rcvd packet           */
);

/*---------------------------------------------------------------------------
  Control Block Constants needed by State Machine
---------------------------------------------------------------------------*/

struct cb_constant_s
{
  call_event_callback_f_type call_event_callback_fcn;
  outgoing_pkt_hdlr_f_type   outgoing_pkt_hdlr_fcn;
  vendor_ext_hdlr_f_type     vendor_ext_hdlr_fcn;
};

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION PPP_GET_DEFAULT_OPTS()

DESCRIPTION
  This function will populate the options data structure with the default
  values for a PPP session.  The intention is to allow users to get a default
  configuration and modify only the fields that they wish and the fields that
  are set to invalid values (the owner of the configuration structure must
  set these values by hand).

  The following fields are set to invalid values:
    lcp_info.mode   -> PPP_CP_INVALID_MODE
    ipcp_info.mode  -> PPP_CP_INVALID_MODE
    rx_f_ptr        -> NULL
    rx_iface_ptr    -> NULL
    hdlc_mode       -> HDLC_INVALID_MODE
    bridge_dev      -> PPP_INVALID_DEV
    serves_addrs    -> FALSE
    auths_peer      -> FALSE
    rx_signal       -> PPP_INVALID_SIGNAL
    rx_sig_f_ptr    -> pppi_get_data_cback;

  NOTE: authentication is not requested in LCP, the desired local IPCP values
  for compression are used.

PARAMETERS
  config_ptr: configuration structure to fill in with the default information

RETURN VALUE
  The passed structure of type ppp_dev_opts_type is initialized with default
  options.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ppp_get_default_opts
(
  ppp_dev_opts_type *opts_ptr
);

/*===========================================================================
FUNCTION PPP_GET_PROTOCOL_OPTS()

DESCRIPTION
  This function will copy the framing options for a particular PPP
  device into the option structure provided by the caller.

PARAMETERS
  device: PPP device to copy
  config_ptr: pointer to configuration structure in which to put information
  swap: should devices local information be put in the configuration
        strucuture's remote slot and visa versa?

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ppp_get_protocol_opts
(
  ppp_dev_enum_type  device,
  ppp_dev_opts_type *config_ptr
);

/*===========================================================================
FUNCTION PPP_GET_BRIDGE_DEV_PROT_OPTS()

DESCRIPTION
  This function will copy the framing options from a particular PPP
  device's bridged device into the specified config_ptr, swapping the
  local and remote options and values.

  This provides a means to obtain the options required to start ppp on the
  opposite side of an interface such that compatible ppp options are
  negotiated on both devices.

PARAMETERS
  device:     PPP device whose bridge device to copy options from
  config_ptr: pointer to configuration structure in which to put information

RETURN VALUE
  -1 in case of error
   0 in case of success
   The passed config_ptr is filled with the configuration information

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ppp_get_bridge_dev_prot_opts
(
  ppp_dev_enum_type  device,
  ppp_dev_opts_type *config_ptr
);

/*===========================================================================
FUNCTION PPP_GET_DEV_SETUP()

DESCRIPTION
  This function will copy the device internal options for a particular PPP
  device into the provided option structure.

PARAMETERS
  device: PPP device to copy settings from
  config_ptr: pointer to configuration structure in which to put information

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ppp_get_dev_setup
(
  ppp_dev_enum_type  device,
  ppp_dev_opts_type *opts_ptr
);

/*===========================================================================
FUNCTION PPP_GET_AUTH_INFO()

DESCRIPTION
  This function will return a pointer to a data structure containing all of
  th authentication information.  The macro PPP_AUTH_WAS_CHAP() can be used
  to determine if CHAP or PAP was done.  The return value will be NULL if the
  device is out of bounds OR no authentication was performed.

  NOTE: this informaion should be treated as READ-ONLY changing values of the
        pointer information has unspecified effects.

PARAMETERS
  device: the PPP device from which to return the auth information.
  auth_info_ptr: pointer to auth_info_type where the auth_info will be copied.
  auth_id_ptr:   pointer to uint8 where the auth ID will be copied - if
                 passed in as NULL, will be ingored.

RETURN VALUE
  0: on success
 -1: on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ppp_get_auth_info
(
  ppp_dev_enum_type   device,
  ppp_auth_info_type *auth_info_ptr,
  uint8              *auth_id_ptr
);

/*===========================================================================
MACRO PPP_AUTH_WAS_CHAP()

DESCRIPTION
  This function will indicate if CHAP was used.  It will return
  FALSE if no authentication was done, and can then be assumed that if CHAP
  was not done, PAP was.

PARAMETERS
  device: PPP device to check for use of CHAP authentication

RETURN VALUE
  TRUE  if chap was done
  FALSE otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PPP_AUTH_WAS_CHAP(device)                                \
  (ppp_cb_array[device].auth.mode == PPP_DO_CHAP)

/*===========================================================================
FUNCTION PPP_UPDATE_PROTO_CONFIG()

DESCRIPTION
  This function reconfigures protocol options for the specified PPP device.
  This prepares PPP device for subsequent resyncs after it has already come
  up or allows reconfiguration while PPP is in the middle of coming up.

PARAMETERS
  device:        PPP device to configure
  config_ptr:    configuration to use

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  Modifies protocol options for the given PPP device
===========================================================================*/
void ppp_update_proto_config
(
  ppp_dev_enum_type       device,
  ppp_protocol_e_type     protocol,
  ppp_dev_opts_type       *config_ptr
);

/*===========================================================================
FUNCTION PPP_SET_V4_SERVER_OPTS()

DESCRIPTION
  This function will change the configuration passed in to serve IP
  addresses.  This includes setting the serves_addrs field, and setting the
  appropriate IPCP bits.

PARAMETERS
  config_ptr:    configuration to convert
  local_addr:    the local IP address to use
  remote_addr:   the address to assign
  primary_dns:   the primary DNS address to advertise
  secondary_dns: the primary DNS address to advertise

RETURN VALUE
  The passed information is used to setup the passed config_ptr.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ppp_set_v4_server_opts
(
  ppp_dev_opts_type *config_ptr,
  uint32             local_addr,
  uint32             remote_addr,
  uint32             primary_dns,
  uint32             secondary_dns,
  /* for WINS support */
  uint32             primary_nbns,
  uint32             secondary_nbns
);

/*===========================================================================
FUNCTION PPP_SET_V6_SERVER_OPTS()

DESCRIPTION
  This function will change the configuration passed in to serve IP
  addresses.  This includes setting the addresses, serves_addrs, and the
  appropriate IPV6CP bits. A local IID (serves as gateway IID) and a 
  remote IID must be passed in to be assigned to the peer.

  Compression is currently not negotiated and is turned off.

PARAMETERS
  config_ptr:   configuration to convert
  local_iid:    gateway IID
  remote_iid:   IID to be assigned to the peer

RETURN VALUE
  The passed information is used to setup the passed config_ptr.

DEPENDENCIES
  NOTE: The local_iid and remote_iid must be in network byte order.
SIDE EFFECTS
  None
===========================================================================*/
void ppp_set_v6_server_opts
(
  ppp_dev_opts_type *config_ptr,
  uint64            local_iid ,
  uint64            remote_iid 
);

/*===========================================================================
FUNCTION PPP_SET_AUTH_SERVER_OPTS()

DESCRIPTION
  This function will setup the configuration to enable the receiving PPP
  instance to authenticate the peer.

PARAMETERS
  config_ptr: pointer to the configuration being modified
  challenge name: string indicating the name that should be sent as part of
      the chap challenge.  This can be NULL or the empty string if no name is
      needed (PAP), or wants to be sent.

RETURN VALUE
  -1 on error
  the number of bytes of challenge_name copied
      NOTE: this may be less than strlen(challenge_name) if that is greater
      than PPP_CHAP_CHAL_NAME_LEN.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ppp_set_auth_server_opts
(
  ppp_dev_opts_type *config_ptr,
  char              *challenge_name
);


/*===========================================================================
FUNCTION PPP_EVENT_ALLOC_EX()

DESCRIPTION
  This function will allocate a PPP event information structure and
  register callback functions for supported PPP protocols.

PARAMETERS
  device: PPP device
  info_ptr: Ptr to event information structure

RETURN VALUE
  Event handle

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void * ppp_event_alloc_ex
(
  ppp_dev_enum_type           device,
  ppp_event_alloc_info_type * info_ptr
);


/*===========================================================================
FUNCTION PPP_EVENT_ALLOC()

DESCRIPTION
  This function will allocate a PPP event information structure and
  register callback functions for supported PPP protocols.

PARAMETERS
  device:           PPP instance to register for
  lcp_cb_f:         LCP callback function to call
  lcp_user_data:    user data to be returned with the LCP callback function
  auth_cb_f:        auth callback function to call
  auth_user_data:   user data to be returned with the auth callback function
  ipcp_cb_f:        IPCP callback function to call
  ipcp_user_data:   user data to be returned with the IPCP callback
  ipv6cp_cb_f:      IPV6CP callback function to call
  ipv6cp_user_data: user data to be returned with the IPV6CP callback

RETURN VALUE
  NULL on error
  handle to the PPP event information structure on success

DEPENDENCIES
  None

SIDE EFFECTS
  Allocates a PPP event information structure and places it on the event
  queue.  Initializes event masks for all protocols to 0 and registers
  callbacks for the PPP protocols
===========================================================================*/
void *ppp_event_alloc
(
  ppp_dev_enum_type           device,
  ppp_event_cback_f_ptr_type  lcp_cb_f,
  void                       *lcp_user_data,
  ppp_event_cback_f_ptr_type  auth_cb_f,
  void                       *auth_user_data,
  ppp_event_cback_f_ptr_type  ipcp_cb_f,
  void                       *ipcp_user_data,
  ppp_event_cback_f_ptr_type  ipv6cp_cb_f,
  void                       *ipv6cp_user_data
);

/*===========================================================================
FUNCTION PPP_EVENT_REGISTER()

DESCRIPTION
  This function will add to an event mask for a PPP protocol.

PARAMETERS
  handle      PPP event structure to add this event mask to
  protocol    Protocol to add the event mask to
  event_mask  event mask to add

RETURN VALUE
  -1 on error
   0 on success

DEPENDENCIES
  An event is NOT generated if the event is TRUE at the time of calling
  of this function.
  TODO: This needs to be discussed.

SIDE EFFECTS
  Adds the passed event mask to the already registered event mask
===========================================================================*/
int32 ppp_event_register
(
  void                  *handle,
  ppp_protocol_e_type    protocol,
  ppp_event_mask_e_type  event_mask
);

/*===========================================================================
FUNCTION PPP_EVENT_DEREGISTER()

DESCRIPTION
  This function will remove an event mask for a PPP protocol.

PARAMETERS
  handle      PPP event structure to add this event mask to
  protocol    Protocol to add the event mask to
  event_mask  event mask to add

RETURN VALUE
 -1 on error
  0 on success

DEPENDENCIES
  None

SIDE EFFECTS
  Removes the passed event mask from the already registered event mask
===========================================================================*/
int32 ppp_event_deregister
(
  void                  *handle,
  ppp_protocol_e_type         protocol,
  ppp_event_mask_e_type  event_mask
);

/*===========================================================================
FUNCTION PPP_EVENT_DEALLOC()

DESCRIPTION
  This function will deallocate a PPP event information structure.

PARAMETERS
  device      PPP device to which this handle belongs
  handle      PPP event structure to deallocate

RETURN VALUE
  -1 on error
   0 on success

DEPENDENCIES
  None

SIDE EFFECTS
  Deallocates this PPP event structure
===========================================================================*/
int32 ppp_event_dealloc
(
  ppp_dev_enum_type   device,
  void              **handle
);

/*===========================================================================
FUNCTION PPP_DEVICE_CONFIGURE()
  
DESCRIPTION
  This function is used to configure the PPP device.
  

PARAMETERS
  device: PPP device to configure
  config_ptr: Pointer to device configuration

RETURN VALUE
  0: Success
 -1: Failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ppp_device_configure
(
  ppp_dev_enum_type     device, 
  ppp_dev_config_type * config_ptr
);

/*===========================================================================
FUNCTION PPP_DEVICE_UNCONFIGURE()
  
DESCRIPTION
  This function is used to unconfigure the device.
  
PARAMETERS
  (IN) device: PPP device to configure

RETURN VALUE
  0: Success
 -1: Failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ppp_device_unconfigure
(
  ppp_dev_enum_type     device
);

/*===========================================================================
FUNCTION PPP_DEVICE_RESET()
  
DESCRIPTION
  This cleans up the active PPP session and resets the device.
  
PARAMETERS
  (IN) device: PPP device to configure

RETURN VALUE
  0: Success
 -1: Failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ppp_device_reset
(
  ppp_dev_enum_type     device
);

/*===========================================================================
FUNCTION PPP_IS_DEVICE_CONFIGURED()
  
DESCRIPTION
  This function is used to check if the PPP device has been configured.
  
PARAMETERS
  (IN) device: PPP device 

RETURN VALUE
  TRUE:  If device has already been configured
  FALSE: If device has not been configured

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean ppp_is_device_configured
(
  ppp_dev_enum_type     device
);

/*===========================================================================
FUNCTION PPP_DEVICE_STOP()

DESCRIPTION
  This function is responsible for tearing down and cleaning up PPP.
  It does following operation in sequence:
  1. Gracefully teardown NCP
  2. Cleanup NCP control block
  3. Gracefully teardown LCP
  4. Cleanup PPP device

PARAMETERS
  device: the PPP device to be stopped

RETURN VALUE
  0: on success
 -1: if the command fails for any reason: such as the inability to get a
     command buffer, or a bqad device was passed in.

DEPENDENCIES
  None

SIDE EFFECTS
  the 'device' field in the configuration structure will be set to the value
  of the 'device' parameter passed in.
===========================================================================*/
int ppp_device_stop
(
  ppp_dev_enum_type  device
);

/*===========================================================================
FUNCTION PPP_TRIGGER_LSD()

DESCRIPTION
  This function is used to trigger PPP link status determination. If LCP is 
  already opened, it sends an LCP Echo-request packet. If no Echo-Reply is 
  received in response, the packet is resent per the supplied configuration. 
  The result is indicated via the supplied callback.

PARAMETERS
  device:      PPP device to perform link status determination on
  lsd_config_ptr: Pointer to the Link Status Determination configuration

RETURN VALUE
  0: on success
 -1: on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ppp_trigger_lsd
(
  ppp_dev_enum_type     device,
  ppp_lsd_config_type * lsd_config_ptr
);

/*===========================================================================
FUNCTION PPP_START()

DESCRIPTION
  This function starts the PPP state machine.  If the opts_ptr field is NULL
  it will get the default configuration for PPP use that.  It also registers
  a callback that is associated with the signal for PPP instance being
  started.

PARAMETERS
  device: the ppp instance to configure
  opts_ptr: pointer to a variable containing the optsion to configure with

RETURN VALUE
  -1: if the ppp instance cannot be configured
  session handle of 0 or greater on success.  This can be used to associate
  PPP events with this instance of PPP.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ppp_start
(
  ppp_dev_enum_type  device,        /* The interface (Rm or Um)            */
  ppp_dev_opts_type *config_ptr     /* the options to start PPP with       */
);

/*===========================================================================
FUNCTION PPP_STOP()

DESCRIPTION
  This function will gracefully terminate PPP.

PARAMETERS
  device: the ppp instance to configure

RETURN VALUE
  -1: if the ppp instance cannot be configured
   0: on success

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ppp_stop
(
  ppp_dev_enum_type   device,
  ppp_protocol_e_type protocol
);

/*===========================================================================
FUNCTION PPP_RESYNC()

DESCRIPTION
  This function will restart PPP - forcing it to renegotiate.  It REQUIRES
  that the LCP mode be active.  NOTE: though a configuration can be passed in
  it is NOT possible to change the RX signal to use.

PARAMETERS
  device: the PPP device to be resync'ed
  config_ptr: pointer to the configuration with which to resync PPP device
          'device'

RETURN VALUE
  0: on success
 -1: if the command fails for any reason: such as the inability to get a
     command buffer, or some configuration is bad.

DEPENDENCIES
  None

SIDE EFFECTS
  If configuration is passed in, it will be copied to the PPP device
  IMMEDIATELY (if the device is free).  This means that the PPP context is
  modified in the callers task context - it also means that the memory for
  configuration passed in is free as soon as this function returns.
===========================================================================*/
int ppp_resync
(
  ppp_dev_enum_type device,
  ppp_protocol_e_type  protocol,
  ppp_dev_opts_type *config_ptr

);

/*===========================================================================
FUNCTION PPP_ABORT()

DESCRIPTION
  This function is used by PPP clients to abort a particular PPP device.

PARAMETERS
  device: the PPP device to be aborted

RETURN VALUE
  0: on success
 -1: if the command fails for any reason: such as the inability to get a
     command buffer, or a bqad device was passed in.

DEPENDENCIES
  None

SIDE EFFECTS
  the 'device' field in the configuration structure will be set to the value
  of the 'device' parameter passed in.
===========================================================================*/
int ppp_abort
(
  ppp_dev_enum_type  device
);

/*===========================================================================
FUNCTION PPP_INJECT_PACKET()

DESCRIPTION
  This function processes a PPP packet as if the packet has been
  received from the underlying device. The processing is performed as
  soon as this function is called before other packets that are already
  queued in the rx queue of the device. Only exception is the packet that is
  currrently being processed in PS context. This function posts a cmd to PS
  so that packet processing always takes place in PS. This function allows
  replaying previously stored PPP packets.

PARAMETERS
  device                ppp device to pass pkt to
  protocol              protocol of packet
  ppp_pkt_ptr           Pointer to PPP pkt

RETURN VALUE
  0: on success
 -1: if an invalid or unconfigured PPP device specified

DEPENDENCIES
  This function should not be called while already processing a PPP
  packet. That means this function should not be called from pppi_incoming(),
  pppi_receive(), pppi_hdlc_sw/hw_frame/unframe() functions.
  It expects an unframed PPP packet, without FCS and ASYNC FLAG (7E)

SIDE EFFECTS
  Processes the specified PPP packet out of order
===========================================================================*/
int ppp_inject_packet
(
  ppp_dev_enum_type device,
  uint16            protocol,
  dsm_item_type     *ppp_pkt_ptr
);



/*===========================================================================
FUNCTION PPP_INPUT()

DESCRIPTION
   The packet that was received from the lower layer is passed to the HDLC
   deframer which takes care of giving the whole PPP packet back, and is then
   given to upper layers.

   PPP should be the current task when this function is executed.

PARAMETERS
  device    :  PPP device to pass the packet to
  hdlc inst :  instance of HDLC for which the received packet belongs to
  ppp_pkt_ptr: pointer to the PPP packet which need to get unframed.

RETURN VALUE
   0 : Successfully unframed the packet.
  -1 : Failed to unframe.

DEPENDENCIES
  This function gets executed in PPP task.

SIDE EFFECTS
  None

===========================================================================*/
int ppp_input
(
  ppp_dev_enum_type   device,               /* PPP device to pass pkt to   */
  void              * hdlc_inst,            /* Handle to HDLC instance     */
  dsm_item_type    ** item_ref_ptr          /* Pointer to PPP pkt          */
);

/*===========================================================================
FUNCTION PPP_OUTPUT()

DESCRIPTION
  PPP link transmit function

PARAMETERS
  device: PPP device.
  item_ref_ptr: reference to a dsm item pointer: this is payload for PPP
  meta_info_ptr: pointer to a meta info item.
  higher_layer_protocol: Higher layer protocol id.

RETURN VALUE
  Same as pppi_outgoing
   0 on success
  -1 on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ppp_output
(
  ppp_dev_enum_type    device,
  dsm_item_type     ** item_ref_ptr,
  uint16               higher_layer_protocol,
  ps_tx_meta_info_type  * meta_info_ptr
);

/*===========================================================================
FUNCTION PPPI_OUTGOING()

DESCRIPTION
  This function will process the passed datagram into a PPP packet for TX.

  Input Conditions:

  1. iface specifies the iface( Rm or Um) where the PPP pkt. is to be sent.

  2. The protocol argument specifies the protocol of this packet.

     This field can be:

     PPP_IP_PROTOCOL:    For IP datagrams. An attempt will be made to
                         compress the protocol header if compression enabled
     PPP_LCP_PROTOCOL: For LCP proto.
     PPP_IPCP_PROTOCOL: For IPCP
     PPP_PAP_PROTOCOL: For PAP.
     PPP_VJCH_PROTOCOL: For VJ Compressed pkts.
     PPP_VJUCH_PROTOCOL: For VJ Uncompressed pkts.

  3. For all packets the address and control
     fields(FF03) and the protocol fields should be stripped.

   Output Conditions:
     This function takes care to free the passed packet chain even if there
     is a error. If there is no error, it passes the packet chain to the
     lower layer.

DEPENDENCIES
  None

RETURN VALUE
  -1 on error.
   0 on success

SIDE EFFECTS
  None
===========================================================================*/
int pppi_outgoing
(
  ppp_dev_enum_type     out_iface,              /* iface to be sent on     */
  uint16                protocol,               /* protocol of packet      */
  dsm_item_type       **incoming_pkt_ptr,       /* ptr to PPP packet chain */
  ps_tx_meta_info_type *meta_info_ptr           /* ptr to Meta Info        */
);

/*===========================================================================
FUNCTION PPPI_INCOMING()

DESCRIPTION
  This function will process the passed PPP packet as an inbound packet.
  Makes sure that the PFC and ACFC compression are not on when they shouldn't
  be.  When in a network model call, checks to see if there are any callbacks
  associated with this protocol and message pair.  If this is not a network
  model call, the mode will be internal and the snooping is not done.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void pppi_incoming
(
   ppp_dev_enum_type device,           /* ppp device pkt arrived on        */
   dsm_item_type **item_ref_ptr        /* Pointer to ref to pkt            */
);


/*===========================================================================
FUNCTION PPP_SET_MODE()

DESCRIPTION
  This function takes a new mode and interface as arguments and changes the
  mode of the interface appropriately.
  Will throw an error if:
  (1) Mode switch to PPP_PSEUDO_NET_MODE or PPP_FULL_NET_MODE is requested
      without a bridge device set.
  (2) Mode switch to PPP_RELAY_MODE is requested from a mode other than
      itself or PPP_NONE_MODE.
  (3) Mode switch to PPP_PSEUDO_NET_MODE is requested while running in HDLC
      hardware mode.
  (4) An invalid PPP mode switch is requested.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ppp_set_mode
(
  ppp_dev_enum_type  device,             /* device to change              */
  ppp_mode_enum_type new_mode            /* new mode for that device      */
);

/*===========================================================================
FUNCTION PPP_GET_PHASE()

DESCRIPTION
  This function returns the current phase of the interface in question.

DEPENDENCIES
  None

RETURN VALUE
  The phase of the requested interface.

SIDE EFFECTS
  None
===========================================================================*/
ppp_link_phase_type ppp_get_phase
(
  ppp_dev_enum_type device              /* device to check              */
);

/*===========================================================================
FUNCTION PPP_GET_MODE()

DESCRIPTION
  This function returns the current mode of the PPP device in question.

DEPENDENCIES
  None

RETURN VALUE
  The mode of the requested interface.

SIDE EFFECTS
  None
===========================================================================*/
ppp_mode_enum_type ppp_get_mode
(
  ppp_dev_enum_type device              /* device to check              */
);

/*===========================================================================
FUNCTION PPP_FAILOVER_ENABLE()

DESCRIPTION
  This macros will set the failover enum in the specified PPP device and
  set LCP to linger.

DEPENDENCIES
  None

RETURN VALUE
  0 on success
 -1 on failure

SIDE EFFECTS
  Sets failover value in a PPP device and sets the special linger bit in
  the NCP mask.
===========================================================================*/
int ppp_failover_enable
(
  ppp_dev_enum_type   device,
  ppp_failover_e_type failover_value
);

/*===========================================================================
FUNCTION PPP_FAILOVER_DISABLE()

DESCRIPTION
  This macros will set the failover enum in the specified PPP device and
  clear the special LCP linger bit in the NCP mask.  If ncp_mask goes to
  zero then call ppp_stop()

DEPENDENCIES
  None

RETURN VALUE
  0 on sucess
 -1 on failure

SIDE EFFECTS
  Sets failover value in a PPP device and clears the special linger bit in
  the NCP mask.  If the ncp_mask goes to zero then call ppp_stop()
===========================================================================*/
int ppp_failover_disable
(
  ppp_dev_enum_type device
);

/*===========================================================================
FUNCTION PPP_UPDATE_AUTH_SETTING()

DESCRIPTION
  Change the authentication encryption requirement at runtime on a
  particular PPP device.

PARAMETERS
  device:  the device for which the setting is changed
  encrypted_auth_reqd:  TRUE  - require encrypted auth (CHAP)
                        FALSE - allow unencrypted auth (PAP or CHAP)

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS/LIMITATION
  ** must only be called from PS context! **

===========================================================================*/
void ppp_update_auth_setting
(
  ppp_dev_enum_type  device,                /* PPP device to reconfigure   */
  boolean            encrypted_auth_reqd
);

/*===========================================================================
FUNCTION PPP_UPDATE_PASSWORD()

DESCRIPTION
  This function is used to update the PPP password at runtime on a particular
  PPP device.

PARAMETERS
  device       : the device for which the password needs to be updated
  password     : the new password 
  password_len : the length of password

RETURN VALUE
   0: Success
  -1: Failure

DEPENDENCIES
  None

SIDE EFFECTS/LIMITATION
  ** must only be called from PS context! **

===========================================================================*/
int ppp_update_password
(
  ppp_dev_enum_type    device,
  uint8              * password,
  uint8                password_len
);

/*===========================================================================
FUNCTION PPP_UPDATE_USER_ID()

DESCRIPTION
  This function is used to update the PPP user id at runtime on a particular
  PPP device.

PARAMETERS
  device       : the device for which the user id needs to be updated
  user_id      : the new user id
  user_id_len  : the length of the new user id

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS/LIMITATION
  ** must only be called from PS context! **

===========================================================================*/
int ppp_update_user_id
(
  ppp_dev_enum_type    device,
  uint8              * user_id,
  uint8                user_id_len
);

/*===========================================================================
FUNCTION PPP_GET_HDLC_HANDLE()

DESCRIPTION
  Returns an opaque pointer to the HDLC handle for the specified PPP device

PARAMETERS
  None
  
RETURN VALUE
  An opaque pointer to the device HDLC handle. 

DEPENDENCIES
  The HDLC handle should have been allocated before calling this function
  None

SIDE EFFECTS/LIMITATION
  None
===========================================================================*/
void *ppp_get_hdlc_handle
(
  ppp_dev_enum_type device
);

/*===========================================================================
=====
FUNCTION PPP_SET_DEVICE_CONSTANT()

DESCRIPTION
  This function is called to set the configurable protocol constants for
  PPP on the interface specified.

PARAMETERS
  device       : the device for which the user id needs to be updated
  constant     : the PPP constant enum value that needs to be set
  value        : the value that the constant needs to be set to

RETURN VALUE
   0 on success
  -1 on failure

DEPENDENCIES

SIDE EFFECTS/LIMITATION
  None
===========================================================================*/

int ppp_set_device_constant
(
  ppp_dev_enum_type device,
  ppp_init_consts_enum_type constant,
  uint32 value
);

/*===========================================================================
FUNCTION PPP_GET_DEVICE_CONSTANT()

DESCRIPTION
  This function is called to obtain a default device constant value 
  from the ppp_dev_const_s_type structure.

PARAMETERS
  device       : the device for which the user id needs to be updated
  constant     : the PPP constant enum value that needs to be set
  value        : pointer which will store the default value of the 
                 constant

RETURN VALUE
   0 on success
  -1 on failure

DEPENDENCIES

SIDE EFFECTS/LIMITATION
  None
===========================================================================*/

int ppp_get_device_constant
(
  ppp_dev_enum_type device,
  ppp_init_consts_enum_type constant,
  uint32 *value 
);

/*===========================================================================
FUNCTION PPP_GET_MAX_DEVICE()

DESCRIPTION
  This function will return the PPP_MAX_DEV enum.

PARAMETERS
  None

RETURN VALUE
  ppp_dev_enum_type
 
DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
ppp_dev_enum_type ppp_get_max_device (void);

/*===========================================================================
FUNCTION PPP_GET_PEER_VJ_RESULT()

DESCRIPTION
  This function returns TRUE if VJ compression proposed by the peer is 
  accepted during IPCP negotiation. Returns FALSE otherwise. It assumes that
  IPCP negotiation is complete.

DEPENDENCIES
  None

RETURN VALUE
  TRUE
  FALSE

SIDE EFFECTS
  None
===========================================================================*/
boolean ppp_get_peer_vj_result
(
  ppp_dev_enum_type device
);

/*===========================================================================
FUNCTION PPP_BUILD_HEADER_IP()

DESCRIPTION
  This function is used to add the PPP header for IP protocol.
  It writes the header bytes to the location pointed to by header_ptr and
  returns actual number of bytes written via the bytes_added_ptr OUT
  parameter.

DEPENDENCIES
  None

RETURN VALUE
 -1: error
  0: succes

SIDE EFFECTS
  None
===========================================================================*/
int ppp_build_header_ip
(
  ppp_dev_enum_type   device,
  uint8             * header_ptr,
  uint8             * bytes_added_ptr
);

/*===========================================================================
FUNCTION PPP_SET_CP_MODE()

DESCRIPTION
  This function is used to set the CP mode associated with a specific PPP 
  protocol and device.

DEPENDENCIES
  None

RETURN VALUE
  0 on success
 -1 on failure

SIDE EFFECTS
  Sets the CP mode in the FSM associated with a specific PPP protocol.
===========================================================================*/
int ppp_set_cp_mode
(
  ppp_dev_enum_type   device,
  ppp_protocol_e_type protocol,
  cp_mode_type        cp_mode
);

#endif /* PS_PPP_H */

#ifndef PS_PPP_FSM_H
#define PS_PPP_FSM_H
/*===========================================================================

                           P S _ P P P _ F S M . H

DESCRIPTION
  Header file for the PPP protocol suite finite state machine

  Copyright 1992-1995 William Allen Simpson
    Licensed to Qualcomm -- 95 Jan 21

Copyright (c) 1995-2011 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $PVCSPath: MM_DATA/ps_ppp_fsm.h_v   1.8   15 Nov 2002 23:40:02   jeffd  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/api/ps_ppp_fsm.h#1 $
  $Author: mplcsds1 $  $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
11/28/11    jz     ppp control packet on dos(FEATURE_DATA_SEND_PPP_CTL_ON_DOS) 
10/17/11    ssh    eHRPD Always On support
09/14/11    ash    Cancel PPP setup timer after AUTH success
03/23/11    op     Data SU API cleanup
08/31/09    mga    Merged from eHRPD branch
07/15/09    pp     Detached CS lock from ppp_type.
05/07/09    pp     CMI Phase-4: SU Level API Effort.
01/12/09    scb    Resolved MoB LINT errors.
09/02/08    rnp    Added STA related changes.
07/18/08    rnp    Fixed Compiler warnings
05/27/08    rnp    Added messages and additional functionality reqd. for IPCP
                   STA tests.
05/04/08    scb    PPP STA development
01/22/08    scb    Mainlined features under FEATURE_JCDMA_1X
09/27/07    ifk    Removed old PPP statistics code.
01/10/06    ssh    Added support for vendor-specific packets
05/25/06    ssh    Added fsm_find_option()
04/20/06    ssh    Changes to PPP rx processing, pseudonet FSM et. al.
03/05/06    ifk    Pass DSM item containing rejected option contents to
                   protocol specific reject function.
12/16/05    ssh    Added rex critical section to PPP control block (for L4)
11/22/05    ssh    Added function pointers for iface input and configuration
09/14/05    ssh    Added new internal header file ps_hdlc_libi.h to includes.
08/15/05    kr     Added support for stats update
07/21/05    ssh    Changes pursuant to new ppp_fail_reason_e_type
03/25/05    ifk    Fixed fsm_n_option() handling
03/21/05    ssh    Changes pursuant to the new typedef ps_timer_handle_type
10/31/04    ifk    Added ccp_config to PPP device to support CCP
07/16/04    ifk    Added failover enum and require_encrypted_passwd to PPP
                   control block, removed remote_initiate.  Added LCP linger
                   bit define
07/01/04    ifk    Removed dsm_kind field from fsm constants
05/25/04    ifk    Changed event notification structures.  Added queue of
                   events information structures in PPP control block.
05/05/04    mvl    Moved the definition of the unframing mode to the hdlc
                   header file.
05/03/04    sy     Added rx_sig_f_ptr to the ppp_s structure.
04/16/04    ifk    Added remote_initiate field to fsm struct
03/15/04    ifk    Removed pend_tx_f_ptr, pend_rx_f_ptr and associated cookies
                   from ppp_cb structure.
01/31/04    usb    Support for new config item passive_wakeup_on_data.
01/28/04    mvl    Will now only perform CHAP when in Network or Auth phase.
10/24/03    ifk    Split timeout in fsm_constant_s to term_timeout and
                   ack_timeout, added compress_enable an associated enum.
                   Removed fsm_timer() prototype.  Added timeout and
                   retry_limit fields to auth structure in ppp_type.
05/13/03    usb    Moved MIP policy info to dssnet control block.
04/08/03    mvl    Added support for not moving to the network phase if auth
                   info not received.
04/06/03    mvl    Addition of capabilities to support server style
                   authentication for both PAP and CHAP.
04/02/03    mvl    Complete modularization of the authentication code.
11/14/02    jd     Split qcmip_mode into will_sip, want_mip
11/04/02    mvl    Fixed type of the guard timer handle.
09/25/02    mvl    Modified signal usage.
09/24/02    usb    Added user_data to ppp control block tx and rx functions
09/18/02    mvl    Added user_data to ppp event callbacks.  Added explicit
                   configuration for client vs. server behavior.  Removed
                   do_mip.
09/12/02    mvl    Added the setup timeout field and the timer handle for
                   this timeout to the PPP CB.
08/05/02  dwp/mvl  Add tail_full to asy_in_var_type.
08/06/02    mvl    Reworked authentication.
07/25/02    mvl    Moved some definitions to ps_ppp_defs.h to clean up
                   dependencies.  Removed network model dependency.  Updates
                   for PPP renaming.
07/15/02    aku    Removed include for pstimer.h
07/11/02    mvl    Updates for new PPP architecture.
07/01/02    mvl    Added event callback and bridging support.
05/23/02    mvl    reworked PPP
05/20/02     vr    Added hdlc_mode in ppp control block
03/08/02    mvl    Fixed some formatting problems (no content change).
03/01/02    vr     Added support for PPP passive mode.
02/05/02    vr     Added flag do_ipcp for configuring No IPCP on AN stream
12/01/01    vas    Added do_auth field to ppp ctl blk
08/31/01    mvl    Changed the #defines of the msg types to an enum for
                   easier debugging.
08/29/01  pjb/mvl  Modified the values option bits to support DNS options.
08/02/01    mvl    iface field in fsm_type is not dependent on network model.
                   removed the ppp_cb_ptr - only ppp_cb_array remains
                   removed prototype for fsm_log(), general code cleanup
07/27/01    mvl    Modified the timer type in the fsm_type so pstimers could
                   be used.  Also reordered fields for better packing.
07/26/01    na     Added do_mip and qcmip_mode to struct ppp_s.
07/22/01    mvl    Added iface field to FSM struct, put conditional compile
                   flags around some fields in the FSM struct to conserve
                   memory, removed obsolete debug macros, and code cleanup
05/02/01    mvl    Added booleans 'in_mip_mode', and 'force_sip_next_call'
                   for MIP.
04/27/01    mvl    Reoredered fields in ppp_cb for more efficient packing.
08/20/99    mvl    Added macro CONFIG_HDR_LEN
08/09/99    mvl    Added network model changes.
04/08/99    na     Added dsm_kind field to the fsm_constant_s struct
11/09/95    jjw    IS-99 Compliant Release
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "customer.h"       /* Customer Specific Features */
#include "comdef.h"
#include "dsm.h"
#include "ps_ppp_defs.h"
#include "ps_ppp_sig.h"
#include "ps_ppp_vend.h"
#include "ps_iface_defs.h"
#include "pstimer.h"
#include "rex.h"
#include "ps_eap.h"
/*===========================================================================

                          PUBLIC DATA DECLARATIONS

===========================================================================*/
/*---------------------------------------------------------------------------
  The code values for LCP/IPCP packets
---------------------------------------------------------------------------*/
enum
{
 VENDOR_SPECIFIC =  0,
 CONFIG_REQ      =  1,
 CONFIG_ACK      =  2,
 CONFIG_NAK      =  3,
 CONFIG_REJ      =  4,
 TERM_REQ        =  5,
 TERM_ACK        =  6,
 CODE_REJ        =  7,
 PROT_REJ        =  8,
 ECHO_REQ        =  9,
 ECHO_REPLY      = 10,
 DISCARD_REQ     = 11,
 IDENTIFICATION  = 12,
 TIME_REMAINING  = 13
};

/*---------------------------------------------------------------------------
  Supported Configuration Protocol index
---------------------------------------------------------------------------*/
enum
{
  LCP       = PPP_PROTOCOL_LCP,
  IPCP      = PPP_PROTOCOL_IPCP,
  IPV6CP    = PPP_PROTOCOL_IPV6CP,
  fsmi_Size,  /* Number of FSMs needed in the ppp_cb              */
  VSNCP     = PPP_PROTOCOL_VSNCP,
  AUTH      = PPP_PROTOCOL_AUTH
};

/* Length of config packet header */
#define CONFIG_HDR_LEN 4

/*---------------------------------------------------------------------------
  This is the max. number of bytes per PPP option that this implementation
  supports. This is used to declare byte arrays to store option values.
---------------------------------------------------------------------------*/
#define PPP_OPT_MAX_LEN 4

/*---------------------------------------------------------------------------
  config option header
---------------------------------------------------------------------------*/
struct option_hdr
{
  uint8  type;          /* protocol dependent types */
  uint8  length;
};
typedef struct option_hdr ppp_opt_hdr_type;

/* Length of option header */
#define OPTION_HDR_LEN 2


/*---------------------------------------------------------------------------
  Maximum length of the PPP "header" (bytes before the first 
  configuration-option in a PPP packet). This needs to be large enough to 
  hold the HDLC-framed version of the following fields:
  Address-Control-Protocol-Code-Identifier-Length
---------------------------------------------------------------------------*/
#define PPP_MAX_HDR      16       /* Size of PPP header */

struct fsm_s;          /* forward declaration */
struct fsm_side_s;     /* forward declaration */

typedef enum ppp_ncp_compress_enable_e_type
{
  PPP_NCP_COMPRESS_IGNORE  = 0,  /* Use value passed in by client */
  PPP_NCP_COMPRESS_ENABLE  = 1,  /* Enable compression            */
  PPP_NCP_COMPRESS_DISABLE = 2   /* Disable compresion            */
} ppp_ncp_compress_enable_e_type;

/*---------------------------------------------------------------------------
  Protocol Constants needed by State Machine
---------------------------------------------------------------------------*/
struct fsm_constant_s
{
  uint16 protocol;         /* Protocol number                      */
  uint16 recognize;        /* Codes to use (bits)                  */

  uint8  fsmi;             /* Finite State Machine index           */
  uint8  try_configure;    /* # tries for configure                */
  uint8  try_nak;          /* # tries for nak substitutes          */
  uint8  try_terminate;    /* # tries for terminate                */
  uint16 term_timeout;     /* Time for TERM REQ timeouts (msecs)   */
  uint16 ack_timeout;      /* Time to wait for C_ACK (msecs)       */

  uint8  option_limit;     /* maximum option number                */
  uint8  n_option_limit;   /* maximum mask number                  */
  uint8  *n_option;        /* mask table                           */

  ppp_ncp_compress_enable_e_type  compress_enable;
                           /* Enable/Disable compression for NCP   */

  /* To free structure */
  void (*fsm_free)
  (
    struct fsm_s *fsm_ptr   /* ptr to a Finite State Machine (FSM) structure */
  );

  /* Lower Layer Down event */
  void (*down)
  (
    struct fsm_s *fsm_ptr   /* ptr to a Finite State Machine (FSM) structure */
  );

  /* This Layer Start: entering Starting */
  void (*starting)
  (
    struct fsm_s *fsm_ptr   /* ptr to a Finite State Machine (FSM) structure */
  );

  /* This Layer Up: entering Opened */
  void (*opening)
  (
    struct fsm_s *fsm_ptr   /* ptr to a Finite State Machine (FSM) structure */
  );

  /* This Layer Down: leaving Opened */
  void (*closing)
  (
    struct fsm_s *fsm_ptr   /* ptr to a Finite State Machine (FSM) structure */
  );

  /* This Layer Finish: entering Closed or Stopped */
  void (*stopping)
  (
    struct fsm_s *fsm_ptr   /* ptr to a Finite State Machine (FSM) structure */
  );

  void (*option)
  (
    struct fsm_s      *fsm_ptr,        /* ptr to a FSM structure */
    void              *vp,           /*   */
    struct dsm_item_s **bpp,         /*   */
    uint8             o_type         /*   */
  );

  int (*check)
  (
    struct fsm_s      *fsm_ptr,      /*  */
    struct option_hdr *ohp,        /*  */
    struct dsm_item_s **bpp,       /*  */
    int               request      /*  */
  );

  int (*reject)
  (
    struct fsm_s      *fsm_ptr,      /*   */
    struct option_hdr *ohp,        /*   */
    dsm_item_type     *bp
  );
};

/*---------------------------------------------------------------------------
  FSM states
---------------------------------------------------------------------------*/
enum
{
  fsmINITIAL  = 0,
  fsmSTARTING = 1,
  fsmCLOSED   = 2,
  fsmSTOPPED  = 3,
  fsmCLOSING  = 4,
  fsmSTOPPING = 5,
  fsmREQ_Sent = 6,
  fsmACK_Rcvd = 7,
  fsmACK_Sent = 8,
  fsmOPENED   = 9,
  fsmState_Size
};

/*---------------------------------------------------------------------------
  local.will:  Options to accept in a NAK from remote,
      which may be different from the desired value.
  local.want:  Options to request.
      Contains desired value.
      Only non-default options need to be negotiated.
      Initially, all are default.
  local.work:  Options currently being negotiated.
      Value is valid only when negotiate bit is set.

  remote.will:  Options to accept in a REQ from remote,
      which may be different from the desired value.
  remote.want:  Options to suggest by NAK if not present in REQ.
      Contains desired value.
  remote.work:  Options currently being negotiated.
      Value is valid only when negotiate bit is set.
---------------------------------------------------------------------------*/
struct fsm_side_s
{
  negotiate_t will;
  negotiate_t want;
  void  *want_pdv;
  negotiate_t work;
  void  *work_pdv;
};

typedef struct fsm_side_s ppp_fsm_side_type;

/*===========================================================================
FUNCTION PPP_IFACE_INPUT_F_TYPE()

DESCRIPTION
  Function pointer of function to call in the PS iface with the input
  packet

PARAMETERS
  this_iface_ptr:    ptr to interface control block on which to operate on.
  pkt_ref_ptr:       ref to dsm item ptr received
  meta_info_ptr:     ptr to meta info (per pkt information)

RETURN VALUE
  0: data was transfered
 -1: data was not transferred, but memory was freed

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
typedef int (*ppp_iface_input_f_type)
(
  void               *this_iface_ptr,
  dsm_item_type     **pkt_ref_ptr,
  void               *meta_info_ptr
);
/*===========================================================================
FUNCTION PS_TIMER_START_F_TYPE()

DESCRIPTION
  Function pointer of function to initiate the ps_timer allocated to start

PARAMETERS
  handle:            ptr to ps_timer_handle_type
  int64 :            length of timer

RETURN VALUE
  PS_TIMER_FAILURE: On failure
  PS_TIMER_SUCCESS: On success

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/

typedef ps_timer_error_type (*timer_start_f_type)
(
  ps_timer_handle_type handle,            /* handle ass. with timer */
  int64                timer_len          /* length of the timer */
);
/*===========================================================================
FUNCTION PPP_TIMER_CANCEL_F_TYPE()

DESCRIPTION
 Function pointer of function to initiate the ps_timer allocated to stop


PARAMETERS
  handle:            ptr to ps_timer_handle_type

RETURN VALUE
  PS_TIMER_FAILURE: On failure
  PS_TIMER_SUCCESS: On success

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
 
typedef ps_timer_error_type (*timer_cancel_f_type)
(
  ps_timer_handle_type handle             /* handle associated with timer */
); 

/*===========================================================================
FUNCTION PS_TIMER_ALLOC

DESCRIPTION
  This is a function pointer to a function which allocates a timer. 
  Before any task uses the timer, it should first acquire a timer by 
  calling alloc_timer. This function returns a timer handle to the caller.
  This timer handle can be used in the future to access this timer 
  (until it is freed).

  Every timer may have a callback associated with it.  So, it takes the
  function callback as an argument.  The callback function must have the
  following signature: void fname(void *);  If NULL is specified as the
  callback parameter, no function is called.

DEPENDENCIES
  After the timer is used, call FREE_TIMER. This will return the timer back
  to the free pool.

PARAMETERS
  callback  :The callback function to call when the timer expires.
  cb_param  :The parameter to pass back in the client callback.

RETURN VALUE
   Success: Returns value between 1 to PS_TIMER_MAX.
   Failure: PS_TIMER_FAILURE

SIDE EFFECTS
  None
===========================================================================*/
typedef ps_timer_handle_type (*timer_alloc_f_type)
(
  void (* callback) (void *),
  void *cb_param
);

/*===========================================================================
FUNCTION PPP_IFACE_CONFIG_F_TYPE()

DESCRIPTION
  Function pointer of function to call in the PS iface with the config
  information

PARAMETERS
  device     PPP device to configure

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
typedef void (*ppp_iface_config_f_type)
(
  ppp_dev_enum_type  device
);

/*===========================================================================
FUNCTION RX_PKT_HDLR_F_TYPE()

DESCRIPTION
  Function pointer for the function handler in PPP to process the packet
  received from the lower layer, according to the mode of the PPP device
  for which the packet is received.

  Here is a description of the processing for various modes.

  +--------------------------+--------------------------------------------+
  |        PPP Mode          |           Processing routine               |
  +--------------------------+--------------------------------------------+
  | PPP_NONE_MODE            | pppi_none_mode_hdlr(), frees incoming pkt  |
  +--------------------------+--------------------------------------------+
  | PPP_RELAY_MODE           | pppi_relay_mode_hdlr(), INVALID condition  |
  +--------------------------+--------------------------------------------+
  | PPP_PSEUDO_NET_MODE      | pppi_pseudo_net_mode_hdlr(), snoops and    |
  |                          | unframes (via HDLC library) if needed      |
  +--------------------------+--------------------------------------------+
  | PPP_INTERNAL_MODE        |                                            |
  +--------------------------+  pppi_internal_mode_hdlr(), unframes (if   |
  | PPP_FULL_NET_MODE        |  HDLC is enabled) and dispatches packet up |
  +--------------------------+  the stack                                 |
  | PPP_INTERNAL_SNOOP_MODE  |                                            |
  +--------------------------+--------------------------------------------+

PARAMETERS
  device:        PPP device pkt arrived on
  hdlc_inst:     Handle to HDLC instance
  item_head_ptr: address of ptr to a dsm_item_type

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  Probably.
===========================================================================*/
typedef void (*rx_pkt_hdlr_f_type)
(
  ppp_dev_enum_type   device,         /* PPP device pkt arrived on         */
  void              * hdlc_inst,            /* Handle to HDLC instance     */
  dsm_item_type    ** item_head_ptr   /* address of ptr to a dsm_item_type */
);

/*---------------------------------------------------------------------------
  State Machine Control Block
---------------------------------------------------------------------------*/
struct fsm_s
{
  struct fsm_side_s local;     /* local protocol option information        */
  struct fsm_side_s remote;    /* local protocol option information        */

  struct fsm_constant_s *pdc;  /* protocol dependent constants             */

  struct cb_constant_s  *cbdc; /* control dependent constants              */
  void *pdv;                   /* protocol dependent variables             */
  ps_timer_handle_type timer;  /* Handle for this FSMs timer               */

  ppp_dev_enum_type device;    /* the interface this struct corresponds to */
  cp_mode_type mode;           /* the mode this fsm                        */

  uint8  state;                /* FSM state                                */
  uint8  lastid;               /* ID of last REQ we sent                   */

  uint8  flags;

  ppp_fail_reason_e_type fail_reason;
                              /* cause of failure to bring up FSM          */

  uint8  retry_request;        /* counter for timeouts                     */
  uint8  try_configure;        /* # tries for configure                    */
  uint8  try_terminate;        /* # tries for terminate                    */

  uint8  retry_nak;            /* counter for naks of requests             */
  uint8  try_nak;              /* # tries for nak substitutes              */

  uint8  id;                   /* id counter for connection                */
  boolean allow_vendor_pkt;    /* is it ok to send vendor-specific pkts ?  */
  ps_iface_type           *rx_iface_ptr; /* ptr to receive interface       */
  /*------------------------------------------------------------------------ 
    The second iface ptr is used only for the VSNCP NCP. The rx_iface_ptr
    will be used as a v4 iface and the second_rx_iface_ptr will be used as a
    v6 iface. For both Dual IP and single IP case the rx_iface_ptr will
    be v4 and the sec_rx_iface_ptr will be v6 iface ptr.
    For other NCPs (IPCP, IPv6CP) the rx_iface_ptr in the corresponding
    FSM will accordingly be used as v4 or as v6 ie IPCP FSM's
    rx_iface_ptr is used as v4 and the IPV6CP FSM's rx_iface_ptr as v6.
  ------------------------------------------------------------------------*/

  ps_iface_type       *second_rx_iface_ptr; /* second iface ptr for DUAL IP*/
  ppp_iface_input_f_type   iface_input;  /* func ptr for iface rx function */
  ppp_iface_config_f_type  iface_config; /* func ptr to configure iface    */

  timer_alloc_f_type       timer_alloc_fcn;  /* func ptr to alloc timer*/
  timer_start_f_type       timer_start_fcn;  /* func ptr to start timer*/
  timer_cancel_f_type      timer_cancel_fcn; /* func ptr to cancel timer*/

  negotiate_t         send_options_mask;  /*Options mask for C-REQ/C-NAK*/

  struct ppp_fsm_stats
  {
    uint32  pkts_rx                 ;
    uint32  pkts_tx                 ;
    uint32  config_requests_rx      ;
    uint32  config_acks_rx          ;
    uint32  config_naks_rx          ;
    uint32  config_rejects_rx       ;
    uint32  term_requests_rx        ;
    uint32  term_acks_rx            ;
    uint32  code_rejects_rx         ;
    uint32  proto_rejects_rx        ;
    uint32  echo_requests_rx        ;
    uint32  echo_replies_rx         ;
    uint32  discard_requests_rx     ;
    uint32  identification_rx       ;
    uint32  time_remaining_rx       ;
    uint32  config_requests_tx      ;
    uint32  config_acks_tx          ;
    uint32  config_naks_tx          ;
    uint32  config_rejects_tx       ;
    uint32  term_requests_tx        ;
    uint32  term_acks_tx            ;
    uint32  code_rejects_tx         ;
    uint32  proto_rejects_tx        ;
    uint32  echo_requests_tx        ;
    uint32  echo_replies_tx         ;
    uint32  discard_requests_tx     ;
  } ppp_i_fsm_stats;

};
typedef struct fsm_s ppp_fsm_type;

/*---------------------------------------------------------------------------
  This struct maintains all the static variables for
  pppi_pseudonet_mode_hdlr (which implements the PPP pseudonet FSM)
---------------------------------------------------------------------------*/
typedef struct
{
  dsm_item_type           * snoop_pkt_ptr;   /* pkt with snooped bytes     */
  dsm_item_type           * framed_pkt_ptr; /* data to fwd to bridge dev   */
  dsm_item_type           * framed_pkt_tail_ptr; /* tail of above chain    */
  hdlc_unframe_state_type   state;            /* current state of the SM   */
  boolean                   escape;     /* was last byte the escape char?  */
} ppp_pseudonet_type;

/*===========================================================================
                              PPP Control Block

TYPEDEF PPP_TYPE
===========================================================================*/
struct ppp_s
{
  /*-------------------------------------------------------------------------
    The array of FSMs should be the first item in the PPP control block.
    Functions liks fsm_down() get the PPP cb pointer from the relative
    position of the fsm_ptr.
  -------------------------------------------------------------------------*/
  ppp_fsm_type          fsm[fsmi_Size];/* finite state machines            */
  ppp_pseudonet_type    pseudonet;     /* vars for pseudonet FSM           */
  negotiate_t           ncp_mask;      /* Which NCPs are active?           */

  /*-------------------------------------------------------------------------
    Queue of events information structures registered with the PPP device.
    This is being implemented as a single queue to simplify memory of the
    event handles in the clients and the event information structures in the
    PPP device.
  -------------------------------------------------------------------------*/
  q_type                event_queue;   /* Queue of PPP event info items    */

  /*-------------------------------------------------------------------------
    PPP failover enum
  -------------------------------------------------------------------------*/
  ppp_failover_e_type   failover;

  /*-------------------------------------------------------------------------
    Authentication related information
  -------------------------------------------------------------------------*/
  struct ppp_cb_auth_s
  {
    ppp_auth_info_type info;          /* auth info: MUST BE FIRST ELEMENT  */
    dsm_item_type *chap_challenge_ptr;/* to store CHAP challenge           */
    void (*retry_proc)(struct ppp_s*);/* function ptr to handle retry proc */
    uint16             timeout;       /* timeout value for auth messages   */
    uint8              mode;          /* auth negotiated:CHAP, PAP or NONE */
    uint8              active_id;     /* in use ID value for auth          */
    uint8              retry_counter; /* retry counter for auth attempts   */
    uint8              retry_limit;   /* num of retry attempts configured  */
#define PPP_AP_LOCAL   0x01           /* waiting to be authenticated       */
#define PPP_AP_REMOTE  0x02           /* waiting to authenticate peer      */
#define PPP_AP_SUCCESS 0x04           /* authentication succeeded          */
#define PPP_AP_FAILED  0x08           /* authentication failed             */
    uint8              flags;
    boolean require_encrypted_passwd; /* Require passwords to be encrypted */
    eap_handle_type ps_eap_handle;    /* EAP Handle */
  } auth;

  /*-------------------------------------------------------------------------
    Configuration Info
  -------------------------------------------------------------------------*/
  tx_f_ptr_type       tx_f_ptr;     /* function user to tx PPP packets     */
  rx_f_ptr_type       rx_f_ptr;     /* Function to receive next data item  */
  void               *tx_user_data; /* user date for tx_function           */
  void               *rx_user_data; /* user date for tx_function           */
  ppp_sig_type        rx_signal;    /* signal to use for RX                */
  ppp_sig_handler_type rx_sig_f_ptr;/* PPP data receive signal handler     */
  ps_timer_handle_type setup_timer; /* handle for the setup timer          */
  uint16              setup_timeout;/* time (ms) before giving up on setup */
  uint16              session_handle;/* session handle to id iface calls   */
  ppp_link_phase_type phase;        /* phase of link initialization        */
  boolean             serves_addrs; /* does this PPP serve IP addresses?   */
  ppp_dev_enum_type   bridge_dev;   /* interface to bridge to if snooping  */
  ppp_mode_enum_type  mode;         /* the mode this device is in          */
  hdlc_mode_enum_type hdlc_mode;    /* HDLC framing in HW/SW or none       */
  boolean             passive_wakeup_on_data; /* passive wakeup on any pkt */
  ppp_ccp_config_e_type ccp_config; /* Configuration for CCP               */

  /*-------------------------------------------------------------------------
    HDLC handle
  -------------------------------------------------------------------------*/
  void              * hdlc_handle;

  /*-------------------------------------------------------------------------
    Header compression handles
  -------------------------------------------------------------------------*/
  /* None right now */

  /*-------------------------------------------------------------------------
    Function pointer for the function handler in PPP to process the packet
    received from the lower layer. This is set according to the mode of the
    PPP device. See prototype definition for a description of the actual
    packet handlers this is  set to.
  -------------------------------------------------------------------------*/
  rx_pkt_hdlr_f_type  rx_pkt_hdlr_f_ptr;

  /*-------------------------------------------------------------------------
    Pending mode is used to switch from pseudonet to one of the "internal"
    modes (PPP_INTERNAL_MODE, PPP_FULL_NET_MODE or  PPP_INTERNAL_SNOOP_MODE).
    This variable stores the mode to move to at the end of current packet
    being processed in pseudonet mode.
  -------------------------------------------------------------------------*/
  ppp_mode_enum_type  pending_mode;

  /*-------------------------------------------------------------------------
    Stats
  -------------------------------------------------------------------------*/
  struct ppp_stats
  {
    uint32  bad_fcs                     ;
    uint32  bad_proto                   ;
    uint32  unsupported_proto           ;
    uint32  not_enabled_proto           ;
    uint32  missing_addr                ;
    uint32  missing_ctrl                ;
    uint32  pkts_dropped_rx             ;
    uint32  pkts_rx                     ;
    uint64  framed_bytes_rx             ;
    uint64  unframed_bytes_rx           ;
    uint32  pkts_dropped_tx             ;
    uint32  pkts_tx                     ;
    uint64  framed_bytes_tx             ;
    uint64  unframed_bytes_tx           ;
    uint32  ipv4_pkts_rx                ;
    uint32  ipv4_pkts_tx                ;
    uint32  vjch_pkts_rx                ;
    uint32  vjch_pkts_tx                ;
    uint32  vjuch_pkts_rx               ;
    uint32  vjuch_pkts_tx               ;
    uint32  pap_pkts_rx                 ;
    uint32  pap_pkts_tx                 ;
    uint32  pap_auth_failures           ;
    uint32  chap_pkts_rx                ;
    uint32  chap_pkts_tx                ;
    uint32  chap_auth_failures          ;
    uint32  ipv6_pkts_rx                ;
    uint32  ipv6_pkts_tx                ;
    uint32  queried                     ;
  } ppp_i_stats;

  /*-------------------------------------------------------------------------
    Control block for vendor-specific OUIs
  -------------------------------------------------------------------------*/
  ppp_vendor_info_s_type  vendor_cb_array[PPP_OUI_MAX];
  
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

  /*-------------------------------------------------------------------------
    Link status Determination info
  -------------------------------------------------------------------------*/
  struct ppp_cb_lsd_s
  {
    ppp_lsd_config_type  config;               /* control block            */
    ps_timer_handle_type timer;                /* handle for the LSD timer */
  } lsd;
};
typedef struct ppp_s ppp_type;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
int fsm_send
(
  struct fsm_s  *fsm_ptr,
  uint8          code,
  uint16         id,
  dsm_item_type *bp
);

int fsm_find_option
(
  dsm_item_type ** bpp,          
  uint16           opt_type,    
  void           * opt_val_ptr,
  uint8            opt_val_len
);

/*===========================================================================
FUNCTION       FSM_GET_PPP_CB_ARRAY

DESCRIPTION    Access method for ppp_cb_array

PARAMETERS
  None
 
RETURN VALUE   
  ppp_cb_array

DEPENDENCIES   
  None

SIDE EFFECTS   
  None
===========================================================================*/
ppp_type *fsm_get_ppp_cb_array(void);

#endif /* _PS_PPP_FSM_H */

#ifndef PS_PPPI_H
#define PS_PPPI_H
/*===========================================================================
                              P S _ P P P I . H

DESCRIPTION
  Header file for the internal PPP protocol suite functions.

  Copyright 1992-1995 William Allen Simpson
    Licensed to Qualcomm -- 95 Jan 21

Copyright (c) 1995-2014 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_pppi.h_v   1.11   03 Feb 2003 13:59:24   jeffd  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/linklayer/inc/ps_pppi.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/23/14    skc    Removing FEATURE_DATA_PS_HDLC_PP
06/06/14    vl     BSS optimization changes
10/17/11    ssh    eHRPD Always On support
06/06/11    ash    Added support for PPP Partial Context
02/24/11    ash    Cleaning up global variables for Q6 free floating effort.
10/05/09    mga    Merged changes related to diag events
08/31/09    mga    Merged from eHRPD branch
07/27/09    sp     Fixed IP Fragmentation issues.
05/07/09    pp     CMI Phase-4: SU Level API Effort.
09/12/08    pp     Metainfo optimizations.
09/02/08    rnp    Added STA related changes.
05/27/08    rnp    Added additional functionality reqd. for IPCP STA tests.
05/04/08    scb    PPP STA development
02/08/08    scb    Added featurization FEATURE_DATA_PS_IPV6 around 
                   pppi_failover_on_resync()
04/02/07    ssh    Common structure for PPP command buffers
08/31/06    mjp    Added IPHC and ROHC Logging
04/20/06    ssh    Cleanup. Removed old sw/hw frame/unframe functions.
09/14/05    ssh    Added defines HDLC_ALL_ADDR and HDLC_UI
08/15/05    kr     Removed features DS_PSTATS and DS_DEBUG_UI
08/11/05    ssh    Added additional checks in pppi_set_mode()
04/11/05    ssh    Added session_handle to ppp_config_buffer_s_type
03/22/05    ssh    Added prototype for new function
                   pppi_report_event_to_diag(), and added ps_ppp_events.h to
                   includes.
02/10/05    ifk    Took out HDR auth registered callback function.  Using PPP
                   auth events now.
10/30/04    ks     Changed ptr to ptr of ps_meta_info_type to single pointer
                   in pppi_iface_tx_function() and pppi_outgoing().
10/14/04    ifk    Added meta info ** argument to pppi_receive()
07/16/04    ifk    Added pppi_failover_on_resync() and pppi_resync()
                   prototypes.  resync_protocol to PPP config buffer type
06/16/04    ifk    Added pppi_call_event_callback()
06/11/04    vp     Constant define for ccDEL.
05/05/04    mvl    Using definitions from hdlc_lib file.
05/03/04    sy     Added pppi_get_data_cback() prototype.
04/30/04    usb    Removed callback func to get peer info.
08/13/03    ifk    Cleaned up in preparation for addding IP6CP
04/07/03    usb    Added prototype for pppi_receive()
04/02/03    mvl    Complete modularization of the authentication code.
01/27/03    jd     Added prototype for process_chap_pkt()
01/27/03    usb    Added pppi_set_mode() to set PPP mode in PS context
11/14/02    mvl    Moved the protocol definitions to the defs file.
10/25/02    usb    Modified prototype of pppi_outgoing to take meta info as
                   double pointer
10/11/02    usb    Modified HDLC hw feature to FEATURE_HDLC_HW_ACCEL
09/25/02    mvl    Modified signal usage.
09/24/02    usb    Added user_data to ppp tx and rx function callbacks
09/02/02    mvl    added declaration for the ps iface tx function.
08/13/02    mvl    added session handle support.
07/25/02    mvl    removed netmodel dependency.  Updates for PPP renaming.
07/11/02    mvl    Updates for new PPP architecture.
05/22/02    mvl    Renamed the file and moved all of the truly public stuff
                   out into the new file ppp.h
05/20/02     vr    Added support for hardware HDLC framer/deframer
03/18/02    vas    Added declaration for callback func to call when CHAP on
                   HDR AN iface succeeds / fails.
02/12/02    vsk    Defined internal ppp_ip_pkt_hdlr_f_ptr_type to make it
                   build for all targets
02/07/02    vsk    Added ppp_install_ip_pkt_handler() &
                   ppp_uninstall_ip_pkt_handler() prototypes
11/17/01    aku    Changed 2nd arg in ppp_initiate_resync() to boolean type
11/12/01    dwp    Added ppp_incoming() prototype.
11/05/01    vr     During MIP re-reg, forward non-reg pkts to laptop
09/14/01    jd     Added iface mode parameter to ppp_initiate_resync
08/02/01    mvl    modified ppp_discard() signature and code cleanup.
07/31/01    na     Added prototype for ppp_initiate_resync().
07/30/01    mvl    added prototype for ppp_get_iface_opts() code cleanup.
07/27/01    mvl    Modified to use pstimers, added code to support getting
                   the options negotiated on a particular interface.
07/24/01    mvl    modified the signatures of ppp_close() and ppp_initiate()
05/04/00    mvl    changed interface to network model version of
                   ppp_outgoing() to fix bug where item was freed in this
                   function then freed again lower down.
12/10/99    mvl    Added comment in front of netmodel PPP stats.
10/28/99    mvl    Added code review changes
10/22/99    mvl    Added support for network model related statistics.
08/25/99    mvl    Added header for ppp_asy_in_init().
08/09/99    mvl    Added network model changes.
10/27/98    ldg    ARM porting changes: ROM to ROM const.
07/31/97    ldg    Added accessor function for UI to see some PPP vars.
06/12/97    ldg    Added new FEATURE_DS_PSTATS
11/09/95    jjw    IS-99 Compliant Release
===========================================================================*/



/*===========================================================================

                                INCLUDE FILES

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */

#include "amssassert.h"
#include "dsm.h"
#include "ps_ppp_defs.h"
#include "ps_ppp_task.h"
#include "ps_iface_defs.h"
#include "ps_tx_meta_info.h"
#include "ps_ppp.h"
#include "ps_ppp_events.h"
#include "ps_pppi_vend.h"
#ifdef FEATURE_DATA_PS_HDLC_A2_AUTONOMOUS
#include "a2_hdlc.h"
#endif /*FEATURE_DATA_PS_HDLC_A2_AUTONOMOUS*/

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
/*---------------------------------------------------------------------------
TYPEDEF PPPI_START_INFO_TYPE

DESCRIPTION
  Type that is used to pass the ppp device and the session handle from the
  callers context to the PPP execution context.
---------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint16 device;
  uint16 session_handle;
} pppi_start_info_type;

/*---------------------------------------------------------------------------
TYPEDEF PPPI_ABORT_INFO_TYPE

DESCRIPTION
  Type that is used to pass the ppp device and the session handle from the
  callers context to the PPP execution context to abort the session.
---------------------------------------------------------------------------*/
typedef pppi_start_info_type pppi_abort_info_type;

/*---------------------------------------------------------------------------
TYPEDEF PPPI_STOP_INFO_TYPE

DESCRIPTION
  Type that is used to pass the ppp device and protocol from the
  callers context to the PPP execution context.
---------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint16 device;
  uint16 protocol;
} pppi_stop_info_type;

/*---------------------------------------------------------------------------
TYPEDEF PPPI_MODE_INFO_TYPE

DESCRIPTION
  Type that is used to pass the ppp device and the PPP mode from the
  callers context to the PPP execution context.
---------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint16 device;
  uint16 mode;
} pppi_mode_info_type;

#define PPP_MIN_SETUP_TIME 5

/*---------------------------------------------------------------------------
  Offsets for certain fields in a PPP packet (assume FF03 is stripped off.
  Also assume no compressions in the PPP header.)
---------------------------------------------------------------------------*/
#define PPP_IPSADDR_OFFSET     12     /* Location of Source IP addr.       */
#define PPP_IPPROTO_OFFSET      9     /* Location of IP protocol field     */

/*---------------------------------------------------------------------------
  The PPP header data structure
---------------------------------------------------------------------------*/
struct ppp_hdr
{
  uint8  addr;
  uint8  control;
  uint16 protocol;
};
typedef struct ppp_hdr ppp_hdr_type;

/*---------------------------------------------------------------------------
  defines for HDLC header
---------------------------------------------------------------------------*/
#define HDLC_ALL_ADDR   0xff   /* HDLC all-station                         */
#define HDLC_UI         0x03   /* HDLC Unnumbered Information              */

/*---------------------------------------------------------------------------
  This is the type declaration for the callback function that is
  registered for processing IP packets received on a PPP interface
---------------------------------------------------------------------------*/
typedef int (*ppp_ip_pkt_hdlr_fn_ptr_type)
(
  ppp_dev_enum_type iface,                               /* Input interface */
  struct dsm_item_s *item_ptr,                               /*Input packet */
  uint8  *source,                             /*media source address if any */
  int    broadcast,                /*True if pkt had link broadcast address */
  void   *context_data                         /* context data for callback */
);

/*---------------------------------------------------------------------------
  Type for PPP configuration buffer type used to pass configuration
  information between application context and PPP context.
---------------------------------------------------------------------------*/
typedef struct
{
  ppp_dev_enum_type device;
  ppp_dev_opts_type dev_opts;
  boolean           dev_opts_null;
  ppp_protocol_e_type resync_protocol;
  uint16              session_handle;
} ppp_config_buffer_s_type;

/*---------------------------------------------------------------------------
  Type for PPP configuration buffer type used to pass Link status 
  determination configuration information between application context and 
  PPP context.
---------------------------------------------------------------------------*/
typedef struct
{
  ppp_dev_enum_type   device;
  ppp_lsd_config_type config;
} ppp_trigger_lsd_info_s_type;

/*---------------------------------------------------------------------------
  Type for PPP command buffer type used to pass configuration information 
  between application context and PPP context.
---------------------------------------------------------------------------*/
typedef union ppp_cmd_buf_u_type
{
  ppp_config_buffer_s_type    config;
  ppp_vendor_pkt_info_s_type  pkt_info;
  ppp_trigger_lsd_info_s_type lsd_config;
} ppp_cmd_buf_u_type;

#define ccDEL  0x7f

#ifdef FEATURE_STA_PPP
/*---------------------------------------------------------------------------
TYPEDEF PPPI_PARAM_ENUM_TYPE

DESCRIPTION
  Enum defining a subset of PPP constants that can acessed by the sta 
  framework
---------------------------------------------------------------------------*/
typedef enum
{
  PPPI_LCP_FSM_STATE                    = 0,
  PPPI_LCP_RX_IFACE_PTR                 = 1,
  PPPI_LCP_CONFIGURE_TRIES              = 2,
  PPPI_LCP_CONFIGURE_TRIES_REMAINING    = 3,
  PPPI_LCP_TERMINATE_TRIES              = 4,
  PPPI_LCP_NAK_TRIES                    = 5,
  PPPI_LCP_NAK_TRIES_REMAINING          = 6,
  PPPI_LCP_ACK_TIMEOUT                  = 7,
  PPPI_LCP_TERM_TIMEOUT                 = 8,
  PPPI_IPCP_FSM_STATE                   = 9,
  PPPI_IPCP_RX_IFACE_PTR                = 10,
  PPPI_IPCP_CONFIGURE_TRIES             = 11,
  PPPI_IPCP_CONFIGURE_TRIES_REMAINING   = 12,
  PPPI_IPCP_TERMINATE_TRIES             = 13,
  PPPI_IPCP_NAK_TRIES                   = 14,
  PPPI_IPCP_NAK_TRIES_REMAINING         = 15,
  PPPI_IPCP_ACK_TIMEOUT                 = 16,
  PPPI_IPCP_TERM_TIMEOUT                = 17,
#ifdef FEATURE_DATA_PS_IPV6
  PPPI_IPV6CP_FSM_STATE                 = 18,
  PPPI_IPV6CP_RX_IFACE_PTR              = 19,
  PPPI_IPV6CP_CONFIGURE_TRIES           = 20,
  PPPI_IPV6CP_CONFIGURE_TRIES_REMAINING = 21,
  PPPI_IPV6CP_TERMINATE_TRIES           = 22,
  PPPI_IPV6CP_NAK_TRIES                 = 23,
  PPPI_IPV6CP_NAK_TRIES_REMAINING       = 24,
  PPPI_IPV6CP_ACK_TIMEOUT               = 25,
  PPPI_IPV6CP_TERM_TIMEOUT              = 26,
#endif /* FEATURE_DATA_PS_IPV6 */
  PPPI_AUTH_PASSWD_ENCRYPT              = 27,
  PPPI_AUTH_TIMEOUT                     = 28,
  PPPI_AUTH_RETRY_COUNT                 = 29,
  PPPI_AUTH_RETRY_LIMIT                 = 30,

  PPPI_VSNCP_3GPP2_CONFIGURE_TRIES      = 31,
  PPPI_VSNCP_3GPP2_TERMINATE_TRIES      = 32,
  PPPI_VSNCP_3GPP2_ACK_TIMEOUT          = 33,
  PPPI_VSNCP_3GPP2_TERM_TIMEOUT         = 34,

  PPPI_MAX_PARAM                       /* max value - array sizing         */
} pppi_param_const_enum_type;
#endif /* FEATURE_STA_PPP */

/*---------------------------------------------------------------------------
  Flag to keep track if HDLC A2 Autonomous mode is ON/OFF
---------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_PS_HDLC_A2_AUTONOMOUS
extern boolean ppp_hdlc_a2_autonomous_enable;
#endif /* FEATURE_DATA_PS_HDLC_A2_AUTONOMOUS */

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION PPPI_POWERUP_INIT()

DESCRIPTION
  This function doe the power up initialization for PPP

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void pppi_powerup_init(void);

/*===========================================================================
FUNCTION PPPI_DEV_INIT()

DESCRIPTION
  Internal function to initialize a PPP device and copy any passed
  configuration

PARAMETERS
  ppp_cb_ptr   PPP device control block for which LCP needs to be
               initialized
  config_ptr   Any passed configuration

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  Initializes PPP device.
===========================================================================*/
void pppi_dev_init
(
  ppp_type          *ppp_cb_ptr,
  ppp_dev_opts_type *config_ptr
);

/*===========================================================================
FUNCTION PPPI_DEV_START()

DESCRIPTION
  This function is called to start a PPP device

PARAMETERS
  ppp_cb_ptr   PPP control block pointer

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  Enable PPP receive signal, start setup timer and if mode is ACTIVE call
  fsm_up on LCP
===========================================================================*/
void pppi_dev_start
(
  ppp_type *ppp_cb_ptr
);

/*===========================================================================
FUNCTION PPPI_DISCARD()

DESCRIPTION
  This function will queue a PPP Discard packet.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
int pppi_discard
(
  ppp_dev_enum_type device,
  dsm_item_type    *item_ptr
);

/*===========================================================================
FUNCTION PPPI_SKIPPED()

DESCRIPTION
  This function will Skip Incoming Packet routine

  NOTE: this funtion is only used internally to this file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void pppi_skipped
(
  ppp_dev_enum_type device,
  dsm_item_type **item_head_ptr
);

/*===========================================================================
FUNCTION PPPI_START()

DESCRIPTION
  This function will actively start PPP (though potentially in passive
  mode).

PARAMETERS
  start_info: parameters to use with start

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void pppi_start
(
  ppp_config_buffer_s_type *configbuf_ptr
);

/*===========================================================================
FUNCTION PPPI_RESYNC()

DESCRIPTION
  This function will resync a PPP device

PARAMETERS
  configbuf_ptr: configuration buffer containing configuration information

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  Resyncs PPP device
===========================================================================*/
void pppi_resync
(
  ppp_config_buffer_s_type *configbuf_ptr
);

/*===========================================================================
FUNCTION PPPI_STOP()

DESCRIPTION
  This function will initiate the closing of a PPP session.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void pppi_stop
(
  pppi_stop_info_type stop_info
);

/*===========================================================================
FUNCTION PPPI_ABORT()

DESCRIPTION
  This function will clean up after PPP is finished.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void pppi_abort
(
  pppi_abort_info_type abort_info
);

/*===========================================================================
FUNCTION PPPI_SET_MODE()

DESCRIPTION
  This function will change the PPP mode as specified.
  Will throw an error if:
  (1) Mode switch to PPP_PSEUDO_NET_MODE or PPP_FULL_NET_MODE is requested
      without a bridge device set.
  (2) Mode switch to PPP_RELAY_MODE is requested from a mode other than
      itself or PPP_NONE_MODE.
  (3) Mode switch to PPP_PSEUDO_NET_MODE is requested while running in HDLC
      hardware mode.
  (4) An invalid PPP mode switch is requested.

PARAMETERS
  mode_info: parameters required to set mode

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void pppi_set_mode
(
  pppi_mode_info_type mode_info
);

/*===========================================================================
FUNCTION PPPI_HDLC_UNFRAME_INIT() (was PPP_ASY_IN_INIT())

DESCRIPTION
  This function will free any packets that pppi_asy_in() had hanging around
  from the previous call.  Also clear out the various other params.

DEPENDENCIES
  Should only be called when entering Autodetect mode!

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void pppi_hdlc_unframe_init
(
  ppp_dev_enum_type device           /* ppp device pkt arrived on        */
);

/*===========================================================================
FUNCTION PPPI_RECEIVE()

DESCRIPTION
  This is the PPP packet receiving function in the stack. It processes each
  PPP packet based upon the protocol type and pass it up to the appropriate
  protocol engine in the stack.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void pppi_receive
(
  ppp_dev_enum_type device,           /* ppp device pkt arrived on         */
  uint16            protocol,               /* protocol of packet          */
  dsm_item_type     **item_ref_ptr,       /* Pointer to ref to pkt         */
  ps_rx_meta_info_type *meta_info_ptr    /* Pointer to PS meta info        */
);

/*===========================================================================
FUNCTION PPPI_IFACE_TX_FUNCTION()

DESCRIPTION
  This function is registered with the associated PS Iface when a PPP
  instance is up (this is done in ipcp_openeing()).  It is a ps_iface tx
  command.

PARAMETERS
  this_iface_ptr: pointer to the interface which is doing the TX
  pkt_chain_ptr: reference to a dsm item pointer: this is payload for PPP
  meta_info_ptr: pointer to a meta info item.
  tx_cmd_info: in the case of PPP this should point to the PPP instance this
               should be transmitted on.

RETURN VALUE
  Same as pppi_outgoing
   0 on success
  -1 on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int pppi_iface_tx_function
(
  ps_iface_type      *this_iface_ptr,
  dsm_item_type     **item_ref_ptr,
  ps_tx_meta_info_type  *meta_info_ptr,
  void               *tx_cmd_info
);

/*===========================================================================
FUNCTION PPPI_DEFAULT_RX()

DESCRIPTION
  This function will simply return NULL indicating to the caller that there
  was no data to receive.

PARAMETERS
  None

RETURN VALUE
  NULL

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
dsm_item_type *pppi_default_rx
(
  void *user_data
);

/*===========================================================================
FUNCTION PPPI_DEFAULT_TX()

DESCRIPTION
  This function will free any packet that is passed in.

PARAMETERS
  item_ref_ptr: pointer to a reference to a dsm item chain.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void pppi_default_tx
(
  dsm_item_type **item_ref_ptr,
  void           *meta_info_ptr,
  void *user_data
);

/*===========================================================================
FUNCTION PPPI_GET_DATA_CBACK()

DESCRIPTION
  This function is intended to be registered with the task that is providing
  the execution context for PPP.  This will call asy_in() if HDLC is enabled,
  otherwise we call pppi_incoming() directly.

PARAMETERS
  ppp_intance: the value of the PPP instance is expected to be stopred in the
  void*, that is this should be used as memory NOT as a pointer.

RETURN VALUE
  true:

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean pppi_get_data_cback
(
  ps_sig_enum_type sig,                    /* Signal to be processed       */
  void *dev_instance                       /* user data, indicates the ppp */
);


#ifdef FEATURE_DATA_PS_IPV6
/*===========================================================================
FUNCTION PPPI_FAILOVER_ON_RESYNC()

DESCRIPTION
  This function is called to handle failover on resyncs.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Performs the failover logic
===========================================================================*/
void pppi_failover_on_resync
(
  ppp_dev_enum_type   device,
  ppp_protocol_e_type protocol
);
#endif /*FEATURE_DATA_PS_IPV6*/

/*===========================================================================
FUNCTION PPPI_CALL_EVENT_CALLBACK()

DESCRIPTION
  Traverses the PPP event queue and calls the passed event on the
  passed protocol.  If the protocol is LCP and th event is DOWN or RESYNC
  then it also calls the callbacks on the NCPs.

  NOTE: This function TASKLOCKs inside and need not be called with TASKLOCK

  NOTE: The callback function should not dealloc the event notification
        structure

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Generates PPP event callbacks
===========================================================================*/
void pppi_call_event_callback
(
  ppp_fsm_type        *fsm_ptr,
  ppp_protocol_e_type protocol,
  ppp_event_e_type    event
);

#ifdef FEATURE_DATA_PS_PPP_ROHC
/*===========================================================================
FUNCTION PPPI_ROHC_ALLOC()

DESCRIPTION
  This function is called by an NCP to allocate ROHC.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Allocate / reconfigure the PPP ROHC instance
===========================================================================*/
void pppi_rohc_alloc
(
  ppp_dev_enum_type     device,
  int                   ncp,
  ppp_rohc_config_type *local_rohc_config,
  ppp_rohc_config_type *remote_rohc_config
);

/*===========================================================================
FUNCTION PPPI_ROHC_FREE()

DESCRIPTION
  This function is called by an NCP to free it's use of ROHC.  It checks to
  see if ROHC is in use by another NCP.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Can free the PPP ROHC instance if not in use
===========================================================================*/
void pppi_rohc_free
(
  ppp_dev_enum_type   device,
  ppp_protocol_e_type ncp
);
#endif /* FEATURE_DATA_PS_PPP_ROHC */

#ifdef FEATURE_DATA_PS_PPP_IPHC
/*===========================================================================
FUNCTION PPPI_IPHC_ALLOC()

DESCRIPTION
  This function is called by an NCP to allocate IPHC.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Allocate / reconfigure the PPP IPHC instance
===========================================================================*/
void pppi_iphc_alloc
(
  ppp_dev_enum_type     device,
  int                   ncp,
  ppp_iphc_config_type *local_config,
  ppp_iphc_config_type *remote_config
);

/*===========================================================================
FUNCTION PPPI_IPHC_FREE()

DESCRIPTION
  This function is called by an NCP to free it's use of IPHC.  It checks to
  see if IPHC is in use by another NCP.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Can free the PPP IPHC instance if not in use
===========================================================================*/
void pppi_iphc_free
(
  ppp_dev_enum_type   device,
  ppp_protocol_e_type ncp
);
#endif /* FEATURE_DATA_PS_PPP_IPHC */

#ifdef FEATURE_STA_PPP
/*===========================================================================
FUNCTION PPPI_GETCONST()
 
DESCRIPTION
 To get constants from the phone

PARAMETERS
 device : PPP device to get the constant from
 param  : the constant or enum to fetch

DEPENDENCIES
 none

RETURN VALUE
 integer value of the constant, -1 in case of an error.

SIDE EFFECTS
 none
===========================================================================*/
int pppi_getconst
(
  ppp_dev_enum_type device,
  pppi_param_const_enum_type param 
);
#endif /* FEATURE_STA_PPP */
#ifdef FEATURE_STA_PPP
/*===========================================================================
FUNCTION PPPI_SETCONST()
 
DESCRIPTION
 To set constants on the phone

PARAMETERS
 device : PPP device to set the constant for
 param  : the constant or enum to set
 value  : the value to set the constant to
 len    : the length of the value in bytes

DEPENDENCIES
 none

RETURN VALUE
 0 for success, -1 in case of an error.

SIDE EFFECTS
 none
===========================================================================*/
int pppi_setconst
(
  ppp_dev_enum_type device,
  pppi_param_const_enum_type param,
  void * value,
  uint32 len
);
#endif /* FEATURE_STA_PPP */

/*===========================================================================
FUNCTION PPPI_APPLY_CACHED_OPTS()
 
DESCRIPTION
  None

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
int pppi_apply_cached_opts
(
  ppp_dev_enum_type   device,
  ppp_dev_opts_type * config_ptr
);


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
void pppi_fsm_call_event_callback
(
  ppp_fsm_type          *fsm_ptr,          /* fsm ptr                     */
  uint16                protocol,         /* protocol of packet      */
  ppp_event_e_type      event,            /* event associated with cback */
  ppp_event_state_type  prev_state,       /* previous state of cb */
  ppp_event_state_type  state             /* current state of cb */
);

/*---------------------------------------------------------------------------
  PPP Control Block device constant definition

DESCRIPTION
  Definition of the cb_constant_s type structure common for LCP,
  IPCP and IPv6CP FSM structures.
  ---------------------------------------------------------------------------*/
extern struct cb_constant_s pppi_fsm_cb_default_constants;


/*===========================================================================
FUNCTION PS_PPPI_FRAG_TX_CMD()

DESCRIPTION
  This function transmits out the passed IP fragment, by calling
  pppi_outgoing

PARAMETERS
  device_ptr:    PPP device
  dsm_item_ptr:  Data Packet
  meta_info_ptr: Meta Info associated with the Packet

RETURN VALUE
 0 for success, -1 in case of an error.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_pppi_frag_tx_cmd
(
  void                 * device_ptr,
  dsm_item_type       ** dsm_item_ptr,
  ps_tx_meta_info_type * meta_info_ptr
);

/*===========================================================================
FUNCTION: PPP_REPORT_EVENT_IID

DESCRIPTION
  This reports the IPV6 IID event to the Diag.

PARAMETERS
  device: PPP device
  iid: The IPV6 iid in network byte order.

RETURN VALUE
  none
===========================================================================*/
void ppp_report_event_iid
(
  ppp_dev_enum_type device,
  uint64 iid 
);

/*===========================================================================
MACRO PS_EVENT_GET_AUTH_PROT_FRM_AUTH_MODE

DESCRIPTION
    Converts the passed in auth mode to the corresponding enum value.
===========================================================================*/
#define PS_EVENT_GET_AUTH_PROT_FRM_AUTH_MODE
INLINE ppp_event_auth_proto_type ps_event_get_auth_frm_auth_mode
(
  uint8 auth_mode
)
{
  switch(auth_mode)
  {
    case PPP_NO_AUTH:
      return PPP_EVENT_AUTH_NONE;

    case PPP_DO_PAP:
      return PPP_EVENT_AUTH_PAP;

    case PPP_DO_CHAP:
      return PPP_EVENT_AUTH_CHAP;    

    case PPP_DO_EAP:
      return PPP_EVENT_AUTH_EAP;
 
    default:
      return PPP_EVENT_AUTH_INVALID;
  }
}

/*===========================================================================
FUNCTION PPP_GET_DEVICE_CONFIG_PTR()

DESCRIPTION
  Access function for PPP device config cache.
 
PARAMETERS
  None
 
RETURN VALUE
  PPP device config cache.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
ppp_dev_config_type* ppp_get_device_config_ptr(uint8 device);

#ifdef FEATURE_DATA_PS_HDLC_A2_AUTONOMOUS
/*===========================================================================
FUNCTION PPPI_A2_ENABLE_HDLC_AUTONOMOUS

DESCRIPTION
  This FUNCTION enables/disables autonomous mode HDLC framing/deframing on 
  a given SIO port.

PARAMETERS
  port_id
  hdlc_accm
  enable

RETURN VALUE
  None

DEPENDENCIES
  
SIDE EFFECTS
  None
===========================================================================*/
void pppi_a2_enable_hdlc_autonomous
(
  uint32 hdlc_accm,
  boolean enable
);
#endif /* FEATURE_DATA_PS_HDLC_A2_AUTONOMOUS */

#ifdef FEATURE_DATA_PS_EHRPD
/*===========================================================================
FUNCTION PPPI_DEVICE_STOP()

DESCRIPTION
  This function will initiate the closing of a PPP session.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void pppi_device_stop
(
  ppp_dev_enum_type device
);

/*===========================================================================
FUNCTION PPPI_TRIGGER_LSD()

DESCRIPTION
  This function processes the PPP Trigger Link Status Determination command

PARAMETERS
  lsd_info_ptr   Pointer to PPP LSD config (allocated from PS mem)

RETURN VALUE
  None

DEPENDENCIES
  PPP device must already have been started

SIDE EFFECTS
===========================================================================*/
void pppi_trigger_lsd
(
  ppp_trigger_lsd_info_s_type * lsd_info_ptr
);
#endif /* FEATURE_DATA_PS_EHRPD */

/*===========================================================================
FUNCTION PPPI_NCP_MODE_CHANGE()

DESCRIPTION
  Figure out based on the passed current control protocol mode passed and
  new mode what mode should the protocol assume

  With old mode in the first row and new mode along the first column the
  mode returned by the function is given in the following table.  This
  table can also be implemented as a two dimensional array.

              INVALID    ACTIVE    PASSIVE    DISABLED   DISCARD

  INVALID     INVALID    ACTIVE    PASSIVE    DISABLED   DISCARD
  ACTIVE      ACTIVE     ACTIVE    ACTIVE     ACTIVE     ACTIVE
  PASSIVE     PASSIVE    ACTIVE    PASSIVE    PASSIVE    PASSIVE
  DISABLED    DISABLED   ACTIVE    PASSIVE    DISABLED   DISABLED
  DISCARD     DISCARD    ACTIVE    PASSIVE    DISCARD    DISCARD

PARAMETERS
  old_mode   Present protocol mode
  new_mode   New mode passed

RETURN VALUE
  the mode the control protocol should assume

DEPENDENCIES
  This should be called only for an NCP

SIDE EFFECTS
  None
===========================================================================*/

cp_mode_type pppi_ncp_mode_change
(
  cp_mode_type old_mode,
  cp_mode_type new_mode
);

#endif  /* PS_PPPI_H */

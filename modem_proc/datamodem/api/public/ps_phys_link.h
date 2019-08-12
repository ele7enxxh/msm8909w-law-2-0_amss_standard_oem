#ifndef PS_PHYS_LINK_H
#define PS_PHYS_LINK_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                             P S _ P H Y S _ L I N K . H

DESCRIPTION
  Header file defining all of the data types and the interface control block
  for the ps physical link architecture.

EXTERNAL FUNCTIONS

  PS_PHYS_LINK_CREATE()
    Used to create ps_phys_links.

  PS_PHYS_LINK_UP_CMD()
    Used to bring up the phys link.  Can be called by external entity.

  PS_PHYS_LINK_DOWN_CMD()
    Used to tear down the phys link.  Can be called by external entity.

  PS_PHYS_LINK_ENABLE_FLOW()
    Enable data flow on the phys link.  User passes in a mask indicating the
    flow control bit being enabled.

  PS_PHYS_LINK_DISABLE_FLOW()
    Disable data flow on the phys link.  User passes in a mask indicating the
    flow control bit being disabled.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None for the module.  Each phys_link is created by calling
  ps_phys_link_create().

Copyright (c) 2003-2009 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/ps_phys_link.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
09/15/11    msr    New API to post a cmd to PS task for flow control 
09/12/08    pp     Metainfo optimizations.
07/18/08    dm     Fixed compiler warnings
05/15/08    dm     Added support to pass the dormancy info code (reject cause)
                   in case of RAB reestablishment reject to applications
12/25/06    msr    Fixed broken secondary link logging
08/18/06    mjp    Fixed IPHC and ROHC Logging
07/17/06    mp     Fixed logging of zero length DPL packets
06/05/06    mp     Moved DPL link logging control block from ps_iface to
                   ps_phys_link
02/22/06    msr    Using single critical section
02/06/06    msr    Updated for L4 tasklock/crit sections.
12/09/05    sv     Added support for new data path framework.
10/18/05    msr    Removed support for FLOW_ACTIVATE_MODIFIED_EV and
                   PHYS_LINK_UP_MODIFIED_EV
08/15/05    msr    Moved DPL related stuff to ps_iface and ps_flow
08/15/05    kr     Added struct phys_link_i_stats to struct ps_phys_link_s
                   for stats update
05/23/05    spn    Changed signature of ps_phys_link_ioctl_f_ptr_type to
                   reflect change in signature of fn ds707_pkt_ioctl
05/12/05    ks     fixed lint errors
05/03/05    msr    Update some macros and added comments
04/22/05    ks     Removed ASSERT in SET and RESET_CAPABILITY macros.
04/17/05    msr    Moved support for QOS to ps_flow.
04/16/05    ks     Changes due to PHYS_LINK_NULL state
03/15/05    tmr    Changed embedded private_info_type of struct ps_phys_link_s
                   to be a data element, instead of a type and then a data
                   element declaration... necessary for RPC type generation
01/14/05    usb    Added macro to return QOS phys link from the meta info.
01/12/05    msr    Added core review comments.
01/10/05    msr    Added support for flow logging.
01/08/05    msr    Moved Rx IP filters to ps_mem and added
                   ps_phys_link_get_rx_filter_handle() and
                   ps_phys_link_get_rx_filter_by_handle().
10/25/04    msr    Removed graceful_dormant_close from ps_phys_link_type
                   structure and removed PS_PHYS_LINK_[GS]ET_GRACEFUL_CLOSE
                   macros.
10/31/04   msr/ks  Changed the variable name in struct dpl_link_dir_cb_type
                   from iid to dpl_id, for data protocol logging. Changed the
                   name of included file from ps_loggingi.h to
                   ps_logging_defs.h.
10/14/04   ks/msr  Added support for Data Protocol Logging.
10/06/04    sv     Added phys_link_down_ind_ex function.
03/22/04    ak     extern for dorm_Status().
02/13/04    ak     Moved sdb interface into this file from iface.
12/20/03    ak     Updated for phys_link/iface separation.
08/15/03    aku    created file.
===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "queue.h"
#include "ps_iface_defs.h"
#include "ps_phys_linki_utils.h"
#include "ps_phys_link_ioctl.h"
#include "ps_meta_info.h"
#include "ps_logging_defs.h"
#include "ps_ppp_ext.h"


/*---------------------------------------------------------------------------
  Higher layer protocol enum type
---------------------------------------------------------------------------*/
typedef enum
{
  PS_PHYS_LINK_HIGHER_LAYER_PROTOCOL_NONE = 0,
  PS_PHYS_LINK_HIGHER_LAYER_PROTOCOL_IPV4 = 1,
  PS_PHYS_LINK_HIGHER_LAYER_PROTOCOL_IPV6,
  PS_PHYS_LINK_HIGHER_LAYER_PROTOCOL_IP,
  PS_PHYS_LINK_HIGHER_LAYER_PROTOCOL_EAP,
  PS_PHYS_LINK_HIGHER_LAYER_PROTOCOL_ROHC,
  PS_PHYS_LINK_HIGHER_LAYER_PROTOCOL_IPHC,
  PS_PHYS_LINK_HIGHER_LAYER_PROTOCOL_MAX
} ps_phys_link_higher_layer_proto_enum_type;


typedef union
{
  struct
  {
    ps_iface_type * v4_iface_ptr;
    ps_iface_type * v6_iface_ptr;
  }ip_proto_handle;

  ps_iface_type * v4_iface_ptr;
  ps_iface_type * v6_iface_ptr;

  void          * eap_handle;
}ps_phys_link_higher_layer_proto_handle_type;

/*---------------------------------------------------------------------------
  Link protocol enum type
---------------------------------------------------------------------------*/
typedef enum
{
  PS_PHYS_LINK_LINK_PROTOCOL_NONE = 0,
  PS_PHYS_LINK_LINK_PROTOCOL_PPP,
  PS_PHYS_LINK_LINK_PROTOCOL_PDP,
  PS_PHYS_LINK_LINK_PROTOCOL_LLC,
  PS_PHYS_LINK_LINK_PROTOCOL_MAX
} ps_phys_link_link_protocol_enum_type;


/*---------------------------------------------------------------------------
  Link protocol handle type
---------------------------------------------------------------------------*/
typedef union
{
  ppp_dev_enum_type  ppp_device;
  void             * llc_handle;
  void             * pdp_handle;

  struct
  {
    ps_phys_link_higher_layer_proto_enum_type high_protocol;
    ps_phys_link_higher_layer_proto_handle_type handle;
  }none_handle;
} ps_phys_link_link_protocol_handle_type;

/*---------------------------------------------------------------------------
  HDR compression protocol enum type
---------------------------------------------------------------------------*/
typedef enum
{
  PS_PHYS_LINK_HDR_COMP_PROTOCOL_NONE = 0,
  PS_PHYS_LINK_HDR_COMP_PROTOCOL_ROHC,
  PS_PHYS_LINK_HDR_COMP_PROTOCOL_IPHC,
  PS_PHYS_LINK_HDR_COMP_PROTOCOL_VJ,
  PS_PHYS_LINK_HDR_COMP_PROTOCOL_MAX
} ps_phys_link_hdr_comp_protocol_enum_type;

/*---------------------------------------------------------------------------
  Framing protocol enum type
---------------------------------------------------------------------------*/
typedef enum
{
  PS_PHYS_LINK_FRAMING_PROTOCOL_NONE = 0,
  PS_PHYS_LINK_FRAMING_PROTOCOL_HDLC,
  PS_PHYS_LINK_FRAMING_PROTOCOL_MAX
} ps_phys_link_framing_protocol_enum_type;

/*===========================================================================
                               CMD F_PTR TYPES
===========================================================================*/
/*---------------------------------------------------------------------------
TYPEDEF PS_PHYS_LINK_CMD_F_PTR_TYPE

DESCRIPTION
  Definitition of the function for an interfaces command functions.

PARAMETERS
  phys_link_ptr:  Ptr to phys link control block on which to operate on.
  client_data:    pointer to any client data that needs to be passed in

RETURN VALUE
  None
---------------------------------------------------------------------------*/
typedef int (*ps_phys_link_cmd_f_ptr_type)
(
  ps_phys_link_type *phys_link_ptr,
  void              *client_data_ptr
);


/*===========================================================================
TYPEDEF PS_PHYS_LINK_IOCTL_F_PTR_TYPE()

DESCRIPTION
  This function performs various operations on the given ps interface.
  Typically, these operations are to get or set a value.

DEPENDENCIES
  None.

PARAMETERS

  phys_link_ptr*            - Pointer to phys link on which operation is
                              being performed

  ioctl_name                - The operation name

  argval_ptr                - Pointer to operation specific structure


  ps_errno                   - Error code returned in case of failure (Error
                              values are those defined in dserrno.h)

                              DS_EINVAL - Returned when the specified IOCTL
                              does not belong to the common set of IOCTLs
                              and there is no IOCTL mode handler registered
                              for the specified interface.

                              DS_EOPNOTSUPP - Returned by the lower level
                              IOCTL mode handler when specified IOCTL is not
                              supported by the interface. For instance, this
                              would be returned by interfaces that do not
                              support a certain "iface specific common IOCTL"
                              (i.e. these are common IOCTLs, but the
                              implementation is mode specific)

                              DS_EFAULT - This error code is returned if the
                              specified arguments for the IOCTL are correct
                              but an error is encountered while executing
                              the IOCTL.

                              DS_NOMEMORY - This error code is returned if we
                              run out of mempory buffers during execution.

RETURN VALUE
  0 - on success
  -1 - on failure

SIDE EFFECTS
  None.
===========================================================================*/
typedef int (*ps_phys_link_ioctl_f_ptr_type)
(
  ps_phys_link_type        *phys_link_ptr,
  ps_phys_link_ioctl_type  ioctl_name,
  void                     *argval_ptr,
  int16                    *ps_errno
);

/*---------------------------------------------------------------------------
TYPEDEF PS_PHYS_LINK_TX_CMD_F_PTR_TYPE

DESCRIPTION
  TX cmd function pointer declaration.

PARAMETERS
  phys_link_ptr:  Ptr to phys link control block on which to operate on.
  pkt_ref_ptr:    Pkt to be sent.
  meta_info_ptr:  Metainfo associated with the packet
  tx_cmd_info:    pointer to any client data that needs to be passed in

RETURN VALUE
  None
---------------------------------------------------------------------------*/

typedef void (*ps_phys_link_tx_cmd_f_ptr_type)
(
  ps_phys_link_type  * phys_link_ptr,
  dsm_item_type     ** pkt_ref_ptr,
  ps_tx_meta_info_type  * meta_info_ptr,
  void               * tx_cmd_info
);

/*---------------------------------------------------------------------------
TYPEDEF PS_PHYS_LINK_LINK_TX_F_PTR_TYPE

DESCRIPTION
  Link TX function pointer

PARAMETERS
  protocol_handle: handle to the link protocol
  pkt_ref_ptr:    Pkt to be sent.
  high_protocol: Higher layer protocol
  meta_info_ptr:  Metainfo associated with the packet

RETURN VALUE
  None
---------------------------------------------------------------------------*/

typedef int (*ps_phys_link_link_tx_f_ptr_type)
(
  void                                       * protocol_handle,
  dsm_item_type                             ** pkt_ref_ptr,
  ps_phys_link_higher_layer_proto_enum_type    high_protocol,
  ps_tx_meta_info_type                       * meta_info_ptr
);


/*---------------------------------------------------------------------------
TYPEDEF PS_PHYS_LINK_LINK_RX_F_PTR_TYPE

DESCRIPTION
  Link RX function pointer

PARAMETERS
  protocol_handle: handle to the link protocol
  pkt_ref_ptr:    Pkt to be sent.

RETURN VALUE
  None
---------------------------------------------------------------------------*/

typedef int (*ps_phys_link_link_rx_f_ptr_type)
(
  void               * protocol_handle,
  void               * framing_handle,
  dsm_item_type     ** pkt_ref_ptr
);


/*===========================================================================

                          EVENT RELATED STUFF

===========================================================================*/
/*---------------------------------------------------------------------------
TYPEDEF PS_PHYS_LINK_EVENT_CBACK_TYPE

DESCRIPTION
  Defines for the event callbacks. There can be any number of events per
  interface and the memory is owned by the clients.

PARAMS
  this_iface_ptr: Ptr to interface control block on which to operate on.
  event: the event being registered for
  event_info: information specific to the event: for details look at typedef
  user_data_ptr: data to pass back to the user

RETURN VALUE
  None
---------------------------------------------------------------------------*/
typedef void (*ps_phys_link_event_cback_type)
(
  ps_phys_link_type           *this_phys_link,
  ps_iface_event_enum_type     event,
  ps_iface_event_info_u_type   event_info,
  void                        *user_data_ptr
);

/*===========================================================================
TYPEDEF PS_PHYS_LINK_CAPABILITY_ENUM_TYPE

DESCRIPTION
  Lists various capabilities of a phys link. If a particular bit is ON, then
  phys link will have corresponding capability

    PS_PHYS_LINK_CAPABILITY_PRIMARY - if this bit is ON, phys link is a
      primary phys link
    PS_PHYS_LINK_CAPABILITY_FLOW_DECOUPLED - if this bit is ON, flows
      are handled by mode handler. If this bit is OFF, phys link state is
      translated directly to a flow's state

  NOTE: If more than PS_PHYS_LINK_CAPABILITY_MAX_ALLOWED capabilities
        are defined then one may have to change type definition of
        capability_mask in ps_phys_link control block
===========================================================================*/
typedef enum
{
  PS_PHYS_LINK_CAPABILITY_MIN            = 0x0,
  PS_PHYS_LINK_CAPABILITY_PRIMARY        = 0x0,
  PS_PHYS_LINK_CAPABILITY_FLOW_DECOUPLED = 0x1,
  PS_PHYS_LINK_CAPABILITY_MAX,
  PS_PHYS_LINK_CAPABILITY_MAX_ALLOWED    = 0x8
} ps_phys_link_capability_enum_type;

typedef uint8  ps_phys_link_capability_mask_type;

typedef struct phys_link_event_info_cache_s
{
  struct
  {
    phys_link_state_type               state;
    ps_extended_info_code_enum_type    info_code;
  } phys_link_event_info;

  struct
  {
    ps_phys_link_type                * pri_phys_link_ptr;
    ps_extended_info_code_enum_type    info_code;
  } pri_changed_info;

  uint32                               flow_mask;

} phys_link_event_info_cache_type;

/*===========================================================================
                           PHYSICAL LINK CONTROL BLOCK

TYPEDEF PS_PHYS_LINK_TYPE

DESCRIPTION
  The interface control block.
  Each interface has an instance of this data structure.
  Interface can be dynamically created and destroyed and can have
  multiple instances (PDP-IP-0, PDP-IP-1, ...)
  The output of each iface always goes to another iface. The IP stack is also
  initialized as an interface.
  ALL REGISTERED CMDs & INDICATIONs must be under TASKLOCKs & INTLOCKs.
===========================================================================*/
struct ps_phys_link_s
{
  /*-------------------------------------------------------------------------
    The private information that should only ever be changed through ps_iface
    functions.  The data in here should NOT changed by the clients hand.
  -------------------------------------------------------------------------*/
  struct private_info_type
  {
    struct ps_phys_link_s *this_phys_link; /* if NULL then block is invalid*/
    q_type                 event_q_array[MAX_EVENT_QUEUES]; /* event q's   */
    uint32                 ref_cnt;       /* num flows bound to this       */
    uint8                  instance;      /* Phys link instance #          */
    uint32                 tx_flow_mask;  /* if !=0, phys link is flow_ctl */
    phys_link_state_type   state;         /* physical link state           */

    ps_phys_link_capability_mask_type  capability_mask;
  } phys_private;

  dpl_link_cb_type  dpl_link_cb;

  /*-------------------------------------------------------------------------
    Link Protocol Parameters
  -------------------------------------------------------------------------*/
  struct
  {
    ps_phys_link_link_protocol_enum_type   link_protocol;
    void                                 * link_protocol_handle;
    ps_phys_link_link_tx_f_ptr_type        link_tx_f_ptr;
    ps_phys_link_link_rx_f_ptr_type        link_rx_f_ptr;
  } link_protocol_info;


  /*-------------------------------------------------------------------------
    TX info (Lower layer)
  -------------------------------------------------------------------------*/
  ps_phys_link_tx_cmd_f_ptr_type    tx_f_ptr;
  void                            * tx_cmd_info;

  /*-------------------------------------------------------------------------
    RX info (Higher layer)
  -------------------------------------------------------------------------*/
  ps_phys_link_higher_layer_proto_enum_type   higher_layer_protocol;
  ps_phys_link_higher_layer_proto_handle_type rx_cmd_info;

  /*-------------------------------------------------------------------------
    Framing info
  -------------------------------------------------------------------------*/
  ps_phys_link_framing_protocol_enum_type  framing_protocol;
  void                                    * framing_handle;

  /*-------------------------------------------------------------------------
    HDR compression info
  -------------------------------------------------------------------------*/
  ps_phys_link_hdr_comp_protocol_enum_type   hdr_comp_protocol;
  void                                     * hdr_comp_handle;

  /*-------------------------------------------------------------------------
    Phys link stats
  -------------------------------------------------------------------------*/
  struct phys_link_stats
  {
    uint32  pkts_tx;
    uint64  bytes_tx;
    uint32  pkts_rx;
    uint64  bytes_rx;
    uint32  queried;
  } phys_link_i_stats;

  /*-------------------------------------------------------------------------
    void pointer which the client can use to store whatever information they
    desire.
  -------------------------------------------------------------------------*/
  void                          *client_data_ptr;

  /*-------------------------------------------------------------------------
    Commands - called from PS->Interface client
  -------------------------------------------------------------------------*/
  ps_phys_link_cmd_f_ptr_type    phys_link_down_cmd_f_ptr;
  ps_phys_link_cmd_f_ptr_type    phys_link_up_cmd_f_ptr;
  ps_phys_link_cmd_f_ptr_type    phys_link_go_null_cmd_f_ptr;

  /*-------------------------------------------------------------------------
    Phys link ioctl function pointer.
  -------------------------------------------------------------------------*/
  ps_phys_link_ioctl_f_ptr_type      ioctl_f_ptr;

  /*-------------------------------------------------------------------------
    Dormancy code to indicate phys link is down
  -------------------------------------------------------------------------*/
  ps_extended_info_code_enum_type    dormancy_info_code;

  phys_link_event_info_cache_type    event_info_cache;

  /*-------------------------------------------------------------------------
    Bearer Id : This would be filled up by Modehandlers whenver they send
    our phys link up indication.
  -------------------------------------------------------------------------*/  
  uint8    bearer_id;

};


/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                              MACRO DEFINITIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
MACRO PS_PHYS_LINK_IS_VALID()

DESCRIPTION
  This macro returns a boolean indicating whether the iface pointer passed in
  is valid.

PARAMETERS
  phys_link_ptr : pointer to the phys link in question.

RETURN VALUE
  TRUE  : if phys link is valid
  FALSE : otherwise.
===========================================================================*/
#define PS_PHYS_LINK_IS_VALID( phys_link_ptr )                           \
  ((phys_link_ptr) != NULL &&                                            \
   (phys_link_ptr)->phys_private.this_phys_link == (phys_link_ptr))

/*===========================================================================
MACRO PS_PHYS_LINK_GET_STATE()

DESCRIPTION
  This macro returns the state of the phys link that was passed in.

PARAMETERS
  phys_link_ptr : pointer to the phys link whose state is requested.

RETURN VALUE
  the state of the passed in phys link.
===========================================================================*/
#define PS_PHYS_LINK_GET_STATE( phys_link_ptr )                          \
  (PS_PHYS_LINK_IS_VALID(phys_link_ptr)                                  \
     ? PS_PHYS_LINKI_GET_STATE(phys_link_ptr)                            \
     : PHYS_LINK_DOWN)

/*===========================================================================
MACRO PS_PHYS_LINK_GET_REF_CNT()

DESCRIPTION
  Returns the number of flows associated with a phys link.

PARAMETERS
  phys_link_ptr : pointer to phys link

RETURN VALUE
  Number of flows : on success
  0               : if phys link is invalid
===========================================================================*/
#define PS_PHYS_LINK_GET_REF_CNT(phys_link_ptr)                          \
  (PS_PHYS_LINK_IS_VALID(phys_link_ptr)                                  \
     ? PS_PHYS_LINKI_GET_REF_CNT(phys_link_ptr)                          \
     : 0)

/*===========================================================================
MACRO PS_PHYS_LINK_SET_CAPABILITY()

DESCRIPTION
  Sets a capability of a phys link.

  NOTE : External clients must not try to set PS_PHYS_LINK_CAPABILITY_PRIMARY.

  NOTE : Only one capability can be set each time.

PARAMETERS
  phys_link_ptr : pointer to phys link
  capability    : capability which needs to be set

RETURN VALUE
  None
===========================================================================*/
#define PS_PHYS_LINK_SET_CAPABILITY(phys_link_ptr, capability)           \
  if (PS_PHYS_LINK_IS_VALID(phys_link_ptr) &&                            \
      ((uint8)capability) < (uint8)PS_PHYS_LINK_CAPABILITY_MAX)          \
  {                                                                      \
    PS_PHYS_LINKI_SET_CAPABILITY(phys_link_ptr, (capability));           \
  }

/*===========================================================================
MACRO PS_PHYS_LINK_RESET_CAPABILITY()

DESCRIPTION
  Resets a capability of a phys link.

  NOTE : External clients must not try to reset
         PS_PHYS_LINK_CAPABILITY_PRIMARY.

  NOTE : Only one capability can be reset each time.

PARAMETERS
  phys_link_ptr : pointer to phys link
  capability    : capability which needs to be reset

RETURN VALUE
  None
===========================================================================*/
#define PS_PHYS_LINK_RESET_CAPABILITY(phys_link_ptr, capability)         \
  if (PS_PHYS_LINK_IS_VALID(phys_link_ptr) &&                            \
      ((uint8)capability) < (uint8)PS_PHYS_LINK_CAPABILITY_MAX)          \
  {                                                                      \
    PS_PHYS_LINKI_RESET_CAPABILITY(phys_link_ptr, capability);           \
  }

/*===========================================================================
MACRO PS_PHYS_LINK_GET_CAPABILITY()

DESCRIPTION
  Returns TRUE if a capability of a phys link is turned on.

  NOTE : Only one capability can be fetched each time.

PARAMETERS
  phys_link_ptr : pointer to phys link
  capability    : capability which needs to be tested

RETURN VALUE
  TRUE  : if capability is turned on
  FALSE : otherwise
===========================================================================*/
#define PS_PHYS_LINK_GET_CAPABILITY(phys_link_ptr, capability)           \
  /*lint -save -e506 */                                                  \
  ((PS_PHYS_LINK_IS_VALID(phys_link_ptr) &&                              \
      ((uint8)capability) < (uint8)PS_PHYS_LINK_CAPABILITY_MAX)          \
       ? PS_PHYS_LINKI_GET_CAPABILITY(phys_link_ptr, capability)         \
       : FALSE)
  /*lint -restore */

/*===========================================================================
MACRO PS_PHYS_LINK_FLOW_ENABLED()

DESCRIPTION
  This macro returns whether the phys link is flow enabled.

PARAMETERS
  phys_link_ptr : pointer to the phys link whose state is requested.

RETURN VALUE
  TRUE  : if flow is enabled
  FALSE : otherwise
===========================================================================*/
#define PS_PHYS_LINK_FLOW_ENABLED(phys_link_ptr)                         \
  (PS_PHYS_LINK_IS_VALID(phys_link_ptr)                                  \
     ? PS_PHYS_LINKI_FLOW_ENABLED(phys_link_ptr)                         \
     : FALSE)

/*===========================================================================
MACRO PS_PHYS_LINK_GET_FLOW_MASK()

DESCRIPTION
  This macro returns the flow mask of the passed in phys link.

PARAMETERS
  phys_link_ptr : pointer to the phys link whose flow mask is requested.

RETURN VALUE
  The flow mask of the interface in question

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define ps_phys_link_get_flow_mask( phys_link_ptr )                      \
  (PS_PHYS_LINK_IS_VALID(phys_link_ptr)                                  \
     ? (phys_link_ptr)->phys_private.tx_flow_mask                        \
     : 0)


/*===========================================================================
MACRO PS_PHYS_LINK_GET_BEARER_ID()

DESCRIPTION
  Returns the bearer id associated with a phys link.

PARAMETERS
  phys_link_ptr : pointer to phys link

RETURN VALUE
  bearer id : on success
  -1               : if phys link is invalid
===========================================================================*/
#define PS_PHYS_LINK_GET_BEARER_ID(phys_link_ptr)                        \
  (PS_PHYS_LINK_IS_VALID(phys_link_ptr)                                  \
     ? (phys_link_ptr)->bearer_id                                        \
     : -1)

/*===========================================================================
MACRO PS_PHYS_LINK_SET_BEARER_ID()

DESCRIPTION
  Sets the bearer id associated with a phys link.

PARAMETERS
  phys_link_ptr : pointer to phys link
  bearer_id     : Bearer Id
RETURN VALUE
  None
===========================================================================*/
#define PS_PHYS_LINK_SET_BEARER_ID(phys_link_ptr, mi_bearer_id)             \
  ((phys_link_ptr)->bearer_id = mi_bearer_id)


/*===========================================================================
MACRO PS_PHYS_LINK_RESET_BEARER_ID()

DESCRIPTION
  Resets the bearer id associated with a phys link to value -1.

PARAMETERS
  phys_link_ptr : pointer to phys link

RETURN VALUE
  None
===========================================================================*/
#define PS_PHYS_LINK_RESET_BEARER_ID(phys_link_ptr)                    \
  ((phys_link_ptr)->bearer_id = -1)
  
#ifdef __cplusplus
extern "C" {
#endif
/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

                        PUBLIC FUNCTION DECLARATIONS

= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/
/*===========================================================================
FUNCTION PS_PHYS_LINK_CREATE()

DESCRIPTION
  This will memset()s the private data struct to 0, initializes the
  "this_phys_link_ptr" to point to the interface control block itself,
  and initializes the queues.

PARAMETERS
  this_phys_link_ptr: ptr to interface control block on which to operate on.
  n_links: Number of phys links to create.

RETURN VALUE
  -1 - error in iface creation
   0 - on success

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_phys_link_create
(
  ps_phys_link_type      *this_phys_link_ptr,
  uint8                   n_links
);

/*===========================================================================
FUNCTION PS_PHYS_LINK_UP_CMD()

DESCRIPTION
  Brings up the physical link

PARAMETERS
  this_phys_link_ptr: ptr to interface control block on which to operate on.

  THIS FUNCTION SHOULD NOT BE CALLED IN AN ISR.

RETURN VALUE
   0: on success
  -1: on failure (which includes DS_EWOULDBLOCK)

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_phys_link_up_cmd
(
  ps_phys_link_type *this_phys_link_ptr,
  int16             *ps_errno,
  void              *client_data_ptr
);

/*===========================================================================
FUNCTION PS_PHYS_LINK_DOWN_CMD()

DESCRIPTION
  This function is used to make the physical link go dormant.

PARAMETERS
  this_phys_link_ptr: ptr to phys link to tear down
  ps_errno: error code to return to caller.
  client_data_ptr: data, that is passed to the client handler function.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_phys_link_down_cmd
(
  ps_phys_link_type   *this_phys_link_ptr,
  int16               *ps_errno,
  void                *client_data_ptr
);

/*===========================================================================
FUNCTION PS_PHYS_LINK_GO_NULL_CMD()

DESCRIPTION
  This function is used to tear down the physical link.

PARAMETERS
  this_phys_link_ptr: ptr to phys link to tear down
  ps_errno: error code to return to caller.
  client_data_ptr: data, that is passed to the client handler function.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_phys_link_go_null_cmd
(
  ps_phys_link_type   *this_phys_link_ptr,
  int16               *ps_errno,
  void                *client_data_ptr
);

/*===========================================================================
FUNCTION PS_PHYS_LINK_ENABLE_FLOW()

DESCRIPTION
  Interface user enables flow on the interface.

PARAMETERS
  this_iface_ptr: Ptr to interface control block on which to operate on.
  flow_mask: bit mask that identifies the caller.

  THIS FUNCTION SHOULD NOT BE CALLED IN AN ISR.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_phys_link_enable_flow
(
  ps_phys_link_type *phys_link_ptr,
  uint32             flow_mask
);

/*===========================================================================
FUNCTION PS_PHYS_LINK_DISABLE_FLOW()

DESCRIPTION
  client disables flow on the interface.

  THIS FUNCTION SHOULD NOT BE CALLED IN AN ISR.

PARAMETERS
  this_iface_ptr: Ptr to interface control block on which to operate on.
  flow_mask: bit mask that identifies the caller.

RETURN VALUE
  None

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
void ps_phys_link_disable_flow
(
  ps_phys_link_type *phys_link_ptr,
  uint32             flow_mask
);

/*===========================================================================
FUNCTION PS_PHYS_LINK_ENABLE_FLOW_EX()

DESCRIPTION
  Posts a cmd to PS task disable flow on the phys link

  THIS FUNCTION SHOULD NOT BE CALLED IN AN ISR.

PARAMETERS
  phys_link_ptr: Ptr to phys link on which to operate on.
  flow_mask: bit mask that identifies the caller.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_phys_link_enable_flow_ex
(
  ps_phys_link_type  * phys_link_ptr,
  uint32               flow_mask
);

/*===========================================================================
FUNCTION PS_PHYS_LINK_DISABLE_FLOW_EX()

DESCRIPTION
  Posts a cmd to PS task disable flow on the phys link

  THIS FUNCTION SHOULD NOT BE CALLED IN AN ISR.

PARAMETERS
  phys_link_ptr: Ptr to phys link on which to operate on.
  flow_mask: bit mask that identifies the caller.

RETURN VALUE
  None

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
void ps_phys_link_disable_flow_ex
(
  ps_phys_link_type  * phys_link_ptr,
  uint32               flow_mask
);

/*===========================================================================
FUNCTION PS_PHYS_LINK_ALLOC_EVENT_CBACK_BUF()

DESCRIPTION
  This function is used to allocate the memory used to register for phys_link
  event callbacks.

PARAMETERS
  event_cback_f_ptr: the callback to be registered
  user_data_ptr: data to be called with the callback.

RETURN VALUE
  NULL: if allocation failed
  ptr to memory being allocated.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void *ps_phys_link_alloc_event_cback_buf
(
  ps_phys_link_event_cback_type   event_cback_f_ptr,
  void                           *user_data_ptr
);

/*===========================================================================
FUNCTION PS_PHYS_LINK_FREE_EVENT_CBACK_BUF()

DESCRIPTION
  This function is used to free the memory used to register for phys_link
  event callbacks.

PARAMETERS
  buf: the previously allocated buffer.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_phys_link_free_event_cback_buf
(
  void *buf
);

/*===========================================================================
FUNCTION PS_PHYS_LINK_EVENT_CBACK_REG()

DESCRIPTION
  Registers callbacks for events on a phys link.  Whenever an event occurs,
  the callback for that event is called.  If the event is true when
  registration happens the callback is called immediately.  This will fail if
  either the phys link is invalid, or the buffer to queue (with callback and
  user data pointer) is NULL.  A callback will only be queued if the event
  has not previously been queued.

  Memory is owned by the caller and it is put into the queue for that
  event.

PARAMETERS
  this_phys_link_ptr: ptr to phys link control block on which to operate on.
  event:  Event that triggers the callbacks
  buf:    Buffer that holds the callback pointer. Should not be NULL.

RETURN VALUE
  0: on success
 -1: on failure

DEPENDENCIES
  The link field must have been initialized with q_link().

SIDE EFFECTS
  None
===========================================================================*/
int ps_phys_link_event_cback_reg
(
  ps_phys_link_type          *this_phys_link_ptr,
  ps_iface_event_enum_type    event,
  void                       *buf
);

/*===========================================================================
FUNCTION PS_PHYS_LINK_EVENT_CBACK_DEREG()

DESCRIPTION
  DeRegisters callbacks for a specific event on an phys link.

PARAMETERS
  this_phys_link_ptr: ptr to phys link control block on which to operate on.
  event:  Event that triggers the callbacks
  buf:    Buffer that holds the callback pointer. This buffer is removed
          from the queue of callbacks. Should not be NULL.

RETURN VALUE
  None

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
void ps_phys_link_event_cback_dereg
(
  ps_phys_link_type          *this_phys_link_ptr,
  ps_iface_event_enum_type    event,
  void                       *buf
);

/*===========================================================================
FUNCTION PS_PHYS_LINK_DOWN_IND()

DESCRIPTION
  The physical link of the given interface is down (707 dormancy).  Call
  callbacks.

  THIS FUNCTION SHOULD NOT BE CALLED IN A ISR.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.

RETURN VALUE
  None

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
void ps_phys_link_down_ind
(
  ps_phys_link_type *this_phys_link_ptr
);

/*===========================================================================
FUNCTION PS_PHYS_LINK_UP_IND()

DESCRIPTION
  The physical link of the given interface is up.  Call callbacks.

  THIS FUNCTION SHOULD NOT BE CALLED IN A ISR.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.

RETURN VALUE
  None

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
void ps_phys_link_up_ind
(
  ps_phys_link_type *this_phys_link_ptr
);

/*===========================================================================
FUNCTION PS_PHYS_LINK_GONE_IND()

DESCRIPTION
  The physical link of the given interface has been aborted, indicating
  that the session has gone NULL.  This is called in lieu of
  phys link_down_ind().

  THIS FUNCTION SHOULD NOT BE CALLED IN A ISR.

PARAMETERS
  this_phys_link_ptr: ptr to interface control block on which to operate
  on.

RETURN VALUE
  None

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
void ps_phys_link_gone_ind
(
  ps_phys_link_type *this_phys_link_ptr
);

/*===========================================================================
FUNCTION PS_PHYS_LINK_DOS_ACK_IND()

DESCRIPTION
  Indicates that a packet sent over an access channel is acknowledged by the
  network

  THIS FUNCTION SHOULD NOT BE CALLED IN A ISR.

PARAMETERS
  phys_link_ptr           : ptr to a phys link
  dos_ack_handle          : handle to the DOS packet
  dos_ack_status_info_ptr : status of DOS transmission

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_phys_link_dos_ack_ind
(
  ps_phys_link_type                      * phys_link_ptr,
  int32                                    dos_ack_handle,
  ps_phys_link_dos_ack_status_info_type  * dos_ack_status_info_ptr
);

/*===========================================================================
FUNCTION PS_PHYS_LINK_DOWN_IND_EX()

DESCRIPTION
  The physical link of the given interface is down (707 dormancy).  Call
  callbacks.

  THIS FUNCTION SHOULD NOT BE CALLED IN A ISR.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.

RETURN VALUE
  None

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
void ps_phys_link_down_ind_ex
(
  ps_phys_link_type *this_phys_link_ptr,
  ps_extended_info_code_enum_type info_code
);

/*===========================================================================
FUNCTION PS_PHYS_LINK_UP_IND_EX()

DESCRIPTION
  The physical link of the given interface is up.  Call callbacks.

  THIS FUNCTION SHOULD NOT BE CALLED IN A ISR.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.
  info_code:  Extended info code for the event.

RETURN VALUE
  None

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
void ps_phys_link_up_ind_ex
(
  ps_phys_link_type *this_phys_link_ptr,
  ps_extended_info_code_enum_type info_code
);

/*===========================================================================
FUNCTION PS_PHYS_LINK_GONE_IND_EX()

DESCRIPTION
  The physical link of the given interface is up.  Call callbacks.

  THIS FUNCTION SHOULD NOT BE CALLED IN A ISR.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.
  info_code:  Extended info code for the event.

RETURN VALUE
  None

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
void ps_phys_link_gone_ind_ex
(
  ps_phys_link_type *this_phys_link_ptr,
  ps_extended_info_code_enum_type info_code
);


/*===========================================================================
FUNCTION PS_PHYS_LINK_SET_LINK_PROTOCOL

DESCRIPTION
  Set the link protocol.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.
  protocol:  Link protocol
  handle :  link protocol handle.

RETURN VALUE
  None

DEPENDENCIES

SIDE EFFECTS
  Set the link protocol
===========================================================================*/

int ps_phys_link_set_link_protocol
(
  ps_phys_link_type *this_phys_link_ptr,
  ps_phys_link_link_protocol_enum_type protocol,
  ps_phys_link_link_protocol_handle_type handle
);


/*===========================================================================
FUNCTION PS_PHYS_LINK_SET_HDR_COMP_HANDLE

DESCRIPTION
  Set the header compression handle

PARAMETERS
  this_phys_link_ptr: pointer to physlink
  protocol:  Header compression protocol
  handle :  Header compression protocol handle.

RETURN VALUE
  None

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/

int ps_phys_link_set_hdr_comp_handle
(
  ps_phys_link_type *this_phys_link_ptr,
  ps_phys_link_hdr_comp_protocol_enum_type protocol,
  void * hdr_comp_handle
);


/*===========================================================================
FUNCTION PS_PHYS_LINK_SET_FRAMING_HANDLE

DESCRIPTION
  Set the framing compression handle

PARAMETERS
  this_phys_link_ptr: pointer to physlink
  protocol:  Framing/unframing protocol
  handle :  Framing/unframing protocol handle.

RETURN VALUE
  None

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/

int ps_phys_link_set_framing_handle
(
  ps_phys_link_type *this_phys_link_ptr,
  ps_phys_link_framing_protocol_enum_type protocol,
  void * hdr_comp_handle
);



/*===========================================================================
FUNCTION PS_IFACE_SET_TX_FUNCTION()

DESCRIPTION
  This function is used to set the TX function.  Unsetting the function is
  done by passing in NULL for the tx_cmd parameter.

PARAMETERS
  this_iface_ptr:  interface on which to operate
  tx_cmd:          Tx command being set
  tx_cmd_info:     Info to pass to registered cmd handler

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_phys_link_set_tx_function
(
  ps_phys_link_type            *this_phys_link_ptr,
  ps_phys_link_tx_cmd_f_ptr_type tx_cmd,
  void                          *tx_cmd_info
);


/*===========================================================================
FUNCTION PS_PHYS_LINK_INPUT()

DESCRIPTION


PARAMETERS
  this_phys_link_ptr:ptr to physlink control block on which to operate on.
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
int ps_phys_link_input
(
  const ps_phys_link_type  * this_phys_link_ptr,
  dsm_item_type           ** pkt_ref_ptr,
  ps_rx_meta_info_type     * meta_info_ptr
);

#ifdef __cplusplus
}
#endif

#endif /* PS_PHYS_LINK_H */

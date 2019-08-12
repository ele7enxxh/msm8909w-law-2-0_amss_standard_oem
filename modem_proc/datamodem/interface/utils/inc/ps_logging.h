#ifndef PS_LOGGING_H
#define PS_LOGGING_H
/*===========================================================================

                        P S _ L O G G I N G . H

GENERAL DESCRIPTION
  This file defines external API used by various protocols to log their
  packets

EXTERNAL FUNCTIONS
  DPL_LOG_NETWORK_TX_PACKET()
    Used to log a network packet on Tx side

  DPL_LOG_NETWORK_RX_PACKET()
    Used to log a network packet on Rx side

  DPL_LOG_FLOW_TX_PACKET()
    Used to log a flow packet on Tx side

  DPL_LOG_LINK_FULL_TX_PACKET()
    Used to log a full link level packet on Tx side

  DPL_LOG_LINK_FULL_RX_PACKET()
    Used to log a full link level packet on Rx side

Copyright (c) 2004-2009 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

 $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/utils/inc/ps_logging.h#1 $
 $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
12/23/09    cp     Lint fixes.
08/31/06    mjp    Added IPHC and ROHC Logging
06/05/06    mp     Moved DPL link logging control block from ps_iface to
                   ps_phys_link and added DPL PPP Partial Logging
08/15/05    msr    Removed FEATURE_DATA_PS_DATA_LOGGING_FLOW and using
                   ps_flow instead of ps_phys_link for flow logging
01/12/05    msr    Added code review comments.
01/10/05    msr    Added support for flow and link logging.
10/31/04   msr/ks  Changed the inline functions to macros.
09/24/04   ks/msr  Created module.
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "customer.h"
#include "comdef.h"
#include "customer.h"

#include "dsm.h"

#include "ps_logging_helper.h"



/*===========================================================================

                                 EXTERNAL FUNCTIONS

===========================================================================*/
/*===========================================================================
MACRO   DPL_LOG_NETWORK_TX_PACKET

DESCRIPTION
  Log a network protocol packet going on tx side

PARAMETERS
  ps_iface_ptr : pointer to ps_iface
  tx_packet    : packet to be logged
  protocol     : network protocol of packet to be logged

RETURN VALUE
   None
===========================================================================*/
#define DPL_LOG_NETWORK_TX_PACKET(ps_iface_ptr, tx_packet, protocol)     \
  DPLI_LOG_NETWORK_PACKET((ps_iface_ptr)->dpl_net_cb.tx_cb,              \
                          (ps_iface_ptr)->dpl_net_cb.tx_dpl_id,          \
                          tx_packet,                                     \
                          protocol)

/*===========================================================================
MACRO   DPL_LOG_NETWORK_RX_PACKET

DESCRIPTION
  Log a network protocol packet going on rx side

PARAMETERS
  ps_iface_ptr : pointer to ps_iface
  rx_packet    : packet to be logged
  protocol     : network protocol of packet to be logged

RETURN VALUE
  None
===========================================================================*/
#define DPL_LOG_NETWORK_RX_PACKET(ps_iface_ptr, rx_packet, protocol)     \
  DPLI_LOG_NETWORK_PACKET((ps_iface_ptr)->dpl_net_cb.recv_cb,            \
                          (ps_iface_ptr)->dpl_net_cb.recv_dpl_id,        \
                          rx_packet,                                     \
                          protocol)

/*===========================================================================
MACRO   DPL_GET_NETWORK_RX_SNAPLEN

DESCRIPTION
  Returns length of packet logged for RX direction

PARAMETERS
  ps_iface_ptr      : pointer to iface
  protocol          : network level protocol of packet to be logged
  
RETURN VALUE
   None
===========================================================================*/
#define DPL_GET_NETWORK_RX_SNAPLEN(ps_iface_ptr, protocol)          \
  DPLI_LOG_GET_PACKET_SNAPLEN((ps_iface_ptr)->dpl_net_cb.recv_cb,   \
                              protocol)


/*===========================================================================
MACRO   DPL_GET_NETWORK_TX_SNAPLEN

DESCRIPTION
  Returns length of packet logged for TX direction

PARAMETERS
  ps_iface_ptr      : pointer to iface
  protocol          : network level protocol of packet to be logged
  
RETURN VALUE
   None
===========================================================================*/
#define DPL_GET_NETWORK_TX_SNAPLEN(ps_iface_ptr, protocol)            \
  DPLI_LOG_GET_PACKET_SNAPLEN((ps_iface_ptr)->dpl_net_cb.tx_cb,       \
                              protocol)

/*===========================================================================
MACRO   DPL_LOG_FLOW_TX_PACKET

DESCRIPTION
  Log a network protocol packet going on tx side at flow level

PARAMETERS
  flow_ptr  : pointer to ps_flow
  tx_packet : packet to be logged
  protocol  : network protocol of packet to be logged

RETURN VALUE
   None
===========================================================================*/
#define DPL_LOG_FLOW_TX_PACKET(flow_ptr, tx_packet, protocol)            \
  DPLI_LOG_NETWORK_PACKET((flow_ptr)->dpl_flow_cb.tx_cb,                 \
                          (flow_ptr)->dpl_flow_cb.tx_dpl_id,             \
                          tx_packet,                                     \
                          protocol)

/*===========================================================================
MACRO   DPL_LOG_LINK_FULL_TX_PACKET

DESCRIPTION
  Log a full link level protocol packet going on tx side

PARAMETERS
  ps_phys_link_ptr : pointer to ps_phys_link
  tx_packet        : packet to be logged
  protocol         : link level protocol of packet to be logged

RETURN VALUE
   None
===========================================================================*/
#define DPL_LOG_LINK_FULL_TX_PACKET( ps_phys_link_ptr, tx_packet, protocol) \
  DPL_LOG_LINK_FULL_TX_PACKET_BY_CB( &((ps_phys_link_ptr)->dpl_link_cb),    \
                                     tx_packet,                             \
                                     protocol)

/*===========================================================================
MACRO   DPL_LOG_LINK_FULL_RX_PACKET

DESCRIPTION
  Log a full link level protocol packet going on rx side

PARAMETERS
  ps_phys_link_ptr : pointer to ps_phys_link
  rx_packet        : packet to be logged
  protocol         : link level protocol of packet to be logged

RETURN VALUE
   None
===========================================================================*/
#define DPL_LOG_LINK_FULL_RX_PACKET( ps_phys_link_ptr, rx_packet, protocol) \
  DPL_LOG_LINK_FULL_RX_PACKET_BY_CB( &((ps_phys_link_ptr)->dpl_link_cb),    \
                                     rx_packet,                             \
                                     protocol)

/*===========================================================================
MACRO   DPL_LOG_LINK_FULL_TX_COMP_PACKET

DESCRIPTION
  Log a full link level Compressed protocol packet going on tx side

PARAMETERS
  dpl_link_cb_ptr : pointer to the DPL control block
  tx_packet       : packet to be logged
  protocol        : link level protocol of packet to be logged

RETURN VALUE
   None
===========================================================================*/
#define DPL_LOG_LINK_FULL_TX_COMP_PACKET(dpl_link_cb_ptr,                \
                                         tx_packet,                      \
                                         protocol)                       \
  DPLI_LOG_LINK_FULL_COMP_PACKET((dpl_link_cb_ptr)->tx_cb,               \
                                 (dpl_link_cb_ptr)->tx_dpl_id,           \
                                 tx_packet,                              \
                                 (uint32)protocol)

/*===========================================================================
MACRO   DPL_LOG_LINK_FULL_RX_COMP_PACKET

DESCRIPTION
  Log a full link level Compressed protocol packet going on rx side

PARAMETERS
  dpl_link_cb_ptr : pointer to the DPL control block
  rx_packet       : packet to be logged
  protocol        : link level protocol of packet to be logged

RETURN VALUE
   None
===========================================================================*/
#define DPL_LOG_LINK_FULL_RX_COMP_PACKET(dpl_link_cb_ptr,                \
                                         rx_packet,                      \
                                         protocol)                       \
  DPLI_LOG_LINK_FULL_COMP_PACKET((dpl_link_cb_ptr)->recv_cb,             \
                                 (dpl_link_cb_ptr)->recv_dpl_id,         \
                                 rx_packet,                              \
                                 (uint32)protocol)

/*===========================================================================
MACRO   DPL_LOG_LINK_FULL_TX_PACKET_BY_CB

DESCRIPTION
  Log a full link level protocol packet going on tx side

PARAMETERS
  dpl_link_cb_ptr : pointer to the DPL control block
  tx_packet       : packet to be logged
  protocol        : link level protocol of packet to be logged

RETURN VALUE
   None
===========================================================================*/
#define DPL_LOG_LINK_FULL_TX_PACKET_BY_CB(dpl_link_cb_ptr,               \
                                          tx_packet,                     \
                                          protocol)                      \
  DPLI_LOG_LINK_FULL_PACKET((dpl_link_cb_ptr)->tx_cb,                    \
                            (dpl_link_cb_ptr)->tx_dpl_id,                \
                            tx_packet,                                   \
                            protocol)

/*===========================================================================
MACRO   DPL_LOG_LINK_FULL_RX_PACKET_BY_CB

DESCRIPTION
  Log a full link level protocol packet going on rx side

PARAMETERS
  dpl_link_cb_ptr : pointer to the DPL control block
  rx_packet       : packet to be logged
  protocol        : link level protocol of packet to be logged

RETURN VALUE
   None
===========================================================================*/
#define DPL_LOG_LINK_FULL_RX_PACKET_BY_CB(dpl_link_cb_ptr,               \
                                          rx_packet,                     \
                                          protocol)                      \
  DPLI_LOG_LINK_FULL_PACKET((dpl_link_cb_ptr)->recv_cb,                  \
                            (dpl_link_cb_ptr)->recv_dpl_id,              \
                            rx_packet,                                   \
                            protocol)

/*===========================================================================
MACRO   DPL_LOG_PPP_PARTIAL_TX_PACKET

DESCRIPTION
  Partially log a ppp protocol packet going on tx side

PARAMETERS
  ps_iface_ptr : pointer to ps_iface
  tx_packet    : packet to be logged

RETURN VALUE
   None
===========================================================================*/
#define DPL_LOG_PPP_PARTIAL_TX_PACKET(ps_phys_link_ptr, tx_packet)       \
  DPLI_LOG_PPP_PARTIAL_PACKET((ps_phys_link_ptr)->dpl_link_cb.tx_cb,     \
                              (ps_phys_link_ptr)->dpl_link_cb.tx_dpl_id, \
                              tx_packet)

/*===========================================================================
MACRO   DPL_LOG_PPP_PARTIAL_RX_PACKET

DESCRIPTION
  Partially log a ppp protocol packet going on rx side

PARAMETERS
  ps_iface_ptr : pointer to ps_iface
  rx_packet    : packet to be logged

RETURN VALUE
   None
===========================================================================*/
#define DPL_LOG_PPP_PARTIAL_RX_PACKET(ps_phys_link_ptr, rx_packet)          \
  DPLI_LOG_PPP_PARTIAL_PACKET((ps_phys_link_ptr)->dpl_link_cb.recv_cb,      \
                              (ps_phys_link_ptr)->dpl_link_cb.recv_dpl_id,  \
                              rx_packet)

#endif /* PS_LOGGING_H */

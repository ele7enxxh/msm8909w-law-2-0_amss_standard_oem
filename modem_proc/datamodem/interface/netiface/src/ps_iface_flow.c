/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                         P S _ I F A C E _ F L O W . C


GENERAL DESCRIPTION
  This is the implementation of the ps_flow specific PS interface
  "member functions".

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2006-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/netiface/src/ps_iface_flow.c#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
12/23/13    gk     Made sure that tmp_qos_spec is NULL initialised to avoid 
                   invalid mem free afterwards.
09/09/09    ss     Critical section released in error cases.
11/14/08    sp     Log packets should be generated during flow
                   creation/deletion, only if the phys_link_ptr is present.
04/23/08    pp     Meta info optimizations.
05/29/08    ssh    Memory leak in ps_iface_create_logical_flows()
09/01/07    ssh    QoS support for logical ifaces
6/28/07    msr/ssh QoS supportfor M.IP CCoA iface (currently featurized)
09/18/06    hm     Changed direct anding of iface_id to get flow instance to
                   use macro PS_IFACE_GET_FLOW_INST_FROM_ID.
07/31/06    msr    Added support for PRIMARY_QOS_MODIFY
05/03/06    msr    Created file.
===========================================================================*/
/*===========================================================================

                                INCLUDE FILES

===========================================================================*/
#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"

#include "msg.h"
#include "amssassert.h"

#include "dserrno.h"
#include "ps_iface.h"
#include "ps_ifacei_utils.h"
#include "ps_flow.h"
#include "ps_utils.h"
#include "ps_mem.h"
#include "ps_metai_info.h"
#include "ds_Utils_DebugMsg.h"
#include "ps_iface_logical_flowi.h"

#ifdef FEATURE_DATA_PS_QOS
#include "ps_flowi.h"
#include "ps_phys_link.h"
#include "ps_iface_rx_qos_fltr.h"
#include "ps_qos_spec_logging.h"
#endif /* FEATURE_DATA_PS_QOS */

#include "ps_stat_logging.h"

#include <stringl/stringl.h>


/*===========================================================================

                             LOCAL DATA DEFINITIONS

===========================================================================*/
#ifdef FEATURE_DATA_PS_QOS
/*---------------------------------------------------------------------------
  Tuning the number of ps flow rx filter buffers needed by this module
---------------------------------------------------------------------------*/
#define PS_FLOW_RX_FLTR_BUF_SIZE  ((sizeof(ps_flow_rx_fltr_buf_type) + 3) & ~3)

#ifdef FEATURE_DATA_PS_LOW_MEM_CHIPSET
  #define PS_FLOW_RX_FLTR_BUF_NUM       40
  #define PS_FLOW_RX_FLTR_BUF_HIGH_WM   35
  #define PS_FLOW_RX_FLTR_BUF_LOW_WM     5

#else
  #define PS_FLOW_RX_FLTR_BUF_NUM       200
  #define PS_FLOW_RX_FLTR_BUF_HIGH_WM   195
  #define PS_FLOW_RX_FLTR_BUF_LOW_WM      5

#endif /* FEATURE_DATA_PS_LOW_MEM_CHIPSET */
/*----------------------------------------------------------------------------
  Allocate memory to hold ps_flow_rx_fltr along with ps_mem header
----------------------------------------------------------------------------*/
static int ps_flow_rx_fltr_buf_mem[PS_MEM_GET_TOT_SIZE_OPT
                                   (
                                     PS_FLOW_RX_FLTR_BUF_NUM,
                                     PS_FLOW_RX_FLTR_BUF_SIZE
                                   )];

#ifdef FEATURE_DATA_PS_MEM_DEBUG
/*----------------------------------------------------------------------------
  Array of pointers used to facilitate easy debugging.  The first one points
  to the ps_mem header and the latter ponts to ps_flow_rx_fltr_buf
----------------------------------------------------------------------------*/
static ps_mem_buf_hdr_type       * ps_flow_rx_fltr_buf_hdr[PS_FLOW_RX_FLTR_BUF_NUM];
static ps_flow_rx_fltr_buf_type  * ps_flow_rx_fltr_buf_ptr[PS_FLOW_RX_FLTR_BUF_NUM];
#endif /* FEATURE_DATA_PS_MEM_DEBUG */

/*---------------------------------------------------------------------------
  Tuning the number of ps iface rx qos filter node buffers needed by this
  module
---------------------------------------------------------------------------*/
#define PS_IFACE_RX_QOS_FLTR_NODE_BUF_SIZE  ((sizeof(ps_iface_rx_qos_fltr_node_type) + 3) & ~3)

#ifdef FEATURE_DATA_PS_LOW_MEM_CHIPSET
  #define PS_IFACE_RX_QOS_FLTR_NODE_BUF_NUM       40
  #define PS_IFACE_RX_QOS_FLTR_NODE_BUF_HIGH_WM   35
  #define PS_IFACE_RX_QOS_FLTR_NODE_BUF_LOW_WM     5

#else
  #define PS_IFACE_RX_QOS_FLTR_NODE_BUF_NUM       200
  #define PS_IFACE_RX_QOS_FLTR_NODE_BUF_HIGH_WM   195
  #define PS_IFACE_RX_QOS_FLTR_NODE_BUF_LOW_WM      5

#endif
/*----------------------------------------------------------------------------
  Allocate memory to hold ps_iface_rx_qos_fltr_node along with ps_mem header
----------------------------------------------------------------------------*/
static int ps_iface_rx_qos_fltr_node_buf_mem[PS_MEM_GET_TOT_SIZE_OPT
                                             (
                                               PS_IFACE_RX_QOS_FLTR_NODE_BUF_NUM,
                                               PS_IFACE_RX_QOS_FLTR_NODE_BUF_SIZE
                                             )];

#ifdef FEATURE_DATA_PS_MEM_DEBUG
/*----------------------------------------------------------------------------
  Array of pointers used to facilitate easy debugging.  The first one points
  to the ps_mem header and the latter ponts to ps_iface_rx_qos_fltr_node_buf
----------------------------------------------------------------------------*/
static ps_mem_buf_hdr_type             * ps_iface_rx_qos_fltr_node_buf_hdr[PS_IFACE_RX_QOS_FLTR_NODE_BUF_NUM];
static ps_iface_rx_qos_fltr_node_type  * ps_iface_rx_qos_fltr_node_buf_ptr[PS_IFACE_RX_QOS_FLTR_NODE_BUF_NUM];
#endif /* FEATURE_DATA_PS_MEM_DEBUG */

/*---------------------------------------------------------------------------
  Tuning the number of qos info buffers needed by this module
---------------------------------------------------------------------------*/
#define QOS_INFO_BUF_SIZE  ((sizeof(qos_info_type) + 3) & ~3)

#define QOS_INFO_BUF_NUM        PS_FLOW_MAX_SEC_PS_FLOWS
#define QOS_INFO_BUF_HIGH_WM    35
#define QOS_INFO_BUF_LOW_WM      5

/*----------------------------------------------------------------------------
  Allocate memory to hold qos_info along with ps_mem header
----------------------------------------------------------------------------*/
static int qos_info_buf_mem[PS_MEM_GET_TOT_SIZE_OPT(QOS_INFO_BUF_NUM, QOS_INFO_BUF_SIZE)];

#ifdef FEATURE_DATA_PS_MEM_DEBUG
/*----------------------------------------------------------------------------
  Array of pointers used to facilitate easy debugging.  The first one points
  to the ps_mem header and the latter ponts to qos_info_buf
----------------------------------------------------------------------------*/
static ps_mem_buf_hdr_type * qos_info_buf_hdr[QOS_INFO_BUF_NUM];
static qos_info_type       * qos_info_buf_ptr[QOS_INFO_BUF_NUM];
#endif /* FEATURE_DATA_PS_MEM_DEBUG */

/*---------------------------------------------------------------------------
  Tuning the number of ps flow ip flow spec buffers needed by this module
---------------------------------------------------------------------------*/
#define PS_FLOW_IP_FLOW_SPEC_BUF_SIZE  ((sizeof(ps_flow_ip_flow_spec_type) + 3) & ~3)

#define PS_FLOW_IP_FLOW_SPEC_BUF_NUM        160
#define PS_FLOW_IP_FLOW_SPEC_BUF_HIGH_WM    155
#define PS_FLOW_IP_FLOW_SPEC_BUF_LOW_WM       5

/*----------------------------------------------------------------------------
  Allocate memory to hold ps_flow_ip_flow_spec along with ps_mem header
----------------------------------------------------------------------------*/
static int ps_flow_ip_flow_buf_mem[PS_MEM_GET_TOT_SIZE_OPT
                                   (
                                     PS_FLOW_IP_FLOW_SPEC_BUF_NUM,
                                     PS_FLOW_IP_FLOW_SPEC_BUF_SIZE
                                   )];

#ifdef FEATURE_DATA_PS_MEM_DEBUG
/*----------------------------------------------------------------------------
  Array of pointers used to facilitate easy debugging.  The first one points
  to the ps_mem header and the latter points to ps_flow_ip_flow_spec_buf
----------------------------------------------------------------------------*/
static ps_mem_buf_hdr_type       * ps_flow_ip_flow_buf_hdr[PS_FLOW_IP_FLOW_SPEC_BUF_NUM];
static ps_flow_ip_flow_spec_type * ps_flow_ip_flow_buf_ptr[PS_FLOW_IP_FLOW_SPEC_BUF_NUM];
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
#endif /* FEATURE_DATA_PS_QOS */



/*===========================================================================

                             EXTERNAL FUNCTIONS

===========================================================================*/
#ifdef FEATURE_DATA_PS_QOS
/*===========================================================================
FUNCTION PS_IFACE_FLOW_INIT()

DESCRIPTION
  Initializes ps_iface_flow module

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_flow_init
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Initialize Pools
  -------------------------------------------------------------------------*/
  if (PS_MEM_POOL_INIT_OPT(PS_MEM_RX_QOS_FLTR_BUF_TYPE,
                           ps_flow_rx_fltr_buf_mem,
                           PS_FLOW_RX_FLTR_BUF_SIZE,
                           PS_FLOW_RX_FLTR_BUF_NUM,
                           PS_FLOW_RX_FLTR_BUF_HIGH_WM,
                           PS_FLOW_RX_FLTR_BUF_LOW_WM,
                           NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                           (int *) ps_flow_rx_fltr_buf_hdr,
                           (int *) ps_flow_rx_fltr_buf_ptr
#else
                           NULL,
                           NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                          ) == -1)
  {
    LOG_MSG_ERROR_0("ps_iface_flow_init(): "
                    "Can't init the module");
  }

  if (PS_MEM_POOL_INIT_OPT(PS_MEM_IFACE_RX_QOS_FLTR_NODE_TYPE,
                           ps_iface_rx_qos_fltr_node_buf_mem,
                           PS_IFACE_RX_QOS_FLTR_NODE_BUF_SIZE,
                           PS_IFACE_RX_QOS_FLTR_NODE_BUF_NUM,
                           PS_IFACE_RX_QOS_FLTR_NODE_BUF_HIGH_WM,
                           PS_IFACE_RX_QOS_FLTR_NODE_BUF_LOW_WM,
                           NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                           (int *) ps_iface_rx_qos_fltr_node_buf_hdr,
                           (int *) ps_iface_rx_qos_fltr_node_buf_ptr
#else
                           NULL,
                           NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                          ) == -1)
  {
    LOG_MSG_ERROR_0("ps_iface_flow_init(): "
                    "Can't init the module");
  }

  if (PS_MEM_POOL_INIT_OPT(PS_MEM_QOS_INFO_TYPE,
                           qos_info_buf_mem,
                           QOS_INFO_BUF_SIZE,
                           QOS_INFO_BUF_NUM,
                           QOS_INFO_BUF_HIGH_WM,
                           QOS_INFO_BUF_LOW_WM,
                           NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                           (int *) qos_info_buf_hdr,
                           (int *) qos_info_buf_ptr
#else
                           NULL,
                           NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                          ) == -1)
     {
       LOG_MSG_ERROR_0("ps_iface_flow_init(): "
                       "Can't init the module");
     }

  if (PS_MEM_POOL_INIT_OPT(PS_MEM_PS_FLOW_IP_FLOW_SPEC_TYPE,
                           ps_flow_ip_flow_buf_mem,
                           PS_FLOW_IP_FLOW_SPEC_BUF_SIZE,
                           PS_FLOW_IP_FLOW_SPEC_BUF_NUM,
                           PS_FLOW_IP_FLOW_SPEC_BUF_HIGH_WM,
                           PS_FLOW_IP_FLOW_SPEC_BUF_LOW_WM,
                           NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                           (int *) ps_flow_ip_flow_buf_hdr,
                           (int *) ps_flow_ip_flow_buf_ptr
#else
                           NULL,
                           NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                          ) == -1)
  {
    LOG_MSG_ERROR_0("ps_iface_flow_init(): "
                    "Can't init the module");
  }
} /* ps_iface_flow_init() */

/*===========================================================================
FUNCTION ps_iface_log_qos_spec_filter_params

DESCRIPTION
This function prints the filter specifications

PARAMETERS
fltr_buf_ptr   - Pointer to PS QOS spec

DEPENDENCIES

RETURN VALUE
None.

SIDE EFFECTS
None.

===========================================================================*/
void ps_iface_log_qos_spec_filter_params
(
 const ip_filter_type    *fltr_buf_ptr        /* Pointer to PS QOS spec   */
)
{
  if (NULL == fltr_buf_ptr)
  {
    return;
  }

  LOG_MSG_INFO1_1( "ps_iface_log_qos_spec_filter_params(): "
                   "ip version: %d", fltr_buf_ptr->ip_vsn );

  if( IP_V4 == fltr_buf_ptr->ip_vsn )
  {
    LOG_MSG_INFO1_1( "ps_iface_log_qos_spec_filter_params(): "
                     "filter param mask: 0x%x",
                     (int)fltr_buf_ptr->ip_hdr.v4.field_mask );	
    if (IPFLTR_MASK_IP4_SRC_ADDR & fltr_buf_ptr->ip_hdr.v4.field_mask)
    {
      LOG_MSG_INFO1_1( "ps_iface_log_qos_spec_filter_params(): "
                       "src ip addr: %u",
                       ps_ntohl(fltr_buf_ptr->ip_hdr.v4.src.addr.ps_s_addr) );

      LOG_MSG_INFO1_1( "ps_iface_log_qos_spec_filter_params(): "
                       "src subnet mask: %u",
                       ps_ntohl(fltr_buf_ptr->ip_hdr.v4.src.subnet_mask.ps_s_addr) );
    }

    if (IPFLTR_MASK_IP4_DST_ADDR & fltr_buf_ptr->ip_hdr.v4.field_mask)
    {
      LOG_MSG_INFO1_1( "ps_iface_log_qos_spec_filter_params(): "
                       "dest ip addr: %u",
                       ps_ntohl(fltr_buf_ptr->ip_hdr.v4.dst.addr.ps_s_addr) );

      LOG_MSG_INFO1_1( "ps_iface_log_qos_spec_filter_params(): "
                       "dest subnet mask: %u",
                       ps_ntohl(fltr_buf_ptr->ip_hdr.v4.dst.subnet_mask.ps_s_addr) );
    }

    if (IPFLTR_MASK_IP4_TOS & fltr_buf_ptr->ip_hdr.v4.field_mask)
    {
      LOG_MSG_INFO1_1( "ps_iface_log_qos_spec_filter_params(): "
                       "tos value: %d", (int)fltr_buf_ptr->ip_hdr.v4.tos.val );

      LOG_MSG_INFO1_1( "ps_iface_log_qos_spec_filter_params(): "
                       "tos mask: %d", (int)fltr_buf_ptr->ip_hdr.v4.tos.mask );
    }

    if (IPFLTR_MASK_IP4_NEXT_HDR_PROT & fltr_buf_ptr->ip_hdr.v4.field_mask)
    {
      LOG_MSG_INFO1_1( "ps_iface_log_qos_spec_filter_params(): "
                       "transport protocol: %d",
                       (int)fltr_buf_ptr->ip_hdr.v4.next_hdr_prot );

      if ( PS_IPPROTO_TCP == fltr_buf_ptr->ip_hdr.v4.next_hdr_prot)
      {
        if (IPFLTR_MASK_TCP_SRC_PORT & fltr_buf_ptr->next_prot_hdr.tcp.field_mask)
        {
          LOG_MSG_INFO1_1( "ps_iface_log_qos_spec_filter_params(): "
                           "tcp src start port: %d",
                           (int)ps_ntohs(fltr_buf_ptr->next_prot_hdr.tcp.src.port) );

          LOG_MSG_INFO1_1( "ps_iface_log_qos_spec_filter_params(): "
                           "tcp src port range: %d",
                           (int)fltr_buf_ptr->next_prot_hdr.tcp.src.range );
        }

        if (IPFLTR_MASK_TCP_DST_PORT &  fltr_buf_ptr->next_prot_hdr.tcp.field_mask)
        {
          LOG_MSG_INFO1_1( "ps_iface_log_qos_spec_filter_params(): "
                           "tcp dest start port: %d",
                           (int)ps_ntohs(fltr_buf_ptr->next_prot_hdr.tcp.dst.port) );

          LOG_MSG_INFO1_1( "ps_iface_log_qos_spec_filter_params(): "
                           "tcp dest port range: %d",
                           (int)fltr_buf_ptr->next_prot_hdr.tcp.dst.range );
        }
      }

      if ( PS_IPPROTO_UDP == fltr_buf_ptr->ip_hdr.v4.next_hdr_prot)
      {
        if (IPFLTR_MASK_UDP_SRC_PORT & fltr_buf_ptr->next_prot_hdr.udp.field_mask)
        {
          LOG_MSG_INFO1_1( "ps_iface_log_qos_spec_filter_params(): "
                           "udp src start port: %d",
                           (int)ps_ntohs(fltr_buf_ptr->next_prot_hdr.udp.src.port) );

          LOG_MSG_INFO1_1( "ps_iface_log_qos_spec_filter_params(): "
                           "udp src port range: %d",
                           (int)fltr_buf_ptr->next_prot_hdr.udp.src.range );
        }

        if (IPFLTR_MASK_UDP_DST_PORT &  fltr_buf_ptr->next_prot_hdr.udp.field_mask)
        {
          LOG_MSG_INFO1_1( "ps_iface_log_qos_spec_filter_params(): "
                           "udp dest start port: %d",
                           (int)ps_ntohs(fltr_buf_ptr->next_prot_hdr.udp.dst.port) );

          LOG_MSG_INFO1_1( "ps_iface_log_qos_spec_filter_params(): "
                           "udp dest port range: %d",
                           (int)fltr_buf_ptr->next_prot_hdr.udp.dst.range );
        }
      }

      if ( PS_IPPROTO_TCP_UDP == fltr_buf_ptr->ip_hdr.v4.next_hdr_prot)
      {
        if (IPFLTR_MASK_TCP_UDP_SRC_PORT & fltr_buf_ptr->next_prot_hdr.tcp_udp_port_range.field_mask)
        {
          LOG_MSG_INFO1_1( "ps_iface_log_qos_spec_filter_params(): "
                           "tcp+udp src start port: %d",
                           (int)ps_ntohs(fltr_buf_ptr->next_prot_hdr.tcp_udp_port_range.src.port) );

          LOG_MSG_INFO1_1( "ps_iface_log_qos_spec_filter_params(): "
                           "tcp+udp src port range: %d",
                           (int)fltr_buf_ptr->next_prot_hdr.tcp_udp_port_range.src.range );
        }

        if (IPFLTR_MASK_TCP_UDP_DST_PORT & fltr_buf_ptr->next_prot_hdr.tcp_udp_port_range.field_mask)
        {
          LOG_MSG_INFO1_1( "ps_iface_log_qos_spec_filter_params(): "
                           "tcp+udp dest start port: %d",
                           (int)ps_ntohs(fltr_buf_ptr->next_prot_hdr.tcp_udp_port_range.dst.port) );

          LOG_MSG_INFO1_1( "ps_iface_log_qos_spec_filter_params(): "
                           "tcp+udp dest port range: %d",
                           (int)fltr_buf_ptr->next_prot_hdr.tcp_udp_port_range.dst.range );
        }
      }
	  
	  if ( PS_IPPROTO_ESP == fltr_buf_ptr->ip_hdr.v4.next_hdr_prot)
	  {
	    if (IPFLTR_MASK_ESP_SPI & fltr_buf_ptr->next_prot_hdr.esp.field_mask)
        {
          LOG_MSG_INFO1_1( "ps_iface_log_qos_spec_filter_params(): "
                           "esp spi: 0x%x",
                           fltr_buf_ptr->next_prot_hdr.esp.spi );
        }
      }
      if ( PS_IPPROTO_AH == fltr_buf_ptr->ip_hdr.v4.next_hdr_prot)
      {
        if (IPFLTR_MASK_AH_SPI & fltr_buf_ptr->next_prot_hdr.ah.field_mask)
        {
          LOG_MSG_INFO1_1( "ps_iface_log_qos_spec_filter_params(): "
                           "ah spi: 0x%x",
                           fltr_buf_ptr->next_prot_hdr.ah.spi );
        }
      } 
    }
  }
  else if( IP_V6 == fltr_buf_ptr->ip_vsn ) 
  {
    LOG_MSG_INFO1_1( "ps_iface_log_qos_spec_filter_params(): "
                     "filter param mask: 0x%x",
                     (int)fltr_buf_ptr->ip_hdr.v6.field_mask );

    if (IPFLTR_MASK_IP6_SRC_ADDR & fltr_buf_ptr->ip_hdr.v6.field_mask)
    {
      LOG_MSG_INFO1_1( "ps_iface_log_qos_spec_filter_params(): "
                       "src ipv6 addr[0]: %u",
                       ((unsigned int*)&fltr_buf_ptr->ip_hdr.v6.src.addr)[0] );
      LOG_MSG_INFO1_1( "ps_iface_log_qos_spec_filter_params(): "
                       "src ipv6 addr[1]: %u",
                       ((unsigned int*)&fltr_buf_ptr->ip_hdr.v6.src.addr)[1] );
      LOG_MSG_INFO1_1( "ps_iface_log_qos_spec_filter_params(): "
                       "src ipv6 addr[2]: %u",
                       ((unsigned int*)&fltr_buf_ptr->ip_hdr.v6.src.addr)[2] );
      LOG_MSG_INFO1_1( "ps_iface_log_qos_spec_filter_params(): "
                       "src ipv6 addr[3]: %u",
                       ((unsigned int*)&fltr_buf_ptr->ip_hdr.v6.src.addr)[3] );

      LOG_MSG_INFO1_1( "ps_iface_log_qos_spec_filter_params(): "
                       "src ipv6 prefix len: %d",
                       (int)fltr_buf_ptr->ip_hdr.v6.src.prefix_len );
    }

    if (IPFLTR_MASK_IP6_DST_ADDR & fltr_buf_ptr->ip_hdr.v6.field_mask)
    {
      LOG_MSG_INFO1_1( "ps_iface_log_qos_spec_filter_params(): "
                       "dest ipv6 addr[0]: %u",
                       ((unsigned int*)&fltr_buf_ptr->ip_hdr.v6.dst.addr)[0] );
      LOG_MSG_INFO1_1( "ps_iface_log_qos_spec_filter_params(): "
                       "dest ipv6 addr[1]: %u",
                       ((unsigned int*)&fltr_buf_ptr->ip_hdr.v6.dst.addr)[1] );
      LOG_MSG_INFO1_1( "ps_iface_log_qos_spec_filter_params(): "
                       "dest ipv6 addr[2]: %u",
                       ((unsigned int*)&fltr_buf_ptr->ip_hdr.v6.dst.addr)[2] );
      LOG_MSG_INFO1_1( "ps_iface_log_qos_spec_filter_params(): "
                       "dest ipv6 addr[3]: %u",
                       ((unsigned int*)&fltr_buf_ptr->ip_hdr.v6.dst.addr)[3] );

      LOG_MSG_INFO1_1( "ps_iface_log_qos_spec_filter_params(): "
                       "dest ipv6 prefix len: %d",
                       (int)fltr_buf_ptr->ip_hdr.v6.dst.prefix_len );
    }

    if (IPFLTR_MASK_IP6_TRAFFIC_CLASS & fltr_buf_ptr->ip_hdr.v6.field_mask)
    {
      LOG_MSG_INFO1_1( "ps_iface_log_qos_spec_filter_params(): "
                       "trafic class value: %d",
                       (int)fltr_buf_ptr->ip_hdr.v6.trf_cls.val );
      LOG_MSG_INFO1_1( "ps_iface_log_qos_spec_filter_params(): "
                       "trafic class mask: %d",
                       (int)fltr_buf_ptr->ip_hdr.v6.trf_cls.mask );
    }

    if (IPFLTR_MASK_IP6_FLOW_LABEL & fltr_buf_ptr->ip_hdr.v6.field_mask)
    {
      LOG_MSG_INFO1_1( "ps_iface_log_qos_spec_filter_params(): "
                       "flow label: %u", fltr_buf_ptr->ip_hdr.v6.flow_label );
    }

    if (IPFLTR_MASK_IP6_NEXT_HDR_PROT & fltr_buf_ptr->ip_hdr.v6.field_mask)
    {
      LOG_MSG_INFO1_1( "ps_iface_log_qos_spec_filter_params(): "
                       "transport protocol: %d",
                       (int)fltr_buf_ptr->ip_hdr.v6.next_hdr_prot );

      if ( PS_IPPROTO_TCP == fltr_buf_ptr->ip_hdr.v6.next_hdr_prot)
      {
        if (IPFLTR_MASK_TCP_SRC_PORT & fltr_buf_ptr->next_prot_hdr.tcp.field_mask)
        {
          LOG_MSG_INFO1_1( "ps_iface_log_qos_spec_filter_params(): "
                           "tcp src start port: %d",
                           (int)ps_ntohs(fltr_buf_ptr->next_prot_hdr.tcp.src.port) );

          LOG_MSG_INFO1_1( "ps_iface_log_qos_spec_filter_params(): "
                           "tcp src port range: %d",
                           (int)fltr_buf_ptr->next_prot_hdr.tcp.src.range );
        }

        if (IPFLTR_MASK_TCP_DST_PORT &  fltr_buf_ptr->next_prot_hdr.tcp.field_mask)
        {
          LOG_MSG_INFO1_1( "ps_iface_log_qos_spec_filter_params(): "
                           "tcp dest start port: %d",
                           (int)ps_ntohs(fltr_buf_ptr->next_prot_hdr.tcp.dst.port) );

          LOG_MSG_INFO1_1( "ps_iface_log_qos_spec_filter_params(): "
                           "tcp dest port range: %d",
                           (int)fltr_buf_ptr->next_prot_hdr.tcp.dst.range );
        }
      }

      if ( PS_IPPROTO_UDP == fltr_buf_ptr->ip_hdr.v6.next_hdr_prot)
      {
        if (IPFLTR_MASK_UDP_SRC_PORT & fltr_buf_ptr->next_prot_hdr.udp.field_mask)
        {
          LOG_MSG_INFO1_1( "ps_iface_log_qos_spec_filter_params(): "
                           "udp src start port: %d",
                           (int)ps_ntohs(fltr_buf_ptr->next_prot_hdr.udp.src.port) );

          LOG_MSG_INFO1_1( "ps_iface_log_qos_spec_filter_params(): "
                           "udp src port range: %d",
                           (int)fltr_buf_ptr->next_prot_hdr.udp.src.range );
        }

        if (IPFLTR_MASK_UDP_DST_PORT &  fltr_buf_ptr->next_prot_hdr.udp.field_mask)
        {
          LOG_MSG_INFO1_1( "ps_iface_log_qos_spec_filter_params(): "
                           "udp dest start port: %d",
                           (int)ps_ntohs(fltr_buf_ptr->next_prot_hdr.udp.dst.port) );

          LOG_MSG_INFO1_1( "ps_iface_log_qos_spec_filter_params(): "
                           "udp dest port range: %d",
                           (int)fltr_buf_ptr->next_prot_hdr.udp.dst.range );
        }
      }

      if ( PS_IPPROTO_TCP_UDP == fltr_buf_ptr->ip_hdr.v6.next_hdr_prot)
      {
        if (IPFLTR_MASK_TCP_UDP_SRC_PORT & fltr_buf_ptr->next_prot_hdr.tcp_udp_port_range.field_mask)
        {
          LOG_MSG_INFO1_1( "ps_iface_log_qos_spec_filter_params(): "
                           "tcp+udp src start port: %d",
                           (int)ps_ntohs(fltr_buf_ptr->next_prot_hdr.tcp_udp_port_range.src.port) );

          LOG_MSG_INFO1_1( "ps_iface_log_qos_spec_filter_params(): "
                           "tcp+udp src port range: %d",
                           (int)fltr_buf_ptr->next_prot_hdr.tcp_udp_port_range.src.range );
        }

        if (IPFLTR_MASK_TCP_UDP_DST_PORT & fltr_buf_ptr->next_prot_hdr.tcp_udp_port_range.field_mask)
        {
          LOG_MSG_INFO1_1( "ps_iface_log_qos_spec_filter_params(): "
                           "tcp+udp dest start port: %d",
                           (int)ps_ntohs(fltr_buf_ptr->next_prot_hdr.tcp_udp_port_range.dst.port) );

          LOG_MSG_INFO1_1( "ps_iface_log_qos_spec_filter_params(): "
                           "tcp+udp dest port range: %d",
                           (int)fltr_buf_ptr->next_prot_hdr.tcp_udp_port_range.dst.range );
        }
      }

      if ( PS_IPPROTO_ESP == fltr_buf_ptr->ip_hdr.v6.next_hdr_prot)
      {
        if (IPFLTR_MASK_ESP_SPI & fltr_buf_ptr->next_prot_hdr.esp.field_mask)
        {
          LOG_MSG_INFO1_1( "ps_iface_log_qos_spec_filter_params(): "
                     "esp spi: 0x%x", fltr_buf_ptr->next_prot_hdr.esp.spi );
        }
      }  
      if ( PS_IPPROTO_AH == fltr_buf_ptr->ip_hdr.v6.next_hdr_prot)
      {
        if (IPFLTR_MASK_AH_SPI & fltr_buf_ptr->next_prot_hdr.ah.field_mask)
        {
          LOG_MSG_INFO1_1( "ps_iface_log_qos_spec_filter_params(): "
                     "AH spi: 0x%x", fltr_buf_ptr->next_prot_hdr.ah.spi );
        }
      }
      
    }
  }

  
} /* ps_iface_log_qos_spec_filter_params */

/*===========================================================================
FUNCTION ps_iface_log_qos_spec_params

DESCRIPTION
This function printf the qos spec params

PARAMETERS
None.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
void ps_iface_log_qos_spec_params
(
 const qos_spec_type*         qos_spec
)
{
  int i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == qos_spec)
  {
    return;
  }

  LOG_MSG_INFO1_2( "ps_iface_log_qos_spec_params(): "
                   "obj 0x%p qos spec field mask: 0x%x",
                   qos_spec, qos_spec->field_mask );

  if (qos_spec->rx.fltr_template.num_filters)
  {
    LOG_MSG_INFO1_1( "ps_iface_log_qos_spec_params(): "
                     "qos spec contains %d RX filters",
                     qos_spec->rx.fltr_template.num_filters );
    for (i = 0; i < qos_spec->rx.fltr_template.num_filters; i++)
    {
      LOG_MSG_INFO1_2( "ps_iface_log_qos_spec_params(): "
                       "obj 0x%p filter %d",
                       &qos_spec->rx.fltr_template.list_ptr[i], i + 1 );

      ps_iface_log_qos_spec_filter_params(&qos_spec->rx.fltr_template.list_ptr[i]);
    }
  }

  if (qos_spec->tx.fltr_template.num_filters)
  {
    LOG_MSG_INFO1_1( "ps_iface_log_qos_spec_params(): "
                     "qos spec contains %d TX filters",
                     qos_spec->tx.fltr_template.num_filters );
    for (i = 0; i < qos_spec->tx.fltr_template.num_filters; i++)
    {
      LOG_MSG_INFO1_2( "ps_iface_log_qos_spec_params(): "
                       "obj 0x%p filter %d",
                       &qos_spec->tx.fltr_template.list_ptr[i], i + 1 );

      ps_iface_log_qos_spec_filter_params(&qos_spec->tx.fltr_template.list_ptr[i]);
    }
  }
} /* ps_iface_log_qos_spec_params */


/*===========================================================================
FUNCTION PS_IFACE_CREATE_FLOW

DESCRIPTION
  This function creates a flow, processes the requested QOS spec and stores
  it in the flow control block for later use. Both flow spec and filter spec
  are processed and validated for each direction.  A generic validation is
  performed on the parameters. If mode handlers require specific validation
  they can pass an optional validation function ptr to perform extended
  validations. Note that generic validations will still be performed before
  calling extended validaiton functions.  Both Rx and Tx flow specs are
  stored as is after making sure all specified parameters are correct. Tx
  filter spec is validated and stored in decreasing order of fltr precedence.
  Mode handlers should provide alternate camparision function if they do not
  wish to apply generic comparison algorithm. Tx filter spec is passed to IP
  filtering library, where it is sorted and stored to be applied to the
  outgoing IP traffic.

PARAMETERS
  iface_ptr        : corresponding iface on which QOS is requested
  phys_link_ptr    : ptr to phys link to which created flow will be bound to
  qos_req_params   : structure containing the qos_spec and all the
                     information needed to process it
  ps_errno         : specific error code in case operation fails
  out_flow_ptr_ptr : flow that is being created. (OUT param)

RETURN VALUE
   0 : on success
  -1 : on failure

DEPENDENCIES
  This function may be called from QMI task context. This function shall not
  call to any blocking QMI APIs.

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_create_flow
(
  ps_iface_type                    * iface_ptr,
  ps_phys_link_type                * phys_link_ptr,
  const ps_flow_create_param_type  * qos_req_param,
  ps_flow_type                    ** out_flow_ptr_ptr,
  int16                            * ps_errno
)
{
  ps_flow_type                    * default_flow_ptr;
  ps_flow_type                    * flow_ptr = NULL;
  ps_flow_type                    * tmp_flow_ptr;
  ps_flow_type                    * prev_flow_ptr;
  ps_iface_ipfltr_add_param_type    fltr_add_param;
  int16                             tmp_errno;
  uint8                             flow_cookie;
  uint8                             i;
  qsl_qos_opcode_enum_type          qsl_opcode; 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (ps_errno == NULL)
  {
    DATA_ERR_FATAL("ps_iface_create_flow(): NULL parameter is passed");
    return -1;
  }

  if (qos_req_param == NULL || qos_req_param->qos_spec == NULL ||
      out_flow_ptr_ptr == NULL)
  {
    DATA_ERR_FATAL("ps_iface_create_flow(): NULL parameters are passed");
    *ps_errno = DS_EFAULT;
    return -1;
  }

  *out_flow_ptr_ptr = NULL;

  if (!PS_IFACE_IS_VALID(iface_ptr))
  {
    LOG_MSG_ERROR_1("ps_iface_create_flow(): "
                    "Invalid iface, 0x%p, is passed", iface_ptr);
    ASSERT(0);
    *ps_errno = DS_EFAULT;
    return -1;
  }

  LOG_MSG_INFO1_1("ps_iface_create_flow(): iface_ptr: 0x%x", iface_ptr);

  /*-------------------------------------------------------------------------
    HDR doesn't know which phys link, this flow will be bound to. So its
    valid for phys link to be NULL when flow is created
  -------------------------------------------------------------------------*/
  if (phys_link_ptr != NULL)
  {
    if (!PS_PHYS_LINK_IS_VALID(phys_link_ptr))
    {
      LOG_MSG_ERROR_1("ps_iface_create_flow(): "
                      "Invalid phys link, 0x%p, is passed", phys_link_ptr);
      ASSERT(0);
      *ps_errno = DS_EFAULT;
      return -1;
    }

    /*-----------------------------------------------------------------------
      Make sure that phys_link_ptr belongs to iface_ptr
    -----------------------------------------------------------------------*/
    if (!PS_IFACEI_IS_PHYS_LINK_VALID(iface_ptr, phys_link_ptr))
    {
      LOG_MSG_ERROR_3("ps_iface_create_flow(): "
                      "Iface 0x%x:%d, not associated with phys link 0x%p",
                      iface_ptr->name, iface_ptr->instance, phys_link_ptr);
      ASSERT(0);
      *ps_errno = DS_EFAULT;
      return -1;
    }
  }

  ps_iface_log_qos_spec_params(qos_req_param->qos_spec);

  if (ps_flowi_validate_qos_spec(qos_req_param,
                                 ps_iface_get_addr_family(iface_ptr),
                                 ps_errno) != 0)
  {
    LOG_MSG_ERROR_0("ps_iface_create_flow(): "
                    "Invalid QOS spec is sent by application");
    /* ps_errno is populated in ps_flowi_validate_qos_spec() */
    return -1;
  }

  if (ps_flowi_create(TRUE, &flow_ptr, ps_errno) != 0)
  {
    LOG_MSG_ERROR_0("ps_iface_create_flow(): "
                    "Couldn't create a buffer for PS flow");
    /* ps_errno is populated in ps_flowi_create() */
    return -1;
  }

  if (ps_flowi_store_qos_spec(flow_ptr,
                              qos_req_param->qos_spec,
                              qos_req_param->fltr_priority,
                              qos_req_param->fltr_compare_f_ptr,
                              ps_errno) != 0)
  {
    LOG_MSG_ERROR_1("ps_iface_create_flow(): "
                    "Couldn't store qos spec in flow, 0x%p", flow_ptr);
    (void) ps_flowi_delete(flow_ptr);
    /* ps_errno is populated in ps_flowi_store_qos_spec() */
    return -1;
  }

  /*-------------------------------------------------------------------------
    Save TX filter spec in ps_iface if Tx flow spec is specified
  -------------------------------------------------------------------------*/
  if (qos_req_param->qos_spec->field_mask & QOS_MASK_TX_FLOW)
  {
    memset(&fltr_add_param, 0, sizeof(ps_iface_ipfltr_add_param_type));
    fltr_add_param.filter_type        = IPFLTR_DEFAULT_TYPE;
    fltr_add_param.subset_id          = qos_req_param->subset_id;
    fltr_add_param.enable             = qos_req_param->enable_filtering;
    fltr_add_param.is_validated       = TRUE;
    fltr_add_param.fltr_priority      = qos_req_param->fltr_priority;
    fltr_add_param.fltr_compare_f_ptr = qos_req_param->fltr_compare_f_ptr;

    fltr_add_param.fi_result   = (ps_iface_ipfltr_result_type) flow_ptr;
    fltr_add_param.fi_ptr_arr  =
      qos_req_param->qos_spec->tx.fltr_template.list_ptr;
    fltr_add_param.num_filters =
      qos_req_param->qos_spec->tx.fltr_template.num_filters;

    PS_FLOWI_SET_TX_FLTR_HANDLE
    (
      flow_ptr,
      ps_iface_ipfltr_add(iface_ptr,
                          IP_FLTR_CLIENT_QOS_OUTPUT,
                          &fltr_add_param,
                          ps_errno)
    );

    if (PS_FLOWI_GET_TX_FLTR_HANDLE(flow_ptr) ==
          PS_IFACE_IPFLTR_INVALID_HANDLE)
    {
      LOG_MSG_ERROR_3("ps_iface_create_flow(): "
                      "Couldn't store Tx fltr spec in iface, 0x%x:%d, "
                      "for flow 0x%p",
                      iface_ptr->name, iface_ptr->instance, flow_ptr);
      (void) ps_flowi_delete(flow_ptr);
      /* ps_errno is populated in ps_iface_ipfltr_add() */
      return -1;
    }
  }

  /*-------------------------------------------------------------------------
    Save pointers to RX filter spec in ps_iface if Rx flow spec is requested
  -------------------------------------------------------------------------*/
  if (qos_req_param->qos_spec->field_mask & QOS_MASK_RX_FLOW)
  {
    PS_FLOWI_SET_RX_FLTR_HANDLE
    (
      flow_ptr,
      ps_ifacei_rx_qos_fltr_add(iface_ptr,
                                flow_ptr->flow_private.qos_info_ptr,
                                qos_req_param->fltr_priority,
                                qos_req_param->fltr_compare_f_ptr,
                                ps_errno)
    );

    if (PS_FLOWI_GET_RX_FLTR_HANDLE(flow_ptr) ==
          PS_IFACE_IPFLTR_INVALID_HANDLE)
    {
      LOG_MSG_ERROR_3("ps_iface_create_flow(): "
                      "Couldn't store Rx fltr spec in iface, 0x%x:%d, "
                      "for flow 0x%p",
                      iface_ptr->name, iface_ptr->instance, flow_ptr);

      (void) ps_iface_ipfltr_delete
             (
               iface_ptr,
               IP_FLTR_CLIENT_QOS_OUTPUT,
               PS_FLOWI_GET_TX_FLTR_HANDLE(flow_ptr),
               &tmp_errno
             );
      (void) ps_flowi_delete(flow_ptr);
      /* ps_errno is populated in ps_ifacei_rx_qos_fltr_add() */
      return -1;
    }
  }

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  if (PS_IFACEI_GET_NUM_SEC_FLOWS(iface_ptr) == PS_IFACE_MAX_SEC_FLOWS)
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_INFO1_3("ps_iface_create_flow(): "
                    "Iface, 0x%x:%d, already has maximum possible number of "
                    "ps_flows (%d). Can't add one more ps_flow",
                    iface_ptr->name,
                    iface_ptr->instance,
                    PS_IFACE_MAX_SEC_FLOWS);

    (void) ps_iface_ipfltr_delete
           (
             iface_ptr,
             IP_FLTR_CLIENT_QOS_OUTPUT,
             PS_FLOWI_GET_TX_FLTR_HANDLE(flow_ptr),
             &tmp_errno
           );
    ps_ifacei_rx_qos_fltr_delete(iface_ptr,
                                 PS_FLOWI_GET_RX_FLTR_HANDLE(flow_ptr));
    (void) ps_flowi_delete(flow_ptr);

    *ps_errno = DS_ENOMEM;
    return -1;
  }

  /*-------------------------------------------------------------------------
    Insert in to the list of secondary flows. Insertion sort is used to keep
    the list sorted

    Algorithm
      1. Always increment instance when a flow is added. Don't slide it back
         when a flow is deleted.
      2. Make sure that "flow_inst" is already used up. If it is, increment
         "flow_inst" and start searching again.
      3. Handle wrap around by resetting "flow_inst" to 1 and search from
         begining of list
  -------------------------------------------------------------------------*/
  tmp_flow_ptr =
    list_peek_front(&(iface_ptr->iface_private.flow.sec_flow_list));
  prev_flow_ptr = NULL;
  flow_cookie = iface_ptr->iface_private.flow.next_cookie;

  /*-------------------------------------------------------------------------
    This won't run in to infinite loop since PS_IFACE_MAX_SEC_FLOWS is <=
    PS_IFACE_MAX_SEC_FLOW_INST, i.e there is a hole
    in the instance space
  -------------------------------------------------------------------------*/
  while (tmp_flow_ptr != NULL &&
         PS_FLOWI_GET_COOKIE(tmp_flow_ptr) <= flow_cookie)
  {
    if (PS_FLOWI_GET_COOKIE(tmp_flow_ptr) == flow_cookie)
    {
      flow_cookie++;
      if (flow_cookie > PS_IFACE_MAX_SEC_FLOW_INST)
      {
        flow_cookie = PS_IFACE_MIN_SEC_FLOW_INST;
        tmp_flow_ptr =
          list_peek_front(&(iface_ptr->iface_private.flow.sec_flow_list));
        prev_flow_ptr = NULL;
      }
    }
    else
    {
      prev_flow_ptr = tmp_flow_ptr;
      tmp_flow_ptr =
        list_peek_next(&(iface_ptr->iface_private.flow.sec_flow_list),
                       &(tmp_flow_ptr->link));
    }
  }

  if (prev_flow_ptr == NULL)
  {
    list_push_front(&(iface_ptr->iface_private.flow.sec_flow_list),
                    &(flow_ptr->link));
  }
  else
  {
    list_push_after(&(iface_ptr->iface_private.flow.sec_flow_list),
                    &(flow_ptr->link),
                    &(prev_flow_ptr->link));
  }

  PS_FLOWI_SET_COOKIE(flow_ptr, flow_cookie);
  if (iface_ptr->iface_private.flow.next_cookie == PS_IFACE_MAX_SEC_FLOW_INST)
  {
    iface_ptr->iface_private.flow.next_cookie = PS_IFACE_MIN_SEC_FLOW_INST;
  }
  else
  {
    (iface_ptr->iface_private.flow.next_cookie)++;
  }

  PS_IFACEI_INC_NUM_SEC_FLOWS(iface_ptr);
  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

  /*-------------------------------------------------------------------------
    Copy mode handler callback functions from default flow so that mode
    handler doesn't have to update these variables every time a flow is
    created
  -------------------------------------------------------------------------*/
  default_flow_ptr = PS_IFACEI_GET_DEFAULT_FLOW(iface_ptr);
  PS_FLOWI_SET_CB_F_PTR(flow_ptr,
                        default_flow_ptr->ps_flow_activate_cmd_f_ptr,
                        default_flow_ptr->ps_flow_configure_cmd_f_ptr,
                        default_flow_ptr->ps_flow_suspend_cmd_f_ptr,
                        default_flow_ptr->ps_flow_go_null_cmd_f_ptr,
                        default_flow_ptr->ps_flow_ioctl_f_ptr);

  /*-------------------------------------------------------------------------
    Copy DPL settings stored in ps_iface to ps_flow
  -------------------------------------------------------------------------*/
  flow_ptr->dpl_flow_cb.tx_cb.mask = iface_ptr->dpl_sec_flow_copy_cb.mask;
  for (i = 0; i < DPL_IID_NETPROT_MAX; i++)
  {
    flow_ptr->dpl_flow_cb.tx_cb.snaplen[i] =
      iface_ptr->dpl_sec_flow_copy_cb.snaplen[i];
  }

  flow_ptr->dpl_flow_cb.tx_dpl_id = default_flow_ptr->dpl_flow_cb.tx_dpl_id;
  flow_ptr->dpl_flow_cb.tx_dpl_id.link_instance =
    PS_FLOWI_GET_COOKIE(flow_ptr);

  flow_ptr->flow_private.this_flow = flow_ptr;

  /*-------------------------------------------------------------------------
  Set nw_initiated flag according to qos_req_param
  -------------------------------------------------------------------------*/
  flow_ptr->flow_private.is_nw_initiated = qos_req_param->is_nw_initiated;

  /*-------------------------------------------------------------------------
    Bind flow to the given phys link
  -------------------------------------------------------------------------*/
  if (phys_link_ptr != NULL)
  {
    (void) ps_flowi_bind_phys_link(flow_ptr, phys_link_ptr);
  }

  flow_ptr->flow_private.iface_ptr = iface_ptr;

  if (TRUE == qos_req_param->is_nw_initiated ) 
  {
    ps_qos_net_initiated_check_requests(iface_ptr,
                                        flow_ptr,
                                        qos_req_param->qos_spec);
  }


  memset (&flow_ptr->event_info_cache, 0, sizeof(flow_event_info_cache_type));

  ps_iface_generic_ind(iface_ptr, IFACE_FLOW_ADDED_EV, flow_ptr);

  *out_flow_ptr_ptr = flow_ptr;

  /*-------------------------------------------------------------------------
    The log packet should be generated only if the phys_link_ptr is present.
    If the phys_link_ptr is NULL, the phys_link_handle can't be determined
    during the log packet generation.
  -------------------------------------------------------------------------*/
  if (phys_link_ptr != NULL)
  {
    ps_stat_logging_inst_update_desc
    (
      PS_STAT_MODULE_INST_FLOW,
      ( (iface_ptr->iface_private.iface_index << 8) |
        flow_ptr->flow_private.cookie),
      PS_STAT_INST_CREATED
    );
  }

  /*---------------------------------------------------------------------
    Log qos_spec requested by the application
  ---------------------------------------------------------------------*/
  if (!PS_IFACEI_IS_LOGICAL(iface_ptr))
  {
    if (qos_req_param->opcode == PS_IFACE_IOCTL_QOS_CONFIGURE_OP)
    {
      qsl_opcode = QSL_QOS_CONFIGURE;
    }
    else
    {
      qsl_opcode = QSL_QOS_REQUEST;
    }

    ps_qsl_log_requested_qos_spec(qos_req_param->qos_spec,
                                  iface_ptr,
                                  flow_ptr,
                                  qsl_opcode); 
  }

  LOG_MSG_INFO1_2("ps_iface_create_flow(): created new flow: 0x%x, "
                  "is_nw_initiated = %d",
                  *out_flow_ptr_ptr, qos_req_param->is_nw_initiated);
  
  return 0;
} /* ps_iface_create_flow() */

/*===========================================================================
FUNCTION PS_IFACE_MODIFY_FLOW()

DESCRIPTION
  This function processes the QOS_MODIFY spec and stores it in the flow
  control block for later use. Both flow spec and filter spec
  are processed and validated for each direction. A generic validation is
  performed on the parameters. If mode handlers require specific validation
  they can pass an optional validation function ptr to perform extended
  validation. Note that generic validation will still be performed before
  calling extended validaiton function. If Rx and/or Tx flow specs are
  modified, they are stored as is after making sure all specified parameters
  are correct. Tx filter spec is validated and stored in decreasing order
  of fltr precedence. Tx filters are disabled until network comes back with
  a response. Mode handlers should provide alternate camparison function if
  they do not wish to apply generic comparison algorithm.

PARAMETERS
  iface_ptr         : corresponding iface on which QOS is requested
  flow_ptr          : flow to be modified
  qos_modify_params : structure containing qos_modified_spec and all the
                      information needed to process it
  ps_errno          : specific error code in case operation fails

RETURN VALUE
   0 : on success
  -1 : on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int32 ps_iface_modify_flow
(
  ps_iface_type                    * iface_ptr,
  ps_flow_type                     * flow_ptr,
  const ps_flow_modify_param_type  * qos_modify_param,
  int16                            * ps_errno
)
{
  ps_iface_ipfltr_add_param_type    fltr_add_param;
  int16                             tmp_errno;
  boolean                           is_rx_fltr_modified = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (ps_errno == NULL)
  {
    DATA_ERR_FATAL("ps_iface_modify_flow(): NULL parameter is passed");
    return -1;
  }

  if (qos_modify_param == NULL || qos_modify_param->qos_spec == NULL)
  {
    DATA_ERR_FATAL("ps_iface_modify_flow(): NULL parameters are passed");
    *ps_errno = DS_EFAULT;
    return -1;
  }

  if (!PS_IFACE_IS_VALID(iface_ptr))
  {
    LOG_MSG_ERROR_1("ps_iface_modify_flow(): "
                    "Invalid iface, 0x%p, is passed", iface_ptr);
    ASSERT(0);
    *ps_errno = DS_EFAULT;
    return -1;
  }

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  if (!PS_FLOW_IS_VALID(flow_ptr))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_iface_modify_flow(): "
                    "Invalid flow, 0x%p, is passed", flow_ptr);
    ASSERT(0);
    *ps_errno = DS_EFAULT;
    return -1;
  }

  if (PS_FLOWI_GET_CAPABILITY(flow_ptr, PS_FLOW_CAPABILITY_DEFAULT))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_iface_modify_flow(): "
                    "QOS is not supported on default flow, 0x%p", flow_ptr);
    ASSERT(0);
    *ps_errno = DS_EFAULT;
    return -1;
  }

  if (PS_FLOWI_IS_QOS_MODIFY_IN_PROGRESS(flow_ptr))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_INFO1_1("ps_iface_modify_flow(): "
                    "Atmost only one QOS_MODIFY can be active on ps_flow, 0x%p",
                    flow_ptr);
    *ps_errno = DS_EINVAL;
    return -1;
  }

  if (ps_flowi_validate_qos_modify_spec(flow_ptr,
                                        qos_modify_param,
                                        ps_iface_get_addr_family(iface_ptr),
                                        ps_errno) != 0)
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_INFO1_1("ps_iface_modify_flow(): "
                    "Invalid QOS_MODIFY spec is specified on ps_flow, 0x%p",
                    flow_ptr);
    /* ps_errno is populated in ps_flowi_validate_qos_modify_spec() */
    return -1;
  }

  if (ps_flowi_store_qos_modify_spec(flow_ptr,
                                     qos_modify_param->qos_spec,
                                     qos_modify_param->fltr_priority,
                                     qos_modify_param->fltr_compare_f_ptr,
                                     ps_errno) != 0)
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_iface_modify_flow(): "
                    "Couldn't store QOS_MODIFY spec in ps_flow, 0x%p",
                    flow_ptr);
    /* ps_errno is populated in ps_flowi_store_qos_modify_spec() */
    return -1;
  }

  /*-------------------------------------------------------------------------
    Store Tx fltr spec if it is added newly or is modified
  -------------------------------------------------------------------------*/
  if (PS_FLOWI_IS_MODIFY_TX_FLTR_SPEC_CREATED(flow_ptr) ||
      PS_FLOWI_IS_MODIFY_TX_FLTR_SPEC_MODIFIED(flow_ptr))
  {
    memset(&fltr_add_param, 0, sizeof(ps_iface_ipfltr_add_param_type));
    /*-----------------------------------------------------------------------
      Save QOS_MODIFY Tx fltr spec in ps_iface. Filters are disabled until
      network accepts QOS_MODIFY
    -----------------------------------------------------------------------*/
    fltr_add_param.filter_type        = IPFLTR_DEFAULT_TYPE;
    fltr_add_param.subset_id          = qos_modify_param->subset_id;
    fltr_add_param.enable             = FALSE;
    fltr_add_param.is_validated       = TRUE;
    fltr_add_param.fltr_priority      = qos_modify_param->fltr_priority;
    fltr_add_param.fltr_compare_f_ptr = qos_modify_param->fltr_compare_f_ptr;

    fltr_add_param.fi_result   = (ps_iface_ipfltr_result_type) flow_ptr;
    fltr_add_param.fi_ptr_arr  =
      qos_modify_param->qos_spec->tx.fltr_template.list_ptr;
    fltr_add_param.num_filters =
      qos_modify_param->qos_spec->tx.fltr_template.num_filters;

    PS_FLOWI_SET_MODIFY_TX_FLTR_HANDLE
    (
      flow_ptr,
      ps_iface_ipfltr_add(iface_ptr,
                          IP_FLTR_CLIENT_QOS_OUTPUT,
                          &fltr_add_param,
                          ps_errno)
    );

    if (PS_FLOWI_GET_MODIFY_TX_FLTR_HANDLE(flow_ptr) ==
          PS_IFACE_IPFLTR_INVALID_HANDLE)
    {
      LOG_MSG_ERROR_3("ps_iface_modify_flow(): "
                      "Couldn't store QOS_MODIFY Tx fltr spec in iface, 0x%x:%d, "
                      "for ps_flow 0x%p",
                      iface_ptr->name, iface_ptr->instance, flow_ptr);
      (void) ps_flowi_delete_qos_modify_spec(flow_ptr);

      PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);

      /* ps_errno is populated in ps_iface_ipfltr_add() */
      return -1;
    }
  }

  /*-------------------------------------------------------------------------
    Store Rx fltr spec if it is added newly or is modified. Different API
    is used based on if Rx filters are newly added or modified
  -------------------------------------------------------------------------*/
  if (PS_FLOWI_IS_MODIFY_RX_FLTR_SPEC_CREATED(flow_ptr))
  {
    /*-----------------------------------------------------------------------
      Save pointers to QOS_MODIFY Rx fltr spec in ps_iface
    -----------------------------------------------------------------------*/
    PS_FLOWI_SET_MODIFY_RX_FLTR_HANDLE
    (
      flow_ptr,
      ps_ifacei_rx_qos_fltr_add(iface_ptr,
                                flow_ptr->flow_private.qos_modify_info_ptr,
                                qos_modify_param->fltr_priority,
                                qos_modify_param->fltr_compare_f_ptr,
                                ps_errno)
    );

    is_rx_fltr_modified = TRUE;
  }
  else if (PS_FLOWI_IS_MODIFY_RX_FLTR_SPEC_MODIFIED(flow_ptr))
  {
    /*-----------------------------------------------------------------------
      Save pointers to QOS_MODIFY Rx fltr spec in ps_iface
    -----------------------------------------------------------------------*/
    PS_FLOWI_SET_MODIFY_RX_FLTR_HANDLE
    (
      flow_ptr,
      ps_ifacei_rx_qos_fltr_modify(iface_ptr,
                                   flow_ptr->flow_private.qos_modify_info_ptr,
                                   PS_FLOWI_GET_RX_FLTR_HANDLE(flow_ptr),
                                   qos_modify_param->fltr_priority,
                                   qos_modify_param->fltr_compare_f_ptr,
                                   ps_errno)
    );

    is_rx_fltr_modified = TRUE;
  }

  /*-------------------------------------------------------------------------
    Clean up state if we failed to add/modify Rx modify filters
  -------------------------------------------------------------------------*/
  if (is_rx_fltr_modified &&
      PS_FLOWI_GET_MODIFY_RX_FLTR_HANDLE(flow_ptr) ==
        PS_IFACE_IPFLTR_INVALID_HANDLE)
  {
    LOG_MSG_ERROR_3("ps_iface_modify_flow(): "
                    "Couldn't store QOS_MODIFY Rx fltr spec in iface, 0x%x:%d, "
                    "for ps_flow 0x%p",
                    iface_ptr->name, iface_ptr->instance, flow_ptr);

    (void) ps_iface_ipfltr_delete
           (
             iface_ptr,
             IP_FLTR_CLIENT_QOS_OUTPUT,
             PS_FLOWI_GET_MODIFY_TX_FLTR_HANDLE(flow_ptr),
             &tmp_errno
           );
    (void) ps_flowi_delete_qos_modify_spec(flow_ptr);

    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);

    /* ps_errno is populated in ps_ifacei_rx_qos_fltr_add/modify() */
    return -1;
  }

  ps_qsl_log_modified_qos_spec(qos_modify_param->qos_spec,
                               flow_ptr);
  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return 0;

} /* ps_iface_modify_flow() */



/*===========================================================================
FUNCTION PS_IFACE_DELETE_FLOW

DESCRIPTION
  This function deletes a flow along with its state

PARAMETERS
  iface_ptr : iface to which flow_ptr belongs
  flow_ptr  : ptr to flow

RETURN VALUE
   0 : on success
  -1 : on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_delete_flow
(
  ps_iface_type    * iface_ptr,
  ps_flow_type     * flow_ptr,
  int16            * ps_errno
)
{
  ps_flow_type        * tmp_flow_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (ps_errno == NULL)
  {
    DATA_ERR_FATAL("ps_iface_delete_flow(): NULL parameter is passed");
    return -1;
  }

  if (!PS_IFACE_IS_VALID(iface_ptr))
  {
    LOG_MSG_ERROR_1("ps_iface_delete_flow(): "
                    "Invalid iface 0x%p is passed", iface_ptr);
    ASSERT(0);
    *ps_errno = DS_EFAULT;
    return -1;
  }

  /*-------------------------------------------------------------------------
    Done outside critical section to prevent problems
  -------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
    The log packet should be generated only if the phys_link_ptr is present.
    If the phys_link_ptr is NULL, the phys_link_handle can't be determined
    during the log packet generation.
  -------------------------------------------------------------------------*/
  if ((flow_ptr != NULL) &&
      (flow_ptr->flow_private.phys_link_ptr != NULL))
  {
    ps_stat_logging_inst_update_desc
    (
      PS_STAT_MODULE_INST_FLOW,
      ( (iface_ptr->iface_private.iface_index << 8) |
      flow_ptr->flow_private.cookie),
      PS_STAT_INST_DELETED
    );
  }

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  if (!PS_FLOW_IS_VALID(flow_ptr))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_iface_delete_flow(): "
                    "Invalid flow, 0x%p, is passed", flow_ptr);
    ASSERT(0);
    *ps_errno = DS_EFAULT;
    return -1;
  }

  if (PS_FLOWI_GET_CAPABILITY(flow_ptr, PS_FLOW_CAPABILITY_DEFAULT))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_iface_delete_flow(): "
                    "Default flow, 0x%p, must not be deleted", flow_ptr);
    ASSERT(0);
    *ps_errno = DS_EFAULT;
    return -1;
  }

  if (PS_FLOWI_GET_STATE(flow_ptr) != FLOW_NULL)
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_2("ps_iface_delete_flow(): "
                    "Deleting flow, 0x%p, in invalid state, 0x%x",
                    flow_ptr, PS_FLOWI_GET_STATE(flow_ptr));
    ASSERT(0);
    return -1;
  }

  /*-------------------------------------------------------------------------
    Make sure that this flow belongs to this iface. Otherwise list_pop_item()
    may result in infinite loop
  -------------------------------------------------------------------------*/
  tmp_flow_ptr =
    list_peek_front(&(iface_ptr->iface_private.flow.sec_flow_list));
  while (tmp_flow_ptr != NULL && tmp_flow_ptr != flow_ptr)
  {
    tmp_flow_ptr =
      list_peek_next(&(iface_ptr->iface_private.flow.sec_flow_list),
                     &(tmp_flow_ptr->link));
  }

  if (tmp_flow_ptr == NULL)
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_3("ps_iface_delete_flow(): "
                    "Flow, 0x%p, doesn't belong to iface, 0x%x:%d",
                    flow_ptr, iface_ptr->name, iface_ptr->instance);
    ASSERT(0);
    *ps_errno = DS_EFAULT;
    return -1;
  }

  /*-------------------------------------------------------------------------
    Clean up Tx fltr spec if it was requested
  -------------------------------------------------------------------------*/
  if (PS_FLOWI_GET_TX_FLTR_HANDLE(flow_ptr) != 0)
  {
    if (ps_iface_ipfltr_delete(iface_ptr,
                               IP_FLTR_CLIENT_QOS_OUTPUT,
                               PS_FLOWI_GET_TX_FLTR_HANDLE(flow_ptr),
                               ps_errno) == -1)
    {
      PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      ASSERT(0);
      return -1;
    }
  }

  /*-------------------------------------------------------------------------
    Clean up pointers to Rx fltr spec if Rx fltr spec was requested
  -------------------------------------------------------------------------*/
  if (PS_FLOWI_GET_RX_FLTR_HANDLE(flow_ptr) != 0)
  {
    ps_ifacei_rx_qos_fltr_delete(iface_ptr,
                                 PS_FLOWI_GET_RX_FLTR_HANDLE(flow_ptr));
  }

  /*-------------------------------------------------------------------------
    Clean up state pertaining to QOS_MODIFY if it is in progress
  -------------------------------------------------------------------------*/
  if (PS_FLOWI_IS_QOS_MODIFY_IN_PROGRESS(flow_ptr))
  {
    /*-----------------------------------------------------------------------
      Delete Tx fltr spec specified in QOS_MODIFY. Note that app may not have
      changed/requested Tx fltr spec
    -----------------------------------------------------------------------*/
    if (PS_FLOWI_GET_MODIFY_TX_FLTR_HANDLE(flow_ptr) != 0)
    {
      if (ps_iface_ipfltr_delete(iface_ptr,
                                 IP_FLTR_CLIENT_QOS_OUTPUT,
                                 PS_FLOWI_GET_MODIFY_TX_FLTR_HANDLE(flow_ptr),
                                 ps_errno) == -1)
      {
        PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
        ASSERT(0);
        return -1;
      }
    }

    /*-----------------------------------------------------------------------
      Delete pointers to Rx fltr spec specified in QOS_MODIFY. Note that app
      may not have changed/requested Rx fltr spec
    -----------------------------------------------------------------------*/
    if (PS_FLOWI_GET_MODIFY_RX_FLTR_HANDLE(flow_ptr) != 0)
    {
      ps_ifacei_rx_qos_fltr_delete
      (
        iface_ptr,
        PS_FLOWI_GET_MODIFY_RX_FLTR_HANDLE(flow_ptr)
      );
    }

    (void) ps_flowi_delete_qos_modify_spec(flow_ptr);
  }

  /*-------------------------------------------------------------------------
    Delete this flow from iface's list of secondary flows
  -------------------------------------------------------------------------*/
  list_pop_item(&(iface_ptr->iface_private.flow.sec_flow_list),
                &(flow_ptr->link));
  PS_IFACEI_DEC_NUM_SEC_FLOWS(iface_ptr);

  (void) ps_flowi_delete(flow_ptr);

  ps_iface_generic_ind(iface_ptr, IFACE_FLOW_DELETED_EV, NULL);

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return 0;

} /* ps_iface_delete_flow() */



/*===========================================================================
FUNCTION PS_IFACE_MODIFY_FLOW_ACCEPTED

DESCRIPTION
  Indicates that QOS_MODIFY/PRIMARY_QOS_MODIFY is accepted by network.
  If it is QOS_MODIFY, migrates all state associated with QOS_MODIFY to
  ps_flow so that ps_flow represents QOS granted by network and cleans old QOS.

PARAMETERS
  iface_ptr : iface to which flow_ptr belongs
  flow_ptr  : ptr to flow
  info_code : reason for accepting QOS_MODIFY
  ps_errno  : specific error code in case operation fails

RETURN VALUE
   0 : on success
  -1 : on failure

DEPENDENCIES
  None

SIDE EFFECTS
  Previously granted QOS is not valid anymore and FLOW_MODIFY_ACCEPTED_EV is
  posted
===========================================================================*/
int32 ps_iface_modify_flow_accepted
(
  ps_iface_type                    * iface_ptr,
  ps_flow_type                     * flow_ptr,
  ps_extended_info_code_enum_type    info_code,
  int16                            * ps_errno
)
{
  ps_flow_type              * tmp_flow_ptr = NULL;
  qos_spec_type             * tmp_qos_spec = NULL;
  int32                       ret_val = -1;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (ps_errno == NULL)
  {
    DATA_ERR_FATAL("ps_iface_modify_flow_accepted(): NULL parameter is passed");
    return -1;
  }

  if (!PS_IFACE_IS_VALID(iface_ptr))
  {
    LOG_MSG_ERROR_1("ps_iface_modify_flow_accepted(): "
                    "Invalid iface 0x%p is passed", iface_ptr);
    ASSERT(0);
    *ps_errno = DS_EFAULT;
    return -1;
  }

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  if (!PS_FLOW_IS_VALID(flow_ptr))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_iface_modify_flow_accepted(): "
                    "Invalid flow, 0x%p, is passed", flow_ptr);
    ASSERT(0);
    *ps_errno = DS_EFAULT;
    return -1;
  }

  if (PS_FLOWI_GET_CAPABILITY(flow_ptr, PS_FLOW_CAPABILITY_DEFAULT))
  {
    if (PS_IFACEI_GET_DEFAULT_FLOW(iface_ptr) != flow_ptr)
    {
      PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      LOG_MSG_ERROR_3("ps_iface_modify_flow_accepted(): "
                      "ps_flow 0x%p is not a default flow of iface 0x%x:%d",
                      flow_ptr, iface_ptr->name, iface_ptr->instance);
      ASSERT(0);
      *ps_errno = DS_EFAULT;
      return -1;
    }
  }
  else
  {
    /*-----------------------------------------------------------------------
      Make sure that this secondary flow belongs to this iface
    -----------------------------------------------------------------------*/
    tmp_flow_ptr =
      list_peek_front(&(iface_ptr->iface_private.flow.sec_flow_list));
    while (tmp_flow_ptr != NULL && tmp_flow_ptr != flow_ptr)
    {
      tmp_flow_ptr =
        list_peek_next(&(iface_ptr->iface_private.flow.sec_flow_list),
                       &(tmp_flow_ptr->link));
    }

    if (tmp_flow_ptr == NULL)
    {
      PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      LOG_MSG_ERROR_3("ps_iface_modify_flow_accepted(): "
                      "ps_flow, 0x%p, doesn't belong to iface, 0x%x:%d",
                      flow_ptr, iface_ptr->name, iface_ptr->instance);
      ASSERT(0);
      *ps_errno = DS_EFAULT;
      return -1;
    }
  }

  LOG_MSG_INFO1_1("ps_iface_modify_flow_accepted(): "
                  "QOS MODIFY ACCEPTED on ps_flow, 0x%p", flow_ptr);

  /*-------------------------------------------------------------------------
    Skip all processing if this is a default ps_flow, as Mode handler manages
    everything for default ps_flow
  -------------------------------------------------------------------------*/
  if (!PS_FLOWI_GET_CAPABILITY(flow_ptr, PS_FLOW_CAPABILITY_DEFAULT))
  {
    /*-----------------------------------------------------------------------
      Delete filters specified in old QOS if app changed/deleted filter spec
    -----------------------------------------------------------------------*/
    if (PS_FLOWI_IS_MODIFY_TX_FLTR_SPEC_DELETED(flow_ptr) ||
        PS_FLOWI_IS_MODIFY_TX_FLTR_SPEC_MODIFIED(flow_ptr))
    {
      if (ps_iface_ipfltr_delete(iface_ptr,
                                 IP_FLTR_CLIENT_QOS_OUTPUT,
                                 PS_FLOWI_GET_TX_FLTR_HANDLE(flow_ptr),
                                 ps_errno) == -1)
      {
        PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
        ASSERT(0);
        return -1;
      }

      PS_FLOWI_SET_TX_FLTR_HANDLE(flow_ptr, 0);
    }

    if (PS_FLOWI_IS_MODIFY_RX_FLTR_SPEC_DELETED(flow_ptr) ||
        PS_FLOWI_IS_MODIFY_RX_FLTR_SPEC_MODIFIED(flow_ptr))
    {
      ps_ifacei_rx_qos_fltr_delete(iface_ptr,
                                   PS_FLOWI_GET_RX_FLTR_HANDLE(flow_ptr));
      PS_FLOWI_SET_RX_FLTR_HANDLE(flow_ptr, 0);
    }

    /*-----------------------------------------------------------------------
      Update Tx fltr handle if Tx fltr spec is either newly added or modified.
      Tx fltr spec specified in QOS_MODIFY was added in disabled state. Since
      network accepted QOS_MODIFY, enable these filters
    -----------------------------------------------------------------------*/
    if (PS_FLOWI_IS_MODIFY_TX_FLTR_SPEC_CREATED(flow_ptr) ||
        PS_FLOWI_IS_MODIFY_TX_FLTR_SPEC_MODIFIED(flow_ptr))
    {
      ASSERT(PS_FLOWI_GET_TX_FLTR_HANDLE(flow_ptr) == 0);
      PS_FLOWI_SET_TX_FLTR_HANDLE
      (
        flow_ptr,
        PS_FLOWI_GET_MODIFY_TX_FLTR_HANDLE(flow_ptr)
      );

      if (ps_iface_ipfltr_control(iface_ptr,
                                  IP_FLTR_CLIENT_QOS_OUTPUT,
                                  PS_FLOWI_GET_TX_FLTR_HANDLE(flow_ptr),
                                  TRUE,
                                  ps_errno) == -1)
      {
        PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
        ASSERT(0);
        return -1;
      }
    }
  }
  // get qos_spec from flow_ptr since the structure inside flow_ptr does not
  // have the correct structure, which is required by 
  // ps_qos_net_initiated_check_requests as a parameter
  ret_val = ps_flowi_get_qos_spec_from_flow(flow_ptr, TRUE, &tmp_qos_spec, ps_errno);
  if (0 != ret_val)
  {
    LOG_MSG_INFO3_2("ps_iface_modify_flow_accepted(): "
                    "ps_flowi_get_qos_spec_type_from_flow() failed, "
                    "flow 0x%p err %d", flow_ptr, *ps_errno);
  }
  else
  {
    if (TRUE == flow_ptr->flow_private.is_nw_initiated)
    {
      ps_qos_net_initiated_check_requests(iface_ptr,
                                          flow_ptr,
                                          tmp_qos_spec);
    }
    ps_iface_logical_flowi_free_qos_spec(&tmp_qos_spec);
  }

  ps_flow_qos_modify_accepted_ind(flow_ptr, info_code);

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return 0;

} /* ps_iface_modify_flow_accepted() */



/*===========================================================================
FUNCTION PS_IFACE_MODIFY_FLOW_REJECTED

DESCRIPTION
  Indicates that QOS_MODIFY/PRIMARY_QOS_MODIFY is rejected by network.
  If QOS_MODIFY, cleans all state associated with it

PARAMETERS
  iface_ptr : iface to which flow_ptr belongs
  flow_ptr  : ptr to flow
  info_code : reason for rejecting QOS_MODIFY
  ps_errno  : error code returned back if operation fails

RETURN VALUE
   0 : on success
  -1 : on failure

DEPENDENCIES
  None

SIDE EFFECTS
  FLOW_MODIFY_REJECTED_EV is posted
===========================================================================*/
int32 ps_iface_modify_flow_rejected
(
  ps_iface_type                    * iface_ptr,
  ps_flow_type                     * flow_ptr,
  ps_extended_info_code_enum_type    info_code,
  int16                            * ps_errno
)
{
  ps_flow_type  * tmp_flow_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (ps_errno == NULL)
  {
    DATA_ERR_FATAL("ps_iface_modify_flow_rejected(): NULL parameter is passed");
    return -1;
  }

  if (!PS_IFACE_IS_VALID(iface_ptr))
  {
    LOG_MSG_ERROR_1("ps_iface_modify_flow_rejected(): "
                    "Invalid iface 0x%p is passed", iface_ptr);
    ASSERT(0);
    *ps_errno = DS_EFAULT;
    return -1;
  }

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  if (!PS_FLOW_IS_VALID(flow_ptr))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_iface_modify_flow_rejected(): "
                    "Invalid flow, 0x%p, is passed", flow_ptr);
    ASSERT(0);
    *ps_errno = DS_EFAULT;
    return -1;
  }

  if (PS_FLOWI_GET_CAPABILITY(flow_ptr, PS_FLOW_CAPABILITY_DEFAULT))
  {
    if (PS_IFACEI_GET_DEFAULT_FLOW(iface_ptr) != flow_ptr)
    {
      PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      LOG_MSG_ERROR_3("ps_iface_modify_flow_rejected(): "
                      "ps_flow 0x%p is not a default flow of iface 0x%x:%d",
                      flow_ptr, iface_ptr->name, iface_ptr->instance);
      ASSERT(0);
      *ps_errno = DS_EFAULT;
      return -1;
    }
  }
  else
  {
    /*-----------------------------------------------------------------------
      Make sure that this secondary flow belongs to this iface
    -----------------------------------------------------------------------*/
    tmp_flow_ptr =
      list_peek_front(&(iface_ptr->iface_private.flow.sec_flow_list));
    while (tmp_flow_ptr != NULL && tmp_flow_ptr != flow_ptr)
    {
      tmp_flow_ptr =
        list_peek_next(&(iface_ptr->iface_private.flow.sec_flow_list),
                       &(tmp_flow_ptr->link));
    }

    if (tmp_flow_ptr == NULL)
    {
      PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      LOG_MSG_ERROR_3("ps_iface_modify_flow_rejected(): "
                      "ps_flow, 0x%p, doesn't belong to iface, 0x%x:%d",
                      flow_ptr, iface_ptr->name, iface_ptr->instance);
      ASSERT(0);
      *ps_errno = DS_EFAULT;
      return -1;
    }
  }

  LOG_MSG_INFO1_1("ps_iface_modify_flow_rejected(): "
                  "QOS_MODIFY_REJECTED on ps_flow, 0x%p", flow_ptr);

  /*-------------------------------------------------------------------------
    Skip all processing if this is a default ps_flow, as Mode handler manages
    everything for default ps_flow
  -------------------------------------------------------------------------*/
  if (!PS_FLOWI_GET_CAPABILITY(flow_ptr, PS_FLOW_CAPABILITY_DEFAULT))
  {
    /*-----------------------------------------------------------------------
      Delete filters specified in QOS_MODIFY. Note that app may not have
      changed/requested filter spec
    -----------------------------------------------------------------------*/
    if (PS_FLOWI_GET_MODIFY_TX_FLTR_HANDLE(flow_ptr) != 0)
    {
      if (ps_iface_ipfltr_delete
          (
            iface_ptr,
            IP_FLTR_CLIENT_QOS_OUTPUT,
            PS_FLOWI_GET_MODIFY_TX_FLTR_HANDLE(flow_ptr),
            ps_errno
          ) == -1)
      {
        PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
        ASSERT(0);
        return -1;
      }
    }

    if (PS_FLOWI_GET_MODIFY_RX_FLTR_HANDLE(flow_ptr) != 0)
    {
      ps_ifacei_rx_qos_fltr_delete
      (
        iface_ptr,
        PS_FLOWI_GET_MODIFY_RX_FLTR_HANDLE(flow_ptr)
      );
    }
  }

  ps_flow_qos_modify_rejected_ind(flow_ptr, info_code);

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return 0;

} /* ps_iface_modify_flow_rejected() */



/*===========================================================================
FUNCTION PS_IFACE_GET_SEC_FLOW_HANDLE()

DESCRIPTION
  This function returns a handle which can be used to fetch secondary flows
  associated with an iface

PARAMETERS
  iface_ptr : pointer to an iface

RETURN VALUE
  A handle using which secondary flows can be fetched : on success
  NULL                                                : on failure

DEPENDENCIES
  All flows must be fetched in one atomic operation. Otherwise flows
  could be deleted while client is fetching them which could lead to
  incorrect operations.

  Do not try to access flows contents outside of atomic operation. Since
  a flow can be deleted, this may lead to memory corruption or data aborts.

  Sample usage of this API is
    TASKLOCK()
      handle = PS_IFACE_GET_SEC_FLOW_HANDLE()
      while (handle != NULL)
      {
        PS_IFACE_GET_SEC_FLOW_BY_HANDLE()
        handle = new_handle
      }
    TASKFREE()

SIDE EFFECTS
  None.
===========================================================================*/
void *ps_iface_get_sec_flow_handle
(
  ps_iface_type  * iface_ptr
)
{
  void  * flow_handle;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!PS_IFACE_IS_VALID(iface_ptr))
  {
    LOG_MSG_ERROR_1("ps_iface_get_sec_flow_handle(): "
                    "Iface, 0x%p is invalid", iface_ptr);
    ASSERT(0);
    return NULL;
  }

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  flow_handle =
    list_peek_front(&(iface_ptr->iface_private.flow.sec_flow_list));

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return flow_handle;

} /* ps_iface_get_sec_flow_handle() */



/*===========================================================================
FUNCTION PS_IFACE_GET_SEC_FLOW_BY_HANDLE()

DESCRIPTION
  Given a handle, returns a secondary flow. A new handle is returned so that
  next secondary flow is fetched when this function is invoked with new
  handle.

PARAMETERS
  iface_ptr       : pointer to the iface.
  sec_flow_handle : handle using which next secondary flow is fetched
  sec_flow_ptr    : pointer to fetched secondary flow (OUT param)
  next_handle     : new handle which can be used to fetch next secondary
                    flow (OUT param)

RETURN VALUE
  TRUE  : on success
  FALSE : on failure

DEPENDENCIES
  All flows must be fetched in one atomic operation. Otherwise flows
  could be deleted while client is fetching them which could lead to
  incorrect operations.

  Do not try to access flows contents outside of atomic operation. Since
  a flow can be deleted, this may lead to memory corruption or data aborts.

  Sample usage of this API is
    TASKLOCK()
      handle = PS_IFACE_GET_SEC_FLOW_HANDLE()
      while (handle != NULL)
      {
        PS_IFACE_GET_SEC_FLOW_BY_HANDLE()
        handle = new_handle
      }
    TASKFREE()

SIDE EFFECTS
  next_handle is updated so that if this function is called with that
  handle, next secondary flow is fetched
===========================================================================*/
boolean ps_iface_get_sec_flow_by_handle
(
  ps_iface_type    * iface_ptr,
  void             * sec_flow_handle,
  ps_flow_type    ** sec_flow_ptr,
  void            ** next_handle
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (!PS_IFACE_IS_VALID(iface_ptr) || sec_flow_ptr == NULL ||
      next_handle == NULL)
  {
    DATA_ERR_FATAL("ps_iface_get_sec_flow_by_handle(): Invalid parameters are passed");
    return FALSE;
  }

  if (sec_flow_handle == NULL)
  {
    *next_handle  = NULL;
    *sec_flow_ptr = NULL;

    return FALSE;
  }

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  *sec_flow_ptr = (ps_flow_type *) sec_flow_handle;
  if (!ps_mem_is_valid(*sec_flow_ptr, PS_MEM_PS_FLOW_TYPE))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_iface_get_sec_flow_by_handle(): "
                    "Buffer (0x%p) doesn't belong to ps_flow's mem pool",
                    sec_flow_handle);
    ASSERT(0);
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Get next element from secondary flow list and update next_handle
  -------------------------------------------------------------------------*/
  *next_handle = list_peek_next(&(iface_ptr->iface_private.flow.sec_flow_list),
                                &((*sec_flow_ptr)->link));

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return TRUE;

} /* ps_iface_get_sec_flow_by_handle() */



/*===========================================================================
FUNCTION PS_IFACE_FILTERING_CONTROL()

DESCRIPTION
  This function enables or disables IP packet filtering over a flow.
  When enabled, all filters installed for this flow are enabled and
  traffic may get routed over this flow. When disbaled, any such traffic
  will go over default flow.

PARAMETERS
  iface_ptr : iface where filters are installed
  flow_ptr  : ptr to flow to which owns these filters
  enable    : Whether to enable or disable filtering on this flow

RETURN VALUE
   0 : on success
  -1 : on failure

DEPENDENCIES
  flow_ptr must not be a default flow, no filtering is performed on default
  flow as it always carries default traffic.

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_filtering_control
(
  ps_iface_type    * iface_ptr,
  ps_flow_type     * flow_ptr,
  boolean            enable,
  int16            * ps_errno
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (ps_errno == NULL)
  {
    DATA_ERR_FATAL("ps_iface_filtering_control(): NULL parameter is passed");
    return -1;
  }

  if (!PS_IFACE_IS_VALID(iface_ptr))
  {
    LOG_MSG_ERROR_1("ps_iface_filtering_control(): "
                    "Iface 0x%p is invalid", iface_ptr);
    ASSERT(0);
    return FALSE;
  }

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  if (!PS_FLOW_IS_VALID(flow_ptr))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_iface_filtering_control(): "
                    "Invalid flow ptr, 0x%p, passed", flow_ptr);
    ASSERT(0);
    return FALSE;
  }

  if (PS_FLOWI_GET_CAPABILITY(flow_ptr, PS_FLOW_CAPABILITY_DEFAULT))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_iface_filtering_control(): "
                    "Filters are not installed on default flow, 0x%p",
                    flow_ptr);
    ASSERT(0);
    *ps_errno = DS_EFAULT;
    return -1;
  }

  /*-------------------------------------------------------------------------
    Enable/disable Tx filtering for this flow
  -------------------------------------------------------------------------*/
  if (ps_iface_ipfltr_control
      (
        iface_ptr,
        IP_FLTR_CLIENT_QOS_OUTPUT,
        PS_FLOWI_GET_TX_FLTR_HANDLE(flow_ptr),
        enable,
        ps_errno
      ) == -1)
  {
    LOG_MSG_ERROR_3("ps_iface_filtering_control(): "
                    "Unable to perform control operation on Tx filters on "
                    "iface 0x%x:%d, fltr handle is 0x%x",
                    iface_ptr->name,
                    iface_ptr->instance,
                    PS_FLOWI_GET_TX_FLTR_HANDLE(flow_ptr));
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    return -1;
  }

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return 0;

} /* ps_iface_filtering_control() */



/*===========================================================================
FUNCTION PS_IFACE_GET_TX_FLTR_CNT()

DESCRIPTION
  Returns number of Tx filters, that match the IP version, specified as an
  argument. Tx filters can pertain to either QOS_REQUEST/active QoS or
  pending QOS_MODIFY

PARAMETERS
  ps_iface_ptr : iface where filters are stored
  flow_ptr     : pointer to the ps_flow whose filters need to be fetched
  ip_vsn       : set of filters, a client is interested in. Mix of V4 and
                 V6 filters can be installed on a flow and client may
                 choose to get only V4 filters
  is_modify    : is the client interested in fetching Tx filters associated
                 with pending QOS_MODIFY? If so value must be TRUE

RETURN VALUE
  Number of Tx filters : on success
  -1                   : otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int8 ps_iface_get_tx_fltr_cnt
(
  ps_iface_type         * ps_iface_ptr,
  ps_flow_type          * flow_ptr,
  ip_version_enum_type    ip_vsn,
  boolean                 is_modify
)
{
  ps_iface_ipfilteri_type      * curr_fltr_buf;
  q_type                       * fltr_q_ptr;
  ps_iface_ipfltr_handle_type    tx_fltr_handle;
  int8                           tx_fltr_cnt = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!PS_IFACE_IS_VALID(ps_iface_ptr))
  {
    DATA_ERR_FATAL("ps_iface_get_tx_fltr_cnt(): Invalid parameters are passed");
    return -1;
  }

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  if (!PS_FLOW_IS_VALID(flow_ptr))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    DATA_ERR_FATAL("ps_iface_get_tx_fltr_cnt(): Invalid parameters are passed");
    return -1;
  }

  if (PS_FLOWI_GET_CAPABILITY(flow_ptr, PS_FLOW_CAPABILITY_DEFAULT))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_iface_get_tx_fltr_cnt(): "
                    "ps_flow, 0x%p is a default flow and it doesn't have "
                    "Tx filters", flow_ptr);
    ASSERT(0);
    return -1;
  }

  /*-------------------------------------------------------------------------
    Fetch appropriate handle based on whether client is interested in
    pending QOS_MODIFY
  -------------------------------------------------------------------------*/
  if (is_modify)
  {
    if (!PS_FLOWI_IS_QOS_MODIFY_IN_PROGRESS(flow_ptr))
    {
      PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      LOG_MSG_ERROR_1("ps_iface_get_tx_fltr_cnt(): "
                      "QOS_MODIFY is not pending on ps_flow, 0x%p", flow_ptr);
      ASSERT(0);
      return -1;
    }

    tx_fltr_handle = PS_FLOWI_GET_MODIFY_TX_FLTR_HANDLE(flow_ptr);
  }
  else
  {
    tx_fltr_handle = PS_FLOWI_GET_TX_FLTR_HANDLE(flow_ptr);
  }

  /*-------------------------------------------------------------------------
    Count the number of filters which match the given IP version. Consider
    only those filters that belong to given ps_flow
  -------------------------------------------------------------------------*/
  fltr_q_ptr =
    &(ps_iface_ptr->iface_private.ipfltr_info[IP_FLTR_CLIENT_QOS_OUTPUT]);
  curr_fltr_buf = (ps_iface_ipfilteri_type *) q_check(fltr_q_ptr);;

  while (curr_fltr_buf != NULL)
  {
    if (curr_fltr_buf->fi_handle == tx_fltr_handle &&
        curr_fltr_buf->filter.fi_default.ip_vsn == ip_vsn)
    {
      tx_fltr_cnt++;
    }

    curr_fltr_buf = q_next(fltr_q_ptr, &(curr_fltr_buf->link));
  }

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return tx_fltr_cnt;

} /* ps_iface_get_tx_fltr_cnt() */

/*===========================================================================
FUNCTION PS_IFACE_FLOW_GET_TX_FLTR_CNT_EX()

DESCRIPTION
  Returns number of Tx filters, specified as an argument.
  Tx filters can pertain to either QOS_REQUEST/active QoS or
  pending QOS_MODIFY

PARAMETERS
  ps_iface_ptr : iface where filters are stored
  flow_ptr     : pointer to the ps_flow whose filters need to be fetched
  is_modify    : is the client interested in fetching Tx filters associated
                 with pending QOS_MODIFY? If so value must be TRUE

RETURN VALUE
  Number of Tx filters : on success
  -1                   : otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int8 ps_iface_flow_get_tx_fltr_cnt_ex
(
  ps_iface_type         * ps_iface_ptr,
  ps_flow_type          * flow_ptr,
  boolean                 is_modify
)
{
  ps_iface_ipfilteri_type      * curr_fltr_buf;
  q_type                       * fltr_q_ptr;
  ps_iface_ipfltr_handle_type    tx_fltr_handle;
  int8                           tx_fltr_cnt = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!PS_IFACE_IS_VALID(ps_iface_ptr))
  {
    DATA_ERR_FATAL("ps_iface_flow_get_tx_fltr_cnt_ex(): Invalid parameters are passed");
    return -1;
  }

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  if (!PS_FLOW_IS_VALID(flow_ptr))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    DATA_ERR_FATAL("ps_iface_flow_get_tx_fltr_cnt_ex(): Invalid parameters are passed");
    return -1;
  }

  if (PS_FLOWI_GET_CAPABILITY(flow_ptr, PS_FLOW_CAPABILITY_DEFAULT))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_iface_flow_get_tx_fltr_cnt_ex(): "
                    "ps_flow, 0x%p is a default flow and it doesn't have "
                    "Tx filters", flow_ptr);
    ASSERT(0);
    return -1;
  }

  /*-------------------------------------------------------------------------
    Fetch appropriate handle based on whether client is interested in
    pending QOS_MODIFY
  -------------------------------------------------------------------------*/
  if (is_modify)
  {
    if (!PS_FLOWI_IS_QOS_MODIFY_IN_PROGRESS(flow_ptr))
    {
      PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      LOG_MSG_ERROR_1("ps_iface_flow_get_tx_fltr_cnt_ex(): "
                      "QOS_MODIFY is not pending on ps_flow, 0x%p", flow_ptr);
      ASSERT(0);
      return -1;
    }

    tx_fltr_handle = PS_FLOWI_GET_MODIFY_TX_FLTR_HANDLE(flow_ptr);
  }
  else
  {
    tx_fltr_handle = PS_FLOWI_GET_TX_FLTR_HANDLE(flow_ptr);
  }

  /*-------------------------------------------------------------------------
    Count the number of filters which match the given IP version. Consider
    only those filters that belong to given ps_flow
  -------------------------------------------------------------------------*/
  fltr_q_ptr =
    &(ps_iface_ptr->iface_private.ipfltr_info[IP_FLTR_CLIENT_QOS_OUTPUT]);

  curr_fltr_buf = (ps_iface_ipfilteri_type *) q_check(fltr_q_ptr);
  while (curr_fltr_buf != NULL)
  {
    if (curr_fltr_buf->fi_handle == tx_fltr_handle)
    {
      tx_fltr_cnt++;
    }

    curr_fltr_buf = q_next(fltr_q_ptr, &(curr_fltr_buf->link));
  }

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return tx_fltr_cnt;

} /* ps_iface_flow_get_tx_fltr_cnt_ex() */

/*===========================================================================
FUNCTION PS_IFACEI_GET_TX_FLTR_HANDLE_EX()

DESCRIPTION
  This function returns a handle which can be used to fetch Tx filters
  associated with a ps_flow independent of the IP version.

  Tx filters can pertain to either QOS_REQUEST/active QOS or pending
  QOS_MODIFY

PARAMETERS
  ps_iface_ptr : iface where filters are stored
  flow_ptr     : pointer to the ps_flow whose filters need to be fetched
  is_modify    : is the client interested in fetching Tx filters associated
                 with pending QOS_MODIFY? If so value must be TRUE

RETURN VALUE
  A handle using which Tx filters can be fetched : on success
  NULL                                           : otherwise

DEPENDENCIES
  All filters must be fetched in one atomic operation. Otherwise filters
  could be deleted (for example because QOS_MODIFY is accepted/rejected)
  while client is fetching them which could lead to incorrect operations

  Sample usage to fetch Tx filters pertaining to QOS_REQUEST is
    TASKLOCK()
      handle = ps_ifacei_get_tx_fltr_handle_ex(iface_ptr, flow_ptr, FALSE)
      while (handle != NULL)
      {
        ps_ifacei_get_tx_fltr_by_handle_ex(iface_ptr, flow_ptr, FALSE,
                                       handle, fltr, prcd, new_handle)
        handle = new_handle
      }
    TASKFREE()

SIDE EFFECTS
  None
===========================================================================*/
void *ps_ifacei_get_tx_fltr_handle_ex
(
  ps_iface_type         * ps_iface_ptr,
  ps_flow_type          * flow_ptr,
  boolean                 is_modify
)
{
  ps_iface_ipfilteri_type      * curr_fltr_buf;
  q_type                       * fltr_q_ptr;
  ps_iface_ipfltr_handle_type    tx_fltr_handle;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!PS_IFACE_IS_VALID(ps_iface_ptr))
  {
    LOG_MSG_ERROR_1("ps_ifacei_get_tx_fltr_handle_ex(): "
                    "Invalid Iface ptr, 0x%p, is passed", ps_iface_ptr);
    ASSERT(0);
    return NULL;
  }

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  if (!PS_FLOW_IS_VALID(flow_ptr))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_ifacei_get_tx_fltr_handle_ex(): "
                    "Invalid ps_flow, 0x%p, is passed", flow_ptr);
    ASSERT(0);
    return NULL;
  }

  if (PS_FLOWI_GET_CAPABILITY(flow_ptr, PS_FLOW_CAPABILITY_DEFAULT))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_ifacei_get_tx_fltr_handle_ex(): "
                    "ps_flow, 0x%p is a default flow and it doesn't have "
                    "Tx filters", flow_ptr);
    ASSERT(0);
    return NULL;
  }

  /*-------------------------------------------------------------------------
    Fetch appropriate handle based on whether client is interested in
    pending QOS_MODIFY
  -------------------------------------------------------------------------*/
  if (is_modify)
  {
    if (!PS_FLOWI_IS_QOS_MODIFY_IN_PROGRESS(flow_ptr))
    {
      PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      LOG_MSG_ERROR_1("ps_ifacei_get_tx_fltr_handle_ex(): "
                      "QOS_MODIFY is not pending on ps_flow, 0x%p", flow_ptr);
      ASSERT(0);
      return NULL;
    }

    tx_fltr_handle = PS_FLOWI_GET_MODIFY_TX_FLTR_HANDLE(flow_ptr);
  }
  else
  {
    tx_fltr_handle = PS_FLOWI_GET_TX_FLTR_HANDLE(flow_ptr);
  }

  /*-------------------------------------------------------------------------
    Return a handle to the filter. Consider only those filters
    that belong to given ps_flow
  -------------------------------------------------------------------------*/
  fltr_q_ptr =
    &(ps_iface_ptr->iface_private.ipfltr_info[IP_FLTR_CLIENT_QOS_OUTPUT]);
  curr_fltr_buf = (ps_iface_ipfilteri_type *) q_check(fltr_q_ptr);

  while (curr_fltr_buf != NULL &&
         (curr_fltr_buf->fi_handle != tx_fltr_handle))
  {
    curr_fltr_buf = q_next(fltr_q_ptr, &(curr_fltr_buf->link));
  }

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return curr_fltr_buf;

} /* ps_ifacei_get_tx_fltr_handle_ex() */

/*===========================================================================
FUNCTION PS_IFACEI_GET_TX_FLTR_BY_HANDLE_EX()

DESCRIPTION
  Given a handle, this function returns a Tx filter, and its precedence
  independent of the IP version.
  Currently 0 is returned as precedence for all Tx filters.

  Tx filters can pertain to either QOS_REQUEST/active QOS or pending
  QOS_MODIFY

PARAMETERS
  ps_iface_ptr   : iface where filters are stored
  flow_ptr       : pointer to the flow
  tx_fltr_handle : handle using which a filter is fetched
  is_modify      : is the client interested in fetching Tx filters associated
                   with pending QOS_MODIFY? If so value must be TRUE
  fltr           : OUT PARAM, pointer to Tx filter
  prcd           : OUT PARAM, precedence of Tx filter
  next_handle    : OUT PARAM, new handle which can be used to fetch next
                   Tx filter

RETURN VALUE
  TRUE  : on success
  FALSE : otherwise

DEPENDENCIES
  All filters must be fetched in one atomic operation. Otherwise filters
  could be deleted (for example because QOS_MODIFY is accepted/rejected)
  while client is fetching them which could lead to incorrect operations.

  Do not try to access filter contents outside of atomic operation. Since
  a filter can be deleted, this may lead to memory corruption or data aborts.

  Sample usage to fetch Tx filters pertaining to QOS_REQUEST is
    TASKLOCK()
      handle = ps_ifacei_get_tx_fltr_handle_ex(iface_ptr, flow_ptr, FALSE)
      while (handle != NULL)
      {
        ps_ifacei_get_tx_fltr_by_handle_ex(iface_ptr, flow_ptr, FALSE,
                                       handle, fltr, prcd, new_handle)
        handle = new_handle
      }
    TASKFREE()

SIDE EFFECTS
  next_handle is set so that if this function is called with that handle,
  next Tx filter is fetched
===========================================================================*/
boolean ps_ifacei_get_tx_fltr_by_handle_ex
(
  ps_iface_type          * ps_iface_ptr,
  ps_flow_type           * flow_ptr,
  void                   * tx_fltr_handle,
  boolean                  is_modify,
  ip_filter_type        ** fltr,
  uint8                  * prcd,
  void                  ** next_handle
)
{
  ps_iface_ipfilteri_type      * tx_fltr_buf;
  ps_iface_ipfilteri_type      * tmp_tx_fltr_buf;
  q_type                       * fltr_q_ptr;
  ps_iface_ipfltr_handle_type    fltr_handle;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!PS_IFACE_IS_VALID(ps_iface_ptr) || fltr == NULL || prcd == NULL ||
      next_handle == NULL)
  {
    DATA_ERR_FATAL("ps_ifacei_get_tx_fltr_by_handle_ex(): Invalid parameters are passed");
    return FALSE;
  }

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  if (!PS_FLOW_IS_VALID(flow_ptr))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_ifacei_get_tx_fltr_by_handle_ex(): "
                    "Invalid ps_flow, 0x%p, is passed", flow_ptr);
    ASSERT(0);
    return FALSE;
  }

  if (PS_FLOWI_GET_CAPABILITY(flow_ptr, PS_FLOW_CAPABILITY_DEFAULT))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_ifacei_get_tx_fltr_by_handle_ex(): "
                    "ps_flow, 0x%p is a default flow and it doesn't have "
                    "Tx filters", flow_ptr);
    ASSERT(0);
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Fetch appropriate handle based on whether client is interested in
    pending QOS_MODIFY
  -------------------------------------------------------------------------*/
  if (is_modify)
  {
    if (!PS_FLOWI_IS_QOS_MODIFY_IN_PROGRESS(flow_ptr))
    {
      PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      LOG_MSG_ERROR_1("ps_ifacei_get_tx_fltr_by_handle_ex(): "
                      "QOS_MODIFY is not pending on ps_flow, 0x%p", flow_ptr);
      ASSERT(0);
      return FALSE;
    }

    fltr_handle = PS_FLOWI_GET_MODIFY_TX_FLTR_HANDLE(flow_ptr);
  }
  else
  {
    fltr_handle = PS_FLOWI_GET_TX_FLTR_HANDLE(flow_ptr);
  }

  if (tx_fltr_handle == NULL)
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);

    *next_handle = NULL;
    *fltr        = NULL;
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Make sure that the passed handle belongs to ps_mem pool from which
    Tx fltrs are allocated
  -------------------------------------------------------------------------*/
  tx_fltr_buf = (ps_iface_ipfilteri_type *) tx_fltr_handle;
  if (!ps_mem_is_valid(tx_fltr_buf, PS_MEM_PS_IFACE_IPFLTER_TYPE))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_ifacei_get_tx_fltr_by_handle_ex(): "
                    "Buffer (0x%X) doesn't belong to Tx filter's mem pool",
                    tx_fltr_handle);
    ASSERT(0);
    return FALSE;
  }

  if (tx_fltr_buf->fi_handle != fltr_handle)
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    DATA_ERR_FATAL("ps_ifacei_get_tx_fltr_by_handle_ex(): Invalid handle is passed");
    return FALSE;
  }

  *fltr = &(tx_fltr_buf->filter.fi_default);
  *prcd = 0;

  /*-------------------------------------------------------------------------
    Update handle to the filter. Consider only those fltrs which
    belong to given ps_flow
  -------------------------------------------------------------------------*/
  fltr_q_ptr =
    &(ps_iface_ptr->iface_private.ipfltr_info[IP_FLTR_CLIENT_QOS_OUTPUT]);

  tmp_tx_fltr_buf = q_next(fltr_q_ptr, &(tx_fltr_buf->link));

  while (tmp_tx_fltr_buf != NULL &&
         (tmp_tx_fltr_buf->fi_handle != fltr_handle))
  {
    tmp_tx_fltr_buf = q_next(fltr_q_ptr, &(tmp_tx_fltr_buf->link));
  }

  *next_handle = tmp_tx_fltr_buf;

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return TRUE;

} /* ps_ifacei_get_tx_fltr_by_handle_ex() */


/*===========================================================================
FUNCTION PS_IFACE_GET_TX_FLTR_HANDLE()

DESCRIPTION
  This function returns a handle which can be used to fetch Tx filters
  associated with a ps_flow. Only those filters which match IP version,
  specified as an argument, are returned

  Tx filters can pertain to either QOS_REQUEST/active QOS or pending
  QOS_MODIFY

PARAMETERS
  ps_iface_ptr : iface where filters are stored
  flow_ptr     : pointer to the ps_flow whose filters need to be fetched
  ip_vsn       : set of filters, a client is interested in. Mix of V4 and
                 V6 filters can be installed on a flow and client may
                 choose to get only V4 filters
  is_modify    : is the client interested in fetching Tx filters associated
                 with pending QOS_MODIFY? If so value must be TRUE

RETURN VALUE
  A handle using which Tx filters can be fetched : on success
  NULL                                           : otherwise

DEPENDENCIES
  All filters must be fetched in one atomic operation. Otherwise filters
  could be deleted (for example because QOS_MODIFY is accepted/rejected)
  while client is fetching them which could lead to incorrect operations

  Sample usage to fetch Tx IPV4 filters pertaining to QOS_REQUEST is
    TASKLOCK()
      handle = ps_iface_get_tx_fltr_handle(iface_ptr, flow_ptr, IPV4, FALSE)
      while (handle != NULL)
      {
        ps_iface_get_tx_fltr_by_handle(iface_ptr, flow_ptr, IPV4, FALSE,
                                       handle, fltr, prcd, new_handle)
        handle = new_handle
      }
    TASKFREE()

SIDE EFFECTS
  None
===========================================================================*/
void *ps_iface_get_tx_fltr_handle
(
  ps_iface_type         * ps_iface_ptr,
  ps_flow_type          * flow_ptr,
  ip_version_enum_type    ip_vsn,
  boolean                 is_modify
)
{
  ps_iface_ipfilteri_type      * curr_fltr_buf;
  q_type                       * fltr_q_ptr;
  ps_iface_ipfltr_handle_type    tx_fltr_handle;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!PS_IFACE_IS_VALID(ps_iface_ptr))
  {
    DATA_ERR_FATAL("ps_iface_get_tx_fltr_handle(): Invalid parameters are passed");
    return NULL;
  }

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  if (!PS_FLOW_IS_VALID(flow_ptr))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    DATA_ERR_FATAL("ps_iface_get_tx_fltr_handle(): Invalid parameters are passed");
    return NULL;
  }

  if (PS_FLOWI_GET_CAPABILITY(flow_ptr, PS_FLOW_CAPABILITY_DEFAULT))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_iface_get_tx_fltr_handle(): "
                    "ps_flow, 0x%p is a default flow and it doesn't have "
                    "Tx filters", flow_ptr);
    ASSERT(0);
    return NULL;
  }

  /*-------------------------------------------------------------------------
    Fetch appropriate handle based on whether client is interested in
    pending QOS_MODIFY
  -------------------------------------------------------------------------*/
  if (is_modify)
  {
    if (!PS_FLOWI_IS_QOS_MODIFY_IN_PROGRESS(flow_ptr))
    {
      PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      LOG_MSG_ERROR_1("ps_iface_get_tx_fltr_handle(): "
                      "QOS_MODIFY is not pending on ps_flow, 0x%p", flow_ptr);
      ASSERT(0);
      return NULL;
    }

    tx_fltr_handle = PS_FLOWI_GET_MODIFY_TX_FLTR_HANDLE(flow_ptr);
  }
  else
  {
    tx_fltr_handle = PS_FLOWI_GET_TX_FLTR_HANDLE(flow_ptr);
  }

  /*-------------------------------------------------------------------------
    Return a handle to the filter which matches IP version. Consider only
    those filters that belong to given ps_flow
  -------------------------------------------------------------------------*/
  fltr_q_ptr =
    &(ps_iface_ptr->iface_private.ipfltr_info[IP_FLTR_CLIENT_QOS_OUTPUT]);
  curr_fltr_buf = (ps_iface_ipfilteri_type *) q_check(fltr_q_ptr);

  while (curr_fltr_buf != NULL &&
         (curr_fltr_buf->fi_handle != tx_fltr_handle ||
            curr_fltr_buf->filter.fi_default.ip_vsn != ip_vsn))
  {
    curr_fltr_buf = q_next(fltr_q_ptr, &(curr_fltr_buf->link));
  }

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return curr_fltr_buf;

} /* ps_iface_get_tx_fltr_handle() */

/*===========================================================================
FUNCTION PS_IFACE_FLOW_GET_TX_FLTR_HANDLE_EX()

DESCRIPTION
  This function returns a handle which can be used to fetch Tx filters
  associated with a ps_flow independent of the IP version.

  Tx filters can pertain to either QOS_REQUEST/active QOS or pending
  QOS_MODIFY

PARAMETERS
  ps_iface_ptr : iface where filters are stored
  flow_ptr     : pointer to the ps_flow whose filters need to be fetched
  is_modify    : is the client interested in fetching Tx filters associated
                 with pending QOS_MODIFY? If so value must be TRUE

RETURN VALUE
  A handle using which Tx filters can be fetched : on success
  NULL                                           : otherwise

DEPENDENCIES
  All filters must be fetched in one atomic operation. Otherwise filters
  could be deleted (for example because QOS_MODIFY is accepted/rejected)
  while client is fetching them which could lead to incorrect operations

  Sample usage to fetch Tx filters pertaining to QOS_REQUEST is
    TASKLOCK()
      handle = ps_iface_flow_get_tx_fltr_handle_ex(iface_ptr, flow_ptr, FALSE)
      while (handle != NULL)
      {
        ps_iface_flow_get_tx_fltr_by_handle_ex(iface_ptr, flow_ptr, FALSE,
                                       handle, fltr, prcd, new_handle)
        handle = new_handle
      }
    TASKFREE()

SIDE EFFECTS
  None
===========================================================================*/
void *ps_iface_flow_get_tx_fltr_handle_ex
(
  ps_iface_type         * ps_iface_ptr,
  ps_flow_type          * flow_ptr,
  boolean                 is_modify
)
{
  ps_iface_ipfilteri_type      * curr_fltr_buf;
  q_type                       * fltr_q_ptr;
  ps_iface_ipfltr_handle_type    tx_fltr_handle;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!PS_IFACE_IS_VALID(ps_iface_ptr))
  {
    LOG_MSG_ERROR_1("ps_iface_flow_get_tx_fltr_handle_ex(): "
                    "Invalid Iface ptr, 0x%p, is passed", ps_iface_ptr);
    ASSERT(0);
    return NULL;
  }

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  if (!PS_FLOW_IS_VALID(flow_ptr))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_iface_flow_get_tx_fltr_handle_ex(): "
                    "Invalid ps_flow, 0x%p, is passed", flow_ptr);
    ASSERT(0);
    return NULL;
  }

  if (PS_FLOWI_GET_CAPABILITY(flow_ptr, PS_FLOW_CAPABILITY_DEFAULT))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_iface_flow_get_tx_fltr_handle_ex(): "
                    "ps_flow, 0x%p is a default flow and it doesn't have "
                    "Tx filters", flow_ptr);
    ASSERT(0);
    return NULL;
  }

  /*-------------------------------------------------------------------------
    Fetch appropriate handle based on whether client is interested in
    pending QOS_MODIFY
  -------------------------------------------------------------------------*/
  if (is_modify)
  {
    if (!PS_FLOWI_IS_QOS_MODIFY_IN_PROGRESS(flow_ptr))
    {
      PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      LOG_MSG_ERROR_1("ps_iface_flow_get_tx_fltr_handle_ex(): "
                      "QOS_MODIFY is not pending on ps_flow, 0x%p", flow_ptr);
      ASSERT(0);
      return NULL;
    }

    tx_fltr_handle = PS_FLOWI_GET_MODIFY_TX_FLTR_HANDLE(flow_ptr);
  }
  else
  {
    tx_fltr_handle = PS_FLOWI_GET_TX_FLTR_HANDLE(flow_ptr);
  }

  /*-------------------------------------------------------------------------
    Return a handle to the filter. Consider only those filters
    that belong to given ps_flow
  -------------------------------------------------------------------------*/
  fltr_q_ptr =
    &(ps_iface_ptr->iface_private.ipfltr_info[IP_FLTR_CLIENT_QOS_OUTPUT]);
  curr_fltr_buf = (ps_iface_ipfilteri_type *) q_check(fltr_q_ptr);

  while (curr_fltr_buf != NULL &&
         (curr_fltr_buf->fi_handle != tx_fltr_handle))
  {
    curr_fltr_buf = q_next(fltr_q_ptr, &(curr_fltr_buf->link));
  }

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return curr_fltr_buf;

} /* ps_iface_flow_get_tx_fltr_handle_ex() */


/*===========================================================================
FUNCTION PS_IFACE_GET_TX_FLTR_BY_HANDLE()

DESCRIPTION
  Given a handle, this function returns a Tx filter, and its precedence.
  Currently 0 is returned as precedence for all Tx filters. Only those
  filters which match IP version, specified as an argument, are returned

  Tx filters can pertain to either QOS_REQUEST/active QOS or pending
  QOS_MODIFY

PARAMETERS
  ps_iface_ptr   : iface where filters are stored
  flow_ptr       : pointer to the flow
  ip_vsn         : set of filters, a client is interested in. Mix of V4 and
                   V6 filters can be installed on a flow and client may
                   choose to get only V4 filters
  tx_fltr_handle : handle using which a filter is fetched
  is_modify      : is the client interested in fetching Tx filters associated
                   with pending QOS_MODIFY? If so value must be TRUE
  fltr           : OUT PARAM, pointer to Tx filter
  prcd           : OUT PARAM, precedence of Tx filter
  next_handle    : OUT PARAM, new handle which can be used to fetch next
                   Tx filter

RETURN VALUE
  TRUE  : on success
  FALSE : otherwise

DEPENDENCIES
  All filters must be fetched in one atomic operation. Otherwise filters
  could be deleted (for example because QOS_MODIFY is accepted/rejected)
  while client is fetching them which could lead to incorrect operations.

  Do not try to access filter contents outside of atomic operation. Since
  a filter can be deleted, this may lead to memory corruption or data aborts.

  Sample usage to fetch Tx IPV4 filters pertaining to QOS_REQUEST is
    TASKLOCK()
      handle = ps_iface_get_tx_fltr_handle(iface_ptr, flow_ptr, IPV4, FALSE)
      while (handle != NULL)
      {
        ps_iface_get_tx_fltr_by_handle(iface_ptr, flow_ptr, IPV4, FALSE,
                                       handle, fltr, prcd, new_handle)
        handle = new_handle
      }
    TASKFREE()

SIDE EFFECTS
  next_handle is set so that if this function is called with that handle,
  next Tx filter is fetched
===========================================================================*/
boolean ps_iface_get_tx_fltr_by_handle
(
  ps_iface_type          * ps_iface_ptr,
  ps_flow_type           * flow_ptr,
  ip_version_enum_type     ip_vsn,
  void                   * tx_fltr_handle,
  boolean                  is_modify,
  ip_filter_type        ** fltr,
  uint8                  * prcd,
  void                  ** next_handle
)
{
  ps_iface_ipfilteri_type      * tx_fltr_buf;
  ps_iface_ipfilteri_type      * tmp_tx_fltr_buf;
  q_type                       * fltr_q_ptr;
  ps_iface_ipfltr_handle_type    fltr_handle;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!PS_IFACE_IS_VALID(ps_iface_ptr) || fltr == NULL || prcd == NULL ||
      next_handle == NULL)
  {
    DATA_ERR_FATAL("ps_iface_get_tx_fltr_by_handle(): Invalid parameters are passed");
    return FALSE;
  }

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  if (!PS_FLOW_IS_VALID(flow_ptr))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    DATA_ERR_FATAL("ps_iface_get_tx_fltr_by_handle(): Invalid parameters are passed");
    return FALSE;
  }

  if (PS_FLOWI_GET_CAPABILITY(flow_ptr, PS_FLOW_CAPABILITY_DEFAULT))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_iface_get_tx_fltr_by_handle(): "
                    "ps_flow, 0x%p is a default flow and it doesn't have "
                    "Tx filters", flow_ptr);
    ASSERT(0);
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Fetch appropriate handle based on whether client is interested in
    pending QOS_MODIFY
  -------------------------------------------------------------------------*/
  if (is_modify)
  {
    if (!PS_FLOWI_IS_QOS_MODIFY_IN_PROGRESS(flow_ptr))
    {
      PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      LOG_MSG_ERROR_1("ps_iface_get_tx_fltr_by_handle(): "
                      "QOS_MODIFY is not pending on ps_flow, 0x%p", flow_ptr);
      ASSERT(0);
      return FALSE;
    }

    fltr_handle = PS_FLOWI_GET_MODIFY_TX_FLTR_HANDLE(flow_ptr);
  }
  else
  {
    fltr_handle = PS_FLOWI_GET_TX_FLTR_HANDLE(flow_ptr);
  }

  if (tx_fltr_handle == NULL)
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);

    *next_handle = NULL;
    *fltr        = NULL;
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Make sure that the passed handle belongs to ps_mem pool from which
    Tx fltrs are allocated
  -------------------------------------------------------------------------*/
  tx_fltr_buf = (ps_iface_ipfilteri_type *) tx_fltr_handle;
  if (!ps_mem_is_valid(tx_fltr_buf, PS_MEM_PS_IFACE_IPFLTER_TYPE))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_iface_get_tx_fltr_by_handle(): "
                    "Buffer (0x%X) doesn't belong to Tx filter's mem pool",
                    tx_fltr_handle);
    ASSERT(0);
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Make sure that API is used correctly. Its invalid to get handle for a
    specific (ip_vsn, is_modify) combination and use it for another
    (ip_vsn, is_modify) combination
  -------------------------------------------------------------------------*/
  if (tx_fltr_buf->fi_handle != fltr_handle ||
      tx_fltr_buf->filter.fi_default.ip_vsn != ip_vsn)
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    DATA_ERR_FATAL("ps_iface_get_tx_fltr_by_handle(): Invalid handle is passed");
    return FALSE;
  }

  *fltr = &(tx_fltr_buf->filter.fi_default);
  *prcd = 0;

  /*-------------------------------------------------------------------------
    Update handle to the filter which matches IP version. Consider only
    those fltrs which belong to given ps_flow
  -------------------------------------------------------------------------*/
  fltr_q_ptr =
    &(ps_iface_ptr->iface_private.ipfltr_info[IP_FLTR_CLIENT_QOS_OUTPUT]);

  tmp_tx_fltr_buf = q_next(fltr_q_ptr, &(tx_fltr_buf->link));

  while (tmp_tx_fltr_buf != NULL &&
         (tmp_tx_fltr_buf->fi_handle != fltr_handle ||
            tmp_tx_fltr_buf->filter.fi_default.ip_vsn != ip_vsn))
  {
    tmp_tx_fltr_buf = q_next(fltr_q_ptr, &(tmp_tx_fltr_buf->link));
  }

  *next_handle = tmp_tx_fltr_buf;

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return TRUE;

} /* ps_iface_get_tx_fltr_by_handle() */


/*===========================================================================
FUNCTION PS_IFACE_FLOW_GET_TX_FLTR_BY_HANDLE_EX()

DESCRIPTION
  Given a handle, this function returns a Tx filter, and its precedence
  independent of the IP version.
  Currently 0 is returned as precedence for all Tx filters.

  Tx filters can pertain to either QOS_REQUEST/active QOS or pending
  QOS_MODIFY

PARAMETERS
  ps_iface_ptr   : iface where filters are stored
  flow_ptr       : pointer to the flow
  tx_fltr_handle : handle using which a filter is fetched
  is_modify      : is the client interested in fetching Tx filters associated
                   with pending QOS_MODIFY? If so value must be TRUE
  fltr           : OUT PARAM, pointer to Tx filter
  prcd           : OUT PARAM, precedence of Tx filter
  next_handle    : OUT PARAM, new handle which can be used to fetch next
                   Tx filter

RETURN VALUE
  TRUE  : on success
  FALSE : otherwise

DEPENDENCIES
  All filters must be fetched in one atomic operation. Otherwise filters
  could be deleted (for example because QOS_MODIFY is accepted/rejected)
  while client is fetching them which could lead to incorrect operations.

  Do not try to access filter contents outside of atomic operation. Since
  a filter can be deleted, this may lead to memory corruption or data aborts.

  Sample usage to fetch Tx filters pertaining to QOS_REQUEST is
    TASKLOCK()
      handle = PS_IFACE_FLOW_GET_TX_FLTR_BY_HANDLE_EX(iface_ptr, flow_ptr, FALSE)
      while (handle != NULL)
      {
        ps_iface_flow_get_tx_fltr_by_handle_ex(iface_ptr, flow_ptr, FALSE,
                                       handle, fltr, prcd, new_handle)
        handle = new_handle
      }
    TASKFREE()

SIDE EFFECTS
  next_handle is set so that if this function is called with that handle,
  next Tx filter is fetched
===========================================================================*/
boolean ps_iface_flow_get_tx_fltr_by_handle_ex
(
  ps_iface_type          * ps_iface_ptr,
  ps_flow_type           * flow_ptr,
  void                   * tx_fltr_handle,
  boolean                  is_modify,
  ip_filter_type        ** fltr,
  uint8                  * prcd,
  void                  ** next_handle
)
{
  ps_iface_ipfilteri_type      * tx_fltr_buf;
  ps_iface_ipfilteri_type      * tmp_tx_fltr_buf;
  q_type                       * fltr_q_ptr;
  ps_iface_ipfltr_handle_type    fltr_handle;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!PS_IFACE_IS_VALID(ps_iface_ptr) || fltr == NULL || prcd == NULL ||
      next_handle == NULL)
  {
    DATA_ERR_FATAL("ps_iface_flow_get_tx_fltr_by_handle_ex(): Invalid parameters are passed");
    return FALSE;
  }

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  if (!PS_FLOW_IS_VALID(flow_ptr))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_iface_flow_get_tx_fltr_by_handle_ex(): "
                    "Invalid ps_flow, 0x%p, is passed", flow_ptr);
    ASSERT(0);
    return FALSE;
  }

  if (PS_FLOWI_GET_CAPABILITY(flow_ptr, PS_FLOW_CAPABILITY_DEFAULT))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_iface_flow_get_tx_fltr_by_handle_ex(): "
                    "ps_flow, 0x%p is a default flow and it doesn't have "
                    "Tx filters", flow_ptr);
    ASSERT(0);
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Fetch appropriate handle based on whether client is interested in
    pending QOS_MODIFY
  -------------------------------------------------------------------------*/
  if (is_modify)
  {
    if (!PS_FLOWI_IS_QOS_MODIFY_IN_PROGRESS(flow_ptr))
    {
      PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      LOG_MSG_ERROR_1("ps_iface_flow_get_tx_fltr_by_handle_ex(): "
                      "QOS_MODIFY is not pending on ps_flow, 0x%p", flow_ptr);
      ASSERT(0);
      return FALSE;
    }

    fltr_handle = PS_FLOWI_GET_MODIFY_TX_FLTR_HANDLE(flow_ptr);
  }
  else
  {
    fltr_handle = PS_FLOWI_GET_TX_FLTR_HANDLE(flow_ptr);
  }

  if (tx_fltr_handle == NULL)
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);

    *next_handle = NULL;
    *fltr        = NULL;
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Make sure that the passed handle belongs to ps_mem pool from which
    Tx fltrs are allocated
  -------------------------------------------------------------------------*/
  tx_fltr_buf = (ps_iface_ipfilteri_type *) tx_fltr_handle;
  if (!ps_mem_is_valid(tx_fltr_buf, PS_MEM_PS_IFACE_IPFLTER_TYPE))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_iface_flow_get_tx_fltr_by_handle_ex(): "
                    "Buffer (0x%X) doesn't belong to Tx filter's mem pool",
                    tx_fltr_handle);
    ASSERT(0);
    return FALSE;
  }

  if (tx_fltr_buf->fi_handle != fltr_handle)
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    DATA_ERR_FATAL("ps_iface_flow_get_tx_fltr_by_handle_ex(): Invalid handle is passed");
    return FALSE;
  }

  *fltr = &(tx_fltr_buf->filter.fi_default);
  *prcd = 0;

  /*-------------------------------------------------------------------------
    Update handle to the filter. Consider only those fltrs which
    belong to given ps_flow
  -------------------------------------------------------------------------*/
  fltr_q_ptr =
    &(ps_iface_ptr->iface_private.ipfltr_info[IP_FLTR_CLIENT_QOS_OUTPUT]);

  tmp_tx_fltr_buf = q_next(fltr_q_ptr, &(tx_fltr_buf->link));

  while (tmp_tx_fltr_buf != NULL &&
         (tmp_tx_fltr_buf->fi_handle != fltr_handle))
  {
    tmp_tx_fltr_buf = q_next(fltr_q_ptr, &(tmp_tx_fltr_buf->link));
  }

  *next_handle = tmp_tx_fltr_buf;

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return TRUE;

} /* ps_iface_flow_get_tx_fltr_by_handle_ex() */

/*===========================================================================
FUNCTION PS_IFACE_FLOW_GET_TX_FLTR_SPEC()

DESCRIPTION
  Given a iface ptr, this function returns all the Tx filters
  installed on an iface.

PARAMETERS
  iface_ptr      : pointer to iface
  flow_ptr       : pointer to the flow
  ip_fltr_spec   : OUT PARAM, pointer to Tx filter spec
  num_fltrs      : Number of filters to be retrieved
  ps_errno       : specific error code in case operation fails

RETURN VALUE
  TRUE  : on success
  FALSE : otherwise

DEPENDENCIES
None

SIDE EFFECTS
None

===========================================================================*/
boolean ps_iface_flow_get_tx_fltr_spec
(
  ps_iface_type       * iface_ptr,
  ps_flow_type        * flow_ptr,
  ip_filter_spec_type * ip_fltr_spec,
  boolean               is_modify,
  uint8                 num_fltrs,
  int16               * ps_errno
)
{
  ip_filter_type             * filter;
  uint8                        precedence = 0;
  uint8                        tx_fltr_cnt;
  void                       * tx_handle;      /* Handle to the Tx filter         */
  void                       * new_handle;/* Handle to the next Tx filter         */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (ps_errno == NULL)
  {
    DATA_ERR_FATAL("ps_iface_flow_get_tx_fltr_spec(): NULL errno");
    return FALSE;
  }

  if (NULL == ip_fltr_spec || NULL == ip_fltr_spec->list_ptr)
  {
    LOG_MSG_ERROR_0("ps_iface_flow_get_tx_fltr_spec(): "
                    "NULL ip_filter_spec");
    *ps_errno = DS_EINVAL;
    return FALSE;
  }

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  if (!PS_IFACE_IS_VALID(iface_ptr))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_iface_flow_get_tx_fltr_spec(): "
                    "Iface 0x%p invalid", iface_ptr);
    ASSERT(0);
    *ps_errno = DS_EINVAL;
    return FALSE;
  }

  if (!PS_FLOW_IS_VALID(flow_ptr))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_iface_flow_get_tx_fltr_spec(): "
                    "Invalid ps_flow, 0x%p, is passed", flow_ptr);
    ASSERT(0);
    *ps_errno = DS_EINVAL;
    return FALSE;
  }

  if (PS_FLOWI_GET_CAPABILITY(flow_ptr, PS_FLOW_CAPABILITY_DEFAULT))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_iface_flow_get_tx_fltr_spec(): "
                    "ps_flow, 0x%p is a default flow and it doesn't have "
                    "Tx filters", flow_ptr);
    ASSERT(0);
    *ps_errno = DS_EINVAL;
    return FALSE;
  }

  tx_fltr_cnt = ps_iface_flow_get_tx_fltr_cnt_ex(iface_ptr, flow_ptr, is_modify);

  if(num_fltrs < tx_fltr_cnt)
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_INFO1_2("ps_iface_flow_get_tx_fltr_spec(): "
                    "The number of filters supplied do not match the number of"
                    "filters already installed for the flow on the iface",
                    num_fltrs, tx_fltr_cnt);
    *ps_errno = DS_ETRUNCATED;
    return FALSE;
  }

  tx_handle = ps_ifacei_get_tx_fltr_handle_ex(iface_ptr, flow_ptr, is_modify );
  while ( tx_handle != NULL && num_fltrs > 0)
  {
    if (ps_ifacei_get_tx_fltr_by_handle_ex(iface_ptr,
                                          flow_ptr,
                                          tx_handle,
                                          is_modify,
                                          &filter,
                                          &precedence,
                                          &new_handle) == FALSE)
    {
      LOG_MSG_INFO1_1("ps_iface_flow_get_tx_fltr_spec(): "
                      "Could not get TX filter by handle for flow %d",
                      flow_ptr);
      PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      return FALSE;
    }

    tx_handle = new_handle;
    memscpy((void *)&(ip_fltr_spec->list_ptr[ip_fltr_spec->num_filters]),
            sizeof (ip_filter_type),
            (const void *)filter, 
            sizeof (ip_filter_type));
    ip_fltr_spec->num_filters++;
    num_fltrs--;
  }

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return TRUE;
} /* ps_iface_flow_get_tx_fltr_spec() */

/*===========================================================================
FUNCTION PS_IFACE_FLOW_SET_TX_FILTER_SPEC()

DESCRIPTION
  Given the ip filter spec, this fucntion sets the filter id and precedence
  of all the Tx filters for which they previous values have been modified.

PARAMETERS
  iface_ptr      : pointer to iface
  flow_ptr       : pointer to the flow
  ip_fltr_spec   : OUT PARAM, pointer to Rx filter spec

RETURN VALUE
  TRUE  : on success
  FALSE : otherwise

DEPENDENCIES
None

SIDE EFFECTS
None

===========================================================================*/
boolean ps_iface_flow_set_tx_filter_spec
(
  ps_iface_type         * iface_ptr,
  ps_flow_type          * flow_ptr,
  ip_filter_spec_type   * ip_fltr_spec,
  boolean                 modify_flag
)
{
  ps_iface_ipfilteri_type   * curr_tx_fltr_buf = NULL;
  uint8                       fltr_cnt;
  uint8                       index = 0;
  uint8                       tx_fltr_cnt = 0;
  ps_iface_ipfltr_handle_type tx_fltr_handle;
  q_type                    * fltr_q_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(ip_fltr_spec == NULL)
  {
    DATA_ERR_FATAL("ps_iface_flow_set_tx_filter_spec(): Null value passed to set_tx_filter_spec()");
    return FALSE;
  }

  /*-----------------------------------------------------------------------
    Validate that number of filters is in between (0, MAX_FLTR_PER_REQ]
  -----------------------------------------------------------------------*/
  if (ip_fltr_spec->num_filters == 0)
  {
    LOG_MSG_INFO1_0("ps_iface_flow_set_tx_filter_spec(): "
                    "Zero filters specified");
    return FALSE;
  }

  if (ip_fltr_spec->num_filters > MAX_FLTR_PER_REQ)
  {
    LOG_MSG_INFO1_3("ps_iface_flow_set_tx_filter_spec(): "
                    "Too many filters specified: %d, max = %d, for flow 0x%p",
                    ip_fltr_spec->num_filters, MAX_FLTR_PER_REQ, flow_ptr);
    return FALSE;
  }

  if (ip_fltr_spec->list_ptr == NULL)
  {
    LOG_MSG_INFO1_1("ps_iface_flow_set_tx_filter_spec(): "
                    "NULL list_ptr in fltr spec for flow 0x%p", flow_ptr);
    return FALSE;
  }

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  if (!PS_IFACE_IS_VALID(iface_ptr))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_iface_flow_set_tx_filter_spec(): "
                    "Invalid ps_iface, 0x%p, is passed", iface_ptr);
    ASSERT(0);
    return FALSE;
  }

  if (!PS_FLOW_IS_VALID(flow_ptr))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_iface_flow_set_tx_filter_spec(): "
                    "Invalid ps_flow, 0x%p, is passed", flow_ptr);
    ASSERT(0);
    return FALSE;
  }

  if (PS_FLOWI_GET_CAPABILITY(flow_ptr, PS_FLOW_CAPABILITY_DEFAULT))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_iface_flow_set_tx_filter_spec(): "
                    "QoS is not supported on default ps_flow, 0x%p", flow_ptr);
    ASSERT(0);
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Fetch appropriate handle based on whether client is interested in
    pending QOS_MODIFY
  -------------------------------------------------------------------------*/
  if (modify_flag)
  {
    if (!PS_FLOWI_IS_QOS_MODIFY_IN_PROGRESS(flow_ptr))
    {
      PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      LOG_MSG_ERROR_1("ps_iface_flow_set_tx_filter_spec(): "
                      "QOS_MODIFY is not pending on ps_flow, 0x%p", flow_ptr);
      ASSERT(0);
      return FALSE;
    }

    tx_fltr_handle = PS_FLOWI_GET_MODIFY_TX_FLTR_HANDLE(flow_ptr);
  }
  else
  {
    tx_fltr_handle = PS_FLOWI_GET_TX_FLTR_HANDLE(flow_ptr);
  }

  /*-------------------------------------------------------------------------
    Count the number of filters. Consider only those filters that
    belong to given ps_flow
  -------------------------------------------------------------------------*/
  fltr_q_ptr =
    &(iface_ptr->iface_private.ipfltr_info[IP_FLTR_CLIENT_QOS_OUTPUT]);
  curr_tx_fltr_buf = (ps_iface_ipfilteri_type *) q_check(fltr_q_ptr);

  while (curr_tx_fltr_buf != NULL)
  {
    if (curr_tx_fltr_buf->fi_handle == tx_fltr_handle)
    {
      tx_fltr_cnt++;
    }

    curr_tx_fltr_buf = q_next(fltr_q_ptr, &(curr_tx_fltr_buf->link));
  }

  fltr_cnt = ip_fltr_spec->num_filters;

  if (tx_fltr_cnt != fltr_cnt)
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_2("ps_iface_flow_set_tx_filter_spec(): "
                    "The number of flows supplied 0x%d do not match with the flows"
                    "stored in iface, 0x%d",
                    ip_fltr_spec->num_filters, tx_fltr_cnt);
    return FALSE;
  }

  curr_tx_fltr_buf = (ps_iface_ipfilteri_type *) q_check(fltr_q_ptr);

  while (curr_tx_fltr_buf != NULL)
  {
    if (curr_tx_fltr_buf->fi_handle == tx_fltr_handle)
    {
      memscpy(&curr_tx_fltr_buf->filter,
              sizeof (ip_filter_type),
              &ip_fltr_spec->list_ptr[index], 
              sizeof (ip_filter_type));
      index++;
    }
    curr_tx_fltr_buf = q_next(fltr_q_ptr, &(curr_tx_fltr_buf->link));
  }

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return TRUE;
} /* ps_iface_flow_set_tx_filter_spec() */
#endif /* FEATURE_DATA_PS_QOS */

/*===========================================================================
FUNCTION PS_IFACEI_GET_FLOW_FROM_ID()

DESCRIPTION
  This function fetches a flow which corresponds to given iface_id.

PARAMETERS
  iface_id:  iface id to extract info from

RETURN VALUE
  flow_ptr : if iface_id is valid
  NULL     : otherwise.

DEPENDENCIES
  iface_id should be a valid iface id.

SIDE EFFECTS
  None
===========================================================================*/
ps_flow_type *ps_ifacei_get_flow_from_id
(
  ps_iface_id_type  iface_id
)
{
  ps_iface_type  * iface_ptr;
  ps_flow_type   * flow_ptr;
  uint8            flow_cookie;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  iface_ptr = ps_iface_get_handle(iface_id);
  if (!PS_IFACE_IS_VALID(iface_ptr))
  {
    LOG_MSG_ERROR_2("ps_ifacei_get_flow_from_id(): "
                    "Invalid iface_id 0x%x: Invalid iface, 0x%p",
                    iface_id, iface_ptr);
    return NULL;
  }

  flow_cookie = PS_IFACE_GET_FLOW_INST_FROM_ID(iface_id);
  if (flow_cookie == PS_IFACE_DEFAULT_FLOW_INST)
  {
    flow_ptr = PS_IFACEI_GET_DEFAULT_FLOW(iface_ptr);
  }
  else if (flow_cookie > PS_IFACE_MAX_SEC_FLOW_INST)
  {
    LOG_MSG_ERROR_2("ps_ifacei_get_flow_from_id(): "
                    "Invalid iface_id 0x%x: flow cookie, %d too big",
                    iface_id, flow_cookie);
    flow_ptr = NULL;
  }
  else
  {
    PS_ENTER_CRIT_SECTION(&global_ps_crit_section);
    flow_ptr = list_peek_front(&(iface_ptr->iface_private.flow.sec_flow_list));
    while (flow_ptr != NULL)
    {
      if (PS_FLOWI_GET_COOKIE(flow_ptr) == flow_cookie)
      {
        break;
      }

      flow_ptr = list_peek_next(&(iface_ptr->iface_private.flow.sec_flow_list),
                                &(flow_ptr->link));
    }
    PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  }


  return flow_ptr;

} /* ps_ifacei_get_flow_from_id() */


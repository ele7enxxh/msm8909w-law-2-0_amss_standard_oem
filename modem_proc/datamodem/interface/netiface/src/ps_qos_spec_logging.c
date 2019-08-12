/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                P S _ Q O S _ S P E C _ L O G G I N G . C


GENERAL DESCRIPTION
  This file implements an API to log QOS specification. Requested QOS,
  granted QOS and QOS status transitions can be logged using this API

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2005-2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_iface.c_v   1.27   24 Feb 2003 10:55:32   ubabbar  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/netiface/src/ps_qos_spec_logging.c#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $
when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/13/09    ar     Suppress routines if built on multiproc Apps image.
10/24/08    pp     Replaced snprintf w/ snprintf.
01/05/07    msr    Added support to log info code in FLOW_INFO_CODE_UPDATED_EV
09/21/06    msr    Enhanced 0x135[A-C] with WLAN QoS enhancements
07/13/06    msr    Fixed broken log code 0x135B
07/07/06    msr    Logging wlan_user_priority in QoS Request log pkt.
                   Deprecated 0x132B, 0x132C, 0x132E and added 0x135[A-C]
04/12/06    msr    Logging info_code along with QoS state
02/24/06    msr    Logging QOS_MASK in QOS_MODIFY
02/15/06    msr    Aliased PS_FLOW_SET_CURRENT_FLOW() to
                   PS_FLOW_SET_GRANTED_FLOW()
01/09/06    msr    Conforming to new QOS_MODIFY API
11/30/05    msr    Logging QOS_MODIFY as well and lint fixes
10/03/05    msr    Fixed a bug where UDP filters are dumped even if
                   next_hdr_prot is not set
08/29/05    msr    Using 16 bits for QOS mask in LOG_QOS_REQUESTED_C and
                   removed support for ICMP in filters
08/15/05    msr    Created file
===========================================================================*/
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                                INCLUDE FILES

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"

#ifdef FEATURE_DATA_PS_QOS

#include <stringl/stringl.h>

#include "ps_qos_defs.h"
#include "ps_iface.h"
#include "ps_flow.h"
#include "ps_flowi.h"
#include "ps_iface_ipfltr.h"
#include "ps_iface_logging.h"
#include "ps_qos_spec_logging.h"
#include "ps_byte.h"
#include "log.h"
#include "log_codes.h"

#include <stdio.h>
#include "ds_Utils_DebugMsg.h"



/*===========================================================================

                           LOCAL DATA DECLARATIONS

===========================================================================*/
/*---------------------------------------------------------------------------
  Size of header added by DIAG subsytem to each log packet
---------------------------------------------------------------------------*/
#define LOG_HDR_SIZE  sizeof(log_hdr_type)

/*---------------------------------------------------------------------------
  Limits on # of flows/fltrs per direction that can be logged in a single
  log pkt
---------------------------------------------------------------------------*/
#define MAX_FLOWS_TO_LOG  7
#define MAX_FLTRS_TO_LOG  8

/*---------------------------------------------------------------------------
  QSL iface desc is "iface_name:iface_inst::flow_ptr"
---------------------------------------------------------------------------*/
#define QSL_IFNAME_DESC_S_LEN  23

/*---------------------------------------------------------------------------
  QOS_REQUEST_LOG_PKT includes
    - Version
    - Diag log header
    - QOS ID
    - opcode
    - QOS mask
    - Networt Interface
    - Rx QoS spec
    - Tx QoS spec

  First six fields are mandatory and min pkt length also accounts for
  flow and fltr cnt fields of Rx and Tx QoS specs
---------------------------------------------------------------------------*/
#define MIN_QOS_REQUEST_LOG_PKT_LEN                                      \
  (12 + QSL_IFNAME_DESC_S_LEN + LOG_HDR_SIZE)

/*---------------------------------------------------------------------------
  QOS_MODIFY_LOG_PKT includes
    - Version
    - Diag log header
    - QOS ID
    - QOS modify mask
    - QOS mask
    - Rx flow spec
    - Rx fltr spec
    - Tx flow spec
    - Tx fltr spec

  First four fields are mandatory
---------------------------------------------------------------------------*/
#define MIN_QOS_MODIFY_LOG_PKT_LEN  (9 + LOG_HDR_SIZE)

/*---------------------------------------------------------------------------
  QOS_GRANTED_LOG_PKT includes
    - Version
    - Diag log header
    - QOS ID
    - QOS_GRANTED mask
    - Rx flow
    - Tx flow

  First four fields are mandatory
---------------------------------------------------------------------------*/
#define MIN_QOS_GRANTED_LOG_PKT_LEN  (6 + LOG_HDR_SIZE)

/*---------------------------------------------------------------------------
  QOS_STATE_LOG_PKT includes
    - Diag log header
    - Version of log packet
    - QOS state
    - Info code
    - Reserved byte
    - QOS ID
---------------------------------------------------------------------------*/
#define QOS_STATE_LOG_PKT_LEN        (8 + LOG_HDR_SIZE)

/*---------------------------------------------------------------------------
  QOS_INFO_CODE_UPDATE_LOG_PKT includes
    - Diag log header
    - Version of log packet
    - Info code
    - Reserved bytes
    - QOS ID
---------------------------------------------------------------------------*/
#define QOS_INFO_CODE_UPDATE_LOG_PKT_LEN  (8 + LOG_HDR_SIZE)

/*---------------------------------------------------------------------------
  Definitions of QoS masks sent in granted QOS spec log packets

    QOS_GRANTED_NONE - No QoS is granted
    RX_QOS_GRANTED   - QoS is granted in Rx direction
    TX_QOS_GRANTED   - QoS is granted in Tx direction
---------------------------------------------------------------------------*/
#define QOS_GRANTED_NONE      0x0
#define RX_QOS_GRANTED        0x1
#define TX_QOS_GRANTED        0x2

/*---------------------------------------------------------------------------
  Version of each log pkt's definition. Version is incremented each time log
  pkt's definition is changed
---------------------------------------------------------------------------*/
#define QOS_REQUESTED_LOG_PKT_VER         4
#define QOS_GRANTED_LOG_PKT_VER           4
#define QOS_MODIFIED_LOG_PKT_VER          4
#define QOS_STATE_LOG_PKT_VER             3
#define QOS_INFO_CODE_UPDATE_LOG_PKT_VER  2



/*===========================================================================

                             INTERNAL FUNCTIONS

===========================================================================*/
/*===========================================================================
FUNCTION PS_QSLI_FILL_FLOW_PARAM()

DESCRIPTION
  Byte dumps a given ip_flow's fields in to a buffer

PARAMETERS
  log_ptr     : buffer which needs to be populated
  ip_flow_ptr : ip_flow to be dumped

RETURN VALUE
  None

DEPENDENCIES
  Parameters must be valid

SIDE EFFECTS
  None
===========================================================================*/
#ifdef FEATURE_DATACOMMON_PACKAGE_MODEM
static void ps_qsli_fill_flow_param
(
  uint8               ** log_ptr,
  const ip_flow_type   * ip_flow_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Based on ip_flow's field mask, dump fields in to buffer. Data is written
    in host order
  -------------------------------------------------------------------------*/
  *log_ptr = hput32(*log_ptr, ip_flow_ptr->field_mask);

  if (ip_flow_ptr->field_mask & IPFLOW_MASK_TRF_CLASS)
  {
    *log_ptr = put8(*log_ptr, ip_flow_ptr->trf_class);
  }

  if (ip_flow_ptr->field_mask & IPFLOW_MASK_LATENCY)
  {
    *log_ptr = hput32(*log_ptr, ip_flow_ptr->latency);
  }

  if (ip_flow_ptr->field_mask & IPFLOW_MASK_LATENCY_VAR)
  {
    *log_ptr = hput32(*log_ptr, ip_flow_ptr->latency_var);
  }

  if (ip_flow_ptr->field_mask & IPFLOW_MASK_PKT_ERR_RATE)
  {
    *log_ptr = hput16(*log_ptr, ip_flow_ptr->pkt_err_rate.multiplier);
    *log_ptr = hput16(*log_ptr, ip_flow_ptr->pkt_err_rate.exponent);
  }

  if (ip_flow_ptr->field_mask & IPFLOW_MASK_MIN_POLICED_PKT_SIZE)
  {
    *log_ptr = hput32(*log_ptr, ip_flow_ptr->min_policed_pkt_size);
  }

  if (ip_flow_ptr->field_mask & IPFLOW_MASK_MAX_ALLOWED_PKT_SIZE)
  {
    *log_ptr = hput32(*log_ptr, ip_flow_ptr->max_allowed_pkt_size);
  }

  if (ip_flow_ptr->field_mask & IPFLOW_MASK_UMTS_RES_BER)
  {
    *log_ptr = hput16(*log_ptr, ip_flow_ptr->umts_params.res_ber);
  }

  if (ip_flow_ptr->field_mask & IPFLOW_MASK_UMTS_TRF_PRI)
  {
    *log_ptr = put8(*log_ptr, ip_flow_ptr->umts_params.trf_pri);
  } 

  if (ip_flow_ptr->field_mask & IPFLOW_MASK_CDMA_PROFILE_ID)
  {
    *log_ptr = hput16(*log_ptr, ip_flow_ptr->cdma_params.profile_id);
  }

  if (ip_flow_ptr->field_mask & IPFLOW_MASK_WLAN_USER_PRI)
  {
    *log_ptr = put8(*log_ptr, ip_flow_ptr->wlan_params.user_priority);
  }

  if (ip_flow_ptr->field_mask & IPFLOW_MASK_WLAN_MIN_SERVICE_INTERVAL)
  {
    *log_ptr = hput32(*log_ptr, ip_flow_ptr->wlan_params.min_service_interval);
  }

  if (ip_flow_ptr->field_mask & IPFLOW_MASK_WLAN_MAX_SERVICE_INTERVAL)
  {
    *log_ptr = hput32(*log_ptr, ip_flow_ptr->wlan_params.max_service_interval);
  }

  if (ip_flow_ptr->field_mask & IPFLOW_MASK_WLAN_INACTIVITY_INTERVAL)
  {
    *log_ptr = hput32(*log_ptr, ip_flow_ptr->wlan_params.inactivity_interval);
  }

  if (ip_flow_ptr->field_mask & IPFLOW_MASK_NOMINAL_SDU_SIZE)
  {
    *log_ptr = put8(*log_ptr, ip_flow_ptr->nominal_sdu_size.is_fixed);
    *log_ptr = hput32(*log_ptr, ip_flow_ptr->nominal_sdu_size.size);
  }

  if (ip_flow_ptr->field_mask & IPFLOW_MASK_CDMA_FLOW_PRIORITY)
  {
    *log_ptr = put8(*log_ptr, ip_flow_ptr->cdma_params.flow_priority);
  }

  if (ip_flow_ptr->field_mask & IPFLOW_MASK_DATA_RATE)
  {
    *log_ptr = put8(*log_ptr, ip_flow_ptr->data_rate.format_type);

    if (ip_flow_ptr->data_rate.format_type == DATA_RATE_FORMAT_MIN_MAX_TYPE)
    {
      *log_ptr = hput32
                 (
                   *log_ptr,
                   ip_flow_ptr->data_rate.format.min_max.max_rate
                 );
      *log_ptr = hput32
                 (
                   *log_ptr,
                   ip_flow_ptr->data_rate.format.min_max.guaranteed_rate
                 );
    }
    else if (ip_flow_ptr->data_rate.format_type == DATA_RATE_FORMAT_MIN_MAX_EX_TYPE)
    {
      *log_ptr = hput64
                 (
                   *log_ptr,
                   ip_flow_ptr->data_rate.format.min_max_ex.max_rate
                 );
      *log_ptr = hput64
                 (
                   *log_ptr,
                   ip_flow_ptr->data_rate.format.min_max_ex.guaranteed_rate
                 );
    }
    else
    {
      *log_ptr = hput32
                 (
                   *log_ptr,
                   ip_flow_ptr->data_rate.format.token_bucket.peak_rate
                 );
      *log_ptr = hput32
                 (
                   *log_ptr,
                   ip_flow_ptr->data_rate.format.token_bucket.token_rate
                 );
      *log_ptr = hput32
                 (
                   *log_ptr,
                   ip_flow_ptr->data_rate.format.token_bucket.size
                 );
    }
  }
  if (ip_flow_ptr->field_mask & IPFLOW_MASK_UMTS_IM_CN_FLAG)                 
  {
    *log_ptr = put8(*log_ptr, ip_flow_ptr->umts_params.im_cn_flag);		
  }  

  if (ip_flow_ptr->field_mask & IPFLOW_MASK_UMTS_SRC_STAT_DESC)  	
  {
    *log_ptr = hput32(*log_ptr, ip_flow_ptr->umts_params.src_stat_desc);		
  }    

  if (ip_flow_ptr->field_mask & IPFLOW_MASK_UMTS_SIG_IND)
  {
    *log_ptr = put8(*log_ptr, ip_flow_ptr->umts_params.sig_ind);		
  }  
  
  if (ip_flow_ptr->field_mask & IPFLOW_MASK_LTE_QCI)
  {
    *log_ptr = hput32(*log_ptr, ip_flow_ptr->lte_params.lte_qci);		
  }

} /* ps_qsli_fill_flow_param() */



/*===========================================================================
FUNCTION PS_QSLI_FILL_FLTR_PARAM()

DESCRIPTION
  Byte dumps a given ip_fltr's fields in to a buffer

PARAMETERS
  log_ptr     : buffer which needs to be populated
  ip_fltr_ptr : ip_fltr to be dumped

RETURN VALUE
  None

DEPENDENCIES
  Parameters must be valid

SIDE EFFECTS
  None
===========================================================================*/
static void ps_qsli_fill_fltr_param
(
  uint8                 ** log_ptr,
  const ip_filter_type   * ip_fltr_ptr
)
{
  uint8  next_hdr_prot = PS_NO_NEXT_HDR;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  *log_ptr = put8(*log_ptr, ip_fltr_ptr->ip_vsn);

  if (ip_fltr_ptr->ip_vsn == IP_V4)
  {
    /*-----------------------------------------------------------------------
      Dump V4 parameters based on field mask. Data is written in host order
    -----------------------------------------------------------------------*/
    *log_ptr = put8(*log_ptr, ip_fltr_ptr->ip_hdr.v4.field_mask);

    if (ip_fltr_ptr->ip_hdr.v4.field_mask & IPFLTR_MASK_IP4_SRC_ADDR)
    {
      /*---------------------------------------------------------------------
        Address and subnet mask are stored in network order. Hence put32 is
        used to convert them back to host order
      ---------------------------------------------------------------------*/
      *log_ptr = put32(*log_ptr, ip_fltr_ptr->ip_hdr.v4.src.addr.ps_s_addr);
      *log_ptr = put32(*log_ptr,
                        ip_fltr_ptr->ip_hdr.v4.src.subnet_mask.ps_s_addr);
    }

    if (ip_fltr_ptr->ip_hdr.v4.field_mask & IPFLTR_MASK_IP4_DST_ADDR)
    {
      /*---------------------------------------------------------------------
        Address and subnet mask are stored in network order. Hence put32 is
        used to convert them back to host order
      ---------------------------------------------------------------------*/
      *log_ptr = put32(*log_ptr, ip_fltr_ptr->ip_hdr.v4.dst.addr.ps_s_addr);
      *log_ptr = put32(*log_ptr,
                        ip_fltr_ptr->ip_hdr.v4.dst.subnet_mask.ps_s_addr);
    }

    if (ip_fltr_ptr->ip_hdr.v4.field_mask & IPFLTR_MASK_IP4_TOS)
    {
      *log_ptr = put8(*log_ptr, ip_fltr_ptr->ip_hdr.v4.tos.val);
      *log_ptr = put8(*log_ptr, ip_fltr_ptr->ip_hdr.v4.tos.mask);
    }

    if (ip_fltr_ptr->ip_hdr.v4.field_mask & IPFLTR_MASK_IP4_NEXT_HDR_PROT)
    {
      *log_ptr = put8(*log_ptr, ip_fltr_ptr->ip_hdr.v4.next_hdr_prot);
      next_hdr_prot = ip_fltr_ptr->ip_hdr.v4.next_hdr_prot;
    }
  }
  else  /* fltr's version is IPV6 */
  {
    /*-----------------------------------------------------------------------
      Dump V6 parameters based on field mask. Data is written in host order
    -----------------------------------------------------------------------*/
    *log_ptr = put8(*log_ptr, ip_fltr_ptr->ip_hdr.v6.field_mask);

    if (ip_fltr_ptr->ip_hdr.v6.field_mask & IPFLTR_MASK_IP6_SRC_ADDR)
    {
      /*---------------------------------------------------------------------
        Address is stored in network order. Hence, to convert it back to host
        order, put64 is used and last 8 bytes are logged first
      ---------------------------------------------------------------------*/
      *log_ptr = put64
                 (
                   *log_ptr, ip_fltr_ptr->ip_hdr.v6.src.addr.ps_s6_addr64[1]
                 );
      *log_ptr = put64
                 (
                   *log_ptr, ip_fltr_ptr->ip_hdr.v6.src.addr.ps_s6_addr64[0]
                 );
      *log_ptr = put8(*log_ptr, ip_fltr_ptr->ip_hdr.v6.src.prefix_len);
    }

    if (ip_fltr_ptr->ip_hdr.v6.field_mask & IPFLTR_MASK_IP6_DST_ADDR)
    {
      /*---------------------------------------------------------------------
        Address is stored in network order. Hence, to convert it back to host
        order, put64 is used and last 8 bytes are logged first
      ---------------------------------------------------------------------*/
      *log_ptr = put64
                 (
                   *log_ptr, ip_fltr_ptr->ip_hdr.v6.dst.addr.ps_s6_addr64[1]
                 );
      *log_ptr = put64
                 (
                   *log_ptr, ip_fltr_ptr->ip_hdr.v6.dst.addr.ps_s6_addr64[0]
                 );
      *log_ptr = put8(*log_ptr, ip_fltr_ptr->ip_hdr.v6.dst.prefix_len);
    }

    if (ip_fltr_ptr->ip_hdr.v6.field_mask & IPFLTR_MASK_IP6_TRAFFIC_CLASS)
    {
      *log_ptr = put8(*log_ptr, ip_fltr_ptr->ip_hdr.v6.trf_cls.val);
      *log_ptr = put8(*log_ptr, ip_fltr_ptr->ip_hdr.v6.trf_cls.mask);
    }

    if (ip_fltr_ptr->ip_hdr.v6.field_mask & IPFLTR_MASK_IP6_FLOW_LABEL)
    {
      *log_ptr = hput32(*log_ptr, ip_fltr_ptr->ip_hdr.v6.flow_label);
    }

    if (ip_fltr_ptr->ip_hdr.v6.field_mask & IPFLTR_MASK_IP6_NEXT_HDR_PROT)
    {
      *log_ptr = put8(*log_ptr, ip_fltr_ptr->ip_hdr.v6.next_hdr_prot);
      next_hdr_prot = ip_fltr_ptr->ip_hdr.v6.next_hdr_prot;
    }
  }

  if (next_hdr_prot == PS_IPPROTO_TCP)
  {
    /*-----------------------------------------------------------------------
      Dump TCP parameters based on field mask. Data is written in host order
    -----------------------------------------------------------------------*/
    *log_ptr = put8(*log_ptr, ip_fltr_ptr->next_prot_hdr.tcp.field_mask);

    if (ip_fltr_ptr->next_prot_hdr.tcp.field_mask & IPFLTR_MASK_TCP_SRC_PORT)
    {
      /*---------------------------------------------------------------------
        Port is stored in network order. Hence put16 is used to convert it
        back to host order
      ---------------------------------------------------------------------*/
      *log_ptr = put16(*log_ptr, ip_fltr_ptr->next_prot_hdr.tcp.src.port);
      *log_ptr = hput16(*log_ptr, ip_fltr_ptr->next_prot_hdr.tcp.src.range);
    }

    if (ip_fltr_ptr->next_prot_hdr.tcp.field_mask & IPFLTR_MASK_TCP_DST_PORT)
    {
      /*---------------------------------------------------------------------
        Port is stored in network order. Hence put16 is used to convert it
        back to host order
      ---------------------------------------------------------------------*/
      *log_ptr = put16(*log_ptr, ip_fltr_ptr->next_prot_hdr.tcp.dst.port);
      *log_ptr = hput16(*log_ptr, ip_fltr_ptr->next_prot_hdr.tcp.dst.range);
    }
  }
  else if (next_hdr_prot == PS_IPPROTO_UDP)
  {
    /*-----------------------------------------------------------------------
      Dump UDP parameters based on field mask. Data is written in host order
    -----------------------------------------------------------------------*/
    *log_ptr = put8(*log_ptr, ip_fltr_ptr->next_prot_hdr.udp.field_mask);

    if (ip_fltr_ptr->next_prot_hdr.udp.field_mask & IPFLTR_MASK_UDP_SRC_PORT)
    {
      /*---------------------------------------------------------------------
        Port is stored in network order. Hence put16 is used to convert it
        back to host order
      ---------------------------------------------------------------------*/
      *log_ptr = put16(*log_ptr, ip_fltr_ptr->next_prot_hdr.udp.src.port);
      *log_ptr = hput16(*log_ptr, ip_fltr_ptr->next_prot_hdr.udp.src.range);
    }

    if (ip_fltr_ptr->next_prot_hdr.udp.field_mask & IPFLTR_MASK_UDP_DST_PORT)
    {
      /*---------------------------------------------------------------------
        Port is stored in network order. Hence put16 is used to convert it
        back to host order
      ---------------------------------------------------------------------*/
      *log_ptr = put16(*log_ptr, ip_fltr_ptr->next_prot_hdr.udp.dst.port);
      *log_ptr = hput16(*log_ptr, ip_fltr_ptr->next_prot_hdr.udp.dst.range);
    }
  }
  else if (next_hdr_prot == PS_IPPROTO_ESP)
  {
    /*-----------------------------------------------------------------------
      Dump ESP parameters based on field mask. Data is written in host order
    -----------------------------------------------------------------------*/
    *log_ptr = put8(*log_ptr, ip_fltr_ptr->next_prot_hdr.esp.field_mask);

    if (ip_fltr_ptr->next_prot_hdr.esp.field_mask & IPFLTR_MASK_ESP_SPI)
    {
      /*---------------------------------------------------------------------
        SPI is stored in network order. Hence hput32 is used
      ---------------------------------------------------------------------*/
      *log_ptr = hput32(*log_ptr, ip_fltr_ptr->next_prot_hdr.esp.spi);
    }
  }
  else if (next_hdr_prot == PS_IPPROTO_AH)
  {
    /*-----------------------------------------------------------------------
      Dump AH parameters based on field mask. Data is written in host order
    -----------------------------------------------------------------------*/
    *log_ptr = put8(*log_ptr, ip_fltr_ptr->next_prot_hdr.ah.field_mask);

    if (ip_fltr_ptr->next_prot_hdr.ah.field_mask & IPFLTR_MASK_AH_SPI)
    {
      /*---------------------------------------------------------------------
        SPI is stored in network order. Hence hput32 is used
      ---------------------------------------------------------------------*/
      *log_ptr = hput32(*log_ptr, ip_fltr_ptr->next_prot_hdr.ah.spi);
    }
  }
  else if (next_hdr_prot == PS_IPPROTO_ICMP)
  {
    /*-----------------------------------------------------------------------
      Dump ICMP parameters based on field mask. Data is written in host order
    -----------------------------------------------------------------------*/
    *log_ptr = put8(*log_ptr, ip_fltr_ptr->next_prot_hdr.icmp.field_mask);

    if (ip_fltr_ptr->next_prot_hdr.icmp.field_mask & IPFLTR_MASK_ICMP_MSG_CODE)
    {
      *log_ptr = put8(*log_ptr, ip_fltr_ptr->next_prot_hdr.icmp.code);        
    }
    
    if (ip_fltr_ptr->next_prot_hdr.icmp.field_mask & IPFLTR_MASK_ICMP_MSG_TYPE)
    {
      *log_ptr = put8(*log_ptr, ip_fltr_ptr->next_prot_hdr.icmp.type);
    }    
  }
 
  *log_ptr = hput16(*log_ptr, ip_fltr_ptr->ipfltr_aux_info.fi_id);        
  *log_ptr = hput16(*log_ptr, ip_fltr_ptr->ipfltr_aux_info.fi_precedence);    

} /* ps_qsli_fill_fltr_param() */
#endif /* FEATURE_DATACOMMON_PACKAGE_MODEM */


/*===========================================================================

                             EXTERNAL FUNCTIONS

===========================================================================*/
/*===========================================================================
FUNCTION PS_QSL_LOG_REQUESTED_QOS_SPEC()

DESCRIPTION
  Logs requested QoS specification

PARAMETERS
  qos_spec_ptr : QoS specification to be logged
  iface_ptr    : iface on which QoS is requested
  flow_ptr     : flow which manages this QoS instance's state
  opcode       : QoS operation (QOS_REQUEST/QOS_CONFIGURE)

RETURN VALUE
  None

DEPENDENCIES
  Parameters must be valid

SIDE EFFECTS
  None
===========================================================================*/
void ps_qsl_log_requested_qos_spec
(
  const qos_spec_type        * qos_spec_ptr,
  const ps_iface_type        * iface_ptr,
  const ps_flow_type         * flow_ptr,
  qsl_qos_opcode_enum_type     opcode
)
{
#ifdef FEATURE_DATACOMMON_PACKAGE_MODEM
  uint8   * log_ptr;
  uint8   * resp_ptr;
  int32     ret_val;
  uint16    log_pkt_len            = 0;
  uint8     num_recv_flows_to_log  = 0;
  uint8     num_recv_fltrs_to_log  = 0;
  uint8     num_tx_flows_to_log    = 0;
  uint8     num_tx_fltrs_to_log    = 0;
  uint8     index;
  char      net_iface[QSL_IFNAME_DESC_S_LEN];
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Log QoS spec only if log code is enabled
  -------------------------------------------------------------------------*/
  if (log_status(LOG_QOS_REQUESTED_EX_C))
  {
    /*-----------------------------------------------------------------------
      Determine # of Rx flows and Rx fltrs to log
    -----------------------------------------------------------------------*/
    if (qos_spec_ptr->field_mask & QOS_MASK_RX_FLOW)
    {
      num_recv_flows_to_log++;

      if (qos_spec_ptr->field_mask & QOS_MASK_RX_MIN_FLOW)
      {
        num_recv_flows_to_log++;
      }

      if (qos_spec_ptr->field_mask & QOS_MASK_RX_AUXILIARY_FLOWS)
      {
        num_recv_flows_to_log += qos_spec_ptr->rx.flow_template.num_aux_flows;
      }

      num_recv_flows_to_log = MIN(MAX_FLOWS_TO_LOG, num_recv_flows_to_log);
      num_recv_fltrs_to_log = MIN(MAX_FLTRS_TO_LOG,
                                  qos_spec_ptr->rx.fltr_template.num_filters);
    }

    /*-----------------------------------------------------------------------
      Determine # of Tx flows and Tx fltrs to log
    -----------------------------------------------------------------------*/
    if (qos_spec_ptr->field_mask & QOS_MASK_TX_FLOW)
    {
      num_tx_flows_to_log++;

      if (qos_spec_ptr->field_mask & QOS_MASK_TX_MIN_FLOW)
      {
        num_tx_flows_to_log++;
      }

      if (qos_spec_ptr->field_mask & QOS_MASK_TX_AUXILIARY_FLOWS)
      {
        num_tx_flows_to_log += qos_spec_ptr->tx.flow_template.num_aux_flows;
      }

      num_tx_flows_to_log = MIN(MAX_FLOWS_TO_LOG, num_tx_flows_to_log);
      num_tx_fltrs_to_log = MIN(MAX_FLTRS_TO_LOG,
                                qos_spec_ptr->tx.fltr_template.num_filters);
    }

    /*-----------------------------------------------------------------------
      Find how much memory needs to be allocated for log pkt
    -----------------------------------------------------------------------*/
    log_pkt_len =
      (num_recv_flows_to_log + num_tx_flows_to_log) * sizeof(ip_flow_type) +
      (num_recv_fltrs_to_log + num_tx_fltrs_to_log) * sizeof(ip_filter_type) +
      MIN_QOS_REQUEST_LOG_PKT_LEN;

    resp_ptr = log_ptr = (uint8 *) log_alloc(LOG_QOS_REQUESTED_EX_C,
                                             log_pkt_len);

    if (log_ptr == NULL)
    {
      LOG_MSG_INFO2_0("ps_qsl_log_requested_qos_spec(): "
                      "log_alloc failed");
      return;
    }

    log_ptr += LOG_HDR_SIZE;

    log_ptr = put8(log_ptr, QOS_REQUESTED_LOG_PKT_VER);

    log_ptr = put32(log_ptr,
                    *((uint32 *) &(flow_ptr->dpl_flow_cb.tx_dpl_id)));

    log_ptr = put8(log_ptr, opcode);
    log_ptr = hput16(log_ptr, qos_spec_ptr->field_mask);

    /*-----------------------------------------------------------------------
      Network interface's format is "iface_name:iface_inst::ps_flow"
    -----------------------------------------------------------------------*/
    (void) snprintf(net_iface,
                          QSL_IFNAME_DESC_S_LEN,
                          "0x%x:%d::0x%p",
                          iface_ptr->name,
                          iface_ptr->instance,
                          flow_ptr);

    ret_val = strlcpy((char *) log_ptr,
                          net_iface,
                          log_pkt_len - (log_ptr - resp_ptr));
    log_ptr += ret_val + 1;

    /*-----------------------------------------------------------------------
      Log Rx flow spec and Rx fltr spec
    -----------------------------------------------------------------------*/
    if (qos_spec_ptr->field_mask & QOS_MASK_RX_FLOW)
    {
      log_ptr = put8(log_ptr, num_recv_flows_to_log);
      log_ptr = put8(log_ptr, num_recv_fltrs_to_log);

      ps_qsli_fill_flow_param(&log_ptr,
                              &(qos_spec_ptr->rx.flow_template.req_flow));
      num_recv_flows_to_log--;

      /*---------------------------------------------------------------------
        If RX_MIN_FLOW is not requested, "num_recv_flows_to_log" auxiliary
        flow specs can be logged, else "num_recv_flows_to_log - 1"
        auxiliaries can be logged.
        Since flows are logged in: RX_FLOW, auxiliary flows, RX_MIN_FLOW,
        decrement "num_recv_flow_to_log" apriori if RX_MIN_FLOW is requested
        so that we can log exact # of auxiliary in one shot
      ---------------------------------------------------------------------*/
      if (qos_spec_ptr->field_mask & QOS_MASK_RX_MIN_FLOW)
      {
        num_recv_flows_to_log--;
      }

      /*---------------------------------------------------------------------
        Log auxiliary Rx flows
      ---------------------------------------------------------------------*/
      if (qos_spec_ptr->field_mask & QOS_MASK_RX_AUXILIARY_FLOWS)
      {
        for (index = 0; num_recv_flows_to_log-- > 0; index++)
        {
          ps_qsli_fill_flow_param
          (
            &log_ptr,
            qos_spec_ptr->rx.flow_template.aux_flow_list_ptr + index
          );
        }
      }

      /*---------------------------------------------------------------------
        Log Rx minimum required flow
      ---------------------------------------------------------------------*/
      if (qos_spec_ptr->field_mask & QOS_MASK_RX_MIN_FLOW)
      {
        ps_qsli_fill_flow_param
        (
          &log_ptr,
          &(qos_spec_ptr->rx.flow_template.min_req_flow)
        );
      }

      /*---------------------------------------------------------------------
        Log Rx fltrs
      ---------------------------------------------------------------------*/
      for (index = 0; num_recv_fltrs_to_log-- > 0; index++)
      {
        ps_qsli_fill_fltr_param
        (
          &log_ptr,
          qos_spec_ptr->rx.fltr_template.list_ptr + index
        );
      }
    }

    /*-----------------------------------------------------------------------
      Log Tx flow spec and Tx fltr spec
    -----------------------------------------------------------------------*/
    if (qos_spec_ptr->field_mask & QOS_MASK_TX_FLOW)
    {
      log_ptr = put8(log_ptr, num_tx_flows_to_log);
      log_ptr = put8(log_ptr, num_tx_fltrs_to_log);

      ps_qsli_fill_flow_param(&log_ptr,
                              &(qos_spec_ptr->tx.flow_template.req_flow));
      num_tx_flows_to_log--;

      /*---------------------------------------------------------------------
        If TX_MIN_FLOW is not requested, "num_tx_flows_to_log" auxiliary
        flow specs can be logged, else "num_tx_flows_to_log - 1"
        auxiliaries can be logged.
        Since flows are logged in: TX_FLOW, auxiliary flows, TX_MIN_FLOW,
        decrement "num_tx_flow_to_log" apriori if TX_MIN_FLOW is requested
        so that we can log exact # of auxiliary in one shot
      ---------------------------------------------------------------------*/
      if (qos_spec_ptr->field_mask & QOS_MASK_TX_MIN_FLOW)
      {
        num_tx_flows_to_log--;
      }

      /*---------------------------------------------------------------------
        Log auxiliary Tx flows
      ---------------------------------------------------------------------*/
      if (qos_spec_ptr->field_mask & QOS_MASK_TX_AUXILIARY_FLOWS)
      {
        for (index = 0; num_tx_flows_to_log-- > 0; index++)
        {
          ps_qsli_fill_flow_param
          (
            &log_ptr,
            qos_spec_ptr->tx.flow_template.aux_flow_list_ptr + index
          );
        }
      }

      /*---------------------------------------------------------------------
        Log Tx minimum required flow
      ---------------------------------------------------------------------*/
      if (qos_spec_ptr->field_mask & QOS_MASK_TX_MIN_FLOW)
      {
        ps_qsli_fill_flow_param
        (
          &log_ptr,
          &(qos_spec_ptr->tx.flow_template.min_req_flow)
        );
      }

      /*---------------------------------------------------------------------
        Log Tx fltrs
      ---------------------------------------------------------------------*/
      for (index = 0; num_tx_fltrs_to_log-- > 0; index++)
      {
        ps_qsli_fill_fltr_param
        (
          &log_ptr,
          qos_spec_ptr->tx.fltr_template.list_ptr + index
        );
      }
    }

    /*-----------------------------------------------------------------------
      Since all fields of ip_flow/ip_fltr may not have been specified by
      an application, # of logged bytes could be less than log_pkt_len
    -----------------------------------------------------------------------*/
    ASSERT((log_ptr - resp_ptr) <= log_pkt_len);

    log_shorten(resp_ptr, (uint32) (log_ptr - resp_ptr));
    log_commit(resp_ptr);
  }
#endif /* FEATURE_DATACOMMON_PACKAGE_MODEM */
} /* ps_qsl_log_requested_qos_spec() */



/*===========================================================================
FUNCTION PS_QSL_LOG_MODIFIED_QOS_SPEC()

DESCRIPTION
  Logs QoS spec specified in QOS_MODIFY ioctl

PARAMETERS
  qos_spec_ptr : QoS specification to be logged
  flow_ptr     : flow which manages this QoS instance's state

RETURN VALUE
  None

DEPENDENCIES
  Parameters must be valid

SIDE EFFECTS
  None
===========================================================================*/
void ps_qsl_log_modified_qos_spec
(
  const qos_spec_type  * qos_spec_ptr,
  const ps_flow_type   * flow_ptr
)
{
#ifdef FEATURE_DATACOMMON_PACKAGE_MODEM
  uint8   * log_ptr;
  uint8   * resp_ptr;
  uint16    log_pkt_len            = 0;
  uint8     num_recv_flows_to_log  = 0;
  uint8     num_recv_fltrs_to_log  = 0;
  uint8     num_tx_flows_to_log    = 0;
  uint8     num_tx_fltrs_to_log    = 0;
  uint8     index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Log QoS spec only if log code is enabled
  -------------------------------------------------------------------------*/
  if (log_status(LOG_QOS_MODIFIED_EX_C))
  {
    /*-----------------------------------------------------------------------
      Determine # of Rx flows to log
    -----------------------------------------------------------------------*/
    if (qos_spec_ptr->field_mask & QOS_MASK_RX_FLOW)
    {
      num_recv_flows_to_log++;

      if (qos_spec_ptr->field_mask & QOS_MASK_RX_MIN_FLOW)
      {
        num_recv_flows_to_log++;
      }

      if (qos_spec_ptr->field_mask & QOS_MASK_RX_AUXILIARY_FLOWS)
      {
        num_recv_flows_to_log += qos_spec_ptr->rx.flow_template.num_aux_flows;
      }

      num_recv_flows_to_log = MIN(MAX_FLOWS_TO_LOG, num_recv_flows_to_log);
    }

    /*-----------------------------------------------------------------------
      Determine # of Rx fltrs to log
    -----------------------------------------------------------------------*/
    if (PS_FLOWI_IS_MODIFY_RX_FLTR_SPEC_CREATED(flow_ptr) ||
        PS_FLOWI_IS_MODIFY_RX_FLTR_SPEC_MODIFIED(flow_ptr))
    {
      num_recv_fltrs_to_log = MIN(MAX_FLTRS_TO_LOG,
                                  qos_spec_ptr->rx.fltr_template.num_filters);
    }

    /*-----------------------------------------------------------------------
      Determine # of Tx flows to log
    -----------------------------------------------------------------------*/
    if (qos_spec_ptr->field_mask & QOS_MASK_TX_FLOW)
    {
      num_tx_flows_to_log++;

      if (qos_spec_ptr->field_mask & QOS_MASK_TX_MIN_FLOW)
      {
        num_tx_flows_to_log++;
      }

      if (qos_spec_ptr->field_mask & QOS_MASK_TX_AUXILIARY_FLOWS)
      {
        num_tx_flows_to_log += qos_spec_ptr->tx.flow_template.num_aux_flows;
      }

      num_tx_flows_to_log = MIN(MAX_FLOWS_TO_LOG, num_tx_flows_to_log);
    }

    /*-----------------------------------------------------------------------
      Determine # of Tx fltrs to log
    -----------------------------------------------------------------------*/
    if (PS_FLOWI_IS_MODIFY_TX_FLTR_SPEC_CREATED(flow_ptr) ||
        PS_FLOWI_IS_MODIFY_TX_FLTR_SPEC_MODIFIED(flow_ptr))
    {
      num_tx_fltrs_to_log = MIN(MAX_FLTRS_TO_LOG,
                                qos_spec_ptr->tx.fltr_template.num_filters);
    }

    /*-----------------------------------------------------------------------
      Find how much memory needs to be allocated for log pkt
    -----------------------------------------------------------------------*/
    log_pkt_len =
      (num_recv_flows_to_log + num_tx_flows_to_log) * sizeof(ip_flow_type) +
      (num_recv_fltrs_to_log + num_tx_fltrs_to_log) * sizeof(ip_filter_type) +
      MIN_QOS_MODIFY_LOG_PKT_LEN;

    resp_ptr = log_ptr = (uint8 *) log_alloc(LOG_QOS_MODIFIED_EX_C,
                                             log_pkt_len);

    if (log_ptr == NULL)
    {
      LOG_MSG_INFO2_0("ps_qsl_log_modified_qos_spec(): "
                      "log_alloc failed");
      return;
    }

    log_ptr += LOG_HDR_SIZE;

    log_ptr = put8(log_ptr, QOS_MODIFIED_LOG_PKT_VER);

    log_ptr = put32(log_ptr,
                    *((uint32 *) &(flow_ptr->dpl_flow_cb.tx_dpl_id)));

    log_ptr = hput16(log_ptr, qos_spec_ptr->field_mask);
    log_ptr = hput16(log_ptr, PS_FLOWI_GET_QOS_FIELD_MASK(flow_ptr));

    /*-----------------------------------------------------------------------
      Log Rx flow spec
    -----------------------------------------------------------------------*/
    if (num_recv_flows_to_log > 0)
    {
      log_ptr = put8(log_ptr, num_recv_flows_to_log);

      ps_qsli_fill_flow_param(&log_ptr,
                              &(qos_spec_ptr->rx.flow_template.req_flow));
      num_recv_flows_to_log--;

      /*---------------------------------------------------------------------
        If RX_MIN_FLOW is not requested, "num_recv_flows_to_log" auxiliary
        flow specs can be logged, else "num_recv_flows_to_log - 1"
        auxiliaries can be logged.
        Since flows are logged in: RX_FLOW, auxiliary flows, RX_MIN_FLOW,
        decrement "num_recv_flow_to_log" apriori if RX_MIN_FLOW is requested
        so that we can log exact # of auxiliary in one shot
      ---------------------------------------------------------------------*/
      if (qos_spec_ptr->field_mask & QOS_MASK_RX_MIN_FLOW)
      {
        num_recv_flows_to_log--;
      }

      /*---------------------------------------------------------------------
        Log auxiliary Rx flows
      ---------------------------------------------------------------------*/
      if (qos_spec_ptr->field_mask & QOS_MASK_RX_AUXILIARY_FLOWS)
      {
        for (index = 0; num_recv_flows_to_log-- > 0; index++)
        {
          ps_qsli_fill_flow_param
          (
            &log_ptr,
            qos_spec_ptr->rx.flow_template.aux_flow_list_ptr + index
          );
        }
      }

      /*---------------------------------------------------------------------
        Log Rx minimum required flow
      ---------------------------------------------------------------------*/
      if (qos_spec_ptr->field_mask & QOS_MASK_RX_MIN_FLOW)
      {
        ps_qsli_fill_flow_param
        (
          &log_ptr,
          &(qos_spec_ptr->rx.flow_template.min_req_flow)
        );
      }
    }

    /*-----------------------------------------------------------------------
      Log Rx fltrs
    -----------------------------------------------------------------------*/
    if (num_recv_fltrs_to_log > 0)
    {
      log_ptr = put8(log_ptr, num_recv_fltrs_to_log);
      for (index = 0; num_recv_fltrs_to_log-- > 0; index++)
      {
        ps_qsli_fill_fltr_param
        (
          &log_ptr,
          qos_spec_ptr->rx.fltr_template.list_ptr + index
        );
      }
    }

    /*-----------------------------------------------------------------------
      Log Tx flow spec
    -----------------------------------------------------------------------*/
    if (num_tx_flows_to_log > 0)
    {
      log_ptr = put8(log_ptr, num_tx_flows_to_log);

      ps_qsli_fill_flow_param(&log_ptr,
                              &(qos_spec_ptr->tx.flow_template.req_flow));
      num_tx_flows_to_log--;

      /*---------------------------------------------------------------------
        If TX_MIN_FLOW is not requested, "num_tx_flows_to_log" auxiliary
        flow specs can be logged, else "num_tx_flows_to_log - 1"
        auxiliaries can be logged.
        Since flows are logged in: TX_FLOW, auxiliary flows, TX_MIN_FLOW,
        decrement "num_tx_flow_to_log" apriori if TX_MIN_FLOW is requested
        so that we can log exact # of auxiliary in one shot
      ---------------------------------------------------------------------*/
      if (qos_spec_ptr->field_mask & QOS_MASK_TX_MIN_FLOW)
      {
        num_tx_flows_to_log--;
      }

      /*---------------------------------------------------------------------
        Log auxiliary Tx flows
      ---------------------------------------------------------------------*/
      if (qos_spec_ptr->field_mask & QOS_MASK_TX_AUXILIARY_FLOWS)
      {
        for (index = 0; num_tx_flows_to_log-- > 0; index++)
        {
          ps_qsli_fill_flow_param
          (
            &log_ptr,
            qos_spec_ptr->tx.flow_template.aux_flow_list_ptr + index
          );
        }
      }

      /*---------------------------------------------------------------------
        Log Tx minimum required flow
      ---------------------------------------------------------------------*/
      if (qos_spec_ptr->field_mask & QOS_MASK_TX_MIN_FLOW)
      {
        ps_qsli_fill_flow_param
        (
          &log_ptr,
          &(qos_spec_ptr->tx.flow_template.min_req_flow)
        );
      }
    }

    /*-----------------------------------------------------------------------
      Log Tx fltrs
    -----------------------------------------------------------------------*/
    if (num_tx_fltrs_to_log > 0)
    {
      log_ptr = put8(log_ptr, num_tx_fltrs_to_log);
      for (index = 0; num_tx_fltrs_to_log-- > 0; index++)
      {
        ps_qsli_fill_fltr_param
        (
          &log_ptr,
          qos_spec_ptr->tx.fltr_template.list_ptr + index
        );
      }
    }

    /*-----------------------------------------------------------------------
      Since all fields of ip_flow/ip_fltr may not have been specified by
      an application, # of logged bytes could be less than log_pkt_len
    -----------------------------------------------------------------------*/
    ASSERT((log_ptr - resp_ptr) <= log_pkt_len);

    log_shorten(resp_ptr, (uint32) (log_ptr - resp_ptr));
    log_commit(resp_ptr);
  }
#endif /* FEATURE_DATACOMMON_PACKAGE_MODEM */
} /* ps_qsl_log_modified_qos_spec() */



/*===========================================================================
FUNCTION PS_QSL_LOG_GRANTED_QOS_SPEC()

DESCRIPTION
  Logs granted QoS specification

PARAMETERS
  flow_ptr : flow which manages a QoS instance's state

RETURN VALUE
  None

DEPENDENCIES
  Parameters must be valid

SIDE EFFECTS
  None
===========================================================================*/
void ps_qsl_log_granted_qos_spec
(
  const ps_flow_type  * flow_ptr
)
{
#ifdef FEATURE_DATACOMMON_PACKAGE_MODEM
  ip_flow_type  * ip_flow_ptr;
  uint8         * log_ptr;
  uint8         * resp_ptr;
  uint8           qos_mask    = QOS_GRANTED_NONE;
  uint16          log_pkt_len = MIN_QOS_GRANTED_LOG_PKT_LEN;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Log granted QoS only if log code is enabled
  -------------------------------------------------------------------------*/
  if (log_status(LOG_QOS_GRANTED_EX_C))
  {
    /*-----------------------------------------------------------------------
      Check if Rx flow is granted
    -----------------------------------------------------------------------*/
    ip_flow_ptr = PS_FLOWI_GET_RX_GRANTED_FLOW2(flow_ptr);
    if (ip_flow_ptr != NULL && ip_flow_ptr->field_mask != IPFLOW_MASK_NONE)
    {
      qos_mask |= RX_QOS_GRANTED;
      log_pkt_len += sizeof(ip_flow_type);
    }

    /*-----------------------------------------------------------------------
      Check if Tx flow is granted
    -----------------------------------------------------------------------*/
    ip_flow_ptr = PS_FLOWI_GET_TX_GRANTED_FLOW2((ps_flow_type*)flow_ptr);
    if (ip_flow_ptr != NULL && ip_flow_ptr->field_mask != IPFLOW_MASK_NONE)
    {
      qos_mask |= TX_QOS_GRANTED;
      log_pkt_len += sizeof(ip_flow_type);
    }

    log_ptr = (uint8 *) log_alloc(LOG_QOS_GRANTED_EX_C, log_pkt_len);

    if (log_ptr == NULL)
    {
      LOG_MSG_INFO2_0("ps_qsl_log_granted_qos_spec(): "
                      "log_alloc failed");
      return;
    }

    resp_ptr = log_ptr;
    log_ptr += LOG_HDR_SIZE;

    log_ptr = put8(log_ptr, QOS_GRANTED_LOG_PKT_VER);

    log_ptr = put32(log_ptr,
                    *((uint32 *) &(flow_ptr->dpl_flow_cb.tx_dpl_id)));

    log_ptr = put8(log_ptr, qos_mask);

    /*-----------------------------------------------------------------------
      Log granted Rx flow
    -----------------------------------------------------------------------*/
    ip_flow_ptr = PS_FLOWI_GET_RX_GRANTED_FLOW2((ps_flow_type*)flow_ptr);
    if (ip_flow_ptr != NULL && ip_flow_ptr->field_mask != IPFLOW_MASK_NONE)
    {
      ps_qsli_fill_flow_param(&log_ptr, ip_flow_ptr);
    }

    /*-----------------------------------------------------------------------
      Log granted Tx flow
    -----------------------------------------------------------------------*/
    ip_flow_ptr = PS_FLOWI_GET_TX_GRANTED_FLOW2((ps_flow_type*)flow_ptr);
    if (ip_flow_ptr != NULL && ip_flow_ptr->field_mask != IPFLOW_MASK_NONE)
    {
      ps_qsli_fill_flow_param(&log_ptr, ip_flow_ptr);
    }

    /*-----------------------------------------------------------------------
      Since all fields of ip_flow may not have been specified by
      an application, # of logged bytes could be less than log_pkt_len
    -----------------------------------------------------------------------*/
    ASSERT((log_ptr - resp_ptr) <= log_pkt_len);

    log_shorten(resp_ptr, (uint32) (log_ptr - resp_ptr));
    log_commit(resp_ptr);
  }
#endif /* FEATURE_DATACOMMON_PACKAGE_MODEM */
} /* ps_qsl_log_granted_qos_spec() */



/*===========================================================================
FUNCTION PS_QSL_LOG_QOS_STATE()

DESCRIPTION
  Logs QoS instance's state

PARAMETERS
  flow_ptr : flow which manages a QoS instance's state
  qos_state : QoS instance's state
  info_code : additional info

RETURN VALUE
  None

DEPENDENCIES
  Parameters must be valid

SIDE EFFECTS
  None
===========================================================================*/
void ps_qsl_log_qos_state
(
  const ps_flow_type               * flow_ptr,
  qsl_qos_state_enum_type            qos_state,
  ps_extended_info_code_enum_type    info_code
)
{
#ifdef FEATURE_DATACOMMON_PACKAGE_MODEM
  uint8  * log_ptr;
  uint8  * resp_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Log QoS state only if log code is enabled
  -------------------------------------------------------------------------*/
  if (log_status(LOG_QOS_STATE_EX_C))
  {
    log_ptr = (uint8 *) log_alloc(LOG_QOS_STATE_EX_C, QOS_STATE_LOG_PKT_LEN);

    if (log_ptr == NULL)
    {
      LOG_MSG_INFO2_0("ps_qsl_log_qos_state(): "
                      "log_alloc failed");
      return;
    }

    resp_ptr = log_ptr;
    log_ptr += LOG_HDR_SIZE;

    log_ptr = put8(log_ptr, QOS_STATE_LOG_PKT_VER);
    log_ptr = put8(log_ptr, qos_state);
    log_ptr = put8(log_ptr, info_code);

    /*-----------------------------------------------------------------------
      Skip reserved byte
    -----------------------------------------------------------------------*/
    log_ptr++;

    log_ptr = put32(log_ptr,
                    *((uint32 *) &(flow_ptr->dpl_flow_cb.tx_dpl_id)));

    ASSERT((log_ptr - resp_ptr) == (int32)QOS_STATE_LOG_PKT_LEN);

    log_commit(resp_ptr);
  }
#endif /* FEATURE_DATACOMMON_PACKAGE_MODEM */
} /* ps_qsl_log_qos_state() */



/*===========================================================================
FUNCTION PS_QSL_LOG_QOS_INFO_CODE_UPDATE()

DESCRIPTION
  Logs info code, which is newly updated on a QoS instance

PARAMETERS
  flow_ptr  : flow which manages a QoS instance's state
  info_code : additional info available for this QoS instance

RETURN VALUE
  None

DEPENDENCIES
  Parameters must be valid

SIDE EFFECTS
  None
===========================================================================*/
void ps_qsl_log_qos_info_code_update
(
  const ps_flow_type               * flow_ptr,
  ps_extended_info_code_enum_type    info_code
)
{
#ifdef FEATURE_DATACOMMON_PACKAGE_MODEM
  uint8  * log_ptr;
  uint8  * resp_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Log QoS state only if log code is enabled
  -------------------------------------------------------------------------*/
  if (log_status(LOG_QOS_INFO_CODE_UPDATE_C))
  {
    log_ptr = (uint8 *) log_alloc(LOG_QOS_INFO_CODE_UPDATE_C,
                                  QOS_INFO_CODE_UPDATE_LOG_PKT_LEN);

    if (log_ptr == NULL)
    {
      LOG_MSG_INFO2_0("ps_qsl_log_qos_info_code_update(): "
                      "log_alloc failed");
      return;
    }

    resp_ptr = log_ptr;
    log_ptr += LOG_HDR_SIZE;

    log_ptr = put8(log_ptr, QOS_INFO_CODE_UPDATE_LOG_PKT_VER);
    log_ptr = put8(log_ptr, info_code);

    /*-----------------------------------------------------------------------
      Skip reserved bytes
    -----------------------------------------------------------------------*/
    log_ptr += 2;

    log_ptr = put32(log_ptr,
                    *((uint32 *) &(flow_ptr->dpl_flow_cb.tx_dpl_id)));

    ASSERT((log_ptr - resp_ptr) == (int32)QOS_INFO_CODE_UPDATE_LOG_PKT_LEN);

    log_commit(resp_ptr);
  }
#endif /* FEATURE_DATACOMMON_PACKAGE_MODEM */
} /* ps_qsl_log_qos_info_code_update() */

#endif /* FEATURE_DATA_PS_QOS */

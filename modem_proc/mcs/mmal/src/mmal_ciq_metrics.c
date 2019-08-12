/*!
  @file mmal_ciq_metrics.c

  @brief

  @detail

*/

/*==============================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/mmal/src/mmal_ciq_metrics.c#1 $

when       who    what, where, why
--------   ---    -------------------------------------------------------------
11/04/11   ejv    Implement query callbacks for ss25 and ss2b.
07/20/11   sk     Fixed indentation
06/23/11   sk     Call back function support for getting the LTE RRC OTA msg content
05/08/11   sk     Used the new mmal_ciq_metrics_free_item
05/06/11   sk     Release the dsm item if the msg send fails
05/05/11   sk     Use the dsm flex api to allocate a dsm item
05/06/11   sk     CR 286772 For the RRC call back handling, do not send the msg if the 
                  dsm_ptr is NULL.                         
05/04/11   sk     Added file name and the line number to the dsm calls
05/03/11   sk     Added some stats collection for the dsm item used
03/28/11   sk     Initial Checkin
           
==============================================================================*/

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include "mcs_variation.h"
#include <customer.h>
#include "mmal_cfg.h"

#ifdef FEATURE_MMAL_CIQ

#include <log.h>
#include <stdio.h>
#include <diag.h>

#include "mmal_ciq_metrics.h"
#include "mmal_lte_metrics.h"
#include "mmal_msg_i.h"
#include "mmal_task.h"
#include "mmal_util.h"

#include <bit.h>
#include <cm.h>
#include <dsm_item.h>
#include <dss_iface_ioctl.h>
#include <ps_iface_defs.h>

#include <IQAgent.h>
#include <IQMetricsGS.h>
#include <IQMetricsSS.h>

/*==============================================================================

                   INTERNAL DEFINITIONS AND TYPES

==============================================================================*/
#define MMAL_CIQ_METRICS_LOG_LEN_POS        0
#define MMAL_CIQ_METRICS_LOG_CODE_POS      16
#define MMAL_CIQ_METRICS_MAX_UMTS_IFACES   14
#define MMAL_CIQ_METRICS_NUM_DNS            2

#define MMAL_CIQ_GS46_MCC_VALID          0x01
#define MMAL_CIQ_GS46_MNC_VALID          0x02
#define MMAL_CIQ_GS46_LAC_VALID          0x04
#define MMAL_CIQ_GS46_CELL_ID_VALID      0x08
#define MMAL_CIQ_GS46_RAC_VALID          0x10
#define MMAL_CIQ_GS46_RNC_ID_VALID       0x20
#define MMAL_CIQ_GS46_ROAMING            0x80

typedef struct
{
  ps_iface_type  *iface_ptr;
  uint32          context_id;
} mmal_ciq_metrics_context_id_map_t;

/*==============================================================================

                         INTERNAL VARIABLES

==============================================================================*/
mmal_ciq_metrics_stats_s            mmal_ciq_metric_stats_info;

mmal_ciq_metrics_context_id_map_t   context_id_map[MMAL_CIQ_METRICS_MAX_UMTS_IFACES];

const unsigned char  szVendor[]       =  "QUALCOMM";
const unsigned char  szModel[]        =  "8960";
const unsigned char  szVersion[]      =  "1.0";

const unsigned char  szVersion_proc[] =  "1.0";

/*==============================================================================

                    INTERNAL FUNCTION PROTOTYPES

==============================================================================*/



/*==============================================================================

                                FUNCTIONS

==============================================================================*/

/*===========================================================================

  FUNCTION:  mmal_ciq_metrics_wipe_context_id_map

===========================================================================*/
/*!
    @brief
    
    @detail
    
    @return
    none

    @Note
    

*/
/*=========================================================================*/
void mmal_ciq_metrics_wipe_context_id_map
(
  void
)
{
  memset( &context_id_map, 0, sizeof(context_id_map) );
}


/*===========================================================================

  FUNCTION:  mmal_ciq_populate_stats

===========================================================================*/
/*!
    @brief
    
    
    @detail
    

    @return
    none

    @Note
    

*/
/*=========================================================================*/
void mmal_ciq_populate_stats
(
  uint8 *idx,
  uint8 limit,
  uint8 *src,
  uint8 *dst,
  uint8 size
)
{
  *idx++;

  if ((*idx) > limit)
  {
    *idx = 0;
  }
  NOTUSED(idx);

  memcpy (dst, src, size);

}

/*===========================================================================

  FUNCTION:  mmal_ciq_metrics_free_item

===========================================================================*/
/*!
    @brief
    This is a utility function which will release a dsm items
    
    @detail
    1. Releases the dsm item
    2. Updates the stats

    @return
    none

    @Note

*/
/*=========================================================================*/
void
mmal_ciq_metrics_free_item
(
  dsm_item_type **dsm_ptr,
  const char       *file,
  uint16             line
)
{
  dsmi_free_packet(dsm_ptr, file, line);
  MMAL_CIQ_INCR_DSM_DEALLOC_CNT();
}

/*===========================================================================

  FUNCTION:  mmal_ciq_metrics_copy_buf_to_dsm_item

===========================================================================*/
/*!
    @brief
    This is a utility function which will copy from buffer to the dsm item
    
    @detail
    1. Copies the mem from buf to the dsm item
    2. Note the dsm items could get chained in the process

    @return
    none

    @Note
    This is a call back function executing in the diag task context

*/
/*=========================================================================*/
dsm_item_type *
mmal_ciq_metrics_copy_buf_to_dsm_item
(
  void           *buf,
  uint16          size,
  const char     *file,
  uint16          line
)
{
  uint32          diff;
  
  /*! Address of ptr to head of packet  */
  dsm_item_type  *pkt_head_ptr = NULL;

  pkt_head_ptr = dsmutili_new_buffer_flexible_by_pool_id( DSM_DS_LARGE_ITEM_POOL,
                                                          size,
                                                          1,
                                                          __FILENAME__,
                                                          __LINE__ );

  if ( pkt_head_ptr )
  {
    /*--------------------------------------------------------------------------
    Allocate a DSM item and copy the buf onto it.
    for now items are taken from the small pool. 
    @TODO : This might suffice now, but optimize this based upon the packets
    that need to be supported
    --------------------------------------------------------------------------*/
    if ( ( dsmi_pushdown_tail( &pkt_head_ptr, 
                               buf, 
                               size, 
                               DSM_DS_LARGE_ITEM_POOL,
                               file, 
                               line ) ) != size )
    {
      /* Failed to allocated needed dsm items. Release those dsm    */
      /* item being allocated and set pkt_head_ptr to NULL          */
      dsm_free_packet( &pkt_head_ptr );
      MSG_ERROR( "Failed to obtain enough memory", 0, 0, 0 );

      MMAL_CIQ_INCR_DSM_ERR_CNT();
      MMAL_CIQ_INCR_ERR_CNT();
    }
    else
    {
      MMAL_CIQ_INCR_DSM_ALLOC_CNT();

      if ( mmal_ciq_metric_stats_info.metrics_info.dsm_alloc_cnt >
           mmal_ciq_metric_stats_info.metrics_info.dsm_dealloc_cnt)
      {
        diff = ( mmal_ciq_metric_stats_info.metrics_info.dsm_alloc_cnt -
                 mmal_ciq_metric_stats_info.metrics_info.dsm_dealloc_cnt );
        
        mmal_ciq_metric_stats_info.metrics_info.dsm_max_usage_cnt =
          ( diff > mmal_ciq_metric_stats_info.metrics_info.dsm_max_usage_cnt ?
            diff : mmal_ciq_metric_stats_info.metrics_info.dsm_max_usage_cnt );
      }
    }
  }
  else
  {
    MMAL_CIQ_INCR_DSM_ERR_CNT();
  }
  return pkt_head_ptr;

}


/*===========================================================================

  FUNCTION:  mmal_ciq_metrics_sniff_diag_events

===========================================================================*/
/*!
    @brief
    This is the call back function which will be called when the diag 
    detects that event of the type registered with is submitted by the AMSS code

    @detail
    1. This function will be called when the particular event of interest 
    is logged by the AMSS code
    2. This function will copy the payload to a SMALL dsm item 
    3. Subsequently a message with UMID (submit_metric) would be sent to 
    be processed in  the MMAL task context

    @return
    none

    @Note
    This is a call back function executing in the diag task context

*/
/*=========================================================================*/
void  mmal_ciq_metrics_sniff_diag_events
(
uint32                          seq_num,
const diag_event_type  *event,
void                            *param
)
{
  /*! The message to be sent */
  mmal_log_listener_req_s   log_info;
  dsm_item_type *pkt_head_ptr=NULL;

  MMAL_MSG_SPRINTF_1("MMAL internal: sniffed diag event %d", seq_num);

  /*--------------------------------------------------------------------------
  Allocate a DSM item and copy the event->payload onto it.
  --------------------------------------------------------------------------*/
  pkt_head_ptr = 
  mmal_ciq_metrics_copy_buf_to_dsm_item(
                     event, sizeof(diag_event_type), __FILENAME__, __LINE__);

  /*--------------------------------------------------------------------------
  Send a message to the MMAL task if the pkt_hdr_ptr != NULL
  --------------------------------------------------------------------------*/
  if (pkt_head_ptr)
  {
    /*! Send a message to the MMAL Task Q */
    log_info.hdr.id = MMAL_LOG_LISTENER_REQ;
    log_info.log_id = event->event_id;
    log_info.payload_dsm_ptr = pkt_head_ptr;
    /*! Send a message to the MMAL Task Q */
    MMAL_MSG_SPRINTF_1("MMAL internal: sending diag event %d for internal processing", seq_num);
    if (!mmal_msg_send(&log_info.hdr))
    {
      mmal_ciq_metrics_free_item(
        &log_info.payload_dsm_ptr,__FILENAME__, __LINE__);
    }
  }
}

/*===========================================================================

  FUNCTION:  mmal_ciq_metrics_sniff_diag_logs

===========================================================================*/
/*!
    @brief
    This is the call back function which will be called when the diag detects that
    log of the type registered with is submitted by the AMSS code

    @detail
    1. This function will be called when the particular event of interest
    is logged by the AMSS code
    2. This function will copy the payload to a SMALL or LARGE dsm item 
    3. Subsequently a message with UMID (submit_metric) would be sent 
    to be processed in
    the MMAL task context

    

    @return
    none

    @Note
    This is a call back function executing in the diag task context

*/
/*=========================================================================*/
void mmal_ciq_metrics_sniff_diag_logs 
(
  uint32                   seq_num, 
  const byte              *log, 
  unsigned int             length, 
  void                    *param
)
{
  dsm_item_type            *pkt_head_ptr=NULL;
  /*! The message to be sent */
  mmal_log_listener_req_s   log_info;
  byte                     *byte_ptr;
  uint16                    num, high, low;

  MMAL_MSG_SPRINTF_1("MMAL internal: sniffed diag log %d", seq_num);

  /*--------------------------------------------------------------------------
   Allocate a DSM item and copy the event->payload onto it.
   --------------------------------------------------------------------------*/
  pkt_head_ptr = mmal_ciq_metrics_copy_buf_to_dsm_item( log,
                                                        length,
                                                        __FILENAME__,
                                                        __LINE__ );

  /*--------------------------------------------------------------------------
  Send a message to the MMAL task if the pkt_hdr_ptr != NULL
  --------------------------------------------------------------------------*/

  if ( pkt_head_ptr )
  {
    byte_ptr = (byte *)pkt_head_ptr->data_ptr;
    log_info.hdr.id = MMAL_LOG_LISTENER_REQ;

    num = b_unpackw( pkt_head_ptr->data_ptr,
                     MMAL_CIQ_METRICS_LOG_CODE_POS, 
                     16 );

    high = ( ( num & 0x00FF ) << 8 );
    low  = ( ( num & 0xFF00 ) >> 8 );

    log_info.log_id = ( high | low );

    // This now points to the beginning of the payload
    log_info.payload_dsm_ptr = pkt_head_ptr; 

    MMAL_MSG_SPRINTF_2("MMAL internal: sending diag log 0x%x #%d for internal processing", 
                       log_info.log_id,
                       seq_num);

    /*! Send a message to the MMAL Task Q */
    if (!mmal_msg_send(&log_info.hdr))
    {
      mmal_ciq_metrics_free_item(
        &log_info.payload_dsm_ptr,__FILENAME__, __LINE__);
    }
  }
}

#ifdef FEATURE_WCDMA
/*==========================================================================

  FUNCTION:  mmal_ciq_metrics_wrrc_log_msg_cb

===========================================================================*/
/*!
    @brief
    This function will be called whenever W RRC is going to send a 
    UL msg or has received a DL msg

    @detail

    @return

    @Note

*/
/*=========================================================================*/
void mmal_ciq_metrics_wrrc_log_msg_process_cb
(
  log_message_type *log_msg_ptr 
)
{
  MMAL_ASSERT(log_msg_ptr);

  /*! The message to be sent */
  mmal_wrrc_log_msg_cb_s   log_info; 

  log_info.hdr.id = MMAL_CIQ_FEND_WRRC_LOG_MSG_CB_REQ;

  log_info.log_msg_ptr = log_msg_ptr;

  log_info.dsm_item_ptr = 
  mmal_ciq_metrics_copy_buf_to_dsm_item(
                                       log_msg_ptr->raw_sdu,
                                       log_msg_ptr->raw_sdu_length,
                                       __FILENAME__, __LINE__
                                       );

  if (log_info.dsm_item_ptr)
  {
    /*! Send a message to the MMAL Task Q */
    if (!mmal_msg_send(&log_info.hdr))
    {
      mmal_ciq_metrics_free_item(&log_info.dsm_item_ptr,
        __FILENAME__, __LINE__);
    }
  }
  else
  {
    mmal_ciq_metric_stats_info.metrics_err_info.gs35_error_cnt++;
    mmal_ciq_metric_stats_info.metrics_err_info.gs34_error_cnt++; 
  }  
}
#endif

/*==========================================================================

  FUNCTION:  mmal_ciq_metrics_lrrc_log_msg_cb

===========================================================================*/
/*!
    @brief
    This function will be called whenever W RRC is going to send a 
    UL msg or has received a DL msg

    @detail

    @return

    @Note

*/
/*=========================================================================*/
void mmal_ciq_metrics_lrrc_log_msg_process_cb
(
  uint16 pdu_num, 
  uint16 encoded_msg_len, 
  unsigned char *encoded_msg, 
  uint8 msg_type, 
  uint8 lc_type
)
{
  /*! The message to be sent */
  mmal_lrrc_log_msg_cb_s   log_info; 

  log_info.hdr.id = MMAL_CIQ_FEND_LRRC_LOG_MSG_CB_REQ;

  log_info.pdu_num = pdu_num;
  log_info.encoded_msg_len = encoded_msg_len;
  log_info.msg_type = msg_type;
  log_info.lc_type = lc_type;

  log_info.dsm_item_ptr = 
  mmal_ciq_metrics_copy_buf_to_dsm_item(
                                       encoded_msg,
                                       encoded_msg_len,
                                       __FILENAME__, __LINE__);

  if (log_info.dsm_item_ptr)
  {
    /*! Send a message to the MMAL Task Q */
    if (!mmal_msg_send(&log_info.hdr))
    {
      mmal_ciq_metrics_free_item(&log_info.dsm_item_ptr,
        __FILENAME__, __LINE__);
    }
  }
  else
  {
    mmal_ciq_metric_stats_info.metrics_err_info.lte01_ul_error_cnt++;
    mmal_ciq_metric_stats_info.metrics_err_info.error_cnt++; 
  }  
}

/*==========================================================================

  FUNCTION:  mmal_ciq_metrics_pdp_context_up_cb

===========================================================================*/
/*!
    @brief
    This function will be called whenever a PDP context is activated 

    @detail

    @return

    @Note

*/
/*=========================================================================*/
void mmal_ciq_metrics_pdp_context_up_cb
(
  ps_iface_type                           *iface_ptr,
  ps_iface_event_enum_type                 event,
  ps_iface_event_info_u_type               event_info,
  void                                    *user_data_ptr
)
{
  ps_iface_name_enum_type                  tech_name;
  ip_addr_enum_type                        ip_type;
  ip_addr_type                             ip;
  ps_iface_ioctl_pdp_context_id_type       context_id;
  dss_iface_ioctl_get_all_dns_addrs_type   dns_addrs;
  ip_addr_type                             dns[MMAL_CIQ_METRICS_NUM_DNS];
  iq_metric_GS15_t                         gs15_metric;
  iq_metric_GS15_alt_t                     gs15_ipv6_metric;
  int16                                    ps_errno, index;

  tech_name = PS_IFACE_GET_NAME_FROM_ID( PS_IFACE_GET_ID( iface_ptr ) );

  MMAL_MSG_SPRINTF_1( "MMAL: GS15 PDP Context Up, tech == 0x%x", 
                      tech_name );

  if ( UMTS_IFACE == tech_name )
  {
    MMAL_MSG( "MMAL: PDP Context Up detected UMTS, will submit" );
    /* Get the IP address (will be either v4 or v6; check for that later */
    MMAL_ASSERT( 0 == ps_iface_ioctl( iface_ptr, 
                                      PS_IFACE_IOCTL_GET_IP_ADDR, 
                                      &ip, 
                                      &ps_errno ) );

    /* Get the DNS addrs */
    dns_addrs.num_dns_addrs = MMAL_CIQ_METRICS_NUM_DNS;
    dns_addrs.dns_addrs_ptr = (ip_addr_type *)&dns;
    MMAL_ASSERT( 0 == ps_iface_ioctl( iface_ptr, 
                                      PS_IFACE_IOCTL_GET_ALL_DNS_ADDRS, 
                                      &dns_addrs, 
                                      &ps_errno ) );

    /* Get the context ID */
    MMAL_ASSERT( 0 == ps_iface_ioctl( iface_ptr,
                                      PS_IFACE_IOCTL_UMTS_GET_PDP_CONTEXT_ID, 
                                      &context_id, 
                                      &ps_errno ) );

    /* Check the IP type */
    ip_type = ps_iface_get_addr_family( iface_ptr );
    
    switch ( ip_type )
    {
      case IPV4_ADDR:
        gs15_metric.dwContextId = context_id;
        /* Reverse the bytes for ipv4 addrs */
        gs15_metric.dwIpAddress = ( ( ip.addr.v4 & 0xFF000000 ) >> 24 ) |
                                  ( ( ip.addr.v4 & 0x00FF0000 ) >> 8 ) |
                                  ( ( ip.addr.v4 & 0x0000FF00 ) << 8 ) |
                                  ( ( ip.addr.v4 & 0x000000FF ) << 24 );
        gs15_metric.dwDns1      = ( ( dns[0].addr.v4 & 0xFF000000 ) >> 24 ) |
                                  ( ( dns[0].addr.v4 & 0x00FF0000 ) >> 8 ) |
                                  ( ( dns[0].addr.v4 & 0x0000FF00 ) << 8 ) |
                                  ( ( dns[0].addr.v4 & 0x000000FF ) << 24 );
        gs15_metric.dwDns2      = ( ( dns[1].addr.v4 & 0xFF000000 ) >> 24 ) |
                                  ( ( dns[1].addr.v4 & 0x00FF0000 ) >> 8 ) |
                                  ( ( dns[1].addr.v4 & 0x0000FF00 ) << 8 ) |
                                  ( ( dns[1].addr.v4 & 0x000000FF ) << 24 );

        IQ_SubmitMetric( IQ_METRIC_ID_GS15, 
                         &gs15_metric, 
                         sizeof(gs15_metric) );
        MMAL_MSG( "MMAL: Submitting GS15" );
        break;
      case IPV6_ADDR:
        gs15_ipv6_metric.dwContextId = context_id;
        gs15_ipv6_metric.dwIpAddress = 0;
        /* Reverse the bytes for ipv4 addrs */
        gs15_ipv6_metric.dwDns1      = ( ( dns[0].addr.v4 & 0xFF000000 ) >> 24 ) |
                                       ( ( dns[0].addr.v4 & 0x00FF0000 ) >> 8 ) |
                                       ( ( dns[0].addr.v4 & 0x0000FF00 ) << 8 ) |
                                       ( ( dns[0].addr.v4 & 0x000000FF ) << 24 );
        gs15_ipv6_metric.dwDns2      = ( ( dns[1].addr.v4 & 0xFF000000 ) >> 24 ) |
                                       ( ( dns[1].addr.v4 & 0x00FF0000 ) >> 8 ) |
                                       ( ( dns[1].addr.v4 & 0x0000FF00 ) << 8 ) |
                                       ( ( dns[1].addr.v4 & 0x000000FF ) << 24 );
        memcpy( &gs15_ipv6_metric.strIpV6Address, 
                &ip.addr.v6, 
                sizeof(gs15_ipv6_metric.strIpV6Address) );

        IQ_SubmitMetric( IQ_METRIC_ID_GS15, 
                         &gs15_ipv6_metric, 
                         sizeof(gs15_ipv6_metric) );
        MMAL_MSG( "MMAL: Submitting GS15" );

        break;
      default:
        MMAL_ASSERT( ( IPV4_ADDR == ip_type ) || ( IPV6_ADDR == ip_type ) );
        break;
    }

    /* Store the pdp context ID/pointer map */
    for ( index = 0; index != MMAL_CIQ_METRICS_MAX_UMTS_IFACES; index++ )
    {
      /* do we already have this pointer? */
      if ( context_id_map[index].iface_ptr == iface_ptr )
      {
        MMAL_MSG_ERROR( "MMAL: GS15 - iface_ptr already present in map" );
        MMAL_MSG_SPRINTF_3( "MMAL: GS15 - old context id = %d, new context id = %d, ptr = 0x%x",
                            context_id_map[index].context_id,
                            context_id,
                            iface_ptr );
      }

      if ( NULL == context_id_map[index].iface_ptr )
      {
        context_id_map[index].iface_ptr = iface_ptr;
        context_id_map[index].context_id = context_id;
        break;
      }
    }
  }
}

/*==========================================================================

  FUNCTION:  mmal_ciq_metrics_pdp_context_down_cb

===========================================================================*/
/*!
    @brief
    This function will be called whenever a PDP context is ended 

    @detail

    @return

    @Note

*/
/*=========================================================================*/
void mmal_ciq_metrics_pdp_context_down_cb
(
  ps_iface_type                  *iface_ptr,
  ps_iface_event_enum_type        event,
  ps_iface_event_info_u_type      event_info,
  void                           *user_data_ptr
)
{
  ps_iface_name_enum_type         tech_name;
  iq_metric_GS18_t                gs18_metric;
  ps_iface_net_down_reason_type   net_down_reason;
  int16                           index, context_id;

  tech_name = PS_IFACE_GET_NAME_FROM_ID( PS_IFACE_GET_ID( iface_ptr ) );

  MMAL_MSG_SPRINTF_1( "MMAL: GS18 PDP Context Down, tech == 0x%x", 
                      tech_name );

  if ( UMTS_IFACE == tech_name )
  {
    MMAL_MSG( "MMAL: PDP Context Down detected UMTS, will submit" );

    context_id = ~0;

    /* Check the pdp context ID/pointer map */
    for ( index = 0; index != MMAL_CIQ_METRICS_MAX_UMTS_IFACES; index++ )
    {
      /* do we already have this pointer? */
      if ( context_id_map[index].iface_ptr == iface_ptr )
      {
        /* grab the context ID from the map */
        context_id = context_id_map[index].context_id;

        /* remove this context ID from the map */
        context_id_map[index].context_id = 0;
        context_id_map[index].iface_ptr = 0;
        break;
      }
    }

    /* If we didn't find a context ID, that's [apparently not?] a problem (must be a deactivation) */
//    MMAL_ASSERT( context_id != ~0 );
//    if ( context_id == ~0 )
//    {
      /* context id not found; ignore this down event */
//      return;
//    }

    gs18_metric.dwContextId = context_id;

    net_down_reason = iface_ptr->event_info_cache.iface_down_info.netdown_reason;

    /* The bottom two bytes are the termination code */
    gs18_metric.wTermCode = (uint16)(net_down_reason & 0xFFFF);

    /* The value '6' in the top 2 bytes corresponds to network initiated teardowns */
    if ( ( net_down_reason & 0xFFFF0000 ) == 0x00060000 )
    {
      gs18_metric.ucInitiator = IQ_PDP_NETWORK_INITIATED;
    }
    else
    {
      gs18_metric.ucInitiator = IQ_PDP_UE_INITIATED;
    }

    IQ_SubmitMetric( IQ_METRIC_ID_GS18, &gs18_metric, sizeof(gs18_metric) );
    MMAL_MSG( "MMAL: Submitting GS18" );
  }
}


/*===========================================================================

FUNCTION: mmal_ciq_metric_extract_nas_ota_msg

===========================================================================*/
/*!
@brief

This function intercepts the 0x713A log packet

@detail

@return
The log packet wil now contain only the nas_message

LOG_RECORD_DEFINE(LOG_UMTS_NAS_OTA_MESSAGE_LOG_PACKET_C)

// Indicates if this was sent to or from the UE 
uint8 nas_message_direction;

// Length of the NAS ota message 
uint32 nas_message_length;

// NAS ota Message 
uint8 nas_message[LOG_MAX_NAS_OTA_MESSAGE_SIZE];

LOG_RECORD_END

OCTET1 TRID- PD
OCTET2 MSG_TYPE

@Note 

=========================================================================*/
uint8 mmal_ciq_metric_extract_nas_ota_msg
(
  dsm_item_type **dsm_log_ptr
)
{
  uint16 read_len;
  uint16 bytes_read;
  uint8  dir;
  uint8  msg[20];

  MMAL_ASSERT(dsm_log_ptr != NULL); 
  MMAL_ASSERT(*dsm_log_ptr != NULL);

  /*Skip past the header */
  /* Read the log header and the first 4 bytes - discard them */
  read_len = sizeof(log_hdr_type);

  bytes_read = dsmi_pullup( dsm_log_ptr, 
    msg, read_len,__FILENAME__,__LINE__ );

  MMAL_ASSERT(bytes_read == read_len);

  /*! Read the direction */
  dir = dsmi_pull8(dsm_log_ptr, __FILENAME__, __LINE__);

  /* read the 4 bytes representing the length */
  read_len = 4;

  bytes_read = dsmi_pullup( dsm_log_ptr, 
    msg, read_len,__FILENAME__,__LINE__ );

  MMAL_ASSERT(bytes_read == read_len);

  return dir;
}


/*===========================================================================

  FUNCTION:  mmal_ciq_metric_pack_and_submit_msg_gs19

===========================================================================*/
/*!
    @brief
    mmal_ciq_metric_pack_and_submit_msg_gs19

    @detail
    

    @return
    

    @Note
    

*/
/*=========================================================================*/
void mmal_ciq_metric_pack_and_submit_msg_gs19 
(
  sm_pdp_context_request_log_info_ind_T  *msg
)
{
  iq_metric_GS19_t                        metric;
  uint32                                  apn_len = 0;
  iq_iovec_t                              iov[3];

/*---------------------------------------------------------------------------*/

  metric.dwContextId                    = msg->context_id;
  metric.dwAssocContextId               = msg->assoc_context_id;
  if ( msg->is_primary_pdp )
  {
    metric.ucOrdinal                    = (uint8)IQ_PDP_PRIMARY;
    metric.dwAssocContextId             = 0xFFFFFFFF;
  }
  else
  {
    metric.ucOrdinal                    = (uint8)IQ_PDP_SECONDARY;
  }
  if ( SM_PDP_UE_INITIATED == msg->initiator )
  {
    metric.ucInitiator                  = IQ_PDP_UE_INITIATED;
  }
  else
  {
    metric.ucInitiator                  = IQ_PDP_NETWORK_INITIATED;
  }
  metric.ucNSAPI                        = msg->nsapi;
  metric.ucSAPI                         = msg->sapi;

  apn_len = strlen( msg->apn );
  if ( apn_len > MAX_APN_ADR_LEN )
  {
    apn_len = MAX_APN_ADR_LEN;
  }

/*---------------------------------------------------------------------------*/

  iov[0].data = &metric;
  iov[0].len = sizeof(metric);

  iov[1].data = msg->apn;
  iov[1].len = apn_len;

  iov[2].data = NULL;
  iov[2].len  = 0;

  IQ_SubmitMetricV( IQ_METRIC_ID_GS19, (iq_iovec_t *)&iov );
}


/*==========================================================================

  FUNCTION:  mmal_ciq_cm_ss_event_srv_changed_cb

===========================================================================*/
/*!
    @brief
    Called when serving system changes to process any CM SS EVENT

    @detail

    @return

    @Note

*/
/*=========================================================================*/
void mmal_ciq_cm_ss_event_srv_changed_cb
(
  cm_ss_event_e_type           ss_event,     /* The event that occurred */
  const cm_mm_ss_info_s_type  *ss_info_ptr   /* Serving system information */
)
{
  MMAL_MSG_ERROR( "MMAL: CM_SS_EVENT callback entered" );

#ifndef FEATURE_MMAL_DISABLE_GS46
  if ( mmal_ciq_fend_check_if_metric_enabled( MMAL_CIQ_GS46_METRIC ) )
  {
    mmal_ciq_gs46_cm_ss_event( ss_event, ss_info_ptr );
  }
#endif /* !FEATURE_MMAL_DISABLE_GS46 */

#ifndef FEATURE_MMAL_DISABLE_GS47
  if ( mmal_ciq_fend_check_if_metric_enabled( MMAL_CIQ_GS47_METRIC ) )
  {
    mmal_ciq_gs47_cm_ss_event( ss_event, ss_info_ptr );
  }
#endif /* !FEATURE_MMAL_DISABLE_GS47 */

#ifndef FEATURE_MMAL_DISABLE_GS6E
  if ( mmal_ciq_fend_check_if_metric_enabled( MMAL_CIQ_GS6E_METRIC ) )
  {
    mmal_ciq_gs6E_cm_ss_event( ss_event, ss_info_ptr );
  }
#endif /* !FEATURE_MMAL_DISABLE_GS6E */

}

/*==========================================================================

  FUNCTION:  mmal_ciq_gs46_cm_ss_event

===========================================================================*/
/*!
    @brief
    Called when serving system changes to process GS46

    @detail

    @return

    @Note

*/
/*=========================================================================*/
void mmal_ciq_gs46_cm_ss_event
(
  cm_ss_event_e_type           ss_event,     /* The event that occurred */
  const cm_mm_ss_info_s_type  *ss_info_ptr   /* Serving system information */
)
{

  iq_metric_GS46_t             metric;
  static iq_metric_GS46_t      last_metric;

/*=========================================================================*/

  MMAL_MSG( "MMAL: GS46 callback entered" );

  memset( &metric, 0, sizeof(metric) );

  /* We have only registered for one event */
  MMAL_ASSERT( CM_SS_EVENT_SRV_CHANGED == ss_event );

  switch ( ss_info_ptr->sys_mode )
  {
    case SYS_SYS_MODE_GSM:
      metric.ucAccessTech       = IQ_ACCESS_TECH_GERAN;
      metric.wRNCId             = 0xFFFF;
      metric.ucFieldValidity    = MMAL_CIQ_GS46_MCC_VALID |
                                  MMAL_CIQ_GS46_MNC_VALID |
                                  MMAL_CIQ_GS46_LAC_VALID |
                                  MMAL_CIQ_GS46_CELL_ID_VALID |
                                  MMAL_CIQ_GS46_RAC_VALID;
      if ( SYS_ROAM_STATUS_OFF != ss_info_ptr->roam_status )
      {
        metric.ucFieldValidity |= MMAL_CIQ_GS46_ROAMING;
      }
      break;
    case SYS_SYS_MODE_WCDMA:
      metric.ucAccessTech       = IQ_ACCESS_TECH_UTRAN;
      metric.wRNCId             = ( ss_info_ptr->cell_info.cell_id & 0x0FFF0000 ) >> 16;
      metric.ucFieldValidity    = MMAL_CIQ_GS46_MCC_VALID |
                                  MMAL_CIQ_GS46_MNC_VALID |
                                  MMAL_CIQ_GS46_LAC_VALID |
                                  MMAL_CIQ_GS46_CELL_ID_VALID |
                                  MMAL_CIQ_GS46_RAC_VALID |
                                  MMAL_CIQ_GS46_RNC_ID_VALID;
      if ( SYS_ROAM_STATUS_OFF != ss_info_ptr->roam_status )
      {
        metric.ucFieldValidity |= MMAL_CIQ_GS46_ROAMING;
      }
      break;
    case SYS_SYS_MODE_NO_SRV:
      metric.ucAccessTech       = IQ_ACCESS_TECH_NONE;
      metric.wRNCId             = 0xFFFF;
      metric.ucFieldValidity    = 0;
      break;
    default:
      metric.ucAccessTech       = IQ_ACCESS_TECH_UNKNOWN;
      metric.wRNCId             = 0xFFFF;
      metric.ucFieldValidity    = 0;
      break;
  }

  MMAL_MSG_SPRINTF_1( "MMAL: GS46, RAC or MME code = %d", ss_info_ptr->rac_or_mme_code );
  metric.ucRAC                  = ss_info_ptr->rac_or_mme_code;
  if ( metric.ucAccessTech != IQ_ACCESS_TECH_NONE )
  {
    metric.ucFreqBand           = ss_info_ptr->active_band;
  }
  else
  {
    metric.ucFreqBand           = 0xFF;
  }
  metric.wMCC                   = ( ss_info_ptr->sys_id.id.plmn.identity[1] & 0x0F ) + 
                                  ( ( ( ss_info_ptr->sys_id.id.plmn.identity[0] & 0xF0 ) >> 4 ) * 10 ) + 
                                  ( ( ss_info_ptr->sys_id.id.plmn.identity[0] & 0x0F ) * 100 );
  MMAL_MSG_SPRINTF_3( "MMAL: GS46, translating MCC 0x%x / 0x%x to %d", 
                      ss_info_ptr->sys_id.id.plmn.identity[1] & 0x0F,
                      ss_info_ptr->sys_id.id.plmn.identity[0],
                      metric.wMCC );

  metric.wMNC                   = ( ( ss_info_ptr->sys_id.id.plmn.identity[2] & 0xF0 ) >> 4 ) + 
                                  ( ( ss_info_ptr->sys_id.id.plmn.identity[2] & 0x0F ) * 10 );
  if ( ( ( ss_info_ptr->sys_id.id.plmn.identity[1] & 0xF0 ) >> 4 ) < 10 )
  {
    /* if this is a 3 digit mnc, promote the other digits and add the last one */
    metric.wMNC *= 10; 
    metric.wMNC += ( ( ss_info_ptr->sys_id.id.plmn.identity[1] & 0xF0 ) >> 4 );
  }
  MMAL_MSG_SPRINTF_3( "MMAL: GS46, translating MNC 0x%x / 0x%x to %d", 
                      ( ss_info_ptr->sys_id.id.plmn.identity[1] & 0xF0 ) >> 4,
                      ss_info_ptr->sys_id.id.plmn.identity[2],
                      metric.wMNC );

  metric.wLAC                   = ss_info_ptr->lac;
  metric.wCellId                = ss_info_ptr->cell_info.cell_id;

  switch ( ss_info_ptr->active_band )
  {
    case SYS_BAND_GSM_450:
      metric.ucFreqBand = (uint8) IQ_BAND_INFO_GSM_450;
      break;
    case SYS_BAND_GSM_480:
      metric.ucFreqBand = (uint8) IQ_BAND_INFO_GSM_480;
      break;
    case SYS_BAND_GSM_750:
      metric.ucFreqBand = (uint8) IQ_BAND_INFO_GSM_750;
      break;
    case SYS_BAND_GSM_850:
      metric.ucFreqBand = (uint8) IQ_BAND_INFO_GSM_850;
      break;
    case SYS_BAND_GSM_PGSM_900:
      metric.ucFreqBand = (uint8) IQ_BAND_INFO_P_GSM_900;
      break;
    case SYS_BAND_GSM_EGSM_900:
      MMAL_MSG("MMAL: GS46, SYS_BAND_GSM_EGSM_900");
      metric.ucFreqBand = (uint8) IQ_BAND_INFO_E_GSM_900;
      break;
    case SYS_BAND_GSM_RGSM_900:
      metric.ucFreqBand = (uint8) IQ_BAND_INFO_R_GSM_900;
      break;
    case SYS_BAND_GSM_DCS_1800:
      MMAL_MSG("MMAL: GS46, SYS_BAND_GSM_DCS_1800");
      metric.ucFreqBand = (uint8) IQ_BAND_INFO_DCS_1800;
      break;
    case SYS_BAND_GSM_PCS_1900:
      metric.ucFreqBand = (uint8) IQ_BAND_INFO_PCS_1900;
      break;
    case SYS_BAND_WCDMA_I_IMT_2000:
      metric.ucFreqBand = (uint8) IQ_BAND_INFO_UTRA_FDD_I;
      break;
    case SYS_BAND_WCDMA_II_PCS_1900:
      metric.ucFreqBand = (uint8) IQ_BAND_INFO_UTRA_FDD_II;
      break;
    case SYS_BAND_WCDMA_III_1700:
      metric.ucFreqBand = (uint8) IQ_BAND_INFO_UTRA_FDD_III;
      break;
    case SYS_BAND_WCDMA_IV_1700:
      metric.ucFreqBand = (uint8) IQ_BAND_INFO_UTRA_FDD_IV;
      break;
    case SYS_BAND_WCDMA_V_850:
      metric.ucFreqBand = (uint8) IQ_BAND_INFO_UTRA_FDD_V;
      break;
    case SYS_BAND_WCDMA_VI_800:
      metric.ucFreqBand = (uint8) IQ_BAND_INFO_UTRA_FDD_VI;
      break;
    case SYS_BAND_WCDMA_VII_2600:
      metric.ucFreqBand = (uint8) IQ_BAND_INFO_UTRA_FDD_VII;
      break;
    case SYS_BAND_WCDMA_VIII_900:
      metric.ucFreqBand = (uint8) IQ_BAND_INFO_UTRA_FDD_VIII;
      break;
    case SYS_BAND_WCDMA_IX_1700:
      metric.ucFreqBand = (uint8) IQ_BAND_INFO_UTRA_FDD_IX;
      break;
    case SYS_BAND_WCDMA_XI_1500:
      metric.ucFreqBand = (uint8) IQ_BAND_INFO_UTRA_FDD_XI;
      break;
    default:
      metric.ucFreqBand = (uint8) IQ_BAND_INFO_NOT_AVAILABLE;
      break;
  }

  MMAL_MSG_SPRINTF_2( "MMAL: GS46, active band = %d, setting metric freq band = %d ", 
                      ss_info_ptr->active_band,
                      metric.ucFreqBand );

/*=========================================================================*/

  /* only submit if there has been a change */
  if ( memcmp( &last_metric, &metric, sizeof(last_metric) ) )
  {
    IQ_SubmitMetric( IQ_METRIC_ID_GS46, &metric, sizeof( metric ) );
    /* store the new metric */
    memcpy( &last_metric, &metric, sizeof(last_metric) );

    MMAL_MSG( "MMAL: GS46 submitted" );
  }
  else
  {
    MMAL_MSG( "MMAL: GS46 saw no change, not submitted" );
  }
}


/*==========================================================================

  FUNCTION:  mmal_ciq_gs47_cm_ss_event

===========================================================================*/
/*!
    @brief
    Called when serving system changes to process GS47

    @detail

    @return

    @Note

*/
/*=========================================================================*/
void mmal_ciq_gs47_cm_ss_event
(
  cm_ss_event_e_type           ss_event,     /* The event that occurred */
  const cm_mm_ss_info_s_type  *ss_info_ptr   /* Serving system information */
)
{

  iq_metric_GS47_t             metric;
  static iq_metric_GS47_t      last_metric;

/*=========================================================================*/

  MMAL_MSG( "MMAL: GS47 callback entered" );

  memset( &metric, 0, sizeof(metric) );

  if ( SYS_SRV_STATUS_NO_SRV == ss_info_ptr->srv_status ||
       SYS_SYS_MODE_NO_SRV   == ss_info_ptr->sys_mode )
  {
    metric.ucRadioSvcState = (uint8)IQ_RADIO_SERV_STATE_SEARCHING;
  }
  else if ( SYS_SRV_STATUS_PWR_SAVE == ss_info_ptr->srv_status )
  {
    metric.ucRadioSvcState = (uint8)IQ_RADIO_SERV_STATE_NO_SERVICE;
  }
  else
  {
    switch( ss_info_ptr->sys_mode )
    {
      case SYS_SYS_MODE_AMPS:
        metric.ucRadioSvcState = (uint8)IQ_RADIO_SERV_STATE_UNKNOWN;
        break;
      case SYS_SYS_MODE_CDMA:
        metric.ucRadioSvcState = (uint8)IQ_RADIO_SERV_STATE_3G;
        break;
      case SYS_SYS_MODE_GSM:
        metric.ucRadioSvcState = (uint8)IQ_RADIO_SERV_STATE_2G;
        break;
      case SYS_SYS_MODE_HDR:
        metric.ucRadioSvcState = (uint8)IQ_RADIO_SERV_STATE_3G;
        break;
      case SYS_SYS_MODE_WCDMA:
        metric.ucRadioSvcState = (uint8)IQ_RADIO_SERV_STATE_3G;
        break;
      case SYS_SYS_MODE_GPS:
        metric.ucRadioSvcState = (uint8)IQ_RADIO_SERV_STATE_UNKNOWN;
        break;
      case SYS_SYS_MODE_GW:
        metric.ucRadioSvcState = (uint8)IQ_RADIO_SERV_STATE_3G;
        break;
      case SYS_SYS_MODE_WLAN:
        metric.ucRadioSvcState = (uint8)IQ_RADIO_SERV_STATE_UNKNOWN;
        break;
      case SYS_SYS_MODE_LTE:
        metric.ucRadioSvcState = (uint8)IQ_RADIO_SERV_STATE_4G;
        break;
      case SYS_SYS_MODE_GWL:
        metric.ucRadioSvcState = (uint8)IQ_RADIO_SERV_STATE_4G;
        break;
      default:
        metric.ucRadioSvcState = (uint8)IQ_RADIO_SERV_STATE_UNKNOWN;
        break;
    }
  }

  metric.ucNetworkSvcState = (uint8)IQ_NETW_SERV_STATE_NONE;

  if ( ( IQ_RADIO_SERV_STATE_2G == metric.ucRadioSvcState ) ||
       ( IQ_RADIO_SERV_STATE_3G == metric.ucRadioSvcState ) ||
       ( IQ_RADIO_SERV_STATE_4G == metric.ucRadioSvcState ) )
  {
    /* TODO: start assuming 'home'? */
    metric.ucNetworkSvcState = (uint8)IQ_NETW_SERV_STATE_HOME;

    /* TODO: check roaming status?  What is a 'home equivalent'?  This list is not exhaustive! */
    switch ( ss_info_ptr->roam_status )
    {
      case SYS_ROAM_STATUS_ON:
        metric.ucNetworkSvcState = (uint8)IQ_NETW_SERV_STATE_ROAM;
        break;
      case SYS_ROAM_STATUS_FULL_SVC:
        metric.ucNetworkSvcState = (uint8)IQ_NETW_SERV_STATE_HOME_EQUIV;
        break;
      default:
        break;
    }

    /* TODO: how do we detect emergency mode? */
    if ( ( SYS_CELL_ACCESS_EMERGENCY_ONLY == ss_info_ptr->cell_access_info.cs_bar_status ) &&
         ( SYS_CELL_ACCESS_EMERGENCY_ONLY == ss_info_ptr->cell_access_info.ps_bar_status ) )
    {
      metric.ucNetworkSvcState = (uint8)IQ_NETW_SERV_STATE_EMERGENCY;
    }
  }

  switch( ss_info_ptr->srv_status )
  {
    case SYS_SRV_STATUS_LIMITED:
    case SYS_SRV_STATUS_LIMITED_REGIONAL:
      metric.ucNetworkSvcState = (uint8)IQ_NETW_SERV_STATE_EMERGENCY;
      break;
    default:
      /* go with above values */
      break;
  }

/*=========================================================================*/

  /* only submit if there has been a change */
  if ( memcmp( &last_metric, &metric, sizeof(last_metric) ) )
  {
    IQ_SubmitMetric( IQ_METRIC_ID_GS47, &metric, sizeof( metric ) );
    /* store the new metric */
    memcpy( &last_metric, &metric, sizeof(last_metric) );

    MMAL_MSG( "MMAL: GS47 submitted" );
  }
  else
  {
    MMAL_MSG( "MMAL: GS47 saw no change, not submitted" );
  }
}


/*==========================================================================

  FUNCTION:  mmal_ciq_gs6E_cm_ss_event

===========================================================================*/
/*!
    @brief
    Called when serving system changes to process GS6E

    @detail

    @return

    @Note

*/
/*=========================================================================*/
void mmal_ciq_gs6E_cm_ss_event
(
  cm_ss_event_e_type           ss_event,     /* The event that occurred */
  const cm_mm_ss_info_s_type  *ss_info_ptr   /* Serving system information */
)
{

  iq_metric_GS6E_t             metric;
  static iq_metric_GS6E_t      last_metric;

/*=========================================================================*/

  MMAL_MSG( "MMAL: GS6E callback entered" );

  memset( &metric, 0, sizeof(metric) );

  switch( ss_info_ptr->sys_mode )
  {
    case SYS_SYS_MODE_NO_SRV:
      break;
    case SYS_SYS_MODE_AMPS:
      break;
    case SYS_SYS_MODE_CDMA:
      metric.ucRadioMode = (uint8)IQ_RADIO_MODE_RTT;
      break;
    case SYS_SYS_MODE_GSM:
      metric.ucRadioMode = (uint8)IQ_RADIO_MODE_GSM;
      break;
    case SYS_SYS_MODE_HDR:
      metric.ucRadioMode = (uint8)IQ_RADIO_MODE_EVDO;
      break;
    case SYS_SYS_MODE_WCDMA:
      metric.ucRadioMode = (uint8)IQ_RADIO_MODE_WCDMA;
      break;
    case SYS_SYS_MODE_GPS:
      break;
    case SYS_SYS_MODE_GW:
      break;
    case SYS_SYS_MODE_WLAN:
      break;
    case SYS_SYS_MODE_LTE:
      metric.ucRadioMode = (uint8)IQ_RADIO_MODE_LTE;
      break;
    default:
      break;
  }
  
//  ss_info_ptr->cell_srv_ind.hs_ind;
//  ss_info_ptr->cell_srv_ind.hs_call_status;
  
/*=========================================================================*/

  /* only submit if there has been a change */
  if ( memcmp( &last_metric, &metric, sizeof(last_metric) ) )
  {
    IQ_SubmitMetric( IQ_METRIC_ID_GS6E, &metric, sizeof( metric ) );
    /* store the new metric */
    memcpy( &last_metric, &metric, sizeof(last_metric) );

    MMAL_MSG( "MMAL: GS6E submitted" );
  }
  else
  {
    MMAL_MSG( "MMAL: GS6E saw no change, not submitted" );
  }
}

/*
typedef enum
{
	 IQ_RADIO_MODE_NONE = 0
	,IQ_RADIO_MODE_GSM
	,IQ_RADIO_MODE_GPRS
	,IQ_RADIO_MODE_EDGE
	,IQ_RADIO_MODE_WCDMA
	,IQ_RADIO_MODE_HSDPA
	,IQ_RADIO_MODE_HSUPA
	,IQ_RADIO_MODE_HSPA
	,IQ_RADIO_MODE_HSPA_PLUS
	,IQ_RADIO_MODE_LTE
	,IQ_RADIO_MODE_RTT
	,IQ_RADIO_MODE_EVDO
} iq_radio_mode_t;
*/

/*===========================================================================

  FUNCTION:  mmal_ciq_metrics_query_submit_metric

===========================================================================*/
/*!
    @brief
    This function will first call the shouldSubmitMetric and if the 
    result is sucessful   will call the submit metric function.
    
    @detail
    This function could be optimized such that if the shouldSubmitMetric returns
    failure, then readjust the masks so that only when the profile changes, 
    this call is amde again for this metric.
    
    @return
    

    @Note
    

*/
/*=========================================================================*/
void mmal_ciq_metrics_query_submit_metric
(
  const iq_metric_id_t metric_id,
  const void *payload,
  const uint32 payload_size 
)
{
//  iq_boolean_t  ret_val;
//  iq_timestamp_t time_stamp;
  iq_result_t     res;


  /*----------------------------------------------------------------------
  @return  The result of the attempt to submit a metric.
  *@retval  ::IQ_SUCCESS      this metric was successfully processed
  *@retval  ::IQ_NOT_INITIALIZED  the library is not initialized; 
  the metric was not submitted
  *@retval  ::IQ_INVALID_PARAM    @a payload was NULL, @a payloadSize 
  was zero, or 
  *                @a timestamp has an invalid value; the metric was not submitted
  *@retval  ::IQ_OUT_OF_MEMORY    the library ran out of memory when 
  trying to submit the metric;
  *                the metric was not submitted
  -----------------------------------------------------------------------*/
  res = IQ_SubmitMetric(
                       metric_id,     payload,     payload_size    );

  if (res != IQ_SUCCESS)
  /*! @TODO -Add stats,  have a f3 to print the Result */
  {
    MMAL_CIQ_INCR_ERR_CNT();
    MMAL_CIQ_INCR_CIQ_ERR_CNT();
  }

}

/*===========================================================================

  FUNCTION:  mmal_ciq_ss25_query

===========================================================================*/
/*!
    @brief
    This function will fill out and submit metric SS25.
    
    @detail
    This function fills out the vendor, model, and version of the chipset
    being used.  In conjunction with a similar imlementation on the
    applications proccessor, it will provide the necessary info for
    the SS25 requirements.
    
    @return
    None
*/
/*=========================================================================*/
void mmal_ciq_ss25_query( void )
{
  iq_iovec_t           iov_ss25[4];
  iq_result_t          iq_sub_retval = IQ_ERROR;

  iov_ss25[0].data = szVendor;
  iov_ss25[0].len  = sizeof(szVendor);

  iov_ss25[1].data = szModel;
  iov_ss25[1].len  = sizeof(szModel);

  iov_ss25[2].data = szVersion;
  iov_ss25[2].len  = sizeof(szVersion);

  iov_ss25[3].data = NULL;
  iov_ss25[3].len  = 0;

  MMAL_MSG("MMAL: Submitting SS25");
  iq_sub_retval = IQ_SubmitMetricV(IQ_METRIC_ID_SS25, (iq_iovec_t *)&iov_ss25);

  if ( iq_sub_retval != IQ_SUCCESS )
  {
    MMAL_MSG("MMAL: Submission of SS25 failed!");
  }
} /* mmal_ciq_ss25_query */

/*===========================================================================

  FUNCTION:  mmal_ciq_ss2b_query

===========================================================================*/
/*!
    @brief
    This function will fill out and submit metric SS2B.
    
    @detail
    This function fills out the version of the baseband processor that
    is being used.  In conjunction with a similar imlementation on the
    applications proccessor, it will provide the necessary info for
    the SS2B requirements.
    
    @return
    None
*/
/*=========================================================================*/
void mmal_ciq_ss2b_query( void )
{
  iq_metric_SS2B_t     ss2b_metric;
  iq_iovec_t           iov_ss2B[4];
  iq_version_struct_t  tVersions[1];
  iq_result_t          iq_sub_retval = IQ_ERROR;

  ss2b_metric.ucNumVersions = 1;

  tVersions[0].ucVersionType = IQ_VERSION_FIRMWARE_BP;

  iov_ss2B[0].data = &ss2b_metric;
  iov_ss2B[0].len  = sizeof(ss2b_metric);

  iov_ss2B[1].data = tVersions;
  iov_ss2B[1].len  = sizeof(tVersions);

  iov_ss2B[2].data = szVersion_proc;
  iov_ss2B[2].len  = sizeof(szVersion_proc);

  iov_ss2B[3].data = NULL;
  iov_ss2B[3].len  = 0;

  MMAL_MSG("MMAL: Submitting SS2B");
  iq_sub_retval = IQ_SubmitMetricV(IQ_METRIC_ID_SS2B, (iq_iovec_t *)&iov_ss2B);

  if ( iq_sub_retval != IQ_SUCCESS )
  {
    MMAL_MSG("MMAL: Submission of SS2B failed!");
  }
} /* mmal_ciq_ss2b_query */

#endif /* FEATURE_CIQ_MMAL*/
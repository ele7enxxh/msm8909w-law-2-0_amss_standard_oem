#ifndef PS_IPDC_LOGGING_H
#define PS_IPDC_LOGGING_H
/*===========================================================================
             P S _ I P D C _ L O G G I N G . H

DESCRIPTION
  Header file for the IP Data Compression (IPDC) protocol's logging support.
 
  

Copyright (c) 2013 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/dc/ipdc/v0/inc/ps_ipdc_logging.h#1 $
  $Author: mplcsds1 $
  $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/06/14    vl    BSS optimization changes
01/07/13    pc    Created module.
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "log.h"
#include "ps_logging_defs.h"
#include "ps_ipdc.h"

/*===========================================================================

                              DATA TYPES

===========================================================================*/

/*---------------------------------------------------------------------------
  Log packet for IPDC Compressor Metrics
  Log Code:  LOG_IPDC_COMPRESSOR_METRICS_C
 
 This information is collected for a Single Packet 
---------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  /* Packet Info */
  uint16                  type;     /* 0: IPV4_PKT, 1: IPV4_TCP_PKT, 2: IPV4_UDP_PKT, 3: IPV6_PKT, 4: UNKNOWN_PKT */
  uint16                  ipdc_hdr; /* 0: Uncompressed, 1: No Matches, 2: Hdr Compressed, 3: Pkt Compressed */

  /* Flow Id */
  uint32                  src_addr[4];  // If IPv4 packet, address resides in first 4 bytes   
  uint16                  src_port;     
  uint32                  dst_addr[4];  // If IPv4 packet, address resides in first 4 bytes    
  uint16                  dst_port; 
 
  // Metrics
  uint16                  orig_pkt_header_len;      /* Total nr of bytes in the original packet that are considered to be header */
  uint16                  orig_pkt_payload_len;     /* Total nr of bytes in the original packet that are considered to be payload */

  uint16                  comp_pkt_header_len;      /* Total nr of header bytes in a compressed Pkt (rough estimation).  Zero if packet is sent uncompressed. */
  uint16                  comp_pkt_payload_len;     /* Total nr of payload bytes in a compressed Pkt (rough estimation). Zero if packet is sent uncompressed. */

  uint16                  uncomp_pkt_len;           /* Total nr of bytes that were Tx'd in an uncompressed Pkt. Zero if packet is sent compressed.  */

} ps_ipdc_log_comp_metrics_s_type;

/* Number of Samples in IPDC Compressor Metrics log packet */
#define PS_IPDC_COMP_METRICS_NUM_SAM 50 

typedef PACKED struct PACKED_POST
{
  uint8                                  num_sample;
  ps_ipdc_log_comp_metrics_s_type        metrics[PS_IPDC_COMP_METRICS_NUM_SAM];
} ps_ipdc_comp_metrics_log_packet_s_type;

LOG_RECORD_DEFINE(LOG_IPDC_COMPRESSOR_METRICS_C)
  uint8                                  version; 
  ps_ipdc_comp_metrics_log_packet_s_type info;
LOG_RECORD_END


/*---------------------------------------------------------------------------
  Log packet for IPDC Compressor Statistics
  Log Code:  LOG_IPDC_COMPRESSOR_PER_FLOW_STATS_C
 
    This information is collected for Multiple Packets and
    for Multiple IP Flows
 
    Multiple IP Flows Stats are only collected when the UE's IPDC
    Configuration is set to compress both Header and Payload.
---------------------------------------------------------------------------*/
#define PS_IPDC_COMP_MAX_COMPRESSOR_FLOWS 16

typedef PACKED struct PACKED_POST
{
  /* Flow Id Info */
  uint8  ip_version;          // 4 - IPv4, 6 – IPv6 (all other values are reserved)
  
  /* Engineering Note:
     Filtered Compression Gain is computed on the payload portions of all incoming packets
     for which header and payload compression was attempted for this flow
  */
  uint16 filtered_comp_gain;  // Filtered compression gain % for this flow

  uint32 src_addr[4];         // If IPv4 packet flow, address resides in first 4 bytes 
  uint16 src_port;     
  uint32 dst_addr[4];         // If IPv4 packet flow, address resides in first 4 bytes 
  uint16 dst_port; 
     
  // Stats for this flow's packets
  uint16 total_incoming_pkts_recvd_for_flow;     /* Total Nr of incoming packets received for this flow.   */
  uint16 total_comp_hdr_payload_pkts_sent;       /* Total Nr of compressed header and payload packets sent for this flow.   */
  uint16 total_comp_hdr_only_pkts_sent;          /* Total Nr of compressed header only packets sent for this flow.   */
  uint16 total_comp_no_matches_pkts_sent;        /* Total Nr of compressed packets with no matches sent for this flow.   */

  /* This Flow’s Incoming Packet Stats */
  uint32 incoming_pkts_header_bytes;    /* Total nr of bytes within the incoming packets that are considered to be header */
  uint32 incoming_pkts_payload_bytes;   /* Total nr of bytes within the incoming packet that are considered to be payload */

  /* This Flow’s Compressed Packet Stats for Pkt Types 11 & 10 & 01 : stats for Original Packets which were compressed      */
  uint32 comp_pkts_header_bytes;    /* Total nr of header bytes Tx’d in compressed Pkts (rough estimation) */
  uint32 comp_pkts_payload_bytes;   /* Total nr of payload bytes Tx’d in compressed Pkts (rough estimation) */

} ps_ipdc_log_comp_flow_stats_s_type;

typedef PACKED struct PACKED_POST
{
// UE Target  : All Stats counters are reset after being reported 

  uint16 total_comp_gain;              // total compression gain across all flows + unclassified packets

  /* The outgoing IPDC Header Byte tags these types of packets */
  uint16 compressed_hdr_payload_pkts; /* Nr of packets sent with header and payload compressed.   */
  uint16 compressed_hdr_only_pkts;    /* Nr of packets sent with only header compressed.   */
  uint16 compressed_no_matches_pkts;  /* Nr of packets sent compressed with no matches.   */
  uint16 uncompressed_pkts;           /* Nr of packets sent uncompressed. */
  uint16 comp_failed_pkts;            /* Nr of compression failures.      */

  /* Stats for Unclassified Packets */
  /* 
     If the UE is configured to compress header and payload, unclassified packets is defined to be:
     - all small packets (packet size less than or equal to the configured header size that bypass the incompressible flows state machine)
     and
     - all packets that are neither IPv4 packets nor IPv6 packets.

     If the UE is configured to compress only headers, unclassified packets is defined to be:
     - all incoming packets for which compression was attempted.
  */
  /* Stats for Unclassified Packets (i.e., packets that are compressed and are Tx'd with a Packet Type of '11', '10', or '01' */
  uint32 unclassified_incoming_pkts_header_bytes;    /* Total nr of header bytes received in incoming unclassified Pkts (rough estimation) */
  uint32 unclassified_incoming_pkts_payload_bytes;   /* Total nr of payload bytes received in incoming unclassified Pkts (rough estimation) */
  uint32 unclassified_compressed_pkts_header_bytes;    /* Total nr of header bytes Tx'd in unclassified compressed outgoing Pkts (rough estimation) */
  uint32 unclassified_compressed_pkts_payload_bytes;   /* Total nr of payload bytes Tx'd in unclassified compressed outgoing Pkts (rough estimation) */

  /* Stats for Uncompressed Packets (i.e., packets that bypass compression and are Tx'd with a Packet Type of '00' */
  uint32 uncompressed_incoming_pkts_bytes;  /* Total nr of packet bytes received in incoming uncompressed Pkts */   
  uint32 uncompressed_outgoing_pkts_bytes;  /* Total nr of packet bytes Tx'd in outgoing uncompressed Pkts. */


  //-------------------------------------------------------------------------
  // Compressor Stats for each Flow
  //-------------------------------------------------------------------------
  uint8                                  num_valid_flows;
  ps_ipdc_log_comp_flow_stats_s_type     flow_stats[PS_IPDC_COMP_MAX_COMPRESSOR_FLOWS];

} ps_ipdc_comp_stats_log_packet_s_type;


LOG_RECORD_DEFINE(LOG_IPDC_COMPRESSOR_STATS_C)
  uint8                                  version; 
  ps_ipdc_comp_stats_log_packet_s_type   stats;
LOG_RECORD_END


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION IPDC_LOGGING_SET

DESCRIPTION
  Sets up IPDC logging if called.

DEPENDENCIES
  The IPDC memheap must have been initialized and ipdc_alloc called prior to
  calling ipdc_set_logging()

PARAMETERS
  ipdc_handle_type *handle - pointer to the ipdc handle

RETURN VALUE
   0   if successful.
  -1   in case of null arguments.

SIDE EFFECTS
  Sets the IPDC logging control block address variable
===========================================================================*/
int ipdc_logging_set
(
  ipdc_handle_type   handle
);

/*===========================================================================
FUNCTION IPDC_LOGGING_FREE

DESCRIPTION
  Frees up logging so it is no longer done.

DEPENDENCIES
  The IPDC memheap must have been initialized and ipdc_alloc() called prior to
  calling ipdc_free_logging()

PARAMETERS
  ipdc_handle_type *handle - pointer to the ipdc handle

RETURN VALUE
   0   if successful.
  -1   in case of null handle.

SIDE EFFECTS
  Sets the IPDC logging control block address variable to null
===========================================================================*/
int ipdc_logging_free
(
  ipdc_handle_type handle
);

/*===========================================================================
FUNCTION PS_IPDC_LOG_UPDATE_LOG_DATA

DESCRIPTION
  Updates the IPDC Instance's Compressor Engine log data.

DEPENDENCIES
  The IPDC instance must have been initialized.

PARAMETERS
  ipdc_handle_type *handle - pointer to the ipdc handle

RETURN VALUE
  none

SIDE EFFECTS
  Copies the IPDC Instance's Compressor Engine stats into the IPDC
  Compressor Statistics log packet
===========================================================================*/
void ps_ipdc_log_comp_update_log_data
(
  ipdc_handle_type  handle,
  boolean           update_metrics,
  boolean           update_stats
);

/*===========================================================================
FUNCTION PS_IPDC_LOG_DECOMP_ENG_METRICS

DESCRIPTION
  Logs the IPDC Instance's Decompressor Engine metrics.

DEPENDENCIES
  The IPDC instance must have been initialized.

PARAMETERS
  ipdc_handle_type *handle - pointer to the ipdc handle

RETURN VALUE
  none

SIDE EFFECTS
  Copies the IPDC Instance's Decompressor Engine metrics into the IPDC
  Decompressor Metrics log packet
===========================================================================*/
void ps_ipdc_log_decomp_eng_metrics
(
  ipdc_handle_type handle
);

/*===========================================================================
FUNCTION IPDC_LOGGING_LOG_COMP_PKT

DESCRIPTION
  Logs an IPDC Compressed Packet.

DEPENDENCIES

PARAMETERS
  ipdc_handle_type *handle - pointer to the ipdc handle
  dsm_item_type    *pkt_ptr - pointer to DSM item containing the Packet to be logged.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ipdc_logging_log_comp_pkt
(
  ipdc_handle_type               handle,
  dsm_item_type                 *pkt_ptr
);

/*===========================================================================
FUNCTION IPDC_LOGGING_LOG_COMP_ENGINE_MEMORY

DESCRIPTION
  Logs an IPDC Compressor Engine's Compression Memory.

DEPENDENCIES

PARAMETERS
  ipdc_handle_type *handle - pointer to the ipdc handle

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ipdc_logging_log_comp_engine_memory
(
  ipdc_handle_type               handle
);

/*===========================================================================
FUNCTION IPDC_ALLOC_COMP_METRICS_STATS_LOG_PKTS

DESCRIPTION
  Allocates memory for logging compression metrics and stats.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean ipdc_alloc_comp_metrics_stats_log_pkts(void);

#endif /* PS_IPDC_LOGGING_H */

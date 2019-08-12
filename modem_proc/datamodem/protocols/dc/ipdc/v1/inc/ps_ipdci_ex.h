#ifndef PS_IPDCI_EX_H
#define PS_IPDCI_EX_H

#ifdef __cplusplus
extern "C" {
#endif

/*==========================================================================

                         P S _ I P D C I . H

GENERAL DESCRIPTION
  This file provides internal IP Data Compression (IPDC) Library's data and
  function definitions.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2013 QUALCOMM Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*==========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/data.mpss/1.0/protocols/hc/src/ps_ipdci.h#1
  $DateTime: 2013/01/10 21:41:01
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/15/15   cx      Added support for optimized UDC v1
01/10/13   pc      Initial version

===========================================================================*/


/*==========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "ps_ipdc_ex.h"
#include "ps_ipdc_platform_defs.h"

#if defined(IPDC_ON_TARGET) || defined(IPDC_ON_MOB)

#include "ps_comp_logging_helper.h"

#endif

  #define IPDC_META_SH_MATCH_LEN        5
  #define IPDC_META_SH_MISMATCH_LEN     3

/*==========================================================================

                              CONSTANTS

===========================================================================*/

/*--------------------------------------------------------------------------
  Always enable debug prints for MOB scenario
---------------------------------------------------------------------------*/
#ifdef TEST_FRAMEWORK 
#error code not present
#endif

/* Decomp Stats not needed on target */
#if !defined(IPDC_ON_TARGET)
#define FEATURE_IPDC_DECOMP_STATS
#endif

/* Only for on target */
#if defined (IPDC_ON_TARGET)
// Feature Flags for Q6 Optimizations
//#define Q6_OPTIMIZED_HKEY_COMPUTATION
#define Q6_OPTIMIZED_SEARCH
#endif

#define IPDC_SH_HDR_SIZE_IN_BYTES                 5   // Includes IPDC Header (1 Byte) + SH Option (2 bits) + 
                                                        // SH Extension bit (1 bit) + SH Lookback Length (10 bits), 
                                                        // SH Num of Mismatches (4 bits) + Num of Bytes 
                                                        // to Copy (7 bits) + UDC Number of Matches (1 Byte) 
#define IPDC_PTR_META_DATA_SH_BLK_SIZE_IN_BYTES   1
#define IDPC_SH_HDR_NO_DACC_HDER_SIZE_IN_BYTES    4   // UDC common header(1 Byte) + SH header(3 Byte) 
#define IPDC_HDR_SIZE_IN_BYTES                 2   // Includes IPDC Header (1 Byte) + Number of Matches (1 Byte)
#define IPDC_PTR_META_DATA_BLK_SIZE_IN_BYTES   4
#define IPDC_SUPPORTS_NO_MATCHES_PKT_TYPE

/*---------------------------------------------------------------------------
  Definitions of Masks for the IPDC Header's Fields
     - Packet Type: bits 7-5 in first byte of IPDC Header
     - Checksum:    bits 4-1 bits in first byte of IPDC Header
     - Extension:   bit  0 in first byte of IPDC Header
---------------------------------------------------------------------------*/
#define PS_IPDC_HDR_PKT_TYPE_MASK                    0xe0
#define PS_IPDC_HDR_EXT_BIT_MASK                     0x01

/*---------------------------------------------------------------------------
  Definitions for IP Data Compression (IPDC) extension bit that is 
  set in the Compressed Packet's IPDC Header (last bit)
---------------------------------------------------------------------------*/
#define PS_IPDC_WITH_EXT_BIT                         0x01
#define PS_IPDC_WO_EXT_BIT                           0x00

  #define PS_IPDC_SH_OPT_WITH_EXT                    0x60
  #define PS_IPDC_SH_OPT_WO_EXT                      0x40
  #define PS_IPDC_SH_LB_LEN_PART_2_LEN               0x05
  #define PS_IPDC_SH_LB_LEN_PART_2_MASK              0x1F
  #define PS_IPDC_SH_NUM_MISMATCH_PART_1_LEN         0x03
  #define PS_IPDC_SH_NUM_MISMATCH_PART_2_LEN         0x01
  #define PS_IPDC_SH_NUM_MISMATCH_PART_2_MASK        0x01
  #define PS_IPDC_SH_NUM_BYTES_TO_COPY_LEN           0x07

/*--------------------------------------------------------------------------
  IPDC Header Size
  First Byte : Packet Type and Checksum 
  Second Byte : Number of Matches in the packet : Maximum 255
---------------------------------------------------------------------------*/
#define IPDC_HDR_SIZE_IN_BYTES                 2

/*--------------------------------------------------------------------------
  IPDC Meta data block size
  One meta data block identifies one match in the compressed packet.
---------------------------------------------------------------------------*/
#define IPDC_PTR_META_DATA_BLK_SIZE_IN_BYTES   4


/*===========================================================================

                              DATA TYPES

===========================================================================*/
/*--------------------------------------------------------------------------
  IPDC Supported Packet Types
---------------------------------------------------------------------------*/
typedef enum
{
  IPDCI_IPV4_PKT = 0,
  IPDCI_IPV4_TCP_PKT,
  IPDCI_IPV4_UDP_PKT,
  IPDCI_IPV6_PKT,
  IPDCI_IPV6_TCP_PKT,
  IPDCI_IPV6_UDP_PKT,
  IPDCI_UNKNOWN_PKT,
  IPDCI_PKT_MAX  
  
} ipdci_pkt_type;

/*--------------------------------------------------------------------------
  IPDC Header "Packet Type" field values
---------------------------------------------------------------------------*/
typedef enum
{
  IPDC_HDR_NOTHING_COMPRESSED_PKT_TYPE = 0,  
  IPDC_HDR_NO_MATCHES_PKT_TYPE,
  IPDC_HDR_COMPRESSED_HDR_PKT_TYPE,
  IPDC_HDR_COMPRESSED_HDR_PAYLOAD_PKT_TYPE,
  IPDC_HDR_RESET_PKT_TYPE,
  IPDC_HDR_USE_PREV_METADATA_PKT_TYPE,
  IPDC_HDR_PKT_TYPE_MAX
  
} ipdci_ipdc_hdr_pkt_type;

/*--------------------------------------------------------------------------
  IPDC internal IP address storage
  Probably unnecessary, we can use standard one
---------------------------------------------------------------------------*/
typedef uint32 ipdc_ipv6_ipv4_addr_type[4]; 

/*--------------------------------------------------------------------------
  IPDC Packet Info, generated to track flows
---------------------------------------------------------------------------*/
typedef struct
{
  ipdci_pkt_type                     pkt_type;
  uint16                             pkt_size;
  uint8                              ip_header_length;
  ipdc_ipv6_ipv4_addr_type           src_addr;
  uint16                             src_port;
  ipdc_ipv6_ipv4_addr_type           dest_addr;
  uint16                             dest_port;
  uint16                             ip4_ipid;
  
}ipdc_pkt_info_type;

/*---------------------------------------------------------------------------
  IPDC Compressor SH Metrics data structure
 
   This information is collected for a Single Packet 
---------------------------------------------------------------------------*/
typedef struct
{
  /* TODO : IPDC_SH - implement num_exceed_mismatch_len & num_exceed_match_len */
  uint8                     num_exceed_mismatch_len;      
  uint8                     num_exceed_match_len;   
  uint8                     num_exceed_max_lookback_len;
  uint8                     static_hdr_len_bytes;
}ipdc_comp_sh_metrics_t;

/*--------------------------------------------------------------------------
  IPDC Compressor Per Packet Metrics data structure
  This information is collected for individual packets being compressed
---------------------------------------------------------------------------*/
typedef struct 
{
  /* Packet Type Info */
  ipdci_pkt_type            type;
  ipdci_ipdc_hdr_pkt_type ipdc_hdr;

  /* Flow Id */
  /* If IPv4 packet, address resides in src_addr[0] */
  ipdc_ipv6_ipv4_addr_type  src_addr;
  uint16                    src_port;

  /* If IPv4 packet, address resides in dst_addr[0] */
  ipdc_ipv6_ipv4_addr_type  dst_addr;
  uint16                    dst_port;

  /* Metrics */
  /* Total nr of bytes in the original packet that are considered to be 
     header */
  uint16                    orig_hdr_len;

  /* Total nr of bytes in the original packet that are considered to be 
     payload */
  uint16                    orig_payload_len;

  /* Total nr of header bytes in a compressed Pkt (rough estimation) */
  uint16                    comp_pkt_hdr_len;

  /* Total nr of payload bytes in a compressed Pkt (rough estimation) */
  uint16                    comp_pkt_payload_len;

  /* Total nr of bytes that were Tx'd in an uncompressed Pkt */
  uint16                    uncomp_packet_len;

  /*------------------------------------------------------------------------
    Metrics NOT reported to QXDM
  -------------------------------------------------------------------------*/
  /* Total nr of bytes in the original packet */
  uint16                    orig_pkt_len;

  /* Needed for Compression Gain computation! 
     Total nr of bytes in the original packet that were treated as data to be
     compressed (either header + payload or header only */
  uint16                    orig_dtc_len;

  /* Total nr of bytes that were Tx'd in a compressed Pkt */
  uint16                    comp_pkt_len;

  /* Needed for Compression Gain computation! 
     Total nr of header bytes or header + payload bytes that were Tx'd in a 
     compressed Pkt */  
  uint16                    comp_dtr_len;

  uint16                    found_matches;
  uint16                    hash_key_searches;
  uint8                     shrink_performed;
  ipdc_comp_sh_metrics_t    sh_metric;
  /*------------------------------------------------------------------------
    END of metrics NOT reported to QXDM
  -------------------------------------------------------------------------*/

} ipdc_comp_metrics_t;


/*--------------------------------------------------------------------------
  IPDC Compressor maximum number of flows tracked
---------------------------------------------------------------------------*/
#define IPDC_MAX_COMPRESSOR_FLOWS  16
#define IPDC_MAX_META_HIST_SIZE    70

/*--------------------------------------------------------------------------
  The size of histgram in statistics
---------------------------------------------------------------------------*/
//#ifdef FEATURE_IPDC_STAT_V2
#define IPDC_DACC_DYN_HDR_COUNT_HIST_SIZE (128)
#define IPDC_DACC_DYN_HDR_MATCH_WITH_PREV_COUNT_HIST_SIZE (128)
#define IPDC_SH_DYN_HDR_COUNT_HIST_SIZE (32)
#define IPDC_SH_DYN_HDR_MATCH_WITH_PREV_COUNT_HIST_SIZE (32)
#define IPDC_SH_HDR_BYTES_TO_COPY_HIST_SIZE (128)
//#endif

/*---------------------------------------------------------------------------
  IPDC Compressor states of operation
---------------------------------------------------------------------------*/
typedef enum
{
  IPDC_STATE_TESTING_FULL_PACKET_COMPRESSION = 0,
  IPDC_STATE_COMPRESS_FULL_PACKET,
  IPDC_STATE_COMPRESS_HEADER_ONLY

} flow_compression_states_e_type;

/*--------------------------------------------------------------------------
  IPDC Compressor Per Flow Statistics
---------------------------------------------------------------------------*/
typedef struct 
{
  /* Flow Identifier Info */
  boolean flow_in_use;

  /*------------------------------------------------------------------------
    Flow parameters REPORTED to QXDM
  -------------------------------------------------------------------------*/

  /* IP Version in use for this flow.  4 - IPv4, 6 – IPv6 (all other values 
     are reserved) */
  uint8   ip_version;

  /* Filtered compression gain % for this flow
     This gain is used to track whether the flow is incompressible or not.
     This is not the absolute gain for the flow.
     It's updated with the following formula for each packet:
     90% of existing value + 10% of new packet gain
     
     We store percentage extended by 2 decimal digits 
     For e.g. 50%    would be stored as 5000
          and 43.21% would be stored as 4321 */
  uint16  filtered_comp_gain;  

  /* If IPv4 flow, address resides in src_addr[0] */
  ipdc_ipv6_ipv4_addr_type   src_addr;
  uint16                     src_port;

  /* If IPv4 flow, address resides in dst_addr[0] */
  ipdc_ipv6_ipv4_addr_type   dst_addr;
  uint16                     dst_port;
  
  /* Total Nr of packets received for this flow.   */
  uint32 total_orig_pkts_recvd_for_flow;

  /* Total Nr of compressed header and payload packets sent for this 
     (compressible) flow */
  uint32 total_comp_hdr_payload_pkts_sent;

  /* Total Nr of compressed header only packets sent for this 
     (compressible) flow */
  uint32 total_comp_hdr_only_pkts_sent;

  /* Total Nr of compressed packets with no matches sent for this 
     (compressible) flow */
  uint32 total_comp_no_matches_pkts_sent;

  /* This Flow’s Original Packets Stats */
  /* Total nr of bytes within the original packets that are considered to be
     header */
  uint64 orig_pkts_hdr_bytes;

  /* Total nr of bytes within the original packet that are considered to be
     payload */
  uint64 orig_pkts_payload_bytes;

  /* This Flow’s Compressed Packets Stats for Pkt Types 11 & 10 & 01 : 
     stats for Original Packets which were compressed */
  /* Total nr of header bytes Tx’d in compressed Pkts (rough estimation) */
  uint64 comp_pkts_header_bytes;

  /* Total nr of payload bytes Tx’d in compressed Pkts (rough estimation) */
  uint64 comp_pkts_payload_bytes;
  
  /*------------------------------------------------------------------------
    END of Flow parameters REPORTED to QXDM
  -------------------------------------------------------------------------*/

  /* Distinguishes the flow based on the packet type used to initialize the
     flow */
  ipdci_pkt_type             flow_type;

  /* Full Pkt Comression, Hdr Only Compression, or Testing Full Pkt 
     Comression */
  flow_compression_states_e_type  flow_state;

  /* Number of packets that need to be processed before a state change can be
     attempted */
  uint32 num_pkts_to_stay_in_cur_mode;

  /* Number of times the flow has "fallen" to Header Only compression */
  uint16 num_drops_to_hdr_only;

  /* This value is computed at init, and used for backoff computation (this 
     variable is for MIPS savings) */
  uint16 initial_num_pkts_to_not_test;

  /* This value is computed at init, and used for backoff computation (this
     variable is for MIPS savings) */
  uint16 max_drops_to_hdr_only;

  /* NEVER RESET. 
     Packet number of the last packet compressed.
     Used for LRU detection */
  uint64  last_packet_processed_num;

  /* Needed for Compression Gain computation! 
     Total nr of bytes within the original packet that are considered to be 
     data-to-compress */
  uint64 orig_pkts_dtc_size;        
 
  /* Needed for Compression Gain computation! 
     Total nr of header bytes or header + payload bytes that were Tx'd in a 
     compressed Pkt */
  uint64 comp_pkts_dtr_size;  

  /* index for the start of the last index */
  uint16    last_pkt_start_idx;

} ipdc_comp_per_flow_stats_t;

/*--------------------------------------------------------------------------
  IPDC SH header statistics
---------------------------------------------------------------------------*/
typedef struct
{
  uint32                          num_exceed_mismatch_len;      
  uint32                          num_exceed_match_len;    
  uint16                          num_zero_checksum_len; 
  uint16                          num_exceed_max_lookback_len;  
  uint64                          num_meta; /* Num of dynamic SH metadata */
  uint32                          num_meta_hist[IPDC_MAX_META_HIST_SIZE];
  uint16                          num_inst_new_flow;
  uint32                          num_exact_metadata_match;
  uint32                          num_lookback_len_mismatch;
  uint32                          num_lookback_len_mismatch_bytes;
  uint32                          num_short_hdr_lookback_len_greater_than_1023;
  uint32                          num_short_hdr_mismatch_bytes;
  uint32                          num_short_hdr_match_bytes;
  uint32                          num_short_hdr_totoal_bytes_to_copy;

}ipdc_sh_comp_stats_t;

/*--------------------------------------------------------------------------
  IPDC Compressor statistics for warnings
---------------------------------------------------------------------------*/
typedef struct ipdc_comp_stats_warning_t
{
  uint32 num_udc_short_dyn_hdr_match_field_count_greater_than_30;
  uint32 num_udc_short_dyn_hdr_mismatch_field_count_0;
  uint32 num_udc_short_dyn_hdr_match_field_count_0;  
  uint32 num_udc_short_dyn_hdr_count_0;
  uint32 num_udc_dacc_dyn_hdr_count_greater_than_30;
  uint32 num_udc_dacc_dyn_hdr_count_0;  
  uint32 num_exact_match_errors;
} ipdc_comp_stats_warning_s;

/*--------------------------------------------------------------------------
  IPDC Compressor Statistics Data Structure
---------------------------------------------------------------------------*/
typedef struct 
{
  /* NEVER RESET
     Keeps track of the total number of packets that have gone through the
     Compressor Engine */
  uint64 global_pkt_number;            

  /* The outgoing IPDC Header tags these types of packets */
  /* Nr of packets sent with header and payload compressed */
  uint16 compressed_hdr_payload_pkts; 

  /* Nr of packets sent with only header compressed */
  uint16 compressed_hdr_only_pkts;

  /* Nr of packets sent compressed with no matches */
  uint16 compressed_no_matches_pkts;

  /* Nr of packets sent uncompressed */
  uint16 uncompressed_pkts;
  
  /* Nr of bytes sent uncompressed */
  uint64 uncompressed_bytes;

  /* Nr of packets whose size is less than the configured header length 
    (packets that bypass the incompressible flows state machine as there is
    no payload compression gain that can be evaluated) for all flows */
  uint16 compressed_small_pkts;

  /* Similar to compressed_small_pkts above, with no matches while 
     compression */
  uint16 compressed_no_matches_small_pkts;

  /* Similar to compressed_small_pkts above, with no compression attempted on
     the packet */
  uint16 uncompressed_small_pkts;

  /* Nr of compression failures */
  uint16 comp_failed_pkts;

  /* Needed for Compression Gain computation! 
     Total nr of bytes within all original packets */
  uint64 all_orig_pkts_size;

  /* Needed for Compression Gain computation! 
     Total nr of bytes that were either TX'd as compressed bytes
     or TX'd as uncompressed bytes or TX'd as reset pkt in all IPDC Pkts */
  uint64 all_comp_uncomp_rst_pkts_size;
  
  /* Compression Gain is calculated as follows:
     comp_gain = 
       ((all_orig_pkts_size - all_comp_uncomp_rst_pkts_size) * 100) / 
         all_orig_pkts_size; */

  /* Small Packets are defined to be any packet whose size is less than or 
     equal to the configured header length.
     Small packets bypass the incompressible flows state machine as there is
     no payload compression gain that can be evaluated */

  /* Nr of Header bytes in original small packets for all flows */
  uint64 small_pkts_orig_header_bytes;

  /* Nr of bytes in compressed small packets (size is less than or equal to 
     the configured header length (packets that bypass the incompressible 
     flows state machine as there is no payload compression gain that can be
     evaluated) for all flows */
  uint64 small_pkts_comp_header_bytes;

  /* Nr of header bytes in incoming unknown packets on which compression is 
     attempted */
  uint64 unknown_pkts_incoming_header_bytes;

  /* Nr of payload bytes in incoming unknown packets on which compression 
    is attempted */
  uint64 unknown_pkts_incoming_payload_bytes; 

  /* Nr of header bytes in outgoing compressed unknown packets */
  uint64 unknown_pkts_compressed_header_bytes;

  /* Nr of payload bytes in outgoing compressed unknown packets */
  uint64 unknown_pkts_compressed_payload_bytes;

  /* These stats are used to calculate the overall compression gain */

  /* NOT REPORTED TO QXDM! 
     Total nr of bytes within all original packets that are considered to be
     header bytes */
  uint64 orig_pkts_hdr_bytes;

  /* NOT REPORTED TO QXDM! 
     Total nr of bytes within all original packets that are considered to be
     payload bytes */
  uint64 orig_pkts_payload_bytes;

  /* Total nr of bytes received in all incoming uncompressed packets */
  uint64 uncompressed_incoming_pkts_bytes;

  /* Total nr of bytes within TX'd in all uncompressed packets */
  uint64 uncompressed_outgoing_pkts_bytes;

  /*------------------------------------------------------------------------
    Stats that are collected only when the configuration is to Compress 
    Headers only (i.e., bypasses incompressible flows state machine)
  -------------------------------------------------------------------------*/
  /* Total nr of bytes within all compressed packets that are considered to 
     be header bytes (rough estimate)  when Header only compression is 
     configured */
  uint64 comp_pkts_hdr_bytes;

  /* Total nr of bytes within all compressed packets that are considered to 
     be payload bytes (rough estimate) when Header only compression is 
     configured */
  uint64 comp_pkts_payload_bytes;
  
  /* Total nr of key ssearch times */
  uint32 hash_key_searches;

  /* Total nr of hash collisons*/
  uint32 hash_collisions;

  /* Total nr of saved bytes by SH*/
  uint64 sh_saved_bytes;
  
  /*------------------------------------------------------------------------
    END of Stats that are collected only when the configuration is to 
    Compress Hdrs only (i.e., bypasses incompressible flows state machine)
  -------------------------------------------------------------------------*/

  /* Stats for each Flow */
  ipdc_comp_per_flow_stats_t  flow_stats[IPDC_MAX_COMPRESSOR_FLOWS];

  /* Flow info for uidentifiable flows (those that are non-IP) */
  ipdc_comp_per_flow_stats_t  untracked_flow_stats;

  uint64                          num_meta; /* Num of dynamic UDC metadata */
  uint32                          num_meta_hist[IPDC_MAX_META_HIST_SIZE];
  ipdc_sh_comp_stats_t            sh_comp_stats;
  uint32                          num_prev_metadata_match;
  uint64                          orig_pkt_len;
  uint64                          comp_pkt_len;
} ipdc_comp_stats_t;

/*---------------------------------------------------------------------------
  IPDC Decompressor Metrics data structure
---------------------------------------------------------------------------*/
typedef struct 
{
  ipdc_pkt_e_type       type;
  uint16                bytes_orig;
  uint16                bytes_decomp;
  uint16                found_matches;
} ipdc_decomp_metrics_t;


/*--------------------------------------------------------------------------
  IPDC Decompressor Statistics data structure
  Channel specific statistics for the decompressor.
  The stats can be accessed via ipdc_decomp_get_stats() function.
---------------------------------------------------------------------------*/
typedef struct 
{
  /* General */
  uint32 out_packets;           /* Nr of packets generated by decompressor.      */
  uint32 orig_pkt_size;         /* Total nr of bytes in original packets         */
  uint32 bytes_decomp;          /* Total nr of bytes after decompression.*/

  /* The incoming IPDC Header tags these types of packets */
  uint32 decompressed;          /* Nr of packets decompressed */
  uint32 out_packets_type_00;   /* Nr of packets with packet type 00 */
  uint32 out_packets_type_11;   /* Nr of packets with packet type 11  */
  uint32 out_packets_type_10;   /* Nr of packets with packet type 10  */
  uint32 out_packets_type_01;   /* Nr of packets with packet type 01  */
  uint32 decomp_failed;         /* Nr of decompression failures  */
  uint32 found_matches;
} ipdc_decomp_stats_t;

/*--------------------------------------------------------------------------
  IPDC decompressor memory 
---------------------------------------------------------------------------*/
typedef struct 
{
  /* ULCM Memory */
  byte     *mem;

  /* ULCM end index, when ULCM is not full */
  uint16    end_idx;

  /* ULCM Capacity */
  uint16    capacity;
  
} DecompMem ;

/*--------------------------------------------------------------------------
  Hash Function Pointer
  Prototype for a hash function, since IPDC supports multiple Hash 
  Functions
---------------------------------------------------------------------------*/
typedef uint16 (*HashFuncPtrType)(uint16 hashbucket_size,byte a,byte b,byte c,byte d,byte e);

/*--------------------------------------------------------------------------
  IPDC Compressor Extended Memory Structure (ECM)
---------------------------------------------------------------------------*/
typedef struct 
{
  /* Boolean denoting whether ECM is empty or prefilled */
  byte      empty;

  /* ECM memory pointer */
  byte     *mem;

  /* ULCM start index within ECM */
  uint16    start_idx;

  /* ULCM end index within ECM */
  uint16    end_idx;

  /* index for the start of the last packet */
  uint16    last_pkt_start_idx;

  /* ULCM capacity */
  uint16    cap;

  /* Maximum accessible index in the ECM */
  uint16    max_end_idx;

  /* Start index of Data to Compress */
  uint16    dtc_start_idx;

  /* End index of Data to Compress */
  uint16    dtc_end_idx;
  
} ExtCompMem;


/*--------------------------------------------------------------------------
  IPDC Hash Bucket 
  Maintains all data for generating/storing hash keys for data present in
  the extended compressor memory (ECM)
---------------------------------------------------------------------------*/
typedef struct
{
  /* Denotes the size of the Hash Value array */
  uint16                     hashbucket_size;

  /* 64 bit hash mask derived from hash_bucket_size
     Feature supported on UE Target Only! */
#ifdef Q6_OPTIMIZED_HKEY_COMPUTATION
  uint64                     hash_mask;
#endif  // Q6_OPTIMIZED_HKEY_COMPUTATION

  /* Array of Hash Values: generated for each 5 byte string sequence in ECM
     Index of an array element represents the hash value, 
     Value of an array element denotes the index in Hash Key array */
  uint16                    *hash_vals_array;

  /* Array of Hash Keys
     Index of an array element represents the index in ECM for the beginning 
     of the five byte block corresponding to this Hash Key
     Value at an index points to another index within same array, forming a 
     chain of all five byte blocks in ECM which generate same hash value. */
  uint16                    *hash_keys_array;

  /* Denotes the size of the Hash Key array
     Same as ECM size */
  uint32                     allocated_hkey_cnt;

  /* Function being used for Hash Value generation */
  HashFuncPtrType            myHashFunc;
  
} HashBuckets;

   typedef struct
   {
     uint8 match_len        :IPDC_META_SH_MATCH_LEN;
     uint8 mismatch_len     :IPDC_META_SH_MISMATCH_LEN;
   }PointerMetadataSH;

/*--------------------------------------------------------------------------
  Pointer Meta Data for each compressed block in the compress packet.
  Each pointer meta data block is 4 bytes in compressed packet.
  Values of fields in this struct are shifted appropriately to fit in a 
  4 byte meta data block.
---------------------------------------------------------------------------*/
typedef struct 
{
  uint16 distance_to_ptr;
  uint16 lookback_len;
  uint8 match_len;
  
}PointerMetadata;

/*--------------------------------------------------------------------------
  SH data structure
---------------------------------------------------------------------------*/
  typedef struct
  {
    uint8                               num_mismatches;    // current number of Pointer Meta Data SH blocks that have been added to the Compressed Packet's DSM Item
    PointerMetadataSH                   pointer_metadata_array[PS_IPDC_SH_MAX_NUM_MISMATCHES];
    uint8                               last_pkt_num_mismatches; // last pkt number of Pointer Meta Data SH blocks that have been added to the Compressed Packet's DSM Item
    PointerMetadataSH                   last_pkt_pointer_metadata_array[PS_IPDC_SH_MAX_NUM_MISMATCHES];
    uint16                              last_pkt_lookback_len;
    uint8                               last_pkt_num_bytes_copy;
    uint16                              lookback_len;
    uint8                               num_bytes_copy;
  }ipdc_sh_comp_ctl_blk_s_type;

/*--------------------------------------------------------------------------
  IPDC Compressor Control Block 
  All of compressor instance specific info is maintained in this.
---------------------------------------------------------------------------*/
typedef struct 
{
  /* Compressor Engine's Configuration Parameters */
  ipdc_comp_config_options_s_type     comp_params;

  /* Compressor Engine's Extended Compression Memory (ECM) data structure */
  ExtCompMem                          cmp_mem;

  /* Pointer to the Compressor Engine's Hash Buckets data structure */
  HashBuckets                        *hb_ptr;

  /* Compressor Engine's Per Packet Metrics */
  ipdc_comp_metrics_t                 per_pkt_metrics;

  /* Compressor Engine's Statistics */
  ipdc_comp_stats_t                   comp_stats;

  /* Current number of Pointer Meta Data blocks that have been added to the 
     Compressed Packet's DSM Item */
  uint8                               num_matches;

  /* Array of meta data blocks added in compressed packet */
  PointerMetadata                     pointer_metadata_array[PS_IPDC_MAX_NUM_MATCHES];
  uint8                               last_pkt_num_matches; // last pkt number of Pointer Meta Data blocks that have been added to the Compressed Packet's DSM Item
  PointerMetadata                     last_pkt_pointer_metadata_array[PS_IPDC_MAX_NUM_MATCHES];
  ipdci_ipdc_hdr_pkt_type             last_pkt_type;
  ipdc_sh_comp_ctl_blk_s_type         sh_comp_ctl_blk;          // control block for SH

  /* Used to track the previous match while compressing the packet */
  uint16                              prev_match_idx;

  /* What packet type is being compressed: IPv4/v6/TCP/UDP */
  ipdci_pkt_type                      base_pkt_type;

  /* Look Forward String (LFS) Start (index into ECM extended region) */
  uint16                              lfs_start;

#if !defined(IPDC_ON_Q6SIM) && !defined(IPDC_ON_OFFTARGET_TOOL)
  /* Support for DPL logging of Compressed Packets & Compression Memory */
  ps_comp_logging_handle_type         logging_handle;
#endif
} ipdc_comp_ctl_blk_s_type;


/*--------------------------------------------------------------------------
  IPDC Decompressor Control Block 
  All of decompressor instance specific info is maintained here.
---------------------------------------------------------------------------*/
typedef struct
{
  /* Decompressor Engine's Configuration Parameters */
  ipdc_decomp_config_options_s_type   decomp_params;

  /* Decompressor Engine's Decompression Memory data structure */
  DecompMem                           decmp_mem;

  /* Decompressor Engine's Per Packet Metrics */
  ipdc_decomp_metrics_t               per_pkt_metrics;

  /* Decompressor Engine's Statistics */
  ipdc_decomp_stats_t                 decomp_stats;
  
} ipdc_decomp_ctl_blk_s_type;


/*--------------------------------------------------------------------------
  IP Data Compression (IPDC) instance structure
---------------------------------------------------------------------------*/
typedef struct ipdc_instance_s_type
{
  /* Version of IPDC library */
  ipdc_lib_version_e_type ipdc_lib_version;

  /* IPDC Compressor Engine Control Block */
  ipdc_comp_ctl_blk_s_type    *comp_ptr;

  /* IPDC Decompressor Engine Control Block */
  ipdc_decomp_ctl_blk_s_type  *decomp_ptr;
  
} ipdc_instance_s_type;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION IPDCI_GET_ALLOC_INST_CNT

DESCRIPTION
  Returns the number of IPDC instances currently active.  

DEPENDENCIES
  None

RETURN VALUE
  Number of active IPDC instances

SIDE EFFECTS
  None
===========================================================================*/
uint8 ipdci_get_alloc_inst_cnt(void);

/*===========================================================================
FUNCTION IPDCI_IS_BUFFER_ALL_ZEROES

DESCRIPTION
  Check if the passed in buffer is all zeroes.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if all zeroes
  FALSE if any non-zero value present in buffer

SIDE EFFECTS
  None
===========================================================================*/
boolean ipdci_is_buffer_all_zeroes
(
  uint8* start_ptr, 
  uint8* end_ptr
);

#ifdef __cplusplus
}
#endif

#endif /* PS_IPDCI_EX_H */

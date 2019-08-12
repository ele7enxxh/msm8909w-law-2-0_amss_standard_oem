#ifndef PS_IPDC_UTILS_EX_H
#define PS_IPDC_UTILS_EX_H

#ifdef __cplusplus
extern "C" {
#endif

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

           P S _ I P D C _ U T I L S . H

GENERAL DESCRIPTION
  Header file for the IP Data Compression protocol utility functions
  
 
  Copyright (c) 2013 by QUALCOMM Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary.  
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/



/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "ps_ipdc_platform_defs.h"

#include "ps_ipdc_ex.h"
#include "ps_ipdci_ex.h"

/*===========================================================================

                     DEFINITIONS AND CONSTANTS

===========================================================================*/
/*--------------------------------------------------------------------------
  This configuration parameter is defined in ps_ipdc_config.c file
  We are supposed to fetch these via ps_ipdc_config_get() API
  But since this is required for each F3 message logging, accessing it 
  directly by declaring extern here.
---------------------------------------------------------------------------*/
extern boolean ps_ipdc_config_param_per_pkt_logging;

#if defined(IPDC_ON_Q6SIM)||defined(IPDC_ON_OFFTARGET_TOOL)
/*--------------------------------------------------------------------------
  Logs does not need in Q6SIM
---------------------------------------------------------------------------*/
#define MSG_IPDC_ERROR(s,x,y,z)    
#define MSG_IPDC_HIGH(s,x,y,z)    
#define MSG_IPDC_HIGH_8(s,a,b,c,d,e,f,g,h) 
#define MSG_IPDC_MED(s,x,y,z)      
#define MSG_IPDC_S_HIGH(s,x,y,z)   
#define MSG_IPDC_S_HIGH_8(s,a,b,c,d,e,f,g,h)  
#define MSG_IPDC_DEBUG(s,x,y,z)    
#define MSG_IPDC_BUFFER(s,x,y,z)  

#define LOG_MSG_ERROR_0(s)
#define LOG_MSG_ERROR_1(s,x)
#define LOG_MSG_ERROR_2(s,x,y)
#define LOG_MSG_ERROR_3(s,x,y,z)
#define LOG_MSG_INFO1_0(s)
#define LOG_MSG_INFO1_1(s,x)
#define LOG_MSG_INFO1_2(s,x,y)
#define LOG_MSG_INFO1_3(s,x,y,z)
#define LOG_MSG_INFO2_0(s)
#define LOG_MSG_INFO2_1(s,x)
#define LOG_MSG_INFO2_2(s,x,y)
#define LOG_MSG_INFO2_3(s,x,y,z)
#else
/*--------------------------------------------------------------------------
  These are the F3 print macros which are not applicable when code is 
  ported to TBS
---------------------------------------------------------------------------*/
#define MSG_IPDC_ERROR(s,x,y,z)    MSG_3(MSG_SSID_DS_PS, MSG_LEGACY_ERROR, s,x,y,z)

#define MSG_IPDC_HIGH(s,x,y,z)     MSG_3(MSG_SSID_DS_PS, MSG_LEGACY_HIGH, s,x,y,z)
#define MSG_IPDC_HIGH_8(s,a,b,c,d,e,f,g,h)  MSG_8(MSG_SSID_DS_PS, MSG_LEGACY_HIGH, s,a,b,c,d,e,f,g,h)

#define MSG_IPDC_MED(s,x,y,z)      MSG_3(MSG_SSID_DS_PS, MSG_LEGACY_MED, s,x,y,z)

#define MSG_IPDC_S_HIGH(s,x,y,z)   MSG_SPRINTF_3(MSG_SSID_DS_PS, MSG_LEGACY_HIGH, s,x,y,z)
#define MSG_IPDC_S_HIGH_8(s,a,b,c,d,e,f,g,h)  MSG_SPRINTF_8(MSG_SSID_DS_PS, MSG_LEGACY_HIGH, s,a,b,c,d,e,f,g,h)

#define MSG_IPDC_DEBUG(s,x,y,z)    MSG_IPDC_HIGH(s,x,y,z)
#define MSG_IPDC_BUFFER(s,x,y,z)   MSG_IPDC_MED(s,x,y,z)

#endif

#ifdef IPDC_PER_PKT_LOGGING_ENABLED
/*--------------------------------------------------------------------------
  Below F3 prints should be used within IPDC for all per packet logging.
---------------------------------------------------------------------------*/
#define MSG_IPDC_PER_PKT_0(s)                          \
{                                                      \
  if( ps_ipdc_config_param_per_pkt_logging )           \
  {                                                    \
    MSG( MSG_SSID_DS_PS, MSG_LEGACY_MED, s );          \
  }                                                    \
}

#define MSG_IPDC_PER_PKT_1(s,x)                        \
{                                                      \
  if( ps_ipdc_config_param_per_pkt_logging )           \
  {                                                    \
    MSG_1( MSG_SSID_DS_PS, MSG_LEGACY_MED, s,x );      \
  }                                                    \
}

#define MSG_IPDC_PER_PKT_2(s,x,y)                      \
{                                                      \
  if( ps_ipdc_config_param_per_pkt_logging )           \
  {                                                    \
    MSG_2( MSG_SSID_DS_PS, MSG_LEGACY_MED, s,x,y );    \
  }                                                    \
}

#define MSG_IPDC_PER_PKT_3(s,x,y,z)                    \
{                                                      \
  if( ps_ipdc_config_param_per_pkt_logging )           \
  {                                                    \
    MSG_3( MSG_SSID_DS_PS, MSG_LEGACY_MED, s,x,y,z );  \
  }                                                    \
}

#define MSG_IPDC_PER_PKT_4(s,w,x,y,z)                    \
{                                                        \
  if( ps_ipdc_config_param_per_pkt_logging )             \
  {                                                      \
    MSG_4( MSG_SSID_DS_PS, MSG_LEGACY_MED, s,w,x,y,z );  \
  }                                                      \
}

#define MSG_IPDC_PER_PKT_6(s,a,b,c,d,e,f)                    \
{                                                            \
  if( ps_ipdc_config_param_per_pkt_logging )                 \
  {                                                          \
    MSG_6( MSG_SSID_DS_PS, MSG_LEGACY_MED, s,a,b,c,d,e,f );  \
  }                                                          \
}

#define MSG_IPDC_PER_PKT_8(s,a,b,c,d,e,f,g,h)                    \
{                                                                \
  if( ps_ipdc_config_param_per_pkt_logging )                     \
  {                                                              \
    MSG_8( MSG_SSID_DS_PS, MSG_LEGACY_MED, s,a,b,c,d,e,f,g,h );  \
  }                                                              \
}

#define MSG_IPDC_PER_PKT_STR(s,a,b,c)                            \
{                                                                \
  if( ps_ipdc_config_param_per_pkt_logging )                     \
  {                                                              \
    MSG_SPRINTF_3(MSG_SSID_DS_PS, MSG_LEGACY_MED, s,a,b,c);      \
  }                                                              \
}
#else

#define MSG_IPDC_PER_PKT_0(s)
#define MSG_IPDC_PER_PKT_1(s,x)
#define MSG_IPDC_PER_PKT_2(s,x,y)
#define MSG_IPDC_PER_PKT_3(s,x,y,z)
#define MSG_IPDC_PER_PKT_4(s,w,x,y,z)
#define MSG_IPDC_PER_PKT_6(s,a,b,c,d,e,f)
#define MSG_IPDC_PER_PKT_8(s,a,b,c,d,e,f,g,h)
#define MSG_IPDC_PER_PKT_STR(s,a,b,c)

#endif /* ifdef IPDC_PER_PKT_LOGGING_ENABLED */


/*--------------------------------------------------------------------------
  Character array declarations for printing names in F3 prints
---------------------------------------------------------------------------*/
/* ipdc_compression_config_e_type */
#define IPDC_COMP_TYPE_NAME_ARR_LEN IPDC_COMP_CFG_COMPRESS_MAX
#define IPDC_COMP_TYPE_NAME_ENTRY_MAX_LEN 20

extern char ipdc_comp_config_e_type_name_array[IPDC_COMP_TYPE_NAME_ARR_LEN]
       [IPDC_COMP_TYPE_NAME_ENTRY_MAX_LEN];

#define IPDC_COMP_CONFIG_TYPE_NAME(val) \
  ipdc_comp_config_e_type_name_array[val]


/* ipdci_pkt_type */
#define IPDC_DTC_PKT_TYPE_NAME_ARR_LEN IPDCI_PKT_MAX
#define IPDC_DTC_PKT_TYPE_NAME_ENTRY_MAX_LEN 20

extern char ipdc_dtc_pkt_type_name_array[IPDC_DTC_PKT_TYPE_NAME_ARR_LEN]
       [IPDC_DTC_PKT_TYPE_NAME_ENTRY_MAX_LEN];

#define IPDC_DTC_PKT_TYPE_NAME(val) \
  ipdc_dtc_pkt_type_name_array[val]


/* ipdci_ipdc_hdr_pkt_type */
#define IPDC_COMP_PKT_TYPE_NAME_ARR_LEN IPDC_HDR_PKT_TYPE_MAX
#define IPDC_COMP_PKT_TYPE_NAME_ENTRY_MAX_LEN 24

extern char ipdc_comp_pkt_type_name_array[IPDC_COMP_PKT_TYPE_NAME_ARR_LEN]
       [IPDC_COMP_PKT_TYPE_NAME_ENTRY_MAX_LEN];

#define IPDC_COMP_PKT_TYPE_NAME(val) \
  ipdc_comp_pkt_type_name_array[val]


/*===========================================================================

                              DATA TYPES

===========================================================================*/
#if defined(IPDC_ON_OFFTARGET_TOOL) || defined(IPDC_ON_Q6SIM)
// Following Data types are not supported on the UE Target!
  /*
      ****  NOTE *****
    Following data structure is intended for use by the OFF-Target IPDC Tool
    Release SW which is used by External Customers only!
    *****************
  */
  // For OFF-Target IPDC External Customer Logs:
  typedef struct 
  {
    ipdci_ipdc_hdr_pkt_type      pkt_type;
    uint16                       orig_pkt_length;  
    uint16                       orig_header_length;
    uint16                       payload_length_before_compression;
    uint16                       payload_length_after_compression;
    int                          gain;
  } ipdcu_ext_log_pkt_metrics;
  
#endif  // #ifdef FEATURE_IPDC_OFFTARGET -  Data types are not supported on the UE Target!

/*--------------------------------------------------------------------------
  The IPDC cumulative stats
---------------------------------------------------------------------------*/
typedef struct ipdc_cumulative_stats_type
{
  /* Number of active ipdc instance */
  uint8  num_ipdc_instance; 

  uint64 num_uncompressed_bytes;

  uint64 num_proc_pkts;

  /* Stats for full compression packet*/
  struct full_comp
  {
    uint32 num_pkts;
    uint64 num_bytes;
  }full_comp;

  /* Stats for header only compression packet*/
  struct hdr_only_comp
  {
    uint32 num_pkts;
    uint64 num_bytes;
  }hdr_only_comp;

  /* Stats for exact match packet*/
  struct exact_match
  {
    uint32 num_pkts;
    uint64 num_saved_dacc_bytes;
    uint64 num_saved_sh_bytes;
    uint64 num_saved_meta_bytes;
  }exact_match;

  /* Stats for short header */
  struct short_hdr
  {
    uint32 num_static_hdr;
    uint32 num_dynamic_hdr;
    uint64 num_total_match_bytes;
    uint64 num_total_bytes_to_copy;
  }short_hdr;

  /* Stats for dacc header */
  struct dacc
  {
    uint32 num_static_hdr;
    uint32 num_dynamic_hdr;
    uint64 num_total_match_bytes;
  }dacc;

  /* Stats for warnings */
  struct warnings
  {
    /* Number of dyamic short headers with 0 mismatch byte */
    uint32 num_dynamic_short_hdr_mismatch_field_count_0;
    /* Number of dyamic short headers with 0 match byte */
    uint32 num_dynamic_short_hdr_match_field_count_0;
    /* Number of packets with reset type */
    uint32 num_pkts_reset;
    /* Number of packets failed to be compressed */
    uint32 num_comp_failed_pkts;
  }warnings;

  struct per_pkt_stats
  {
    boolean is_dacc_static_present;
    uint16 dacc_match_len;
    uint16 sh_match_len;
  }per_pkt_stats;

}ipdc_cumulative_stats_type;


#ifdef IPDC_ON_Q6SIM
/*--------------------------------------------------------------------------
  pcycle statisitics for Q6sim
---------------------------------------------------------------------------*/
typedef struct
{
  uint64 pcycles;
  uint32 calls;
}ipdc_pcycles_calls_type;

typedef struct
{
  ipdc_pcycles_calls_type compress;
  ipdc_pcycles_calls_type lz77_compress;
  ipdc_pcycles_calls_type build_compressed_packet;
  ipdc_pcycles_calls_type ps_ipdc_hb_add_hkeys;
  ipdc_pcycles_calls_type load_dtc_bytes_into_comp_mem;
  ipdc_pcycles_calls_type shrink;
  ipdc_pcycles_calls_type sh_compress;
}ipdc_q6sim_stats_type;

extern ipdc_q6sim_stats_type ps_ipdc_q6sim_global_stats;
#endif


/*===========================================================================

                               MACROS

===========================================================================*/
/*===========================================================================
MACRO IPDC_DCM_IDX_DEC

DESCRIPTION
  This macro decreases the passed in index number by 1 and modulo with capacity

PARAMETERS
  idx : Current Index
  capacity : modulo value

DEPENDENCIES
  None

RETURN VALUE
  idx.

SIDE EFFECTS
  None
===========================================================================*/
//#define IPDC_DCM_IDX_DEC(idx,capacity)  (((idx)+(capacity-1)) & (capacity-1))


/*===========================================================================
MACRO IPDC_DCM_IDX_INC

DESCRIPTION
  This macro increases the passed in index number by 1 and modulo with capacity

  NOTE: This MACRO only works when the Decomp Memory size is a power of 2!


PARAMETERS
  idx : Current Index
  capacity : modulo value

DEPENDENCIES
  None

RETURN VALUE
  idx.

SIDE EFFECTS
  None
===========================================================================*/
//#define IPDC_DCM_IDX_INC(idx,capacity) (((idx)+1) % (capacity))  // NOTE: Do not use this MACRO!! The MIPS cost is too high!!!
#define IPDC_DCM_IDX_INC(idx,capacity) (((idx)+1) & (capacity-1))

/*===========================================================================
MACRO IPDC_DCM_IDX_ADD

DESCRIPTION
  This macro adds the passed in index number by 'add' and modulo with capacity

  NOTE: This MACRO only works when the DecompMem size is a power of 2!

PARAMETERS
  idx : Current Index
  add : value  to add
  capacity : modulo value

DEPENDENCIES
  None

RETURN VALUE
  idx.

SIDE EFFECTS
  None
===========================================================================*/
//#define IPDC_DCM_IDX_ADD(idx,add,capacity) (((idx)+(add)) % (capacity))//  // NOTE: Do not use this MACRO!! The MIPS cost is too high!!!
#define IPDC_DCM_IDX_ADD(idx,add,capacity) (((idx)+(add)) & (capacity-1))

/*===========================================================================
MACRO IPDC_DCM_IDX_SUB

DESCRIPTION
  This macro decreases the passed in index number by 'sub' and modulo with capacity

PARAMETERS
  idx : Current Index
  sub : Value to subtract
  capacity : modulo value

DEPENDENCIES
  None

RETURN VALUE
  idx.

SIDE EFFECTS
  None
===========================================================================*/
#define IPDC_DCM_IDX_SUB(idx,sub,capacity)  ((((idx)-(sub)+1)+(capacity-1)) & (capacity-1))


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION ps_ipdcu_alloc_compressor_mem()

DESCRIPTION
  Allocates and initializes Extended Compression Memory. 
  If the Memory is to be pre-filled, updates contents of the Memory 
  with the Prefill data. 

DEPENDENCIES
  None

RETURN VALUE
  True if Compressor Engine's Extended Compression Memory was Allocated
  & Initialized
  False otherwise

SIDE EFFECTS
  Will load the Compressor Engine's Extended Compression Memory with
  the Prefill data if prefill_ptr != NULL and prefill_size > 0.
===========================================================================*/
boolean ps_ipdcu_alloc_compressor_mem_ex
(
  ExtCompMem                           * ecm_ptr,
  uint16                                 capacity,
  ipdc_hk_ecm_prealloc_size_e_type       prealloc_size,
  const byte                           * const prefill_ptr,
  uint16                                 prefill_size
);

/*===========================================================================
FUNCTION ps_ipdcu_flush_compressor_mem()

DESCRIPTION
  Function to flush the Compressor Engine's Extended Compression Memory.
 
DEPENDENCIES
  None

RETURN VALUE
  return FALSE is failure to flush else return TRUE

SIDE EFFECTS
  Flushes Compressor Engine's Extended Compression Memory and sets 
  contents to all zeroes
===========================================================================*/
boolean ps_ipdcu_flush_compressor_mem_ex
(
  ExtCompMem              * ecm_ptr,
  const byte              * const mem_prefill_ptr,
  uint16                    mem_prefill_size
);

/*===========================================================================
FUNCTION ps_ipdcu_free_compressor_mem()

DESCRIPTION
  Frees the Compressor Engine's allocated Extended Compression Memory.

DEPENDENCIES
  None

RETURN VALUE
  True if  Compressor Engine's allocated Extended Compression Memory was Freed
  False otherwise

SIDE EFFECTS
  None
===========================================================================*/
boolean ps_ipdcu_free_compressor_mem_ex
(
  ExtCompMem *ecm_ptr
);

/*===========================================================================
FUNCTION ps_ipdcu_alloc_decompressor_mem()

DESCRIPTION
  Allocates and initializes Memory for use by the Decompressor Engine. 
  If the Memory is to be pre-filled, updates contents of the Memory 
  with the Prefill data. 

DEPENDENCIES
  None

RETURN VALUE
  True if Decompressor Engine's Memory was Allocated & Initialized
  False otherwise

SIDE EFFECTS
  Will load the Decompressor Engine's Memory with the Prefill data if 
  prefill_ptr != NULL and prefill_size > 0.
===========================================================================*/
boolean ps_ipdcu_alloc_decompressor_mem_ex
(
  DecompMem   *dcm_ptr,
  int          alloc_memsize,
  const byte * const prefill_ptr,
  uint16       prefill_size
);

/*===========================================================================
FUNCTION ps_ipdcu_flush_decompressor_mem()

DESCRIPTION
  Function to flush the Decompressor Engine's Memory.
 
DEPENDENCIES
  None

RETURN VALUE
  return FALSE is failure to flush else return TRUE

SIDE EFFECTS
  Flushes Decompressor Engine's Memory and sets contents to all zeroes
===========================================================================*/
boolean ps_ipdcu_flush_decompressor_mem_ex
(
  DecompMem               * dcm_ptr,
  const byte              * const mem_prefill_ptr,
  uint16                    mem_prefill_size
);

/*===========================================================================
FUNCTION ps_ipdcu_free_decompressor_mem()

DESCRIPTION
  Frees the Decompressor Engine's allocated Memory.

DEPENDENCIES
  None

RETURN VALUE
  True if Decompressor Engine's Memory was Freed
  False otherwise

SIDE EFFECTS
  None
===========================================================================*/
boolean ps_ipdcu_free_decompressor_mem_ex
(
  DecompMem *dcm_ptr
);

/*===========================================================================
FUNCTION ps_ipdcu_get_compressor_packet_info

DESCRIPTION
  Gets information for the incoming packet

PARAMETERS
  dsm_item_type - dsm item containing the pdcp sdu

RETURN VALUE
  Fills out the output parameter 'ip_header'
===========================================================================*/
void ps_ipdcu_get_compressor_packet_info_ex
(
  dsm_item_type        *p_pdcp_sdu,
  ipdc_pkt_info_type   *ip_header
);

/*===========================================================================
FUNCTION ps_ipdcu_get_actual_compression_mem_size

DESCRIPTION
  This function converts the enumerated compression memory size configuration
  parameter into the actual value in bytes.

DEPENDENCIES
  None

RETURN VALUE
   uint16

SIDE EFFECTS
  None 
===========================================================================*/
uint16 ps_ipdcu_get_actual_compression_mem_size_ex(ipdc_compression_mem_size_e_type cmp_mem_size);

/*===========================================================================
FUNCTION ps_ipdcu_get_actual_hash_bucket_size

DESCRIPTION
  This function converts the enumerated hash bucket configuration
  parameter into the actual value in bytes.

DEPENDENCIES
  None

RETURN VALUE
   uint16

SIDE EFFECTS
  None 
===========================================================================*/
uint16 ps_ipdcu_get_actual_hash_bucket_size_ex(ipdc_hb_size_e_type hb_size);

/*===========================================================================
FUNCTION ps_ipdcu_calculate_ipdc_hdr_checksum

DESCRIPTION
  Calculates the 4 bit IPDC Header checksum for the current packet.

  This function computes the checksum of the given  
  data buffer across PS_IPDC_MIN_MATCH_IN_BYTES bytes

PARAMETERS

RETURN VALUE
  uint8 checksum value (lower 4 bits contain checksum)
===========================================================================*/
uint8 ps_ipdcu_calculate_ipdc_hdr_checksum_ex
(
  uint8                    *buffer,
  ipdc_lib_version_e_type   lib_version
);

/*===========================================================================
FUNCTION ps_ipdcu_validate_ipdc_hdr_checksum

DESCRIPTION
  Validates the current packet's IPDC Header checksum.

PARAMETERS

RETURN VALUE
  if checksum validation passes, return TRUE
  otherwise return FALSE
===========================================================================*/
boolean ps_ipdcu_validate_ipdc_hdr_checksum_ex
(
  uint8                         pkt_hdr_checksum,
  uint8                        *buffer,
  ipdc_lib_version_e_type       lib_version
);

/*===========================================================================
FUNCTION PS_IPDCU_COMPUTE_BUFFER_CKSUM

DESCRIPTION
  Compute 2 byte checksum for the passed in buffer.
  Checksum involves the following steps:
  - Sum up all 2 byte words in the buffer (network byte order) from the 
    beginning
  - Discard the overflow beyond 2 bytes
  - Finally compute 2's complement of the sum

  In case the buffer length is odd, last remaining byte is padded with a 
  zero byte and then used for the summation.
  e.g. 
  If 0x76 is left at buffer end, 0x7600 is added to the cumulative sum.

DEPENDENCIES
  None

ARGUMENTS
  Memory buffer which is to be checksummed
  Length of the memory buffer

RETURN VALUE
  2 byte checksum value
  
SIDE EFFECTS
  None
===========================================================================*/
uint16 ps_ipdcu_compute_buffer_cksum
(
  uint8*  buf_ptr,
  uint32  buf_len
);

/*===========================================================================
FUNCTION ip_to_str

DESCRIPTION
 Converts an IP address (v4 or v6) into a decimal String.

DEPENDENCIES
  None

PARAMETERS

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
char* ip_to_str_ex
(
  ipdc_ipv6_ipv4_addr_type ip, 
  uint8 ip_version, 
  char *buf, 
  uint8 buf_len
);

/*===========================================================================
FUNCTION ps_ipdcu_log_comp_eng_stats

DESCRIPTION
  Logs IPDC Compressor Engine Stats.

DEPENDENCIES
  The IPDC memheap must have been initialized and ipdc_alloc() called prior to
  calling ps_ipdc_logging_log_comp_eng_stats()

PARAMETERS
  ipdc_comp_stats_ptr - compressor statistics stored in instance

RETURN VALUE
   none

SIDE EFFECTS
  
===========================================================================*/
void ps_ipdcu_log_comp_eng_stats_ex
(
  ipdc_comp_stats_t*  ipdc_comp_stats_in_ptr,
  boolean             hdr_only_compression_configured
);

/*===========================================================================
FUNCTION ps_ipdcu_get_sh_saved_bytes_ex

DESCRIPTION
  Return the short header saved bytes

DEPENDENCIES
  None

PARAMETERS
  None
  
RETURN VALUE
  none

SIDE EFFECTS
  
===========================================================================*/
IPDC_INLINE uint64 ps_ipdcu_get_sh_saved_bytes_ex(void);

/* Functions needed only on MOB and TBS */
#if defined (IPDC_ON_MOB) || defined(IPDC_ON_OFFTARGET_TOOL)||defined(IPDC_ON_Q6SIM)
/*===========================================================================
FUNCTION ps_ipdcu_get_comp_gain_ex

DESCRIPTION
  Return the compression ratio.

DEPENDENCIES
  None

PARAMETERS
  handle - the IPDC handle

RETURN VALUE
   none

SIDE EFFECTS
   none
===========================================================================*/
float ps_ipdcu_get_comp_gain_ex(ipdc_handle_type handle);

/*===========================================================================
FUNCTION ps_ipdcu_get_uncompressed_bytes_ex

DESCRIPTION
  return the uncompressed bytes

DEPENDENCIES
  None

PARAMETERS
  handle - ipdc instance handle

RETURN VALUE
   none

SIDE EFFECTS
   none
===========================================================================*/
uint64 ps_ipdcu_get_uncompressed_bytes_ex(ipdc_handle_type handle);

/*===========================================================================
FUNCTION ps_ipdcu_get_compressed_bytes_ex

DESCRIPTION
  return the compressed bytes

DEPENDENCIES
  None

PARAMETERS
  handle - ipdc instance handle

RETURN VALUE
   none

SIDE EFFECTS
   none
===========================================================================*/
uint64 ps_ipdcu_get_compressed_bytes_ex(ipdc_handle_type handle);


/*===========================================================================
FUNCTION PS_IPDCU_GET_IPV4_PKT_SRC_IP_ADDR_EX

DESCRIPTION
  Returns the Source IP Address from the IPv4 Packet.

DEPENDENCIES
  None

PARAMETERS
  dsm_item_type *pkt_ptr - pointer to dsm item containing the IPv4 packet
  uint32 *src_ip_addr_ptr  - pointer to memory in which to store the Src IP Address

RETURN VALUE
  returns TRUE if 'src_ip_addr_ptr' contains a valid Src IP Address
  returns FALSE if 'src_ip_addr_ptr' is invalid

SIDE EFFECTS
  None
===========================================================================*/
boolean ps_ipdcu_get_ipv4_pkt_src_ip_addr_ex
(
  dsm_item_type                 *pkt_ptr,           /* Pointer to IPv4 Packet */
  uint32                        *src_ip_addr_ptr
);

/*===========================================================================
FUNCTION PS_IPDCU_GET_IPV4_PKT_DST_IP_ADDR_EX

DESCRIPTION
  Returns the Destination IP Address from the IPv4 Packet.

DEPENDENCIES
  None

PARAMETERS
  dsm_item_type *pkt_ptr - pointer to dsm item containing the IPv4 packet
  uint32 *dst_ip_addr_ptr  - pointer to memory in which to store the Dst IP Address

RETURN VALUE
  returns TRUE if 'dst_ip_addr_ptr' contains a valid Src IP Address
  returns FALSE if 'dst_ip_addr_ptr' is invalid

SIDE EFFECTS
  None
===========================================================================*/
boolean ps_ipdcu_get_ipv4_pkt_dst_ip_addr_ex
(
  dsm_item_type                 *pkt_ptr,           /* Pointer to IPv4 Packet */
  uint32                        *dst_ip_addr_ptr
);

/*===========================================================================
FUNCTION PS_IPDCU_GET_IPV6_PKT_SRC_IP_ADDR_EX

DESCRIPTION
  Returns the Source IP Address from the IPv6 Packet.

DEPENDENCIES
  None

PARAMETERS
  dsm_item_type *pkt_ptr - pointer to dsm item containing the IPv6 packet
  ipdc_ipv6_ipv4_addr_type *src_ip_addr_ptr  - pointer to memory in which to store the Src IP Address

RETURN VALUE
  returns TRUE if 'src_ip_addr_ptr' contains a valid Src IP Address
  returns FALSE if 'src_ip_addr_ptr' is invalid

SIDE EFFECTS
  None
===========================================================================*/
boolean ps_ipdcu_get_ipv6_pkt_src_ip_addr_ex
(
  dsm_item_type                 *pkt_ptr,           /* Pointer to IPv6 Packet */
  ipdc_ipv6_ipv4_addr_type      *src_ip_addr_ptr
);

/*===========================================================================
FUNCTION PS_IPDCU_GET_IPV6_PKT_DST_IP_ADDR_EX

DESCRIPTION
  Returns the Destination IP Address from the IPv6 Packet.

DEPENDENCIES
  None

PARAMETERS
  dsm_item_type *pkt_ptr - pointer to dsm item containing the IPv6 packet
  uint32 *dst_ip_addr_ptr  - pointer to memory in which to store the Dst IP Address

RETURN VALUE
  returns TRUE if 'dst_ip_addr_ptr' contains a valid Src IP Address
  returns FALSE if 'dst_ip_addr_ptr' is invalid

SIDE EFFECTS
  None
===========================================================================*/
boolean ps_ipdcu_get_ipv6_pkt_dst_ip_addr_ex
(
  dsm_item_type                 *pkt_ptr,           /* Pointer to IPv6 Packet */
  ipdc_ipv6_ipv4_addr_type      *dst_ip_addr_ptr
); 

/*===========================================================================
FUNCTION PS_IPDCU_GET_PKT_COMPRESSION_METRICS_EX

DESCRIPTION

  ****  NOTE *****
  Function is intended for use by the OFF-Target IPDC Release SW which is
  used by External Customers only!
  *****************

  Gets packet metrics from the IPDC Compressor Engine for the last packet.  
  This information will then be logged to the external customer log file.

DEPENDENCIES
  The IPDC memheap must have been initialized and ipdc_alloc() called prior to
  calling ps_ipdc_log_ext_cust_pkt_compression_metrics()

PARAMETERS
  ipdc_handle_type *handle - pointer to the ipdc handle

RETURN VALUE
   none

SIDE EFFECTS
   Contents of metrics_ptr's data structure is updated 
===========================================================================*/
void ps_ipdcu_get_pkt_compression_metrics_ex
(
  ipdc_handle_type               handle,           /* IPDC instance         */
  ipdcu_ext_log_pkt_metrics     *metrics_ptr
);

/*===========================================================================
FUNCTION PS_IPDCU_GET_COMP_STATS_EX

DESCRIPTION
  Returns the IPDC Compressor Engine's statistic.

DEPENDENCIES
  The IPDC memheap must have been initialized and ipdc_alloc() called prior to
  calling ps_ipdcu_get_comp_eng_hash_miss_cnt_stat()

PARAMETERS
  ipdc_handle_type *handle - pointer to the ipdc handle

RETURN VALUE
  Pionter points to the statistics

SIDE EFFECTS
  
===========================================================================*/
ipdc_comp_stats_t* ps_ipdcu_get_comp_stats_ex
(
  ipdc_handle_type               handle           /* IPDC instance         */
);

/*===========================================================================
FUNCTION ps_ipdcu_log_decomp_eng_stats_ex

DESCRIPTION
  Logs IPDC DeCompressor Engine Stats.

DEPENDENCIES
  The IPDC memheap must have been initialized and ipdc_alloc() called prior to
  calling ps_ipdc_logging_log_decomp_eng_stats()

PARAMETERS
  ipdc_handle_type *handle - pointer to the ipdc handle

RETURN VALUE
   none

SIDE EFFECTS
  
===========================================================================*/
void ps_ipdcu_log_decomp_eng_stats_ex
(
  ipdc_handle_type               handle
);

/*===========================================================================
FUNCTION PS_IPDC_GET_COMP_ENG_HASH_COLLISION_CNT_EX

DESCRIPTION
  Returns the IPDC Compressor Engine's hash miss count statistic.

DEPENDENCIES
  The IPDC memheap must have been initialized and ipdc_alloc() called prior to
  calling ps_ipdcu_get_comp_eng_hash_miss_cnt_stat()

PARAMETERS
  ipdc_handle_type *handle - pointer to the ipdc handle

RETURN VALUE
   Number of Hash collisions

SIDE EFFECTS
  
===========================================================================*/
unsigned long long ps_ipdcu_get_comp_eng_hash_collision_cnt_ex
(
  ipdc_handle_type               handle           /* IPDC instance         */
);

/*===========================================================================
FUNCTION PS_IPDC_GET_COMP_ENG_SRCH_CNT_STAT_EX

DESCRIPTION
  Returns the IPDC Compressor Engine's overall search count statistic.

DEPENDENCIES
  The IPDC memheap must have been initialized and ipdc_alloc() called prior to
  calling ps_ipdc_logging_log_comp_stats()

PARAMETERS
  ipdc_handle_type *handle - pointer to the ipdc handle

RETURN VALUE
   current overall search count statistic

SIDE EFFECTS
  
===========================================================================*/
unsigned long long ps_ipdcu_get_comp_eng_srch_cnt_stat_ex
(
  ipdc_handle_type               handle           /* IPDC instance         */
);

/*===========================================================================
FUNCTION ps_ipdcu_log_comp_eng_hash_key_distribution_stat_ex

DESCRIPTION
  Logs the IPDC Compressor Engine's Hash Key Distribution statistic.

DEPENDENCIES
  The IPDC memheap must have been initialized and ipdc_alloc() called prior to
  calling ps_ipdc_logging_log_comp_stats()

PARAMETERS
  ipdc_handle_type *handle - pointer to the ipdc handle

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_ipdcu_log_comp_eng_hash_key_distribution_stat_ex
(
  ipdc_handle_type handle 
);

/*===========================================================================
FUNCTION ps_ipdcu_get_pkt_compression_gain_ex

DESCRIPTION
  Computes the compression gain for the packet.

DEPENDENCIES
  The IPDC memheap must have been initialized and ipdc_alloc() called prior to
  calling get_pkt_compression_gain()

PARAMETERS
  ipdc_handle_type *handle - pointer to the ipdc handle

RETURN VALUE
   none

SIDE EFFECTS
  
===========================================================================*/
int  ps_ipdcu_get_pkt_compression_gain_ex
(
  ipdc_handle_type               handle           /* IPDC instance         */
);

/*===========================================================================
FUNCTION ps_ipdcu_print_cumulative_stats_ex

DESCRIPTION
  Print the cumulative stats

DEPENDENCIES
  None
  
PARAMETERS
  None

RETURN VALUE
   none

SIDE EFFECTS
  
===========================================================================*/
void ps_ipdcu_print_cumulative_stats_ex();

#endif

/* Functions needed only on MOB */
#if defined (IPDC_ON_MOB)
/*===========================================================================
FUNCTION ps_ipdc_hb_dump_hkeys_for_hval_ex()

DESCRIPTION
  This utility function logs the contents of the hash key chain for the
  specified Hash Value.

DEPENDENCIES
  IPDC Instance must be valid and Compressor Engine's Hash Bucket must have
  been initialized.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_ipdc_hb_dump_hkeys_for_hval_ex
(
  HashBuckets *hb, 
  ExtCompMem *ecm, 
  uint16 hash_val
);

/*===========================================================================
FUNCTION ps_ipdcu_get_compressor_mem_size_ex()

DESCRIPTION
  Function to get the Compressor Engine's current Memory size.

DEPENDENCIES
  None

RETURN VALUE
  Current ExtCompMem size

SIDE EFFECTS
  None

===========================================================================*/
static uint16 ps_ipdcu_get_compressor_mem_size_ex(ExtCompMem *ecm);

/*===========================================================================
FUNCTION ps_ipdcu_get_compressor_mem_5_char_blk_ex()

DESCRIPTION 
 
  This utility function returns the 5 characters in ExtCompMem starting from
  the provided Compressor Memory index and packs them into a uint64 value.

DEPENDENCIES
  The IPDC Compressor Engine must have been allocated!

RETURN VALUE
  uint64 value - contains the 5 characters in ExtCompMem at the requested
                 index into ExtCompMem

SIDE EFFECTS 
  None 
  
===========================================================================*/
uint64 ps_ipdcu_get_compressor_mem_5_char_blk_ex
(
  ExtCompMem             *ecm, 
  uint16                  ecm_idx
);

/*===========================================================================
FUNCTION ps_ipdcu_dump_dsm_ex()

DESCRIPTION
  Helper function to log the contents of the dsm packet 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_ipdcu_dump_dsm_ex
(
  dsm_item_type **dsm,
  int offset,
  int len
);
//#endif

//#if defined (IPDC_ON_MOB) || defined(IPDC_ON_Q6SIM)|| defined(IPDC_ON_OFFTARGET_TOOL)
/*===========================================================================
FUNCTION ps_ipdcu_log_comp_eng_metrics_ex

DESCRIPTION
  Logs IPDC Compressor Engine's Per PDCP SDU Metrics.

DEPENDENCIES
  The IPDC memheap must have been initialized and ipdc_alloc() called prior to
  calling ps_ipdc_logging_log_comp_eng_metrics()

PARAMETERS
  ipdc_handle_type *handle - pointer to the ipdc handle

RETURN VALUE
   none

SIDE EFFECTS
  
===========================================================================*/
void ps_ipdcu_log_comp_eng_metrics_ex
(
  ipdc_handle_type               handle           /* IPDC instance         */
);

/*===========================================================================
FUNCTION ps_ipdcu_dump_decompressor_mem_ex()

DESCRIPTION
  Function to dump the current contents of Decompressor Engine's Memory

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_ipdcu_dump_decompressor_mem_ex
(
  DecompMem * dcm_ptr
);


/*===========================================================================
FUNCTION ps_ipdcu_dump_compressor_mem_ex()

DESCRIPTION
  Function to dump the current contents of Compressor Engine's Memory 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_ipdcu_dump_compressor_mem_ex
(
  ExtCompMem * ecm_ptr
);
#endif

#ifdef IPDC_ON_Q6SIM
/*===========================================================================
FUNCTION ps_ipdcu_q6sim_print_stats_ex

DESCRIPTION
  Print the Q6 state

DEPENDENCIES

PARAMETERS
   none

RETURN VALUE
   none

SIDE EFFECTS
  
===========================================================================*/
void ps_ipdcu_q6sim_print_stats_ex(void);
#endif

/* Functions needed only on TBS */
#if defined (IPDC_ON_TBS)

/*===========================================================================
FUNCTION ipdc_memscpy_ex()

DESCRIPTION

  Size bounded memory copy. 
   
  Copies bytes from the source buffer to the destination buffer.
   
  This function ensures that there will not be a copy beyond 
  the size of the destination buffer. 

  The result of calling this on overlapping source and destination
  buffers is undefined.
   
  @param[out] dst       Destination buffer.
  @param[in]  dst_size  Size of the destination buffer in bytes.
  @param[in]  src       Source buffer.
  @param[in]  src_size  Number of bytes to copy from source buffer.
  
RETURN VALUE
  The number of bytes that were copied into the destination buffer. 

===========================================================================*/
size_t  ipdc_memscpy_ex
(
  void        *dst,
  size_t      dst_size,
  const void  *src,
  size_t      src_size
);

/*===========================================================================
FUNCTION IPDC_LOGGING_LOG_DECOMP_ENGINE_MEM_EX

DESCRIPTION
  Logs an IPDC Decompressor Engine's Compression Memory.

DEPENDENCIES

PARAMETERS
  ipdc_handle_type *handle - pointer to the ipdc handle

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ipdc_logging_log_decomp_engine_memory_ex
(
  ipdc_handle_type               handle
);

#endif /* #if defined (IPDC_ON_TBS) */


/* Functions needed only on MOB and TBS */
#if defined (IPDC_ON_MOB) || defined(IPDC_ON_TBS)

/*===========================================================================
FUNCTION ps_ipdcu_log_decomp_eng_metrics_ex

DESCRIPTION
  Logs IPDC DeCompressor Engine's Per PDCP SDU Metrics.

DEPENDENCIES
  The IPDC memheap must have been initialized and ipdc_alloc() called prior to
  calling ps_ipdc_logging_log_decomp_eng_metrics()

PARAMETERS
  ipdc_handle_type *handle - pointer to the ipdc handle

RETURN VALUE
   none

SIDE EFFECTS
  
===========================================================================*/
void ps_ipdcu_log_decomp_eng_metrics_ex
(
  ipdc_handle_type               handle
);

#endif

/*===========================================================================
FUNCTION ps_ipdcu_get_cumulative_stats

DESCRIPTION
  Get the statistics for QSH

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  The pointer points to stat struture

SIDE EFFECTS
  None
===========================================================================*/
ipdc_cumulative_stats_type *ps_ipdcu_get_cumulative_stats(void);

/*===========================================================================
FUNCTION ps_ipdcu_reset_cumulative_stats

DESCRIPTION
  Reset the statistics for QSH

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_ipdcu_reset_cumulative_stats(void);

#ifdef __cplusplus
}
#endif

#endif  // #ifndef PS_IPDC_UTILS_EX_H

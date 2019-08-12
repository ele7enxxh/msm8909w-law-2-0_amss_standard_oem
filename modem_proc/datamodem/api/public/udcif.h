#ifndef UDCIF_H
#define UDCIF_H
/*===========================================================================
                           U D C I F . H

DESCRIPTION
  Header file containing the API for the Uplink Data Compression Module.
 

Copyright (c) 2015 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE


when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/15/15    cx     Updated new checksum for new prefill
03/22/15    ss     Initial version.
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

#include "dsm.h"
#include "sys.h"

/*===========================================================================

                              CONSTANTS

===========================================================================*/
/*--------------------------------------------------------------------------
  Prefill buffer checksum
  List of prefill buffers would be maintained in UDC and the user can specify
  which one to use while creating compressor instance.
---------------------------------------------------------------------------*/
#define UDC_PREFILL_BUFFER_ALL_ZEROES_CKSUM  0x0000
  
/* This is the sum of 2 byte words of the prefill buffer in network byte
   order.
   i.e. 
   - pick up first 2 bytes as it is (network byte order) 
   - pick up next 2 bytes as it is ..
   - Keep adding and discard the overflow beyond 2 bytes
  
   Also, the buffer is padded at the end with 0x00 in case number of bytes in
   prefill buffer is odd to form the last 2 byte word.

   Then compute the 2's complement of this 2 byte sum value.

   The below checksum value corresponds to ipdc_udc_prefill_1 declared in
   ps_ipdc.h */
//#define UDC_PREFILL_BUFFER_1_CKSUM  IPDC_PREFILL_CKSUM_UDC_PREFILL_1
#define UDC_PREFILL_BUFFER_1_CKSUM  0xd0c8

/*--------------------------------------------------------------------------
  ULCM checksum with standard prefill buffers
  These are the ULCM checksum when populated initially with prefill buffer
  Each value below is corresponding to a prefill buffer cksum value above
---------------------------------------------------------------------------*/
#define UDC_ULCM_CKSUM_WITH_PREFILL_BUFFER_ALL_ZEROES  0x0000

/* This is the checksum of the ULCM with prefill buffer at the end.
   Different from Prefill Buffer Checksum itself, since the prefill buffer 
   length is currently odd */
//#define UDC_ULCM_CKSUM_WITH_PREFILL_BUFFER_1  IPDC_ULCM_CKSUM_UDC_PREFILL_1
#define UDC_ULCM_CKSUM_WITH_PREFILL_BUFFER_1  0x485e


/*===========================================================================

                              DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  UDC Compressor Handle
---------------------------------------------------------------------------*/
typedef void* udc_comp_inst_handle_type;

/*---------------------------------------------------------------------------
  UDC Library version
  Meant to ensure same library implementation is being used on UE and 
  Network.
---------------------------------------------------------------------------*/
typedef enum
{
  /* UDC Version 0 corresponds to the legacy IPDC library implementation */
  UDC_LIB_VERSION_0       = 0x00,

  /* UDC Version 1 corresponds to the IPDC library version with 3 bit packet
     type, 4 bit checksum and 1 extension bit header format. */
  UDC_LIB_VERSION_1       = 0x01,

  /* This version is not yet supported, requirements under discussion. */
  UDC_LIB_VERSION_2       = 0x02,
  
  UDC_LIB_VERSION_MAX
  
} udc_lib_version_e_type;

/*---------------------------------------------------------------------------
  UDC Compression Mode enumeration
  Defines how the input packet should be compressed
---------------------------------------------------------------------------*/
typedef enum
{
  UDC_COMP_MODE_INVALID = 0,

  /* Full packet compression is attempted. */
  UDC_COMP_MODE_FULL_PKT,

  /* Only header compression is attempted.
     Header length is configured while creating compression instance. */
  UDC_COMP_MODE_HEADER_ONLY,
  
  UDC_COMP_MODE_MAX
  
} udc_comp_mode_e_type;

/*---------------------------------------------------------------------------
  UDC Compression Memory Buffer Size
  How large memory buffer to allocate for the compressor.
---------------------------------------------------------------------------*/
typedef enum
{
  UDC_CONTEXT_BUF_SIZE_INVALID = 0,

  UDC_CONTEXT_BUF_SIZE_2048_BYTES,
  UDC_CONTEXT_BUF_SIZE_4096_BYTES,
  UDC_CONTEXT_BUF_SIZE_8192_BYTES,
  UDC_CONTEXT_BUF_SIZE_16384_BYTES,

  UDC_CONTEXT_BUF_SIZE_MAX
  
} udc_context_buf_size_e_type;

/*---------------------------------------------------------------------------
  Packet Compression Action
  Action required on packet DSM item passed to compressor.
  Through this config item, UDC user will indicate what to do for individual
  packets passed in to compress API.
---------------------------------------------------------------------------*/
typedef enum
{
  UDC_PKT_COMP_ACTION_INVALID = 0,

  /* Compression Header followed by Compressed Data are written to Target DSM. 
     Source DSM untouched and continue to have uncompressed data */
  UDC_PKT_COMP_ACTION_COMPRESS,

  /* No Compression is performed – IPDC Header will indicate “No Compression”.     
     Source DSM untouched.
     Target DSM created as per following config parameter in this case:
     copy_src_to_target_for_no_comp */
  UDC_PKT_COMP_ACTION_DO_NOT_COMPRESS,

  /* No Compression is performed and compressor memory buffer is Reset to 
     all zeroes
     IPDC Header will indicate “RESET”
     Target DSM created as per following config parameter in this case:
     copy_src_to_target_for_no_comp */
  UDC_PKT_COMP_ACTION_RESET_CONTEXT_BUF,

  UDC_PKT_COMP_ACTION_MAX

} udc_pkt_comp_action_e_type;

/*---------------------------------------------------------------------------
  UDC Error Values
  Used across all UDC APIs
---------------------------------------------------------------------------*/
typedef enum
{
  /* To be used before invoking APIs */
  UDC_RETVAL_INVALID = -1,

  UDC_RETVAL_SUCCESS = 0,

  /* Invalid arguments error */
  UDC_RETVAL_ERROR_INVALID_ARGS,

  /* Memory exhaustion errors */
  UDC_RETVAL_ERROR_OUT_OF_DSM_MEM,
  UDC_RETVAL_ERROR_OUT_OF_HEAP_MEM,

  /* IPDC Internal Error */
  UDC_RETVAL_ERROR_IPDC_LIB_ERROR,

  /* UDC Internal Error */
  UDC_RETVAL_ERROR_UDC_INTERNAL_ERROR,
 
  /* Invalid configuratin specified */
  UDC_RETVAL_ERROR_INVALID_CONFIG,

  /* Invalid prefill buffer cksum specified */
  UDC_RETVAL_ERROR_INVALID_PREFILL_BUF_CKSUM,
  
  UDC_RETVAL_MAX
  
} udc_retval_e_type;

/*---------------------------------------------------------------------------
  UDC Compressor Static Config Parameters
  These config items can only be specified while creating the compressor
  instance.
  These cannot be updated during the compressor operation.
---------------------------------------------------------------------------*/
typedef struct
{
  udc_lib_version_e_type         lib_version;     /* UDC Library Version */
  udc_context_buf_size_e_type    context_buf_size; /* Memory Buffer Size */

  /* Packet Header Length: Approximate length of headers in the packets 
                           being compressed.
     should lie in the following range:
     UDC_PACKET_HEADER_LENGTH_MIN
     UDC_PACKET_HEADER_LENGTH_MAX */
  uint8                          hdr_length;

  /* Compression Gain Threshold
     Compressor would resort to header only compression when compression
     gain goes below this threshold. */
  uint8                          comp_gain_thld;  /* 0 to 99 range */

  /* Prefill Buffer
     Used to initialize the compressor memory buffer.
     List of prefill buffers is maintained internally by UDC, 
     specify which one to use.
     Buffer is identified by the checksum of all 4 byte words in the buffer.
     i.e. just sum of all the 4 byte words discarding the overflow.
     No 1's or 2's complement is done, as that's not necessary here.
     In case no prefill buffer required, specify:
     UDC_PREFILL_BUFFER_ALL_ZEROES */
  uint16                         prefill_buffer_cksum;
  
} udc_comp_static_cfg_params_s_type;

/*---------------------------------------------------------------------------
  UDC Compressor Dynamic Config Parameters
  These can be udpated dynamically for an operating UDC instance.
---------------------------------------------------------------------------*/
typedef struct
{
  sys_modem_as_id_e_type      sub_id;          /* Subscription ID     */
  uint8                       bearer_id;       /* Bearer ID           */

  /* Packet Compression Action
     Instructions for per packet compression */
  udc_pkt_comp_action_e_type  pkt_comp_action;

  /* Compression Mode
     UDC compressor mode of operation */
  udc_comp_mode_e_type        comp_mode;

  /* Dynamically control how many bytes to compress in a packet.
     Value = 0 indicates full packet compression */
  uint16                      partial_pkt_len_to_compress;

  /* Should we copy contents from source DSM to target DSM when no 
     compression is performed.
     When below parameter is TRUE: 
     - Target DSM contains IPDC header followed by original packet content.
     When below parameter is FALSE: 
     - Original DSM is DUP'd
     - New DSM is allocated to hold IPDC header info
     - DUP of original DSM chained to new DSM allocated for header info. */
  boolean                     copy_src_to_target_dsm_for_no_comp;
  
} udc_comp_dynamic_cfg_params_s_type;

/*---------------------------------------------------------------------------
  UDC Compressor Config Parameters
  These parameters are passed in by user while creating a UDC instance
  Identify how UDC would be setup for compression.
---------------------------------------------------------------------------*/
typedef struct
{
  /* Static Config Items */
  udc_comp_static_cfg_params_s_type  static_cfg;

  /* Dynamic Config Items */
  udc_comp_dynamic_cfg_params_s_type dynamic_cfg;
  
} udc_comp_config_options_s_type;

/*---------------------------------------------------------------------------
  UDC Compress Packet Info
  These parameters are passed in by user compressing a packet.

  -- NOTE --
  This structure can contain ONLY POINTERS since its passed by value to the 
  udc_compress() API
---------------------------------------------------------------------------*/
typedef struct
{
  udc_comp_inst_handle_type  comp_instance_handle;

  /* Packet to be compressed */
  dsm_item_type**            src_dsm_item;

  /* Stores the compressed packet */
  dsm_item_type**            target_dsm_item;

  /* Length of the compressed packet
     This is same as the length of target DSM item
     UDC is returning so that client doesn't need to perform DSM length
     call : MIPs Optimization
     This is added as a pointer since this entire structure is passed in by
     value by clients (PDCP) */
  uint16*                    target_dsm_len_ptr;
  
} udc_comp_req_info_s_type;


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION UDC_GET_SUPPORTED_LIBRARY_VERSION_MASK

DESCRIPTION
  Returns mask of all library versions supported by current code base.
  Mask contains values from udc_lib_version_e_type

DEPENDENCIES
  None

ARGUMENTS
  None

RETURN VALUE
  Mask Value returned

SIDE EFFECTS
  None
===========================================================================*/
uint32 udc_get_supported_library_version_mask
(
  void
);

/*===========================================================================
FUNCTION UDC_IS_LIBRARY_VERSION_SUPPORTED

DESCRIPTION
  Checks if specified IPDC library version is supported

DEPENDENCIES
  None

ARGUMENTS
  UDC Library version to check.

RETURN VALUE
  TRUE if version supported, FALSE if not supported.

SIDE EFFECTS
  None
===========================================================================*/
boolean udc_is_library_version_supported
(
  udc_lib_version_e_type udc_lib_version
);

/*===========================================================================
FUNCTION UDC_INIT_COMP_CONFIG_OPTIONS

DESCRIPTION
  Initializes the compressor config options with default values.
 
DEPENDENCIES
  None

ARGUMENTS
  [out]  comp_config_options_ptr : Compressor configuration to set.

RETURN VALUE
  udc_retval_e_type

SIDE EFFECTS
  None
===========================================================================*/
udc_retval_e_type udc_init_comp_config_options
(
  udc_comp_config_options_s_type*  comp_config_options_ptr
);

/*===========================================================================
FUNCTION UDC_FETCH_CURRENT_COMP_CONFIG

DESCRIPTION
  Fetch the current compressor configuration.
 
DEPENDENCIES
  None

ARGUMENTS
  [in]  comp_instance_handle : Compressor Instance Handle
  [out] comp_config_options_ptr : Current config options are copied to the 
                                  passed in memory.

RETURN VALUE
  udc_retval_e_type

SIDE EFFECTS
  None
===========================================================================*/
udc_retval_e_type udc_fetch_current_comp_config
(
  udc_comp_inst_handle_type        comp_instance_handle,
  udc_comp_config_options_s_type*  comp_config_options_ptr
);

/*===========================================================================
FUNCTION UDC_UPDATE_COMP_DYNAMIC_CFG_PARAMS

DESCRIPTION
  Update the dynamic config options.
  All dynamic options present in comp_dynamic_cfg_params_ptr will be copied.
  Client is supposed to fetch the current config, update it as required and
  then send it here.
 
DEPENDENCIES
  None

ARGUMENTS
  [in] comp_instance_handle : Compressor Instance Handle
  [in] comp_dynamic_cfg_params_ptr : Compressor configuration to update.

RETURN VALUE
  udc_retval_e_type

SIDE EFFECTS
  None
===========================================================================*/
udc_retval_e_type udc_update_comp_dynamic_cfg_params
(
  udc_comp_inst_handle_type            comp_instance_handle,
  udc_comp_dynamic_cfg_params_s_type*  comp_dynamic_cfg_params_ptr
);

/*===========================================================================
FUNCTION UDC_ALLOC_COMPRESSOR

DESCRIPTION
  Allocates a UDC Compressor Instance.  
 
DEPENDENCIES
  For now only one instance allocation supported.

ARGUMENTS
  [in]  comp_config_options_ptr : Compressor configuration.
  [out] udc_retval_ptr : Success/Error value

RETURN VALUE
  Non-zero handle for success, NULL handle for failure.

SIDE EFFECTS
  None
===========================================================================*/
udc_comp_inst_handle_type udc_alloc_compressor
(
  udc_comp_config_options_s_type*  comp_config_options_ptr,
  udc_retval_e_type*               udc_retval_ptr
);

/*===========================================================================
FUNCTION UDC_FREE_COMPRESSOR

DESCRIPTION
  Free the specified compressor instance.
  All internal memory allocated will be freed.

DEPENDENCIES
  NONE
  
ARGUMENTS
  [in]  comp_instance_handle : Compressor Instance Handle returned by 
                               udc_alloc_compressor
  [out] udc_err_ptr : Error value in case of failure.

RETURN VALUE
  udc_retval_e_type

SIDE EFFECTS
  None
===========================================================================*/
udc_retval_e_type udc_free_compressor
(
  udc_comp_inst_handle_type  comp_instance_handle
);

/*===========================================================================
FUNCTION UDC_COMPRESS_PKT

DESCRIPTION
  Compression attempted on the input packet.

DEPENDENCIES
  None

ARGUMENTS
  [in] udc_comp_req_info : Packet compress request info

RETURN VALUE
  udc_retval_e_type

SIDE EFFECTS
  None
===========================================================================*/
udc_retval_e_type udc_compress_pkt
(
  udc_comp_req_info_s_type  udc_comp_req_info
);

/*===========================================================================
FUNCTION UDC_GET_COMP_MEM_BUF_CHECKSUM

DESCRIPTION
  Compute checksum on the current compressor memory buffer.

DEPENDENCIES
  NONE
  
ARGUMENTS
  [in]  comp_instance_handle : Compressor Instance Handle
  [out] cksum_ptr : Argument to return the checksum value
  Checksum value: In Network Byte Order
  This is the sum of 4 byte words of the prefill buffer in network byte
   order.
   i.e. 
   - pick up first 4 bytes as it is (network byte order) 
   - pick up next 4 bytes as it is ..
   - Keep adding
  
   Also, the buffer is padded at the end with 0x00 in 1,2 or 3 bytes as
   required to make its length a multiple of four.
  
   Checksum value can be zero.
   To further confirm is memory buffer is all zeroes, following API should
   be used: udc_is_comp_mem_all_zeroes()

RETURN VALUE
  0 for success
  -1 for failure

SIDE EFFECTS
  None
===========================================================================*/
int32 udc_get_comp_mem_buf_checksum
(
  udc_comp_inst_handle_type  comp_instance_handle,
  uint16*                    cksum_ptr
);

/*===========================================================================
FUNCTION UDC_IS_COMP_MEM_ALL_ZEROES

DESCRIPTION
  Check if UDC compressor memory (ULCM) is all zeroes.
  Can be used to confirm in case a zero checksum if received.

DEPENDENCIES
  NONE
  
ARGUMENTS
  [in] comp_instance_handle : Compressor Instance Handle

RETURN VALUE
  TRUE is compressor memory is all zeroes
  FALSE otherwise

SIDE EFFECTS
  None
===========================================================================*/
boolean udc_is_comp_mem_all_zeroes
(
  udc_comp_inst_handle_type  comp_instance_handle
);

/*===========================================================================
FUNCTION UDC_RESET_COMPRESSOR

DESCRIPTION
  Reset the memory buffer (ULCM) and compressor statistics.

DEPENDENCIES
  NONE
  
ARGUMENTS
  [in] comp_instance_handle : Compressor Instance Handle

RETURN VALUE
  udc_retval_e_type

SIDE EFFECTS
  None
===========================================================================*/
udc_retval_e_type udc_reset_compressor
(
  udc_comp_inst_handle_type  comp_instance_handle
);

/*===========================================================================
FUNCTION UDC_RESET_COMP_STATS

DESCRIPTION
  Reset compressor statistics.

DEPENDENCIES
  NONE
  
ARGUMENTS
  [in] comp_instance_handle : Compressor Instance Handle

RETURN VALUE
  udc_retval_e_type

SIDE EFFECTS
  None
===========================================================================*/
udc_retval_e_type udc_reset_comp_stats
(
  udc_comp_inst_handle_type  comp_instance_handle
);

/*===========================================================================
FUNCTION UDC_SET_CONFIG_LOG_COMP_PKT

DESCRIPTION
  Specify whether to log compressed packets.
  Once set to TRUE, all compressed packets will be logged henceforth.

DEPENDENCIES
  None
  
ARGUMENTS
  [in] log_comp_pkt : Flag indicating whether to log compress packet.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void udc_set_config_log_comp_pkt
(
  boolean log_comp_pkt
);

/*===========================================================================
FUNCTION UDC_SET_CONFIG_LOG_PER_PKT_METRICS

DESCRIPTION
  Specify whether to log per packet metrics.
  Once set to TRUE, metrics will be logged per packet.

DEPENDENCIES
  None
  
ARGUMENTS
  [in] log_per_pkt_metrics : Flag indicating whether to log per pkt metrics.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void udc_set_config_log_per_pkt_metrics
(
  boolean log_per_pkt_metrics
);

/*===========================================================================
FUNCTION UDC_SET_CONFIG_PKT_BET_COMP_MEM_DUMP

DESCRIPTION
  Specify how many packets between compressor memory dump logging.
  Value of 0 indicates no compressor memory dump logging.

DEPENDENCIES
  None
  
ARGUMENTS
  [in] pkt_bet_comp_mem_dump : How many packets compressed between compressor
                               memory dump logs.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void udc_set_config_pkt_bet_comp_mem_dump
(
  uint16 pkt_bet_comp_mem_dump
);

/*===========================================================================
FUNCTION UDC_SET_CONFIG_PKT_BET_STATS_LOG_PKT

DESCRIPTION
  Specify how many packets between statistics log packet.
  Value of 0 indicates no statistics packet logging.

DEPENDENCIES
  None
  
ARGUMENTS
  [in] pkt_bet_stats_log_pkt : How many packets compressed between statistics
                               log packets.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void udc_set_config_pkt_bet_stats_log_pkt
(
  uint16 pkt_bet_stats_log_pkt
);

#ifndef TEST_FRAMEWORK
/*===========================================================================
FUNCTION UDC_QSH_HANDLER()

DESCRIPTION
  Allocate target DSM item buffer.

DEPENDENCIES
  None
  
ARGUMENTS
  QSH parameters
  UDC handle

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean udc_qsh_handler
(
  void * params,
  void * udc_handle
);

#endif

#ifdef __cplusplus
}
#endif

#endif /* UDCIF_H */

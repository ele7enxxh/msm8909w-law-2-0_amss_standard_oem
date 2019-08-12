/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*==

                          H D R  U T I L I T I E S

GENERAL DESCRIPTION
  This module contains generic utilites used by many different HDR modules.

EXTERNALIZED FUNCTIONS

  hdrutil_hash
    This function calculates a hash value based on the arguments passed to it.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2000 - 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */

/*=============================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: L:/src/asw/COMMON/vcs/hdrutil.c_v   1.26   12 Mar 2003 10:29:54   vsali  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/common/src/hdrutil.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
05/06/16   vko     Reduce F3 logging
05/09/14   vko     Support to handle bad NV read gracefully
12/19/13   sat     Changed memcpy() to assignment or memscpy() (safer version).
10/29/13   sat     Reduced F3 messages (or/and) ASSERTs
08/27/13   cnx     Remove unsupported channel in hash before session negotiation.
08/01/13   smd     Used new secapi to get random number.
04/17/12   cnx     Support rfm_is_band_chan_supported_v2().
09/16/11   wsh     Reduce heap mem usage by moving memory back to static
07/27/11   cnx     Supported byte stream to dsm and vice vesa.
04/15/11   cnx     Removed hdrutil_band_lists if RF_HAS_RFM_BAND_CHAN_V2_API defined.
02/23/11   cnx     Supported subclass L in BC5.
01/24/11   cnx     Support listing valid subclasses in BC0.   
10/24/10   pxu     Fiexed compiler warnings. 
09/07/10   pxu     Memory heap changes. 
06/21/10   cnx     Fixed compiler warnings.
02/04/10   wsh     Filter invalid channel before providing NSET
09/15/09   kss     Use new PACK() macro for Q6 compatibility.
04/24/09   wsh     Changed code for REX_DISABLE/ENABLE_PREMPTION
04/06/09   wsh     Replaced INTLOCK with TASKLOCK
03/23/09   wsh     CMI: featurized CMI under FEATURE_CMI--
12/10/08   smd     Replaced assert.h with amssassert.h
11/10/08   kss     Removed/replaced intlocks.
10/01/08   wsh     Fixed critical/high lint warnings
05/09/08   sju     Added support to remove avoided channels from NSET
04/18/08   mpa     Cleaned up mode from hdrsrch_nset_pilot_type struct
12/20/07   hal     Fixed subclass range testing
12/12/07   sju     Renamed SystemType enum
09/11/07   wsh     Removed lint high warnings
08/15/07   sju     Addd SystemType checking in hdrutil_remove_invalid_neighbors()
07/30/07   hal     Added hdrutil_pushdown()
07/13/07   sju     Removed hdrutil_mode_num_to_enum()
06/20/07   hal     Reverted DSM change to prevent crashing due to direct DSM data
                   pointer manipulation in legacy code
06/18/07   hal     Added hdrutil_chan_is_in_band_with_all_subclasses()
03/22/07   hal     Changed dsm_pushdown() to dsm_pushdown_packed() for optimization
                   Added F3 messages to trace DSM calls (FEATURE_HDRUTIL_DEBUG)
12/22/06   etv     Added hdrutil_bit_count32().
12/18/06   pba     Modified the bytes reserved at the head of DSM item for 
                   signaling messages to 5.
11/30/06   hal     Added hdrutil_pretty_print_byte_array for F3 debugging
09/29/06   hal(sy) Added backup memory pool for memory based flow control
09/08/06   hal     Added hdrutil_new_dsm_buffer() for DSM buffers without offsets
07/31/06   hal     Moved HDR message item pool to UTIL
07/24/06   hal     Enabled use of HDR message item pool
05/31/06   hal     Added hdrutil_chan_is_in_band()
                   Added hdrutil_get_band_all_sc_mask()
                   Added hdrutil_band_has_subclasses()
04/28/06   etv     Added hdrutil_write_nv_for_hdrmc.
04/28/06   pba     Fixed compiler warnings
04/07/06   hal     Added hdrutil_new_msg_buffer()
                   Added hdrutil_pushdown_head()
                   Added hdrutil_pushdown_tail()
                   Changed hdrutil_new_buffer() to use hdrutil_new_msg_buffer()
11/21/05   hal     Added hdrutil_unpack_channel_record()
10/31/05   hal     Added hdrutil_bit_count16()
08/28/05   mpa     Removed NV_HDRSCP_BCMCS_ENABLE_I (obsolete).
07/07/05   dna     Added support for Enhanced Idle State protocol
06/22/05   mpa     Corrected bad ERR_FATAL parameter.
05/19/05   dna     Backed out previous change since MEID isn't stored in RUIM.
05/10/05   dna     Add support for MEID (NV_HDRAMP_HARDWARE_IDV2_I)
04/04/05   pba     Removed Lint errors
02/04/05   mpa     Added NV_HDRSCP_BCMCS_ENABLE_I.
12/07/04   hal     Added hdrutil_new_buffer wrapper function for dsm_offset_new_buffer.
11/29/04   grl     Added time duration calculating func to support logging.
11/08/04   pba     Added hdrutil_remove_invalid_neighbors() 
11/03/04   mpa     Removed dependency on std.h (merged with sys.h)
11/01/04   mpa     Added BCMCS NV items.
08/26/04   mpa     Added NV_HDRAMP_HARDWARE_ID_I.
09/10/03   mpa     Converted F3 messages to use new MSG2.0 HDRPROT SSID
05/08/03   mpa     Added suspend_nv_write_timer to pause NV writes after 
                   session negotiation.
03/12/03   vas     Do not Error Fatal, but return invalid mode if conversion of
                   mode to enum fails.
10/10/02   mpa     Added Attrib Override Allowed item.
06/14/02   kss     Add hdrmac item support to NV write mechanism; allow 
                   hdrutil_write_nv_item() to be called from other tasks.
06/05/02   dna     Queue NV writes and write the items one at a time.
03/18/02   dna     ASSERT that read/write util functions only called from HDRMC
11/20/01   dna     Added enum HDRHAI_SYSTYPE
06/06/01   dna     new param for hmp_send_msg()
05/22/01   nmn     added send_empty_config_rsp
03/19/01   vas     Fixed seq number validation function. Removed band number to
                   enum conversion. Removed hash function.
01/30/01   dna     Fixed NV read/Write functions
01/18/01   nmn     Added NV read/write functions
06/27/00   vas     Created module.

===============================================================================*/

/* <EJECT> */

/*=============================================================================

                     INCLUDE FILES FOR MODULE

===============================================================================*/
#include "hdr_variation.h"
#include "customer.h"
#include "comdef.h"
#include "err.h"
#include "hdrdebug.h"
#include "hdrutil.h"
#include "amssassert.h"
#include "sd.h"

#ifdef FEATURE_CMI
#include "sd_v.h"
#endif

#include "secapi.h"
#include "task.h"
#include "hdrmci.h"
#include "hdrbit.h"
#include "nv.h"
#include "hdrhai.h"
#include "hdrrupc.h"

#include "modem_mem.h"
#include "fs_public.h"

#ifdef FEATURE_MEMCPY_REMOVAL
#include "stringl.h"
#endif /* FEATURE_MEMCPY_REMOVAL */

/*=============================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/
#undef FEATURE_HDRUTIL_DEBUG /* Debug F3s should be enabled if needed */

/* <EJECT> */

/*---------------------------------------------------------------------------
  Size, Count, Few, many and do not exceed counts for HDR items
---------------------------------------------------------------------------*/
#define DSMI_HDR_MSG_ITEM_SIZ                 128

#define DSMI_HDR_MSG_ITEM_CNT                  20
#define DSMI_HDR_MSG_ITEM_DONT_EXCEED           5
#define DSMI_HDR_MSG_ITEM_FEW_MARK              8
#define DSMI_HDR_MSG_ITEM_MANY_MARK            12
  
/*---------------------------------------------------------------------------
  Definitions for the HDR Message Items Pool 
---------------------------------------------------------------------------*/
#define DSMI_HDR_MSG_ITEM_ARRAY_SIZ ((DSMI_HDR_MSG_ITEM_CNT * \
   (DSMI_HDR_MSG_ITEM_SIZ + DSM_ITEM_HEADER_SIZE)) + 31)
   
static uint32 dsm_hdr_msg_item_array[DSMI_HDR_MSG_ITEM_ARRAY_SIZ/4];

#define DSM_HDR_MSG_ITEM_POOL ((dsm_mempool_id_type)(&dsm_hdr_msg_item_pool))
dsm_pool_mgmt_table_type dsm_hdr_msg_item_pool;

/* Includes 1 byte SNP (HMP), 3 bytes Stream/SLP (worst case), 1 byte PCP */
#define HDRUTIL_MAX_MSG_HEADER_SIZE 5

#define HDRUTIL_SYSTEM_TYPE_SIZE     8
#define HDRUTIL_BAND_CLASS_SIZE      5
#define HDRUTIL_CHANNEL_NUMBER_SIZE 11

/* Channel Record System Type */
#define HDRUTIL_UNPACK_SYSTEM_TYPE( ptr, val_ptr, offset )  \
          HDRBIT_UNPACK8( ptr, val_ptr, (uint16) offset,  8 )

/* Channel from Channel Record */
#define HDRUTIL_UNPACK_BAND_CLASS( ptr, val_ptr, offset) \
          HDRBIT_UNPACK8( ptr, val_ptr, (uint16) offset,  5 )

/* Channel Record. Channel Number */
#define HDRUTIL_UNPACK_CHANNEL_NUMBER( ptr, val_ptr, offset )  \
          HDRBIT_UNPACK16( ptr, val_ptr, (uint16) offset,  11 )

#define IS_INRANGE( val, min, max )  ( (val) >= (min) && (val) <= (max) )
#define IS_LESS_THAN( val, max )  ( (val) <= (max) )

#define HDRUTIL_NV_BUFS_NUM 30

#define HDRUTIL_NV_WRITE_MAX_ITEMS 13 /* Process no more than 13 queued up items
                                         in a row */
#define HDRUTIL_NV_WRITE_HOLD_TIME 55 /* Suspend NV writes for 55ms if we just 
                                         wrote 13 NV items back to back */

#define HDRUTIL_NV_READ_MAX_ITEMS 33 /* Number of HDR NV's that are read at max */

#define HDRUTIL_MEM_DIR  "/hdr"
#define HDRUTIL_MEM_FILE "/hdr/hdr_heap.bin"

/* This structure has storage for one instance of each NV item used by 
 * HDRMC task.  It is a cache for items being written by the various 
 * protocols, until they can actually be written to NV.  This is needed
 * because when a session first opens, HDRMC task needs to write most 
 * of these items at the same time. The values are pulled from many places
 * and passed to hdrutil_write_nv_item, but HDRMC can not affort to block
 * servicing its event queue while they are all written.  Rather than 
 * declaring a buffer of 15 nv_item_type values and queueing them all at 
 * once (which would waste RAM), we use this cache and then write the items 
 * one at a time.
 * This structure has a total size of 154 + 27 (mac items) = 181 bytes.
 */

typedef PACK( struct )
{
/* Type for  NV_HDRIDLE_SLOT_CYCLE_TIMEOUTS_I item.                        */
/* -------------------------------                                         */
  nv_hdridle_slot_cycle_timeouts_type   hdridle_slot_cycle_timeouts;

/* Type for  NV_HDRIDLE_ENHANCED_CONFIG_I item.                            */
/* -------------------------------                                         */
  nv_hdridle_enhanced_config_type       hdridle_enhanced_config;

/* Type for  NV_HDRSMP_KEEP_ALIVE_REQ_INT_I item.                          */
/* -------------------------------                                         */
  uint16                                hdrsmp_keep_alive_req_int;

/* Type for  NV_HDRAMP_ADDRESS_DATA_I item. (28 bytes)                     */
/* -------------------------------                                         */
  nv_hdramp_address_data_type           hdramp_address_data;

/* Type for  NV_HDRSCP_SESSION_STATUS_I item.                              */
/* -------------------------------                                         */
  nv_hdrscp_session_status_enum_type    hdrscp_session_status;  

  /* The session status is written to NV_SCP_SESSION_INACTIVE
   * before we start writing the session information to NV, then 
   * back to NV_SCP_SESSION_ACTIVE after it is all written (to 
   * ensure that power loss will not leave the session
   * information in an unknown state).  For this to work, we need
   * to store the NV_SCP_SESSION_INACTIVE value separately.
   */
  nv_hdrscp_session_status_enum_type    hdrscp_session_inactive;  

/* Type for  NV_HDRSCP_TOKEN_I item.                                       */
/* -------------------------------                                         */
  uint16                                hdrscp_token;

/* Type for  NV_HDRSCP_PROTOCOL_SUBTYPE_I item. (48 bytes)                 */
/* -------------------------------                                         */
  uint16                                
    hdrscp_protocol_subtype[NV_HDRSCP_PRTL_SUBTYPE_SIZ];

/* Type for  NV_HDRSTREAM_CURR_STREAM_CFG_I item. (8 bytes)                */
/* -------------------------------                                         */
  uint16                                
    hdrstream_curr_stream_cfg[NV_HDR_CURR_STREAM_CFG_SIZ];

/* Type for  NV_HDR_SET_MNGMT_SAME_CHAN_I item. (9 bytes)                  */
/* -------------------------------                                         */
  nv_hdr_chan_mgmt_params_type          hdr_set_mngmt_same_chan;

/* Type for  NV_HDR_SET_MNGMT_DIFF_CHAN_I item. (9 bytes)                  */
/* -------------------------------                                         */
  nv_hdr_chan_mgmt_params_type          hdr_set_mngmt_diff_chan;

/* Type for  NV_HDRLUP_UNSOLICITED_ENABLED_I item.                         */
/* -------------------------------                                         */
  boolean                               hdrlup_unsolicited_enabled;

/* Type for  NV_HDRKEP_CONFIG_I item. (23 bytes)                           */
/* -------------------------------                                         */
  nv_hdrkep_config_type                  hdrkep_config;

/* Type for  NV_HDRAUTH_CONFIG_I item. (2 bytes)                           */
/* -------------------------------                                         */
  nv_hdrauth_config_type                 hdrauth_config;

/* Type for NV_HDRSCP_AN_AUTH_STATUS_I item.                               */
/* -------------------------------                                         */
  boolean                                hdrscp_an_auth_status;

/* Type for NV_HDRSMPKA_START_TIME_I item.                                 */
/* -------------------------------                                         */
  qword                                  hdrsmpka_start_time;

/* Type for NV_HDRSMPKA_SENT_TIME_I item.                                  */
/* -------------------------------                                         */
  qword                                  hdrsmpka_sent_time;

/* Type for NV_HDRRUP_SRCH_PARAMS_I item. (5 bytes)                        */
/* -------------------------------                                         */
  nv_hdrrup_srch_params_type             hdrrup_srch_params;


/* Type for  NV_HDRAMAC_INITIAL_CONFIG_I item. (5 bytes)                   */
/* -------------------------------                                         */
  nv_hdramac_initial_config_type        hdramac_initial_config;

/* Type for  NV_HDRAMAC_POWER_PARAMS_I item. (2 bytes)                     */
/* -------------------------------                                         */
  nv_hdramac_power_params_type          hdramac_power_params;

/* Type for  NV_HDRFMAC_DRC_GATING_I item. (1 byte)                        */
/* -------------------------------                                         */
  uint8                                 hdrfmac_drc_gating;

/* Type for  NV_HDRFMAC_DRC_LOCK_I item. (2 bytes)                         */
/* -------------------------------                                         */
  nv_hdrfmac_drc_lock_type              hdrfmac_drc_lock;

/* Type for  NV_HDRFMAC_HANDOFF_DELAYS_I item. (2 bytes)                   */
/* -------------------------------                                         */
  nv_hdrfmac_handoff_delays_type        hdrfmac_handoff_delays;

/* Type for  NV_HDRRMAC_POWER_PARAMS_I item. (7 bytes)                     */
/* -------------------------------                                         */
  nv_hdrrmac_power_params_type          hdrrmac_power_params;

/* Type for  NV_HDRRMAC_RATE_PARAMS_I item. (8 bytes)                      */
/* -------------------------------                                         */
  nv_hdrrmac_rate_params_type           hdrrmac_rate_params;

/* Type for NV_HDRRUP_OVERRIDE_ALLOWED_I item. (1 byte)                    */
/* -------------------------------                                         */
  byte                                  hdrrup_override_allowed;

/* Type for NV_HDRAMP_HARDWARE_I item. (4 bytes)                           */
/* -------------------------------                                         */
  uint32                                hdramp_hardware_id;

/* Type for NV_HDRBCMCS_OVHD_PERIOD_I item. (1 byte)                       */
/* -------------------------------                                         */
  uint8                                 hdrbcmcs_ovhd_period;

/* Type for NV_HDRBCMCS_PAGE_REG_PERIOD_I item. (1 byte)                   */
/* -------------------------------                                         */
  uint8                                 hdrbcmcs_page_reg_period;

/* Type for NV_HDRBCMCS_DYN_REG_PERIOD_I item. (1 byte)                    */
/* -------------------------------                                         */
  uint8                                 hdrbcmcs_dyn_reg_period;

/* Type for NV_HDRBCMCS_MONITOR_PERIOD_I item. (1 byte)                    */
/* -------------------------------                                         */
  uint8                                 hdrbcmcs_monitor_period;

/* Type for NV_HDRBCMCS_NUM_PERIODS_I item. (1 byte)                       */
/* -------------------------------                                         */
  uint8                                 hdrbcmcs_num_periods;

/* Type for NV_HDRSCP_BCMCS_ENABLE_I item. (4 bytes)                       */
/* -------------------------------                                         */
  uint32                                hdrscp_bcmcs_enable;

} hdrutil_nv_item_cache_type;



/* A union of all HDRMC NV items is needed to store the item currently being
 * written to NV, because the hdrutil.nv_item_cache can be modified while
 * NV is in the middle of writing the item.
 */

typedef union
{
/* Type for  NV_HDRIDLE_SLOT_CYCLE_TIMEOUTS_I item.                        */
/* -------------------------------                                         */
  nv_hdridle_slot_cycle_timeouts_type   hdridle_slot_cycle_timeouts;

/* Type for  NV_HDRIDLE_ENHANCED_CONFIG_I item.                            */
/* -------------------------------                                         */
  nv_hdridle_enhanced_config_type       hdridle_enhanced_config;

/* Type for  NV_HDRSMP_KEEP_ALIVE_REQ_INT_I item.                          */
/* -------------------------------                                         */
  uint16                                hdrsmp_keep_alive_req_int;

/* Type for  NV_HDRAMP_ADDRESS_DATA_I item. (28 bytes)                     */
/* -------------------------------                                         */
  nv_hdramp_address_data_type           hdramp_address_data;

/* Type for  NV_HDRSCP_SESSION_STATUS_I item.                              */
/* -------------------------------                                         */
  nv_hdrscp_session_status_enum_type    hdrscp_session_status;  

/* Type for  NV_HDRSCP_TOKEN_I item.                                       */
/* -------------------------------                                         */
  uint16                                hdrscp_token;

/* Type for  NV_HDRSCP_PROTOCOL_SUBTYPE_I item. (48 bytes)                 */
/* -------------------------------                                         */
  uint16                                
    hdrscp_protocol_subtype[NV_HDRSCP_PRTL_SUBTYPE_SIZ];

/* Type for  NV_HDRSTREAM_CURR_STREAM_CFG_I item. (8 bytes)                */
/* -------------------------------                                         */
  uint16                                
    hdrstream_curr_stream_cfg[NV_HDR_CURR_STREAM_CFG_SIZ];

/* Type for  NV_HDR_SET_MNGMT_SAME_CHAN_I item. (9 bytes)                  */
/* -------------------------------                                         */
  nv_hdr_chan_mgmt_params_type          hdr_set_mngmt_same_chan;

/* Type for  NV_HDR_SET_MNGMT_DIFF_CHAN_I item. (9 bytes)                  */
/* -------------------------------                                         */
  nv_hdr_chan_mgmt_params_type          hdr_set_mngmt_diff_chan;

/* Type for  NV_HDRLUP_UNSOLICITED_ENABLED_I item.                         */
/* -------------------------------                                         */
  boolean                               hdrlup_unsolicited_enabled;

/* Type for  NV_HDRKEP_CONFIG_I item. (23 bytes)                           */
/* -------------------------------                                         */
  nv_hdrkep_config_type                  hdrkep_config;

/* Type for  NV_HDRAUTH_CONFIG_I item. (2 bytes)                           */
/* -------------------------------                                         */
  nv_hdrauth_config_type                 hdrauth_config;

/* Type for NV_HDRSCP_AN_AUTH_STATUS_I item.                               */
/* -------------------------------                                         */
  boolean                                hdrscp_an_auth_status;

/* Type for NV_HDRSMPKA_START_TIME_I item.                                 */
/* -------------------------------                                         */
  qword                                  hdrsmpka_start_time;

/* Type for NV_HDRSMPKA_SENT_TIME_I item.                                  */
/* -------------------------------                                         */
  qword                                  hdrsmpka_sent_time;

/* Type for NV_HDRRUP_SRCH_PARAMS_I item. (5 bytes)                        */
/* -------------------------------                                         */
  nv_hdrrup_srch_params_type             hdrrup_srch_params;

/* Type for  NV_HDRAMAC_INITIAL_CONFIG_I item. (5 bytes)                   */
/* -------------------------------                                         */
  nv_hdramac_initial_config_type        hdramac_initial_config;

/* Type for  NV_HDRAMAC_POWER_PARAMS_I item. (2 bytes)                     */
/* -------------------------------                                         */
  nv_hdramac_power_params_type          hdramac_power_params;

/* Type for  NV_HDRFMAC_DRC_GATING_I item. (1 byte)                        */
/* -------------------------------                                         */
  uint8                                 hdrfmac_drc_gating;

/* Type for  NV_HDRFMAC_DRC_LOCK_I item. (2 bytes)                         */
/* -------------------------------                                         */
  nv_hdrfmac_drc_lock_type              hdrfmac_drc_lock;

/* Type for  NV_HDRFMAC_HANDOFF_DELAYS_I item. (2 bytes)                   */
/* -------------------------------                                         */
  nv_hdrfmac_handoff_delays_type        hdrfmac_handoff_delays;

/* Type for  NV_HDRRMAC_POWER_PARAMS_I item. (7 bytes)                     */
/* -------------------------------                                         */
  nv_hdrrmac_power_params_type          hdrrmac_power_params;

/* Type for  NV_HDRRMAC_RATE_PARAMS_I item. (8 bytes)                      */
/* -------------------------------                                         */
  nv_hdrrmac_rate_params_type           hdrrmac_rate_params;

/* Type for NV_HDRRUP_OVERRIDE_ALLOWED_I item. (1 byte)                    */
/* -------------------------------                                         */
  byte                                  hdrrup_override_allowed;

/* Type for NV_HDRAMP_HARDWARE_I item. (4 bytes)                           */
/* -------------------------------                                         */
  uint32                                hdramp_hardware_id;

/* Type for NV_HDRBCMCS_OVHD_PERIOD_I item. (1 byte)                       */
/* -------------------------------                                         */
  uint8                                 hdrbcmcs_ovhd_period;

/* Type for NV_HDRBCMCS_PAGE_REG_PERIOD_I item. (1 byte)                   */
/* -------------------------------                                         */
  uint8                                 hdrbcmcs_page_reg_period;

/* Type for NV_HDRBCMCS_DYN_REG_PERIOD_I item. (1 byte)                    */
/* -------------------------------                                         */
  uint8                                 hdrbcmcs_dyn_reg_period;

/* Type for NV_HDRBCMCS_MONITOR_PERIOD_I item. (1 byte)                    */
/* -------------------------------                                         */
  uint8                                 hdrbcmcs_monitor_period;

/* Type for NV_HDRBCMCS_NUM_PERIODS_I item. (1 byte)                       */
/* -------------------------------                                         */
  uint8                                 hdrbcmcs_num_periods;

/* Type for NV_HDRSCP_BCMCS_ENABLE_I item. (4 bytes)                       */
/* -------------------------------                                         */
  uint32                                hdrscp_bcmcs_enable;

} hdrutil_nv_item_union_type;

typedef struct 
{
  q_link_type           link;        /* Queue link */
  q_type                *done_q_ptr; /* Queue to place buffer on when done */
  nv_items_enum_type    item;        /* NV item */
  void                  *data_ptr;   /* Pointer into hdrutil_nv_item_cache */
  uint8                 size;        /* size of item being written */
} hdrutil_nv_buf_type;

typedef enum
{
  HDRUTIL_SUSPEND_NV_WRITE_TIMER
} hdrutil_timer_enum_type;

LOCAL struct
{
  rex_timer_type suspend_nv_write_timer;   
    /* suspend timer for pausing nv writes after session negotiation */
  uint8          suspend_nv_write_threshold;
    /* Number of NV items that have been written back to back. 
       NV writes will be suspended after a certain threshold is reached. */
  q_type nv_q;       
    /* Buffers for nv_q */
  nv_cmd_type nv_cmd; 
    /* Command buffer for writing to NV */
  hdrutil_nv_item_union_type current_nv_item; 
    /* NV Item being written */

  /* Static cache of items waiting to be written  to NV */
  hdrutil_nv_item_cache_type nv_item_cache; 
} hdrutil;

static nv_items_enum_type hdrutil_nv_list[HDRUTIL_NV_READ_MAX_ITEMS]=
{
NV_ACCOLC_I,                            
NV_HDRSCP_SESSION_STATUS_I,             
NV_HDRRMAC_RATE_PARAMS_I,               
NV_COLLOC_DISALLOWED_TIME_I,            
NV_HOLD_HDR_TIME_I,                     
NV_HDRSMPKA_START_TIME_I,               
NV_HDRSMPKA_SENT_TIME_I,                
NV_HDR_RX_DIVERSITY_CTRL_I,            
NV_HDR_AN_AUTH_USER_ID_LONG_I,          
NV_SN_PPP_STATUS_I,                     
NV_HDR_EQUALIZER_CTRL_I,                
NV_HDRMRLP_NUM_BYTES_TO_LOG_I,          
NV_HDRSCP_SUBTYPE_CUSTOM_CONFIG_I,      
NV_HDRBCMCS_DISABLE_ONE_TIME_REG_I,     
NV_HDRBCMCS_DISABLE_SUBNET_REG_I,       
NV_SD_CFG_ITEMS_I,                      
NV_HDR_MIN_UATI_MODE_I,                 
NV_HDRSCMDB_SECURE_CONFIG_I,            
NV_HDRL1_DBG_MSK_I,                     
NV_HDRIDLE_PREF_SLOTTED_MODE_CONFIG_I,  
NV_HIT_ENABLE_I,                        
NV_HDR_RX_DIV_BC_ENABLE_MASK_I,         
NV_HDRSRCH_DBG_MSK_I,                   
NV_HDRSCP_FORCE_AT_CONFIG_I,            
NV_ESN_ME_I,                            
NV_MEID_ME_I,                           
NV_HDRSCP_SELECT_BCMCS_CONFIG_I,        
NV_HDRMRLPARDA_PARAM_I,                 
NV_PPP_EAP_SHARED_SECRET_I,             
NV_EHRPD_MILENAGE_OP_I,                 
NV_EHRPD_IMSI_I,                        
NV_HDRSCP_FORCE_RESTRICTED_CF_I,        
NV_EHRPD_AUTH_IN_USIM_I
};

uint64 hdrutil_nv_fail_mask = 0;
boolean hdrutil_nv_error = FALSE;

/*=============================================================================

                        FUNCTION DEFINITIONS

=============================================================================*/

/*===========================================================================

FUNCTION HDRUTIL_TIMER_CB

DESCRIPTION
  This function is the callback for util timers.
  This function is called by REX when the timer expires, and runs
  in the context of the task that defined the timer (HDRMC).

DEPENDENCIES
  None.

PARAMETERS
  timer_id - ID of the timer that expired
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrutil_timer_cb 
( 
  uint32 timer_id
)
{

  switch ( (hdrutil_timer_enum_type) timer_id)
  {
    case HDRUTIL_SUSPEND_NV_WRITE_TIMER:
      (void) hdrutil_nv_q_srv();
      break;

    default:
      HDR_MSG_PROT_1 (MSG_LEGACY_ERROR, "Unknown timer_id (%d)", timer_id);
  }

} /* hdrutil_timer_cb() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRUTIL_POWERUP_INIT

DESCRIPTION
  This function performs powerup initialization for the hdrutil module.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrutil_powerup_init( void )
{
  /* Initialize queues. */
  ( void ) q_init( &hdrutil.nv_q );

  /* Since hdrutil_nv_q_srv() checks this before it will read from the NV
   * queue, this needs to be initialized at powerup in case the first NV
   * write comes from outside the hdrmc task.
   */
  hdrutil.nv_cmd.status = NV_DONE_S;

  /* Define a suspend timer to take a pause in the nv writes.  This is to 
     give lower task priorities a chance to run when many nv items need
     to be written in a row (i.e. after session negotiation) */
  hdrutil.suspend_nv_write_threshold = 0;
  rex_def_timer_ex(&hdrutil.suspend_nv_write_timer, 
                   hdrutil_timer_cb, 
                   HDRUTIL_SUSPEND_NV_WRITE_TIMER);

  (void) dsm_init_pool(DSM_HDR_MSG_ITEM_POOL,
        (uint8*)dsm_hdr_msg_item_array,
        DSMI_HDR_MSG_ITEM_ARRAY_SIZ,
        DSMI_HDR_MSG_ITEM_SIZ);

} /* hdrutil_powerup_init() */

/*=============================================================================
FUNCTION HDRUTIL_MSG_SEQ_NUM_IS_GREATER

DESCRIPTION
  This function validates a received message sequence number according to 
  the rules specified in the HAI.

DEPENDENCIES
  None.

PARAMETERS
  new_seq_num - New Sequence munber
  old_seq_num - Old (previously received) Sequence number
  seq_num_bits - Number of bits in the Sequence number

RETURN VALUE
  TRUE  - If the new message sequence number is valid.
  FALSE - Otherwise

SIDE EFFECTS
  None.
=============================================================================*/
boolean hdrutil_msg_seq_num_is_greater
( 
  uint8 new_seq_num, 
  uint8 old_seq_num, 
  uint8 seq_num_bits 
)
{
  uint16 seq_space = (1<<seq_num_bits); /* If # of bits = s, seq space = 2^s */
  uint16 great_num_limit;       /* Limit for valid greater seq number values */
  boolean num_is_greater;       /* Flag for if new seq num > old seq num */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*---------------------------------------------------------------------------
  Check that number of bits is not 0 & the sequence number do not exceed the
  sequence space.
  ---------------------------------------------------------------------------*/
  ASSERT((seq_num_bits != 0) &&
         (new_seq_num < seq_space ) && (old_seq_num < seq_space ));

  /*---------------------------------------------------------------------------
  Calculate the range limit for which a new sequence number is considered 
  greater than the old one.
  ---------------------------------------------------------------------------*/
  great_num_limit = old_seq_num + seq_space/2 ;

  /*---------------------------------------------------------------------------
  For any sequence number N, the numbers in the range [N+1, N+2^(s-1)-1] is
  greater than N. Any number in the range [N-2^(s-1), N-1] is smaller
  ---------------------------------------------------------------------------*/
  if( new_seq_num > old_seq_num ) 
  {
    if( new_seq_num < great_num_limit ) 
    {
      num_is_greater = TRUE;
    }
    else
    {
      num_is_greater = FALSE;
    }
  }
  else /* new num < old num */
  {
    if( great_num_limit < seq_space )
    {
      /* (great_num_limit < seq_space) implies no modulo is needed */
      num_is_greater = FALSE;
    }
    else if( (uint16) new_seq_num >= great_num_limit - seq_space )
    {
      num_is_greater = FALSE;
    }
    else
    {
      num_is_greater = TRUE;
    }
  }

  return num_is_greater;

} /* hdrutil_msg_seq_num_is_greater */

/* <EJECT> */

#ifdef FEATURE_HDR_HAIX4
#error code not present
#endif /* FEATURE_HDR_HAIX4 */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRUTIL_NV_WRITE

DESCRIPTION
  This function writes an item to NV.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrutil_nv_write
(
  nv_items_enum_type item_code, /* item to write */
  nv_item_type * item_ptr,      /* pointer to the data to be written */
  uint8 size                    /* size of item being written */
)
{

  /* Copy item into or static location to avoid race conditions */
#ifdef FEATURE_MEMCPY_REMOVAL
  (void) memscpy( &hdrutil.current_nv_item, sizeof(hdrutil.current_nv_item), 
                  (void *) item_ptr, size );
#else
  (void) memcpy( &hdrutil.current_nv_item, (void *) item_ptr, size );
#endif /* FEATURE_MEMCPY_REMOVAL */
  /* Prepare command buffer to NV */

  hdrutil.nv_cmd.cmd = NV_WRITE_F;
  hdrutil.nv_cmd.tcb_ptr = HDRMC_TASK_PTR;
  hdrutil.nv_cmd.sigs = HDRMC_NV_WRITE_SIG;
  hdrutil.nv_cmd.done_q_ptr = NULL;
  hdrutil.nv_cmd.item = item_code;
  hdrutil.nv_cmd.data_ptr = (nv_item_type *)&hdrutil.current_nv_item;

  nv_cmd(&hdrutil.nv_cmd);
} /* hdrutil_nv_write() */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRUTIL_WRITE_NV_FOR_HDRMC

DESCRIPTION
  This function could be used to write an NV item in hdrmc task context.

  Note: This function call would block hdrmc task until the NV write is 
  complete. 
  
DEPENDENCIES
  None.

ASSUMPTIONS
  1. This function assumes that the calling function would fill-in the NV item
     name and the data_ptr in the NV command. 
  2. This function would be called only from HDRMC task context.
  
RETURN VALUE
  None

SIDE EFFECTS
  The call to this function blocks until the NV write is complete.
  This function would result in a FATAL error if the NV write fails.

===========================================================================*/
LOCAL void hdrutil_write_nv_for_hdrmc
(
  nv_cmd_type *cmd
)
{

  /* Make sure this function is called only in HDRMC task context. */
  ASSERT(rex_self() == HDRMC_TASK_PTR);

  cmd->cmd = NV_WRITE_F;                    /* NV write command */
  cmd->tcb_ptr = HDRMC_TASK_PTR;            /* HDRMC task's tcb */
  cmd->sigs = HDRMC_BLOCKING_NV_WRITE_SIG;  /* Blocking NV write signal */
  cmd->done_q_ptr = NULL;                   

  /* Post the NV command to NV Queue */
  nv_cmd(cmd);

  /* Block until the signal is set (i.e. until the command has been serviced) */
  (void)hdrmc_wait(HDRMC_BLOCKING_NV_WRITE_SIG);

  /* Clear the signal as soon as the command is serviced */
  (void) rex_clr_sigs(HDRMC_TASK_PTR, HDRMC_BLOCKING_NV_WRITE_SIG);

  /* Check status. If there was an error writing to the NV item... */
  if (cmd->status != NV_DONE_S)
  {
    ERR_FATAL("Error writing NV: %d", cmd->item, 0, 0);
  }

} /* hdrutil_write_nv_for_hdrmc() */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRUTIL_WRITE_NV_ITEM

DESCRIPTION
  This function writes an item to NV

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  ERR_FATAL if the command return is not DONE

===========================================================================*/
void hdrutil_write_nv_item
(
  nv_items_enum_type item_code, /* item to write */
  nv_item_type * item_ptr       /* pointer to the data to be written */
)
{
  /* Pointer to NV item to be enqueued */
  hdrutil_nv_buf_type  *buf_ptr;

  /* Pointer to NV item cache to copy item into */
  hdrutil_nv_item_cache_type *cache_ptr = &hdrutil.nv_item_cache;
  
  /* Pointer to entry in NV item cache where current item belongs */
  void *cache_entry_ptr;

  uint8 size=0; /* Size of NV item being written */

  boolean nv_set_sig = FALSE;
    /* Whether NV write signal needs to be set for HDRMC */

  boolean direct_write = FALSE;
    /* Whether AT can directly issue NV write command without queuing */

  void* tmp_ptr;
    /* Temporary pointer */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(item_ptr != NULL);
  
  /* Get a pointer to the cache entry where we want to store this item. */
  switch (item_code)
  {
    case NV_HDRIDLE_SLOT_CYCLE_TIMEOUTS_I:
      cache_entry_ptr = (void *)&cache_ptr->hdridle_slot_cycle_timeouts;
      size = sizeof(nv_hdridle_slot_cycle_timeouts_type);
      break;

    case NV_HDRIDLE_ENHANCED_CONFIG_I:
      cache_entry_ptr = (void *)&cache_ptr->hdridle_enhanced_config;
      size = sizeof(nv_hdridle_enhanced_config_type);
      break;

    case NV_HDRSMP_KEEP_ALIVE_REQ_INT_I:
      cache_entry_ptr = (void *)&cache_ptr->hdrsmp_keep_alive_req_int;
      size = sizeof(uint16);
      break;

    case NV_HDRAMP_ADDRESS_DATA_I:
      cache_entry_ptr = (void *)&cache_ptr->hdramp_address_data;
      size = sizeof(nv_hdramp_address_data_type);
      break;

    case NV_HDRSCP_SESSION_STATUS_I:
      /* Store the value NV_SCP_SESSION_INACTIVE in a separate cache
       * entry so it won't be overwritten by NV_SCP_SESSION_ACTIVE
       * in the event that both are queued before either is written.
       */

      /* Use void pointer as a bridge to avoid the following compiler warning: 
         "cast increases required alignment of target type". */
      tmp_ptr = (void *)item_ptr;
      if (*((nv_hdrscp_session_status_enum_type*) tmp_ptr) == 
           NV_SCP_SESSION_INACTIVE)
      {
        HDR_MSG_PROT_2 (MSG_LEGACY_HIGH, 
                        "NVWrite SessionStatus->%d (%d items q'd)",
                        *((nv_hdrscp_session_status_enum_type*) tmp_ptr),
                        q_cnt(&hdrutil.nv_q));
        cache_entry_ptr = (void *)&cache_ptr->hdrscp_session_inactive;  
      }
      else
      {
        HDR_MSG_PROT_2 (MSG_LEGACY_HIGH, 
                        "NVWrite SessionStatus->%d (%d items q'd)",
                        *((nv_hdrscp_session_status_enum_type*) tmp_ptr),
                        q_cnt(&hdrutil.nv_q));
        cache_entry_ptr = (void *)&cache_ptr->hdrscp_session_status;  
      }
      size = sizeof(nv_hdrscp_session_status_enum_type);
      break;

    case NV_HDRSCP_TOKEN_I:
      cache_entry_ptr = (void *)&cache_ptr->hdrscp_token;
      size = sizeof(uint16);
      break;

    case NV_HDRSCP_PROTOCOL_SUBTYPE_I:
      cache_entry_ptr = (void *)cache_ptr->hdrscp_protocol_subtype;
      size = NV_HDRSCP_PRTL_SUBTYPE_SIZ * sizeof(uint16);
      break;

    case NV_HDRSTREAM_CURR_STREAM_CFG_I:
      cache_entry_ptr = (void *)cache_ptr->hdrstream_curr_stream_cfg;
      size = NV_HDR_CURR_STREAM_CFG_SIZ * sizeof(uint16);
      break;

    case NV_HDR_SET_MNGMT_SAME_CHAN_I:
      cache_entry_ptr = (void *)&cache_ptr->hdr_set_mngmt_same_chan;
      size = sizeof(nv_hdr_chan_mgmt_params_type);
      break;

    case NV_HDR_SET_MNGMT_DIFF_CHAN_I:
      cache_entry_ptr = (void *)&cache_ptr->hdr_set_mngmt_diff_chan;
      size = sizeof(nv_hdr_chan_mgmt_params_type);
      break;

    case NV_HDRLUP_UNSOLICITED_ENABLED_I:
      cache_entry_ptr = (void *)&cache_ptr->hdrlup_unsolicited_enabled;
      size = sizeof(boolean);
      break;

    case NV_HDRKEP_CONFIG_I:
      cache_entry_ptr = (void *)&cache_ptr->hdrkep_config;
      size = sizeof(nv_hdrkep_config_type);
      break;

    case NV_HDRAUTH_CONFIG_I:
      cache_entry_ptr = (void *)&cache_ptr->hdrauth_config;
      size = sizeof(nv_hdrauth_config_type);
      break;

    case NV_HDRSCP_AN_AUTH_STATUS_I:
      cache_entry_ptr = (void *)&cache_ptr->hdrscp_an_auth_status;
      size = sizeof(boolean);
      break;

    case NV_HDRSMPKA_START_TIME_I:
      cache_entry_ptr = (void *)cache_ptr->hdrsmpka_start_time;
      size = sizeof(qword);
      break;

    case NV_HDRSMPKA_SENT_TIME_I:
      cache_entry_ptr = (void *)cache_ptr->hdrsmpka_sent_time;
      size = sizeof(qword);
      break;

    case NV_HDRRUP_SRCH_PARAMS_I:
      cache_entry_ptr = (void *)&cache_ptr->hdrrup_srch_params;
      size = sizeof(nv_hdrrup_srch_params_type);
      break;

    case NV_HDRAMAC_INITIAL_CONFIG_I:
      cache_entry_ptr = (void *)&cache_ptr->hdramac_initial_config;
      size = sizeof(nv_hdramac_initial_config_type);
      break;

    case NV_HDRAMAC_POWER_PARAMS_I:
      cache_entry_ptr = (void *)&cache_ptr->hdramac_power_params;
      size = sizeof(nv_hdramac_power_params_type);
      break;

    case NV_HDRFMAC_DRC_GATING_I:
      cache_entry_ptr = (void *)&cache_ptr->hdrfmac_drc_gating;
      size = sizeof(uint8);
      break;

    case NV_HDRFMAC_DRC_LOCK_I:
      cache_entry_ptr = (void *)&cache_ptr->hdrfmac_drc_lock;
      size = sizeof(nv_hdrfmac_drc_lock_type);
      break;

    case NV_HDRFMAC_HANDOFF_DELAYS_I:
      cache_entry_ptr = (void *)&cache_ptr->hdrfmac_handoff_delays;
      size = sizeof(nv_hdrfmac_handoff_delays_type);
      break;

    case NV_HDRRMAC_POWER_PARAMS_I:
      cache_entry_ptr = (void *)&cache_ptr->hdrrmac_power_params;
      size = sizeof(nv_hdrrmac_power_params_type);
      break;

    case NV_HDRRMAC_RATE_PARAMS_I:
      cache_entry_ptr = (void *)&cache_ptr->hdrrmac_rate_params;
      size = sizeof(nv_hdrrmac_rate_params_type);
      break;

    case NV_HDRRUP_OVERRIDE_ALLOWED_I:
      cache_entry_ptr = (void *)&cache_ptr->hdrrup_override_allowed;
      size = sizeof(uint8);
      break;

    case NV_HDRAMP_HARDWARE_ID_I:
      cache_entry_ptr = (void *)&cache_ptr->hdramp_hardware_id;
      size = sizeof(uint32);
      break;

    case NV_HDRBCMCS_OVHD_PERIOD_I:
      cache_entry_ptr = (void *)&cache_ptr->hdrbcmcs_ovhd_period;
      size = sizeof(uint8);
      break;

    case NV_HDRBCMCS_PAGE_REG_PERIOD_I:
      cache_entry_ptr = (void *)&cache_ptr->hdrbcmcs_page_reg_period;
      size = sizeof(uint8);
      break;

    case NV_HDRBCMCS_DYN_REG_PERIOD_I:
      cache_entry_ptr = (void *)&cache_ptr->hdrbcmcs_dyn_reg_period;
      size = sizeof(uint8);
      break;

    case NV_HDRBCMCS_MONITOR_PERIOD_I:
      cache_entry_ptr = (void *)&cache_ptr->hdrbcmcs_monitor_period;
      size = sizeof(uint8);
      break;

    case NV_HDRBCMCS_NUM_PERIODS_I:
      cache_entry_ptr = (void *)&cache_ptr->hdrbcmcs_num_periods;
      size = sizeof(uint8);
      break;

    default:
      ERR("Can't write Unknown NV Item %d",item_code,0,0);
      return;
  }

  /* Lock signal set and/or queue ops, since this could be called from 
  * multiple tasks.
  */
  REX_DISABLE_PREMPTION();

  /* If nothing is going on and we're in HDRMC task, 
   * we can write the item now. 
   */
  if ((q_cnt(&hdrutil.nv_q) == 0) &&
      (hdrutil.nv_cmd.status != NV_BUSY_S))
  {
    if (rex_self() == HDRMC_TASK_PTR)
    {
      direct_write = TRUE;
    }
    else
    {
      /* If the request is coming from another task, we will queue it. Set
       * flag to set it later. 
       */
      nv_set_sig = TRUE;
    }
  }                           

  if (!direct_write) 
  {
    /* Get a buffer from the heap */
    if ( ( buf_ptr = ( hdrutil_nv_buf_type * ) modem_mem_alloc( 
      sizeof( hdrutil_nv_buf_type ), MODEM_MEM_CLIENT_HDR_CRIT ) ) == NULL )
    {
      ERR( "No free memory in heap.", 0, 0, 0 );
    }
    else
    {
      /* Using a cache with one entry for each item has the side effect that
       * if the same item is written multiple times quickly to different values,
       * the previous values are lost and only the most recent value is 
       * remembered, and that value will be written instead, at the time the 
       * previous value was intended to be written.  Because of this, special
       * handling was added for NV_HDRSCP_SESSION_STATUS_I.
       */
      buf_ptr->item = item_code;
      buf_ptr->size = size;
      buf_ptr->data_ptr = cache_entry_ptr;
#ifdef FEATURE_MEMCPY_REMOVAL
      (void) memscpy( cache_entry_ptr, size, (void *) item_ptr, size );
#else
      (void) memcpy( cache_entry_ptr, (void *) item_ptr, size );
#endif /* FEATURE_MEMCPY_REMOVAL */
      ( void ) q_link( buf_ptr, &buf_ptr->link );
      q_put( &hdrutil.nv_q, &buf_ptr->link );
    }
  }

  REX_ENABLE_PREMPTION();

  if (direct_write) 
  {
    /* Write the item directly without queueing it. */
    hdrutil_nv_write(item_code, item_ptr, size);
  }

  if(nv_set_sig)
  {
    /* The following is somewhat of a misuse of the NV_WRITE sig,
     * since it is meant to signal that an NV write is done.
     * However, since no write is currently in progress we can use
     * it to signal the hdrmc task to process the NV queue.
     */
    (void) rex_set_sigs(HDRMC_TASK_PTR, HDRMC_NV_WRITE_SIG);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION HDRUTIL_NV_Q_SRV

DESCRIPTION
  This function services the NV queue to write the next item.

DEPENDENCIES
  None.

RETURN VALUE
  True if an NV write is in progress as a result of this call
  
SIDE EFFECTS
  None

===========================================================================*/
uint32 hdrutil_nv_q_srv ( void )
{
  /* Pointer to NV item to be written to NV */
  hdrutil_nv_buf_type  *buf_ptr;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* We are in the nv write suspend period.  We shouldn't attempt to write 
     to NV during this time. */
  if ( rex_get_timer(&hdrutil.suspend_nv_write_timer) != 0 ) 
  {
    return TRUE;
  }
  
  /* If there is an error in the request then ERR_FATAL */
  if (hdrutil.nv_cmd.status == NV_BUSY_S)
  {
    /* We are still writing an item, so the signal will be set again later */
    HDR_MSG_PROT (MSG_LEGACY_HIGH, "Previous write still busy");
    return TRUE;
  }

  if (hdrutil.nv_cmd.status != NV_DONE_S)
  {
    ERR_FATAL("Failed write item to NV %d", hdrutil.nv_cmd.item, 0, 0);
  }

  /* If we've written 11 items consecutively and there are more to be written,
     suspend the NV writes for 55ms.  This is to give lower tasks a chance
     to run, usually right after session negotiation */
  if ( hdrutil.suspend_nv_write_threshold++ == HDRUTIL_NV_WRITE_MAX_ITEMS &&
       q_cnt(&hdrutil.nv_q) > 0 )
  {
    hdrutil.suspend_nv_write_threshold = 0;
    (void) rex_set_timer(&hdrutil.suspend_nv_write_timer, HDRUTIL_NV_WRITE_HOLD_TIME );
    return TRUE;
  }

  if ( ( buf_ptr = ( hdrutil_nv_buf_type * ) q_get( &hdrutil.nv_q ) ) ==
                   NULL )
  {
    /* No more items to write */
    hdrutil.suspend_nv_write_threshold = 0;
    return FALSE;
  }

  hdrutil_nv_write( buf_ptr->item, 
                    (nv_item_type *)(buf_ptr->data_ptr), 
                    buf_ptr->size );

  modem_mem_free( buf_ptr, MODEM_MEM_CLIENT_HDR_CRIT );

  return (TRUE);
} /* hdrutil_nv_q_srv */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRUTIL_READ_NV_ITEM

DESCRIPTION
  This function reads an item from NV

DEPENDENCIES
  None.

RETURN VALUE
  NV command return value

SIDE EFFECTS
  ERR_FATAL if the command return is not DONE or NOTACTIVE

===========================================================================*/
nv_stat_enum_type hdrutil_read_nv_item
(
  nv_items_enum_type item_code, /* item to read */
  nv_item_type * item_ptr /* pointer to where to put the item */
)
{
  uint8 i = 0;
  static nv_cmd_type hdr_nv_cmd_buf; /* Command buffer to NV */

  ASSERT(rex_self() == HDRMC_TASK_PTR);
  /* Prepare command buffer to NV */

  hdr_nv_cmd_buf.cmd = NV_READ_F;
  hdr_nv_cmd_buf.tcb_ptr = HDRMC_TASK_PTR;
  hdr_nv_cmd_buf.sigs = HDRMC_NV_CMD_SIG;
  hdr_nv_cmd_buf.done_q_ptr = NULL;
  hdr_nv_cmd_buf.item = item_code;
  hdr_nv_cmd_buf.data_ptr = item_ptr;

  /* Clear signal, issue the command, wait for the response */

  (void) rex_clr_sigs(HDRMC_TASK_PTR, HDRMC_NV_CMD_SIG);
  nv_cmd(&hdr_nv_cmd_buf);
  (void)hdrmc_wait(HDRMC_NV_CMD_SIG);
  (void) rex_clr_sigs(HDRMC_TASK_PTR, HDRMC_NV_CMD_SIG);

  for(i = 0; i < HDRUTIL_NV_READ_MAX_ITEMS; i++)
  {
	if(item_code == hdrutil_nv_list[i])
	   break;
  }
  if(HDRUTIL_NV_READ_MAX_ITEMS == i)
  {
	 HDR_MSG_PROT_1 (MSG_LEGACY_ERROR, "Bad NV read for Unexpected NV %d", item_code);
         return (hdr_nv_cmd_buf.status);
  }
  /* check and return status */

  if ((hdr_nv_cmd_buf.status == NV_DONE_S) ||
      (hdr_nv_cmd_buf.status == NV_NOTACTIVE_S))
  {
      /* reset the flag */
    hdrutil_nv_fail_mask = hdrutil_nv_fail_mask & (~(1<<i));
  }
  else
  {
     /* set the flag */
    hdrutil_nv_fail_mask = hdrutil_nv_fail_mask | (1<<i);
    hdrutil_nv_error = TRUE;
    HDR_MSG_PROT_3 (MSG_LEGACY_ERROR, "Bad NV read for NV %d status %d. Using default value. NV failed mask 0x%016llX",
	                                   item_code,hdr_nv_cmd_buf.status, hdrutil_nv_fail_mask);
  }

  return (hdr_nv_cmd_buf.status);
} /* hdrutil_read_nv_item */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRUTIL_SEND_EMPTY_CONFIG_RSP

DESCRIPTION
  This function delivers an empty Configuration Response Message
    on behalf of the in-config instance of the caller.  
    SCP can not use this function because it sends config messages from
    its in-use instance.

DEPENDENCIES
  None.

PARAMETERS
  caller - name of the protocol sending the empty config response message
  transaction_id - transaction_id to use for response

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrutil_send_empty_config_rsp
(
  hdrhai_protocol_name_enum_type caller,
  uint8 transaction_id
)
{
  dsm_item_type * send_msg_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  send_msg_ptr = hdrutil_new_msg_buffer();

  HDRBIT_PACK8(send_msg_ptr, 0x51, 0, 8);
  HDRBIT_PACK8(send_msg_ptr, transaction_id, 8, 8);

  hdrhmp_send_msg(caller,
                  HDRHAI_RTC,
                  TRUE,
                  40,
                  HDRHAI_IN_CONFIG, /* Instance of protocol sending message */
                  send_msg_ptr,
                  NULL,
                  NULL);

} /* hdrutil_send_empty_config_rsp */

/*===========================================================================

FUNCTION HDRUTIL_IS_BAND_SUPPORTED

DESCRIPTION
  This function checks if a band is supported based on hardware configuration
  and user preference.
  
DEPENDENCIES
  None.

PARAMETERS
  channel - the band class to be checked.

RETURN VALUE
  TRUE if the band is supported.
  FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
boolean hdrutil_is_band_supported
(
  sys_band_class_e_type band
)
{
  /* sd_misc_is_target_supp_mode_band_class() only checks if band is supported
   * by hardware. 
   * sd_misc_get_supp_band_pref() checks to further check if band is supported 
   * based on target capability and user preference, e.g., NV441. 
   */ 
  if ( sd_misc_is_target_supp_mode_band_class( 
         SD_SS_MODE_PREF_HDR, band ) &&
       ( sd_misc_get_supp_band_pref() &
#ifdef FEATURE_CMI
         sd_map_band_to_band_pref((sd_band_e_type)band)
#else
         sdss_map_band_to_band_pref((sd_band_e_type)band)
#endif
       ) )
  {
    HDR_MSG_PROT_1( MSG_LEGACY_LOW, "band class %d is supported", band );
    return TRUE;
  }
  else
  {
    HDR_MSG_PROT_1( MSG_LEGACY_HIGH, "band class %d is not supported", band );
    return FALSE;
  }
} /* hdrutil_is_band_supported() */

/* <EJECT> */
/*============================================================================

FUNCTION HDRUTIL_REMOVE_INVALID_NEIGHBORS

DESCRIPTION
This function scans through the neighbor list and removes all entries
corresponding to band class not supported by AT

DEPENDENCIES
None

PARAMETERS
  nset_ptr - record of neighbor set pilots; from which invalid band class
             neighbors are to be removed
  ncnt - count of number of neighbors
  avoided_chan_ptr - Pointer to avoided channel list 
  num_avoided_channels - Number of channels currently avoided by SD


RETURN VALUE
TRUE if any (invalid) neighbor entry is removed, FALSE otherwise

SIDE EFFECTS
None

============================================================================*/
boolean hdrutil_remove_invalid_neighbors
( 
  hdrsrch_nset_pilot_type *nset_ptr,
  uint8 *ncnt,
  const sys_avoided_channel_record_type *avoided_chan_ptr,       
  const uint8 num_avoided_channels
)
{
  boolean nghbrs_removed = FALSE;  /* Result of remove_invalid_neighbors */
  boolean nghbrs_avoided = FALSE;  /* Neighbor is in the SD avoided list */
  int cnt = 0;                     /* index counter */
  int index = 0;                   /* secondary index counter */
  int avoided_id = 0;              /* index counter */
  uint8 nghbr_cnt = *ncnt;         /* count of number of neighbors */
 
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - --*/

  /* Consider SystemType 0x01 and 0x02 as invalid neighbors
   * TODO: in later phase, the AT may support 0x02
   */
  while ( cnt < nghbr_cnt )
  {
    /* Remove neighbor if it's currently avoided by SD*/
    nghbrs_avoided = FALSE; /* reset flag for each neighbor*/
    for (avoided_id = 0; avoided_id < num_avoided_channels; avoided_id++)
    {
      if ((nset_ptr[cnt].link.channel.band == 
           avoided_chan_ptr[avoided_id].channel.band) &&
          (nset_ptr[cnt].link.channel.chan_num  == 
           avoided_chan_ptr[avoided_id].channel.chan_num))
      {
        HDR_MSG_PROT_3(
        MSG_LEGACY_HIGH, 
        "remove_avoided_neighbors Ch = %d BC = %d Pilot_pn = %d", 
        nset_ptr[cnt].link.channel.chan_num, nset_ptr[cnt].link.channel.band,
        nset_ptr[cnt].link.pilot_pn );

        index = cnt;
        while ( index < (nghbr_cnt-1) )
        {
          nset_ptr[index] = nset_ptr[index+1];
          index++;
        }
        nghbr_cnt--;
        nghbrs_removed = TRUE;
        nghbrs_avoided = TRUE;

        break;
      }
    }

    /* If this channel is not in the avoided list, continue other checks with
     * current channel.
     * otherwise, go to next channel (without increasing cnt counter becasue we 
     * already move next channel up one positioni)
     */
    if (!nghbrs_avoided)
    {
            /* if band class not supported (reported by SD), OR */
      if ( (!hdrutil_is_band_supported(nset_ptr[cnt].link.channel.band)) ||
            /* if band/chan not supported (reported by RF), OR */
           (!hdrrup_is_band_chan_supported(&nset_ptr[cnt].link.channel)) ||
           (nset_ptr[cnt].link.mode != SYS_HDR_CHAN_REC_MODE_IS856 ) )
      {
        HDR_MSG_PROT_3(
        MSG_LEGACY_HIGH, 
        "remove_invalid_neighbors Ch = %d BC = %d Pilot_pn = %d", 
        nset_ptr[cnt].link.channel.chan_num, nset_ptr[cnt].link.channel.band,
        nset_ptr[cnt].link.pilot_pn );
                     
        index = cnt;
        while ( index < (nghbr_cnt-1) )
        {
          nset_ptr[index] = nset_ptr[index+1];
          index++;
        }
        nghbr_cnt--;
        nghbrs_removed = TRUE;
      }
      else 
      {
        cnt++;
      }
    }

  }

  *ncnt = nghbr_cnt;

  return nghbrs_removed;
} /* hdrutil_remove_invalid_neighbors */

/* <EJECT> */
/*=========================================================================

FUNCTION HDRUTIL_CALC_DURATION

DESCRIPTION  
  Calculate the duration between two time stamps.
               
DEPENDENCIES 
  None

RETURN VALUE 
  The duration in ms between the end and start time.

SIDE EFFECTS 
  None

=========================================================================*/

uint64 hdrutil_calc_duration 
(
  PACKED time_type start_time,
  PACKED time_type end_time
)
{
  time_type up_start_time; /* Unpacked start time. */
  time_type up_end_time;   /* Unpacked end time. */
   
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Convert to unpacked variables. */
  qw_equ( up_start_time, start_time );
  qw_equ( up_end_time, end_time );

  /* Now convert to milliseconds and calculate the duration. */
  return ( time_to_ms_native( up_end_time ) - 
           time_to_ms_native( up_start_time ) );

} /* hdrutil_calc_duration */

/* <EJECT> */
/*============================================================================

FUNCTION HDRUTIL_NEW_BUFFER

DESCRIPTION
  This function acts as a wrapper for dsm_offset_new_buffer; in the event the 
  dsm item pool returns NULL, this function will catch it and exit gracefully.

DEPENDENCIES
  None

PARAMETERS
  pool_id	- a dsm_mempool_id_enum_type
  offset - a predefined header size 

RETURN VALUE
  A pointer to the buffer item or nothing if the pool is empty.

SIDE EFFECTS
  None

============================================================================*/
dsm_item_type *hdrutil_new_buffer
(
  dsm_mempool_id_enum_type pool_id,
  uint16 offset
)
{
  dsm_item_type *p_dsm_item;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  p_dsm_item = dsm_offset_new_buffer(pool_id, offset);
  
  /* check to see if the pool's empty */
  if ( p_dsm_item == NULL )
  {
    ERR_FATAL("Out of DSM items in memory pool.", 0, 0, 0);
  }
  
  return p_dsm_item;
} /* hdrutil_new_buffer */

#ifdef FEATURE_HDRUTIL_DEBUG
/*===========================================================================

FUNCTION HDRUTIL_STRIP_FILENAME

DESCRIPTION
  <enter description here>

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static char *hdrutil_strip_filename( char *file )
{
  int len = 0;

  while( file[len] )
  {
    len += 1;
  }
  len += 1; // add the '\0' character

  while( file[len] != '\\' && len > 0 )
  {
    len -= 1;
  }
  if( len > 0 )
  {
    len += 1; // remove the actual '\' character
  }

  return file + len;
}
#endif /* FEATURE_HDRUTIL_DEBUG */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRUTIL_NEW_MSG_BUFFER

DESCRIPTION
  This function allocates a DSM item for new HDR signaling messages to be 
  transmitted and also PCP headers (if needed by dsm_pushdown).  It uses a 
  dedicated DSM item pool as well as a fixed offset that leaves room for the 
  protocol headers that are added as the message is passed down the stack.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  A pointer to the buffer item. If the pool is empty, the software will
  halt and throw an ERR_FATAL.

SIDE EFFECTS
  None.

===========================================================================*/
dsm_item_type *hdrutil_new_msg_buffer_ext
(
  void *file,
  void *line
)
{
  dsm_item_type *dsm_item_ptr = NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifndef FEATURE_HDRUTIL_DEBUG
  SYS_ARG_NOT_USED(file);
  SYS_ARG_NOT_USED(line);
  #endif /* FEATURE_HDRUTIL_DEBUG */

  #ifdef FEATURE_HDRUTIL_DEBUG
  HDR_MSG_SPRINTF_2( MSG_LEGACY_HIGH, "hdrutil_new_msg() called from: %s, %d", 
                     hdrutil_strip_filename( (char *) file ), line );
  #endif /* FEATURE_HDRUTIL_DEBUG */

  dsm_item_ptr = dsm_offset_new_buffer( DSM_HDR_MSG_ITEM_POOL,
                                        HDRUTIL_MAX_MSG_HEADER_SIZE );

  #ifdef FEATURE_HDRUTIL_DEBUG
  HDR_MSG_PROT_1( MSG_LEGACY_HIGH, "%d items left in HDR messaging pool",
                  DSM_POOL_FREE_CNT( DSM_HDR_MSG_ITEM_POOL ) );
  #endif /* FEATURE_HDRUTIL_DEBUG */

  /* Sanity check to ensure the HDR message item pool is nonempty */
  if( dsm_item_ptr == NULL )
  {
    HDR_MSG_PROT( MSG_LEGACY_HIGH, 
      "Out of DSM items in HDR_MSG_ITEM pool. " \
      "Falling back to DS_SMALL_ITEM pool" );
    
    /* Fall back to the DS small item pool */
    dsm_item_ptr = dsm_offset_new_buffer( DSM_DS_SMALL_ITEM_POOL,
                                          HDRUTIL_MAX_MSG_HEADER_SIZE );

    /* Halt the software if the fall back memory pool is empty */
    if( dsm_item_ptr == NULL )
    {
      ERR_FATAL( "Out of DSM items in DS_SMALL_ITEM pool", 0, 0, 0 );
    }
  }
  
  return dsm_item_ptr;
} /* hdrutil_new_msg_buffer() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRUTIL_NEW_DSM_BUFFER

DESCRIPTION
  This function allocates a new DSM item from the HDR messaging pool.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  A pointer to the buffer item. If the pool is empty, the software will
  halt and throw an ERR_FATAL.

SIDE EFFECTS
  None.

===========================================================================*/
dsm_item_type *hdrutil_new_dsm_buffer_ext
(
  void *file,
  void *line
)
{
  dsm_item_type *dsm_item_ptr = NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifndef FEATURE_HDRUTIL_DEBUG
  SYS_ARG_NOT_USED(file);
  SYS_ARG_NOT_USED(line);
  #endif /* FEATURE_HDRUTIL_DEBUG */

  #ifdef FEATURE_HDRUTIL_DEBUG
  HDR_MSG_SPRINTF_2( MSG_LEGACY_HIGH, "hdrutil_new_dsm_buffer() called from: %s, %d", 
                     hdrutil_strip_filename( (char *) file ), line );
  #endif /* FEATURE_HDRUTIL_DEBUG */

  dsm_item_ptr = dsm_new_buffer( DSM_HDR_MSG_ITEM_POOL );

  #ifdef FEATURE_HDRUTIL_DEBUG
  HDR_MSG_PROT_1( MSG_LEGACY_HIGH, "%d items left in HDR messaging pool", 
                  DSM_POOL_FREE_CNT( DSM_HDR_MSG_ITEM_POOL ) );
  #endif /* FEATURE_HDRUTIL_DEBUG */

  /* Sanity check to ensure the HDR message item pool is nonempty */
  if( dsm_item_ptr == NULL )
  {
    HDR_MSG_PROT( MSG_LEGACY_HIGH, 
      "Out of DSM items in HDR_MSG_ITEM pool. " \
      "Falling back to DS_SMALL_ITEM pool" );
    
    /* Fall back to the DS small item pool */
    dsm_item_ptr = dsm_new_buffer( DSM_DS_SMALL_ITEM_POOL );

    /* Halt the software if the fall back memory pool is empty */
    if( dsm_item_ptr == NULL )
    {
      ERR_FATAL( "Out of DSM items in DS_SMALL_ITEM pool", 0, 0, 0 );
    }
  }
  
  return dsm_item_ptr;
} /* hdrutil_new_dsm_buffer() */

/*===========================================================================

FUNCTION HDRUTIL_PUSHDOWN_HEAD

DESCRIPTION
  This function acts as an intelligent wrapper to the dsm_pushdown() routine.
  The routine will attempt to pushdown from the private HDR dsm item pool, and 
  if such an attempt fails it will fallback to the standard DS small item pool.
  If there is a mismatch during execution, the routine will halt with an
  ERR_FATAL. The use of two pools reduces the risk of having an application
  consume all available small items from the DS small item pool and causing
  unexpected or undefined behavior throughout the code.

DEPENDENCIES
  None.

PARAMETERS
  dsm_item_ptr - Pointer to the DSM item being modified.
  data_ptr - Pointer to the data to be inserted.
  data_size - Size of the data to be inserted.

RETURN VALUE
  The number of bytes successfully pushed down.

SIDE EFFECTS
  None.

===========================================================================*/
uint16 hdrutil_pushdown_head_ext
(
  dsm_item_type **dsm_item_ptr,
  void *data_ptr,
  uint16 data_size,
  void *file,
  void *line
)
{
  uint16 pushed_size = 0;
  uint16 pulled_size = 0;

  #ifdef FEATURE_HDRUTIL_DEBUG
  uint32 pre_push = 0;
  uint32 post_push = 0;
  #endif /* FEATURE_HDRUTIL_DEBUG */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifndef FEATURE_HDRUTIL_DEBUG
  SYS_ARG_NOT_USED(file);
  SYS_ARG_NOT_USED(line);
  #endif /* FEATURE_HDRUTIL_DEBUG */

  #ifdef FEATURE_HDRUTIL_DEBUG
  HDR_MSG_SPRINTF_2( MSG_LEGACY_HIGH, "hdrutil_pushdown_head() called from: %s, %d", 
                     hdrutil_strip_filename( (char *) file ), line );

  pre_push = DSM_POOL_FREE_CNT( DSM_HDR_MSG_ITEM_POOL );
  if( pre_push == 0 )
  {
    HDR_MSG_PROT( MSG_LEGACY_ERROR, "HDR messaging pool is empty!" );
  }
  #endif /* FEATURE_HDRUTIL_DEBUG */

  pushed_size = dsm_pushdown_packed( dsm_item_ptr, data_ptr, data_size, 
                              DSM_HDR_MSG_ITEM_POOL );

  #ifdef FEATURE_HDRUTIL_DEBUG
  post_push = DSM_POOL_FREE_CNT(DSM_HDR_MSG_ITEM_POOL);
  if( post_push != pre_push )
  {
    HDR_MSG_PROT_1( MSG_LEGACY_HIGH, 
                    "hdrutil_pushdown_head() took new item, %d items left in pool", 
                    post_push );
  }
  #endif /* FEATURE_HDRUTIL_DEBUG */

  if( pushed_size != data_size )
  {
    pulled_size = dsm_pullup( dsm_item_ptr, NULL, pushed_size );
    if( pulled_size != pushed_size )
    {
      ERR_FATAL( "DSM routine malfunction", 0, 0, 0 );
    }
    
    pushed_size = dsm_pushdown_packed( dsm_item_ptr, data_ptr, data_size, 
                                DSM_DS_SMALL_ITEM_POOL );

    if( pushed_size != data_size )
    {
      /*lint --e{527} suppress code unreachable */
      ERR_FATAL(" Running out of small buffers, small_free_cnt:%d, lar_free_cnt:%d", 
              DSM_POOL_FREE_CNT(DSM_DS_SMALL_ITEM_POOL) ,
              DSM_POOL_FREE_CNT(DSM_DS_LARGE_ITEM_POOL), 0);
      
      pushed_size = dsm_pushdown_packed( dsm_item_ptr, data_ptr, data_size,
                                  DSM_DS_LARGE_ITEM_POOL );
      if( pushed_size == 0 )
      {
        ERR(" Running out of buffers, small_free_cnt:%d, lar_free_cnt:%d", 
              DSM_POOL_FREE_CNT(DSM_DS_SMALL_ITEM_POOL) ,
              DSM_POOL_FREE_CNT(DSM_DS_LARGE_ITEM_POOL), 0);
      }
      
    }
    else
    {
      HDR_MSG_PROT( MSG_LEGACY_HIGH, 
      "Out of DSM items in HDR_MSG_ITEM pool. " \
      "Falling back to DS_SMALL_ITEM pool - PUSHDOWN_HEAD" );
    }

  }

  return pushed_size;
} /* hdrutil_pushdown_head() */

/*===========================================================================

FUNCTION HDRUTIL_PUSHDOWN

DESCRIPTION
  This function acts as an intelligent wrapper to the dsm_pushdown() routine.
  The routine will attempt to pushdown from the private HDR dsm item pool, and 
  if such an attempt fails it will fallback to the standard DS small item pool.
  If there is a mismatch during execution, the routine will halt with an
  ERR_FATAL. The use of two pools reduces the risk of having an application
  consume all available small items from the DS small item pool and causing
  unexpected or undefined behavior throughout the code.

DEPENDENCIES
  None.

PARAMETERS
  dsm_item_ptr - Pointer to the DSM item being modified.
  data_ptr - Pointer to the data to be inserted.
  data_size - Size of the data to be inserted.

RETURN VALUE
  The number of bytes successfully pushed down.

SIDE EFFECTS
  None.

===========================================================================*/
uint16 hdrutil_pushdown_ext
(
  dsm_item_type **dsm_item_ptr,
  void *data_ptr,
  uint16 data_size,
  void *file,
  void *line
)
{
  uint16 pushed_size = 0;
  uint16 pulled_size = 0;

  #ifdef FEATURE_HDRUTIL_DEBUG
  uint32 pre_push = 0;
  uint32 post_push = 0;
  #endif /* FEATURE_HDRUTIL_DEBUG */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifndef FEATURE_HDRUTIL_DEBUG
  SYS_ARG_NOT_USED(file);
  SYS_ARG_NOT_USED(line);
  #endif /* FEATURE_HDRUTIL_DEBUG */

  #ifdef FEATURE_HDRUTIL_DEBUG
  HDR_MSG_SPRINTF_2( MSG_LEGACY_HIGH, "hdrutil_pushdown() called from: %s, %d", 
                     hdrutil_strip_filename( (char *) file ), line );

  pre_push = DSM_POOL_FREE_CNT( DSM_HDR_MSG_ITEM_POOL );
  if( pre_push == 0 )
  {
    HDR_MSG_PROT( MSG_LEGACY_ERROR, "HDR messaging pool is empty!" );
  }
  #endif /* FEATURE_HDRUTIL_DEBUG */

  pushed_size = dsm_pushdown( dsm_item_ptr, data_ptr, data_size, 
                              DSM_HDR_MSG_ITEM_POOL );

  #ifdef FEATURE_HDRUTIL_DEBUG
  post_push = DSM_POOL_FREE_CNT(DSM_HDR_MSG_ITEM_POOL);
  if( post_push != pre_push )
  {
    HDR_MSG_PROT_1( MSG_LEGACY_HIGH, 
                    "hdrutil_pushdown() took new item, %d items left in pool", 
                    post_push );
  }
  #endif /* FEATURE_HDRUTIL_DEBUG */

  if( pushed_size != data_size )
  {
    pulled_size = dsm_pullup( dsm_item_ptr, NULL, pushed_size );
    if( pulled_size != pushed_size )
    {
      ERR_FATAL( "DSM routine malfunction", 0, 0, 0 );
    }
    
    pushed_size = dsm_pushdown( dsm_item_ptr, data_ptr, data_size, 
                                DSM_DS_SMALL_ITEM_POOL );

    if( pushed_size != data_size )
    {
      /*lint --e{527} suppress code unreachable */
      ERR_FATAL(" Running out of small buffers, small_free_cnt:%d, lar_free_cnt:%d", 
              DSM_POOL_FREE_CNT(DSM_DS_SMALL_ITEM_POOL) ,
              DSM_POOL_FREE_CNT(DSM_DS_LARGE_ITEM_POOL), 0);
      
      pushed_size = dsm_pushdown( dsm_item_ptr, data_ptr, data_size,
                                  DSM_DS_LARGE_ITEM_POOL );
      if( pushed_size == 0 )
      {
        ERR(" Running out of buffers, small_free_cnt:%d, lar_free_cnt:%d", 
              DSM_POOL_FREE_CNT(DSM_DS_SMALL_ITEM_POOL) ,
              DSM_POOL_FREE_CNT(DSM_DS_LARGE_ITEM_POOL), 0);
      }
      
    }
    else
    {
      HDR_MSG_PROT( MSG_LEGACY_HIGH, 
      "Out of DSM items in HDR_MSG_ITEM pool. " \
      "Falling back to DS_SMALL_ITEM pool - PUSHDOWN" );
    }

  }

  return pushed_size;
} /* hdrutil_pushdown() */

/*===========================================================================

FUNCTION HDRUTIL_PUSHDOWN_TAIL

DESCRIPTION
  This function acts as an intelligent wrapper to the dsm_pushdown_tail() 
  routine. The routine will attempt to pushdown from the private HDR dsm item 
  pool, and if such an attempt fails it will fallback to the standard DS small 
  item pool. If there is a mismatch during execution, the routine will halt 
  with an ERR_FATAL. The use of two pools reduces the risk of having an 
  application consume all available small items from the DS small item pool 
  and causing unexpected or undefined behavior throughout the code.

DEPENDENCIES
  None.

PARAMETERS
  dsm_item_ptr - Pointer to the DSM item being modified.
  data_ptr - Pointer to the data to be inserted.
  data_size - Size of the data to be inserted.  

RETURN VALUE
  The number of bytes successfully pushed down.

SIDE EFFECTS
  None.

===========================================================================*/
uint16 hdrutil_pushdown_tail_ext
(
  dsm_item_type **dsm_item_ptr,
  void *data_ptr,
  uint16 data_size,
  void *file,
  void *line
)
{
  uint16 pushed_size = 0;
  uint16 pulled_size = 0;

  #ifdef FEATURE_HDRUTIL_DEBUG
  uint32 pre_push = 0;
  uint32 post_push = 0;
  #endif /* FEATURE_HDRUTIL_DEBUG */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifndef FEATURE_HDRUTIL_DEBUG
  SYS_ARG_NOT_USED(file);
  SYS_ARG_NOT_USED(line);
  #endif /* FEATURE_HDRUTIL_DEBUG */

  #ifdef FEATURE_HDRUTIL_DEBUG
  HDR_MSG_SPRINTF_2( MSG_LEGACY_HIGH, "hdrutil_pushdown_tail() called from: %s, %d", 
                     hdrutil_strip_filename( (char *) file ), line );

  pre_push = DSM_POOL_FREE_CNT( DSM_HDR_MSG_ITEM_POOL );
  if( pre_push == 0 )
  {
    HDR_MSG_PROT(MSG_LEGACY_ERROR, "HDR messaging pool is empty!");
  }
  #endif /* FEATURE_HDRUTIL_DEBUG */

  pushed_size = dsm_pushdown_tail( dsm_item_ptr, data_ptr, data_size, 
                                   DSM_HDR_MSG_ITEM_POOL );

  #ifdef FEATURE_HDRUTIL_DEBUG
  post_push = DSM_POOL_FREE_CNT(DSM_HDR_MSG_ITEM_POOL);
  if( post_push != pre_push )
  {
    HDR_MSG_PROT_1( MSG_LEGACY_HIGH, 
                    "hdrutil_pushdown_tail() took new item, %d items left in pool", 
                    post_push);
  }  
  #endif /* FEATURE_HDRUTIL_DEBUG */

  if( pushed_size != data_size )
  {
    pulled_size = dsm_pullup_tail( dsm_item_ptr, NULL, pushed_size );
    if( pulled_size != pushed_size )
    {
      ERR_FATAL( "DSM routine malfunction", 0, 0, 0 );
    }
    
    pushed_size = dsm_pushdown_tail( dsm_item_ptr, data_ptr, data_size, 
                                     DSM_DS_SMALL_ITEM_POOL );

    if( pushed_size != data_size )
    {
      ERR_FATAL( "Memory pools exhausted", 0, 0, 0 );
    }
    else
    {
      HDR_MSG_PROT( MSG_LEGACY_HIGH, 
      "Out of DSM items in HDR_MSG_ITEM pool. " \
      "Falling back to DS_SMALL_ITEM pool - PUSHDOWN_TAIL" );
    }

  }
  
  return pushed_size;
} /* hdrutil_pushdown_tail() */

/*===========================================================================

FUNCTION HDRUTIL_BYTE_TO_DSM

DESCRIPTION
  This function converts a byte stream to a packet (dsm item).
  
DEPENDENCIES
  None.

PARAMETERS
  buf - pointer to input buffer
  buf_len - length of input buffer

RETURN VALUE 
  pointer to the packet.

SIDE EFFECTS
  None.

===========================================================================*/
dsm_item_type * hdrutil_byte_to_dsm
( 
  uint8 *buf,
  uint32 buf_len 
)
{
  /* Pointer to the new packet */
  dsm_item_type *item_ptr;  

  /* Number of bytes converted. */
  uint32 length = 0;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  item_ptr = hdrutil_new_msg_buffer();

  /* Convert from a byte stream to a packet. */
  length = (uint32)hdrutil_pushdown_tail( &item_ptr, buf, buf_len );

  if ( length != buf_len ) {
    ERR_FATAL( "Memory pools exhausted, requested %d, converted %d", 
               buf_len, length, 0 );
  }
 
  return item_ptr;
} /* hdrutil_byte_to_dsm */

/*===========================================================================

FUNCTION HDRUTIL_DSM_TO_BYTE

DESCRIPTION
  This function converts a packet (dsm item) to byte stream.
  
DEPENDENCIES
  None.

PARAMETERS
  dsm_item_ptr - pointer to pointer of the input packet to convert
  buf - pointer to output buffer
  max_buf_len - maximum length of the buffer 
  buf_len - pointer to the size of output buffer

RETURN VALUE 
  E_SUCCESS - convert successfully
  E_NO_MEMORY - maximum allowed buffer size is smaller than the packet size

SIDE EFFECTS
  None.

===========================================================================*/
errno_enum_type hdrutil_dsm_to_byte
( 
  dsm_item_type **dsm_item_ptr,
  uint8 *buf,
  uint32 max_buf_len, 
  uint32 *buf_len
)
{
  /* Length of packet's data buffer. */
  uint32 pkt_length = 0;
  /* Number of bytes converted from the packet. */
  uint32 length = 0;

  /* Retrieve the length of buffer chain in dsm item in term of bytes. */
  pkt_length = dsm_length_packet(*dsm_item_ptr);

  if ( max_buf_len < pkt_length )
  {
    ERR( "Not enough buffer to hold the packet",0 ,0 ,0 );
    return E_NO_MEMORY;
  }

  /* Place the entire packet on the buffer. */
  length = (uint32)dsm_pullup( dsm_item_ptr, buf, pkt_length );

  *buf_len = length;

  if (length != pkt_length) {
    ERR_FATAL( "DSM routine malfunction", 0, 0, 0 );
  }

  /* Free the DSM item pointer. Acutally it should have released in dsm_pullup. */
  dsm_free_packet(dsm_item_ptr);

  return E_SUCCESS;

} /* hdrutil_dsm_to_byte */

/* <EJECT> */
/*============================================================================

FUNCTION HDRUTIL_SCI_TO_NS

DESCRIPTION
  This function converts a Slot Cycle Index to a sleep duration in 
  nanoseconds.

DEPENDENCIES
  None

PARAMETERS
  sci - The Slot Cycle Index to convert to nanoseconds (range of 0 - 31)

RETURN VALUE
  Number of nanoseconds for the given slot cycle index, or zero if the given
  SCI is out of range.

SIDE EFFECTS
  None

============================================================================*/
uint64 hdrutil_sci_to_ns
(
  uint8 sci
)
{
  uint64 slots; /* Number of slots in one cycle at given SCI */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* SCI 0-6: 2^sci * 4 slots
   * SCI 7-31: 2^(sci - 7) * 768 slots
   */

  if (sci < 7)
  {
    slots = (((uint64)1 << sci) * 4);
  }
  else if (sci < 32)
  {
    /* 2^24 * 768 * 1666 is still only about 42 bits :) */
    slots = (((uint64)1 << (sci - 7)) * 768);
  }
  else
  {
    /* SCI was out of range */
    return 0;
  }

  return (slots * 1666);

} /* hdrutil_sci_to_ns() */

/*===========================================================================

FUNCTION HDRUTIL_BIT_COUNT16

DESCRIPTION
  This function counts the number of 1 bits in a 16 bit field.

DEPENDENCIES
  None.

PARAMETERS
  bitfield - The 16 bit field to be counted.

RETURN VALUE
  The number of 1 bits in the 16 bit field.

SIDE EFFECTS
  None.

===========================================================================*/
int hdrutil_bit_count16
(
  uint16 bitfield
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  bitfield = ( bitfield & 0x5555 ) + ( ( bitfield >> 1 ) & 0x5555 );
  bitfield = ( bitfield & 0x3333 ) + ( ( bitfield >> 2 ) & 0x3333 );
  bitfield = ( bitfield & 0x0f0f ) + ( ( bitfield >> 4 ) & 0x0f0f );
  bitfield = ( bitfield & 0x00ff ) + ( ( bitfield >> 8 ) & 0x00ff );

  return bitfield;

} /* hdrutil_bit_count16() */


/*===========================================================================

FUNCTION HDRUTIL_BIT_COUNT32

DESCRIPTION
  This function counts the number of bits set in an unsigned int.

DEPENDENCIES
  None.

PARAMETERS
  bitfield - The unsigned int

RETURN VALUE
  The number of set bits in the 32 bit field.

SIDE EFFECTS
  None.

===========================================================================*/
uint32 hdrutil_bit_count32
(
  uint32 bitfield
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Number of bits set in a 1-bit field is its bit value (0 or 1) */

  /* Add bitcounts stored in each 1-bit within each two bits and store the 
     result in the same two bits. */
  bitfield = ( bitfield & 0x55555555 ) + ( ( bitfield >> 1 ) & 0x55555555 );

  /* Add bitcounts stored in each 2-bits with in a Nibble and store the result
     in the same Nibble */
  bitfield = ( bitfield & 0x33333333 ) + ( ( bitfield >> 2 ) & 0x33333333 );

  /* Add bitcounts stored in each Nibble with in a byte and store the result in 
     the same byte */
  bitfield = ( bitfield & 0x0F0F0F0F ) + ( ( bitfield >> 4 ) & 0x0F0F0F0F );

  /* Add the counts stored in each byte to get the final bit count */
  bitfield = ( bitfield & 0x000000FF ) +
             ( ( bitfield >> 8 )  & 0x000000FF ) +
             ( ( bitfield >> 16 ) & 0x000000FF ) +
             ( ( bitfield >> 24 ) & 0x000000FF );

  return bitfield;

} /* hdrutil_bit_count32 */


/*===========================================================================

FUNCTION HDRUTIL_UNPACK_CHANNEL_RECORD

DESCRIPTION
  This function unpacks a channel record from a given message.

DEPENDENCIES
  None.

PARAMETERS
  item_ptr         - The message containing the channel record.
  offset_ptr       - The offset to begin unpacking the channel record.
  sys_type_ptr     - A pointer to hold the system type of the unpacked
                     channel record.
  channel_list_ptr - A pointer to the structure that holds the extracted 
                     channel record.

RETURN VALUE
  E_SUCCESS if the extraction is successful; non-zero if the extraction
  failed.

SIDE EFFECTS
  The offset_ptr will be updated to the end of the channel record block.

===========================================================================*/
errno_enum_type hdrutil_unpack_channel_record
(
  dsm_item_type    *const item_ptr,       /* IN */
  uint16           *const offset_ptr,     /* IN/OUT */
  uint8            *const sys_type_ptr,   /* OUT */
  sys_channel_type *const channel_rec_ptr /* OUT */
)
{
  /* Temp variable to hold the current channel record's system type */
  uint8 system_type;

  /* Temp variable to track the current offset into the message */
  uint16 field_offset = *offset_ptr;

  /* Temp variable to hold the current channel record's band class */
  uint8 band_class;

  /* Initialize return value */
  int err = 0;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Unpack the system type */
  err |= (int) HDRUTIL_UNPACK_SYSTEM_TYPE( 
                 item_ptr, &system_type, field_offset );
  field_offset += HDRUTIL_SYSTEM_TYPE_SIZE;

  /* Write out the system type */
  *sys_type_ptr = system_type;

  /* Unpack the band class */
  err |= (int) HDRUTIL_UNPACK_BAND_CLASS( 
                 item_ptr, &band_class, field_offset );
  field_offset += HDRUTIL_BAND_CLASS_SIZE;

  if( (sys_band_class_e_type) band_class >= SYS_BAND_CLASS_MAX )
  {
    ERR( "band_class out of range: %d", band_class, 0, 0 );
    err |= E_NO_DATA;
  }

  /* Store the band class */
  channel_rec_ptr->band = (sys_band_class_e_type) band_class;

  /* Unpack the channel number directly into the record */
  err |= (int) HDRUTIL_UNPACK_CHANNEL_NUMBER( item_ptr,
           &channel_rec_ptr->chan_num,
           field_offset );
  field_offset += HDRUTIL_CHANNEL_NUMBER_SIZE;

  /* Update the offset */
  *offset_ptr = field_offset;
  if( err )
  {
    return E_DATA_INVALID;
  }
  else
  {
    return E_SUCCESS;
  }

} /* hdrutil_unpack_channel_record() */

/*===========================================================================

FUNCTION HDRUTIL_PRETTY_PRINT_BYTE_ARRAY

DESCRIPTION
  This function pretty prints a byte array as an F3 message.

DEPENDENCIES
  None.

PARAMETERS
  msg_string_ptr - An informational message to be displayed, passed in
    as a double quoted string constant.
  byte_array - The array holding the bytes to be printed.
  size - The size of the array holding the bytes to be printed.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrutil_pretty_print_byte_array
(
  void *msg_string_ptr,
  uint8 *byte_array,
  uint32 size
)
{
  uint8 char_array[34]; /* holds 16 bytes of information 
                           including NULL terminator */

  uint32 byte_pos = 0;
  int char_pos = 0;

  int i;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  while( byte_pos < size )
  {
    if( byte_array[byte_pos] > 0x0F )
    {
      char_array[char_pos] = (byte_array[byte_pos] & 0xF0) >> 4;
    }
    else
    {
      char_array[char_pos] = 0x00;
    }

    char_pos++;
    char_array[char_pos] = byte_array[byte_pos] & 0x0F;
    char_pos++;
    byte_pos++;

    if( char_pos >= 32 )
    {
      char_pos = 32;
      break; /* prevent overflow of char_array[34] and leave room for
                terminating NULL */
    }
  }

  char_array[char_pos] = '\0';

  for( i = 0; i < char_pos; i++ )
  {
    if( char_array[i] >= 0x0A && char_array[i] <= 0x0F )
    {
      char_array[i] += 0x37;
    }
    else
    {
      char_array[i] += 0x30;
    }
  }

  if( msg_string_ptr != NULL )
  {
    HDR_MSG_SPRINTF_3( MSG_LEGACY_HIGH, 
      "%s; dumped %d bytes: 0x%s", msg_string_ptr, size, char_array );
  }
  else
  {
    HDR_MSG_SPRINTF_2( MSG_LEGACY_HIGH,
      "Byte dump of %d bytes: 0x%s", size, char_array );
  }
  
} /* hdrutil_pretty_print_byte_array() */


/*===========================================================================

FUNCTION HDRUTIL_GET_RANDOM()

DESCRIPTION
  This function returns a random number. This is a wrap up function for
  secapi_get_random.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
dword  hdrutil_get_random ( void )
{
  dword               random_value = 0;
    /* random number for persist checking */

  secerrno_enum_type  sec_errno = E_FAILURE;
    /* return value of random number generate API */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  sec_errno = secapi_get_random( SECAPI_SECURE_RANDOM, 
                                 (uint8 *)&random_value, 
                                 sizeof(random_value) );  
  if(sec_errno != E_SUCCESS)
  {
    ERR_FATAL("HDR: Error generating random numbers %d", sec_errno,0,0 );
  }
  else
  {
    return random_value;
  }

} /* hdrutil_get_random */

/*===========================================================================

FUNCTION HDRUTIL_RESET_NV_ERROR

DESCRIPTION
  This function rests the NV error flag. Used for tracking if there is 
  any NV error during a specific operation.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrutil_reset_nv_error(void)
{
   hdrutil_nv_error = FALSE;
}

/*===========================================================================

FUNCTION HDRUTIL_GET_NV_ERROR

DESCRIPTION
  This function retrieves the NV error flag. Used for tracking if there is 
  any NV error during a specific operation.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  TRUE - if there is any NV error from the last time it is reset.
  FALSE - otherwise

SIDE EFFECTS
  None.

===========================================================================*/
boolean hdrutil_get_nv_error(void)
{
   return hdrutil_nv_error;     
}
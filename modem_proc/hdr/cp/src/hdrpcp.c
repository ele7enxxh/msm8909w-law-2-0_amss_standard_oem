/*===========================================================================

      H D R   P A C K E T   C O N S O L I D A T I O N   P R O T O C O L
                                 M O D U L E

DESCRIPTION
  This file contains the implementation of the 1x-EVDO Release B Packet 
  Consolidation Protocol.

  Copyright (c) 2007 - 2016 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

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

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdrpcp.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
05/16/16   vko     Reduce excessive F3
01/22/14   vko     Fixed buffer overflow problem in data size for mac flow
04/18/14   cnx     Move tx failure callbacks out of TX critical section.
04/16/14   cnx     Do not queue RLP signaling until connection is fully estabilished.
12/10/13   sat     Changed memcpy() to assignment or memscpy() (safer version).
10/29/13   sat     Reduced F3 messages (or/and) ASSERTs
04/04/13   cnx     Deregister roll callback when connection is closed.
11/24/12   ukl     Fix Klocwork issues
04/30/12   cnx     Free the DSM item if duplicating packet fails.
01/05/12   sju     Check trans id when SAPTX receives ACK for UL messages.
01/03/12   sju     Added support to enable verbose debug f3 at runtime. 
10/28/11   pba     Optimized handoff phase 2 support
09/30/11   cnx     Changed %P in F3 to 0x%x.
08/08/11   wsh     Fixed race cond. in RMAC shutdown, created RX/TX crit_sec
07/27/11   cnx     Merged Optimized Handoff changes.
06/22/10   wsh     Fixed race condition when reg/unreg PCP flows
02/26/10   wsh     Fixed lint warnings 
02/19/10   wsh     Fixed crash in PCP when MAC packet is malformed
02/09/10   wsh     New API to notify RLP RX buffer empty
09/30/09   wsh     Fixed KW warnings
09/18/09   wsh     CMI: support changes from modem DATA
08/13/09   wsh     Fixed not freeing MAC flows when unregistered
03/26/03   wsh     Moved subframe_count to hdrpcp_get_packet()
12/10/08   smd     Replaced assert.h with amssassert.h
11/10/08   kss     Removed/replaced intlocks.
11/10/08   smd     Replaced errno.h with IxErrno.h  
10/30/08   smd     Added a type casting to remove off-target build error. 
10/01/08   wsh     Fixed critical/high lint warnings
06/30/08   wsh     PCP should log carriers in same order as RMAC
06/12/08   wsh     PCP not passing MARQ to upper layers
05/27/08   wsh     Fixed crashing caused by PCP logging
01/17/08   hal     Added PCP log packet support
01/14/08   cc      Stubbed hdrpcp_report_prev_subframe_status to accept
                   "subframe_count" argument.
12/05/07   etv     (for hal) Fixed inconsistent carrier indexing.
12/04/07   etv     Copy the app_ptr field in the dup DSM item.
10/16/07   hal     Added fields used by MAC for logging
11/06/07   hal     Changed ASSERT to support stream 3 as valid
09/17/07   hal     Updated UNREGISTER_ALL_SUBSTREAMS handling for TAP
08/06/07   hal     Fixed DORB Phase 1.5 stability issues
                   Added temporary F3 messaging for debugging purposes
08/01/07   hal     Fixed spurious outcome reporting
07/31/07   hal     Moved large local structs to static memory
                   Fixed issue with NULL mac_flows pointer in hdrpcp_get_pkt()
07/30/07   hal     Initialized pkt_ptr in hdrpcp_get_pkt()
                   Mark packets received by hdrpcp_put_pkt() as End-of-Slot
                   Skip unused, free TX flows while reporting status
07/26/07   hal     Changed name of DSM macro
06/27/07   hal     Initial version

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

/*-----------------------------------------------------
  Common AMSS include files
-----------------------------------------------------*/

#include "hdr_variation.h"
#ifndef FEATURE_HDRPCP_UNITTEST
#include "amssassert.h"
#endif /* FEATURE_HDRPCP_UNITTEST */

#include "comdef.h"

#ifndef FEATURE_HDRPCP_UNITTEST
#include "dsm.h"
#include "err.h"
#include "IxErrno.h"
#endif /* FEATURE_HDRPCP_UNITTEST */


/*-----------------------------------------------------
  HDR specific include files
-----------------------------------------------------*/

#ifndef FEATURE_HDRPCP_UNITTEST
#include "hdrdebug.h"
#include "hdrdsm.h"
#include "hdrhai.h"
#include "hdrlogi.h"
#include "hdrmrlp_api.h"
#endif /* FEATURE_HDRPCP_UNITTEST */

#include "hdrpcp.h"
#include "hdrpcp_slp_pkt_app.h"
#include "hdrpcp_queue.h"

#ifndef FEATURE_HDRPCP_UNITTEST
#include "hdrslp.h"
#include "hdrrlp_api.h"
#include "hdrts.h"
#include "hdrutil.h"
#endif /* FEATURE_HDRPCP_UNITTEST */

#include "hdralmp.h"

#ifdef FEATURE_LTE_TO_HDR_OH
#include "hdrsap.h"
#include "hdrsaptx.h"
#endif /* FEATURE_LET_TO_HDR_OH */


/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*-----------------------------------------------------
  Definitions and macros
-----------------------------------------------------*/

/*
 * The stream mask and the individual stream headers; each 
 * packet being transmitted or received by PCP will make use 
 * of these stream headers. The TX path will need to set the 
 * headers while the RX path uses the headers to determine 
 * which packet app will receive the data.
 *
 */
#define HDRPCP_STREAM_MASK          0xC0
#define HDRPCP_STREAM0_HEADER       0x00
#define HDRPCP_STREAM1_HEADER       0x40
#define HDRPCP_STREAM2_HEADER       0x80
#define HDRPCP_STREAM3_HEADER       0xC0
#define HDRPCP_STREAM_ID_SHIFT_BITS 0x06
#define HDRPCP_STREAM_INV_MASK      0x3F

#define HDRPCP_STREAM0_ID           0x00
#define HDRPCP_STREAM1_ID           0x01
#define HDRPCP_STREAM2_ID           0x02
#define HDRPCP_STREAM3_ID           0x03

/*
 * An M-ARQ indication can come up to 13 subframes from the
 * initial tranmission; therefore, PCP needs to maintain at
 * most 13 buffers worth of packet handles for use with the
 * M-ARQ indication.
 *
 */
#define HDRPCP_MAX_MARQ_BUFFERS  13
#define HDRPCP_MARQ_BUFFER_OFFSET 1

#define HDRPCP_MAX_QUEUE_ITEMS \
          ((HDRPCP_MAX_TX_FLOWS * 2) + 0)

#define HDRPCP_GEN_TX_FLOW_ID( sid, ssid ) \
          (uint16) (( (sid) << 0x08 ) | (ssid));

/* TODO: Might need the combo_id as well */
#define HDRPCP_GEN_PKT_HANDLE( c_id, m_buf_index ) \
          (hdrpcp_pkt_handle_type) ( ( (c_id) << 0x08 ) | (m_buf_index) )

#define HDRPCP_GET_PKT_HANDLE_CARRIER_ID( pkt_handle ) \
          ( ( (pkt_handle) & 0xFF00 ) >> 0x08 )

#define HDRPCP_GET_PKT_HANDLE_MARQ_BUFFER( pkt_handle ) \
          ( (pkt_handle) & 0x00FF )

#define HDRPCP_SIZEOF_ARRAY( array ) \
          ( (sizeof(array)) / sizeof((array)[0]) )
 
/*
 * PCP makes use of many unsigned data types, and so safe 
 * unsigned subtraction without underflow would be useful.
 *
 */
#define HDRPCP_SAFE_UNSIGNED_SUB( ux, uy ) ( (ux) <= (uy) ? 0 : (ux) - (uy) )

/*-----------------------------------------------------
  Data structures and types
-----------------------------------------------------*/

/*
 * hdrpcp_flow_type_enum_type
 *
 * PCP handles three types of flows: 
 *   a.) RX flows for Forward Link data
 *   b.) TX flows for Reverse Link data
 *   c.) MAC flows to manage bindings of TX flows
 *
 */
typedef enum
{
  HDRPCP_RX_FLOW_TYPE = 0,
  HDRPCP_TX_FLOW_TYPE = 1,
  HDRPCP_MAC_FLOW_TYPE = 2,

  HDRPCP_NUM_FLOW_TYPES

} hdrpcp_flow_type_enum_type;

/*
 * hdrpcp_data_priority_enum_type
 *
 * There are two levels of data priority in PCP:
 *   HI - high priority or retransmit data
 *   LO - low priority or new transmit data
 *
 * PCP steps through this enum in its data collection
 * routine: hdrpcp_collect_data_on_given_carrier()
 *
 */
typedef enum
{
  HDRPCP_HI_DATA_PRIORITY = 0,
  HDRPCP_LO_DATA_PRIORITY = 1,

  HDRPCP_NUM_DATA_PRIORITIES

} hdrpcp_data_priority_enum_type;

/* Counterpart of hdrlog_revb_pcp_stats_xxx_data_type,
 * those structs have static size of carriers and flows
 */

typedef struct
{
  uint8 carrier_id;
  boolean prev_sf_canceled;
  boolean is_fmt_b;

  uint8 num_flows;
  hdrlog_revb_pcp_pkt_stats_flow_data_type tx_flows[HDRPCP_MAX_TX_FLOWS];
} hdrpcp_revb_pkt_stats_carrier_data_type;

typedef struct
{
  uint8 combo_id;
  uint8 num_carriers;
  hdrpcp_revb_pkt_stats_carrier_data_type carriers[HDRPCP_MAX_CARRIERS];

} hdrpcp_revb_pkt_stats_combo_data_type;

/* 
 * hdrpcp_log_data_type
 *
 * Data structure containing the packet statistics to be logged.
 *
 */
typedef struct
{
  uint8 version;
  uint8 log_record_number;
  uint8 sub_frame_count;
  uint8 prev_chosen_combo_id;

  uint8 num_combos;
  hdrpcp_revb_pkt_stats_combo_data_type combos[HDRPCP_MAX_COMBOS];
} hdrpcp_log_data_type;

/*
 * hdrpcp_tx_flow_type
 *
 * The TX flow type is PCP's internal representation of a
 * packet application that has registered to transmit data.
 *
 */
typedef struct
{
  void *sent_id_history[HDRPCP_MAX_COMBOS][HDRPCP_MAX_CARRIERS];
  hdrpcp_get_data_size_func_type get_data_size_func_ptr;
  hdrpcp_get_data_func_type get_data_func_ptr[HDRPCP_NUM_DATA_PRIORITIES];
  hdrpcp_tx_status_func_type tx_status_func_ptr;
  hdrpcp_tx_failed_func_type tx_failed_func_ptr;
  
  uint8 stream_id;
  uint8 substream_id;
  uint8 mac_flow_id;
  boolean always_notify_packet_status;

  void *user_data_ptr;
  uint32 bytes_pending;

  uint16 tx_flow_id;
  boolean has_hp_data_pending;
  boolean enqueued_for_hp_data; /* only reset if this flow has been dequeued */

  uint8 current_marq_buffer;
  void *marq_buffers[HDRPCP_MAX_MARQ_BUFFERS][HDRPCP_MAX_CARRIERS];

} hdrpcp_tx_flow_type;

/*
 * hdrpcp_rx_flow_type
 *
 * The RX flow type is PCP's internal representation of a
 * packet application that has registered to receive data.
 *
 */
typedef struct
{
  hdrpcp_put_data_func_type put_data_func_ptr;
  hdrpcp_eos_notification_func_type eos_notif_func_ptr;
  hdrpcp_rx_buf_empty_notification_func_type rx_buf_empty_notif_func_ptr;
  void *user_data_ptr;
  
  /* mapped to stream_id in an array */
  uint8 stream_id; /* stream ID and RX flow ID are the same value */
  uint8 rx_flow_id;

  boolean put_data_this_frame;

} hdrpcp_rx_flow_type;

/*
 * hdrpcp_mac_flow_type
 *
 * The MAC flow type represents the binding of the
 * registered packet applications to distinct MAC flows.
 *
 */
typedef struct
{
  hdrpcp_queue_type flows_queue[HDRPCP_NUM_DATA_PRIORITIES];
  uint8 mac_flow_id;

} hdrpcp_mac_flow_type;

/*
 * hdrpcp_carrier_build_info_type
 *
 * This data structure is used by PCP  to hold all of the
 * temporary variables involved in building a MAC packet.
 *
 */
typedef struct
{
  uint8 carrier_id;
  hdrpcp_mac_flow_allocation_type *mac_flows_ptr;
  uint8 num_mac_flows;

  dsm_item_type **pkt_ptr;
  uint16 pkt_len;
  uint16 payload_len;
  uint16 max_pkt_size;
  uint16 size_limit;

  hdrpcp_pkt_format_enum_type pkt_format;
  hdrpcp_pkt_handle_type pkt_handle;

  uint32 bytes_per_mac_flow[HDRPCP_MAX_MAC_FLOWS];

  uint8 num_mac_flows_with_data;

  uint8 combo_no;
  uint8 carrier_no;
} hdrpcp_carrier_build_info_type;

/*
 * hdrpcp_pkt_history_info_type
 *
 * This data structure holds the history information as well as
 * the mappings from combo ID/carrier ID to combo number/carrier number.
 *
 */
typedef struct
{
  uint8 current_marq_buffer;

  uint8 combo_id_table[HDRPCP_MAX_COMBOS];

} hdrpcp_pkt_history_info_type;

/*
 * hdrpcp_flow_storage_type
 *
 * Physical storage of the different flow types in PCP.
 *
 */
typedef struct
{
  hdrpcp_tx_flow_type tx_flows[HDRPCP_MAX_TX_FLOWS];
  hdrpcp_rx_flow_type rx_flows[HDRPCP_MAX_RX_FLOWS];
  hdrpcp_mac_flow_type mac_flows[HDRPCP_MAX_MAC_FLOWS];

} hdrpcp_flow_storage_type;

/*
 * hdrpcp_static_data_type
 *
 * The data structure containing PCP.
 *
 */
typedef struct
{
  hdrpcp_pkt_history_info_type pkt_history_info;
  hdrpcp_flow_storage_type flows;

  hdrpcp_queue_item_type queue_items[HDRPCP_MAX_QUEUE_ITEMS];
  hdrpcp_queue_type free_q;

  uint16 roll_count;

  boolean first_use_of_roll;
  boolean logging_enabled;

  /* If RMAC0 or RMAC1 is active, packet applications will need
     to register with a special predefined MAC flow ID */
  uint8 rmac_flow_id;

  /* Encoder window misses */
  boolean data_collected[HDRPCP_MAX_CARRIERS];

  uint32 msg_mask;

  /* Log data */
  hdrpcp_log_data_type log_data;
  uint8 log_record_number;

  /* Critical section used for reg/unreg flows */
  rex_crit_sect_type rx_crit_sect;
  rex_crit_sect_type tx_crit_sect;

#ifdef FEATURE_LTE_TO_HDR_OH
  hdrpcp_pkt_handle_type curr_pkt_handle;
#endif /* FEATURE_LTE_TO_HDR_OH */

  /* Verbose debugging setting */
  uint8 verbose_debug;
} hdrpcp_static_data_type;



/*===========================================================================

                         LOCAL VARIABLES

===========================================================================*/

static hdrpcp_static_data_type hdrpcp;


/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/* -- None -- */



/*===========================================================================

                                FUNCTIONS

===========================================================================*/

/*===========================================================================

FUNCTION hdrpcp_log_data_init

DESCRIPTION
  This function initializes logging data structure. It needs to be
  called before hdrpcp_log_data_commit() is called everytime.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void hdrpcp_log_data_init( void )
{
  hdrpcp.logging_enabled = log_status( LOG_HDR_REVB_PCP_PKT_STATS_C );

  if (hdrpcp.logging_enabled)
  {
    memset(&hdrpcp.log_data, 0, sizeof(hdrpcp.log_data));
    hdrpcp.log_data.log_record_number = hdrpcp.log_record_number;

    /* Increment the log record number */
    hdrpcp.log_record_number++;
  }
} /* hdrpcp_log_data_init() */

/*===========================================================================

FUNCTION hdrpcp_log_data_commit

DESCRIPTION
  This function checks if PCP Rev B. RMAC flow log packet is enabled and
  logs the RMAC statistics

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void hdrpcp_log_data_commit( void )
{
  hdrpcp_log_data_type *log_data_ptr = &(hdrpcp.log_data);

  hdrlog_revb_pcp_pkt_stats_flow_data_type* flow_ptr;
  hdrlog_revb_pcp_pkt_stats_carrier_data_type* carrier_ptr;
  hdrlog_revb_pcp_pkt_stats_combo_data_type* combo_ptr;

  LOG_HDR_REVB_PCP_PKT_STATS_C_type *log_ptr;

  uint8 flows_per_carrier[HDRPCP_MAX_COMBOS][HDRPCP_MAX_CARRIERS];
  uint16 carrier_sizes[HDRPCP_MAX_COMBOS][HDRPCP_MAX_CARRIERS];
  uint16 combo_sizes[HDRPCP_MAX_COMBOS];

  uint32 log_pkt_size = sizeof( LOG_HDR_REVB_PCP_PKT_STATS_C_type ) - 
                        sizeof( hdrlog_revb_pcp_pkt_stats_combo_data_type );

  uint32 combo = 0;
  uint32 carrier = 0;
  uint32 flow = 0;

  uint32 num_combos = log_data_ptr->num_combos;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!hdrpcp.logging_enabled)
  {
    return;
  }

  if (hdrpcp.verbose_debug)
  {
    HDR_MSG_PROT_1(MSG_LEGACY_MED, " Total Combos: %d", num_combos);
  }

  memset(flows_per_carrier, 0, sizeof(flows_per_carrier));
  memset(carrier_sizes, 0, sizeof(carrier_sizes));
  memset(combo_sizes, 0, sizeof(combo_sizes));

  /* Do the real magic here */
  for( combo = 0; combo < num_combos; ++combo )
  {
    combo_sizes[combo] = sizeof( hdrlog_revb_pcp_pkt_stats_combo_data_type ) - 
                         sizeof( hdrlog_revb_pcp_pkt_stats_carrier_data_type );

    if (hdrpcp.verbose_debug)
    {
      HDR_MSG_PROT_2(MSG_LEGACY_MED, " %d carriers in combos: %d", 
                   log_data_ptr->combos[combo].num_carriers, combo);
    }

    for( carrier = 0; 
         carrier < log_data_ptr->combos[combo].num_carriers; 
         ++carrier )
    {
      for( flow = 0; flow < HDRPCP_MAX_TX_FLOWS; ++flow )
      {
        flow_ptr = 
          &log_data_ptr->combos[combo].carriers[carrier].tx_flows[flow];
        if ( (flow_ptr->re_tx_q.total_size > 0) ||
             (flow_ptr->new_tx_q.total_size > 0) )
        {
          flows_per_carrier[combo][carrier]++;
        }
      }

      carrier_sizes[combo][carrier] = sizeof( hdrlog_revb_pcp_pkt_stats_carrier_data_type ) +
        (flows_per_carrier[combo][carrier]- 1) * sizeof( hdrlog_revb_pcp_pkt_stats_flow_data_type );

      combo_sizes[combo] += carrier_sizes[combo][carrier];
    }
    log_pkt_size += combo_sizes[combo];
  }
  
  if (hdrpcp.verbose_debug)
  {
    HDR_MSG_PROT_1(MSG_LEGACY_MED, " Alloc %d bytes for logging", 
                 log_pkt_size);
  }

  log_ptr = (LOG_HDR_REVB_PCP_PKT_STATS_C_type *) 
    log_alloc_ex( (log_code_type) LOG_HDR_REVB_PCP_PKT_STATS_C, log_pkt_size );

  if (log_ptr == NULL)
  {
      /* Not able to allocate memory for logging. Skipping than crash */
      HDR_MSG_PROT(MSG_LEGACY_FATAL, "PCPLOG: log_alloc_ex returned NULL");
      return;
  }

  log_ptr->version = log_data_ptr->version;
  log_ptr->log_record_number = log_data_ptr->log_record_number;
  log_ptr->sub_frame_count = log_data_ptr->sub_frame_count;
  log_ptr->prev_chosen_combo_id = log_data_ptr->prev_chosen_combo_id;

  log_ptr->num_combos = (uint8) num_combos;

  combo_ptr = log_ptr->combos;

  for( combo = 0; combo < num_combos; ++combo )
  {
    combo_ptr->combo_id = log_data_ptr->combos[combo].combo_id;
    combo_ptr->num_carriers = log_data_ptr->combos[combo].num_carriers;

    carrier_ptr = combo_ptr->carriers;
    for( carrier = 0; carrier < combo_ptr->num_carriers; ++carrier )
    {
      carrier_ptr->carrier_id = 
        log_data_ptr->combos[combo].carriers[carrier].carrier_id;
      carrier_ptr->prev_sf_canceled = 
        log_data_ptr->combos[combo].carriers[carrier].prev_sf_canceled;
      carrier_ptr->is_fmt_b = 
        log_data_ptr->combos[combo].carriers[carrier].is_fmt_b;
      carrier_ptr->num_flows = flows_per_carrier[combo][carrier];

      flow_ptr = carrier_ptr->tx_flows;
      for( flow = 0; flow < HDRPCP_MAX_TX_FLOWS; ++flow )
      {
        if ( (log_data_ptr->combos[combo].carriers[carrier].tx_flows[flow].re_tx_q.total_size > 0) ||
             (log_data_ptr->combos[combo].carriers[carrier].tx_flows[flow].new_tx_q.total_size > 0) )
        {
#ifdef FEATURE_MEMCPY_REMOVAL
          *flow_ptr = log_data_ptr->combos[combo].carriers[
                                              carrier].tx_flows[flow];
#else
          memcpy(flow_ptr, 
                 &log_data_ptr->combos[combo].carriers[carrier].tx_flows[flow],
                 sizeof(hdrlog_revb_pcp_pkt_stats_flow_data_type));
#endif /* FEATURE_MEMCPY_REMOVAL */
          flow_ptr++;
          if (hdrpcp.verbose_debug)
          {
          HDR_MSG_PROT_6(
              MSG_LEGACY_MED, 
              " Combo %d Carrier %d Flow %d Mac %d re[%d] new[%d]",
            combo, carrier, flow, 
            log_data_ptr->combos[combo].carriers[carrier].tx_flows[flow].mac_flow_id,
            log_data_ptr->combos[combo].carriers[carrier].tx_flows[flow].re_tx_q.total_size,
            log_data_ptr->combos[combo].carriers[carrier].tx_flows[flow].new_tx_q.total_size
            );
          }
        }
      }

      carrier_ptr = (hdrlog_revb_pcp_pkt_stats_carrier_data_type*)
                    ((uint32)carrier_ptr + carrier_sizes[combo][carrier]);
    }

    combo_ptr = (hdrlog_revb_pcp_pkt_stats_combo_data_type*)
                ((uint32)combo_ptr + combo_sizes[combo]);
  }

  log_commit( (log_type *) log_ptr );

} /* hdrpcp_log_data_commit() */

/*===========================================================================

FUNCTION HDRPCP_FIND_FLOW

DESCRIPTION
  This function searches for a particular flow given both the type and the ID.

DEPENDENCIES
  None.

PARAMETERS
  flow_type  - The type of flow to search for; TX, RX or MAC
  flow_id    - The ID of the flow

RETURN VALUE
  void *  - A pointer to the flow found; this pointer must be cast to
            the proper type before being used

SIDE EFFECTS
  None.

===========================================================================*/
static void *hdrpcp_find_flow
(
  hdrpcp_flow_type_enum_type flow_type,
  uint32 flow_id
)
{
  void *flow_ptr = NULL;
  int flow_pos = 0;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( flow_type )
  {
    case HDRPCP_TX_FLOW_TYPE:
      for( flow_pos = 0; flow_pos < HDRPCP_MAX_TX_FLOWS; flow_pos++ )
      {
        if( hdrpcp.flows.tx_flows[flow_pos].tx_flow_id == (uint16) flow_id )
        {
          flow_ptr = &hdrpcp.flows.tx_flows[flow_pos];
          break;
        }
      }
      break;

    case HDRPCP_RX_FLOW_TYPE:
      for( flow_pos = 0; flow_pos < HDRPCP_MAX_RX_FLOWS; flow_pos++ )
      {
        if( hdrpcp.flows.rx_flows[flow_pos].rx_flow_id == (uint8) flow_id )
        {
          flow_ptr = &hdrpcp.flows.rx_flows[flow_pos];
          break;
        }
      }
      break;

    case HDRPCP_MAC_FLOW_TYPE:
      for( flow_pos = 0; flow_pos < HDRPCP_MAX_MAC_FLOWS; flow_pos++ )
      {
        if( hdrpcp.flows.mac_flows[flow_pos].mac_flow_id == (uint8) flow_id )
        {
          flow_ptr = &hdrpcp.flows.mac_flows[flow_pos];
          break;
        }
      }
      break;

    default:
      break;
  }

  HDR_MSG_PROT_4( MSG_LEGACY_LOW, 
                  "FIND FLOW type %d flow id %d pos %d ptr 0x%x",
                  flow_type,
                  flow_id,
                  flow_pos,
                  (uint32) flow_ptr );

  return flow_ptr;

} /* hdrpcp_find_flow() */

/*===========================================================================

FUNCTION HDRPCP_FIND_FREE_FLOW

DESCRIPTION
  This function searches for a free flow of a given type.

DEPENDENCIES
  None.

PARAMETERS
  flow_type  - The type of flow to search for; TX, RX or MAC

RETURN VALUE
  void *  - A pointer to the flow found; this pointer must be cast to 
            the proper type before being used

SIDE EFFECTS
  None.

===========================================================================*/
static void *hdrpcp_find_free_flow
(
  hdrpcp_flow_type_enum_type flow_type
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return hdrpcp_find_flow( flow_type, HDRPCP_FREE_FLOW_ENTRY );

} /* hdrpcp_find_free_flow() */

/*===========================================================================

FUNCTION HDRPCP_BUILD_PACKET

DESCRIPTION
  This function builds the connection layer packet given the new app layer
  packet, the size, and the current build info.

DEPENDENCIES
  None.

PARAMETERS
  cur_pkt_ptr    -
  cur_pkt_len    -
  crr_build_ptr  -
  tx_flow_ptr    -

RETURN VALUE
  uint16  -

SIDE EFFECTS
  None.

===========================================================================*/
static uint16 hdrpcp_build_packet
(
  dsm_item_type *cur_pkt_ptr,
  uint16 cur_pkt_len,
  hdrpcp_carrier_build_info_type *crr_build_ptr,
  hdrpcp_tx_flow_type *tx_flow_ptr /* used for stream_id */
)
{
  const uint8 stream_headers[] = { HDRPCP_STREAM0_HEADER,
                                   HDRPCP_STREAM1_HEADER,
                                   HDRPCP_STREAM2_HEADER,
                                   HDRPCP_STREAM3_HEADER };

  
  dsm_item_type **mac_pkt_ptr = crr_build_ptr->pkt_ptr;

  uint8 stream_id = 0;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT( cur_pkt_len > 0 );

  /* If this argument is NULL, then we are building in Rel 0 Mode */
  if( tx_flow_ptr == NULL )
  {
    /* If this function is called with a NULL TX flow pointer, use
       stream 0 for this build packet */
    stream_id = stream_headers[0];
  }
  else
  {
    ASSERT( tx_flow_ptr->stream_id < ARR_SIZE( stream_headers ) );
    stream_id = stream_headers[tx_flow_ptr->stream_id];
  }
  
  /* Set the two-bit stream header */
  *((uint8 *) cur_pkt_ptr->data_ptr) &= HDRPCP_STREAM_INV_MASK;
  *((uint8 *) cur_pkt_ptr->data_ptr) |= stream_id;
  
  crr_build_ptr->payload_len += cur_pkt_len;

  if( *mac_pkt_ptr )
  {
    ASSERT( cur_pkt_len != crr_build_ptr->size_limit );

    (void) dsm_pushdown_packed( &cur_pkt_ptr, (void *) &cur_pkt_len, 
                         HDRPCP_FMT_B_PKT_HEADER_LEN,
                         DSM_DS_SMALL_ITEM_POOL );
    cur_pkt_len++;

    dsm_append( mac_pkt_ptr, &cur_pkt_ptr );
  }
  else /* if the MAC packet is empty */
  {
    /* If the current packet being built does not take the 
       entire available size */
    if( cur_pkt_len != crr_build_ptr->size_limit )
    {
      (void) dsm_pushdown_packed( &cur_pkt_ptr, (void *) &cur_pkt_len, 
                                  HDRPCP_FMT_B_PKT_HEADER_LEN,
                                  DSM_DS_SMALL_ITEM_POOL );

      cur_pkt_len++;
    }

    *mac_pkt_ptr = cur_pkt_ptr;
  }

  if (hdrpcp.verbose_debug)
  {
    HDR_MSG_PROT_1(MSG_LEGACY_MED, "=TX=  Item left in pool %d",
                 DSM_POOL_FREE_CNT(DSM_DS_SMALL_ITEM_POOL));
  }

  crr_build_ptr->pkt_len += cur_pkt_len;
  crr_build_ptr->max_pkt_size = 
    HDRPCP_SAFE_UNSIGNED_SUB( crr_build_ptr->max_pkt_size, cur_pkt_len );

  return cur_pkt_len;

} /* hdrpcp_build_packet() */

/*===========================================================================

FUNCTION HDRPCP_COLLECT_DATA_ON_GIVEN_CARRIER

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
static uint32 hdrpcp_collect_data_on_given_carrier
(
  uint8 combo_id,
  hdrpcp_carrier_build_info_type *crr_build_ptr
)
{
  uint8 carrier_id = crr_build_ptr->carrier_id;
  uint8 cur_marq_buffer = hdrpcp.pkt_history_info.current_marq_buffer;

  hdrpcp_mac_flow_allocation_type *mac_allocs_ptr = crr_build_ptr->mac_flows_ptr;
  uint32 num_mac_flows = crr_build_ptr->num_mac_flows;

  uint32 mac_flow = 0;

  hdrpcp_mac_flow_type *mac_flow_ptr = NULL;
  hdrpcp_tx_flow_type *tx_flow_ptr = NULL;

  hdrpcp_queue_item_type *q_item_ptr = NULL;
  hdrpcp_queue_item_type *q_start_item_ptr = NULL;

  uint32 cur_mac_flow_alloc = 0;
  uint32 priority = 0;

  boolean collect_data = FALSE;

  #ifdef FEATURE_HDRPCP_EXTRA_SPACE
  #error code not present
#endif

  uint16 space_remaining = 0;
  const uint16 size_limit = crr_build_ptr->size_limit;

  uint32 num_pkts_this_carrier = 0;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the number of MAC flows with data to 0 */
  crr_build_ptr->num_mac_flows_with_data = 0;

  /* Calculate the extra space */
  #ifdef FEATURE_HDRPCP_EXTRA_SPACE
  #error code not present
#endif
  
  rex_enter_crit_sect(&hdrpcp.tx_crit_sect);

  for( mac_flow = 0; mac_flow < num_mac_flows; mac_flow++ )
  {
    mac_flow_ptr = (hdrpcp_mac_flow_type *)
      hdrpcp_find_flow( HDRPCP_MAC_FLOW_TYPE, mac_allocs_ptr[mac_flow].mac_flow_id );
#ifndef FEATURE_HDR_COMMON_PCP_RMAC0
    ASSERT( mac_flow_ptr );
#endif /* FEATURE_HDR_COMMON_PCP_RMAC0 */
    if( mac_flow_ptr == NULL )
    {
      HDR_MSG_PROT_1( MSG_LEGACY_ERROR, 
        "MAC Flow ID: 0x%x not found!", mac_allocs_ptr[mac_flow].mac_flow_id );

      continue; /* onto the next MAC flow */
    }

    /* Zero out the number of bytes sent on this mac flow */
    crr_build_ptr->bytes_per_mac_flow[mac_flow] = 0;

    cur_mac_flow_alloc = mac_allocs_ptr[mac_flow].size;

    for( priority = HDRPCP_HI_DATA_PRIORITY; 
         priority < HDRPCP_NUM_DATA_PRIORITIES; 
         priority++ )
    {
      
      /* Check the first item on the queue */
      q_start_item_ptr = 
        hdrpcp_queue_check_front_item( 
          &mac_flow_ptr->flows_queue[priority] );

      if( q_start_item_ptr )
      {
        collect_data = TRUE;
      }
      else
      {
        /* No items on this queue */
        collect_data = FALSE;
      }

      while( collect_data )
      {
        uint16 allowed_bytes       = 0;
        uint16 bytes_to_tx         = 0;
        void *tx_set_id            = NULL;
        void *user_data_ptr        = NULL;
        dsm_item_type *tx_data_ptr = NULL;

        space_remaining = crr_build_ptr->max_pkt_size;
        /* TODO: + extra_space */

        q_item_ptr = 
          hdrpcp_queue_check_front_item( 
            &mac_flow_ptr->flows_queue[priority] );
        ASSERT(q_item_ptr);

        tx_flow_ptr = (hdrpcp_tx_flow_type *) q_item_ptr->data;
        ASSERT( tx_flow_ptr );
        user_data_ptr = tx_flow_ptr->user_data_ptr;

        if( cur_mac_flow_alloc > 0 && space_remaining > 0 )
        { 
          if( space_remaining == size_limit && /* No packets built thus far */
              cur_mac_flow_alloc == size_limit && /* There is only one flow w/ data */
              tx_flow_ptr->bytes_pending >= size_limit ) /* Enough data to fill entire packet */
          {
            allowed_bytes = (uint16) size_limit;
          }
          else
          {
            allowed_bytes = (uint16) MIN( HDRPCP_FMT_B_MAX_PKT_SIZE,
                                          cur_mac_flow_alloc - 1 );

            allowed_bytes = (uint16) MIN( space_remaining - 1,
                                          allowed_bytes );
          }
        }
        else
        {
          allowed_bytes = 0;
        }

        if( allowed_bytes == 0 )
        {
          collect_data = FALSE;
          HDR_MSG_PROT( MSG_LEGACY_MED, "Allowed bytes = 0!" );
          break;
        }

        if( tx_flow_ptr->get_data_func_ptr[priority] != NULL )
        {
          HDR_MSG_PROT_6( MSG_LEGACY_MED,
            "=TX= allowed: %d, bytes_p: %d, tx_flow_id %d, size_l: %d, sp_rem: %d Total bytes:%d",
            allowed_bytes, tx_flow_ptr->bytes_pending, tx_flow_ptr->tx_flow_id, 
            size_limit, space_remaining, cur_mac_flow_alloc );

          if( tx_flow_ptr->tx_flow_id == HDRPCP_SLP_FLOW_ID )
          {
            HDR_MSG_PROT_1( MSG_LEGACY_LOW, "SLP!!! Max size: %d", space_remaining - 1 );
            allowed_bytes = space_remaining - 1;
          }
          else if (hdrpcp.verbose_debug)
          {
            HDR_MSG_PROT_1( MSG_LEGACY_MED, "RLP!!! Max size: %d", space_remaining - 1 );
          }

          bytes_to_tx = 
            tx_flow_ptr->get_data_func_ptr[priority](
              combo_id, carrier_id, allowed_bytes, 
              &tx_data_ptr, &tx_set_id, user_data_ptr );
        }
        else
        {
          HDR_MSG_PROT_2( MSG_LEGACY_MED, 
            "No get_data[%d] callback for TX flow ID: 0x%x",
            priority, tx_flow_ptr->tx_flow_id );

          bytes_to_tx = 0;
        }

        if (hdrpcp.verbose_debug)
        {
          HDR_MSG_PROT_4( MSG_LEGACY_MED, 
                          "=TX=  Collect data combo:%d flow:%d bytes %d TX SET:0x%x",
                          crr_build_ptr->combo_no, tx_flow_ptr->tx_flow_id, 
                          bytes_to_tx, tx_set_id );
        }

        if( bytes_to_tx > 0 )
        {
          uint8 combo_no   = crr_build_ptr->combo_no;
          uint8 carrier_no = crr_build_ptr->carrier_no;
          uint16 built_sz  = 0;
         
          if (hdrpcp.logging_enabled)
          {
            uint8 flow_pos = 0;

            for( flow_pos = 0; flow_pos < HDRPCP_MAX_TX_FLOWS; flow_pos++ )
            {
              if( hdrpcp.flows.tx_flows[flow_pos].tx_flow_id == (uint16) tx_flow_ptr->tx_flow_id )
              {
                break;
              }
            }

            if (flow_pos == HDRPCP_MAX_TX_FLOWS)
            {
              HDR_MSG_PROT_1( MSG_LEGACY_ERROR, "PCPLOG: flow_id %d not found!", 
                              tx_flow_ptr->tx_flow_id );
            }
            else 
            {
              hdrpcp.log_data.combos[combo_no].carriers[carrier_no].tx_flows[flow_pos].mac_flow_id = 
                mac_flow_ptr->mac_flow_id;


              if (priority == HDRPCP_HI_DATA_PRIORITY)
              {
                if (hdrpcp.verbose_debug)
                {
                HDR_MSG_PROT_6(
                  MSG_LEGACY_MED, 
                    "=TX=  collect re-xmit, combo %d carrier %d flow %d flow_id %d mac_id %d bytes %d", 
                  combo_no, carrier_id, flow_pos, tx_flow_ptr->tx_flow_id, 
                  mac_flow_ptr->mac_flow_id, bytes_to_tx);
                }

                if (hdrpcp.log_data.combos[combo_no].carriers[carrier_no].tx_flows[flow_pos].re_tx_q.total_size == 0)
                {
                  (void ) dsm_extract( tx_data_ptr, 0, 
                    hdrpcp.log_data.combos[combo_no].carriers[carrier_no].tx_flows[flow_pos].re_tx_q.header_bytes, 
                    sizeof(hdrpcp.log_data.combos[combo_no].carriers[carrier_no].tx_flows[flow_pos].re_tx_q.header_bytes) );

                  if (hdrpcp.verbose_debug)
                  {
                    HDR_MSG_PROT_4( 
                      MSG_LEGACY_MED, "=TX=  collect re-xmit, header:0x%02x%02x%02x%02x",
                      hdrpcp.log_data.combos[combo_no].carriers[carrier_no].tx_flows[flow_pos].re_tx_q.header_bytes[0],
                      hdrpcp.log_data.combos[combo_no].carriers[carrier_no].tx_flows[flow_pos].re_tx_q.header_bytes[1],
                      hdrpcp.log_data.combos[combo_no].carriers[carrier_no].tx_flows[flow_pos].re_tx_q.header_bytes[2],
                      hdrpcp.log_data.combos[combo_no].carriers[carrier_no].tx_flows[flow_pos].re_tx_q.header_bytes[3]);
                  }
                }
    
                hdrpcp.log_data.combos[combo_no].carriers[carrier_no].tx_flows[flow_pos].re_tx_q.total_size += bytes_to_tx;
              }
              else
              {
                if (hdrpcp.verbose_debug)
                {
                HDR_MSG_PROT_6( 
                   MSG_LEGACY_MED, 
                     "=TX=  collect new, combo %d carrier %d flow %d flow_id %d mac_id %d bytes %d", 
                    combo_no, carrier_id, flow_pos, tx_flow_ptr->tx_flow_id,
                    mac_flow_ptr->mac_flow_id, bytes_to_tx);
                }

                if (hdrpcp.log_data.combos[combo_no].carriers[carrier_no].tx_flows[flow_pos].new_tx_q.total_size == 0)
                {
                  (void ) dsm_extract( tx_data_ptr, 0, 
                     hdrpcp.log_data.combos[combo_no].carriers[carrier_no].tx_flows[flow_pos].new_tx_q.header_bytes,
                     sizeof(hdrpcp.log_data.combos[combo_no].carriers[carrier_no].tx_flows[flow_pos].new_tx_q.header_bytes));
                  if (hdrpcp.verbose_debug)
                  {
                    HDR_MSG_PROT_4( 
                      MSG_LEGACY_MED, "=TX=  collect new, header:0x%02x%02x%02x%02x", 
                    hdrpcp.log_data.combos[combo_no].carriers[carrier_no].tx_flows[flow_pos].new_tx_q.header_bytes[0],
                    hdrpcp.log_data.combos[combo_no].carriers[carrier_no].tx_flows[flow_pos].new_tx_q.header_bytes[1],
                    hdrpcp.log_data.combos[combo_no].carriers[carrier_no].tx_flows[flow_pos].new_tx_q.header_bytes[2],
                    hdrpcp.log_data.combos[combo_no].carriers[carrier_no].tx_flows[flow_pos].new_tx_q.header_bytes[3]);
                  }
                }

                hdrpcp.log_data.combos[combo_no].carriers[carrier_no].tx_flows[flow_pos].new_tx_q.total_size += bytes_to_tx;
              }
            }
          }

          /* M-ARQ info will be updated on the next subframe 
             in hdrpcp_get_packet_info() */
          tx_flow_ptr->sent_id_history[combo_no][carrier_id] = tx_set_id;

          /* Reduce the number of bytes pending */
          tx_flow_ptr->bytes_pending -= bytes_to_tx;

          /* Build the actual connection layer packet */
          built_sz = 
            hdrpcp_build_packet( tx_data_ptr, bytes_to_tx, crr_build_ptr, tx_flow_ptr ); 

          /* Update the number of bytes sent on this mac flow */
          crr_build_ptr->bytes_per_mac_flow[mac_flow] += built_sz;

          num_pkts_this_carrier++;

          /* Reduce the amount we can attempt to send on the next iteration */
          cur_mac_flow_alloc =
            HDRPCP_SAFE_UNSIGNED_SUB( cur_mac_flow_alloc, built_sz );
        }
        else /* otherwise if bytes to transmit is 0 */
        {
          q_item_ptr = hdrpcp_queue_get_item( &mac_flow_ptr->flows_queue[priority] );
          ASSERT(q_item_ptr);
          switch( priority )
          {
            case HDRPCP_HI_DATA_PRIORITY:

              /* We have dequeued this flow and therefore this flag can be reset */
              tx_flow_ptr->enqueued_for_hp_data = FALSE;

              /* Clear the queue item */
              q_item_ptr->data = NULL;

              /* Free the high priority queue item holding the TX flow */
              hdrpcp_queue_put_item( &hdrpcp.free_q, q_item_ptr );

              if( hdrpcp_queue_check_front_item( 
                    &mac_flow_ptr->flows_queue[HDRPCP_HI_DATA_PRIORITY] ) == NULL )
              {
                collect_data = FALSE;
              }
              break;
            case HDRPCP_LO_DATA_PRIORITY:
              hdrpcp_queue_put_item( 
                &mac_flow_ptr->flows_queue[HDRPCP_LO_DATA_PRIORITY], q_item_ptr );

              if( hdrpcp_queue_check_front_item( 
                    &mac_flow_ptr->flows_queue[HDRPCP_LO_DATA_PRIORITY] ) == q_start_item_ptr )
              {
                collect_data = FALSE;
              }
              break;
            default:
              /* Impossible */
              break;
          }
        }
      } /* end while ( collect data ); */
    
    } /* end for( priority ) */

    /* TODO: extra_space = current mac flow allocation */

    /* Update the "sent" size for this mac flow */
    mac_allocs_ptr[mac_flow].size = crr_build_ptr->bytes_per_mac_flow[mac_flow];

    /* Update the number of mac flows transmitting data for this carrier */
    if( mac_allocs_ptr[mac_flow].size > 0 )
    {
      crr_build_ptr->num_mac_flows_with_data++;
    }
    
  }

  rex_leave_crit_sect(&hdrpcp.tx_crit_sect);
  
  /* Determine the packet transmission format */
  if( crr_build_ptr->payload_len == crr_build_ptr->size_limit )
  {
    crr_build_ptr->pkt_format = HDRPCP_PACKET_FORMAT_A;
  }
  else
  {
    crr_build_ptr->pkt_format = HDRPCP_PACKET_FORMAT_B;
  }

  crr_build_ptr->pkt_handle = 
    HDRPCP_GEN_PKT_HANDLE( 
      crr_build_ptr->carrier_id, 
      cur_marq_buffer + HDRPCP_MARQ_BUFFER_OFFSET );

  if (hdrpcp.verbose_debug)
  {
    HDR_MSG_PROT_1(MSG_LEGACY_MED, "=TX=  Collect data pkt hdl:0x%04x",
                 crr_build_ptr->pkt_handle);
  }

  HDR_MSG_PROT_1( MSG_LEGACY_MED, "numpkts this carr: %d", num_pkts_this_carrier );

  return num_pkts_this_carrier;

} /* hdrpcp_collect_data_on_given_carrier() */

/*===========================================================================

FUNCTION HDRPCP_IS_PS_VAL_EQUAL_TO_BYTES_PENDING

DESCRIPTION
  This function performs an explicit binary search on the known RMAC PS values
  to determine if the number of bytes pending is exact.

DEPENDENCIES
  None.

PARAMETERS
  bytes_pending  - The number of bytes pending

RETURN VALUE
  TRUE if the value is an exact RMAC PS value; FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static boolean hdrpcp_is_ps_val_equal_to_bytes_pending
(
  uint32 bytes_pending
)
{
  boolean ps_val_found = FALSE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* This function makes use of the known, fixed RMAC3 PS table
     and performs an explicit binary search for the value in question */
  if( bytes_pending <= 188 )
  {
    if( bytes_pending <= 60 )
    {
      if( bytes_pending == 12 || bytes_pending == 28 || bytes_pending == 60 )
      {
        ps_val_found = TRUE;
      }
    }
    else
    {
      if( bytes_pending == 92 || bytes_pending == 124 || bytes_pending == 188 )
      {
        ps_val_found = TRUE;
      }
    }
  }
  else
  {
    if( bytes_pending <= 508 )
    {
      if( bytes_pending == 252 || bytes_pending == 380 || bytes_pending == 508 )
      {
        ps_val_found = TRUE;
      }
    }
    else
    {
      if( bytes_pending == 764 || bytes_pending == 1020 || bytes_pending == 1532 )
      {
        ps_val_found = TRUE;
      }
    }
  }

  return ps_val_found;

} /* hdrpcp_is_ps_val_equal_to_bytes_pending() */

/*===========================================================================

FUNCTION HDRPCP_PARSE_PACKET_HEADER

DESCRIPTION
  This function parses a connection layer packet header and determines which
  packet application/stream the individual app layer packet is destined for.

DEPENDENCIES
  None.

PARAMETERS
  frame_ptr         - The connection layer packet
  frame_offset_ptr  - The offset into the connection layer packet
  frame_length      - The length of the connection layer packet
  pkt_format        - The format of the connection layer packet to parse

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static boolean hdrpcp_parse_packet_header
(
  dsm_item_type *frame_ptr,
  uint16 *frame_offset_ptr,
  uint16 frame_length,
  hdrpcp_pkt_format_enum_type pkt_format
)
{
  dsm_item_type *dup_pkt_ptr = NULL;
  boolean pkt_parsed = FALSE;

  uint16 dup_pkt_length = 0;
  uint16 pkt_length = 0;
  uint16 pkt_offset = 0;

  uint8 stream_id = HDRPCP_INVALID_ID;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  pkt_offset = *frame_offset_ptr;

  if( pkt_format == HDRPCP_PACKET_FORMAT_B )
  {
    if( dsm_peek_byte( frame_ptr, pkt_offset, (uint8 *) &pkt_length ) == FALSE )
    {
      HDR_MSG_PROT( MSG_LEGACY_ERROR, "Cannot read format B packet header" );
      return FALSE;
    }

    if( pkt_length == 0 )
    {
      if (hdrpcp.verbose_debug)
      {
        HDR_MSG_PROT( MSG_LEGACY_MED, "=RX=  Padding bytes hit in format B packet" );
      }
      
      *frame_offset_ptr += frame_length;
      return TRUE;
    }

    pkt_offset++;
    dup_pkt_length = dsm_dup_packet( &dup_pkt_ptr, frame_ptr, 
                                     pkt_offset, pkt_length );

    if (dup_pkt_ptr == NULL) 
    {
      /* Cannot dup pkt, possible because length field in Formbat B header is 
         incorrect. */
      HDR_MSG_PROT_4(MSG_LEGACY_ERROR, "Incorrect Format B size: %d offset: %d header: %d dup %d",
                     frame_length, pkt_offset, pkt_length, dup_pkt_length);
      return FALSE;
    }
  }
  else /* if the pkt_format == HDRPCP_PACKET_FORMAT_A */
  {
    pkt_length = frame_length;
    dup_pkt_ptr = frame_ptr;
    dup_pkt_length = pkt_length;
  }

  if( dup_pkt_length == pkt_length )
  {
    if( dup_pkt_ptr == NULL )
    {
      if( pkt_offset == frame_length )
      {
        HDR_MSG_PROT( MSG_LEGACY_ERROR, "Spurious byte at end of frame" );
      }
      else
      {
        HDR_MSG_PROT( MSG_LEGACY_ERROR, "dsm_dup_packet returned OK w/ NULL" );
      }

      return FALSE;
    }

    /* dsm_dup_packet doesn't copy the app_ptr field to the dup'd item.
       Copying the app_ptr explicitly. */
    HDRDSM_COPY_APP_PTR_TO_DUPD_ITEM( frame_ptr, dup_pkt_ptr );

    pkt_offset += pkt_length;

    if( dup_pkt_ptr->used > 0 )
    {
      stream_id = *(dup_pkt_ptr->data_ptr) & HDRPCP_STREAM_MASK;

      switch( stream_id )
      {
      case HDRPCP_STREAM0_HEADER:
        HDR_MSG_PROT_1( MSG_LEGACY_LOW, "%d bytes for SLP", dup_pkt_length );

        stream_id = HDRPCP_STREAM0_ID;
        break;

      case HDRPCP_STREAM1_HEADER:
        stream_id = HDRPCP_STREAM1_ID;
        break;

      case HDRPCP_STREAM2_HEADER:
        stream_id = HDRPCP_STREAM2_ID;
        break;

      case HDRPCP_STREAM3_HEADER:
        stream_id = HDRPCP_STREAM3_ID;
        break;

      default:
        HDR_MSG_PROT_1( MSG_LEGACY_ERROR, "Unknown stream id 0x%x", stream_id );
        dsm_free_packet( &dup_pkt_ptr );
        break;
      }

      if( stream_id != HDRPCP_INVALID_ID )
      {
        hdrpcp_rx_flow_type *rx_flow_ptr;

        rex_enter_crit_sect(&hdrpcp.rx_crit_sect);

        rx_flow_ptr = (hdrpcp_rx_flow_type *)
          hdrpcp_find_flow( HDRPCP_RX_FLOW_TYPE, stream_id );

        if( rx_flow_ptr && rx_flow_ptr->put_data_func_ptr )
        {        
          rx_flow_ptr->put_data_func_ptr(
            dup_pkt_ptr,
            rx_flow_ptr->user_data_ptr );

          rx_flow_ptr->put_data_this_frame = TRUE;

          if (hdrpcp.verbose_debug)
          {
            HDR_MSG_PROT_2( MSG_LEGACY_MED, "=RX=  Put %d bytes to stream %d", 
                          dup_pkt_length, stream_id );
          }

          pkt_parsed = TRUE;
        }
        else
        {
          HDR_MSG_PROT_2( MSG_LEGACY_ERROR, 
                          "%d bytes rx'd for unregistered stream %d",
                           dup_pkt_length, stream_id );
          dsm_free_packet( &dup_pkt_ptr );
        }

        rex_leave_crit_sect(&hdrpcp.rx_crit_sect);
      }
    } /* end if( dup_pkt_ptr->used > 0 ) */    
  }
  else /* if( dup_pkt_length != pkt_length ) */
  {
    HDR_MSG_PROT( MSG_LEGACY_ERROR, "Could not dup packet" );
    dsm_free_packet( &dup_pkt_ptr );
  }

  *frame_offset_ptr = pkt_offset;

  return pkt_parsed;

} /* hdrpcp_parse_packet_header() */

/*===========================================================================

FUNCTION HDRPCP_ROLL_CB

DESCRIPTION
  This is the callback function registered with the PN roll ISR to increment
  the roll count for use with time_since_prev_req fields in both Release A 
  and Release B modes.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Increments the roll_count variable in the hdrpcp structure.

===========================================================================*/
void hdrpcp_roll_cb
( 
  void /* no arguments */
)
{
  /* Increments the roll count in the ISR's operating context */
  hdrpcp.roll_count++;

  if (hdrpcp.verbose_debug)
  {
    HDR_MSG_PROT_1( MSG_LEGACY_MED, "roll_cb %d", hdrpcp.roll_count);
  }
} /* hdrpcp_roll_cb() */

/*===========================================================================

FUNCTION HDRPCP_INIT

DESCRIPTION
  Performs one time initialization for PCP.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
   uint32  - Size of storage required by PCP in bytes

SIDE EFFECTS
  None.

===========================================================================*/
uint32 hdrpcp_init
(
  void /* no arguments */
)
{
  int q_item = 0;
  int flow_pos = 0;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  memset( &hdrpcp, 0, sizeof(hdrpcp_static_data_type) );

  hdrpcp.first_use_of_roll = TRUE;

  rex_init_crit_sect(&hdrpcp.rx_crit_sect);
  rex_init_crit_sect(&hdrpcp.tx_crit_sect);

  hdrpcp_queue_init( &hdrpcp.free_q );

  /* Initialize the free queue */
  for( q_item = 0; q_item < HDRPCP_MAX_QUEUE_ITEMS; q_item++ )
  {
    hdrpcp.queue_items[q_item].next = NULL;
    hdrpcp.queue_items[q_item].data = NULL;

    hdrpcp_queue_put_item( &hdrpcp.free_q, &hdrpcp.queue_items[q_item] );
  }

  for( flow_pos = 0; flow_pos < HDRPCP_MAX_MAC_FLOWS; flow_pos++ )
  {
    hdrpcp.flows.mac_flows[flow_pos].mac_flow_id = (uint8) HDRPCP_FREE_FLOW_ENTRY;
  }

  for( flow_pos = 0; flow_pos < HDRPCP_MAX_TX_FLOWS; flow_pos++ )
  {
    hdrpcp.flows.tx_flows[flow_pos].tx_flow_id = (uint16) HDRPCP_FREE_FLOW_ENTRY;
  }

  for( flow_pos = 0; flow_pos < HDRPCP_MAX_RX_FLOWS; flow_pos++ )
  {
    hdrpcp.flows.rx_flows[flow_pos].rx_flow_id = (uint8) HDRPCP_FREE_FLOW_ENTRY;
  }

  /* Initialize the SLP Packet Application */
  hdrpcp_slp_pkt_app_init();

  hdrpcp_log_data_init();

  return sizeof(hdrpcp);

} /* hdrpcp_init() */

/*===========================================================================

FUNCTION HDRPCP_GET_PKT

DESCRIPTION
  This function is called by legacy protocols to collect data to be 
  transmitted.

DEPENDENCIES
  None.

PARAMETERS
  max_bytes            - The maximum number of bytes that can be collected
  channel              - The channel for which this data is destined
  time_since_prev_req  - The number of PN ROLLs since the previous request
  pkt_ptr              - DSM item containing the collected data
  is_fmt_b_ptr         - Set by PCP to indicate the packet format

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
uint16 hdrpcp_get_pkt
(
  uint16 max_bytes,
  hdrhai_channel_enum_type channel,
  uint16 time_since_prev_req,
  dsm_item_type **pkt_ptr,
  boolean *is_fmt_b_ptr
)
{
  static hdrpcp_build_data_per_combo_type combo_data;
  static hdrpcp_mac_flow_allocation_type mac_flows[2];
  static hdrpcp_prev_subframe_combo_info_type combo_info;
  static hdrpcp_carrier_build_info_type carrier_build_info;
  
  dsm_item_type *tx_pkt_ptr = NULL;
  uint16 pkt_size = 0;

  uint16 bytes_to_tx = 0;
  uint16 allowed_bytes = 0;

  uint8 num_rtc_mac_flows = 0;
#ifdef FEATURE_LTE_TO_HDR_OH
  hdrhai_tunnel_mode_type curr_tunnel_mode;
#endif  /* FEATURE_LTE_TO_HDR_OH */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Guarantee that all static variables are initialized to zero */
  memset( &combo_data, 0, sizeof(hdrpcp_build_data_per_combo_type) );
  memset( mac_flows, 0, sizeof(hdrpcp_mac_flow_allocation_type) * 2 );
  memset( &combo_info, 0, sizeof(hdrpcp_prev_subframe_combo_info_type) );
  memset( &carrier_build_info, 0, sizeof(hdrpcp_carrier_build_info_type) );

  /* Spoof the chosen combo with a status of E_SUCCESS for all 
     packets transmitted */
  combo_info.combo_id = 0;
  combo_info.num_carriers = 1;
  combo_info.carrier_info[0].carrier_id = 0;
  combo_info.carrier_info[0].pkt_handle = HDRPCP_LEGACY_PKT_HANDLE;
  combo_info.carrier_info[0].pkt_accepted = E_SUCCESS;

  /* Report the previous subframe's transmission status */
  hdrpcp_report_prev_subframe_status( &combo_info );

  /* If ACMAC or RMAC wants to flush SLP callbacks the pkt_ptr will be NULL */
  if( pkt_ptr == NULL || max_bytes == 0 )
  {
    (void) hdrslp_get_pkt( TRUE, 0, time_since_prev_req, &tx_pkt_ptr, &channel );

    HDR_MSG_PROT_1( 
      MSG_LEGACY_MED, "Flushing SLP callbacks %d", time_since_prev_req );

    return 0;
  }

  if( channel == HDRHAI_AC )
  {
    /* Spoof the MAC flow allocations structure */
    mac_flows[0].mac_flow_id = HDRPCP_SLP_FLOW_ID;
    mac_flows[0].size = max_bytes;

    /* Spoof the carrier build info structure */
    carrier_build_info.carrier_id = 0;
    carrier_build_info.carrier_no = 0;
    carrier_build_info.combo_no = 0;
    carrier_build_info.mac_flows_ptr = mac_flows;
    carrier_build_info.num_mac_flows = 1;
    carrier_build_info.size_limit = max_bytes;
    carrier_build_info.max_pkt_size = max_bytes;
    carrier_build_info.pkt_ptr = pkt_ptr;

#ifdef FEATURE_LTE_TO_HDR_OH
    curr_tunnel_mode = hdrsap_get_current_tunnel_mode_value(
                              HDRHAI_PACKET_CONSOLIDATION_PROTOCOL);

    if ( (curr_tunnel_mode == HDRHAI_TUNNEL_MODE_ACTIVE_HO) ||
         (curr_tunnel_mode == HDRHAI_TUNNEL_MODE_DISABLED) )
    {
      HDR_MSG_PROT(MSG_LEGACY_HIGH, "LTE_HDR_OH: Tunnel is not enabled"); 
#endif /* FEATURE_LTE_TO_HDR_OH*/
      /* Need to collect the AMAC data first */
      allowed_bytes = MIN( HDRPCP_FMT_B_MAX_PKT_SIZE, max_bytes -  1 );

      bytes_to_tx = 
        hdrslp_get_common_ac_pkt( 
          allowed_bytes, time_since_prev_req, &tx_pkt_ptr, &channel );

      if( bytes_to_tx > 0 )
      {
        (void) hdrpcp_build_packet( 
          tx_pkt_ptr, bytes_to_tx, &carrier_build_info, NULL );
      }
#ifdef FEATURE_LTE_TO_HDR_OH
    }
    else
    {
      HDR_MSG_PROT(MSG_LEGACY_HIGH, "LTE_HDR_OH: Tunnel is enabled, Skip Common AC"); 
    }
#endif /* FEATURE_LTE_TO_HDR_OH*/

    /* Initialize the SLP packet application */
    hdrpcp_slp_pkt_app_init_ctrl_blk( channel, time_since_prev_req );

    /* Collect any pending SLP data */
    (void) hdrpcp_collect_data_on_given_carrier( 0, &carrier_build_info );

    /* Write back the collected values */
    pkt_size = carrier_build_info.pkt_len;
    *pkt_ptr = *(carrier_build_info.pkt_ptr);
    *is_fmt_b_ptr = (boolean) carrier_build_info.pkt_format;
#ifdef FEATURE_LTE_TO_HDR_OH
    /* pkt handle is not used for AC packet */
    hdrpcp.curr_pkt_handle = carrier_build_info.pkt_handle; 
#endif /* FEATURE_LTE_TO_HDR_OH */
  }
  else /* channel == HDRHAI_RTC */
  {
    mac_flows[0].mac_flow_id = HDRPCP_SLP_FLOW_ID;
    mac_flows[1].mac_flow_id = hdrpcp.rmac_flow_id;

    if( hdrpcp.rmac_flow_id == 0 )
    {
      num_rtc_mac_flows = 1;
    }
    else
    {
      num_rtc_mac_flows = 2;
    }

    /* Get the upcoming subframe's info and queue any packet applications
       with high priority retransmit data */
    hdrpcp_get_packet_info( mac_flows, num_rtc_mac_flows );

    /* Display the amount of pending data for each flow */
    HDR_MSG_PROT_2( MSG_LEGACY_MED, "=TX= MAC Flow ID: 0x%x, bytes pending: %d",
      mac_flows[0].mac_flow_id, mac_flows[0].size );

    if( num_rtc_mac_flows > 1 )
    {
      HDR_MSG_PROT_2( MSG_LEGACY_MED, "=TX= MAC Flow ID: 0x%x, bytes pending: %d",
        mac_flows[1].mac_flow_id, mac_flows[1].size );
    }

    /* Simulate RMAC3 algorithm being run on the 'size' */
    mac_flows[0].size = max_bytes;
    mac_flows[1].size = max_bytes;


    /* Spoof DoRB data structures for call to new APIs to collect data */
    combo_data.combo_id = 0;
    combo_data.num_carriers = 1;

    combo_data.carrier_data[0].carrier_id = 0;
    combo_data.carrier_data[0].recommended_info.mac_flows = mac_flows;

    combo_data.carrier_data[0].recommended_info.num_mac_flows = num_rtc_mac_flows;
    combo_data.carrier_data[0].recommended_info.max_pkt_size = max_bytes;

    combo_data.carrier_data[0].built_data.tx_pkt_ptr = *pkt_ptr;

    /* Build the packet, legacy interface always report subframe 0 */
    (void) hdrpcp_get_packet( &combo_data, 1, 0 /* num_combos */ );

    /* Write back the collected values */
    pkt_size = combo_data.carrier_data[0].built_data.actual_pkt_size;
    *pkt_ptr = combo_data.carrier_data[0].built_data.tx_pkt_ptr;
    *is_fmt_b_ptr = (boolean) combo_data.carrier_data[0].built_data.pkt_format;
#ifdef FEATURE_LTE_TO_HDR_OH
    hdrpcp.curr_pkt_handle = combo_data.carrier_data[0].built_data.pkt_handle;
#endif /* FEATURE_LTE_TO_HDR_OH */
  }

  return pkt_size;

} /* hdrpcp_get_pkt() */

/*===========================================================================

FUNCTION HDRPCP_PUT_PKT

DESCRIPTION
  This function is called by the legacy protocols to put received data to
  the packet applications.

DEPENDENCIES
  None.

PARAMETERS
  pkt_ptr   - The data received
  is_fmt_b  - Indicates if the data received is Format A or Format B

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrpcp_put_pkt
(
  dsm_item_type *pkt_ptr,
  boolean is_fmt_b
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Overloading the is_fmt_b boolean for HDRDSM_PACKET_IS_FORMAT_B */
  HDRDSM_SET_CONN_FORMAT( pkt_ptr, is_fmt_b );

  /* Mark this packet as the last of simultaneous packets */
  HDRDSM_MARK_LAST_OF_SIMUL_PACKETS( pkt_ptr );

  /* Call the new Forward Link API */
  hdrpcp_put_packet( pkt_ptr );
  
} /* hdrpcp_put_pkt() */

/*===========================================================================

FUNCTION HDRPCP_PACKET_FAILED

DESCRIPTION
  This function is called by RMAC to give an M-ARQ indication.

DEPENDENCIES
  None.

PARAMETERS
  pkt_handle  - The packet handle of the transmission that failed.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrpcp_packet_failed
(
  hdrpcp_pkt_handle_type pkt_handle
)
{
  uint8 carrier_id = HDRPCP_GET_PKT_HANDLE_CARRIER_ID( pkt_handle );

  /* Need to adjust the buffer by the M-ARQ buffer offset */
  uint8 marq_buffer = 
    HDRPCP_GET_PKT_HANDLE_MARQ_BUFFER( pkt_handle ) - HDRPCP_MARQ_BUFFER_OFFSET;

  uint8 flow = 0;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(( carrier_id < HDRPCP_MAX_CARRIERS ) &&
         ( marq_buffer < HDRPCP_MAX_MARQ_BUFFERS ));

  if (hdrpcp.verbose_debug)
  {
    HDR_MSG_PROT_1(MSG_LEGACY_MED, "=TX=  MARQ pkt_fail hdl:0x%04x",
                 pkt_handle);
  }

  rex_enter_crit_sect(&hdrpcp.tx_crit_sect);

  for( flow = 0; flow < HDRPCP_MAX_TX_FLOWS; flow++ )
  {
    hdrpcp_tx_flow_type *tx_flow_ptr = &hdrpcp.flows.tx_flows[flow];
    void *tx_set_id = tx_flow_ptr->marq_buffers[marq_buffer][carrier_id];

    if (hdrpcp.verbose_debug)
    {
      HDR_MSG_PROT_2( MSG_LEGACY_MED, "=TX=  MARQ pkt_fail flow:%d, TX SET:0x%x", 
                    hdrpcp.flows.tx_flows[flow].tx_flow_id, tx_set_id);
    }

    if( tx_set_id )
    {
      if( tx_flow_ptr->tx_failed_func_ptr )
      {
        hdrpcp_tx_failed_func_type tx_failed_func_ptr = tx_flow_ptr->tx_failed_func_ptr;
        void *user_data_ptr = tx_flow_ptr->user_data_ptr;

        /* To avoid deadlock, we leave TX critical section when processing RLP/MRLP
         * failure callback. */
        rex_leave_crit_sect(&hdrpcp.tx_crit_sect);
        tx_failed_func_ptr( tx_set_id, user_data_ptr );
        rex_enter_crit_sect(&hdrpcp.tx_crit_sect);
      }
    }

    tx_flow_ptr->marq_buffers[marq_buffer][carrier_id] = NULL;
  }

  rex_leave_crit_sect(&hdrpcp.tx_crit_sect);

} /* hdrpcp_packet_failed() */

/*===========================================================================

FUNCTION HDRPCP_REPORT_PREV_SUBFRAME_STATUS

DESCRIPTION
  This function is called by RMAC to report the status of the previous 
  subframe's transmission.

DEPENDENCIES
  None.

PARAMETERS
  subframe_count - The current subframe count.
  chosen_combo  - The combo chosen by RMAC last subframe

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrpcp_report_prev_subframe_status
(
  hdrpcp_prev_subframe_combo_info_type *chosen_combo
)
{ 
  uint8 num_carriers = 0;
  uint8 carrier = 0;
  uint8 combo_id = 0;
  uint8 combo_val = HDRPCP_MAX_COMBOS;
  uint8 combo = 0;

  uint8 flow = 0;
  uint8 marq_buffer = hdrpcp.pkt_history_info.current_marq_buffer;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  combo_id = chosen_combo->combo_id;
  num_carriers = chosen_combo->num_carriers;

  if (hdrpcp.verbose_debug)
  {
    HDR_MSG_PROT_2( MSG_LEGACY_HIGH, "=TX= report prev tx status %d, %d", 
                    combo_id, num_carriers );
  }

  hdrpcp_log_data_init();

  if (hdrpcp.logging_enabled)
  {
    hdrpcp.log_data.prev_chosen_combo_id = combo_id;
  }

  /* Search for the actual combo value; combo IDs provided do not 
     necessarily come as an indexable, increasing sequence from [0, MAX] */
  for( combo = 0; combo < HDRPCP_MAX_COMBOS; combo++ )
  {
    if( hdrpcp.pkt_history_info.combo_id_table[combo] == combo_id )
    {
      combo_val = combo;
      break;
    }
  }

  if( combo_val == HDRPCP_MAX_COMBOS )
  {
    HDR_MSG_PROT_1( MSG_LEGACY_FATAL, "Combo ID: %d not found!", combo_id );
    return;
  }

  /* Loop through all the registered flows and build the carrier
     tx outcome array to report back the status of the previous transmission */
  rex_enter_crit_sect(&hdrpcp.tx_crit_sect);

  for( flow = 0; flow < HDRPCP_MAX_TX_FLOWS; flow++ )
  {
    uint8 i = 0;
    uint8 num_outcomes = 0;
    hdrpcp_carrier_tx_outcome_type carrier_tx_outcome[HDRPCP_MAX_TX_SETS];
    hdrpcp_tx_flow_type *tx_flow_ptr = &hdrpcp.flows.tx_flows[flow];

    memset(carrier_tx_outcome, 0, sizeof(carrier_tx_outcome));
    /* Only continue if the flow is in use */
    if( tx_flow_ptr->tx_flow_id != (uint16) HDRPCP_FREE_FLOW_ENTRY )
    {
      if( combo_id != HDRPCP_NO_COMBO_ID_ACCEPTED )
      {
        for( carrier = 0; carrier < num_carriers; carrier++ )
        {
          hdrpcp_pkt_info_per_carrier_type *crr_info_ptr = &chosen_combo->carrier_info[carrier];
          uint8 carrier_id = crr_info_ptr->carrier_id;
          void *tx_set_id = tx_flow_ptr->sent_id_history[combo_val][carrier_id];
          hdrpcp_pkt_handle_type pkt_handle = crr_info_ptr->pkt_handle;

          if (hdrpcp.logging_enabled)
          {
            hdrpcp.log_data.combos[combo_val].carriers[carrier].prev_sf_canceled = 
              (crr_info_ptr->pkt_accepted == E_CANCELED);
          }
  
          if( pkt_handle == HDRPCP_IDLE_PKT_HANDLE &&
              hdrpcp.data_collected[carrier_id] == TRUE )
          {
            /* Need to guarantee that idle packets have sets all canceled */
            HDR_MSG_PROT( MSG_LEGACY_FATAL, "Possible encoder window miss!" );
          }
  
          /* Reset the data collected flag for this carrier */
          hdrpcp.data_collected[carrier_id] = FALSE;

          if (hdrpcp.verbose_debug)
          {
            if( tx_set_id != NULL )
            {
              HDR_MSG_PROT_6( MSG_LEGACY_MED,
                              "=TX=  MARQ report combo:%d flow:%d marq:%d accept:%d "
                              "hdl:0x%04x TX SET:0x%x",
                              combo_id, tx_flow_ptr->tx_flow_id, marq_buffer, 
                              crr_info_ptr->pkt_accepted, pkt_handle, tx_set_id );
            }
          }

          if( tx_flow_ptr->always_notify_packet_status ||
              /* if the handle is non-NULL and we sent data */
              ( tx_set_id != NULL ) )
          {
            ASSERT( i < HDRPCP_MAX_TX_SETS );
			/*Checking possible out of bound array acees when i>=HDRPCP_MAX_TX_SETS*/

            carrier_tx_outcome[i].last_rri = crr_info_ptr->rri;
            carrier_tx_outcome[i].tx_set_accepted = crr_info_ptr->pkt_accepted;
            carrier_tx_outcome[i].tx_set_id = tx_set_id;
            carrier_tx_outcome[i].carrier_id = crr_info_ptr->carrier_id;
  
            i++;
          }

          if( (crr_info_ptr->pkt_accepted != E_SUCCESS) ||
              (crr_info_ptr->pkt_handle == HDRPCP_LEGACY_PKT_HANDLE) )
          {
            tx_set_id = NULL;
          }

          /* Store the accepted value into the M-ARQ buffer */
          tx_flow_ptr->marq_buffers[marq_buffer][carrier_id] = tx_set_id;
  
          /* Clear out the sent id history value */
          tx_flow_ptr->sent_id_history[combo_val][carrier_id] = NULL;
        }
  
        num_outcomes = i;
      } /* if combo_id != HDRPCP_NO_COMBO_ID_ACCEPTED */
  
      ASSERT( tx_flow_ptr->tx_status_func_ptr );

      /* We must have at least one outcome to report or no combos accepted */
      if( num_outcomes > 0 || combo_id == HDRPCP_NO_COMBO_ID_ACCEPTED )
      {
        if (hdrpcp.verbose_debug)
        {
          HDR_MSG_PROT_1( MSG_LEGACY_MED, "=TX= call tx status cb, flow_id %d",
                          tx_flow_ptr->tx_flow_id );
        }
        tx_flow_ptr->tx_status_func_ptr( 
          combo_id, carrier_tx_outcome, num_outcomes, tx_flow_ptr->user_data_ptr );
      }
    }
  }

  rex_leave_crit_sect(&hdrpcp.tx_crit_sect);

  /* Clean up packet history */
  memset( hdrpcp.pkt_history_info.combo_id_table, 0, 
          sizeof(hdrpcp.pkt_history_info.combo_id_table) );

  hdrpcp.pkt_history_info.current_marq_buffer = 
    (marq_buffer + 1) % HDRPCP_MAX_MARQ_BUFFERS;
} /* hdrpcp_report_prev_subframe_status() */

/*===========================================================================

FUNCTION HDRPCP_GET_PACKET_INFO

DESCRIPTION
  This function is called by RMAC to determine the amount of pending data
  each packet app has to send.

DEPENDENCIES
  None.

PARAMETERS
  mac_allocs_ptr  - Contains the MAC flow ID/size pairs
  num_mac_flows   - The number of MAC flows

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrpcp_get_packet_info
(
  hdrpcp_mac_flow_allocation_type *mac_allocs_ptr,
  uint8 num_mac_flows
)
{
  uint8 mac_flow = 0;
  uint8 flow = 0;

  uint16 time_since_prev_req = 0;

  uint32 num_pkts_per_flow[HDRPCP_MAX_MAC_FLOWS];
  uint32 num_bytes_per_flow[HDRPCP_MAX_MAC_FLOWS];

  uint32 tot_bytes_pending = 0;
  uint32 tot_pkts_pending = 0;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT(num_mac_flows <= HDRPCP_MAX_MAC_FLOWS);

  memset(num_pkts_per_flow, 0, sizeof(num_pkts_per_flow));
  memset(num_bytes_per_flow, 0, sizeof(num_bytes_per_flow));

  if( hdrpcp.first_use_of_roll )
  {
    hdrpcp.roll_count = 0;

#ifdef FEATURE_LTE_TO_HDR_OH
    hdrsaptx_register_frame_timer_callback( hdrpcp_roll_cb );
#endif /* FEATURE_LTE_TO_HDR_OH */
    hdrts_register_roll_callback( hdrpcp_roll_cb );

    hdrpcp.first_use_of_roll = FALSE;
  }

  REX_DISABLE_PREMPTION();
  time_since_prev_req = hdrpcp.roll_count;
  hdrpcp.roll_count = 0;
  REX_ENABLE_PREMPTION();
  
  if (hdrpcp.verbose_debug)
  {
    HDR_MSG_PROT_1( MSG_LEGACY_MED, "=TX=, get_packet_info, time_since %d", 
                    time_since_prev_req);
  }

  /* Initialize the SLP packet application's control block */
  hdrpcp_slp_pkt_app_init_ctrl_blk( HDRHAI_RTC, time_since_prev_req );

  if ( hdralmp_connection_is_open(HDRHAI_PACKET_CONSOLIDATION_PROTOCOL) )
  {
    /* Queue RLP NAK messages if necessary */
    hdrrlp_queue_msg( time_since_prev_req );
    hdrmrlp_queue_msg( time_since_prev_req );
  }

  rex_enter_crit_sect(&hdrpcp.tx_crit_sect);

  for( mac_flow = 0; mac_flow < num_mac_flows; mac_flow++ )
  {
    uint8 mac_flow_id = mac_allocs_ptr[mac_flow].mac_flow_id;

    for( flow = 0; flow < HDRPCP_MAX_TX_FLOWS; flow++ )
    {
      uint32 num_pkts_pending = 0;
      uint32 num_bytes_pending = 0;
      hdrpcp_tx_flow_type *tx_flow_ptr = &hdrpcp.flows.tx_flows[flow];
      
      if( (tx_flow_ptr->tx_flow_id != (uint16) HDRPCP_FREE_FLOW_ENTRY) &&
          (tx_flow_ptr->mac_flow_id == mac_flow_id) &&
          (tx_flow_ptr->get_data_size_func_ptr != NULL) )
      {
        num_bytes_pending = 
          tx_flow_ptr->get_data_size_func_ptr(
            &num_pkts_pending, tx_flow_ptr->user_data_ptr );

        num_bytes_per_flow[mac_flow] += num_bytes_pending;
        num_pkts_per_flow[mac_flow] += num_pkts_pending;

        tx_flow_ptr->bytes_pending = num_bytes_pending;
      }
  
      /* If the HP data flag is set, enqueue this tx flow onto its
         MAC flow's hp data queue */
      if( tx_flow_ptr->has_hp_data_pending  &&
          !tx_flow_ptr->enqueued_for_hp_data )
      {
        hdrpcp_mac_flow_type *mac_flow_ptr = (hdrpcp_mac_flow_type *)
          hdrpcp_find_flow( HDRPCP_MAC_FLOW_TYPE, tx_flow_ptr->mac_flow_id );
  
        if( mac_flow_ptr )
        {
          hdrpcp_queue_item_type *q_item_ptr = hdrpcp_queue_get_item( &hdrpcp.free_q );
          ASSERT( q_item_ptr );
 
          q_item_ptr->data = tx_flow_ptr;
          hdrpcp_queue_put_item( 
            &mac_flow_ptr->flows_queue[HDRPCP_HI_DATA_PRIORITY], q_item_ptr );
        }
        else
        {
          HDR_MSG_PROT_1( MSG_LEGACY_ERROR, 
            "MAC Flow ID: 0x%x does not exist?", tx_flow_ptr->mac_flow_id );
        }

        tx_flow_ptr->has_hp_data_pending = FALSE;
        tx_flow_ptr->enqueued_for_hp_data = TRUE;
      }

    } /* end loop over tx flows */

    mac_allocs_ptr[mac_flow].size = num_bytes_per_flow[mac_flow];
    
    tot_bytes_pending += num_bytes_per_flow[mac_flow];
    tot_pkts_pending += num_pkts_per_flow[mac_flow];

    if (hdrpcp.verbose_debug)
    {
      HDR_MSG_PROT_3( MSG_LEGACY_MED, 
        "=TX= MAC Flow ID: 0x%x tot_pkts %d tot_bytes %d",
                      mac_flow, tot_pkts_pending, tot_bytes_pending );
    }
  } /* end loop over mac flows */

  rex_leave_crit_sect(&hdrpcp.tx_crit_sect);

  if( tot_pkts_pending == 1 &&
      hdrpcp_is_ps_val_equal_to_bytes_pending(tot_bytes_pending) )
  {
    /* Add no bytes because we found the exact MAC size */
  }
  else
  {
    /* SLP flow is always reserved on MAC flow 0; we add one
       byte per packet for the PCP Format B header */
    mac_allocs_ptr[0].size += num_pkts_per_flow[0];

    /* MAC flows reserved for data begin at 1 */
    for( mac_flow = 1; mac_flow < num_mac_flows; mac_flow++ )
    {
      if( mac_allocs_ptr[mac_flow].size > HDRPCP_FMT_B_MAX_PKT_SIZE )
      {
        /* Format B packets are at most 255 bytes; if the pending size
           is greater than 255 we need to break it up into 255 byte chunks
           to estimate the number of PCP packets required to transmit */
        num_pkts_per_flow[mac_flow] = 
          (mac_allocs_ptr[mac_flow].size / HDRPCP_FMT_B_MAX_PKT_SIZE) + 1;
      }

      /* Add one byte per packet to account for the PCP header estimate */
      mac_allocs_ptr[mac_flow].size += num_pkts_per_flow[mac_flow];
    }
  }

} /* hdrpcp_get_packet_info() */

/*===========================================================================

FUNCTION HDRPCP_GET_PACKET

DESCRIPTION
  This function is called by RMAC to collect data for transmission.

DEPENDENCIES
  None.

PARAMETERS
  per_combo_data  - The different combo/carrier pair allocations
  num_combos      - The number of combos to build

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
uint32 hdrpcp_get_packet
(
  hdrpcp_build_data_per_combo_type *per_combo_data,
  uint8 num_combos,
  uint8 subframe_count
)
{
  hdrpcp_log_data_type *log_data_ptr = &(hdrpcp.log_data);

  uint8 combo_id = 0;

  uint8 combo = 0;
  uint8 carrier = 0;
  uint8 num_carriers = 0;

  uint32 num_pkts_this_carrier = 0;

  hdrpcp_per_carrier_data_type *carrier_ptr = NULL;
  hdrpcp_carrier_build_info_type build_info;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset(&build_info, 0, sizeof(build_info));

  if (!hdrpcp.logging_enabled)
  {
    hdrpcp_log_data_init();
  }

  log_data_ptr->sub_frame_count = subframe_count;
  log_data_ptr->num_combos = num_combos;

  for( combo = 0; combo < num_combos; combo++ )
  {
    /* Combo id is not necessarily [0,2] */
    combo_id = per_combo_data[combo].combo_id;
   
    /* So we need the combo id for lookup later */
    hdrpcp.pkt_history_info.combo_id_table[combo] = combo_id;
    
    /* Number of carriers this combo can carry */
    num_carriers = per_combo_data[combo].num_carriers;

    /* Store logging information for this combo */
    log_data_ptr->combos[combo].combo_id = combo_id;
    log_data_ptr->combos[combo].num_carriers = num_carriers;

    for( carrier = 0; carrier < num_carriers; carrier++ )
    {
      carrier_ptr = &per_combo_data[combo].carrier_data[carrier];
      carrier_ptr->built_data.tx_pkt_ptr = NULL;

      /* Setup the build info for this carrier */
      build_info.combo_no = combo;
      build_info.carrier_no = carrier;

      build_info.carrier_id = carrier_ptr->carrier_id;
    
      build_info.mac_flows_ptr = carrier_ptr->recommended_info.mac_flows;
      build_info.num_mac_flows = carrier_ptr->recommended_info.num_mac_flows;

      build_info.size_limit = carrier_ptr->recommended_info.max_pkt_size;
      build_info.max_pkt_size = carrier_ptr->recommended_info.max_pkt_size;
      build_info.payload_len = 0;
      build_info.pkt_len = 0;
      build_info.pkt_ptr = &carrier_ptr->built_data.tx_pkt_ptr;

      /* Run the data collection algorithm on this carrier */
      num_pkts_this_carrier = 
        hdrpcp_collect_data_on_given_carrier( combo_id, &build_info );

      /* Write out the collected values for this carrier */
      carrier_ptr->built_data.actual_pkt_size = build_info.pkt_len;
      carrier_ptr->built_data.tx_pkt_ptr = *build_info.pkt_ptr;
      carrier_ptr->built_data.pkt_handle = build_info.pkt_handle;
      carrier_ptr->built_data.pkt_format = build_info.pkt_format;
      carrier_ptr->built_data.num_pkts = num_pkts_this_carrier;
   
      /* This flag is set here and cleared in report previous subframe
         only if the data is encoded (the subsequent frame is not IDLE) */
      if( build_info.payload_len > 0 )
      {
        hdrpcp.data_collected[carrier] = TRUE;
        log_data_ptr->combos[combo].carriers[carrier].carrier_id = carrier_ptr->carrier_id;
        log_data_ptr->combos[combo].carriers[carrier].is_fmt_b = (boolean) build_info.pkt_format;
      }
    }
  }

  hdrpcp_log_data_commit();
 
  return 0;

} /* hdrpcp_get_packet() */

/*=============================================================================

FUNCTION HDRPCP_PUT_PACKET

DESCRIPTION
  This function is called to parse and deliver the received frame from the AN.
  Format A frames will be sent in their entirety to the receiving layers. If
  the frame is format B, the frame will be demuxed into its respective packets
  and each will be sent individually to the receiving layers. This function is
  part of the new Rev. A interface.

DEPENDENCIES
  None.

PARAMETERS
  rx_pkt_ptr  - The DSM item chain containing the received data
  
RETURN VALUE
  None.

SIDE EFFECTS
  Format B frames will have their individual packets duped and then at the
  end of execution the entire frame will be freed.
  
=============================================================================*/
void hdrpcp_put_packet
(
  dsm_item_type *rx_pkt_ptr
)
{
  uint16 frame_length = 0;
  uint16 frame_offset = 0;

  boolean pkt_parsed = FALSE;
  boolean eos_received = FALSE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* The message mask is used to indicate which messages belong in the
     same connection layer packet; it must be reset for each new
     connection layer packet received */
  hdrpcp.msg_mask = 0;

  if( rx_pkt_ptr == NULL )
  {
    HDR_MSG_PROT( MSG_LEGACY_ERROR, "put_packet() with a NULL frame" );
    return;
  }

  frame_length = dsm_length_packet( rx_pkt_ptr );

  eos_received = HDRDSM_IS_LAST_OF_SIMUL_PACKETS( rx_pkt_ptr );

  if( HDRDSM_IS_CONN_FORMAT_A( rx_pkt_ptr ) )
  {
    if (hdrpcp.verbose_debug)
    {
      HDR_MSG_PROT( MSG_LEGACY_MED, "=RX=  Rx'd format a frame (rev. A)" );
    }
    
    pkt_parsed = hdrpcp_parse_packet_header( rx_pkt_ptr,
      &frame_offset, frame_length, HDRPCP_PACKET_FORMAT_A );
  }
  else
  {
    if (hdrpcp.verbose_debug)
    {
      HDR_MSG_PROT( MSG_LEGACY_MED, "=RX=  Rx'd format b frame (rev. A)" );
    }
    
    do
    {
      pkt_parsed = hdrpcp_parse_packet_header( rx_pkt_ptr,
        &frame_offset, frame_length, HDRPCP_PACKET_FORMAT_B );

    } while( frame_offset < frame_length && pkt_parsed );
    
    dsm_free_packet( &rx_pkt_ptr );
  }

  rex_enter_crit_sect(&hdrpcp.rx_crit_sect);

  if( eos_received )
  {
    uint8 flow = 0;
    for( flow = 0; flow < HDRPCP_MAX_RX_FLOWS; flow++ )
    {
      hdrpcp_rx_flow_type *rx_flow_ptr = &hdrpcp.flows.rx_flows[flow];
      if( rx_flow_ptr->put_data_this_frame && rx_flow_ptr->eos_notif_func_ptr )
      {
        if (hdrpcp.verbose_debug)
        {
          HDR_MSG_PROT_1( MSG_LEGACY_MED, "eos notification, flow %d", flow );
        }
        rx_flow_ptr->eos_notif_func_ptr( rx_flow_ptr->user_data_ptr );
      }

      rx_flow_ptr->put_data_this_frame = FALSE;
    }
  }

  rex_leave_crit_sect(&hdrpcp.rx_crit_sect);

  if (hdrpcp.verbose_debug)
  {
    if( pkt_parsed )
    {
      HDR_MSG_PROT( MSG_LEGACY_MED, "=RX=  Rx'd frame successfully processed" );
    }
    else
    {
      HDR_MSG_PROT( MSG_LEGACY_ERROR, "=RX=  Could not process rx'd frame" );
    }
  }

} /* hdrpcp_put_packet() */

/*=============================================================================

FUNCTION HDRPCP_REGISTER_RECEIVE_CALLBACKS

DESCRIPTION
  This function is called by each packet app/stream to register a set of 
  callbacks to handle data received by the MAC layer.
  
DEPENDENCIES
  None.

PARAMETERS
  stream_id           - The stream associated with the packet app
  put_data_func_ptr   - The callback to handle received data
  eos_notif_func_ptr  - The callback to give an End-of-Slot indication
  user_data_ptr       - Pointer to the packet app's control block

RETURN VALUE
  TRUE if the packet app/stream is successfully registered; FALSE otherwise.

SIDE EFFECTS
  None.
  
=============================================================================*/
boolean hdrpcp_register_receive_callbacks
(
  uint8 stream_id,
  hdrpcp_put_data_func_type put_data_func_ptr,
  hdrpcp_eos_notification_func_type eos_notif_func_ptr,
  hdrpcp_rx_buf_empty_notification_func_type rx_buf_empty_notif_func_ptr,
  void *user_data_ptr
)
{
  hdrpcp_rx_flow_type *rx_flow_ptr = NULL;
  boolean reg_is_successful = FALSE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  HDR_MSG_PROT_1( MSG_LEGACY_MED, "REG RX stream %d", stream_id);

  ASSERT( put_data_func_ptr != NULL );
  
  #ifdef FEATURE_HDRPCP_DORMANCY_CHECK
  #error code not present
#endif

  rex_enter_crit_sect(&hdrpcp.rx_crit_sect);

  rx_flow_ptr = (hdrpcp_rx_flow_type *) 
    hdrpcp_find_free_flow( HDRPCP_RX_FLOW_TYPE );

  if( rx_flow_ptr )
  {
    rx_flow_ptr->stream_id = stream_id;
    rx_flow_ptr->put_data_func_ptr = put_data_func_ptr;
    rx_flow_ptr->eos_notif_func_ptr = eos_notif_func_ptr;
    rx_flow_ptr->rx_buf_empty_notif_func_ptr = rx_buf_empty_notif_func_ptr;
    rx_flow_ptr->user_data_ptr = user_data_ptr;

    rx_flow_ptr->rx_flow_id = stream_id;

    reg_is_successful = TRUE;
  }

  rex_leave_crit_sect(&hdrpcp.rx_crit_sect);

  return reg_is_successful;

} /* hdrpcp_register_receive_callbacks() */

/*=============================================================================

FUNCTION HDRPCP_REGISTER_TRANSMIT_CALLBACKS

DESCRIPTION
  This function is called by each packet app to register a set of callbacks for
  data transmission on the reverse link. 
  
DEPENDENCIES
  None.

PARAMETERS
  substream_id   - The substream ID of the packet app
  mac_flow_id    - The MAC flow ID of the registering packet app
  stream_id      - The stream ID of the registering packet app
  get_data_size_func_ptr  - The callback to determine the data pending
  get_hp_data_func_ptr    - The callback to request high priority data
  get_lp_data_func_ptr    - The callback to request low priority data
  tx_status_func_ptr      - The callback to report the status of a tranmission
  tx_failed_func_ptr      - The callback to give an M-ARQ indication
  always_notify_tx_status - Set if the packet app always wants the status
                            notification regardless of transmitting data
  user_data_ptr           - Pointer to the packet app's control block
  
RETURN VALUE
  TRUE if the packet app is sucessfully registered; FALSE otherwise.

SIDE EFFECTS
  None.
  
=============================================================================*/
boolean hdrpcp_register_transmit_callbacks
(
  uint8 substream_id,
  uint8 mac_flow_id,
  uint8 stream_id,
  hdrpcp_get_data_size_func_type get_data_size_func_ptr,
  hdrpcp_get_data_func_type get_hp_data_func_ptr,
  hdrpcp_get_data_func_type get_lp_data_func_ptr,
  hdrpcp_tx_status_func_type tx_status_func_ptr,
  hdrpcp_tx_failed_func_type tx_failed_func_ptr,
  boolean always_notify_tx_status,
  void *user_data_ptr
)
{
  hdrpcp_mac_flow_type *mac_flow_ptr = NULL;
  hdrpcp_tx_flow_type *tx_flow_ptr = NULL;
  hdrpcp_queue_item_type *q_item_ptr = NULL;
  boolean reg_is_successful = FALSE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT_4( MSG_LEGACY_MED,
                  "REG TX stream %d/%d mac %d data 0x%x",
                  stream_id, substream_id, mac_flow_id,
                  (uint32) user_data_ptr);

  #ifdef FEATURE_HDRPCP_DORMANCY_CHECK
  #error code not present
#endif

  rex_enter_crit_sect(&hdrpcp.tx_crit_sect);

  hdrpcp.rmac_flow_id = mac_flow_id;

  tx_flow_ptr = (hdrpcp_tx_flow_type *) 
    hdrpcp_find_free_flow( HDRPCP_TX_FLOW_TYPE );

  if( tx_flow_ptr )
  {
    /* Store packet app details */
    tx_flow_ptr->stream_id = stream_id;
    tx_flow_ptr->substream_id = substream_id;
    tx_flow_ptr->mac_flow_id = mac_flow_id;
    tx_flow_ptr->user_data_ptr = user_data_ptr;

    /* Generate tx_flow_id for internal use */
    tx_flow_ptr->tx_flow_id = HDRPCP_GEN_TX_FLOW_ID( stream_id, substream_id );

    /* Store function callbacks */
    tx_flow_ptr->tx_failed_func_ptr = tx_failed_func_ptr;
    tx_flow_ptr->tx_status_func_ptr = tx_status_func_ptr;
    tx_flow_ptr->get_data_size_func_ptr = get_data_size_func_ptr;

    tx_flow_ptr->\
      get_data_func_ptr[HDRPCP_HI_DATA_PRIORITY] = get_hp_data_func_ptr;

    tx_flow_ptr->\
      get_data_func_ptr[HDRPCP_LO_DATA_PRIORITY] = get_lp_data_func_ptr;

    tx_flow_ptr->always_notify_packet_status = always_notify_tx_status;

    reg_is_successful = TRUE;
  }

  if( reg_is_successful )
  {
    /* Find the MAC flow associated with this tx flow */
    mac_flow_ptr = (hdrpcp_mac_flow_type *)
      hdrpcp_find_flow( HDRPCP_MAC_FLOW_TYPE, mac_flow_id );
  
    if( mac_flow_ptr == NULL )
    {
      /* If this MAC flow doesn't exist, create a new entry */
      mac_flow_ptr = (hdrpcp_mac_flow_type *)
        hdrpcp_find_free_flow( HDRPCP_MAC_FLOW_TYPE );

      ASSERT( mac_flow_ptr );

      mac_flow_ptr->mac_flow_id = mac_flow_id;    
    }

    /* Get an empty queue item from the free queue */
    q_item_ptr = hdrpcp_queue_get_item( &hdrpcp.free_q );
    ASSERT( q_item_ptr );

    /* Store a pointer to the newly registered flow */
    q_item_ptr->data = tx_flow_ptr;
  
    /* Queue the new flow for data collection */
    hdrpcp_queue_put_item( &mac_flow_ptr->flows_queue[HDRPCP_LO_DATA_PRIORITY], q_item_ptr );
  }
  else
  {
    HDR_MSG_PROT( MSG_LEGACY_ERROR,
                  "REG TX FAILED!" );
  }

  rex_leave_crit_sect(&hdrpcp.tx_crit_sect);

  return reg_is_successful;

} /* hdrpcp_register_transmit_callbacks() */

/*=============================================================================

FUNCTION HDRPCP_UNREGISTER_TRANSMIT_CALLBACKS

DESCRIPTION
  This function is called by a registered packet app to unregister its set of 
  callbacks from PCP.
  
DEPENDENCIES
  None.

PARAMETERS
  stream_id     - The stream ID of the registered packet app to unregister
  substream_id  - The substream ID of the registered packet app to unregister

RETURN VALUE
  A boolean TRUE indicates the unregistration was successful; a boolean FALSE
  indicates the unregistration failed.

SIDE EFFECTS
  None.
  
=============================================================================*/
boolean hdrpcp_unregister_transmit_callbacks
(
  uint8 stream_id,
  uint8 substream_id
)
{
  hdrpcp_tx_flow_type *tx_flow_ptr = NULL;
  hdrpcp_mac_flow_type *mac_flow_ptr = NULL;
  hdrpcp_queue_item_type *q_item_ptr = NULL;
  uint16 tx_flow_id = 0;
  boolean unreg_is_successful = FALSE;

  int flow = 0;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  HDR_MSG_PROT_2( MSG_LEGACY_MED, "UNREG TX stream %d/%d",
                  stream_id, substream_id );

  rex_enter_crit_sect(&hdrpcp.tx_crit_sect);

  if( substream_id == HDRPCP_UNREGISTER_ALL_SUBSTREAMS )
  {
    for( flow = 0; flow < HDRPCP_MAX_TX_FLOWS; flow++ )
    {
      tx_flow_ptr = &hdrpcp.flows.tx_flows[flow];
      if( tx_flow_ptr->stream_id == stream_id )
      {
        mac_flow_ptr = (hdrpcp_mac_flow_type *)
          hdrpcp_find_flow( HDRPCP_MAC_FLOW_TYPE, tx_flow_ptr->mac_flow_id );

        HDR_MSG_PROT_3( MSG_LEGACY_MED, 
                        "UNREG TX substream %d mac %d data 0x%x",
                        tx_flow_ptr->substream_id, 
                        tx_flow_ptr->mac_flow_id,
                        (uint32) tx_flow_ptr->user_data_ptr );

        if( mac_flow_ptr )
        {
          q_item_ptr = hdrpcp_queue_del_item(
            &mac_flow_ptr->flows_queue[HDRPCP_HI_DATA_PRIORITY], tx_flow_ptr );

          if( q_item_ptr )
          {
            q_item_ptr->data = NULL;
            hdrpcp_queue_put_item( &hdrpcp.free_q, q_item_ptr );
          }

          q_item_ptr = hdrpcp_queue_del_item(
            &mac_flow_ptr->flows_queue[HDRPCP_LO_DATA_PRIORITY], tx_flow_ptr );

          if( q_item_ptr )
          {
            q_item_ptr->data = NULL;
            hdrpcp_queue_put_item( &hdrpcp.free_q, q_item_ptr );
          }

          if( (mac_flow_ptr->flows_queue[HDRPCP_HI_DATA_PRIORITY].count == 0 ) &&
              (mac_flow_ptr->flows_queue[HDRPCP_LO_DATA_PRIORITY].count == 0 ) )
          {
            mac_flow_ptr->mac_flow_id = (uint8) HDRPCP_FREE_FLOW_ENTRY;
          }
        }

        memset( tx_flow_ptr, 0, sizeof(hdrpcp_tx_flow_type) );
        tx_flow_ptr->tx_flow_id = (uint16) HDRPCP_FREE_FLOW_ENTRY;
      }
    }

    unreg_is_successful = TRUE;
  }
  else
  {
    tx_flow_id = HDRPCP_GEN_TX_FLOW_ID( stream_id, substream_id );
    tx_flow_ptr = (hdrpcp_tx_flow_type *)
      hdrpcp_find_flow( HDRPCP_TX_FLOW_TYPE, tx_flow_id );

    if( tx_flow_ptr )
    {
      mac_flow_ptr = (hdrpcp_mac_flow_type *)
        hdrpcp_find_flow( HDRPCP_MAC_FLOW_TYPE, tx_flow_ptr->mac_flow_id );
      
      if( mac_flow_ptr )
      {
        HDR_MSG_PROT_2( MSG_LEGACY_MED, 
                        "UNREG TX mac %d data 0x%x",
                        tx_flow_ptr->mac_flow_id,
                        (uint32) tx_flow_ptr->user_data_ptr );
        q_item_ptr = 
          hdrpcp_queue_del_item( 
            &mac_flow_ptr->flows_queue[HDRPCP_HI_DATA_PRIORITY], tx_flow_ptr );

        if( q_item_ptr )
        {
          q_item_ptr->data = NULL;
          hdrpcp_queue_put_item( &hdrpcp.free_q, q_item_ptr );
        }

        q_item_ptr =
          hdrpcp_queue_del_item( 
            &mac_flow_ptr->flows_queue[HDRPCP_LO_DATA_PRIORITY], tx_flow_ptr );

        if( q_item_ptr )
        {
          q_item_ptr->data = NULL;
          hdrpcp_queue_put_item( &hdrpcp.free_q, q_item_ptr );
        }

        if( (mac_flow_ptr->flows_queue[HDRPCP_HI_DATA_PRIORITY].count == 0 ) &&
            (mac_flow_ptr->flows_queue[HDRPCP_LO_DATA_PRIORITY].count == 0 ) )
        {
          mac_flow_ptr->mac_flow_id = (uint8) HDRPCP_FREE_FLOW_ENTRY;
        }
      }

      memset( tx_flow_ptr, 0, sizeof(hdrpcp_tx_flow_type) );
      tx_flow_ptr->tx_flow_id = (uint16) HDRPCP_FREE_FLOW_ENTRY;
            
      unreg_is_successful = TRUE;
    }
  }

  rex_leave_crit_sect(&hdrpcp.tx_crit_sect);

  if( !unreg_is_successful )
  {
    HDR_MSG_PROT( MSG_LEGACY_HIGH, 
                  "UNREG TX FAILED!" );
  }

  return unreg_is_successful;

} /* hdrpcp_unregister_transmit_callbacks() */

/*=============================================================================

FUNCTION HDRPCP_UNREGISTER_RECEIVE_CALLBACKS

DESCRIPTION
  This function is called by the app layer to unregister the set of callbacks 
  for a particular stream.
  
DEPENDENCIES
  None.

PARAMETERS
  stream_id  - The ID of the stream being unregistered.

RETURN VALUE
  A boolean TRUE indicates the unregistration was successful; a boolean FALSE
  indicates the unregistration failed.

SIDE EFFECTS
  After a successful unregistration, the callbacks for the unregistered stream
  will be set to NULL.
  
=============================================================================*/
boolean hdrpcp_unregister_receive_callbacks
(
  uint8 stream_id
)
{
  hdrpcp_rx_flow_type *rx_flow_ptr = NULL;
  boolean unreg_is_successful = FALSE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  HDR_MSG_PROT_1( MSG_LEGACY_MED, "UNREG RX stream %d", stream_id);

  rex_enter_crit_sect(&hdrpcp.rx_crit_sect);

  rx_flow_ptr = (hdrpcp_rx_flow_type *) 
    hdrpcp_find_flow( HDRPCP_RX_FLOW_TYPE, stream_id );

  if( rx_flow_ptr )
  {
    rx_flow_ptr->eos_notif_func_ptr = NULL;
    rx_flow_ptr->rx_buf_empty_notif_func_ptr = NULL;
    rx_flow_ptr->put_data_func_ptr = NULL;
    rx_flow_ptr->user_data_ptr = NULL;
    rx_flow_ptr->stream_id = 0;
    
    /* Mark this RX flow as a free flow entry  */
    rx_flow_ptr->rx_flow_id = (uint8) HDRPCP_FREE_FLOW_ENTRY;
    
    unreg_is_successful = TRUE;
  }

  rex_leave_crit_sect(&hdrpcp.rx_crit_sect);

  return unreg_is_successful;

} /* hdrpcp_unregister_receive_callbacks() */

/*===========================================================================

FUNCTION HDRPCP_GET_MSG_MASK

DESCRIPTION
  This function is used by HMP to check if particular message is in the 
  current connection layer packet. 

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  uint32  - The current msg_mask

SIDE EFFECTS
  None.

===========================================================================*/
uint32 hdrpcp_get_msg_mask
( 
  void /* no arguments */
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return hdrpcp.msg_mask;

} /* hdrpcp_get_msg_mask() */

/*===========================================================================

FUNCTION HDRPCP_SET_MSG_MASK

DESCRIPTION
  This function is used by HMP to add a msg bit to hdrpcp.msg_mask to 
  indicate if a particular message is in the current connection layer packet.

DEPENDENCIES
  None.

PARAMETERS
  msg_mask  - The message bit mask 

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrpcp_add_msg_mask
(
  uint32 msg_mask
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  hdrpcp.msg_mask |= msg_mask;

} /* hdrpcp_add_msg_mask() */


/*===========================================================================

FUNCTION HDRPCP_NOTIFY_APP_HAS_HP_DATA

DESCRIPTION
  This function is called by a packet app to notify PCP that it has high
  priority data pending.

DEPENDENCIES
  None.

PARAMETERS
  substream_id   - The substream ID of the packet app
  stream_id      - The stream ID of the packet app
  user_data_ptr  - Pointer to the packet app's control block

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrpcp_notify_app_has_hp_data
(
  uint8 substream_id,
  uint8 stream_id,
  void *user_data_ptr
)
{
  hdrpcp_tx_flow_type *tx_flow_ptr = NULL;
  uint16 tx_flow_id = 0;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Generate the TX flow ID used internally by PCP from the 
     stream and substream IDs */
  tx_flow_id = HDRPCP_GEN_TX_FLOW_ID( stream_id, substream_id );

  rex_enter_crit_sect(&hdrpcp.tx_crit_sect);

  tx_flow_ptr = (hdrpcp_tx_flow_type *)    
    hdrpcp_find_flow( HDRPCP_TX_FLOW_TYPE, tx_flow_id );

  if( tx_flow_ptr )
  {
    /* Assert that this is the same flow PCP has stored internally */
    ASSERT( tx_flow_ptr->user_data_ptr == user_data_ptr );

    /* Set the has HP data pending flag to TRUE; during a call to
       hdrpcp_get_packet_info() at the start of the next subframe,
       if this flag is set this TX flow will be enqueued to its MAC
       flow's high priority queue */
    tx_flow_ptr->has_hp_data_pending = TRUE;
  }
  else
  {
    HDR_MSG_PROT_2( MSG_LEGACY_ERROR,
      "Cannot find TX flow w/ sid: %d, ssid: %d to enqueue as HP",
      stream_id, substream_id );
  }

  rex_leave_crit_sect(&hdrpcp.tx_crit_sect);

} /* hdrpcp_notify_app_has_hp_data() */

/*===========================================================================

FUNCTION HDRPCP_RX_BUF_EMPTY

DESCRIPTION
  This function is used by RMAC to notify RLP that all consecutive packets
  in RX buffer have been processed.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrpcp_rx_buf_empty( void )
{
  uint8 flow = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* notify all registered RX flow of rx buf empty */

  rex_enter_crit_sect(&hdrpcp.rx_crit_sect);

  for( flow = 0; flow < HDRPCP_MAX_RX_FLOWS; flow++ )
  {
    if (hdrpcp.flows.rx_flows[flow].rx_buf_empty_notif_func_ptr != NULL)
    {
      if (hdrpcp.verbose_debug)
      {
        HDR_MSG_PROT_1( MSG_LEGACY_MED, 
                        "Buf empty notification, flow %d",flow);
      }
      hdrpcp.flows.rx_flows[flow].rx_buf_empty_notif_func_ptr(
          hdrpcp.flows.rx_flows[flow].user_data_ptr);
    }
  }

  rex_leave_crit_sect(&hdrpcp.rx_crit_sect);

}

/*===========================================================================
FUNCTION HDRPCP_SET_VERBOSE_DEBUG

DESCRIPTION
  This function initializes the PCP verbose debug setting .
  
DEPENDENCIES
  None.

PARAMETERS
  verbose_debug_setting - current verbose debug setting.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
void hdrpcp_set_verbose_debug 
( 
  uint8 verbose_debug_setting 
)
{
  hdrpcp.verbose_debug = verbose_debug_setting;
} /* hdrpcp_set_verbose_debug() */

/*===========================================================================

FUNCTION HDRPCP_GET_CURR_PKT_HANDLE

DESCRIPTION
  This function is called by SAPTX to get current packet handle 

DEPENDENCIES
  This functin need to be called immediately after calling hdrpcp_get_pkt().

PARAMETERS
  pkt_handle  - The packet handle of the last transmission 

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrpcp_get_curr_pkt_handle
(
  hdrpcp_pkt_handle_type *pkt_handle
)
{
#ifdef FEATURE_LTE_TO_HDR_OH
  *pkt_handle = hdrpcp.curr_pkt_handle;
  HDR_MSG_PROT_1( MSG_LEGACY_HIGH, "curr pkt handle: 0x%04x", *pkt_handle );
#else
  HDR_MSG_PROT( MSG_LEGACY_ERROR, 
                  "hdrpcp_get_curr_pkt_handle() should not be called!" );
#endif /* FEATURE_LTE_TO_HDR_OH */

} /* hdrpcp_get_curr_pkt_handle() */

/*===========================================================================

FUNCTION HDRPCP_RESET_ROLL_CB

DESCRIPTION
  This function deregisters roll callback

DEPENDENCIES
  This function must be called after connection is torn down.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrpcp_reset_roll_cb( void )
{
  hdrpcp.first_use_of_roll = TRUE;
  hdrts_deregister_roll_callback(hdrpcp_roll_cb);
} /* hdrpcp_reset_roll_cb() */
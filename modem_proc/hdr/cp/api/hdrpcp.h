/*===========================================================================

      H D R   P A C K E T   C O N S O L I D A T I O N   P R O T O C O L
                           D E F I N I T I O N S

DESCRIPTION
  This file contains the externalized declarations and definitions for the 
  1x-EVDO Release B Packet Consolidation Protocol.

  Copyright (c) 2007 - 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/api/hdrpcp.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/04/13   cnx     Deregister roll callback when connection is closed.
01/05/12   sju     Check trans id when SAPTX receives ACK for UL messages.
01/03/12   sju     Added support to enable verbose debug f3 at runtime. 
10/24/11   smd     Moved more to SU API header files.
10/20/11   wsh     Changed PCP init code path to avoid double init
09/23/11   smd     SU API cleanup.
07/27/11   cnx     Merged Optimized Handoff changes.
06/22/10   wsh     Fixed race condition when reg/unreg PCP flows
02/09/10   wsh     Notify RLP when RX buffer is empty
03/26/03   wsh     Moved subframe_count to hdrpcp_get_packet()
11/10/08   smd     Replaced errno.h with IxErrno.h  
01/14/08   cc      Added argument "subframe_count" in
                   hdrpcp_report_prev_subframe_status.
11/06/07   cws     Added HDRPCP_RRI_CANCELED definition.
10/16/07   hal     Added fields used by MAC for logging
06/27/07   hal     Initial version

===========================================================================*/

#ifndef HDRPCP_H
#define HDRPCP_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

/*-----------------------------------------------------
  Common AMSS include files
-----------------------------------------------------*/

#include "hdr_variation.h"
#include "comdef.h"

#ifndef FEATURE_HDRPCP_UNITTEST
#include "dsm.h"
#include "IxErrno.h"
#endif /* FEATURE_HDRPCP_UNITTEST */

/*-----------------------------------------------------
  HDR specific include files
-----------------------------------------------------*/

#ifndef FEATURE_HDRPCP_UNITTEST
#include "hdrhai.h"
#endif /* FEATURE_HDRPCP_UNITTEST */

#include "hdrcp_api.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*-----------------------------------------------------
  Definitions and macros
-----------------------------------------------------*/

/*
 * SLP flows are always given an identifier of 0 for the 
 * MAC flow ID, stream ID and substream ID.
 *
 */
#define HDRPCP_SLP_FLOW_ID   0x00



/*
 * If EMPA support is enabled, the number of TX flows
 * handled in PCP is doubled.
 *
 */
#ifdef FEATURE_HDR_EMPA
  #define HDRPCP_MAX_TX_FLOWS 20
#else
  #define HDRPCP_MAX_TX_FLOWS 10
#endif /* FEATURE_HDR_EMPA */

/*
 * Each TX flow has a TX set per carrier (although this 
 * max sets concept is currently unused in PCP).
 *
 */
#define HDRPCP_MAX_TX_SETS (HDRPCP_MAX_CARRIERS * HDRPCP_MAX_TX_FLOWS)

/*
 * Currently, PCP is set to support 8 MAC flows (1 signaling
 * and 7 data) and handles 4 RX flows (mapped to the 4 forward
 * link streams)
 *
 */
#define HDRPCP_MAX_MAC_FLOWS 8
#define HDRPCP_MAX_RX_FLOWS  4

#define HDRPCP_ARG_NOT_USED( arg ) if( (arg) ) { }

 /*
 * Flag indicates that the RRI for the previous transmission attempt was 
 * invalid and that the previous transmission was still continuing.
 *
 */
#define HDRPCP_RRI_CANCELED              0xFF

/* Flag indicates that the RRI variable is not used*/
#define HDRPCP_RRI_NOT_USED              0xFE
/*-----------------------------------------------------
  Data structures and types
-----------------------------------------------------*/

/*
 * hdrpcp_bundle_enum_type
 *
 * This enum is a bit mask so each value must be a power of 2.
 * It is used in HMP to check if messages are bundled in same 
 * connection layer packet.
 *
 */
typedef enum
{
  HDRPCP_BUNDLE_NONE = 0,
  HDRPCP_BUNDLE_CON_CLOSE_MSG = 1,
  HDRPCP_BUNDLE_TCA_MSG = 2,
  HDRPCP_BUNDLE_MAX = (int)0xFFFFFFFFuL

} hdrpcp_bundle_enum_type;

/*
 * hdrpcp_pkt_handle_type
 *
 * Packet handles are a bitpacked combination of:
 *   carrier_id | marq_buffer -- although this may need to change
 * 
 */
typedef uint16 hdrpcp_pkt_handle_type;

/*
 * hdrpcp_pkt_format_enum_type
 *
 * An enum indicating the type of packet returned by/
 * given to PCP.
 *
 */
typedef enum
{
  HDRPCP_PACKET_FORMAT_A = 0,
  HDRPCP_PACKET_FORMAT_B = 1,

} hdrpcp_pkt_format_enum_type;

/*
 * hdrpcp_pkt_info_per_carrier_type
 *
 * Each carrier will need to have its status reported independently;
 * the status includes the carrier ID, handle, RRI and whether it was
 * accepted or canceled.
 *
 */
typedef struct
{
  uint8 carrier_id;
  hdrpcp_pkt_handle_type pkt_handle;
  uint8 rri;
  errno_enum_type pkt_accepted;

} hdrpcp_pkt_info_per_carrier_type;

/*
 * hdrpcp_prev_subframe_combo_info_type
 *
 * This struct contains the status of the previously chosen and
 * and transmitted combination.
 *
 */
typedef struct
{
  uint8 combo_id;
  uint8 num_carriers;
  hdrpcp_pkt_info_per_carrier_type carrier_info[HDRPCP_MAX_CARRIERS];

} hdrpcp_prev_subframe_combo_info_type;

/*
 * hdrpcp_mac_flow_allocation_type
 *
 * The allocations for each MAC flow (given a particular MAC flow ID).
 *
 */
typedef struct
{
  uint8 mac_flow_id;
  uint32 size;

} hdrpcp_mac_flow_allocation_type;

/*
 * hdrpcp_per_carrier_recommended_info_type
 *
 * The recommended allotments suggest by RMAC for each carrier.
 *
 */
typedef struct
{
  hdrpcp_mac_flow_allocation_type *mac_flows;
  uint8 num_mac_flows;
  uint16 max_pkt_size;

} hdrpcp_per_carrier_recommended_info_type;

/*
 * hdrpcp_per_carrier_built_data_type
 *
 * The data built for each carrier consists of a DSM item containing
 * the data, the actual packet size (in octets), a handle to this data,
 * and the format (A or B).
 *
 */
typedef struct
{
  dsm_item_type *tx_pkt_ptr;
  uint16 actual_pkt_size; /* octets */
  uint16 num_pkts; /* number of packets in this MAC packet */
  uint16 num_mac_flows_with_data; /* number of MAC flows sending data */
  hdrpcp_pkt_handle_type pkt_handle;
  hdrpcp_pkt_format_enum_type pkt_format;

} hdrpcp_per_carrier_built_data_type;

/*
 * hdrpcp_per_carrier_data_type
 *
 * The carrier specific data is broken down into two categories:
 *   recommended_info - provided by RMAC to determine flow allotments, and
 *   build_data - filled in by PCP with the data collected from 
 *                the packet applications.
 *
 */
typedef struct
{
  uint8 carrier_id;
  hdrpcp_per_carrier_recommended_info_type recommended_info;
  hdrpcp_per_carrier_built_data_type built_data;

} hdrpcp_per_carrier_data_type;

/*
 * hdrpcp_build_data_per_combo_type
 *
 * The data built for each transmit combo.
 *
 */
typedef struct
{
  uint8 combo_id;
  uint8 num_carriers;
  hdrpcp_per_carrier_data_type carrier_data[HDRPCP_MAX_CARRIERS];

} hdrpcp_build_data_per_combo_type;




/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

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
extern uint32 hdrpcp_init
( 
  void /* no arguments */
);

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
extern uint16 hdrpcp_get_pkt
(
  uint16 max_bytes,
  hdrhai_channel_enum_type channel,
  uint16 time_since_prev_req,
  dsm_item_type **pkt_ptr,
  boolean *is_fmt_b_ptr
);

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
extern void hdrpcp_put_pkt
(
  dsm_item_type *pkt_ptr,
  boolean is_fmt_b
);

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
extern void hdrpcp_packet_failed
(
  hdrpcp_pkt_handle_type pkt_handle
);

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
extern void hdrpcp_get_packet_info
(
  hdrpcp_mac_flow_allocation_type *mac_flows_ptr,
  uint8 num_mac_flows
);

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
extern void hdrpcp_report_prev_subframe_status
(
  hdrpcp_prev_subframe_combo_info_type *chosen_combo
);

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
extern uint32 hdrpcp_get_packet
(
  hdrpcp_build_data_per_combo_type *per_combo_data,
  uint8 num_combos,
  uint8 subframe_count
);

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
extern void hdrpcp_put_packet
(
  dsm_item_type *rx_pkt_ptr
);

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
extern boolean hdrpcp_register_receive_callbacks
(
  uint8 stream_id,
  hdrpcp_put_data_func_type put_data_func_ptr,
  hdrpcp_eos_notification_func_type eos_notif_func_ptr,
  hdrpcp_rx_buf_empty_notification_func_type rx_buf_empty_notif_func_ptr,
  void *user_data_ptr
);

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
extern boolean hdrpcp_register_transmit_callbacks
(
  uint8 substream_id,
  uint8 mac_flow_id,
  uint8 stream_id,
  hdrpcp_get_data_size_func_type get_data_size_func_ptr,
  hdrpcp_get_data_func_type get_hp_data_func_ptr,
  hdrpcp_get_data_func_type get_lp_data_func_ptr,
  hdrpcp_tx_status_func_type tx_status_func_ptr,
  hdrpcp_tx_failed_func_type tx_failed_func_ptr,
  boolean always_notify_packet_status,
  void *user_data_ptr
);

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
extern void hdrpcp_notify_app_has_hp_data
(
  uint8 substream_id,
  uint8 stream_id,
  void *user_data_ptr
);

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
extern boolean hdrpcp_unregister_transmit_callbacks
(
  uint8 stream_id,
  uint8 substream_id
);

/*=============================================================================

FUNCTION HDRPCP_UNREGISTER_RECEIVE_CALLBACKS

DESCRIPTION
  This function is called by the app layer to unregister the set of callbacks 
  for a particular stream.
  
DEPENDENCIES
  None.

PARAMETERS
  stream_id  - The ID of the stream being unregistered

RETURN VALUE
  A boolean TRUE indicates the unregistration was successful; a boolean FALSE
  indicates the unregistration failed.

SIDE EFFECTS
  After a successful unregistration, the callbacks for the unregistered stream
  will be set to NULL.
  
=============================================================================*/
extern boolean hdrpcp_unregister_receive_callbacks
(
  uint8 stream_id
);

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
extern void hdrpcp_roll_cb
( 
  void /* no arguments */
);

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
extern uint32 hdrpcp_get_msg_mask
( 
  void /* no arguments */
);

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
extern void hdrpcp_add_msg_mask
(
  uint32 msg_mask
);

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
extern void hdrpcp_rx_buf_empty( void );

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
extern void hdrpcp_get_curr_pkt_handle
(
  hdrpcp_pkt_handle_type *pkt_handle
);

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
extern void hdrpcp_set_verbose_debug 
( 
  uint8 verbose_debug_setting 
);

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
void hdrpcp_reset_roll_cb( void );

#endif /* HDRPCP_H */


#ifndef HDRDEC_H
#define HDRDEC_H

/** @file hdrdec.h
    @brief This module contains definitions and declarations necessary for 
           using the HDR Decoder driver. */

/*===========================================================================
  Copyright (c) 2000 - 2012 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/drivers/api/hdrdec.h#1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/08/14   rmv     Updated FL Packet Header log packet to include packet
                   cancelled status and Preamble Detection type 
03/20/12   cnx     Resolved TCB compiler errors for Dime. 
03/16/12   arm     Resolve TCB errors for Dime. 
10/27/11   vlc     Fixed compiler errors. 
07/19/11   lyl     Added hdrdec_get_last_ftc_pkt_hstr();
06/01/11   cnx     Fixed compiler error.
05/31/11   cnx     Added hdrdec_get_packet_info().
02/20/10   kss     Added interface for setting logging cb.
10/07/09   etv     Moved some defines from src to header to make it available
                   for test module.
08/12/09   wsh     Reverted CMI related change due to restructuring
07/28/09   etv     Removed hdrmsg.h and moved event typedefs to hdrdec.c
07/15/09   etv     Moved sw module definitions to hdrmsg.h
05/21/09   etv     Moved Decoder header logging to HDRDEC module.
03/10/09   etv     Created HDR Decoder task, MSG interface with fw and 
                   Doxygen commenting style.
03/16/09   etv     Added hdrdec_init_clock_speed.
03/23/09   wsh     CMI: featurized CMI under FEATURE_CMI
02/02/09   etv     Added support for QPCH packet.
08/20/07   etv     Changed "carrier_id" to "demod_id"
08/06/07   etv     Removed the FEATURE_HDR_REVB in decoder header type.
07/11/07   etv     Updated hdrdec_pkt_type_enum_type for new Type definition.
07/06/07   etv     Include new MUP types in hdrdec_pkt_type_enum_type.
04/05/07   etv     Updated for Rev-B support.
09/18/06   mt      Added forced preamble detection status field to decoder info.
09/15/06   etv     Added hdrdec_flush_packet.
09/08/06   ljl     Added hdrdec_uninstall_isr() and 
                   hdrdec_reset_and_deactivate().
10/10/05   jyw     Added mac_index field to the pkt info struct.
07/26/05   etv     Added user-defined Rel-A CC in hdrdec_pkt_type_enum_type.
04/15/05   kss     Changed drop_reason to crc_status, corrected typos.
04/05/05   kss     Rev A changes; move macros to hdrdeci.h.
03/28/05   hrk     Added prototype for function hdrdec_pre_deactivate().
04/26/04   kss     Added macro for detecting enhanced broadcast packets.
03/07/04   kss     Use correct register macro in get_pkt functions;
                   added pkt_is_bcc and pkt_is_ftc check macros.
05/15/03   kss     Reduced min LLR check to 0.
04/04/03   kss     Restore original min LLR check of 1.
03/04/03   kss     Added HDRDEC_GET_CRC() macro, reduced min LLR check to 0.
08/03/02   kss     Added macro to check header integrity, also corrected the
                   metric (now MIN_LLR) macro.
07/31/02   kss     Fixed problem with get_metric -- it was looking at the 
                   energy metric rather than the min LLR data.
03/03/02   kss     Added the hdrdec_set_isr() function.
09/16/01   kss     Implemented hdrdec_get_pkt() as a macro, added macro for
                   slots remaining.
09/16/01   kss     Implemented hdrdec_get_pkt() as a macro.
06/20/01   kss     Added packet sequence macros.
04/23/01   kss     Modify for MSM5500; added MSM5500-specific macros.
03/29/01   vas     Added simulator code (featurized FEATURE_HDR_MOBILE_SIM)
02/20/01   kss     Get frame size from DRC.
10/23/00   kss     Added code optimizations. 
10/11/00   kss     Code review update.
09/25/00   kss     Converted the header-information extraction functions to
                   macros for better efficiency.
09/01/00   kss     Added more functions for retrieving header info.
05/01/00   kss     Created.

===========================================================================*/
/**
    @defgroup hdrdec HDR Decoder Module */
/*\{*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "comdef.h"       /* Needed for byte, uint16, uint32, etc. */
#include "msm.h"          /* Needed for MSM macros */
#include "hdrsrchtypes.h" /* SRCH types for link Id and SchedGroupID */

#include "hdrbuf.h"

/*==========================================================================
                               MACROS
===========================================================================*/

/** Address of the TCB for the task in which this protocol runs */
extern rex_tcb_type*    hdrdec_tcb_ptr;
/** Pointer to HDRDEC task TCB */
#define HDRDEC_TASK_PTR hdrdec_tcb_ptr
/** @def   HDRDEC_RPT_TIMER_SIG
    @brief Watchdog signal */
#define HDRDEC_RPT_TIMER_SIG                                0x00000001

/** @def   HDRDEC_EVENT_SIG
    @brief Signal for any events (Command or Message) in the event buffer */
#define HDRDEC_EVENT_SIG                                    0x00000002

/** @def HDRDEC_ENTER_HDR_SIG
    @brief Signal to enter HDR online mode */
#define  HDRDEC_ENTER_HDR_SIG                               0x00000040

/** @def HDRDEC_EXIT_HDR_SIG
    @brief Signal to exit HDR online mode */
#define  HDRDEC_EXIT_HDR_SIG                                0x00000080

/** Decoder Interrupt rate (in slots) during Idle. DSP will interrupt ARM
    after every packet decoded */
#define HDRDEC_IDLE_DECOB_MSG_RATE_V                        1

/** Decoder Interrupt Rate (in slots) during traffic. DSP will not interrupt
    ARM faster than once every 4 slots */
#define HDRDEC_TRAFFIC_DECOB_MSG_RATE_V                     4

#ifndef FEATURE_HDR_MPS_MIN_PREAMBLE_THRESH

/** Minimum Preamble detection threshold during Traffic  */
#define HDRDEC_MIN_PREAMBLE_THRESH_TRAFFIC                  2048 

/** Minimum Preamble detection threshold during Idle  */
#define HDRDEC_MIN_PREAMBLE_THRESH_IDLE                     8192

#endif /* FEATURE_HDR_MPS_MIN_PREAMBLE_THRESH */

/*==========================================================================
                               TYPEDEFS 
===========================================================================*/
/** @fn     hdrdec_shutdown_complete_cb_type
    @brief  Function pointer type for callbacks used to notify that decoder 
            shutdown has completed. */
typedef void (*hdrdec_shutdown_complete_cb_type) ( void );

/** @enum hdrdec_decoder_mode_e_type
    @brief Enum of all modes supported by decoder */
typedef enum
{
  HDRDEC_MONITOR_MODE = 0,
    /**< AT is in Idle */
  HDRDEC_TRAFFIC_MODE = 1,
    /**< AT is in traffic */
  HDRDEC_NUM_MODES
    /**< Total number of decoder modes */

} hdrdec_decoder_mode_e_type;


/** @enum  hdrdec_pkt_type_e_type
    @brief Enum of all decoder recognized forward link packet types */
typedef enum
{
  HDRDEC_UNI_PKT        = 0x00,   
    /**< Single-user Traffic Channel Packet */
  HDRDEC_MUL_PKT_BASE_0 = 0x08,   
    /**< Multi-user Traffic Channel Packet decoded at Mac index 
         MUPPreambleBase */
  HDRDEC_MUL_PKT_BASE_1 = 0x09,   
    /**< Multi-user Traffic Channel Packet decoded at Mac index 
         MUPPreambleBase + 1 */
  HDRDEC_MUL_PKT_BASE_2 = 0x0A,   
    /**< Multi-user Traffic Channel Packet decoded at Mac index 
         MUPPreambleBase + 2 */
  HDRDEC_MUL_PKT_BASE_3 = 0x0B,   
    /**< Multi-user Traffic Channel Packet decoded at Mac index 
         MUPPreambleBase + 3 */
  HDRDEC_MUL_PKT_BASE_4 = 0x0C,   
    /**< Multi-user Traffic Channel Packet decoded at Mac index 
         MUPPreambleBase + 4 */
  HDRDEC_CC_0_PKT       = 0x10,   
    /**< Release-0 (std.) Control Channel Packet */
  HDRDEC_CC_A_PKT       = 0x11,   
    /**< Rev-A (short) Control channel packet */
  HDRDEC_USER_CC_A_PKT  = 0x12,   
    /**< Rev-A Control channel packet received at user-defined Short
        packet MAC index */
  HDRDEC_QPCH_PKT       = 0x13,   
    /**< Quick Page packet */
  HDRDEC_BCC_PKT        = 0x18,   
    /**< Broadcast */
  HDRDEC_BCC_ENH_PKT    = 0x19,   
    /**< Enhanced Broadcast */
  HDRDEC_PB_PKT         = 0x1A    
    /**< Platinum Broadcast */

} hdrdec_pkt_type_e_type;


/*! @enum  hdrdec_pkt_channel_e_type
    @brief Enum of all forward link channel types */
typedef enum
{
  HDRDEC_FTC_UNICAST = 0x0,
    /**< Unicast Forward Traffic Channel */

  HDRDEC_FTC_MUP     = 0x1,
    /**< Multicast Forward Traffic Channel */

  HDRDEC_CC          = 0x2,
    /**< Control channel */

  HDRDEC_BCC         = 0x3,
    /**< Broadcast channel */

  HDRDEC_MAX_NUM_FL_CHANNELS
    /**< Maximum number of forward link channels */

} hdrdec_pkt_channel_e_type;


/** @enum  hdrdec_crc_status_e_type
    @brief Enum of all CRC statuses. */
typedef enum
{
  HDRDEC_CRC_FAIL            = 0x0,
    /**< CRC failure */

  HDRDEC_CRC_PASS            = 0x1,
    /**< CRC pass */

  HDRDEC_CRC_FALSE_PASS      = 0x2,
    /**< False CRC pass */

  HDRDEC_NUM_CRC_STATUS
    /**< Maximum Number of CRC statuses */

} hdrdec_crc_status_e_type;

/** @enum  hdrdec_pd_e_type
 *  @brief Enum of all Preamble Detection types */
typedef enum
{
  HDRDEC_PD_NORMAL            = 0x0,
    /**< Normal Preamble Detection */

  HDRDEC_PD_FORCED_CC         = 0x1,
    /**< Control Channel Forced Preamble Detection */

  HDRDEC_PD_FORCED_TC         = 0x2,
    /**< Traffic Channel Forced Preamble Detection */

  HDRDEC_NUM_PD_TYPES
    /**< Maximum Number of Preamble Detection types */

} hdrdec_pd_e_type;

/** @enum  hdrdec_pkt_status_e_type
    @brief Enumerations of all possible decoded packet status */
typedef enum
{
  HDRDEC_PKT_WAITING                      = 0,
    /**< Waiting for packet */
  HDRDEC_CC_PKT_READY                     = 1,
    /**< Packet has been received */
  HDRDEC_FTC_PKT_READY                    = 2,
    /**< Packet has been received */
  HDRDEC_PKT_CANCELED                     = 3,
    /**< Packet was canceled or had failed CRC */
#ifdef FEATURE_HDR_BCMCS
  HDRDEC_BCC_PKT_RXD                      = 4,
    /**< Broadcast packet received */
#endif
  HDRDEC_CC_PKT_DELIVERED_ON_ARRIVAL      = 5
    /**< CC Packet to be delivered on arrival */
} hdrdec_pkt_status_e_type;

/** @typedef hdrdec_fl_drc_type 
    @brief   Data type for Forward link data rate */
typedef uint16 hdrdec_fl_drc_type;

/** @typedef hdrdec_sequence_num_type
    @brief   Data type for decoder sequence number */
typedef uint32 hdrdec_sequence_num_type;

/** @typedef hdrdec_half_slot_time_type
    @brief   Data type for half slot time reference */
typedef uint16 hdrdec_half_slot_time_type;

/** @struct hdrdec_pkt_info_s_type
    @brief  Structure containing decoder packet info, filled in for client */
typedef struct 
{
  boolean pkt_is_cancelled;
    /**< Has this packet been cancelled?  */
  uint8 demod_id;
    /**< Demod index on which the packet was received */
  uint16 num_mac_pkts;
    /**< Number of MAC packets in the decoder frame */
  int byte_length;
    /**< Length of decoder packet in bytes */
  hdrdec_fl_drc_type drc_decoded;
    /**< Decoded DRC */
  hdrdec_fl_drc_type drc_requested;
    /**< Requested DRC */
  hdrsrch_link_id_type link_id;
    /**< Link ID for packet's serving sector */
  hdrdec_sequence_num_type sequence;
    /**< Sequence number for packet */
  hdrdec_half_slot_time_type hstr;
    /**< HSTR at which packet was detected */
  uint16 slots_to_decode;
    /**< Number of slots taken to decode packet */
  hdrdec_pkt_type_e_type pkt_type; 
    /**< Packet type */
  hdrdec_pkt_channel_e_type channel;
    /**< Packet channel (CC, FTC, BC) */
  hdrdec_crc_status_e_type crc_status;
    /**< CRC status */
  boolean rev_a;
    /**< TRUE if Rev A/B packet and FALSE if Release 0 packet */
  hdrdec_pd_e_type sw_forced_preamble;
    /**< Indicates Preamble Detection type used to decode packet 
         Normal, CC FPD, TC FPD */
  hdrsrch_schdgrp_gid_type sched_group_id;
    /**< Scheduler Group ID */
  uint8 num_concurrent_pkts;
    /**< Number of concurrent packets detected on the same slot */
} hdrdec_pkt_info_s_type;


/*===========================================================================

                          FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION HDRDEC_INIT

DESCRIPTION
  Initialized the decoder and decoder driver by initiating a load of the
  Frame Format Table, and setting the watermark and hunt byte values to
  the defaults.
 
DEPENDENCIES
  The decoder/demod clock must already be set up and enabled.
  
PARAMETERS
  None.  

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrdec_init( void );

/*============================================================================
  HDRDEC_ACTIVATE
*/
/**
  Activates the decoder.

  If called from a task other than HDRDEC, 
    - posts command HDRDEC_ACTIVATE_CMD to HDRDEC task to activate the decoder
  otherwise 
    - Enable turbo decoder clocks. 
    - Reset the turbo decoder hardware, setup turbo decoder mode for HDR mode. 

  @assumptions hdrdec_init() called already at power-up. 

  @context HDRDEC

  @see hdrdec_init() */
/*===========================================================================*/
extern void hdrdec_activate ( void );

/*============================================================================
  HDRDEC_RESET_PKT_SEQUENCES
*/
/**
  Reset the decoder sequence numbers; 

  If called from a task other than HDRDEC, 
    - posts command HDRDEC_RESET_SEQ_CMD to HDRDEC task to reset the decoder 
    sequence space. 
  otherwise 
    - Reset the mDSP sequence space. 
    - Reset the decoder re-sequencing buffer 

  @context HDRDEC */
/*===========================================================================*/
extern void hdrdec_reset_pkt_sequences ( void );


/*============================================================================
  HDRDEC_GET_NEXT_PKT
*/
/**
  Returns the next packet available to serve from the resequence buffer.

  @context HDRRX task.

  \internal 
  There is a potential race condition here between HDRDEC and HDRRX tasks in 
  updating the resequencing buffer and internal pointers. Need to use rex 
  critical section variables to prevent race condition 

  @return TRUE if a valid packet was returned and FALSE otherwise */
/*===========================================================================*/
extern boolean hdrdec_get_next_pkt 
(
  dsm_item_type** pkt_ptr,
    /**< Next packet in resequencing buffer to be delivered */
  hdrdec_pkt_status_e_type *pkt_status,
    /**< Decode status of the packet to be delivered */
  int16 *pkt_slot_time 
    /**< Slot time of the packet to be delivered */
);


/*============================================================================
  HDRDEC_ENABLE_ON_ARRIVAL_DELIVERY
*/
/**
  Enables on arrival delivery of control channel packets 

  There are two modes of RX packet delivery to upper layers.  
    - In-order delivery: A circular buffer is used to resequence the packets 
      and packets are not delivered until all their predecessors have been 
      delivered.  
    - On-Arrival delivery: An high priority queue is maintained to deliver 
      packets as and when they arrive. This mode applies only to control channel 
      packets. 
  Mode 1 is default mode. This function is used to turn ON the second mode.  

  @context Calling task */
/*===========================================================================*/
extern void hdrdec_enable_on_arrival_delivery ( void );


/*============================================================================
  HDRDEC_DISABLE_ON_ARRIVAL_DELIVERY
*/
/**
  Disables on arrival delivery of control channel packets 

  There are two modes of RX packet delivery to upper layers.  
    - In-order delivery: A circular buffer is used to resequence the packets 
      and packets are not delivered until all their predecessors have been 
      delivered.  
    - On-Arrival delivery: An high priority queue is maintained to deliver 
      packets as and when they arrive. This mode applies only to control channel 
      packets. 

  Mode 1 is default mode. This function is used to turn OFF the second mode and 
  revert back to Mode 1. 

  @context Calling task.  */
/*===========================================================================*/
extern void hdrdec_disable_on_arrival_delivery ( void );


/*============================================================================
  HDRDEC_DEACTIVATE
*/
/**
  Deactivates the decoder. 

  If called from a task other than HDRDEC, 
    - posts command HDRDEC_DEACTIVATE_CMD to HDRDEC task to deactivate the 
      decoder 
  otherwise 
    - Cancels all pending decodes by sending the message HDRFW_DECODE_CTL_MSG_T 
    - Waits for the fw response message (hdrfw_cancel_active_packet_rsp_msg_t) 

  @sideeffects The BCMCS scheduler configuration is also cleared implicitly 
  when firmware cancels the active packets 

  @context HDRDEC task */
/*===========================================================================*/
extern void hdrdec_deactivate 
(
  hdrdec_shutdown_complete_cb_type cb 
    /**< Pointer to the callback function called after decoder shutdown is 
         complete */
);


/*============================================================================
  HDRDEC_ENTER_MODE
*/
/**
  Enter Traffic mode or Monitor (Idle) mode 

  If called from a task other than HDRDEC, 
    - posts command HDRDEC_ENTER_MODE_CMD to HDRDEC 
  otherwise 
    - Sets interrupt rate for the mode 
    - Sets MinPreamble Threshold. 

  @context HDRDEC */
/*===========================================================================*/
extern void hdrdec_enter_mode 
( 
  hdrdec_decoder_mode_e_type mode 
    /**< Decoder mode. Could be either one of the following 
          - HDRDEC_MONITOR_MODE 
          - HDRDEC_TRAFFIC_MODE */
);


/*============================================================================
  HDRDEC_TASK
*/
/**
  Entry point to the HDR decoder task. 

  This function is the main task processing loop that processes the signals, 
  commands and indication messages to this task. */
/*===========================================================================*/
extern void hdrdec_task 
( 
  dword dummy 
    /**< Parameter to maintain REX backward compatibility */
);


/*============================================================================
  HDRDEC_INIT_CLOCK_SPEED
*/
/**
  Initializes decoder clock speed 

  Initialized decoder clock speed based on whether AT is capable of DO 
  release B or not. 

  @sideeffects This API would stop the TDEC and OFFLINE clock briefly to 
  program the clock speed. So, it should only be called at power-up. */
/*===========================================================================*/
extern void hdrdec_init_clock_speed 
( 
  boolean is_dorb_capable 
    /**< True if the AT is DORB capable; False otherwise */
);

/*============================================================================
  HDRDEC_LOG_MC_PKT_HEADERS
*/
/**
  Log the pkt PHY header through the logging service.

  @context HDRRX task. 

  @sideeffects: This function locks the interrupts briefly to avoid race 
  condition that could arise from reading some variables updated in the decoder 
  ISR context as well. */
/*============================================================================*/
extern void hdrdec_log_mc_pkt_headers ( void );

/*============================================================================
  HDRDEC_SHIP_AGED_LOG_BUFFER
*/
/**
  The function is called from the HDRSRCH task to ship the pkt header info log 
  if the current active buffer is too old.

  @context HDRRX task. */
/*============================================================================*/
extern void hdrdec_ship_aged_log_buffer ( void );

/*============================================================================
  HDRDEC_LOG_PKT_HEADERS
*/
/**
  Log the pkt PHY header through the logging service.

  @context HDRRX task. 

  @sideeffects: This function disables preemption briefly to avoid race 
  condition that could arise from reading some variables updated in the 
  HDR decoder task context as well. */
/*============================================================================*/
extern void hdrdec_log_pkt_headers
(
  boolean immediate
  /**< Do we log the packet immediately? */
);

/*============================================================================
  HDRDEC_GET_PACKET_INFO
*/
/**
  This function copies packet info.

  @context Calling task */
/*===========================================================================*/
extern void hdrdec_get_packet_info
(
  hdrdec_pkt_info_s_type *pkt_info_ptr
  /**< Pointer to the decoder packet info structure to be copied to */
);

/*============================================================================
  HDRDEC_SET_FW_LOGGING_CB
*/
/**
  Set callback for FW subframe logging.

  @context HDRDEC */
/*===========================================================================*/
extern void hdrdec_set_fw_logging_cb
( 
  void(*fw_log_cb)(int active_buffer)
  /**< Logging callback for subframe logging. */
);

#ifdef FEATURE_HDR_FL_DATA_ADAPTIVE_RXD
/*===========================================================================
 HDRDEC_GET_LAST_FTC_PKT_HSTR
*/
/**
  Returns the HSTR timestamp of last FTC packet from decoder header.

  @context Calling task */
/*===========================================================================*/
uint16 hdrdec_get_last_ftc_pkt_hstr( void );
#endif /* FEATURE_HDR_FL_DATA_ADAPTIVE_RXD */
/*\}*/

#endif /* HDRDEC_H */

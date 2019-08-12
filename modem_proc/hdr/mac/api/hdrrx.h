#ifndef HDRRX_H
#define HDRRX_H
/** @file hdrrx.h
    @brief This module contains external declarations for HDR Receive task
    processing.

    The HDR Receive task runs the following protocols: 
    - Radio Link Protocol (RLP), 
    - Signalinng Link Protocol (SLP), 
    - Packet Consolidatio Protocol (PCP), 
    - Control Channel MAC Protocol (CCMAC), 
    - Forward Traffic Channel MAC Protocol (FTCMAC). */

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

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/mac/api/hdrrx.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/20/12   cnx     Resolved TCB compiler errors for Dime. 
03/16/12   arm     Resolve TCB errors for Dime. 
10/19/11   smd     SU API cleanup updates.
09/30/11   smd     SU API cleanup updates.
07/27/11   cnx     Merged Optimized Handoff changes.
05/23/11   ssu     Removed inclusion of "dsrlpi.h"
10/12/10   kss     Moved missed SCC detection to CCMAC for Q6.
06/07/10   kss     Added hdrrx_get_connected_state_per_data().
10/16/09   ac      Added HDRRX_RLP_LOGGING_SIG for HDRRLP logging
                   Avoided using reserved signal for RLP CTA.
08/17/09   kss     Added hdrrx_get_fl_slot_count().
07/29/09   etv     Added support for connected state PER.
05/21/09   etv     Moved Decoder header logging to HDRDEC module.
                   Added support for processing firmware ind messages.
04/21/09   etv     Q6 development. Adopted Doxygen commenting style.
02/26/09   etv     Floor RxAGC1 during diversity tune away.
03/23/09   wsh     CMI: featurized CMI under FEATURE_CMI
01/12/09   etv     Fixed MC FL Packet header buffer overflow issue.
11/04/08   rmg     Added HDRRX_RLP_CTA_EXP_SIG.
10/06/08   etv     Logged power log pkt even during SOODA but floored rxagc.
05/29/08   etv     Added support for CBUFF based flow control.
01/31/08   pba     Added hdrrx_queue_ind().
01/14/08   etv     Refactored Air link Summary log packet for Artemis.
12/19/07   etv     Added support for per-carrier Instantaneous PER.
11/01/07   etv     Added support for MC Air link summary log packet.
07/24/07   etv     Cleaned up the interfaces.
07/05/07   etv     Added HDRRX_RLP_NAK_DELAY_TIMER_SIG signal.
11/02/06   kss     Moved HDRRX_SCC_OFFSET_UNKNOWN definition here.
10/26/06   etv     Added support for on-arrival delivery of RX packets.
09/08/06   spn     Removed the Signal, for Reservation Module. 
06/20/06   etv     Handle HDRRX timers as events.
05/09/06   sy      Added new signal HDRRX_RESV_BLD_MSG_TIMER_SIG for
                   reservation messages handling in HDRRX task.
11/28/05   ksu     CR 81596: Rev 0 data call fails all the time with dsrlpsq
                   crash during bi-directional data transfer
11/04/05   ksu/sy  Delete 5500-specific hdrfmac_rlp_reg_srvc().
09/28/05   ksu/sy  HDR: use one-shot abort timer instead of 20ms ticks.
05/13/05   etv     Added FMAC packet forwarding function pointer registration.
11/19/04   etv     Added hdrrx_clear_bcc_stats() to reset MCS packet rate metrics.
09/09/09   vas     Added HDRRX_MRLP_ABT_TIMER_SIG for QOS
03/29/04   kss     Added hdrrx_get_average_per() function.
11/11/03   sq      Added support for HDRSRCH SLEEP INFO logging
02/24/03   kss     Added hdrrx_enable/disable_missed_scc_detection() functions.
01/30/03    vr     Added support for HDRRX to yield CPU if needed and flow
                   controlling the base station based on MAC layer buffer.
01/29/03   kss     Added signal and function for handling missed SCCs.
08/30/02   kss     Removed macro for hdrrx_set_ack_mode().
07/23/02   om      Added CC decode attempt count accesssors for IS-890.
03/08/02   kss     Moved hdrrx_set_ack_mode() to hdrfmac (left temp macro).
12/11/01   kss     Added hdrrx_reset_pkt_sequences().
10/29/01   mpa     Added hdrrx_reset_dm_fwd_stats().
10/08/01   om      Added HDRRX_IS890_FTAP_PN_SIG
09/20/01   kss     Added hdrrx_set_ack_mode().
09/13/01   vas     Added function hdrrx_rlp_reg_srvc()
09/10/01   kss     Externalized reset logging statistics function.
08/07/01   kss     Added a signal for NV operations.
06/20/01   kss     Externalized log function.
06/15/01   kss     Added HDR enter/exit signals.
05/24/00   om      Created Module

===========================================================================*/
/**
    @defgroup hdrrx HDR Receive Task Module */
/*\{*/


/* <EJECT> */
#include "hdr_variation.h"
#include "rcinit.h"
#include "hdrhai.h"
#include "hdrfmac.h"
#include "hdrlogi.h"
#include "hdrcom_api.h"

#ifndef FEATURE_CMI
#include "hdrtask.h"
#endif

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
/** Address of the TCB for the task in which this protocol runs */
extern rex_tcb_type*    hdrrx_tcb_ptr;
/** Pointer to HDRDEC task TCB */
#define HDRRX_TASK_PTR hdrrx_tcb_ptr

/*---------------------------------------------------------------------------
                             Signal Masks
---------------------------------------------------------------------------*/

/* Reserved signals 
     TASK_START_SIG       0x8000
     TASK_STOP_SIG        0x4000
     TASK_OFFLINE_SIG     0x2000 */

/** Sig for watchdog timer */
#define HDRRX_RPT_TIMER_SIG                                   0x00000001
/** Sig for event queue */
#define HDRRX_EVENT_Q_SIG                                     0x00000002
/** Sig for data queue */
#define HDRRX_PKT_READY_SIG                                   0x00000004
/* Sig for CCMAC supvis. timer  */
/* Obsoleted #define  HDRRX_CMAC_TIMER_SIG    0x0008 */
#define HDRRX_MSG_Q_SIG                                       0x00000008

/** Sig for frame isr. @todo Is this used? */
#define HDRRX_FRAME_ISR_SIG                                   0x00000020
/** Sig for HDR entry */
#define HDRRX_ENTER_HDR_SIG                                   0x00000040
/** Sig for HDR exit */
#define HDRRX_EXIT_HDR_SIG                                    0x00000080
/** Sig for NV cmd t @todo Is this used? */
#define HDRRX_NV_CMD_SIG                                      0x00000100
/** Fwd test pn roll processing */
#define HDRRX_IS890_FTAP_PN_SIG                               0x00000200
/** Sig for missed sync CC */
#define HDRRX_SYNC_CC_LOST_SIG                                0x00000400
/** Sig for logging */
#define HDRRX_LOG_BUFFER_READY_SIG                            0x00000800
/** SCC Offset value indicating unknown SCC offset */
#define HDRRX_SCC_OFFSET_UNKNOWN                              -1

/*---------------------------------------------------------------------------
                             TYPEDEFS
---------------------------------------------------------------------------*/

/* FMAC packet forwarding function pointer type */
typedef void (*hdrrx_put_fmac_pkt_cb_type)(dsm_item_type *ftc_pkt_ptr);

/** Structure for retrieving average PER */
typedef struct
{
  uint16 meas_pkts;       
    /**< Number of packets sampled for PER measurement. */

  uint16 err_pkts;
    /**< Number of errors in sample */

} hdrrx_average_per_type;


/*============================================================================
  HDRRX_TASK
*/
/**
  This function is the entry point to the HDR Receive task.  It contains the
  task event processing loop, which executes while waiting to enter HDR mode.

  @context HDRRX task. 

  @sideeffects None */
/*============================================================================*/
extern void hdrrx_task
(
  dword dummy              
    /**< Required for REX, ignore */
);


/*============================================================================
  HDRRX_SET_CC_OFFSET
*/
/**
  Function to set the CC offset so that SyncCC's can be determined from
  HSTR value.

  @context Calling task. 

  @sideeffects None */
/*============================================================================*/
extern void hdrrx_set_cc_offset
( 
  int offset 
    /**< SCC Offset in half slots */
);


/*============================================================================
  HDRRX_ENABLE_MISSED_SCC_DETECTION
*/
/**
  Enables missed sync CC detection. hdrcmac_handle_lost_scc() is called
  when a missed packet is suspected.

  @context Calling task. 

  @sideeffects None */
/*============================================================================*/
extern void hdrrx_enable_missed_scc_detection 
(
  boolean clear_pending
    /**< Whether to clear any pending indication */
);


/*============================================================================
  HDRRX_DISABLE_MISSED_SCC_DETECTION
*/
/**
  Disables missed sync CC detection.

  @context Calling task. 

  @sideeffects None */
/*============================================================================*/
extern void hdrrx_disable_missed_scc_detection ( void );


/*============================================================================
  HDRRX_REGISTER_FMAC_PKT_CB
*/
/**
  FMAC uses this function to register a packet forwarding call-back routine 
  based on the negotiated FMAC subtype.  

  @context HDRRX task. 

  @sideeffects None */
/*============================================================================*/
extern void hdrrx_register_fmac_pkt_cb
( 
  hdrrx_put_fmac_pkt_cb_type ptr_fmac_put_pkt 
    /**< Pointer to function used to forward the FTCMAC packet */
);


/*============================================================================
  HDRRX_GET_AVERAGE_PER
*/
/**
  Function to access average packet error rate.  The supplied structure
  is filled in with the measured sample size and the number of those packets
  that were in error.

  @context Calling task. 

  @sideeffects None */
/*============================================================================*/
extern void hdrrx_get_average_per
(
  hdrrx_average_per_type * per
    /* Pointer to location where the PER information will be filled */
);


/*============================================================================
  HDRRX_QUEUE_IND
*/
/**
  This function queues an indication on the hdrrx.event_q and sets the
  signal.  If there is any data associated with the indication, it is copied
  into the buffer before it is queued.  This function is called in the task
  context of whoever is giving the indication.

  @assumptions This routine must be multi-thread safe, as two different tasks 
  may give an indication to protocols in the RX task at the same time. 
  ind_data_size must not be larger than hdrind_ind_data_union_type, because 
  this is the data type the command gets copied into.

  @context Calling task. 

  @sideeffects None */
/*============================================================================*/
extern void hdrrx_queue_ind
(
  hdrhai_protocol_name_enum_type protocol_name,
    /**< Who the indication is for */
  hdrind_ind_name_enum_type ind_name,
    /**< Name of the indication given */
  void *input_ind_data_ptr
    /**< Size of the data associated with this indication */
);


/*===========================================================================

FUNCTION HDRRX_QUEUE_TIMER

DESCRIPTION
  This function queues a timer event on the hdrrx.event_q and sets the
  signal.

DEPENDENCIES
  None

PARAMETERS
  protocol_name - Who the indication is for
  timer_id - The ID of the timer that has expired

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrrx_queue_timer
(
  hdrhai_protocol_name_enum_type  protocol_name,
  uint32 timer_id
);


/*============================================================================
  HDRRX_RESET_DM_FWD_STATS
*/
/**
  This function resets all RX rate statistics and sends a log packet to the DM.

  @context HDRRX task. */
/*============================================================================*/
extern void hdrrx_reset_dm_fwd_stats ( void );

#ifdef FEATURE_HDR_BCMCS 
/*============================================================================
  HDRRX_CLEAR_BCC_STATS
*/
/**
  This function resets the broadcast channel statistics.
  - BCC early decode and
  - BCC rate CRC count

  @context HDRRX task. */
/*============================================================================*/
extern void hdrrx_clear_bcc_stats ( void );

#endif /* FEATURE_HDR_BCMCS */


/*============================================================================
  HDRRX_RESET_CONNECTED_STATE_PER
*/
/**
  Reset packet error rate statistics maintained for recent traffic session.

  @context Calling task. 

  @sideeffects None */
/*============================================================================*/

extern void hdrrx_reset_connected_state_per ( void );

/*============================================================================
  HDRRX_GET_CONNECTED_STATE_PER
*/
/**
  Return packet error rate observed during current or recent traffic session.

  Upper layers expect support for PERs in range 0.01% to 8.0%. In order
  to support this range, PER is scaled up by 10000, so that the resulting 
  PER would be an integer for easy representation and also support the 
  expected range

  @context Calling task. 

  @return Connected State Packet error rate scaled up by 10000

  @sideeffects None */
/*============================================================================*/

extern uint16 hdrrx_get_connected_state_per ( void );

/*============================================================================
  HDRRX_GET_CONNECTED_STATE_PER_DATA
*/
/**
  Return packet counts for bad and total traffic packets. Control channel
  is not counted.

  total_tc_packets and total_tc_errors must point to valid uint32 
    variable locations to be filled in.

  @context Calling task. 

  @return  Bad and total packet counts are filled in. 

  @sideeffects None */
/*============================================================================*/

extern void hdrrx_get_connected_state_per_data
(
  uint32 * total_tc_packets, 
    /**< Total traffic channel packets received. */

  uint32 * total_tc_errors
    /**< Traffic channel packets received with errors (failed CRC). */
);



/*============================================================================
  HDRRX_GET_FL_SLOT_COUNT
*/
/**
  Returns the number of slots occupied by packet directed at this AT. All 
  TC packets (Rev 0, Rev A/B SU and MU), good or bad, contribute to this 
  count.

  @return Number of slots occupied by packets directed at this AT since the last 
  time the counter was cleared.

  @context Calling task. 

  @sideeffects Count is cleared if reset is TRUE */
/*============================================================================*/

extern uint32 hdrrx_get_fl_slot_count
(
  boolean reset
    /**< Count is reset if TRUE */
);

/*============================================================================
  HDRRX_HANDLE_LOST_SYNC_CC
*/
/**
  Sets the HDRRX_SYNC_CC_LOST_SIG.  This routine is called when the AT
  misses sync CC preamble. *It may be called in interrupt context.*

  @context HDRRX task. 

  @sideeffects None */
/*============================================================================*/
extern void hdrrx_handle_lost_sync_cc ( void );

/*\}*/
#endif /* HDRRX_H */

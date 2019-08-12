#ifndef __ONEX_VOICE_ADAPT_IF_H__
#define __ONEX_VOICE_ADAPT_IF_H__
/*===========================================================================

         I N T E R F A C E   W I T H   A U D I O    S Y S T E M

DESCRIPTION
  This contains all the declarations for the Authentication Task.

  Copyright (c) 2015 Qualcomm Technologies, Inc. 
  All Rights Reserved Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.


===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/onex_voice_adapt_if.h   1.8   10 Mar 2015 13:34:04   azafer  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/api/public/onex_voice_if.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $


when      who   what, where, why
--------  ---   ------------------------------------------------------------
09/09/15   agh     Initial revision

  ============================================================================*/

/*----------------------------------------------------------------------------
  Include files for Module
----------------------------------------------------------------------------*/
#include "sys.h"

/****************************************************************************
 * ONEX VOCODER INTERFACE DEFINITION                                 *
 ****************************************************************************/
/*
 * ONEX Vocoder types.
 */
#define ONEX_VOC_ID_EVRCA    ( 0x20000000 )
  /**< EVRC-A vocoder format. */
  
#define ONEX_VOC_ID_EVRCB    ( 0x20000001 )
  /**< EVRC-B vocoder format. */
  
#define ONEX_VOC_ID_EVRCNW   ( 0x20000002 )
  /**< EVRC-NW vocoder format. */
  
#define ONEX_VOC_ID_EVRCNW2K ( 0x20000003 )
  /**< EVRC-NW2K vocoder format. */
  
#define ONEX_VOC_ID_EVRCWB   ( 0x20000004 )
  /**< EVRC-WB vocoder format. */
  
#define ONEX_VOC_ID_QCELP13K ( 0x20000005 )
  /**< QCELP-13K vocoder format. */
  

/**
* Defined vocoder frame types.
*/
#define ONEX_VOC_FRAME_RATE_BLANK              ( 0x20000006 )
  /**< Blank Rate. */

#define ONEX_VOC_FRAME_RATE_EIGHTH             ( 0x20000007 )
  /**< 1/8 Rate. */

#define ONEX_VOC_FRAME_RATE_QUARTER            ( 0x20000008 )
  /**< 1/4 Rate. */

#define ONEX_VOC_FRAME_RATE_HALF               ( 0x20000009 )
  /**< 1/2 Rate. */

#define ONEX_VOC_FRAME_RATE_FULL               ( 0x2000000A )
  /**< 1 Rate. */

#define ONEX_VOC_FRAME_RATE_ERASURE            ( 0x2000000B )
  /**< Erasure rate; applicable only for downlink. */

#define ONEX_VOC_FRAME_RATE_EIGHTH_NONCRITICAL ( 0x2000000C )
  /**< Non-critical 1/8 rate. */

/*
* Defined vocoder (capacity operating point) COP rates.
*/
#define ONEX_VOC_COP_0 ( 0x2000000D )
     /* COP -- 0. */
  
#define ONEX_VOC_COP_1 ( 0x2000000E )
     /* COP -- 1. */
  
#define ONEX_VOC_COP_2 ( 0x2000000F )
     /* COP -- 2. */
  
#define ONEX_VOC_COP_3 ( 0x20000010 )
     /* COP -- 3. */
  
#define ONEX_VOC_COP_4 ( 0x20000011 )
     /* COP -- 4. */
  
#define ONEX_VOC_COP_5 ( 0x20000012 )
     /* COP -- 5. */
  
#define ONEX_VOC_COP_6 ( 0x20000013 )
     /* COP -- 6. */
  
#define ONEX_VOC_COP_7 ( 0x20000014 )
     /* COP -- 7. */

#define ONEX_VOC_OPEN_CMD                ( 0x20000040 )
#define ONEX_VOC_CLOSE_CMD               ( 0x20000041 )
#define ONEX_VOC_START_CMD               ( 0x20000042 )
#define ONEX_VOC_STOP_CMD                ( 0x20000043 )

#define ONEX_VOC_EVENT_RQST_MIN_MAX_RATE ( 0x20000044 )
#define ONEX_VOC_EVENT_RQST_AVG_ENC_RATE ( 0x20000045 )
#define ONEX_VOC_EVENT_RQST_DTX_MODE     ( 0x20000046 )
#define ONEX_VOC_EVENT_RQST_START        ( 0x20000047 )
#define ONEX_VOC_EVENT_RQST_STOP         ( 0x20000048 )

#define ONEX_EOK          ( 0x00000000 ) /**< Success; completed with no errors. */
#define ONEX_EFAILED      ( 0x00000001 ) /**< General failure. */
#define ONEX_EBADPARAM    ( 0x00000002 ) /**< Bad operation parameter(s). */
#define ONEX_EUNSUPPORTED ( 0x00000003 ) /**< Unsupported routine/operation. */
#define ONEX_EVERSION     ( 0x00000004 ) /**< Unsupported version. */
#define ONEX_EUNEXPECTED  ( 0x00000005 ) /**< Unexpected problem encountered. */
#define ONEX_EPANIC       ( 0x00000006 ) /**< Unhandled problem occurred. */

#define ONEX_ENORESOURCE  ( 0x00000007 ) /**< Unable to allocate resource(s). */
#define ONEX_EHANDLE      ( 0x00000008 ) /**< Invalid handle. */
#define ONEX_EALREADY     ( 0x00000009 ) /**< Operation is already processed. */
#define ONEX_ENOTREADY    ( 0x0000000A ) /**< Operation is not ready to be processed. */
#define ONEX_EPENDING     ( 0x0000000B ) /**< Operation is pending completion. */
#define ONEX_EBUSY        ( 0x0000000C ) /**< Operation could not be accepted or processed. */
#define ONEX_EABORTED     ( 0x0000000D ) /**< Operation aborted due to an error. */
#define ONEX_EPREEMPTED   ( 0x0000000E ) /**< Operation preempted by a higher priority. */
#define ONEX_ECONTINUE    ( 0x0000000F ) /**< Operation requests intervention to complete. */
#define ONEX_EIMMEDIATE   ( 0x00000010 ) /**< Operation requests immediate intervention to complete. */
#define ONEX_ENOTIMPL     ( 0x00000011 ) /**< Operation is not implemented. */
#define ONEX_ENEEDMORE    ( 0x00000012 ) /**< Operation needs more data or resources. */
#define ONEX_ELPC         ( 0x00000013 ) /**< Operation is a local procedure call. */
#define ONEX_ENOMEMORY    ( 0x00000014 ) /**< Unable to allocate enough memory. */
#define ONEX_ENOTEXIST    ( 0x00000015 ) /**< Item does not exist. */

/****************************************************************************
 * ONEX COMMON INTERFACE DEFINITION                                         *
 ****************************************************************************/

/**
 * vocoder Frame information structure.
 */

typedef struct {

  uint32 frame_rate;
    /* frame rate. */
} onex_voc_frame_info_t;

/**
 * Event callback provided by a client of the ONEX protocol software.
 *
 * The client returns ONEX_EOK from the callback when the client was successful
 * in queuing the event to its own thread context for processing.
 *
 * All other error codes indicate a problem.
 *
 * The event parameter pointer is valid only within the callback context.
 */
typedef uint32 ( *onex_common_event_callback_fn_t ) (

  void* session_context,
    /**< Client-provided session_context pointer from the OPEN command. */

  uint32 event_id,
    /**< Event ID. See ONEX_IXXX_EVENT_YYY. */

  void* params,
    /**< Pointer to an associated event data structure. */

  uint32 size
    /**< Size of the event data structure. */

);

/**
 * ONEX Vocoder buffer structure.
 */

typedef struct {

  boolean flags;
    /**<
      * Flags:
      * - Bit 0 set: This indicates that the buffer is filled with a valid
      *   speech frame when TRUE.
      *
      *   The client sets this when delivering valid speech frame in the
      *   uplink vocoder packet callback.
      *
      *   The onex protocol software sets this when delivering valid speech
      *   frame in the downlink vocoder packet callback.
      */

  uint32 vocoder_id;
    /**< Vocoder ID. See CDMA_IVOCODER_ID_XXX. */

  void*  frame_info;
    /**<
      * Frame information payload.
      *
      * The vocoder ID determines to what frame information data type, is
      * associated to.
      *
      * See ONEX_vocxxx_frame_info_t for the data structures.
      */

  uint8* frame;
    /**< Pointer to the vocoder packet data. */

  uint32 size;
    /**<
      * Actual frame size of the vocoder packet data in bytes.
      *
      * If used in the downlink packet exchange context, the actual frame
      * size is the size of what the ONEX protocol software wants to send to
      * the client. The ONEX protocol software must set the value of this size.
      *
      * If used in the uplink packet exchange context, the actual frame size
      * is the size of what the client wants to send to the ONEX protocol
      * software. The client must set the value of size.
      */

} onex_vocoder_buffer_t;

/**
 * Uplink/Downlink vocoder callback function type provided by client.
 */
typedef void ( *onex_vocoder_callback_fn_t ) (

  void* session_context,
    /**<
     * Client-provided session context pointer from the onex_VOC_CMD_OPEN
     * command.
     */

  onex_vocoder_buffer_t* buffer
    /**<
     * Reference to vocoder buffer for exchange between the client and the ONEX
     * protocol software.
     */

);

/*----------------------------------------------------------------------------
  ONEX Call function.
----------------------------------------------------------------------------*/

/**
 * Provides a single API entry point into the ONEX protocol software.
 *
 * @param[in] cmd_id Command identifier to execute.
 * @param[in] params Command parameters.
 * @param[in] size Size of the command parameters in bytes.
 *
 * @return
 * ONEX_EOK (0) when successful.
 */
uint32 onex_voice_call (

  uint32 cmd_id,
  void* params,
  uint32 size

);

/****************************************************************************
 * ONEX VOICE CMD INTERFACE DEFINITION                                      *
 ****************************************************************************/

/** Open command (synchronous API).
 *
 * Use this command to acquire a handle to a new ONEX voice session instance.
 *
 * The client must use the returned handle (ret_handle) provided by
 * the Open command to execute any subsequent commands.
 *
 * The ONEX protocol software sends the voice traffic channel events to the
 * client who has opened a session for a particular subscription. The client
 * will use "asid" to indicate the subscription that it is interested in
 * receiving the events for.
 *
 * The client will use these events to set up voice session thereby
 * facilitating vocoder packet exchange between the ONEX protocol software and
 * the client.
 *
 * The client provides uplink and downlink vocoder callback functions as part
 * of the Open command. The uplink callback function is called by the ONEX
 * protocol software every time when a vocoder packet is required to be
 * transmitted on the uplink. The downlink callback function is called by the
 * ONEX protocol software every time when a vocoder packet is available from
 * the network for decoding and playback.
 *
 * The ONEX protocol software would ensure providing client with
 *   - valid reference to vocoder buffers for read/write.
 *   - call context to indicate the involved subscription.
 *
 * Upon a successful open, the command shall return ONEX_EOK to the caller.
 */

typedef struct {

  uint32* ret_handle;
    /**<
     * Returns the handle that the client must use when making subsequent
     * commands.
     */

  sys_modem_as_id_e_type asid;
    /**< Modem Active Subscription ID. */

  onex_common_event_callback_fn_t event_cb;
    /**<
     * Central event callback function, which receives asynchronous events
     * from the ONEX protocol software.
     *
     * Operational contract:
     *
     * - The client may only queue the incoming event and signal a worker
     * thread to process the event. The client must not perform any other
     * processing in the callback context.
     *
     * - The client may not call any APIs on the ONEX protocl software in the
     * callback context. This will cause synchronization issues for the driver
     * and may lead to a system failure or deadlock.
     *
     * - The client may not perform any blocking operations or acquire any
     * locks in the event callback context that lead to a system deadlock.
     *
     * - The client may spend no more than 5 us while in the callback
     * context.
     *
     * - It is highly recommended to use atomic operations for
     * synchronization needs.
     *
     * Failure to meet the operational contract may lead to an undefined
     * state with system stability and performance issues.
     */

  onex_vocoder_callback_fn_t ul_cb;
    /**<
     * Uplink callback: The client shall copy the vocoder packet available to
     * the buffer provided by the ONEX protocol software for OTA.
     */

  onex_vocoder_callback_fn_t dl_cb;
    /**<
     * Downlink callback: The client shall copy the vocoder packet available
     * in the buffer from the ONEX protocol software for decoding.
     */

  void* session_context;
    /**<
     * Pointer to the session context. The client stores its session
     * context pointer here to retrieve its session control data structure,
     * which the client uses to queue and signal events into its worker
     * thread.
     *
     * The session_context is returned to the client each time an event
     * callback is triggered.
     */

} onex_voc_cmd_open_t;


/** Close command (Synchronous blocking API).
 *
 * Use this command to close the previously opened ONEX voice session instance.
 *
 * The client may not use the open handle after receiving successful close
 * indication.
 *
 * Upon a successful close, the command shall return ONEX_EOK to the caller.
 */

typedef struct {

  uint32 handle;   /* Open handle. */

} onex_voc_cmd_close_t;

/** Stop command (synchronous API.)
 *
 * This command shows the client cannot do any more vocoder packet exchange
 * with the ONEX protocol software. The client sends this command to the
 * ONEX protocol software as an acknowledgement for the event
 * ONEX_VOC_EVENT_REQUEST_STOP.
 *
 * The ONEX protocol software will not do any vocoder packet exchanges
 * after receiving this command from the client.
 *
 * The ONEX protocol software will wait for this command before proceeding
 * with VFR timing reconfiguration during handovers.
 *
 */

typedef struct {

  uint32 handle;
    /**< Open handle. */

} onex_voc_cmd_stop_t;


/** Start command ( synchronous API ).
 *
 * This command shows the client's readiness for vocoder packet exchange with
 * the ONEX protocol software.
 *
 * The client sends this command to the ONEX protocol software as an
 * acknowledgement for the event ONEX_VOC_EVENT_REQUEST_START.
 *
 * The ONEX protocol software need not wait for this command, and need not
 * perform any actions.
 *
 */
typedef struct {

  uint32 handle;
    /**< Open handle. */

} onex_voc_cmd_start_t;

/****************************************************************************
 * ONEX VOICE INTERFACE DEFINITION                                          *
 ****************************************************************************/

/** minimum and maximum frame rate event (Asynchronous event).
 *
 * The ONEX protocol software sends this event as part voice traffic set up.
 *
 * If the ONEX protocol software provides the frame rate information before
 * selecting vocoder type, the client shall cache the frame rate settings,
 * and then reapply when vocoder is available.
 *
 * If the frame rate information is not provided, then the default minumim and
 * maximum rate corresponding to the active vocoder shall be used.
 *
 * The ONEX protocol software may also send this event to select a new frame
 * rate during an active voice call.
 *
 */

typedef struct {

  uint32 min_rate;
    /* Minimum frame rate, see ONEX_VOCXXX_FRAME_YYY. */
  uint32 max_rate;
    /* Maximum frame rate, see ONEX_VOCXXX_FRAME_YYY. */

} onex_voc_event_rqst_min_max_rate_t;


/** Average encoder rate event (Asynchronous event).
 *
 * The ONEX protocol software sends this event as part voice traffic set up.
 *
 * If the ONEX protocol software provides the frame rate information before
 * selecting vocoder type, the client shall cache the frame rate settings,
 * and then reapply when vocoder is available.
 *
 * If the average encoder rate information is not provided, then the default
 * average encoder rate corresponding to the active vocoder shall be used.
 *
 * The ONEX protocol software may also send this event to select a new average
 * encoder rate during an active voice call.
 *
 */

typedef struct {

  uint32 avg_enc_rate;
    /*
     * Average Encoder Rate, indicates the capacity operating point
     * or COP rate.
     */

} onex_voc_event_rqst_avg_enc_rate_t;


/** DTX mode event (Asynchronous event).
 *
 * The ONEX protocol software sends this event as part of the voice traffic
 * set up.
 *
 * If the ONEX protocol software provides the DTX mode information before
 * selecting vocoder type, the client shall cache the DTX mode information, and
 * then reapply when vocoder is available.
 *
 * If the DTX mode information is not provided, then the default DTX mode
 * corresponding to the active vocoder shall be used.
 *
 * The ONEX protocol software may also send this event to select a new DTX
 * mode during an active voice call.
 *
 */

typedef struct {

  boolean enable_flag;
    /*
     * Enable flag:
     * - TRUE -- Enable DTX
     * - FALSE -- Disable DTX
     */

} onex_voc_event_rqst_dtx_mode_t;

/** Voice traffic start request (asynchronous event).
 *
 * The ONEX protocol software uses this event to request client to set up
 * the vocoder and start packet exchange with it.
 *
 * The ONEX protocol software will send this event only after VFR timing is
 * stabilized. The ONEX protocol software will also provide vocoder type as
 * part of this event.
 *
 */
typedef struct {

  uint32 vocoder_id;
    /**< vocoder identifier, See CDMA_IVOCODER_ID_XXX. */

}onex_voc_event_rqst_start_t;

/** Voice traffic stop request (asynchronous event).
 *
 * The ONEX protocol software uses this event to request client to stop vocoder
 * and the packet exchange.
 *
 * The ONEX protocol software will send this event during Intra-RAT handover
 * scenarios such as: frame offset change, service option change, etc.
 *
 * VFR timing reconfiguration shall not be done by the ONEX protocol software
 * until the vocoder is stopped and ONEX_VOC_CMD_STOP is sent by the client.
 *
 * This event does not have any parameters.
 *
 */

#endif /* __ONEX_VOICE_ADAPT_IF_H__ */

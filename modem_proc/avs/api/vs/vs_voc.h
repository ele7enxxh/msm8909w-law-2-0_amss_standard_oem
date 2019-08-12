#ifndef __VS_VOC_H__
#define __VS_VOC_H__

/**
  @file  vs_voc.h
  @brief This file contains vocoder function definitions of the Voice Services
         API.

*/

/*
  ============================================================================

   Copyright (C) 2012, 2014-2015 Qualcomm Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.

  ============================================================================

                             Edit History

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/api/vs/vs_voc.h#1 $
  $Author: mplcsds1 $

  when      who   what, where, why
  --------  ---   ------------------------------------------------------------
  12/05/12  sud   Initial revision

  ============================================================================
*/

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

#include "vs_errcodes.h"
#include "vs_common.h"

/****************************************************************************
 * TYPE DEFINITIONS                                                         *
 ****************************************************************************/

/** @addtogroup vs_voc
@{ */

#define VS_VOC_CLIENT_GSM ( 0x00012E26 )
/* GSM modem subsystem. */

#define VS_VOC_CLIENT_MVS_GSM ( 0x000131E2 ) /*  MVS client for VS on GSM Network. */

#define VS_VOC_CLIENT_WCDMA ( 0x000130E2 ) /**< WCDMA modem subsystem. */

#define VS_VOC_CLIENT_TDSCDMA ( 0x000130E3 ) /**< TD-SCDMA modem subsystem. */

#define VS_VOC_CLIENT_CDMA ( 0x000130E4 ) /**< CDMA modem subsystem client for SUB1. */

#define VS_VOC_CLIENT_CDMA2 ( 0x000131E3 ) /**< CDMA modem subsystem client for SUB2. */

#define VS_VOC_CLIENT_TEST ( 0x000130E5 ) /* CDMA Modem Subsystem. */

/*----------------------------------------------------------------------------
  Start of Structures
----------------------------------------------------------------------------*/

typedef struct vs_voc_buffer_t vs_voc_buffer_t;

/** Vocoder buffer structure. */
struct vs_voc_buffer_t
{
  uint16_t version;
    /**< Version: \n
      *  Vocoder buffer format minor version. The currently supported minor
      *  version is 1. Any changes are backward compatible.
      */

  uint32_t flags;
    /**< Flags: \n
      * - Bit 0 set: Buffer is filled with a valid encoder speech frame. \n
      *   This indicates that the buffer is filled with a valid speech frame
      *   when TRUE. The voice driver sets this when delivering encoder buffers
      *   for the read operation. This flag is ignored for buffers given from
      *   the client to the voice driver.
      *
      * - Bit 1 set: The timestamp value is valid. Currently not supported. @tablebulletend
      */

  uint32_t media_id;
    /**< Media ID. See VS_COMMON_MEDIA_ID_XXX. */

  void* frame_info;
    /**< Frame information payload.
      *
      * The media ID determines the actual frame information data type.
      *
      * See vs_vocxxx_frame_info_t for the respective data structures.
      */
  uint8_t* frame;
    /**< Pointer to the vocoder packet data. */

  uint32_t size;
    /**< Actual frame size of the vocoder packet data in bytes.
      *
      * If used in the context of writing buffers, the actual frame
      * size is the size of what the client wants to send to the vocoder
      * decoder. The client must set the value of this size. This size must be
      * equal to or less than the max_size field.
      *
      * If used in the context of reading buffers, the actual
      * frame size is the size of what the vocoder encoder is sending.
      * This value is set by the vocoder driver.
      */
  uint32_t max_size;
    /**< Maximum buffer size allocated to store the vocoder frame.
      *
      * This is a read-only field. The client must not change its value.
      */

  uint64_t timestamp;
    /**< Timestamp in microseconds. Currently not supported. */

  uint32_t tag;
	/**< Tag field filled by the client to tag a vocoder session. The client
	  * must write a unique value to this field for each vocoder session.
	  *
	  * This can be used by a client to flush out stale vocoder packet data
	  * that it may get during handover or when the session is
	  * disabled and then enabled within a short time. @newpagetable
	  */
};

/*----------------------------------------------------------------------------
  VS Control functions
-----------------------------------------------------------------------------*/

 /** Open command (synchronous blocking API).
 *
 * Use this command to acquire a handle to a new vocoder session.
 *
 * The vocoder resource is granted to the client upon the client receiving a
 * VS_COMMON_EVENT_READY event. Clients must assume that they do not have
 * the vocoder resource until the VS_COMMON_EVENT_READY event is received.
 *
 * The vocoder resource may be taken back by the system upon the client
 * receiving a VS_COMMON_EVENT_NOT_READY event.
 *
 * The client must use the returned handle (ret_handle) provided by
 * the Open command to execute any subsequent commands on the vocoder session.
 *
 * The client is permitted to send any vocoder data commands to the vocoder
 * session after successfully calling the Open command. The client is
 * permitted to call vocoder data commands before enabling the vocoder
 * session. Updates to the vocoder resource are only applied after the
 * client is granted the resource.
 *
 * Each voice system controls at most one vocoder resource. Only one
 * client per voice system is granted the vocoder resource. The
 * capability to run multiple vocoder resources across multiple voice systems
 * is a product-specific implementation and may not be the same across
 * products.
 */
#define VS_VOC_CMD_OPEN ( 0x00012E27 )

typedef struct vs_voc_cmd_open_t vs_voc_cmd_open_t;

/** Vocoder Open command. */
struct vs_voc_cmd_open_t
{
  uint32_t* ret_handle;
    /**< Returns the handle that the client must use when making subsequent
      * commands.
      */
  uint32_t vsid;
    /**< System level published/documented voice system ID. */

  uint32_t client_id;
    /**< Client ID. See VS_VOC_CLIENT_XXX. */

  void* session_context;
    /**< Pointer to the session context. The client stores its session
      *  context pointer here to retrieve its session control data structure,
      *  which the client uses to queue and signal events into its worker
      *  thread.
      *
      * The session_context is returned to the client each time the event
      * callback is triggered.
      */
  vs_common_event_callback_fn_t event_cb;
    /**< Central event callback function, which receives
      *  asynchronous events from the voice driver.
      *
      * Operational contract:
      *
      * - The client may only queue the incoming event and signal a worker
      * thread to process the event. The client must not perform any other
      * processing in the callback context.
      *
      * - The client may not call any APIs on the voice driver in the
      * callback context. This will cause synchronization issues for the
      * driver and may lead to a system failure or deadlock.
      *
      * - The client may not perform any blocking operations or acquire any
      * locks in the callback context that lead to a system deadlock.
      *
      * - The client may spend no more than 5 us while in the callback
      * context.
      *
      * - It is highly recommended to use atomic operations for
      * synchronization needs.
      *
      * Failure to meet the operational contract may lead to an undefined
      * state with system stability and performance issues. Voice Services
      * does not provide any warnings for the above items.
      */
};


 /** Close command (asynchronous nonblocking API).
 *
 * Closes the already opened session.
 *
 * The client may not use the open handle after receiving a successful
 * Close indication.
 *
 * The VS_COMMON_EVENT_CMD_RESPONSE event is sent to the client to indicate
 * success or failure, with the status_id set to VS_EOK upon success.
 */
#define VS_VOC_CMD_CLOSE ( 0x00012E28 )

typedef struct vs_voc_cmd_close_t vs_voc_cmd_close_t;

/** Vocoder Close command. */
struct vs_voc_cmd_close_t
{
  uint32_t handle; /**< Open handle. */

  void* client_context; /**< Client-provided context for the client's use. */
};


 /** Enable command (asynchronous nonblocking API).
 *
 * Requests that a vocoder resource be enabled.
 *
 * The vocoder does not actually begin until the client receives a
 * VS_COMMON_EVENT_READY event.
 * 
 * The vocoder can be taken back by the voice driver after receiving a
 * VS_COMMON_EVENT_NOT_READY event. This can occur at any time. Some reasons
 * for losing the vocoder resource can be due to, but not limited to, a DSP
 * crash, preemptions, calls to the VS_VOC_CMD_STANDBY or VS_VOC_CMD_DISABLE
 * commands, a device switch, and others.
 *
 * Calling VS_VOC_CMD_ENABLE more than once is not permitted. The client must
 * first call VS_VOC_CMD_DISABLE before calling VS_VOC_CMD_ENABLE again to
 * request a different vocoder resource.
 *
 * The VS_COMMON_EVENT_CMD_RESPONSE event is sent to the client to indicate
 * success or failure.
 */
#define VS_VOC_CMD_ENABLE ( 0x00012E29 )

typedef struct vs_voc_cmd_enable_t vs_voc_cmd_enable_t;

/** Vocoder Enable command. */
struct vs_voc_cmd_enable_t
{
  uint32_t handle; /**< Open handle. */
  uint32_t media_id; /**< Media type to request enabling. See VS_COMMON_MEDIA_ID_XXX. */
  void* client_context; /**< Client-provided context for the client's use. */
};


 /** Standby command (asynchronous nonblocking API).

 * Used by the client to request a vocoder resource to be brought up but
 * kept paused to reduce the enabling time when actually required for
 * optimization purposes.
 *
 * The client supplied media type is used to predict the vocoder that is to be
 * enabled later through the VS_VOC_CMD_ENABLE command.
 *
 * The VS_COMMON_EVENT_NOT_READY event is sent to the client only if the
 * client has previously received a VS_COMMON_EVENT_READY event.
 *
 * The VS_COMMON_EVENT_CMD_RESPONSE event is sent to the client to indicate
 * success or failure.
 */
#define VS_VOC_CMD_STANDBY ( 0x00012E2A )

typedef struct vs_voc_cmd_standby_t vs_voc_cmd_standby_t;

/** Vocoder Standby command. */
struct vs_voc_cmd_standby_t
{
  uint32_t handle; /**< Open handle. */
  uint32_t media_id; /**< Media type to request getting ready. See VS_COMMON_MEDIA_ID_XXX. */
  void* client_context; /**< Client-provided context for the client's use. */
};


 /** Disable command (asynchronous nonblocking API).
 *
 * Disables the vocoder resource.
 *
 * The VS_COMMON_EVENT_NOT_READY event is sent to the client only if the
 * client has previously received a VS_COMMON_EVENT_READY event.
 *
 * The VS_COMMON_EVENT_CMD_RESPONSE event is sent to the client to indicate
 * success or failure.
 */
#define VS_VOC_CMD_DISABLE ( 0x00012E2B )

typedef struct vs_voc_cmd_disable_t vs_voc_cmd_disable_t;

/** Vocoder Disable command. */
struct vs_voc_cmd_disable_t
{
  uint32_t handle; /**< Open handle. */
  void* client_context; /**< Client-provided context for the client's use. */
};

/*----------------------------------------------------------------------------
  Data functions
-----------------------------------------------------------------------------*/


 /** Allocate Buffer command (synchronous blocking API).
 *
 * Allocates one buffer from the voice driver for asynchronously
 * sending and receiving vocoder packets.
 *
 * The client is recommended to allocate the buffers during power on or during
 * call setup. Allocations of buffers during steady state may add undesirable
 * delays.
 *
 * This command must not be called from Interrupt Service Request (ISR) or
 * Interrupt Service Task (IST) contexts.
 *
 * The buffers must be returned to the voice driver by calling
 * VS_VOC_CMD_FREE_BUFFER before the VS_VOC_CMD_CLOSE command is called.
 */
#define VS_VOC_CMD_ALLOC_BUFFER ( 0x00012E2C )

typedef struct vs_voc_cmd_alloc_buffer_t vs_voc_cmd_alloc_buffer_t;

/** Vocoder Allocate Buffer command. */
struct vs_voc_cmd_alloc_buffer_t
{
  uint32_t handle; /**< Open handle. */
  vs_voc_buffer_t** ret_buffer;
    /**< Returns a buffer structure that the client can use for asynchronously
      * sending and receiving vocoder packets.
      *
      * The buffer structure contains a vocoder packet header, a speech frame,
      * and additional auxillary information.
      */
  uint32_t req_max_frame_size;
    /**< Client-requested maximum vocoder speech frame size.
      */
};


 /** Free Buffer command (synchronous blocking API).
 *
 * Frees a buffer back to the voice driver.
 *
 * The client must not access the buffer once the buffer is freed.
 *
 * This command must not be called from ISR or IST contexts.
 */
#define VS_VOC_CMD_FREE_BUFFER ( 0x00012E2D )

typedef struct vs_voc_cmd_free_buffer_t vs_voc_cmd_free_buffer_t;

/** Vocoder Free Buffer command. */
struct vs_voc_cmd_free_buffer_t
{
  uint32_t handle; /**< Open handle. */
  vs_voc_buffer_t* buffer; /**< Buffer to return. */
};


 /** Prime Read Buffer command (synchronous nonblocking API).
 *
 * Pushes a buffer into the voice driver to asynchronously receive an encoder
 *   packet from the voice driver.
 *
 * The client calls the VS_VOC_CMD_READ_BUFFER command to retrieve one
 * or more encoder buffers upon receiving the VS_VOC_EVENT_READ_AVAILABLE
 * event.
 */
#define VS_VOC_CMD_PRIME_READ_BUFFER ( 0x00012E2E )

typedef struct vs_voc_cmd_prime_read_buffer_t vs_voc_cmd_prime_read_buffer_t;

/** Vocoder Prime Read Buffer command. */
struct vs_voc_cmd_prime_read_buffer_t
{
  uint32_t handle; /**< Open handle. */
  vs_voc_buffer_t* buffer; /**< Buffer to prime into the voice driver. */
};


 /** Read Available event (asynchronous event).
 *
 * Event that notifies the client that an encoder buffer is
 *   available to read.
 *
 * There is no parameter for this event.
 */
#define VS_VOC_EVENT_READ_AVAILABLE ( 0x00012E2F )


 /** Read Buffer command (synchronous nonblocking API).
 *
 * Reads a buffer from the encoder. The client calls the
 * Read command to retrieve a buffer for processing after receiving
 * a VS_VOC_EVENT_READ_AVAILABLE event.
 *
 * The buffers are read in the sequence of encoding. The buffer structures are
 * not necessarily returned in the same order in which they were primed by the
 * client.
 *
 * The read command is the only way to retrieve buffers for the encoder from
 * the voice driver.
 *
 * To handle the Flush command, after a VS_VOC_CMD_FLUSH_BUFFERS command
 * completes, the client must call the Read command to retrieve the
 * flushed buffers.
 *
 * Not all buffers contain speech frames. The client should check the buffer
 * to see if there is a valid speech frame.
 *
 * The client may continue reading for buffers until the command returns
 * VS_ENOTEXIST to indicate that there are no more buffers available to read.
 *
 * - Returns VS_EOK if a buffer is available. \n
 * - Returns VS_ENOTEXIST when no buffers are available.
 */
#define VS_VOC_CMD_READ_BUFFER ( 0x00012E30 )

typedef struct vs_voc_cmd_read_buffer_t vs_voc_cmd_read_buffer_t;

/** Vocoder Read Buffer command. */
struct vs_voc_cmd_read_buffer_t
{
  uint32_t handle; /**< Open handle. */
  vs_voc_buffer_t** ret_buffer; /**< Buffer structure that is returned to the client. */
};


 /** Write Buffer command (synchronous nonblocking API).
 *
 * Writes a buffer to the decoder.
 *
 * The buffer does not return until the decoder consumes the buffer or receives
 * the VS_VOC_CMD_FLUSH_BUFFERS command. In all cases, a buffer is
 * returned to the client after the client receives the
 * VS_VOC_EVENT_WRITE_BUFFER_RETURNED event that notifies the client of the
 * specific buffer that is returned.
 */
#define VS_VOC_CMD_WRITE_BUFFER ( 0x00012E31 )

typedef struct vs_voc_cmd_write_buffer_t vs_voc_cmd_write_buffer_t;

/** Vocoder Write Buffer command. */
struct vs_voc_cmd_write_buffer_t
{
  uint32_t handle; /**< Open handle. */
  vs_voc_buffer_t* buffer; /**< Buffer to send to the decoder. @newpagetable */
};


 /** Write Buffer Returned event (asynchronous event).
 *
 * Event that notifies the client that a write buffer is returned.
 */
#define VS_VOC_EVENT_WRITE_BUFFER_RETURNED ( 0x00012E32 )

typedef struct vs_voc_event_write_buffer_returned_t vs_voc_event_write_buffer_returned_t;

/** Vocoder Write Buffer Returned event. */
struct vs_voc_event_write_buffer_returned_t
{
  vs_voc_buffer_t* buffer; /**< Buffer structure that is returned to the client. */
};


 /** Flush Buffers command (asynchronous nonblocking API).
 *
 * Flushes both the read and write buffers from the voice driver.
 *
 * The write buffers are returned through the
 * VS_VOC_EVENT_WRITE_BUFFER_RETURNED events before the Flush command
 * completes. One event per buffer is sent by VS to its client.
 *
 * The read buffers are returned through the VS_VOC_CMD_READ_BUFFER
 * command upon receiving the VS_VOC_EVENT_READ_AVAILABLE events before the
 * Flush command completes. The client can optionally repeatedly call the Read
 * command after the Flush command completes to retrieve all the flushed read
 * buffers at once.
 *
 * The VS_COMMON_EVENT_CMD_RESPONSE event is sent to the client after the
 * flush has completed.
 */
#define VS_VOC_CMD_FLUSH_BUFFERS ( 0x00012E33 )

typedef struct vs_voc_cmd_flush_buffers_t vs_voc_cmd_flush_buffers_t;

/** Vocoder Flush Buffers command. */
struct vs_voc_cmd_flush_buffers_t
{
  uint32_t handle; /**< Open handle. */
  void* client_context; /**< Client-provided data pointer for the client's use. */
};

/** @} */ /* end_addtogroup vs_voc */

#endif /* __VS_VOC_H__ */


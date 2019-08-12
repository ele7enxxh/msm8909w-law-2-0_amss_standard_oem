#ifndef __VSS_IPKTEXG_PUBLIC_IF_H__
#define __VSS_IPKTEXG_PUBLIC_IF_H__

/**
  @file vss_ipktexg_public_if.h
  @brief This file contains the APR API definitions for CVD vocoder packet
  exchange.
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The description that displays in the PDF is located in the
      CVD_mainpage.dox file. Contact Tech Pubs for assistance.
=============================================================================*/
/*===========================================================================
Copyright (c) 2014 Qualcomm Technologies, Inc.
All rights reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/audio_avs/api/cvd/vss_ipktexg_public_if.h#1 $
  $Author: mplcsds1 $

when        who    what, where, why
-------     ---    ----------------------------------------------------------
05/05/14     sw    (Tech Pubs) Edited Doxygen comments for 2.4.

===========================================================================*/


#include "apr_comdef.h"


/****************************************************************************
 * VOCODER PACKET EXCHANGE MODE SELECTION APIS                              *
 ****************************************************************************/

/** @addtogroup cvd_voc_px_constants
@{ */
/** In-band packet exchange mode. */
#define VSS_IPKTEXG_MODE_IN_BAND ( 0 )

/** Out-of-band packet exchange mode. */
#define VSS_IPKTEXG_MODE_OUT_OF_BAND ( 1 )

/** Mailbox packet exchange mode. */
#define VSS_IPKTEXG_MODE_MAILBOX ( 2 )
/** @} */ /* end_addtogroup cvd_voc_px_constants */

/** @ingroup cvd_voc_px_cmd_set_mode
  Sets the vocoder packet exchange mode.
 
  @apr_msgpayload{vss_ipktexg_cmd_set_mode_t}
  @wktable{weak__vss__ipktexg__cmd__set__mode__t}

  @detdesc
  Following are the properties of vocoder packet exchange mode selection:
  - The default mode is #VSS_IPKTEXG_MODE_IN_BAND.
  - The mode can be changed only when the stream is in the Disabled state.
  - If the mode being selected requires any configuration parameters, they
    must be provided via the correspoding configuration command (while the
    stream is in the Disabled state).
  @par
  If the configuration information is not provided, packet exchanges do not
  occur.
 
  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q2,[Q2]}).

  @dependencies
  None.
*/ /* Q2 = Async Packet Router API for MDM9x15 Interface Spec (80-N1463-2)
*/
#define VSS_IPKTEXG_CMD_SET_MODE ( 0x0001136A )

/* Type definition for vss_ipktexg_cmd_set_mode_t. */
typedef struct vss_ipktexg_cmd_set_mode_t vss_ipktexg_cmd_set_mode_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_ipktexg_cmd_set_mode_t
@{ */
/* Payload structure for the VSS_IPKTEXG_CMD_SET_MODE command.
*/
struct vss_ipktexg_cmd_set_mode_t
{
   uint32_t mode;
   /**< Vocoder packet exchange mode.

        @values
        - #VSS_IPKTEXG_MODE_IN_BAND
        - #VSS_IPKTEXG_MODE_OUT_OF_BAND
        - #VSS_IPKTEXG_MODE_MAILBOX @tablebulletend */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_ipktexg_cmd_set_mode_t */


/****************************************************************************
 * IN-BAND VOCODER PACKET EXCHANGE APIS                                     *
 ****************************************************************************/

/** @ingroup cvd_voc_px_evt_ib_send_enc
  Sent by the stream to provide an encoded packet to its client.

  @apr_msgpayload{vss_ipktexg_evt_in_band_send_enc_buffer_t}
  @wktablens{weak__vss__ipktexg__evt__in__band__send__enc__buffer__t}

  @return
  None.

  @dependencies
  None.
*/
#define VSS_IPKTEXG_EVT_IN_BAND_SEND_ENC_BUFFER ( 0x00011015 )

/* Type definition for vss_ipktexg_evt_in_band_send_enc_buffer_t. */
typedef struct vss_ipktexg_evt_in_band_send_enc_buffer_t vss_ipktexg_evt_in_band_send_enc_buffer_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_ipktexg_evt_in_band_send_enc_buffer_t
@{ */
/* Payload structure for the VSS_IPKTEXG_EVT_IN_BAND_SEND_ENC_BUFFER
    event.
*/
struct vss_ipktexg_evt_in_band_send_enc_buffer_t
{
   uint32_t media_id;
   /**< Media ID of the packet.
           
        @values See Section @xref{dox:MediaDefinitions} */

#if __STDC_VERSION__ >= 199901L
   uint8_t content[];   /**< Data content. */
#endif /* __STDC_VERSION__ >= 199901L */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_ipktexg_evt_in_band_send_enc_buffer_t */


/** @ingroup cvd_voc_px_evt_ib_req_dec
  Sent by the stream to request a decoder packet from its client.
  The client responds with a #VSS_IPKTEXG_EVT_IN_BAND_SEND_DEC_BUFFER event.

  @apr_msgpayload
  None.

  @return
  None.

  @dependencies
  None. @newpage
*/
#define VSS_IPKTEXG_EVT_IN_BAND_REQUEST_DEC_BUFFER ( 0x00011017 )


/** @ingroup cvd_voc_px_evt_ib_send_dec
  Sent by the client to the stream in response to a
  #VSS_IPKTEXG_EVT_IN_BAND_REQUEST_DEC_BUFFER event that provides a decoder
  packet.

  @apr_msgpayload{vss_ipktexg_evt_in_band_send_dec_buffer_t}
  @wktablens{weak__vss__ipktexg__evt__in__band__send__dec__buffer__t}

  @return
  None.

  @dependencies
  None.
*/
#define VSS_IPKTEXG_EVT_IN_BAND_SEND_DEC_BUFFER ( 0x00011016 )

/* Type definition for vss_ipktexg_evt_in_band_send_dec_buffer_t. */
typedef struct vss_ipktexg_evt_in_band_send_dec_buffer_t vss_ipktexg_evt_in_band_send_dec_buffer_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_ipktexg_evt_in_band_send_dec_buffer_t
@{ */
/* Payload structure for the VSS_IPKTEXG_EVT_IN_BAND_SEND_DEC_BUFFER
    event.
*/
struct vss_ipktexg_evt_in_band_send_dec_buffer_t
{
   uint32_t media_id;
   /**< Media ID of the packet.
           
        @values See Section @xref{dox:MediaDefinitions} */

#if __STDC_VERSION__ >= 199901L
   uint8_t content[];   /**< Data content. */
#endif /* __STDC_VERSION__ >= 199901L */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_ipktexg_evt_in_band_send_dec_buffer_t */


/****************************************************************************
 * OUT-OF-BAND VOCODER PACKET EXCHANGE APIS                                 *
 ****************************************************************************/

/** @ingroup cvd_voc_px_cmd_oob_set_cfg
  Sets the configuration information for #VSS_IPKTEXG_MODE_OUT_OF_BAND.
 
  @apr_msgpayload{vss_ipktexg_cmd_oob_set_config_t}
  @wktable{weak__vss__ipktexg__cmd__oob__set__config__t}
  
  @detdesc
  Following are the properties of out-of-band vocoder packet exchange:
  - The encoder and decoder buffers provided must be large enough to
    accommodate the maximum possible vocoder packet size.
  - The client can reclaim the encoder and decoder buffers only after the
    vocoder packet exchange mode has been successfully set on the stream to
    something other than #VSS_IPKTEXG_MODE_OUT_OF_BAND.
  - If the current vocoder packet exchange mode set on the stream is not
    VSS_IPKTEXG_MODE_OUT_OF_BAND, the configuration information is ignored
    and a failure is returned.
 
  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q2,[Q2]}).

  @dependencies
  The vocoder packet exchange mode must be successfully set on the stream to
  #VSS_IPKTEXG_MODE_OUT_OF_BAND via #VSS_IPKTEXG_CMD_SET_MODE.
  @par
  The memory for the encoder and decoder buffers must first be mapped with the
  CVD by following the guidelines in Section @xref{hdr:ShMemGuidelines}. @newpage
*/
#define VSS_IPKTEXG_CMD_OOB_SET_CONFIG ( 0x0001136B )

/* Type definition for vss_ipktexg_cmd_oob_set_config_t. */
typedef struct vss_ipktexg_cmd_oob_set_config_t vss_ipktexg_cmd_oob_set_config_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_ipktexg_cmd_oob_set_config_t
@{ */
/* Payload structure for the VSS_IPKTEXG_CMD_OOB_SET_CONFIG command.
*/
struct vss_ipktexg_cmd_oob_set_config_t
{
  uint32_t mem_handle;
  /**< Handle to the shared memory that holds encoder and decoder buffers. */

  uint64_t enc_buf_addr;
  /**< Location of the memory that holds the encoder buffer.

       For information on how the address is interpreted, see
       Section @xref{hdr:ShMemGuidelines}. */

  uint32_t enc_buf_size;
  /**< Size of the encoder buffer.

       The amount of memory allocated at enc_buf_addr must meet the
       requirement as specified in Section @xref{hdr:ShMemGuidelines}. */

  uint64_t dec_buf_addr;
  /**< Location of the memory that holds the decoder buffer.

       For information on how the address is interpreted, see
       Section @xref{hdr:ShMemGuidelines}. */

  uint32_t dec_buf_size;
  /**< Size of the decoder buffer.

       The amount of memory allocated at enc_buf_addr must meet the
       requirement as specified in Section @xref{hdr:ShMemGuidelines}. */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_ipktexg_cmd_oob_set_config_t */


/** @ingroup cvd_voc_px_evt_oob_enc_buf_rdy
  Sent by the stream to notify its client that an encoder packet is ready to
  be consumed by the client from shared memory.
  @apr_msgpayload
  None.

  @par Expected encoder buffer format in shared memory
@code
<-------- 32 bits -------->

 -------------------------
| timestamp               |  ---> Set to zero if not used
 -------------------------
| media_id                |
 -------------------------
| enc_data_size           |
 -------------------------
| enc_data                |
|                         |
| ...                     |
 -------------------------

@endcode

  @detdesc
  Except for the first encoder packet -- if the client does not send a
  #VSS_IPKTEXG_EVT_OOB_NOTIFY_ENC_BUFFER_CONSUMED event in response to the
  previous #VSS_IPKTEXG_EVT_OOB_NOTIFY_ENC_BUFFER_READY event by the time a
  new encoder packet is ready for delivery, the stream drops the new encoder
  packet.
 
  @return
  None.

  @dependencies
  The vocoder packet exchange mode must be set to
  #VSS_IPKTEXG_MODE_OUT_OF_BAND.
  @par
  The shared memory encoder buffer and size must be provided via 
  #VSS_IPKTEXG_CMD_OOB_SET_CONFIG.
*/
#define VSS_IPKTEXG_EVT_OOB_NOTIFY_ENC_BUFFER_READY ( 0x0001136C )


/** @ingroup cvd_voc_px_evt_oob_enc_buf_cons
  Sent by the client to notify the stream that the encoder packet has been
  consumed from shared memory, and the encoder buffer is available to the
  stream for the next encoder packet.
 
  @apr_msgpayload
  None.

  @detdesc
  Except for the first encoder packet -- if the client does not send this
  event in response to the previous
  #VSS_IPKTEXG_EVT_OOB_NOTIFY_ENC_BUFFER_READY event by the time a new encoder
  packet is ready for delivery, the stream drops the new encoder packet.

  @return
  None.

  @dependencies
  The vocoder packet exchange mode must be set to
  #VSS_IPKTEXG_MODE_OUT_OF_BAND.
  @par
  The shared memory encoder buffer and size must be provided via
  #VSS_IPKTEXG_CMD_OOB_SET_CONFIG.
*/
#define VSS_IPKTEXG_EVT_OOB_NOTIFY_ENC_BUFFER_CONSUMED ( 0x0001136D )


/** @ingroup cvd_voc_px_evt_oob_dec_buf_req
  Sent by the stream to notify its client that the stream is requesting the
  next decoder packet in shared memory.
  
  @apr_msgpayload
  None.

  @detdesc
  If the client does not send a #VSS_IPKTEXG_EVT_OOB_NOTIFY_DEC_BUFFER_READY
  event in response to this event by the time the decoder must begin 
  decoding the packet, the decoder generates an erasure frame.

  @return
  None.

  @dependencies
  The vocoder packet exchange mode must be set to
  #VSS_IPKTEXG_MODE_OUT_OF_BAND.
  @par
  The shared memory decoder buffer and size must be provided via
  #VSS_IPKTEXG_CMD_OOB_SET_CONFIG.
*/
#define VSS_IPKTEXG_EVT_OOB_NOTIFY_DEC_BUFFER_REQUEST ( 0x0001136E )


/** @ingroup cvd_voc_px_evt_oob_dec_buf_rdy
  Sent by the client to notify the stream that the decoder packet is available
  and ready to be consumed from shared memory.

  @apr_msgpayload
  None.

  @par Expected decoder buffer format in shared memory
@code
<-------- 32 bits -------->

 -------------------------
| timestamp               |  ---> Set to zero if not used
 -------------------------
| media_id                |
 -------------------------
| dec_data_size           |
 -------------------------
| dec_data                |
|                         |
| ...                     |
 -------------------------

@endcode

  @detdesc
  If the client does not send this event in response to the previous
  #VSS_IPKTEXG_EVT_OOB_NOTIFY_DEC_BUFFER_REQUEST event by the time the decoder
  must begin decoding the packet, the decoder generates an erasure frame.

  @return
  None.

  @dependencies
  The vocoder packet exchange mode must be set to
  #VSS_IPKTEXG_MODE_OUT_OF_BAND.
  @par
  The shared memory decoder buffer and size must be provided via 
  #VSS_IPKTEXG_CMD_OOB_SET_CONFIG.
*/
#define VSS_IPKTEXG_EVT_OOB_NOTIFY_DEC_BUFFER_READY ( 0x0001136F )


/****************************************************************************
 * MAILBOX VOCODER PACKET EXCHANGE APIS                                     *
 ****************************************************************************/

/** @ingroup cvd_voc_px_cmd_mbox_set_cfg
  Sets the configuration information for #VSS_IPKTEXG_MODE_MAILBOX.
 
  @apr_msgpayload{vss_ipktexg_cmd_mailbox_set_config_t}
  @wktable{weak__vss__ipktexg__cmd__mailbox__set__config__t}
  
  @par Encoder packet payload (vss_ipktexg_mailbox_enc_packet_t)
  @wktable{weak__vss__ipktexg__mailbox__enc__packet__t}
 
  @par Decoder packet payload (vss_ipktexg_mailbox_dec_packet_t)
  @wktable{weak__vss__ipktexg__mailbox__dec__packet__t}

  @par Encoding request format (vss_ipktexg_mailbox_enc_request_t)
  @wktable{weak__vss__ipktexg__mailbox__enc__request__t}
    @par
    An encoding request allows the client to specify the timestamp of when
    an encoder packet is to be delivered into shared memory.
    @par
    The client ensures that the encoding request is available in shared
    memory at least 5 milliseconds before the requested timestamp. The
    client is to provide the next encoding request immediately after picking
    up an encoder packet.
    @par
    If the difference in timestamp values between two adjacent encoding
    requests is not exactly an encoder packet duration in microseconds, a
    skew occurs between the client timeline and stream timeline.

  @par Decoding request format (vss_ipktexg_mailbox_dec_request_t)
  @wktable{weak__vss__ipktexg__mailbox__dec__request__t}
    @par
    A decoding request allows the client to specify the timestamp of when the
    decoding is to start for a decoder packet.
    @par
    The client ensures that the decoding request is available in shared
    memory by the requested timestamp. The client does not modify the
    content of the decoder packet associated with a request after the request
    is made and before it is completed. Although the client may provide the 
    decoding request at any time before the requested timestamp, the client 
    is to provide it as close to the requested timestamp as possible to
    reduce voice call round trip delay.
    @par
    If the difference in timestamp values between two adjacent decoding
    requests is not exactly a decoder packet duration in microseconds, a
    skew occurs between the client timeline and stream timeline. @newpage

  @par Circular buffer format (vss_ipktexg_mailbox_voc_req_circ_buffer_t)
  @wktable{weak__vss__ipktexg__mailbox__voc__req__circ__buffer__t}
    @par
    The data[] array holds the vocoding requests.
    The total size of data[] in bytes is ( ( circular buffer mem_size ) -
    sizeof( vss_ipktexg_mailbox_voc_req_circ_buffer_t ) ).
    @par
    Each time a vocoding request is written into data[], the write offset is
    advanced by the size of the request.
    After each vocoding request is read from data[], the read offset is
    advanced by the size of the request. Any request written into data[] is
    not modified before it is read.
    @par
    The read offset and write offset reset to 0 when they reach the end
    of data[].
    @par
    The data[] array is empty when the read offset and write offset are equal.
    It is full when space is available to hold only one vocoding request. The
    client does not write a new vocoding request when data[] is full.

  @detdesc
    The client provides a separate circular buffer
    (#vss_ipktexg_mailbox_voc_req_circ_buffer_t) for each direction of the
    stream. The circular buffer is used to hold the client's vocoding requests.
    @par
    Any circular buffer can be reclaimed by the client only after one of the
    following:
     - The vocoder packet exchange mode has been successfully set on the stream
       to something other than #VSS_IPKTEXG_MODE_MAILBOX
     - The client has successfully destroyed the stream session.

  @par Start the mailbox vocoder packet exchange
    Following are the steps for starting the exchange:
     -# The stream sends #VSS_IPKTEXG_EVT_MAILBOX_REQUEST_TO_START to request
        its client to start the mailbox vocoder packet exchange.
     -# The client issues #VSS_IPKTEXG_CMD_MAILBOX_RESET to clear the Tx and Rx
        circular buffers, i.e. sets each circular buffer's
        read offset = write offset = 0.
     -# The client calls certain CVD APIs, such as getting the CVD's current
        time.
     -# The client starts to make vocoding requests to the stream.
     -# The client issues #VSS_IPKTEXG_CMD_MAILBOX_START to provide the
        vocoder packet exchange time reference to the stream.
     -# The client continues to make vocoding requests to the stream.

  @par Make a vocoding request
    Following are the steps for a vocoding request on a stream:
    -# The client places the request inside the corresponding circular buffer.
    -# The client advances the circular buffer's write offset by the size of
       the request.
    -# When the stream completes a vocoding request, it advances the circular
       buffer's read offset by the size of the request.
    -# When the stream is active, the client can reclaimed the vocoder packet
       associated with the vocoding request only after the request is
       completed.

  @par Stop the mailbox vocoder packet exchange
    Following are the steps for stopping the mailbox vocoder packet exchange:
    -# The stream sends #VSS_ISTREAM_EVT_NOT_READY to notify its cclient that
       the stream has become inactive. \n
       Any pending vocoding requests in either the Tx or Rx circular buffer
       are not processed by the stream after the stream becomes inactive.
    -# After receiving VSS_ISTREAM_EVT_NOT_READY, the client stops sending
       vocoding reuqests.
    -# After generating the VSS_ISTREAM_EVT_NOT_READY event, the stream does 
       not modify the circular buffer content (until the next
       #VSS_IPKTEXG_CMD_MAILBOX_RESET command).
    @par
    Qualcomm recommends that upon receiving #VSS_ISTREAM_EVT_NOT_READY, the
    client examines and delivers to the network any available encoder packets
    associated with the completed encoding requests in the Tx circular buffer.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q2,[Q2]}).

  @dependencies
  The vocoder packet exchange mode must be successfully set on the stream to
  #VSS_IPKTEXG_MODE_MAILBOX via #VSS_IPKTEXG_CMD_SET_MODE.
  @par
  The memory to be used for mailbox vocoder packet exchange must first be 
  mapped with the CVD by following the guidelines in
  Section @xref{hdr:ShMemGuidelines}.
  @par
  This command can be issued to the stream only when the stream is in
  the Disabled state.
  @par
  A valid media ID must have been set on each direction of the stream via
  #VSS_ISTREAM_CMD_SET_MEDIA_TYPE.
  @par
  The client must handle the #VSS_IPKTEXG_EVT_MAILBOX_REQUEST_TO_START,
  #VSS_ISTREAM_EVT_READY, and #VSS_ISTREAM_EVT_NOT_READY events as described
  in the procedures above for starting and stopping the mailbox vocoder
  packet exchange.
*/
#define VSS_IPKTEXG_CMD_MAILBOX_SET_CONFIG ( 0x00013054 )

/* Type definition for vss_ipktexg_mailbox_enc_packet_t. */
typedef struct vss_ipktexg_mailbox_enc_packet_t vss_ipktexg_mailbox_enc_packet_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_ipktexg_mailbox_enc_packet_t
@{ */
/* Structure for specifying an encoder packet.
*/
struct vss_ipktexg_mailbox_enc_packet_t
{
  uint16_t minor_version;
  /**< Minor version of the encoder packet.

       @values 0 */

  uint64_t capture_timestamp_us;
  /**< Capture timestamp at microphone in microseconds.

       The clock source used for interpreting the timestamp is specific
       per product requirements. */

  uint32_t status_mask;
  /**< Bitmask that indicates the encoder packet status.

       @values
       - if bit 0 is set -- Indicates a general encoding error, informing the
         client that the packet has been dropped. The data_size set to 0.
       - If bits 1 to 31 are set - Reserved; set to 0. @tablebulletend */

  uint32_t media_id;
  /**< Media ID of the encoder packet.

       @values See Section @xref{dox:MediaDefinitions} */

  uint32_t data_size;
  /**< Actual size of the encoder media data, in bytes. */

#if __STDC_VERSION__ >= 199901L
  uint8_t data[];
  /**< Encoder media data of size data_size.
       The media data format is specified by @xhyperref{Q6,[Q6]}. */
  /* Q6 = Vocoder Media Types Application Note (80-N8828-1) */
#endif /* __STDC_VERSION__ >= 199901L */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_ipktexg_mailbox_enc_packet_t */

/* Type definition for vss_ipktexg_mailbox_dec_packet_t. */
typedef struct vss_ipktexg_mailbox_dec_packet_t vss_ipktexg_mailbox_dec_packet_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_ipktexg_mailbox_dec_packet_t
@{ */
/* Structure for specifying a decoder packet.
*/
struct vss_ipktexg_mailbox_dec_packet_t
{
  uint16_t minor_version;
  /**< Minor version of the decoder packet.
  
       @values 0 */

  uint64_t render_timestamp_us;
  /**< Reserved. Set to 0. */

  uint32_t status_mask;
  /**< Bitmask that indicates the decoder packet status.
  
       @values
       - If bit 0 is set -- Indicates a general error, informing the stream
         that the packet has been dropped. The data_size is set to 0.
       - If bits 1 to 31 are set -- Reserved; set to 0. @tablebulletend */

  uint32_t media_id;
  /**< Media ID of the decoder packet.

       @values See Section @xref{dox:MediaDefinitions} */

  uint32_t data_size;
  /**< Actual size of the decoder media data, in bytes. */

#if __STDC_VERSION__ >= 199901L
  uint8_t data[];
  /**< Decoder media data of size data_size.
       The media data format is specified by @xhyperref{Q6,[Q6]}. */
#endif /* __STDC_VERSION__ >= 199901L */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_ipktexg_mailbox_dec_packet_t */

typedef struct vss_ipktexg_mailbox_enc_request_t vss_ipktexg_mailbox_enc_request_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_ipktexg_mailbox_enc_request_t
@{ */
/* Structure for specifying an encoding request.
*/
struct vss_ipktexg_mailbox_enc_request_t
{
  uint64_t timestamp_us;
  /**< Timestamp in microseconds that specifies when an encoder packet is to
       be delivered into shared memory.

       The clock source to be used for interpreting the timestamp is product
       requirement specific. */

  uint64_t mem_address;
  /**< Location of the encoder packet. The format of the encoder packet is 
       specified by vss_ipktexg_mailbox_enc_packet_t.

       For information on how the address is interpreted, see
       Section @xref{hdr:ShMemGuidelines}. */

  uint32_t mem_size;
  /**< Size of the encoder packet in bytes.

       The amount of memory allocated in mem_address must meet the
       requirement specified in Section @xref{hdr:ShMemGuidelines}. The
       amount of memory allocated must be sufficient to hold the largest 
       possible encoder packet for the media ID currently configured on the
       stream.

       The size of the largest possible encoder packet can be queried via the
       #VSS_IPKTEXG_CMD_MAILBOX_GET_VOC_PACKET_PROPERTIES command. */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_ipktexg_mailbox_enc_request_t */

/* Type definition for vss_ipktexg_mailbox_dec_request_t. */
typedef struct vss_ipktexg_mailbox_dec_request_t vss_ipktexg_mailbox_dec_request_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_ipktexg_mailbox_dec_request_t
@{ */
/* Structure for specifying a decoding request.
*/
struct vss_ipktexg_mailbox_dec_request_t
{
  uint64_t timestamp_us;
  /**< Timestamp in microseconds that specifies when the decoding is to start
       for a decoder packet.

       The clock source to be used for interpreting the timestamp is product
       requirement specific. */

  uint64_t mem_address;
  /**< Location of the decoder packet. The format of the decoder packet is 
       specified by vss_ipktexg_mailbox_dec_packet_t.

       For information on how the address is interpreted, see
       Section @xref{hdr:ShMemGuidelines}. */

  uint32_t mem_size;
  /**< Size of the decoder packet in bytes.

       The amount of memory allocated at mem_address must meet the requirement
       specified in Section @xref{hdr:ShMemGuidelines}. */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_ipktexg_mailbox_dec_request_t */

/* Type definition for vss_ipktexg_mailbox_voc_req_circ_buffer_t. */
typedef struct vss_ipktexg_mailbox_voc_req_circ_buffer_t vss_ipktexg_mailbox_voc_req_circ_buffer_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_ipktexg_mailbox_voc_req_circ_buffer_t
@{ */
/* Structure for specifying the format of a circular buffer that holds
    vocoding requests.
*/
struct vss_ipktexg_mailbox_voc_req_circ_buffer_t
{
  uint64_t read_offset;
  /**< Offset in bytes from the beginning of data[] where the next vocoding
       request is read. */

  uint8_t padding_1[ 120 ];
  /**< Padding for cache line alignment. The content in the padding is
       ignored. */

  uint64_t write_offset;
  /**< Offset in bytes from the beginning of data[] where the next vocoding
       request is written. */

  uint8_t padding_2[ 120 ];
  /**< Padding for cache line alignment. The content in the padding is
       ignored. */

#if __STDC_VERSION__ >= 199901L
  uint8_t data[];       /**< Circular buffer data. */
#endif /* __STDC_VERSION__ >= 199901L */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_ipktexg_mailbox_voc_req_circ_buffer_t */

/* Type definition for vss_ipktexg_cmd_mailbox_set_config_t.
*/
typedef struct vss_ipktexg_cmd_mailbox_set_config_t vss_ipktexg_cmd_mailbox_set_config_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_ipktexg_cmd_mailbox_set_config_t
@{ */
/* Payload structure for the VSS_IPKTEXG_CMD_MAILBOX_SET_CONFIG
    command.
*/
struct vss_ipktexg_cmd_mailbox_set_config_t
{
  uint32_t mem_handle;
  /**< Handle to the shared memory to be used for mailbox vocoder packet 
       exchange. For more information, see Section @xref{hdr:ShMemGuidelines}. */

  uint64_t tx_circ_buf_mem_address;
  /**< Location of the Tx circular buffer used to hold encoding requests.

       For information on how the address is interpreted, see
       Section @xref{hdr:ShMemGuidelines}.

       If the stream direction is Rx only, this field is set to 0 and ignored. */

  uint32_t tx_circ_buf_mem_size;
  /**< Size of the Tx circular buffer, in bytes.

       The amount of memory allocated at tx_circ_buf_mem_address must meet the
       requirement specified in Section @xref{hdr:ShMemGuidelines}.

       Theamount of memory available to hold encoding requests must be an
       integer multiple of the size of a single encoding request data
       structure.

       If the stream direction is Rx only, this field is set to 0 and ignored. */

  uint64_t rx_circ_buf_mem_address;
  /**< Location of the Rx circular buffer used to hold decoding requests.

       For information on how the address is interpreted, see
       Section @xref{hdr:ShMemGuidelines}.

       If the stream direction is Tx only, this field is set to 0 and ignored. */

  uint32_t rx_circ_buf_mem_size;
  /**< Size of the Rx circular buffer in bytes.

       The amount of memory allocated at rx_circ_buf_mem_address must meet the
       requirement specified in Section @xref{hdr:ShMemGuidelines}.

       The amount of memory available to hold decoding requests must be an
       integer multiple of the size of a single decoding request data
       structure.

       If the stream direction is Tx only, this field is set to 0 and ignored. */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_ipktexg_cmd_mailbox_set_config_t */


/** @ingroup cvd_voc_px_evt_mbox_req_start
  Sent by the stream to request its client to start the mailbox vocoder packet
  exchange.

  @apr_msgpayload
  None.

  @detdesc
  This event is sent to the source address and source port of the client who
  issued the #VSS_IPKTEXG_CMD_MAILBOX_SET_CONFIG command.

  @return
  None.

  @dependencies
  For the client to receive this event, the vocoder packet exchange
  mode must be set to #VSS_IPKTEXG_MODE_MAILBOX, and the mailbox vocoder packet
  exchange must be configured via #VSS_IPKTEXG_CMD_MAILBOX_SET_CONFIG.
*/
#define VSS_IPKTEXG_EVT_MAILBOX_REQUEST_TO_START ( 0x00013055 )


/** @ingroup cvd_voc_px_cmd_mbox_reset
  Resets the mailbox vocoder packet exchange.
  
  @apr_msgpayload
  None.

  @detdesc
  The client issues this command to the stream to clear the circular buffers
  in both the Tx and Rx directions.
  @par
  This command is mandatory for the mailbox vocoder packet exchange start
  handling procedure.
  @par
  This command is issued after the client receives
  #VSS_IPKTEXG_EVT_MAILBOX_REQUEST_TO_START, and before the client issues
  #VSS_IPKTEXG_CMD_MAILBOX_START.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q2,[Q2]}).

  @dependencies
  The vocoder packet exchange mode must be set to #VSS_IPKTEXG_MODE_MAILBOX.
  @par
  The mailbox vocoder packet exchange must be configured via the 
  #VSS_IPKTEXG_CMD_MAILBOX_SET_CONFIG command.
*/
#define VSS_IPKTEXG_CMD_MAILBOX_RESET ( 0x000130AD )


/** @ingroup cvd_voc_px_cmd_mbox_start
  Starts the mailbox vocoder packet exchange operation.

  @apr_msgpayload{vss_ipktexg_cmd_mailbox_start_t}
  @wktable{weak__vss__ipktexg__cmd__mailbox__start__t}

  @detdesc
  The client issues this command to the stream to provide the client's vocoder
  packet exchange time reference to the stream.
  @par
  This command is mandatory for the mailbox vocoder packet exchange start
  procedure.
  @par
  The time reference provided in the payload of this command is used to
  align the stream's timeline to the client's timeline at the beginning of the 
  mailbox vocoder packet exchange operation. For more information, see the
  #VSS_IPKTEXG_CMD_MAILBOX_SET_CONFIG command.
  @par
  This command is issued after the client receives
  #VSS_IPKTEXG_EVT_MAILBOX_REQUEST_TO_START and issues
  #VSS_IPKTEXG_CMD_MAILBOX_RESET.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q2,[Q2]}).

  @dependencies
  The vocoder packet exchange mode must be set to #VSS_IPKTEXG_MODE_MAILBOX.
  @par
  The mailbox vocoder packet exchange must be configured via the 
  #VSS_IPKTEXG_CMD_MAILBOX_SET_CONFIG command.
*/
#define VSS_IPKTEXG_CMD_MAILBOX_START ( 0x00013056 )

/* Type definition for vss_ipktexg_cmd_mailbox_start_t.
*/
typedef struct vss_ipktexg_cmd_mailbox_start_t vss_ipktexg_cmd_mailbox_start_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_ipktexg_cmd_mailbox_start_t
@{ */
/* Payload structure for the VSS_IPKTEXG_CMD_MAILBOX_START command.
*/
struct vss_ipktexg_cmd_mailbox_start_t
{
  uint64_t tx_ref_timstamp_us;
  /**< Client's encoder packet exchange time reference in microseconds.

       The time reference is used to align the encoder's timeline to the
       client's timeline at the beginning of the mailbox vocoder packet 
       exchange operation.

       The first encoder packet is ready for delivery at an integer multiple
       of a single encoder packet duration away from tx_ref_timstamp_us.
       The clock source to be used for interpreting the timestamp is product
       specific.

       If the stream direction is Rx only, this field is set to 0 and ignored. */

  uint64_t rx_ref_timstamp_us;
  /**< Client's decoder packet exchange time reference in microseconds.

       The time reference is used to align the decoder's timeline to the
       client's timeline at the beginning of the mailbox vocoder packet 
       exchange operation.

       Decoding of the first decoder packet starts at an integer multiple of
       a single decoder packet duration away from rx_ref_timstamp_us.
       The clock source to be used for interpreting the timestamp is product
       specific.

       If the stream direction is Tx only, this field is set to 0 and ignored. */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_ipktexg_cmd_mailbox_start_t */


/** @ingroup cvd_voc_px_cmd_mbx_get_pkt_prop
  Gets the properties of the vocoder packets used in mailbox vocoder packet
  exchange for the media ID currently configured on each direction of the
  stream.

  @apr_msgpayload
  None.

  @return
  #VSS_IPKTEXG_RSP_MAILBOX_GET_VOC_PACKET_PROPERTIES -- Success.
  @par
  APRV2_IBASIC_RSP_RESULT -- Failure (refer to @xhyperref{Q2,[Q2]}).

  @dependencies
  A valid media ID must have been configured on each direction of the stream
  via #VSS_ISTREAM_CMD_SET_MEDIA_TYPE.
*/
#define VSS_IPKTEXG_CMD_MAILBOX_GET_VOC_PACKET_PROPERTIES ( 0x00013057 )


/** @ingroup cvd_voc_px_rsp_mbx_get_pkt_prop
  Response to the #VSS_IPKTEXG_CMD_MAILBOX_GET_VOC_PACKET_PROPERTIES command.

  @apr_msgpayload{vss_ipktexg_rsp_mailbox_get_voc_packet_properties_t}
  @wktable{weak__vss__ipktexg__rsp__mailbox__get__voc__packet__properties__t}

  @return
  None.

  @dependencies
  None.
*/
#define VSS_IPKTEXG_RSP_MAILBOX_GET_VOC_PACKET_PROPERTIES ( 0x00013058 )

/* Type definition for vss_ipktexg_rsp_mailbox_get_voc_packet_properties_t. */
typedef struct vss_ipktexg_rsp_mailbox_get_voc_packet_properties_t vss_ipktexg_rsp_mailbox_get_voc_packet_properties_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_ipktexg_rsp_mailbox_get_voc_packet_properties_t
@{ */
/* Payload structure for the 
    VSS_IPKTEXG_RSP_MAILBOX_GET_VOC_PACKET_PROPERTIES command response.
*/
struct vss_ipktexg_rsp_mailbox_get_voc_packet_properties_t
{
  uint32_t tx_duration_us;
  /**< Duration in microseconds of the media data contained in a single
       encoder packet.

       If the stream direction is Rx only, this field is set to 0 and ignored. */

  uint32_t tx_max_size;
  /**< Maximum encoder packet size in bytes.
       The size includes the encoder packet header and encoder media data.

       If the stream direction is Rx only, this field is set to 0 and ignored. */

  uint32_t rx_duration_us;
  /**< Duration in microseconds of the media data contained in a single
       decoder packet.

       If the stream direction is Tx only, this field is set to 0 and ignored. */

  uint32_t rx_max_size;
  /**< Maximum decoder packet size in bytes.
       The size includes the decoder packet header and decoder media data.

       If the stream direction is Tx only, this field is set to 0 and ignored. */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_ipktexg_rsp_mailbox_get_voc_packet_properties_t */

#endif /* __VSS_IPKTEXG_PUBLIC_IF_H__ */


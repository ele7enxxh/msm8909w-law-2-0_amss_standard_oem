#ifndef __VSS_ISTREAM_DEPRECATED_PUBLIC_IF_H__
#define __VSS_ISTREAM_DEPRECATED_PUBLIC_IF_H__

/**
  @file vss_istream_deprecated_public_if.h
  @brief This file contains the deprecated APR APIs for Core Voice Stream (CVS)
  module.
*/

/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The description that displays in the PDF is located in the
      CVD_mainpage.dox file. Contact Tech Pubs for assistance.
=============================================================================*/
/*===========================================================================
Copyright (C) 2011, 2014, 2016 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/api/cvd/vss_istream_deprecated_public_if.h#1 $
  $Author: mplcsds1 $

when        who    what, where, why
-------     ---    ----------------------------------------------------------
08/27/14    sw     (Tech Pubs) Updated with Doxygen comments from 2.0 and 2.4,
                   and edited for 2.6.

===========================================================================*/


#include "apr_comdef.h"
#include "vss_istream_public_if.h"

/** @addtogroup cvd_cvs_deprecated 
@{ */
/** @name Calibration data 
@{ */

/****************************************************************************
 * CALIBRATION DATA APIS                                                    *
 ****************************************************************************/

/**	Deprecated. Use #VSS_ISTREAM_CMD_REGISTER_STATIC_CALIBRATION_DATA.

  Provides per-network calibration data to the stream. Data provided here
  is cached and applied every time the stream transitions to the Run state. If
  this command is issued when the stream is in the Run state, the data is not
  applied until the next time the stream transitions to the Run state.

  @par Expected buffer format
@code
<-------- 32 bits -------->           <-------- 32 bits -------->

 -------------------------
| network_id              | --> network_id
 -------------------------
| data_len                | --> byte length of data block that follows
 -------------------------
| data                    | -------->  -------------------------
|                         |           | mod_id                  |
|                         |            -------------------------
| ...                     |           | param_id                |
 -------------------------             -------------------------
| network_id              |           | param_len  | reserved   |
 -------------------------             -------------------------
| data_len                |           | param_data              |
 -------------------------            |                         |
| data                    |           | ...                     |
|                         |            -------------------------
|                         |           | mod_id                  |
| ...                     |           .                         .
 -------------------------            .                         .
|                         |           .                         .
.                         .
.                         .

@endcode

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q2,[Q2]}).

  @dependencies
  None. @newpage
*/
#define VSS_ISTREAM_CMD_CACHE_CALIBRATION_DATA ( 0x000110FB )


/**	Deprecated. Use #VSS_ISTREAM_CMD_REGISTER_STATIC_CALIBRATION_DATA.

  Registers the memory that contains common calibration data table with the
  stream.

  @note1hang For guidelines on using shared memory, see
             Section @xref{hdr:ShMemGuidelines}.

  @apr_msgpayload{vss_istream_cmd_register_calibration_data_v2_t}
  @wktable{weak__vss__istream__cmd__register__calibration__data__v2__t}

  @par Expected buffer format
@code
                                      <-------- 32 bits -------->
 -------------------------
| column_1                | --> eg. network_id
 -------------------------
| column_2                | --> eg. rx_sampling_rate
 -------------------------
|  .                      |
|  .                      |
 -------------------------
| column_n                |
 -------------------------
| data_size               | --> byte length of data block that follows
 -------------------------
| data                    | -------->  -------------------------
|                         |           | mod_id                  |
|                         |            -------------------------
| ...                     |           | param_id                |
 -------------------------             -------------------------
| column_1                |           | param_size | reserved   |
 -------------------------             -------------------------
| column_2                |           | param_data              |
 -------------------------	          |                         |
|  .                      |           | ...                     |
|  .                      |            -------------------------
 -------------------------	          | mod_id                  |
| column_n                |           |                         |
 -------------------------	          |                         |
| data_size               |           .                         .
 -------------------------            .                         .
| data                    |           .                         .
|                         |
|                         |
| ...                     |
 ------------------------- 
|                         |
.                         .
.                         .

@endcode
  
  @detdesc
  As part of this registration, in addition to the address, size, and handle
  of the memory containing the calibration table, the client also provides
  the columns and associated column types the stream uses to form a
  key that is used to index into the calibration table and look up a matching
  entry. The columns/types must be provided in this command in the same order
  that they exist in the calibration table. If no matching entry is found,
  calibration data is not applied.
  @par
  Calibration data is applied when the stream transitions to the Run state.
  @par
  keep6
  The client is not allowed to modify the calibration data memory while it
  is registered and the stream does not modify this memory while registered.
  The calibration data at the registered memory is valid until unregistered. 
  Registering a memory region for calibration data while another region is
  still registered causes the command to fail.
  @par
  The supported column IDs that can be registered in this command are:
  - #VSS_ICOMMON_CAL_COLUMN_NETWORK
  - #VSS_ICOMMON_CAL_COLUMN_RX_SAMPLING_RATE
  - #VSS_ICOMMON_CAL_COLUMN_TX_SAMPLING_RATE

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q2,[Q2]}).

  @dependencies
  Any memory registered here must first be mapped with the CVD by following
  the guidelines in Section @xref{hdr:ShMemGuidelines}. @newpage
*/
#define VSS_ISTREAM_CMD_REGISTER_CALIBRATION_DATA_V2 ( 0x00011369 )

/* Type definition for vss_istream_cmd_register_calibration_data_v2_t. */

typedef struct vss_istream_cmd_register_calibration_data_v2_t vss_istream_cmd_register_calibration_data_v2_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_istream_cmd_register_calibration_data_v2_t 
@{ */
/* Payload of the VSS_ISTREAM_CMD_REGISTER_CALIBRATION_DATA_V2 command.
*/
struct vss_istream_cmd_register_calibration_data_v2_t
{
  uint32_t cal_mem_handle;
  /**< Handle to the shared memory that holds the calibration data. */


  uint64_t cal_mem_address;
  /**< Location of the calibration data. */


  uint32_t cal_mem_size;
  /**< Size of the calibration data in bytes.

       The amount of memory allocated at cal_mem_address must meet the
       requirement as specified in Section @xref{hdr:ShMemGuidelines}. */


  uint32_t num_columns;
  /**< Number of columns used to form the key for looking up a calibration 
       data entry. */


#if __STDC_VERSION__ >= 199901L
  vss_istream_cal_column_t columns[];
  /**< Array of columns of size num_columns. The order in which the columns
       are provided here must match the order in which they exist in the
       calibration table. */

#endif /* __STDC_VERSION__ >= 199901L */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_istream_cmd_register_calibration_data_v2_t */


/** Deprecated. Use #VSS_ISTREAM_CMD_DEREGISTER_STATIC_CALIBRATION_DATA.

  Deregisters the memory that contains common calibration data from the
  stream.

  @apr_msgpayload
  None.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q2,[Q2]}).

  @dependencies
  None. @newpage
*/
#define VSS_ISTREAM_CMD_DEREGISTER_CALIBRATION_DATA ( 0x0001127A )

/** @} */ /* end_name Calibration data */

/** @name In-call conversation recording
@{ */
/****************************************************************************
 * IN-CALL CONVERSATION RECORDING APIS                                      *
 ****************************************************************************/

/** Deprecated. Use #VSS_IRECORD_CMD_START.

  Starts the in-call conversation recording.
  This command forwards the in-call conversation stream to Rx AFE port 0x8003
  and Tx AFE port 0x8004.

  @apr_msgpayload{vss_istream_cmd_start_record_t}
  @wktable{weak__vss__istream__cmd__start__record__t}

  @detdesc
  The audio path is used to record the voice streams. For recording silence
  before a call or during handovers, AFE ports 0x8003 and 0x8004 must be
  configured to Live mode.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q2,[Q2]}).

  @dependencies
  Recording must be stopped before sending this command.
*/
#define VSS_ISTREAM_CMD_START_RECORD ( 0x00011236 )

/** Indicates that there is no tapping for the specified path. */
#define VSS_TAP_POINT_NONE ( 0x00010F78 )

/** Indicates that the specified path is to be tapped at the end of the stream. */
#define VSS_TAP_POINT_STREAM_END ( 0x00010F79 )

/* Type definition for vss_istream_cmd_start_record_t. */
typedef struct vss_istream_cmd_start_record_t vss_istream_cmd_start_record_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_istream_cmd_start_record_t 
@{ */
/* Payload structure for the VSS_ISTREAM_CMD_START_RECORD command.
*/
struct vss_istream_cmd_start_record_t
{
   uint32_t rx_tap_point;
   /**< Tap point to use on the Rx path.
      
        @values
        - #VSS_TAP_POINT_NONE
        - #VSS_TAP_POINT_STREAM_END @tablebulletend */

   uint32_t tx_tap_point;
   /**< Tap point to use on the Tx path.
      
        @values
        - #VSS_TAP_POINT_NONE
        - #VSS_TAP_POINT_STREAM_END @tablebulletend */
   }
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_istream_cmd_start_record_t */

/** Deprecated. Use #VSS_IRECORD_CMD_STOP.

  Stops the in-call conversation recording.
  This command stops forwarding the in-call conversation stream to Rx AFE
  port 0x8003 and Tx AFE port 0x8004.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q2,[Q2]}).

  @dependencies
  None.
*/
#define VSS_ISTREAM_CMD_STOP_RECORD ( 0x00011237 )

/** @} */ /* end_name In-call conversation recording */

/** @name In-call music delivery
@{ */
/****************************************************************************
 * IN-CALL MUSIC DELIVERY APIS                                              *
 ****************************************************************************/

/** Deprecated. Use #VSS_IPLAYBACK_CMD_START.

  Starts the in-call music delivery on the Tx voice path.

  @apr_msgpayload
  None.

  @detdesc
  This command establishes a connection between the Tx voice stream and Tx AFE
  port 0x8005, allowing the voice stream to receive music from the audio path.
  @par
  The audio path is used to inject music to the Tx voice path. Configuring AFE
  port 0x8005 to Non-live mode allows the voice modules to throttle transfer
  jitters.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q2,[Q2]}).

  @dependencies
  None. @newpage
*/
#define VSS_ISTREAM_CMD_START_PLAYBACK ( 0x00011238 )

/** Deprecated. Use #VSS_IPLAYBACK_CMD_STOP.

  Stops the in-call music delivery on the Tx voice path.

  @apr_msgpayload
  None.

  @detdesc
  This command stops the Tx voice stream from receiving music from Tx AFE
  port 0x8005.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q2,[Q2]}).

  @dependencies
  None.
*/
#define VSS_ISTREAM_CMD_STOP_PLAYBACK ( 0x00011239 )

/** @} */ /* end_name In-call music delivery */

/** @name Vocoder packet exchange mode selection
@{ */
/****************************************************************************
 * VOCODER PACKET EXCHANGE MODE SELECTION APIS                              *
 ****************************************************************************/

/** In-band packet exchange mode. */
#define VSS_ISTREAM_PACKET_EXCHANGE_MODE_INBAND ( 0 )

/** Out-of-band packet exchange mode. @newpage */
#define VSS_ISTREAM_PACKET_EXCHANGE_MODE_OUT_OF_BAND ( 1 )


/** Deprecated. Use #VSS_IPKTEXG_CMD_SET_MODE.

  Sets the vocoder packet exchange mode.

  @apr_msgpayload{vss_istream_cmd_set_packet_exchange_mode_t}
  @wktable{weak__vss__istream__cmd__set__packet__exchange__mode__t}

  @detdesc
  The properties of vocoder packet exchange mode selection are:
  - The default mode is #VSS_ISTREAM_PACKET_EXCHANGE_MODE_INBAND.
  - The mode can be changed only when the stream is in the Disabled state.
  - If the mode being selected requires any configuration parameters, they
    must be provided via the corresponding configuration command (while the
    stream is in the Disabled state). If the configuration information
    is not provided, packet exchanges do not occur.
 
  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q2,[Q2]}).

  @dependencies
  None. @newpage
*/
#define VSS_ISTREAM_CMD_SET_PACKET_EXCHANGE_MODE ( 0x0001136A )

/* Type definition for vss_istream_cmd_set_packet_exchange_mode_t. */
typedef struct vss_istream_cmd_set_packet_exchange_mode_t vss_istream_cmd_set_packet_exchange_mode_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_istream_cmd_set_packet_exchange_mode_t
@{ */
/* Payload structure for the VSS_ISTREAM_CMD_SET_PACKET_EXCHANGE_MODE
    command.
*/
struct vss_istream_cmd_set_packet_exchange_mode_t
{
   uint32_t mode;
   /**< Vocoder packet exchange mode. 
      
        @values
        - #VSS_ISTREAM_PACKET_EXCHANGE_MODE_INBAND
        - #VSS_ISTREAM_PACKET_EXCHANGE_MODE_OUT_OF_BAND @tbend2 */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_istream_cmd_set_packet_exchange_mode_t */

/** @} */ /* end_name Vocoder packet exchange mode selection */

/** @name In-band vocoder packet exchange
@{ */
/****************************************************************************
 * IN-BAND VOCODER PACKET EXCHANGE APIS                                     *
 ****************************************************************************/

/** Deprecated. Use #VSS_IPKTEXG_EVT_IN_BAND_SEND_ENC_BUFFER.

  Provides an encoded packet when sent by the stream to its client.

  @apr_msgpayload{vss_istream_evt_send_enc_buffer_t}
  @wktable{weak__vss__istream__evt__send__enc__buffer__t}

  @return
  None.

  @dependencies
  None.
*/
#define VSS_ISTREAM_EVT_SEND_ENC_BUFFER ( 0x00011015 )

/* Type definition for vss_istream_evt_send_enc_buffer_t. */
typedef struct vss_istream_evt_send_enc_buffer_t vss_istream_evt_send_enc_buffer_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_istream_evt_send_enc_buffer_t 
@{ */
/* Payload structure for the VSS_ISTREAM_EVT_SEND_ENC_BUFFER event.
*/
struct vss_istream_evt_send_enc_buffer_t
{
   uint32_t media_id;      /**< Media ID of the packet. */

#if __STDC_VERSION__ >= 199901L
   uint8_t content[];      /**< Data content. */
#endif /* __STDC_VERSION__ >= 199901L */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_istream_evt_send_enc_buffer_t */

/** Deprecated. Use #VSS_IPKTEXG_EVT_IN_BAND_REQUEST_DEC_BUFFER.

  Requests a decoded packet from the client. The client responds with a
  #VSS_ISTREAM_EVT_SEND_DEC_BUFFER event.

  @apr_msgpayload
  None.

  @return
  None.

  @dependencies
  None. @newpage
*/
#define VSS_ISTREAM_EVT_REQUEST_DEC_BUFFER ( 0x00011017 )

/** Deprecated. Use #VSS_IPKTEXG_EVT_IN_BAND_SEND_DEC_BUFFER.

  Sends a decoded packet from the client to the stream in response to a
  #VSS_ISTREAM_EVT_REQUEST_DEC_BUFFER event, which provides a decoder packet.

  @apr_msgpayload{vss_istream_evt_send_dec_buffer_t}
  @wktable{weak__vss__istream__evt__send__dec__buffer__t}

  @return
  None.

  @dependencies
  None. @newpage
*/
#define VSS_ISTREAM_EVT_SEND_DEC_BUFFER ( 0x00011016 )

/* Type definition for vss_istream_evt_send_dec_buffer_t. */
typedef struct vss_istream_evt_send_dec_buffer_t vss_istream_evt_send_dec_buffer_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_istream_evt_send_dec_buffer_t
@{ */
/* Payload structure for the VSS_ISTREAM_EVT_SEND_DEC_BUFFER event.
*/
struct vss_istream_evt_send_dec_buffer_t
{
   uint32_t media_id;      /**< Media ID of the packet. */

#if __STDC_VERSION__ >= 199901L
   uint8_t content[];      /**< Data content. */
#endif /* __STDC_VERSION__ >= 199901L */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_istream_evt_send_dec_buffer_t */

/** @} */ /* end_name In-band vocoder packet exchange */

/** @name Out-of-band vocoder packet exchange
@{ */
/****************************************************************************
 * OUT-OF-BAND VOCODER PACKET EXCHANGE APIS                                 *
 ****************************************************************************/

/** Deprecated. Use #VSS_IPKTEXG_CMD_OOB_SET_CONFIG.

  Sets the configuration information for 
  #VSS_ISTREAM_PACKET_EXCHANGE_MODE_OUT_OF_BAND.
 
  @note1hang For guidelines on using shared memory, see
             Section @xref{hdr:ShMemGuidelines}.

  @apr_msgpayload{vss_istream_cmd_set_oob_packet_exchange_config_t}
  @wktable{weak__vss__istream__cmd__set__oob__packet__exchange__config__t}

  @detdesc
  The properties of out-of-band vocoder packet exchange are:
  - The encoder and decoder buffers provided must be large enough to
    accommodate the maximum possible vocoder packet size.
  - The encoder and decoder buffers can be reclaimed by the client only after
    the vocoder packet exchange mode has been successfully set on the stream
    to something other than #VSS_ISTREAM_PACKET_EXCHANGE_MODE_OUT_OF_BAND.  
  - If the current vocoder packet exchange mode set on the stream is not
    VSS_ISTREAM_PACKET_EXCHANGE_MODE_OUT_OF_BAND, the configuration
    information is ignored and a failure is returned.
 
  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q2,[Q2]}).

  @dependencies
  The vocoder packet exchange mode must be successfully set on the stream to
  #VSS_ISTREAM_PACKET_EXCHANGE_MODE_OUT_OF_BAND via
  #VSS_ISTREAM_CMD_SET_PACKET_EXCHANGE_MODE.
  @par
  The memory for the encoder and decoder buffers must first be mapped with the
  CVD by following the guidelines in Section @xref{hdr:ShMemGuidelines}.
  @newpage
*/
#define VSS_ISTREAM_CMD_SET_OOB_PACKET_EXCHANGE_CONFIG ( 0x0001136B )

/* Type definition for vss_istream_cmd_set_oob_packet_exchange_config_t. */
typedef struct vss_istream_cmd_set_oob_packet_exchange_config_t vss_istream_cmd_set_oob_packet_exchange_config_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_istream_cmd_set_oob_packet_exchange_config_t 
@{ */
/* Payload structure for the VSS_ISTREAM_CMD_SET_OOB_PACKET_EXCHANGE_CONFIG
    command.
*/
struct vss_istream_cmd_set_oob_packet_exchange_config_t
{
  uint32_t mem_handle;
  /**< Handle to the shared memory that holds the encoder and decoder 
       buffers. */

  uint64_t enc_buf_addr;
  /**< Location of the memory that holds the encoder buffer. */

  uint32_t enc_buf_size;
  /**< Size of the encoder buffer. The amount of memory allocated
       at enc_buf_addr must meet the requirement as specified in
       Section&nbsp;@xref{hdr:ShMemGuidelines}. */

  uint64_t dec_buf_addr;
  /**< Location of memory that holds the decoder buffer. */

  uint32_t dec_buf_size;
  /**< Size of the decoder buffer. The amount of memory allocated
       at dec_buf_size must meet the requirement as specified in
       Section&nbsp@xref{hdr:ShMemGuidelines}. */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_istream_cmd_set_oob_packet_exchange_config_t */

/** Deprecated. Use #VSS_IPKTEXG_EVT_OOB_NOTIFY_ENC_BUFFER_READY.

  Sent by the stream to notify its client that an encoder packet is
  ready to be consumed by the client from shared memory.

  @apr_msgpayload
  None.  

  @par Encoder buffer format in shared memory
@code
<-------- 32 bits -------->

 -------------------------
| timestamp               |  ---> Set to zero if not used
 -------------------------
| media_type              |
 -------------------------
| enc_data_size           |
 -------------------------
| enc_data                |
|                         |
| ...                     |
 -------------------------

@endcode

  @detdesc
  With the exception of the first encoder packet, the stream drops the new
  encoder packet if a #VSS_ISTREAM_EVT_OOB_NOTIFY_ENC_BUFFER_CONSUMED event is
  not received from the client in response to the previous 
  #VSS_ISTREAM_EVT_OOB_NOTIFY_ENC_BUFFER_READY event by the time a new encoder
  packet is ready for delivery.

  @apr_msgpayload
  None.

  @dependencies
  The vocoder packet exchange mode must be set to
  #VSS_ISTREAM_PACKET_EXCHANGE_MODE_OUT_OF_BAND, and the shared memory encoder 
  buffer and size must be provided via 
  #VSS_ISTREAM_CMD_SET_OOB_PACKET_EXCHANGE_CONFIG. @newpage
*/
#define VSS_ISTREAM_EVT_OOB_NOTIFY_ENC_BUFFER_READY ( 0x0001136C )

/** Deprecated. Use #VSS_IPKTEXG_EVT_OOB_NOTIFY_ENC_BUFFER_CONSUMED.

  Sent by the client to notify the stream that the encoder packet has
  been consumed from shared memory, and the encoder buffer is available to the
  stream for the next encoder packet.
 
  @apr_msgpayload
  None.  

  @detdesc
  With the exception of the first encoder packet, the stream drops the new
  encoder packet if this event is not received from the client in response to
  the previous #VSS_ISTREAM_EVT_OOB_NOTIFY_ENC_BUFFER_READY event by the time
  a new encoder packet is ready for delivery.

  @return
  None.

  @dependencies
  The vocoder packet exchange mode must be set to
  #VSS_ISTREAM_PACKET_EXCHANGE_MODE_OUT_OF_BAND, and the shared memory encoder
  buffer and size must be provided via 
  #VSS_ISTREAM_CMD_SET_OOB_PACKET_EXCHANGE_CONFIG. @newpage
*/
#define VSS_ISTREAM_EVT_OOB_NOTIFY_ENC_BUFFER_CONSUMED ( 0x0001136D )

/** Deprecated. Use #VSS_IPKTEXG_EVT_OOB_NOTIFY_DEC_BUFFER_REQUEST.

  Sent by the stream to notify its client that the stream is
  requesting the next decoder packet in shared memory.
  
  @apr_msgpayload
  None.   

  @detdesc
  An erasure is produced by the decoder if a
  #VSS_ISTREAM_EVT_OOB_NOTIFY_DEC_BUFFER_READY event is not received from
  the client in response to this event by the time the decoder must begin 
  decoding the packet.  

  @return
  None.

  @dependencies
  The vocoder packet exchange mode must be set to
  #VSS_ISTREAM_PACKET_EXCHANGE_MODE_OUT_OF_BAND, and the shared memory decoder
  buffer and size must be provided via 
  #VSS_ISTREAM_CMD_SET_OOB_PACKET_EXCHANGE_CONFIG. @newpage
*/
#define VSS_ISTREAM_EVT_OOB_NOTIFY_DEC_BUFFER_REQUEST ( 0x0001136E )

/** Deprecated. Use #VSS_IPKTEXG_EVT_OOB_NOTIFY_DEC_BUFFER_READY.

  Sent by the client to notify the stream that the decoder packet is
  available and ready to be consumed from shared memory.
 
  @apr_msgpayload
  None.  

  @par Expected decoder buffer format in shared memory
@code
<-------- 32 bits -------->

 -------------------------
| timestamp               |  ---> Set to zero if not used
 -------------------------
| media_type              |
 -------------------------
| dec_data_size           |
 -------------------------
| dec_data                |
|                         |
| ...                     |
 -------------------------

@endcode

  @detdesc
  An erasure is produced by the decoder if this event is not received from the
  client in response to the previous
  #VSS_ISTREAM_EVT_OOB_NOTIFY_DEC_BUFFER_REQUEST event by the time the decoder
  must begin decoding the packet.
 
  @return
  None.

  @dependencies
  The vocoder packet exchange mode must be set to
  #VSS_ISTREAM_PACKET_EXCHANGE_MODE_OUT_OF_BAND, and the shared memory decoder
  buffer and size must be provided via 
  #VSS_ISTREAM_CMD_SET_OOB_PACKET_EXCHANGE_CONFIG.
*/
#define VSS_ISTREAM_EVT_OOB_NOTIFY_DEC_BUFFER_READY ( 0x0001136F )

/** @} */ /* end_name Out-of-band vocoder packet exchange */


/****************************************************************************
 * VARIABLE SAMPLE RATE VOCODER APIS                                        *
 ****************************************************************************/

/** @ingroup cvd_cvs_cmd_set_vocoder_samp_rate
  Sets the sampling rates to be used for vocoders that support variable
  sampling rates.

  @apr_msgpayload{vss_istream_cmd_set_var_voc_sampling_rate_t}
  @wktable{weak__vss__istream__cmd__set__var__voc__sampling__rate__t}

  @detdesc
  For the vocoder that is currently selected on the stream:
  - If the vocoder does not support variable sample rates, this command has no
    effect.
  - If the vocoder supports variable sample rates and no sample rates are
    provided (i.e., this command is not called), the sample rates provided to
    the MVM via #VSS_IMVM_CMD_SET_CAL_VAR_VOC_SAMPLING_RATE are used.
  - If the vocoder supports variable sample rates and no sample rates are
    provided (i.e., this command is not called) and no sample rates are
    provided to the MVM (i.e., the vocoder seleted on MVM is not a variable
    sample rate or VSS_IMVM_CMD_SET_CAL_VAR_VOC_SAMPLING_RATE is not called),
    the defaults are used.
  - If the vocoder supports variable sample rates and no sample rates are
    provided (i.e., this command is not called) and the stream is not attached
    to the MVM, the default rates are used.
  @par 
  The following combinations of Tx/Rx sample rates are supported:
  - 8 kHz Tx / 8 kHz Rx
  - 8 kHz Tx / 16 kHz Rx
  - 16 kHz Tx / 16 kHz Rx
  @par
  The 48 kHz Tx/48 kHz Rx combination is supported for prototyping;
  end-to-end support is not available.
 
  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q2,[Q2]}).

  @dependencies
  None.
*/
#define VSS_ISTREAM_CMD_SET_VAR_VOC_SAMPLING_RATE ( 0x00011368 )

/* Type definition for vss_istream_cmd_set_var_voc_sampling_rate_t. */
typedef struct vss_istream_cmd_set_var_voc_sampling_rate_t vss_istream_cmd_set_var_voc_sampling_rate_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_istream_cmd_set_var_voc_sampling_rate_t 
@{ */
/* Payload of the #VSS_ISTREAM_CMD_SET_VAR_VOC_SAMPLING_RATE command. */
struct vss_istream_cmd_set_var_voc_sampling_rate_t
{
  uint32_t rx;
  /**< Rx sample rate.
    
       @values 8000, 16000, 48000 Hz */
  uint32_t tx;
  /**< Tx sampling rate.
    
       @values 8000, 16000, 48000 Hz */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weak_vss_istream_cmd_set_var_voc_sampling_rate_t */
/** @} */ /* end_addtogroup cvd_cvs_deprecated */


#endif /* __VSS_ISTREAM_DEPRECATED_PUBLIC_IF_H__ */


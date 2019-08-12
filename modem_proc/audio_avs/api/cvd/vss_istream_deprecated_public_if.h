#ifndef __VSS_ISTREAM_DEPRECATED_PUBLIC_IF_H__
#define __VSS_ISTREAM_DEPRECATED_PUBLIC_IF_H__

/**
  @file vss_istream_deprecated_public_if.h
  @brief This file contains the deprecated APR APIs for Core Voice Stream (CVS)
  module.
*/

/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear
      in the PDF.

      The CVD_mainpage.dox file contains all file/group descriptions
      that are in the output PDF generated using Doxygen and Latex. To edit or
      update any of the file/group text in the PDF, edit the
      CVD_mainpage.dox file or contact Tech Pubs.
=============================================================================*/
/*===========================================================================
Copyright (C) 2011-2014 QUALCOMM Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/audio_avs/api/cvd/vss_istream_deprecated_public_if.h#1 $
  $Author: mplcsds1 $

===========================================================================*/


#include "apr_comdef.h"
#include "vss_istream_public_if.h"

/** @addtogroup vss_istream_deprecated_public_if
@{ */

/****************************************************************************
 * CALIBRATION DATA APIS                                                    *
 ****************************************************************************/

/**
  Registers the memory that contains common calibration data table with the
  stream.

  As part of this registration, in addition to the address, size and handle
  of the memory containing the calibration table, the client also porvides
  the columns and associated column types which the stream uses to form a
  key that is used to index into the calibration table and look-up a matching
  entry. The columns/types must be provided in this command in the same order
  that they exist in the calibration table. If no matching entry is found,
  calibration data is not applied.

  Calibration data is applied when the stream transisions to the run state.

  @deprecated
  This command is deprecated. Use
  #VSS_ISTREAM_CMD_REGISTER_STATIC_CALIBRATION_DATA for sending static
  calibration data to the stream.

  @par Payload
  #vss_istream_cmd_register_calibration_data_v2_t

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  Any memory registered here must first be mapped with CVD by following
  @latexonly \hyperref[shmem_guideline]{Shared Memory Usage Guideline}
  @endlatexonly. 

  @comments
  The client is not allowed to modify the calibration data memory while it
  is registered and the stream does not modify this memory while registered.
  The calibration data at the registered memory is valid until unregistered. 
  Registering a memory region for calibration data while another region is
  still registered causes the command to fail.

  The supported column IDs that can be registered in this command are:

  - #VSS_ICOMMON_CAL_COLUMN_NETWORK
  - #VSS_ICOMMON_CAL_COLUMN_RX_SAMPLING_RATE
  - #VSS_ICOMMON_CAL_COLUMN_TX_SAMPLING_RATE
*/
#define VSS_ISTREAM_CMD_REGISTER_CALIBRATION_DATA_V2 ( 0x00011369 )

/** @brief Type definition for #vss_istream_cmd_register_calibration_data_v2_t.
*/
typedef struct vss_istream_cmd_register_calibration_data_v2_t vss_istream_cmd_register_calibration_data_v2_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the
    #VSS_ISTREAM_CMD_REGISTER_CALIBRATION_DATA_V2 command.

    Expected buffer format:

    @verbatim
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
     -------------------------            |                         |
    |  .                      |           | ...                     |
    |  .                      |            -------------------------
     -------------------------            | mod_id                  |
    | column_n                |           |                         |
     -------------------------            |                         |
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

    @endverbatim
*/
struct vss_istream_cmd_register_calibration_data_v2_t
{
  uint32_t cal_mem_handle;
    /**< Handle to the shared memory that holds the calibration data.
         See @latexonly \hyperref[shmem_guideline]{Shared Memory Usage
         Guideline} @endlatexonly. */
  uint64_t cal_mem_address;
    /**< Location of calibration data. See @latexonly \hyperref
         [shmem_guideline]{Shared Memory Usage Guideline} @endlatexonly on how
         the address is intrepreted. */
  uint32_t cal_mem_size;
    /**< Size of the calibration data in bytes. The amount of memory allocated
         at cal_mem_address must meet the requirement as specified in 
         @latexonly \hyperref[shmem_guideline]{Shared Memory Usage Guideline} 
         @endlatexonly. */
  uint32_t num_columns;
    /**< Number of columns used to form the key for looking up a calibration data
         entry. */
#if __STDC_VERSION__ >= 199901L
  vss_istream_cal_column_t columns[];
#endif /* __STDC_VERSION__ >= 199901L */
    /**< Array of columns of size num_columns. The order in which the colums are
         provided here must match the order in which they exist in the calibration
         table provided. */
}
#include "apr_pack_end.h"
;


/**
  Deregisters the memory that contains common calibration data from the stream.

  @deprecated
  This command is deprecated. Use
  #VSS_ISTREAM_CMD_DEREGISTER_STATIC_CALIBRATION_DATA to deregister static
  calibration data from the stream.

  @par Payload
  None.

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.
*/
#define VSS_ISTREAM_CMD_DEREGISTER_CALIBRATION_DATA ( 0x0001127A )


/****************************************************************************
 * IN-CALL CONVERSATION RECORDING APIS                                      *
 ****************************************************************************/

/**
  Starts the in-call conversation recording.

  This command forwards the in-call conversation stream to Rx Analog Front
  End (AFE) port 0x8003 and to Tx AFE port 0x8004.

  @deprecated
  This command is deprecated. Use #VSS_IRECORD_CMD_START.

  @par Payload
  #vss_istream_cmd_start_record_t

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  Recording is to be stopped before sending this command.

  @comments
  The audio path is used to record the voice streams. If recording silence
  before a call or during handovers is desired, AFE ports 0x8003 and 0x8004
  must be configured to live mode.
*/
#define VSS_ISTREAM_CMD_START_RECORD ( 0x00011236 )

/** Indicates no tapping for the specified path. */
#define VSS_TAP_POINT_NONE ( 0x00010F78 )

/** Indicates that the specified path is to be tapped at the end of the stream. */
#define VSS_TAP_POINT_STREAM_END ( 0x00010F79 )

/** @brief Type definition for #vss_istream_cmd_start_record_t.
*/
typedef struct vss_istream_cmd_start_record_t vss_istream_cmd_start_record_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_ISTREAM_CMD_START_RECORD command.
*/
struct vss_istream_cmd_start_record_t
{
   uint32_t rx_tap_point;
      /**< Tap point to use on the Rx path. Supported values:\n
           #VSS_TAP_POINT_NONE -- Do not record Rx path.\n
           #VSS_TAP_POINT_STREAM_END -- Rx tap point is at the end of the stream. */
   uint32_t tx_tap_point;
      /**< Tap point to use on the Tx path. Supported values:\n
           #VSS_TAP_POINT_NONE -- Do not record Tx path.\n
           #VSS_TAP_POINT_STREAM_END -- Tx tap point is at the end of the stream. */
   }
#include "apr_pack_end.h"
;


/**
  Stops the in-call conversation recording.

  This command stops forwarding the in-call conversation stream
  to Rx AFE port 0x8003 and to Tx AFE port 0x8004.

  @deprecated
  This command is deprecated. Use #VSS_IRECORD_CMD_STOP.

  @par Payload
  None.

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.
*/
#define VSS_ISTREAM_CMD_STOP_RECORD ( 0x00011237 )


/****************************************************************************
 * IN-CALL MUSIC DELIVERY APIS                                              *
 ****************************************************************************/

/**
  Starts the in-call music delivery on the Tx voice path.

  This command establishes a connection between the TX voice stream and TX AFE
  port 0x8005, allowing the voice stream to receive music from the audio path.

  @deprecated
  This command is deprecated. Use #VSS_IPLAYBACK_CMD_START.
  
  @par Payload
  None.

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.

  @comments
  The audio path is used to inject music to the Tx voice path. Configuring AFE
  port 0x8005 to non-live mode allows the voice modules to throttle transfer
  jitters.
*/
#define VSS_ISTREAM_CMD_START_PLAYBACK ( 0x00011238 )


/**
  Stops the in-call music delivery on the Tx voice path.

  This command causes the Tx voice stream to stop receiving music from Tx AFE
  port 0x8005.

  @deprecated
  This command is deprecated. Use #VSS_IPLAYBACK_CMD_STOP.

  @par Payload
  None.

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.
*/
#define VSS_ISTREAM_CMD_STOP_PLAYBACK ( 0x00011239 )


/****************************************************************************
 * VOCODER PACKET EXCHANGE MODE SELECTION APIS                              *
 ****************************************************************************/

/** In-band packet exchange mode. */
#define VSS_ISTREAM_PACKET_EXCHANGE_MODE_INBAND ( 0 )

/** Out-of-band packet exchange mode. */
#define VSS_ISTREAM_PACKET_EXCHANGE_MODE_OUT_OF_BAND ( 1 )

/**
  Sets the vocoder packet exchang mode.

  @deprecated
  This command is deprecated. Use #VSS_IPKTEXG_CMD_SET_MODE.
 
  @par Payload
  #vss_istream_cmd_set_packet_exchange_mode_t
 
  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.

  @comments
  Properties of vocoder packet exchange mode selection:
  1. The default mode is VSS_ISTREAM_PACKET_EXCHANGE_MODE_IN_BAND.  
  2. The mode can be changed only when the stream is in disabled state.
  3. If the mode being selected requires any configuration parameters, they must
     be provided via the correspoding configuration command (while the stream is
     in disabled state). If the configuration information is not provided packet
     exchanges do not occur.
*/
#define VSS_ISTREAM_CMD_SET_PACKET_EXCHANGE_MODE ( 0x0001136A )

/** @brief Type definition for #vss_istream_cmd_set_packet_exchange_mode_t.
*/
typedef struct vss_istream_cmd_set_packet_exchange_mode_t vss_istream_cmd_set_packet_exchange_mode_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_ISTREAM_CMD_SET_PACKET_EXCHANGE_MODE
    command.
*/
struct vss_istream_cmd_set_packet_exchange_mode_t
{
   uint32_t mode;
      /**< Vocoder packet exchange mode. Supported values:
           #VSS_ISTREAM_PACKET_EXCHANGE_MODE_IN_BAND \n
           #VSS_ISTREAM_PACKET_EXCHANGE_MODE_OUT_OF_BAND. */
}
#include "apr_pack_end.h"
;

/****************************************************************************
 * IN-BAND VOCODER PACKET EXCHANGE APIS                                     *
 ****************************************************************************/

/**
  Event that provides an encoded packet when sent by the stream to its client.

  @deprecated
  This event is deprecated. Use #VSS_IPKTEXG_EVT_IN_BAND_SEND_ENC_BUFFER.

  @par Payload
  #vss_istream_evt_send_enc_buffer_t

  @return
  None.

  @dependencies
  None.
*/
#define VSS_ISTREAM_EVT_SEND_ENC_BUFFER ( 0x00011015 )

/** @brief Type definition for #vss_istream_evt_send_enc_buffer_t.
*/
typedef struct vss_istream_evt_send_enc_buffer_t vss_istream_evt_send_enc_buffer_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_ISTREAM_EVT_SEND_ENC_BUFFER event.
*/
struct vss_istream_evt_send_enc_buffer_t
{
   uint32_t media_id;
      /**< Media ID of the packet. */

#if __STDC_VERSION__ >= 199901L
   uint8_t content[];
      /**< Data content. */
#endif /* __STDC_VERSION__ >= 199901L */
}
#include "apr_pack_end.h"
;


/**
  Event that requests a decoder packet when sent by the stream to its client.
  The client responds with a VSS_ISTREAM_EVT_SEND_DEC_BUFFER event.

  @deprecated
  This event is deprecated. Use #VSS_IPKTEXG_EVT_IN_BAND_REQUEST_DEC_BUFFER.

  @par Payload
  None.

  @return
  None.

  @dependencies
  None.
*/
#define VSS_ISTREAM_EVT_REQUEST_DEC_BUFFER ( 0x00011017 )


/**
  Event sent by the client to the stream in response to a
  VSS_ISTREAM_EVT_REQUEST_DEC_BUFFER event that provides a decoder packet.

  @deprecated
  This event is deprecated. Use #VSS_IPKTEXG_EVT_IN_BAND_SEND_DEC_BUFFER.

  @par Payload
  #vss_istream_evt_send_dec_buffer_t

  @return
  None.

  @dependencies
  None.
*/
#define VSS_ISTREAM_EVT_SEND_DEC_BUFFER ( 0x00011016 )

/** @brief Type definition for #vss_istream_evt_send_dec_buffer_t.
*/
typedef struct vss_istream_evt_send_dec_buffer_t vss_istream_evt_send_dec_buffer_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_ISTREAM_EVT_SEND_DEC_BUFFER event.
*/
struct vss_istream_evt_send_dec_buffer_t
{
   uint32_t media_id;
      /**< Media ID of the packet. */

#if __STDC_VERSION__ >= 199901L
   uint8_t content[];
      /**< Data content. */
#endif /* __STDC_VERSION__ >= 199901L */
}
#include "apr_pack_end.h"
;

/****************************************************************************
 * OUT-OF-BAND VOCODER PACKET EXCHANGE APIS                                 *
 ****************************************************************************/

/**
  Sets the configuration information for 
  #VSS_ISTREAM_PACKET_EXCHANGE_MODE_OUT_OF_BAND.
 
  @deprecated
  This command is deprecated. Use #VSS_IPKTEXG_CMD_OOB_SET_CONFIG.

  @par Payload
  #vss_istream_cmd_set_oob_packet_exchange_config_t
 
  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  The vocoder packet exchange mode must be successfully set on the stream to
  VSS_ISTREAM_PACKET_EXCHANGE_MODE_OUT_OF_BAND via
  #VSS_ISTREAM_CMD_SET_PACKET_EXCHANGE_MODE and the memory for the encoder and
  decoder buffers must first be mapped with CVD by following @latexonly
  \hyperref[shmem_guideline]{Shared Memory Usage Guideline} @endlatexonly. 
  
  @comments
  Properties of out-of-band vocoder packet exchange:
  1. The encoder and decoder buffers provided must be large enough to accommodate
     the maximum possible vocoder packet size.
  2. The encoder and decoder buffers can be re-claimed by the client only after
     the vocoder packet exchange mode has been successfully set on the stream to
     something other than #VSS_ISTREAM_PACKET_EXCHANGE_MODE_OUT_OF_BAND.  
  3. If the current vocoder packet exchange mode set on the stream is not
     #VSS_ISTREAM_PACKET_EXCHANGE_MODE_OUT_OF_BAND, the configuration information
     is ignored and a failure is returned.
*/
#define VSS_ISTREAM_CMD_SET_OOB_PACKET_EXCHANGE_CONFIG ( 0x0001136B )

/** @brief Type definition for #vss_istream_cmd_set_packet_exchange_mode_t.
*/
typedef struct vss_istream_cmd_set_oob_packet_exchange_config_t vss_istream_cmd_set_oob_packet_exchange_config_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_ISTREAM_CMD_SET_OOB_PACKET_EXCHANGE_CONFIG
    command.
*/
struct vss_istream_cmd_set_oob_packet_exchange_config_t
{
  uint32_t mem_handle;
    /**< Handle to the shared memory that holds encoder and decoder buffers.
         See @latexonly \hyperref[shmem_guideline]{Shared Memory Usage Guideline}
         @endlatexonly. */
  uint64_t enc_buf_addr;
    /**< Location of memory that holds the encoder buffer. See @latexonly \hyperref
         [shmem_guideline]{Shared Memory Usage Guideline} @endlatexonly on how
         the address is intrepreted. */
  uint32_t enc_buf_size;
    /**< Size of the encoder buffer. The amount of memory allocated
         at enc_buf_addr must meet the requirement as specified in @latexonly
         \hyperref[shmem_guideline]{Shared Memory Usage Guideline} 
         @endlatexonly. */
  uint64_t dec_buf_addr;
    /**< Location of memory that holds the decoder buffer. See @latexonly \hyperref
         [shmem_guideline]{Shared Memory Usage Guideline} @endlatexonly on how
         the address is intrepreted. */
  uint32_t dec_buf_size;
    /**< Size of the decoder buffer. The amount of memory allocated
         at dec_buf_addr must meet the requirement as specified in @latexonly
         \hyperref[shmem_guideline]{Shared Memory Usage Guideline} 
         @endlatexonly. */
}
#include "apr_pack_end.h"
;


/**
  Event sent by the stream to its client notifying that an encoder packet is
  ready to be consumed by the client from shared memory.

  Encoder buffer format in shared memory:

  @verbatim

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

  @endverbatim

  @deprecated
  This event is deprecated. Use #VSS_IPKTEXG_EVT_OOB_NOTIFY_ENC_BUFFER_READY.

  @payload
  None.  
 
  @return
  None.

  @dependencies
  The vocoder packet exchange mode must be set to
  #VSS_ISTREAM_PACKET_EXCHANGE_MODE_OUT_OF_BAND and the shared memory encoder buffer
  and size must be provided via #VSS_ISTREAM_CMD_SET_OOB_PACKET_EXCHANGE_CONFIG.

  @comments
  With the exception of the first encoder packet, if
  a #VSS_ISTREAM_EVT_OOB_NOTIFY_ENC_BUFFER_CONSUMED event is not received from the
  client in response to the previous #VSS_ISTREAM_EVT_OOB_NOTIFY_ENC_BUFFER_READY
  event by the time a new encoder packet is ready for delivery, the stream will drop
  the new encoder packet.
*/
#define VSS_ISTREAM_EVT_OOB_NOTIFY_ENC_BUFFER_READY ( 0x0001136C )


/**
  Event sent by the client to the stream notifying that the encoder packet has been
  consumed from shared memory and the encoder buffer is available to the stream for
  the next encoder packet.

  @deprecated
  This event is deprecated. Use #VSS_IPKTEXG_EVT_OOB_NOTIFY_ENC_BUFFER_CONSUMED.
 
  @payload
  None.  

  @return
  None.

  @dependencies
  The vocoder packet exchange mode must be set to
  #VSS_ISTREAM_PACKET_EXCHANGE_MODE_OUT_OF_BAND and the shared memory encoder buffer
  and size must be provided via #VSS_ISTREAM_CMD_SET_OOB_PACKET_EXCHANGE_CONFIG.

  @comments
  With the exception of the first encoder packet, if this event is not received from
  the client in response to the previous #VSS_ISTREAM_EVT_OOB_NOTIFY_ENC_BUFFER_READY
  event by the time a new encoder packet is ready for delivery, the stream will drop
  the new encoder packet.
*/
#define VSS_ISTREAM_EVT_OOB_NOTIFY_ENC_BUFFER_CONSUMED ( 0x0001136D )


/**
  Event sent by the stream to its client notifying that the stream is requesting
  the next decoder packet in shared memory.
  
  @deprecated
  This event is deprecated. Use #VSS_IPKTEXG_EVT_OOB_NOTIFY_DEC_BUFFER_REQUEST.

  @payload
  None.   

  @return
  None.

  @dependencies
  The vocoder packet exchange mode must be set to
  #VSS_ISTREAM_PACKET_EXCHANGE_MODE_OUT_OF_BAND and the shared memory decoder buffer
  and size must be provided via #VSS_ISTREAM_CMD_SET_OOB_PACKET_EXCHANGE_CONFIG.

  @comments
  If a #VSS_ISTREAM_EVT_OOB_NOTIFY_DEC_BUFFER_READY event is not received from the
  client in response to this event by the time the decoder must begin decoding
  the packet, then an erasure is produced by the decoder.  
*/
#define VSS_ISTREAM_EVT_OOB_NOTIFY_DEC_BUFFER_REQUEST ( 0x0001136E )


/**
  Event sent by the client to the stream, notifying that the decoder packet is
  available and ready to be consumed from shared memory.
 
  Expected decoder buffer format in shared memory:

  @verbatim

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

  @endverbatim

  @deprecated
  This event is deprecated. Use #VSS_IPKTEXG_EVT_OOB_NOTIFY_DEC_BUFFER_READY.

  @payload
  None.  
 
  @return
  None.

  @dependencies
  The vocoder packet exchange mode must be set to
  #VSS_ISTREAM_PACKET_EXCHANGE_MODE_OUT_OF_BAND and the shared memory decoder buffer
  and size must be provided via #VSS_ISTREAM_CMD_SET_OOB_PACKET_EXCHANGE_CONFIG.

  @comments
  If this event is not received from the client in response to the previous
  #VSS_ISTREAM_EVT_OOB_NOTIFY_DEC_BUFFER_REQUEST event by the time the decoder must
  begin decoding the packet, then an erasure is produced by the decoder.
*/
#define VSS_ISTREAM_EVT_OOB_NOTIFY_DEC_BUFFER_READY ( 0x0001136F )


/** @} */ /* end_addtogroup vss_istream_deprecated_public_if */

#endif /* __VSS_ISTREAM_DEPRECATED_PUBLIC_IF_H__ */


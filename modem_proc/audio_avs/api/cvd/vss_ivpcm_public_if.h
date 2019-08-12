#ifndef __VSS_IVPCM_PUBLIC_IF_H__
#define __VSS_IVPCM_PUBLIC_IF_H__

/**
  @file vss_ivpcm_public_if.h
  @brief This file contains the APR API definitions for the vococder PCM
  interface.
*/

/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear
      in the PDF.

      The CVD_mainpage.dox file contains all file/group descriptions
      that are in the output PDF generated using Doxygen and Latex. To edit or
      update any of the file/group text in the PDF, edit the
      CVD_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the vss_ivpcm_public_if
      group description in the CVD_mainpage.dox file.
=============================================================================*/
/*===========================================================================
Copyright (C) 2011, 2012 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/audio_avs/api/cvd/vss_ivpcm_public_if.h#1 $
  $Author: mplcsds1 $

when        who    what, where, why
-------     ---    ----------------------------------------------------------
04/18/12     HL    Deleted VSS_IVPCM_CMD_START, VSS_IVPCM_EVT_PUSH_BUFFER and
                   VSS_IVPCM_EVT_NOTIFY. Introduced 
                   VSS_IVPCM_CMD_START_V2, VSS_IVPCM_EVT_PUSH_BUFFER_V2 and
                   VSS_IVPCM_EVT_NOTIFY_V2.
04/18/11   AT/SB   Initial version.

===========================================================================*/

#include "apr_comdef.h"

/** @addtogroup vss_ivpcm_public_if
@{ */

/****************************************************************************
 * VOCODER PCM APIS                                                         *
 ****************************************************************************/

/**
  Starts a vocoder PCM session.

  @par Payload
  #vss_ivpcm_cmd_start_v2_t

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  Shared memory that will be used for vocoder PCM data buffer transfers must 
  first be mapped with CVD by following @latexonly \hyperref[shmem_guideline]
  {Shared Memory Usage Guideline} @endlatexonly. The memory must be valid until
  #VSS_IVPCM_CMD_STOP is successfully completed. If destroying the a CVD module
  session while vocoder PCM is running, the memory must be valid until 
  #APRV2_IBASIC_CMD_DESTROY_SESSION is successfully completed.

  @comments
  This command is used to start a vocoder PCM session by specifying one or more
  PCM tap points and a handle to the shared memory that will be used for 
  vocoder PCM data buffer transfers. Only one vocoder PCM start is supported by
  the CVD module session at any given time. During device switch, client has to
  stop and re-configure vocoder PCM. Otherwise, for any configuration changes 
  during device switch, client has to check the new configuration information 
  from #VSS_IVPCM_EVT_NOTIFY_V2 event.
*/
#define VSS_IVPCM_CMD_START_V2 ( 0x00011339 )

/** Default tap point location on the TX path. */
#define VSS_IVPCM_TAP_POINT_TX_DEFAULT ( 0x00011289 )

/** Default tap point location on the RX path. */
#define VSS_IVPCM_TAP_POINT_RX_DEFAULT ( 0x0001128A )

/** Indicates tap point direction is output. */
#define VSS_IVPCM_TAP_POINT_DIR_OUT ( 0 )

/** Indicates tap point direction is input. */
#define VSS_IVPCM_TAP_POINT_DIR_IN ( 1 )

/** Indicates tap point direction is output and input. */
#define VSS_IVPCM_TAP_POINT_DIR_OUT_IN ( 2 )

/** 
  Indicates vocoder PCM sampling rate matches the operating sampling rate of
  the post-proc chain on the VDSP at the location of the tap point.
*/
#define VSS_IVPCM_SAMPLING_RATE_AUTO ( 0 )

/** Indicates 8 KHz vocoder PCM sampling rate. */
#define VSS_IVPCM_SAMPLING_RATE_8K ( 8000 )

/** Indicates 16 KHz vocoder PCM sampling rate. */
#define VSS_IVPCM_SAMPLING_RATE_16K ( 16000 )

/** @brief Type definition for #vss_ivpcm_tap_point_t.
*/
typedef struct vss_ivpcm_tap_point_t vss_ivpcm_tap_point_t;

#include "apr_pack_begin.h"

/** @brief Structure for specifying tap point characteristics.
*/
struct vss_ivpcm_tap_point_t
{
  uint32_t tap_point;
    /**< Tap point location GUID. Supported values:\n
         #VSS_IVPCM_TAP_POINT_TX_DEFAULT \n
         #VSS_IVPCM_TAP_POINT_RX_DEFAULT */
  uint16_t direction;
    /**< Data flow direction of the tap point. Supported values:\n
         #VSS_IVPCM_TAP_POINT_DIR_OUT \n
         #VSS_IVPCM_TAP_POINT_DIR_IN \n
         #VSS_IVPCM_TAP_POINT_DIR_OUT_IN */
  uint16_t sampling_rate;
    /**< Sampling rate of the tap point. If the tap point is output-input, then
         output sampling rate and the input sampling rate are the same.
         Supported values:\n
         #VSS_IVPCM_SAMPLING_RATE_AUTO \n
         #VSS_IVPCM_SAMPLING_RATE_8K \n
         #VSS_IVPCM_SAMPLING_RATE_16K */
  uint16_t duration;
    /**< Duration of buffer in milliseconds. Unsupported, set to 0. */
}
#include "apr_pack_end.h"
;

/** @brief Type definition for #vss_ivpcm_cmd_start_v2_t.
*/
typedef struct vss_ivpcm_cmd_start_v2_t vss_ivpcm_cmd_start_v2_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_IVPCM_CMD_START_V2 command.
*/
struct vss_ivpcm_cmd_start_v2_t
{
  uint32_t mem_handle;
    /**< Handle to the shared memory that will be used for vocoder PCM data 
         buffer transfers. See @latexonly \hyperref[shmem_guideline]{Shared 
         Memory Usage Guideline} @endlatexonly. */
  uint32_t num_tap_points;
    /**< Number of tap point being specified in this vocoder PCM start
         command. */
#if __STDC_VERSION__ >= 199901L
  vss_ivpcm_tap_point_t tap_points[];
#endif /* __STDC_VERSION__ >= 199901L */
    /**< Array of tap point speciffiers of size num_tap_points. */
}
#include "apr_pack_end.h"
;


/**
  Provides a vocoder PCM input buffer or output buffer or both to CVD.

  The client specifies whethter it is providing an output buffer, or
  an input buffer, or both input and output buffers using the push_buf_mask
  bitmask in the payload.

  @par Payload
  #vss_ivpcm_evt_push_buffer_v2_t

  @return
  None.

  @dependencies
  The output buffer and input buffer must resides in the shared memory pointed
  to by the memory handle that was provided in the #VSS_IVPCM_CMD_START_V2. The
  memory used for output buffer and input buffer must not overlap.
  Buffer properties:\n
  - Buffer size is in bytes.\n
  - In case of multi-channel input/output, the buffer is non interleaved and
    partitioned to hold data from each channel.\n
  - All buffers are instantaneous and are consumed real time. Only one buffer 
    will be consumed in a timetick. All additional buffers in the same timetick
    will be dropped and notified back with corresponding error bit set.\n
  - Multiple buffers pushed will be consumed in subsequent timeticks.\n
  - Buffer format must always match the expected format. Excess data in the 
    buffer will be discarded. Partial data buffers will be dropped.
*/
#define VSS_IVPCM_EVT_PUSH_BUFFER_V2 ( 0x0001133A )

/** Push buffer event mask indicating output buffer is filled. */
#define VSS_IVPCM_PUSH_BUFFER_MASK_OUTPUT_BUFFER ( 1 )

/** Push buffer event mask indicating input buffer is consumed. */
#define VSS_IVPCM_PUSH_BUFFER_MASK_INPUT_BUFFER ( 2 )

/** @brief Type definition for #vss_ivpcm_evt_push_buffer_v2_t.
*/
typedef struct vss_ivpcm_evt_push_buffer_v2_t vss_ivpcm_evt_push_buffer_v2_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_IVPCM_EVT_PUSH_BUFFER_V2 command.
*/
struct vss_ivpcm_evt_push_buffer_v2_t
{
  uint32_t tap_point;
    /**< GUID identifying tap point for which the buffer(s) is(are) provided.  
         Supported values:\n
         #VSS_IVPCM_TAP_POINT_TX_DEFAULT \n
         #VSS_IVPCM_TAP_POINT_RX_DEFAULT */
  uint32_t push_buf_mask;
    /**< Bitmask inticating whether an output buffer is being provided or an
         input buffer or both. Supported values:\n
         bit 0 - Output buffer, use 
                 #VSS_IVPCM_PUSH_BUFFER_MASK_OUTPUT_BUFFER \n
         bit 1 - Input buffer, use #VSS_IVPCM_PUSH_BUFFER_MASK_INPUT_BUFFER. */
  uint64_t out_buf_mem_address;
    /**< If bit 0 of the push_buf_mask is set, this field indicates the 
         location of the output buffer. Otherwise it is ignored. See 
         @latexonly \hyperref[shmem_guideline]{Shared Memory Usage Guideline}
         @endlatexonly on how the address is intrepreted. */
  uint16_t out_buf_mem_size;
    /**< If bit 0 of the push_buf_mask is set, this field indicates the size of
         the buffer at out_buf_mem_address. Otherwise it is ignored. The client
         must allocate the output buffer to accommodate the maximum expected
         sampling rate and also meet the size requirement as specified in 
         @latexonly \hyperref[shmem_guideline]{Shared Memory Usage Guideline}
         @endlatexonly. */
  uint64_t in_buf_mem_address;
    /**< If bit 1 of the push_buf_mask is set, this field indicates the 
         location of the input buffer. Otherwise it is ingored. See 
         @latexonly \hyperref[shmem_guideline]{Shared Memory Usage Guideline}
         @endlatexonly on how the address is intrepreted. */
  uint16_t in_buf_mem_size;
    /**< If bit 1 of the push_buf_mask is set, this field indicates the size of
         the input buffer at in_buf_mem_address. Otherwise it is ignored. The 
         size of memory allocated at in_buf_mem_address must meet the 
         requirement as specified in @latexonly \hyperref[shmem_guideline]
         {Shared Memory Usage Guideline} @endlatexonly. */
  uint16_t sampling_rate;
    /**< If bit 1 of the push_buf_mask is set, this field indicates the 
         sampling rate of the input buffer. Otherwise it is ignored. Supported 
         values:\n
         #VSS_IVPCM_SAMPLING_RATE_8K \n
         #VSS_IVPCM_SAMPLING_RATE_16K */
  uint16_t num_in_channels;
    /**< If bit 1 of the push_buf_mask is set, this field indicates the number of
         channels contained in the input buffer. Otherwise it is ignored.
         Supported values:\n
         1 - One channel. */
}
#include "apr_pack_end.h"
;


/**
  Per tap-point vocoder PCM notification issued by CVD, providing a buffer
  transfer notification to the client. Additionally, this notification is
  overloaded to provide timetick and various information to the client, 
  depending on the use case, such as: requested output buffer size, requested
  input buffer size and sampling rate, input buffer consumed notification, 
  output buffer filled notification. The interpretation of this notification is
  indicated by the notify_mask in the payload.

  If the timetick bit of notify_mask is set, this notification is used as a 
  timetick which is issued to the client every 'x' milliseconds, where 'x' is
  the PCM frame size which the VDSP operates on, and denotes the boundary upon
  which PCM buffer transfers between the client and CVD occur. If the timetick
  bit is not set, notify event is used for buffer notifications only. In case 
  of output or input error in buffer operation, the correponding error bit will
  be set in the notify_mask.

  Each notify event can accommodate any combination of timetick, output buffer 
  filled or input buffer consumed masks.

  @par Payload
  #vss_ivpcm_evt_notify_v2_t

  @return
  None.

  @dependencies
  None.

  @comments
  After a VSS_IVPCM_CMD_START_V2 command is issued, buffer transfer between the
  client and CVD commences when the first #VSS_IVPCM_EVT_NOTIFY_V2 event is 
  received by the client (it cannot start prior to receiving the first event).
  First notification is for buffer setup information only. The interpretation 
  of the #VSS_IVPCM_EVT_NOTIFY_V2 event and the information conveyed by it to
  the client depends on the use case and the time when it is issued:

  USE CASE 1 -- Output only tap point:

    - First #VSS_IVPCM_EVT_NOTIFY_V2 event after #VSS_IVPCM_CMD_START_V2
        - notify_mask = 0
        - Indicates the requested size of the output buffer to be provided by 
          the client.
    - Subsequent #VSS_IVPCM_EVT_NOTIFY_V2 events
        - notify_mask bit 0 set
        - Indicates the previously provided output buffer has been filled and 
          its sampling rate and number of channels are as speciffied by the 
          sampling_rate and num_out_channels fields. Also indicates the size of
          the next output buffer to be provided by the client.

  USE CASE 2 -- Input only tap point:

    - First #VSS_IVPCM_EVT_NOTIFY_V2 event after #VSS_IVPCM_CMD_START_V2
        - notify_mask = 0
        - Indicates the requested size and sampling rate of the input buffer to
          be provided by the client.
    - Subsequent #VSS_IVPCM_EVT_NOTIFY_V2 events
        - notify_mask bit 1 set
        - Indicates the previously provided input buffer has been consumed.
          Also indicates the size and sampling rate of the next input buffer to
          be provided by the client.

  USE CASE 3 -- Output-input tap point:

    Sampling rate of the input and output must always be the same in this case.

    - First #VSS_IVPCM_EVT_NOTIFY_V2 event after #VSS_IVPCM_CMD_START_V2
        - notify_mask = 0
        - Indicates the requested size of the output buffer to be provided by 
          the client.
    - Second #VSS_IVPCM_EVT_NOTIFY_V2 event after #VSS_IVPCM_CMD_START_V2
        - notify_mask bit 0 set
        - Indicates that the first provided output buffer has been filled and 
          its sampling rate and number of channels are as speciffied by the 
          sampling_rate and num_out_channels fields. Also indicates the size
          and sampling rate of the first input buffer to be provided by the 
          client.
    - Subsequent #VSS_IVPCM_EVT_NOTIFY_V2 events
        - notify_mask bits 0 and 1 both set
        - Indicates the previously provided output buffer has been filled and 
          its sampling rate and number of channels are as specified by the 
          sampling_rate and num_out_channels fields.
          Also indicates the previously provided input buffer has been 
          consumed. Also indicates the requested size of the next output buffer
          and the requested size and sampling rate of the next input buffer.
*/
#define VSS_IVPCM_EVT_NOTIFY_V2 ( 0x0001133B )

/** Notify event mask indicates output buffer is filled. */
#define VSS_IVPCM_NOTIFY_MASK_OUTPUT_BUFFER ( 1 )

/** Notify event mask indicates input buffer is consumed. */
#define VSS_IVPCM_NOTIFY_MASK_INPUT_BUFFER ( 2 )

/** Notify event mask indicates a timetick. */
#define VSS_IVPCM_NOTIFY_MASK_TIMETICK ( 4 )

/** Notify event mask indicates an error occured in output buffer operation. */
#define VSS_IVPCM_NOTIFY_MASK_OUTPUT_ERROR ( 8 )

/** Notify event mask indicates an error occured in input buffer operation. */
#define VSS_IVPCM_NOTIFY_MASK_INPUT_ERROR ( 16 )

/** @brief Type definition for #vss_ivpcm_evt_notify_v2_t.
*/
typedef struct vss_ivpcm_evt_notify_v2_t vss_ivpcm_evt_notify_v2_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_IVPCM_EVT_NOTIFY_V2 command.
*/
struct vss_ivpcm_evt_notify_v2_t
{
  uint32_t tap_point;
    /**< GUID indicating tap point location for which this notification is
         being issued. */
  uint32_t notify_mask;
    /**< Bitmask indicating the notification mode. Supported values:\n
         bit 0 - Output buffer filled, use 
                 #VSS_IVPCM_NOTIFY_MASK_OUTPUT_BUFFER.\n
         bit 1 - Input buffer consumed, use 
                 #VSS_IVPCM_NOTIFY_MASK_INPUT_BUFFER.\n
         bit 2 - Event is a timetick, use 
                 #VSS_IVPCM_NOTIFY_MASK_TIMETICK.\n
         bit 3 - Error in output buffer operation, use 
                 #VSS_IVPCM_NOTIFY_MASK_OUTPUT_ERROR. This bit will be set if 
                 there is an error in output buffer operation. This bit will be 
                 set along with "output buffer filled" bit to return the output 
                 buffer with error indication.\n
         bit 4 - Error in input buffer operation, use 
                 #VSS_IVPCM_NOTIFY_MASK_INPUT_ERROR. This bit will be set if 
                 there is an error in input buffer operation. This bit will be 
                 set along with "input buffer consumed" bit to return the input
                 buffer with error indication. */
  uint64_t out_buf_mem_address;
    /**< If bit 0 of the notify_mask is set, this field indicates the location
         of the output buffer. Otherwise ignore. See @latexonly \hyperref
         [shmem_guideline]{Shared Memory Usage Guideline} @endlatexonly on how
         the address is intrepreted. */
  uint64_t in_buf_mem_address;
    /**< If bit 1 of the notify_mask is set, this field indicates the location
         of the input buffer. Otherwise ignore. See @latexonly \hyperref
         [shmem_guideline]{Shared Memory Usage Guideline} @endlatexonly on how
         the address is intrepreted. */
  uint16_t filled_out_size;
    /**< If bit 0 of the notify_mask is set, this field indicates the filled 
         size of the output buffer located at the out_buf_mem_address.
         Otherwise ignore. */
  uint16_t request_buf_size;
    /**< Request size of the input buffer. */
  uint16_t sampling_rate;
    /**< Sampling rate of the input/output buffer. Supported values:\n
         #VSS_IVPCM_SAMPLING_RATE_8K \n
         #VSS_IVPCM_SAMPLING_RATE_16K */
  uint16_t num_out_channels;
    /**< Number of output channels contained in the filled output buffer. */
}
#include "apr_pack_end.h"
;


/**
  Stops an active vocoder PCM seesion.

  @par Payload
  None.

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.

  @comments
  Any buffer still in the ownership of the CVD module at the time this command
  is issued can be reclaimed by the client upon receiving a response to this
  command. If destroying the CVD module session while vocoder PCM is running,
  any buffers still in the ownership of the CVD module can be reclaimed by the
  client upon receiving a response to the #APRV2_IBASIC_CMD_DESTROY_SESSION
  command.
*/
#define VSS_IVPCM_CMD_STOP ( 0x0001100B )

/** @} */ /* end_addtogroup vss_ivpcm_public_if */

#endif /*__VSS_IVPCM_PUBLIC_IF_H__ */


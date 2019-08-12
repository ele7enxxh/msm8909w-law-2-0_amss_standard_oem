#ifndef __VSS_ISTREAM_PUBLIC_IF_H__
#define __VSS_ISTREAM_PUBLIC_IF_H__

/**
  @file vss_istream_public_if.h
  @brief This file contains the APR API definitions for Core Voice Stream (CVS)
  module.
*/

/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear
      in the PDF.

      The CVD_mainpage.dox file contains all file/group descriptions
      that are in the output PDF generated using Doxygen and Latex. To edit or
      update any of the file/group text in the PDF, edit the
      CVD_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the vss_istream_public_if
      group description in the CVD_mainpage.dox file.
=============================================================================*/
/*===========================================================================
Copyright (C) 2011-2014 QUALCOMM Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/audio_avs/api/cvd/vss_istream_public_if.h#1 $
  $Author: mplcsds1 $

===========================================================================*/


#include "apr_comdef.h"

/** @addtogroup vss_istream_public_if
@{ */

/****************************************************************************
 * CORE STREAM APIS                                                         *
 ****************************************************************************/

/**
  Creates a new passive control stream session.

  Passive stream sessions enable clients to manage UI controls
  such as muting and volume levels while the corresponding full control
  sessions manage data exchange.

  @par Payload
  The session name string used to identify the full control stream session for
  which a handle is to be obtained. The string size, including the NULL
  termination character, is limited to 31 characters.

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.

  @comments
  Use the session name <i>default modem voice</i> to obtain a handle to the
  default stream session for circuit-switched voice calls.
*/
#define VSS_ISTREAM_CMD_CREATE_PASSIVE_CONTROL_SESSION ( 0x00011140 )

/**
  Creates a new full control stream session.

  Full stream sessions give clients full control over the stream, including
  data exchange, in addition to UI controls.

  @par Payload
  #vss_istream_cmd_create_full_control_session_t

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  The session name <i>default modem voice</i> is reserved for the default
  stream session for circuit-switched voice calls and cannot be given to
  any new full control sessions. Two or more full control stream sessions cannot
  be created with the same name.
*/
#define VSS_ISTREAM_CMD_CREATE_FULL_CONTROL_SESSION ( 0x000110F7 )

/** @brief Type definition for #vss_istream_cmd_create_full_control_session_t.
*/
typedef struct vss_istream_cmd_create_full_control_session_t vss_istream_cmd_create_full_control_session_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the
    #VSS_ISTREAM_CMD_CREATE_FULL_CONTROL_SESSION command.
*/
struct vss_istream_cmd_create_full_control_session_t
{
   uint16_t direction;
     /**< Stream direction. Possible values:\n
          - 0 -- Tx only \n
          - 1 -- Rx only \n
          - 2 -- Tx and Rx \n
          - 3 -- Tx and Rx loopback */
   uint32_t enc_media_type;
     /**< Tx vocoder type (see @latexonly
          \hyperref[media_ID]{Media Type IDs}@endlatexonly). */
   uint32_t dec_media_type;
     /**< Rx vocoder type (see @latexonly
          \hyperref[media_ID]{Media Type IDs}@endlatexonly). */
   uint32_t network_id;
     /**< Network ID (see @latexonly
          \hyperref[cal_network_ID]{Calibration Network IDs}@endlatexonly). */
#if __STDC_VERSION__ >= 199901L
   char_t name[];
     /**< Variable-sized stream name (optional).

          The stream name size is the payload size minus the size of the other
          fields. The string size, including the NULL termination character, is
          limited to 31 characters. The string must be NULL terminated if
          supplied. A size 0 name is interpreted as a nameless session where
          only the handle is used to access to the session. */

#endif /* __STDC_VERSION__ >= 199901L */
}
#include "apr_pack_end.h"
;


/**
  Sets the media type on the stream (see @latexonly
  \hyperref[media_ID]{Media Type IDs}@endlatexonly).

  @par Payload
  #vss_istream_cmd_set_media_type_t

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.
*/
#define VSS_ISTREAM_CMD_SET_MEDIA_TYPE ( 0x00011186 )

/** @brief Type definition for #vss_istream_cmd_set_media_type_t.
*/
typedef struct vss_istream_cmd_set_media_type_t vss_istream_cmd_set_media_type_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_ISTREAM_CMD_SET_MEDIA_TYPE command.
*/
struct vss_istream_cmd_set_media_type_t
{
   uint32_t rx_media_id;
      /**< Sets the Rx vocoder type (see @latexonly
          \hyperref[media_ID]{Media Type IDs}@endlatexonly). */
   uint32_t tx_media_id;
      /**< Sets the Tx vocoder type (see @latexonly
           \hyperref[media_ID]{Media Type IDs}@endlatexonly). */
}
#include "apr_pack_end.h"
;


/**
  Attaches a vocproc to the stream.

  @par Payload
  #vss_istream_cmd_attach_vocproc_t

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.
*/
#define VSS_ISTREAM_CMD_ATTACH_VOCPROC ( 0x000110F8 )

/** @brief Type definition for #vss_istream_cmd_attach_vocproc_t.
*/
typedef struct vss_istream_cmd_attach_vocproc_t vss_istream_cmd_attach_vocproc_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_ISTREAM_CMD_ATTACH_VOCPROC command.
*/
struct vss_istream_cmd_attach_vocproc_t
{
  uint16_t handle;
    /**< Vocproc handle to attach to the stream. */
}
#include "apr_pack_end.h"
;


/**
  Detaches a vocproc from the stream.

  @par Payload
  #vss_istream_cmd_detach_vocproc_t

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.
*/
#define VSS_ISTREAM_CMD_DETACH_VOCPROC ( 0x000110F9 )

/** @brief Type definition for #vss_istream_cmd_detach_vocproc_t.
*/
typedef struct vss_istream_cmd_detach_vocproc_t vss_istream_cmd_detach_vocproc_t;

#include "apr_pack_begin.h"

/** @brief Payload stucture for the #VSS_ISTREAM_CMD_DETACH_VOCPROC command.
*/
struct vss_istream_cmd_detach_vocproc_t
{
  uint16_t handle;
    /**< Vocproc handle to detach from the stream. */
}
#include "apr_pack_end.h"
;

/****************************************************************************
 * VARIABLE SAMPLE RATE VOCODER APIS                                        *
 ****************************************************************************/

/**
  Sets the sampling rates to be used for vocoders that support variable
  sampling rates.

  If the vocoder that is currently selected on the stream does not support
  variable sample reates, this command has no effect.

  If the vocoder that is currently selected on the stream supports variable
  sample rates and no sample rates are provided (i.e. this command is not
  called) then the sample rates that are provided to the MVM via
  #VSS_IMVM_CMD_SET_CAL_VAR_VOC_SAMPLING_RATE are used.

  If the vocoder that is currently selected on the stream supports variable
  sample rates and no sample rates are provided (i.e. this command is not
  called) and no sample rates are provided to the MVM either (i.e. the
  vocoder seleted on MVM is not variable sample rate or
  #VSS_IMVM_CMD_SET_CAL_VAR_VOC_SAMPLING_RATE is not called) then defaults
  are used.

  If the vocoder that is currently selected on the stream supports variable
  sample rates and no sample rates are provided (i.e. this command is not
  called) and the stream is not attached to the MVM, then defaults are used.

  @par Payload
  #vss_istream_cmd_set_var_voc_sampling_rate_t
 
  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.
*/
#define VSS_ISTREAM_CMD_SET_VAR_VOC_SAMPLING_RATE ( 0x00011368 )

/** @brief Type definition for #vss_istream_cmd_set_var_voc_sampling_rate_t.
*/
typedef struct vss_istream_cmd_set_var_voc_sampling_rate_t vss_istream_cmd_set_var_voc_sampling_rate_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_ISTREAM_CMD_SET_VAR_VOC_SAMPLING_RATE command.
*/
struct vss_istream_cmd_set_var_voc_sampling_rate_t
{
  uint32_t rx;
    /**< Rx sample rate. Supported values: \n
         - 8000 Hz \n
         - 16000 Hz \n
         - 48000 Hz */
  uint32_t tx;
    /**< Tx sampling rate. Supported values: \n
         - 8000 Hz \n
         - 16000 Hz \n
         - 48000 Hz */
}
#include "apr_pack_end.h"
;

/****************************************************************************
 * CALIBRATION DATA APIS                                                    *
 ****************************************************************************/

/** @brief Type definition for #vss_istream_cal_na_value_t.
*/
typedef union vss_istream_cal_na_value_t vss_istream_cal_na_value_t;

#include "apr_pack_begin.h"

/** @brief Union for specifying a calibration column N/A value.
*/
union vss_istream_cal_na_value_t
{
  uint8_t uint8_val;
    /**< UINT8 N/A value. */
  uint16_t uint16_val;
    /**< UINT16 N/A value. */
  uint32_t uint32_val;
    /**< UINT32 N/A value. */
  uint64_t uint64_val;
    /**< UINT64 N/A value. */
}
#include "apr_pack_end.h"
;


/** @brief Type definition for #vss_istream_cal_column_t.
*/
typedef struct vss_istream_cal_column_t vss_istream_cal_column_t;

#include "apr_pack_begin.h"

/** @brief Structure for specifying a calibration column.
*/
struct vss_istream_cal_column_t
{
  uint32_t id;
    /**< Column ID. Supported values: \n
         see @latexonly \hyperref[calibration_column]{Calibration Columns}
         @endlatexonly. */
  uint32_t type;
    /**< Column type. Supported values: \n
         see @latexonly \hyperref[calibration_column_type]{Calibration Column Types}
         @endlatexonly. */
  vss_istream_cal_na_value_t na_value;
    /**< The N/A column value associated with this column ID and type.
         
         The N/A value is a reseved value indicating that the calibration
         data entry corresponding to it in the table is independent of this
         column (and therefore the data is applied irrespective of what
         the current system configuration is for this column).

         The size of na_value is dependent on the column type. */
}
#include "apr_pack_end.h"
;


/**
  Registers the memory that contains the static calibration data table with the
  stream.

  The static calibration data table captures calibration data for system
  configurations that do not change while the stream is running (e.g. network,
  Tx/Rx PP sampling rate).
  
  Static calibration data is applied when the stream transitions to the run
  state.

  As part of this registration, in addition to the address, size and handle
  of the memory containing the calibration table, the client also provides
  the columns and associated column types which the stream uses to form a
  key that is used to index into the calibration table and look-up a matching
  entry. The columns/types must be provided in this command in the same order
  that they exist in the calibration table. If no matching entry is found,
  calibration data is not applied. If multiple matching entries are found, the
  order in which they are applied is specified in @latexonly \hyperref
  [cal_order]{Order of Applying Multiple Matching Calibration Entries}
  @endlatexonly.

  @par Payload
  #vss_istream_cmd_register_static_calibration_data_t

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  Any memory registered here must first be mapped with CVD by following
  @latexonly \hyperref[shmem_guideline]{Shared Memory Usage Guideline}
  @endlatexonly.

  @comments
  The client shall not modify the calibration data memory while it is
  registered and the stream does not modify this memory while registered.
  System stability problems could result if the client modifies the calibration
  data memory while it is registered. The calibration data at the registered
  memory is valid until deregistered. Registering a memory region for
  calibration data while another region is still registered causes the command
  to fail.

  The currently supported order for the columns provided in this command is:

  1. #VSS_ICOMMON_CAL_COLUMN_NETWORK
  2. #VSS_ICOMMON_CAL_COLUMN_TX_PP_SAMPLING_RATE
  3. #VSS_ICOMMON_CAL_COLUMN_RX_PP_SAMPLING_RATE
  4. #VSS_ICOMMON_CAL_COLUMN_TX_VOC_OPERATING_MODE
  5. #VSS_ICOMMON_CAL_COLUMN_RX_VOC_OPERATING_MODE
  6. #VSS_ICOMMON_CAL_COLUMN_MEDIA_ID
  7. #VSS_ICOMMON_CAL_COLUMN_FEATURE
*/
#define VSS_ISTREAM_CMD_REGISTER_STATIC_CALIBRATION_DATA ( 0x0001307D )

/** @brief Type definition for #vss_istream_cmd_register_static_calibration_data_t.
*/
typedef struct vss_istream_cmd_register_static_calibration_data_t vss_istream_cmd_register_static_calibration_data_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the
    #VSS_ISTREAM_CMD_REGISTER_STATIC_CALIBRATION_DATA command.

    Expected calibration data buffer format is specified in @latexonly
    \hyperref[cal_format]{Calibration Data Format} @endlatexonly.
*/
struct vss_istream_cmd_register_static_calibration_data_t
{
  uint32_t cal_mem_handle;
    /**< Handle to the shared memory that holds the calibration data.
         See @latexonly \hyperref[shmem_guideline]{Shared Memory Usage
         Guideline} @endlatexonly. */
  uint64_t cal_mem_address;
    /**< Location of calibration data. See @latexonly \hyperref
         [shmem_guideline]{Shared Memory Usage Guideline} @endlatexonly on how
         the address is interpreted. */
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
    /**< Array of columns of size num_columns. The order in which the columns are
         provided here must match the order in which they exist in the calibration
         table provided. */
}
#include "apr_pack_end.h"
;


/**
  Deregisters the memory that contains static calibration data from the stream.

  @par Payload
  None.

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.
*/
#define VSS_ISTREAM_CMD_DEREGISTER_STATIC_CALIBRATION_DATA ( 0x0001307E )


/****************************************************************************
 * COMMON VOCODER APIS                                                      *
 ****************************************************************************/

/**
  Sets the encoder Discontinuous Transmission (DTX) mode.

  @par Payload
  #vss_istream_cmd_set_enc_dtx_mode_t

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.
*/
#define VSS_ISTREAM_CMD_SET_ENC_DTX_MODE ( 0x0001101D )

/** @brief Type definition for #vss_istream_cmd_set_enc_dtx_mode_t.
*/
typedef struct vss_istream_cmd_set_enc_dtx_mode_t vss_istream_cmd_set_enc_dtx_mode_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_ISTREAM_CMD_SET_ENC_DTX_MODE command.
*/
struct vss_istream_cmd_set_enc_dtx_mode_t
{
   uint32_t enable;
      /**< Toggle DTX on or off. Possible values:\n
           - 0 -- Disables DTX \n
           - 1 -- Enables DTX */
}
#include "apr_pack_end.h"
;


/**
  Sets the decoder time warping parameters.

  @par Payload
  #vss_istream_cmd_set_dec_timewarp_t

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.
*/
#define VSS_ISTREAM_CMD_SET_DEC_TIMEWARP ( 0x0001101E )

/** @brief Type definition for #vss_istream_cmd_set_dec_timewarp_t.
*/
typedef struct vss_istream_cmd_set_dec_timewarp_t vss_istream_cmd_set_dec_timewarp_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_ISTREAM_CMD_SET_DEC_TIMEWARP command.
*/
struct vss_istream_cmd_set_dec_timewarp_t
{
   uint16_t enable_time_warp;
      /**< Toggle time warping on or off. Possible values:\n
           - 0x0000 -- Time warping is disabled.\n
           - 0x0001 -- Time warping is enabled. */
   uint16_t exp_length;
      /**<
        Expansion length. Generic timewarping tries to meet the target.
        Possible values:

        Narrowband:
        - Any value between 80 to 160 PCM samples -- Leads to compression.\n
        - Any value between 160 to 320 PCM samples -- Leads to expansion.

        Wideband:
        - Any value between 160 to 320 PCM samples -- Leads to compression.\n
        - Any value between 320 to 640 PCM samples -- Leads to expansion.
      */
   uint16_t enable_phase_match;
      /**< Toggle phase matching on or off. Possible values:\n
           - 0x0000 -- Phase matching is disabled.\n
           - 0x0001 -- Phase matching is enabled. */
   uint16_t run_length;
      /**< Run length is equal to the number of consecutive erasures the
           decoder has decoded immediately before the decoding of the current
           packet (>0). */
   int16_t phase_offset;
      /**< Phase offset is equal to the difference between the number of
           frames encoded and decoded. Possible values: -2, -1, 0, 1, 2. */
}
#include "apr_pack_end.h"
;

/****************************************************************************
 * CDMA SPECIFIC VOCODER APIS                                               *
 ****************************************************************************/

/**
  Sets the encoder minimum and maximum rate.

  @par Payload
  #vss_istream_cmd_cdma_set_enc_minmax_rate_t

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.
*/
#define VSS_ISTREAM_CMD_CDMA_SET_ENC_MINMAX_RATE ( 0x00011019 )

/** @brief Type definition for #vss_istream_cmd_cdma_set_enc_minmax_rate_t.
*/
typedef struct vss_istream_cmd_cdma_set_enc_minmax_rate_t vss_istream_cmd_cdma_set_enc_minmax_rate_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_ISTREAM_CMD_CDMA_SET_ENC_MINMAX_RATE
    command.
*/
struct vss_istream_cmd_cdma_set_enc_minmax_rate_t
{
   uint16_t min_rate;
      /**<
        Sets the lower boundary of the encoder rate. Possible values:\n
        - 0x0000 -- Blank frame \n
        - 0x0001 -- Eighth rate \n
        - 0x0002 -- Quarter rate \n
        - 0x0003 -- Half rate \n
        - 0x0004 -- Full rate */
   uint16_t max_rate;
      /**<
        Sets the upper boundary of the encoder rate. Possible values:\n
        - 0x0000 -- Blank frame \n
        - 0x0001 -- Eighth rate \n
        - 0x0002 -- Quarter rate \n
        - 0x0003 -- Half rate \n
        - 0x0004 -- Full rate */
}
#include "apr_pack_end.h"
;


/**
  Sets the encoder rate modulation.

  @par Payload
  #vss_istream_cmd_cdma_set_enc_rate_modulation_t

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.
*/
#define VSS_ISTREAM_CMD_CDMA_SET_ENC_RATE_MODULATION ( 0x0001101C )

/** Type definition for #vss_istream_cmd_cdma_set_enc_rate_modulation_t.
*/
typedef struct vss_istream_cmd_cdma_set_enc_rate_modulation_t vss_istream_cmd_cdma_set_enc_rate_modulation_t;

/* For struct vss_istream_cmd_cdma_set_enc_rate_modulation_t, 'mode' is
  equivalent to the 'rateModulationCmd' in the "6550 VDSP Firmware Interface
  Specification". */

#include "apr_pack_begin.h"

/** @brief Payload structure for the
    #VSS_ISTREAM_CMD_CDMA_SET_ENC_RATE_MODULATION command.
*/
struct vss_istream_cmd_cdma_set_enc_rate_modulation_t
{
   uint32_t mode;
      /**< Sets the vocoder Reduced-rate Modulation mode.

          The mode's bit structure is:\n
@verbatim
        b0    = Vocoder rate modulation is enabled when 1 and disabled
                when 0.
        b1    = Select X=S when 1 and select X=1/S when 0.
      b9..b2  = Rate limit factor is the value of S.
     b31..b10 = Reserved; keep as 0's.
@endverbatim

        The equation for the vocoder rate modulation ratio is:\n
@verbatim
                2X+1
     ratio = --------
              2(X+1)
@endverbatim

        Where X is S or 1/S, depending on the option selected. S is the
        rate limit factor. S is an integer that causes the ratio, when
        multiplied by the 14.4 kbps (full rate), to become the
        average bitrate. \n

        For convenience, some precalculated modes for EVRC include:\n
@verbatim
     0x00000000    Target 14.4 kbps (8/8 rate) on the average.
                   Bit values ORed:
                   0x00000000    Vocoder rate modulation disabled.

     0x0000000F    Target 12.2 kbps (7/8 rate) on the average.
                   Bit values ORed:
                   0x00000001    Vocoder rate modulation enabled.
                   0x00000002    Select X=S.
                   0x0000000C    Rate limit factor S=3.

     0x00000007    Target 11.2 kbps (6/8 rate) on the average.
                   Bit values ORed:
                   0x00000001    Vocoder rate modulation enabled.
                   0x00000002    Select X=S.
                   0x00000004    Rate limit factor S=1.

     0x00000005    Target 9.0 kbps (5/8 rate) on the average.
                   Bit values ORed:
                   0x00000001    Vocoder rate modulation enabled.
                   0x00000000    Select X=1/S.
                   0x00000004    Rate limit factor S=3.

     0x00000003    Target 7.2 kbps (4/8 rate) on the average
                   (1/2 rate is not supported for EVRC; must use 0x0000).
                   Bit values ORed:
                   0x00000001    Vocoder rate modulation enabled.
                   0x00000001    Select X=S.
                   0x00000000    Rate limit factor S=0.
@endverbatim
*/
}
#include "apr_pack_end.h"
;

/****************************************************************************
 * QCELP-13K VOCODER APIS                                                   *
 ****************************************************************************/

/**
  Sets the Qualcomm Code Excited Linear Prediction 13K (QCELP-13K) average encoder rate.

  @par Payload
  #vss_istream_cmd_voc_qcelp13k_set_rate_t

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.
*/
#define VSS_ISTREAM_CMD_VOC_QCELP13K_SET_RATE ( 0x0001113B )

/** @brief Type definition for #vss_istream_cmd_voc_qcelp13k_set_rate_t.
*/
typedef struct vss_istream_cmd_voc_qcelp13k_set_rate_t vss_istream_cmd_voc_qcelp13k_set_rate_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_ISTREAM_CMD_VOC_QCELP13K_SET_RATE
    command.
*/
struct vss_istream_cmd_voc_qcelp13k_set_rate_t
{
   uint32_t rate;
      /**< Sets the average vocoder rate. Possible values:\n
           - 0x00000000 -- 14.4 kbps \n
           - 0x00000001 -- 12.2 kbps \n
           - 0x00000002 -- 11.2 kbps \n
           - 0x00000003 --  9.0 kbps \n
           - 0x00000004 --  7.2 kbps */
}
#include "apr_pack_end.h"
;

/****************************************************************************
 * 4GV-NB VOCODER APIs                                                      *
 ****************************************************************************/

/**
  Sets the 4th Generation Vocoder Narrowband (4GV-NB) average encoder rate.

  @par Payload
  #vss_istream_cmd_voc_4gvnb_set_rate_t

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.
*/
#define VSS_ISTREAM_CMD_VOC_4GVNB_SET_RATE ( 0x0001113C )

/** @brief Type definition for #vss_istream_cmd_voc_4gvnb_set_rate_t.
*/
typedef struct vss_istream_cmd_voc_4gvnb_set_rate_t vss_istream_cmd_voc_4gvnb_set_rate_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_ISTREAM_CMD_VOC_4GVNB_SET_RATE
    command.
*/
struct vss_istream_cmd_voc_4gvnb_set_rate_t
{
   uint32_t rate;
      /**< Sets the average vocoder rate. Possible values:\n
           - 0x00000000 -- 10.0 kbps \n
           - 0x00000001 --  8.5 kbps \n
           - 0x00000002 --  7.5 kbps \n
           - 0x00000003 --  7.0 kbps \n
           - 0x00000004 --  6.6 kbps \n
           - 0x00000005 --  6.2 kbps \n
           - 0x00000006 --  5.8 kbps \n
           - 0x00000007 --  4.8 kbps */
}
#include "apr_pack_end.h"
;

/****************************************************************************
 * 4GV-WB VOCODER APIS                                                      *
 ****************************************************************************/

/**
  Sets the 4th Generation Vocoder Wideband (4GV-WB) average encoder rate.

  @par Payload
  #vss_istream_cmd_voc_4gvwb_set_rate_t

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.
*/
#define VSS_ISTREAM_CMD_VOC_4GVWB_SET_RATE ( 0x0001113D )

/** @brief Type definition for #vss_istream_cmd_voc_4gvwb_set_rate_t.
*/
typedef struct vss_istream_cmd_voc_4gvwb_set_rate_t vss_istream_cmd_voc_4gvwb_set_rate_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_ISTREAM_CMD_VOC_4GVWB_SET_RATE
    command.
*/
struct vss_istream_cmd_voc_4gvwb_set_rate_t
{
   uint32_t rate;
      /**< Sets the average vocoder rate. Possible values:\n
           - 0x00000000 --  8.5 kbps \n
           - 0x00000004 -- 10.0 kbps \n
           - 0x00000007 --  4.8 kbps */
}
#include "apr_pack_end.h"
;

/****************************************************************************
 * 4GV-NW VOCODER APIS                                                      *
 ****************************************************************************/

/**
  Sets the 4th Generation Vocoder Narrowband-Wideband (4GV-NW) average encoder rate.

  @par Payload
  #vss_istream_cmd_voc_4gvnw_set_rate_t

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.
*/
#define VSS_ISTREAM_CMD_VOC_4GVNW_SET_RATE ( 0x00011281 )

/** @brief Type definition for #vss_istream_cmd_voc_4gvnw_set_rate_t.
*/
typedef struct vss_istream_cmd_voc_4gvnw_set_rate_t vss_istream_cmd_voc_4gvnw_set_rate_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the
    #VSS_ISTREAM_CMD_VOC_4GVNW_SET_RATE command.
*/
struct vss_istream_cmd_voc_4gvnw_set_rate_t
{
   uint32_t rate;
      /**<
        Sets the average vocoder rate. Possible values:\n
        - 0x00000000 --  8.5 kbps.\n
        - 0x00000001 --  9.3 kbps.\n
        - 0x00000002 --  7.5 kbps.\n
        - 0x00000003 --  7.0 kbps.\n
        - 0x00000004 --  6.6 kbps.\n
        - 0x00000005 --  6.2 kbps.\n
        - 0x00000006 --  5.8 kbps.\n
        - 0x00000007 --  4.8 kbps.\n
      */
}
#include "apr_pack_end.h"
;

/****************************************************************************
 * 4GV-NW-2K VOCODER APIS                                                   *
 ****************************************************************************/

/**
  Sets the 4th Generation Narrow-Wide-2K (4GV-NW-2K) average encoder rate.

  @par Payload
  #vss_istream_cmd_voc_4gvnw2k_set_rate_t

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.
*/
#define VSS_ISTREAM_CMD_VOC_4GVNW2K_SET_RATE ( 0x00012E5F )

/** @brief Type definition for #vss_istream_cmd_voc_4gvnw2k_set_rate_t.
*/
typedef struct vss_istream_cmd_voc_4gvnw2k_set_rate_t vss_istream_cmd_voc_4gvnw2k_set_rate_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_ISTREAM_CMD_VOC_4GVNW2K_SET_RATE command.
*/
struct vss_istream_cmd_voc_4gvnw2k_set_rate_t
{
   uint32_t rate;
      /**<
        Sets the average vocoder rate. Possible values:\n
        - 0x00000000 --  8.5 kbps.\n
        - 0x00000001 --  9.3 kbps.\n
        - 0x00000002 --  7.5 kbps.\n
        - 0x00000003 --  2.4 kbps.\n
        - 0x00000004 --  6.6 kbps.\n
        - 0x00000005 --  6.2 kbps.\n
        - 0x00000006 --  5.8 kbps.\n
        - 0x00000007 --  4.8 kbps.\n
      */
}
#include "apr_pack_end.h"
;

/****************************************************************************
 * AMR VOCODER APIS                                                         *
 ****************************************************************************/

/**
  Sets the Adaptive Multirate Codec (AMR) encoder rate.

  @par Payload
  #vss_istream_cmd_voc_amr_set_enc_rate_t

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.
*/
#define VSS_ISTREAM_CMD_VOC_AMR_SET_ENC_RATE ( 0x0001113E )

/** @brief Type definition for #vss_istream_cmd_voc_amr_set_enc_rate_t.
*/
typedef struct vss_istream_cmd_voc_amr_set_enc_rate_t vss_istream_cmd_voc_amr_set_enc_rate_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_ISTREAM_CMD_VOC_AMR_SET_ENC_RATE
    command.
*/
struct vss_istream_cmd_voc_amr_set_enc_rate_t
{
   uint32_t mode;
      /**< Sets the AMR encoder rate. Possible values:\n
           - 0x00000000 -- 4.75 kbps \n
           - 0x00000001 -- 5.15 kbps \n
           - 0x00000002 -- 5.90 kbps \n
           - 0x00000003 -- 6.70 kbps \n
           - 0x00000004 -- 7.40 kbps \n
           - 0x00000005 -- 7.95 kbps \n
           - 0x00000006 -- 10.2 kbps \n
           - 0x00000007 -- 12.2 kbps */
}
#include "apr_pack_end.h"
;

/****************************************************************************
 * AMR-WB VOCODER APIS                                                      *
 ****************************************************************************/

/**
  Sets the Adaptive Multirate Codec Wideband (AMR-WB) encoder rate.

  @par Payload
  #vss_istream_cmd_voc_amrwb_set_enc_rate_t

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.
*/
#define VSS_ISTREAM_CMD_VOC_AMRWB_SET_ENC_RATE ( 0x0001113F )

/** @brief Type definition for #vss_istream_cmd_voc_amrwb_set_enc_rate_t.
*/
typedef struct vss_istream_cmd_voc_amrwb_set_enc_rate_t vss_istream_cmd_voc_amrwb_set_enc_rate_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_ISTREAM_CMD_VOC_AMRWB_SET_ENC_RATE
    command.
*/
struct vss_istream_cmd_voc_amrwb_set_enc_rate_t
{
   uint32_t mode;
      /**< Sets the AMR-WB encoder rate. Possible values:\n
           - 0x00000000 --  6.60 kbps \n
           - 0x00000001 --  8.85 kbps \n
           - 0x00000002 -- 12.65 kbps \n
           - 0x00000003 -- 14.25 kbps \n
           - 0x00000004 -- 15.85 kbps \n
           - 0x00000005 -- 18.25 kbps \n
           - 0x00000006 -- 19.85 kbps \n
           - 0x00000007 -- 23.05 kbps \n
           - 0x00000008 -- 23.85 kbps */
}
#include "apr_pack_end.h"
;

/****************************************************************************
 * DTMF GENERATION APIS                                                     *
 ****************************************************************************/

/**
  Starts/stops the Dual-Tone Multifrequency (DTMF) generation. The completion
  of the DTMF generation (either due to a Stop command or because the requested
  duration has elapsed) is indicated to the client via a
  VSS_ISTREAM_EVT_DTMF_GENERATION_ENDED event.

  @par Payload
  #vss_istream_cmd_set_dtmf_generation_t

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.

  @comments
  A new DTMF generation command replaces a previously unfinished command in the
  same direction. If the generation of a single tone is needed, set tone_1 and
  tone_2 parameters to the same value. If the stream is to be closed before the
  requested tone duration has elapsed, it is recommended that the client first stop
  the tone that is currently playing, thus ensuring proper rampdown and avoiding
  audio artifacts (pops).
*/
#define VSS_ISTREAM_CMD_SET_DTMF_GENERATION ( 0x00011023 )

/** DTMF generation direction. */
#define VSS_ISTREAM_DTMF_GENERATION_DIRECTION_TX ( 0 )

/** DTMF generation mix with a speech disable flag. */
#define VSS_ISTREAM_DTMF_GENERATION_MIX_WITH_SPEECH_DISABLE ( 0 )

/** DTMF generation mix with a speech enable flag. */
#define VSS_ISTREAM_DTMF_GENERATION_MIX_WITH_SPEECH_ENABLE ( 1 )

/** @brief Type definition for #vss_istream_cmd_set_dtmf_generation_t.
*/
typedef struct vss_istream_cmd_set_dtmf_generation_t vss_istream_cmd_set_dtmf_generation_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_ISTREAM_CMD_SET_DTMF_GENERATION
    command.
*/
struct vss_istream_cmd_set_dtmf_generation_t
{
   uint16_t direction;
      /**< DTMF generation direction. Supported values:\n
           #VSS_ISTREAM_DTMF_GENERATION_DIRECTION_TX */
   uint16_t mix_flag;
      /**< Mix with speech flag. Supported values:\n
           #VSS_ISTREAM_DTMF_GENERATION_MIX_WITH_SPEECH_DISABLE -- Generated
           DTMF replaces the speech.\n
           #VSS_ISTREAM_DTMF_GENERATION_MIX_WITH_SPEECH_ENABLE -- Generated DTMF
           mixes with the speech. */
   uint16_t tone_1;
      /**< DTMF tone 1. Possible values: 100 to 4000 Hz. */
   uint16_t tone_2;
      /**< DTMF tone 2. Possible values: 100 to 4000 Hz. */
   uint16_t gain;
      /**< DTMF tone gain. Possible value: Linear value in Q13 format. Because the
           level of tone generation is fixed at 0 dBFS, this value must be set to
           a negative gain. */
   int16_t duration;
      /**< Duration of the tone. Duration includes ramp-up and ramp-down periods.
           The ramp-up and ramp-down periods are 1 ms and 2 ms, respectively.
           Possible values:\n
           - -1 -- Infinite duration \n
           -  0 -- Stop infinite tone \n
           - >0 -- Finite duration in units of milliseconds */
}
#include "apr_pack_end.h"
;


/**
  Event sent by the stream to the client that enabled DTMF generation,
  indicating that the generation of the tones has ended either
  because the client issued a Stop command or the duration requested by
  the client has elapsed.

  @par Payload
  #vss_istream_evt_dtmf_generation_ended_t

  @return
  None.

  @dependencies
  None.
*/
#define VSS_ISTREAM_EVT_DTMF_GENERATION_ENDED ( 0x00011032 )

/** @brief Type definition for #vss_istream_evt_dtmf_generation_ended_t.
*/
typedef struct vss_istream_evt_dtmf_generation_ended_t vss_istream_evt_dtmf_generation_ended_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_ISTREAM_EVT_DTMF_GENERATION_ENDED
    event.
*/
struct vss_istream_evt_dtmf_generation_ended_t
{
   uint16_t direction;
     /**< Direction in which the DTMF tone has been generated.
          Supported values:\n
          #VSS_ISTREAM_DTMF_GENERATION_DIRECTION_TX */
}
#include "apr_pack_end.h"
;

/****************************************************************************
 * DTMF DETECTION APIS                                                      *
 ****************************************************************************/

/**
  Enables/disables Rx DTMF detection. The DTMF detection status is sent
  to the client sending this command via VSS_ISTREAM_EVT_RX_DTMF_DETECTED
  events.

  @par Payload
  #vss_istream_cmd_set_rx_dtmf_detection_t

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  Only one client can request Rx DTMF detection on a given stream at a
  given time. When another client requests Rx DTMF detection while the
  previous client's Rx DTMF detection is still active, the request
  fails.
*/
#define VSS_ISTREAM_CMD_SET_RX_DTMF_DETECTION ( 0x00011027 )

/** DTMF detection enable flag. */
#define VSS_ISTREAM_RX_DTMF_DETECTION_ENABLE ( 1 )

/** DTMF detection disable flag. */
#define VSS_ISTREAM_RX_DTMF_DETECTION_DISABLE ( 0 )

/** @brief Type definition for #vss_istream_cmd_set_rx_dtmf_detection_t.
*/
typedef struct vss_istream_cmd_set_rx_dtmf_detection_t vss_istream_cmd_set_rx_dtmf_detection_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_ISTREAM_CMD_SET_RX_DTMF_DETECTION
    command.
*/
struct vss_istream_cmd_set_rx_dtmf_detection_t
{
   uint32_t enable;
      /**< Enables/disables Rx DTMF detection. Supported values:\n
           #VSS_ISTREAM_RX_DTMF_DETECTION_ENABLE \n
           #VSS_ISTREAM_RX_DTMF_DETECTION_DISABLE */
}
#include "apr_pack_end.h"
;


/**
  Event sent by the stream to the client that enables Rx DTMF detection
  whenever DTMF is detected in the Rx path.

  @par Payload
  #vss_istream_evt_rx_dtmf_detected_t

  @return
  None.

  @dependencies
  None.

  @comments
  The DTMF detection feature can only be used to detect DTMF frequencies
  as listed in the ITU-T Recommendation Q.23: <i>Technical Features of
  Push-button Telephone Sets</i>, Section 6 (see @latexonly
  \hyperref[S5]{[S5]}@endlatexonly). Therefore, the possible values for the
  low_freq and high_freq parameters are as listed in the
  vss_istream_evt_rx_dtmf_detected_t structure.
*/
#define VSS_ISTREAM_EVT_RX_DTMF_DETECTED ( 0x0001101A )

/** @brief Type definition for #vss_istream_evt_rx_dtmf_detected_t.
*/
typedef struct vss_istream_evt_rx_dtmf_detected_t vss_istream_evt_rx_dtmf_detected_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_ISTREAM_EVT_RX_DTMF_DETECTED event.
*/
struct vss_istream_evt_rx_dtmf_detected_t
{
   uint16_t low_freq;
      /**< Detected low frequency. Possible values:\n
           - 697 Hz \n
           - 770 Hz \n
           - 852 Hz \n
           - 941 Hz */
   uint16_t high_freq;
      /**< Detected high frequency. Possible values:\n
           - 1209 Hz \n
           - 1336 Hz \n
           - 1477 Hz \n
           - 1633 Hz */
}
#include "apr_pack_end.h"
;

/****************************************************************************
 * EAMR EVENTS                                                              *
 ****************************************************************************/

/**
  Event class that identifies the eAMR mode change events. The 
  supported value is #VSS_ISTREAM_EVT_EAMR_MODE_CHANGED. 
*/
#define VSS_ISTREAM_EVENT_CLASS_EAMR_MODE_CHANGE ( 0x00012E58 )


/**
  Event sent by the stream to the client to notify that eAMR 
  mode has changed. 

  @par Payload
  #vss_istream_evt_eamr_mode_changed_t

  @return
  None.

  @dependencies
  None.

  @comments
  None.
*/
#define VSS_ISTREAM_EVT_EAMR_MODE_CHANGED ( 0x00012E59 )

/** eAMR narrowband mode. */
#define VSS_ISTREAM_EAMR_MODE_NARROWBAND ( 0 )

/** eAMR wideband mode. */
#define VSS_ISTREAM_EAMR_MODE_WIDEBAND ( 1 )

/** @brief Type definition for
 *         #vss_istream_evt_eamr_mode_changed_t.
                                                       */
typedef struct vss_istream_evt_eamr_mode_changed_t vss_istream_evt_eamr_mode_changed_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the
 *         #VSS_ISTREAM_EVT_EAMR_MODE_CHANGED event.
*/
struct vss_istream_evt_eamr_mode_changed_t
{
   uint32_t mode;
      /**< eAMR mode. Supported values:\n
           #VSS_ISTREAM_EAMR_MODE_NARROWBAND \n
           #VSS_ISTREAM_EAMR_MODE_WIDEBAND */  
}
#include "apr_pack_end.h"
;


/****************************************************************************
 * NOTIFICATIONS ISSUED BY THE STREAM TO ITS CLIENTS                        *
 ****************************************************************************/

/**
  Event sent by the stream session to its client when streaming is
  active.

  @par Payload
  None.

  @return
  None.

  @dependencies
  None.
*/
#define VSS_ISTREAM_EVT_READY ( 0x000110FC )


/**
  Event sent by the stream session to its client when streaming is
  inactive.

  @par Payload
  None.

  @return
  None.

  @dependencies
  None.
*/
#define VSS_ISTREAM_EVT_NOT_READY ( 0x000110FD )


/** @} */ /* end_addtogroup vss_istream_public_if */

#endif /* __VSS_ISTREAM_PUBLIC_IF_H__ */


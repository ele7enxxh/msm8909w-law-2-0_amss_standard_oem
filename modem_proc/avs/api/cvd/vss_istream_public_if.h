#ifndef __VSS_ISTREAM_PUBLIC_IF_H__
#define __VSS_ISTREAM_PUBLIC_IF_H__

/**
  @file vss_istream_public_if.h
  @brief This file contains the APR API definitions for Core Voice Stream (CVS)
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/api/cvd/vss_istream_public_if.h#1 $
  $Author: mplcsds1 $

when        who    what, where, why
-------     ---    ----------------------------------------------------------
08/27/14    sw     (Tech Pubs) Updated with Doxygen comments from 2.0, and
                   edited comments for 2.6.
===========================================================================*/


#include "apr_comdef.h"


/****************************************************************************
 * CORE STREAM APIS                                                         *
 ****************************************************************************/

/** @ingroup cvd_cvs_cmd_create_passive_ctrl 
  Creates a new passive control stream session.

  @apr_msgpayload
  The payload is the session name string used to identify the full control
  stream session for which a handle is to be obtained. The string size,
  including the NULL termination character, is limited to 31 characters.

  @detdesc
  Passive stream sessions allow clients to manage UI controls such as muting
  and volume levels while the corresponding full control sessions manage data
  exchange.
  @par
  Use the session name, <i>default modem voice</i>, to obtain a handle to the
  default stream session for a circuit-switched voice call.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q2,[Q2]}).

  @dependencies
  None.
*/ /* Q2 = Async Packet Router API for MDM9x15 Interface Spec (80-N1463-2)
*/
#define VSS_ISTREAM_CMD_CREATE_PASSIVE_CONTROL_SESSION ( 0x00011140 )

/** @ingroup cvd_cvs_cmd_create_full_ctrl
  Creates a new full control stream session.

  @apr_msgpayload{vss_istream_cmd_create_full_control_session_t}
  @wktable{weak__vss__istream__cmd__create__full__control__session__t}

  @detdesc
  Full stream sessions give clients full control over the stream, including
  data exchange and UI controls.
  @par
  If the client sets the direction to both Tx and Rx (bidirectional), the same
  media type must be used on each path.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q2,[Q2]}).

  @dependencies
  The session name, <i>default modem voice</i>, is reserved for the default
  stream session for circuit-switched voice calls and cannot be given to
  any new full control sessions.
  @par
  Multiple full control stream sessions cannot be created with the same name.
*/
#define VSS_ISTREAM_CMD_CREATE_FULL_CONTROL_SESSION ( 0x000110F7 )

/* Type definition for vss_istream_cmd_create_full_control_session_t. */
typedef struct vss_istream_cmd_create_full_control_session_t vss_istream_cmd_create_full_control_session_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_istream_cmd_create_full_control_session_t 
@{ */
/* Payload structure for the
    VSS_ISTREAM_CMD_CREATE_FULL_CONTROL_SESSION command.
*/
struct vss_istream_cmd_create_full_control_session_t
{
   uint16_t direction;
   /**< Specifies the path on which the session is created.
     
        @values
        - 0 -- Tx only
        - 1 -- Rx only
        - 2 -- Tx and Rx
        
        Currently, Tx and Rx loopback (3) is not supported. */

   uint32_t enc_media_type;
   /**< Tx vocoder type (see Section @xref{dox:MediaDefinitions}). */

   uint32_t dec_media_type;
   /**< Rx vocoder type (see Section @xref{dox:MediaDefinitions}). */

   uint32_t network_id;
   /**< Network ID (see @xhyperref{hdr:CalNetworkID,Network ID column values}}. */

#if __STDC_VERSION__ >= 199901L
   char_t name[];
     /**< Variable-sized stream name (optional).

          The stream name size is the payload size minus the size of the other
          fields. The string size, including the NULL termination character, is
        limited to 31 characters.

        The string must be NULL terminated if it is supplied.

        A size 0 name is interpreted as a nameless session where only the
        handle is used to access to the session. */
#endif /* __STDC_VERSION__ >= 199901L */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_istream_cmd_create_full_control_session_t */

/** @ingroup cvd_cvs_cmd_set_media_type
  Sets the media type on the stream. This command is supported for full control
  clients only.

  @apr_msgpayload{vss_istream_cmd_set_media_type_t}
  @wktable{weak__vss__istream__cmd__set__media__type__t}

  @detdesc
  If the client sets the direction to both Tx and Rx (bidirectional), the same
  media type must be used on each path.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q2,[Q2]}).

  @dependencies
  None.
*/
#define VSS_ISTREAM_CMD_SET_MEDIA_TYPE ( 0x00011186 )

/* Type definition for vss_istream_cmd_set_media_type_t. */
typedef struct vss_istream_cmd_set_media_type_t vss_istream_cmd_set_media_type_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_istream_cmd_set_media_type_t 
@{ */
/* Payload structure for the VSS_ISTREAM_CMD_SET_MEDIA_TYPE command.
*/
struct vss_istream_cmd_set_media_type_t
{
   uint32_t rx_media_id;
   /**< Specifies the Rx vocoder type.

        @values See Section @xref{dox:MediaDefinitions} */

   uint32_t tx_media_id;
   /**< Specifies the Tx vocoder type.

        @values See Section @xref{dox:MediaDefinitions} */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_istream_cmd_set_media_type_t */


/** @ingroup cvd_cvs_cmd_attach_vocproc
  Attaches a vocproc to the stream.

  @apr_msgpayload{vss_istream_cmd_attach_vocproc_t}
  @wktable{weak__vss__istream__cmd__attach__vocproc__t}

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q2,[Q2]}).

  @dependencies
  None.
*/
#define VSS_ISTREAM_CMD_ATTACH_VOCPROC ( 0x000110F8 )

/* Type definition for vss_istream_cmd_attach_vocproc_t. */
typedef struct vss_istream_cmd_attach_vocproc_t vss_istream_cmd_attach_vocproc_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_istream_cmd_attach_vocproc_t 
@{ */
/* Payload structure for the VSS_ISTREAM_CMD_ATTACH_VOCPROC command.
*/
struct vss_istream_cmd_attach_vocproc_t
{
  uint16_t handle;    /**< Vocproc handle to attach to the stream. */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_istream_cmd_attach_vocproc_t */


/** @ingroup cvd_cvs_cmd_detach_vocproc
  Detaches a vocproc from the stream.

  @apr_msgpayload{vss_istream_cmd_detach_vocproc_t}
  @wktable{weak__vss__istream__cmd__detach__vocproc__t}

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q2,[Q2]}).

  @dependencies
  None.
*/
#define VSS_ISTREAM_CMD_DETACH_VOCPROC ( 0x000110F9 )

/* Type definition for vss_istream_cmd_detach_vocproc_t. */
typedef struct vss_istream_cmd_detach_vocproc_t vss_istream_cmd_detach_vocproc_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_istream_cmd_detach_vocproc_t 
@{ */
/* Payload stucture for the VSS_ISTREAM_CMD_DETACH_VOCPROC command.
*/
struct vss_istream_cmd_detach_vocproc_t
{
  uint16_t handle;    /**< Vocproc handle to detach from the stream. */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weak_vss_istream_cmd_detach_vocproc_t */

/****************************************************************************
 * CALIBRATION DATA APIS                                                    *
 ****************************************************************************/

/* Type definition for vss_istream_cal_na_value_t. */
typedef union vss_istream_cal_na_value_t vss_istream_cal_na_value_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_istream_cal_na_value_t
@{ */
/* Union for specifying a calibration column N/A value.
*/
union vss_istream_cal_na_value_t
{
  uint8_t uint8_val;      /**< UINT8 N/A value. */
  uint16_t uint16_val;    /**< UINT16 N/A value. */
  uint32_t uint32_val;    /**< UINT32 N/A value. */
  uint64_t uint64_val;    /**< UINT64 N/A value. */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_istream_cal_na_value_t */

/* Type definition for vss_istream_cal_column_t. */
typedef struct vss_istream_cal_column_t vss_istream_cal_column_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_istream_cal_column_t 
@{ */
/* Structure for specifying a calibration column.
*/
struct vss_istream_cal_column_t
{
  uint32_t id;
  /**< Column ID. 
    
       @values See @xhyperref{hdr:CalibrationColumns,Calibration columns} */

  uint32_t type;
  /**< Column type. 

       @values See @xhyperref{hdr:CalibrationColumnTypes,Calibration column types} */

  vss_istream_cal_na_value_t na_value;
  /**< N/A column value associated with this column ID and type.
         
       N/A is a reserved value indicating that the calibration data entry
       corresponding to it in the table is independent of this column (and
       therefore the data is applied regardless of the current system
       configuration for this column). 

       The size of na_value is dependent on the column type. */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_istream_cal_column_t */


/** @ingroup cvd_cvs_cmd_reg_stat_cal_data
  Registers the memory that contains the static calibration data table with the
  stream.

  @note1hang For guidelines on using shared memory, see
             Section @xref{hdr:ShMemGuidelines}.

  @apr_msgpayload{vss_istream_cmd_register_static_calibration_data_t}
  @wktablens{weak__vss__istream__cmd__register__static__calibration__data__t}

  @par Calibration column payload (vss_istream_cal_column_t)
  @wktable{weak__vss__istream__cal__column__t} @newpage

  @par Calibration column N/A values (vss_istream_cal_na_value_t)
  @wktable{weak__vss__istream__cal__na__value__t}

  @detdesc
  The static calibration data table captures calibration data for system
  configurations that do not change while the stream is running
  (such as the network or the Tx/Rx PP sampling rate).
  @par
  Static calibration data is applied when the stream transitions to the Run
  state.
  @par
  During registration, the client provides the address, size, and handle of the
  memory containing the calibration table. The client also provides the columns
  and associated column types that the stream uses to form a key. This key is
  used to index into the calibration table and look up a matching entry. The
  columns and types must be provided in this command in the same order that
  they exist in the calibration table.
  - If no matching entry is found, calibration data is not applied. 
  - If multiple matching entries are found, the order in which they are applied
    is specified in Section&nbsp;@xref{hdr:OrderMultiMatchingCalibEntries}.
  @par
  The currently supported order for the columns provided in this command is:
  -# #VSS_ICOMMON_CAL_COLUMN_NETWORK
  -# #VSS_ICOMMON_CAL_COLUMN_TX_PP_SAMPLING_RATE
  -# #VSS_ICOMMON_CAL_COLUMN_RX_PP_SAMPLING_RATE
  -# #VSS_ICOMMON_CAL_COLUMN_TX_VOC_OPERATING_MODE
  -# #VSS_ICOMMON_CAL_COLUMN_RX_VOC_OPERATING_MODE
  -# #VSS_ICOMMON_CAL_COLUMN_MEDIA_ID
  -# #VSS_ICOMMON_CAL_COLUMN_FEATURE
  @par
  The client cannot modify the calibration data memory while it is registered,
  and the stream does not modify this memory while it is registered.
  System stability problems might result if the client modifies the calibration
  data memory while it is registered.
  @par
  The calibration data at the registered memory is valid until it is
  deregistered.
  Registering a memory region for calibration data while another region is
  still registered causes the command to fail.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q2,[Q2]}).

  @dependencies
  Any memory registered here must first be mapped with the CVD by following
  the guidelines in Section @xref{hdr:ShMemGuidelines}. 
*/
#define VSS_ISTREAM_CMD_REGISTER_STATIC_CALIBRATION_DATA ( 0x0001307D )

/* Type definition for #vss_istream_cmd_register_static_calibration_data_t. */
typedef struct vss_istream_cmd_register_static_calibration_data_t vss_istream_cmd_register_static_calibration_data_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_istream_cmd_register_static_calibration_data_t
@{ */
/* Payload of the VSS_ISTREAM_CMD_REGISTER_STATIC_CALIBRATION_DATA command. */
/** The expected calibration data buffer format is specified in
    Section @xref{hdr:CalibrationDataFormat}.
*/
struct vss_istream_cmd_register_static_calibration_data_t
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
/** @} */ /* end_weakgoup weak_vss_istream_cmd_register_static_calibration_data_t */

/** @ingroup cvd_cvs_cmd_dereg_stat_cal_data
  Deregisters the memory that contains static calibration data from the stream.

  @apr_msgpayload
  None.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q2,[Q2]}).

  @dependencies
  None.
*/
#define VSS_ISTREAM_CMD_DEREGISTER_STATIC_CALIBRATION_DATA ( 0x0001307E )


/****************************************************************************
 * COMMON VOCODER APIS                                                      *
 ****************************************************************************/

/** @ingroup cvd_cvs_cmd_set_enc_dtx_mode
  Sets the encoder Discontinuous Transmission (DTX) mode.

  @apr_msgpayload{vss_istream_cmd_set_enc_dtx_mode_t}
  @wktable{weak__vss__istream__cmd__set__enc__dtx__mode__t}

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q2,[Q2]}).

  @dependencies
  None.
*/
#define VSS_ISTREAM_CMD_SET_ENC_DTX_MODE ( 0x0001101D )

/* Type definition for vss_istream_cmd_set_enc_dtx_mode_t. */
typedef struct vss_istream_cmd_set_enc_dtx_mode_t vss_istream_cmd_set_enc_dtx_mode_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_istream_cmd_set_enc_dtx_mode_t 
@{ */
/* Payload structure for the VSS_ISTREAM_CMD_SET_ENC_DTX_MODE command.
*/
struct vss_istream_cmd_set_enc_dtx_mode_t
{
   uint32_t enable;
   /**< Specifies whether DTX is DTX mode is disabled (0) or enabled (1). */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_istream_cmd_set_enc_dtx_mode_t */


/** @ingroup cvd_cvs_cmd_set_dec_timewarp
  Sets the decoder time warping parameters.

  @apr_msgpayload{vss_istream_cmd_set_dec_timewarp_t}
  @wktable{weak__vss__istream__cmd__set__dec__timewarp__t}

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q2,[Q2]}).

  @dependencies
  None.
*/
#define VSS_ISTREAM_CMD_SET_DEC_TIMEWARP ( 0x0001101E )

/* Type definition for vss_istream_cmd_set_dec_timewarp_t. */
typedef struct vss_istream_cmd_set_dec_timewarp_t vss_istream_cmd_set_dec_timewarp_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_istream_cmd_set_dec_timewarp_t 
@{ */
/* Payload structure for the VSS_ISTREAM_CMD_SET_DEC_TIMEWARP command.
*/
struct vss_istream_cmd_set_dec_timewarp_t
{
   uint16_t enable_time_warp;
   /**< Specifies whether time warping is disabled (0) or enabled (1). */

   uint16_t exp_length;
   /**< Expansion length for time warping.
           
        @values{for narrowband}
        - Any value between 80 to 160 PCM samples leads to compression
        - Any value between 160 to 320 PCM samples leads to expansion 

        @values{for wideband}
        - Any value between 160 to 320 PCM samples leads to compression
        - Any value between 320 to 640 PCM samples leads to expansion

        A generic time warp tries to meet the target. */

   uint16_t enable_phase_match;
   /**< Specifies whether phase matching is disabled (0) or enabled (1). */

   uint16_t run_length;
   /**< Number of consecutive erasures the decoder has decoded immediately
        before the current packet is decoded.
        
        @values > 0 */

   int16_t phase_offset;
   /**< Difference between the number of frames encoded and the number of
        frames decoded.
           
        @values -2, -1, 0, 1, 2 */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_istream_cmd_set_dec_timewarp_t */


/****************************************************************************
 * CDMA SPECIFIC VOCODER APIS                                               *
 ****************************************************************************/

/** @cond OEM_only */
/** @ingroup cvd_cvs_cmd_cdma_set_enc_mnmx_rate
  Sets the minimum and maximum rates on the encoder.

  @apr_msgpayload{vss_istream_cmd_cdma_set_enc_minmax_rate_t}
  @wktable{weak__vss__istream__cmd__cdma__set__enc__minmax__rate__t}

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q2,[Q2]}).

  @dependencies
  None.
*/
#define VSS_ISTREAM_CMD_CDMA_SET_ENC_MINMAX_RATE ( 0x00011019 )

/* Type definition for vss_istream_cmd_cdma_set_enc_minmax_rate_t. */
typedef struct vss_istream_cmd_cdma_set_enc_minmax_rate_t vss_istream_cmd_cdma_set_enc_minmax_rate_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_istream_cmd_cdma_set_enc_minmax_rate_t 
@{ */
/* Payload structure for the VSS_ISTREAM_CMD_CDMA_SET_ENC_MINMAX_RATE
    command.
*/
struct vss_istream_cmd_cdma_set_enc_minmax_rate_t
{
   uint16_t min_rate;
   /**< Specifies the lower boundary of the encoder rate.
      
        @values
        - 0x0000 -- Blank frame
        - 0x0001 -- Eighth rate
        - 0x0002 -- Quarter rate
        - 0x0003 -- Half rate
        - 0x0004 -- Full rate @tablebulletend */

   uint16_t max_rate;
   /**< Specifies the upper boundary of the encoder rate.
      
        @values
        - 0x0000 -- Blank frame
        - 0x0001 -- Eighth rate
        - 0x0002 -- Quarter rate
        - 0x0003 -- Half rate
        - 0x0004 -- Full rate @tablebulletend */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_istream_cmd_cdma_set_enc_minmax_rate_t */
/** @endcond */


/** @cond OEM_only */
/** @ingroup cvd_cvs_cmd_cdma_set_enc_rate_mod
  Sets the rate modulation parameter on the encoder.

  @apr_msgpayload{vss_istream_cmd_cdma_set_enc_rate_modulation_t}
  @wktable{weak__vss__istream__cmd__cdma__set__enc__rate__modulation__t}

  @detdesc
  In the mode parameter, the equation for the vocoder rate modulation 
  ratio is: \n
@code
                2X+1
     ratio = --------
              2(X+1)
@endcode
  @par
  Where:
  - X is S or 1/S, depending on the selected option
  - S is the rate limit factor
  @par
  S is an integer that causes the ratio, when multiplied by 14.4 kbps (full
  rate), to become the average bitrate.
  @par
  For convenience, some precalculated modes for EVRC include:
@code
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
@endcode

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q2,[Q2]}).

  @dependencies
  None.
*/
#define VSS_ISTREAM_CMD_CDMA_SET_ENC_RATE_MODULATION ( 0x0001101C )

/* Type definition for vss_istream_cmd_cdma_set_enc_rate_modulation_t. */
typedef struct vss_istream_cmd_cdma_set_enc_rate_modulation_t vss_istream_cmd_cdma_set_enc_rate_modulation_t;

/* For struct vss_istream_cmd_cdma_set_enc_rate_modulation_t, 'mode' is
  equivalent to the 'rateModulationCmd' in the "6550 VDSP Firmware Interface
  Specification". */

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_istream_cmd_cdma_set_enc_rate_modulation_t 
@{ */
/* Payload of the VSS_ISTREAM_CMD_CDMA_SET_ENC_RATE_MODULATION command. */
struct vss_istream_cmd_cdma_set_enc_rate_modulation_t
{
   uint32_t mode;
   /**< Sets the vocoder Reduced Rate Modulation mode.

        @values{for bit 0}
        - 0 -- Disables vocoder rate modulation
        - 1 -- Enables vocoder rate modulation

        @values{for bit 1}
        - 0 -- Selects X=1/S
        - 1 -- Selects X=S

        @values{for bits 9 to 2}
        - Rate limit factor is the value of S (8-bit unsigned integer)

        Bits 31 to 10 are reserved; they must be set to zero. */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_istream_cmd_cdma_set_enc_rate_modulation_t */
/** @endcond */


/****************************************************************************
 * QCELP-13K VOCODER APIS                                                   *
 ****************************************************************************/

/** @cond OEM_only */
/** @ingroup cvd_cvs_cmd_voc_qcelp13k_set_rate
  Sets the Qualcomm Code Excited Linear Prediction 13K (QCELP-13K) average
  encoder rate.

  @apr_msgpayload{vss_istream_cmd_voc_qcelp13k_set_rate_t}
  @wktable{weak__vss__istream__cmd__voc__qcelp13k__set__rate__t}

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q2,[Q2]}).

  @dependencies
  None.
*/
#define VSS_ISTREAM_CMD_VOC_QCELP13K_SET_RATE ( 0x0001113B )

/* Type definition for vss_istream_cmd_voc_qcelp13k_set_rate_t. */
typedef struct vss_istream_cmd_voc_qcelp13k_set_rate_t vss_istream_cmd_voc_qcelp13k_set_rate_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_istream_cmd_voc_qcelp13k_set_rate_t 
@{ */
/* Payload structure for the VSS_ISTREAM_CMD_VOC_QCELP13K_SET_RATE
    command.
*/
struct vss_istream_cmd_voc_qcelp13k_set_rate_t
{
   uint32_t rate;
   /**< Specifies the average vocoder rate. 
      
        @values
        - 0x00000000 -- 14.4 kbps
        - 0x00000001 -- 12.2 kbps
        - 0x00000002 -- 11.2 kbps
        - 0x00000003 --  9.0 kbps
        - 0x00000004 --  7.2 kbps @tablebulletend */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_istream_cmd_voc_qcelp13k_set_rate_t */
/** @endcond */


/****************************************************************************
 * 4GV-NB VOCODER APIs                                                      *
 ****************************************************************************/

/** @cond OEM_only */
/** @ingroup cvd_cvs_cmd_voc_4gvnb_set_rate
  Sets the 4GV-NB average encoder rate.

  @apr_msgpayload{vss_istream_cmd_voc_4gvnb_set_rate_t}
  @wktable{weak__vss__istream__cmd__voc__4gvnb__set__rate__t}

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q2,[Q2]}).

  @dependencies
  None.
*/
#define VSS_ISTREAM_CMD_VOC_4GVNB_SET_RATE ( 0x0001113C )

/* Type definition for vss_istream_cmd_voc_4gvnb_set_rate_t. */
typedef struct vss_istream_cmd_voc_4gvnb_set_rate_t vss_istream_cmd_voc_4gvnb_set_rate_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_istream_cmd_voc_4gvnb_set_rate_t 
@{ */
/* Payload structure for the VSS_ISTREAM_CMD_VOC_4GVNB_SET_RATE
    command.
*/
struct vss_istream_cmd_voc_4gvnb_set_rate_t
{
   uint32_t rate;
   /**< Specifies the average vocoder rate.
      
        @values
        - 0x00000000 -- 10.0 kbps
        - 0x00000001 --  8.5 kbps
        - 0x00000002 --  7.5 kbps
        - 0x00000003 --  7.0 kbps
        - 0x00000004 --  6.6 kbps
        - 0x00000005 --  6.2 kbps
        - 0x00000006 --  5.8 kbps
        - 0x00000007 --  4.8 kbps @tablebulletend */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_istream_cmd_voc_4gvnb_set_rate_t */
/** @endcond */


/****************************************************************************
 * 4GV-WB VOCODER APIS                                                      *
 ****************************************************************************/

/** @cond OEM_only */
/** @ingroup cvd_cvs_cmd_voc_4gvwb_set_rate
  Sets the 4GV-WB average encoder rate.

  @apr_msgpayload{vss_istream_cmd_voc_4gvwb_set_rate_t}
  @wktable{weak__vss__istream__cmd__voc__4gvwb__set__rate__t}

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q2,[Q2]}).

  @dependencies
  None.
*/
#define VSS_ISTREAM_CMD_VOC_4GVWB_SET_RATE ( 0x0001113D )

/* Type definition for vss_istream_cmd_voc_4gvwb_set_rate_t. */
typedef struct vss_istream_cmd_voc_4gvwb_set_rate_t vss_istream_cmd_voc_4gvwb_set_rate_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_istream_cmd_voc_4gvwb_set_rate_t 
@{ */
/* Payload structure for the VSS_ISTREAM_CMD_VOC_4GVWB_SET_RATE
    command.
*/
struct vss_istream_cmd_voc_4gvwb_set_rate_t
{
   uint32_t rate;
   /**< Specifies the average vocoder rate.
      
        @values
        - 0x00000000 --  8.5 kbps
        - 0x00000004 -- 10.0 kbps
        - 0x00000007 --  4.8 kbps @tablebulletend */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_istream_cmd_voc_4gvwb_set_rate_t */
/** @endcond */


/****************************************************************************
 * 4GV-NW VOCODER APIS                                                      *
 ****************************************************************************/

/** @cond OEM_only */
/** @ingroup cvd_cvs_cmd_voc_4gvnw_set_rate
  Sets the 4GV-NW average encoder rate.

  @apr_msgpayload{vss_istream_cmd_voc_4gvnw_set_rate_t}
  @wktable{weak__vss__istream__cmd__voc__4gvnw__set__rate__t}

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q2,[Q2]}).

  @dependencies
  None.
*/
#define VSS_ISTREAM_CMD_VOC_4GVNW_SET_RATE ( 0x00011281 )

/* Type definition for vss_istream_cmd_voc_4gvnw_set_rate_t. */
typedef struct vss_istream_cmd_voc_4gvnw_set_rate_t vss_istream_cmd_voc_4gvnw_set_rate_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_istream_cmd_voc_4gvnw_set_rate_t 
@{ */
/* Payload structure for the
    VSS_ISTREAM_CMD_VOC_4GVNW_SET_RATE command.
*/
struct vss_istream_cmd_voc_4gvnw_set_rate_t
{
   uint32_t rate;
   /**< Specifies the average vocoder rate.
        
        @values
        - 0x00000000 --  8.5 kbps
        - 0x00000001 --  9.3 kbps
        - 0x00000002 --  7.5 kbps
        - 0x00000003 --  7.0 kbps
        - 0x00000004 --  6.6 kbps
        - 0x00000005 --  6.2 kbps
        - 0x00000006 --  5.8 kbps
        - 0x00000007 --  4.8 kbps @tablebulletend */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_istream_cmd_voc_4gvnw_set_rate_t */
/** @endcond */


/****************************************************************************
 * 4GV-NW-2K VOCODER APIS                                                   *
 ****************************************************************************/

/** @ingroup cvd_cvs_cmd_voc_4_set_rate
  Sets the 4th Generation Narrow-Wide-2K (4GV-NW-2K) average encoder rate.

  @apr_msgpayload{vss_istream_cmd_voc_4gvnw2k_set_rate_t}
  @wktable{weak__vss__istream__cmd__voc__4gvnw2k__set__rate__t}

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q2,[Q2]}).

  @dependencies
  None.
*/
#define VSS_ISTREAM_CMD_VOC_4GVNW2K_SET_RATE ( 0x00012E5F )

/* Type definition for vss_istream_cmd_voc_4gvnw2k_set_rate_t. */
typedef struct vss_istream_cmd_voc_4gvnw2k_set_rate_t vss_istream_cmd_voc_4gvnw2k_set_rate_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_istream_cmd_voc_4gvnw2k_set_rate_t
@{ */
/* Payload structure for the VSS_ISTREAM_CMD_VOC_4GVNW2K_SET_RATE command.
*/
struct vss_istream_cmd_voc_4gvnw2k_set_rate_t
{
   uint32_t rate;
   /**< Sets the average vocoder rate.
   
        @values
        - 0x00000000 --  8.5 kbps
        - 0x00000001 --  9.3 kbps
        - 0x00000002 --  7.5 kbps
        - 0x00000003 --  2.4 kbps
        - 0x00000004 --  6.6 kbps
        - 0x00000005 --  6.2 kbps
        - 0x00000006 --  5.8 kbps
        - 0x00000007 --  4.8 kbps @tablebulletend */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_istream_cmd_voc_4gvnw2k_set_rate_t */


/****************************************************************************
 * AMR VOCODER APIS                                                         *
 ****************************************************************************/

/** @cond OEM_only */
/** @ingroup cvd_cvs_cmd_voc_amr_set_enc_rate
  Sets the Adaptive Multirate Codec (AMR) encoder rate.

  @apr_msgpayload{vss_istream_cmd_voc_amr_set_enc_rate_t}
  @wktable{weak__vss__istream__cmd__voc__amr__set__enc__rate__t}

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q2,[Q2]}).

  @dependencies
  None.
*/
#define VSS_ISTREAM_CMD_VOC_AMR_SET_ENC_RATE ( 0x0001113E )

/* Type definition for vss_istream_cmd_voc_amr_set_enc_rate_t. */
typedef struct vss_istream_cmd_voc_amr_set_enc_rate_t vss_istream_cmd_voc_amr_set_enc_rate_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_istream_cmd_voc_amr_set_enc_rate_t 
@{ */
/* Payload structure for the VSS_ISTREAM_CMD_VOC_AMR_SET_ENC_RATE
    command.
*/
struct vss_istream_cmd_voc_amr_set_enc_rate_t
{
   uint32_t mode;
   /**< Specifies the AMR encoder rate.
      
        @values
        - 0x00000000 -- 4.75 kbps
        - 0x00000001 -- 5.15 kbps
        - 0x00000002 -- 5.90 kbps
        - 0x00000003 -- 6.70 kbps
        - 0x00000004 -- 7.40 kbps
        - 0x00000005 -- 7.95 kbps
        - 0x00000006 -- 10.2 kbps
        - 0x00000007 -- 12.2 kbps @tablebulletend */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_istream_cmd_voc_amr_set_enc_rate_t */
/** @endcond */


/****************************************************************************
 * AMR-WB VOCODER APIS                                                      *
 ****************************************************************************/

/** @cond OEM_only */
/** @ingroup cvd_cvs_cmd_voc_amrwb_set_enc_rate
  Sets the Adaptive Multirate Codec Wideband (AMR-WB) encoder rate.

  @apr_msgpayload{vss_istream_cmd_voc_amrwb_set_enc_rate_t}
  @wktable{weak__vss__istream__cmd__voc__amrwb__set__enc__rate__t}

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q2,[Q2]}).

  @dependencies
  None.
*/
#define VSS_ISTREAM_CMD_VOC_AMRWB_SET_ENC_RATE ( 0x0001113F )

/* Type definition for vss_istream_cmd_voc_amrwb_set_enc_rate_t. */
typedef struct vss_istream_cmd_voc_amrwb_set_enc_rate_t vss_istream_cmd_voc_amrwb_set_enc_rate_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_istream_cmd_voc_amrwb_set_enc_rate_t 
@{ */
/* Payload structure for the VSS_ISTREAM_CMD_VOC_AMRWB_SET_ENC_RATE
    command.
*/
struct vss_istream_cmd_voc_amrwb_set_enc_rate_t
{
   uint32_t mode;
   /**< Sets the AMR-WB encoder rate.
      
        @values
        - 0x00000000 --  6.60 kbps
        - 0x00000001 --  8.85 kbps
        - 0x00000002 -- 12.65 kbps
        - 0x00000003 -- 14.25 kbps
        - 0x00000004 -- 15.85 kbps
        - 0x00000005 -- 18.25 kbps
        - 0x00000006 -- 19.85 kbps
        - 0x00000007 -- 23.05 kbps
        - 0x00000008 -- 23.85 kbps @tablebulletend */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_istream_cmd_voc_amrwb_set_enc_rate_t */
/** @endcond */

/****************************************************************************
 * EVS VOCODER APIS                                                      *
 ****************************************************************************/

/** @cond OEM_only */

/** @ingroup cvd_cvs_cmd_set_evs_voc_enc_operating_mode
  Sets the Enhanced Voice Services (EVS) encoder operating mode.

  @apr_msgpayload{vss_istream_cmd_set_evs_voc_enc_operating_mode_t}
  @wktable{weak__vss__istream__cmd__set__evs__voc__enc__operating__mode__t}

  @detdesc
  The operating mode consists of the bit-rate and the bit-rate's
  associated bandwidth.

  This command will have no effect and will return and error when either:
  - The EVS vocoder is not being used by the stream
  - The payload contains an invalid bit-rate or bandwidth
  - The payload contains an invalid EVS bit-rate with respect to the associated bandwidth
  - The bandwidth is higher than the sampling rates set on the PP blocks by default values
    or from #VSS_IMVM_CMD_SET_MAX_VAR_VOC_SAMPLING_RATE.

  Valid bandwidths for EVS bit-rates are described in the payload descriptor.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q2,[Q2]}).

  @dependencies
  None.
*/
#define VSS_ISTREAM_CMD_SET_EVS_VOC_ENC_OPERATING_MODE ( 0x00013166 )

/** Bandwidth is Narrowband for EVS. */
#define VSS_ISTREAM_EVS_VOC_BANDWIDTH_NB ( 0 )

/** Bandwidth is Wideband for EVS. */
#define VSS_ISTREAM_EVS_VOC_BANDWIDTH_WB ( 1 )

/** Bandwidth is Super-wideband for EVS. */
#define VSS_ISTREAM_EVS_VOC_BANDWIDTH_SWB ( 2 )

/** Bandwidth is Fullband for EVS. */
#define VSS_ISTREAM_EVS_VOC_BANDWIDTH_FB ( 3 )

/* Type definition for vss_istream_cmd_set_evs_voc_enc_operating_mode_t. */
typedef struct vss_istream_cmd_set_evs_voc_enc_operating_mode_t vss_istream_cmd_set_evs_voc_enc_operating_mode_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_istream_cmd_set_evs_voc_enc_operating_mode_t
@{ */
/* Payload structure for the VSS_ISTREAM_CMD_SET_EVS_VOC_ENC_OPERATING_MODE
    command.
*/
struct vss_istream_cmd_set_evs_voc_enc_operating_mode_t
{
   uint8_t mode;
   /**< Sets the operating bit-rate on the vocoder encoder.
      
        @values
        - 0x00000000 --  6.60 kbps, AMR-WB IO (WB)
        - 0x00000001 --  8.85 kbps, AMR-WB IO (WB)
        - 0x00000002 -- 12.65 kbps, AMR-WB IO (WB)
        - 0x00000003 -- 14.25 kbps, AMR-WB IO (WB)
        - 0x00000004 -- 15.85 kbps, AMR-WB IO (WB)
        - 0x00000005 -- 18.25 kbps, AMR-WB IO (WB)
        - 0x00000006 -- 19.85 kbps, AMR-WB IO (WB)
        - 0x00000007 -- 23.05 kbps, AMR-WB IO (WB)
        - 0x00000008 -- 23.85 kbps, AMR-WB IO (WB)
        - 0x00000009 -- 5.90 kbps, EVS (NB, WB)
        - 0x0000000A -- 7.20 kbps, EVS (NB, WB)
        - 0x0000000B -- 8.00 kbps, EVS (NB, WB)
        - 0x0000000C -- 9.60 kbps, EVS (NB, WB, SWB)
        - 0x0000000D -- 13.20 kbps, EVS (NB, WB, SWB)
        - 0x0000000E -- 16.40 kbps, EVS (NB, WB, SWB, FB)
        - 0x0000000F -- 24.40 kbps, EVS (NB, WB, SWB, FB)
        - 0x00000010 -- 32.00 kbps, EVS (WB, SWB, FB)
        - 0x00000011 -- 48.00 kbps, EVS (WB, SWB, FB)
        - 0x00000012 -- 64.00 kbps, EVS (WB, SWB, FB)
        - 0x00000013 -- 96.00 kbps, EVS (WB, SWB, FB)
        - 0x00000014 -- 128.00 kbps, EVS (WB, SWB, FB) @tablebulletend */

  uint8_t bandwidth;
  /**< Sets the operating audio bandwidth on the vocoder encoder.
    
        @values
        - #VSS_ISTREAM_EVS_VOC_BANDWIDTH_NB
        - #VSS_ISTREAM_EVS_VOC_BANDWIDTH_WB 
        - #VSS_ISTREAM_EVS_VOC_BANDWIDTH_SWB 
        - #VSS_ISTREAM_EVS_VOC_BANDWIDTH_FB @tbend2 */

}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_istream_cmd_set_evs_voc_enc_operating_mode_t */

/** @ingroup cvd_cvs_cmd_set_evs_enc_channel_aware_mode_enable
  Enables the encoder channel aware mode for EVS

  @apr_msgpayload{vss_istream_cmd_set_evs_enc_channel_aware_mode_enable_t}
  @wktable{weak__vss__istream__cmd__set__evs_enc__channel__aware__mode__enable__t}

@detdesc
  This command will have no effect and will return an error when either:
  - The EVS vocoder is not being used by the stream
  - The payload parameters are invalid

  Channel aware mode operates only at 13.2kbps on WB or SWB bandwidths.

  It is recommended to enable channel aware mode before #VSS_ISTREAM_CMD_SET_EVS_VOC_ENC_OPERATING_MODE
  to start the feature before the bit rate changes to 13.2kbps.

  Valid payload parameters are specified in the payload descriptor.

  Please refer to 3GPP document TS 26.445 Version 12.0.0 Release 12 for more information
  about channel-aware mode.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q2,[Q2]}).

  @dependencies
  None.
*/
#define VSS_ISTREAM_CMD_SET_EVS_ENC_CHANNEL_AWARE_MODE_ENABLE ( 0x00013168 )

/* Type definition for vss_istream_cmd_set_evs_enc_channel_aware_mode_enable_t. */
typedef struct vss_istream_cmd_set_evs_enc_channel_aware_mode_enable_t vss_istream_cmd_set_evs_enc_channel_aware_mode_enable_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_istream_cmd_set_evs_enc_channel_aware_mode_enable_t
@{ */
/* Payload structure for the VSS_ISTREAM_CMD_SET_EVS_ENC_CHANNEL_AWARE_MODE_ENABLE command.
*/
struct vss_istream_cmd_set_evs_enc_channel_aware_mode_enable_t
{
   uint8_t fec_offset;
   /**< Specifies the forward-error correction offset.

       @values 2, 3, 5, 7 */

   uint8_t fer_rate;
   /**< Specifies FER rate threshold to LOW (0) or HIGH (1). */

}
#include "apr_pack_end.h"
;

/** @} */ /* end_weakgroup weak_vss_istream_cmd_set_evs_enc_channel_aware_mode_enable_t */

/** @ingroup cvd_cvs_cmd_set_evs_enc_channel_aware_mode_disable
  Disables the encoder channel aware mode for EVS

@detdesc
  This command will have no effect and will return an error when either the EVS vocoder is 
  not being used or if Channel Aware mode is already disabled.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q2,[Q2]}).

  @dependencies
  None.
*/
#define VSS_ISTREAM_CMD_SET_EVS_ENC_CHANNEL_AWARE_MODE_DISABLE ( 0x0001316C )

/** @endcond */

/****************************************************************************
 * DTMF GENERATION APIS                                                     *
 ****************************************************************************/

/** @cond OEM_only */
/** @addtogroup cvd_cvs_cmd_set_dtmf_generation 
@{ */
/**
  Starts and stops Dual-tone Multifrequency (DTMF) generation.

  @apr_msgpayload{vss_istream_cmd_set_dtmf_generation_t}
  @wktable{weak__vss__istream__cmd__set__dtmf__generation__t}

  @detdesc
  The completion of the DTMF generation (either due to a Stop command or
  because the requested duration has elapsed) is indicated to the client via
  the #VSS_ISTREAM_EVT_DTMF_GENERATION_ENDED event.
  @par
  A new DTMF generation command replaces a previously unfinished command in the
  same direction. If the generation of a single tone is needed, set the tone_1
  and tone_2 parameters to the same value. If the stream is to be closed before
  the requested tone duration has elapsed, first stop the tone that is
  currently playing, thus ensuring proper rampdown and avoiding audio
  artifacts (pops).

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q2,[Q2]}).

  @dependencies
  None.
*/
#define VSS_ISTREAM_CMD_SET_DTMF_GENERATION ( 0x00011023 )

/** DTMF generation path is Tx. */
#define VSS_ISTREAM_DTMF_GENERATION_DIRECTION_TX ( 0 )

/** Disables DTMF generation mixing. The generated DTMF replaces the speech. */
#define VSS_ISTREAM_DTMF_GENERATION_MIX_WITH_SPEECH_DISABLE ( 0 )

/** Enables DTMF generation mixing with the speech. */
#define VSS_ISTREAM_DTMF_GENERATION_MIX_WITH_SPEECH_ENABLE ( 1 )

/** @} */ /* end_addtogroup cvd_cvs_cmd_set_dtmf_generation */

/* Type definition for vss_istream_cmd_set_dtmf_generation_t. */
typedef struct vss_istream_cmd_set_dtmf_generation_t vss_istream_cmd_set_dtmf_generation_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_istream_cmd_set_dtmf_generation_t 
@{ */
/* Payload structure for the VSS_ISTREAM_CMD_SET_DTMF_GENERATION
    command.
*/
struct vss_istream_cmd_set_dtmf_generation_t
{
   uint16_t direction;
   /**< Specifies the path on which the DTMF tone is generated. Currently,
        only Tx is supported.
      
        @values #VSS_ISTREAM_DTMF_GENERATION_DIRECTION_TX */

   uint16_t mix_flag;
   /**< Specifies whether DTMF is mixed with a speech flag.
      
        @values
        - #VSS_ISTREAM_DTMF_GENERATION_MIX_WITH_SPEECH_DISABLE
        - #VSS_ISTREAM_DTMF_GENERATION_MIX_WITH_SPEECH_ENABLE @tbend2 */

   uint16_t tone_1;
   /**< Frequency of tone 1 for DTMF.
      
        @values 100 to 4000 Hz */

   uint16_t tone_2;
   /**< Frequency of tone 2 for DTMF.
      
        @values 100 to 4000 Hz */

   uint16_t gain;
   /**< DTMF tone linear gain. Because the level of tone generation is fixed
        at 0 dBFS, this parameter must be set to a negative value.
           
        @values Negative linear value in Q13 format */

   int16_t duration;
   /**< Duration of the tone with a 5 ms resolution. The duration includes
        ramp-up and ramp-down periods of 1 ms and 2 ms, respectively.

        @values
        - -1 -- Infinite duration
        -  0 -- Stop infinite tone
        - >0 -- Finite duration in milliseconds @tablebulletend */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup vss_istream_cmd_set_dtmf_generation_t */
/** @endcond */


/** @cond OEM_only */
/** @ingroup cvd_cvs_evt_dtmf_generation_ended
  Sent by the stream to the client that enabled DTMF generation, this event
  indicates that tone generation has ended either because the client issued a
  Stop command or the duration requested by the client has elapsed.

  @apr_msgpayload{vss_istream_evt_dtmf_generation_ended_t}
  @wktable{weak__vss__istream__evt__dtmf__generation__ended__t}

  @return
  None.

  @dependencies
  None.
*/
#define VSS_ISTREAM_EVT_DTMF_GENERATION_ENDED ( 0x00011032 )

/* Type definition for vss_istream_evt_dtmf_generation_ended_t. */
typedef struct vss_istream_evt_dtmf_generation_ended_t vss_istream_evt_dtmf_generation_ended_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_istream_evt_dtmf_generation_ended_t 
@{ */
/* Payload structure for the VSS_ISTREAM_EVT_DTMF_GENERATION_ENDED
    event.
*/
struct vss_istream_evt_dtmf_generation_ended_t
{
   uint16_t direction;
   /**< Specifies the path on which the DTMF tone was generated. Currently,
        only Tx is supported.
          
        @values #VSS_ISTREAM_DTMF_GENERATION_DIRECTION_TX */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_istream_evt_dtmf_generation_ended_t */
/** @endcond */


/****************************************************************************
 * DTMF DETECTION APIS                                                      *
 ****************************************************************************/

/** @cond OEM_only */
/** @addtogroup cvd_cvs_cmd_set_rx_dtmf_detect
@{ */
/**
  Configures DTMF detection on the Rx path.

  @apr_msgpayload{vss_istream_cmd_set_rx_dtmf_detection_t}
  @wktable{weak__vss__istream__cmd__set__rx__dtmf__detection__t}

  @detdesc
  The DTMF detection status is sent to the client sending this command via
  #VSS_ISTREAM_EVT_RX_DTMF_DETECTED events.
  @par
  Only one client can request Rx DTMF detection on a given stream at a time.
  When another client requests Rx DTMF detection while the previous client's
  Rx DTMF detection is still active, the request fails.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q2,[Q2]}).

  @dependencies
  A valid session must have been created.
*/
#define VSS_ISTREAM_CMD_SET_RX_DTMF_DETECTION ( 0x00011027 )

/** DTMF detection enable flag. */
#define VSS_ISTREAM_RX_DTMF_DETECTION_ENABLE ( 1 )

/** DTMF detection disable flag. */
#define VSS_ISTREAM_RX_DTMF_DETECTION_DISABLE ( 0 )

/** @} */ /* end_addtogroup cvd_cvs_cmd_set_rx_dtmf_detect */

/* Type definition for vss_istream_cmd_set_rx_dtmf_detection_t. */
typedef struct vss_istream_cmd_set_rx_dtmf_detection_t vss_istream_cmd_set_rx_dtmf_detection_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_istream_cmd_set_rx_dtmf_detection_t 
@{ */
/* Payload structure for the VSS_ISTREAM_CMD_SET_RX_DTMF_DETECTION
    command.
*/
struct vss_istream_cmd_set_rx_dtmf_detection_t
{
   uint32_t enable;
   /**< Specifies whether Rx DTMF detection is enabled.
           
        @values
        - #VSS_ISTREAM_RX_DTMF_DETECTION_ENABLE
        - #VSS_ISTREAM_RX_DTMF_DETECTION_DISABLE @tablebulletend */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup vss_istream_cmd_set_rx_dtmf_detection_t */
/** @endcond */


/** @cond OEM_only */
/** @ingroup cvd_cvs_evt_rx_dtmf_detected
  Sends a DTMF detection tone status to the client that enabled DTMF detection.

  @apr_msgpayload{vss_istream_evt_rx_dtmf_detected_t}
  @wktable{weak__vss__istream__evt__rx__dtmf__detected__t}

  @detdesc
  The stream sends this event whenever DTMF has been detected on the Rx path.
  @par
  The DTMF detection feature can only be used to detect DTMF frequencies as
  listed in Section&nbsp;6 of the ITU-T Recommendation Q.23 (refer to
  @xhyperref{S2,[S2]}). Therefore, the possible values for the low_freq and
  high_freq parameters are as listed in the message payload.

  @return
  None.

  @dependencies
  A valid session must have been created.
*/ /* S2 = Technical Features of Push-button Telephone Sets (ITU-T
   Recommendation Q.23)
*/
#define VSS_ISTREAM_EVT_RX_DTMF_DETECTED ( 0x0001101A )

/* Type definition for vss_istream_evt_rx_dtmf_detected_t. */
typedef struct vss_istream_evt_rx_dtmf_detected_t vss_istream_evt_rx_dtmf_detected_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_istream_evt_rx_dtmf_detected_t 
@{ */
/* Payload structure for the VSS_ISTREAM_EVT_RX_DTMF_DETECTED event.
*/
struct vss_istream_evt_rx_dtmf_detected_t
{
   uint16_t low_freq;
   /**< Low frequency detected in the DTMF tone.
      
        @values 697, 770, 852, 941 Hz */

   uint16_t high_freq;
   /**< Detected high frequency.
      
        @values 1209, 1336, 1477, 1633 Hz */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_istream_evt_rx_dtmf_detected_t */
/** @endcond */


/****************************************************************************
 * EAMR EVENTS                                                              *
 ****************************************************************************/

/** @cond OEM_only */
/** @ingroup cvd_cvs_evt_class_eamr_mode_chg
  Event class that identifies the Enhanced AMR (EAMR) mode change
  events. This class comprises one event: #VSS_ISTREAM_EVT_EAMR_MODE_CHANGED. 
*/
#define VSS_ISTREAM_EVENT_CLASS_EAMR_MODE_CHANGE ( 0x00012E58 )

/** @addtogroup cvd_cvs_evt_eamr_mode_chgd
@{ */
/**Event sent by the stream to notify the client that the EAMR mode has
 changed.

  @apr_msgpayload{vss_istream_evt_eamr_mode_changed_t}
  @wktable{weak__vss__istream__evt__eamr__mode__changed__t}

  @detdesc
  To receive this event, the client must first use
  #VSS_INOTIFY_CMD_LISTEN_FOR_EVENT_CLASS to register to listen for the
  #VSS_ISTREAM_EVENT_CLASS_EAMR_MODE_CHANGE event class with the CVS module.
  @par
  To stop receiving this event, the client must use
  #VSS_INOTIFY_CMD_CANCEL_EVENT_CLASS to cancel the
  VSS_ISTREAM_EVENT_CLASS_EAMR_MODE_CHANGE event class.

  @return
  None.

  @dependencies
  None.
*/
#define VSS_ISTREAM_EVT_EAMR_MODE_CHANGED ( 0x00012E59 )

/** EAMR narrowband mode. */
#define VSS_ISTREAM_EAMR_MODE_NARROWBAND ( 0 )

/** EAMR wideband mode. */
#define VSS_ISTREAM_EAMR_MODE_WIDEBAND ( 1 )

/** @} */ /* end_addtogroup cvd_cvs_evt_eamr_mode_chgd */

/* Type definition for vss_istream_evt_eamr_mode_changed_t. */
typedef struct vss_istream_evt_eamr_mode_changed_t vss_istream_evt_eamr_mode_changed_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_istream_evt_eamr_mode_changed_t
@{ */
/* Payload structure for the VSS_ISTREAM_EVT_EAMR_MODE_CHANGED event.
*/
struct vss_istream_evt_eamr_mode_changed_t
{
   uint32_t mode;
   /**< Specifies the EAMR mode.
   
        @values
        - #VSS_ISTREAM_EAMR_MODE_NARROWBAND
        - #VSS_ISTREAM_EAMR_MODE_WIDEBAND @tablebulletend */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_istream_evt_eamr_mode_changed_t */
/** @endcond */


/****************************************************************************
 * NOTIFICATIONS ISSUED BY THE STREAM TO ITS CLIENTS                        *
 ****************************************************************************/

/** @ingroup cvd_cvs_evt_ready
  Sent by the stream session to its client when streaming is active.

  @apr_msgpayload
  None.

  @return
  None.

  @dependencies
  None.
*/
#define VSS_ISTREAM_EVT_READY ( 0x000110FC )

/** @ingroup cvd_cvs_evt_not_ready
  Sent by the stream session to its client when streaming is inactive.

  @apr_msgpayload
  None.

  @return
  None.

  @dependencies
  None.
*/
#define VSS_ISTREAM_EVT_NOT_READY ( 0x000110FD )

#endif /* __VSS_ISTREAM_PUBLIC_IF_H__ */


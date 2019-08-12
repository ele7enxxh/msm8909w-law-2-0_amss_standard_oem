#ifndef __VSS_COMMON_PUBLIC_IF_H__
#define __VSS_COMMON_PUBLIC_IF_H__

/**
  @file vss_common_public_if.h
  @brief This file contains the APR API definitions that are common among
  modules.
*/

/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The description that displays in the PDF is located in the
      CVD_mainpage.dox file. Contact Tech Pubs for assistance.
=============================================================================*/
/*===========================================================================
Copyright (C) 2011, 2014-2016 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/api/cvd/vss_common_public_if.h#1 $
  $Author: mplcsds1 $

when        who    what, where, why
-------     ---    ----------------------------------------------------------
04/29/15    aim    (CVD) Updated with G711 V2 media ID's.
08/27/14    sw     (Tech Pubs) Updated with Doxygen comments from 2.0 and 2.2.

===========================================================================*/

#include "apr_comdef.h"

/*--------------------------------------------------------------------------*
 * MEDIA TYPE DEFINITIONS                                                   *
 *--------------------------------------------------------------------------*/

/** @addtogroup cvd_common_media_defs
@{ */
/** Default media type ID. */
#define VSS_MEDIA_ID_NONE ( 0x00010FC0 )

/** @cond OEM_only */
/** CDMA variable 13K vocoder format. */
#define VSS_MEDIA_ID_13K ( 0x00010FC1 )

/** CDMA Enhanced Variable Rate Vocoder (EVRC) format. */
#define VSS_MEDIA_ID_EVRC ( 0x00010FC2 )

/** CDMA 4th generation narrowband vocoder format. */
#define VSS_MEDIA_ID_4GV_NB ( 0x00010FC3 )

/** CDMA 4th generation wideband vocoder format. */
#define VSS_MEDIA_ID_4GV_WB ( 0x00010FC4 )

/** CDMA 4th generation narrow-wide vocoder format. */
#define VSS_MEDIA_ID_4GV_NW ( 0x00010FC5 )

/** CDMA 4th generation narrow-2ide-2K vocoder format. */
#define VSS_MEDIA_ID_4GV_NW2K ( 0x00010FD7 )

/** Universal Mobile Telecommunications System (UMTS) AMR-NB vocoder format. */
#define VSS_MEDIA_ID_AMR_NB ( 0x00010FC6 )

/** UMTS AMR-WB vocoder format. */
#define VSS_MEDIA_ID_AMR_WB ( 0x00010FC7 )

/** Enhanced AMR vocoder format. */
#define VSS_MEDIA_ID_EAMR ( 0x00010FD4 )

/** GSM enhanced full rate vocoder format. */
#define VSS_MEDIA_ID_EFR ( 0x00010FC8 )

/** GSM full rate vocoder format. */
#define VSS_MEDIA_ID_FR ( 0x00010FC9 )

/** GSM half rate vocoder format. */
#define VSS_MEDIA_ID_HR ( 0x00010FCA )

/** Enhanced Voice Services (EVS) vocoder format. */
#define VSS_MEDIA_ID_EVS ( 0x00010FD8 )

/** @endcond */

/** Linear Pulse Code Modulation (PCM) vocoder format; mono, 16 bits,
    little endian, 8 kHz. */
#define VSS_MEDIA_ID_PCM_8_KHZ ( 0x00010FCB )

/** Linear PCM vocoder format; mono, 16 bits, little endian, 16 kHz. */
#define VSS_MEDIA_ID_PCM_16_KHZ ( 0x00010FCC )

/** Linear PCM vocoder format; mono, 16 bits, little endian, 44.1 kHz. */
#define VSS_MEDIA_ID_PCM_44_1_KHZ ( 0x00010FD5 )

/** Linear PCM vocoder format; mono, 16 bits, little endian, 48 kHz. */
#define VSS_MEDIA_ID_PCM_48_KHZ ( 0x00010FD6 )

/** @cond OEM_only */

/** G.711 a-law V2 vocoder format; contains 20 ms vocoder frame. */
#define VSS_MEDIA_ID_G711_ALAW_V2 ( 0x000131A1 )

/** G.711 mu-law V2 vocoder format; contains 20 ms vocoder frame. */
#define VSS_MEDIA_ID_G711_MULAW_V2 ( 0x000131A2 )

/** G.729AB vocoder format; contains two 10 ms vocoder frames. Refer to
    Figure 4 in @xhyperref{S1,[S1]}.
*/ /* S1 = RTP Profile for Audio and Video Conferences with Minimal Control,
           RFC 3551 (07/2003)
*/
#define VSS_MEDIA_ID_G729 ( 0x00010FD0 )

/** G.722 vocoder format; contains one 20 ms vocoder frame. */
#define VSS_MEDIA_ID_G722 ( 0x00010FD1 )

/** @endcond */

/** @} */ /* end_addtogroup cvd_common_media_defs */


/****************************************************************************
 * CALIBRATION FRAMEWORK DEFINITIONS                                        *
 ****************************************************************************/

/* Order of Applying Multiple Matching Calibration Entries*******************/
/** @weakgroup weakf_order_multi_match_cal_entries
  A system configuration is determined by the network, Tx and Rx
  Pre/Postprocessing (PP) sampling rates, and so on.
  When multiple matching calibration entries in the table must
  be applied to a system configuration, the order in which the matching entries
  are applied is determined using a rule called Specific Overwirtes N/A, which
  is explained as follows.

  @par Specific Overwrites N/A rule
  A matching calibration entry that contains a non-N/A value in a column is
  applied after all other matching entries that contain N/A values in the same
  column.
  @par
  For all matching entries found during calibration application, this rule is
  applied in reverse order of the columns that were registered. 
  Starting with the last column, the
  Specific Overwrites N/A rule is applied on all matching entries. Then the
  rule is applied on the next-to-last column, and so on.
  @par
  Thus, if N columns are provided as part of the calibration registration,
  start from the Nth column:
    -# For all matching entries that contain the non-N/A value in this
       column, apply the Specific Overwrites N/A rule.
    -# After the rule is applied, the matching entries are separated into two
       groups: one group with non-N/A values (e.g., group A) and one group with
       N/A values (e.g., group B). Then the group order is determined (e.g., group A
       is applied after group B).
    -# For each group, apply the rule on the preceding (Nth-1) column.
       Again, two groups are created and their order is determined.       
       This process continues on all preceding columns until the calibration
       order for all matching entries is determined.

  @par Example
  Assumptions:
  - The following is the order of the columns in the dynamic calibration
    table registered with a specific vocproc session:
     -# #VSS_ICOMMON_CAL_COLUMN_NETWORK
     -# #VSS_ICOMMON_CAL_COLUMN_TX_PP_SAMPLING_RATE
     -# #VSS_ICOMMON_CAL_COLUMN_RX_PP_SAMPLING_RATE
     -# #VSS_ICOMMON_CAL_COLUMN_RX_VOLUME_INDEX
     -# #VSS_ICOMMON_CAL_COLUMN_TX_VOC_OPERATING_MODE
     -# #VSS_ICOMMON_CAL_COLUMN_RX_VOC_OPERATING_MODE
     -# #VSS_ICOMMON_CAL_COLUMN_MEDIA_ID
     -# #VSS_ICOMMON_CAL_COLUMN_FEATURE
  - The N/A column value for column 2
    (VSS_ICOMMON_CAL_COLUMN_TX_PP_SAMPLING_RATE) is 0. @newpage
  - Three matching entries are found in the vocproc dynamic calibration table
    based on the current system configuration, and they must be applied. Each
    matching entry is represented by a composition of column values for the
    columns listed above.
     -# Calibration entry 1 is composed by:
@code
<
 #VSS_ICOMMON_CAL_NETWORK_ID_WCDMA,
 0 (i.e. N/A Tx PP sampling rate),
 16000 (i.e. 16000 Hz Rx PP sampling rate),
 1 (i.e. volume index 1),
 VSS_ICOMMON_CAL_COLUMN_VOC_OPERATING_MODE_NONE,
 VSS_ICOMMON_CAL_COLUMN_VOC_OPERATING_MODE_NONE,
 VSS_MEDIA_ID_NONE,
 VSS_ICOMMON_CAL_FEATURE_NONE
>

@endcode
     -# Calibration entry 2 is composed by:
@code
<
 VSS_ICOMMON_CAL_NETWORK_ID_WCDMA,
 0,
 16000,
 1,
 VSS_ICOMMON_CAL_COLUMN_VOC_OPERATING_MODE_NONE,
 VSS_ICOMMON_CAL_COLUMN_VOC_OPERATING_MODE_NB,
 VSS_MEDIA_ID_EAMR,
 VSS_ICOMMON_CAL_FEATURE_NONE
>

@endcode
     -# Calibration entry 3 is composed by:
@code
<
 #VSS_ICOMMON_CAL_NETWORK_ID_WCDMA,
 0,
 16000,
 1,
 VSS_ICOMMON_CAL_COLUMN_VOC_OPERATING_MODE_NONE,
 VSS_ICOMMON_CAL_COLUMN_VOC_OPERATING_MODE_NB,
 VSS_MEDIA_ID_NONE,
 VSS_ICOMMON_CAL_FEATURE_BEAMR
>

@endcode
  @par
  The Specific Overwrites N/A rule is first applied on column 8
  (VSS_ICOMMON_CAL_COLUMN_FEATURE). As a result, calibration entry 3
  is applied after calibration entries 1 and 2 are applied.
  @par
  Next, to determine the order of applying calibration entries 1 and 2, the
  Specific Overwrites N/A rule is applied on column 7
  (VSS_ICOMMON_CAL_COLUMN_MEDIA_ID). As a result, calibration entry 2
  is applied after calibration entry 1 is applied.
  @par
  Therefore, the final order in which the three calibration entries are
  applied is:
  -# Calibration entry 1 is applied first.
  -# Calibration entry 2 is applied next.
  -# Calibration entry 3 is applied last.

*****************************************************************************/


/* Calibration Data Format **************************************************/
/** @weakgroup weakf_calib_data_format
  The client provides shared memory that contains calibration data when
  registering calibration data with CVD. The calibration data is in a
  specific format, which is specified as follows. 

  The calibration data buffer contains the following fields at its beginning:

  - Major version -- Calibration data format major version. The currently
    supported major version is 1.
  - Minor version -- Calibration data format minor version. The currently
    supported minor version is 0.

  In Version 1.0 (major version 1, minor version 0) of the calibration data
  format, the calibration data is represented by four tables:

  - Lookup table (LUT) -- Contains the calibration profiles, where each profile
    comprises a list of column values along with a byte offset to the
    content definition table as well as a byte offset to the content data
    offsets table.
  - Content Definition Table (CDFT) -- Contains the lists of <mid, pid> (Module
    ID, Parameter ID) pairs for calibration profiles in the LUT.
  - Content Data Offsets Table (CDOT) -- Contains the location (offsets in
    bytes) of the data table where the calibration data resides for each
    <mid, pid> pair for the lists of <mid, pid> pairs in the CDFT.
  - Data table -- Contains calibration data parameters.

  @par Expected data format
@code
   ---------------------------  --> Calibration data buffer start
  | Major version             | --> This field is 32 bits.
   ---------------------------
  | Minor version             | --> This field is 32 bits.
   --------------------------- 
  | Size in bytes of the LUT  | --> This field is 32 bits.
  | that follows              |
   ---------------------------  --> This is the beginning of the LUT.
  | Number of LUT entries     | --> This field is 32 bits.
   ---------------------------  --> Entry 1 start
  | column_1                  | --> eg. network_id
   ---------------------------
  | column_2                  | --> eg. tx_pp_sampling_rate
   ---------------------------
  |  .                        |
  |  .                        |
   ---------------------------
  | column_n                  |
   ---------------------------
  | CDFT OFFSET               | --> Byte offset from the beginning of the CDFT.
  |                           |     This field is 32 bits.
   ---------------------------
  | CDOT OFFSET               | --> Byte offset from the beginning of CDOT.
  |                           |     This field is 32 bits.
   ---------------------------  --> Entry 2 start
  | column_1                  |
   ---------------------------
  | column_2                  |
  |  .                        |
  |  .                        |
  |  ...                      |
   --------------------------- 
  | Size in bytes of the CDFT | --> This field is 32 bits.
  | that follows              |
   ---------------------------  --> Entry 1 start (beginning of the CDFT).
  | Number of <mid, pid> pairs| --> This field is 32 bits.
   ---------------------------
  | mod_id                    | --> This field is 32 bits.
   ---------------------------
  | param_id                  | --> This field is 32 bits.
   ---------------------------
  | mod_id                    |
   ---------------------------
  | param_id                  |
  |                           |
  |                           |
  | ...                       |
   ---------------------------  --> Entry 2 start
  | Number of <mid, pid> pairs|
   ----------------------------
  | mod_id                    |
   ----------------------------
  | param_id                  |
   ----------------------------
  | mod_id                    |
   ----------------------------
  | param_id                  |
  |                           |
  |                           |
  | ...                       |
   ---------------------------- 
  | Size in bytes of the CDOT | --> This field is 32 bits.
  | that follows              |
   ---------------------------  --> Entry 1 start (beginning of the CDOT).
  | Number of data offset     | --> This field is 32 bits.
  | entries                   |
   ----------------------------
  | data table offset         | --> This field is 32 bits.
   ----------------------------
  | data table offset         |
   ----------------------------
  |                           |
  |                           |
  | ...                       |
   ---------------------------  --> Entry 2 start
  | Number of data offset     |
  | entries                   |
   ---------------------------
  | data table offset         |
   ---------------------------
  | data table offset         |
   ---------------------------
  |                           |
  |                           |
  | ...                       |
   --------------------------- 
  | Size in bytes of the data | --> This field is 32 bits.
  | table that follows        |
   ---------------------------  --> Entry 1 start (beginning of the data table).
  | data_size                 | --> Size in bytes of the calibration data for
  |                           |     a specific <mid, pid> pair. This field is 32 bits.
   ---------------------------
  | data                      | --> Calibration data for a specific <mid, pid>
  |                           |     pair. This field is a multiple of 4 bytes.
  |                           |
  |                           |
  |  ...                      |
   ---------------------------  --> Entry 2 start
  | data_size                 |
   ---------------------------
  | data                      |
  |                           |
  |                           |
  | ...                       |
@endcode
*****************************************************************************/


/** @addtogroup cvd_common_module_cal_framework 
@{ */
/*--------------------------------------------------------------------------*
 * CALIBRATION COLUMN IDS                                                   *
 *--------------------------------------------------------------------------*/

/** @name Calibration columns
    @xreflabel{hdr:CalibrationColumns}
@{ */

/** Calibration column for the network ID (listed above in
  @xhyperref{hdr:CalNetworkID,Network ID column values}). */
#define VSS_ICOMMON_CAL_COLUMN_NETWORK ( 0x00011350 )

/** Calibration column for the Rx pre/postprocessing sampling rate. */
#define VSS_ICOMMON_CAL_COLUMN_RX_PP_SAMPLING_RATE ( 0x00011351 )

/** Calibration column for the Tx pre/postprocessing sampling rate. */
#define VSS_ICOMMON_CAL_COLUMN_TX_PP_SAMPLING_RATE ( 0x00011352 )

/** Calibration column for the Rx volume index.

  Given a specific set of system configurations (composed by network, Tx and
  Rx PP sampling rates, feature, etc., but excluding the Rx volume index), the
  list of <mid, pid> pairs to be calibrated must be the same between the
  different Rx volume indices (except the N/A Rx volume index). The Rx
  volume calibration data is placed in the dynamic calibration data table.
*/
#define VSS_ICOMMON_CAL_COLUMN_RX_VOLUME_INDEX ( 0x00011358 )

/** Calibration column for the media ID (see Section @xref{dox:MediaDefinitions}).
*/
#define VSS_ICOMMON_CAL_COLUMN_MEDIA_ID ( 0x00013081 )

/** Calibration column for the Tx vocoder operating mode
  (see @xhyperref{hdr:CalVocClassCol,Vocoder class column values}).

  The Tx operating mode for some vocoders can change dynamically during
  runtime. Given a specific set of system configurations (composed by network,
  Tx and Rx PP sampling rates, feature, etc., but excluding the Tx vocoder
  operating mode), the list of <mid, pid> pairs to be calibrated must be the
  same between the different Tx vocoder operating modes (except the N/A Tx
  vocoder operating mode). The Tx vocoder operating mode-specific calibration
  data is placed in the dynamic calibration data table. @newpage
*/
#define VSS_ICOMMON_CAL_COLUMN_TX_VOC_OPERATING_MODE ( 0x00013082 )

/** Calibration column for the Rx vocoder operating mode
  (see @xhyperref{hdr:CalVocClassCol,Vocoder class column values}).

  The Rx operating mode for some vocoders can change dynamically during
  runtime. Given a specific set of system configurations (composed by network,
  Tx and Rx PP sampling rates, feature, etc., but excluding the Rx vocoder
  operating mode), the list of <mid, pid> pairs to be calibrated must be the
  same between the different Rx vocoder operating modes (except the N/A Rx
  vocoder operating mode). The Rx vocoder operating mode specific calibration
  data is placed in the dynamic calibration data table.
*/
#define VSS_ICOMMON_CAL_COLUMN_RX_VOC_OPERATING_MODE ( 0x00013083 )

/** Calibration column for the feature
  (see @xhyperref{hdr:CalColFeature,Calibration features}).

  Some features can be enabled or disabled dynamically during runtime.
  Given a specific set of system configurations (composed by network, Tx and
  Rx PP sampling rates, feature, etc., but excluding the feature), the list of
  <mid, pid> pairs to be calibrated must be the same when a feature is
  enabled and when a feature is disabled. The feature specific calibration
  data is placed in the dynamic calibration data table.
*/
#define VSS_ICOMMON_CAL_COLUMN_FEATURE ( 0x00013084 )

/** @} */ /* end_name_group Calibration columns */

/*--------------------------------------------------------------------------*
 * CALIBRATION COLUMN TYPES                                                 *
 *--------------------------------------------------------------------------*/

/** @name Calibration column types
    @xreflabel{hdr:CalibrationColumnTypes}
@{ */

/** 8-bit calibration column data type. */
#define VSS_ICOMMON_CAL_COLUMN_TYPE_UINT8 ( 0x0001135A )

/** 16-bit calibration column data type. */
#define VSS_ICOMMON_CAL_COLUMN_TYPE_UINT16 ( 0x0001135B )

/** 32-bit calibration column data type. */
#define VSS_ICOMMON_CAL_COLUMN_TYPE_UINT32 ( 0x0001135C )

/** 64-bit calibration column data type. */
#define VSS_ICOMMON_CAL_COLUMN_TYPE_UINT64 ( 0x0001135D )

/** @} */ /* end_name_group Column types */

/*--------------------------------------------------------------------------*
 * NETWORK ID COLUMN VALUES                                                 *
 *--------------------------------------------------------------------------*/

/** @name Network ID column values
    @xreflabel{hdr:CalNetworkID}
@{ */

/** Network ID is not applicable. */
#define VSS_ICOMMON_CAL_NETWORK_ID_NONE ( 0x0001135E )

/** CDMA network ID. */
#define VSS_ICOMMON_CAL_NETWORK_ID_CDMA ( 0x0001135F )

/** GSM network ID. */
#define VSS_ICOMMON_CAL_NETWORK_ID_GSM ( 0x00011360 )

/** WCDMA network ID. */
#define VSS_ICOMMON_CAL_NETWORK_ID_WCDMA ( 0x00011361 )

/** Voice over Internet Protocol (VoIP) network ID. */
#define VSS_ICOMMON_CAL_NETWORK_ID_VOIP ( 0x00011362 )

/** LTE network ID. */
#define VSS_ICOMMON_CAL_NETWORK_ID_LTE ( 0x00011363 )

/** @} */ /* end_name_group Network ID column values */

/*--------------------------------------------------------------------------*
 * TX/RX VOCODER OPERATING MODE COLUMN VALUES                               *
 *--------------------------------------------------------------------------*/

/** @name Vocoder class column values
    @xreflabel{hdr:CalVocClassCol}
@{ */
/** Vocoder operating mode is not applicable. */
#define VSS_ICOMMON_CAL_VOC_OPERATING_MODE_NONE ( 0x00013085 )

/** Common narrowband vocoder operating mode. Represents audio bandwidth up to
    4 kHz.
*/
#define VSS_ICOMMON_CAL_VOC_OPERATING_MODE_NB ( 0x00013086 )

/** Common wideband vocoder operating mode. Represents audio bandwidth up to
    8 kHz.
*/
#define VSS_ICOMMON_CAL_VOC_OPERATING_MODE_WB ( 0x00013087 )

/** Common super wideband vocoder operating mode. Represents audio bandwidth
    up to 16 kHz.
*/
#define VSS_ICOMMON_CAL_VOC_OPERATING_MODE_SWB ( 0x00013088 )

/** Common fullband vocoder operating mode. Represents audio bandwidth up to
    24 kHz.
*/
#define VSS_ICOMMON_CAL_VOC_OPERATING_MODE_FB ( 0x00013089 )

/** @} */ /* end_name_group Vocoder class column values */

/*--------------------------------------------------------------------------*
 * FEATURE COLUMN VALUES                                                    *
 *--------------------------------------------------------------------------*/

/** @name Calibration features
    @xreflabel{hdr:CalColFeature}
@{ */
/** Feature is not applicable. */
#define VSS_ICOMMON_CAL_FEATURE_NONE ( 0x0001308A )

/** WideVoice feature. */
#define VSS_ICOMMON_CAL_FEATURE_WIDEVOICE ( 0x0001308B )

/** Blind Extended AMR feature. */
#define VSS_ICOMMON_CAL_FEATURE_BEAMR ( 0x0001308C )

/** WideVoice 2.0 feature. */
#define VSS_ICOMMON_CAL_FEATURE_WIDEVOICE2 ( 0x000130A7 )

/** @} */ /* end_name_group Calibration features */

/****************************************************************************
 * STREAM/VOCPROC CALIBRATION APIS                                          *
 ****************************************************************************/

/* Type definition for vss_icommon_param_data_t. */
typedef struct vss_icommon_param_data_t vss_icommon_param_data_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_icommon_param_data_t 
@{ */
/* Parameter data structure used by the VSS_ICOMMON_CMD_SET_PARAM_V2
   VSS_ICOMMON_RSP_GET_PARAM commands. */
/** When parameter data is received in-band, immediately following this
    structure are param_size bytes of uint8_t param_data[param_size].
    The structure and size depend on the module_id/param_id pair.
*/
struct vss_icommon_param_data_t
{
  uint32_t module_id;
  /**< Valid ID of the module.

       @values Refer to @xhyperref{Q4,[Q4]} */
  /* Q4 = Hex Multimedia: Voice API Interface Spec for 2.4 (80-NF769-2) */

  uint32_t param_id;
  /**< Valid ID of the parameter.

       @values Refer to @xhyperref{Q4,[Q4]} */

  uint16_t param_size;
  /**< Data size of the param_id/module_id combination.

       @values > 0 bytes */

  uint16_t reserved;
  /**< This field must be set to zero. */

#if 0
  uint8_t param_data[param_size];
  /**< Parameter data when received in-band. @newpagetable */

  /* Additional instances of the module_id, param_id, param_size,
     reserved sequence can continue below:
         uint32_t module_id;
         .
         .
         . */
#endif /* 0 */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_icommon_param_data_t */


/** @ingroup cvd_common_cmd_set_param
  Sets one or more calibration parameters.

  @note1hang For guidelines on using shared memory, see
             Section @xref{hdr:ShMemGuidelines}.

  @apr_msgpayload{vss_icommon_cmd_set_param_v2_t}
  @wktablens{weak__vss__icommon__cmd__set__param__v2__t}

  @par Parameter data variable payload (vss_icommon_param_data_t)
  @wktablens{weak__vss__icommon__param__data__t}

  @detdesc
  This command is intended for private use. Clients must not use this command
  for calibration or UI control purposes.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q2,[Q2]}).

  @dependencies
  If the parameter data is out-of-band, the memory that holds the data must
  first be mapped with the CVD by following the guidelines in
  Section @xref{hdr:ShMemGuidelines}.
  @par
  The parameter data in the memory must be valid and read only until a
  response to this command is received.
*/ /* Q2 = Async Packet Router API for MDM9x15 Interface Spec (80-N1463-2)
*/
#define VSS_ICOMMON_CMD_SET_PARAM_V2 ( 0x0001133D )

/* Type definition for vss_icommon_cmd_set_param_v2_t. */
typedef struct vss_icommon_cmd_set_param_v2_t vss_icommon_cmd_set_param_v2_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_icommon_cmd_set_param_v2_t 
@{ */
/* Payload structure for the VSS_ICOMMON_CMD_SET_PARAM_V2 command.
*/
/** When parameter data is in-band (mem_handle = 0), immediately following
    this structure are mem_size bytes of %vss_icommon_param_data_t param_data[].
*/
struct vss_icommon_cmd_set_param_v2_t
{
  uint32_t mem_handle;
    /**< If the parameter data is in-band, set this field to 0. If the
         parameter data is out-of-band, this field is the handle to the shared
         memory that holds the parameter data.

         @values
         - 0 -- Parameter data payloads are within the message payload
                (in-band)
         - Otherwise -- Pointer to the physical address in shared memory

         If the payload address is in-band, the <b>Parameter data variable
         payload</b> begins at the specified data payload address. */

  uint64_t mem_address;
    /**< Location of the parameter data payload.

         The payload is an array of %vss_icommon_param_data_t. If the
         mem_handle is 0, this field is ignored. */

  uint32_t mem_size;
    /**< Size of the parameter data payload in bytes.
         This field is applicable to both in-band and out-of-band parameter
         data.

         For an out-of-band parameter, the amount of memory allocated at
         mem_address must meet the requirement as specified in
         Section&nbsp;@xref{hdr:ShMemGuidelines}. */

#if 0
  vss_icommon_param_data_t param_data[];
  /**< Parameter data is present when the data is in-band. */
#endif /* 0 */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_icommon_cmd_set_param_t */


/** @ingroup cvd_common_cmd_get_param
  Gets a calibration parameter. 

  @note1hang For guidelines on using shared memory, see
             Section @xref{hdr:ShMemGuidelines}.

  @apr_msgpayload{vss_icommon_cmd_get_param_v2_t}
   @wktable{weak__vss__icommon__cmd__get__param__v2__t}

  @detdesc
  This function is intended for private use. Clients must not use this function
  for calibration or UI control purposes.

  @return
  #VSS_ICOMMON_RSP_GET_PARAM -- Success, invalid module ID, invalid
  parameter ID.
  @par
  APRV2_IBASIC_RSP_RESULT -- Any failure other than an invalid module
  ID or invalid parameter ID (refer to @xhyperref{Q2,[Q2]}). @newpage

  @dependencies
  If the parameter data is out-of-band, the memory that holds the data must
  first be mapped with the CVD by following the guidelines in
  Section @xref{hdr:ShMemGuidelines}.
  @par
  Clients must not modify the memory and the memory must be valid and writable
  until a response to this command is received.
*/
#define VSS_ICOMMON_CMD_GET_PARAM_V2 ( 0x0001133E )

/* Type definition for vss_icommon_cmd_get_param_t. */
typedef struct vss_icommon_cmd_get_param_v2_t vss_icommon_cmd_get_param_v2_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_icommon_cmd_get_param_v2_t 
@{ */
/* Payload structure for the VSS_ICOMMON_CMD_GET_PARAM_V2 command.
*/
struct vss_icommon_cmd_get_param_v2_t
{
  uint32_t mem_handle;
    /**< If the parameter data is in-band, set this field to 0. If the
         parameter data is out-of-band, this field is the handle to the shared
       memory that holds the parameter data.

       @values
       - 0 -- Parameter data is in-band
       - Otherwise -- Pointer to the physical address in shared memory
       @tablebulletend */

  uint64_t mem_address;
  /**< Location to hold the parameter data.

       A single %vss_icommon_param_data_t that contains the header and the
       algorithm's parameter data is placed at this location. If the
       mem_handle is 0, this field is ignored. */

  uint16_t mem_size;
  /**< Size of the memory in bytes to hold the parameter.

       This field is applicable to both in-band and out-of-band parameters.
       The size must be large enough to hold the algorithm's parameter data
       plus the header %vss_icommon_param_data_t.

       For an out-of-band parameter, the size must also meet the requirement
       as specified in Section @xref{hdr:ShMemGuidelines}. */

  uint32_t module_id;
  /**< Valid ID of the module).

       @values Refer to @xhyperref{Q4,[Q4]} */

  uint32_t param_id;
  /**< Valid ID of the parameter.

       @values Refer to @xhyperref{Q4,[Q4]} */ 
	   
	   /* ?NOT BW COMP? NO PARAM_SIZE field? */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup vss_icommon_cmd_get_param_t */


/** @ingroup cvd_common_resp_get_param
  Response to the #VSS_ICOMMON_CMD_GET_PARAM_V2 command.

  @apr_msgpayload{vss_icommon_rsp_get_param_t}
  @wktablens{weak__vss__icommon__rsp__get__param__t}

  @return
  None.

  @dependencies
  None.
*/
#define VSS_ICOMMON_RSP_GET_PARAM ( 0x00011008 )

/* Type definition for vss_icommon_rsp_get_param_t. */
typedef struct vss_icommon_rsp_get_param_t vss_icommon_rsp_get_param_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_icommon_rsp_get_param_t 
@{ */
/* Payload structure for the VSS_ICOMMON_RSP_GET_PARAM command
   response. */
/** When parameter data (%vss_icommon_param_data_t) is in-band, it immediately
    follows the status field.
*/
struct vss_icommon_rsp_get_param_t
{
  uint32_t status;
  /**< Status message (error code). For more information, refer to
       @xhyperref{Q2,[Q2]}.
    
       @values
       - 0 -- Success
       - 1 -- Invalid module ID
       - 2 -- Invalid parameter ID @tablebulletend */

#if 0
  vss_icommon_param_data_t param_data[1];
  /**< Parameter data is present when the data is in-band. */
#endif /* 0 */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_icommon_rsp_get_param_t */


/****************************************************************************
 * STREAM/VOCPROC UI PROPERTY APIS                                          *
 ****************************************************************************/

/** @ingroup cvd_common_cmd_set_ui_prop
  Sets a UI property.

  @apr_msgpayload{vss_icommon_cmd_set_ui_property_t}
  @wktablens{weak__vss__icommon__cmd__set__ui__property__t}

  @detdesc
  This command caches specific UI settings like volume, mute, etc.
  Calibration and other controls are not to use this command.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q2,[Q2]}).

  @dependencies
  None.
*/
#define VSS_ICOMMON_CMD_SET_UI_PROPERTY ( 0x00011103 )

/* Type definition for vss_icommon_cmd_set_ui_property_t. */
typedef struct vss_icommon_cmd_set_ui_property_t vss_icommon_cmd_set_ui_property_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_icommon_cmd_set_ui_property_t 
@{ */
/* Payload structure for the VSS_ICOMMON_CMD_SET_UI_PROPERTY command.
*/
/** When parameter data is sent in-band, immediately following this structure
    are param_size bytes of uint8_t param_data[param_size].
    The structure and size depend on the module_id/param_id pair.
*/
struct vss_icommon_cmd_set_ui_property_t
{
  uint32_t module_id;
  /**< Valid ID of the module.

       @values Refer to @xhyperref{Q4,[Q4]} */

  uint32_t param_id;
  /**< Valid ID of the parameter.

       @values Refer to @xhyperref{Q4,[Q4]} */

  uint16_t param_size;
  /**< Data size of the param_id/module_id combination.

       @values > 0 bytes */

  uint16_t reserved;
  /**< This field must be set to zero. */

#if 0
  uint8_t param_data[param_size];
    /**< Parameter data sent in-band. */
#endif /* 0 */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_icommon_cmd_set_ui_property_t */


/** @ingroup cvd_common_cmd_get_ui_prop
  Gets a UI property.

  @apr_msgpayload{vss_icommon_cmd_get_ui_property_t}
  @wktable{weak__vss__icommon__cmd__get__ui__property__t}

  @return
  #VSS_ICOMMON_RSP_GET_UI_PROPERTY -- Success; the UI property was retrieved.
  @par
  APRV2_IBASIC_RSP_RESULT -- Failure; the UI property was not retrieved (refer to
  @xhyperref{Q2,[Q2]}).

  @dependencies
  None.
*/
#define VSS_ICOMMON_CMD_GET_UI_PROPERTY ( 0x00011104 )

/* Type definition for vss_icommon_cmd_get_ui_property_t. */
typedef struct vss_icommon_cmd_get_ui_property_t vss_icommon_cmd_get_ui_property_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_icommon_cmd_get_ui_property_t 
@{ */
/* Payload structure for the VSS_ICOMMON_CMD_GET_UI_PROPERTY command.
*/
struct vss_icommon_cmd_get_ui_property_t
{
  uint32_t module_id;
  /**< Valid ID of the module.

       @values Refer to @xhyperref{Q4,[Q4]} */

  uint32_t param_id;
  /**< Valid ID of the parameter

       @values Refer to @xhyperref{Q4,[Q4]} */

  uint32_t param_size;
  /**< Data size of the param_id/module_id combination.

       @values > 0 bytes */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_icommon_cmd_get_ui_property_t */


/** @ingroup cvd_common_resp_get_ui_prop
  Response to the #VSS_ICOMMON_CMD_GET_UI_PROPERTY command.

  @apr_msgpayload{vss_icommon_rsp_get_ui_property_t}
  @wktablens{weak__vss__icommon__rsp__get__ui__property__t}

  @return
  None.

  @dependencies
  None.
 */
#define VSS_ICOMMON_RSP_GET_UI_PROPERTY ( 0x00011105 )

/* Type definition for vss_icommon_rsp_get_ui_property_t. */
typedef struct vss_icommon_rsp_get_ui_property_t vss_icommon_rsp_get_ui_property_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_icommon_rsp_get_ui_property_t 
@{ */ 
/* Payload structure for the VSS_ICOMMON_RSP_GET_UI_PROPERTY command
    response. */
/** When a UI property is received in-band, it immediately follows the reserved
    field, and it's size is indicated in param_size.
*/
struct vss_icommon_rsp_get_ui_property_t
{
  uint32_t status;
    /**< Status of the GET_UI_PROPERTY command.
    
       @values
       - 0 -- Success
       - 1 -- Invalid module ID
       - 2 -- Invalid parameter ID @tablebulletend */

  uint32_t module_id;
  /**< Valid ID of the module 

       @values Refer to @xhyperref{Q4,[Q4]} */

  uint32_t param_id;
  /**< Valid ID of the parameter

       @values Refer to @xhyperref{Q4,[Q4]} */

  uint16_t param_size;
  /**< Data size of the param_id/module_id combination.

       @values > 0 bytes */

  uint16_t reserved;
  /**< This field must be set to zero. */

#if 0
  uint8_t param_data[param_size];
    /**< Parameter data received in-band. */
#endif /* 0 */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_icommon_rsp_get_ui_property_t */

/** @} */ /* end_addtogroup cvd_common_module_cal_framework */


/****************************************************************************
 * VOICE TIMING APIS                                                        *
 ****************************************************************************/

/** @addtogroup cvd_common_cmd_set_voice_tmg
@{ */
/**Sets the voice timing parameters and Vocoder Frame Reference (VFR).
  This command is for full control stream clients only. @vertspace{-9}

  @apr_msgpayload{vss_icommon_cmd_set_voice_timing_v2_t}
  @wktable{weak__vss__icommon__cmd__set__voice__timing__v2__t}

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q2,[Q2]}).
  @par
  Offset values greater than 20000 microseconds result in failures.

  @dependencies
  None. @vertspace{-6}
*/
#define VSS_ICOMMON_CMD_SET_VOICE_TIMING_V2 ( 0x00012E66 )

/** Soft VFR mode flag that indicates the voice data path is driven
    by the software timer reference. */
#define VSS_ICOMMON_VFR_MODE_SOFT ( 0 )

/** Hard VFR mode flag that indicates the voice data path is driven
    by hardware interrupts aligned to the network timing reference. */
#define VSS_ICOMMON_VFR_MODE_HARD ( 1 )

/** @} */ /* end_addtogroup cvd_common_cmd_set_voice_tmg */

/* Type definition for vss_icommon_cmd_set_voice_timing_v2_t. */
typedef struct vss_icommon_cmd_set_voice_timing_v2_t vss_icommon_cmd_set_voice_timing_v2_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_icommon_cmd_set_voice_timing_v2_t 
@{ */
/* Payload structure for the VSS_ICOMMON_CMD_SET_VOICE_TIMING command.
*/
struct vss_icommon_cmd_set_voice_timing_v2_t
{
  uint16_t mode;
  /**< Vocoder frame synchronization mode.
    
       @values
       - #VSS_ICOMMON_VFR_MODE_SOFT -- No frame synchronization
       - #VSS_ICOMMON_VFR_MODE_HARD -- Hard VFR; a 20 ms VFR interrupt
         @tablebulletend */

  uint32_t vsid;
  /**< Voice System ID as defined by @xhyperref{Q5,[Q5]}. */
  /* Q5 = Voice System ID (VSID) Definition and Use Cases (80-NF711-1) */

  uint16_t enc_offset;
    /**< Offset in microseconds from the VFR to deliver a Tx vocoder
       packet.

       @values < 20000 microseconds. */

  uint16_t dec_req_offset;
  /**< Offset in microseconds from the VFR to request for an Rx vocoder
       packet.

       @values < 20000 microseconds. */

  uint16_t dec_offset;
    /**< Offset in microseconds from the VFR to indicate the deadline to
       receive an Rx vocoder packet.

       @values < 20000 microseconds.

         Rx vocoder packets received after this deadline are not guaranteed to
         be processed.  */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_icommon_cmd_set_voice_timing_v2_t */


#endif /* __VSS_COMMON_PUBLIC_IF_H__ */


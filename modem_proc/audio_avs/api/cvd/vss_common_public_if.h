#ifndef __VSS_COMMON_PUBLIC_IF_H__
#define __VSS_COMMON_PUBLIC_IF_H__

/**
  @file vss_common_public_if.h
  @brief This file contains the APR API definitions that are common among
  modules.
*/

/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear
      in the PDF.

      The CVD_mainpage.dox file contains all file/group descriptions
      that are in the output PDF generated using Doxygen and Latex. To edit or
      update any of the file/group text in the PDF, edit the
      CVD_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the vss_common_public_if
      group description in the CVD_mainpage.dox file.
=============================================================================*/
/*===========================================================================
 Copyright (C) 2011-2015 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc. 
===========================================================================*/

/*===========================================================================

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/audio_avs/api/cvd/vss_common_public_if.h#1 $
  $Author: mplcsds1 $

===========================================================================*/

#include "apr_comdef.h"
#include "vss_common_deprecated_public_if.h"

/** @addtogroup vss_common_public_if
@{ */

/*--------------------------------------------------------------------------*
 * MEDIA TYPE DEFINITIONS                                                   *
 *--------------------------------------------------------------------------*/

/** @name Media Type IDs
  @latexonly \label{media_ID} @endlatexonly
  @{ */

/** Default media type ID. */
#define VSS_MEDIA_ID_NONE ( 0x00010FC0 )

/** CDMA variable 13K vocoder format. */
#define VSS_MEDIA_ID_13K ( 0x00010FC1 )

/** CDMA Enhanced Variable Rate Vocoder (EVRC) format. */
#define VSS_MEDIA_ID_EVRC ( 0x00010FC2 )

/** CDMA 4th generation NB vocoder format. */
#define VSS_MEDIA_ID_4GV_NB ( 0x00010FC3 )

/** CDMA 4th generation WB vocoder format. */
#define VSS_MEDIA_ID_4GV_WB ( 0x00010FC4 )

/** CDMA 4th generation Narrow-Wide (NW) vocoder format. */
#define VSS_MEDIA_ID_4GV_NW ( 0x00010FC5 )

/** CDMA 4th generation Narrow-Wide-2K (NW2K) vocoder format. */
#define VSS_MEDIA_ID_4GV_NW2K ( 0x00010FD7 )

/** Universal Mobile Telecommunications System (UMTS) AMR-NB vocoder format. */
#define VSS_MEDIA_ID_AMR_NB ( 0x00010FC6 )

/** UMTS AMR-WB vocoder format. */
#define VSS_MEDIA_ID_AMR_WB ( 0x00010FC7 )

/** eAMR (Enhanced AMR) vocoder format. */
#define VSS_MEDIA_ID_EAMR ( 0x00010FD4 )

/** GSM enhanced full rate vocoder format. */
#define VSS_MEDIA_ID_EFR ( 0x00010FC8 )

/** GSM full rate vocoder format. */
#define VSS_MEDIA_ID_FR ( 0x00010FC9 )

/** GSM half rate vocoder format. */
#define VSS_MEDIA_ID_HR ( 0x00010FCA )

/** Linear Pulse Code Modulation (PCM) vocoder format; mono, 16 bits,
  little endian, 8 KHz. */
#define VSS_MEDIA_ID_PCM_8_KHZ ( 0x00010FCB )

/** Linear PCM vocoder format; mono, 16 bits, little endian, 16 KHz. */
#define VSS_MEDIA_ID_PCM_16_KHZ ( 0x00010FCC )

/** Linear PCM vocoder format; mono, 16 bits, little endian, 44.1 KHz. */
#define VSS_MEDIA_ID_PCM_44_1_KHZ ( 0x00010FD5 )

/** Linear PCM vocoder format; mono, 16 bits, little endian, 48 KHz. */
#define VSS_MEDIA_ID_PCM_48_KHZ ( 0x00010FD6 )

/** G.711 a-law V2 vocoder format; contains 20 ms vocoder frame. */
#define VSS_MEDIA_ID_G711_ALAW_V2 ( 0x000131A1 )

/** G.711 mu-law V2 vocoder format; contains 20 ms vocoder frame. */
#define VSS_MEDIA_ID_G711_MULAW_V2 ( 0x000131A2 )

/** G.729AB vocoder format; contains two 10 ms vocoder frames. Refer to
  @latexonly [\hyperref[S4]{S4}]@endlatexonly, Figure 4. */
#define VSS_MEDIA_ID_G729 ( 0x00010FD0 )

/** G.722 vocoder format; contains one 20 ms vocoder frame. */
#define VSS_MEDIA_ID_G722 ( 0x00010FD1 )

/** @} */ /* end_name_group Media Type IDs */


/****************************************************************************
 * CALIBRATION FRAMEWORK DEFINITIONS                                        *
 ****************************************************************************/

/** @name Order of Applying Multiple Matching Calibration Entries
  @latexonly \label{cal_order} @endlatexonly
  @{ */

/**
  When multiple matching calibration entries in a calibration data table must
  be applied for a particular system configuration (composed by network, Tx PP
  sampling rate, Rx PP sampling rate, etc.), the order in which the matching
  entries are applied is specified below:

  - Specific overwrites N/A rule: A matching entry that corresponds to a
    non-N/A column value for a particular column shall be applied after
    applying all other matching entries that correspond to the N/A column value
    for the column.
  
  - For all the matching entries, in the reverse order of the columns provided
    as part of the calibration data registration, for each column, the specific
    overwrites N/A rule will be applied. Specifically, when N columns are provided
    as part of the calibration registration, starting from the Nth column:

    1. Apply the specific overwrites N/A rule on this column.

    2. For all the matching entries that correspond to the non-N/A value for
       this column, repeat step 1, 2 and 3 for the next column (i.e. the column
       immediately preceding this column) until the calibration order for the
       matching entries is determined.

    3. For all the matching entries that correspond to the N/A value for this
       column, repeat step 1, 2 and 3 for the next column (i.e. the column
       immediately preceding this column) until the calibration order for the
       matching entries are determined.

  The following example illustrates the order of applying multiple matching
  calibration entries:

  Assume the order of the columns provided by the client in the dynamic
  calibration table registered with a particular vocproc session is:
  1. #VSS_ICOMMON_CAL_COLUMN_NETWORK
  2. #VSS_ICOMMON_CAL_COLUMN_TX_PP_SAMPLING_RATE
  3. #VSS_ICOMMON_CAL_COLUMN_RX_PP_SAMPLING_RATE
  4. #VSS_ICOMMON_CAL_COLUMN_RX_VOLUME_INDEX
  5. #VSS_ICOMMON_CAL_COLUMN_TX_VOC_OPERATING_MODE
  6. #VSS_ICOMMON_CAL_COLUMN_RX_VOC_OPERATING_MODE
  7. #VSS_ICOMMON_CAL_COLUMN_MEDIA_ID
  8. #VSS_ICOMMON_CAL_COLUMN_FEATURE

  Assume the N/A column value provided by the client for the
  #VSS_ICOMMON_CAL_COLUMN_TX_PP_SAMPLING_RATE column is 0.

  Assume the following are 3 of the matching entries found in the vocproc
  dynamic calibration table based on the current system configuration and must
  be applied. Each matching entry is represented by a composition of column
  values for the columns listed above.

  1. Calibration entry composed by:
     <
       #VSS_ICOMMON_CAL_NETWORK_ID_WCDMA,
       0 (i.e. N/A Tx PP sampling rate),
       16000 (i.e. 16000 Hz Rx PP sampling rate),
       1 (i.e. volume index 1),
       #VSS_ICOMMON_CAL_COLUMN_VOC_OPERATING_MODE_NONE,
       #VSS_ICOMMON_CAL_COLUMN_VOC_OPERATING_MODE_NONE,
       #VSS_MEDIA_ID_NONE,
       #VSS_ICOMMON_CAL_FEATURE_NONE
     >

  2. Calibration entry composed by:
     <
       #VSS_ICOMMON_CAL_NETWORK_ID_WCDMA,
       0,
       16000,
       1,
       #VSS_ICOMMON_CAL_COLUMN_VOC_OPERATING_MODE_NONE,
       #VSS_ICOMMON_CAL_COLUMN_VOC_OPERATING_MODE_NB,
       #VSS_MEDIA_ID_EAMR,
       #VSS_ICOMMON_CAL_FEATURE_NONE
     >

  3. Calibration entry composed by:
     <
       #VSS_ICOMMON_CAL_NETWORK_ID_WCDMA,
       0,
       16000,
       1,
       #VSS_ICOMMON_CAL_COLUMN_VOC_OPERATING_MODE_NONE,
       #VSS_ICOMMON_CAL_COLUMN_VOC_OPERATING_MODE_NB,
       #VSS_MEDIA_ID_NONE,
       #VSS_ICOMMON_CAL_FEATURE_BEAMR
     >

  The specific overwrites N/A rule is first applied on the
  #VSS_ICOMMON_CAL_COLUMN_FEATURE column. As a result, calibration entry 3
  will be applied after applying calibration entries 1 and 2.

  Next, to determine the order of applying calibration entries 1 and 2, the
  specific overwrites N/A rule is applied on the
  #VSS_ICOMMON_CAL_COLUMN_MEDIA_ID column. As a result, calibration entry 2
  will be applied after applying calibration entry 1.

  Therefore, the final order in which the three calibration entries are applied
  is: calibration entry 1 will be applied first, calibration entry 2 will be
  applied next, and calibration entry 3 will be applied last.
*/

/** @} */ /* end_name_group Order of Applying Multiple Matching Calibration Entries */

/** @name Calibration Data Format
  @latexonly \label{cal_format} @endlatexonly
  @{ */

/**
  The client shall provide shared memory that contains calibration data with
  the format specified below when registering calibration data with CVD.

  The calibration data buffer contains the following fields at its beginning:

  - Major version: Calibration data format major version. The currently
    supported major version is 1.

  - Minor version: Calibration data format minor version. The currently
    supported minor version is 0.

  The 1.0 (major version 1, minor version 0) version of the calibration data
  format is specified below.

  The calibration data is represented by 4 tables:

  - Lookup table (LUT): Contains the calibration profiles, where each profile
    is composed by a list of column values along with a byte offset to the
    content definition table as well as a byte offset to the content data
    offsets table.

  - Content definition table (CDFT): Contains the lists of <mid, pid> (Module
    ID, Parameter ID) pairs for calibration profiles in the LUT.

  - Content Data offsets table (CDOT): Contains the location (offsets in bytes)
    of the data table where the calibration data resides for each of the
    <mid, pid> pair for the lists of <mid, pid> pairs in the CDFT.

  - Data table: Contains calibration data parameters.

  @verbatim

  ----------------------------  --> Calibration data buffer start
  | Major version             | --> This field is 32-bit.
  ----------------------------
  | Minor version             | --> This field is 32-bit.
  ---------------------------- 
  | Size in bytes of the LUT  | --> This field is 32-bit.
  | that follows              |
  ----------------------------  --> This is the beginning of the LUT.
  | Number of LUT entries     | --> This field is 32-bit.
  ----------------------------  --> Entry 1 start
  | column_1                  | --> eg. network_id
  ----------------------------
  | column_2                  | --> eg. tx_pp_sampling_rate
  ----------------------------
  |  .                        |
  |  .                        |
  ----------------------------
  | column_n                  |
  ----------------------------
  | CDFT OFFSET               | --> Byte offset from the beginning of the CDFT. This field is 32-bit.
  ----------------------------
  | CDOT OFFSET               | --> Byte offset from the beginning of CDOT. This field is 32-bit.
  ----------------------------  --> Entry 2 start
  | column_1                  |
  ----------------------------
  | column_2                  |
  |  .                        |
  |  .                        |
  |  ...                      |
  ---------------------------- 
  | Size in bytes of the CDFT | --> This field is 32-bit.
  | that follows              |
  ----------------------------  --> Entry 1 start (this is the beginning of the CDFT).
  | Number of <mid, pid> pairs| --> This field is 32-bit.
  ----------------------------
  | mod_id                    | --> This field is 32-bit.
  ----------------------------
  | param_id                  | --> This field is 32-bit.
  ----------------------------
  | mod_id                    |
  ----------------------------
  | param_id                  |
  |                           |
  |                           |
  | ...                       |
  ----------------------------  --> Entry 2 start
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
  | Size in bytes of the CDOT | --> This field is 32-bit.
  | that follows              |
  ----------------------------  --> Entry 1 start (this is the beginning of the CDOT).
  | Number of data offset     | --> This field is 32-bit.
  | entries                   |
  ----------------------------
  | data table offset         | --> This field is 32-bit.
  ----------------------------
  | data table offset         |
  ----------------------------
  |                           |
  |                           |
  | ...                       |
  ----------------------------  --> Entry 2 start
  | Number of data offset     |
  | entries                   |
  ----------------------------
  | data table offset         |
  ----------------------------
  | data table offset         |
  ----------------------------
  |                           |
  |                           |
  | ...                       |
  ---------------------------- 
  | Size in bytes of the data | --> This field is 32-bit.
  | table that follows        |
  ----------------------------  --> Entry 1 start (this is the beginning of the data table).
  | data_size                 | --> Size of the calibration data in bytes for a particular <mid, pid> pair. This field is 32-bit.
  ----------------------------
  | data                      | --> Calibration data for a particular <mid, pid> pair. This field is a multiple of 4 bytes.
  |                           |
  |                           |
  |  ...                      |
  ----------------------------  --> Entry 2 start
  | data_size                 |
  ----------------------------
  | data                      |
  |                           |
  |                           |
  | ...                       |

    @endverbatim
*/

/** @} */ /* end_name_group Calibration Data Format */

/*--------------------------------------------------------------------------*
 * CALIBRATION COLUMN IDS                                                   *
 *--------------------------------------------------------------------------*/

/** @name Calibration Columns
  @latexonly \label{calibration_column} @endlatexonly
  @{ */

/**
  Column Network ID.
  (see @latexonly \hyperref[cal_network_ID]{Calibration Network IDs} @endlatexonly).
*/
#define VSS_ICOMMON_CAL_COLUMN_NETWORK ( 0x00011350 )

/** Column RX Pre/post-processing (PP) Sampling Rate. */
#define VSS_ICOMMON_CAL_COLUMN_RX_PP_SAMPLING_RATE ( 0x00011351 )

/** Column TX Pre/post-processing (PP) Sampling Rate. */
#define VSS_ICOMMON_CAL_COLUMN_TX_PP_SAMPLING_RATE ( 0x00011352 )

/**
  Column RX Volume Index.
  Given a particular set of system configurations (composed by network, Tx PP
  sampling rate, Rx PP sampling rate, etc. excluding the Rx volume index), the
  list of <mid, pid> pairs to be calibrated shall be the same between the
  different Rx volume indices (except the N/A Rx volume index) and the Rx
  volume calibration data shall be placed in the dynamic calibration data
  table.
*/
#define VSS_ICOMMON_CAL_COLUMN_RX_VOLUME_INDEX ( 0x00011358 )

/**
  Column Media ID.
  (see @latexonly \hyperref[media_ID]{Media Type IDs} @endlatexonly).
*/
#define VSS_ICOMMON_CAL_COLUMN_MEDIA_ID ( 0x00013081 )

/**
  Column TX Vocoder Operating Mode.
  (see @latexonly \hyperref[cal_voc_op_mode]{Calibration Vocoder Operating
  Modes} @endlatexonly). For vocoders whose Tx operating mode can change
  dynamically during run-time, given a particular set of system configurations
  (composed by network, Tx PP sampling rate, Rx PP sampling rate, etc.
  excluding the Tx vocoder operating mode), the list of <mid, pid> pairs to be
  calibrated shall be the same between the different Tx vocoder operating modes
  (except the N/A Tx vocoder operating mode) and the Tx vocoder operating mode
  specific calibration data shall be placed in the dynamic calibration data
  table.
*/
#define VSS_ICOMMON_CAL_COLUMN_TX_VOC_OPERATING_MODE ( 0x00013082 )

/**
  Column RX Vocoder Operating Mode.
  (see @latexonly \hyperref[cal_voc_op_mode]{Calibration Vocoder Operating
  Modes} @endlatexonly). For vocoders whose Rx operating mode can change
  dynamically during run-time, given a particular set of system configurations
  (composed by network, Tx PP sampling rate, Rx PP sampling rate, etc.
  excluding the Rx vocoder operating mode), the list of <mid, pid> pairs to be
  calibrated shall be the same between the different Rx vocoder operating modes
  (except the N/A Rx vocoder operating mode) and the Rx vocoder operating mode
  specific calibration data shall be placed in the dynamic calibration data
  table.
*/
#define VSS_ICOMMON_CAL_COLUMN_RX_VOC_OPERATING_MODE ( 0x00013083 )

/**
  Column Feature.
  (see @latexonly \hyperref[cal_feature]{Calibration Features} @endlatexonly).
  For features that can be enabled or disabled dynamically during run-time,
  given a particular set of system configurations (composed by network, Tx PP
  sampling rate, Rx PP sampling rate, etc. excluding the feature), the list of
  <mid, pid> pairs to be calibrated shall be the same between when a feature is
  enabled and when a feature is disabled, and the feature specific calibration
  data shall be placed in the dynamic calibration data table.
*/
#define VSS_ICOMMON_CAL_COLUMN_FEATURE ( 0x00013084 )

/** @} */ /* end_name_group Calibration Columns */

/*--------------------------------------------------------------------------*
 * CALIBRATION COLUMN TYPES                                                 *
 *--------------------------------------------------------------------------*/

/** @name Calibration Column Types
  @latexonly \label{calibration_column_type} @endlatexonly
  @{ */

/** 8-bit calibration column data type */
#define VSS_ICOMMON_CAL_COLUMN_TYPE_UINT8 ( 0x0001135A )

/** 16-bit calibration column data type */
#define VSS_ICOMMON_CAL_COLUMN_TYPE_UINT16 ( 0x0001135B )

/** 32-bit calibration column data type */
#define VSS_ICOMMON_CAL_COLUMN_TYPE_UINT32 ( 0x0001135C )

/** 64-bit calibration column data type */
#define VSS_ICOMMON_CAL_COLUMN_TYPE_UINT64 ( 0x0001135D )

/** @} */ /* end_name_group Calibration Column Types */

/*--------------------------------------------------------------------------*
 * NETWORK ID COLUMN VALUES                                                 *
 *--------------------------------------------------------------------------*/

/** @name Calibration Network IDs
  @latexonly \label{cal_network_ID} @endlatexonly
  @{ */

/** NONE network ID. Network ID is not applicable. */
#define VSS_ICOMMON_CAL_NETWORK_ID_NONE ( 0x0001135E )

/** CDMA network ID. */
#define VSS_ICOMMON_CAL_NETWORK_ID_CDMA ( 0x0001135F )

/** GSM network ID. */
#define VSS_ICOMMON_CAL_NETWORK_ID_GSM ( 0x00011360 )

/** WCDMA network ID. */
#define VSS_ICOMMON_CAL_NETWORK_ID_WCDMA ( 0x00011361 )

/** VOIP network ID. */
#define VSS_ICOMMON_CAL_NETWORK_ID_VOIP ( 0x00011362 )

/** LTE network ID. */
#define VSS_ICOMMON_CAL_NETWORK_ID_LTE ( 0x00011363 )

/** @} */ /* end_name_group Network IDs */

/*--------------------------------------------------------------------------*
 * TX/RX VOCODER OPERATING MODE COLUMN VALUES                               *
 *--------------------------------------------------------------------------*/

/** @name Calibration Vocoder Operating Modes
  @latexonly \label{cal_voc_op_mode} @endlatexonly
  @{ */

/** NONE vocoder operating mode. Vocoder operating mode is not applicable. */
#define VSS_ICOMMON_CAL_VOC_OPERATING_MODE_NONE ( 0x00013085 )

/**
  Common narrowband vocoder operating mode representing audio bandwidth up to
  4KHz.
*/
#define VSS_ICOMMON_CAL_VOC_OPERATING_MODE_NB ( 0x00013086 )

/**
  Common wideband vocoder operating mode representing audio bandwidth up to
  8KHz.
*/
#define VSS_ICOMMON_CAL_VOC_OPERATING_MODE_WB ( 0x00013087 )

/**
  Common super wideband vocoder operating mode representing audio bandwidth up
  to 16KHz.
*/
#define VSS_ICOMMON_CAL_VOC_OPERATING_MODE_SWB ( 0x00013088 )

/**
  Common fullband vocoder operating mode representing audio bandwidth up to
  24KHz.
*/
#define VSS_ICOMMON_CAL_VOC_OPERATING_MODE_FB ( 0x00013089 )

/** @} */ /* end_name_group Calibration Vocoder Operating Modes */

/*--------------------------------------------------------------------------*
 * FEATURE COLUMN VALUES                                                    *
 *--------------------------------------------------------------------------*/

/** @name Calibration Features
  @latexonly \label{cal_feature} @endlatexonly
  @{ */

/** NONE feature. Feature is not applicable. */
#define VSS_ICOMMON_CAL_FEATURE_NONE ( 0x0001308A )

/** Wide Voice feature. */
#define VSS_ICOMMON_CAL_FEATURE_WIDEVOICE ( 0x0001308B )

/** BeAMR feature. */
#define VSS_ICOMMON_CAL_FEATURE_BEAMR ( 0x0001308C )

/** Wide Voice 2.0 feature. */
#define VSS_ICOMMON_CAL_FEATURE_WIDEVOICE2 ( 0x000130A7 )

/** @} */ /* end_name_group Calibration Features */


/****************************************************************************
 * STREAM/VOCPROC CALIBRATION APIS                                          *
 ****************************************************************************/

/** @brief Type definition for #vss_icommon_param_data_t.
*/
typedef struct vss_icommon_param_data_t vss_icommon_param_data_t;

#include "apr_pack_begin.h"

/** @brief Parameter data structure used by the #VSS_ICOMMON_CMD_SET_PARAM_V2
    and #VSS_ICOMMON_RSP_GET_PARAM commands.
*/
struct vss_icommon_param_data_t
{
  uint32_t module_id;
    /**< Unique ID of the module. */
  uint32_t param_id;
    /**< Unique ID of the parameter. */
  uint16_t param_size;
    /**< Size of the parameter in bytes. */
  uint16_t reserved;
    /**< Reserved; set to 0. */
#if 0
  uint8_t param_data[param_size];
    /**< Parameter data when received in-band.\n
         Additional repetitions of the modules_id, param_id, param_size,
         reserved sequence can continue below: \n
         uint32_t next_module_id; \n
         . \n
         . \n
         . */

#endif /* 0 */
}
#include "apr_pack_end.h"
;

/**
  Sets one or more calibration parameters. This API is intended for private
  use. Client must not use this API for calibration or UI control purposes.

  @par Payload
  #vss_icommon_cmd_set_param_v2_t

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  If the parameter data is out-of-band, the memory that holds the data must
  first be mapped with CVD by following @latexonly \hyperref[shmem_guideline]
  {Shared Memory Usage Guideline} @endlatexonly. The parameter data in the 
  memory must be valid and read-only until a response to this command is 
  received.
*/
#define VSS_ICOMMON_CMD_SET_PARAM_V2 ( 0x0001133D )

/** @brief Type definition for #vss_icommon_cmd_set_param_v2_t.
*/
typedef struct vss_icommon_cmd_set_param_v2_t vss_icommon_cmd_set_param_v2_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_ICOMMON_CMD_SET_PARAM_V2 command.
*/
struct vss_icommon_cmd_set_param_v2_t
{
  uint32_t mem_handle;
    /**< If the parameter data is in-band, set this field to 0. If the
         parameter data is out-of-band, this field is the handle to the shared
         memory that holds the parameter data. See @latexonly \hyperref
         [shmem_guideline]{Shared Memory Usage Guideline} @endlatexonly. */
  uint64_t mem_address;
    /**< Location of the parameter data payload. The payload is an array of 
         #vss_icommon_param_data_t. See @latexonly \hyperref[shmem_guideline]
         {Shared Memory Usage Guideline} @endlatexonly on how the address is 
         intrepreted. If the mem_handle is 0, this field is ignored. */
  uint32_t mem_size;
    /**< Size of the parameter data payload in bytes. This field is applicable
         to both in-band and out-of-band parameter data. For out-of-band 
         parameter, the amount of memory allocated at mem_address must meet the
         requirement as specified in @latexonly \hyperref[shmem_guideline]
         {Shared Memory Usage Guideline} @endlatexonly. */

#if 0
  vss_icommon_param_data_t param_data[];
    /**< param_data is present when the data is in-band. */
#endif /* 0 */
}
#include "apr_pack_end.h"
;

/**
  Gets a calibration parameter. This API is intended for private use. Client
  must not use this API for calibration or UI control purposes.

  @par Payload
  #vss_icommon_cmd_get_param_v2_t

  @return
  #VSS_ICOMMON_RSP_GET_PARAM -- In case of success, invalid module ID, invalid
                                parameter ID. \n
  #APRV2_IBASIC_RSP_RESULT -- In case of any failure other than invalid module
                              ID or invalid parameter ID.

  @dependencies
  If the parameter data is out-of-band, the memory that holds the data must
  first be mapped with CVD by following @latexonly \hyperref[shmem_guideline]
  {Shared Memory Usage Guideline} @endlatexonly. The client must not modify 
  the memory and the memory must be valid and writable until a response to this
  command is received.
*/
#define VSS_ICOMMON_CMD_GET_PARAM_V2 ( 0x0001133E )

/** @brief Type definition for #vss_icommon_cmd_get_param_v2_t.
*/
typedef struct vss_icommon_cmd_get_param_v2_t vss_icommon_cmd_get_param_v2_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_ICOMMON_CMD_GET_PARAM_V2 command.
*/
struct vss_icommon_cmd_get_param_v2_t
{
  uint32_t mem_handle;
    /**< If the parameter data is in-band, set this field to 0. If the
         parameter data is out-of-band, this field is the handle to the shared
         memory that holds the parameter data. See @latexonly \hyperref
         [shmem_guideline]{Shared Memory Usage Guideline} @endlatexonly. */
  uint64_t mem_address;
    /**< Location to hold the parameter data. See @latexonly \hyperref
         [shmem_guideline]{Shared Memory Usage Guideline} @endlatexonly on how
         the address is intrepreted. A single #vss_icommon_param_data_t that 
         contains the header and the algorithm's parameter data will be placed
         at this location. If the mem_handle is 0, this field is ignored. */
  uint16_t mem_size;
    /**< Size of the memory in bytes to hold the parameter. This field is 
         applicable to both in-band and out-of-band parameter. The size must
         be large enough to hold the algorithm's parameter data plus the header
         #vss_icommon_param_data_t. For out-of-band parameter, the size must
         also meet the requirement as specified in @latexonly \hyperref
         [shmem_guideline]{Shared Memory Usage Guideline} @endlatexonly. */
  uint32_t module_id;
    /**< Unique ID of the module. */
  uint32_t param_id;
    /**< Unique ID of the parameter. */
}
#include "apr_pack_end.h"
;

/**
  Response to the #VSS_ICOMMON_CMD_GET_PARAM_V2 command.

  @par Payload
  #vss_icommon_rsp_get_param_t

  @return
  None.

  @dependencies
  None.
*/
#define VSS_ICOMMON_RSP_GET_PARAM ( 0x00011008 )

/** @brief Type definition for #vss_icommon_rsp_get_param_t.
*/
typedef struct vss_icommon_rsp_get_param_t vss_icommon_rsp_get_param_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_ICOMMON_RSP_GET_PARAM command
   response.
*/
struct vss_icommon_rsp_get_param_t
{
  uint32_t status;
    /**< Status of the GET_PARAM command. Possible values:\n
         - 0 -- Success.\n
         - 1 -- Invalid module ID.\n
         - 2 -- Invalid parameter ID. */

#if 0
  vss_icommon_param_data_t param_data[1];
    /**< param_data is present when the data is in-band. */
#endif /* 0 */
}
#include "apr_pack_end.h"
;

/****************************************************************************
 * STREAM/VOCPROC UI PROPERTY APIS                                          *
 ****************************************************************************/

/**
  Sets a UI property.

  @par Payload
  #vss_icommon_cmd_set_ui_property_t

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.
*/
#define VSS_ICOMMON_CMD_SET_UI_PROPERTY ( 0x00011103 )

/** @brief Type definition for #vss_icommon_cmd_set_ui_property_t.
*/
typedef struct vss_icommon_cmd_set_ui_property_t vss_icommon_cmd_set_ui_property_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_ICOMMON_CMD_SET_UI_PROPERTY command.
*/
struct vss_icommon_cmd_set_ui_property_t
{
  uint32_t module_id;
    /**< Unique ID of the module. */
  uint32_t param_id;
    /**< Unique ID of the parameter. */
  uint16_t param_size;
    /**< Size of the parameter in bytes. */
  uint16_t reserved;
    /**< Reserved; set to 0. */
#if 0
  uint8_t param_data[param_size];
    /**< Parameter data sent in-band. */
#endif /* 0 */
}
#include "apr_pack_end.h"
;

/**
  Gets a UI property.

  @par Payload
  #vss_icommon_cmd_get_ui_property_t

  @return
  #VSS_ICOMMON_RSP_GET_UI_PROPERTY -- Success; UI property was retrieved.\n
  #APRV2_IBASIC_RSP_RESULT -- Failure; UI property was not retrieved.

  @dependencies
  None.
*/
#define VSS_ICOMMON_CMD_GET_UI_PROPERTY ( 0x00011104 )

/** @brief Type definition for #vss_icommon_cmd_get_ui_property_t.
*/
typedef struct vss_icommon_cmd_get_ui_property_t vss_icommon_cmd_get_ui_property_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_ICOMMON_CMD_GET_UI_PROPERTY command.
*/
struct vss_icommon_cmd_get_ui_property_t
{
  uint32_t module_id;
    /**< Unique ID of the module. */
  uint32_t param_id;
    /**< Unique ID of the parameter. */
  uint32_t param_size;
    /**< Size of the parameter in bytes. */
}
#include "apr_pack_end.h"
;

/**
  Response to the #VSS_ICOMMON_CMD_GET_UI_PROPERTY command.

  @par Payload
  #vss_icommon_rsp_get_ui_property_t

  @return
  None.

  @dependencies
  None.
 */
#define VSS_ICOMMON_RSP_GET_UI_PROPERTY ( 0x00011105 )

/** @brief Type definition for vss_icommon_rsp_get_ui_property_t.
*/
typedef struct vss_icommon_rsp_get_ui_property_t vss_icommon_rsp_get_ui_property_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_ICOMMON_RSP_GET_UI_PROPERTY command
    response.
*/
struct vss_icommon_rsp_get_ui_property_t
{
  uint32_t status;
    /**< Status of the GET_UI_PROPERTY command. Possible values:\n
         - 0 -- Success \n
         - 1 -- Invalid module ID \n
         - 2 -- Invalid parameter ID */
  uint32_t module_id;
    /**< Unique ID of the module. */
  uint32_t param_id;
    /**< Unique ID of the parameter. */
  uint16_t param_size;
    /**< Size of the parameter in bytes. */
  uint16_t reserved;
    /**< Reserved; set to 0. */
#if 0
  uint8_t param_data[param_size];
    /**< Parameter data received in-band. */
#endif /* 0 */
}
#include "apr_pack_end.h"
;

/****************************************************************************
 * VOICE TIMING APIS                                                        *
 ****************************************************************************/

/**
  Sets the voice timing parameters and VFR (Vocoder Frame Reference).

  @par Payload
  #vss_icommon_cmd_set_voice_timing_v2_t

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.
*/
#define VSS_ICOMMON_CMD_SET_VOICE_TIMING_V2 ( 0x00012E66 )

/** Soft VFR mode flag. */
#define VSS_ICOMMON_VFR_MODE_SOFT ( 0 )

/** Hard VFR mode flag.*/
#define VSS_ICOMMON_VFR_MODE_HARD ( 1 )

/** @brief Type definition for #vss_icommon_cmd_set_voice_timing_v2_t.
*/
typedef struct vss_icommon_cmd_set_voice_timing_v2_t vss_icommon_cmd_set_voice_timing_v2_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_ICOMMON_CMD_SET_VOICE_TIMING_V2 command.
*/
struct vss_icommon_cmd_set_voice_timing_v2_t
{
  uint16_t mode;
    /**< Vocoder frame synchronization mode. Possible values:\n
         - #VSS_ICOMMON_VFR_MODE_SOFT -- No frame synchronization. \n
         - #VSS_ICOMMON_VFR_MODE_HARD -- Hard Vocoder Frame Reference (VFR). 
           A 20 ms VFR interrupt. */
  uint32_t vsid;
    /**< Voice System ID as defined by DCN 80-NF711-1. */
  uint16_t enc_offset;
    /**< Offset in microseconds from the VFR to deliver a Tx vocoder
        packet. The offset is to be less than 20000 us. */
  uint16_t dec_req_offset;
    /**< The offset in microseconds from the VFR to request for an Rx vocoder
         packet. The offset is to be less than 20000 us. */
  uint16_t dec_offset;
    /**< Offset in microseconds from the VFR to indicate the deadline to
         receive an Rx vocoder packet. The offset is to be less than 20000 us.
         Rx vocoder packets received after this deadline are not guaranteed to
         be processed.  */
}
#include "apr_pack_end.h"
;

/** @} */ /* end_addtogroup vss_common_public_if */

#endif /* __VSS_COMMON_PUBLIC_IF_H__ */

